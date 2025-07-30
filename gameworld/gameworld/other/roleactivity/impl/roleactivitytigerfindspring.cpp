#include "roleactivitytigerfindspring.hpp"
#include "config/randactivityconfig/impl/randactivitytigerfindspringconfig.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "global/randactivity/randactivityimpl/randactivitytigerfindspring.hpp"

#include "servercommon/errornum.h"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "gamelog.h"

#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"

#include "servercommon/errornum.h"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "servercommon/uniqueidgenerator.hpp"

RoleActivityTigerFindSpring::RoleActivityTigerFindSpring(int type)
	:RoleActivity(type)
{

}

RoleActivityTigerFindSpring::~RoleActivityTigerFindSpring()
{

}

void RoleActivityTigerFindSpring::Init(RoleModuleManager * mgr, const RoleActivityRoleData & p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(m_param));
}

void RoleActivityTigerFindSpring::GetInitParam(RoleActivityRoleData * p)
{
	UNSTD_STATIC_CHECK(sizeof(m_param) <= sizeof(p->base_data));
	memcpy(p->base_data, &m_param, sizeof(m_param));
}

void RoleActivityTigerFindSpring::OnLoginSendInfo()
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_TIGER_FIND_SPRING))
	{
		return;
	}

	this->RASendInfo();
}

void RoleActivityTigerFindSpring::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (m_param.is_receive_reward == 1)
	{
		const RandActivityTigerFindSpringConfig * config = static_cast<const RandActivityTigerFindSpringConfig *>
			(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_TIGER_FIND_SPRING));
		if (NULL == config)  return;

		const RATFSRewardCfg * reward_cfg = config->GetRewardCfg(ramgr, true);
		if (NULL == reward_cfg) return;

		if (!m_role_module_mgr->GetKnapsack()->SendMail(&reward_cfg->reward_list[0], (int)reward_cfg->reward_list.size(), SEND_MAIL_TYPE_TIGER_FIND_SPRING_YESTERDAY))
		{
			return;
		}

		m_param.is_receive_reward = 2;

		this->RASendInfo();
		this->SetRoleDataChange();
	}

	if (ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_TIGER_FIND_SPRING))
	{
		m_param.DayReset();

		this->RASendInfo();
		this->SetRoleDataChange();
	}
}

void RoleActivityTigerFindSpring::CheckInitRandActivity(int rand_act_type, bool is_force)
{
	if (RAND_ACTIVITY_TYPE_TIGER_FIND_SPRING != rand_act_type)
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

		this->RASendInfo();
		this->SetRoleDataChange();
	}
}

void RoleActivityTigerFindSpring::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq * raor)
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

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_TIGER_FIND_SPRING))
	{
		return;
	}
	this->CheckInitRandActivity(RAND_ACTIVITY_TYPE_TIGER_FIND_SPRING);

	switch (raor->opera_type)
	{
	case 0:
		{
			this->RASendInfo();					// 发协议
		}
		break;
	case 1:
		{
			this->ApplyFinishTask();			// 客户端申请完成任务
		}
		break;
	case 2:
		{
			this->ReceiveReward();				// 领取奖励
		}
		break;
	default:
		break;
	}
}

void RoleActivityTigerFindSpring::RASendInfo()
{
	const RandActivityTigerFindSpringConfig * config = static_cast<const RandActivityTigerFindSpringConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_TIGER_FIND_SPRING));
	if (NULL == config)  return;

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	const RATFSRewardCfg * reward_cfg = config->GetRewardCfg(ramgr);
	if (NULL == reward_cfg) return;

	Protocol::SCRATigerFindSpringInfo msg;
	msg.is_receive_reward = m_param.is_receive_reward;
	msg.seq = reward_cfg->seq;

	m_role_module_mgr->NetSend(&msg, sizeof(msg));
}

void RoleActivityTigerFindSpring::ApplyFinishTask()
{
	if (m_param.is_receive_reward != 0) return;

	const RandActivityTigerFindSpringConfig * config = static_cast<const RandActivityTigerFindSpringConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_TIGER_FIND_SPRING));
	if (NULL == config)  return;

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	const RATFSRewardCfg * reward_cfg = config->GetRewardCfg(ramgr);
	if (NULL == reward_cfg) return;

	const RATFSOtherCfg & other_cfg = config->GetOtherCfg();

	Posi cur_position = m_role_module_mgr->GetRole()->GetPos();

	// 判断玩家是否在坐标范围内
	if (((cur_position.x - reward_cfg->reward_pos.x)*(cur_position.x - reward_cfg->reward_pos.x) +
		(cur_position.y - reward_cfg->reward_pos.y)*(cur_position.y - reward_cfg->reward_pos.y)) > (other_cfg.explore_radius * other_cfg.explore_radius))
	{
		return;
	}

	m_param.is_receive_reward = 1;

	m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_FIND_SPRING_TASK_FINISH);

	gamelog::g_log_rand_activity.printf(LL_INFO, "%s role[%d, %s] Task_Finish!!!", __FUNCTION__,
		m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName());

	this->RASendInfo();
	this->SetRoleDataChange();
}

void RoleActivityTigerFindSpring::ReceiveReward()
{
	if (m_param.is_receive_reward != 1)
	{
		return;
	}

	const RandActivityTigerFindSpringConfig * config = static_cast<const RandActivityTigerFindSpringConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_TIGER_FIND_SPRING));
	if (NULL == config)  return;

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	const RATFSRewardCfg * reward_cfg = config->GetRewardCfg(ramgr);
	if (NULL == reward_cfg) return;

	if (m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporaryVec(reward_cfg->reward_list))
	{
		m_role_module_mgr->GetKnapsack()->PutVec(reward_cfg->reward_list, PUT_REASON_RA_TIGER_FIND_SPRING);
	}
	else
	{
		m_role_module_mgr->GetKnapsack()->SendMail(&reward_cfg->reward_list[0], (int)reward_cfg->reward_list.size(), SEND_MAIL_TYPE_TIGER_FIND_SPRING);
	}

	m_param.is_receive_reward = 2;

	std::string gamelog_str;
	for (int i = 0; i < (int)reward_cfg->reward_list.size(); ++i)
	{
		gamelog_str = STRING_SPRINTF("{item[%d, %d]} ", reward_cfg->reward_list[i].item_id, reward_cfg->reward_list[i].num);
	}

	gamelog::g_log_rand_activity.printf(LL_INFO, "%s role[%d, %s] get_reward[%s]", __FUNCTION__,
		m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), gamelog_str.c_str());

	this->RASendInfo();
	this->SetRoleDataChange();
}
