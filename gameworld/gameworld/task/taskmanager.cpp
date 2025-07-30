#include "taskmanager.h"
#include "taskrecorder.h"
#include "gamelog.h"
#include "protocol/msgtask.h"
#include "obj/character/role.h"
#include "servercommon/errornum.h"
#include "item/knapsack.h"
#include "item/itemextern.hpp"
#include "scene/fbmanager.hpp"
#include "global/team/team.hpp"
#include "global/team/teammanager.hpp"
#include "gameworld/npc/npcmanager.h"
#include "other/rolemodulemanager.hpp"
#include "other/partner/partner.hpp"
#include "other/prestige/prestige.hpp"
#include "other/pet/pet.hpp"
#include "other/event/eventhandler.hpp"
#include "drop/droppool.hpp"
#include "protocol/msgitem.h"
#include "servercommon/noticenum.h"
#include "servercommon/string/gameworldstr.h"
#include "servercommon/userprotocal/msgsystem.h"
#include "gameworld/gameworld/scene/scenemanager.hpp"
#include "config/logicconfigmanager.hpp"
#include "config/otherconfig/sundryconfig.hpp"
#include "other/route/mailroute.hpp"
#include "other/advancefb/advancefbconfig.hpp"
#include "other/funopen/funopenconfig.hpp"
#include "other/bountytask/bountytask.hpp"
#include "other/funopen/funopen.hpp"
#include "other/escort/escort.hpp"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "other/couragechallenge/couragechallenge.h"
#include "other/levelcomplement/levelcomplement.h"
#include "other/dreamnoteschallenge/dreamnoteschallenge.hpp"
#include "other/rolecrystalfb/rolecrystalfb.h"
#include "other/rolematerial/rolematerial.hpp"
#include "other/lunhuiwangchuan/lunhuiwangchuan.hpp"
#include "other/miyi/rolemiyi.hpp"
#include "other/fabao/fabao.hpp"
#include "other/collection/collection.hpp"
#include "other/yinyangeye/yinyangeye.hpp"
#include "other/realmbreak/realmbreak.hpp"
#include "config/otherconfig/realmbreakconfig.hpp"
#include "other/advancecareer/advancecareer.hpp"
#include "other/flyupsystem/flyupsystem.hpp"
#include "other/pet/pet.hpp"
#include "other/anecdotetask/anecdotetask.hpp"
#include "other/wanlingfb/wanlingfb.hpp"
#include "other/shengqi/holyequipmentconfig.hpp"
#include "other/roleguildtask/roleguildtask.hpp"

TaskManager::TaskManager() : m_mgr(NULL), task_expire_check_time(0)
{
}

TaskManager::~TaskManager()
{
}

void TaskManager::Init(RoleModuleManager * role_module_manager, const TaskParam & task_param, const TaskTraceParam & task_trace_param, const TaskWenXiangLouFbParam & task_wenxianglou_param)
{
	m_mgr = role_module_manager;
	m_task_param = task_param;
	m_trace_param = task_trace_param;
	m_wenxianglou_param = task_wenxianglou_param;

	// 检查任务版本，把版本修改的任务删掉；
	for (int idx = 0; idx < m_task_param.task_count;)
	{
		TaskInfo & task_check = m_task_param.task_info_list[idx];
		const TaskInfomaConfig * task_cfg = TASKPOOL->GetTaskCfgByID(task_check.task_id);
		if (NULL == task_cfg || task_cfg->task_ver != task_check.versions)	// 删掉任务
		{
			m_task_param.RemoveTask(task_check.task_id);
			if ((NULL != task_cfg && TASK_TYPE_PARTNER_SOAR_TASK == task_cfg->task_type) || task_check.task_id == m_task_param.cur_partner_soar_task_id)
			{
				m_task_param.partner_soar_task_status = PARTNER_SOAR_TASK_STATUS_BEGIN;
				m_task_param.cur_partner_soar_task_id = 0;
				m_task_param.is_cur_accept_partner_soar_task = 0;
			}
		}
		else if (TASK_TYPE_ANECDOTE_TASK == task_cfg->task_type)
		{
			m_task_param.RemoveTask(task_cfg->task_id);
		}
		else
		{
			switch (task_cfg->task_condition)
			{
			case TASK_NEED_KILL_MONSTERS:
				{
					m_mgr->AddNeedMonster(task_cfg->param1);
				}
				break;
			case TASK_NEED_KILL_MONGROUP:
				{
					const std::vector<int>* monster_list = TASKPOOL->GetMonsterGroupMonsterList(task_cfg->param1);
					if (NULL != monster_list)
					{
						for (size_t k = 0; k < monster_list->size(); ++k)
						{
							m_mgr->AddNeedMonster((*monster_list)[k]);
						}
					}
				}
				break;
			case TASK_NEED_GAIN_TASKPROP:
				{
					m_mgr->AddNeedMonster(task_cfg->param1);
				}
				break;
			}

			if (task_cfg->can_auto_commit)
			{
				this->CheckAutoCommit(task_cfg->task_id, task_cfg->commit_npc);
			}
			else
			{
				this->CanCommitTask(task_cfg->task_id); // 调用一次以获取m_can_commit_set的数据
			}

			++idx;
		}
	}
}

void TaskManager::GetTaskInitParam(TaskParam * param, TaskTraceParam * trace_param, TaskWenXiangLouFbParam * task_wenxianglou_param)
{
	*param = m_task_param;
	*trace_param = m_trace_param;
	*task_wenxianglou_param = m_wenxianglou_param;
}

void TaskManager::InitEnd()
{
	if (0 == m_task_param.is_old_user_partner_upgrade_task_buy_flag)
	{
		m_mgr->GetPrestige()->OnCheckPartnerUpgradeTask();
		m_task_param.is_old_user_partner_upgrade_task_buy_flag = 1;
	}

	this->FinishAllPreTaskForMainQuest(); // 必须保证TaskRecorder的Init在TaskManager的Init之前

	this->CalcAcceptableTask(ACB_RECALC_INITIALIZES);
	this->SendSummaryTaskList();

	this->CalcWaitingMapOnInit();
	this->SendWaitingTaskList();
}

void TaskManager::FinishAllPreTaskForMainQuest()
{
	TaskID task_id = this->GetCurMainTaskID();
	if (0 == task_id) return;

	const TaskInfomaConfig* task_cfg = TASKPOOL->GetTaskCfgByID(task_id);
	if (NULL == task_cfg || TASK_TYPE_MAIN_QUEST != task_cfg->task_type) return;

	TaskID max_finished_task_id = m_mgr->GetTaskRecorder()->GetMaxFinishedMainTaskID();
	task_cfg = TASKPOOL->GetTaskCfgByID(max_finished_task_id);
	if (NULL == task_cfg || TASK_TYPE_MAIN_QUEST != task_cfg->task_type) return;

	task_id = GetMax(task_id, max_finished_task_id);
	gamelog::g_log_task.printf(LL_INFO, "TaskManager::FinishAllPreTaskForMainQuest role[%d,%s] call SetAllPreTaskFinish(task_id=%d)",
		m_mgr->GetUid(), m_mgr->GetRoleName(), task_id);

	this->SetAllPreTaskFinish(task_id);
}

void TaskManager::SetAllPreTaskFinish(TaskID task_id)
{
	const TaskInfomaConfig* task_cfg = TASKPOOL->GetTaskCfgByID(task_id);
	if (NULL == task_cfg) return;

	while (task_cfg->pre_task_id > 0)
	{
		TaskID pre_task = task_cfg->pre_task_id;

		const TaskInfomaConfig* pre_task_cfg = TASKPOOL->GetTaskCfgByID(pre_task);
		if (NULL == pre_task_cfg) break;

		if (!m_mgr->GetTaskRecorder()->HaveFinished(pre_task))
		{
			m_task_param.RemoveTask(pre_task);
			m_mgr->GetTaskRecorder()->TaskComplete(pre_task);
			gamelog::g_log_task.printf(LL_INFO, "TaskManager::SetAllPreTaskFinish role[%d,%s] complete_task[%d]",
				m_mgr->GetUid(), m_mgr->GetRoleName(), pre_task);
		}

		task_cfg = pre_task_cfg;
	}
}

void TaskManager::GmResetTaskImpl(TaskID task_id)
{
	const TaskInfomaConfig* cur_task = TASKPOOL->GetTaskCfgByID(task_id);
	if (NULL == cur_task) return;

	this->GmResetTaskImpl(cur_task->nex_task_id);

	if (!m_mgr->GetTaskRecorder()->HaveFinished(task_id)) return;

	m_mgr->GetTaskRecorder()->TaskRecordRemove(task_id);

	m_task_param.RemoveTask(task_id);
	this->SendSingleTaskDelete(task_id, TASK_REMOVE_REASON_OTHERS);			// 移除任务

	gamelog::g_log_debug.printf(LL_INFO, "TaskManager::GmResetTaskImpl role[%d,%s] task_id[%d]", m_mgr->GetUid(), m_mgr->GetRoleName(), task_id);
}

void TaskManager::ExecuteATaskForMeetRequireCondition(int condition, int p1, int p2, const TaskInfomaConfig * task_cfg, TaskInfo& p_task)
{
	UNSTD_STATIC_CHECK(46 == TASK_NEED_MAX);
	UNSTD_STATIC_CHECK(26 == TASK_MEET_TYPE_MAX);
	switch (task_cfg->param1)
	{
	case TASK_MEET_PROF_PROMOTE:	// p2 = prof
		{
			int up_times = p2 % 100;		// 进阶次数
			if (up_times >= task_cfg->param2) p_task.prog_num = 1;
		}
		break;

	case TASK_MEET_HAVE_PARTNER:	// p2 -> partner_id
		if (p2 == task_cfg->param2 && m_mgr->GetPartner()->IsPartnerAlreadyActive(p2)) p_task.prog_num = 1;
		break;

	case TASK_MEET_LEARN_SKILLS:	// p2 -> unuse
		p_task.prog_num = 1;
		break;

	case TASK_MEET_JOIN_BATTLES:	// p2 = group_id
	case TASK_MEET_OPEN_A_CHEST:	// p2 = chest_id
		if (p2 == task_cfg->param2) p_task.prog_num = 1;
		break;

	case TASK_MEET_OWN_CREATURE:	// p2 -> pet_id
		if (p2 == task_cfg->param2 && m_mgr->GetPet()->IsHavePet(p2)) p_task.prog_num = 1;
		break;
	case TASK_MEET_PLAY_ONCE_COLOSSEUM:	// p2 -> unuse
		{
			p_task.prog_num = 1;
		}
		break;
	case TASK_MEET_TRANSACTION_TASK: //p2 -> unuse
		{
			p_task.prog_num = 1;
		}
		break;

	case TASK_MEET_MEDAL_TASK_PLAY_VIDEO: //p2 -> unuse
		{
			p_task.prog_num = 1;
		}
		break;
	case TASK_MEET_PET_RELEASE: //p2 -> unuse
		{
			p_task.prog_num = 1;
		}
		break;
	case TASK_MEET_PET_COMPOSE: //p2 -> unuse
		{
			p_task.prog_num = 1;
		}
		break;
	case TASK_MEET_DREAM_NOTES_CHALLENGE:		//p2 -> pass_level
		{
			p_task.prog_num = m_mgr->GetDreamNotesChallenge()->GetPassLevel();
		}
		break;
	case TASK_MEET_BOUNTY_TASK:
		{
			p_task.prog_num += p2;
		}
		break;
	case TASK_MEET_RESEARCH_TASK:
		{
			p_task.prog_num += p2;
		}
		break;
	case TASK_MEET_PET_MONOPOLY_CATCH_PET: //p2 -> times
		{
			p_task.prog_num += p2;
		}
		break;
	case TASK_MEET_LUN_HUI_WANG_CHUAN:
		{
			if (m_mgr->GetLunHuiWangChuan()->GetPassIDX() >= task_cfg->param2)
			{
				p_task.prog_num = 1;
			}
		}
		break;
	case TASK_MEET_GOD_PRINT_WISH:
		{
			p_task.prog_num = m_mgr->GetPet()->GetWishGodPrintTimes();
		}
		break;
	case TASK_MEET_SKILL_LEVEL:
		{
			p_task.prog_num = m_mgr->GetRoleSkill()->GetMaxSkillLevel();
		}
		break;
	case TASK_MEET_JOIN_GUILD:	//p2 -> unuse
		{
			if(m_mgr->GetRole()->GetGuild() != NULL) p_task.prog_num = 1;
		}
		break;
	case TASK_MEET_TREASURE_TASK:
		{
			p_task.prog_num += p2;
		}
		break;
	case TASK_MEET_BOUNTY_LAST_TASK:
		{
			p_task.prog_num += p2;
		}
		break;
	case TASK_MEET_TREASURE_LAST_TASK:
		{
			p_task.prog_num += p2;
		}
		break;
	case TASK_MEET_BOUNTY_TASK2:
		{
			if (m_mgr->GetBountyTask()->GetTodayFinish() >= task_cfg->param2 * 2)p_task.prog_num = 1;
		}
		break;
	case TASK_MEET_WANLING_FB_TIMES:
		{
			if (m_mgr->GetRoleWanLingFB()->GetFBPassRewardTimes() >= task_cfg->param2)p_task.prog_num = 1;
		}
		break;
	case TASK_MEET_KILL_WILD_BOSS:
		{
			p_task.prog_num += p2;
		}
		break;
	case TASK_MEET_REALM_BREAK_FIGHT:
		{
			if (p2 == task_cfg->param2)
			{
				p_task.prog_num = 1;
			}
		}
		break;
	}
}

void TaskManager::ExecuteATaskForCondition(int condition, int p1, int p2, const TaskInfomaConfig * task_cfg, TaskInfo& p_task)
{
	UNSTD_STATIC_CHECK(46 == TASK_NEED_MAX);
	UNSTD_STATIC_CHECK(26 == TASK_MEET_TYPE_MAX);
	switch (condition)
	{
	case TASK_NEED_TO_DO_NOTHING:	// p1 = unuse, p2 -> unuse
		++p_task.prog_num;
		break;
	case TASK_NEED_PASS_TRANSCRI:	// p1 = fb_type, p2 -> times
	case TASK_NEED_PASS_CRYSTAL_FB_SEQ:	// p1 = 水晶副本type oa
		if (p1 == task_cfg->param1) p_task.prog_num += p2;
		break;
	case TASK_NEED_BEAT_MONGROUP:	// p1 = group_id, p2 -> unuse
		{
			if (p1 == task_cfg->param1) ++p_task.prog_num;
			this->OnTaskBattleFinish(p1, task_cfg->battle_mode);
		}
		break;
	case TASK_NEED_INTO_SCENARIO:	// p1 = scene_id, p2 -> unuse
	case TASK_NEED_LOAD_PROGRESS:	// p1 = gather_id, p2 -> unuse
	case TASK_NEED_PLAY_CUTSCENE:	// p1 = scene_id, p2 -> unuse
	case TASK_NEED_KILL_MONSTERS:	// p1 = monster_id, p2 -> unuse
	case TASK_NEED_TRAP_CREATURE:	// p1 = pet_id, p2 -> unuse
	case TASK_NEED_ESCORT:			// p1 = escort_seq, p2 -> unuse
	case TASK_NEED_ACCEPT_ESCORT:	// p1 = escort_seq, p2 -> unuse
		if (p1 == task_cfg->param1) ++p_task.prog_num;
		break;
	case TASK_NEED_DREAM_NOTES_CHALLENGE:	// p1 = 目标层数
		if (m_mgr->GetDreamNotesChallenge()->GetPassLevel() >= task_cfg->param1) p_task.prog_num = 1;
		break;

	case TASK_NEED_MEET_ANECDOTE_REQUIRES: // p1 = anecdote_task_type, p2 -> unuse
		if (p1 == task_cfg->param1)
		{
			p_task.prog_num = task_cfg->param2;
		}
		break;
	case TASK_NEED_KILL_MONGROUP:	// p1 = monster_id, p2 -> unuse
		if (TASKPOOL->IsMonsterGroupsMember(task_cfg->param1, p1)) ++p_task.prog_num;
		break;
	case TASK_NEED_USES_PROGROUP:	// p1 = item_id, p2 = num
		if (TASKPOOL->IsItemGroupsMember(task_cfg->param1, p1)) p_task.prog_num += p2;
		break;
	case TASK_NEED_BUY_PROPERTY:	// p1 = item_id, p2 = num
	case TASK_NEED_USES_PROPERTY:	// p1 = item_id, p2 = num
		if (p1 == task_cfg->param1) p_task.prog_num += p2;
		break;
	case TASK_NEED_KILL_IN_ORDER:	// p1 = monster_id, p2 -> unuse
		if (TASKPOOL->IsInRightOrder(task_cfg->param1, p1, p_task.prog_num)) ++p_task.prog_num;
		break;
	case TASK_NEED_HAVE_PROPERTY:	// p1 = item_id, p2 = unuse
		if (p1 == task_cfg->param1) p_task.prog_num = m_mgr->GetKnapsack()->Count(p1);
		break;
	case TASK_NEED_MEET_REQUIRES:	// p1 = condition, p2 = various param
		if (p1 == task_cfg->param1)
		{
			this->ExecuteATaskForMeetRequireCondition(condition, p1, p2, task_cfg, p_task);
		}
		break;
	case TASK_NEED_GAIN_TASKPROP:		// p1 = monster_id, p2 -> unuse
		{
			const DefeatMonsGroups * group_cfg = TASKPOOL->GetDefeatMonsGroups(task_cfg->param1);
			if (NULL != group_cfg && group_cfg->drop_monster_list.end() != group_cfg->drop_monster_list.find(p1))
			{
				DROPPOOL->DropToRole(task_role, group_cfg->drop_id);
				p_task.prog_num = m_mgr->GetKnapsack()->Count(group_cfg->need_item);
			}
		}
		break;

	case TASK_NEED_ENTER_FB:
		{
			if (p1 == task_cfg->param1 && p2 == task_cfg->param2)
			{
				p_task.prog_num += 1;
			}
		}
		break;
	case TASK_NEED_PASS_DESIGNATED_MEDAL_FB:
		{
			if (p1 == task_cfg->param1)
			{
				p_task.prog_num += 1;
			}
		}
		break;

	case TASK_NEED_COMMIT_PROPERTY:
		{
			// 什么都不做，等提交任务时才会生效
		}
		break;

	case TASK_NEED_WAIT_UNTIL_NEXT_DAY:
		{
			// 什么都不做，等待时间到，自动完成
		}
		break;

	case TASK_NEED_HAVE_PROPERTY_LIST:
		{
			const std::set<ItemID> *  need_item_list = TASKPOOL->GetItemGroupList(task_cfg->param1);
			if (need_item_list != NULL)
			{
				bool ret = false;
				for (std::set<ItemID>::const_iterator it = need_item_list->begin(); it != need_item_list->end(); ++it)
				{
					if (m_mgr->GetKnapsack()->HasItem(*it, TASK_NEED_HAVE_PROPERTY_LIST_REQUIRE_NUM))
					{
						ret = true;
					}
					else
					{
						ret = false;
						break;
					}
				}

				if (ret)
				{
					p_task.prog_num = TASK_NEED_HAVE_PROPERTY_LIST_REQUIRE_NUM;
				}
			}
		}
		break;
	case TASK_NEED_MATERIAL:
		{
			if (p1 == task_cfg->param1)
			{
				p_task.prog_num = m_mgr->GetRoleMaterial()->GetPassLevelByChapter(task_cfg->param1 - 1);
			}
		}
		break;
	case TASK_NEED_MI_YI:
		{
			if (p1 == task_cfg->param1)
			{
				p_task.prog_num = m_mgr->GetMiYi()->GetCurLayerFinishNum(task_cfg->param1);
			}
		}
		break;
	case TASK_NEED_PARTNER_LEVEL_ACHIVE:
		{
			if (p1 == task_cfg->param1)
			{
				p_task.prog_num = p2;
			}
		}
		break;
	case TASK_NEED_VILLAGE_LEVEL_ACHIVE:
		{
			if (p1 == task_cfg->param1)
			{
				p_task.prog_num = p2;
			}
		}
		break;
	case TASK_NEED_COLLECTION_TU_JIAN_ACTIVE_NUM:
		{
			p_task.prog_num = m_mgr->GetCollection()->GetCollectionActiveTuJianNum();
		}
		break;
	case TASK_NEED_ONE_FA_BAO_UP_LEVEL:
		{
			p_task.prog_num = m_mgr->GetFaBao()->GetFaBaoMaxLevel();
		}
		break;
	case TASK_NEED_OPEN_YIN_YANG_EYE:
		{
			p_task.prog_num = p2;
		}
		break;
	case TASK_NEED_PLAY_GAME:
		{
			p_task.prog_num += 1;
		}
		break;
	case TASK_NEED_PASS_WANLING_FB:
		{
			if (p1 == task_cfg->param1)
			{
				p_task.prog_num += p2;
			}
		}
		break;
	case TASK_NEED_CAREER_ADVANCE:
		{
			if (p1 > task_cfg->param1)
			{
				p_task.prog_num = 1;
			}
			else if (p1 == task_cfg->param1 && p2 >= task_cfg->param2)
			{
				p_task.prog_num = 1;
			}
		}
		break;
	case TASK_NEED_PARTNER_STRENGTHEN:
		{
			if (p1 == task_cfg->param1)
			{
				p_task.prog_num += 1;
			}
		}
		break;
	case TASK_NEED_PARTNER_BREAK:
		{
			if (p1 == task_cfg->param1)
			{
				p_task.prog_num += 1;
			}
		}
		break;
	case TASK_NEED_ROLE_BEAT_MONGROUP:
		break;
	case TASK_NEED_COMMIT_PET:		
		break;
	default:
		break;
	}

	if (TASK_NEED_MEET_REQUIRES != condition
		&& TASK_NEED_ENTER_FB != condition
		&& p_task.prog_num > task_cfg->param2
		&& TASK_NEED_CAREER_ADVANCE != condition)
	{
		p_task.prog_num = task_cfg->param2;
	}
}

bool TaskManager::IsTaskNeedWaiting(const TaskInfomaConfig* task_cfg, ARG_OUT unsigned int* wait_timestamp /* = NULL */)
{
	assert(NULL != task_cfg);

	if (task_cfg->open_server_day > 0)
	{
		UniqueServerID usid = m_mgr->GetRole()->GetUniqueServerID();
		int cur_open_days = World::GetInstWorld()->GetOpenServerDays_0(usid);
		if (cur_open_days < task_cfg->open_server_day)
		{
			if (NULL != wait_timestamp)
			{
				unsigned int open_server_timestamp = World::GetInstWorld()->GetServerOpenZeroTime(usid);
				*wait_timestamp = open_server_timestamp + (task_cfg->open_server_day - 1) * SECOND_PER_DAY;
			}
			return true;
		}
	}

	return false;
}

void TaskManager::OnDayChangeResetWaitingMap()
{
	unsigned int now = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	std::map<TaskID, unsigned int>::iterator it = m_waiting_map.begin();
	for (; it != m_waiting_map.end();)
	{
		if (it->second < now)
		{
			m_waiting_map.erase(it++);
		}
		else
		{
			++it;
		}
	}
	this->SendWaitingTaskList();
}

void TaskManager::CalcWaitingMapOnInit()
{
	for (int index = 0; index < m_task_param.task_count; ++index)
	{
		int check_task_id = m_task_param.task_info_list[index].task_id;
		const TaskInfomaConfig* task_cfg = TASKPOOL->GetTaskCfgByID(check_task_id);
		if (NULL == task_cfg) continue;

		unsigned int wait_timestamp = 0;
		if (this->IsTaskNeedWaiting(task_cfg, &wait_timestamp))
		{
			m_waiting_map[check_task_id] = wait_timestamp;
		}
	}
}

bool TaskManager::IsTaskTypeAcceptIgnoreLevelLimit(int task_type)
{
	return TASK_TYPE_MAIN_QUEST == task_type
		|| TASK_TYPE_SERIAL_FEEDER_TASK == task_type;
}

void TaskManager::GmResetTask(TaskID task_id)
{
	this->GmResetTaskImpl(task_id);
	this->CalcAcceptableTask(ACB_RECALC_INITIALIZES);
	this->SendSummaryTaskList();
	this->SendWaitingTaskList();
}

void TaskManager::GmClearAllTask()
{
	for (int t_cur = 0; t_cur < m_task_param.task_count; ++t_cur)
	{
		TaskInfo & p_task = m_task_param.task_info_list[t_cur];

		int task_id = p_task.task_id;
		this->SendSingleTaskDelete(task_id, TASK_REMOVE_REASON_FINISH);
		m_mgr->GetTaskRecorder()->TaskComplete(task_id);
		p_task.Reset();
	}


	std::set<TaskID> all_task_id;
	for (int lv = 1; lv <= MAX_ROLE_LEVEL; ++lv)
	{
		const std::set<TaskID> * lv_task_set = TASKPOOL->GetTaskListAtLv(lv);
		if (NULL == lv_task_set) continue;

		all_task_id.insert(lv_task_set->begin(), lv_task_set->end());		
	}

	for (std::set<TaskID>::iterator iter = all_task_id.begin(); iter != all_task_id.end(); ++iter)
	{
		m_mgr->GetTaskRecorder()->TaskComplete(*iter);
	}
}

void TaskManager::Update(time_t now_second)
{
	if (now_second < task_expire_check_time) return;

	for (int t_cur = 0; t_cur < m_task_param.task_count; ++t_cur)
	{
		TaskInfo & p_task = m_task_param.task_info_list[t_cur];
		if (0 == p_task.expire_time || now_second < p_task.expire_time) continue;

		const TaskInfomaConfig* task_cfg = TASKPOOL->GetTaskCfgByID(p_task.task_id);
		if (NULL != task_cfg && TASK_NEED_WAIT_UNTIL_NEXT_DAY == task_cfg->task_condition)
		{
			this->OnCommitTask(p_task.task_id);
		}
		else
		{
			this->OnGiveupTask(p_task.task_id, TASK_MANAGER_GIVE_UP_TYPE_UPDATE);	// 任务超时，只自己放弃；

			gamelog::g_log_task.printf(LL_INFO, "[ TaskManager::Update | Task TimeOut! -> TaskID: %d | ExpireTime: %u | NowTime: %lld ]",
				p_task.task_id, p_task.expire_time, (long long)now_second);

			if (CrossConfig::Instance().IsHiddenServer())
			{
				World::GetInstWorld()->SyncLogToOriginServer(m_mgr->GetRole()->GetUserId().db_index, SYNC_LOG_TYPE_TASK,
					"[ TaskManager::Update | Task TimeOut! -> TaskID: %d | ExpireTime: %u | NowTime: %lld ]",
					p_task.task_id, p_task.expire_time, (long long)now_second);
			}
		}
	}

	// 检测奇遇任务超时
	if (m_task_param.adv_task_timestamp != 0 && now_second >= m_task_param.adv_task_timestamp)
	{
		this->OnGiveupTask(m_task_param.adv_task_id, TASK_MANAGER_GIVE_UP_TYPE_UPDATE);
		m_task_param.adv_task_timestamp = 0;
	}

	task_expire_check_time = now_second + TASK_EXPIRE_CHECK_INTERVAL;

	this->DoDelayCommit();
}

void TaskManager::OnTaskManagerDayChange()
{
	this->OnDayChangeResetWaitingMap();

	m_task_param.today_adv_tri_times = 0;
	if (PARTNER_SOAR_TASK_STATUS_ACCEPT != m_task_param.partner_soar_task_status || 0 == m_task_param.cur_partner_soar_task_id)
	{
		m_task_param.partner_soar_task_status = PARTNER_SOAR_TASK_STATUS_BEGIN;
		m_task_param.cur_partner_soar_task_id = 0;
		m_task_param.is_cur_accept_partner_soar_task = 0;
		this->CalcAcceptableTask(ACB_RECALC_TASK_TYPE, TASK_TYPE_PARTNER_SOAR_TASK);
	}
	else	//接取状态
	{
		m_task_param.is_cur_accept_partner_soar_task = 2;
	}

	this->OnSendPartnerSoarInfo();
	this->ExecuteATask(TASK_NEED_CREATE_ROLE_DAYS, 0, 0);
}

bool TaskManager::CanAcceptTask(TaskID task_id, bool notify, ARG_OUT unsigned int* wait_timestamp)	// 只纯粹地检测自身条件是否满足要求，不检测组队状态
{
	const TaskInfomaConfig * task_cfg = TASKPOOL->GetTaskCfgByID(task_id);
	if (NULL == task_cfg) return false;

	//当前已经接取了伙伴飞升任务
	if (TASK_TYPE_PARTNER_SOAR_TASK == task_cfg->task_type)
	{
		if (m_mgr->GetFlyUpSystem()->GetFlyUpFlag() < FlyUpSystemParam::FLY_UP_STAG_1)	//未满足角色飞升
		{
			if (notify) m_mgr->NoticeNum(errornum::EN_TASK_PARTNER_SOAR_NOT_ACCPET_BY_ROLE_NOT_SOAR);
			return false;
		}
		else if (PARTNER_SOAR_TASK_STATUS_ACCEPT == m_task_param.partner_soar_task_status && task_cfg->pre_task_id != m_task_param.cur_partner_soar_task_id)
		{
			if (notify) m_mgr->NoticeNum(errornum::EN_TASK_PARTNER_SOAR_TASK_IN_PROGRESS);
			return false;
		}
		else if (PARTNER_SOAR_TASK_STATUS_FINISH == m_task_param.partner_soar_task_status)
		{
			if (notify) m_mgr->NoticeNum(errornum::EN_TASK_PARTNER_SOAR_TASK_FINISH);
			return false;
		}
	}

	if (!m_mgr->GetPet()->CanAcceptFeiShengTask(task_id))
	{
		return false;
	}

	if (TASK_TYPE_FLY_UP_FRONT_TASK == task_cfg->task_type)
	{
		if (!m_mgr->GetFlyUpSystem()->CanAcceptFeiShengTask(task_id))
		{
			return false;
		}
	}

	if (TASK_TYPE_ADVENTURES == task_cfg->task_type && task_id != m_task_param.adv_task_id)
	{
		return false;
	}

	if (TASK_TYPE_PET_FEI_SHENG_TASK != task_cfg->task_type
		&& TASK_TYPE_FLY_UP_TASK != task_cfg->task_type
		&& TASK_TYPE_ADVENTURES != task_cfg->task_type
		&& m_mgr->GetTaskRecorder()->HaveFinished(task_id))
	{
		if (notify) m_mgr->NoticeNum(errornum::EN_TASK_ALREADY_FINISHED);
		return false;
	}

	if (TASK_NEED_ACCEPT_ESCORT == task_cfg->task_condition && m_mgr->GetEscort()->HaveEscortTask())
	{
		if (notify) m_mgr->NoticeNum(errornum::EN_NOW_ESCORTING_NO_MORE_ACCEPT_ESCORT_TASK);
		return false;
	}

	if (TASK_TYPE_PET_FEI_SHENG_TASK != task_cfg->task_type)
	{
		if (task_cfg->pre_task_id > 0 && !m_mgr->GetTaskRecorder()->HaveFinished(task_cfg->pre_task_id))
		{
			if (notify) m_mgr->NoticeNum(errornum::EN_TASK_PREDECESSOR_NEED);
			return false;
		}

		if (task_cfg->pre_task_id2 > 0 && !m_mgr->GetTaskRecorder()->HaveFinished(task_cfg->pre_task_id2))
		{
			if (notify) m_mgr->NoticeNum(errornum::EN_TASK_PREDECESSOR_NEED);
			return false;
		}
	}


	if (NULL != m_task_param.GetTaskByID(task_id))
	{
		if (notify) m_mgr->NoticeNum(errornum::EN_TASK_ALREADY_ACCEPTED);
		return false;
	}
	// 检查最低等级限制 - 如果是主线，则跳过等级检测
	if (!IsTaskTypeAcceptIgnoreLevelLimit(task_cfg->task_type) && task_cfg->level_min > 0 && task_role->GetLevel() < task_cfg->level_min)
	{
		if (notify) m_mgr->NoticeNum(errornum::EN_ROLE_LEVEL_TOO_LOW);
		return false;
	}
	// 检查最高等级限制
	if (task_cfg->level_max > 0 && task_role->GetLevel() > task_cfg->level_max)
	{
		if (notify) m_mgr->NoticeNum(errornum::EN_ROLE_LEVEL_OVERTOP);
		return false;
	}
	// 检查职业是否符合要求
	if (task_cfg->prof_limit_min != PROF_TYPE_PROF_NO_LIMIT
		&& (task_role->GetProfession() < task_cfg->prof_limit_min || task_role->GetProfession() > task_cfg->prof_limit_max))
	{
		if (notify) m_mgr->NoticeNum(errornum::EN_ROLE_OCCUP_NOT_FIT);
		return false;
	}

	if (m_task_param.task_count >= MAX_ACCEPTED_TASK_NUM)
	{
		if (notify)m_mgr->NoticeNum(errornum::EN_TASK_LIST_ALREADY_MAX);
		return false;
	}

	if (task_cfg->task_type != TASK_TYPE_MAIN_QUEST && task_cfg->task_type != TASK_TYPE_STAGE_TASK &&
		task_cfg->task_type != TASK_TYPE_STAGE_TASK_1 && task_cfg->task_type != TASK_TYPE_SERIAL_FEEDER_TASK &&
		task_cfg->task_type != TASK_TYPE_SERIAL_FEEDER_TASK2 && task_cfg->task_type != TASK_TYPE_SERIAL_FEEDER_TASK_HOLY)
	{
		if (m_task_param.task_count >= MAX_ACCEPTED_TASK_NUM_ALLOW)
		{
			if (notify)m_mgr->NoticeNum(errornum::EN_TASK_LIST_ALREADY_MAX);
			return false;
		}
	}

	if (task_cfg->task_type == TASK_TYPE_SERIAL_FEEDER_TASK_HOLY)	// 圣器环支线任务需要判断飞升阶段接取
	{
		if(m_mgr->GetFlyUpSystem()->GetFlyUpFlag() < LOGIC_CONFIG->GetHolyEquipMentConfig()->GetOtherCfg().feisheng_type)
		{ 
			if (notify) m_mgr->NoticeNum(errornum::EN_NEED_ROLE_FEI_SHENG);
			return false;
		}
	}

	if (task_cfg->task_type == TASK_TYPE_MAIN_QUEST && this->HasMainTask())
	{
		return false;
	}

	//即使要等待也允许接取，但不能完成任务
	this->IsTaskNeedWaiting(task_cfg, wait_timestamp);	
	return true;
}

bool TaskManager::CanCommitTask(TaskID task_id, bool notify /* = false */, bool is_from_protocol /* = false */)
{
	if (is_from_protocol)
	{
		return m_can_commit_set.find(task_id) != m_can_commit_set.end();
	}

	const TaskInfo * task_info = m_task_param.GetTaskByID(task_id);
	if (NULL == task_info)
	{
		if (!is_from_protocol)
		{
			m_can_commit_set.erase(task_id); // 更新CanCommitSet
		}
		return false;	// 如果身上没有此任务
	}

	const TaskInfomaConfig * task_cfg = TASKPOOL->GetTaskCfgByID(task_id);
	if (NULL == task_cfg)
	{
		if (notify) m_mgr->NoticeNum(errornum::EN_TASK_NOLONGER_EXISTED);

		if (!is_from_protocol)
		{
			m_can_commit_set.erase(task_id); // 更新CanCommitSet
		}
		return false;
	}

	if (task_role->GetLevel() < task_cfg->level_min)
	{
		if (notify) m_mgr->NoticeNum(errornum::EN_ROLE_LEVEL_TOO_LOW);

		if (!is_from_protocol)
		{
			m_can_commit_set.erase(task_id); // 更新CanCommitSet
		}
		return false;
	}

	if (!m_mgr->GetPet()->CanCommitFeiShengTask(task_id, notify))
	{
		return false;
	}

	if (this->IsTaskNeedWaiting(task_cfg)) return false;

	bool can_commit = false;
	switch (task_cfg->task_condition)
	{
	case TASK_NEED_TO_DO_NOTHING:
		{
			can_commit = true;
		}
		break;

	case TASK_NEED_TALK_WITH_NPC:
	case TASK_NEED_KILL_MONGROUP:
	case TASK_NEED_PASS_TRANSCRI:
	case TASK_NEED_INTO_SCENARIO:
	case TASK_NEED_LOAD_PROGRESS:
	case TASK_NEED_GIVE_PROGROUP:
	case TASK_NEED_USES_PROGROUP:
	case TASK_NEED_BUY_PROPERTY:
	case TASK_NEED_PLAY_CUTSCENE:
	case TASK_NEED_KILL_MONSTERS:
	case TASK_NEED_GIVE_PROPERTY:
	case TASK_NEED_USES_PROPERTY:
	case TASK_NEED_KILL_IN_ORDER:
	case TASK_NEED_BEAT_MONGROUP:
	case TASK_NEED_TRAP_CREATURE:
	case TASK_NEED_ESCORT:
	case TASK_NEED_ACCEPT_ESCORT:
	case TASK_NEED_HAVE_PROPERTY_LIST:
	case TASK_NEED_PASS_CRYSTAL_FB_SEQ:
		{
			if (task_info->prog_num >= task_cfg->param2)
			{
				can_commit = true;
			}
		}
		break;

	case TASK_NEED_HAVE_PROPERTY:
		{
			if (m_mgr->GetKnapsack()->HasItem(task_cfg->param1, task_cfg->param2))
			{
				can_commit = true;
			}
		}
		break;

	case TASK_NEED_COMMIT_PROPERTY:
		{
			if (m_mgr->GetKnapsack()->HasItem(task_cfg->param1, task_cfg->param2))
			{
				can_commit = true;
			}
		}
		break;

	case TASK_NEED_GAIN_TASKPROP:
		{
			const DefeatMonsGroups * group_cfg = TASKPOOL->GetDefeatMonsGroups(task_cfg->param1);
			if (NULL != group_cfg && m_mgr->GetKnapsack()->HasItem(group_cfg->need_item, task_cfg->param2))
			{
				can_commit = true;
			}
		}
		break;

	case TASK_NEED_ENTER_FB:
		{
			if (task_info->prog_num >= 1)
			{
				can_commit = true;
			}
		}
		break;

	case TASK_NEED_MEET_REQUIRES:
		{
			switch (task_cfg->param1)
			{
			case TASK_MEET_GOD_PRINT_WISH:
				{
					short wish_times = task_role->GetRoleModuleManager()->GetPet()->GetWishGodPrintTimes();
					if (wish_times >= task_cfg->param2)
					{
						can_commit = true;
					}
				}
				break;
			case TASK_MEET_PROF_PROMOTE:
				{
					int up_times = task_role->GetProfession() % PROFESSION_BASE;		// 进阶次数
					if (up_times >= task_cfg->param2)
					{
						can_commit = true;
					}
				}
				break;

			case TASK_MEET_HAVE_PARTNER:
				if (m_mgr->GetPartner()->IsPartnerAlreadyActive(task_cfg->param2))
				{
					can_commit = true;
				}
				break;

			case TASK_MEET_LEARN_SKILLS:
			case TASK_MEET_JOIN_BATTLES:
			case TASK_MEET_OPEN_A_CHEST:
				if (task_info->prog_num >= 1)
				{
					can_commit = true;
				}
				break;

			case TASK_MEET_OWN_CREATURE:
				if (m_mgr->GetPet()->IsHavePet(task_cfg->param2))
				{
					can_commit = true;
				}
				break;

			case TASK_MEET_TRANSACTION_TASK:
				if (task_info->prog_num >= 1)
				{
					can_commit = true;
				}
				break;

			case TASK_MEET_MEDAL_TASK_PLAY_VIDEO:
				if (task_info->prog_num >= 1)
				{
					can_commit = true;
				}
				break;

			case TASK_MEET_PET_RELEASE:
				{
					if (task_info->prog_num >= 1)
					{
						can_commit = true;
					}
				}
				break;

			case TASK_MEET_PET_COMPOSE:
				{
					if (task_info->prog_num >= 1)
					{
						can_commit = true;
					}
				}
				break;
			case TASK_MEET_DREAM_NOTES_CHALLENGE:
				{
					if (m_mgr->GetDreamNotesChallenge()->IsPassLevel(task_cfg->param2))
					{
						can_commit = true;
					}
				}
				break;
			case TASK_MEET_BOUNTY_TASK:
				{
					if (task_info->prog_num >= task_cfg->param2)
					{
						can_commit = true;
					}
				}
				break;
			case TASK_MEET_RESEARCH_TASK:
				{
					if (task_info->prog_num >= task_cfg->param2)
					{
						can_commit = true;
					}
				}
				break;

			case TASK_MEET_PET_MONOPOLY_CATCH_PET:
				{
					if (task_info->prog_num >= task_cfg->param2)
					{
						can_commit = true;
					}
				}
				break;
			case TASK_MEET_LUN_HUI_WANG_CHUAN:
				{
					if (m_mgr->GetLunHuiWangChuan()->GetPassIDX() >= task_cfg->param2 || task_info->prog_num >= 1)
					{
						can_commit = true;
					}
				}
				break;
			case TASK_MEET_SKILL_LEVEL:
				{
					if (m_mgr->GetRoleSkill()->GetMaxSkillLevel() >= task_cfg->param2)
					{
						can_commit = true;
					}
				}
				break;
			case TASK_MEET_JOIN_GUILD:
				{
					if (m_mgr->GetRole()->GetGuild() != NULL)
					{
						can_commit = true;
					}
				}
				break;
			case TASK_MEET_TREASURE_TASK:
				{
					if (task_info->prog_num >= task_cfg->param2)
					{
						can_commit = true;
					}
				}
				break;
			case TASK_MEET_BOUNTY_LAST_TASK:
				{
					if (task_info->prog_num >= task_cfg->param2 || m_mgr->GetBountyTask()->HasFinishTodayTask())
					{
						can_commit = true;
					}
				}
				break;
			case TASK_MEET_TREASURE_LAST_TASK:
				{
					if (task_info->prog_num >= task_cfg->param2)
					{
						can_commit = true;
					}
				}
				break;
			case TASK_MEET_BOUNTY_TASK2:
				{
					if (m_mgr->GetBountyTask()->GetTodayFinish() >= task_cfg->param2 * 2 || task_info->prog_num >= 1)
					{
						can_commit = true;
					}
				}
				break;
			case TASK_MEET_WANLING_FB_TIMES:
				{
					if (m_mgr->GetRoleWanLingFB()->GetFBPassRewardTimes() >= task_cfg->param2 || task_info->prog_num >= 1)
					{
						can_commit = true;
					}
				}
				break;
			case TASK_MEET_KILL_WILD_BOSS:
				{
					if (task_info->prog_num >= task_cfg->param2)
					{
						can_commit = true;
					}
				}
				break;
			case TASK_MEET_REALM_BREAK_FIGHT:
				{
					if (task_info->prog_num >= 1)
					{
						can_commit = true;
					}
				}
				break;
			default:
				break;
			}
		}
		break;

	case TASK_NEED_PASS_DESIGNATED_MEDAL_FB:
		{
			if (task_info->prog_num >= 1)
			{
				can_commit = true;
			}
		}
		break;

	case TASK_NEED_MEET_ANECDOTE_REQUIRES:
		{
			if (m_mgr->GetAnecdoteTask()->IsFinishTask(task_cfg->param1) || task_info->prog_num >= task_cfg->param2)
			{
				can_commit = true;
			}
		}
		break;
	case TASK_NEED_COURAGE_CHALLENGE:
		{
			if (m_mgr->GetCourageChallenge()->IsFinishCurLayer(task_cfg->param1))
			{
				can_commit = true;
			}
		}
		break;

	case TASK_NEED_CREATE_ROLE_DAYS:
		{
			if (m_mgr->GetCommonData().role_create_count_day >= task_cfg->param2)
			{
				can_commit = true;
			}
		}
		break;
	case TASK_NEED_MATERIAL:
		{
			if (m_mgr->GetRoleMaterial()->GetPassLevelByChapter(task_cfg->param1 - 1) >= task_cfg->param2)
			{
				can_commit = true;
			}
		}
		break;
	case TASK_NEED_MI_YI:
		{
			if (m_mgr->GetMiYi()->GetCurLayerFinishNum(task_cfg->param1) >= task_cfg->param2)
			{
				can_commit = true;
			}
		}
		break;
	case TASK_NEED_WAIT_UNTIL_NEXT_DAY:
		{
			unsigned int now = (unsigned int)EngineAdapter::Instance().Time();
			if (now >= task_info->expire_time)
			{
				can_commit = true;
			}
		}
		break;
	case TASK_NEED_PARTNER_LEVEL_ACHIVE:
		{
			const NewPartnerBaseInfo* partner_base = m_mgr->GetPartner()->GetPartnerBaseInfoById(task_cfg->param1);
			if (partner_base != NULL)
			{
				if (partner_base->level >= task_cfg->param2)
				{
					can_commit = true;
				}
			}
		}
		break;
	case TASK_NEED_VILLAGE_LEVEL_ACHIVE:
		{
			if (task_info->prog_num >= task_cfg->param2)
			{
				can_commit = true;
			}
		}
		break;
	case TASK_NEED_COLLECTION_TU_JIAN_ACTIVE_NUM:
		{
			if (task_info->prog_num >= task_cfg->param2)
			{
				can_commit = true;
			}
		}
		break;
	case TASK_NEED_ONE_FA_BAO_UP_LEVEL:
		{
			if (task_info->prog_num >= task_cfg->param2)
			{
				can_commit = true;
			}
		}
		break;
	case TASK_NEED_OPEN_YIN_YANG_EYE:
		{
			if (m_mgr->GetYinYangEye()->IsOpen() ? 1 : 0 == task_cfg->param2
				|| task_info->prog_num == task_cfg->param2)
			{
				can_commit = true;
			}
		}
		break;
	case TASK_NEED_PLAY_GAME:
		{
			if (task_info->prog_num >= 1)
			{
				can_commit = true;
			}
		}
		break;

	case TASK_NEED_PASS_WANLING_FB:
		{
			if (task_info->prog_num >= task_cfg->param2)
			{
				can_commit = true;
			}
		}
		break;

	case TASK_NEED_CAREER_ADVANCE:
		{
			if (task_info->prog_num >= 1)
			{
				can_commit = true;
			}
		}
		break;

	case TASK_NEED_COMMIT_MONEY:
		{
			if (task_info->prog_num >= 1)
			{
				can_commit = true;
			}
		}
		break;
	case TASK_NEED_DREAM_NOTES_CHALLENGE:
		{
			if (m_mgr->GetDreamNotesChallenge()->GetPassLevel() >= task_cfg->param1 || task_info->prog_num >= 1)
			{
				can_commit = true;
			}
		}
		break;
	case TASK_NEED_PARTNER_STRENGTHEN:
		{
			if (task_info->prog_num >= task_cfg->param2)
			{
				can_commit = true;
			}
		}
		break;
		case TASK_NEED_PARTNER_BREAK:
		{
			if (task_info->prog_num >= task_cfg->param2)
			{
				can_commit = true;
			}
		}
		break;
	case TASK_NEED_ROLE_BEAT_MONGROUP:
	{
		if (task_info->prog_num != 0)
		{
			can_commit = true;
		}
	}
	break;
	case TASK_NEED_COMMIT_PET:
	{
		if (task_info->prog_num != 0)
		{
			can_commit = true;
		}
	}
	break;
	default:
		break;
	}
	UNSTD_STATIC_CHECK(46 == TASK_NEED_MAX);
	UNSTD_STATIC_CHECK(TASK_MEET_TYPE_MAX == 26);

	if (!can_commit && notify)
	{
		m_mgr->NoticeNum(errornum::EN_TASK_IS_NOT_COMPLETED);
	}

	if (can_commit)
	{
		m_can_commit_set.insert(task_id);
	}
	else
	{
		m_can_commit_set.erase(task_id);
	}

	return can_commit;
}

bool TaskManager::CanCommitTaskTeam(TaskID task_id, bool notify)
{
	Team * p_team = TeamManager::Instance().GetTeamByUid(task_role->GetUID());
	if (NULL == p_team)		// 如果玩家没有队伍，就自己提交任务
	{
		if (!this->CanCommitTask(task_id, notify))return false;
	}
	else		// 否则默认全队一起提交任务
	{
		int mem_count = p_team->GetMemberCount();
		for (int i = 0; i < mem_count; ++i)
		{
			Role * mem_role = p_team->GetMemberRoleByIndex(i);
			if (NULL == mem_role) continue;

			if (!mem_role->GetRoleModuleManager()->GetTaskManager()->CanCommitTask(task_id, notify))return false;
		}
	}
	return true;
}

void TaskManager::AcceptTask(TaskID task_id)
{
	const TaskInfomaConfig * t_tic = TASKPOOL->GetTaskCfgByID(task_id);
	if (NULL == t_tic)
	{
		gamelog::g_log_task.printf(LL_INFO, "[ TaskManager::AcceptTask | NULL Config! -> User:[ %d | %s ] | TaskID: %d | OccurTime: %lld ][Handle: return only.]",
			task_role->GetUID(), task_role->GetName(), task_id, (long long)m_nowTime);

		if (CrossConfig::Instance().IsHiddenServer())
		{
			World::GetInstWorld()->SyncLogToOriginServer(m_mgr->GetRole()->GetUserId().db_index, SYNC_LOG_TYPE_TASK,
				"[ TaskManager::AcceptTask | NULL Config! -> User:[ %d | %s ] | TaskID: %d | OccurTime: %lld ][Handle: return only.]",
				task_role->GetUID(), task_role->GetName(), task_id, (long long)m_nowTime);
		}

		m_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);	// 提示玩家一个错误
		return;
	}
	// 检查 NPC 距离 - 只检测接受任务的人
	if (0 != t_tic->accept_npc && !NpcManager::Instance().CanTouchNpc(task_role, t_tic->accept_npc)) return;

	if (TASK_IS_SHARE_NONE == t_tic->is_share)	// 如果不是全队共同接受的任务，则固定个人接受
	{
		this->OnAcceptTask(task_id);
	}
	else	// 否则全队一起接受 - 组队状态，任务共享
	{
		Team * p_team = TeamManager::Instance().GetTeamByUid(task_role->GetUID());
		if (NULL == p_team || p_team->GetLeaderUID() != task_role->GetUID())		// 如果玩家没有队伍，就自己接受任务
		{
			this->OnAcceptTask(task_id);
		}
		else
		{
			int mem_count = p_team->GetMemberCount();
			for (int i = 0; i < mem_count; ++i)
			{
				TeamMember* team_member = p_team->GetMemberInfoByIndex(i);
				if (NULL == team_member) continue;

				if (team_member->uid != p_team->GetLeaderUID() && !team_member->is_follow) continue; // 暂离的不一起提交

				Role * mem_role = p_team->GetMemberRoleByIndex(i);
				if (NULL == mem_role) continue;

				mem_role->GetRoleModuleManager()->GetTaskManager()->OnAcceptTask(task_id);
			}
		}
	}
}

void TaskManager::CommitTask(TaskID task_id)
{
	// 检测提交 NPC 的距离 - 只检测执行者(队长)
	const TaskInfomaConfig * task_cfg = TASKPOOL->GetTaskCfgByID(task_id);
	if (NULL == task_cfg) return;

	if (!task_cfg->can_auto_commit && 0 != task_cfg->commit_npc && !NpcManager::Instance().CanTouchNpc(task_role, task_cfg->commit_npc)) return;

	if (TASK_IS_SHARE_NONE == task_cfg->is_share)	// 如果不是全队共同接受的任务，则固定自己提交
	{
		this->OnCommitTask(task_id);
	}
	else
	{
		Team * p_team = TeamManager::Instance().GetTeamByUid(task_role->GetUID());
		if (NULL == p_team || p_team->GetLeaderUID() != task_role->GetUID())		// 如果玩家没有队伍，就自己提交任务
		{
			this->OnCommitTask(task_id);
		}
		else		// 否则默认全队一起提交任务
		{
			int mem_count = p_team->GetMemberCount();
			for (int i = 0; i < mem_count; ++i)
			{
				TeamMember* team_member = p_team->GetMemberInfoByIndex(i);
				if (NULL == team_member) continue;

				if (team_member->uid != p_team->GetLeaderUID() && !team_member->is_follow) continue; // 暂离的不一起提交

				Role * mem_role = p_team->GetMemberRoleByIndex(i);
				if (NULL == mem_role) continue;

				mem_role->GetRoleModuleManager()->GetTaskManager()->OnCommitTask(task_id);
			}
		}
	}
}

void TaskManager::GiveupTask(TaskID task_id)
{
	const TaskInfomaConfig * tic_cfg = TASKPOOL->GetTaskCfgByID(task_id);
	if (NULL == tic_cfg)
	{
		gamelog::g_log_task.printf(LL_INFO, "[ TaskManager::GiveupTask | NULL Config! -> User:[ %d | %s ] | TaskID: %d | OccurTime: %lld ][Handle: Delete This Task In Player's Task List.]",
			task_role->GetUID(), task_role->GetName(), task_id, (long long)m_nowTime);

		if (CrossConfig::Instance().IsHiddenServer())
		{
			World::GetInstWorld()->SyncLogToOriginServer(m_mgr->GetRole()->GetUserId().db_index, SYNC_LOG_TYPE_TASK,
				"[ TaskManager::GiveupTask | NULL Config! -> User:[ %d | %s ] | TaskID: %d | OccurTime: %lld ][Handle: Delete This Task In Player's Task List.]",
				task_role->GetUID(), task_role->GetName(), task_id, (long long)m_nowTime);
		}

		m_task_param.RemoveTask(task_id);
		this->SendSingleTaskDelete(task_id, TASK_REMOVE_REASON_OTHERS);			// 移除任务
		return;
	}

	// 检查玩家组队类型
	Team * p_team = TeamManager::Instance().GetTeamByUid(task_role->GetUID());
	int param =
		/*if*/(NULL == p_team) ?
		GIVEUP_TYPE_ONESELF_CAN :
		/*else if*/((task_role->GetUID() == p_team->GetLeaderUID()) ?
			GIVEUP_TYPE_CAPTAIN_CAN :
			/*else*/
			GIVEUP_TYPE_MEMBERS_CAN);

	if (TASK_GIVEUP_TYPE_NONE == tic_cfg->giveup[param])
	{
		m_mgr->NoticeNum(errornum::EN_TASK_CAN_NOT_BE_ABORT);
		return;
	}

	// 如果有队伍，同时操作者是队长，同时逻辑上此任务是队长波及全队放弃，则全队放弃
	if (NULL != p_team && task_role->GetUID() == p_team->GetLeaderUID() && TASK_GIVEUP_TYPE_TRUE == tic_cfg->giveup[GIVEUP_TYPE_CAPTAIN_ALL])
	{
		int mem_num = p_team->GetMemberCount();
		for (int mem_cur = 0; mem_cur < mem_num; ++mem_cur)
		{
			TeamMember* team_member = p_team->GetMemberInfoByIndex(mem_cur);
			if (NULL == team_member) continue;

			if (team_member->uid != p_team->GetLeaderUID() && !team_member->is_follow) continue; // 暂离的不一起提交

			Role * t_role = p_team->GetMemberRoleByIndex(mem_cur);
			if (NULL != t_role) t_role->GetRoleModuleManager()->GetTaskManager()->OnGiveupTask(task_id);
		}
	}
	else	// 否则只有执行者自己放弃
	{
		this->OnGiveupTask(task_id);
	}
}

void TaskManager::EraseTask(TaskID task_id)
{
	gamelog::g_log_task.printf(LL_INFO, "[ TaskManager::EraseTask | User:[ %d | %s ] | TaskID: %d | OccurTime: %lld ][Handle: Delete This Task In Player's Task List.]",
		task_role->GetUID(), task_role->GetName(), task_id, (long long)m_nowTime);

	if (CrossConfig::Instance().IsHiddenServer())
	{
		World::GetInstWorld()->SyncLogToOriginServer(m_mgr->GetRole()->GetUserId().db_index, SYNC_LOG_TYPE_TASK,
			"[ TaskManager::EraseTask | User:[ %d | %s ] | TaskID: %d | OccurTime: %lld ][Handle: Delete This Task In Player's Task List.]",
			task_role->GetUID(), task_role->GetName(), task_id, (long long)m_nowTime);
	}

	m_task_param.RemoveTask(task_id);
	this->SendSingleTaskDelete(task_id, TASK_REMOVE_REASON_OTHERS);
}

void TaskManager::DemandTask(TaskID task_id)
{
	Protocol::SCTaskSingleStatusInfo tssi;
	tssi.info_con.item_id = task_id;
	tssi.info_con.com_num = (NULL != m_task_param.GetTaskByID(task_id)) ? TASK_STATE_STILLS_IN_PROCESS :
		(m_mgr->GetTaskRecorder()->HaveFinished(task_id)) ? TASK_STATE_ALREADY_COMPLETED : TASK_STATE_HAVE_NO_CONDUCTED;

	m_mgr->NetSend((const char *)&tssi, sizeof(tssi));
}

void TaskManager::SetupTrace(int task_num, unsigned int * param_list)
{
	if (task_num <= 0 || task_num > TASK_TRACE_MAX_NUM || NULL == param_list) return;

	m_trace_param.list_len = task_num;
	memcpy(m_trace_param.trace_list, param_list, task_num * sizeof(m_trace_param.trace_list[0]));
}

void TaskManager::TaskSkipBattle(TaskID task_id)
{
	const TaskInfomaConfig* cfg = TASKPOOL->GetTaskCfgByID(task_id);
	if (NULL == cfg || !cfg->can_skip_battle)
	{
#ifdef _DEBUG
		m_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
#endif
		return;
	}

	if (!this->CheckRoleHasTask(task_id))
	{
#ifdef _DEBUG
		m_mgr->NoticeNum(errornum::EN_TASK_NOT_ACCEPTEND);
#endif
		return;
	}

	if (cfg->skip_battle_cost_coin > 0 && !m_mgr->GetMoney()->UseCoinBind(cfg->skip_battle_cost_coin, __FUNCTION__))
	{
		m_mgr->NoticeNum(errornum::EN_COIN_NOT_ENOUGH);

		return;
	}

	int monster_group_id = cfg->param1;
	EventHandler::Instance().OnFightMonsterGroup(m_mgr->GetRole(), monster_group_id, true, false);
}

bool TaskManager::CheckTaskByType(int task_type, int opera_type, int param)
{
	bool ret = false;
	switch (opera_type)
	{
	case TASK_CHECK_OPERA_TYPE_CLEAR:
		{

			std::vector<unsigned short> remove_task_vec;
			for (int task_idx = 0; task_idx < m_task_param.task_count && task_idx < MAX_ACCEPTED_TASK_NUM; task_idx++)
			{
				const TaskInfomaConfig * task_cfg = TASKPOOL->GetTaskCfgByID(m_task_param.task_info_list[task_idx].task_id);
				if (NULL != task_cfg)
				{
					if (task_cfg->task_type == task_type)
					{
						remove_task_vec.push_back(task_cfg->task_id);//记录统一删除
					}
				}
			}
			if (remove_task_vec.size() > 0)
			{
				for (std::vector<unsigned short>::iterator it = remove_task_vec.begin(); it != remove_task_vec.end(); it++)
				{
					m_task_param.RemoveTask(*it);
					this->SendSingleTaskDelete(*it, TASK_REMOVE_REASON_OTHERS);
				}
				ret = true;
			}
		}
		break;
	case TASK_CHECK_OPERA_TYPE_HAS_TASK:
		{
			for (int task_idx = 0; task_idx < m_task_param.task_count && task_idx < MAX_ACCEPTED_TASK_NUM; task_idx++)
			{
				const TaskInfomaConfig * task_cfg = TASKPOOL->GetTaskCfgByID(m_task_param.task_info_list[task_idx].task_id);
				if (NULL != task_cfg)
				{
					if (task_cfg->task_type == task_type)
					{
						ret = true;
					}
				}
			}
		}
		break;
	default:
		break;
	}
	return ret;
}

void TaskManager::SkipTask(TaskID task_id)
{
	const TaskInfomaConfig * task_cfg = TASKPOOL->GetTaskCfgByID(task_id);
	if (NULL != task_cfg)
	{
		const TaskInfo* task_info = m_task_param.GetTaskByID(task_id);
		if (NULL == task_info)
		{
			m_mgr->NoticeNum(errornum::EN_TASK_NOLONGER_EXISTED);
			return; // 如果身上没有这个任务，就不要提交了
		}

		if (!task_cfg->is_skip)
		{
			m_mgr->NoticeNum(errornum::EN_TASK_CANNT_SKIP);
			return;
		}

		if (task_cfg->skip_cost_num <= 0)return;

		if (!m_mgr->GetMoney()->MoneyTypeMoreThan(task_cfg->skip_cost_type, task_cfg->skip_cost_num))
		{
			return;
		}
		
		const TaskInfomaConfig * task_cfg = TASKPOOL->GetTaskCfgByID(task_id);
		if (NULL == task_cfg || task_role->GetLevel() < task_cfg->level_min) return;

		m_can_commit_set.insert(task_id);//直接标记完成

		//unsigned int now = static_cast<unsigned int>(m_nowTime);

		this->SendSingleTaskInfo(task_id, TASK_LIST_TYPE_ACCEPTED);
		{
			// 否则说明任务完成，开始发放任务报酬
			const TaskInfomaConfig * task_cfg = TASKPOOL->GetTaskCfgByID(task_id);
			if (NULL == task_cfg) return;

			// 发放任务报酬
			const CommonTaskReward * ctr_cfg = TASKPOOL->GetCommonTaskReward(task_id, task_role);
			if (NULL != ctr_cfg)	// 允许出现没有奖励的任务，所以当奖励为空时不返回。
			{
				if (ctr_cfg->reward_count > 0 && !m_mgr->GetKnapsack()->CheckForPutList(ctr_cfg->reward_count, ctr_cfg->reward_list))
				{
					return; // 没有足够位置发放任务报酬
				}
			}

			if (!m_mgr->GetMoney()->UseMoneyType(task_cfg->skip_cost_type, task_cfg->skip_cost_num, __FUNCTION__))
			{
				return;
			}

			if (NULL != ctr_cfg)
			{
				if (ctr_cfg->reward_count > 0)
				{
					m_mgr->GetKnapsack()->PutList2(ctr_cfg->reward_count, ctr_cfg->reward_list, PUT_REASON_TASK_REWARD, GET_REASON_TASK, task_id);
				}

				m_mgr->GetMoney()->Add(ctr_cfg->gold, ctr_cfg->immortal, ctr_cfg->coin, "CommonTaskReward");
				if (ctr_cfg->exp > 0)
				{
					long long real_add_exp = task_role->AddExp(ctr_cfg->exp, ADD_EXP_TYPE_TASK);
					if (task_cfg->task_type == TASK_TYPE_BOUNTY_TASK)
					{
						m_mgr->GetBountyTask()->OnAddDayExp(task_id, real_add_exp);
						m_mgr->GetLevelComplement()->OnAddExp(real_add_exp);
						EventHandler::Instance().OnFinishLevelComplementRelatedFun(m_mgr->GetRole(), ctr_cfg->exp, __FUNCTION__);
					}
				}
				if (ctr_cfg->repu_numb > 0) m_mgr->GetPrestige()->AddPrestige(ctr_cfg->repu_city, ctr_cfg->repu_numb, __FUNCTION__);
				if (ctr_cfg->pet_exp > 0)
				{
					int fight_pet_idx = task_role->GetRoleModuleManager()->GetPet()->GetFightIndex();
					if (fight_pet_idx >= 0) task_role->GetRoleModuleManager()->GetPet()->AddExp(fight_pet_idx, ctr_cfg->pet_exp, __FUNCTION__, Protocol::SCSendPetSingleInfo::PET_NOTIE_TYPE_EXP);
				}
				if (ctr_cfg->power > 0)
				{
					m_mgr->GetRoleCrystalFb()->AddCrystalFbEnergy(ctr_cfg->power, __FUNCTION__, ENTER_ENERGY_CHANGE_TYPE_OTHER);
				}
			}

			// 任务报酬已发放，注意不能中途返回
			m_task_param.RemoveTask(task_id);
			this->SendSingleTaskDelete(task_id, TASK_REMOVE_REASON_FINISH);

			m_mgr->GetTaskRecorder()->TaskComplete(task_id);				// 记录任务完成；
			this->CalcAcceptableTask(ACB_RECALC_TASK_COMMIT, task_id);		// 必须在记录任务完成后面，因为这里面要删除这个任务；

			EventHandler::Instance().OnCommitTask(task_role, task_id);

			m_old_give_up_state.erase(task_id);
		}

		ROLE_LOG_QUICK6(LOG_TYPE_COMPLETE_TASK, task_role, task_cfg->task_type, task_cfg->task_id, NULL, NULL);

		gamelog::g_log_task_stats.printf(LL_INFO, "[TaskResult::Complete][role[%d %s] task_id:%d task_level:%d task_type:%d role_professional:%d]",
			UidToInt(task_role->GetUserId()), task_role->GetName(), task_cfg->task_id, task_cfg->level_min, task_cfg->task_type, task_role->GetProfession());

		if (CrossConfig::Instance().IsHiddenServer())
		{
			World::GetInstWorld()->SyncLogToOriginServer(m_mgr->GetRole()->GetUserId().db_index, SYNC_LOG_TYPE_TASK_STAT,
				"[TaskResult::Complete][role[%d %s] task_id:%d task_level:%d task_type:%d role_professional:%d]",
				UidToInt(task_role->GetUserId()), task_role->GetName(), task_cfg->task_id, task_cfg->level_min, task_cfg->task_type, task_role->GetProfession());
		}
	}
}

void TaskManager::OnCommitMoney(TaskID task_id)
{
	const TaskInfomaConfig * task_cfg = TASKPOOL->GetTaskCfgByID(task_id);
	if (NULL != task_cfg)
	{
		const TaskInfo* task_info = m_task_param.GetTaskByID(task_id);
		if (NULL == task_info)
		{
			m_mgr->NoticeNum(errornum::EN_TASK_NOLONGER_EXISTED);
			return; // 如果身上没有这个任务，就不要提交了
		}
		if (TASK_NEED_COMMIT_MONEY != task_cfg->task_condition)return;
		if (!m_mgr->GetMoney()->UseMoneyType(task_cfg->param1, task_cfg->param2, __FUNCTION__))
		{
			return;
		}
		this->ExecuteATask(task_id, 1);
	}
}

void TaskManager::OnTalkWithNPC(TaskID task_id, UInt16 npc_id)
{
	// 检测 NPC 距离 - 只检测执行者(队长)
	if (!NpcManager::Instance().CanTouchNpc(task_role, npc_id)) return;

	const TaskInfomaConfig * p_tic = TASKPOOL->GetTaskCfgByID(task_id);
	if (NULL == p_tic || TASK_NEED_TALK_WITH_NPC != p_tic->task_condition || npc_id != p_tic->param1)
	{
		m_mgr->NoticeNum(errornum::EN_TASK_NOLONGER_EXISTED);
		return;
	}

	// 检测队伍状态 - 如果队伍状态跟任务要求状态不符，则返回
	if (!this->CheckTeamForTask(task_id)) return;

	Team * p_team = TeamManager::Instance().GetTeamByUid(task_role->GetUID());
	if (NULL == p_team || p_team->GetLeaderUID() != task_role->GetUID()) // 玩家没有队伍或不是队长
	{
		this->ExecuteATask(task_id, 1);
	}
	else
	{
		int mem_count = p_team->GetMemberCount();
		for (int t_mem = 0; t_mem < mem_count; ++t_mem)
		{
			TeamMember* team_member = p_team->GetMemberInfoByIndex(t_mem);
			if (NULL == team_member) continue;

			if (team_member->uid != p_team->GetLeaderUID() && !team_member->is_follow) continue; // 暂离的不一起提交

			Role * t_role = p_team->GetMemberRoleByIndex(t_mem);
			if (NULL != t_role) t_role->GetRoleModuleManager()->GetTaskManager()->ExecuteATask(task_id, 1);
		}
	}
}

void TaskManager::OnKillMonster(int monster_id)
{
	this->ExecuteATask(TASK_NEED_KILL_MONGROUP, monster_id, 1);
	this->ExecuteATask(TASK_NEED_KILL_MONSTERS, monster_id, 1);
	this->ExecuteATask(TASK_NEED_KILL_IN_ORDER, monster_id, 1);
	this->ExecuteATask(TASK_NEED_GAIN_TASKPROP, monster_id, 1);
}

void TaskManager::OnPassFB(int fb_type, int count/* = 1*/)
{
	Team * p_team = TeamManager::Instance().GetTeamByUid(task_role->GetUID());
	if (NULL == p_team || p_team->GetLeaderUID() != task_role->GetUID())
	{
		this->ExecuteATask(TASK_NEED_PASS_TRANSCRI, fb_type, count);
	}
	else
	{
		int mem_count = p_team->GetMemberCount();
		for (int t_mem = 0; t_mem < mem_count; ++t_mem)
		{
			TeamMember* team_member = p_team->GetMemberInfoByIndex(t_mem);
			if (NULL == team_member) continue;

			if (team_member->uid != p_team->GetLeaderUID() && !team_member->is_follow) continue; // 暂离的不一起提交

			Role * t_role = p_team->GetMemberRoleByIndex(t_mem);
			if (NULL == t_role) continue;

			t_role->GetRoleModuleManager()->GetTaskManager()->ExecuteATask(TASK_NEED_PASS_TRANSCRI, fb_type, count);
		}
	}
}

void TaskManager::OnPassCrystalFB(int crystal_fb_seq)
{
	this->ExecuteATask(TASK_NEED_PASS_CRYSTAL_FB_SEQ, crystal_fb_seq, 1);
}

void TaskManager::OnEnterScene(int scene_id)
{
	this->ExecuteATask(TASK_NEED_INTO_SCENARIO, scene_id, 1);
}

void TaskManager::OnLoadProgress(int gather_id)
{
	Team * p_team = TeamManager::Instance().GetTeamByUid(task_role->GetUID());
	if (NULL == p_team || p_team->GetLeaderUID() != task_role->GetUID())
	{
		this->ExecuteATask(TASK_NEED_LOAD_PROGRESS, gather_id, 1);
	}
	else
	{
		int mem_count = p_team->GetMemberCount();
		for (int t_mem = 0; t_mem < mem_count; ++t_mem)
		{
			TeamMember* team_member = p_team->GetMemberInfoByIndex(t_mem);
			if (NULL == team_member) continue;

			if (team_member->uid != p_team->GetLeaderUID() && !team_member->is_follow) continue; // 暂离的不一起提交

			Role * t_role = p_team->GetMemberRoleByIndex(t_mem);
			if (NULL == t_role) continue;

			t_role->GetRoleModuleManager()->GetTaskManager()->ExecuteATask(TASK_NEED_LOAD_PROGRESS, gather_id, 1);
		}
	}
}

void TaskManager::OnGiveProperty(TaskID task_id, int item_id, int num)
{
	if (num <= 0) return;

	const TaskInfomaConfig * p_tic = TASKPOOL->GetTaskCfgByID(task_id);
	if (NULL == p_tic || TASK_NEED_GIVE_PROPERTY != p_tic->task_condition || item_id != p_tic->param1)
	{
		m_mgr->NoticeNum(errornum::EN_TASK_NOLONGER_EXISTED);
		return;
	}

	const TaskInfo* task_info = m_task_param.GetTaskByID(task_id);
	if (NULL == task_info)
	{
		return; // 如果身上没有这个任务，就不要提交物品了
	}

	// 检测队伍状态 - 如果队伍状态跟任务要求状态不符，则返回
	if (!this->CheckTeamForTask(task_id)) return;

	// 队长提交道具 - 只消耗队长的道具
	if (!m_mgr->GetKnapsack()->ConsumeItem(item_id, num, __FUNCTION__, KNAPSACK_CONSUME_TYPE_CANNOT_BUY, false))
	{
		// 通知缺少物品
		static Protocol::SCTaskItemLackForCommitNotice notice;
		notice.count = 1;
		notice.item_list[0].item_id = item_id;
		notice.item_list[0].num = num;

		int send_len = sizeof(notice) - sizeof(notice.item_list) + notice.count * sizeof(notice.item_list[0]);
		EngineAdapter::Instance().NetSend(m_mgr->GetRole()->GetNetId(), (const char*)&notice, send_len);

		return;
	}
	else
	{
		// 通知提交了物品
		static Protocol::SCTaskItemCommitNotice notice;
		notice.count = 1;
		notice.item_list[0].item_id = item_id;
		notice.item_list[0].num = num;

		int send_len = sizeof(notice) - sizeof(notice.item_list) + notice.count * sizeof(notice.item_list[0]);
		EngineAdapter::Instance().NetSend(m_mgr->GetRole()->GetNetId(), (const char*)&notice, send_len);
	}

	this->OnItemChange(item_id);// 只有队长提交了道具，也就是说只有队长的道具数量改变了，所以只检测队长的。

	this->ExecuteATask(task_id, num);
}

void TaskManager::OnGiveProGroup(TaskID task_id, const std::set<ItemContainer> & com_item_list)
{
	if (com_item_list.empty()) return;	// 如果什么都没提交，直接返回

	const TaskInfomaConfig * p_tic = TASKPOOL->GetTaskCfgByID(task_id);
	if (NULL == p_tic || TASK_NEED_GIVE_PROGROUP != p_tic->task_condition)
	{
		m_mgr->NoticeNum(errornum::EN_TASK_NOLONGER_EXISTED);
		return;
	}

	const TaskInfo* task_info = m_task_param.GetTaskByID(task_id);
	if (NULL == task_info)
	{
		return; // 如果身上没有这个任务，就不要提交物品了
	}

	// 检查并消耗道具，如果消耗成功，至少队长部分不能中途返回 - 只消耗队长的道具	
	const std::set<ItemID>* com_item_list_2 = TASKPOOL->GetItemGroupList(p_tic->param1);
	if (NULL == com_item_list_2)
	{
		m_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	int stuff_count = 0;
	ItemExtern::ItemConsumeConfig stuff_list[ItemNamespace::MAX_BATCH_CONSUME_ITEM_NUM];
	for (std::set<ItemID>::const_iterator s_it = com_item_list_2->begin(); s_it != com_item_list_2->end(); ++s_it)
	{
		ItemExtern::ItemConsumeConfig & stuff_obj = stuff_list[stuff_count++];
		stuff_obj.item_id = *s_it;
		stuff_obj.num = p_tic->param2;
		stuff_obj.buyable = false;
	}

	//int com_num = 0;
	//for (std::set<ItemContainer>::iterator s_it = com_item_list.begin(); s_it != com_item_list.end(); ++s_it)
	//{
	//	com_num += s_it->com_num;
	//}
	//if (com_num < p_tic->param2 * stuff_count)
	//{
	//	m_mgr->NoticeNum(errornum::EN_TASK_IS_NOT_COMPLETED);
	//	return;
	//}

	if (stuff_count > 0)
	{
		static ItemExtern::ItemConsumeList consume_list; consume_list.Reset();
		if (!ItemExtern::GetConsumeListByOrder(task_role, false, stuff_count, stuff_list, &consume_list, true))
		{
			m_mgr->NoticeNum(errornum::EN_TASK_IS_NOT_COMPLETED);
			return;
		}

		// 消耗
		if (consume_list.count > 0 && !m_mgr->GetKnapsack()->ConsumeItemByIndexList(consume_list.count, consume_list.column_list, consume_list.index_list, consume_list.num_list, "TaskGiveItem"))
		{
			m_mgr->NoticeNum(errornum::EN_TASK_IS_NOT_COMPLETED);
			return;
		}
	}

	// 检测道具
	for (int ic_cur = 0; ic_cur < stuff_count; ++ic_cur)
	{
		this->OnItemChange(stuff_list[ic_cur].item_id);
	}

	{
		// 通知提交了物品
		static Protocol::SCTaskItemCommitNotice notice;
		notice.count = 0;
		for (int i = 0; i < stuff_count; ++i)
		{
			notice.item_list[notice.count].item_id = stuff_list[i].item_id;
			notice.item_list[notice.count].num = stuff_list[i].num;
			notice.count += 1;
		}
		int send_len = sizeof(notice) - sizeof(notice.item_list) + notice.count * sizeof(notice.item_list[0]);
		EngineAdapter::Instance().NetSend(m_mgr->GetRole()->GetNetId(), (const char*)&notice, send_len);
	}

	this->ExecuteATask(task_id, p_tic->param2);
}

bool TaskManager::OnCommitProperty(TaskID task_id, int item_id, int num)
{
	if (num <= 0) return false;

	const TaskInfomaConfig * p_tic = TASKPOOL->GetTaskCfgByID(task_id);
	if (NULL == p_tic || TASK_NEED_COMMIT_PROPERTY != p_tic->task_condition || item_id != p_tic->param1)
	{
		m_mgr->NoticeNum(errornum::EN_TASK_NOLONGER_EXISTED);
		return false;
	}

	const TaskInfo* task_info = m_task_param.GetTaskByID(task_id);
	if (NULL == task_info)
	{
		return false; // 如果身上没有这个任务，就不要提交物品了
	}

	// 提交道具
	if (!m_mgr->GetKnapsack()->ConsumeItem(item_id, num, __FUNCTION__, KNAPSACK_CONSUME_TYPE_CANNOT_BUY, false))
	{
		// 通知缺少了物品
		static Protocol::SCTaskItemLackForCommitNotice notice;
		notice.count = 1;
		notice.item_list[0].item_id = item_id;
		notice.item_list[0].num = num;

		int send_len = sizeof(notice) - sizeof(notice.item_list) + notice.count * sizeof(notice.item_list[0]);
		EngineAdapter::Instance().NetSend(m_mgr->GetRole()->GetNetId(), (const char*)&notice, send_len);

		return false;
	}
	else
	{
		// 通知提交了物品
		static Protocol::SCTaskItemCommitNotice notice;
		notice.count = 1;
		notice.item_list[0].item_id = item_id;
		notice.item_list[0].num = num;

		int send_len = sizeof(notice) - sizeof(notice.item_list) + notice.count * sizeof(notice.item_list[0]);
		EngineAdapter::Instance().NetSend(m_mgr->GetRole()->GetNetId(), (const char*)&notice, send_len);
	}

	return true;
}

void TaskManager::OnUseProperty(int item_id, int num)
{
	if (num <= 0) return;

	const ItemBase* item_base = ITEMPOOL->GetItem(item_id);
	if (NULL == item_base) return;

	if (ItemNamespace::ITEM_COLUMN_TYPE_TASK_MATERIAL == item_base->GetItemColumn()
		|| TASKPOOL->IsConditionNeedItem(item_id))
	{
		this->ExecuteATask(TASK_NEED_USES_PROGROUP, item_id, num);
		this->ExecuteATask(TASK_NEED_USES_PROPERTY, item_id, num);
	}
}

void TaskManager::OnBuyProperty(int item_id, int num)
{
	if (num <= 0) return;

	const ItemBase* item_base = ITEMPOOL->GetItem(item_id);
	if (NULL == item_base) return;

	if (ItemNamespace::ITEM_COLUMN_TYPE_TASK_MATERIAL == item_base->GetItemColumn()
		|| TASKPOOL->IsConditionNeedItem(item_id))
	{
		this->ExecuteATask(TASK_NEED_BUY_PROPERTY, item_id, num);
	}
}

void TaskManager::OnPlayGame(int game_type, int param1, int param2)
{
	switch (game_type)
	{
	case TASK_GAME_PLAY_TYPE_0:
		this->OnElementRestraintGame(true);
		break;
	case TASK_GAME_PLAY_TYPE_1:
		this->OnDestructionFaZhen();
		break;
	case TASK_GAME_PLAY_TYPE_2:
		this->OnFixFaZhen();
		break;
	case TASK_GAME_PLAY_TYPE_3:
		this->OnXiuXing();
		break;
	default:
		break;
	}
}

void TaskManager::OnPlayCutscene(int cutscene_id)
{
	Team * p_team = TeamManager::Instance().GetTeamByUid(task_role->GetUID());
	if (NULL == p_team || p_team->GetLeaderUID() == task_role->GetUID())
	{
		this->ExecuteATask(TASK_NEED_PLAY_CUTSCENE, cutscene_id, 1);
	}
	else
	{
		int mem_count = p_team->GetMemberCount();
		for (int t_mem = 0; t_mem < mem_count; ++t_mem)
		{
			TeamMember* team_member = p_team->GetMemberInfoByIndex(t_mem);
			if (NULL == team_member) continue;

			if (team_member->uid != p_team->GetLeaderUID() && !team_member->is_follow) continue; // 暂离的不一起提交

			Role * t_role = p_team->GetMemberRoleByIndex(t_mem);
			if (NULL == t_role) continue;

			t_role->GetRoleModuleManager()->GetTaskManager()->ExecuteATask(TASK_NEED_PLAY_CUTSCENE, cutscene_id, 1);
		}
	}
}

void TaskManager::OnPlayMedalTaskVideo(int video_id)
{
	this->ExecuteATask(TASK_NEED_MEET_REQUIRES, TASK_MEET_MEDAL_TASK_PLAY_VIDEO, video_id);
}

void TaskManager::OnTrapCreature(int pet_id)
{
	this->ExecuteATask(TASK_NEED_TRAP_CREATURE, pet_id, 1);
}

void TaskManager::OnGetACreature(int pet_id)
{
	this->ExecuteATask(TASK_NEED_MEET_REQUIRES, TASK_MEET_OWN_CREATURE, pet_id);
}

void TaskManager::OnActivePartner(int partner_id)
{
	this->ExecuteATask(TASK_NEED_MEET_REQUIRES, TASK_MEET_HAVE_PARTNER, partner_id);
}

void TaskManager::OnPartnerUpLevel(int partner_id, int old_level, int new_level)
{
	this->ExecuteATask(TASK_NEED_PARTNER_LEVEL_ACHIVE, partner_id, new_level);
}

void TaskManager::OnLearnSkill()
{
	this->ExecuteATask(TASK_NEED_MEET_REQUIRES, TASK_MEET_LEARN_SKILLS, 0);
	this->ExecuteATask(TASK_NEED_MEET_REQUIRES, TASK_MEET_SKILL_LEVEL, 0);
}

void TaskManager::OnVictBattle(int group_id)
{
	this->ExecuteATask(TASK_NEED_BEAT_MONGROUP, group_id, 1);
}

void TaskManager::OnJoinBattle(int group_id)
{
	this->ExecuteATask(TASK_NEED_MEET_REQUIRES, TASK_MEET_JOIN_BATTLES, group_id);
}

void TaskManager::OnOpenAChest(int chest_id)
{
	this->ExecuteATask(TASK_NEED_MEET_REQUIRES, TASK_MEET_OPEN_A_CHEST, chest_id);
}

void TaskManager::OnItemChange(int item_id)
{
	const ItemBase *item_base = ITEMPOOL->GetItem(item_id);
	if (NULL == item_base) return;

	if (item_base->GetItemColumn() == ItemNamespace::ITEM_COLUMN_TYPE_TASK_MATERIAL
		|| TASKPOOL->IsConditionNeedItem(item_id))
	{
		this->ExecuteATask(TASK_NEED_HAVE_PROPERTY, item_id, 0);
		this->ExecuteATask(TASK_NEED_HAVE_PROPERTY_LIST, 0, 0);
		this->ExecuteATask(TASK_NEED_COMMIT_PROPERTY, item_id, 0);
	}
}

void TaskManager::PlayColosseum()
{
	this->ExecuteATask(TASK_NEED_MEET_REQUIRES, TASK_MEET_PLAY_ONCE_COLOSSEUM, 0);
}

void TaskManager::OnTransactionTask()
{
	this->ExecuteATask(TASK_NEED_MEET_REQUIRES, TASK_MEET_TRANSACTION_TASK, 0);
}

void TaskManager::OnPetRelease()
{
	this->ExecuteATask(TASK_NEED_MEET_REQUIRES, TASK_MEET_PET_RELEASE, 0);
}

void TaskManager::OnPetCompose()
{
	this->ExecuteATask(TASK_NEED_MEET_REQUIRES, TASK_MEET_PET_COMPOSE, 0);
}

void TaskManager::OnEnterFB(int fb_type, int seq)
{
	this->ExecuteATask(TASK_NEED_ENTER_FB, fb_type, seq);
}

void TaskManager::OnPassCourageChallenge()
{
	this->ExecuteATask(TASK_NEED_COURAGE_CHALLENGE, 0, 0);
}

void TaskManager::OnPassDreamNotesChallenge(int level)
{
	this->ExecuteATask(TASK_NEED_MEET_REQUIRES, TASK_MEET_DREAM_NOTES_CHALLENGE, 0);
	this->ExecuteATask(TASK_NEED_DREAM_NOTES_CHALLENGE, level, 1);
}

void TaskManager::OnFinishBountyTask(int count)
{
	this->ExecuteATask(TASK_NEED_MEET_REQUIRES, TASK_MEET_BOUNTY_TASK, count);
	this->ExecuteATask(TASK_NEED_MEET_REQUIRES, TASK_MEET_BOUNTY_TASK2, count);
}

void TaskManager::OnFinsihResearchTask(int count)
{
	this->ExecuteATask(TASK_NEED_MEET_REQUIRES, TASK_MEET_RESEARCH_TASK, count);
}

void TaskManager::OnPassMaterial(int chapter_id)
{
	this->ExecuteATask(TASK_NEED_MATERIAL, chapter_id + 1, 0);
}

void TaskManager::OnPetMonopolyCatchPet()
{
	this->ExecuteATask(TASK_NEED_MEET_REQUIRES, TASK_MEET_PET_MONOPOLY_CATCH_PET, 1);
}

void TaskManager::OnPassLunHuiWangChuan()
{
	this->ExecuteATask(TASK_NEED_MEET_REQUIRES, TASK_MEET_LUN_HUI_WANG_CHUAN, 0);
}

void TaskManager::OnPassKillMiYiBoss(int seq)
{
	this->ExecuteATask(TASK_NEED_MI_YI, seq, 0);
}

void TaskManager::OnAddPrestige(int village_id, int level)
{
	this->ExecuteATask(TASK_NEED_VILLAGE_LEVEL_ACHIVE, village_id, level);
}

void TaskManager::OnCollectionTuJianActive()
{
	this->ExecuteATask(TASK_NEED_COLLECTION_TU_JIAN_ACTIVE_NUM, 0, 0);
}

void TaskManager::OnFaBaoLevelChange()
{
	this->ExecuteATask(TASK_NEED_ONE_FA_BAO_UP_LEVEL, 0, 0);
}

void TaskManager::OnFinishAnedoteGame(short anedote_type)
{
	this->ExecuteATask(TASK_NEED_MEET_ANECDOTE_REQUIRES, anedote_type, 0);
}

void TaskManager::OnGodPrintWish()
{
	this->ExecuteATask(TASK_NEED_MEET_REQUIRES, TASK_MEET_GOD_PRINT_WISH, 0);
}

void TaskManager::OnOpenOrCloseYinYangEye(int open_mark)
{
	this->ExecuteATask(TASK_NEED_OPEN_YIN_YANG_EYE, 0, open_mark);
}

void TaskManager::OnElementRestraintGame(bool is_win)
{
	this->ExecuteATask(TASK_NEED_PLAY_GAME, TASK_GAME_PLAY_TYPE_0, is_win ? 1 : 2);
}

void TaskManager::OnDestructionFaZhen()
{
	this->ExecuteATask(TASK_NEED_PLAY_GAME, TASK_GAME_PLAY_TYPE_1, 0);
}

void TaskManager::OnFixFaZhen()
{
	this->ExecuteATask(TASK_NEED_PLAY_GAME, TASK_GAME_PLAY_TYPE_2, 0);
}

void TaskManager::OnXiuXing()
{
	this->ExecuteATask(TASK_NEED_PLAY_GAME, TASK_GAME_PLAY_TYPE_3, 0);
}

void TaskManager::OnJoinGuild()
{
	this->ExecuteATask(TASK_NEED_MEET_REQUIRES, TASK_MEET_JOIN_GUILD, 0);
}

void TaskManager::OnCommitTreasureTask(int count)
{
	this->ExecuteATask(TASK_NEED_MEET_REQUIRES, TASK_MEET_TREASURE_TASK, count);
}

void TaskManager::OnFinishTreasureTask()
{
	this->ExecuteATask(TASK_NEED_MEET_REQUIRES, TASK_MEET_TREASURE_LAST_TASK, 1);
}

void TaskManager::OnFinishBountyToday()
{
	this->ExecuteATask(TASK_NEED_MEET_REQUIRES, TASK_MEET_BOUNTY_LAST_TASK, 1);
}

void TaskManager::OnRoleFightWildBoss()
{
	this->ExecuteATask(TASK_NEED_MEET_REQUIRES, TASK_MEET_KILL_WILD_BOSS, 1);
}

void TaskManager::OnRoleRealmBreakFight(int fight_seq)
{	
	this->ExecuteATask(TASK_NEED_MEET_REQUIRES, TASK_MEET_REALM_BREAK_FIGHT, fight_seq);
}

void TaskManager::OnRoleStrParnter(int partner_id)
{
	this->ExecuteATask(TASK_NEED_PARTNER_STRENGTHEN, partner_id, 1);
}

void TaskManager::OnRoleParnterBreak(int partner_id)
{
	this->ExecuteATask(TASK_NEED_PARTNER_BREAK, partner_id, 1);
}

void TaskManager::OnCommitPet(int task_id, int pet_index)
{
	const TaskInfomaConfig * p_tic = TASKPOOL->GetTaskCfgByID(task_id);
	if (! p_tic )
	{
		return;
	}

	if (p_tic->task_condition != TASK_NEED_COMMIT_PET)
	{
		return;
	}

	const TaskInfo * task_info = m_task_param.GetTaskByID(task_id);
	if (!task_info)
	{
		return; // 如果身上没有这个任务，就不要提交物品了
	}

	const PetParam * pet_param = m_mgr->GetPet()->GetPetParam(pet_index);
	if (!pet_param)
	{
		m_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	if (!m_mgr->GetPet()->SystemTakeBackPet(pet_index, p_tic->param1))
	{
		return;
	}
	
	this->ExecuteATask(task_id, 1);
}

bool TaskManager::CheckTeamForTask(TaskID task_id, bool notify)
{
	const TaskInfomaConfig * task_cfg = TASKPOOL->GetTaskCfgByID(task_id);
	if (NULL == task_cfg) return false;

	if (TASK_TEAM_UNCONFINED == task_cfg->team_limit_type) return true;	// 如果本身没有限制，直接返回

	Team * p_team = TeamManager::Instance().GetTeamByUid(task_role->GetUID());
	if (NULL == p_team)		// 如果没有队伍
	{
		if (TASK_TEAM_MUST_ALONE == task_cfg->team_limit_type)
		{
			return true;
		}
		else
		{
			if (notify) m_mgr->NoticeNum(errornum::EN_TASK_CONDI_NEED_MORES);
			return false;
		}
	}

	UNSTD_STATIC_CHECK(6 == TASK_TEAM_TYPE_MAX)
		switch (task_cfg->team_limit_type)
		{
		case TASK_TEAM_MUST_ALONE:
			{
				if (p_team->GetMemberCount() > TASK_TEAM_MUST_ALONE)
				{
					if (notify) m_mgr->NoticeNum(errornum::EN_TASK_CONDI_MUST_ALONE);
					return false;
				}
			}
			break;

		case TASK_TEAM_AT_LEAST_2:
		case TASK_TEAM_AT_LEAST_3:
		case TASK_TEAM_AT_LEAST_4:
		case TASK_TEAM_AT_LEAST_5:
			{
				if (p_team->GetMemberCount() < task_cfg->team_limit_type)
				{
					if (notify) m_mgr->NoticeNum(errornum::EN_TASK_CONDI_NEED_MORES);
					return false;
				}
			}
			break;

		default:
			{
				if (notify) m_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
				return false;
			}
			break;
		}

	return true;
}

bool TaskManager::CheckRoleHasTask(TaskID task_id)
{
	return NULL != m_task_param.GetTaskByID(task_id);
}

void TaskManager::SendSummaryTaskList()
{
#ifdef _DEBUG
	//printf("TaskManager::SendSummaryTaskList role[%d,%s]\n",
	//	m_mgr->GetRole()->GetUID(), m_mgr->GetRole()->GetName());
#endif

	this->SendTaskTraceListInfo();

	Protocol::SCTaskDetailedInfoList tdil;
	tdil.list_count = 0;

	// 储存已接列表
	for (int acpted_cur = 0; acpted_cur < m_task_param.task_count; ++acpted_cur)
	{
		if (tdil.list_count >= MAX_TASK_LIST_LEN_NUM) break;

		Protocol::SCTaskDetailedInfoList::TaskProCon & task_ptc = tdil.task_list_info[tdil.list_count++];
		task_ptc.list_type = TASK_LIST_TYPE_ACCEPTED;
		this->TaskDataToProtocol(&task_ptc.task_con, m_task_param.task_info_list[acpted_cur]);
	}

	// 储存可接任务列表
	for (std::set<TaskID>::iterator s_it = m_acceptable_set.begin(); s_it != m_acceptable_set.end(); ++s_it)
	{
		if (tdil.list_count >= MAX_TASK_LIST_LEN_NUM) break;

		Protocol::SCTaskDetailedInfoList::TaskProCon & task_ptc = tdil.task_list_info[tdil.list_count++];
		task_ptc.list_type = TASK_LIST_TYPE_ACCEPTABLE;
		task_ptc.task_con.task_id = *s_it;
		task_ptc.task_con.state_type = 0;
		task_ptc.task_con.prograss = 0;
		task_ptc.task_con.expire_time = 0;
	}

	int send_len = sizeof(tdil) - (MAX_TASK_LIST_LEN_NUM - tdil.list_count) * sizeof(Protocol::SCTaskDetailedInfoList::TaskProCon);
	if (send_len > 0) m_mgr->NetSend((const char *)&tdil, send_len);
}

void TaskManager::SendWaitingTaskList()
{
	Protocol::SCTaskWaitingList msg;
	msg.count = 0;

	std::map<TaskID, unsigned int>::iterator it = m_waiting_map.begin();
	for (; it != m_waiting_map.end() 
		&& msg.count < ARRAY_ITEM_COUNT(msg.task_list); ++it)
	{
		msg.task_list[msg.count].task_id = it->first;
		msg.task_list[msg.count].wait_until_timestamp = it->second;
		msg.task_list[msg.count].reserve_sh = 0;
		msg.count += 1;
	}

	int send_len = sizeof(msg) - sizeof(msg.task_list) + msg.count * sizeof(msg.task_list[0]);
	m_mgr->NetSend(&msg, send_len);
}

void TaskManager::OnRoleLevelUp()
{
	this->CalcAcceptableTask(ACB_RECALC_ON_LEVEL_UP);
}

void TaskManager::OnProfPromote()
{
	this->ExecuteATask(TASK_NEED_MEET_REQUIRES, TASK_MEET_PROF_PROMOTE, task_role->GetProfession());
	this->CalcAcceptableTask(ACB_RECALC_PROF_CHANGE);
}

void TaskManager::OnTeamConvert()
{
	Team * p_team = TeamManager::Instance().GetTeamByUid(task_role->GetUID());
	int identify_label = (NULL == p_team) ? GIVEUP_TYPE_ONESELF_CAN :
		(task_role->GetUID() == p_team->GetLeaderUID()) ? GIVEUP_TYPE_CAPTAIN_CAN : GIVEUP_TYPE_MEMBERS_CAN;

	this->RefreshTaskCanGiveUp(identify_label);
}

void TaskManager::OnTaskBattleFinish(int group_id, int battle_mode)
{
	switch (battle_mode)
	{
	case BATTLE_MODE_REALM_BREAK_FIGHT:
		{
			const RealmBreakFightCfg* cfg = LOGIC_CONFIG->GetRealmBreakConfig()->MonsterGroupGetFightCft(group_id);
			if (NULL == cfg) break;

			m_mgr->GetRealmBreak()->OnBattleFinish(true, cfg->seq);
		}
		break;
	default:
		break;
	}
}

void TaskManager::GmTaskManagerOrder(int op_type, int param1, int param2)
{
	enum TaskManagerOrderType
	{
		TASK_MGR_SKIPS_TO_TASK = 0,		// 跳转到指定任务并完成所有前置 -> P1: 任务ID | P2: 是否强制
		TASK_MGR_SET_TASK_PROG = 1,		// 设置某个已接任务的进度 -> P1: 任务ID | P2: 进度值
		TASK_MGR_RAND_ADV_TASK = 2,		// 随机一次奇遇任务
		TASK_MGR_CHECK_CAN_ACC = 3,		// 检查某任务是否可接 -> P1: 任务ID
		TASK_MSR_ACCEPT_ADV_TASK = 4,	// 领取一个指定的奇遇任务 P1: 任务ID
		TASK_MSR_ADV_TIME_OUT = 5,		// 奇遇任务设置超时
	};

	switch (op_type)
	{
	case TASK_MGR_SKIPS_TO_TASK:
		{
			int task_id = param1;
			const TaskInfomaConfig * task_cfg = TASKPOOL->GetTaskCfgByID(task_id);
			if (NULL == task_cfg) return;

			if (0 == param2)
			{
				if (m_mgr->GetTaskRecorder()->HaveFinished(task_id))
				{
					m_mgr->NoticeNum(errornum::EN_TASK_ALREADY_FINISHED);
					return;
				}

				if (NULL != m_task_param.GetTaskByID(task_id))
				{
					m_mgr->NoticeNum(errornum::EN_TASK_ALREADY_ACCEPTED);
					return;
				}
				// 检查最低等级限制 - 如果是主线，则跳过等级检测
				if (!IsTaskTypeAcceptIgnoreLevelLimit(task_cfg->task_type) && task_cfg->level_min > 0 && task_role->GetLevel() < task_cfg->level_min)
				{
					m_mgr->NoticeNum(errornum::EN_ROLE_LEVEL_TOO_LOW);
					return;
				}
				// 检查最高等级限制
				if (task_cfg->level_max > 0 && task_role->GetLevel() > task_cfg->level_max)
				{
					m_mgr->NoticeNum(errornum::EN_ROLE_LEVEL_OVERTOP);
					return;
				}
				// 检查职业是否符合要求
				if (task_cfg->prof_limit_min != PROF_TYPE_PROF_NO_LIMIT
					&& (task_role->GetProfession() < task_cfg->prof_limit_min || task_role->GetProfession() > task_cfg->prof_limit_max))
				{
					m_mgr->NoticeNum(errornum::EN_ROLE_OCCUP_NOT_FIT);
					return;
				}
			}

			this->GmPreTaskForceFinish(task_id);
			this->CalcAcceptableTask(ACB_RECALC_INITIALIZES);
			this->SendSummaryTaskList();
			this->SendWaitingTaskList();

			{
				const TaskInfomaConfig * next_cfg = TASKPOOL->GetTaskCfgByID(task_cfg->nex_task_id);
				if (NULL != next_cfg)
				{
					if (task_role->GetLevel() < next_cfg->level_min)
					{
						task_role->GmSetRoleLevel(next_cfg->level_min);
					}
				}
			}
		}
		break;
	case TASK_MGR_SET_TASK_PROG:
		{
			int task_id = param1, prog_num = param2;
			for (int t_cur = 0; t_cur < m_task_param.task_count; ++t_cur)
			{
				TaskInfo & p_ti = m_task_param.task_info_list[t_cur];
				if (task_id != p_ti.task_id) continue;

				const TaskInfomaConfig * task_cfg = TASKPOOL->GetTaskCfgByID(task_id);
				if (NULL == task_cfg) return;

				if (prog_num > task_cfg->param2) prog_num = task_cfg->param2;
				p_ti.prog_num = prog_num;

				if (this->CanCommitTask(task_cfg->task_id))
				{
					ROLE_LOG_QUICK6(LOG_TYPE_COMPLETE_TASK, task_role, task_cfg->task_type, task_cfg->task_id, NULL, NULL);

					gamelog::g_log_task_stats.printf(LL_INFO, "[TaskResult::Complete][role[%d %s] task_id:%d task_level:%d task_type:%d role_professional:%d]",
						UidToInt(task_role->GetUserId()), task_role->GetName(), task_cfg->task_id, task_cfg->level_min, task_cfg->task_type, task_role->GetProfession());

					if (CrossConfig::Instance().IsHiddenServer())
					{
						World::GetInstWorld()->SyncLogToOriginServer(m_mgr->GetRole()->GetUserId().db_index, SYNC_LOG_TYPE_TASK_STAT,
							"[TaskResult::Complete][role[%d %s] task_id:%d task_level:%d task_type:%d role_professional:%d]",
							UidToInt(task_role->GetUserId()), task_role->GetName(), task_cfg->task_id, task_cfg->level_min, task_cfg->task_type, task_role->GetProfession());
					}
				}

				this->SendSingleTaskInfo(task_id, TASK_LIST_TYPE_ACCEPTED);

				if (task_cfg->can_auto_commit)
				{
					this->CheckAutoCommit(task_id, task_cfg->commit_npc);
				}

				break;
			}
		}
		break;
	case TASK_MGR_RAND_ADV_TASK:
		{
			m_task_param.adv_task_id = TASKPOOL->GetAdventureTaskByGroupID(TASKPOOL->RandAdventureTask(task_role->GetLevel()));
			if (0 == m_task_param.adv_task_id) return;	// 任务获取失败，没随机到任务；

			if (!this->CanAcceptTask(m_task_param.adv_task_id, true))
			{
				m_task_param.adv_task_id = 0;
				return;
			}

			++m_task_param.today_adv_tri_times;
			//printf("\nAdvTask: %d | TodayTimes: %d", m_task_param.adv_task_id, m_task_param.today_adv_tri_times);
			m_acceptable_set.insert(m_task_param.adv_task_id);
			this->SendSingleTaskInfo(m_task_param.adv_task_id, TASK_LIST_TYPE_ACCEPTABLE);
			this->OnAcceptTask(m_task_param.adv_task_id);
		}
		break;
	case TASK_MGR_CHECK_CAN_ACC:
		this->CanAcceptTask(param1, true);
		break;
	case TASK_MSR_ACCEPT_ADV_TASK:
		{
			m_task_param.adv_task_id = param1;
			if (!this->CanAcceptTask(m_task_param.adv_task_id, true))
			{
				m_task_param.adv_task_id = 0;
				return;
			}
			++m_task_param.today_adv_tri_times;
			m_acceptable_set.insert(m_task_param.adv_task_id);
			this->SendSingleTaskInfo(m_task_param.adv_task_id, TASK_LIST_TYPE_ACCEPTABLE);
			this->OnAcceptTask(m_task_param.adv_task_id);
		}
		break;
	case TASK_MSR_ADV_TIME_OUT:
		{
			if (0 != m_task_param.adv_task_id)
			{
				m_task_param.adv_task_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());
			}
		}
		break;
	default:
		m_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		break;
	}
}

bool TaskManager::OnAcceptTask(TaskID task_id, bool notify)
{
	// 如果不能接受任务，说明条件不符合，返回
	if (!this->CanAcceptTask(task_id, notify)) return false;

	// 开始接受任务 - 若任务成功接受，注意后续不能中途返回
	const TaskInfomaConfig * t_ti = TASKPOOL->GetTaskCfgByID(task_id);
	if (NULL == t_ti) return false;

	UInt32 time_limit = 0;
	if (TASK_TYPE_ADVENTURES == t_ti->task_type)		// 奇遇任务的计时逻辑不同
	{	// 如果能找到限时，则刷新限时，否则保留当前限时；
		int adv_limit = t_ti->timer_limit * SECOND_PER_MIN;
		if (adv_limit > 0) m_task_param.adv_task_timestamp = (UInt32)(m_nowTime + adv_limit);

		time_limit = m_task_param.adv_task_timestamp;
	}
	else if (t_ti->timer_limit > 0)
	{
		time_limit = (UInt32)(m_nowTime + t_ti->timer_limit);
	}
	else if (TASK_NEED_WAIT_UNTIL_NEXT_DAY == t_ti->task_condition)
	{
		//#ifdef _MSC_VER
		//		time_limit = static_cast<unsigned int>(m_nowTime + 60);
		//#else
		time_limit = static_cast<unsigned int>(m_nowTime + EngineAdapter::Instance().NextDayInterval(0, 0, 0));
		//#endif
	}

	if (!m_task_param.InsertTask(TaskInfo(task_id, t_ti->task_ver, time_limit)))
	{
		if (notify) m_mgr->NoticeNum(errornum::EN_TASK_LIST_ALREADY_MAX);
		return false;
	}
	if (TASK_TYPE_PARTNER_SOAR_TASK == t_ti->task_type)
	{
		m_task_param.partner_soar_task_status = PARTNER_SOAR_TASK_STATUS_ACCEPT;
		m_task_param.cur_partner_soar_task_id = t_ti->task_id;
		if (0 == t_ti->pre_task_id)	//再检测一遍,去除其它可接任务
		{
			m_task_param.is_cur_accept_partner_soar_task = 1;
			this->OnSendPartnerSoarInfo();
			this->CalcAcceptableTask(ACB_RECALC_TASK_TYPE, TASK_TYPE_PARTNER_SOAR_TASK);
		}
	}

	m_acceptable_set.erase(task_id);
	if (notify)
	{
		this->SendSingleTaskInfo(task_id, TASK_LIST_TYPE_ACCEPTED);
		this->OnTeamConvert();
	}

	unsigned int wait_timestamp = 0;
	if (this->IsTaskNeedWaiting(t_ti, &wait_timestamp))
	{
		m_waiting_map[task_id] = wait_timestamp;
		if (notify) this->SendWaitingTaskList();
	}

	ROLE_LOG_QUICK6(LOG_TYPE_ACCEPT_TASK, task_role, t_ti->task_type, task_id, NULL, NULL);

	{
		gamelog::g_log_task.printf(LL_DEBUG, "[TaskManager::AddTask][user[%d %s] task_id:%d task_count:%d]",
			task_role->GetUID(), task_role->GetName(), (int)task_id, m_task_param.task_count);

		if (CrossConfig::Instance().IsHiddenServer())
		{
			World::GetInstWorld()->SyncLogToOriginServer(m_mgr->GetRole()->GetUserId().db_index, SYNC_LOG_TYPE_TASK,
				"[TaskManager::AddTask][user[%d %s] task_id:%d task_count:%d]",
				task_role->GetUID(), task_role->GetName(), (int)task_id, m_task_param.task_count);
		}

		gamelog::g_log_task_stats.printf(LL_INFO, "[TaskResult::accept][role[%d %s] task_id:%d task_level:%d task_type:%d role_professional:%d]",
			task_role->GetUID(), task_role->GetName(), (int)task_id, t_ti->level_min, t_ti->task_type, task_role->GetProfession());

		if (CrossConfig::Instance().IsHiddenServer())
		{
			World::GetInstWorld()->SyncLogToOriginServer(m_mgr->GetRole()->GetUserId().db_index, SYNC_LOG_TYPE_TASK_STAT,
				"[TaskResult::accept][role[%d %s] task_id:%d task_level:%d task_type:%d role_professional:%d]",
				task_role->GetUID(), task_role->GetName(), (int)task_id, t_ti->level_min, t_ti->task_type, task_role->GetProfession());
		}
	}

	EventHandler::Instance().OnAcceptTask(m_mgr->GetRole(), task_id);

	this->RefreshTaskProgNum(task_id, notify);

	return true;
}

void TaskManager::OnCommitTask(TaskID task_id)
{
	// 如果不能提交任务，说明任务未完成，返回
	if (!this->CanCommitTask(task_id, true)) return;

	// 否则说明任务完成，开始发放任务报酬
	const TaskInfomaConfig * task_cfg = TASKPOOL->GetTaskCfgByID(task_id);
	if (NULL == task_cfg) return;

	// 发放任务报酬
	const CommonTaskReward * ctr_cfg = TASKPOOL->GetCommonTaskReward(task_id, task_role);
	if (NULL != ctr_cfg)	// 允许出现没有奖励的任务，所以当奖励为空时不返回。
	{
		if (ctr_cfg->reward_count > 0 && !m_mgr->GetKnapsack()->CheckForPutList(ctr_cfg->reward_count, ctr_cfg->reward_list))
		{
			return; // 没有足够位置发放任务报酬
		}
	}

	if (TASKPOOL->GetTaskOtherCfg().career_rewards == task_id)
	{	// 检查背包
		const std::vector<ItemConfigData>* reward_list = TASKPOOL->GetSpecialRewardCfg(m_mgr->GetRole()->GetBaseProfession());
		if (NULL != reward_list)
		{
			if(!m_mgr->GetKnapsack()->CheckForPutList2(*reward_list)) return;
		}
	}

	if (TASK_NEED_COMMIT_PROPERTY == task_cfg->task_condition)
	{
		if (!this->OnCommitProperty(task_id, task_cfg->param1, task_cfg->param2))
		{
			return; // 如果提交物品失败，则返回
		}
	}
	
	if (TASKPOOL->GetTaskOtherCfg().career_rewards == task_id)
	{		// 特殊逻辑奖励
		const std::vector<ItemConfigData>* reward_list = TASKPOOL->GetSpecialRewardCfg(m_mgr->GetRole()->GetBaseProfession());
		if (NULL != reward_list)
		{
			m_mgr->GetKnapsack()->PutList2((short)reward_list->size(), &(*reward_list)[0], PUT_REASON_TASK_REWARD, GET_REASON_TASK, task_id);
		}
	}

	if (NULL != ctr_cfg)
	{
		if (ctr_cfg->reward_count > 0)
		{
			m_mgr->GetKnapsack()->PutList2(ctr_cfg->reward_count, ctr_cfg->reward_list, PUT_REASON_TASK_REWARD, GET_REASON_TASK, task_id);

			if (task_cfg->task_type == TASK_TYPE_SERIAL_FEEDER_TASK || task_cfg->task_type == TASK_TYPE_SERIAL_FEEDER_TASK2 || task_cfg->task_type == TASK_TYPE_SERIAL_FEEDER_TASK_HOLY)
			{
				m_mgr->GetKnapsack()->NotifyGetItemList(ctr_cfg->reward_count, ctr_cfg->reward_list, GET_REASON_LIANHUAN_TASK, task_id, ctr_cfg->exp);
			}
		}

		if (TASKPOOL->GetTaskOtherCfg().career_rewards == task_id)
		{
			const std::vector<ItemConfigData>* reward_list = TASKPOOL->GetSpecialRewardCfg(m_mgr->GetRole()->GetBaseProfession());
			if (NULL != reward_list)
			{
				m_mgr->GetKnapsack()->PutList2((short)reward_list->size(), &(*reward_list)[0], PUT_REASON_TASK_REWARD, GET_REASON_TASK, task_id);
			}
		}

		m_mgr->GetMoney()->Add(ctr_cfg->gold, ctr_cfg->immortal, ctr_cfg->coin, "CommonTaskReward");
		if (ctr_cfg->exp > 0)
		{
			long long real_add_exp = task_role->AddExp(ctr_cfg->exp, ADD_EXP_TYPE_TASK);
			if (task_cfg->task_type == TASK_TYPE_BOUNTY_TASK)
			{
				m_mgr->GetBountyTask()->OnAddDayExp(task_id, real_add_exp);
				m_mgr->GetLevelComplement()->OnAddExp(real_add_exp);
				EventHandler::Instance().OnFinishLevelComplementRelatedFun(m_mgr->GetRole(), ctr_cfg->exp, __FUNCTION__);
			}
		}
		if (ctr_cfg->repu_numb > 0) m_mgr->GetPrestige()->AddPrestige(ctr_cfg->repu_city, ctr_cfg->repu_numb, __FUNCTION__);
		if (ctr_cfg->pet_exp > 0)
		{
			int fight_pet_idx = task_role->GetRoleModuleManager()->GetPet()->GetFightIndex();
			if (fight_pet_idx >= 0) task_role->GetRoleModuleManager()->GetPet()->AddExp(fight_pet_idx, ctr_cfg->pet_exp, __FUNCTION__, Protocol::SCSendPetSingleInfo::PET_NOTIE_TYPE_EXP);
		}
		if (ctr_cfg->power > 0)
		{
			m_mgr->GetRoleCrystalFb()->AddCrystalFbEnergy(ctr_cfg->power, __FUNCTION__, ENTER_ENERGY_CHANGE_TYPE_OTHER);
		}
	}

	// 任务报酬已发放，注意不能中途返回
	m_task_param.RemoveTask(task_id);
	this->SendSingleTaskDelete(task_id, TASK_REMOVE_REASON_FINISH);

	m_mgr->GetTaskRecorder()->TaskComplete(task_id);				// 记录任务完成；
	this->CalcAcceptableTask(ACB_RECALC_TASK_COMMIT, task_id);		// 必须在记录任务完成后面，因为这里面要删除这个任务；

	EventHandler::Instance().OnCommitTask(task_role, task_id);

	m_old_give_up_state.erase(task_id);

	gamelog::g_log_task_stats.printf(LL_INFO, "[TaskResult::finish][role[%d %s] task_id:%d task_level:%d task_type:%d role_professional:%d]",
		UidToInt(task_role->GetUserId()), task_role->GetName(), (int)task_id, task_cfg->level_min, task_cfg->task_type, (int)task_role->GetProfession());

	if (CrossConfig::Instance().IsHiddenServer())
	{
		World::GetInstWorld()->SyncLogToOriginServer(m_mgr->GetRole()->GetUserId().db_index, SYNC_LOG_TYPE_TASK_STAT,
			"[TaskResult::finish][role[%d %s] task_id:%d task_level:%d task_type:%d role_professional:%d]",
			UidToInt(task_role->GetUserId()), task_role->GetName(), (int)task_id, task_cfg->level_min, task_cfg->task_type, (int)task_role->GetProfession());
	}

	ROLE_LOG_QUICK6(LOG_TYPE_COMMIT_TASK, task_role, task_cfg->task_type, task_id, NULL, NULL);
}

void TaskManager::OnGiveupTask(TaskID task_id, int type)
{
	bool is_from_update = (type == TASK_MANAGER_GIVE_UP_TYPE_UPDATE);
	const TaskInfomaConfig * p_tic = TASKPOOL->GetTaskCfgByID(task_id);
	if (p_tic == NULL)return;

	if (p_tic->task_type != TASK_TYPE_ADVENTURES && this->CanCommitTask(task_id))		// 已完成的任务不能放弃,除奇遇任务外
	{
		if (!is_from_update)
		{
			m_mgr->NoticeNum(errornum::EN_TASK_MISSION_COMPLETE);
		}
		return;
	}
	if (TASK_TYPE_PARTNER_SOAR_TASK == p_tic->task_type)
	{
		m_mgr->NoticeNum(errornum::EN_TASK_NOT_GIVE_UP_TASK);
		return;
	}

	m_mgr->GetEscort()->GiveUpAdvTask(task_id);
	m_mgr->GetRoleGuildTask()->OnGiveUpTask(task_id, type);

	if (TASK_TYPE_ADVENTURES == p_tic->task_type)
	{
		m_task_param.adv_task_id = 0;	// 放弃奇遇任务，就没有了；
		m_task_param.adv_task_timestamp = 0;
		m_mgr->GetTaskRecorder()->ClearAppoiontedTypeTaskRecord(TASK_TYPE_ADVENTURES);
	}
	else if (TASK_NEED_ESCORT == p_tic->task_condition)
	{
		m_mgr->GetEscort()->GiveupTask();
	}

	m_task_param.RemoveTask(task_id);
	if (this->CanAcceptTask(task_id))	// 如果这个任务是可接的，则插入玩家可接列表内
	{
		// 可接就立刻接上任务，否则才放入可接列表
		if (!this->OnAcceptTask(task_id))
		{
			m_acceptable_set.insert(task_id);
			this->SendSingleTaskInfo(task_id, TASK_LIST_TYPE_ACCEPTABLE);
		}		
	}
	else		// 否则直接通知删除
	{
		this->SendSingleTaskDelete(task_id, TASK_REMOVE_REASON_OTHERS);
	}
	if (TASK_TYPE_PARTNER_SOAR_TASK == p_tic->task_type)
	{
		m_task_param.partner_soar_task_status = PARTNER_SOAR_TASK_STATUS_BEGIN;
		m_task_param.cur_partner_soar_task_id = 0;
		m_task_param.is_cur_accept_partner_soar_task = 0;
		this->CalcAcceptableTask(ACB_RECALC_TASK_TYPE, TASK_TYPE_PARTNER_SOAR_TASK);
		this->OnSendPartnerSoarInfo();
	}

	EventHandler::Instance().OnGiveupTask(m_mgr->GetRole(), task_id);

	m_old_give_up_state.erase(task_id);

	ROLE_LOG_QUICK6(LOG_TYPE_GIVEUP_TASK, task_role, p_tic->task_type, task_id, NULL, NULL);

	gamelog::g_log_task_stats.printf(LL_INFO, "[TaskResult::quit][role[%d %s] task_id:%d task_level:%d task_type:%d role_profession:%d]",
		UidToInt(task_role->GetUserId()), task_role->GetName(), (int)task_id, p_tic->level_min, p_tic->task_type, (int)task_role->GetProfession());

	if (CrossConfig::Instance().IsHiddenServer())
	{
		World::GetInstWorld()->SyncLogToOriginServer(m_mgr->GetRole()->GetUserId().db_index, SYNC_LOG_TYPE_TASK_STAT,
			"[TaskResult::quit][role[%d %s] task_id:%d task_level:%d task_type:%d role_profession:%d]",
			UidToInt(task_role->GetUserId()), task_role->GetName(), (int)task_id, p_tic->level_min, p_tic->task_type, (int)task_role->GetProfession());
	}
}

void TaskManager::ExecuteATask(TaskID task_id, int prog_num)
{
	const TaskInfomaConfig * task_cfg = TASKPOOL->GetTaskCfgByID(task_id);
	if (NULL == task_cfg || task_role->GetLevel() < task_cfg->level_min) return;

	if (this->IsTaskNeedWaiting(task_cfg)) return;

	unsigned int now = static_cast<unsigned int>(m_nowTime);
	for (int t_cur = 0; t_cur < m_task_param.task_count; ++t_cur)
	{
		TaskInfo & p_task = m_task_param.task_info_list[t_cur];
		if (p_task.task_id != task_id || (0 != p_task.expire_time && now > p_task.expire_time)) continue;
		if (TASK_NEED_MEET_REQUIRES != task_cfg->task_condition
			&& TASK_NEED_ENTER_FB != task_cfg->task_condition
			&& p_task.prog_num >= task_cfg->param2) return;

		int result = p_task.prog_num + prog_num;
		p_task.prog_num = result > task_cfg->param2 ? task_cfg->param2 : result;

		if (this->CanCommitTask(task_id))
		{
			ROLE_LOG_QUICK6(LOG_TYPE_COMPLETE_TASK, task_role, task_cfg->task_type, task_cfg->task_id, NULL, NULL);

			gamelog::g_log_task_stats.printf(LL_INFO, "[TaskResult::Complete][role[%d %s] task_id:%d task_level:%d task_type:%d role_professional:%d]",
				UidToInt(task_role->GetUserId()), task_role->GetName(), task_cfg->task_id, task_cfg->level_min, task_cfg->task_type, task_role->GetProfession());

			if (CrossConfig::Instance().IsHiddenServer())
			{
				World::GetInstWorld()->SyncLogToOriginServer(m_mgr->GetRole()->GetUserId().db_index, SYNC_LOG_TYPE_TASK_STAT,
					"[TaskResult::Complete][role[%d %s] task_id:%d task_level:%d task_type:%d role_professional:%d]",
					UidToInt(task_role->GetUserId()), task_role->GetName(), task_cfg->task_id, task_cfg->level_min, task_cfg->task_type, task_role->GetProfession());
			}
		}

		this->SendSingleTaskInfo(task_id, TASK_LIST_TYPE_ACCEPTED);

		if (task_cfg->can_auto_commit)
		{
			this->CheckAutoCommit(task_id, task_cfg->commit_npc);
		}

		break;
	}
}

bool CheckIsContinue(int condition, int p1, int p2, const TaskInfomaConfig * task_cfg, const TaskInfo & p_task)
{
	UNSTD_STATIC_CHECK(46 == TASK_NEED_MAX);
	UNSTD_STATIC_CHECK(26 == TASK_MEET_TYPE_MAX);
	bool is_continue = false;
	switch (condition)
	{
		case TASK_NEED_CAREER_ADVANCE:
		{
			if (p_task.prog_num >= 1)
			{
				is_continue = true;
			}
		}
		break;
		case TASK_NEED_PLAY_GAME:
		{
			if (p1 != task_cfg->param1)
			{
				is_continue = true;
			}
			else
			{
				if (p_task.prog_num >= task_cfg->param2)
				{
					is_continue = true;
				}
			}
		}
		break;
	case TASK_NEED_MEET_REQUIRES:
		{
			switch (p1)
			{
			case TASK_MEET_GOD_PRINT_WISH:
			case TASK_MEET_DREAM_NOTES_CHALLENGE:
			case TASK_MEET_BOUNTY_TASK:
			case TASK_MEET_RESEARCH_TASK:
			case TASK_MEET_SKILL_LEVEL:
			case TASK_MEET_TREASURE_TASK:
			case TASK_MEET_BOUNTY_LAST_TASK:
			case TASK_MEET_TREASURE_LAST_TASK:
			case TASK_MEET_WANLING_FB_TIMES:
			case TASK_MEET_KILL_WILD_BOSS:
				{
					if (p_task.prog_num >= task_cfg->param2)
					{
						is_continue = true;
					}
				}
				break;
			default:
				{
					if (p_task.prog_num >= 1)
					{
						is_continue = true;
					}
				}
				break;
			}
		}
		break;
	case TASK_NEED_ENTER_FB:
	case TASK_NEED_COURAGE_CHALLENGE:
		{
			if (p_task.prog_num >= 1)
			{
				is_continue = true;
			}
		}
		break;
	case TASK_NEED_ROLE_BEAT_MONGROUP:
	{
		if (p_task.prog_num != 0)
		{
			is_continue = true;
		}
	}
	break;
	case TASK_NEED_COMMIT_PET:
	{
		if (p_task.prog_num != 0)
		{
			is_continue = true;
		}
	}
	break;
	default:
		{
			if (p_task.prog_num >= task_cfg->param2)
			{
				is_continue = true;
			}
		}
		break;
	}
	return is_continue;
}

void TaskManager::ExecuteATask(int condition, int p1, int p2)
{
	if (condition >= TASK_NEED_MAX || condition < 0 || TASK_NEED_TALK_WITH_NPC == condition
		|| TASK_NEED_GIVE_PROGROUP == condition || TASK_NEED_GIVE_PROPERTY == condition) return;

	unsigned int now = static_cast<unsigned int>(m_nowTime);
	for (int t_cur = 0; t_cur < m_task_param.task_count; ++t_cur)
	{
		TaskInfo & p_task = m_task_param.task_info_list[t_cur];
		if ((0 != p_task.expire_time && now > p_task.expire_time) || !this->CheckTeamForTask(p_task.task_id, false)) continue;

		const TaskInfomaConfig * task_cfg = TASKPOOL->GetTaskCfgByID(p_task.task_id);
		if (NULL == task_cfg || condition != task_cfg->task_condition || task_role->GetLevel() < task_cfg->level_min) continue;

		if (this->IsTaskNeedWaiting(task_cfg)) continue;

		// 已完成的任务不检测，防止死循环
		bool is_continue = CheckIsContinue(condition, p1, p2, task_cfg, p_task);
		if (is_continue) continue;

		this->ExecuteATaskForCondition(condition, p1, p2, task_cfg, p_task);

		if (this->CanCommitTask(task_cfg->task_id))
		{
			ROLE_LOG_QUICK6(LOG_TYPE_COMPLETE_TASK, task_role, task_cfg->task_type, task_cfg->task_id, NULL, NULL);

			gamelog::g_log_task_stats.printf(LL_INFO, "[TaskResult::Complete][role[%d %s] task_id:%d task_level:%d task_type:%d role_professional:%d]",
				UidToInt(task_role->GetUserId()), task_role->GetName(), task_cfg->task_id, task_cfg->level_min, task_cfg->task_type, task_role->GetProfession());

			if (CrossConfig::Instance().IsHiddenServer())
			{
				World::GetInstWorld()->SyncLogToOriginServer(m_mgr->GetRole()->GetUserId().db_index, SYNC_LOG_TYPE_TASK_STAT,
					"[TaskResult::Complete][role[%d %s] task_id:%d task_level:%d task_type:%d role_professional:%d]",
					UidToInt(task_role->GetUserId()), task_role->GetName(), task_cfg->task_id, task_cfg->level_min, task_cfg->task_type, task_role->GetProfession());
			}
		}

		this->SendSingleTaskInfo(p_task.task_id, TASK_LIST_TYPE_ACCEPTED);

		if (task_cfg->can_auto_commit)
		{
			this->CheckAutoCommit(p_task.task_id, task_cfg->commit_npc);
		}
	}
}

void TaskManager::AdventureTaskTriggerJudge(int adv_type)
{
	const TaskInfomaConfig * task_cfg = TASKPOOL->GetTaskCfgByID(m_task_param.adv_task_id);		// 如果已有奇遇任务，则不随机；
	if (m_mgr->GetRole()->IsAutoGather() || (NULL != task_cfg && TASK_TYPE_ADVENTURES == task_cfg->task_type && this->CheckRoleHasTask(m_task_param.adv_task_id))) return;

	const AdventruesOthers & adv_other = TASKPOOL->GetAdvTaskOtherCfg();	// 如果超过每日限次，则不随机；
	if (m_task_param.today_adv_tri_times >= adv_other.limit_times_daily) return;
	if (m_mgr->GetRole()->GetLevel() < adv_other.task_level_limit) return;

	int prob = rand() % 100;
	switch (adv_type)
	{
	case ADV_TYPE_DEFEAT:
		if (prob >= adv_other.basic_trigger_rate) return;
		break;
	case ADV_TYPE_TALK:
		if (prob >= adv_other.talk_rate) return;
		break;
	default:
		return;
	}
	std::set<int> unlocked_partner_set;
	m_mgr->GetPartner()->GetUnlockedPartnerSet(&unlocked_partner_set);
	int task_id = TASKPOOL->GetAdventureTaskByGroupID(TASKPOOL->RandAdventureTask(task_role->GetLevel(), &unlocked_partner_set));
	m_task_param.adv_task_id = task_id;
	if (0 == m_task_param.adv_task_id) return;	// 任务获取失败，没随机到任务；

	if (!this->CanAcceptTask(m_task_param.adv_task_id))
	{
		m_task_param.adv_task_id = 0;
		return;
	}

	++m_task_param.today_adv_tri_times;

	//m_acceptable_set.insert(m_task_param.adv_task_id);
	this->AcceptTask(m_task_param.adv_task_id);
	this->SendSingleTaskInfo(m_task_param.adv_task_id, TASK_LIST_TYPE_ACCEPTED);
	m_mgr->NoticeNum(noticenum::NT_TASK_ADV_ADD_SUCC);
}

bool TaskManager::OnUseItemToTriggerAdventureTask(int item_id)
{
	const TaskInfomaConfig * task_cfg = TASKPOOL->GetTaskCfgByID(m_task_param.adv_task_id);
	if (NULL != task_cfg && TASK_TYPE_ADVENTURES == task_cfg->task_type && this->CheckRoleHasTask(m_task_param.adv_task_id))
	{
		m_mgr->NoticeNum(errornum::EN_TASK_ALREADY_ACCEPTED);
		return false;
	}

	const AdventruesOthers & adv_other = TASKPOOL->GetAdvTaskOtherCfg();
	if (m_task_param.today_adv_tri_times >= adv_other.limit_times_daily)
	{
		m_mgr->NoticeNum(errornum::EN_TASK_QIYU_TIMES_MAX);
		return false;
	}

	int task_group_id = TASKPOOL->GetAdventureTaskByUsingItem(item_id, task_role->GetLevel());
	int task_id = TASKPOOL->GetAdventureTaskByGroupID(task_group_id);
	m_task_param.adv_task_id = task_id;
	if (0 == m_task_param.adv_task_id)
	{
		m_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return false;
	}

	if (!this->CanAcceptTask(m_task_param.adv_task_id, true))
	{
		m_task_param.adv_task_id = 0;
		return false;
	}

	++m_task_param.today_adv_tri_times;

	this->AcceptTask(m_task_param.adv_task_id);
	this->SendSingleTaskInfo(m_task_param.adv_task_id, TASK_LIST_TYPE_ACCEPTED);
	m_mgr->NoticeNum(noticenum::NT_TASK_ADV_ADD_SUCC);
	return true;
}

bool TaskManager::HasMainTask()
{
	return this->GetCurMainTaskID() > 0;
}

TaskID TaskManager::GetCurMainTaskID()
{
	for (int index = 0; index < m_task_param.task_count && index < ARRAY_LENGTH(m_task_param.task_info_list); ++index)
	{
		const TaskInfomaConfig * task_cfg = TASKPOOL->GetTaskCfgByID(m_task_param.task_info_list[index].task_id);
		if (task_cfg != NULL)
		{
			if (task_cfg->task_type == TASK_TYPE_MAIN_QUEST)
			{
				return task_cfg->task_id;
			}
		}
	}

	return 0;
}

void TaskManager::OnRoleLogin()
{
	//强制修复
	if (0 == m_mgr->GetCommonData().is_give_task_item_flag)
	{
		if (m_mgr->GetTaskRecorder()->HaveFinished(41134) && !m_mgr->GetTaskRecorder()->HaveFinished(41144))
		{
			if (!m_mgr->GetKnapsack()->HasItem(27329, 1))
			{
				ItemConfigData item(27329, true, 1);
				if (!m_mgr->GetKnapsack()->Put(item, PUT_REASON_BUG_REPAIR))
				{
					m_mgr->GetKnapsack()->SendMail(&item, 1, SEND_MAIL_TYPE_TASK_REISSUE_ITEM);
				}
			}
		}
		if (m_mgr->GetTaskRecorder()->HaveFinished(41167) && !m_mgr->GetTaskRecorder()->HaveFinished(41181))
		{
			if (!m_mgr->GetKnapsack()->HasItem(27330, 1))
			{
				ItemConfigData item(27330, true, 1);
				if (!m_mgr->GetKnapsack()->Put(ItemConfigData(27330, true, 1), PUT_REASON_BUG_REPAIR))
				{
					m_mgr->GetKnapsack()->SendMail(&item, 1, SEND_MAIL_TYPE_TASK_REISSUE_ITEM);
				}
			}
		}
		m_mgr->GetCommonData().is_give_task_item_flag = 1;
	}

	for (int i = 0; i < m_task_param.task_count; ++i)
	{
		TaskInfo & p_task = m_task_param.task_info_list[i];
		const TaskInfomaConfig * task_cfg = TASKPOOL->GetTaskCfgByID(p_task.task_id);
		if (NULL != task_cfg && task_cfg->can_auto_commit)
		{
			this->CheckAutoCommit(p_task.task_id, task_cfg->commit_npc);
		}
	}

	if (!FunOpen::IsFunOpen(m_mgr->GetRole(), FUN_TYPE_PATROLTASK)) return;

	this->AcceptPatrolTask();
}

void TaskManager::AcceptPatrolTask()
{
	if (m_mgr->GetCommonData().today_patrol_task == 1)return;
	m_mgr->GetCommonData().today_patrol_task = 1;
	this->SendPatrolTaskInfo();
}

void TaskManager::AddPatrolTaskProgress()
{
	if (m_mgr->GetCommonData().today_patrol_task != 1 || m_mgr->GetCommonData().patrol_task_reward == 1 || m_mgr->GetCommonData().patrol_task_status > 0)return;

	m_mgr->GetCommonData().patrol_task_progress += 1;
	const PatrolTaskCfg * cfg = LOGIC_CONFIG->GetSundryConfig()->GetPatrolTaskCfg();
	if (cfg == NULL)return;
	if (m_mgr->GetCommonData().patrol_task_progress >= cfg->kill_num)//完成任务
	{
		m_mgr->GetCommonData().patrol_task_status = 1;
		m_mgr->GetCommonData().patrol_task_coin = this->GetCoinByLevel(m_mgr->GetRole()->GetLevel());
		this->SendPatrolTaskInfo();
	}
}

void TaskManager::SendPatrolTaskInfo()
{
	Protocol::SCPatrolTaskInfo info;
	info.today_flag = m_mgr->GetCommonData().today_patrol_task;
	info.today_reward = m_mgr->GetCommonData().patrol_task_reward;
	info.task_progress = m_mgr->GetCommonData().patrol_task_progress;
	info.task_status = m_mgr->GetCommonData().patrol_task_status;
	info.task_coin = m_mgr->GetCommonData().patrol_task_coin;
	m_mgr->NetSend((const char *)&info, sizeof(info));
}

void TaskManager::ClearPetFeiShengTask()
{
	this->CheckTaskByType(TASK_TYPE_PET_FEI_SHENG_TASK, TASK_CHECK_OPERA_TYPE_CLEAR, 0);
	m_mgr->GetTaskRecorder()->ClearPetFeiShengRecord();
}

std::vector<int> TaskManager::GetPetFeiShengTask()
{
	 std::vector<int> ret;

	int num = 0;
	for (int task_idx = 0; task_idx < m_task_param.task_count && task_idx < MAX_ACCEPTED_TASK_NUM; task_idx++)
	{
		const TaskInfomaConfig * task_cfg = TASKPOOL->GetTaskCfgByID(m_task_param.task_info_list[task_idx].task_id);
		if (NULL != task_cfg && task_cfg->task_type == TASK_TYPE_PET_FEI_SHENG_TASK)
		{
			ret.push_back(task_cfg->task_id);
		}
	}

	return ret;
}

bool TaskManager::HasPetFeiShengTask()
{
	return !this->GetPetFeiShengTask().empty();
}

void TaskManager::FlyUpTask(int task_id)
{
	//检查今天完成飞升任务次数
	FlyUpSystem * fly_up_param = m_mgr->GetFlyUpSystem();
	if (NULL != fly_up_param)
	{
		int today_time = fly_up_param->GetTaskIdx();
		if (today_time >= MAX_FLY_UP_TASK_IDX)return;

		//检查是否已经接有飞升任务了，有就放弃刷新成现在的
		this->CheckTaskByType(TASK_TYPE_FLY_UP_TASK, TASK_CHECK_OPERA_TYPE_CLEAR, 0);

		this->AcceptTask(task_id);
		this->SendSingleTaskInfo(task_id, TASK_LIST_TYPE_ACCEPTED);
	}
}

void TaskManager::OnRoleFlyUpFinish()
{
	this->CalcAcceptableTask(ACB_RECALC_TASK_TYPE, TASK_TYPE_PARTNER_SOAR_TASK);
	this->CalcAcceptableTask(ACB_RECALC_TASK_TYPE, TASK_TYPE_FLY_UP_FRONT_TASK);
}

void TaskManager::OnPartnerSoarFinish()
{
	if (m_task_param.is_cur_accept_partner_soar_task <= 1)
	{
		m_task_param.partner_soar_task_status = PARTNER_SOAR_TASK_STATUS_FINISH;
	}
	else
	{
		m_task_param.partner_soar_task_status = PARTNER_SOAR_TASK_STATUS_BEGIN;
		m_task_param.cur_partner_soar_task_id = 0;
		m_task_param.is_cur_accept_partner_soar_task = 0;
		this->CalcAcceptableTask(ACB_RECALC_TASK_TYPE, TASK_TYPE_PARTNER_SOAR_TASK);
		this->OnSendPartnerSoarInfo();
	}
}

void TaskManager::OnSendPartnerSoarInfo()
{
	Protocol::SCNewTaskPartnerSoarInfo info;
	info.is_cur_accept_partner_soar_task = m_task_param.is_cur_accept_partner_soar_task;
	info.reserve_ch = 0;
	info.reserve_sh = 0;

	m_mgr->NetSend(&info, sizeof(info));
}

int TaskManager::GetCoinByLevel(int _role_level)
{
	return LOGIC_CONFIG->GetSundryConfig()->GetCoinByLevel(_role_level);
}

void TaskManager::CommitPatrolTask()
{
	if (m_mgr->GetCommonData().patrol_task_status == 1)
	{
		m_mgr->GetCommonData().patrol_task_reward = 1;
		m_mgr->GetCommonData().patrol_task_status = 2;
		//m_mgr->GetRole()->AddExp(m_mgr->GetCommonData().patrol_task_exp);
		m_mgr->GetMoney()->AddCoinBind(m_mgr->GetCommonData().patrol_task_coin, __FUNCTION__);
		this->SendPatrolTaskInfo();
	}
}

void TaskManager::ClearRoleGuildTask()
{
	this->CheckTaskByType(TASK_TYPE_ROLE_GUILD_TASK, TASK_CHECK_OPERA_TYPE_CLEAR, 0);
	m_mgr->GetTaskRecorder()->ClearRoleGuildTaskRecord();
}

bool TaskManager::HasRoleGuildTask()
{
	for (int task_idx = 0; task_idx < m_task_param.task_count && task_idx < MAX_ACCEPTED_TASK_NUM; task_idx++)
	{
		const TaskInfomaConfig * task_cfg = TASKPOOL->GetTaskCfgByID(m_task_param.task_info_list[task_idx].task_id);
		if (NULL != task_cfg && task_cfg->task_type == TASK_TYPE_PET_FEI_SHENG_TASK)
		{
			return true;
		}
	}

	return false;
}

void TaskManager::TaskDataToProtocol(TaskInfoPro * out_info, const TaskInfo & t_idx)
{
	out_info->Reset();

	out_info->task_id = t_idx.task_id;
	out_info->state_type = this->CanCommitTask(t_idx.task_id, false, true) ? TASK_STATE_FINISHED : TASK_STATE_UNDERWAY;
	out_info->prograss = t_idx.prog_num;
	out_info->expire_time = t_idx.expire_time;
}

bool TaskManager::CheckTaskIsLegal(TaskID task_id)
{
	const TaskInfomaConfig * task_cfg = TASKPOOL->GetTaskCfgByID(task_id);
	if (NULL == task_cfg) return false;

	if (TASK_TYPE_ADVENTURES == task_cfg->task_type && task_id != m_task_param.adv_task_id)
	{
		return false;
	}

	if (m_mgr->GetTaskRecorder()->HaveFinished(task_id))
	{
		return false;
	}

	if (task_cfg->pre_task_id > 0 && !m_mgr->GetTaskRecorder()->HaveFinished(task_cfg->pre_task_id))
	{
		return false;
	}

	if (task_cfg->pre_task_id2 > 0 && !m_mgr->GetTaskRecorder()->HaveFinished(task_cfg->pre_task_id2))
	{
		return false;
	}

	// 检查最低等级限制 - 如果是主线，则跳过等级检测
	if (!IsTaskTypeAcceptIgnoreLevelLimit(task_cfg->task_type) && task_cfg->level_min > 0 && task_role->GetLevel() < task_cfg->level_min)
	{
		return false;
	}
	// 检查最高等级限制
	if (task_cfg->level_max > 0 && task_role->GetLevel() > task_cfg->level_max)
	{
		return false;
	}
	// 检查职业是否符合要求
	int prof = task_role->GetProfession();
	if (task_cfg->prof_limit_min != PROF_TYPE_PROF_NO_LIMIT && (prof < task_cfg->prof_limit_min || prof > task_cfg->prof_limit_max))
	{
		return false;
	}

	return true;
}

void TaskManager::CalcAcceptableTask(int op_type, int param1)
{
	//printf("CalcAcceptableTask op_type[%d]\n", op_type);

	std::set<TaskID> delete_list;
	std::set<TaskID> change_list;
	std::set<TaskID> accept_list;

	// 从已接受列表中把不该出现的任务咔嚓掉
	this->CalcAcceptableTaskRemoveIllegalTask(delete_list);

	switch (op_type)
	{
	case ACB_RECALC_INITIALIZES:	// 初始化 - 重算全部任务
		{
			this->CalcAcceptableTaskOnInit();
		}
		break;

	case ACB_RECALC_ON_LEVEL_UP:	// 升级
		{
			this->CalcAcceptableTaskOnLevelUp(delete_list, change_list);
		}
		break;

	case ACB_RECALC_TASK_COMMIT:
		{
			this->CalcAcceptableTaskOnTaskCommit(param1, change_list);
		}
		break;

	case ACB_RECALC_PROF_CHANGE:	// 职业进阶
		{
			this->CalcAcceptableTaskOnProfChange(delete_list, change_list);
		}
		break;

	case ACB_RECALC_TASK_TYPE:	//对单个任务类型进行重算
		{
			this->CalcAcceptableTaskOnRecalcTaskType(param1, delete_list, change_list);
		}
		break;

	case ACB_RECALC_ON_DAY_CHANGE:
		{
			this->CalcAcceptableTaskOnDayChange(delete_list, change_list);
		}
		break;

	default:
		break;
	}

	// 没有配置接取 NPC 的任务要自动接取(赏金任务,丝绸之路除外） - 因为出现在可接列表里，故 this->CanAcceptTask(task_id) 必定为 true
	std::set<TaskID> tmp_acceptable_task(m_acceptable_set);// 不直接修改m_acceptable_set的原因是因为会调用OnAcceptTask,会令迭代器失效
	for (std::set<TaskID>::iterator tmp_it = tmp_acceptable_task.begin(); tmp_it != tmp_acceptable_task.end(); ++tmp_it)
	{
		const TaskInfomaConfig * t_tic = TASKPOOL->GetTaskCfgByID(*tmp_it);
		if (NULL == t_tic) continue;
		
		if (this->IsNoShowAtAcceptableList(t_tic, &m_acceptable_set, &change_list))
		{
			continue;
		}
	
		if (0 == t_tic->accept_npc 
			&& this->CheckTeamForTask(t_tic->task_id, false) 
			&& this->OnAcceptTask(t_tic->task_id))
		{
			change_list.erase(t_tic->task_id);
			accept_list.insert(t_tic->task_id);
		}
	}


	if (ACB_RECALC_INITIALIZES != op_type)
	{
		this->SendTaskDeleteNotify(delete_list, TASK_REMOVE_REASON_OTHERS);
		this->SendChangeTaskList(change_list, TASK_LIST_TYPE_ACCEPTABLE);
		this->SendChangeTaskList(accept_list, TASK_LIST_TYPE_ACCEPTED);
		if (!accept_list.empty())
		{
			// 有新接任务时，下发等待列表
			this->SendWaitingTaskList();
		}
		this->OnTeamConvert();
	}
}

void TaskManager::CalcAcceptableTaskRemoveIllegalTask(std::set<TaskID>& delete_list)
{
	for (int index = 0; index < m_task_param.task_count;)
	{
		int check_task_id = m_task_param.task_info_list[index].task_id;
		if (this->CheckTaskIsLegal(check_task_id))
		{
			++index;
			continue;
		}

		m_task_param.RemoveTaskByIdx(index);
		delete_list.insert(check_task_id);
	}
}

void TaskManager::CalcAcceptableTaskOnInit()
{
	int cur_lv = task_role->GetLevel();
	for (int lv = 0; lv <= cur_lv; ++lv)
	{
		const std::set<TaskID>* lv_task_set = TASKPOOL->GetTaskListAtLv(lv);
		if (NULL == lv_task_set) continue;

		for (std::set<TaskID>::const_iterator s_it = lv_task_set->begin(); s_it != lv_task_set->end(); ++s_it)
		{
			if (this->CanAcceptTask(*s_it))
			{
				m_acceptable_set.insert(*s_it);
			}
		}
	}
}

void TaskManager::CalcAcceptableTaskOnLevelUp(std::set<TaskID>& delete_list, std::set<TaskID>& change_list)
{
	for (std::set<TaskID>::iterator s_it = m_acceptable_set.begin(); s_it != m_acceptable_set.end();)
	{
		unsigned int wait_timestamp = 0;
		if (!this->CanAcceptTask(*s_it, false, &wait_timestamp))
		{
			delete_list.insert(*s_it);
			m_acceptable_set.erase(s_it++);
		}
		else
		{
			++s_it;
		}
	}

	const std::set<TaskID>* lv_task_set = TASKPOOL->GetTaskListAtLv(task_role->GetLevel());
	if (NULL != lv_task_set)
	{
		for (std::set<TaskID>::const_iterator s_it = lv_task_set->begin(); s_it != lv_task_set->end(); ++s_it)
		{
			if (m_acceptable_set.end() != m_acceptable_set.find(*s_it))
			{
				continue;
			}

			unsigned int wait_timestamp = 0;
			if (this->CanAcceptTask(*s_it, false, &wait_timestamp))
			{
				change_list.insert(*s_it);
				m_acceptable_set.insert(*s_it);
			}
		}
	}
}

void TaskManager::CalcAcceptableTaskOnTaskCommit(int task_id, std::set<TaskID>& change_list)
{
	const TaskInfomaConfig * task_cfg = TASKPOOL->GetTaskCfgByID(task_id);
	if (NULL == task_cfg)
	{
		gamelog::g_log_task.printf(LL_INFO, "[ TaskManager::CalcAcceptableTask | NULL config! -> User:[ %d | %s ] | TaskID: %d | OccurTime: %lld ][Handle: return only.]",
			task_role->GetUID(), task_role->GetName(), task_id, (long long)m_nowTime);

		if (CrossConfig::Instance().IsHiddenServer())
		{
			World::GetInstWorld()->SyncLogToOriginServer(m_mgr->GetRole()->GetUserId().db_index, SYNC_LOG_TYPE_TASK,
				"[ TaskManager::CalcAcceptableTask | NULL config! -> User:[ %d | %s ] | TaskID: %d | OccurTime: %lld ][Handle: return only.]",
				task_role->GetUID(), task_role->GetName(), task_id, (long long)m_nowTime);
		}

		return;
	}

	int next_task_id = task_cfg->nex_task_id;

	if (TASK_TYPE_ADVENTURES == task_cfg->task_type)
	{
		if (0 == next_task_id)	// 如果是奇遇任务的最后一个任务，清理奇遇任务记录；
		{
			int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_task_adv_succ_content, task_id);
			if (length > 0)
			{
				Protocol::SCSystemMsg sm;
				int sendlen = 0;
				if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT)))
				{
					EngineAdapter::Instance().NetSend(m_mgr->GetRole()->GetNetId(), (const char*)&sm, sendlen);
				}
			}
			m_task_param.adv_task_timestamp = 0;
			m_mgr->GetTaskRecorder()->ClearAppoiontedTypeTaskRecord(TASK_TYPE_ADVENTURES);


			if (task_cfg->nex_task_id2 > 0)
			{
				if (m_acceptable_set.end() == m_acceptable_set.find(task_cfg->nex_task_id2) && this->CanAcceptTask(task_cfg->nex_task_id2))
				{
					change_list.insert(task_cfg->nex_task_id2);
					m_acceptable_set.insert(task_cfg->nex_task_id2);
				}
			}
		}
		else		// 否则记录为标记
		{
			m_task_param.adv_task_id = next_task_id;
		}
	}
	else if (TASK_TYPE_STAGE_TASK == task_cfg->task_type || TASK_TYPE_STAGE_TASK_1 == task_cfg->task_type)
	{
		if (LOGIC_CONFIG->GetAdvanceFbConfig()->HasNoticeTaskID(task_id))
		{
			m_mgr->NoticeNum(noticenum::NT_STAGE_TASK_LAST_SUCC);
		}
	}

	if (next_task_id > 0)
	{
		unsigned int wait_timestamp = 0;
		if (m_acceptable_set.end() == m_acceptable_set.find(next_task_id)
			&& this->CanAcceptTask(next_task_id, false, &wait_timestamp))
		{
			change_list.insert(next_task_id);
			m_acceptable_set.insert(next_task_id);
		}
	}

	if (task_cfg->nex_task_id2 > 0)
	{
		unsigned int wait_timestamp = 0;
		if (m_acceptable_set.end() == m_acceptable_set.find(task_cfg->nex_task_id2)
			&& this->CanAcceptTask(task_cfg->nex_task_id2, false, &wait_timestamp))
		{
			change_list.insert(task_cfg->nex_task_id2);
			m_acceptable_set.insert(task_cfg->nex_task_id2);
		}
	}
}

void TaskManager::CalcAcceptableTaskOnProfChange(std::set<TaskID>& delete_list, std::set<TaskID>& change_list)
{
	for (std::set<TaskID>::iterator s_it = m_acceptable_set.begin(); s_it != m_acceptable_set.end();)
	{
		unsigned int wait_timestamp = 0;
		if (!this->CanAcceptTask(*s_it, false, &wait_timestamp))
		{
			delete_list.insert(*s_it);
			m_acceptable_set.erase(s_it++);	
		}
		else
		{
			++s_it;
		}
	}

	const std::set<TaskID>* prof_task_list = TASKPOOL->GetTaskListWithBaseProf(task_role->GetAppearanceProf());
	if (prof_task_list != NULL)
	{
		for (std::set<TaskID>::const_iterator s_it = prof_task_list->begin(); s_it != prof_task_list->end(); ++s_it)
		{
			unsigned int wait_timestamp = 0;
			if (m_acceptable_set.end() == m_acceptable_set.find(*s_it)
				&& this->CanAcceptTask(*s_it, false, &wait_timestamp))
			{
				change_list.insert(*s_it);
				m_acceptable_set.insert(*s_it);
			}
		}
	}
}

void TaskManager::CalcAcceptableTaskOnRecalcTaskType(int task_type, std::set<TaskID>& delete_list, std::set<TaskID>& change_list)
{
	const std::set<TaskID>* task_set = TASKPOOL->GetTaskListAtTaskType(task_type);
	if (NULL == task_set) return;

	for (std::set<TaskID>::iterator s_it = m_acceptable_set.begin(); s_it != m_acceptable_set.end();)
	{
		unsigned int wait_timestamp = 0;
		if (!this->CanAcceptTask(*s_it, false, &wait_timestamp))
		{
			delete_list.insert(*s_it);
			m_acceptable_set.erase(s_it++);
		}
		else
		{
			++s_it;
		}
	}

	for (std::set<TaskID>::const_iterator s_it = task_set->begin(); s_it != task_set->end(); ++s_it)
	{
		unsigned int wait_timestamp = 0;
		if (m_acceptable_set.end() == m_acceptable_set.find(*s_it) 
			&& this->CanAcceptTask(*s_it, false, &wait_timestamp))
		{
			change_list.insert(*s_it);
			m_acceptable_set.insert(*s_it);
		}
	}
}

void TaskManager::CalcAcceptableTaskOnDayChange(std::set<TaskID>& delete_list, std::set<TaskID>& change_list)
{
	/* 暂时不需要这个逻辑
	std::map<TaskID, unsigned int>::iterator it = m_waiting_map.begin();
	for (; it != m_waiting_map.end(); )
	{
		if (this->CanAcceptTask(it->first))
		{
			change_list.insert(it->first);
			m_acceptable_set.insert(it->first);
			m_waiting_map.erase(it++);
		}
		else
		{
			++it;
		}
	}
	*/
}

bool TaskManager::IsNoShowAtAcceptableList(const TaskInfomaConfig* task_cfg, ARG_OUT std::set<TaskID>* acceptable_list, ARG_OUT std::set<TaskID>* change_list)
{
	switch (task_cfg->task_type)
	{
		// 赏金任务,丝绸之路,奇闻任务不出现在可接列表里
	case TASK_TYPE_BOUNTY_TASK:
	case TASK_TYPE_TRANSACTION_TASK:
	case TASK_TYPE_ANECDOTE_TASK:
	case TASK_TYPE_PRESTIGE_TASK:
	case TASK_TYPE_FLY_UP_TASK:
	case TASK_TYPE_PET_FEI_SHENG_TASK:	
	case TASK_TYPE_DIANHUA_TASK:
	case TASK_TYPE_ROLE_GUILD_TASK:
		{
			change_list->erase(task_cfg->task_id);
			acceptable_list->erase(task_cfg->task_id);
			return true;
		}
		break;

	case TASK_TYPE_PARTNER_SOAR_TASK:
		{
			if (0 == task_cfg->pre_task_id)
			{
				return true;
			}
		}
		break;

	case TASK_TYPE_SPECIAL_QUEST:
		{
			if (task_cfg->task_id == TASKPOOL->GetTaskOtherCfg().anecdotes_recommend_task_id && m_mgr->GetAnecdoteTask()->IsAcceptOrFinish(ANECDOTE_TASK_TYPE_STAR_KING))
			{
				change_list->erase(task_cfg->task_id);
				acceptable_list->erase(task_cfg->task_id);
				return true;
			}
			else if (task_cfg->task_id == TASKPOOL->GetTaskOtherCfg().partner_upgrade_task_id && this->IsPartnerUpgradeTaskBuyFlag())
			{
				change_list->erase(task_cfg->task_id);
				acceptable_list->erase(task_cfg->task_id);
				return true;
			}
		}
		break;
	default:
		break;
	}

	return false;
}

void TaskManager::SendSingleTaskDelete(int task_id, int reason)
{
	std::set<TaskID> t_remove_list; t_remove_list.insert(task_id);
	this->SendTaskDeleteNotify(t_remove_list, reason);
}

void TaskManager::SendTaskDeleteNotify(const std::set<TaskID> & task_id_list, int reason)
{
	if (task_id_list.empty()) return;

#ifdef _DEBUG
	//printf("TaskManager::SendTaskDeleteNotify role[%d,%s]\n",
	//	m_mgr->GetRole()->GetUID(), m_mgr->GetRole()->GetName());
#endif

	Protocol::SCTaskDeleteNotifyList tdnl;

	tdnl.list_count = 0;
	for (std::set<TaskID>::const_iterator loop_it = task_id_list.begin(); loop_it != task_id_list.end(); ++loop_it)
	{
		if (tdnl.list_count >= MAX_TASK_LIST_LEN_NUM) break;
		Protocol::SCTaskDeleteNotifyList::TaskRemoveCon & remove_con = tdnl.info_list[tdnl.list_count++];
		remove_con.task_id = *loop_it;
		remove_con.remove_reason = reason;
	}

	int send_len = sizeof(tdnl) - (MAX_TASK_LIST_LEN_NUM - tdnl.list_count) * sizeof(Protocol::SCTaskDeleteNotifyList::TaskRemoveCon);
	if (send_len > 0) m_mgr->NetSend((const char *)&tdnl, send_len);
}

void TaskManager::SendSingleTaskInfo(int task_id, int to_list_type)
{
#ifdef _DEBUG
	//printf("TaskManager::SendSingleTaskInfo role[%d,%s] task_id[%d] type[%d]\n", 
	//	m_mgr->GetRole()->GetUID(), m_mgr->GetRole()->GetName(), task_id, to_list_type);
#endif
	//if (TASK_LIST_TYPE_ACCEPTABLE == to_list_type) printf("\nSend Acptable -> TaskID: %d\n", task_id);

	std::set<TaskID> change_list; change_list.insert(task_id);
	this->SendChangeTaskList(change_list, to_list_type);
}

void TaskManager::SendChangeTaskList(const std::set<TaskID> & task_id_list, int to_list_type)
{
#ifdef _DEBUG
	//printf("TaskManager::SendChangeTaskList role[%d,%s] to_list_type[%d]\n",
	//	m_mgr->GetRole()->GetUID(), m_mgr->GetRole()->GetName(), to_list_type);
#endif

	if (task_id_list.empty()) return;

	Protocol::SCTaskDetailedInfoList tdil;

	tdil.list_count = 0;
	for (std::set<TaskID>::iterator loop_it = task_id_list.begin(); loop_it != task_id_list.end(); ++loop_it)
	{
		if (tdil.list_count >= MAX_TASK_LIST_LEN_NUM || tdil.list_count < 0) break;

		Protocol::SCTaskDetailedInfoList::TaskProCon & pro_info = tdil.task_list_info[tdil.list_count++];
		pro_info.list_type = to_list_type;

		if (TASK_LIST_TYPE_ACCEPTABLE == to_list_type)
		{
			pro_info.task_con.Reset();
			pro_info.task_con.task_id = *loop_it;
		}
		else		// 已接表
		{
			const TaskInfo * target_task = m_task_param.GetTaskByID(*loop_it);
			if (NULL == target_task) 	// 如果已接表没有此任务，继续
			{
				if (tdil.list_count > 0) --tdil.list_count;
				continue;
			}

			this->TaskDataToProtocol(&pro_info.task_con, *target_task);
		}
	}

	int send_len = sizeof(tdil) - (MAX_TASK_LIST_LEN_NUM - tdil.list_count) * sizeof(Protocol::SCTaskDetailedInfoList::TaskProCon);
	if (send_len > 0) m_mgr->NetSend((const char *)&tdil, send_len);
}

void TaskManager::RefreshTaskProgNum(int task_id, bool is_notify)
{
	const TaskInfomaConfig * task_cfg = TASKPOOL->GetTaskCfgByID(task_id);
	if (NULL == task_cfg) return;

	for (int t_cur = 0; t_cur < m_task_param.task_count; ++t_cur)
	{
		TaskInfo & p_ti = m_task_param.task_info_list[t_cur];
		if (task_id != p_ti.task_id) continue;

		UNSTD_STATIC_CHECK(46 == TASK_NEED_MAX)
		UNSTD_STATIC_CHECK(26 == TASK_MEET_TYPE_MAX)
		switch (task_cfg->task_condition)
		{
		case TASK_NEED_HAVE_PROPERTY:
			p_ti.prog_num = m_mgr->GetKnapsack()->Count(task_cfg->param1);
			break;

		case TASK_NEED_COMMIT_PROPERTY:
			// 什么也不做， 等提交任务时再去计算
			break;

		case TASK_NEED_MEET_REQUIRES:
			{
				switch (task_cfg->param1)
				{
				case TASK_MEET_GOD_PRINT_WISH:
					{
						p_ti.prog_num = m_mgr->GetPet()->GetWishGodPrintTimes();
					}
					break;

				case TASK_MEET_PROF_PROMOTE:
					{
						int up_times = task_role->GetProfession() % 100;		// 进阶次数
						if (up_times >= task_cfg->param2) p_ti.prog_num = 1;
					}
					break;

				case TASK_MEET_HAVE_PARTNER:
					if (m_mgr->GetPartner()->IsPartnerAlreadyActive(task_cfg->param2)) p_ti.prog_num = 1;
					break;

				case TASK_MEET_OWN_CREATURE:
					if (m_mgr->GetPet()->IsHavePet(task_cfg->param2)) p_ti.prog_num = 1;
					break;
				case TASK_MEET_DREAM_NOTES_CHALLENGE:
					{
						p_ti.prog_num = m_mgr->GetDreamNotesChallenge()->GetPassLevel();
					}
					break;
				case TASK_MEET_LUN_HUI_WANG_CHUAN:
					{
						if (m_mgr->GetLunHuiWangChuan()->GetPassIDX() >= task_cfg->param2)
						{
							p_ti.prog_num = 1;
						}
					}
					break;
				case TASK_MEET_SKILL_LEVEL:
					{
						p_ti.prog_num = m_mgr->GetRoleSkill()->GetMaxSkillLevel();
					}
					break;
				case TASK_MEET_REALM_BREAK_FIGHT:
					{
						p_ti.prog_num = (m_mgr->GetRealmBreak()->GetCurLevel() >= task_cfg->param2) ? 1 : 0;
					}
					break;
				default:
					break;
				}
			}
			break;

		case TASK_NEED_GAIN_TASKPROP:
			{
				const DefeatMonsGroups * group_cfg = TASKPOOL->GetDefeatMonsGroups(task_cfg->param1);
				if (NULL != group_cfg) p_ti.prog_num = m_mgr->GetKnapsack()->Count(group_cfg->need_item);
			}
			break;
		case TASK_NEED_ENTER_FB:
			{
				if (task_cfg->param1 == FBManager::FB_TYPE_CRYSTAL_FB)
				{
					p_ti.prog_num = m_mgr->GetRoleCrystalFb()->HasFristPassCrystalFb(task_cfg->param2) ? 1 : 0;
				}
			}
			break;
		case TASK_NEED_MEET_ANECDOTE_REQUIRES:
			{
				if (m_mgr->GetAnecdoteTask()->IsFinishTask(task_cfg->param1))
				{
					p_ti.prog_num = task_cfg->param2;
				}
			}
			break;
		case TASK_NEED_MATERIAL:
			{
				p_ti.prog_num = m_mgr->GetRoleMaterial()->GetPassLevelByChapter(task_cfg->param1 - 1);
			}
			break;
		case TASK_NEED_MI_YI:
			{
				p_ti.prog_num = m_mgr->GetMiYi()->GetCurLayerFinishNum(task_cfg->param1);
			}
			break;
		case TASK_NEED_PARTNER_LEVEL_ACHIVE:
			{
				const NewPartnerBaseInfo* partner_base = m_mgr->GetPartner()->GetPartnerBaseInfoById(task_cfg->param1);
				if (partner_base != NULL)
				{
					p_ti.prog_num = partner_base->level;
				}
			}
			break;
		case TASK_NEED_VILLAGE_LEVEL_ACHIVE:
			{
				p_ti.prog_num = m_mgr->GetPrestige()->GetVillageLevel(task_cfg->param1);
			}
			break;
		case TASK_NEED_COLLECTION_TU_JIAN_ACTIVE_NUM:
			{
				p_ti.prog_num = m_mgr->GetCollection()->GetCollectionActiveTuJianNum();
			}
			break;
		case TASK_NEED_ONE_FA_BAO_UP_LEVEL:
			{
				p_ti.prog_num = m_mgr->GetFaBao()->GetFaBaoMaxLevel();
			}
			break;
		case TASK_NEED_OPEN_YIN_YANG_EYE:
			{
				p_ti.prog_num = m_mgr->GetYinYangEye()->IsOpen() ? 1 : 0;
			}
			break;
		case TASK_NEED_CAREER_ADVANCE:
			{
				int main_num = m_mgr->GetAdvanceCareer()->GetMainLevel();
				int sub_num = m_mgr->GetAdvanceCareer()->GetSubLevel();
				if (main_num > task_cfg->param1)
				{
					p_ti.prog_num = 1;
				}
				else if (main_num == task_cfg->param1 && sub_num >= task_cfg->param2)
				{
					p_ti.prog_num = 1;
				}
				else
				{
					p_ti.prog_num = 0;
				}
			}
			break;
		default:
			break;
		}

		if (TASK_NEED_MEET_REQUIRES != task_cfg->task_condition
			&& TASK_NEED_ENTER_FB != task_cfg->task_condition
			&& p_ti.prog_num > task_cfg->param2)
		{
			p_ti.prog_num = task_cfg->param2;
		}

		break;
	}

	if (this->CanCommitTask(task_cfg->task_id))
	{
		ROLE_LOG_QUICK6(LOG_TYPE_COMPLETE_TASK, task_role, task_cfg->task_type, task_cfg->task_id, NULL, NULL);

		gamelog::g_log_task_stats.printf(LL_INFO, "[TaskResult::Complete][role[%d %s] task_id:%d task_level:%d task_type:%d role_professional:%d]",
			UidToInt(task_role->GetUserId()), task_role->GetName(), task_cfg->task_id, task_cfg->level_min, task_cfg->task_type, task_role->GetProfession());

		if (CrossConfig::Instance().IsHiddenServer())
		{
			World::GetInstWorld()->SyncLogToOriginServer(m_mgr->GetRole()->GetUserId().db_index, SYNC_LOG_TYPE_TASK_STAT,
				"[TaskResult::Complete][role[%d %s] task_id:%d task_level:%d task_type:%d role_professional:%d]",
				UidToInt(task_role->GetUserId()), task_role->GetName(), task_cfg->task_id, task_cfg->level_min, task_cfg->task_type, task_role->GetProfession());
		}
	}

	if (is_notify)
	{
		this->SendSingleTaskInfo(task_id, TASK_LIST_TYPE_ACCEPTED);
	}

	if (task_cfg->can_auto_commit)
	{
		this->CheckAutoCommit(task_id, task_cfg->commit_npc);
	}
}

void TaskManager::RefreshTaskCanGiveUp(int label)
{
	if (m_task_param.task_count <= 0 || label < 0 || label >= GIVEUP_TYPE_MAX) return;

	std::set<ItemContainer> t_change_list;

	for (int t_cur = 0; t_cur < m_task_param.task_count; ++t_cur)
	{
		TaskInfo & p_ti = m_task_param.task_info_list[t_cur];
		if (0 == p_ti.task_id) continue;

		const TaskInfomaConfig * p_tic = TASKPOOL->GetTaskCfgByID(p_ti.task_id);
		if (NULL == p_tic) continue;

		const UInt16 & can_give_up = (UInt16)p_tic->giveup[label];
		std::map<TaskID, UInt16>::iterator f_it = m_old_give_up_state.find(p_ti.task_id);
		if (m_old_give_up_state.end() != f_it)
		{
			if (can_give_up == f_it->second) continue;
			f_it->second = can_give_up;
		}
		else
		{
			m_old_give_up_state.insert(std::pair<TaskID, UInt16>(p_ti.task_id, can_give_up));
		}

		t_change_list.insert(ItemContainer(p_ti.task_id, can_give_up));
	}

	if (!t_change_list.empty()) this->SendTaskCanGiveUpList(t_change_list);
}

void TaskManager::SendTaskCanGiveUpList(const std::set<ItemContainer> & can_give_up_list)
{
#ifdef _DEBUG
	//printf("TaskManager::SendTaskCanGiveUpList role[%d,%s] num[%d]\n",
	//	m_mgr->GetRole()->GetUID(), m_mgr->GetRole()->GetName(), can_give_up_list.size());
#endif

	Protocol::SCTaskCanAbandonedList tcal;

	tcal.list_num = 0;
	for (std::set<ItemContainer>::const_iterator loop_it = can_give_up_list.begin(); loop_it != can_give_up_list.end(); ++loop_it)
	{
		if (tcal.list_num >= MAX_ACCEPTED_TASK_NUM) break;
		tcal.can_abandon_list[tcal.list_num++] = *loop_it;
	}

	m_mgr->NetSend((const char *)&tcal, sizeof(tcal) - (MAX_ACCEPTED_TASK_NUM - tcal.list_num) * sizeof(ItemContainer));
}

void TaskManager::CheckAutoCommit(TaskID task_id, int commit_npc)
{
	if (this->CanCommitTask(task_id))
	{
		//this->CommitTask(task_id); 
		m_auto_commit_set.insert(task_id);
	}
}

void TaskManager::SendTaskTraceListInfo()
{
#ifdef _DEBUG
	//printf("TaskManager::SendTaskTraceListInfo role[%d,%s]\n",
	//	m_mgr->GetRole()->GetUID(), m_mgr->GetRole()->GetName());
#endif

	Protocol::SCTaskTraceInformation tti;

	tti.list_num = m_trace_param.list_len;
	memcpy(tti.param_list, m_trace_param.trace_list, tti.list_num * sizeof(tti.param_list[0]));

	int send_len = sizeof(Protocol::SCTaskTraceInformation) - (TASK_TRACE_MAX_NUM - tti.list_num) * sizeof(tti.param_list[0]);
	if (send_len > 0) m_mgr->NetSend((const char *)&tti, send_len);
}

void TaskManager::DoDelayCommit()
{
	if (!m_auto_commit_set.empty())
	{
		std::set<TaskID> tmp_set;
		tmp_set.swap(m_auto_commit_set);

		for (std::set<TaskID>::iterator it = tmp_set.begin(); it != tmp_set.end(); ++it)
		{
			int skill_id = *it;
			this->CommitTask(skill_id);
		}
	}
}

void TaskManager::GmPreTaskForceFinish(TaskID task_id)
{
	const TaskInfomaConfig* cur_task = TASKPOOL->GetTaskCfgByID(task_id);
	if (NULL != cur_task)
	{
		if (!m_mgr->GetTaskRecorder()->HaveFinished(cur_task->pre_task_id))
		{
			const TaskInfomaConfig * pre_task = TASKPOOL->GetTaskCfgByID(cur_task->pre_task_id);
			if (NULL != pre_task)
			{
				this->GmPreTaskForceFinish(cur_task->pre_task_id); // 递归完成前置任务
			}
		}
	}

	if (m_mgr->GetTaskRecorder()->HaveFinished(task_id)) return;

	const TaskInfomaConfig * task_cfg = TASKPOOL->GetTaskCfgByID(task_id);
	if (NULL == task_cfg) return;



	// 发放任务报酬
	const CommonTaskReward * ctr_cfg = TASKPOOL->GetCommonTaskReward(task_id, task_role);
	if (NULL != ctr_cfg)	// 允许出现没有奖励的任务，所以当奖励为空时不返回。
	{
		if (ctr_cfg->reward_count > 0 && !m_mgr->GetKnapsack()->PutList(ctr_cfg->reward_count, ctr_cfg->reward_list, PUT_REASON_TASK_REWARD)) return;
		m_mgr->GetMoney()->Add(ctr_cfg->gold, ctr_cfg->immortal, ctr_cfg->coin, "CommonTaskReward");
		if (ctr_cfg->exp > 0) task_role->AddExp(ctr_cfg->exp, ADD_EXP_TYPE_TASK);
		if (ctr_cfg->repu_numb > 0) m_mgr->GetPrestige()->AddPrestige(ctr_cfg->repu_city, ctr_cfg->repu_numb, __FUNCTION__);
		if (ctr_cfg->power > 0) m_mgr->GetRoleCrystalFb()->AddCrystalFbEnergy(ctr_cfg->power, __FUNCTION__, ENTER_ENERGY_CHANGE_TYPE_OTHER);
	}

	// 任务报酬已发放，注意不能中途返回
	m_mgr->GetTaskRecorder()->TaskComplete(task_id);

	if (NULL != m_task_param.GetTaskByID(task_id))
	{
		m_task_param.RemoveTask(task_id);
	}
	this->SendSingleTaskDelete(task_id, TASK_REMOVE_REASON_FINISH);

	m_old_give_up_state.erase(task_id);
	EventHandler::Instance().OnCommitTask(task_role, task_id);
}