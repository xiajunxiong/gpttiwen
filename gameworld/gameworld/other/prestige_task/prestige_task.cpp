#include "prestige_task.hpp"
#include "servercommon/servercommon.h"
#include "other/rolemodulemanager.hpp"
#include "servercommon/errornum.h"
#include "protocol/msgtask.h"
#include "other/prestige/prestigeconfig.hpp"
#include "config/logicconfigmanager.hpp"
#include "task/taskpool.h"
#include "obj/character/role.h"
#include "other/prestige/prestige.hpp"
#include "task/taskrecorder.h"
#include "gamelog.h"
#include "other/event/eventhandler.hpp"
#include "task/taskmanager.h"
#include "servercommon/activesystemdef.hpp"
#include "other/activesystem/activesystem.hpp"
#include "other/rolejoinactivityreward/rolejoinactivityreward.hpp"

PrestigeTask::PrestigeTask() : m_mgr(NULL)
{

}

PrestigeTask::~PrestigeTask()
{

}

void PrestigeTask::Init(RoleModuleManager * mgr, const PrestigeTaskParam& param)
{
	m_mgr = mgr;
	m_param = param;
}

void PrestigeTask::GetInitParam(PrestigeTaskParam* param)
{
	if (NULL != param) *param = m_param;
}

void PrestigeTask::OnDayChange()
{
	for (int i = 0; i < ARRAY_ITEM_COUNT(m_param.today_task_list); ++i)
	{
		int task_id = m_param.today_task_list[i];
		const TaskInfomaConfig* task_cfg = TaskPool::Instance()->GetTaskCfgByID(task_id);
		if (NULL == task_cfg) continue;
		if (TASK_TYPE_PRESTIGE_TASK != task_cfg->task_type) continue;

		this->ClearTask(task_id);
	}

	m_param.village_id = 0;
	m_param.today_task_num = 0;
	m_param.today_finish_task_num = 0;
	m_param.cur_task_id = 0;
	memset(m_param.today_task_list, 0, sizeof(m_param.today_task_list));
	m_param.today_total_prestige = 0;
	memset(m_param.today_star_list, 0, sizeof(m_param.today_star_list));

	this->SendPrestigeTaskInfo();

	gamelog::g_log_prestige_task.printf(LL_INFO, "PrestigeTask::OnDayChange role[%d,%s]", m_mgr->GetRoleUID(), m_mgr->GetRoleName());
}

void PrestigeTask::OnSelectVillageReq(int village_id)
{
	if (m_param.village_id > 0)
	{
		m_mgr->NoticeNum(errornum::EN_TODAY_VILLAGE_ALREADY_SELECT);
		return;
	}

	const PrestigeVillageCfg* village_cfg = LOGIC_CONFIG->GetPrestigeConfig()->GetVillageCfg(village_id);
	if (NULL == village_cfg)
	{
		m_mgr->NoticeNum(errornum::EN_VILLAGE_NOT_FOUND);
		return;
	}

	const PrestigeTaskVillageCfg* task_village_cfg = TaskPool::Instance()->GetPrestigeTaskVillageCfg(village_id);
	if (NULL == task_village_cfg)
	{
		m_mgr->NoticeNum(errornum::EN_VILLAGE_NO_TASK_FOUND);
		return;
	}

	if (m_mgr->GetRole()->GetLevel() < task_village_cfg->unlock_level)
	{
		m_mgr->NoticeNum(errornum::EN_ROLE_LEVEL_NOT_ENOUTH);
		return;
	}

	int task_group = TaskPool::Instance()->RandPrestigeTaskGroup(village_id);
	if (0 == task_group)
	{
		m_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	UInt16 task_list[MAX_PRESTIGE_TASK_NUM_PER_DAY] = { 0 };
	int task_num = TaskPool::Instance()->FillPrestigeTaskList(village_id, task_group, ARRAY_ITEM_COUNT(task_list), task_list);
	if (0 == task_num)
	{
		m_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	m_param.village_id = village_id;
	m_param.today_task_num = task_num;
	m_param.today_finish_task_num = 0;

	bool is_rate_2 = false;
	for (int i = 0; i < task_num && i < ARRAY_ITEM_COUNT(m_param.today_task_list); ++i)
	{
		m_param.today_task_list[i] = task_list[i];

		const PrestigeTaskCfg* task_cfg = TaskPool::Instance()->GetPrestigeTaskCfg(task_list[i]);

		// 随机星级
		if (NULL != task_cfg && task_cfg->is_activedegree)
		{
			int star = TaskPool::Instance()->GetPrestigeTaskStar(is_rate_2);
			if (star > 0)
			{
				m_param.today_star_list[i] = star;
				if (star == 5) is_rate_2 = true;
			}
		}
	}

	this->SendPrestigeTaskInfo();
}

void PrestigeTask::OnReq(void * msg)
{
	if (NULL == msg)
		return;

	CSPrestigeTaskReq * req = (CSPrestigeTaskReq *)msg;
	switch (req->type)
	{
	case 0:		// 刷新任务星级
		{
			this->OnRandTaskStar();
		}
		break;
	case 1:		// 开始任务
		{
			this->OnTaskAccept();
		}
		break;
	case 2:
		{
			this->SendAllInfo();
		}
		break;
	}
}

void PrestigeTask::OnRandTaskStar()
{
	if (m_param.village_id <= 0)
	{
		m_mgr->NoticeNum(errornum::EN_VILLAGE_NO_TASK_FOUND);
		return;
	}

	if (m_param.cur_task_id > 0)
	{
		return;
	}

	if (!m_mgr->GetKnapsack()->ConsumeItem(TaskPool::Instance()->GetPrestigeTaskOtherCfg()->use_item, 1, __FUNCTION__, 0, false))
	{
		m_mgr->NoticeNum(errornum::EN_ITEM_AMOUNT_NOT_ENOUGH);
		return;
	}

	bool is_rate_2 = false;
	if (m_param.baodi_refresh_num >= TaskPool::Instance()->GetPrestigeTaskOtherCfg()->reset_times)
	{
		is_rate_2 = true;
		std::vector<short> pool;

		for (int i = 0; i < m_param.today_task_num && i < ARRAY_ITEM_COUNT(m_param.today_task_list); ++i)
		{
			const PrestigeTaskCfg* task_cfg = TaskPool::Instance()->GetPrestigeTaskCfg(m_param.today_task_list[i]);

			// 随机星级
			if (NULL != task_cfg && task_cfg->is_activedegree)
			{
				int star = TaskPool::Instance()->GetPrestigeTaskStar(is_rate_2);
				if (star > 0)
				{
					m_param.today_star_list[i] = star;
					pool.push_back(i);
				}
			}
		}

		int rand = RandomNum(pool.size());
		int i = pool[rand];
		m_param.today_star_list[i] = 5;
	}
	else
	{
		for (int i = 0; i < m_param.today_task_num && i < ARRAY_ITEM_COUNT(m_param.today_task_list); ++i)
		{
			const PrestigeTaskCfg* task_cfg = TaskPool::Instance()->GetPrestigeTaskCfg(m_param.today_task_list[i]);

			// 随机星级
			if (NULL != task_cfg && task_cfg->is_activedegree)
			{
				int star = TaskPool::Instance()->GetPrestigeTaskStar(is_rate_2);
				if (star > 0)
				{
					m_param.today_star_list[i] = star;
					if (star == 5) is_rate_2 = true;
				}
			}
		}
	}

	if (is_rate_2) m_param.baodi_refresh_num = 0;
	else m_param.baodi_refresh_num++;

	this->SendPrestigeTaskInfo();
}

void PrestigeTask::OnTaskAccept()
{
	if (m_param.village_id <= 0)
	{
		m_mgr->NoticeNum(errornum::EN_VILLAGE_NO_TASK_FOUND);
		return;
	}

	if (!this->AcceptPrestigeTask(m_param.today_task_list[0]))
	{
		m_mgr->NoticeNum(errornum::EN_PRESTIGE_TASK_ACCEPT_FAILED);
		return;
	}

	m_param.cur_task_id = m_param.today_task_list[0];

	this->SendPrestigeTaskInfo();

	gamelog::g_log_prestige_task.buff_printf("PrestigeTask::OnSelectVillageReq role[%d,%s] village_id[%d] first_task_id[%d]", 
		m_mgr->GetRoleUID(), m_mgr->GetRoleName(), m_param.village_id, m_param.cur_task_id);
	for (int i = 0; i < m_param.today_task_num && i < ARRAY_ITEM_COUNT(m_param.today_task_list); ++i)
	{
		gamelog::g_log_prestige_task.buff_printf("[taskid:%d star:%d] ", m_param.today_task_list[i], m_param.today_star_list[i]);
	}
	gamelog::g_log_prestige_task.commit_buff(LL_INFO);
}

void PrestigeTask::OnTaskCommit(int task_id)
{
	if (m_param.cur_task_id == task_id && task_id != 0)
	{
		gamelog::g_log_prestige_task.printf(LL_INFO, "PrestigeTask::OnTaskCommit role[%d,%s] task_id[%d]",
			m_mgr->GetRoleUID(), m_mgr->GetRoleName(), task_id);

		this->IssuePrestigeTaskReward(task_id);

		m_param.today_finish_task_num += 1;
		if (m_param.today_finish_task_num >= m_param.today_task_num)
		{
			m_param.cur_task_id = 0;
			if (m_param.today_finish_task_num == m_param.today_task_num)
			{
				this->IssuePrestigeTaskEndReward(m_param.village_id);
				this->SendLastNotice();
			}
		}
		else
		{
			m_param.cur_task_id = m_param.today_task_list[m_param.today_finish_task_num];

			if (!this->AcceptPrestigeTask(m_param.cur_task_id))
			{
				gamelog::g_log_prestige_task.printf(LL_ERROR, "PrestigeTask::OnTaskCommit Accept Next Task FAILED!!! role[%d,%s] next_tast_id[%d]",
					m_mgr->GetRoleUID(), m_mgr->GetRoleName(), m_param.cur_task_id);
			}
		}

		gamelog::g_log_prestige_task.printf(LL_INFO, "PrestigeTask::OnTaskCommit role[%d,%s] today_finish_task_num[%d] cur_task_id[%d]",
			m_mgr->GetRoleUID(), m_mgr->GetRoleName(), m_param.today_finish_task_num, m_param.cur_task_id);

		this->SendPrestigeTaskInfo();

		EventHandler::Instance().OnRoleCommitPrestigeTask(m_mgr->GetRole(), task_id);
	}
}

bool PrestigeTask::IsCanOneClick()
{
	return m_param.village_id <= 0 && !m_mgr->GetActiveSystem()->IsStartedActive(ACTIVE_SYSTEM_TYPE_PRESTIGE_TASK);
}

void PrestigeTask::OneClickFinishTask(unsigned int & exp, int & coin, int & jade, std::vector<ItemConfigData>& rewards)
{
	int village_id = m_mgr->GetPrestige()->GetMaxPrestigeVillageId();

	const PrestigeVillageCfg* village_cfg = LOGIC_CONFIG->GetPrestigeConfig()->GetVillageCfg(village_id);
	if (NULL == village_cfg)
	{
		m_mgr->NoticeNum(errornum::EN_VILLAGE_NOT_FOUND);
		return;
	}

	{
		const PrestigeTaskVillageCfg* task_village_cfg = TaskPool::Instance()->GetPrestigeTaskVillageCfg(village_id);
		if (NULL == task_village_cfg)
		{
			m_mgr->NoticeNum(errornum::EN_VILLAGE_NO_TASK_FOUND);
			return;
		}

		if (m_mgr->GetRole()->GetLevel() < task_village_cfg->unlock_level)
		{
			m_mgr->NoticeNum(errornum::EN_ROLE_LEVEL_NOT_ENOUTH);
			return;
		}
	}

	int task_group = TaskPool::Instance()->RandPrestigeTaskGroup(village_id);
	if (0 == task_group)
	{
		m_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	UInt16 task_list[MAX_PRESTIGE_TASK_NUM_PER_DAY] = { 0 };
	int task_num = TaskPool::Instance()->FillPrestigeTaskList(village_id, task_group, ARRAY_ITEM_COUNT(task_list), task_list);
	if (0 == task_num)
	{
		m_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	m_param.village_id = village_id;
	m_param.today_task_num = task_num;
	m_param.today_finish_task_num = 0;

	bool is_rate_2 = false;
	for (int i = 0; i < task_num && i < ARRAY_ITEM_COUNT(m_param.today_task_list); ++i)
	{
		m_param.today_task_list[i] = task_list[i];

		const PrestigeTaskCfg* task_cfg = TaskPool::Instance()->GetPrestigeTaskCfg(task_list[i]);

		// 随机星级
		if (NULL != task_cfg && task_cfg->is_activedegree)
		{
			int star = TaskPool::Instance()->GetPrestigeTaskStar(is_rate_2);
			if (star > 0)
			{
				m_param.today_star_list[i] = star;
				if (star == 5) is_rate_2 = true;
			}
		}
	}

	int add_prestige = 0;
	for (int i = 0; i < task_num; i++)
	{
		m_param.today_finish_task_num += 1;
		m_mgr->GetActiveSystem()->AddActiveDegree(ACTIVE_SYSTEM_TYPE_PRESTIGE_TASK, 1, __FUNCTION__);

		const PrestigeTaskCfg* task_cfg = TaskPool::Instance()->GetPrestigeTaskCfg(m_param.today_task_list[i]);
		if (NULL != task_cfg)
		{
			if (task_cfg->is_activedegree)
			{
				int star = m_param.today_star_list[m_param.today_finish_task_num];
				const PrestigeTaskStarCfg * cfg = TaskPool::Instance()->GetPrestigeTaskStarCfg(star);
				if (NULL != cfg)
				{
					for (size_t i = 0; i < cfg->reward_item.size(); i++)
					{
						RoleJoinActivityReward::AddRewards(rewards, cfg->reward_item[i]);
					}
				}
			}

			if (0 == task_cfg->prestige_reward_group)
				continue;
			
			int prestige_reward_num = TaskPool::Instance()->GetPrestigeTaskReward(m_mgr->GetRole()->GetLevel(), task_cfg->prestige_reward_group);
			if (prestige_reward_num > 0)
			{
				add_prestige += prestige_reward_num;
			}			
		}

		this->ClearTask(m_param.today_task_list[i]);
	}

	{
		const PrestigeTaskVillageCfg* task_village_cfg = TaskPool::Instance()->GetPrestigeTaskVillageCfg(village_id);
		if (NULL != task_village_cfg)
		{
			int prestige_reward_num = TaskPool::Instance()->GetPrestigeTaskEndReward(m_mgr->GetRole()->GetLevel(), task_village_cfg->finish_all_reward_group);
			if (prestige_reward_num > 0)
			{
				add_prestige += prestige_reward_num;
			}

			for (size_t i = 0; i < task_village_cfg->reward_num; i++)
			{
				RoleJoinActivityReward::AddRewards(rewards, task_village_cfg->reward_list[i]);
			}
		}
	}

	if (m_mgr->GetPrestige()->AddPrestige(village_id, add_prestige, __FUNCTION__))
	{
		m_param.today_total_prestige += add_prestige;

		gamelog::g_log_prestige_task.printf(LL_INFO, "PrestigeTask::OneClickFinishTask role[%d,%s] village_id[%d] prestige_num[%d]",
			m_mgr->GetRoleUID(), m_mgr->GetRoleName(), village_id, add_prestige);
	}

	this->SendAllInfo();
}

void PrestigeTask::SendAllInfo()
{
	this->SendPrestigeTaskInfo();
}

void PrestigeTask::IssuePrestigeTaskReward(int task_id)
{
	const PrestigeTaskCfg* task_cfg = TaskPool::Instance()->GetPrestigeTaskCfg(task_id);
	if (NULL != task_cfg)
	{
		if (task_cfg->is_activedegree)
		{
			int star = m_param.today_star_list[m_param.today_finish_task_num];
			const PrestigeTaskStarCfg * cfg = TaskPool::Instance()->GetPrestigeTaskStarCfg(star);
			if (NULL != cfg)
			{
				m_mgr->GetKnapsack()->PutVec(cfg->reward_item, GET_REASON_PRESTIGE_TASK);
			}
		}

		if (0 == task_cfg->prestige_reward_group)
		{
			gamelog::g_log_prestige_task.printf(LL_INFO, "PrestigeTask::IssuePrestigeTaskReward TASK_NO_REWARD, role[%d,%s] task_id[%d]",
				m_mgr->GetRoleUID(), m_mgr->GetRoleName(), task_id);
			
			return;
		}

		int prestige_reward_num = TaskPool::Instance()->GetPrestigeTaskReward(m_mgr->GetRole()->GetLevel(), task_cfg->prestige_reward_group);
		if (prestige_reward_num > 0)
		{
			if (m_mgr->GetPrestige()->AddPrestige(task_cfg->village_id, prestige_reward_num, __FUNCTION__))
			{
				m_param.today_total_prestige += prestige_reward_num;

				gamelog::g_log_prestige_task.printf(LL_INFO, "PrestigeTask::IssuePrestigeTaskReward role[%d,%s] task_id[%d] village_id[%d] prestige_num[%d]",
					m_mgr->GetRoleUID(), m_mgr->GetRoleName(), task_id, task_cfg->village_id, prestige_reward_num);
			}
			else
			{
				gamelog::g_log_prestige_task.printf(LL_INFO, "PrestigeTask::IssuePrestigeTaskReward FAILED!!!!! role[%d,%s] task_id[%d] village_id[%d] prestige_num[%d]",
					m_mgr->GetRoleUID(), m_mgr->GetRoleName(), task_id, task_cfg->village_id, prestige_reward_num);
			}
			
		}
		else
		{
			gamelog::g_log_prestige_task.printf(LL_WARNING, "PrestigeTask::IssuePrestigeTaskReward PRESTIGE_NUM_ERROR, role[%d,%s] task_id[%d] village_id[%d] prestige_num[%d]",
				m_mgr->GetRoleUID(), m_mgr->GetRoleName(), task_id, task_cfg->village_id, prestige_reward_num);
		}
	}
	else
	{
		gamelog::g_log_prestige_task.printf(LL_WARNING, "PrestigeTask::IssuePrestigeTaskReward TASK_NOT_FOUND_ERROR, role[%d,%s] task_id[%d]",
			m_mgr->GetRoleUID(), m_mgr->GetRoleName(), task_id);
	}
}

void PrestigeTask::IssuePrestigeTaskEndReward(int village_id)
{
	const PrestigeTaskVillageCfg* village_cfg = TaskPool::Instance()->GetPrestigeTaskVillageCfg(village_id);
	if (NULL != village_cfg)
	{
		int prestige_reward_num = TaskPool::Instance()->GetPrestigeTaskEndReward(m_mgr->GetRole()->GetLevel(), village_cfg->finish_all_reward_group);
		if (prestige_reward_num > 0)
		{
			if (m_mgr->GetPrestige()->AddPrestige(village_id, prestige_reward_num, __FUNCTION__))
			{
				m_param.today_total_prestige += prestige_reward_num;

				gamelog::g_log_prestige_task.printf(LL_INFO, "PrestigeTask::IssuePrestigeTaskEndReward role[%d,%s] village_id[%d] prestige_num[%d]",
					m_mgr->GetRoleUID(), m_mgr->GetRoleName(), village_id, prestige_reward_num);
			}
			else
			{
				gamelog::g_log_prestige_task.printf(LL_INFO, "PrestigeTask::IssuePrestigeTaskEndReward FAILED!!!!! role[%d,%s] village_id[%d] prestige_num[%d]",
					m_mgr->GetRoleUID(), m_mgr->GetRoleName(), village_id, prestige_reward_num);
			}
		}
		else
		{
			gamelog::g_log_prestige_task.printf(LL_WARNING, "PrestigeTask::IssuePrestigeTaskEndReward PRESTIGE_NUM_ERROR, role[%d,%s] village_id[%d] prestige_num[%d]",
				m_mgr->GetRoleUID(), m_mgr->GetRoleName(), village_id, prestige_reward_num);
		}

		if (!m_mgr->GetKnapsack()->PutList(village_cfg->reward_num, village_cfg->reward_list, PUT_REASON_PRESTIGE_TASK))
		{
			m_mgr->GetKnapsack()->SendMail(village_cfg->reward_list, village_cfg->reward_num);
		}
	}
	else
	{
		gamelog::g_log_prestige_task.printf(LL_WARNING, "PrestigeTask::IssuePrestigeTaskEndReward VILLAGE_NOT_FOUND_ERROR, role[%d,%s] village[%d]",
			m_mgr->GetRoleUID(), m_mgr->GetRoleName(), village_id);
	}
}

void PrestigeTask::SendPrestigeTaskInfo()
{
	Protocol::SCPrestigeTaskInfo info;
	info.village_id = m_param.village_id;
	info.is_accept_task = m_param.cur_task_id == 0 ? 0 : 1;
	info.today_finish_task_num = m_param.today_finish_task_num;
	info.today_task_num = m_param.today_task_num;
	for (int i = 0; i < MAX_PRESTIGE_TASK_NUM_PER_DAY; ++i)
	{
		info.today_task_list[i].task_id = m_param.today_task_list[i];
		info.today_task_list[i].star = m_param.today_star_list[i];
	}

	int len = (int)sizeof(info) - (int)sizeof(info.today_task_list) + info.today_task_num * (int)sizeof(info.today_task_list[0]);
	m_mgr->NetSend(&info, len);
}

bool PrestigeTask::AcceptPrestigeTask(int task_id)
{
	const TaskInfomaConfig* cfg = TASKPOOL->GetTaskCfgByID(task_id);
	if (NULL == cfg || cfg->task_type != TASK_TYPE_PRESTIGE_TASK) return false;

	// 确保这个任务没有被执行过或正在执行中
	this->ClearTask(task_id);

	return m_mgr->GetTaskManager()->OnAcceptTask(task_id);
}

void PrestigeTask::ClearTask(int task_id)
{
	if (m_mgr->GetTaskRecorder()->HaveFinished(task_id))
	{
		m_mgr->GetTaskRecorder()->TaskRecordRemove(task_id);
	}
	if (m_mgr->GetTaskManager()->CheckRoleHasTask(task_id))
	{
		m_mgr->GetTaskManager()->EraseTask(task_id);
	}
}

void PrestigeTask::SendLastNotice()
{
	Protocol::SCPrestigeTaskFinalNotice notice;
	notice.village_id = m_param.village_id;
	notice.total_add_prestige = m_param.today_total_prestige;

	m_mgr->NetSend(&notice, sizeof(notice));
}

