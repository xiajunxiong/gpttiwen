#include "config/logicconfigmanager.hpp"
#include "scene/scene.h"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "engineadapter.h"
#include "global/activity/activitymanager.hpp"
#include "config/logicconfigmanager.hpp"
#include "gamelog.h"

#include "config/activityconfig/chiefpeakbattle/activitychiefpeakbattlenconfig.hpp"
#include "battle/battle_manager_local.hpp"
#include "global/activity/activityimpl/activitychiefpeakbattle.hpp"
#include "speciallogicchiefpeakbattle.hpp"

SpecialLogicChiefPeakBattle::SpecialLogicChiefPeakBattle(Scene * scene) :SpecialLogic(scene)
{
	m_start_battle = false;
	m_is_battle_finish = false;
}

SpecialLogicChiefPeakBattle::~SpecialLogicChiefPeakBattle()
{
}

void SpecialLogicChiefPeakBattle::OnRoleEnterScene(Role * role)
{
	gamelog::g_Log_chief_peak.printf(LL_INFO, "%s line:%d | is_start:%d is_finish:%d role_id:%d name:%s scene_id:%d scene_key:%d", __FUNCTION__, __LINE__, (int)m_start_battle, (int)m_is_battle_finish, role->GetUID(), role->GetName(), m_scene->GetSceneID(), m_scene->GetSceneKey());
}

void SpecialLogicChiefPeakBattle::OnRoleLeaveScene(Role * role, bool is_logout)
{
	std::string bt_string = PrintStackBackTrace(false);

	gamelog::g_Log_chief_peak.printf(LL_INFO, "%s line:%d | is_start:%d is_finish:%d role_id:%d name:%s scene_id:%d scene_key:%d trace:%s", __FUNCTION__, __LINE__, (int)m_start_battle, (int)m_is_battle_finish, role->GetUID(), role->GetName(), m_scene->GetSceneID(), m_scene->GetSceneKey(), bt_string.c_str());
}

bool SpecialLogicChiefPeakBattle::CanDestroy()
{
	return true;
}

void SpecialLogicChiefPeakBattle::Update(unsigned long interval, time_t now_second)
{
	if (m_is_battle_finish)
	{
		this->DelayKickOutAllRole();
	}
}

void SpecialLogicChiefPeakBattle::StartRoundFight(int role_id1, int role_id2)
{
	if (m_start_battle)
	{
		return;
	}
	m_start_battle = true;

	ActivityChiefPeakBattle * activity = (ActivityChiefPeakBattle *)ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_CHIEF_PEAK_BATTLE);
	if (activity == NULL)
	{
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

	int result = BattleManagerLocal::GetInstance().StartChiefPeakBattlePVPReq(role1, role2, LOGIC_CONFIG->GetActivityChiefPeakBattleConfig()->GetOhterCfg().max_round, ACTIVITY_CHIEF_PEAK_BATTLE_BATTLE_TIME - 2);
	gamelog::g_Log_chief_peak.printf(LL_INFO, "%s  line:%d | role_id1:%d role_id2:%d result:%d", __FUNCTION__, __LINE__, role_id1, role_id2, result);
	switch (result)
	{
		case RESULT_ATTACKER_FAIL:
			{
				activity->SetFightResult(role_id1, false);
				activity->SetFightResult(role_id2, true);
			}
			break;
		case RESULT_DEFENDER_FAIL:
			{
				activity->SetFightResult(role_id1, true);
				activity->SetFightResult(role_id2, false);
			}
			break;
		case RESULT_BOTH_FAIL:
			{
				activity->SetFightResult(role_id1, false);
				activity->SetFightResult(role_id2, false);
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

void SpecialLogicChiefPeakBattle::OnFinish(int role_id, bool is_finish)
{
	m_battle_role_id.erase(role_id);
	if (m_battle_role_id.size() == 0 || is_finish)
	{
		m_is_battle_finish = true;
	}
}

void SpecialLogicChiefPeakBattle::OnActivityClose()
{
	m_is_battle_finish = true;
}