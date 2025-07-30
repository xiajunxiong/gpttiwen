#include "speciallogicteamfight.hpp"
#include "global/activity/activitymanager.hpp"
#include "global/activity/activityimpl/activityteamfight.hpp"
#include "global/team/teammanager.hpp"
#include "obj/character/role.h"
#include "gamelog.h"
#include "servercommon/errornum.h"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "global/team/team.hpp"
#include "other/rolemodulemanager.hpp"
#include "other/event/eventhandler.hpp"
#include "global/worldstatus/worldstatus.hpp"

SpecialLogicTeamFight::SpecialLogicTeamFight(Scene * scene) : SpecialLogic(scene)
{
}

SpecialLogicTeamFight::~SpecialLogicTeamFight()
{
}

void SpecialLogicTeamFight::OnRoleEnterScene(Role * role)
{
	if(NULL == role) return;
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		this->DelayKickOutRole(role);
	}

	ActivityTeamFight* activity = (ActivityTeamFight*)ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_TEAM_FIGHT);
	if (NULL != activity)
	{
		if (!activity->IsActivityOpen())
		{
			this->DelayKickOutRole(role);
			return;
		}

		activity->SyncRoleScoreToActivityData(role);
		activity->SendAdvanceFetchInfo(role);
		EventHandler::Instance().OnParticipateLimitActivity(role, ACTIVITY_TYPE_TEAM_FIGHT, __FUNCTION__);
		activity->SendAdvanceFetchInfo(role);
	}
}

void SpecialLogicTeamFight::OnRoleLeaveScene(Role * role, bool is_logout)
{
	ActivityTeamFight* activity = (ActivityTeamFight*)ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_TEAM_FIGHT);
	if (NULL != activity)
	{
		activity->SyncActivityDataToRoleScore(role);

		Team* team = role->GetMyTeam();
		if (NULL == team)
		{
			activity->OnCancelReq(role);
		}
		else
		{
			activity->OnCancelReq(role);
		}
	}
}

void SpecialLogicTeamFight::OnRoleLeaveTeam(Role * role)
{
	ActivityTeamFight* activity = (ActivityTeamFight*)ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_TEAM_FIGHT);
	if (NULL != activity)
	{
		activity->OnCancelReq(role);
	}
}

bool SpecialLogicTeamFight::CanEnterTeam(Role * role, Team * team, bool is_from_create)
{
	if (team != NULL)
	{
		return team->GetMemberCount() < TUAN_DUI_JING_JI_MAX_MEMBER_NUM;
	}
	else
	{
		return is_from_create;
	}

	return true;
}

void SpecialLogicTeamFight::OnFightFinish(battlegameprotocol::BattleGameFightResultAck * ack)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	if (ack->battle_mode != BATTLE_MODE_SQUARD_FIGHT) return;
	if (ack->special_business_data.data_type != BATTLE_BUSINESS_DATA_TYPE_SQUAD_FIGHT)
	{
		assert(0);
		return;
	}

	if (RESULT_TYPE_ATTACKER_WIN != ack->battle_result &&
		RESULT_TYPE_DEFENDER_WIN != ack->battle_result &&
		RESULT_TYPE_DRAW != ack->battle_result &&
		RESULT_TYPE_EXCEED_MAX_ROUND != ack->battle_result)
	{
		assert(0);
		return;
	}

	ActivityTeamFight* activity = (ActivityTeamFight*)ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_TEAM_FIGHT);
	if (NULL != activity && activity->IsActivityOpen())
	{
		int attack_list[SIDE_MAX_ROLE_NUM] = { 0 };
		memset(attack_list, 0, sizeof(attack_list));
		int defend_list[SIDE_MAX_ROLE_NUM] = { 0 };
		memset(defend_list, 0, sizeof(defend_list));

		BattleSquardFightData* fight_data = (BattleSquardFightData*)ack->special_business_data.data;
		for (int i = 0; i < ack->attacker_list.role_count && i < ARRAY_ITEM_COUNT(attack_list); ++i)
		{
			attack_list[i] = ack->attacker_list.role_list[i].uid;
			activity->OnRoleFightRet(attack_list[i], true, ack->battle_result, fight_data->attacker_alive_num, fight_data->defender_alive_num);
		}

		for (int i = 0; i < ack->defender_list.role_count && i < ARRAY_ITEM_COUNT(defend_list); ++i)
		{
			defend_list[i] = ack->defender_list.role_list[i].uid;
			activity->OnRoleFightRet(defend_list[i], false, ack->battle_result, fight_data->attacker_alive_num, fight_data->defender_alive_num);
		}

		if (RESULT_TYPE_ATTACKER_WIN == ack->battle_result)
		{
			activity->FightRecord(attack_list, defend_list);
		}
		else if (RESULT_TYPE_DEFENDER_WIN == ack->battle_result)
		{
			activity->FightRecord(defend_list, attack_list);
		}

		UNSTD_STATIC_CHECK(ARRAY_ITEM_COUNT(attack_list) == 5);
		UNSTD_STATIC_CHECK(ARRAY_ITEM_COUNT(defend_list) == 5);
		gamelog::g_log_team_fight.printf(LL_INFO, "OnFightFinish win[%d] attack[%d %d %d %d %d] defend[%d %d %d %d %d]", ack->battle_result,
										 attack_list[0], attack_list[1], attack_list[2], attack_list[3], attack_list[4],
										 defend_list[0], defend_list[1], defend_list[2], defend_list[3], defend_list[4]);
	}
}

void SpecialLogicTeamFight::OnRoleRunAway(battlegameprotocol::BattleGameRoleRunAway * ack)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}
}
