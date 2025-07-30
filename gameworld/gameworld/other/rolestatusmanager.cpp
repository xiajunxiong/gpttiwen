#include "obj/character/role.h"
#include "rolestatusmanager.hpp"
#include "servercommon/errornum.h"
#include "other/event/eventhandler.hpp"
#include "battle/battle_manager_local.hpp"
#include "global/drawmanager/drawmanager.hpp"
#include "global/team/team.hpp"
#include "scene/speciallogic.hpp"
#include "scene/scene.h"

RoleStatusManager::RoleStatusManager() : m_role(NULL), 
	m_role_status(ROLE_STATUS_IDLE), m_move_status(ROLE_MOVE_STATUS_NORMAL)
{
	m_xun_you_status = 0;
}

RoleStatusManager::~RoleStatusManager()
{
}

void RoleStatusManager::OnRoleStartFight(int battle_mode)
{
	if (ROLE_STATUS_GATHER == m_role_status && BATTLE_MODE_DUEL == battle_mode && NULL != m_role)
	{
		m_role->NoticeNum(errornum::EN_GATHER_FAIL_BY_DUEL);
	}

	this->SetRoleStatus(ROLE_STATUS_FIGHT);
}

void RoleStatusManager::OnRoleFinishFight()
{
	this->SetRoleStatus(ROLE_STATUS_IDLE);
}

void RoleStatusManager::OnRoleStartMove()
{
	this->SetRoleStatus(ROLE_STATUS_MOVE);
}

void RoleStatusManager::OnRoleStopMove()
{
	this->SetRoleStatus(ROLE_STATUS_IDLE);
}

void RoleStatusManager::OnRoleStartGather()
{
	this->SetRoleStatus(ROLE_STATUS_GATHER);
}

void RoleStatusManager::OnRoleStopGather()
{
	this->SetRoleStatus(ROLE_STATUS_IDLE);
}

void RoleStatusManager::OnRoleStartFishing()
{
	this->SetRoleStatus(ROLE_STATUS_FISHING);
}

void RoleStatusManager::OnRoleStopFishing()
{
	this->SetRoleStatus(ROLE_STATUS_IDLE);
}

void RoleStatusManager::OnRoleStartSneak()
{
	this->SetRoleMoveStatus(ROLE_MOVE_STATUS_SNEAK);
}

void RoleStatusManager::OnRoleStartShiFa()
{
	this->SetRoleMoveStatus(ROLE_MOVE_STATUS_SHIFA);
}

void RoleStatusManager::OnRoleStartYinDao()
{
	this->SetRoleMoveStatus(ROLE_MOVE_STATUS_YINDAO);
}

void RoleStatusManager::OnRoleStopMoveStatus()
{
	this->SetRoleMoveStatus(ROLE_MOVE_STATUS_NORMAL);
}

bool RoleStatusManager::CanRoleStartFight(int battle_mode)
{
	switch (m_role_status)
	{	
		case ROLE_STATUS_IDLE:
		case ROLE_STATUS_MOVE:
		break;
		case ROLE_STATUS_GATHER:
		{
			if (BATTLE_MODE_DUEL != battle_mode)
			{
				return false;
			}
		}
		break;
		default:
		return false;
		break;
	}
	/*if (ROLE_STATUS_IDLE != m_role_status && ROLE_STATUS_MOVE != m_role_status)
	{
		return false;
	}*/
	{//场景战斗限制
		Scene *scene = m_role->GetScene();
		if (NULL == scene)
		{
			return false;
		}
		static SpecialLogic::CanRoleStartFightParam fight_param;
		fight_param.battle_mode = battle_mode;
		if (!scene->GetSpecialLogic()->CanRoleStartFight(m_role, fight_param))
		{
			return false;
		}
	}
	//队友正在翻牌
	Team* team = m_role->GetMyTeam();
	if (NULL != team && DrawManager::Instance().IsDrawing(m_role->GetUID()))
	{
		Role* leader = team->GetLeaderRole();
		if (NULL != leader)
		{
			leader->NoticeNum(errornum::EN_DRAW_MANAGER_SELECTING);
			return false;
		}
	}
	//巡游
	if (1 == this->GetXunYouStatus())
	{
		return false;
	}
	return true;
}

bool RoleStatusManager::CanRoleMove()
{
	if (ROLE_STATUS_FIGHT == m_role_status)
	{
		//m_role->NoticeNum(errornum::EN_ROLE_IN_BATTLE_9);

		BattleManagerLocal::GetInstance().CheckRoleReallyInBattle(m_role, __FUNCTION__);
		return false;
	}

	return true;
}

bool RoleStatusManager::CanFlyByShoe(int target_scene_id)
{
	if (ROLE_STATUS_FIGHT == m_role_status || ROLE_STATUS_GATHER == m_role_status || ROLE_STATUS_FISHING == m_role_status)
	{
		return false;
	}

	return true;
}

bool RoleStatusManager::CanTransportTo(int target_scene_id)
{
	if (ROLE_STATUS_FIGHT == m_role_status || ROLE_STATUS_GATHER == m_role_status || ROLE_STATUS_FISHING == m_role_status)
	{
		return false;
	}

	return true;
}

bool RoleStatusManager::CanGather(int seq_id)
{
	if (ROLE_STATUS_FIGHT == m_role_status || ROLE_STATUS_GATHER == m_role_status || ROLE_STATUS_FISHING == m_role_status) return false;
	
	return true;
}

bool RoleStatusManager::CanStartCross()
{
	if (ROLE_STATUS_FIGHT == m_role_status || ROLE_STATUS_GATHER == m_role_status || ROLE_STATUS_FISHING == m_role_status) return false;

	return true;
}

bool RoleStatusManager::CanFish()
{
	if (ROLE_STATUS_FIGHT == m_role_status || ROLE_STATUS_GATHER == m_role_status || ROLE_STATUS_FISHING == m_role_status) return false;

	return true;
}

bool RoleStatusManager::IsInBattleStatus()
{
	return ROLE_STATUS_FIGHT == m_role_status;
}

void RoleStatusManager::SetRoleStatus(int status)
{
	int old_status = m_role_status;
	m_role_status = status;
	EventHandler::Instance().OnRoleChangeStatus(m_role, old_status, m_role_status);
}

void RoleStatusManager::SetRoleMoveStatus(int status)
{
	int old_status = m_move_status;
	m_move_status = status;
	EventHandler::Instance().OnRoleChangeStatus(m_role, old_status, m_move_status);
}
