#include "roleactivitytigerfisheveryyear.hpp"
#include "config/randactivityconfig/impl/randactivitytigerfisheveryyearconfig.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "global/randactivity/randactivityimpl/randactivitytigerfisheveryyear.hpp"

#include "servercommon/errornum.h"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "gamelog.h"

#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"

#include "servercommon/errornum.h"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "servercommon/uniqueidgenerator.hpp"

RoleActivityTigerFishEveryYear::RoleActivityTigerFishEveryYear(int type)
	:RoleActivity(type)
{

}

RoleActivityTigerFishEveryYear::~RoleActivityTigerFishEveryYear()
{

}

void RoleActivityTigerFishEveryYear::Init(RoleModuleManager * mgr, const RoleActivityRoleData & p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(m_param));
}

void RoleActivityTigerFishEveryYear::GetInitParam(RoleActivityRoleData * p)
{
	UNSTD_STATIC_CHECK(sizeof(m_param) <= sizeof(p->base_data));
	memcpy(p->base_data, &m_param, sizeof(m_param));
}

void RoleActivityTigerFishEveryYear::OnLoginSendInfo()
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_TIGER_FISH_EVERY_YEAR))
	{
		return;
	}

	this->RASendInfo();
}

void RoleActivityTigerFishEveryYear::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_TIGER_FISH_EVERY_YEAR))
	{
		return;
	}

	m_param.DayReset();

	this->RASendInfo();
	this->SetRoleDataChange();
}

void RoleActivityTigerFishEveryYear::CheckInitRandActivity(int rand_act_type, bool is_force)
{
	if (RAND_ACTIVITY_TYPE_TIGER_FISH_EVERY_YEAR != rand_act_type)
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
		m_param.open_activity_level = m_role_module_mgr->GetRole()->GetLevel();

		this->RASendInfo();
		this->SetRoleDataChange();
	}

}

void RoleActivityTigerFishEveryYear::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq * raor)
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

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_TIGER_FISH_EVERY_YEAR))
	{
		return;
	}
	this->CheckInitRandActivity(RAND_ACTIVITY_TYPE_TIGER_FISH_EVERY_YEAR);

	switch (raor->opera_type)
	{
	case 0:
		{
			this->RASendInfo();					// 发协议
		}
		break;
	case 1:
		{
			this->FishermanTask();				// 渔夫任务
		}
		break;
	case 2:
		{
			this->ReceiveTaskReward(raor->param_1);	// 获取任务奖励	p1--索引seq
		}
		break;
	case 3:
		{
			this->ReceiveFishReward(raor->param_1);		// 获取鱼数奖励 p1--索引seq
		}
		break;
	default:
		break;
	}

}

void RoleActivityTigerFishEveryYear::AddTaskParam(int task_type, int task_param)
{
	if (task_type < 0 || task_type >= RATIGERFEY::TIRGER_FEY_TYPE_MAX) return;

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_TIGER_FISH_EVERY_YEAR)) return;

	if(task_param != 0)
	{
		if (m_param.task_status.IsBitSet(task_type))
		{
			return;
		}
		m_param.task_extent[task_type] += task_param;
	}

	const RandActivityTigerFishEveryYearConfig * config = static_cast<const RandActivityTigerFishEveryYearConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_TIGER_FISH_EVERY_YEAR));
	if (NULL == config) return;

	const std::vector<RATigerFEYBasisCfg> * basis_cfg = config->GetBasisCfg(ramgr, m_param.role_level);
	if (NULL == basis_cfg) return;

	for (int i = 0; i < RATIGERFEY::TIRGER_FEY_TYPE_MAX; ++i)
	{
		std::vector<RATigerFEYBasisCfg>::const_iterator it = basis_cfg->begin();
		for (; it != basis_cfg->end(); ++it)
		{
			if (it->task_type == i)
			{
				if (m_param.task_extent[i] >= it->task_param)
				{
					m_param.task_extent[i] = it->task_param;

					m_param.task_status.SetBit(i);

					gamelog::g_log_rand_activity.printf(LL_INFO, "%s role[%d, %s] activity_type[%d] finish_task_type[%d- param:%d]", __FUNCTION__,
						m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), RAND_ACTIVITY_TYPE_TIGER_FISH_EVERY_YEAR, task_type, task_param);
				}
				break;
			}
		}

		if (it == basis_cfg->end())		// 当日无此任务，置0
		{
			m_param.task_extent[i] = 0;
		}
	}

	this->RASendInfo();
	this->SetRoleDataChange();
}

void RoleActivityTigerFishEveryYear::UpdateLevel()
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_TIGER_FISH_EVERY_YEAR)) return;

	m_param.open_activity_level = m_role_module_mgr->GetRole()->GetLevel();
}

void RoleActivityTigerFishEveryYear::RASendInfo()
{
	Protocol::SCRATigerFishEveryYearInfo msg;

	msg.is_accept_task = m_param.is_accept_task;
	msg.role_level = m_param.role_level;
	msg.fish_num = m_param.fish_num;
	msg.is_receive_reward = m_param.is_receive_reward;
	msg.reward_stage_status = m_param.reward_stage_status;
	msg.task_reward_status = m_param.task_reward_status;
	msg.task_status = m_param.task_status;

	memset(msg.task_extent, 0, sizeof(msg.task_extent));
	memcpy(msg.task_extent, m_param.task_extent,sizeof(msg.task_extent));

	m_role_module_mgr->NetSend(&msg, sizeof(msg));
}

void RoleActivityTigerFishEveryYear::FishermanTask()
{
	m_param.role_level = m_role_module_mgr->GetRole()->GetLevel();
	m_param.is_accept_task = 1;

	this->AddTaskParam(0, 0);		// 仅做触发，不实际加进度

	this->RASendInfo();
	this->SetRoleDataChange();
}

void RoleActivityTigerFishEveryYear::ReceiveTaskReward(int seq)
{
	if (seq < 0 || seq >= RATIGERFEY::TIRGER_FEY_TYPE_MAX) return;

	if (m_param.is_accept_task == 0)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_NO_RECEIVE_FISHERMAN_TASK);
		return;
	}



	const RandActivityTigerFishEveryYearConfig * config = static_cast<const RandActivityTigerFishEveryYearConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_TIGER_FISH_EVERY_YEAR));
	if (NULL == config) return;

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	const std::vector<RATigerFEYBasisCfg> * basis_cfg = config->GetBasisCfg(ramgr, m_param.role_level);
	if (NULL == basis_cfg) return;

	bool has_reward = false;
	int fish_num = 0;
	std::vector<ItemConfigData> reward_list;
	std::vector<RATigerFEYBasisCfg>::const_iterator it = basis_cfg->begin();
	for (;it != basis_cfg->end();++it)
	{
		if (it->seq == seq)
		{
			if (!m_param.task_status.IsBitSet(it->task_type))
			{
				return;
			}

			if (m_param.task_reward_status.IsBitSet(it->task_type))
			{
				m_role_module_mgr->NoticeNum(errornum::EN_REWARD_ALREADY_FETCHED);
				return;
			}

			if (!it->reward_item.empty())
			{
				reward_list = it->reward_item;
				has_reward = true;
			}

			fish_num = it->reward_fish_num;

			break;
		}
	}
	if (it == basis_cfg->end()) return;

	if (has_reward)		// 如果有物品奖励
	{
		if (m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporaryVec(reward_list))
		{
			m_role_module_mgr->GetKnapsack()->PutVec(reward_list, PUT_REASON_RA_TIGER_FISH_EVERY_YEAR);
		}
		else
		{
			m_role_module_mgr->GetKnapsack()->SendMail(&reward_list[0], (int)reward_list.size(), SEND_MAIL_TYPE_TIGER_FISH_EVERY_YEAR);
		}
	}

	m_param.fish_num += fish_num;


	m_param.task_reward_status.SetBit(it->task_type);

	this->UpdateAvailableReward();

	std::string gamelog_str;
	for (int i = 0; i < (int)reward_list.size(); ++i)
	{
		gamelog_str = STRING_SPRINTF("{item[%d, %d]} ", reward_list[i].item_id, reward_list[i].num);
	}

	gamelog::g_log_rand_activity.printf(LL_INFO, "%s role[%d, %s] get_reward[%s]", __FUNCTION__,
		m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), gamelog_str.c_str());

	this->RASendInfo();
	this->SetRoleDataChange();
}

void RoleActivityTigerFishEveryYear::ReceiveFishReward(int seq)
{
	if (seq < 0 || seq >= RATIGERFEY::MAX_REWARD_STAGE) return;

	if (!m_param.is_receive_reward.IsBitSet(seq))
	{
		return;
	}

	if (m_param.reward_stage_status.IsBitSet(seq))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_REWARD_ALREADY_FETCHED);
		return;
	}

	const RandActivityTigerFishEveryYearConfig * config = static_cast<const RandActivityTigerFishEveryYearConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_TIGER_FISH_EVERY_YEAR));
	if (NULL == config) return;

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	const std::vector<RATigerFEYRewardCfg> * reward_cfg = config->GetRewardCfg(ramgr, m_param.open_activity_level);
	if (NULL == reward_cfg) return;

	std::vector<ItemConfigData> reward_list;
	for (std::vector<RATigerFEYRewardCfg>::const_iterator it = reward_cfg->begin(); it != reward_cfg->end(); ++it)
	{
		if (it->seq == seq)
		{
			reward_list = it->reward_item;
		}
	}
	if (reward_list.empty()) return;

	if (m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporaryVec(reward_list))
	{
		m_role_module_mgr->GetKnapsack()->PutVec(reward_list, PUT_REASON_RA_TIGER_FISH_EVERY_YEAR);
	}
	else
	{
		m_role_module_mgr->GetKnapsack()->SendMail(&reward_list[0], (int)reward_list.size(), SEND_MAIL_TYPE_TIGER_FISH_EVERY_YEAR);
	}

	m_param.reward_stage_status.SetBit(seq);

	std::string gamelog_str;
	for (int i = 0; i < (int)reward_list.size(); ++i)
	{
		gamelog_str = STRING_SPRINTF("{item[%d, %d]} ", reward_list[i].item_id, reward_list[i].num);
	}

	gamelog::g_log_rand_activity.printf(LL_INFO, "%s role[%d, %s] get_reward[%s]", __FUNCTION__,
		m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), gamelog_str.c_str());

	this->RASendInfo();
	this->SetRoleDataChange();
}

void RoleActivityTigerFishEveryYear::UpdateAvailableReward()
{
	const RandActivityTigerFishEveryYearConfig * config = static_cast<const RandActivityTigerFishEveryYearConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_TIGER_FISH_EVERY_YEAR));
	if (NULL == config) return;

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	const std::vector<RATigerFEYRewardCfg> * reward_cfg = config->GetRewardCfg(ramgr, m_param.open_activity_level);
	if (NULL == reward_cfg) return;

	for (std::vector<RATigerFEYRewardCfg>::const_iterator it = reward_cfg->begin(); it != reward_cfg->end(); ++it)
	{
		if (m_param.fish_num >= it->fish_num)
		{
			m_param.is_receive_reward.SetBit(it->seq);
		}
	}

}

