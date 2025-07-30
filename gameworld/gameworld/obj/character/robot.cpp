#include "robot.h"
#include "servercommon/robotdef.hpp"
#include "other/robot/robotmanager.hpp"
#include "global/team/teammanager.hpp"
#include "global/team/team.hpp"
#include "engineadapter.h"
#include "scene/scene.h"
#include "protocol/msgrole.h"
#include "other/rolestatusmanager.hpp"

#include "servercommon/struct/global/robotaiparam.hpp"
#include "ai/robotai/robotaibase.hpp"
#include "ai/robotai/robotaimission.hpp"
#include "ai/robotai/robotairanger.hpp"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "world.h"
#include "other/route/offlinerobotroute.hpp"

Robot::Robot() : Character(OBJ_TYPE_ROBOT), m_robot_id(0), m_level(0), m_profession(0), 
	m_avatar_type(0), m_team(NULL), m_team_index(0), m_delay_delete_time(0),
	m_special_appearance_type(0), m_special_appearance_param(0),
	m_bound_team_type(0), m_bound_battle_mode(0), m_real_role_uid(0)
{
	m_robot_ai_type = ROBOT_AI_TYPE_DEF;
	m_robot_ai = NULL;
}

Robot::~Robot()
{
	if (!this->IsRobotInvalid())
	{
		ROBOTMANAGER.RemoveRobot(this, __FUNCTION__, false);	
	}
	
	if (NULL != m_robot_ai)
	{
		delete m_robot_ai;
		m_robot_ai = NULL;
	}
}

void Robot::Init(const RobotParam& param)
{
	m_robot_id = param.robot_id;
	m_robot_user_id = IntToUid(m_robot_id);
	F_STRNCPY(m_name, param.robot_name, sizeof(m_name));
	if (CrossConfig::Instance().IsHiddenServer() 
		&& !CheckNameHasServerSuffix(m_name)
		&& 0 == param.real_role_uid)
	{
		int rand_mother_server_id = 0;
		if (World::GetInstWorld()->RandCanCreatAutoRobotServerID(&rand_mother_server_id))
		{
			SNPRINTF(m_name, sizeof(m_name), "%s_s%d", param.robot_name, rand_mother_server_id);
		}
	}

	m_level = param.level;
	m_profession = param.profession;
	m_avatar_type = param.avatar_type;
	m_battle_data = param.role_battle_data;
	m_robot_type = param.robot_type;
	m_bound_team_type = param.bound_team_type;
	m_bound_battle_mode = param.bound_battle_mode;
	m_real_role_uid = param.real_role_uid;

	if (NULL != m_robot_ai)
	{
		delete m_robot_ai;
		m_robot_ai = NULL;
	}
	switch (m_robot_type)
	{
	case ROBOT_TYPE_MISSION_ROBOT:
		{
			m_robot_ai = new RobotAIMission(this);

		}
		break;
	case ROBOT_TYPE_RANGER_ROBOT:
		{
			m_robot_ai = new RobotAIRanger(this);

		}
		break;
	case ROBOT_TYPE_NORMAL:
	case ROBOT_TYPE_TASK_ROBOT:
	case ROBOT_TYPE_STATIC_ROBOT:
	default:
		{
			m_robot_ai = new RobotAIBase(this, ROBOT_AI_TYPE_DEF);
		}
		break;
	}
	m_robot_ai->Init(param.robot_ai_param);
}

void Robot::GetInitParam(RobotParam* out_param)
{
	if (NULL == out_param) return;

	out_param->robot_id = m_robot_id;
	F_STRNCPY(out_param->robot_name, m_name, sizeof(out_param->robot_name));
	out_param->level = m_level;
	out_param->profession = m_profession;
	out_param->avatar_type = m_avatar_type;
	out_param->role_battle_data = m_battle_data;
	out_param->robot_type = m_robot_type;
	out_param->bound_team_type = m_bound_team_type;
	out_param->bound_battle_mode = m_bound_battle_mode;
	out_param->real_role_uid = m_real_role_uid;

	m_robot_ai->GetInitParam(out_param->robot_ai_param);

	/*if (NULL != m_team)
	{
		out_param->team_index = m_team->GetTeamIndex();
	}
	else
	{
		out_param->team_index = 0;
	}*/
}

void Robot::Update(unsigned long interval, time_t now_second, unsigned int now_dayid)
{
	if (NULL == m_scene) return;

	Character::Update(interval, now_second, now_dayid);

	if (m_pos_data.m_x > 0)
	{
		if (this->Move(m_pos_data.m_x, m_pos_data.m_y, m_pos_data.m_dir, m_pos_data.m_distance, 0))
		{	
			m_pos_data.m_x = 0;
		}
	}

	if (m_delay_delete_time > 0u && now_second > m_delay_delete_time && this->IsValid())
	{
		if (!this->IsRobotInvalid())
		{
			ROBOTMANAGER.RemoveRobot(this, __FUNCTION__);
		}
		m_scene->DeleteObj(this->GetId());
	}
	else if (NULL != m_robot_ai)
	{
		m_robot_ai->Update(interval, now_second);
	}
}

void Robot::SetTeam(Team* team)
{
	if (NULL != team)
	{
		m_team = team;
		m_team_index = team->GetTeamIndex();
	}
	else
	{
		m_team = NULL;
		m_team_index = 0;
	}
}

bool Robot::ResetPos(Axis x, Axis y)
{
	Posi old_pos = m_posi;

	if (Move(x, y, Dir_S, 0, 0, false))
	{
		m_last_check_valid_time = 0;
	}

	return false;
}

void Robot::UpdateHpMp(int remain_hp, int remain_mp)
{
	int max_hp = m_battle_data.role_character_data.attr_list[BATTLE_ATTR_MAX_HP];
	int max_mp = m_battle_data.role_character_data.attr_list[BATTLE_ATTR_MAX_MP];
	int cur_hp = GetMin(remain_hp, max_hp);
	cur_hp = GetMax(1, cur_hp);
	int cur_mp = GetMin(remain_mp, max_mp);
	cur_mp = GetMax(0, cur_mp);

	m_battle_data.role_character_data.attr_list[BATTLE_ATTR_CUR_HP] = cur_hp;
	m_battle_data.role_character_data.attr_list[BATTLE_ATTR_CUR_MP] = cur_mp;

	this->SendHpMpInfoToTeam();
}

void Robot::UpdatePetHpMp(int pet_id, int pet_origin_idx, int remain_hp, int remain_mp)
{
	for (int i = 0; i < m_battle_data.pet_num; ++i)
	{
		BattleCharacterData& data = m_battle_data.pet_character_data[i].battle_data;
		if (pet_id == data.character_id && 
			pet_origin_idx == data.id_param)
		{
			int max_hp = data.attr_list[BATTLE_ATTR_MAX_HP];
			int max_mp = data.attr_list[BATTLE_ATTR_MAX_MP];
			int cur_hp = GetMin(remain_hp, max_hp);
			cur_hp = GetMax(1, cur_hp);
			int cur_mp = GetMin(remain_mp, max_mp);
			cur_mp = GetMax(0, cur_mp);

			data.attr_list[BATTLE_ATTR_CUR_HP] = cur_hp;
			data.attr_list[BATTLE_ATTR_CUR_MP] = cur_mp;
		}
	}
}

void Robot::SetDelayDelete()
{
	m_delay_delete_time = static_cast<unsigned int>(EngineAdapter::Instance().Time()) + 1;
}

void Robot::SetSpecialAppearance(short special_appearance_type, int special_appearance_param)
{
	m_special_appearance_type = special_appearance_type;
	m_special_appearance_param = special_appearance_param;
}

void Robot::GetSpecialAppearance(short* special_appearance_type, int* special_appearance_param)
{
	if (NULL != special_appearance_type) *special_appearance_type = m_special_appearance_type;
	if (NULL != special_appearance_param) *special_appearance_param = m_special_appearance_param;
}

void Robot::OnRobotStartBattle()
{
	m_robot_ai->SetAction(RobotAIBase::ROBOT_AI_ACTION_FIGHT, 0);

}

void Robot::OnRobotStopBattle()
{
	m_robot_ai->SetAction(RobotAIBase::ROBOT_AI_ACTION_IDLE, 0);

}

bool Robot::ChangeHp(int ch_value, bool is_percent /* = false*/)
{
	if (is_percent)
	{
		ch_value = (int)(1.0 * ch_value * m_battle_data.role_character_data.attr_list[BATTLE_ATTR_MAX_HP] / BATTLE_PERCENT_NUM);
	}
	if (ch_value > 0 && m_battle_data.role_character_data.attr_list[BATTLE_ATTR_CUR_HP] >= m_battle_data.role_character_data.attr_list[BATTLE_ATTR_MAX_HP])
	{
		return false;
	}

	IntHelpAdd(m_battle_data.role_character_data.attr_list[BATTLE_ATTR_CUR_HP], m_battle_data.role_character_data.attr_list[BATTLE_ATTR_MAX_HP], ch_value);

	this->SendHpMpInfoToTeam();
	return true;
}

bool Robot::ChangeMp(int ch_value, bool is_percent /* = false*/)
{
	if (is_percent)
	{
		ch_value = (int)(1.0 * ch_value * m_battle_data.role_character_data.attr_list[BATTLE_ATTR_MAX_MP] / BATTLE_PERCENT_NUM);
	}
	if (ch_value > 0 && m_battle_data.role_character_data.attr_list[BATTLE_ATTR_CUR_MP] >= m_battle_data.role_character_data.attr_list[BATTLE_ATTR_MAX_MP])
	{
		return false;
	}

	IntHelpAdd(m_battle_data.role_character_data.attr_list[BATTLE_ATTR_CUR_MP], m_battle_data.role_character_data.attr_list[BATTLE_ATTR_MAX_MP], ch_value);
	this->SendHpMpInfoToTeam();
	return true;
}

bool Robot::ChangePetHp(int ch_value, bool is_percent /* = false*/)
{
	if (m_battle_data.pet_num < 0)
	{
		return false;
	}

	BattleCharacterData& data = m_battle_data.pet_character_data[0].battle_data;
	if (is_percent)
	{
		ch_value = (int)(1.0 * ch_value * data.attr_list[BATTLE_ATTR_MAX_HP] / BATTLE_PERCENT_NUM);
	}
	if (ch_value > 0 && data.attr_list[BATTLE_ATTR_CUR_HP] >= data.attr_list[BATTLE_ATTR_MAX_HP])
	{
		return false;
	}

	IntHelpAdd(data.attr_list[BATTLE_ATTR_CUR_HP], data.attr_list[BATTLE_ATTR_MAX_HP], ch_value);

	return true;
}

bool Robot::ChangePetMp(int ch_value, bool is_percent /* = false*/)
{
	if (m_battle_data.pet_num < 0)
	{
		return false;
	}

	BattleCharacterData& data = m_battle_data.pet_character_data[0].battle_data;
	if (is_percent)
	{
		ch_value = (int)(1.0 * ch_value * data.attr_list[BATTLE_ATTR_MAX_MP] / BATTLE_PERCENT_NUM);
	}
	if (ch_value > 0 && data.attr_list[BATTLE_ATTR_CUR_MP] >= data.attr_list[BATTLE_ATTR_MAX_MP])
	{
		return false;
	}

	IntHelpAdd(data.attr_list[BATTLE_ATTR_CUR_MP], data.attr_list[BATTLE_ATTR_MAX_MP], ch_value);

	return true;
}

void Robot::SetRobotLevel(int level)
{
	m_level = level;
}

void Robot::SendHpMpInfoToTeam()
{
	Team* team = this->GetTeam();
	if (NULL != team)
	{
		BattleCharacterData& data = m_battle_data.role_character_data;
		Attribute cur_hp = data.attr_list[BATTLE_ATTR_CUR_HP];
		Attribute max_hp = data.attr_list[BATTLE_ATTR_MAX_HP];
		Attribute cur_mp = data.attr_list[BATTLE_ATTR_CUR_MP];
		Attribute max_mp = data.attr_list[BATTLE_ATTR_MAX_MP];

		Protocol::SCUpdateMemberInfo msg;
		msg.uid = m_robot_id;
		msg.is_partner = 0;
		msg.hp = cur_hp;
		msg.max_hp = max_hp;
		msg.mp = cur_mp;
		msg.max_mp = max_mp;
		msg.skin_id = 0;
		msg.level = m_level;

		team->SendToMember(&msg, sizeof(msg));
	}
}

void Robot::OnLeaveTeam()
{
	if (this->GetRealRoleUID() > 0)
	{
		OfflineRobotRoute::NotifyRealRoleRobotLeaveTeam(this->GetRealRoleUID());
	}
}

