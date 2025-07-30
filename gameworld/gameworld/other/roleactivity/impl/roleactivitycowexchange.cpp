#include "roleactivitycowexchange.hpp"

#include "engineadapter.h"

#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivitycowexchangeconfig.hpp"

#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"

#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"

#include "servercommon/errornum.h"
#include "servercommon/noticenum.h"
#include "gamelog.h"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "servercommon/string/gameworldstr.h"
#include "gameworld/other/route/mailroute.hpp"

RoleActivityCowExchange::RoleActivityCowExchange(int type)
	:RoleActivity(type)
{

}

RoleActivityCowExchange::~RoleActivityCowExchange()
{

}

void RoleActivityCowExchange::Init(RoleModuleManager *mgr, const RoleActivityRoleData &p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(RACowExchangeParam));
}

void RoleActivityCowExchange::GetInitParam(RoleActivityRoleData *p)
{
	memcpy(p->base_data, &m_param, sizeof(RACowExchangeParam));
}

void RoleActivityCowExchange::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	//检查刷新
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_COW_EXCHANGE))
	{
		return;
	}

	int rand_open_day = LOGIC_CONFIG->GetRandActivityConfigManager()->GetActOpenDay(ramgr, RAND_ACTIVITY_TYPE_COW_EXCHANGE);

	if (m_param.open_day == rand_open_day)
	{
		//return;//屏蔽让GM命令能直接刷新题目
	}

	const RandActivityCowExchangeConfig * config = static_cast<const RandActivityCowExchangeConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_COW_EXCHANGE));
	if (config != NULL)
	{
		const RACowExchangeCfg * old_cfg = config->GetCowExchangeCfgByActivityDay(m_param.open_day, 0);
		const RACowExchangeCfg * new_cfg = config->GetCowExchangeCfgByActivityDay(rand_open_day, 0);
		if (NULL != old_cfg && NULL != new_cfg)
		{
			if (old_cfg->section_start != new_cfg->section_start)
			{
				//阶段刷新
				memset(m_param.activity_buy_times, 0, sizeof(m_param.activity_buy_times));
			}
		}
	}

	m_param.open_day = rand_open_day;
	m_param.DayReset();
	this->SendInfo();
	this->SetRoleDataChange();
}

void RoleActivityCowExchange::OnLoginSendInfo()
{
	//检查刷新
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_COW_EXCHANGE))
	{
		return;
	}

	this->SendInfo();
}

void RoleActivityCowExchange::CheckInitRandActivity(int rand_act_type, bool is_force /*= false*/)
{
	if (RAND_ACTIVITY_TYPE_COW_EXCHANGE != rand_act_type)
	{
		return;
	}

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(rand_act_type))
	{
		return;
	}

	// 防止误启动活动，不在跨服启动的活动会导致重置
	if (CrossConfig::Instance().IsHiddenServer())
	{
		RandActivity *activity = ramgr->ForceGetRandActivity(rand_act_type);
		if (NULL != activity && !activity->CanSyncActivityDataToCross())
		{
			return;
		}
	}

	bool in_activity_time = ramgr->IsInRandActivityOpenTime(rand_act_type, m_param.begin_time);
	if (is_force || !in_activity_time)
	{
		m_param.Reset();
		m_param.begin_time = static_cast<unsigned int>(EngineAdapter::Instance().Time());
		int rand_open_day = LOGIC_CONFIG->GetRandActivityConfigManager()->GetActOpenDay(ramgr, RAND_ACTIVITY_TYPE_COW_EXCHANGE);
		const RandActivityCowExchangeConfig * config = static_cast<const RandActivityCowExchangeConfig *>
			(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_COW_EXCHANGE));
		if (NULL == config)
		{
			m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
			return;
		}
		m_param.open_day = rand_open_day;
		this->SetRoleDataChange();
		this->SendInfo();
	}
}

void RoleActivityCowExchange::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor)
{
	if (NULL == raor)
	{
		return;
	}

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_COW_EXCHANGE))
	{
		return;
	}
	this->CheckInitRandActivity(RAND_ACTIVITY_TYPE_COW_EXCHANGE);

	switch (raor->opera_type)
	{
	case 0:
		{
			this->SendInfo();
		}
		break;
	case 1://购买道具，param1，type，param2，seq
		{
			this->BuyItem(raor->param_1, raor->param_2);
		}
		break;
	}
}

bool RoleActivityCowExchange::OnNoreItemUse(int add_jifen)
{
	if (add_jifen <= 0)return false;
	m_param.m_jifen += add_jifen;
	m_param.m_unlock_integral += add_jifen;
	Protocol::SCRAMoneyAddInfo add_info;
	add_info.rand_activity = RAND_ACTIVITY_TYPE_COW_EXCHANGE;
	add_info.add_money = add_jifen;
	m_role_module_mgr->NetSend(&add_info, sizeof(add_info));
	this->SendInfo();
	return true;
}

bool RoleActivityCowExchange::OnCmdBuyItem(int type_id,int real_pay)
{
	//直购操作不允许返回，一旦购买失败均是重大问题
	const RandActivityCowExchangeConfig * config = static_cast<const RandActivityCowExchangeConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_COW_EXCHANGE));
	if (NULL == config)
	{
		gamelog::g_log_serious_error.printf(LL_INFO, "RoleActivityCowExchange::OnCmdBuyItem Err!user[%d %s] type_id[%d] chongzhi_gold[%d] reason[config NULL]",
			m_role_module_mgr->GetUid(), m_role_module_mgr->GetRole()->GetName(), type_id, real_pay);
		return false;
	}

	const RACowExchangeCfg * buy_item_cfg = config->GetCowExchangeCfgByIndex(type_id);
	if (NULL == buy_item_cfg)
	{
		gamelog::g_log_serious_error.printf(LL_INFO, "RoleActivityCowExchange::OnCmdBuyItem Err!user[%d %s] type_id[%d] chongzhi_gold[%d] reason[buy_item_cfg NULL]",
			m_role_module_mgr->GetUid(), m_role_module_mgr->GetRole()->GetName(), type_id, real_pay);
		return false;
	}

	if (real_pay < buy_item_cfg->price)
	{
		gamelog::g_log_serious_error.printf(LL_INFO, "RoleActivityCowExchange::OnCmdBuyItem Err!user[%d %s] type_id[%d] chongzhi_gold[%d] reason[real_pay < buy_item_cfg->price NULL]",
			m_role_module_mgr->GetUid(), m_role_module_mgr->GetRole()->GetName(), type_id, real_pay);
		return false;
	}

	short & today_buy_times = m_param.today_buy_times[buy_item_cfg->seq];
	short & activity_buy_times = m_param.activity_buy_times[buy_item_cfg->seq];
	switch (buy_item_cfg->limit_type)
	{
	case RACowExchangeCfg::LIMIT_TYPE_TODAY:
		{
			if (today_buy_times > 0 && today_buy_times >= buy_item_cfg->today_times)
			{
				m_role_module_mgr->NoticeNum(errornum::EN_RA_SPRING_FESTIVAL_STORE_TIMES_LIMIT);
				return false;
			}
		}
		break;
	case RACowExchangeCfg::LIMIT_TYPE_ACTIVITY:
		{
			if (activity_buy_times > 0 && activity_buy_times >= buy_item_cfg->buy_times)
			{
				m_role_module_mgr->NoticeNum(errornum::EN_RA_SPRING_FESTIVAL_STORE_TIMES_LIMIT);
				return false;
			}
		}
		break;
	default:
		break;
	}

	if (m_role_module_mgr->GetKnapsack()->CheckForPut(buy_item_cfg->reward_item.item_id, buy_item_cfg->reward_item.num))
	{
		if (!m_role_module_mgr->GetKnapsack()->Put(buy_item_cfg->reward_item, PUT_REASON_RA_COW_EXCHANGE_STORE))
		{
			gamelog::g_log_serious_error.printf(LL_INFO, "RoleActivityCowExchange::OnCmdBuyItem Err!user[%d %s] type_id[%d] chongzhi_gold[%d] reason[buy_item_cfg NULL]",
				m_role_module_mgr->GetUid(), m_role_module_mgr->GetRole()->GetName(), type_id, real_pay);
			return false;
		}
	}
	else
	{
		//放入背包失败，改发邮件
		static MailContentParam contentparam; contentparam.Reset();
		const ItemBase * item_base = ITEMPOOL->GetItem(buy_item_cfg->reward_item.item_id);

		if (item_base != NULL)
		{
			contentparam.item_list[0].item_id = buy_item_cfg->reward_item.item_id;
			contentparam.item_list[0].num = buy_item_cfg->reward_item.num;
			contentparam.item_list[0].is_bind = (buy_item_cfg->reward_item.is_bind) ? 1 : 0;
			contentparam.item_list[0].invalid_time = item_base->CalInvalidTime();
		}
		int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_randactivity_reward_subject);
		int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_randactivity_reward_content, RAND_ACTIVITY_TYPE_COW_EXCHANGE);
		if (lensub > 0 && length > 0)
		{
			MailRoute::MailToUser(m_role_module_mgr->GetUid(), MAIL_REASON_DEFAULT, contentparam);
		}
	}

	today_buy_times += 1;
	activity_buy_times += 1;

	this->SendInfo();
	this->SetRoleDataChange();
	return true;
}

void RoleActivityCowExchange::SendInfo()
{
	Protocol::SCRACowExchangeInfo info;
	
	info.m_jifen = m_param.m_jifen;
	info.m_unlock_integral = m_param.m_unlock_integral;
	memcpy(info.today_buy_times, m_param.today_buy_times, sizeof(info.today_buy_times));
	memcpy(info.activity_buy_times, m_param.activity_buy_times, sizeof(info.activity_buy_times));
	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void RoleActivityCowExchange::BuyItem(int type_id, int seq_id_check)
{
	if (seq_id_check < 0 || seq_id_check >= MAX_LOG_TIMES)
	{
		return;
	}
	const RandActivityCowExchangeConfig * config = static_cast<const RandActivityCowExchangeConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_COW_EXCHANGE));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	if (0 == m_param.open_day)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	const RACowExchangeCfg *check_item_cfg = config->GetCowExchangeCfgByActivityDay(m_param.open_day, seq_id_check);
	if (NULL == check_item_cfg)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	const RACowExchangeCfg * buy_item_cfg = config->GetCowExchangeCfgByIndex(type_id);
	if (NULL == buy_item_cfg)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	//检查购买道具的对应是否正确
	if (buy_item_cfg->seq != seq_id_check || buy_item_cfg->seq < 0 || buy_item_cfg->seq >= MAX_LOG_TIMES)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	if (buy_item_cfg->unlock_integral > m_param.m_unlock_integral)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RA_COW_EXCHANGE_JIFEN_LIMIT);
		return;
	}

	short &today_buy_times = m_param.today_buy_times[buy_item_cfg->seq];
	short &activity_buy_times = m_param.activity_buy_times[buy_item_cfg->seq];
	switch (buy_item_cfg->limit_type)
	{
	case RACowExchangeCfg::LIMIT_TYPE_TODAY:
		{
			if (today_buy_times > 0 && today_buy_times >= buy_item_cfg->today_times)
			{
				m_role_module_mgr->NoticeNum(errornum::EN_RA_SPRING_FESTIVAL_STORE_TIMES_LIMIT);
				return;
			}
		}
		break;
	case RACowExchangeCfg::LIMIT_TYPE_ACTIVITY:
		{
			if (activity_buy_times > 0 && activity_buy_times >= buy_item_cfg->buy_times)
			{
				m_role_module_mgr->NoticeNum(errornum::EN_RA_SPRING_FESTIVAL_STORE_TIMES_LIMIT);
				return;
			}
		}
		break;
	default:
		break;
	}

	//bool is_free = false;
	//检查消耗
	switch (buy_item_cfg->price_type)
	{
	case RACowExchangeCfg::PRICE_TYPE_COIN:
		{
			if (!m_role_module_mgr->GetMoney()->CoinBindMoreThan(buy_item_cfg->price))
			{
				m_role_module_mgr->NoticeNum(errornum::EN_COIN_NOT_ENOUGH);
				return;
			}
			m_role_module_mgr->GetMoney()->UseCoinBind(buy_item_cfg->price, __FUNCTION__);
		}
		break;
	case RACowExchangeCfg::PRICE_TYPE_JIFEN:
		{
			if (m_param.m_jifen < buy_item_cfg->price)
			{
				m_role_module_mgr->NoticeNum(errornum::EN_RA_COW_LUCKY_EXCHANGE_JIFEN_LIMIT);
				return;
			}
			m_param.m_jifen -= buy_item_cfg->price;
		}
		break;
	case RACowExchangeCfg::PRICE_TYPE_GOLD:
		{
			if (!m_role_module_mgr->GetMoney()->GoldMoreThan(buy_item_cfg->price))
			{
				m_role_module_mgr->NoticeNum(errornum::EN_GOLD_NOT_ENOUGH);
				return;
			}
			m_role_module_mgr->GetMoney()->UseGold(buy_item_cfg->price, __FUNCTION__);
		}
		break;
	case RACowExchangeCfg::PRICE_TYPE_MONEY:
		{
			//直购自行返回
			return;
		}
		break;
	default:
		break;
	}

	if (!m_role_module_mgr->GetKnapsack()->CheckForPut(buy_item_cfg->reward_item.item_id, buy_item_cfg->reward_item.num))
	{
		//放入背包失败，改发邮件
		static MailContentParam contentparam; contentparam.Reset();

		const ItemBase * item_base = ITEMPOOL->GetItem(buy_item_cfg->reward_item.item_id);

		if (item_base != NULL)
		{
			contentparam.item_list[0].item_id = buy_item_cfg->reward_item.item_id;
			contentparam.item_list[0].num = buy_item_cfg->reward_item.num;
			contentparam.item_list[0].is_bind = (buy_item_cfg->reward_item.is_bind) ? 1 : 0;
			contentparam.item_list[0].invalid_time = item_base->CalInvalidTime();
		}
		int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_randactivity_reward_subject);
		int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_randactivity_reward_content, RAND_ACTIVITY_TYPE_COW_EXCHANGE);
		if (lensub > 0 && length > 0)
		{
			MailRoute::MailToUser(m_role_module_mgr->GetUid(), MAIL_REASON_DEFAULT, contentparam);
		}
	}
	else
	{
		if (!m_role_module_mgr->GetKnapsack()->Put(buy_item_cfg->reward_item, PUT_REASON_RA_COW_EXCHANGE_STORE))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
			return;
		}
	}

	today_buy_times += 1;
	activity_buy_times += 1;

	this->SendInfo();
	this->SetRoleDataChange();
	this->OnParticipateRA(__FUNCTION__);

	gamelog::g_log_rand_activity.printf(LL_INFO, "RoleActivityCowExchange::BuyItem user[%d, %s],type_id[%d] seq_id_check[%d] level[%d], type[%d], reward_item[%d] num[%d]",
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), type_id, seq_id_check, m_role_module_mgr->GetRole()->GetLevel(),
		RAND_ACTIVITY_TYPE_COW_EXCHANGE, buy_item_cfg->reward_item.item_id, buy_item_cfg->reward_item.num);
}
