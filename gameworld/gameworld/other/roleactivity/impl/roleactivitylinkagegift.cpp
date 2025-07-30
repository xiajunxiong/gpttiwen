#include "roleactivitylinkagegift.hpp"

#include "engineadapter.h"

#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivitylinkagegiftconfig.hpp"

#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"

#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "item/knapsack.h"
#include "protocol/randactivity/msgralinkagegift.hpp"

#include "servercommon/errornum.h"
#include "servercommon/noticenum.h"
#include "gamelog.h"
#include "servercommon/serverconfig/crossconfig.hpp"


RoleActivityLinkageGift::RoleActivityLinkageGift(int type)
	:RoleActivity(type)
{

}

RoleActivityLinkageGift::~RoleActivityLinkageGift()
{

}

void RoleActivityLinkageGift::Init(RoleModuleManager *mgr, const RoleActivityRoleData &p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(RALinkageGiftParam));
}

void RoleActivityLinkageGift::GetInitParam(RoleActivityRoleData *p)
{
	memcpy(p->base_data, &m_param, sizeof(RALinkageGiftParam));
}

void RoleActivityLinkageGift::OnLoginSendInfo()
{
	this->SendRALinkageGiftInfo();
}

void RoleActivityLinkageGift::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (old_dayid == now_dayid || !ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_LINKAGE_GIFT))
	{
		return;
	}
	this->CheckInitRandActivity(RAND_ACTIVITY_TYPE_LINKAGE_GIFT);

	const RandActivityLinkageGiftConfig * config = static_cast<const RandActivityLinkageGiftConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_LINKAGE_GIFT));
	if (NULL == config)
	{
		return;
	}

	const LinkageGiftOtherCfg & other_cfg = config->GetOhterCfg();
	if (m_role_module_mgr->GetCommonData().role_create_count_day >= other_cfg.receive_time)
	{
		if (m_param.ra_linkage_gfit_fetch_flag == 0)
		{
			m_param.ra_linkage_gfit_fetch_flag = 1;
		}
	}

	this->SendRALinkageGiftInfo();

	this->SetRoleDataChange();
}

void RoleActivityLinkageGift::CheckInitRandActivity(int rand_act_type, bool is_force /*= false*/)
{
	if (RAND_ACTIVITY_TYPE_LINKAGE_GIFT != rand_act_type)
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

	bool in_activity_time = ramgr->IsInRandActivityOpenTime(rand_act_type, m_param.ra_linkage_gift_begin_timestamp);
	if (is_force || !in_activity_time)
	{
		m_param.ra_linkage_gift_begin_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());
		m_param.ra_linkage_gfit_fetch_flag = LINKAGE_GIFT_FETCH_TYPE_CAN_NOT_FETCH;
		m_param.ra_linkage_gift_open_activity_level_seq = 0;
		m_param.ra_linkage_gfit_can_receive_timestamp = 0;
		this->OnCheckLinkageGiftCondition();

		this->SetRoleDataChange();
	}
}

void RoleActivityLinkageGift::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor)
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

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_LINKAGE_GIFT))
	{
		return;
	}
	this->CheckInitRandActivity(RAND_ACTIVITY_TYPE_LINKAGE_GIFT);

	switch (raor->opera_type)
	{
	case Protocol::RA_LINKAGE_GIFT_OPERA_TYPE_INFO:
		{
			this->SendRALinkageGiftInfo();
		}
		break;
	case Protocol::RA_LINKAGE_GIFT_OPERA_TYPE_FETCH:
		{
			this->OnRALinkageGiftFetchReward();
		}
		break;
	}

}

void RoleActivityLinkageGift::OnCheckLinkageGiftCondition()
{
	const RandActivityLinkageGiftConfig * config = static_cast<const RandActivityLinkageGiftConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_LINKAGE_GIFT));
	if (NULL == config)
	{
		return;
	}

	const LinkageGiftOtherCfg & other_cfg = config->GetOhterCfg();
	int role_create_count_day = m_role_module_mgr->GetCommonData().role_create_count_day;
	unsigned int now_timestamp = (unsigned int)EngineAdapter::Instance().Time();
	int next_day_zero_hour = EngineAdapter::Instance().NextDayInterval(0, 0, 0);
	unsigned int today_six_hour_time = now_timestamp - (SECOND_PER_DAY - next_day_zero_hour);

	if (role_create_count_day >= other_cfg.receive_time)
	{
		if (m_param.ra_linkage_gfit_fetch_flag == LINKAGE_GIFT_FETCH_TYPE_CAN_NOT_FETCH)
		{
			m_param.ra_linkage_gfit_fetch_flag = LINKAGE_GIFT_FETCH_TYPE_CAN_FETCH;
		}
		m_param.ra_linkage_gfit_can_receive_timestamp = today_six_hour_time - (role_create_count_day - other_cfg.receive_time) * SECOND_PER_DAY;
	}
	else
	{
		m_param.ra_linkage_gfit_can_receive_timestamp = today_six_hour_time + (other_cfg.receive_time - role_create_count_day) * SECOND_PER_DAY;
	}
	int seq = config->GetLinkageGiftRewardCfgSeq(m_role_module_mgr->GetRole()->GetLevel());
	if (seq == -1)
	{
#ifdef _DEBUG
		assert(0);
#endif
		return;
	}
	m_param.ra_linkage_gift_open_activity_level_seq = seq;
	this->SendRALinkageGiftInfo();

	this->SetRoleDataChange();
}

void RoleActivityLinkageGift::SendRALinkageGiftInfo()
{
	Protocol::SCRALinkageGiftInfo info;
	info.fetch_flag = m_param.ra_linkage_gfit_fetch_flag;
	info.seq = m_param.ra_linkage_gift_open_activity_level_seq;
	info.can_receive_timestamp = m_param.ra_linkage_gfit_can_receive_timestamp;

	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void RoleActivityLinkageGift::OnRALinkageGiftFetchReward()
{
	if (m_param.ra_linkage_gfit_fetch_flag == LINKAGE_GIFT_FETCH_TYPE_CAN_NOT_FETCH)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_CONDITIONS_NOT_REACHED);
		return;
	}
	else if (m_param.ra_linkage_gfit_fetch_flag == LINKAGE_GIFT_FETCH_TYPE_ALREADY_FETCH)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_REWARD_ALREADY_FETCHED);
		return;
	}

	const RandActivityLinkageGiftConfig * config = static_cast<const RandActivityLinkageGiftConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_LINKAGE_GIFT));
	if (NULL == config)
	{
		return;
	}

	const LinkageGiftRewardCfg * cfg = config->GetLinkageGiftRewardCfgBySeq(m_param.ra_linkage_gift_open_activity_level_seq);
	if (NULL == cfg) return;

	if (!m_role_module_mgr->GetKnapsack()->CheckForPut(cfg->item.item_id, cfg->item.num))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return;
	}
	if (!m_role_module_mgr->GetKnapsack()->Put(cfg->item, PUT_REASON_LINKAGE_GIFT))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return;
	}
	gamelog::g_log_rand_activity.printf(LL_INFO, "%s user[%d, %s], level[%d], type[%d] item[id:%d , num:%d]", __FUNCTION__,
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
		m_role_module_mgr->GetRole()->GetLevel(), RAND_ACTIVITY_TYPE_LINKAGE_GIFT, cfg->item.item_id, cfg->item.num);

	m_param.ra_linkage_gfit_fetch_flag = LINKAGE_GIFT_FETCH_TYPE_ALREADY_FETCH;
	this->SendRALinkageGiftInfo();

	this->SetRoleDataChange();

	this->OnParticipateRA(__FUNCTION__);
}
