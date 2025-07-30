#include "speciallogicuta.hpp"
#include "engineadapter.h"
#include "obj/character/role.h"
#include "servercommon/internalprotocal/bigcrosshgameprotocal.h"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "servercommon/serverconfig/localconfig.hpp"
#include "internalcomm.h"
#include "servercommon/userprotocal/msguta.h"
#include "scene/scene.h"
#include "other/rolemodulemanager.hpp"
#include "other/robot/robotmanager.hpp"
#include "servercommon/robotdef.hpp"
#include "gamelog.h"

SpecialLogicUTA::SpecialLogicUTA(Scene* scene) : SpecialLogic(scene), m_schedule_id(0LL),
	m_team_unique_id(0LL), m_opponent_team_unique_id(0), m_is_main_match(false), 
	m_is_win_group(false), m_round_idx(0), m_destroy_timestamp(0u), 
	m_can_destroy(false), m_start_battle_timestamp(0u), m_is_attacker(false),
	m_has_send_vs_info(false)
{
	memset(m_opponent_team_name, 0, sizeof(m_opponent_team_name));
	memset(m_team_name, 0, sizeof(m_team_name));
	memset(m_member_uid_list, 0, sizeof(m_member_uid_list));
#ifdef UTA_BATTLE_TEST
	memset(m_opponent_uid_list, 0, sizeof(m_opponent_uid_list));
#endif

	memset(m_role_next_sync_battle_data_timestamps, 0, sizeof(m_role_next_sync_battle_data_timestamps));
}

SpecialLogicUTA::~SpecialLogicUTA()
{

}

void SpecialLogicUTA::InitBusinessParam(const UTASpecialLogicParam& param)
{
	m_schedule_id = param.schedule_id;
	for (int k = 0; k < UTA_TEAM_MEMBER_NUM; ++k)
	{
		m_member_uid_list[k] = param.member_uid_list[k];
	}
	m_team_unique_id = param.team_unique_id;
	F_STRNCPY(m_team_name, param.team_name, sizeof(GameName));
	m_opponent_team_unique_id = param.opponent_team_unique_id;
	F_STRNCPY(m_opponent_team_name, param.opponent_team_name, sizeof(GameName));
	m_is_main_match = param.is_main_match;
	m_is_win_group = param.is_win_group;
	m_round_idx = param.round_idx;
	m_start_battle_timestamp = param.start_battle_timestamp - 3; // 留3秒倒计时显示VS信息
	m_destroy_timestamp = param.destroy_timestamp;
	m_is_attacker = param.is_attacker;
	m_can_destroy = false;

#ifdef UTA_BATTLE_TEST
	for (int k = 0; k < UTA_TEAM_MEMBER_NUM; ++k)
	{
		m_opponent_uid_list[k] = param.opponent_uid_list[k];
	}
#endif

	this->RequestTeamMemberDetailInfo();
}

void SpecialLogicUTA::Update(unsigned long interval, time_t now_second)
{
	unsigned int now = static_cast<unsigned int>(now_second);

	if (now <= m_start_battle_timestamp)
	{
		this->CheckAllRoleNeedsSyncBattleData(now);
	}

	if (now >= m_start_battle_timestamp && !m_has_send_vs_info)
	{
		m_has_send_vs_info = true;
		this->SendBattleVSInfo();
	}

	if (now >= m_destroy_timestamp)
	{
		this->DelayKickOutAllRole();
	}
}

void SpecialLogicUTA::OnRoleEnterScene(Role *role)
{
	if (NULL == role) return;

	if (!this->IsTeamMember(role->GetUID()))
	{
		this->DelayKickOutRole(role);
		return;
	}

	//获取玩家战斗信息并同步到大跨服
	this->SyncRoleBattleInfoToBigCross(role);
	this->SendSceneInfo(role);

	gamelog::g_log_uta.printf(LL_INFO, "SpecialLogicUTA::OnRoleEnterScene team_name[%s] team_unique_id[%lld] role[%d,%s] schedule[%lld]",
		m_team_name, m_team_unique_id, role->GetUID(), role->GetName(), m_schedule_id);
}

void SpecialLogicUTA::OnRoleLeaveScene(Role *role, bool is_logout)
{
	if (is_logout)
	{
		// 可能是掉线，仍然要同步玩家数据
		this->CheckRoleNeedsSyncBattleData(role, true);
	}
	else
	{
		// 主动离开场景，则要让玩家不进入战斗
		unsigned int now = static_cast<unsigned int>(EngineAdapter::Instance().Time());
		if (now <= m_start_battle_timestamp)
		{
			bigchgameprotocol::HGameBigCUserActivelyLeaveUTASceneBeforeBattle msg;
			msg.plat_type = LocalConfig::Instance().GetPlatType();
			msg.hidden_server_idx = CrossConfig::Instance().GetHiddenServerIDX();
			msg.role_id = role->GetUID();
			msg.schedule_id = m_schedule_id;
			msg.team_unique_id = m_team_unique_id;
			InternalComm::Instance().SendToBigCrossServer((const char*)&msg, sizeof(msg));
		}
	}

	gamelog::g_log_uta.printf(LL_INFO, "SpecialLogicUTA::OnRoleLeaveScene team_name[%s] team_unique_id[%lld] role[%d,%s] schedule[%lld]",
		m_team_name, m_team_unique_id, role->GetUID(), role->GetName(), m_schedule_id);
}

bool SpecialLogicUTA::CanDestroy()
{
	if (m_can_destroy) return true;

	unsigned int now = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	return m_destroy_timestamp > 0u && now > m_destroy_timestamp;
}

bool SpecialLogicUTA::CanEnterTeam(Role * role, Team * team, bool is_from_create)
{
	if (NULL != role)
	{
		return this->IsTeamMember(role->GetUID());
	}

	return false;
}

bool SpecialLogicUTA::CanInvitationRole(Role *role, int uid)
{
	return this->IsTeamMember(uid);
}

bool SpecialLogicUTA::CanChangeTeamLeader(Role * role, Team * team)
{
	if (NULL != role)
	{
		return this->IsTeamMember(role->GetUID());
	}
	return false;
}

bool SpecialLogicUTA::CanTeamMemberGotoLeaderFromOtherScene(Role* member, Role* leader)
{
	if (NULL == member) return false;

	return this->IsTeamMember(member->GetUID());
}

bool SpecialLogicUTA::CanRoleStartFight(Role * role, CanRoleStartFightParam &fight_param)
{
	return false; // 由大跨服拉入大跨服的战斗，不允许玩家在原服进入任何战斗
}

void SpecialLogicUTA::ResetForNextBattle()
{
	memset(m_member_uid_list, 0, sizeof(m_member_uid_list));
	memset(m_opponent_team_name, 0, sizeof(m_opponent_team_name));
	m_schedule_id = 0LL;
	m_team_unique_id = 0LL;
	m_opponent_team_unique_id = 0LL;
	m_is_main_match = false;
	m_is_win_group = false;
	m_round_idx = 0;
	m_destroy_timestamp = 0u;
	m_can_destroy = false;
	m_start_battle_timestamp = 0u;
	m_is_attacker = false;
	m_has_send_vs_info = false;
	memset(m_ally_member_infos, 0, sizeof(m_ally_member_infos));
	memset(m_opponent_member_infos, 0, sizeof(m_opponent_member_infos));
	memset(m_role_next_sync_battle_data_timestamps, 0, sizeof(m_role_next_sync_battle_data_timestamps));
}

bool SpecialLogicUTA::IsTeamMember(int uid)
{
	for (int k = 0; k < UTA_TEAM_MEMBER_NUM; ++k)
	{
		if (uid == m_member_uid_list[k]) return true;
	}
	return false;
}

void SpecialLogicUTA::OnTeamMemberDetailResp(bigchgameprotocol::BigCHGameUTAScheduleTeamMemberDetailResp* msg)
{
	if (m_is_attacker)
	{
		for (int k = 0; k < UTA_TEAM_MEMBER_NUM; ++k)
		{
			m_ally_member_infos[k].headshot_id = msg->attackers[k].headshot_id;
			m_ally_member_infos[k].level = msg->attackers[k].level;
			m_ally_member_infos[k].profession = msg->attackers[k].profession;
			m_ally_member_infos[k].avatar_type = msg->attackers[k].avatar_type;
			F_STRNCPY(m_ally_member_infos[k].member_name, msg->attackers[k].member_name, sizeof(GameName));

			m_opponent_member_infos[k].headshot_id = msg->defenders[k].headshot_id;
			m_opponent_member_infos[k].level = msg->defenders[k].level;
			m_opponent_member_infos[k].profession = msg->defenders[k].profession;
			m_opponent_member_infos[k].avatar_type = msg->defenders[k].avatar_type;
			F_STRNCPY(m_opponent_member_infos[k].member_name, msg->defenders[k].member_name, sizeof(GameName));
		}
	}
	else
	{
		for (int k = 0; k < UTA_TEAM_MEMBER_NUM; ++k)
		{
			m_opponent_member_infos[k].headshot_id = msg->attackers[k].headshot_id;
			m_opponent_member_infos[k].level = msg->attackers[k].level;
			m_opponent_member_infos[k].profession = msg->attackers[k].profession;
			m_opponent_member_infos[k].avatar_type = msg->attackers[k].avatar_type;
			F_STRNCPY(m_opponent_member_infos[k].member_name, msg->attackers[k].member_name, sizeof(GameName));

			m_ally_member_infos[k].headshot_id = msg->defenders[k].headshot_id;
			m_ally_member_infos[k].level = msg->defenders[k].level;
			m_ally_member_infos[k].profession = msg->defenders[k].profession;
			m_ally_member_infos[k].avatar_type = msg->defenders[k].avatar_type;
			F_STRNCPY(m_ally_member_infos[k].member_name, msg->defenders[k].member_name, sizeof(GameName));
		}
	}
}

void SpecialLogicUTA::OnBattleDataChanged(Role* role)
{
	//设置一个玩家下次同步战斗数据的时间戳   以免频繁同步数据（可以在改变后5秒同步）
	unsigned int now = EngineAdapter::Instance().Time();
	if (now > m_start_battle_timestamp) return; // 超过开始战斗的时间，没有必要再同步了

	for (int i = 0; i < UTA_TEAM_MEMBER_NUM; ++i)
	{
		if (m_member_uid_list[i] == role->GetUID())
		{
			m_role_next_sync_battle_data_timestamps[i] = GetMin(now + 5, m_start_battle_timestamp);
		}
	}
}

void SpecialLogicUTA::OnBattleEndDelayKickoutAll()
{
	this->DelayKickOutAllRole(CHANGE_SCENE_REASON_INVALID, 3000);
}

void SpecialLogicUTA::RequestTeamMemberDetailInfo()
{
	bigchgameprotocol::HGameBigCUTASyncScheduleTeamMemberDetailReq req;
	req.schedule_id = m_schedule_id;
	req.plat_type = LocalConfig::Instance().GetPlatType();
	req.hidden_server_idx = CrossConfig::Instance().GetHiddenServerIDX();
	InternalComm::Instance().SendToBigCrossServer((const char*)&req, sizeof(req));
}

void SpecialLogicUTA::SendBattleVSInfo()
{
	Protocol::SCUTABattleVSInfo msg;
	msg.round_idx = m_round_idx;
	msg.is_win_group = m_is_win_group ? 1 : 0;
	msg.is_main_match = m_is_main_match ? 1 : 0;
	F_STRNCPY(msg.team_name, m_team_name, sizeof(GameName));
	F_STRNCPY(msg.opponent_team_name, m_opponent_team_name, sizeof(GameName));

	for (int k = 0; k < UTA_TEAM_MEMBER_NUM; ++k)
	{
		msg.ally_members[k].headshot_id = m_ally_member_infos[k].headshot_id;
		msg.ally_members[k].level = m_ally_member_infos[k].level;
		msg.ally_members[k].prof_base_type = GetBaseProf(m_ally_member_infos[k].profession);
		msg.ally_members[k].avatar_type = m_ally_member_infos[k].avatar_type;
		F_STRNCPY(msg.ally_members[k].member_name, m_ally_member_infos[k].member_name, sizeof(GameName));
	
		msg.opponent_members[k].headshot_id = m_opponent_member_infos[k].headshot_id;
		msg.opponent_members[k].level = m_opponent_member_infos[k].level;
		msg.opponent_members[k].prof_base_type = GetBaseProf(m_opponent_member_infos[k].profession);
		msg.opponent_members[k].avatar_type = m_opponent_member_infos[k].avatar_type;
		F_STRNCPY(msg.opponent_members[k].member_name, m_opponent_member_infos[k].member_name, sizeof(GameName));
	}

	m_scene->SendToRole((const char*)&msg, sizeof(msg));
}

void SpecialLogicUTA::SyncRoleBattleInfoToBigCross(Role* role)
{
	bigchgameprotocol::HGameBigCSyncBattleData msg;
	msg.plat_type = LocalConfig::Instance().GetPlatType();
	msg.hidden_server_idx = CrossConfig::Instance().GetHiddenServerIDX();
	msg.uid = role->GetUID();
	msg.schedule_id = m_schedule_id;
	msg.team_unique_id = m_team_unique_id;
	role->GetRoleModuleManager()->CollectRoleBattleData(&msg.rbd, false, true, true, true);

	InternalComm::Instance().SendSerializeMsgToBigCrossServer(&msg);

#ifdef UTA_BATTLE_TEST
	this->MakeRobotDataForOpponent();
#endif
}

void SpecialLogicUTA::CheckRoleNeedsSyncBattleData(Role* role, bool ignore_current_time)
{
	int index = -1;
	for (int i = 0; i < UTA_TEAM_MEMBER_NUM; ++i)
	{
		if (role->GetUID() == m_member_uid_list[i])
		{
			index = i;
			break;
		}
	}

	if (-1 == index) return;

	if (m_role_next_sync_battle_data_timestamps[index] == 0u) return;

	if (!ignore_current_time)
	{
		unsigned int now = static_cast<unsigned int>(EngineAdapter::Instance().Time());
		if (m_role_next_sync_battle_data_timestamps[index] > now) return;
	}

	this->SyncRoleBattleInfoToBigCross(role);	

	m_role_next_sync_battle_data_timestamps[index] = 0u;
}

void SpecialLogicUTA::CheckAllRoleNeedsSyncBattleData(unsigned int now)
{
	for (int i = 0; i < UTA_TEAM_MEMBER_NUM; ++i)
	{
		if (m_role_next_sync_battle_data_timestamps[i] == 0u
			|| now < m_role_next_sync_battle_data_timestamps[i])
		{
			continue;
		}

		m_role_next_sync_battle_data_timestamps[i] = 0u;
		
		Role* role = m_scene->GetRoleByUID(m_member_uid_list[i]);
		if (NULL == role)
		{
			continue;
		}

		this->SyncRoleBattleInfoToBigCross(role);
	}
}

void SpecialLogicUTA::SendSceneInfo(Role* role /* = NULL */)
{
	Protocol::SCUTASceneInfo msg;
	msg.is_main_match = m_is_main_match ? 1 : 0;
	msg.is_win_group = m_is_win_group ? 1 : 0;
	msg.round_idx = m_round_idx;
	msg.start_battle_timestamp = m_start_battle_timestamp;
	msg.opponent_team_unique_id = m_opponent_team_unique_id;
	F_STRNCPY(msg.opponent_team_name, m_opponent_team_name, sizeof(msg.opponent_team_name));

	if (NULL == role)
	{
		m_scene->SendToRole((const char*)&msg, sizeof(msg));
	}
	else
	{
		EngineAdapter::Instance().NetSend(role->GetNetId(), &msg, sizeof(msg));
	}
}

#ifdef UTA_BATTLE_TEST

void SpecialLogicUTA::MakeRobotDataForOpponent()
{
	for (int i = 0; i < UTA_TEAM_MEMBER_NUM; ++i)
	{
		UTATeamMemberDetailInfo& info = m_opponent_member_infos[i];
		
		RobotParam param;
		RobotManager::Instance().ConstructRobotParamByLevel(info.level, ROBOT_CONFIG_USE_TYPE_JJC_AND_PLATFORM, true, &param);
	
		bigchgameprotocol::HGameBigCSyncBattleData msg;
		msg.plat_type = LocalConfig::Instance().GetPlatType();
		msg.hidden_server_idx = CrossConfig::Instance().GetHiddenServerIDX();
		msg.uid = m_opponent_uid_list[i];
		msg.schedule_id = m_schedule_id;
		msg.team_unique_id = m_opponent_team_unique_id;
		msg.rbd = param.role_battle_data;
		msg.rbd.role_character_data.character_id = m_opponent_uid_list[i];
		msg.rbd.is_auto = 1;
		msg.rbd.is_robot = 1;

		InternalComm::Instance().SendSerializeMsgToBigCrossServer(&msg);
	}
}

#endif
