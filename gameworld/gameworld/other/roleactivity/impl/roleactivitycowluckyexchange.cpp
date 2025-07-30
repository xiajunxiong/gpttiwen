#include "roleactivitycowluckyexchange.hpp"

#include "engineadapter.h"

#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivitycowluckyexchangeconfig.hpp"

#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"

#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"

#include "servercommon/errornum.h"
#include "servercommon/noticenum.h"
#include "gamelog.h"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "other/roleactivity/roleactivitymanager.hpp"
#include "servercommon/string/gameworldstr.h"
#include "other/route/mailroute.hpp"

RoleActivityCowLuckyExchange::RoleActivityCowLuckyExchange(int type)
	:RoleActivity(type)
{

}

RoleActivityCowLuckyExchange::~RoleActivityCowLuckyExchange()
{

}

void RoleActivityCowLuckyExchange::Init(RoleModuleManager *mgr, const RoleActivityRoleData &p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(RACowLuckyExchangeParam));
}

void RoleActivityCowLuckyExchange::GetInitParam(RoleActivityRoleData *p)
{
	memcpy(p->base_data, &m_param, sizeof(RACowLuckyExchangeParam));
}

void RoleActivityCowLuckyExchange::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	//检查刷新
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_COW_LUCKY_EXCHANGE))
	{
		return;
	}

	int rand_open_day = LOGIC_CONFIG->GetRandActivityConfigManager()->GetActOpenDay(ramgr, RAND_ACTIVITY_TYPE_COW_LUCKY_EXCHANGE);

	if (m_param.open_day == rand_open_day)
	{
		//return;//屏蔽让GM命令能直接刷新题目
	}

	m_param.open_day = rand_open_day;
	this->SendInfo();
	this->SetRoleDataChange();
}

void RoleActivityCowLuckyExchange::OnLoginSendInfo()
{
	//检查刷新
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_COW_LUCKY_EXCHANGE))
	{
		return;
	}

	this->SendInfo();
}

void RoleActivityCowLuckyExchange::CheckInitRandActivity(int rand_act_type, bool is_force /*= false*/)
{
	if (RAND_ACTIVITY_TYPE_COW_LUCKY_EXCHANGE != rand_act_type)
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
		int rand_open_day = LOGIC_CONFIG->GetRandActivityConfigManager()->GetActOpenDay(ramgr, RAND_ACTIVITY_TYPE_COW_LUCKY_EXCHANGE);
		const RandActivityCowLuckyExchangeConfig * config = static_cast<const RandActivityCowLuckyExchangeConfig *>
			(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_COW_LUCKY_EXCHANGE));
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

void RoleActivityCowLuckyExchange::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor)
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

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_COW_LUCKY_EXCHANGE))
	{
		return;
	}
	this->CheckInitRandActivity(RAND_ACTIVITY_TYPE_COW_LUCKY_EXCHANGE);

	switch (raor->opera_type)
	{
	case 0://请求下发
		{
			this->SendInfo();
		}
		break;
	case 1:
		{
			this->BuyItem();
		}
		break;
	}
}

bool RoleActivityCowLuckyExchange::OnNoreItemUse(int add_intimacy)
{
	int & initmacy = m_param.initmacy;
	initmacy += add_intimacy;
	Protocol::SCRAMoneyAddInfo add_info;
	add_info.rand_activity = RAND_ACTIVITY_TYPE_COW_LUCKY_EXCHANGE;
	add_info.add_money = add_intimacy;
	m_role_module_mgr->NetSend(&add_info, sizeof(add_info));
	this->SendInfo();
	this->SetRoleDataChange();
	return true;
}

void RoleActivityCowLuckyExchange::SendInfo()
{
	Protocol::SCRACowLuckyExchangeInfo info;
	
	info.initmacy = m_param.initmacy;
	info.history_chongzhi_num = m_role_module_mgr->GetRoleActivityManager()->GetCommonParam().history_chongzhi_num;
	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void RoleActivityCowLuckyExchange::BuyItem()
{
	const RandActivityCowLuckyExchangeConfig * config = static_cast<const RandActivityCowLuckyExchangeConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_COW_LUCKY_EXCHANGE));
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
	long long history_chongzhi_num = m_role_module_mgr->GetRoleActivityManager()->GetCommonParam().history_chongzhi_num;
	const RACowLuckyExchangeCfg *buy_item_cfg = config->RandCowLuckyExchangeCfgByChongzhi(history_chongzhi_num);
	if (NULL == buy_item_cfg)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	//检查消耗
	int &m_initmacy = m_param.initmacy;
	if (m_initmacy < buy_item_cfg->price)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RA_COW_LUCKY_EXCHANGE_IND_LIMIT);
		return;
	}

	if (static_cast<short>(buy_item_cfg->reward_item_vec.size()) <= 0)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}
	m_initmacy -= buy_item_cfg->price;
	if (!m_role_module_mgr->GetKnapsack()->CheckForPutList(static_cast<short>(buy_item_cfg->reward_item_vec.size()), &buy_item_cfg->reward_item_vec[0]))
	{
		//放入背包失败，改发邮件
		static MailContentParam contentparam; contentparam.Reset();
		for (int i = 0; i < MAX_ATTACHMENT_ITEM_NUM && i < (short)buy_item_cfg->reward_item_vec.size(); i++)
		{
			const ItemBase * item_base = ITEMPOOL->GetItem(buy_item_cfg->reward_item_vec[i].item_id);

			if (item_base != NULL)
			{
				contentparam.item_list[i].item_id = buy_item_cfg->reward_item_vec[i].item_id;
				contentparam.item_list[i].num = buy_item_cfg->reward_item_vec[i].num;
				contentparam.item_list[i].is_bind = (buy_item_cfg->reward_item_vec[i].is_bind) ? 1 : 0;
				contentparam.item_list[i].invalid_time = item_base->CalInvalidTime();
			}
		}
		int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_randactivity_reward_subject);
		int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_randactivity_reward_content, RAND_ACTIVITY_TYPE_COW_LUCKY_EXCHANGE);
		if (lensub > 0 && length > 0)
		{
			MailRoute::MailToUser(m_role_module_mgr->GetUid(), MAIL_REASON_DEFAULT, contentparam);
		}
	}
	else
	{
		if (!m_role_module_mgr->GetKnapsack()->PutList(static_cast<short>(buy_item_cfg->reward_item_vec.size()), &buy_item_cfg->reward_item_vec[0], PUT_REASON_RA_COW_LUCKY_EXCHANGE))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
			return;
		}
	}
	this->SendInfo();
	this->SetRoleDataChange();
	this->OnParticipateRA(__FUNCTION__);

	gamelog::g_log_rand_activity.printf(LL_INFO, "RoleActivityCowLuckyExchange::BuyItem user[%d, %s], level[%d], type[%d]",
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(),
		RAND_ACTIVITY_TYPE_COW_LUCKY_EXCHANGE);
}
