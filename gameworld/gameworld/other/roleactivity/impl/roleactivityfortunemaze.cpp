#include "roleactivityfortunemaze.hpp"

#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"

#include "item/knapsack.h"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"

#include "protocol/msgrandactivity.hpp"

#include "servercommon/errornum.h"
#include "servercommon/serverconfig/crossconfig.hpp"

#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivityfortunemazeconfig.hpp"
#include "scene/fbmanager.hpp"
#include "gameworld/gamelog.h"

#include "other/roleactivity/impl/roleactivitywanlinggongying.hpp"
#include "other/roleactivity/roleactivitymanager.hpp"

RoleActivityFortuneMaze::RoleActivityFortuneMaze(int type)
	:RoleActivity(type)
{

}

RoleActivityFortuneMaze::~RoleActivityFortuneMaze()
{
}

void RoleActivityFortuneMaze::Init(RoleModuleManager * mgr, const RoleActivityRoleData & p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(RAFortuneMazeParam));
}

void RoleActivityFortuneMaze::GetInitParam(RoleActivityRoleData * p)
{
	memcpy(p->base_data, &m_param, sizeof(RAFortuneMazeParam));
}

void RoleActivityFortuneMaze::Update(time_t now_second)
{
	if (m_param.login_refresh_tamp > 0 && m_param.login_refresh_tamp <= now_second)
	{
		RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
		if (NULL == ramgr) return;

		if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_FORTUNE_MAZE))
		{
			return;
		}
		this->OnCheckRoleLoginDelay(0, 1);
		m_param.login_refresh_tamp = 0;
		this->SendInfo();
	}
}

void RoleActivityFortuneMaze::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	if(old_dayid == now_dayid) return;

	m_param.DayReset();
	//检查刷新
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_FORTUNE_MAZE))
	{
		return;
	}

	m_param.login_refresh_tamp = static_cast<unsigned int>(EngineAdapter::Instance().Time() + 2);
	this->SetRoleDataChange();
	this->SendInfo();
}

void RoleActivityFortuneMaze::OnLoginSendInfo()
{
	//检查刷新
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_FORTUNE_MAZE))
	{
		return;
	}
	this->SendInfo();
}

void RoleActivityFortuneMaze::CheckInitRandActivity(int rand_act_type, bool is_force)
{
	if (RAND_ACTIVITY_TYPE_FORTUNE_MAZE != rand_act_type)
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
		const RandActivityFortuneMazeConfig * config = static_cast<const RandActivityFortuneMazeConfig *>
			(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_FORTUNE_MAZE));
		if (NULL == config) return;

		m_param.Reset();
		m_param.begin_time = static_cast<unsigned int>(EngineAdapter::Instance().Time());
		m_param.open_day = config->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_FORTUNE_MAZE);
		m_param.login_refresh_tamp = static_cast<unsigned int>(EngineAdapter::Instance().Time() + 2);
		gamelog::g_log_rand_activity.printf(LL_INFO, "RoleActivityFortuneMaze::CheckInitRandActivity user[%s,%d] login_refresh_tamp[%d]",m_role_module_mgr->GetRoleName(),m_role_module_mgr->GetUid(),
			m_param.login_refresh_tamp);
		this->SetRoleDataChange();
	}
}

void RoleActivityFortuneMaze::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq * raor)
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

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_FORTUNE_MAZE))
	{
		return;
	}
	this->CheckInitRandActivity(RAND_ACTIVITY_TYPE_FORTUNE_MAZE);

	switch (raor->opera_type)
	{
		case 0:				//下发信息
		{
			this->SendInfo();
		}
		break;
		case 1:			//翻牌
		{
			this->RandPinTuIndex(raor->param_1);
		}
		break;
		case 2:			//移位
			{
				this->MoivePinTu(raor->param_1, raor->param_2, raor->param_3);
			}
			break;
		case 3:			//进入特殊副本
			{
				this->OnRoleEnterSpecialFb();
			}
			break;
		case 4:
			{
				this->SetMapMarkIndex(raor->param_1);
			}
			break;
		case 5:			// 拼图下发
			{
				this->SendMapInfo();
			}
			break;
	default:
		break;
	}

}

void RoleActivityFortuneMaze::OnPetLearnSkill()
{
	const RandActivityFortuneMazeConfig * config = static_cast<const RandActivityFortuneMazeConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_FORTUNE_MAZE));
	if (NULL == config) return;
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;
	const FortuneMazeTaskListCfg * task_list = config->GetFortuneMazeTaskListConfig(ramgr);
	if (task_list == NULL)return;
	bool has_change = false;
	for (int i = 0; i < MAX_FORTUNE_MAZE_TASK_NUM && i < (int)task_list->task_list.size(); i++)
	{
		FortuneMazeTaskCfg task_info = task_list->task_list[i];
		if (task_info.type == FORTUNE_MAZE_TASK_TYPE_1)
		{
			if (m_param.task_info[i] < task_info.parameter)
			{
				m_param.task_info[i] += 1;
				if (task_info.parameter <= m_param.task_info[i])
				{
					m_param.rand_pin_tu_times += task_info.open_number;
					has_change = true;
				}
			}
			gamelog::g_log_rand_activity.printf(LL_INFO, "RoleActivityFortuneMaze::OnPetLearnSkill user[%s,%d] task_index[%d] task_type[%d] task_param[%d] task_cfg_param[%d] rand_pin_tu_times[%d] param[%d]",
				m_role_module_mgr->GetRoleName(), m_role_module_mgr->GetUid(), i, FORTUNE_MAZE_TASK_TYPE_1, m_param.task_info[i], task_info.parameter, m_param.rand_pin_tu_times, 1);
		}
	}
	if (has_change)this->SendInfo();
}

void RoleActivityFortuneMaze::JoinTimeLimitActivity()
{
	const RandActivityFortuneMazeConfig * config = static_cast<const RandActivityFortuneMazeConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_FORTUNE_MAZE));
	if (NULL == config) return;
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;
	const FortuneMazeTaskListCfg * task_list = config->GetFortuneMazeTaskListConfig(ramgr);
	if (task_list == NULL)return;
	bool has_change = false;
	for (int i = 0; i < MAX_FORTUNE_MAZE_TASK_NUM && i < (int)task_list->task_list.size(); i++)
	{
		FortuneMazeTaskCfg task_info = task_list->task_list[i];
		if (task_info.type == FORTUNE_MAZE_TASK_TYPE_2)
		{
			if (m_param.task_info[i] < task_info.parameter)
			{
				m_param.task_info[i] += 1;
				if (task_info.parameter <= m_param.task_info[i])
				{
					m_param.rand_pin_tu_times += task_info.open_number;
					has_change = true;
				}
			}
			gamelog::g_log_rand_activity.printf(LL_INFO, "RoleActivityFortuneMaze::JoinTimeLimitActivity user[%s,%d] task_index[%d] task_type[%d] task_param[%d] task_cfg_param[%d] rand_pin_tu_times[%d] param[%d]",
				m_role_module_mgr->GetRoleName(), m_role_module_mgr->GetUid(), i, FORTUNE_MAZE_TASK_TYPE_2, m_param.task_info[i], task_info.parameter, m_param.rand_pin_tu_times, 1);
		}
	}
	if (has_change)this->SendInfo();
}

void RoleActivityFortuneMaze::CostCoin(int param)
{
	const RandActivityFortuneMazeConfig * config = static_cast<const RandActivityFortuneMazeConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_FORTUNE_MAZE));
	if (NULL == config) return;
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;
	const FortuneMazeTaskListCfg * task_list = config->GetFortuneMazeTaskListConfig(ramgr);
	if (task_list == NULL)return;
	bool has_change = false;
	for (int i = 0; i < MAX_FORTUNE_MAZE_TASK_NUM && i < (int)task_list->task_list.size(); i++)
	{
		FortuneMazeTaskCfg task_info = task_list->task_list[i];
		if (task_info.type == FORTUNE_MAZE_TASK_TYPE_3)
		{
			if (m_param.task_info[i] < task_info.parameter)
			{
				m_param.task_info[i] += param;
				if (task_info.parameter <= m_param.task_info[i])
				{
					m_param.rand_pin_tu_times += task_info.open_number;
					has_change = true;
				}
			}
			else
			{
				m_param.task_info[i] = task_info.parameter;
				has_change = true;
			}
			gamelog::g_log_rand_activity.printf(LL_INFO, "RoleActivityFortuneMaze::CostCoin user[%s,%d] task_index[%d] task_type[%d] task_param[%d] task_cfg_param[%d] rand_pin_tu_times[%d] param[%d]",
				m_role_module_mgr->GetRoleName(), m_role_module_mgr->GetUid(), i, FORTUNE_MAZE_TASK_TYPE_3, m_param.task_info[i], task_info.parameter, m_param.rand_pin_tu_times, param);
		}
	}
	if (has_change)this->SendInfo();
}

void RoleActivityFortuneMaze::CatchPet()
{
	const RandActivityFortuneMazeConfig * config = static_cast<const RandActivityFortuneMazeConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_FORTUNE_MAZE));
	if (NULL == config) return;
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;
	const FortuneMazeTaskListCfg * task_list = config->GetFortuneMazeTaskListConfig(ramgr);
	if (task_list == NULL)return;
	bool has_change = false;
	for (int i = 0; i < MAX_FORTUNE_MAZE_TASK_NUM && i < (int)task_list->task_list.size(); i++)
	{
		FortuneMazeTaskCfg task_info = task_list->task_list[i];
		if (task_info.type == FORTUNE_MAZE_TASK_TYPE_4)
		{
			if (m_param.task_info[i] < task_info.parameter)
			{
				m_param.task_info[i] += 1;
				if (task_info.parameter <= m_param.task_info[i])
				{
					m_param.rand_pin_tu_times += task_info.open_number;
					has_change = true;
				}
			}
			gamelog::g_log_rand_activity.printf(LL_INFO, "RoleActivityFortuneMaze::CatchPet user[%s,%d] task_index[%d] task_type[%d] task_param[%d] task_cfg_param[%d] rand_pin_tu_times[%d] param[%d]",
				m_role_module_mgr->GetRoleName(), m_role_module_mgr->GetUid(), i, FORTUNE_MAZE_TASK_TYPE_4, m_param.task_info[i], task_info.parameter, m_param.rand_pin_tu_times, 1);
		}
	}
	if (has_change)this->SendInfo();
}

void RoleActivityFortuneMaze::PassCrystal()
{
	const RandActivityFortuneMazeConfig * config = static_cast<const RandActivityFortuneMazeConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_FORTUNE_MAZE));
	if (NULL == config) return;
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;
	const FortuneMazeTaskListCfg * task_list = config->GetFortuneMazeTaskListConfig(ramgr);
	if (task_list == NULL)return;
	bool has_change = false;
	for (int i = 0; i < MAX_FORTUNE_MAZE_TASK_NUM && i < (int)task_list->task_list.size(); i++)
	{
		FortuneMazeTaskCfg task_info = task_list->task_list[i];
		if (task_info.type == FORTUNE_MAZE_TASK_TYPE_5)
		{
			if (m_param.task_info[i] < task_info.parameter)
			{
				m_param.task_info[i] += 1;
				if (task_info.parameter <= m_param.task_info[i])
				{
					m_param.rand_pin_tu_times += task_info.open_number;
					has_change = true;
				}
			}
			gamelog::g_log_rand_activity.printf(LL_INFO, "RoleActivityFortuneMaze::PassCrystal user[%s,%d] task_index[%d] task_type[%d] task_param[%d] task_cfg_param[%d] rand_pin_tu_times[%d] param[%d]",
				m_role_module_mgr->GetRoleName(), m_role_module_mgr->GetUid(), i, FORTUNE_MAZE_TASK_TYPE_5, m_param.task_info[i], task_info.parameter, m_param.rand_pin_tu_times, 1);
		}
	}
	if (has_change)this->SendInfo();
}

void RoleActivityFortuneMaze::ExchangeShop()
{
	const RandActivityFortuneMazeConfig * config = static_cast<const RandActivityFortuneMazeConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_FORTUNE_MAZE));
	if (NULL == config) return;
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;
	const FortuneMazeTaskListCfg * task_list = config->GetFortuneMazeTaskListConfig(ramgr);
	if (task_list == NULL)return;
	bool has_change = false;
	for (int i = 0; i < MAX_FORTUNE_MAZE_TASK_NUM && i < (int)task_list->task_list.size(); i++)
	{
		FortuneMazeTaskCfg task_info = task_list->task_list[i];
		if (task_info.type == FORTUNE_MAZE_TASK_TYPE_6)
		{
			if (m_param.task_info[i] < task_info.parameter)
			{
				m_param.task_info[i] += 1;
				if (task_info.parameter <= m_param.task_info[i])
				{
					m_param.rand_pin_tu_times += task_info.open_number;
					has_change = true;
				}
			}
			gamelog::g_log_rand_activity.printf(LL_INFO, "RoleActivityFortuneMaze::ExchangeShop user[%s,%d] task_index[%d] task_type[%d] task_param[%d] task_cfg_param[%d] rand_pin_tu_times[%d] param[%d]",
				m_role_module_mgr->GetRoleName(), m_role_module_mgr->GetUid(), i, FORTUNE_MAZE_TASK_TYPE_6, m_param.task_info[i], task_info.parameter, m_param.rand_pin_tu_times, 1);
		}
	}
	if (has_change)
	{
		this->SendInfo();
		this->OnParticipateRA(__FUNCTION__);
	}
}

void RoleActivityFortuneMaze::GetPrestige(int param)
{
	const RandActivityFortuneMazeConfig * config = static_cast<const RandActivityFortuneMazeConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_FORTUNE_MAZE));
	if (NULL == config) return;
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;
	const FortuneMazeTaskListCfg * task_list = config->GetFortuneMazeTaskListConfig(ramgr);
	if (task_list == NULL)return;
	bool has_change = false;
	for (int i = 0; i < MAX_FORTUNE_MAZE_TASK_NUM && i < (int)task_list->task_list.size(); i++)
	{
		FortuneMazeTaskCfg task_info = task_list->task_list[i];
		if (task_info.type == FORTUNE_MAZE_TASK_TYPE_7)
		{
			if (m_param.task_info[i] < task_info.parameter)
			{
				m_param.task_info[i] += param;
				if (task_info.parameter <= m_param.task_info[i])
				{
					m_param.rand_pin_tu_times += task_info.open_number;
					has_change = true;
				}
			}
			else
			{
				m_param.task_info[i] = task_info.parameter;
				has_change = true;
			}
			gamelog::g_log_rand_activity.printf(LL_INFO, "RoleActivityFortuneMaze::GetPrestige user[%s,%d] task_index[%d] task_type[%d] task_param[%d] task_cfg_param[%d] rand_pin_tu_times[%d] param[%d]",
				m_role_module_mgr->GetRoleName(), m_role_module_mgr->GetUid(), i, FORTUNE_MAZE_TASK_TYPE_7, m_param.task_info[i], task_info.parameter, m_param.rand_pin_tu_times, param);
		}
	}
	if (has_change)this->SendInfo();
}

void RoleActivityFortuneMaze::TreasureHunt(int param)
{
	const RandActivityFortuneMazeConfig * config = static_cast<const RandActivityFortuneMazeConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_FORTUNE_MAZE));
	if (NULL == config) return;
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;
	const FortuneMazeTaskListCfg * task_list = config->GetFortuneMazeTaskListConfig(ramgr);
	if (task_list == NULL)return;
	bool has_change = false;
	for (int i = 0; i < MAX_FORTUNE_MAZE_TASK_NUM && i < (int)task_list->task_list.size(); i++)
	{
		FortuneMazeTaskCfg task_info = task_list->task_list[i];
		if (task_info.type == FORTUNE_MAZE_TASK_TYPE_8)
		{
			if (m_param.task_info[i] < task_info.parameter)
			{
				m_param.task_info[i] += param;
				if (task_info.parameter <= m_param.task_info[i])
				{
					m_param.rand_pin_tu_times += task_info.open_number;
					has_change = true;
				}
			}
			else
			{
				m_param.task_info[i] = task_info.parameter;
				has_change = true;
			}
			gamelog::g_log_rand_activity.printf(LL_INFO, "RoleActivityFortuneMaze::TreasureHunt user[%s,%d] task_index[%d] task_type[%d] task_param[%d] task_cfg_param[%d] rand_pin_tu_times[%d] param[%d]",
				m_role_module_mgr->GetRoleName(), m_role_module_mgr->GetUid(), i, FORTUNE_MAZE_TASK_TYPE_8, m_param.task_info[i], task_info.parameter, m_param.rand_pin_tu_times, param);
		}
	}
	if (has_change)this->SendInfo();
}

void RoleActivityFortuneMaze::KillWildBoos()
{
	const RandActivityFortuneMazeConfig * config = static_cast<const RandActivityFortuneMazeConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_FORTUNE_MAZE));
	if (NULL == config) return;
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;
	const FortuneMazeTaskListCfg * task_list = config->GetFortuneMazeTaskListConfig(ramgr);
	if (task_list == NULL)return;
	bool has_change = false;
	for (int i = 0; i < MAX_FORTUNE_MAZE_TASK_NUM && i < (int)task_list->task_list.size(); i++)
	{
		FortuneMazeTaskCfg task_info = task_list->task_list[i];
		if (task_info.type == FORTUNE_MAZE_TASK_TYPE_9)
		{
			if (m_param.task_info[i] < task_info.parameter)
			{
				m_param.task_info[i] += 1;
				if (task_info.parameter <= m_param.task_info[i])
				{
					m_param.rand_pin_tu_times += task_info.open_number;
					has_change = true;
				}
			}
			gamelog::g_log_rand_activity.printf(LL_INFO, "RoleActivityFortuneMaze::KillWildBoos user[%s,%d] task_index[%d] task_type[%d] task_param[%d] task_cfg_param[%d] rand_pin_tu_times[%d] param[%d]",
				m_role_module_mgr->GetRoleName(), m_role_module_mgr->GetUid(), i, FORTUNE_MAZE_TASK_TYPE_9, m_param.task_info[i], task_info.parameter, m_param.rand_pin_tu_times, 1);
		}
	}
	if (has_change)this->SendInfo();
}

void RoleActivityFortuneMaze::StrengThenPet()
{
	const RandActivityFortuneMazeConfig * config = static_cast<const RandActivityFortuneMazeConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_FORTUNE_MAZE));
	if (NULL == config) return;
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;
	const FortuneMazeTaskListCfg * task_list = config->GetFortuneMazeTaskListConfig(ramgr);
	if (task_list == NULL)return;
	bool has_change = false;
	for (int i = 0; i < MAX_FORTUNE_MAZE_TASK_NUM && i < (int)task_list->task_list.size(); i++)
	{
		FortuneMazeTaskCfg task_info = task_list->task_list[i];
		if (task_info.type == FORTUNE_MAZE_TASK_TYPE_10)
		{
			if (m_param.task_info[i] < task_info.parameter)
			{
				m_param.task_info[i] += 1;
				if (task_info.parameter <= m_param.task_info[i])
				{
					m_param.rand_pin_tu_times += task_info.open_number;
					has_change = true;
				}
			}
			gamelog::g_log_rand_activity.printf(LL_INFO, "RoleActivityFortuneMaze::StrengThenPet user[%s,%d] task_index[%d] task_type[%d] task_param[%d] task_cfg_param[%d] rand_pin_tu_times[%d] param[%d]",
				m_role_module_mgr->GetRoleName(), m_role_module_mgr->GetUid(), i, FORTUNE_MAZE_TASK_TYPE_10, m_param.task_info[i], task_info.parameter, m_param.rand_pin_tu_times, 1);
		}
	}
	if (has_change)this->SendInfo();
}

void RoleActivityFortuneMaze::ExpeditionModron()
{
	const RandActivityFortuneMazeConfig * config = static_cast<const RandActivityFortuneMazeConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_FORTUNE_MAZE));
	if (NULL == config) return;
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;
	const FortuneMazeTaskListCfg * task_list = config->GetFortuneMazeTaskListConfig(ramgr);
	if (task_list == NULL)return;
	bool has_change = false;
	for (int i = 0; i < MAX_FORTUNE_MAZE_TASK_NUM && i < (int)task_list->task_list.size(); i++)
	{
		FortuneMazeTaskCfg task_info = task_list->task_list[i];
		if (task_info.type == FORTUNE_MAZE_TASK_TYPE_11)
		{
			if (m_param.task_info[i] < task_info.parameter)
			{
				m_param.task_info[i] += 1;
				if (task_info.parameter <= m_param.task_info[i])
				{
					m_param.rand_pin_tu_times += task_info.open_number;
					has_change = true;
				}
			}
			gamelog::g_log_rand_activity.printf(LL_INFO, "RoleActivityFortuneMaze::ExpeditionModron user[%s,%d] task_index[%d] task_type[%d] task_param[%d] task_cfg_param[%d] rand_pin_tu_times[%d] param[%d]",
				m_role_module_mgr->GetRoleName(), m_role_module_mgr->GetUid(), i, FORTUNE_MAZE_TASK_TYPE_11, m_param.task_info[i], task_info.parameter, m_param.rand_pin_tu_times, 1);
		}
	}
	if (has_change)this->SendInfo();
}

void RoleActivityFortuneMaze::UseCrystalPower(int param)
{
	const RandActivityFortuneMazeConfig * config = static_cast<const RandActivityFortuneMazeConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_FORTUNE_MAZE));
	if (NULL == config) return;
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;
	const FortuneMazeTaskListCfg * task_list = config->GetFortuneMazeTaskListConfig(ramgr);
	if (task_list == NULL)return;
	bool has_change = false;
	for (int i = 0; i < MAX_FORTUNE_MAZE_TASK_NUM && i < (int)task_list->task_list.size(); i++)
	{
		FortuneMazeTaskCfg task_info = task_list->task_list[i];
		if (task_info.type == FORTUNE_MAZE_TASK_TYPE_12)
		{
			if (m_param.task_info[i] < task_info.parameter)
			{
				m_param.task_info[i] += param;
				if (task_info.parameter <= m_param.task_info[i])
				{
					m_param.rand_pin_tu_times += task_info.open_number;
					has_change = true;
				}
			}
			else
			{
				m_param.task_info[i] = task_info.parameter;
				has_change = true;
			}
			gamelog::g_log_rand_activity.printf(LL_INFO, "RoleActivityFortuneMaze::UseCrystalPower user[%s,%d] task_index[%d] task_type[%d] task_param[%d] task_cfg_param[%d] rand_pin_tu_times[%d] param[%d]",
				m_role_module_mgr->GetRoleName(), m_role_module_mgr->GetUid(), i, FORTUNE_MAZE_TASK_TYPE_12, m_param.task_info[i], task_info.parameter, m_param.rand_pin_tu_times, param);
		}
	}
	if (has_change)this->SendInfo();
}

void RoleActivityFortuneMaze::OnCheckRoleLoginDelay(int old_dayid, int now_dayid)
{
	if (old_dayid == now_dayid)return;
	if (m_param.today_login_flag == 1)return;
	const RandActivityFortuneMazeConfig * config = static_cast<const RandActivityFortuneMazeConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_FORTUNE_MAZE));
	if (NULL == config) return;
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;
	const FortuneMazeTaskListCfg * task_list = config->GetFortuneMazeTaskListConfig(ramgr);
	if (task_list == NULL)return;
	bool has_change = false;
	for (int i = 0; i < MAX_FORTUNE_MAZE_TASK_NUM && i < (int)task_list->task_list.size(); i++)
	{
		FortuneMazeTaskCfg task_info = task_list->task_list[i];
		if (task_info.type == FORTUNE_MAZE_TASK_TYPE_13)
		{
			if (m_param.task_info[i] < task_info.parameter)
			{
				m_param.task_info[i] += 1;
				if (task_info.parameter <= m_param.task_info[i])
				{
					m_param.rand_pin_tu_times += task_info.open_number;
					m_param.today_login_flag = 1;
					has_change = true;
				}
			}
			gamelog::g_log_rand_activity.printf(LL_INFO, "RoleActivityFortuneMaze::OnCheckRoleLoginDelay user[%s,%d] task_index[%d] task_type[%d] task_param[%d] task_cfg_param[%d] rand_pin_tu_times[%d] param[%d]",
				m_role_module_mgr->GetRoleName(), m_role_module_mgr->GetUid(), i, FORTUNE_MAZE_TASK_TYPE_13, m_param.task_info[i], task_info.parameter, m_param.rand_pin_tu_times, 1);
		}
	}
	if (has_change)this->SendInfo();
}

void RoleActivityFortuneMaze::BuyStone(int param)
{
	const RandActivityFortuneMazeConfig * config = static_cast<const RandActivityFortuneMazeConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_FORTUNE_MAZE));
	if (NULL == config) return;
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;
	const FortuneMazeTaskListCfg * task_list = config->GetFortuneMazeTaskListConfig(ramgr);
	if (task_list == NULL)return;
	bool has_change = false;
	for (int i = 0; i < MAX_FORTUNE_MAZE_TASK_NUM && i < (int)task_list->task_list.size(); i++)
	{
		FortuneMazeTaskCfg task_info = task_list->task_list[i];
		if (task_info.type == FORTUNE_MAZE_TASK_TYPE_14)
		{
			if (m_param.task_info[i] < task_info.parameter)
			{
				m_param.task_info[i] += param;
				if (task_info.parameter <= m_param.task_info[i])
				{
					m_param.rand_pin_tu_times += task_info.open_number;
					has_change = true;
				}
			}
			else
			{
				m_param.task_info[i] = task_info.parameter;
				has_change = true;
			}
			gamelog::g_log_rand_activity.printf(LL_INFO, "RoleActivityFortuneMaze::BuyStone user[%s,%d] task_index[%d] task_type[%d] task_param[%d] task_cfg_param[%d] rand_pin_tu_times[%d] param[%d]",
				m_role_module_mgr->GetRoleName(), m_role_module_mgr->GetUid(), i, FORTUNE_MAZE_TASK_TYPE_14, m_param.task_info[i], task_info.parameter, m_param.rand_pin_tu_times, param);
		}
	}
	if (has_change)
	{
		this->SendInfo();
		this->OnParticipateRA(__FUNCTION__);
	}
}

void RoleActivityFortuneMaze::OnRoleEnterSpecialFb()
{	
	if (m_param.can_enter_fb != 1)return;
	if (FBManager::Instance().EnterFB(m_role_module_mgr->GetRole(), FBManager::FB_TYPE_FORTUNE_MAZE, m_param.old_scene_id, 0, false))
	{
		this->SendInfo();
		this->SendMapInfo();
	}
}

void RoleActivityFortuneMaze::OnRoleOpenTreasureChest()
{
	m_param.old_scene_id = 0;
	m_param.can_enter_fb = 0;
	this->SendInfo();
	this->SendMapInfo();
}

void RoleActivityFortuneMaze::SetRoleOldSceneId(int old_scene_id)
{
	m_param.old_scene_id = old_scene_id;
}

void RoleActivityFortuneMaze::SetMapMarkIndex(int map_index)
{
	if (map_index < 0 || map_index >= PIN_TU_TYPE_NUM)
	{
		return;
	}
	m_param.mark_index = (short)map_index;
	this->SendMapInfo();
}

void RoleActivityFortuneMaze::RandPinTuIndex(int index)
{
	if (index < 0 || index >= PIN_TU_TYPE_NUM)
	{
		return;
	}

	if (m_param.rand_pin_tu_times <= 0)return;
	if (m_param.can_enter_fb == 1)return;

	std::vector<int> can_rand_vec;
	std::vector<int> can_rand_posi_vec;
	for (int i = 0; i < PIN_TU_INDEX_NUM; i++)
	{
		if (!m_param.HasPinTu(index, i + 1))//碎片id比下标大1
		{
			can_rand_vec.push_back(i + 1);
		}

		if (!m_param.PinTuHasOpen(index, i))
		{
			can_rand_posi_vec.push_back(i);
		}
	}
	if (can_rand_vec.size() <= 0)return;
	if (can_rand_posi_vec.size() <= 0)return;
	
	int rand_id = rand() % (int)can_rand_vec.size();
	int rand_posi = rand() % (int)can_rand_posi_vec.size();

	m_param.m_pin_tu[index][can_rand_posi_vec[rand_posi]] = can_rand_vec[rand_id];

	m_param.today_pin_tu_times += 1;
	m_param.rand_pin_tu_times -= 1;
	if (m_param.HasOpenAll(index))
	{
		this->CheckPinTuFinish(index);
	}
	this->SendMapInfo();
	this->SendInfo();
}

void RoleActivityFortuneMaze::MoivePinTu(int map_index, int moive_index_1, int moive_index_2)
{
	if (!m_param.HasOpenAll(map_index))return;
	if (m_param.can_enter_fb == 1)return;
	if (moive_index_1 < 0 || moive_index_1 >= PIN_TU_INDEX_NUM || moive_index_2 < 0 || moive_index_2 >= PIN_TU_INDEX_NUM)
	{
		return;
	}

	if (!m_param.ExchangePosi(map_index, moive_index_1, moive_index_2))return;
	this->SendMapInfo();
	this->CheckPinTuFinish(map_index);
}

void RoleActivityFortuneMaze::GMAddTimes(int times)
{
	m_param.rand_pin_tu_times += times;
	this->SendInfo();
}

void RoleActivityFortuneMaze::ItemAddTimes(int times)
{
	m_param.rand_pin_tu_times += times;
	this->SendInfo();
}

void RoleActivityFortuneMaze::CheckPinTuFinish(int map_index)
{
	if (!m_param.HasOpenAll(map_index))return;
	
	for (int index = 0; index < PIN_TU_INDEX_NUM; index++)
	{
		if (m_param.m_pin_tu[map_index][index] != PIN_TU_INDEX[index])
		{
			return;
		}
	}

	m_param.can_enter_fb = 1;
	//拼图完成
	//m_param.ResetMap();
	this->SendInfo();
	this->SendMapInfo();

	{
		RoleActivityWanLingGongYing * role_activity = static_cast<RoleActivityWanLingGongYing *>
			(m_role_module_mgr->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_WAN_LING_GONG_YING));
		if (NULL != role_activity)
		{
			role_activity->OnCompleteTask(RAND_ACTIVITY_TYPE_FORTUNE_MAZE);
		}
	}
}

void RoleActivityFortuneMaze::SendInfo()
{
	Protocol::SCRAFortuneMazeInfo info;
	
	info.begin_time = m_param.begin_time;
	info.open_day = m_param.open_day;
	info.rand_pin_tu_times = m_param.rand_pin_tu_times;
	info.today_pin_tu_times = m_param.today_pin_tu_times;
	info.can_enter_fb = m_param.can_enter_fb;
	memcpy(info.task_info, m_param.task_info, sizeof(info.task_info));
	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void RoleActivityFortuneMaze::SendMapInfo()
{
	Protocol::SCRAFortuneMazePinTuInfo info;
	info.mark_index = (int)m_param.mark_index;
	memcpy(info.m_pin_tu, m_param.m_pin_tu, sizeof(info.m_pin_tu));
	m_role_module_mgr->NetSend(&info, sizeof(info));
}
