#include "robotaibase.hpp"
#include "obj/character/character.h"
#include "obj/MoveAssist/MoveAssist.hpp"
#include "other/rolestatusmanager.hpp"
#include "scene/scene.h"

#include "protocol/msgrole.h"

RobotAIBase::RobotAIBase(Character * me, int robot_ai_type)
	: AIBase(me)
{
	m_robot_ai_type = robot_ai_type;

	m_action_type = ROBOT_AI_ACTION_IDLE;
	m_action_end_time = 0;
	
}

RobotAIBase::~RobotAIBase()
{
}

void RobotAIBase::Update(unsigned long interval, time_t now_second)
{
	UNSTD_STATIC_CHECK(6 == ROBOT_AI_ACTION_MAX);
	switch (m_action_type)
	{
	case ROBOT_AI_ACTION_IDLE:
	case ROBOT_AI_ACTION_MOVE:
		{
			this->AI(now_second);
		}
	default:
		{
			if (now_second > m_action_end_time)
			{
				this->OnActionEnd(now_second);
			}
		}
	}
}

void RobotAIBase::SetAction(int action_type, time_t action_end_time)
{
	int old_role_status = this->GetRoleStatus(m_action_type);

	if (ROBOT_AI_ACTION_MOVE != action_type)
	{
		m_me->ActionStop();
	}


	m_action_type = action_type;
	m_action_end_time = action_end_time;

	if (this->GetRoleStatus(m_action_type) != old_role_status)
	{
		this->OnRobotAIChangeRoleStatus(m_action_type);
	}
}

int RobotAIBase::GetRoleStatus(int action_type)
{
	action_type = action_type == -1 ? m_action_type : action_type;
	UNSTD_STATIC_CHECK(6 == ROBOT_AI_ACTION_MAX);
	switch (action_type)
	{
	case ROBOT_AI_ACTION_IDLE:
	case ROBOT_AI_ACTION_WAIT:
		{
			return RoleStatusManager::ROLE_STATUS_IDLE;
		}
		break;
	case ROBOT_AI_ACTION_MOVE:
		{
			return RoleStatusManager::ROLE_STATUS_MOVE;
		}
		break;
	case ROBOT_AI_ACTION_FIGHT:
		{
			return RoleStatusManager::ROLE_STATUS_FIGHT;
		}
		break;
	case ROBOT_AI_ACTION_GATHER:
		{
			return RoleStatusManager::ROLE_STATUS_GATHER;
		}
		break;
	case ROBOT_AI_ACTION_HIDE:
	default:
		return RoleStatusManager::ROLE_STATUS_INVALID;
	}
}

void RobotAIBase::OnRobotAIChangeRoleStatus(int action_type)
{
	Scene *scene = m_me->GetScene();
	if (NULL == m_me->GetScene())
	{
		return;
	}
	static Protocol::SCRoleStatusChangeNotice rscn;

	rscn.obj_id = m_me->GetId();
	rscn.cur_status = action_type;
	rscn.cur_move_status = 0;

	scene->GetZoneMatrix()->NotifyAreaMsg(m_me->GetObserHandle(), &rscn, sizeof(rscn));
}
