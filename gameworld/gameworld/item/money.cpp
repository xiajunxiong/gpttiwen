#include "money.h"

#include "engineadapter.h"
#include "obj/character/role.h"
#include "internalcomm.h"
#include "knapsack.h"
#include "scene/scene.h"
#include "rmibackobjdef.h"
#include "servercommon/errornum.h"
#include "servercommon/string/gameworldstr.h"
#include "gamelog.h"
#include "world.h"
#include "other/vip/vip.hpp"
#include <sstream>
#include "other/rolemodulemanager.hpp"
#include "other/monitor/monitor.hpp"
#include "servercommon/logdef.h"
#include "protocol/msgrole.h"
#include "other/event/eventhandler.hpp"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "global/cross/crossuserregister.hpp"
#include "other/route/mailroute.hpp"

static const int FETCH_ACCOUNTDB_INTERVAL = 300;

Money::Money() : 
	m_module_mgr(NULL),
	m_gold(0),
	m_coin_bind(0),
	m_immortal_coin(0),
	m_last_account_get_time(0)
{

}

Money::~Money()
{

}

void Money::Init(RoleModuleManager* module_mgr, Int64 coin_bind, Int64 immortal_coin, Int64 gold)
{
	m_module_mgr = module_mgr;

	m_coin_bind = coin_bind;
	m_immortal_coin = immortal_coin;
	m_gold = gold;

	gamelog::g_log_debug.printf(LL_DEBUG, "Money::Init role[%d,%s] coin_bind[%lld] immortal_coin[%lld] gold[%lld]",
		m_module_mgr->GetRole()->GetUID(), m_module_mgr->GetRole()->GetName(), m_coin_bind, m_immortal_coin, m_gold);
}

void Money::InitOtherCoin(const MoneyOtherParam &param)
{
	m_other_currency_param = param;

	// 以下三种货币不属于其他货币，要从其他数据中同步
	m_other_currency_param.other_currency[MONEY_TYPE_GOLD] = m_gold;
	m_other_currency_param.other_currency[MONEY_TYPE_COIN_BIND] = m_coin_bind;
	m_other_currency_param.other_currency[MONEY_TYPE_IMMORTAL_COIN] = m_immortal_coin;
}

void Money::GetInitOtherCoin(MoneyOtherParam * param)
{
	// 以下三种货币不属于其他货币，要从其他数据中同步
	m_other_currency_param.other_currency[MONEY_TYPE_GOLD] = m_gold;
	m_other_currency_param.other_currency[MONEY_TYPE_COIN_BIND] = m_coin_bind;
	m_other_currency_param.other_currency[MONEY_TYPE_IMMORTAL_COIN] = m_immortal_coin;

	*param = m_other_currency_param;
}

Int64 Money::GetMoneyByType(int coin_type) const
{
	if(coin_type < 0 || coin_type >= MONEY_TYPE_MAX) return 0;

	if (MONEY_TYPE_GOLD == coin_type)
	{
		return m_gold;
	}
	else if (MONEY_TYPE_COIN_BIND == coin_type)
	{
		return m_coin_bind;
	}
	else if (MONEY_TYPE_IMMORTAL_COIN == coin_type)
	{
		return m_immortal_coin;
	}

	return m_other_currency_param.other_currency[coin_type];
}

bool Money::CoinBindMoreThan(Int64 consume_coin_bind)
{
	return m_coin_bind >= consume_coin_bind;
}

bool Money::ImmortalCoinMoreThan(Int64 consume_immortal_coin)
{
	return m_immortal_coin >= consume_immortal_coin;
}

bool Money::GoldMoreThan(Int64 consume_gold)
{
	return m_gold >= consume_gold;
}

bool Money::AllCoinMoreThan(Int64 consume_coin)
{
	return m_coin_bind + m_immortal_coin >= consume_coin;
}

bool Money::MoneyTypeMoreThan(int type, Int64 num, bool is_notice)
{
	switch (type)
	{
		case MONEY_TYPE_GOLD:
			{
				if (!this->GoldMoreThan(num))
				{
					if(is_notice) m_module_mgr->NoticeNum(errornum::EN_GOLD_NOT_ENOUGH);
					return false;
				}
				return true;
			}
			break;
		case MONEY_TYPE_IMMORTAL_COIN:
			{
				if (!this->ImmortalCoinMoreThan(num))
				{
					if(is_notice) m_module_mgr->NoticeNum(errornum::EN_IMMORTAL_COIN_NOT_ENOUGH);
					return false;
				}
				return true;
			}
			break;
		case MONEY_TYPE_COIN_BIND:
			{
				if (!this->CoinBindMoreThan(num))
				{
					if(is_notice) m_module_mgr->NoticeNum(errornum::EN_COIN_NOT_ENOUGH);
					return false;
				}
				return true;
			}
			break;
		default:
			{
				if (!this->OtherCoinMoreThan(num, type))
				{
					if(is_notice) m_module_mgr->NoticeNum(errornum::EN_OTHER_MONEY_NOT_ENOUGH);
					return false;
				}

				return true;
			}
			break;
	}

	return false;
}

bool Money::UseGold(Int64 consume_gold, const char* type, bool is_consume_sys /* = true */, bool is_use_in_cross /* = false */, bool is_no_show_on_client /* = false */)
{
	//if (!is_use_in_cross && CrossConfig::Instance().IsHiddenServer()) // 在跨服里面，普通的消耗元宝不会成功
	//{
	//	return false;
	//}

	if (consume_gold < 0 || m_gold < consume_gold || NULL == type) return false;

	if (0 == consume_gold) return true;

	m_gold -= consume_gold;

	this->Send(MONEY_TYPE_GOLD, is_no_show_on_client);

	m_module_mgr->GetMonitor()->AddMonitorNum(MONITOR_TYPE_DAY_GOLD_CONSUME, (int)consume_gold);
	if (is_consume_sys)
	{
		EventHandler::Instance().OnComsuneGold(m_module_mgr->GetRole(), consume_gold);
	}

	gamelog::g_log_moneygold.printf(LL_INFO, "[Money::UseGold Succ][user[%s %d] [level:%d] type:%s use_gold:%u remain_gold:%u]",
		m_module_mgr->GetRole()->GetName(), UidToInt(m_module_mgr->GetRole()->GetUserId()), m_module_mgr->GetRole()->GetLevel(), type, (unsigned int)consume_gold, (unsigned int)m_gold);

	if (CrossConfig::Instance().IsHiddenServer())
	{
		World::GetInstWorld()->SyncLogToOriginServer(m_module_mgr->GetRole()->GetUserId().db_index, SYNC_LOG_TYPE_MONEY_GOLD, 
			"[Money::UseGold Succ][user[%s %d] [level:%d] type:%s use_gold:%u remain_gold:%u]",
			m_module_mgr->GetRole()->GetName(), UidToInt(m_module_mgr->GetRole()->GetUserId()), 
			m_module_mgr->GetRole()->GetLevel(), type, (unsigned int)consume_gold, (unsigned int)m_gold);
	}

	ROLE_LOG_QUICK6(LOG_TYPE_GOLD_USE, m_module_mgr->GetRole(), consume_gold, m_gold, type, is_consume_sys ? "Y" : "N");
	EventHandler::Instance().OnConsumeGold(m_module_mgr->GetRole(), consume_gold);

	return true;
}

bool Money::UseCoinBind(Int64 consume_coin_bind, const char* type, bool is_use_in_cross /* = false */, bool is_no_show_on_client /* = false */)
{
	//if (!is_use_in_cross && CrossConfig::Instance().IsHiddenServer()) // 在跨服里面，普通的消耗元宝不会成功
	//{
	//	return false;
	//}

	if (consume_coin_bind < 0 || m_coin_bind < consume_coin_bind || NULL == type) return false;

	if (0 == consume_coin_bind) return true;

	m_coin_bind -= consume_coin_bind;

	this->Send(MONEY_TYPE_COIN_BIND, is_no_show_on_client);

	m_module_mgr->GetMonitor()->AddMonitorNum(MONITOR_TYPE_DAY_COINBIND_CONSUME, (int)consume_coin_bind);

	gamelog::g_log_moneycoin.printf(LL_INFO, "[Money::UseCoinBind Succ][user[%s %d] [level:%d] type:%s use_coin_bind:%lld remain_coin_bind:%lld]",
		m_module_mgr->GetRole()->GetName(), UidToInt(m_module_mgr->GetRole()->GetUserId()), m_module_mgr->GetRole()->GetLevel(), type, consume_coin_bind, m_coin_bind);

	if (CrossConfig::Instance().IsHiddenServer())
	{
		World::GetInstWorld()->SyncLogToOriginServer(m_module_mgr->GetRole()->GetUserId().db_index, SYNC_LOG_TYPE_MONEY_COIN,
			"[Money::UseCoinBind Succ][user[%s %d] [level:%d] type:%s use_coin_bind:%lld remain_coin_bind:%lld]",
			m_module_mgr->GetRole()->GetName(), UidToInt(m_module_mgr->GetRole()->GetUserId()), m_module_mgr->GetRole()->GetLevel(), type, consume_coin_bind, m_coin_bind);
	}

	ROLE_LOG_QUICK6(LOG_TYPE_COIN_BIND_USE, m_module_mgr->GetRole(), consume_coin_bind, m_coin_bind, type, NULL);
	EventHandler::Instance().OnComsuneCoinBind(m_module_mgr->GetRole(), consume_coin_bind);
	return true;
}

bool Money::UseImmortalCoin(Int64 consume_immortal_coin, const char* type, bool is_use_in_cross /* = false */, bool is_no_show_on_client /* = false */)
{
#ifdef _DEBUG
	int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF),
		"UseImmortalCoin function[%s] please check config!!!", type);
	if (length > 0)
	{
		World::GetInstWorld()->GetSceneManager()->SystemMsgPerson(m_module_mgr->GetRole()->GetUserId(), gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT, SYS_MSG_SYSTEM_CHAT));
	}
#endif

	return false; // 暂时屏蔽仙币使用

	//if (!is_use_in_cross && CrossConfig::Instance().IsHiddenServer()) // 在跨服里面，普通的消耗元宝不会成功
	//{
	//	return false;
	//}

	if (consume_immortal_coin < 0 || m_immortal_coin < consume_immortal_coin || NULL == type) return false;

	if (0 == consume_immortal_coin) return true;

	m_immortal_coin -= consume_immortal_coin;

	this->Send(MONEY_TYPE_IMMORTAL_COIN, is_no_show_on_client);

	m_module_mgr->GetMonitor()->AddMonitorNum(MONITOR_TYPE_DAY_IMMORTALCOIN_CONSUME, (int)consume_immortal_coin);

	gamelog::g_log_moneyimmortalcoin.printf(LL_INFO, "[Money::UseImmortalCoin Succ] [user[%s %d] [level:%d] type:%s use_immortal_coin:%lld remain_immortal_coin:%lld]",
		m_module_mgr->GetRole()->GetName(), UidToInt(m_module_mgr->GetRole()->GetUserId()), m_module_mgr->GetRole()->GetLevel(), type, consume_immortal_coin, m_immortal_coin);

	ROLE_LOG_QUICK6(MONITOR_TYPE_DAY_IMMORTALCOIN_CONSUME, m_module_mgr->GetRole(), consume_immortal_coin, m_immortal_coin, type, NULL);
	EventHandler::Instance().OnComsuneImmortalCoin(m_module_mgr->GetRole(), consume_immortal_coin);

	return true;
}

bool Money::UseAllCoin(Int64 consume_coin, const char* type, bool bind_first /* = true */, Int64* use_bind_coin /* = NULL */, Int64* use_immortal_coin /* = NULL */, bool is_use_in_cross /* = false */, bool is_no_show_on_client /* = false */)
{
	//if (!is_use_in_cross && CrossConfig::Instance().IsHiddenServer()) // 在跨服里面，普通的消耗元宝不会成功
	//{
	//	return false;
	//}

	if (consume_coin <= 0 || NULL == type) return false;

	if (!this->AllCoinMoreThan(consume_coin)) return false;

	Int64 need_bind_coin = 0;
	Int64 need_immortal_coin = 0;

	need_bind_coin = consume_coin; // 暂不使用仙币
	/*
	{
		if (bind_first)
		{
			need_bind_coin = consume_coin;
			if (m_coin_bind < need_bind_coin)
			{
				need_bind_coin = m_coin_bind;
				need_immortal_coin = consume_coin - need_bind_coin;
			}
		}
		else
		{
			need_immortal_coin = consume_coin;
			if (m_immortal_coin < need_immortal_coin)
			{
				need_immortal_coin = m_immortal_coin;
				need_bind_coin = consume_coin - need_immortal_coin;
			}
		}
	}
	*/

	if ((need_bind_coin > 0 && !this->UseCoinBind(need_bind_coin, type, is_use_in_cross, is_no_show_on_client)) ||
		(need_immortal_coin > 0 && !this->UseImmortalCoin(need_immortal_coin, type, is_use_in_cross, is_no_show_on_client)))
	{
		return false;
	}

	gamelog::g_log_money_allcoin.printf(LL_INFO, "Money::UseAllCoin role[%d,%s] use_bind_coin[%lld] cur_bind_coin[%lld] need_immortal_coin[%lld] cur_immortal_coin[%lld]",
		m_module_mgr->GetRole()->GetUID(), m_module_mgr->GetRole()->GetName(), need_bind_coin, m_coin_bind, need_immortal_coin, m_immortal_coin);
	//如果开放使用仙币,下面逻辑需要调整
	ROLE_LOG_QUICK6(LOG_TYPE_COIN_BIND_USE, m_module_mgr->GetRole(), need_bind_coin, m_coin_bind, type, NULL);

	if (NULL != use_bind_coin) *use_bind_coin = need_bind_coin;
	if (NULL != use_immortal_coin) *use_immortal_coin = need_immortal_coin;

	return true;
}

bool Money::UseMoneyType(int money_type, Int64 num, const char * type)
{
	return this->UseMoney(money_type,num, type);
}

bool Money::Add(Int64 gold, Int64 immortal_coin, Int64 coin_bind, const char* type, bool is_use_in_cross /* = false */, bool is_no_show_on_client /* = false */)
{
	//if (!is_use_in_cross && CrossConfig::Instance().IsHiddenServer()) // 在跨服里面，普通的加元宝不会成功
	//{
	//	return false;
	//}

	if (gold < 0 || immortal_coin < 0 || coin_bind < 0 || NULL == type) return false;

	immortal_coin = 0; //屏蔽仙币

	m_gold += gold;
	m_immortal_coin += immortal_coin;
	m_coin_bind += coin_bind;

	this->Send(MONEY_TYPE_ALL, is_no_show_on_client);

	m_module_mgr->GetMonitor()->AddMonitorNum(MONITOR_TYPE_DAY_GOLD_GET, (int)gold);

	if (gold > 0) EventHandler::Instance().OnAddGold(m_module_mgr->GetRole(), gold);
	if (immortal_coin > 0) EventHandler::Instance().OnAddImmortalCoin(m_module_mgr->GetRole(), immortal_coin);
	if (coin_bind > 0) EventHandler::Instance().OnAddCoinBind(m_module_mgr->GetRole(), coin_bind);

	if (gold > 0)
	{
		gamelog::g_log_moneygold.printf(LL_INFO, "[Money::AddGold Succ][user[%s %d] [level:%d] type:%s add_gold:%lld remain_gold:%lld]",
			m_module_mgr->GetRole()->GetName(), UidToInt(m_module_mgr->GetRole()->GetUserId()), m_module_mgr->GetRole()->GetLevel(), type, gold, m_gold);
	
		if (CrossConfig::Instance().IsHiddenServer())
		{
			World::GetInstWorld()->SyncLogToOriginServer(m_module_mgr->GetRole()->GetUserId().db_index, SYNC_LOG_TYPE_MONEY_GOLD,
				"[Money::AddGold Succ][user[%s %d] [level:%d] type:%s add_gold:%lld remain_gold:%lld]",
				m_module_mgr->GetRole()->GetName(), UidToInt(m_module_mgr->GetRole()->GetUserId()), m_module_mgr->GetRole()->GetLevel(), type, gold, m_gold);
		}
	}

	if (immortal_coin > 0)
	{
		gamelog::g_log_moneyimmortalcoin.printf(LL_INFO, "[Money::AddImmortalCoin Succ][user[%s %d] [level:%d] type:%s add_immortalcoin:%lld remain_immortalcoin:%lld]",
			m_module_mgr->GetRole()->GetName(), UidToInt(m_module_mgr->GetRole()->GetUserId()), m_module_mgr->GetRole()->GetLevel(), type, immortal_coin, m_immortal_coin);
	}

	if (coin_bind > 0)
	{
		gamelog::g_log_moneycoin.printf(LL_INFO, "[Money::AddAllCoin Succ][user[%s %d] [level:%d] type:%s add_coin_bind:%u  remain_coin_bind:%u]",
			m_module_mgr->GetRole()->GetName(), UidToInt(m_module_mgr->GetRole()->GetUserId()), m_module_mgr->GetRole()->GetLevel(), type, (unsigned int)coin_bind, (unsigned int)m_coin_bind);
	
		if (CrossConfig::Instance().IsHiddenServer())
		{
			World::GetInstWorld()->SyncLogToOriginServer(m_module_mgr->GetRole()->GetUserId().db_index, SYNC_LOG_TYPE_MONEY_COIN,
				"[Money::AddAllCoin Succ][user[%s %d] [level:%d] type:%s add_coin_bind:%u  remain_coin_bind:%u]",
				m_module_mgr->GetRole()->GetName(), UidToInt(m_module_mgr->GetRole()->GetUserId()), m_module_mgr->GetRole()->GetLevel(), type, (unsigned int)coin_bind, (unsigned int)m_coin_bind);
		}
	}

	if (gold > 0)
	{
		ROLE_LOG_QUICK6(LOG_TYPE_GOLD_ADD, m_module_mgr->GetRole(), gold, m_gold, type, NULL);
	}
	if (immortal_coin > 0)
	{
		ROLE_LOG_QUICK6(LOG_TYPE_IMMORTAL_COIN_ADD, m_module_mgr->GetRole(), immortal_coin, m_immortal_coin, type, NULL);
	}

	if (coin_bind > 0)
	{
		ROLE_LOG_QUICK6(LOG_TYPE_COIN_BIND_ADD, m_module_mgr->GetRole(), coin_bind, m_coin_bind, type, NULL);
	}

	return true;
}

bool Money::AddGold(Int64 gold, const char* type, bool is_chongzhi /* = false */, bool is_use_in_cross /* = false */, bool is_no_show_on_client /* = false */)
{
	//if (!is_use_in_cross && CrossConfig::Instance().IsHiddenServer()) // 在跨服里面，普通的加元宝不会成功
	//{
	//	return false;
	//}

	if (gold < 0 || NULL == type) return false;
	if (0 == gold) return true;

	if (MAX_INT64 - gold < m_gold)
	{
		gamelog::g_log_moneygold.printf(LL_INFO, "[Money::AddGold Failed][user[%s %d] [level:%d] type:%s add_gold:%u remain_gold:%u reason:result over limit]",
			m_module_mgr->GetRole()->GetName(), UidToInt(m_module_mgr->GetRole()->GetUserId()), m_module_mgr->GetRole()->GetLevel(), type, (unsigned int)gold, (unsigned int)m_gold);

		if (CrossConfig::Instance().IsHiddenServer())
		{
			World::GetInstWorld()->SyncLogToOriginServer(m_module_mgr->GetRole()->GetUserId().db_index, SYNC_LOG_TYPE_MONEY_GOLD,
				"[Money::AddGold Failed][user[%s %d] [level:%d] type:%s add_gold:%u remain_gold:%u reason:result over limit]",
				m_module_mgr->GetRole()->GetName(), UidToInt(m_module_mgr->GetRole()->GetUserId()), m_module_mgr->GetRole()->GetLevel(), type, (unsigned int)gold, (unsigned int)m_gold);
		}
		return false; //超出64位整型的限制
	}

	m_gold += gold;

	this->Send(MONEY_TYPE_GOLD, is_no_show_on_client);
	EventHandler::Instance().OnAddGold(m_module_mgr->GetRole(), gold);

	if (!is_chongzhi) m_module_mgr->GetMonitor()->AddMonitorNum(MONITOR_TYPE_DAY_GOLD_GET, (int)gold);

	gamelog::g_log_moneygold.printf(LL_INFO, "[Money::AddGold Succ][user[%s %d] [level:%d] type:%s add_gold:%u remain_gold:%u]",
		m_module_mgr->GetRole()->GetName(), UidToInt(m_module_mgr->GetRole()->GetUserId()), m_module_mgr->GetRole()->GetLevel(), type, (unsigned int)gold, (unsigned int)m_gold);

	if (CrossConfig::Instance().IsHiddenServer())
	{
		World::GetInstWorld()->SyncLogToOriginServer(m_module_mgr->GetRole()->GetUserId().db_index, SYNC_LOG_TYPE_MONEY_GOLD,
			"[Money::AddGold Succ][user[%s %d] [level:%d] type:%s add_gold:%u remain_gold:%u]",
			m_module_mgr->GetRole()->GetName(), UidToInt(m_module_mgr->GetRole()->GetUserId()), m_module_mgr->GetRole()->GetLevel(), type, (unsigned int)gold, (unsigned int)m_gold);
	}

	ROLE_LOG_QUICK6(LOG_TYPE_GOLD_ADD, m_module_mgr->GetRole(), gold, m_gold, type, NULL);

	return true;
}

bool Money::AddImmortalCoin(Int64 immortal_coin, const char* type, bool is_use_in_cross /* = false */, bool is_no_show_on_client /* = false */)
{
#ifdef _DEBUG
	int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF),
		"AddImmortalCoin function[%s] please check config!!!", type);
	if (length > 0)
	{
		World::GetInstWorld()->GetSceneManager()->SystemMsgPerson(m_module_mgr->GetRole()->GetUserId(), gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT, SYS_MSG_SYSTEM_CHAT));
	}
#endif

	return false; // 暂时屏蔽仙币

	//if (!is_use_in_cross && CrossConfig::Instance().IsHiddenServer()) // 在跨服里面，普通的加铜币不会成功
	//{
	//	return false;
	//}

	if (immortal_coin < 0 || NULL == type) return false;
	if (0 == immortal_coin) return true;

	if (MAX_INT64 - immortal_coin < m_immortal_coin)
	{
		gamelog::g_log_moneyimmortalcoin.printf(LL_INFO, "[Money::AddImmortalCoin Failed][user[%s %d] [level:%d] type:%s add_immortalcoin:%lld remain_immortalcoin:%lld reason:result over limit]",
			m_module_mgr->GetRole()->GetName(), UidToInt(m_module_mgr->GetRole()->GetUserId()), m_module_mgr->GetRole()->GetLevel(), type, immortal_coin, m_immortal_coin);

		return false; //超出64位整型的限制
	}

	m_immortal_coin += immortal_coin;

	this->Send(MONEY_TYPE_IMMORTAL_COIN, is_no_show_on_client);
	EventHandler::Instance().OnAddImmortalCoin(m_module_mgr->GetRole(), immortal_coin);

	gamelog::g_log_moneyimmortalcoin.printf(LL_INFO, "[Money::AddImmortalCoin Succ][user[%s %d] [level:%d] type:%s add_immortalcoin:%lld remain_immortalcoin:%lld]",
		m_module_mgr->GetRole()->GetName(), UidToInt(m_module_mgr->GetRole()->GetUserId()), m_module_mgr->GetRole()->GetLevel(), type, immortal_coin, m_immortal_coin);

	ROLE_LOG_QUICK6(LOG_TYPE_IMMORTAL_COIN_ADD, m_module_mgr->GetRole(), immortal_coin, m_immortal_coin, type, NULL);

	return true;
}

void Money::Send(int money_type, bool is_no_show_on_client /* = false */)
{
	static Protocol::SCMoney money;
	money.count = 0;
	money.is_no_show = is_no_show_on_client ? 1 : 0;

	switch (money_type)
	{
	case MONEY_TYPE_ALL:
		{
			money.money_list[money.count].money_num = m_gold;
			money.money_list[money.count].money_type = MONEY_TYPE_GOLD;
			money.money_list[money.count].reserve_sh = 0;
			money.count++;

			money.money_list[money.count].money_num = m_immortal_coin;
			money.money_list[money.count].money_type = MONEY_TYPE_IMMORTAL_COIN;
			money.money_list[money.count].reserve_sh = 0;
			money.count++;

			money.money_list[money.count].money_num = m_coin_bind;
			money.money_list[money.count].money_type = MONEY_TYPE_COIN_BIND;
			money.money_list[money.count].reserve_sh = 0;
			money.count++;

			money.money_list[money.count].money_num = m_other_currency_param.other_currency[MONEY_TYPE_GUILD_CONTRIBUTION];
			money.money_list[money.count].money_type = MONEY_TYPE_GUILD_CONTRIBUTION;
			money.money_list[money.count].reserve_sh = 0;
			money.count++;

			money.money_list[money.count].money_num = m_other_currency_param.other_currency[MONEY_TYPE_CHIVALROUS];
			money.money_list[money.count].money_type = MONEY_TYPE_CHIVALROUS;
			money.money_list[money.count].reserve_sh = 0;
			money.count++;

			money.money_list[money.count].money_num = m_other_currency_param.other_currency[MONEY_TYPE_COLOSSEUM];
			money.money_list[money.count].money_type = MONEY_TYPE_COLOSSEUM;
			money.money_list[money.count].reserve_sh = 0;
			money.count++;

			money.money_list[money.count].money_num = m_other_currency_param.other_currency[MONEY_TYPE_FACE_SCORE];
			money.money_list[money.count].money_type = MONEY_TYPE_FACE_SCORE;
			money.money_list[money.count].reserve_sh = 0;
			money.count++;

			money.money_list[money.count].money_num = m_other_currency_param.other_currency[MONEY_TYPE_SURFACE_COIN];
			money.money_list[money.count].money_type = MONEY_TYPE_SURFACE_COIN;
			money.money_list[money.count].reserve_sh = 0;
			money.count++;

			money.money_list[money.count].money_num = m_other_currency_param.other_currency[MONEY_TYPE_PLATFORM_BATTLE_CREDITS];
			money.money_list[money.count].money_type = MONEY_TYPE_PLATFORM_BATTLE_CREDITS;
			money.money_list[money.count].reserve_sh = 0;
			money.count++;

			money.money_list[money.count].money_num = m_other_currency_param.other_currency[MONEY_TYPE_RESERVE];
			money.money_list[money.count].money_type = MONEY_TYPE_RESERVE;
			money.money_list[money.count].reserve_sh = 0;
			money.count++;

			money.money_list[money.count].money_num = m_other_currency_param.other_currency[MONEY_TYPE_LUCK_COIN];
			money.money_list[money.count].money_type = MONEY_TYPE_LUCK_COIN;
			money.money_list[money.count].reserve_sh = 0;
			money.count++;

			money.money_list[money.count].money_num = m_other_currency_param.other_currency[MONEY_TYPE_HUAN_JIE_ZHAN_CHANG_COIN];
			money.money_list[money.count].money_type = MONEY_TYPE_HUAN_JIE_ZHAN_CHANG_COIN;
			money.money_list[money.count].reserve_sh = 0;
			money.count++;

			money.money_list[money.count].money_num = m_other_currency_param.other_currency[MONEY_TYPE_YUAN_SUI_ZHI_LING];
			money.money_list[money.count].money_type = MONEY_TYPE_YUAN_SUI_ZHI_LING;
			money.money_list[money.count].reserve_sh = 0;
			money.count++;

			money.money_list[money.count].money_num = m_other_currency_param.other_currency[MONEY_TYPE_DUI_ZHANG_GONG_ZI];
			money.money_list[money.count].money_type = MONEY_TYPE_DUI_ZHANG_GONG_ZI;
			money.money_list[money.count].reserve_sh = 0;
			money.count++;


			money.money_list[money.count].money_num = m_other_currency_param.other_currency[MONEY_TYPE_CLOUD_ARENA_COIN];
			money.money_list[money.count].money_type = MONEY_TYPE_CLOUD_ARENA_COIN;
			money.money_list[money.count].reserve_sh = 0;
			money.count++;

			money.money_list[money.count].money_num = m_other_currency_param.other_currency[MONEY_TYPE_SILVER_COIN];
			money.money_list[money.count].money_type = MONEY_TYPE_SILVER_COIN;
			money.money_list[money.count].reserve_sh = 0;
			money.count++;

			money.money_list[money.count].money_num = m_other_currency_param.other_currency[MONEY_TYPE_FEN_CHEN];
			money.money_list[money.count].money_type = MONEY_TYPE_FEN_CHEN;
			money.money_list[money.count].reserve_sh = 0;
			money.count++;

			money.money_list[money.count].money_num = m_other_currency_param.other_currency[MONEY_TYPE_GUESS_COIN];
			money.money_list[money.count].money_type = MONEY_TYPE_GUESS_COIN;
			money.money_list[money.count].reserve_sh = 0;
			money.count++;

			money.money_list[money.count].money_num = m_other_currency_param.other_currency[MONEY_TYPE_CAN_HUN];
			money.money_list[money.count].money_type = MONEY_TYPE_CAN_HUN;
			money.money_list[money.count].reserve_sh = 0;
			money.count++;

			UNSTD_STATIC_CHECK(20 == MONEY_TYPE_MAX);
		}
		break;

	case MONEY_TYPE_GOLD:
		{
			money.money_list[money.count].money_num = m_gold;
			money.money_list[money.count].money_type = MONEY_TYPE_GOLD;
			money.money_list[money.count].reserve_sh = 0;
			money.count++;
		}
		break;

	case MONEY_TYPE_IMMORTAL_COIN:
		{
			money.money_list[money.count].money_num = m_immortal_coin;
			money.money_list[money.count].money_type = MONEY_TYPE_IMMORTAL_COIN;
			money.money_list[money.count].reserve_sh = 0;
			money.count++;
		}
		break;

	case MONEY_TYPE_COIN_BIND:
		{
			money.money_list[money.count].money_num = m_coin_bind;
			money.money_list[money.count].money_type = MONEY_TYPE_COIN_BIND;
			money.money_list[money.count].reserve_sh = 0;
			money.count++;
		}
		break;

	case MONEY_TYPE_GUILD_CONTRIBUTION:
		{
			money.money_list[money.count].money_num = m_other_currency_param.other_currency[MONEY_TYPE_GUILD_CONTRIBUTION];
			money.money_list[money.count].money_type = MONEY_TYPE_GUILD_CONTRIBUTION;
			money.money_list[money.count].reserve_sh = 0;
			money.count++;
		}
		break;

	case MONEY_TYPE_CHIVALROUS:
		{
			money.money_list[money.count].money_num = m_other_currency_param.other_currency[MONEY_TYPE_CHIVALROUS];
			money.money_list[money.count].money_type = MONEY_TYPE_CHIVALROUS;
			money.money_list[money.count].reserve_sh = 0;
			money.count++;
		}
		break;

	case MONEY_TYPE_FACE_SCORE:
		{
			money.money_list[money.count].money_num = m_other_currency_param.other_currency[MONEY_TYPE_FACE_SCORE];
			money.money_list[money.count].money_type = MONEY_TYPE_FACE_SCORE;
			money.money_list[money.count].reserve_sh = 0;
			money.count++;
		}
		break;

	case MONEY_TYPE_SURFACE_COIN:
		{
			money.money_list[money.count].money_num = m_other_currency_param.other_currency[MONEY_TYPE_SURFACE_COIN];
			money.money_list[money.count].money_type = MONEY_TYPE_SURFACE_COIN;
			money.money_list[money.count].reserve_sh = 0;
			money.count++;
		}
		break;
	case MONEY_TYPE_COLOSSEUM:
		{
			money.money_list[money.count].money_num = m_other_currency_param.other_currency[MONEY_TYPE_COLOSSEUM];
			money.money_list[money.count].money_type = MONEY_TYPE_COLOSSEUM;
			money.money_list[money.count].reserve_sh = 0;
			money.count++;
		}	
		break;
	case MONEY_TYPE_PLATFORM_BATTLE_CREDITS:
		{
			money.money_list[money.count].money_num = m_other_currency_param.other_currency[MONEY_TYPE_PLATFORM_BATTLE_CREDITS];
			money.money_list[money.count].money_type = MONEY_TYPE_PLATFORM_BATTLE_CREDITS;
			money.money_list[money.count].reserve_sh = 0;
			money.count++;
		}
		break;
	case MONEY_TYPE_RESERVE:
		{
			money.money_list[money.count].money_num = m_other_currency_param.other_currency[money_type];
			money.money_list[money.count].money_type = money_type;
			money.money_list[money.count].reserve_sh = 0;
			money.count++;
		}
		break;
	case MONEY_TYPE_HUAN_JIE_ZHAN_CHANG_CREDITS:
		{
			money.money_list[money.count].money_num = m_other_currency_param.other_currency[money_type];
			money.money_list[money.count].money_type = money_type;
			money.money_list[money.count].reserve_sh = 0;
			money.count++;
		}
		break;

	case MONEY_TYPE_LUCK_COIN:
		{
			money.money_list[money.count].money_num = m_other_currency_param.other_currency[money_type];
			money.money_list[money.count].money_type = money_type;
			money.money_list[money.count].reserve_sh = 0;
			money.count++;
		}
		break;
	case MONEY_TYPE_HUAN_JIE_ZHAN_CHANG_COIN:
		{
			money.money_list[money.count].money_num = m_other_currency_param.other_currency[money_type];
			money.money_list[money.count].money_type = money_type;
			money.money_list[money.count].reserve_sh = 0;
			money.count++;
		}
		break;
	case MONEY_TYPE_YUAN_SUI_ZHI_LING:
		{
			money.money_list[money.count].money_num = m_other_currency_param.other_currency[money_type];
			money.money_list[money.count].money_type = money_type;
			money.money_list[money.count].reserve_sh = 0;
			money.count++;
		}
		break;
	case MONEY_TYPE_DUI_ZHANG_GONG_ZI:
		{
			money.money_list[money.count].money_num = m_other_currency_param.other_currency[money_type];
			money.money_list[money.count].money_type = money_type;
			money.money_list[money.count].reserve_sh = 0;
			money.count++;
		}
		break;
	case MONEY_TYPE_CLOUD_ARENA_COIN:
		{
			money.money_list[money.count].money_num = m_other_currency_param.other_currency[money_type];
			money.money_list[money.count].money_type = money_type;
			money.money_list[money.count].reserve_sh = 0;
			money.count += 1;
		}
		break;
	case MONEY_TYPE_SILVER_COIN:
		{
			money.money_list[money.count].money_num = m_other_currency_param.other_currency[money_type];
			money.money_list[money.count].money_type = money_type;
			money.money_list[money.count].reserve_sh = 0;
			money.count += 1;
		}
		break;
	case MONEY_TYPE_FEN_CHEN:
		{
			money.money_list[money.count].money_num = m_other_currency_param.other_currency[money_type];
			money.money_list[money.count].money_type = money_type;
			money.money_list[money.count].reserve_sh = 0;
			money.count += 1;
		}
		break;
	case MONEY_TYPE_GUESS_COIN:
		{
			money.money_list[money.count].money_num = m_other_currency_param.other_currency[money_type];
			money.money_list[money.count].money_type = money_type;
			money.money_list[money.count].reserve_sh = 0;
			money.count += 1;
		}
		break;
	case MONEY_TYPE_CAN_HUN:
		{
			money.money_list[money.count].money_num = m_other_currency_param.other_currency[money_type];
			money.money_list[money.count].money_type = money_type;
			money.money_list[money.count].reserve_sh = 0;
			money.count += 1;
		}
		break;
	}

	UNSTD_STATIC_CHECK(20 == MONEY_TYPE_MAX);

	int len = sizeof(money) - sizeof(money.money_list[0]) * (MONEY_TYPE_MAX - money.count);
	EngineAdapter::Instance().NetSend(m_module_mgr->GetRole()->GetNetId(), (const char*)&money, len);
}

void RMIGetGoldBackObjectImpl::GetGoldRet(Int64 gold, int authority_type)
{
	if (gold > 0) 
	{
		Role *role = scene_manager->GetRole(user_id);
		if (NULL != role)
		{
			role->GetRoleModuleManager()->GetMoney()->GetAccountGoldToRole(gold, authority_type);
		}
		else
		{
			// 这里不打个log么
			gamelog::g_log_serious_error.printf(LL_CRITICAL, "RMIGetGoldBackObjectImpl::GetGoldRet Callback role is null! role_id=%d", UidToInt(user_id));
		}
	}
}

bool Money::AddCoinBind(Int64 coin_bind, const char* type, bool is_use_in_cross /* = false */, bool is_no_show_on_client /* = false */)
{
	//if (!is_use_in_cross && CrossConfig::Instance().IsHiddenServer()) // 在跨服里面，普通的加铜币不会成功
	//{
	//	return false;
	//}

	if (coin_bind < 0 || NULL == type) return false;
	if (0 == coin_bind) return true;

	if (MAX_INT64 - coin_bind < m_coin_bind)
	{
		gamelog::g_log_moneycoin.printf(LL_INFO, "[Money::AddCoinBind Failed][user[%s %d] [level:%d] type:%s add_coin_bind:%u remain_coin_bind:%u reason:result over limit]",
			m_module_mgr->GetRole()->GetName(), UidToInt(m_module_mgr->GetRole()->GetUserId()), m_module_mgr->GetRole()->GetLevel(), type, (unsigned int)coin_bind, (unsigned int)m_coin_bind);

		if (CrossConfig::Instance().IsHiddenServer())
		{
			World::GetInstWorld()->SyncLogToOriginServer(m_module_mgr->GetRole()->GetUserId().db_index, SYNC_LOG_TYPE_MONEY_COIN,
				"[Money::AddCoinBind Failed][user[%s %d] [level:%d] type:%s add_coin_bind:%u remain_coin_bind:%u reason:result over limit]",
				m_module_mgr->GetRole()->GetName(), UidToInt(m_module_mgr->GetRole()->GetUserId()), m_module_mgr->GetRole()->GetLevel(), type, (unsigned int)coin_bind, (unsigned int)m_coin_bind);
		}

		return false; //超出64位整型的限制
	}

	m_coin_bind += coin_bind;

	this->Send(MONEY_TYPE_COIN_BIND, is_no_show_on_client);
	EventHandler::Instance().OnAddCoinBind(m_module_mgr->GetRole(), coin_bind);

	gamelog::g_log_moneycoin.printf(LL_INFO, "[Money::AddCoinBind Succ][user[%s %d] [level:%d] type:%s add_coin_bind:%u remain_coin_bind:%u]",
		m_module_mgr->GetRole()->GetName(), UidToInt(m_module_mgr->GetRole()->GetUserId()), m_module_mgr->GetRole()->GetLevel(), type, (unsigned int)coin_bind, (unsigned int)m_coin_bind);

	if (CrossConfig::Instance().IsHiddenServer())
	{
		World::GetInstWorld()->SyncLogToOriginServer(m_module_mgr->GetRole()->GetUserId().db_index, SYNC_LOG_TYPE_MONEY_COIN,
			"[Money::AddCoinBind Succ][user[%s %d] [level:%d] type:%s add_coin_bind:%u remain_coin_bind:%u]",
			m_module_mgr->GetRole()->GetName(), UidToInt(m_module_mgr->GetRole()->GetUserId()), m_module_mgr->GetRole()->GetLevel(), type, (unsigned int)coin_bind, (unsigned int)m_coin_bind);
	}

	ROLE_LOG_QUICK6(LOG_TYPE_COIN_BIND_ADD, m_module_mgr->GetRole(), coin_bind, m_coin_bind, type, NULL);

	return true;
}

Int64 Money::GetOtherCoin(int coin_type) const
{
	if (coin_type >= MONEY_TYPE_MAX || coin_type < 0) return 0;

#ifdef _DEBUG  // 以下三种不属于other_coin
	if (MONEY_TYPE_GOLD == coin_type || MONEY_TYPE_COIN_BIND == coin_type || MONEY_TYPE_IMMORTAL_COIN == coin_type)
	{
		assert(0);
	}
#endif

	return m_other_currency_param.other_currency[coin_type];
}

bool Money::OtherCoinMoreThan(Int64 consume_other_currency, int coin_type)
{
	if (coin_type >= MONEY_TYPE_MAX || coin_type < 0) return false;

#ifdef _DEBUG  // 以下三种不属于other_coin
	if (MONEY_TYPE_GOLD == coin_type || MONEY_TYPE_COIN_BIND == coin_type || MONEY_TYPE_IMMORTAL_COIN == coin_type)
	{
		assert(0);
	}
#endif

	return m_other_currency_param.other_currency[coin_type] >= consume_other_currency;
}

bool Money::UseOtherCoin(Int64 consume_other_currency, int coin_type, const char* type, bool is_use_in_cross /* = false */, bool is_no_show_on_client /* = false */)
{
	//if (!is_use_in_cross && CrossConfig::Instance().IsHiddenServer()) // 在跨服里面，普通的消耗不会成功
	//{
	//	return false;
	//}

	if (coin_type >= MONEY_TYPE_MAX || coin_type < 0) return false;

#ifdef _DEBUG  // 以下三种不属于other_coin
	if (MONEY_TYPE_GOLD == coin_type || MONEY_TYPE_COIN_BIND == coin_type || MONEY_TYPE_IMMORTAL_COIN == coin_type)
	{
		assert(0);
	}
#endif

	if (consume_other_currency < 0 || m_other_currency_param.other_currency[coin_type] < consume_other_currency || NULL == type) return false;

	if (0 == consume_other_currency) return true;
	long long old_currency = m_other_currency_param.other_currency[coin_type];
	m_other_currency_param.other_currency[coin_type] -= consume_other_currency;
	EventHandler::Instance().OnUseOtherCoin(m_module_mgr->GetRole(), coin_type, consume_other_currency);

	this->Send(coin_type, is_no_show_on_client);
	
	gamelog::g_log_money_other_currency.printf(LL_INFO, "[Money::UseOtherCoin Succ] [user[%s %d] [level:%d] type:%s use_other_currency:%lld remain_other_currency:%lld other_currency_type:%d]",
		m_module_mgr->GetRole()->GetName(), UidToInt(m_module_mgr->GetRole()->GetUserId()), m_module_mgr->GetRole()->GetLevel(), type, consume_other_currency, m_other_currency_param.other_currency[coin_type], (int)coin_type);
	ROLE_LOG_QUICK10(LOG_TYPE_OTHER_MONEY, m_module_mgr->GetRole(), coin_type, consume_other_currency, __FUNCTION__, type, old_currency, m_other_currency_param.other_currency[coin_type], 0, 0, 0);
	return true;
}

bool Money::AddOtherCoin(Int64 add_other_currency, int coin_type, const char* type, bool is_use_in_cross /* = false */, bool is_no_show_on_client /* = false */)
{
	// 如果 consume_other_currency 为负数 , 调用前,应该先调用 OtherCoinMoreThan 来判断
	//if (!is_use_in_cross && CrossConfig::Instance().IsHiddenServer()) // 在跨服里面，普通的加铜币不会成功
	//{
	//	return false;
	//}
	if (coin_type >= MONEY_TYPE_MAX || coin_type < 0 || NULL == type) return false;

#ifdef _DEBUG  // 以下三种不属于other_coin
	if (MONEY_TYPE_GOLD == coin_type || MONEY_TYPE_COIN_BIND == coin_type || MONEY_TYPE_IMMORTAL_COIN == coin_type)
	{
		assert(0);
	}
#endif

	if (0 == add_other_currency) return true;

	if (m_other_currency_param.other_currency[coin_type] + add_other_currency > MAX_INT64)
	{
		m_other_currency_param.other_currency[coin_type] = MAX_INT64;

		gamelog::g_log_money_other_currency.printf(LL_INFO, "[Money::AddOtherCoin Failed][user[%s %d] [level:%d] type:%s add_gold:%lld remain_gold:%lld coin_type:%d  reason:result over limit]",
			m_module_mgr->GetRole()->GetName(), UidToInt(m_module_mgr->GetRole()->GetUserId()), m_module_mgr->GetRole()->GetLevel(), type, add_other_currency, m_other_currency_param.other_currency[coin_type], (int)coin_type);
		return false; //超出64位整型的限制
	}
	long long old_currency = m_other_currency_param.other_currency[coin_type];
	m_other_currency_param.other_currency[coin_type] += add_other_currency;
	EventHandler::Instance().OnAddOtherCoin(m_module_mgr->GetRole(), coin_type, add_other_currency);

	this->Send(coin_type, is_no_show_on_client);

	gamelog::g_log_money_other_currency.printf(LL_INFO, "[Money::AddOtherCoin Succ] [user[%s %d] [level:%d] type:%s use_other_currency:%lld remain_other_currency:%lld other_currency_type:%d]",
		m_module_mgr->GetRole()->GetName(), UidToInt(m_module_mgr->GetRole()->GetUserId()), m_module_mgr->GetRole()->GetLevel(), type, add_other_currency, m_other_currency_param.other_currency[coin_type], (int)coin_type);
	ROLE_LOG_QUICK10(LOG_TYPE_OTHER_MONEY, m_module_mgr->GetRole(), coin_type, add_other_currency, __FUNCTION__, type, old_currency, m_other_currency_param.other_currency[coin_type], 0, 0, 0);
	return true;
}

void Money::GetAccountGoldFromDB(time_t now_second)
{
	if (now_second > FETCH_ACCOUNTDB_INTERVAL + m_last_account_get_time)
	{
		RMIGetGoldBackObjectImpl *ggboi = new RMIGetGoldBackObjectImpl();

		ggboi->scene_manager = World::GetInstWorld()->GetSceneManager();
		ggboi->user_id = m_module_mgr->GetRole()->GetUserId();

		PlatName pname;
		m_module_mgr->GetRole()->GetPlatName(pname);

		UserID original_user_id = m_module_mgr->GetRole()->GetUserId();
		const rmi::Session *session = &InternalComm::Instance().GetDBHandler().GetAcounterDB();

		if (CrossConfig::Instance().IsHiddenServer())
		{
			const CrossUserRegister::CrossUserInfo* cui = CrossUserRegister::Instance().GetCrossUserInfo(original_user_id);
			if (NULL == cui)
			{
				delete ggboi;
				return;
			}

			UniqueServerID usid(cui->original_plat_type, cui->original_server_id);
			const DBHandler* db_handler = InternalComm::Instance().GetHiddenDBHandler(usid);
			if (NULL == db_handler)
			{
				delete ggboi;
				return;
			}

			session = &db_handler->GetAcounterDB();
			original_user_id = cui->original_user_id;
		}

		RMILoginClient lc;
		lc.__bind_session(*session);
		lc.GetGoldAsyn(pname, m_module_mgr->GetRole()->GetUID(), ggboi);

		m_last_account_get_time = now_second;
	}
}

void RMIChangeGoldBackObjectImplErrorAdd::ChangeGoldRet(int result, Int64 new_gold)
{
	if (result != 0)
	{
		Error("result not 0");
	}
}

void RMIChangeGoldBackObjectImplErrorAdd::Error(const char *err_str)
{
	// 记录到Log
	printf("RMIChangeGoldBackObject::Error:%s, user_id[%d, %d] glod_get:%d\n", NULL == err_str ? "" : err_str, user_id.db_index, user_id.role_id, (int)gold_get);
}

void RMIChangeGoldBackObjectImpl::ChangeGoldRet(int result, Int64 new_gold)
{
	gamelog::g_log_moneygold.printf(LL_INFO, "[RMIChangeGoldBackObjectImpl::ChangeGoldRet][result[%d] user[%d] [gold_get:%d] [new_gold:%d]",
		result, UidToInt(user_id), (int)gold_get, (int)new_gold);

	if (0 == result)
	{
		Role *role = scene_manager->GetRole(user_id);
		if (NULL != role)
		{
			Money *money = role->GetRoleModuleManager()->GetMoney();
			if (money->AddGold(gold_get,"GetAccountGold", true))
			{
				if (1 == authority_type && Role::AUTHORITY_TYPE_INVALID == role->GetAuthorityType())
				{
					role->SetAuthorityType(Role::AUTHORITY_TYPE_TEST, false);
				}

				role->Save();	// 迅速保存
				EventHandler::Instance().OnAddChongzhi(role, gold_get);
				return;
			}
		}
		
		RMIChangeGoldBackObjectImplErrorAdd *cgboi = new RMIChangeGoldBackObjectImplErrorAdd();  // 把钱加回来
		
		cgboi->gold_get = gold_get;
		cgboi->user_id = user_id;

		UserID original_user_id = user_id;
		const rmi::Session *session = &InternalComm::Instance().GetDBHandler().GetAcounterDB();

		if (CrossConfig::Instance().IsHiddenServer())
		{
			const CrossUserRegister::CrossUserInfo* cui = CrossUserRegister::Instance().GetCrossUserInfo(original_user_id);
			if (NULL == cui)
			{
				delete cgboi;
				return;
			}

			UniqueServerID usid(cui->original_plat_type, cui->original_server_id);
			const DBHandler* db_handler = InternalComm::Instance().GetHiddenDBHandler(usid);
			if (NULL == db_handler)
			{
				delete cgboi;
				return;
			}

			session = &db_handler->GetAcounterDB();
			original_user_id = cui->original_user_id;

		}

		RMILoginClient lc;
		lc.__bind_session(*session);
		bool ret = lc.ChangeGoldAsyn(pname, UidToInt(user_id), -gold_get, cgboi);

		gamelog::g_log_moneygold.printf(LL_INFO, "[RMIChangeGoldBackObjectImplErrorAdd %s] [user[%d] [gold_get:%d] ]",
			ret ? "succ" : "fail", UidToInt(user_id), (int)gold_get);
	}
}

void RMIChangeGoldBackObjectImpl::Error(const char *err_str)
{
	printf("RMIChangeGoldBackObject::Error:%s\n", NULL == err_str ? "" : err_str);  // 记录Log
}

void Money::GetAccountGoldToRole(Int64 gold, int authority_type)
{
	RMIChangeGoldBackObjectImpl *cgboi = new RMIChangeGoldBackObjectImpl();

	cgboi->scene_manager = World::GetInstWorld()->GetSceneManager();
	cgboi->user_id = m_module_mgr->GetRole()->GetUserId();
	cgboi->gold_get = gold;
	cgboi->authority_type = authority_type;

	m_module_mgr->GetRole()->GetPlatName(cgboi->pname);

	UserID original_user_id = m_module_mgr->GetRole()->GetUserId();
	const rmi::Session *session = &InternalComm::Instance().GetDBHandler().GetAcounterDB();

	if (CrossConfig::Instance().IsHiddenServer())
	{
		const CrossUserRegister::CrossUserInfo* cui = CrossUserRegister::Instance().GetCrossUserInfo(original_user_id);
		if (NULL == cui)
		{
			delete cgboi;
			return;
		}

		UniqueServerID usid(cui->original_plat_type, cui->original_server_id);
		const DBHandler* db_handler = InternalComm::Instance().GetHiddenDBHandler(usid);
		if (NULL == db_handler)
		{
			delete cgboi;
			return;
		}

		session = &db_handler->GetAcounterDB();
		original_user_id = cui->original_user_id;
	}

	RMILoginClient lc;
	lc.__bind_session(*session);
	lc.ChangeGoldAsyn(cgboi->pname, UidToInt(original_user_id), gold, cgboi);
}

void Money::SysGuildContribution(int Contribution, bool is_no_show_on_client /* = false */)
{
	m_other_currency_param.other_currency[MONEY_TYPE_GUILD_CONTRIBUTION] = Contribution;
	this->Send(MONEY_TYPE_GUILD_CONTRIBUTION, is_no_show_on_client);
}

void Money::ForceGetGold()
{
	m_last_account_get_time = EngineAdapter::Instance().Time() - FETCH_ACCOUNTDB_INTERVAL;
}

bool Money::RetrieveMoney(Int64 gold_unbind, Int64 gold_bind, Int64 coin_bind, Int64 spirit_jade, int retrieve_reason)
{
	if (gold_unbind < 0 || gold_bind < 0 || coin_bind < 0 || spirit_jade < 0) return false;

	if (gold_unbind <= 0 && gold_bind <= 0 && coin_bind <= 0 && spirit_jade <= 0) return false;

	const char* type = g_GetRetrieveReasonTypeStr(retrieve_reason);
	if (NULL == type)
	{
		return false;
	}

	Int64 add_gold_unbind_debt = 0;
	Int64 add_coin_bind_debt = 0;
	Int64 add_spirit_jade_debt = 0;
	
	if (m_gold < gold_unbind)
	{
		if (m_gold > 0)
		{
			add_gold_unbind_debt = gold_unbind - m_gold;
		}
		else
		{
			add_gold_unbind_debt = gold_unbind;
		}
	}

	if (m_coin_bind < coin_bind)
	{
		if (m_coin_bind > 0)
		{
			add_coin_bind_debt = coin_bind - m_coin_bind;
		}
		else
		{
			add_coin_bind_debt = coin_bind;
		}
	}

	if (m_other_currency_param.other_currency[MONEY_TYPE_SILVER_COIN] < spirit_jade)
	{
		if (m_other_currency_param.other_currency[MONEY_TYPE_SILVER_COIN] > 0)
		{
			add_spirit_jade_debt = spirit_jade - m_other_currency_param.other_currency[MONEY_TYPE_SILVER_COIN];
		}
		else
		{
			add_spirit_jade_debt = spirit_jade;
		}
	}

	m_gold -= gold_unbind;
	m_coin_bind -= coin_bind;
	m_other_currency_param.other_currency[MONEY_TYPE_SILVER_COIN] -= spirit_jade;

	this->Send(MONEY_TYPE_ALL);

	const char* mail_content = gamestring::g_retrieve_mail_content_bug_money;
	if (RETRIEVE_REASON_REFUND == retrieve_reason)
	{
		mail_content = gamestring::g_retrieve_mail_content_refund_money;
	}
	else if (RETRIEVE_REASON_BUG == retrieve_reason)
	{
		mail_content = gamestring::g_retrieve_mail_content_bug_money;
	}
	else if (RETRIEVE_REASON_IN_DEBT == retrieve_reason)
	{
		mail_content = gamestring::g_retrieve_mail_content_retrieve_debt;
	}

	std::stringstream ss_gold_unbind;
	std::stringstream ss_coin_bind;
	std::stringstream ss_spirit_jade;

	ss_gold_unbind << gold_unbind;
	ss_coin_bind << coin_bind;
	ss_spirit_jade << spirit_jade;

	static MailContentParam contentparam; contentparam.Reset();
	int length1 = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_retrieve_mail_subject);
	int length2 = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), mail_content, ss_gold_unbind.str().c_str(), ss_coin_bind.str().c_str(), ss_spirit_jade.str().c_str());
	if (length1 > 0 && length2 > 0)
	{
		MailRoute::MailToUser(m_module_mgr->GetRole()->GetUserId(), MAIL_REASON_DEFAULT, contentparam);
	}

	if (m_gold < 0 || m_coin_bind < 0 || m_other_currency_param.other_currency[MONEY_TYPE_SILVER_COIN] < 0)
	{
		ss_gold_unbind.str("");
		ss_coin_bind.str("");
		ss_spirit_jade.str("");

		ss_gold_unbind << (m_gold < 0 ? -m_gold : 0);
		ss_coin_bind << (m_coin_bind < 0 ? -m_coin_bind : 0);
		ss_spirit_jade << (m_other_currency_param.other_currency[MONEY_TYPE_SILVER_COIN] < 0 ? -m_other_currency_param.other_currency[MONEY_TYPE_SILVER_COIN] : 0);

		int length1 = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_retrieve_mail_subject);
		int length2 = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_retrieve_mail_content_indebt_money, ss_gold_unbind.str().c_str(),ss_coin_bind.str().c_str(), ss_spirit_jade.str().c_str());
		if (length1 > 0 && length2 > 0)
		{
			MailRoute::MailToUser(m_module_mgr->GetRole()->GetUserId(), MAIL_REASON_DEFAULT, contentparam);
		}
	}
	
	if (gold_unbind > 0)
	{
		gamelog::g_log_moneygold.printf(LL_INFO, "[Money::RetrieveMoney Succ][user[%s %d] [level:%d] \
			type:%s  retrieve_gold:%lld  remain_gold:%lld  add_gold_debt:%lld ]",
			m_module_mgr->GetRole()->GetName(), UidToInt(m_module_mgr->GetRole()->GetUserId()), 
			m_module_mgr->GetRole()->GetLevel(), type, gold_unbind, m_gold, add_gold_unbind_debt);
	
		if (CrossConfig::Instance().IsHiddenServer())
		{
			World::GetInstWorld()->SyncLogToOriginServer(m_module_mgr->GetRole()->GetUserId().db_index, SYNC_LOG_TYPE_MONEY_GOLD,
				"[Money::RetrieveMoney Succ][user[%s %d] [level:%d] \
				type:%s  retrieve_gold:%lld  remain_gold:%lld  add_gold_debt:%lld ]",
				m_module_mgr->GetRole()->GetName(), UidToInt(m_module_mgr->GetRole()->GetUserId()),
				m_module_mgr->GetRole()->GetLevel(), type, gold_unbind, m_gold, add_gold_unbind_debt);
		}

		ROLE_LOG_QUICK6(LOG_TYPE_RETRIEVE_GOLD_UNBIND, m_module_mgr->GetRole(), gold_unbind, m_gold, type, NULL);
		ROLE_LOG_QUICK6(LOG_TYPE_GOLD_USE, m_module_mgr->GetRole(), gold_unbind, m_gold, type, NULL);
	}

	if (coin_bind > 0)
	{
		gamelog::g_log_moneycoin.printf(LL_INFO, "[Money::RetrieveMoney Succ][user[%s %d] [level:%d] type:%s retrieve_coin_bind:%lld  remain_coin_bind:%lld  add_coin_bind_debt:%lld ]",
			m_module_mgr->GetRole()->GetName(), UidToInt(m_module_mgr->GetRole()->GetUserId()), m_module_mgr->GetRole()->GetLevel(), type, coin_bind, m_coin_bind, add_coin_bind_debt);
	
		if (CrossConfig::Instance().IsHiddenServer())
		{
			World::GetInstWorld()->SyncLogToOriginServer(m_module_mgr->GetRole()->GetUserId().db_index, SYNC_LOG_TYPE_MONEY_COIN,
				"[Money::RetrieveMoney Succ][user[%s %d] [level:%d] type:%s retrieve_coin_bind:%lld  remain_coin_bind:%lld  add_coin_bind_debt:%lld]",
				m_module_mgr->GetRole()->GetName(), UidToInt(m_module_mgr->GetRole()->GetUserId()), m_module_mgr->GetRole()->GetLevel(), type, coin_bind, m_coin_bind, add_coin_bind_debt);
		}

		ROLE_LOG_QUICK6(LOG_TYPE_RETRIEVE_COIN_BIND, m_module_mgr->GetRole(), coin_bind, m_coin_bind, type, NULL);
		ROLE_LOG_QUICK6(LOG_TYPE_COIN_BIND_USE, m_module_mgr->GetRole(), coin_bind, m_coin_bind, type, NULL);
	}

	if (spirit_jade > 0)
	{
		gamelog::g_log_money_other_currency.printf(LL_INFO, "[Money::RetrieveMoney Succ][user[%s %d] [level:%d] type:%s retrieve_spirit_jade:%lld  remain_spirit_jade:%lld  add_spirit_jade_debt:%lld ]",
			m_module_mgr->GetRole()->GetName(), UidToInt(m_module_mgr->GetRole()->GetUserId()), m_module_mgr->GetRole()->GetLevel(),
			type, spirit_jade, m_other_currency_param.other_currency[MONEY_TYPE_SILVER_COIN], add_spirit_jade_debt);

		if (CrossConfig::Instance().IsHiddenServer())
		{
			World::GetInstWorld()->SyncLogToOriginServer(m_module_mgr->GetRole()->GetUserId().db_index, SYNC_LOG_TYPE_MONEY_COIN,
				"[Money::RetrieveMoney Succ][user[%s %d] [level:%d] type:%s retrieve_spirit_jade:%lld  remain_spirit_jade:%lld  add_spirit_jade_debt:%lld]",
				m_module_mgr->GetRole()->GetName(), UidToInt(m_module_mgr->GetRole()->GetUserId()), m_module_mgr->GetRole()->GetLevel(), type,
				spirit_jade, m_other_currency_param.other_currency[MONEY_TYPE_SILVER_COIN], add_spirit_jade_debt);
		}

		ROLE_LOG_QUICK6(LOG_TYPE_RETRIEVE_OTHER_CURRENCY, m_module_mgr->GetRole(), spirit_jade, m_other_currency_param.other_currency[MONEY_TYPE_SILVER_COIN], type, NULL);
		ROLE_LOG_QUICK6(LOG_TYPE_OTHER_MONEY_USE, m_module_mgr->GetRole(), spirit_jade, m_other_currency_param.other_currency[MONEY_TYPE_SILVER_COIN], type, NULL);
	}

	return true;
}

bool Money::RetrieveMoney(MoneyOtherParam retrieve_other_currency, int retrieve_reason)
{
	bool is_valid = false;
	for (int i = 0; i < MONEY_TYPE_MAX; ++i)
	{
		if (retrieve_other_currency.other_currency[i] < 0)
		{
			return false;
		}

		is_valid = is_valid || (retrieve_other_currency.other_currency[i] <= 0 ? false : true);
	}

	if (!is_valid)
	{
		return false;
	}

	// 不走这个逻辑或者字段无效
	if (retrieve_other_currency.other_currency[MONEY_TYPE_GOLD] != 0 || retrieve_other_currency.other_currency[MONEY_TYPE_COIN_BIND] != 0 || retrieve_other_currency.other_currency[MONEY_TYPE_IMMORTAL_COIN]
		|| retrieve_other_currency.other_currency[MONEY_TYPE_RESERVE] != 0 || retrieve_other_currency.other_currency[MONEY_TYPE_SILVER_COIN] != 0)
	{
		return false;
	}

	const char* type = g_GetRetrieveReasonTypeStr(retrieve_reason);
	if (NULL == type)
	{
		return false;
	}

	Int64 add_other_currency_debt[MONEY_TYPE_MAX];
	memset(add_other_currency_debt, 0, sizeof(add_other_currency_debt));

	for (int i = 0; i < MONEY_TYPE_MAX; i++)
	{
		if (m_other_currency_param.other_currency[i] < retrieve_other_currency.other_currency[i])
		{
			add_other_currency_debt[i] = retrieve_other_currency.other_currency[i] - m_other_currency_param.other_currency[i];
		}
		else
		{
			add_other_currency_debt[i] = retrieve_other_currency.other_currency[i];
		}
	}

	for (int i = 0; i < MONEY_TYPE_MAX; i++)
	{
		m_other_currency_param.other_currency[i] -= retrieve_other_currency.other_currency[i];
	}

	this->Send(MONEY_TYPE_ALL);
	//////////////////////////////////////////////////////////////////////////
	const char* mail_content = gamestring::g_retrieve_mail_content_bug_other_money;
	if (RETRIEVE_REASON_REFUND == retrieve_reason)
	{
		mail_content = gamestring::g_retrieve_mail_content_refund_other_money;
	}
	else if (RETRIEVE_REASON_BUG == retrieve_reason)
	{
		mail_content = gamestring::g_retrieve_mail_content_bug_other_money;
	}
	else if (RETRIEVE_REASON_IN_DEBT == retrieve_reason)
	{
		mail_content = gamestring::g_retrieve_mail_content_retrieve_debt_other_money;
	}
	//////////////////////////////////////////////////////////////////////////

	for (int i = 0; i < MONEY_TYPE_MAX; ++i)
	{
		std::stringstream ss_other_money;
		ss_other_money << retrieve_other_currency.other_currency[i];

		static MailContentParam contentparam; contentparam.Reset();

		if (retrieve_other_currency.other_currency[i] > 0)
		{			
			int length1 = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_retrieve_mail_subject);
			int length2 = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), mail_content, ss_other_money.str().c_str(), i);
			if (length1 > 0 && length2 > 0)
			{
				MailRoute::MailToUser(m_module_mgr->GetRole()->GetUserId(), MAIL_REASON_DEFAULT, contentparam);
			}
		}

		if (m_other_currency_param.other_currency[i] < 0)	// 快还钱
		{
			ss_other_money.str("");

			ss_other_money << (m_other_currency_param.other_currency[i] < 0 ? -m_other_currency_param.other_currency[i] : 0);

			int length1 = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_retrieve_mail_subject);
			int length2 = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_retrieve_mail_content_indebt_other_money, ss_other_money.str().c_str(), i);
			if (length1 > 0 && length2 > 0)
			{
				MailRoute::MailToUser(m_module_mgr->GetRole()->GetUserId(), MAIL_REASON_DEFAULT, contentparam);
			}
		}

		if (retrieve_other_currency.other_currency[i] > 0)
		{
			gamelog::g_log_money_other_currency.printf(LL_INFO, "[Money::RetrieveMoney Succ][user[%s %d] [level:%d] \
				type:%s money_type[%d] retrieve_other_money:%lld  remain_other_money:%lld  add_other_money_debt:%lld ]",
				m_module_mgr->GetRole()->GetName(), UidToInt(m_module_mgr->GetRole()->GetUserId()), m_module_mgr->GetRole()->GetLevel(),
				type, i, retrieve_other_currency.other_currency[i], m_other_currency_param.other_currency[i], add_other_currency_debt[i]);

			if (CrossConfig::Instance().IsHiddenServer())
			{
				World::GetInstWorld()->SyncLogToOriginServer(m_module_mgr->GetRole()->GetUserId().db_index, SYNC_LOG_TYPE_MONEY_COIN,
					"[Money::RetrieveMoney Succ][user[%s %d] [level:%d] type:%s money_type[%d] retrieve_other_money:%lld  remain_other_money:%lld  add_spirit_other_money:%lld]",
					m_module_mgr->GetRole()->GetName(), UidToInt(m_module_mgr->GetRole()->GetUserId()), m_module_mgr->GetRole()->GetLevel(), type, i,
					retrieve_other_currency.other_currency[i], m_other_currency_param.other_currency[i], add_other_currency_debt[i]);
			}

			ROLE_LOG_QUICK6(LOG_TYPE_RETRIEVE_OTHER_CURRENCY, m_module_mgr->GetRole(), retrieve_other_currency.other_currency[i], m_other_currency_param.other_currency[i], type, NULL);
			ROLE_LOG_QUICK6(LOG_TYPE_OTHER_MONEY_USE, m_module_mgr->GetRole(), retrieve_other_currency.other_currency[i], m_other_currency_param.other_currency[i], type, NULL);
		}
	}

	return true;
}

bool Money::OnAddMoney(int money_type, Int64 money_num, const char * type, bool is_use_in_cross, bool is_no_show_on_client)
{
	if (money_type < MONEY_TYPE_GOLD || money_type >= MONEY_TYPE_MAX || money_num <= 0)
	{
		return false;
	}
	UNSTD_STATIC_CHECK(MONEY_TYPE_MAX == 20);
	switch (money_type)
	{
	case MONEY_TYPE_GOLD:
		this->AddGold(money_num, type);
		break;
	case MONEY_TYPE_IMMORTAL_COIN:
		this->AddImmortalCoin(money_num, type);
		break;
	case MONEY_TYPE_COIN_BIND:
		this->AddCoinBind(money_num, type);
		break;
	default:
		this->AddOtherCoin(money_num, money_type, type);
		break;
	}
	return true;
}

bool Money::UseMoney(int money_type, Int64 money_num, const char * type, bool is_use_in_cross, bool is_no_show_on_client)
{
	if (money_type < MONEY_TYPE_GOLD || money_type >= MONEY_TYPE_MAX)
	{
		return false;
	}
	bool ret = false;
	UNSTD_STATIC_CHECK(MONEY_TYPE_MAX == 20);
	switch (money_type)
	{
	case MONEY_TYPE_GOLD:
		ret = this->UseGold(money_num, type);
		break;
	case MONEY_TYPE_IMMORTAL_COIN:
		ret = this->UseImmortalCoin(money_num, type);
		break;
	case MONEY_TYPE_COIN_BIND:
		ret = this->UseCoinBind(money_num, type);
		break;
	default:
	case MONEY_TYPE_YUAN_SUI_ZHI_LING:
	case MONEY_TYPE_DUI_ZHANG_GONG_ZI:
	case MONEY_TYPE_CLOUD_ARENA_COIN:
	case MONEY_TYPE_SILVER_COIN:
	case MONEY_TYPE_GUESS_COIN:
	case MONEY_TYPE_CAN_HUN:
		ret = this->UseOtherCoin(money_num, money_type, type);
		break;
	}
	return ret;
}

void Money::GmClearMoney(int money_type, const char * reason)
{
	if(money_type < -1 || money_type >= MONEY_TYPE_MAX) return;

	for (int i = 0; i < MONEY_TYPE_MAX; i++)
	{
		if(-1 != money_type && i != money_type) continue;

		int use_money_num = 0;
		if (MONEY_TYPE_GOLD == money_type)
		{
			use_money_num = this->GetGold();
		}
		else if (MONEY_TYPE_IMMORTAL_COIN == money_type)
		{
			use_money_num = this->GetImmortalCoin();
		}
		else if (MONEY_TYPE_COIN_BIND == money_type)
		{
			use_money_num = this->GetCoinBind();
		}
		else
		{
			use_money_num = this->GetMoneyByType(i);
		}

		this->UseMoney(i, use_money_num, reason);
	}
}

void Money::GmRetieveMoney(int role_id, int money_type, int money_num, int retiveve_type, int retrieve_reason)
{
	Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(role_id));
	if (NULL == role) return;

	MoneyOtherParam retrieve_other_currency;
	retrieve_other_currency.Reset();

	if (retiveve_type == 1)
	{
		role->GetRoleModuleManager()->GetMoney()->RetrieveMoney(money_num, 0, 0, money_num, retrieve_reason);
	}
	else if (retiveve_type == 2)
	{
		retrieve_other_currency.other_currency[money_type] = money_num;
		role->GetRoleModuleManager()->GetMoney()->RetrieveMoney(retrieve_other_currency, retrieve_reason);
	}

}