#include "other/rolemodulemanager.hpp"
#include "config/logicconfigmanager.hpp"
#include "task/taskpool.h"
#include "task/taskmanager.h"
#include "gamelog.h"
#include "global/guild/guild.hpp"
#include "obj/character/role.h"
#include "battle/battle_manager_local.hpp"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "servercommon/errornum.h"

#include "other/roleguildtask/roleguildtaskconfig.hpp"
#include "protocol/msgroleguildtask.h"
#include "roleguildtask.hpp"

RoleGuildTask::RoleGuildTask()
{
	m_role_module_mgr = NULL;
}

RoleGuildTask::~RoleGuildTask()
{
}

void RoleGuildTask::Init(RoleModuleManager * role_module_manager, const RoleGuildTaskParam & param)
{
	m_role_module_mgr = role_module_manager;
	m_param = param;
}

void RoleGuildTask::GetInitParam(RoleGuildTaskParam * param)
{
	*param = m_param;
}

void RoleGuildTask::Update(time_t now_second)
{
	this->UpdateRefresh(now_second);
}

void RoleGuildTask::UpdateRefresh(time_t now_second)
{
	bool need_send = false;
	int role_level = m_role_module_mgr->GetRole()->GetLevel();
	for (int i = 0; i < ARRAY_LENGTH(m_param.task_list); ++i)
	{
		RoleGuildTaskItem & curr = m_param.task_list[i];

		if (!curr.is_get && now_second >= curr.refresh_time)
		{
			const RoleGuildTaskItemCfg * cfg = LOGIC_CONFIG->GetRoleGuildTaskConfig()->GetRandRoleGuildTaskItemCfg(role_level);
			if (!cfg)
			{
				continue;
			}

			curr.is_get = 1;
			curr.task_id = cfg->task_id;
			need_send = true;
		}
	}

	if (need_send)
	{
		this->SendInfo();
	}
}

void RoleGuildTask::OnDayChange()
{
	m_param.finis_times = 0;
	this->SendInfo();
}

void RoleGuildTask::OnFinishTask(int finish_task_id)
{
	int task_index = m_param.accept_task_index;
	RoleGuildTaskItem * task_item = this->GetRoleGuildTaskItem(task_index);
	if (!task_item)
	{
		return;
	}

	if (task_item->curr_task_id != finish_task_id)
	{
		return;
	}

	int task_id = task_item->task_id;
	const RoleGuildTaskItemCfg * cfg = LOGIC_CONFIG->GetRoleGuildTaskConfig()->GetRoleGuildTaskItemCfg(task_id);
	if (!cfg)
	{
		return;
	}

	if (cfg->finish_task_id != finish_task_id)
	{
		const TaskInfomaConfig * task_cfg = TASKPOOL->GetTaskCfgByID(finish_task_id);
		if (task_cfg && m_role_module_mgr->GetTaskManager()->OnAcceptTask(task_cfg->nex_task_id))
		{
			task_item->curr_task_id = task_cfg->nex_task_id;
			this->SendChangeInfo(Protocol::ROLE_GUILD_TASK_CHANGE_REASON_START, task_index);
			return;
		}
	}

	unsigned int now = (unsigned int)EngineAdapter::Instance().Time();
	const RoleGuildTaskOtherCfg & other_cfg = LOGIC_CONFIG->GetRoleGuildTaskConfig()->GetOtherCfg();

	m_param.finis_times += 1;
	m_param.accept_task_index = -1;
	task_item->Reset();
	task_item->refresh_time = now + other_cfg.finish_cd;

	this->SendChangeInfo(Protocol::ROLE_GUILD_TASK_CHANGE_REASON_FINISH, task_index);

	Guild * guild = m_role_module_mgr->GetGuild();
	if (guild)
	{
		guild->GuildTaskClearSolicit(m_role_module_mgr->GetRoleUID());
		guild->GuildTaskSetFinishTimes(m_role_module_mgr->GetRole());
		guild->AddExp(cfg->guild_exp);
	}


	const RoleGuildTaskRewardGroupItemCfg * rand_reward_cfg = LOGIC_CONFIG->GetRoleGuildTaskConfig()->RandGuildTaskRewardGroupItemCfg(cfg->reward_group_id);
	if (rand_reward_cfg)
	{
		m_role_module_mgr->GetKnapsack()->PutVec(rand_reward_cfg->rewards, PUT_REASON_GUILD_TASK_FINISH);
	}
	m_role_module_mgr->GetMoney()->OnAddMoney(MONEY_TYPE_GUILD_CONTRIBUTION, cfg->guild_gong_xian, __FUNCTION__);
	
	
	gamelog::g_log_role_guild_task.printf(LL_INFO, "%s line:%d |%s task_idx:%d  task_id:%d finis_times:%d", __FUNCTION__, __LINE__,
		m_role_module_mgr->GetRoleInfoString().c_str(), task_index, finish_task_id, m_param.finis_times);
}

void RoleGuildTask::SendInfo()
{
	Protocol::SCRoleGuildTaskInfo protocol;
	protocol.finish_times = m_param.finis_times;
	protocol.task_index = m_param.accept_task_index;
	SameArrayTypeCopy(protocol.task_list, m_param.task_list);

	m_role_module_mgr->NetSend(&protocol,sizeof(protocol));
}

void RoleGuildTask::SendChangeInfo(int reason, int task_index)
{
	if (task_index < 0 || task_index >= ARRAY_LENGTH(m_param.task_list))
	{
		return;
	}

	const RoleGuildTaskItem & item = m_param.task_list[task_index];

	Protocol::SCRoleGuildTaskChange protocol;
	protocol.reason = reason;
	protocol.task_index = task_index;
	protocol.task = item;

	m_role_module_mgr->NetSend(&protocol, sizeof(protocol));
}

int RoleGuildTask::GetFinishTimes() const
{
	return m_param.finis_times;
}

void RoleGuildTask::OnGiveUpTask(int task_id, int type)
{
	if (type == TASK_MANAGER_GIVE_UP_TYPE_GUILD_TASK)
	{
		return;
	}

	int task_index = m_param.accept_task_index;
	RoleGuildTaskItem * item = this->GetRoleGuildTaskItem(task_index);
	if (!item)
	{
		return;
	}

	if (item->curr_task_id != task_id)
	{
		return;
	}

	Guild * guild = m_role_module_mgr->GetGuild();
	if (guild)
	{
		guild->GuildTaskClearSolicit(m_role_module_mgr->GetRoleUID(), true);
	}

	m_param.accept_task_index = -1;
	this->SendChangeInfo(Protocol::ROLE_GUILD_TASK_CHANGE_REASON_GIVE_UP, task_index);
}

int RoleGuildTask::ReqStartTask(int task_index)
{
	if (m_param.accept_task_index != -1)
	{
		ret_line;
	}

	RoleGuildTaskItem * item = this->GetRoleGuildTaskItem(task_index);
	if (!item)
	{
		ret_line;
	}

	int task_id = item->task_id;
	if (m_role_module_mgr->GetTaskManager()->HasRoleGuildTask())
	{
		ret_line;
	}

	const RoleGuildTaskOtherCfg & other_cfg = LOGIC_CONFIG->GetRoleGuildTaskConfig()->GetOtherCfg();
	if (m_param.finis_times >= other_cfg.max_finish_times)
	{
		ret_line;
	}

	m_role_module_mgr->GetTaskManager()->ClearRoleGuildTask();

	if (!m_role_module_mgr->GetTaskManager()->OnAcceptTask(task_id))
	{
		m_param.accept_task_index = -1;
		ret_line;
	}

	m_param.accept_task_index = task_index;
	item->curr_task_id = task_id;
	this->SendChangeInfo(Protocol::ROLE_GUILD_TASK_CHANGE_REASON_START, task_index);

	gamelog::g_log_role_guild_task.printf(LL_INFO, "%s line:%d |%s task_idx:%d  task_id:%d", __FUNCTION__, __LINE__,
		m_role_module_mgr->GetRoleInfoString().c_str(), task_index, task_id);

	return 0;
}

int RoleGuildTask::ReqGiveUpTask(int task_index)
{
	RoleGuildTaskItem * item = this->GetRoleGuildTaskItem(task_index);
	if (!item)
	{
		ret_line;
	}

	unsigned int now = (unsigned int)EngineAdapter::Instance().Time();
	const RoleGuildTaskOtherCfg & other_cfg = LOGIC_CONFIG->GetRoleGuildTaskConfig()->GetOtherCfg();
	
	if (m_param.accept_task_index == task_index)
	{
		m_role_module_mgr->GetTaskManager()->OnGiveupTask(item->curr_task_id, TASK_MANAGER_GIVE_UP_TYPE_GUILD_TASK);
		m_role_module_mgr->GetTaskManager()->ClearRoleGuildTask();
		m_param.accept_task_index = -1;

		Guild * guild = m_role_module_mgr->GetGuild();
		if (guild)
		{
			guild->GuildTaskClearSolicit(m_role_module_mgr->GetRoleUID(), true);
		}
	}

	item->Reset();
	item->refresh_time = now + other_cfg.give_up_cd;
	this->SendChangeInfo(Protocol::ROLE_GUILD_TASK_CHANGE_REASON_GIVE_UP, task_index);

	gamelog::g_log_role_guild_task.printf(LL_INFO, "%s line:%d |%s task_idx:%d  task_id:%d", __FUNCTION__, __LINE__,
		m_role_module_mgr->GetRoleInfoString().c_str(), task_index, item->task_id);

	return 0;
}

int RoleGuildTask::ReqBattle()
{
	RoleGuildTaskItem * item = this->GetCurrRoleGuildTaskItem();
	if (!item)
	{
		ret_line;
	}

	const RoleGuildTaskItemCfg * cfg = LOGIC_CONFIG->GetRoleGuildTaskConfig()->GetRoleGuildTaskItemCfg(item->task_id);
	if (!cfg)
	{
		ret_line;
	}

	if (cfg->type != ROLE_GUILD_TASK_TYPE_SINGLE_BATTLE && cfg->type != ROLE_GUILD_TASK_TYPE_TEAM_BATTLE)
	{
		ret_line;
	}

	Team * team = m_role_module_mgr->GetRole()->GetMyTeam();
	if (cfg->type == ROLE_GUILD_TASK_TYPE_SINGLE_BATTLE)
	{
		if (team)
		{
			ret_line;
		}
	}

	if (cfg->type == ROLE_GUILD_TASK_TYPE_TEAM_BATTLE)
	{
		if (!team)
		{
			ret_line;
		}
	}

	const TaskInfomaConfig * task_cfg = TASKPOOL->GetTaskCfgByID(item->curr_task_id);
	if (!task_cfg)
	{
		ret_line;
	}

	BattleManagerLocal::GetInstance().StartBattleModeFightReq(m_role_module_mgr->GetRole(),
		BATTLE_MODE_TASK_ROLE_MONSTER, task_cfg->param1, item->curr_task_id, 0, true);

	return 0;
}

int RoleGuildTask::ReqSolicit()
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		m_role_module_mgr->NoticeNum(errornum::EN_OPERATE_FORBID_IN_CROSS);
		ret_line;
	}

	RoleGuildTaskItem * item = this->GetCurrRoleGuildTaskItem();
	if (!item)
	{
		ret_line;
	}
	int task_index = m_param.accept_task_index;
	const RoleGuildTaskItemCfg * cfg = LOGIC_CONFIG->GetRoleGuildTaskConfig()->GetRoleGuildTaskItemCfg(item->task_id);
	if (!cfg)
	{
		ret_line;
	}

	if (!cfg->can_help)
	{
		ret_line;
	}

	Guild * guild = m_role_module_mgr->GetGuild();
	if (!guild)
	{
		ret_line;
	}

	if (!guild->GuildTaskSolicit(m_role_module_mgr->GetRoleUID(), item->task_id, task_index))
	{
		ret_line;
	}

	item->is_solicit = 1;

	gamelog::g_log_role_guild_task.printf(LL_INFO, "%s line:%d |%s task_idx:%d  task_id:%d", __FUNCTION__, __LINE__,
		m_role_module_mgr->GetRoleInfoString().c_str(), task_index, item->task_id);

	this->SendChangeInfo(Protocol::ROLE_GUILD_TASK_CHANGE_REASON_SOLICIT, task_index);

	return 0;
}

int RoleGuildTask::ReqGive(int target_role_id)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		m_role_module_mgr->NoticeNum(errornum::EN_OPERATE_FORBID_IN_CROSS);
		ret_line;
	}

	Guild * guild = m_role_module_mgr->GetGuild();
	if (guild)
	{
		guild->GuildTaskGive(m_role_module_mgr->GetRole(), target_role_id);
	}

	return 0;
}

int RoleGuildTask::ReqRefresh()
{
	const RoleGuildTaskOtherCfg & other_cfg = LOGIC_CONFIG->GetRoleGuildTaskConfig()->GetOtherCfg();

	if (!m_role_module_mgr->GetMoney()->UseMoney(MONEY_TYPE_SILVER_COIN, other_cfg.refresh_comsume_value, __FUNCTION__))
	{
		ret_line;
	}

	this->ReqGiveUpTask(m_param.accept_task_index);
	m_param.accept_task_index = -1;
	ArrayCallReset(m_param.task_list);

	return 0;
}

int RoleGuildTask::ReqMailNotice(bool is_all,int role_id)
{
	Guild * guild = m_role_module_mgr->GetGuild();
	if (guild)
	{
		guild->GuildTaskMailNotice(m_role_module_mgr->GetUid(), is_all, role_id);
	}

	return 0;
}

int RoleGuildTask::ReqFetch()
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		m_role_module_mgr->NoticeNum(errornum::EN_OPERATE_FORBID_IN_CROSS);
		ret_line;
	}

	Guild * guild = m_role_module_mgr->GetGuild();
	if (!guild)
	{
		ret_line;
	}

	guild->GuildTaskFetch(m_role_module_mgr->GetRole());

	return 0;
}

RoleGuildTaskItem * RoleGuildTask::GetCurrRoleGuildTaskItem()
{
	return this->GetRoleGuildTaskItem(m_param.accept_task_index);
}

RoleGuildTaskItem * RoleGuildTask::GetRoleGuildTaskItem(int task_index)
{
	if (task_index < 0 || task_index >= ARRAY_LENGTH(m_param.task_list))
	{
		return NULL;
	}

	RoleGuildTaskItem & tmp = m_param.task_list[task_index];
	if (!tmp.is_get)
	{
		return NULL;
	}

	return &tmp;
}
