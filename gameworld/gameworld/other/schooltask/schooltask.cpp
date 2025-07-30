#include "schooltask.h"
#include <algorithm>
#include "protocol/msgschooltask.h"
#include "other/rolemodulemanager.hpp"
#include "other/rolegatherfb/rolegatherfb.hpp"
#include "other/event/eventhandler.hpp"
#include "config/logicconfigmanager.hpp"
#include "schooltaskconfig.hpp"
#include "obj/character/role.h"
#include "item/knapsack.h"
#include "item/money.h"
#include "servercommon/errornum.h"
#include "other/funopen/funopen.hpp"
#include "other/funopen/funopenconfig.hpp"
#include "global/team/team.hpp"
#include "global/worldstatus/worldstatus.hpp"
#include "servercommon/taskdef.h"
#include "scene/scene.h"
#include "other/prestige/prestige.hpp"
#include "other/dayreward/dayrewardconfig.hpp"
#include "other/levelcomplement/levelcomplement.h"
#include "gamelog.h"

SchoolTask::SchoolTask()
{
}

SchoolTask::~SchoolTask()
{
}

void SchoolTask::Init(RoleModuleManager * mgr, const SchoolTaskParam & param)
{
	m_role_module_mgr = mgr;
	m_param = param;

	this->SetIndexByMonster();
}

void SchoolTask::GetInitParam(SchoolTaskParam * param)
{
	*param = m_param;
}

void SchoolTask::OnResetModuleData(unsigned int old_dayid, unsigned int now_dayid)
{
}

void SchoolTask::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	if (old_dayid == now_dayid) return;
	this->OnFindDataReset();

	if (!FunOpen::IsFunOpen(m_role_module_mgr->GetRole(), FUN_TYPE_SCHOOL_TASK)) return;

	this->RefreshTaskList(m_role_module_mgr->GetRole()->GetLevel());
}

void SchoolTask::OnFindDataReset()
{
	const DayRewardFindCfg *cfg = LOGIC_CONFIG->GetDayRewardConfig()->GetFindReward(DAY_ACTIVITY_TYPE_SCHOOL_TASK);
	if (NULL == cfg || m_role_module_mgr->GetRole()->GetLevel() < cfg->find_level) return;

	const SchoolTaskOtherCfg & other_cfg = LOGIC_CONFIG->GetSchoolTaskConfig()->GetOtherCfg();
	m_param.reward_find_count = other_cfg.daily_num;

	if (m_role_module_mgr->GetCommonData().offine_day < 2)
	{
		for (int i = 0; i < SCHOOL_TASK_NUM; ++i)
		{
			if (m_param.task_status[i] == SCHOOL_TASK_STATUS_FINISH)
			{
				--m_param.reward_find_count;
			}
		}
	}
}

bool SchoolTask::OnKillMonster(int monster_id)
{
	std::map<int, int>::const_iterator it = m_index_by_monster.find(monster_id);
	if (it == m_index_by_monster.end() || it->second < 0 || it->second >= SCHOOL_TASK_NUM || SCHOOL_TASK_STATUS_ONGOING != m_param.task_status[it->second]) return false;

	++m_param.task_list[it->second].progress;
	
	const SchoolTaskCfg * cfg = LOGIC_CONFIG->GetSchoolTaskConfig()->GetTaskCfg(m_param.task_list[it->second].task_id);
	if (NULL != cfg && m_param.task_list[it->second].progress >= cfg->c_param2)
	{
		this->FinishTask(it->second);
	}
	else
	{
		this->SendSingleInfo(it->second);
	}
	return true;
}

void SchoolTask::OnRoleEnterTeam()
{
	for (int i = 0; i < SCHOOL_TASK_NUM; ++i)
	{
		this->SyncTeamLeaderTask(i);
	}	
}

void SchoolTask::OnTeamLeaderChange()
{
	for (int i = 0; i < SCHOOL_TASK_NUM; ++i)
	{
		this->SyncTeamLeaderTask(i);
	}
}

void SchoolTask::OnTaskReq(int index)
{
	if (index < 0 || index >= SCHOOL_TASK_NUM) return;

	SchoolTaskData & task_data = m_param.task_list[index];
	if (0 == task_data.task_id) return;

	switch (m_param.task_status[index])
	{
	case SCHOOL_TASK_STATUS_DEFAULT:
		{
			m_param.task_status[index] = SCHOOL_TASK_STATUS_ONGOING;
			this->SendSingleInfo(index, true);
			this->OnAcceptTask(index);
		}
		break;

	case SCHOOL_TASK_STATUS_ONGOING:
		{
			const SchoolTaskCfg * cfg = LOGIC_CONFIG->GetSchoolTaskConfig()->GetTaskCfg(task_data.task_id);
			if (NULL != cfg && task_data.progress >= cfg->c_param2)
			{
				this->FinishTask(index);
			}
		}
		break;

	case SCHOOL_TASK_STATUS_FINISH:
		{

		}
		break;
	}

	WorldStatus::Instance().LogFunctionStats(m_role_module_mgr->GetRole(), "SchoolTask");
}

void SchoolTask::OnQuickFinish(int index)
{
	if (index < 0 || index >= SCHOOL_TASK_NUM) return;

	if (m_param.task_status[index] == SCHOOL_TASK_STATUS_FINISH)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_TASK_ALREADY_FINISHED);
		return;
	}

	int need_gold = LOGIC_CONFIG->GetSchoolTaskConfig()->GetOtherCfg().quick_finish;
	if (!m_role_module_mgr->GetMoney()->UseGold(need_gold, __FUNCTION__))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_GOLD_NOT_ENOUGH);
		return;
	}

	this->FinishTask(index);
	WorldStatus::Instance().LogFunctionStats(m_role_module_mgr->GetRole(), "SchoolTask");
}

void SchoolTask::OnLevelUp(int cur_level)
{
	const FunOpenCfg* cfg = LOGIC_CONFIG->GetFunOpenConfig()->GetFunOpenCfg(FUN_TYPE_SCHOOL_TASK);
	if (NULL != cfg && cur_level == cfg->limit_level)
	{
		this->RefreshTaskList(cur_level);
	}
}

bool SchoolTask::OnSchoolTaskFindReward(int consume_type, int count)
{
	if (count > m_param.reward_find_count)
	{
		m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_FIND_COUNT_NOT_ENOUGH);
		return false;
	}

	int exp = 0, coin = 0;
	std::vector<ItemConfigData> item_list;
	for (int i = 0; i < count; ++i)
	{
		int temp_exp = 0;
		int temp_coin = 0;

		if (LOGIC_CONFIG->GetSchoolTaskConfig()->GetFindReward(m_role_module_mgr->GetRole()->GetLevel(), consume_type, &temp_coin, &temp_exp,&item_list))
		{
			if (temp_exp > 0)exp += temp_exp;
			if (temp_coin > 0)coin += temp_coin;
		}
		else
		{
			m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_FIND_REWARD_ERROR);
			return false;
		}
	}
	if (!item_list.empty())
	{
		if (!m_role_module_mgr->GetKnapsack()->CheckForPutList((short)item_list.size(), &item_list[0]))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
			return false;
		}
		if (!m_role_module_mgr->GetKnapsack()->PutList((short)item_list.size(), &item_list[0], PUT_REASON_FIND_REWARD))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
			return false;
		}
	}
	m_role_module_mgr->GetMoney()->AddCoinBind(coin, __FUNCTION__);
	long long real_add_exp = m_role_module_mgr->GetRole()->AddExp(exp, ADD_EXP_TYPE_FIND_REWARD, __FUNCTION__);
	m_role_module_mgr->GetLevelComplement()->OnAddExp(real_add_exp);

	m_param.reward_find_count -= count;
	m_role_module_mgr->GetCommonData().today_activity_type_find_count[DAY_ACTIVITY_TYPE_SCHOOL_TASK] += count;
	m_role_module_mgr->GetCommonData().today_find_reward_flag.SetBit(DAY_ACTIVITY_TYPE_SCHOOL_TASK);
	return true;
}

//void SchoolTask::OneKeyFinish(int _sonsume_money, int _money_type)
//{
//	gamelog::g_log_login.printf(LL_INFO, "today finish count : ", this->GetFinishCount());
//
//	const SchoolTaskOtherCfg st_cfg = LOGIC_CONFIG->GetSchoolTaskConfig()->GetOtherCfg();
//	if (this->GetFinishCount() == st_cfg.daily_num) return;
//
//	switch (_money_type)
//	{
//	case MONEY_TYPE_COIN_BIND:
//		{
//			if (!m_role_module_mgr->GetMoney()->CoinBindMoreThan(_sonsume_money))
//			{
//				m_role_module_mgr->NoticeNum(errornum::EN_COIN_NOT_ENOUGH);
//				return;
//			}
//			m_role_module_mgr->GetMoney()->UseCoinBind(_sonsume_money, __FUNCTION__);
//		}
//		break;
//	case MONEY_TYPE_GOLD:
//		{
//			if (!m_role_module_mgr->GetMoney()->GoldMoreThan(_sonsume_money))
//			{
//				m_role_module_mgr->NoticeNum(errornum::EN_GOLD_NOT_ENOUGH);
//				return;
//			}
//			m_role_module_mgr->GetMoney()->UseGold(_sonsume_money, __FUNCTION__);
//		}
//		break;
//	default:
//		return;
//		break;
//	}
//	for (int i=0;i<SCHOOL_TASK_NUM;++i)
//	{
//		if (SCHOOL_TASK_STATUS_FINISH == m_param.task_status[i])
//		{
//			continue;
//		}
//		this->OnQuickFinish(i);
//	}
//
//	gamelog::g_log_login.printf(LL_INFO, "today finish count : ", this->GetFinishCount());
////}

void SchoolTask::SetIndexByMonster()
{
	m_index_by_monster.clear();
	for (int i = 0; i < SCHOOL_TASK_NUM; ++i)
	{
		if (0 == m_param.task_list[i].task_id) continue;

		const SchoolTaskCfg * cfg = LOGIC_CONFIG->GetSchoolTaskConfig()->GetTaskCfg(m_param.task_list[i].task_id);
		if (NULL == cfg) continue;

		int monster_id = cfg->c_param1;
		m_index_by_monster[monster_id] = i;
	}
}

void SchoolTask::RefreshTaskList(int level)
{
	const FunOpenCfg* cfg = LOGIC_CONFIG->GetFunOpenConfig()->GetFunOpenCfg(FUN_TYPE_SCHOOL_TASK);
	if (NULL != cfg && level < cfg->limit_level) return;

	UNSTD_STATIC_CHECK(0 == SCHOOL_TASK_STATUS_DEFAULT);
	memset(m_param.task_status, SCHOOL_TASK_STATUS_DEFAULT, sizeof(m_param.task_status));	// 把任务清理掉

	static int task_list[SCHOOL_TASK_NUM] = { 0 };
	memset(task_list, 0, sizeof(task_list));
	const SchoolTaskOtherCfg & st_cfg = LOGIC_CONFIG->GetSchoolTaskConfig()->GetOtherCfg();
	int task_count = st_cfg.daily_num;
	WorldStatus::Instance().GetServerSchoolTask(task_list, task_count);

	for (int i = 0; i < SCHOOL_TASK_NUM && i < task_count; ++i)
	{
		m_param.task_list[i].task_id = task_list[i];
		m_param.task_list[i].progress = 0;
	}

	this->SetIndexByMonster();
	this->SendAllInfo();
}

void SchoolTask::SendAllInfo()
{
	Protocol::SCSchoolTaskAllInfo info;
	for (int i = 0; i < SCHOOL_TASK_NUM; ++i)
	{
		info.task_status[i] = m_param.task_status[i];
		info.task_list[i] = m_param.task_list[i].task_id;
		info.task_progress_list[i] = m_param.task_list[i].progress;
	}

	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void SchoolTask::SendSingleInfo(int task_index, bool is_new)
{
	if (task_index < 0 || task_index >= SCHOOL_TASK_NUM) return;

	Protocol::SCSchoolTaskSingleInfo info;
	info.task_status = m_param.task_status[task_index];
	info.task_list = m_param.task_list[task_index].task_id;
	info.task_progress_list = m_param.task_list[task_index].progress;
	info.is_new = is_new ? 1 : 0;

	m_role_module_mgr->NetSend(&info, sizeof(info));
}

int SchoolTask::GetFinishCount()
{
	int count = 0;
	const SchoolTaskOtherCfg & st_other = LOGIC_CONFIG->GetSchoolTaskConfig()->GetOtherCfg();
	if(&st_other == NULL) return count;

	for (int idx = 0; idx < st_other.daily_num && idx < SCHOOL_TASK_NUM; ++idx)
	{
		if (SCHOOL_TASK_STATUS_FINISH == m_param.task_status[idx])
		{
			count++;
		}
	}
	return count;
}

void SchoolTask::GmSchoolTaskOrder(int op_type)
{
	enum SchoolTaskOrder
	{
		SCHOOL_TASK_REFRESH_NOW = 0,		// 立刻执行每日刷新
	};

	switch (op_type)
	{
	case SCHOOL_TASK_REFRESH_NOW:
	//	this->OnResetModuleData(0, 1);
		this->OnDayChange(0, 1);
		break;
	default:
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		break;
	}
}

bool SchoolTask::IsInTaskScene()
{
	int cur_scene_id = m_role_module_mgr->GetRole()->GetScene()->GetSceneID();
	for (int i = 0; i < SCHOOL_TASK_NUM; ++i)
	{
		const SchoolTaskCfg * cfg = LOGIC_CONFIG->GetSchoolTaskConfig()->GetTaskCfg(m_param.task_list[i].task_id);
		if (NULL == cfg) continue;

		if (cur_scene_id == cfg->target_scene) return true;
	}

	return false;
}

void SchoolTask::FinishTask(int index)
{
	if (index < 0 || index >= SCHOOL_TASK_NUM) return;

	if (m_param.task_status[index] == SCHOOL_TASK_STATUS_FINISH)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_TASK_ALREADY_FINISHED);
		return;
	}
	const SchoolTaskCfg * cfg = LOGIC_CONFIG->GetSchoolTaskConfig()->GetTaskCfg(m_param.task_list[index].task_id);
	if (NULL == cfg) return;

	const SchoolTaskLevelRewardCfg* level_cfg = LOGIC_CONFIG->GetSchoolTaskConfig()->GetLevelReward(m_role_module_mgr->GetRole()->GetLevel());
	if (NULL == level_cfg) return;

	m_role_module_mgr->GetMoney()->AddCoinBind(level_cfg->coin, __FUNCTION__);
	long long real_add_exp = m_role_module_mgr->GetRole()->AddExp(level_cfg->exp, ADD_EXP_TYPE_SCHOOL_TASK, __FUNCTION__);
	m_role_module_mgr->GetLevelComplement()->OnAddExp(real_add_exp);
	EventHandler::Instance().OnFinishLevelComplementRelatedFun(m_role_module_mgr->GetRole(), level_cfg->exp, __FUNCTION__);
	m_role_module_mgr->GetPrestige()->AddPrestige(cfg->reputation_city, cfg->reputation_num, __FUNCTION__);

	for (int i = 0; i < level_cfg->reward_group_cnt; ++i)
	{
		const ItemConfigData * reward = LOGIC_CONFIG->GetSchoolTaskConfig()->RandItemReward(level_cfg->reward_group_list[i]);
		if (NULL != reward) m_role_module_mgr->GetKnapsack()->Put(*reward, PUT_REASON_SCHOOL_TASK);
	}
	
	m_param.task_status[index] = SCHOOL_TASK_STATUS_FINISH;
	this->SendSingleInfo(index);
	EventHandler::Instance().OnCommitSchoolTask(m_role_module_mgr->GetRole(), index);

	if (this->CheckIsAllFinish())
	{
		const SchoolTaskOtherCfg & st_other = LOGIC_CONFIG->GetSchoolTaskConfig()->GetOtherCfg();
		m_role_module_mgr->GetRoleGatherFb()->AddGatherPower(st_other.power_reward);
	}

	if (TASK_NEED_KILL_MONSTERS == cfg->condition)
	{
		m_role_module_mgr->RemoveNeedMonster(cfg->c_param1);
	}
	else if (TASK_NEED_KILL_MONGROUP == cfg->condition || TASK_NEED_KILL_IN_ORDER == cfg->condition)
	{
#ifdef _DEBUG
		assert(0); // 只有加了怪物组配置才可以对门派任务加怪物组任务条件
#endif
	}
	else if (TASK_NEED_GAIN_TASKPROP == cfg->condition)
	{
#ifdef _DEBUG
		assert(0); // 只有加了掉落组配置才可以对门派任务加掉落组任务条件
#endif
	}
}

bool SchoolTask::CheckIsAllFinish()
{
	const SchoolTaskOtherCfg & st_other = LOGIC_CONFIG->GetSchoolTaskConfig()->GetOtherCfg();

	for (int idx = 0; idx < st_other.daily_num && idx < SCHOOL_TASK_NUM; ++idx)
	{
		if (SCHOOL_TASK_STATUS_FINISH != m_param.task_status[idx]) return false;
	}

	return true;
}

void SchoolTask::SyncTeamLeaderTask(int index)
{
	if (index < 0 || index >= SCHOOL_TASK_NUM) return;

	Team* team = m_role_module_mgr->GetRole()->GetMyTeam();
	if (NULL == team) return;

	Role* leader_role = team->GetLeaderRole();
	if (NULL == leader_role || leader_role->GetUID() == m_role_module_mgr->GetRole()->GetUID()) return;
	if (NULL == leader_role->GetRoleModuleManager()) return;

	if (SCHOOL_TASK_STATUS_DEFAULT == m_param.task_status[index] &&
		SCHOOL_TASK_STATUS_ONGOING == leader_role->GetRoleModuleManager()->GetSchoolTask()->GetTaskStatus(index))
	{
		m_param.task_status[index] = SCHOOL_TASK_STATUS_ONGOING;
		this->SendSingleInfo(index, true);

		const SchoolTaskCfg * cfg = LOGIC_CONFIG->GetSchoolTaskConfig()->GetTaskCfg(m_param.task_list[index].task_id);
		if (NULL == cfg) return;

		if (TASK_NEED_KILL_MONSTERS == cfg->condition)
		{
			m_role_module_mgr->AddNeedMonster(cfg->c_param1);
		}
		else if (TASK_NEED_KILL_MONGROUP == cfg->condition || TASK_NEED_KILL_IN_ORDER == cfg->condition)
		{
#ifdef _DEBUG
			assert(0); // 只有加了怪物组配置才可以对门派任务加怪物组任务条件
#endif
		}
		else if (TASK_NEED_GAIN_TASKPROP == cfg->condition)
		{
#ifdef _DEBUG
			assert(0); // 只有加了掉落组配置才可以对门派任务加掉落组任务条件
#endif
		}
	}
}

void SchoolTask::OnAcceptTask(int index)
{
	Role* role = m_role_module_mgr->GetRole();
	Team* team = role->GetMyTeam();
	if (NULL != team && team->GetLeaderUID() == role->GetUID())
	{
		int member_count = team->GetMemberCount();
		for (int i = 0; i < member_count; ++i)
		{
			Role* member_role = team->GetMemberRoleByIndex(i);
			if (NULL != member_role && member_role->GetUID() != role->GetUID())
			{
				member_role->GetRoleModuleManager()->GetSchoolTask()->SyncTeamLeaderTask(index);
			}
		}
	}

	const SchoolTaskCfg * cfg = LOGIC_CONFIG->GetSchoolTaskConfig()->GetTaskCfg(m_param.task_list[index].task_id);
	if (NULL == cfg) return;

	if (TASK_NEED_KILL_MONSTERS == cfg->condition)
	{
		m_role_module_mgr->AddNeedMonster(cfg->c_param1);
	}
	else if (TASK_NEED_KILL_MONGROUP == cfg->condition || TASK_NEED_KILL_IN_ORDER == cfg->condition)
	{
#ifdef _DEBUG
		assert(0); // 只有加了怪物组配置才可以对门派任务加怪物组任务条件
#endif
	}
	else if (TASK_NEED_GAIN_TASKPROP == cfg->condition)
	{
#ifdef _DEBUG
		assert(0); // 只有加了掉落组配置才可以对门派任务加掉落组任务条件
#endif
	}
}

int SchoolTask::GetTaskStatus(int index)
{
	return m_param.task_status[index];
}
