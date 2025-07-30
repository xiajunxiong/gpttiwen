#include "config/logicconfigmanager.hpp"
#include "scene/scene.h"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "engineadapter.h"
#include "global/activity/activitymanager.hpp"
#include "config/logicconfigmanager.hpp"
#include "gamelog.h"
#include "servercommon/string/gameworldstr.h"

#include "config/activityconfig/chiefelection/activitychiefelectionconfig.hpp"
#include "battle/battle_manager_local.hpp"
#include "global/activity/activityimpl/activitychiefelection.hpp"
#include "speciallogicchiefelection.hpp"

SpecialLogicChiefElection::SpecialLogicChiefElection(Scene * scene) :SpecialLogic(scene)
{
	m_start_battle = false;
	m_is_battle_finish = false;
	m_which_prof_match = 0;
}

SpecialLogicChiefElection::~SpecialLogicChiefElection()
{
}

void SpecialLogicChiefElection::OnRoleEnterScene(Role * role)
{
	gamelog::g_Log_chief_election.printf(LL_INFO, "%s line:%d | prof:%d is_start:%d is_finish:%d role_id:%d name:%s scene_id:%d scene_key:%d", __FUNCTION__, __LINE__, role->GetBaseProfession(), (int)m_start_battle, (int)m_is_battle_finish, role->GetUID(), role->GetName(), m_scene->GetSceneID(), m_scene->GetSceneKey());

	ActivityChiefElection * activity = (ActivityChiefElection *)ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_CHIEF_ELECTION);
	if (activity == NULL)
	{
		gamelog::g_Log_chief_election.printf(LL_INFO, "%s line:%d activity is null", __FUNCTION__, __LINE__);
		this->DelayKickOutRole(role);
		return;
	}

	if (!activity->CanEnter(role->GetUID(),role->GetBaseProfession()))
	{
		this->DelayKickOutRole(role);
		return;
	}
}

void SpecialLogicChiefElection::OnRoleLeaveScene(Role * role, bool is_logout)
{
	std::string bt_string = PrintStackBackTrace(false);

	gamelog::g_Log_chief_election.printf(LL_INFO, "%s line:%d | prof:%d is_start:%d is_finish:%d role_id:%d name:%s scene_id:%d scene_key:%d trace:%s", __FUNCTION__, __LINE__, role->GetBaseProfession(), (int)m_start_battle, (int)m_is_battle_finish, role->GetUID(), role->GetName(), m_scene->GetSceneID(), m_scene->GetSceneKey(), bt_string.c_str());
}

bool SpecialLogicChiefElection::CanDestroy()
{
	return true;
}

void SpecialLogicChiefElection::Update(unsigned long interval, time_t now_second)
{
	if (m_is_battle_finish)
	{
		this->DelayKickOutAllRole();
	}
}

bool SpecialLogicChiefElection::CanChallengeOthers(Role * role, Role * target)
{
	return false;
}

void SpecialLogicChiefElection::StartRoundFight(int role_id1, int role_id2, int which_prof_match)
{
	if (m_start_battle)
	{
		return;
	}
	m_start_battle = true;

	m_which_prof_match = which_prof_match;
	ActivityChiefElection * activity = (ActivityChiefElection *)ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_CHIEF_ELECTION);
	if (activity == NULL)
	{
		gamelog::g_Log_chief_election.printf(LL_INFO, "%s line:%d activity is null", __FUNCTION__, __LINE__);
		return;
	}

	if (role_id1 != 0)
	{
		m_battle_role_id.insert(role_id1);
	}

	if (role_id2 != 0)
	{
		m_battle_role_id.insert(role_id2);
	}

	Role * role1 = m_scene->GetRoleByUID(role_id1);
	Role * role2 = m_scene->GetRoleByUID(role_id2);

	const ChiefElectionOtherCfg& other_cfg = LOGIC_CONFIG->GetActivityChiefElectionConfig()->GetOhterCfg();

	int result = BattleManagerLocal::GetInstance().StartChiefElectionPVPReq(role1, role2, which_prof_match, other_cfg.max_round, ACTIVITY_CHIEF_ELECTION_BATTLE_TIME - 10, 0);

	gamelog::g_Log_chief_election.printf(LL_INFO, "%s line:%d | prof:%d role_id1:%d role_id2:%d result:%d", __FUNCTION__, __LINE__, which_prof_match, role_id1, role_id2, result);

	switch (result)
	{
		case RESULT_ATTACKER_FAIL:
			{
				activity->SetFightResult(role_id1, false, m_which_prof_match);
				activity->SetFightResult(role_id2, true, m_which_prof_match);

				int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_chief_election_sea_tip2);
				if (length > 0)
				{
					World::GetInstWorld()->GetSceneManager()->SystemMsgPerson2(role1, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_SYSTEM_CHAT));
				}

				length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_chief_election_sea_tip1);
				if (length > 0)
				{
					World::GetInstWorld()->GetSceneManager()->SystemMsgPerson2(role2, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_SYSTEM_CHAT));
				}
			}
			break;
		case RESULT_DEFENDER_FAIL:
			{
				activity->SetFightResult(role_id1, true, m_which_prof_match);
				activity->SetFightResult(role_id2, false, m_which_prof_match);

				int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_chief_election_sea_tip1);
				if (length > 0)
				{
					World::GetInstWorld()->GetSceneManager()->SystemMsgPerson2(role1, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_SYSTEM_CHAT));
				}

				length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_chief_election_sea_tip2);
				if (length > 0)
				{
					World::GetInstWorld()->GetSceneManager()->SystemMsgPerson2(role2, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_SYSTEM_CHAT));
				}
			}
			break;
		case RESULT_BOTH_FAIL:
			{
				activity->SetFightResult(role_id1, false,m_which_prof_match);
				activity->SetFightResult(role_id2, false,m_which_prof_match);

				int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_chief_election_sea_tip2);
				if (length > 0)
				{
					World::GetInstWorld()->GetSceneManager()->SystemMsgPerson2(role1, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_SYSTEM_CHAT));
					World::GetInstWorld()->GetSceneManager()->SystemMsgPerson2(role2, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_SYSTEM_CHAT));
				}
			}
			break;
		case START_BATTLE_RESULT_SUCC:
			{
			}
			break;
		default:
			break;
	}


}

void SpecialLogicChiefElection::OnFinish(int role_id, bool is_finish)
{
	m_battle_role_id.erase(role_id);
	if (m_battle_role_id.size() == 0 || is_finish)
	{
		m_is_battle_finish = true;
	}
}

void SpecialLogicChiefElection::OnActivityClose()
{
	m_is_battle_finish = true;
}

