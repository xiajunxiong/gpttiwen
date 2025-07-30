#include "robotaimission.hpp"
#include "obj/character/robot.h"
#include "obj/MoveAssist/MoveAssist.hpp"
#include "scene/scenemanager.hpp"
#include "scene/scene.h"
#include "scene/speciallogic.hpp"
#include "task/taskpool.h"
#include "npc/npcpool.h"
#include "world.h"

RobotAIMission::RobotAIMission(Character * me)
	: RobotAIBase(me, ROBOT_AI_TYPE_MISSION)
{
	m_last_task_id = 0;
	m_next_idle_check_time = 0;

	m_cur_task_id = 0;
	m_force_next_task_status = 0;

	m_task_status = TASK_STATUS_END;
	m_task_show_action = TASK_SHOW_ACTION_NONE;
	m_task_show_action_param = 0;
	m_action_target_posi = Posi(-1, -1);

}

RobotAIMission::~RobotAIMission()
{
}

void RobotAIMission::Init(const RobotAIParam & mp)
{
	static InitParam info_param;
	UNSTD_STATIC_CHECK(sizeof(info_param) <= sizeof(mp));
	memcpy(&info_param, &mp, sizeof(info_param));

	m_last_task_id = info_param.m_last_task_id;

	m_next_idle_check_time = 0;

	m_cur_task_id = info_param.m_cur_task_id;
	m_force_next_task_status = info_param.m_force_next_task_status;
	m_task_status = info_param.m_task_status;
	m_task_show_action = info_param.m_task_show_action;
	m_task_show_action_param = info_param.m_task_show_action_param;
	m_action_target_posi = info_param.m_action_target_posi;

}

void RobotAIMission::GetInitParam(RobotAIParam & out_param)
{
	static InitParam info_param;
	info_param.m_last_task_id = m_last_task_id;

	info_param.m_cur_task_id = m_cur_task_id;
	info_param.m_force_next_task_status = m_force_next_task_status;
	info_param.m_task_status = m_task_status;
	info_param.m_task_show_action = m_task_show_action;
	info_param.m_task_show_action_param = m_task_show_action_param;
	info_param.m_action_target_posi = m_action_target_posi;

	UNSTD_STATIC_CHECK(sizeof(info_param) <= sizeof(out_param));
	memcpy(&out_param, &info_param, sizeof(info_param));
}

void RobotAIMission::AI(time_t now_second)
{
	switch (m_me->GetActionStatus())
	{
	case Character::ACTION_IDLE:
		{
			if (ROBOT_AI_ACTION_MOVE == m_action_type)
			{//机器人行为与实际状态不符  移动状态中断
				this->SetAction(ROBOT_AI_ACTION_IDLE, 0);
			}
			if (ROBOT_AI_ACTION_IDLE == m_action_type
				&& m_next_idle_check_time > now_second)
			{
				break;
			}
			m_next_idle_check_time = now_second + 5;
			//待机状态(处理函数可能会换场景  导致对此机器人 后续操作 无意义)
			//切换任务
			if (!this->AITask(now_second))
			{
				return;
			}

			//切换任务状态
			if (!this->AITaskStatus(now_second))
			{
				return;
			}
			this->AITaskShowAction(now_second);
		}
		break;
	case Character::ACTION_RUN:
		{
			if (ROBOT_AI_ACTION_MOVE != m_action_type)
			{//开始移动了 设置状态
				if (TASK_SHOW_ACTION_EMPLOY_RANGER == m_task_show_action)
				{//巡逻 计算遇怪时间戳
					Scene *scene = m_me->GetScene();
					if (NULL == scene)
					{
						return;
					}
					time_t next_fight_time = now_second + scene->RandMineMonsterEncounterTimeMS() / 1000;
					if (5 + now_second > next_fight_time)
					{
						next_fight_time = now_second + DEF_ENCOUNTER_FIGHT_TIME;
					}
					this->SetAction(ROBOT_AI_ACTION_MOVE, next_fight_time);
				}
				else
				{//设置移动超时
					this->SetAction(ROBOT_AI_ACTION_MOVE, now_second + 300);
				}
			}
		}
		break;
	default:
		break;
	}
}

bool RobotAIMission::AITask(time_t now_second)
{
	if (-1 == m_cur_task_id)
	{//任务已完成
		return false;
	}

	if (TASK_STATUS_END <= m_task_status
		|| now_second > m_force_next_task_status)
	{//当前任务完成
		if (m_last_task_id == m_cur_task_id)
		{//任务做完了
			this->DeleteMe();
			return false;
		}

		const TaskInfomaConfig * task_cfg = TASKPOOL->GetTaskCfgByID(m_cur_task_id);
		if (task_cfg == NULL
			|| task_cfg->task_type != TASK_TYPE_MAIN_QUEST)
		{
			this->DeleteMe();
			return false;
		}

		const TaskInfomaConfig * next_task_cfg = TASKPOOL->GetTaskCfgByID(task_cfg->nex_task_id);
		if (next_task_cfg == NULL
			|| next_task_cfg->task_type != TASK_TYPE_MAIN_QUEST)
		{
			this->DeleteMe();
			return false;
		}
		if (next_task_cfg->level_min > ((Robot *)m_me)->GetLevel())
		{
			((Robot *)m_me)->SetRobotLevel(next_task_cfg->level_min);
		}
		m_cur_task_id = next_task_cfg->task_id;
		
		this->SetTaskStatus(TASK_STATUS_INVALID);
		m_force_next_task_status = now_second + TASK_STATUS_OUT_TIME;

		//清空移动辅助
		MoveAssist* move_assist = m_me->GetMoveAssist(false);
		if (NULL != move_assist)
		{
			move_assist->ReSetDestination();
		}
	}

	return true;
}

bool RobotAIMission::AITaskStatus(time_t now_second)
{
	if (TASK_SHOW_ACTION_NONE != m_task_show_action)
	{
		return true;
	}
	const TaskInfomaConfig * task_cfg = TASKPOOL->GetTaskCfgByID(m_cur_task_id);
	if (task_cfg == NULL)
	{
		this->DeleteMe();
		return false;
	}

	//切换场景参数
	int need_change_scene = 0;
	int scene_id = 0;
	int scene_key = 0;
	Posi target_posi(-1, -1);

	switch (m_task_status)
	{
	case TASK_STATUS_INVALID:
		{
			Scene *scene = m_me->GetScene();
			if (NULL == scene)
			{
				this->SetTaskStatus(TASK_STATUS_ACCEPT);
				return false;
			}

			bool ret = this->GetNPCTarget(task_cfg->accept_npc, scene->GetSceneID(),
				need_change_scene, scene_id, scene_key, target_posi);
			if (!ret)
			{
				this->SetTaskStatus(TASK_STATUS_ACCEPT);
				return true;
			}
			if (0 == need_change_scene)
			{
				this->SetTaskStatus(TASK_STATUS_GO_ACCEPT);
				this->SetTaskShowAction(TASK_SHOW_ACTION_EMPLOY);
				m_action_target_posi = target_posi;
				return true;
			}
		}
		break;
	case TASK_STATUS_ACCEPT:
		{
			int temp_show_action = this->GetTaskShowActionType(task_cfg->task_condition, task_cfg->param1,
				need_change_scene, scene_id, scene_key, target_posi);
			if (1 == need_change_scene)
			{
				break;
			}
			if (TASK_SHOW_ACTION_NONE == temp_show_action)
			{
				this->SetTaskStatus(TASK_STATUS_DONE);
			}
			else
			{
				this->SetTaskStatus(TASK_STATUS_ONGOING);
			}
			this->SetTaskShowAction(temp_show_action);

			m_action_target_posi = target_posi;

			return true;
		}
		break;
	case TASK_STATUS_DONE:
		{
			if (task_cfg->can_auto_commit)
			{
				this->SetTaskStatus(TASK_STATUS_COMMIT);
				return true;
			}
			Scene *scene = m_me->GetScene();
			if (NULL == scene)
			{
				this->SetTaskStatus(TASK_STATUS_COMMIT);
				return false;
			}

			bool ret = this->GetNPCTarget(task_cfg->commit_npc, scene->GetSceneID(),
				need_change_scene, scene_id, scene_key, target_posi);
			if (!ret)
			{
				this->SetTaskStatus(TASK_STATUS_COMMIT);
				return true;
			}
			if (0 == need_change_scene)
			{
				this->SetTaskStatus(TASK_STATUS_GO_COMMIT);
				this->SetTaskShowAction(TASK_SHOW_ACTION_EMPLOY);
				m_action_target_posi = target_posi;
				return true;
			}
		}
		break;
	case TASK_STATUS_COMMIT:
		{
			this->SetTaskStatus(TASK_STATUS_END);
			return false;
		}
		break;
	default:
		return false;
	}
	//换场景
	if (1 == need_change_scene)
	{
		//换场景 后续操作无意义
		World::GetInstWorld()->GetSceneManager()->GoTo((Robot *)m_me, scene_id, scene_key, target_posi);
	}
	return false;
}

bool RobotAIMission::AITaskShowAction(time_t now_second)
{
	switch (m_task_status)
	{
	case TASK_STATUS_GO_ACCEPT:
	case TASK_STATUS_ONGOING:
	case TASK_STATUS_GO_COMMIT:
		break;
	default:
		return false;
	}


	MoveAssist* move_assist = m_me->GetMoveAssist(true);
	if (NULL == move_assist)
	{
		return false;
	}
	//巡路结束
	if (MoveAssist::MOVE_ASSIST_STATUS_TYPE_NONE != move_assist->GetMoveAssistStatus())
	{
		return false;
	}
	
	switch (m_task_show_action)
	{
	case TASK_SHOW_ACTION_EMPLOY:
	case TASK_SHOW_ACTION_EMPLOY_GATHER:
	case TASK_SHOW_ACTION_EMPLOY_FIGHT:
	case TASK_SHOW_ACTION_EMPLOY_FB:
		{
			Posi now_pos = m_me->GetPos();
			Posi deltaP = m_action_target_posi - now_pos;
			if (deltaP.IsLongerThan(3))
			{//未到目标点 重新巡路
				move_assist->SetDestination(m_action_target_posi, MoveAssist::MOVE_ASSIST_REQ_TYPE_EMPLOY);
				return false;
			}
		}
		break;
	case TASK_SHOW_ACTION_EMPLOY_RANGER:
		{//巡逻 直接重寻
			move_assist->SetDestination(Posi(-1, -1), MoveAssist::MOVE_ASSIST_REQ_TYPE_AROND);
			return false;
		}
		break;
	default:
		return false;
	}

	//抵达目标点 清空寻路
  	switch (m_task_show_action)
	{
	case TASK_SHOW_ACTION_EMPLOY:
		{
			this->SetTaskStatus(m_task_status + 1);
		}
		break;
	case TASK_SHOW_ACTION_EMPLOY_FIGHT:
		{
			if (0 != m_task_show_action_param)
			{//参数不对 跳过
				this->SetTaskStatus(m_task_status + 1);
				break;
			}
			//战斗
			m_task_show_action_param++;
			m_me->ActionStop();
			time_t fight_end_time = now_second
				+ RandomNum(DEF_VIRTUAL_FIGHT_TIME_MIN, DEF_VIRTUAL_FIGHT_TIME_MAX);
			this->SetAction(ROBOT_AI_ACTION_FIGHT, fight_end_time);
		}
		break;
	case TASK_SHOW_ACTION_EMPLOY_FB:
		{
			if (0 != m_task_show_action_param)
			{//参数不对 跳过
				this->SetTaskStatus(m_task_status + 1);
				break;
			}
			//副本
			m_task_show_action_param++;
			m_me->ActionStop();
			time_t fight_end_time = now_second
				+ RandomNum(DEF_VIRTUAL_FB_TIME_MIN, DEF_VIRTUAL_FB_TIME_MAX);
			this->SetAction(ROBOT_AI_ACTION_HIDE, fight_end_time);
		}
		break;
	case TASK_SHOW_ACTION_EMPLOY_GATHER:
		{
			if (0 != m_task_show_action_param)
			{//参数不对 跳过
				this->SetTaskStatus(m_task_status + 1);
				break;
			}
			//战斗
			m_task_show_action_param++;
			m_me->ActionStop();
			time_t temp_end_time = now_second + 3;
			this->SetAction(ROBOT_AI_ACTION_GATHER, temp_end_time);
		}
		break;
	default:
		return false;
	}

	return false;
}

int RobotAIMission::GetTaskShowActionType(int task_condition, int task_meet_requires_type, int & need_change_scene, int & scene_id, int & scene_key, Posi & target_posi)
{
	int show_action_type = TASK_SHOW_ACTION_NONE;
	switch (task_condition)
	{
	case TASK_NEED_TO_DO_NOTHING:
	case TASK_NEED_INTO_SCENARIO:
	case TASK_NEED_GIVE_PROGROUP:
	case TASK_NEED_USES_PROGROUP:
	case TASK_NEED_BUY_PROPERTY:
	case TASK_NEED_PLAY_CUTSCENE:
	case TASK_NEED_GIVE_PROPERTY:
	case TASK_NEED_USES_PROPERTY:
	case TASK_NEED_HAVE_PROPERTY:
	case TASK_NEED_MEET_REQUIRES:
	case TASK_NEED_TRAP_CREATURE:
		{
			//跳过
		}
		break;
	case TASK_NEED_TALK_WITH_NPC:
		{
			// NPC动态配置表的seq
			Scene *scene = m_me->GetScene();
			if (NULL != scene)
			{
				bool ret = this->GetNPCTarget(task_meet_requires_type, scene->GetSceneID(),
					need_change_scene, scene_id, scene_key, target_posi);
				if (ret)
				{
					show_action_type = TASK_SHOW_ACTION_EMPLOY;
				}
			}
		}
		break;
	case TASK_NEED_KILL_MONSTERS:
	case TASK_NEED_KILL_MONGROUP:
	case TASK_NEED_KILL_IN_ORDER:
	case TASK_NEED_GAIN_TASKPROP:
		{
			//巡逻
			show_action_type = TASK_SHOW_ACTION_EMPLOY_RANGER;
		}
		break;
	case TASK_NEED_PASS_TRANSCRI:
		{
			//直接副本 无法获得副本入口位置  接受处消失
			const TaskInfomaConfig * task_cfg = TASKPOOL->GetTaskCfgByID(m_cur_task_id);

			Scene *scene = m_me->GetScene();
			if (NULL != scene && task_cfg != NULL)
			{
				bool ret = this->GetNPCTarget(task_cfg->accept_npc, scene->GetSceneID(),
					need_change_scene, scene_id, scene_key, target_posi);
				if (ret)
				{
					show_action_type = TASK_SHOW_ACTION_EMPLOY_FB;
				}
			}
		}
		break;
	case TASK_NEED_LOAD_PROGRESS:
		{
			//采集 提交处交互
			const TaskInfomaConfig * task_cfg = TASKPOOL->GetTaskCfgByID(m_cur_task_id);

			Scene *scene = m_me->GetScene();
			if (NULL != scene && task_cfg != NULL)
			{
				bool ret = this->GetNPCTarget(task_cfg->commit_npc, scene->GetSceneID(),
					need_change_scene, scene_id, scene_key, target_posi);
				if (ret)
				{
					show_action_type = TASK_SHOW_ACTION_EMPLOY_GATHER;
				}
			}
		}
		break;
	case TASK_NEED_BEAT_MONGROUP:
		{
			//战斗 提交处交互
			const TaskInfomaConfig * task_cfg = TASKPOOL->GetTaskCfgByID(m_cur_task_id);

			Scene *scene = m_me->GetScene();
			if (NULL != scene && task_cfg != NULL)
			{
				bool ret = this->GetNPCTarget(task_cfg->commit_npc, scene->GetSceneID(),
					need_change_scene, scene_id, scene_key, target_posi);
				if (ret)
				{
					show_action_type = TASK_SHOW_ACTION_EMPLOY_FIGHT;
				}
			}
		}
		break;
	default:
		break;
	}

	return show_action_type;
}

int RobotAIMission::OnActionEnd(time_t now_second)
{
	int action_type = m_action_type;
	switch (m_task_status)
	{
	case TASK_STATUS_GO_ACCEPT:
	case TASK_STATUS_ONGOING:
	case TASK_STATUS_GO_COMMIT:
		break;
	default:
		return action_type;
	}
	switch (action_type)
	{
	case ROBOT_AI_ACTION_GATHER:
	case ROBOT_AI_ACTION_HIDE:
		{
			int temp_param = m_task_show_action_param;
			this->SetTaskStatus(m_task_status + 1);
			m_task_show_action_param = temp_param + 1;
			this->SetAction(ROBOT_AI_ACTION_IDLE, 0);
		}
		break;
	case ROBOT_AI_ACTION_FIGHT:
		{//战斗结束
			if (TASK_SHOW_ACTION_EMPLOY_RANGER == m_task_show_action
				&& 6 > m_task_show_action_param)
			{
				MoveAssist* move_assist = m_me->GetMoveAssist(true);
				if (NULL == move_assist)
				{
					this->SetAction(ROBOT_AI_ACTION_IDLE, 0);
					return action_type;
				}

				move_assist->MoveContinue();

				this->SetAction(ROBOT_AI_ACTION_IDLE, 0);
				int temp_param = m_task_show_action_param;
				this->AITaskShowAction(TASK_SHOW_ACTION_NONE);
				m_task_show_action_param = temp_param + 1;
			}
			else
			{
				int temp_param = m_task_show_action_param;
				this->SetTaskStatus(m_task_status + 1);
				m_task_show_action_param = temp_param + 1;

				this->SetAction(ROBOT_AI_ACTION_IDLE, 0);
			}
		}
		break;
	case ROBOT_AI_ACTION_MOVE:
		{//移动结束 开始战斗
			if (TASK_SHOW_ACTION_EMPLOY_RANGER != m_task_show_action)
			{
				int temp_param = m_task_show_action_param;
				this->SetTaskStatus(m_task_status + 1);
				m_task_show_action_param = temp_param + 1;
				return action_type;
			}

			MoveAssist* move_assist = m_me->GetMoveAssist(true);
			if (NULL == move_assist)
			{
				this->SetAction(ROBOT_AI_ACTION_IDLE, 0);
				return action_type;
			}
			move_assist->MovePause();

			m_me->ActionStop();

			time_t fight_end_time = now_second
				+ RandomNum(DEF_VIRTUAL_FIGHT_TIME_MIN, DEF_VIRTUAL_FIGHT_TIME_MAX);
			this->SetAction(ROBOT_AI_ACTION_FIGHT, fight_end_time);
			m_task_show_action_param++;

		}
		break;
	default:
		break;
	}
	return action_type;
}

void RobotAIMission::DeleteMe()
{
	if (-1 != m_cur_task_id)
	{
		m_cur_task_id = -1;
		Robot *robot = (Robot *)m_me;
		robot->SetDelayDelete();
	}
}

bool RobotAIMission::GetNPCTarget(int npc_seq_id, int now_scene_id, int & need_change_scene, int & scene_id, int & scene_key, Posi & target_posi) const
{
	if (0 == npc_seq_id)
	{
		return false;
	}
	const NPCDynamicInformaCfg *npc_cfg = NPCPOOL->GetNPCInformaCfg(npc_seq_id);
	if (NULL == npc_cfg)
	{
		return false;
	}

	if (now_scene_id != npc_cfg->scene_id)
	{
		Scene * to_scene = World::GetInstWorld()->GetSceneManager()->GetSceneById(npc_cfg->scene_id, COMMON_SCENE_KEY);
		if (NULL == to_scene)
		{//场景不存在  跳过
			return false;
		}

		//换场景
		need_change_scene = 1;
		to_scene->GetTownPoint(&target_posi);
		scene_id = npc_cfg->scene_id;
		scene_key = COMMON_SCENE_KEY;
	}
	else
	{
		need_change_scene = 0;
		target_posi = npc_cfg->npc_posi;
	}
	return true;
}

void RobotAIMission::SetTaskShowAction(int task_show_action)
{
	m_task_show_action = task_show_action;
	m_task_show_action_param = 0;
	m_next_idle_check_time = 0;
	this->SetAction(ROBOT_AI_ACTION_IDLE, 0);
}

void RobotAIMission::SetTaskStatus(int task_status)
{
	m_task_status = task_status;

	this->SetTaskShowAction(TASK_SHOW_ACTION_NONE);
}
