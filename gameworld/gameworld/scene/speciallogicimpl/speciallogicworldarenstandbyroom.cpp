#include "speciallogicworldarenstandbyroom.hpp"
#include "engineadapter.h"
#include "global/worldarena/worldarena.hpp"
#include "scene/scene.h"
#include "other/robot/robotmanager.hpp"
#include "obj/character/robot.h"
#include "obj/character/role.h"
#include "servercommon/robotdef.hpp"
#include "other/rolebattledatacapabilitycalculator.hpp"
#include "servercommon/string/gameworldstr.h"
#include "world.h"
#include "battle/battle_manager_local.hpp"
#include "global/worldarena/worldarenaconfig.hpp"
#include "config/logicconfigmanager.hpp"
#include "gamelog.h"
#include "scene/map.h"
#include "protocol/msgworldarena.hpp"

SpecialLogicWorldArenaStandbyRoom::SpecialLogicWorldArenaStandbyRoom(Scene *scene) : SpecialLogic(scene),
	m_schedule_id(0LL), m_attacker_uuid(0LL), m_defender_uuid(0LL), m_attacker_uid(0),
	m_defender_uid(0), m_create_robot_timestamp(0u), m_two_robot_pk_result_timestamp(0u),
	m_next_reminder_timestamp(0u), m_force_start_battle_reminder_timestamp(0u),
	m_force_start_battle_timestamp(0u), m_start_battle_timestamp(0u), m_kick_all_out_timestamp(0u),
	m_countdown_notice_timestamp(0u), m_attacker(NULL), m_defender(NULL), m_is_attacker_robot(false),
	m_is_defender_robot(false), m_attacker_ready(false), m_defender_ready(false),
	m_status(STATUS_NOT_STANDBY), m_can_destroy(false), m_has_attacker_entered(false),
	m_has_defender_entered(false)
{
	m_create_robot_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time()) + RandomNum(20);
	m_two_robot_pk_result_timestamp = m_create_robot_timestamp + RandomNum(60, 120);
	m_next_reminder_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time()) + EngineAdapter::Instance().NextMinuteInterval(0);
	m_force_start_battle_reminder_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time()) + 9 * 60 + 50; 
	m_force_start_battle_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time()) 
		+ LOGIC_CONFIG->GetWorldArenaConfig()->GetOtherCfg().wait_enter_scene_minute * 60;
	m_kick_all_out_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time()) + 20 * 60;
}

SpecialLogicWorldArenaStandbyRoom::~SpecialLogicWorldArenaStandbyRoom()
{

}

void SpecialLogicWorldArenaStandbyRoom::Update(unsigned long interval, time_t now_second)
{
	unsigned int now = static_cast<unsigned int>(now_second);
	if (m_create_robot_timestamp > 0u && now > m_create_robot_timestamp)
	{
		this->CreateRobotIfNeeded();
		m_create_robot_timestamp = 0u;
		return;
	}

	if (m_two_robot_pk_result_timestamp > 0u && now > m_two_robot_pk_result_timestamp)
	{
		this->CalculateResultIfTwoRobot();
		m_two_robot_pk_result_timestamp = 0u;
		return;
	}

	if (m_next_reminder_timestamp > 0u && now > m_next_reminder_timestamp)
	{
		this->SendStandbyReminder();
		m_next_reminder_timestamp = now + EngineAdapter::Instance().NextMinuteInterval(0);
		return;
	}

	if (m_force_start_battle_reminder_timestamp > 0u && now > m_force_start_battle_reminder_timestamp)
	{
		this->SendForceStartReminderIfNeeded();
		m_force_start_battle_reminder_timestamp = 0u;
		return;
	}

	if (m_force_start_battle_timestamp > 0u && now > m_force_start_battle_timestamp)
	{
		this->ForceStartBattleIfNeeded();
		m_force_start_battle_timestamp = 0u;
		return;
	}

	if (m_start_battle_timestamp > 0u && now >= m_start_battle_timestamp)
	{
		if (this->TryStartBattle())
		{
			m_start_battle_timestamp = 0u;
		}
		else
		{
			m_start_battle_timestamp = now + WORLD_ARENA_COUNT_DOWN_S + 2;
			m_countdown_notice_timestamp = now + 2;
		}
		return;
	}

	if (m_kick_all_out_timestamp > 0u && now > m_kick_all_out_timestamp)
	{
		this->DelayKickOutAllRole();
		m_kick_all_out_timestamp = 0u;
		m_can_destroy = true;
	}

	if (m_countdown_notice_timestamp > 0u && now >= m_countdown_notice_timestamp)
	{
		m_countdown_notice_timestamp = 0u;
		this->SendCountDownToFightNotice(WORLD_ARENA_COUNT_DOWN_S);
	}
}

void SpecialLogicWorldArenaStandbyRoom::OnRoleEnterScene(Role *role)
{
	if (this->IsInFightFinishStatus())
	{
		this->DelayKickOutRole(role);
		return;
	}

	if (role->GetUUID() != m_attacker_uuid && role->GetUUID() != m_defender_uuid)
	{
		this->DelayKickOutRole(role);
		return;
	}

	if (role->GetUUID() == m_attacker_uuid)
	{
		m_attacker = role;
		m_has_attacker_entered = true;

		Posi pos = LOGIC_CONFIG->GetWorldArenaConfig()->GetOtherCfg().attacker_posi;
		role->ResetPos(pos.x, pos.y);
	}

	if (role->GetUUID() == m_defender_uuid)
	{
		m_defender = role;
		m_has_defender_entered = true;

		Posi pos = LOGIC_CONFIG->GetWorldArenaConfig()->GetOtherCfg().defender_posi;
		role->ResetPos(pos.x, pos.y);
	}

	gamelog::g_log_world_arena.printf(LL_INFO, "OnRoleEnterScene role[%d,%lld,%s] schedule_id[%lld]",
		role->GetUID(), role->GetUUID(), role->GetName(), m_schedule_id);

	this->OnRoleReady(role, true);
	this->SendShowUpInfo();
	this->SendNoticeOnRoleEnter(role);

	BattleManagerLocal::GetInstance().CheckRoleReallyInBattle(role, __FUNCTION__);
}

void SpecialLogicWorldArenaStandbyRoom::OnRoleLeaveScene(Role *role, bool is_logout)
{
	gamelog::g_log_world_arena.printf(LL_INFO, "OnRoleLeaveScene role[%d,%lld,%s] schedule_id[%lld]",
		role->GetUID(), role->GetUUID(), role->GetName(), m_schedule_id);

	this->OnRoleReady(role, false);

	if (role->GetUUID() == m_attacker_uuid)
	{
		m_attacker = NULL;
	}

	if (role->GetUUID() == m_defender_uuid)
	{
		m_defender = NULL;
	}

	this->SendShowUpInfo();
	this->SendSomeOneLeaveSceneWaitNotice();
}

bool SpecialLogicWorldArenaStandbyRoom::CanDestroy()
{
	return m_can_destroy;
}

long long SpecialLogicWorldArenaStandbyRoom::GetScheduleID()
{
	return m_schedule_id;
}

void SpecialLogicWorldArenaStandbyRoom::SetScheduleID(long long schedule_id)
{
	m_schedule_id = schedule_id;
}

void SpecialLogicWorldArenaStandbyRoom::SetAttackerUUID(long long attacker_uuid)
{
	m_attacker_uuid = attacker_uuid;
	m_is_attacker_robot = WorldArena::Instance().IsRobot(attacker_uuid);
}

void SpecialLogicWorldArenaStandbyRoom::SetDefenderUUID(long long defender_uuid)
{
	m_defender_uuid = defender_uuid;
	m_is_defender_robot = WorldArena::Instance().IsRobot(defender_uuid);
}

void SpecialLogicWorldArenaStandbyRoom::SetAttackerUID(int uid)
{
	m_attacker_uid = uid;
}

void SpecialLogicWorldArenaStandbyRoom::SetDefenderUID(int uid)
{
	m_defender_uid = uid;
}

void SpecialLogicWorldArenaStandbyRoom::SetAttackerName(const char* name)
{
	F_STRNCPY(m_attacker_name, name, sizeof(m_attacker_name));
}

void SpecialLogicWorldArenaStandbyRoom::SetDefenderName(const char* name)
{
	F_STRNCPY(m_defender_name, name, sizeof(m_defender_name));
}

void SpecialLogicWorldArenaStandbyRoom::OnInitFinish()
{
	this->SetStatus(STATUS_NOT_STANDBY, __FUNCTION__);
}

void SpecialLogicWorldArenaStandbyRoom::SendStandbyReminder()
{
	if (m_is_attacker_robot && m_is_defender_robot) return;
	if (!this->IsInNotStandbyStatus()) return;

	//int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_world_arena_standby_reminder);
	//World::GetInstWorld()->GetSceneManager()->SystemMsgScene(m_scene, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT));
}

void SpecialLogicWorldArenaStandbyRoom::SendForceStartReminderIfNeeded()
{
	if (!this->IsInNotStandbyStatus()) return;

	//int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_world_arena_force_start_battle_reminder);
	//World::GetInstWorld()->GetSceneManager()->SystemMsgScene(m_scene, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT));
}

void SpecialLogicWorldArenaStandbyRoom::SendBattleEndLeaveSceneNotice()
{
	int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_world_arena_battle_end_leave_scene);
	World::GetInstWorld()->GetSceneManager()->SystemMsgScene(m_scene, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT));
}

void SpecialLogicWorldArenaStandbyRoom::OnRoleReady(Role* role, bool is_ready)
{
	if (NULL == role) return;

	if (!this->IsInNotStandbyStatus() && !this->IsInBothStandbyStatus())
	{
		return;
	}

	if (is_ready)
	{
		if (role->GetUUID() == m_attacker_uuid)
		{
			m_attacker_ready = true;
		}

		if (role->GetUUID() == m_defender_uuid)
		{
			m_defender_ready = true;
		}

		if (m_attacker_ready && m_defender_ready)
		{
			this->SetStatus(STATUS_BOTH_STANDBY, __FUNCTION__);
			m_next_reminder_timestamp = 0u;
			m_start_battle_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time()) + WORLD_ARENA_COUNT_DOWN_S + 2;
			m_countdown_notice_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time()) + 2;
		}
	}
	else
	{
		if (role->GetUUID() == m_attacker_uuid)
		{
			m_attacker_ready = false;
		}

		if (role->GetUUID() == m_defender_uuid)
		{
			m_defender_ready = false;
		}

		if (!m_attacker_ready || !m_defender_ready)
		{
			this->SetStatus(STATUS_NOT_STANDBY, __FUNCTION__);
			m_next_reminder_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time()) + EngineAdapter::Instance().NextMinuteInterval(0);
			m_start_battle_timestamp = 0u;
		}
	}
}

void SpecialLogicWorldArenaStandbyRoom::SendCountDownToFightNotice(int count_down_s)
{
	Protocol::SCWorldArenaCountDownToFight msg;
	msg.count_down_s = count_down_s;

	m_scene->SendToRole((const char*)&msg, sizeof(msg));
}

void SpecialLogicWorldArenaStandbyRoom::SendSomeOneLeaveSceneWaitNotice()
{
	if (this->IsInFightStartStatus() || this->IsInFightFinishStatus()) return;

	int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_world_arena_someone_leave_scene_wait_notice);
	if (length <= 0) return;

	if (NULL != m_attacker && !m_is_attacker_robot)
	{
		World::GetInstWorld()->GetSceneManager()->SystemMsgPerson(IntToUid(m_attacker_uid), gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT));
	}
	
	if (NULL != m_defender && !m_is_defender_robot)
	{
		World::GetInstWorld()->GetSceneManager()->SystemMsgPerson(IntToUid(m_defender_uid), gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT));
	}
}

bool SpecialLogicWorldArenaStandbyRoom::IsInNotStandbyStatus()
{
	return STATUS_NOT_STANDBY == m_status;
}

bool SpecialLogicWorldArenaStandbyRoom::IsInBothStandbyStatus()
{
	return STATUS_BOTH_STANDBY == m_status;
}

bool SpecialLogicWorldArenaStandbyRoom::IsInFightStartStatus()
{
	return STATUS_START_BATTLE == m_status;
}

bool SpecialLogicWorldArenaStandbyRoom::IsBothRobot()
{
	return m_is_attacker_robot && m_is_defender_robot;
}

bool SpecialLogicWorldArenaStandbyRoom::IsInFightFinishStatus()
{
	return STATUS_BATTLE_END == m_status;
}

long long SpecialLogicWorldArenaStandbyRoom::GetAttackerUUID() const
{
	return m_attacker_uuid;
}

long long SpecialLogicWorldArenaStandbyRoom::GetDefenderUUID() const
{
	return m_defender_uuid;
}

bool SpecialLogicWorldArenaStandbyRoom::HasAttackerEverEntered() const
{
	return m_has_attacker_entered;
}

bool SpecialLogicWorldArenaStandbyRoom::HasDefenderEverEntered() const
{
	return m_has_defender_entered;
}

void SpecialLogicWorldArenaStandbyRoom::OnFightFinish(battlegameprotocol::BattleGameFightResultAck* ack)
{
	if (ack->battle_mode != BATTLE_MODE_WORLD_ARENA) return;

	if (RESULT_TYPE_ATTACKER_WIN == ack->battle_result)
	{
		WorldArena::Instance().SetFightResult(m_schedule_id, WORLD_ARENA_RESULT_ATTACKER_WIN, ack->statistic_data.round_num);
	}
	else if (RESULT_TYPE_DEFENDER_WIN == ack->battle_result)
	{
		WorldArena::Instance().SetFightResult(m_schedule_id, WORLD_ARENA_RESULT_DEFENDER_WIN, ack->statistic_data.round_num);
	}
	else
	{
		WorldArena::Instance().SetFightResult(m_schedule_id, WORLD_ARENA_RESULT_DRAW, ack->statistic_data.round_num);
	}

	this->OnBattleEnd();
}

void SpecialLogicWorldArenaStandbyRoom::OnRoleRunAway(battlegameprotocol::BattleGameRoleRunAway* ack)
{
	if (ack->battle_mode != BATTLE_MODE_WORLD_ARENA) return;

	if (ack->uid == UidToInt((LongLongToUniqueUserID(m_attacker_uuid).user_id)) && m_is_attacker_robot)
	{
		WorldArena::Instance().SetFightResult(m_schedule_id, WORLD_ARENA_RESULT_DEFENDER_WIN, ack->statistic_data.round_num);
		this->OnBattleEnd();
	}
	else if (ack->uid == UidToInt((LongLongToUniqueUserID(m_defender_uuid).user_id)) && m_is_defender_robot)
	{
		WorldArena::Instance().SetFightResult(m_schedule_id, WORLD_ARENA_RESULT_ATTACKER_WIN, ack->statistic_data.round_num);
		this->OnBattleEnd();
	}
}

void SpecialLogicWorldArenaStandbyRoom::OnBattleForcedFinish(battlegameprotocol::BattleGameForceFinishFight* ack)
{
	if (ack->battle_mode != BATTLE_MODE_WORLD_ARENA) return;

	WorldArena::Instance().SetFightResult(m_schedule_id, WORLD_ARENA_RESULT_DRAW, ack->statistic_data.round_num);

	this->OnBattleEnd();
}

bool SpecialLogicWorldArenaStandbyRoom::CanRoleStartFight(Role * role, CanRoleStartFightParam &fight_param)
{
	return BATTLE_MODE_WORLD_ARENA == fight_param.battle_mode;
}

int SpecialLogicWorldArenaStandbyRoom::GetAttackerUID()
{
	return UidToInt(LongLongToUniqueUserID(m_attacker_uuid).user_id);
}

int SpecialLogicWorldArenaStandbyRoom::GetDefenderUID()
{
	return UidToInt(LongLongToUniqueUserID(m_defender_uuid).user_id);
}

void SpecialLogicWorldArenaStandbyRoom::OnBattleEnd()
{
	m_kick_all_out_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());

	//this->SendBattleEndLeaveSceneNotice();
}

void SpecialLogicWorldArenaStandbyRoom::SendShowUpInfo()
{
	Protocol::SCWorldArenaShowUpInfo msg;
	msg.is_attacker_show_up = (NULL != m_attacker ? 1 : 0);
	msg.is_defender_show_up = (NULL != m_defender ? 1 : 0);
	this->AssembleShowUpRoleInfo(m_attacker_uuid, &msg.attacker_info);
	this->AssembleShowUpRoleInfo(m_defender_uuid, &msg.defender_info);

	m_scene->SendToRole((const char*)&msg, sizeof(msg));
}

void SpecialLogicWorldArenaStandbyRoom::AssembleShowUpRoleInfo(long long uuid, Protocol::MsgWorldArenaShowUpRoleInfo* role_info)
{
	WorldArena::Instance().AssembleShowUpRoleInfo(uuid, role_info);
}

void SpecialLogicWorldArenaStandbyRoom::LogRoomStatus(const char* func)
{
	gamelog::g_log_world_arena.printf(LL_INFO, "LogRoomStatus from[%s] status[%d] schedule_id[%lld] attacker[%d,%s,%lld,%s,%s] defender[%d,%s,%lld,%s,%s] \
create_robot_time[%u] start_battle_time[%u] kick_all_out_time[%u]",
		func, m_status, m_schedule_id, 
		m_attacker_uid, m_attacker_name, m_attacker_uuid, m_is_attacker_robot ? "Robot" : "Role", m_attacker_ready ? "Ready" : "NotReady",
		m_defender_uid, m_defender_name, m_defender_uuid, m_is_defender_robot ? "Robot" : "Role", m_defender_ready ? "Ready" : "NotReady",
		m_create_robot_timestamp, m_start_battle_timestamp, m_kick_all_out_timestamp);
}

void SpecialLogicWorldArenaStandbyRoom::SendNoticeOnRoleEnter(Role* role)
{
	if (role->GetUUID() == m_attacker_uuid)
	{
		if (NULL == m_defender)
		{
			this->SendOpponentNotShowupYetNotice(role);
		}
		else
		{
			this->SendOpponentShowupNotice(role, m_defender_name);

			if (!m_is_defender_robot)
			{
				Role* defender_role = (Role*)m_defender;
				this->SendOpponentShowupNotice(defender_role, m_attacker_name);
			}
		}
	}
	else
	{
		if (NULL == m_attacker)
		{
			this->SendOpponentNotShowupYetNotice(role);
		}
		else
		{
			this->SendOpponentShowupNotice(role, m_attacker_name);

			if (!m_is_attacker_robot)
			{
				Role* attacker_role = (Role*)m_attacker;
				this->SendOpponentShowupNotice(attacker_role, m_defender_name);
			}
		}
	}
}

void SpecialLogicWorldArenaStandbyRoom::SendNoticeOnRobotCreated(const char* robot_name)
{
	if (!m_is_attacker_robot && NULL != m_attacker)
	{
		Role* role = (Role*)m_attacker;
		this->SendOpponentShowupNotice(role, robot_name);
	}

	if (!m_is_defender_robot && NULL != m_defender)
	{
		Role* role = (Role*)m_defender;
		this->SendOpponentShowupNotice(role, robot_name);
	}
}

void SpecialLogicWorldArenaStandbyRoom::SendOpponentNotShowupYetNotice(Role* role)
{
	int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_world_arena_opponent_not_showup_yet);
	World::GetInstWorld()->GetSceneManager()->SystemMsgPerson2(role, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT));
}

void SpecialLogicWorldArenaStandbyRoom::SendOpponentShowupNotice(Role* role, const char* opponent_name)
{
	int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_world_arena_opponent_showup, opponent_name);
	World::GetInstWorld()->GetSceneManager()->SystemMsgPerson2(role, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT));
}

void SpecialLogicWorldArenaStandbyRoom::SendStartBattleFailedDrawNotice(Character* character)
{
	if (NULL == character) return;
	if (character->GetObjType() != Obj::OBJ_TYPE_ROLE) return;

	Role* role = (Role*)character;

	int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_world_arena_start_battle_failed_draw);
	World::GetInstWorld()->GetSceneManager()->SystemMsgPerson2(role, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT, SYS_MSG_SYSTEM_CHAT));
}

void SpecialLogicWorldArenaStandbyRoom::SendStartBattleFailedWinNotice(Character* character)
{
	if (NULL == character) return;
	if (character->GetObjType() != Obj::OBJ_TYPE_ROLE) return;

	Role* role = (Role*)character;

	int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_world_arena_start_battle_failed_win);
	World::GetInstWorld()->GetSceneManager()->SystemMsgPerson2(role, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT, SYS_MSG_SYSTEM_CHAT));
}

void SpecialLogicWorldArenaStandbyRoom::SendStartBattleFailedLoseNotice(Character* character)
{
	if (NULL == character) return;
	if (character->GetObjType() != Obj::OBJ_TYPE_ROLE) return;

	Role* role = (Role*)character;

	int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_world_arena_start_battle_failed_lose);
	World::GetInstWorld()->GetSceneManager()->SystemMsgPerson2(role, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT, SYS_MSG_SYSTEM_CHAT));
}

void SpecialLogicWorldArenaStandbyRoom::CreateRobotIfNeeded()
{
	assert(m_schedule_id > 0);
	assert(m_attacker_uuid > 0);
	assert(m_defender_uuid > 0);

	if (!m_is_attacker_robot && !m_is_defender_robot)
	{
		return;
	}
	
	if (m_is_attacker_robot)
	{
		RobotParam robot_param;
		if (WorldArena::Instance().AssembleRobotParam(m_attacker_uuid, robot_param))
		{
			Posi pos = LOGIC_CONFIG->GetWorldArenaConfig()->GetOtherCfg().attacker_posi;
			if (!m_scene->GetMap()->Validate(Obj::OBJ_TYPE_ROBOT, pos.x, pos.y))
			{
				pos = m_scene->GetARandPosi();
			}
			Robot* robot = RobotManager::Instance().CreateRobotToScene(m_scene, pos, robot_param, __FUNCTION__);
			assert(NULL != robot);

			m_attacker = robot;
			m_attacker_ready = true;

			this->SendNoticeOnRobotCreated(robot_param.robot_name);
		}
	}

	if (m_is_defender_robot)
	{
		RobotParam robot_param;
		if (WorldArena::Instance().AssembleRobotParam(m_defender_uuid, robot_param))
		{
			Posi pos = LOGIC_CONFIG->GetWorldArenaConfig()->GetOtherCfg().defender_posi;
			if (!m_scene->GetMap()->Validate(Obj::OBJ_TYPE_ROBOT, pos.x, pos.y))
			{
				pos = m_scene->GetARandPosi();
			}
			Robot* robot = RobotManager::Instance().CreateRobotToScene(m_scene, pos, robot_param, __FUNCTION__);
			assert(NULL != robot);

			m_defender = robot;
			m_defender_ready = true;

			this->SendNoticeOnRobotCreated(robot_param.robot_name);
		}
	}

	this->SendShowUpInfo();

	if (m_attacker_ready && m_defender_ready && !this->IsBothRobot())
	{
		this->SetStatus(STATUS_BOTH_STANDBY, __FUNCTION__);
		m_next_reminder_timestamp = 0u;
		m_start_battle_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time()) + WORLD_ARENA_COUNT_DOWN_S + 2;
		m_countdown_notice_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time()) + 2;
	}
}

void SpecialLogicWorldArenaStandbyRoom::CalculateResultIfTwoRobot()
{
	if (!this->IsBothRobot()) return;

	int result = WORLD_ARENA_RESULT_DRAW;
	if (NULL == m_attacker && NULL != m_defender)
	{
		result = WORLD_ARENA_RESULT_DEFENDER_WIN;
	}
	else if (NULL == m_defender && NULL != m_attacker)
	{
		result = WORLD_ARENA_RESULT_ATTACKER_WIN;
	}
	else if (NULL != m_attacker && NULL != m_defender)
	{
		Robot* robot_attacker = static_cast<Robot*>(m_attacker);
		Robot* robot_defender = static_cast<Robot*>(m_defender);
		
		int attacker_cap = RoleBattleDataCapabilityCalculator::CalcCapability(robot_attacker->GetRoleBattleData());
		int defender_cap = RoleBattleDataCapabilityCalculator::CalcCapability(robot_defender->GetRoleBattleData());
		if (attacker_cap > defender_cap)
		{
			result = WORLD_ARENA_RESULT_ATTACKER_WIN;
		}
		else
		{
			result = WORLD_ARENA_RESULT_DEFENDER_WIN;
		}
	}

	this->SetStatus(STATUS_BATTLE_END, __FUNCTION__);
	this->OnBattleEnd();
	WorldArena::Instance().SetFightResult(m_schedule_id, result, RandomNum(2, 6));
}

void SpecialLogicWorldArenaStandbyRoom::SetStatus(int status, const char* src_func)
{
	m_status = status;

	this->LogRoomStatus(src_func);
}

void SpecialLogicWorldArenaStandbyRoom::ForceStartBattleIfNeeded()
{
	if (this->IsInFightStartStatus() || this->IsInFightFinishStatus()) return;
	if (this->IsBothRobot()) return;

	this->SetStatus(STATUS_START_BATTLE, __FUNCTION__);
	this->StartBattle(true);
}

bool SpecialLogicWorldArenaStandbyRoom::TryStartBattle()
{
	if (!this->IsInBothStandbyStatus())
	{
		return false;
	}

	if (this->IsBothRobot())
	{
		return false;
	}

	this->SetStatus(STATUS_START_BATTLE, __FUNCTION__);
	this->StartBattle(false);
	return true;
}

void SpecialLogicWorldArenaStandbyRoom::StartBattle(bool is_forced_start)
{
	if (NULL == m_attacker && NULL == m_defender)
	{
		WorldArena::Instance().SetFightResult(m_schedule_id, WORLD_ARENA_RESULT_BOTH_GIVEUP, 0, true, true);
		this->OnBattleEnd();

		gamelog::g_log_world_arena.printf(LL_INFO, "%s Both NULL, schedule_id[%lld] attacker[%d,%lld,%s] defender[%d,%lld,%s]",
			__FUNCTION__, m_schedule_id,
			this->GetAttackerUID(), this->GetAttackerUUID(), m_is_attacker_robot ? "ROBOT" : "ROLE",
			this->GetDefenderUID(), this->GetDefenderUUID(), m_is_defender_robot ? "ROBOT" : "ROLE");
	}
	else if (NULL == m_attacker)
	{
		WorldArena::Instance().SetFightResult(m_schedule_id, WORLD_ARENA_RESULT_DEFENDER_WIN, 0, true, false);
		this->OnBattleEnd();

		gamelog::g_log_world_arena.printf(LL_INFO, "%s attacker NULL, schedule_id[%lld] attacker[%d,%lld,%s] defender[%d,%lld,%s]",
			__FUNCTION__, m_schedule_id,
			this->GetAttackerUID(), this->GetAttackerUUID(), m_is_attacker_robot ? "ROBOT" : "ROLE",
			this->GetDefenderUID(), this->GetDefenderUUID(), m_is_defender_robot ? "ROBOT" : "ROLE");
	}
	else if (NULL == m_defender)
	{
		WorldArena::Instance().SetFightResult(m_schedule_id, WORLD_ARENA_RESULT_ATTACKER_WIN, 0, false, true);
		this->OnBattleEnd();

		gamelog::g_log_world_arena.printf(LL_INFO, "%s defender NULL, schedule_id[%lld] attacker[%d,%lld,%s] defender[%d,%lld,%s]",
			__FUNCTION__, m_schedule_id,
			this->GetAttackerUID(), this->GetAttackerUUID(), m_is_attacker_robot ? "ROBOT" : "ROLE",
			this->GetDefenderUID(), this->GetDefenderUUID(), m_is_defender_robot ? "ROBOT" : "ROLE");
	}
	else
	{
		BattleData_Business business;
		business.limit_max_round = LOGIC_CONFIG->GetWorldArenaConfig()->GetOtherCfg().max_limit_round;
		business.limit_max_time = 60 * 10;
		business.is_use_system_food = 1;
		
		bool attacker_failed = false;
		bool defender_failed = false;
		if (!BattleManagerLocal::GetInstance().StartSinglePVPReq2(m_attacker, m_defender, BATTLE_MODE_WORLD_ARENA, business, &attacker_failed, &defender_failed))
		{
			gamelog::g_log_world_arena.printf(LL_INFO, "%s Failed, schedule_id[%lld] attacker[%d,%lld,%s] defender[%d,%lld,%s] atk_failed[%s] def_failed[%s]",
				__FUNCTION__, m_schedule_id, 
				this->GetAttackerUID(), this->GetAttackerUUID(), m_is_attacker_robot ? "ROBOT" : "ROLE",
				this->GetDefenderUID(), this->GetDefenderUUID(), m_is_defender_robot ? "ROBOT" : "ROLE",
				attacker_failed ? "T" : "F", defender_failed ? "T" : "F");
			
			if (attacker_failed && defender_failed)
			{
				WorldArena::Instance().SetFightResult(m_schedule_id, WORLD_ARENA_RESULT_DRAW, 0);
				this->SendStartBattleFailedDrawNotice(m_attacker);
				this->SendStartBattleFailedDrawNotice(m_defender);
			}
			else if (attacker_failed)
			{
				WorldArena::Instance().SetFightResult(m_schedule_id, WORLD_ARENA_RESULT_DEFENDER_WIN, 0);
				this->SendStartBattleFailedLoseNotice(m_attacker);
				this->SendStartBattleFailedWinNotice(m_defender);
			}
			else
			{
				WorldArena::Instance().SetFightResult(m_schedule_id, WORLD_ARENA_RESULT_ATTACKER_WIN, 0);
				this->SendStartBattleFailedWinNotice(m_attacker);
				this->SendStartBattleFailedLoseNotice(m_defender);
			}
			this->OnBattleEnd();
		}
		else
		{
			gamelog::g_log_world_arena.printf(LL_INFO, "%s SUCC, schedule_id[%lld] attacker[%d,%lld,%s] defender[%d,%lld,%s]",
				__FUNCTION__, m_schedule_id,
				this->GetAttackerUID(), this->GetAttackerUUID(), m_is_attacker_robot ? "ROBOT" : "ROLE",
				this->GetDefenderUID(), this->GetDefenderUUID(), m_is_defender_robot ? "ROBOT" : "ROLE");
		}
	}
}

