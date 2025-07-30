#include "robotairanger.hpp"
#include "obj/MoveAssist/MoveAssist.hpp"
#include "scene/scene.h"
#include "scene/speciallogic.hpp"

RobotAIRanger::RobotAIRanger(Character * me)
	: RobotAIBase(me, ROBOT_AI_TYPE_RANGER)
{
	m_next_idle_check_time = 0;
}

RobotAIRanger::~RobotAIRanger()
{
}

void RobotAIRanger::AI(time_t now_second)
{
	//未移动代表没有辅助移动返回(没人 可以待机) 或战斗(也不用管)
	switch (m_me->GetActionStatus())
	{
	case Character::ACTION_IDLE:
		{
			if (ROBOT_AI_ACTION_IDLE != m_action_type)
			{
				break;
			}
			if (m_next_idle_check_time > now_second)
			{
				break;
			}
			m_next_idle_check_time = now_second + 10;
		
			//巡逻路径检查
			MoveAssist* move_assist = m_me->GetMoveAssist(true);
			if (NULL == move_assist)
			{
				return;
			}
			switch (move_assist->GetMoveAssistStatus())
			{
			case MoveAssist::MOVE_ASSIST_STATUS_TYPE_NONE:
				{//巡路结束 或未开始巡路
					move_assist->SetDestination(Posi(-1, -1), MoveAssist::MOVE_ASSIST_REQ_TYPE_AROND);
				}
				break;
			default:
				break;
			}
			return;
		}
		break;
	case Character::ACTION_RUN:
		{
			if (ROBOT_AI_ACTION_MOVE != m_action_type)
			{//开始移动了 设置状态
				//计算遇怪时间戳
				Scene *scene = m_me->GetScene();
				if (NULL == scene)
				{
					return;
				}
// 				SpecialLogic * sp_logic = scene->GetSpecialLogic();
// 				int next_fight_time =
// 					(NULL != sp_logic) ? sp_logic->GetMineMonsterEncounterInterval(this) : scene->RandMineMonsterEncounterTimeMS();
				time_t next_fight_time = now_second + scene->RandMineMonsterEncounterTimeMS() / 1000;
				if (5 + now_second > next_fight_time)
				{
					next_fight_time = now_second + DEF_ENCOUNTER_FIGHT_TIME;
				}
				this->SetAction(ROBOT_AI_ACTION_MOVE, next_fight_time);
			}
		}
		break;
	default:
		break;
	}
}

int RobotAIRanger::OnActionEnd(time_t now_second)
{
	int action_type = m_action_type;
	switch (action_type)
	{
	case ROBOT_AI_ACTION_FIGHT:
		{//战斗结束
			MoveAssist* move_assist = m_me->GetMoveAssist(true);
			if (NULL != move_assist)
			{
				move_assist->MoveContinue();

			}
			//可以不用设置状态 在ai中处理
			this->SetAction(ROBOT_AI_ACTION_IDLE, 0);
			return action_type;
		}
		break;
	case ROBOT_AI_ACTION_MOVE:
		{//移动结束 开始战斗
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
		}
		break;
	default:
		break;
	}
	return 0;
}
