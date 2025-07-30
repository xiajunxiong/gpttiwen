#include "roleactivityspringfestivalstore.hpp"

#include "engineadapter.h"

#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivityspringfestivalstoreconfig.hpp"

#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"

#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"

#include "servercommon/errornum.h"
#include "servercommon/noticenum.h"
#include "gamelog.h"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "servercommon/string/gameworldstr.h"
#include "other/route/mailroute.hpp"

RoleActivitySpringFestivalStore::RoleActivitySpringFestivalStore(int type)
	:RoleActivity(type)
{

}

RoleActivitySpringFestivalStore::~RoleActivitySpringFestivalStore()
{

}

void RoleActivitySpringFestivalStore::Init(RoleModuleManager *mgr, const RoleActivityRoleData &p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(RASpringFesticalStoreParam));
}

void RoleActivitySpringFestivalStore::GetInitParam(RoleActivityRoleData *p)
{
	memcpy(p->base_data, &m_param, sizeof(RASpringFesticalStoreParam));
}

void RoleActivitySpringFestivalStore::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	//检查刷新
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_SPRING_FESTIVAL_STORE))
	{
		return;
	}

	int rand_open_day = LOGIC_CONFIG->GetRandActivityConfigManager()->GetActOpenDay(ramgr, RAND_ACTIVITY_TYPE_SPRING_FESTIVAL_STORE);
	int open_day = LOGIC_CONFIG->GetRandActivityConfigManager()->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_SPRING_FESTIVAL_STORE);
	if (m_param.open_day == rand_open_day)
	{
		//return;//屏蔽让GM命令能直接刷新题目
	}

	const RandActivitySpringFestivalStoreConfig * config = static_cast<const RandActivitySpringFestivalStoreConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_SPRING_FESTIVAL_STORE));
	if (NULL != config)
	{
		const RASpringFestivalStoreCfg * old_cfg = config->GetSpringFestivalStoreCfgByActivityDay(open_day, m_param.open_day, 0);
		const RASpringFestivalStoreCfg * new_cfg = config->GetSpringFestivalStoreCfgByActivityDay(open_day, rand_open_day, 0);
		if (NULL != old_cfg && NULL != new_cfg)
		{
			if (old_cfg->section_start != new_cfg->section_start)
			{
				//阶段刷新
				memset(m_param.activity_buy_times, 0, sizeof(m_param.activity_buy_times));
			}
		}

		if (NULL != new_cfg)
		{
			m_param.phase = new_cfg->phase;
		}
	}

	m_param.open_day = rand_open_day;
	m_param.DayReset();
	this->SendInfo();
	this->SetRoleDataChange();
}

void RoleActivitySpringFestivalStore::OnLoginSendInfo()
{
	//检查刷新
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_SPRING_FESTIVAL_STORE))
	{
		return;
	}

	this->SendInfo();
}

void RoleActivitySpringFestivalStore::CheckInitRandActivity(int rand_act_type, bool is_force /*= false*/)
{
	if (RAND_ACTIVITY_TYPE_SPRING_FESTIVAL_STORE != rand_act_type)
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
		int rand_open_day = LOGIC_CONFIG->GetRandActivityConfigManager()->GetActOpenDay(ramgr, RAND_ACTIVITY_TYPE_SPRING_FESTIVAL_STORE);
		int open_day = LOGIC_CONFIG->GetRandActivityConfigManager()->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_SPRING_FESTIVAL_STORE);
		const RandActivitySpringFestivalStoreConfig * config = static_cast<const RandActivitySpringFestivalStoreConfig *>
			(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_SPRING_FESTIVAL_STORE));
		if (NULL == config)
		{
			m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
			return;
		}
		m_param.open_day = rand_open_day;

		const RASpringFestivalStoreCfg * cfg = config->GetSpringFestivalStoreCfgByActivityDay(open_day, rand_open_day, 0);
		if (NULL != cfg)
		{
			m_param.phase = cfg->phase;
		}

		this->SetRoleDataChange();
		this->SendInfo();
	}
}

void RoleActivitySpringFestivalStore::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor)
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

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_SPRING_FESTIVAL_STORE))
	{
		return;
	}
	this->CheckInitRandActivity(RAND_ACTIVITY_TYPE_SPRING_FESTIVAL_STORE);

	switch (raor->opera_type)//0，下发，1购买
	{
	case 0://请求下发
		{
			this->SendInfo();
		}
		break;
	case 1://购买 param1 = 索引id，param2 = 对应seq
		{
			this->BuyItem(raor->param_1, raor->param_2);
		}
		break;
	}
}

bool RoleActivitySpringFestivalStore::OnCmdBuyItem(int type_id, int real_pay)
{
	//直购操作不允许返回，一旦购买失败均是重大问题
	const RandActivitySpringFestivalStoreConfig * config = static_cast<const RandActivitySpringFestivalStoreConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_SPRING_FESTIVAL_STORE));
	if (NULL == config)
	{
		gamelog::g_log_serious_error.printf(LL_INFO, "RoleActivitySpringFestivalStore::OnCmdBuyItem Err!user[%d %s] type_id[%d] chongzhi_gold[%d] reason[config NULL]",
			m_role_module_mgr->GetUid(), m_role_module_mgr->GetRole()->GetName(), type_id, real_pay);
		return false;
	}

	const RASpringFestivalStoreCfg * buy_item_cfg = config->GetSpringFestivalStoreCfgByIndex(type_id);
	if (NULL == buy_item_cfg)
	{
		gamelog::g_log_serious_error.printf(LL_INFO, "RoleActivitySpringFestivalStore::OnCmdBuyItem Err!user[%d %s] type_id[%d] chongzhi_gold[%d] reason[buy_item_cfg NULL]",
			m_role_module_mgr->GetUid(), m_role_module_mgr->GetRole()->GetName(), type_id, real_pay);
		return false;
	}

	if (real_pay < buy_item_cfg->price)
	{
		gamelog::g_log_serious_error.printf(LL_INFO, "RoleActivitySpringFestivalStore::OnCmdBuyItem Err!user[%d %s] type_id[%d] chongzhi_gold[%d] reason[real_pay < buy_item_cfg->price NULL]",
			m_role_module_mgr->GetUid(), m_role_module_mgr->GetRole()->GetName(), type_id, real_pay);
		return false;
	}

	short & today_buy_times = m_param.today_buy_times[buy_item_cfg->seq];
	short & activity_buy_times = m_param.activity_buy_times[buy_item_cfg->seq];
	switch (buy_item_cfg->limit_type)
	{
	case RASpringFestivalStoreCfg::LIMIT_TYPE_TODAY:
		{
			if (today_buy_times > 0 && today_buy_times >= buy_item_cfg->today_times)
			{
				m_role_module_mgr->NoticeNum(errornum::EN_RA_SPRING_FESTIVAL_STORE_TIMES_LIMIT);
				return false;
			}
		}
		break;
	case RASpringFestivalStoreCfg::LIMIT_TYPE_ACTIVITY:
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

	if (m_role_module_mgr->GetKnapsack()->CheckForPut(buy_item_cfg->item_reward.item_id, buy_item_cfg->item_reward.num))
	{
		if (!m_role_module_mgr->GetKnapsack()->Put(buy_item_cfg->item_reward, PUT_REASON_RA_SPRING_FESTIVAL_STORE))
		{
			gamelog::g_log_serious_error.printf(LL_INFO, "RoleActivitySpringFestivalStore::OnCmdBuyItem Err!user[%d %s] type_id[%d] chongzhi_gold[%d] reason[buy_item_cfg NULL]",
				m_role_module_mgr->GetUid(), m_role_module_mgr->GetRole()->GetName(), type_id, real_pay);
			return false;
		}
	}
	else
	{
		//放入背包失败，改发邮件
		static MailContentParam contentparam; contentparam.Reset();
		const ItemBase * item_base = ITEMPOOL->GetItem(buy_item_cfg->item_reward.item_id);

		if (item_base != NULL)
		{
			contentparam.item_list[0].item_id = buy_item_cfg->item_reward.item_id;
			contentparam.item_list[0].num = buy_item_cfg->item_reward.num;
			contentparam.item_list[0].is_bind = (buy_item_cfg->item_reward.is_bind) ? 1 : 0;
			contentparam.item_list[0].invalid_time = item_base->CalInvalidTime();
		}
		int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_randactivity_reward_subject);
		int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_randactivity_reward_content, RAND_ACTIVITY_TYPE_SPRING_FESTIVAL_STORE);
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

int RoleActivitySpringFestivalStore::OnBuyGiftItem(int seq, ARG_OUT int * price)
{
	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return 1;
	}

	if (!ramgr->IsRandActivityOpen(m_role_activity_type))
	{
		return 1;
	}

	const RandActivitySpringFestivalStoreConfig * config = static_cast<const RandActivitySpringFestivalStoreConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return 1;
	}
	int open_day = LOGIC_CONFIG->GetRandActivityConfigManager()->GetActRealOpenDay(ramgr, m_role_activity_type);
	const RASpringFestivalStoreCfg * buy_item_cfg = config->GetSpringFestivalStoreCfgByActivityDay(open_day , m_param.open_day, seq);
	if (NULL == buy_item_cfg)
	{
		return 1;
	}
	if (NULL != price)
	{
		*price = buy_item_cfg->price;
		return 0;
	}

	short & today_buy_times = m_param.today_buy_times[buy_item_cfg->seq];
	short & activity_buy_times = m_param.activity_buy_times[buy_item_cfg->seq];
	switch (buy_item_cfg->limit_type)
	{
	case RASpringFestivalStoreCfg::LIMIT_TYPE_TODAY:
	{
		if (today_buy_times > 0 && today_buy_times >= buy_item_cfg->today_times)
		{
			return 2;
		}
	}
	break;
	case RASpringFestivalStoreCfg::LIMIT_TYPE_ACTIVITY:
	{
		if (activity_buy_times > 0 && activity_buy_times >= buy_item_cfg->buy_times)
		{
			return 2;
		}
	}
	break;
	default:
		break;
	}

	if (m_role_module_mgr->GetKnapsack()->CheckForPut(buy_item_cfg->item_reward.item_id, buy_item_cfg->item_reward.num))
	{
		m_role_module_mgr->GetKnapsack()->Put(buy_item_cfg->item_reward, PUT_REASON_RA_SPRING_FESTIVAL_STORE);
	}
	else
	{
		//放入背包失败，改发邮件
		static MailContentParam contentparam; contentparam.Reset();
		const ItemBase * item_base = ITEMPOOL->GetItem(buy_item_cfg->item_reward.item_id);

		if (item_base != NULL)
		{
			contentparam.item_list[0].item_id = buy_item_cfg->item_reward.item_id;
			contentparam.item_list[0].num = buy_item_cfg->item_reward.num;
			contentparam.item_list[0].is_bind = (buy_item_cfg->item_reward.is_bind) ? 1 : 0;
			contentparam.item_list[0].invalid_time = item_base->CalInvalidTime();
		}
		int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_randactivity_reward_subject);
		int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_randactivity_reward_content, RAND_ACTIVITY_TYPE_SPRING_FESTIVAL_STORE);
		if (lensub > 0 && length > 0)
		{
			MailRoute::MailToUser(m_role_module_mgr->GetUid(), MAIL_REASON_DEFAULT, contentparam);
		}
	}

	today_buy_times += 1;
	activity_buy_times += 1;

	this->SendInfo();
	this->SetRoleDataChange();
	return 0;
}

void RoleActivitySpringFestivalStore::SendInfo()
{
	Protocol::SCRASpringFestivalStoreInfo info;
	
	info.open_day = m_param.open_day;
	info.phase = m_param.phase;
	memcpy(info.today_buy_times, m_param.today_buy_times, sizeof(info.today_buy_times));
	memcpy(info.activity_buy_times, m_param.activity_buy_times, sizeof(info.activity_buy_times));

	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void RoleActivitySpringFestivalStore::BuyItem(int answer_id, int seq_id_check)
{
	if (seq_id_check < 0 || seq_id_check >= MAX_LOG_TIMES)
	{
		return;
	}

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}

	const RandActivitySpringFestivalStoreConfig * config = static_cast<const RandActivitySpringFestivalStoreConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_SPRING_FESTIVAL_STORE));
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
	int open_day = LOGIC_CONFIG->GetRandActivityConfigManager()->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_SPRING_FESTIVAL_STORE);
	const RASpringFestivalStoreCfg *check_item_cfg = config->GetSpringFestivalStoreCfgByActivityDay(open_day, m_param.open_day, seq_id_check);
	if (NULL == check_item_cfg)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	const RASpringFestivalStoreCfg * buy_item_cfg = config->GetSpringFestivalStoreCfgByIndex(answer_id);
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

	//购买次数
	short has_buy_times_today = m_param.today_buy_times[buy_item_cfg->seq];
	short has_buy_times_activity = m_param.activity_buy_times[buy_item_cfg->seq];

	switch (buy_item_cfg->limit_type)
	{
	case RASpringFestivalStoreCfg::LIMIT_TYPE_TODAY:
		{
			if (has_buy_times_today > 0 && has_buy_times_today >= buy_item_cfg->today_times)
			{
				m_role_module_mgr->NoticeNum(errornum::EN_RA_SPRING_FESTIVAL_STORE_TIMES_LIMIT);
				return;
			}
		}
		break;
	case RASpringFestivalStoreCfg::LIMIT_TYPE_ACTIVITY:
		{
			if (has_buy_times_activity > 0 && has_buy_times_activity >= buy_item_cfg->buy_times)
			{
				m_role_module_mgr->NoticeNum(errornum::EN_RA_SPRING_FESTIVAL_STORE_TIMES_LIMIT);
				return;
			}
		}
		break;
	default:
		break;
	}

	//检查消耗
	switch (buy_item_cfg->price_type)
	{
	case RASpringFestivalStoreCfg::PRICE_TYPE_COIN:
		{
			if (!m_role_module_mgr->GetMoney()->CoinBindMoreThan(buy_item_cfg->price))
			{
				m_role_module_mgr->NoticeNum(errornum::EN_COIN_NOT_ENOUGH);
				return;
			}

			if (!m_role_module_mgr->GetMoney()->UseCoinBind(buy_item_cfg->price, "RoleActivitySpringFestivalStore::BuyItem"))
			{
				return;
			}
		}
		break;
	case RASpringFestivalStoreCfg::PRICE_TYPE_GOLD:
		{
			if (!m_role_module_mgr->GetMoney()->GoldMoreThan(buy_item_cfg->price))
			{
				m_role_module_mgr->NoticeNum(errornum::EN_GOLD_NOT_ENOUGH);
				return;
			}

			if (!m_role_module_mgr->GetMoney()->UseGold(buy_item_cfg->price, "RoleActivitySpringFestivalStore::BuyItem"))
			{
				return;
			}
		}
		break;
	case RASpringFestivalStoreCfg::PRICE_TYPE_MONEY:
		{
			//直接返回
			return;
		}
		break;
	default:
		break;
	}

	if (!m_role_module_mgr->GetKnapsack()->CheckForPut(buy_item_cfg->item_reward.item_id, buy_item_cfg->item_reward.num))
	{
		//放入背包失败，改发邮件
		static MailContentParam contentparam; contentparam.Reset();
		const ItemBase * item_base = ITEMPOOL->GetItem(check_item_cfg->item_reward.item_id);

		if (item_base != NULL)
		{
			contentparam.item_list[0].item_id = check_item_cfg->item_reward.item_id;
			contentparam.item_list[0].num = check_item_cfg->item_reward.num;
			contentparam.item_list[0].is_bind = (check_item_cfg->item_reward.is_bind) ? 1 : 0;
			contentparam.item_list[0].invalid_time = item_base->CalInvalidTime();
		}
		int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_randactivity_reward_subject);
		int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_randactivity_reward_content, RAND_ACTIVITY_TYPE_SPRING_FESTIVAL_STORE);
		if (lensub > 0 && length > 0)
		{
			MailRoute::MailToUser(m_role_module_mgr->GetUid(), MAIL_REASON_DEFAULT, contentparam);
		}
	}
	else
	{
		if (!m_role_module_mgr->GetKnapsack()->Put(buy_item_cfg->item_reward, PUT_REASON_RA_SPRING_FESTIVAL_STORE))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
			return;
		}
	}

	m_param.activity_buy_times[seq_id_check] += 1;
	m_param.today_buy_times[seq_id_check] += 1;
	
	this->SendInfo();
	this->SetRoleDataChange();
	this->OnParticipateRA(__FUNCTION__);

	gamelog::g_log_rand_activity.printf(LL_INFO, "RoleActivitySpringFestivalStore::BuyItem user[%d, %s], level[%d], type[%d], reward_item[%d] num[%d]",
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(),
		RAND_ACTIVITY_TYPE_SPRING_FESTIVAL_STORE, buy_item_cfg->item_reward.item_id, buy_item_cfg->item_reward.num);
}
