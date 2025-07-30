#include "roleactivityyunzebloomingrecord.hpp"
#include "gamelog.h"
#include "obj/character/role.h"

#include "other/rolemodulemanager.hpp"
#include "other/roleactivity/roleactivitymanager.hpp"

#include "servercommon/errornum.h"
#include "servercommon/serverconfig/crossconfig.hpp"

#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivityyunzebloomingrecordconfig.hpp"

#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"
#include "other/roleactivity/impl/roleactivitycolorfuldouble.hpp"
#include "other/roleactivity/impl/roleactivitychaozhifenxiang.hpp"
#include "other/roleactivity/impl/roleactivityyunzecutepet.hpp"

RoleActivityYunZeBloomingRecord::RoleActivityYunZeBloomingRecord(int type)
	:RoleActivity(type)
{

}

RoleActivityYunZeBloomingRecord::~RoleActivityYunZeBloomingRecord()
{

}

void RoleActivityYunZeBloomingRecord::Init(RoleModuleManager * mgr, const RoleActivityRoleData & p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(m_param));
}

void RoleActivityYunZeBloomingRecord::GetInitParam(RoleActivityRoleData * p)
{
	UNSTD_STATIC_CHECK(sizeof(m_param) <= sizeof(p->base_data));
	memcpy(p->base_data, &m_param, sizeof(m_param));
}

void RoleActivityYunZeBloomingRecord::RASendInfo()
{
	Protocol::SCRAYunZeBloomingRecordInfo msg;
	msg.times = m_param.times;
	msg.cur_integral = m_param.cur_integral;
	msg.challenge_start_timestamp = m_param.challenge_end_timestamp;
	memcpy(msg.reward_list, m_param.reward_list, sizeof(msg.reward_list));

	m_role_module_mgr->NetSend(&msg, sizeof(msg));
}

void RoleActivityYunZeBloomingRecord::CheckInitRandActivity(int rand_act_type, bool is_force /*= false*/)
{
	if (RAND_ACTIVITY_TYPE_YUN_ZE_BLOOMING_RECORD != rand_act_type)
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

	bool in_activity_time = ramgr->IsInRandActivityOpenTime(rand_act_type, m_param.ra_begin_timestamp);
	if (is_force || !in_activity_time)
	{
		m_param.Reset();
		m_param.ra_begin_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());

		this->SetRoleDataChange();
	}
}

void RoleActivityYunZeBloomingRecord::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor)
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

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_YUN_ZE_BLOOMING_RECORD))
	{
		return;
	}
	this->CheckInitRandActivity(RAND_ACTIVITY_TYPE_YUN_ZE_BLOOMING_RECORD);

	switch (raor->opera_type)
	{
	case 0:
		{
			this->RASendInfo();		
		}
		break;
	case 1:
		{
			this->OnStart();		// 游戏开始，记录时间戳
		}
		break;
	case 2:
		{
			this->RecordIntegral(raor->param_1);		// 游戏结束，param1-玩家本局分数
		}
		break;
	case 3:
		{
			this->SendReward(raor->param_1);	// 申请奖励，param1-玩家申请的奖励id
		}
		break;
	default:
		break;
	}

	return;
}

void RoleActivityYunZeBloomingRecord::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	if (old_dayid == now_dayid) return;

	//检查刷新
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_YUN_ZE_BLOOMING_RECORD))
	{
		return;
	}

	m_param.DayReset();
	this->SetRoleDataChange();
	this->RASendInfo();
}

void RoleActivityYunZeBloomingRecord::OnLoginSendInfo()
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_YUN_ZE_BLOOMING_RECORD))
	{
		return;
	}
	this->RASendInfo();
}

void RoleActivityYunZeBloomingRecord::Update(time_t now_second)
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_YUN_ZE_BLOOMING_RECORD))
		return;

	const RandActivityYunZeBloomingRecordConfig * config = static_cast<const RandActivityYunZeBloomingRecordConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_YUN_ZE_BLOOMING_RECORD));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	if (m_param.challenge_end_timestamp > 0 && now_second == m_param.challenge_end_timestamp)
	{
		m_param.challenge_end_timestamp = 0;

		this->RASendInfo();
		this->SetRoleDataChange();
	}	
}


void RoleActivityYunZeBloomingRecord::OnStart()
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_YUN_ZE_BLOOMING_RECORD))
	{
		return;
	}

	const RandActivityYunZeBloomingRecordConfig * config = static_cast<const RandActivityYunZeBloomingRecordConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_YUN_ZE_BLOOMING_RECORD));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	if (m_param.times >= config->GetOtherCfg().frequency)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_NOT_ENOUGH);
		return;
	}


	if (m_param.challenge_end_timestamp != 0)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_GAME_IN_PROGRESS);
		return;
	}

	m_param.challenge_end_timestamp = ((unsigned int)EngineAdapter::Instance().Time() + config->GetOtherCfg().time);
	m_param.times++;
	m_param.is_available_integral = 1;

	this->RASendInfo();
	this->SetRoleDataChange();
	this->OnParticipateRA(__FUNCTION__);

	gamelog::g_log_rand_activity.printf(LL_INFO, "RoleActivityYunZeBloomingRecord::%s role[%d, %s] activity_type[%d] times[%d] end_timestamp[%d] ",__FUNCTION__,
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), RAND_ACTIVITY_TYPE_YUN_ZE_BLOOMING_RECORD, m_param.times, m_param.challenge_end_timestamp);
}

void RoleActivityYunZeBloomingRecord::RecordIntegral(int integral)
{
	if (integral < 0)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	if (m_param.is_available_integral == 0)
	{
		return;
	}

	m_param.cur_integral += integral;
	m_param.is_available_integral = 0;
	m_param.challenge_end_timestamp = 0;

	this->RASendInfo();
	this->SetRoleDataChange();

	{
		RoleActivityColorfulDouble * role_activity = static_cast<RoleActivityColorfulDouble *>
			(m_role_module_mgr->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_COLORFUL_DOUBLE));
		if (NULL != role_activity)
		{
			role_activity->AddTaskParam(RAColorfulDouble::TSAK_TYPE_YUN_ZE_BLOOMING_RECORD, integral);
		}
	}
	{
		RoleActivityChaoZhiFenXiang * role_activity = static_cast<RoleActivityChaoZhiFenXiang *>
			(m_role_module_mgr->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_CHAOZHI_FENXIANG));
		if (NULL != role_activity)
		{
			role_activity->AddTaskComplete(RA_CHAOZHI_FENXIANG_TASK_TYPE_YUNZEFANHUALU, integral);
		}
	}
	{
		RoleActivityYunZeCutePet * role_activity = static_cast<RoleActivityYunZeCutePet *>
			(m_role_module_mgr->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_YUN_ZE_CUTE_PET));
		if (NULL != role_activity)
		{
			role_activity->AddTaskParam(RAYZCutePet::TASK_TYPE_YZ_BLOOMING_RECORD, integral);
		}
	}

	gamelog::g_log_rand_activity.printf(LL_INFO, "RoleActivityYunZeBloomingRecord::%s role[%d, %s] activity_type[%d] cur_integral[%d] ", __FUNCTION__,
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), RAND_ACTIVITY_TYPE_YUN_ZE_BLOOMING_RECORD, m_param.cur_integral);
}

void RoleActivityYunZeBloomingRecord::SendReward(int reward_id)
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	const RandActivityYunZeBloomingRecordConfig * config = static_cast<const RandActivityYunZeBloomingRecordConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_YUN_ZE_BLOOMING_RECORD));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	if (!config->IsCanReceiveReward(m_param.cur_integral, reward_id + 1))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_CONDITIONS_NOT_REACHED);
		return;
	}

	if (1 == m_param.reward_list[reward_id])
	{
		m_role_module_mgr->NoticeNum(errornum::EN_REWARD_ALREADY_FETCHED);
		return;
	}

	const RAYZBloomingRecordRewardCfg * reward_cfg = config->GetRewardByIntegralCfg(ramgr, reward_id + 1);
	if (NULL == reward_cfg)	return;


	if (m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporary((short)reward_cfg->reward_list.size(), &reward_cfg->reward_list[0]))
	{
		if (!m_role_module_mgr->GetKnapsack()->PutList((short)reward_cfg->reward_list.size(), &reward_cfg->reward_list[0], PUT_REASON_YUNZE_BLOOMING_RECORD))
		{
			return;
		}
	}
	else
	{
		m_role_module_mgr->GetKnapsack()->SendMail(&reward_cfg->reward_list[0], (int)reward_cfg->reward_list.size(), SEND_MAIL_TYPE_YUN_ZE_BLOOMING_RECORD, true);
	}

	m_param.reward_list[reward_id] = 1;		// 置奖励位为已领取

	this->RASendInfo();
	this->SetRoleDataChange();

	std::string gamelog_str;
	for (int i = 0; i < (int)reward_cfg->reward_list.size(); ++i)
	{
		gamelog_str += STRING_SPRINTF("{item[%d, %d]} ", reward_cfg->reward_list[i].item_id, reward_cfg->reward_list[i].num);
	}

	gamelog::g_log_rand_activity.printf(LL_INFO, "RoleActivityYunZeBloomingRecord::%s role[%d, %s] activity_type[%d] reward[%s] ", __FUNCTION__,
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), RAND_ACTIVITY_TYPE_YUN_ZE_BLOOMING_RECORD, gamelog_str.c_str());
}