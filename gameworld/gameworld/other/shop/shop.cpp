#include "shop.h"
#include "config/logicconfigmanager.hpp"
#include "shopconfig.h"
#include "other/rolemodulemanager.hpp"
#include "servercommon/errornum.h"
#include "item/money.h"
#include "item/knapsack.h"
#include "protocol/msgshop.h"
#include "other/event/eventhandler.hpp"
#include "obj/character/role.h"
#include "global/colosseum/colosseummanager.hpp"
#include "other/prestige/prestigeconfig.hpp"
#include "other/prestige/prestige.hpp"

#include "other/huashen/huashenmanager.hpp"
#include "other/roleactivity/roleactivitymanager.hpp"
#include "other/roleactivity/impl/roleactivitygodtoken.hpp"
#include "other/roleactivity/impl/roleactivityfortunemaze.hpp"
#include "global/marketmanager/marketmanager.hpp"

Shop::Shop()
{
}

Shop::~Shop()
{
}

void Shop::Init(RoleModuleManager * role_module_manager, const ShopParam & param)
{
	m_role_module_mgr = role_module_manager;
	m_param = param;
}

void Shop::GetOtherInitParam(ShopParam * param)
{
	*param = m_param;
}

void Shop::OnUserLogin()
{
	this->SendPurchaseNotify();
}

void Shop::OnResetModuleData(unsigned int old_dayid, unsigned int now_dayid)
{
	this->CalcPurchaseNotify();
	LOGIC_CONFIG->GetShopConfig()->ResetBuyCount(SHOP_BUY_LIMIT_TYPE_DAILY, &m_param);
	LOGIC_CONFIG->GetShopConfig()->ResetGoldBuyCount(SHOP_BUY_LIMIT_TYPE_DAILY, &m_param);
	this->SendAllInfo();
}

void Shop::OnResetModuleDataWeek()
{
	LOGIC_CONFIG->GetShopConfig()->ResetBuyCount(SHOP_BUY_LIMIT_TYPE_WEEKLY, &m_param);
	LOGIC_CONFIG->GetShopConfig()->ResetGoldBuyCount(SHOP_BUY_LIMIT_TYPE_WEEKLY, &m_param);
}

void Shop::OnBuy(int index, int num)
{
	if (index < 0 || index >= SHOP_ITEM_MAX_COUNT)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	const ShopItemCfg * cfg = LOGIC_CONFIG->GetShopConfig()->GetShopItemCfg(index);
	if (NULL == cfg)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	if (cfg->limit_type > SHOP_BUY_LIMIT_TYPE_INVALID)
	{
		if (m_param.shop_buy_count[index] + num > cfg->limit_times)
		{
			m_role_module_mgr->NoticeNum(errornum::EN_SHOP_BUY_LIMIT);
			return;
		}
	}

	if (!m_role_module_mgr->GetKnapsack()->CheckForPut(cfg->item_id, num, false))
	{
		m_role_module_mgr->GetKnapsack()->SendErrorNotice();
		//m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return;
	}

	///////////////////尝试消耗 成功则给予 失败则return/////////////////
	switch (cfg->money_type)
	{
	case CURRENCY_TYPE_GOLD:
		if (!m_role_module_mgr->GetMoney()->UseGold(cfg->price * num, __FUNCTION__))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_GOLD_NOT_ENOUGH);
			return;
		}
		break;
	case CURRENCY_TYPE_IMMORTAL_COIN:
		if (!m_role_module_mgr->GetMoney()->UseImmortalCoin(cfg->price * num, __FUNCTION__))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_IMMORTAL_COIN_NOT_ENOUGH);
			return;
		}
		break;
	case CURRENCY_TYPE_COIN_BIND:
		if (!m_role_module_mgr->GetMoney()->UseCoinBind(cfg->price * num, __FUNCTION__))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_COIN_NOT_ENOUGH);
			return;
		}
		break;
	}
	
	if (!m_role_module_mgr->GetKnapsack()->Put(ItemConfigData(cfg->item_id, cfg->is_bind > 0, num), PUT_REASON_SHOP))
	{
		m_role_module_mgr->GetKnapsack()->SendErrorNotice();
		return;
	}

	if (cfg->limit_type != SHOP_BUY_LIMIT_TYPE_INVALID)
	{
		m_param.shop_buy_count[index] += num;
		this->SendSingleInfo(index);
	}
}

void Shop::OnGoldShopBuy(int index, int num)
{
	if (index < 0 || index >= GOLD_SHOP_ITEM_MAX_COUNT)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	const GoldShopItemCfg * cfg = LOGIC_CONFIG->GetShopConfig()->GetGoldShopItemCfg(index);
	if (NULL == cfg)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	if (cfg->limit_type > SHOP_BUY_LIMIT_TYPE_INVALID)
	{
		if (m_param.gold_shop_buy_count[index] + num > cfg->limit_times)
		{
			m_role_module_mgr->NoticeNum(errornum::EN_SHOP_BUY_LIMIT);
			return;
		}
	}

	if (!m_role_module_mgr->GetKnapsack()->CheckForPut(cfg->item_id, num))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return;
	}

	///////////////////尝试消耗 成功则给予 失败则return/////////////////
	switch (cfg->money_type)
	{
	case CURRENCY_TYPE_GOLD:
		if (!m_role_module_mgr->GetMoney()->UseGold(cfg->price * num, __FUNCTION__))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_GOLD_NOT_ENOUGH);
			return;
		}
		break;
	case CURRENCY_TYPE_IMMORTAL_COIN:
		if (!m_role_module_mgr->GetMoney()->UseImmortalCoin(cfg->price * num, __FUNCTION__))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_IMMORTAL_COIN_NOT_ENOUGH);
			return;
		}
		break;
	case CURRENCY_TYPE_COIN_BIND:
		if (!m_role_module_mgr->GetMoney()->UseCoinBind(cfg->price * num, __FUNCTION__))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_COIN_NOT_ENOUGH);
			return;
		}
		break;
	}

	m_role_module_mgr->GetKnapsack()->Put(ItemConfigData(cfg->item_id, cfg->is_bind > 0, num), PUT_REASON_SHOP);

	if (cfg->limit_type != SHOP_BUY_LIMIT_TYPE_INVALID)
	{
		m_param.gold_shop_buy_count[index] += num;
		this->SendGoldSingleInfo(index);
	}

	if (cfg->money_type == CURRENCY_TYPE_GOLD)
	{
		EventHandler::Instance().OnGoldShopConsume(m_role_module_mgr->GetRole(),cfg->price * num);
	}
}

void Shop::OnFastBuy(int item_id, int num)
{
	if (num <= 0) return;

	const FastBuyContainer * buy_cfg = LOGIC_CONFIG->GetShopConfig()->GetFastBuyCfg(item_id);
	if (NULL == buy_cfg)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_SHOP_ITEM_NOT_SUPPORT_FAST_BUY);
		return;
	}

	if (!m_role_module_mgr->GetKnapsack()->CheckForPut(item_id, num))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return;
	}

	int need_money = buy_cfg->unit_price * num;
	switch (buy_cfg->currency_type)
	{
	case MONEY_TYPE_SILVER_COIN:
		if (!m_role_module_mgr->GetMoney()->UseOtherCoin(need_money, MONEY_TYPE_SILVER_COIN, "ShopFastBuy"))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_SILVER_COIN_NOT_ENOUGH);
			return;
		}
		break;
	case MONEY_TYPE_GOLD:
		if (!m_role_module_mgr->GetMoney()->GoldMoreThan(need_money))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_GOLD_NOT_ENOUGH);
			return;
		}

		if (!m_role_module_mgr->GetMoney()->UseGold(need_money, "ShopFastBuy"))
		{
			return;
		}
		break;
	case MONEY_TYPE_IMMORTAL_COIN:
		if (!m_role_module_mgr->GetMoney()->ImmortalCoinMoreThan(need_money))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_IMMORTAL_COIN_NOT_ENOUGH);
			return;
		}

		if (!m_role_module_mgr->GetMoney()->UseImmortalCoin(need_money, "ShopFastBuy"))
		{
			return;
		}
		break;
	case MONEY_TYPE_COIN_BIND:
		if (!m_role_module_mgr->GetMoney()->CoinBindMoreThan(need_money))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_COIN_NOT_ENOUGH);
			return;
		}

		if (!m_role_module_mgr->GetMoney()->UseCoinBind(need_money, "ShopFastBuy"))
		{
			return;
		}
		break;
	default:
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	// 货币已消耗，注意不能中途返回
	m_role_module_mgr->GetKnapsack()->Put(ItemConfigData(item_id, 0 != buy_cfg->is_bind, num), PUT_REASON_SHOP);

	const ItemBase* itembase = ITEMPOOL->GetItem(item_id);
	if (NULL != itembase)
	{
		if (itembase->GetItemSubType() == ItemNamespace::ITEM_SUB_TYPE_GEM)
		{
			RoleActivityGodToken * god_token = static_cast<RoleActivityGodToken *>
				(m_role_module_mgr->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_GOD_TOKEN));
			if (NULL != god_token)
			{
				god_token->OnRAGodTokenAddCount(RA_GOD_TOKEN_DAY_TYPE_SHOP_BUY_GEM, num);
				god_token->OnRAGodTokenWeekAddCount(RA_GOD_TOKEN_TYPE_WEEK_SHOP_BUY_GEM, num);
			}

			RoleActivityFortuneMaze * fortune_maze = static_cast<RoleActivityFortuneMaze *>
				(m_role_module_mgr->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_FORTUNE_MAZE));
			if (NULL != fortune_maze)
			{
				fortune_maze->BuyStone(num);
			}
		}
	}
}

bool Shop::GetFastBuyGoldUnitPrice(int item_id, ARG_OUT int* price)
{
	if (NULL == price) return false;

	const FastBuyContainer * buy_cfg = LOGIC_CONFIG->GetShopConfig()->GetFastBuyCfg(item_id);
	if (NULL == buy_cfg)
	{
		return false;
	}

	if (buy_cfg->currency_type != MONEY_TYPE_GOLD)
	{
		return false;
	}

	*price = buy_cfg->unit_price;

	return true;
}

void Shop::OnBuyNewShop(int index, int num, short is_huashen_buy)
{
	const NewShopItemCfg* cfg = LOGIC_CONFIG->GetShopConfig()->GetNewShopItemCfg(index);
	if (NULL == cfg)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	if (!cfg->is_show)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_NEW_SHOP_ITEM_NOT_SHOW);
		return;
	}

	if (CONVERT_REQUIRE_TYPE_ROLE_LEVEL == cfg->require_type)
	{
		if (m_role_module_mgr->GetRole()->GetLevel() < cfg->require_value)
		{
			m_role_module_mgr->NoticeNum(errornum::EN_ROLE_LEVEL_NOT_ENOUTH);
			return;
		}
	}
	else if (CONVERT_REQUIRE_TYPE_VILLAGE_PRESTIGE == cfg->require_type)
	{
		int village_id = cfg->require_value;
		const PrestigeVillageCfg* village_cfg = LOGIC_CONFIG->GetPrestigeConfig()->GetVillageCfg(village_id);
		if (NULL == village_cfg)
		{
			m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
			return;
		}

		static const int PRESTIGE_REVERENCE = 3; // 声望-崇敬
		if (!m_role_module_mgr->GetPrestige()->CheckPrestige(village_id, PRESTIGE_REVERENCE))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_PRESTIGE_NOT_ENOUGH);
			return;
		}
	}
	else if (CONVERT_REQUIRE_TYPE_COLOSSEUM == cfg->require_type)
	{
		int rank = 0;
		bool is_in_pool = false;
		ColosseumManager::Instance().GetUserGladiator(m_role_module_mgr->GetRole()->GetUserId(), &rank, &is_in_pool);

		if (!is_in_pool && rank >= cfg->require_value)
		{
			m_role_module_mgr->NoticeNum(errornum::EN_ITEM_REQUIRE_COLOSSEUM);
			return;
		}
	}
	else if (CONVERT_REQUIRE_TYPE_CLOUD_ARENA_SEASON_HIGHEST_DAN_ID == cfg->require_type)
	{
		int dan_id = m_role_module_mgr->GetCommonData().cloud_arena_season_highest_dan_id;
		if (dan_id < cfg->require_value)
		{
			m_role_module_mgr->NoticeNum(errornum::EN_ITEM_REQUIRE_CLOUD_ARENA_DAN);
			return;
		}
	}
	else if (CONVERT_REQUIRE_TYPE_CLOUD_ARENA_SEASON_HIGHEST_RANK == cfg->require_type)
	{
		int rank = m_role_module_mgr->GetCommonData().cloud_arena_season_highest_rank;
		if (rank >= cfg->require_value) // 因为服务端rank是从0开始，配置从1开始，所以用>=
		{
			m_role_module_mgr->NoticeNum(errornum::EN_ITEM_REQUIRE_CLOUD_ARENA_RANK);
			return;
		}
	}

	ItemConfigData item = cfg->item;
	item.num *= num;
	if (!m_role_module_mgr->GetKnapsack()->CheckForPut(item.item_id, item.num, false))
	{
		m_role_module_mgr->GetKnapsack()->SendErrorNotice();
		//m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return;
	}

	///////////////////尝试消耗 成功则给予 失败则return/////////////////
	int price = cfg->price * cfg->item.num * num;
	switch (cfg->price_type)
	{
	case CURRENCY_TYPE_GOLD:
		if (!m_role_module_mgr->GetMoney()->UseGold(price, __FUNCTION__))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_GOLD_NOT_ENOUGH);
			return;
		}
		break;
	case CURRENCY_TYPE_IMMORTAL_COIN:
		if (!m_role_module_mgr->GetMoney()->UseImmortalCoin(price, __FUNCTION__))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_IMMORTAL_COIN_NOT_ENOUGH);
			return;
		}
		break;
	case CURRENCY_TYPE_COIN_BIND:
		if (!m_role_module_mgr->GetMoney()->UseCoinBind(price, __FUNCTION__))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_COIN_NOT_ENOUGH);
			return;
		}
		break;

	default:
		if (!m_role_module_mgr->GetMoney()->UseOtherCoin(price, cfg->price_type, __FUNCTION__))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_OTHER_MONEY_NOT_ENOUGH);
			return;
		}
		break;
	}

	if (!m_role_module_mgr->GetKnapsack()->Put(item, PUT_REASON_SHOP))
	{
		m_role_module_mgr->GetKnapsack()->SendErrorNotice();
		//m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return;
	}
	if (cfg->price_type == CURRENCY_TYPE_COIN_BIND)
		EventHandler::Instance().OnNewShopConsume(m_role_module_mgr->GetRole(),price);

	const ItemBase* itembase = ITEMPOOL->GetItem(cfg->item.item_id);
	if (NULL != itembase)
	{
		if (itembase->GetItemSubType() == ItemNamespace::ITEM_SUB_TYPE_GEM)
		{
			RoleActivityGodToken * god_token = static_cast<RoleActivityGodToken *>
				(m_role_module_mgr->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_GOD_TOKEN));
			if (NULL != god_token)
			{
				god_token->OnRAGodTokenAddCount(RA_GOD_TOKEN_DAY_TYPE_SHOP_BUY_GEM, num);
				god_token->OnRAGodTokenWeekAddCount(RA_GOD_TOKEN_TYPE_WEEK_SHOP_BUY_GEM, num);
			}

			RoleActivityFortuneMaze * fortune_maze = static_cast<RoleActivityFortuneMaze *>
				(m_role_module_mgr->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_FORTUNE_MAZE));
			if (NULL != fortune_maze)
			{
				fortune_maze->BuyStone(num);
			}
		}
		else if (0 != is_huashen_buy && itembase->GetItemSubType() == ItemNamespace::ITEM_SUB_TYPE_ELEMENT_CRYSTAL)
		{
			if (m_role_module_mgr->GetHuaShenManager()->IsSeasonInTime())
			{
				m_role_module_mgr->GetHuaShenManager()->OnSynKnapackData(true);
			}
		}
	}
}

void Shop::GmReserBuyCount(int type)
{
	LOGIC_CONFIG->GetShopConfig()->ResetBuyCount(type, &m_param);
	this->SendAllInfo();
}

void Shop::SendAllInfo()
{
	Protocol::SCShopAllInfo info;

	memcpy(info.shop_buy_count, m_param.shop_buy_count, sizeof(info.shop_buy_count));

	m_role_module_mgr->NetSend(&info, sizeof(info));

	Protocol::SCGoldShopAllInfo info2;
	memcpy(info2.shop_buy_count, m_param.gold_shop_buy_count, sizeof(info2.shop_buy_count));
	m_role_module_mgr->NetSend(&info2, sizeof(info2));
}

void Shop::SendSingleInfo(int index)
{
	if (index < 0 || index >= SHOP_ITEM_MAX_COUNT) return;

	Protocol::SCShopSingleInfo info;
	info.index = index;
	info.count = m_param.shop_buy_count[index];
	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void Shop::SendGoldSingleInfo(int index)
{
	if (index < 0 || index >= GOLD_SHOP_ITEM_MAX_COUNT) return;

	Protocol::SCGoldShopSingleInfo info;
	info.index = index;
	info.count = m_param.gold_shop_buy_count[index];
	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void Shop::CancelSingleNotify(int index)
{
	m_param.active_flag.UnSetBit(index);
}

void Shop::CalcPurchaseNotify()
{
	m_param.active_flag.Reset();
	for (int idx = 0; idx < GOLD_SHOP_ITEM_MAX_COUNT; ++idx)
	{
		const GoldShopItemCfg * cfg = LOGIC_CONFIG->GetShopConfig()->GetGoldShopItemCfg(idx);
		if (NULL == cfg || cfg->limit_type != SHOP_BUY_LIMIT_TYPE_DAILY || m_param.gold_shop_buy_count[idx] < cfg->limit_times) continue;

		m_param.active_flag.SetBit(idx);
	}
}

void Shop::SendPurchaseNotify()
{
	Protocol::SCGoldShopPurchaseNotifys gspn;
	gspn.notify_flag = m_param.active_flag;
	m_role_module_mgr->NetSend(&gspn, sizeof(gspn));
}