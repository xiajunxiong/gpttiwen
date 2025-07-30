#include "UTASceneManager.hpp"
#include "UTAShadow.hpp"
#include "servercommon/serverconfig/bigcrossconfig/utaconfig.hpp"
#include "world.h"
#include "scene/scenemanager.hpp"
#include "scene/scene.h"
#include "scene/speciallogicimpl/speciallogicuta.hpp"
#include "gamelog.h"

UTASceneManager::UTASceneManager(UTAShadow* uta_shadow) : m_shadow(uta_shadow)
{

}

UTASceneManager::~UTASceneManager()
{

}

Scene* UTASceneManager::CreateSceneForAttacker(const UTAScheduleShadow& schedule, unsigned int start_battle_timestamp, unsigned int scene_timeout_timestamp)
{
	Scene* scene = this->CreateScene(schedule.atk_member_uid_list[0]);
	if (NULL == scene) return NULL;

	if (scene->GetSceneType() != Scene::SCENE_TYPE_UTA)
	{
		assert(0);
		return NULL;
	}
#ifdef _DEBUG
	SpecialLogicUTA* spl = dynamic_cast<SpecialLogicUTA*>(scene->GetSpecialLogic());
	if (NULL == spl)
	{
		assert(0);
		return NULL;
	}
#else
	SpecialLogicUTA* spl = static_cast<SpecialLogicUTA*>(scene->GetSpecialLogic());
#endif

	UTASpecialLogicParam param;
	param.destroy_timestamp = scene_timeout_timestamp;
	param.start_battle_timestamp = start_battle_timestamp;
	param.is_main_match = schedule.is_main_match;
	param.round_idx = schedule.round_idx;
	param.is_win_group = schedule.is_win_group;
	param.team_unique_id = schedule.atk_team_unique_id;
	param.opponent_team_unique_id = schedule.def_team_unique_id;
	param.schedule_id = schedule.schedule_id;
	F_STRNCPY(param.team_name, schedule.atk_team_name, sizeof(GameName));
	F_STRNCPY(param.opponent_team_name, schedule.def_team_name, sizeof(GameName));
	param.is_attacker = true;
	for (int k = 0; k < UTA_TEAM_MEMBER_NUM; ++k)
	{
		param.member_uid_list[k] = schedule.atk_member_uid_list[k];
	}
#ifdef UTA_BATTLE_TEST
	for (int k = 0; k < UTA_TEAM_MEMBER_NUM; ++k)
	{
		param.opponent_uid_list[k] = schedule.def_member_uid_list[k];
	}
#endif

	spl->InitBusinessParam(param);

	gamelog::g_log_uta.printf(LL_INFO, "%s schedule_id[%lld] team_unique_id[%lld] opponent_unique_id[%lld]",
		__FUNCTION__, param.schedule_id, param.team_unique_id, param.opponent_team_unique_id);

	return scene;
}

Scene* UTASceneManager::CreateSceneForDefender(const UTAScheduleShadow& schedule, unsigned int start_battle_timestamp, unsigned int scene_timeout_timestamp)
{
	Scene* scene = this->CreateScene(schedule.def_member_uid_list[0]);
	if (NULL == scene) return NULL;

	if (scene->GetSceneType() != Scene::SCENE_TYPE_UTA)
	{
		assert(0);
		return NULL;
	}
#ifdef _DEBUG
	SpecialLogicUTA* spl = dynamic_cast<SpecialLogicUTA*>(scene->GetSpecialLogic());
	if (NULL == spl)
	{
		assert(0);
		return NULL;
	}
#else
	SpecialLogicUTA* spl = static_cast<SpecialLogicUTA*>(scene->GetSpecialLogic());
#endif

	UTASpecialLogicParam param;
	param.destroy_timestamp = scene_timeout_timestamp;
	param.start_battle_timestamp = start_battle_timestamp;
	param.is_main_match = schedule.is_main_match;
	param.round_idx = schedule.round_idx;
	param.is_win_group = schedule.is_win_group;
	param.team_unique_id = schedule.def_team_unique_id;
	param.opponent_team_unique_id = schedule.atk_team_unique_id;
	param.schedule_id = schedule.schedule_id;
	F_STRNCPY(param.team_name, schedule.def_team_name, sizeof(GameName));
	F_STRNCPY(param.opponent_team_name, schedule.atk_team_name, sizeof(GameName));
	param.is_attacker = false;
	for (int k = 0; k < UTA_TEAM_MEMBER_NUM; ++k)
	{
		param.member_uid_list[k] = schedule.def_member_uid_list[k];
	}
#ifdef UTA_BATTLE_TEST
	for (int k = 0; k < UTA_TEAM_MEMBER_NUM; ++k)
	{
		param.opponent_uid_list[k] = schedule.atk_member_uid_list[k];
	}
#endif

	spl->InitBusinessParam(param);

	gamelog::g_log_uta.printf(LL_INFO, "%s schedule_id[%lld] team_unique_id[%lld] opponent_unique_id[%lld]",
		__FUNCTION__, param.schedule_id, param.team_unique_id, param.opponent_team_unique_id);

	return scene;
}

bool UTASceneManager::HasCreateSceneForAttacker(const UTAScheduleShadow& schedule)
{
	return this->HasCreateScene(schedule.atk_member_uid_list[0]);
}

bool UTASceneManager::HasCreateSceneForDefender(const UTAScheduleShadow& schedule)
{
	return this->HasCreateScene(schedule.def_member_uid_list[0]);
}

SpecialLogicUTA* UTASceneManager::GetSpecialLogic(int scene_key)
{
	int scene_id = UTAConfig::Instance().GetOtherCfg().scene_id;
	Scene* scene = World::GetInstWorld()->GetSceneManager()->GetSceneById(scene_id, scene_key);
	if (NULL == scene)
	{
		return NULL;
	}
	
	if (scene->GetSceneType() != Scene::SCENE_TYPE_UTA)
	{
		assert(0);
		return NULL;
	}
#ifdef _DEBUG
	SpecialLogicUTA* spl = dynamic_cast<SpecialLogicUTA*>(scene->GetSpecialLogic());
	if (NULL == spl)
	{
		assert(0);
		return NULL;
	}
#else
	SpecialLogicUTA* spl = static_cast<SpecialLogicUTA*>(scene->GetSpecialLogic());
#endif

	return spl;
}

SpecialLogicUTA* UTASceneManager::GetSpecialLogic(int plat_type, int uid, const UTAScheduleShadow& schedule)
{
	if (plat_type == schedule.atk_plat_type)
	{
		for (int i = 0; i < ARRAY_ITEM_COUNT(schedule.atk_member_uid_list); ++i)
		{
			if (uid == schedule.atk_member_uid_list[i])
			{
				return this->GetSpecialLogic(schedule.atk_member_uid_list[0]);
			}
		}
	}
	
	if (plat_type == schedule.def_plat_type)
	{
		for (int i = 0; i < ARRAY_ITEM_COUNT(schedule.def_member_uid_list); ++i)
		{
			if (uid == schedule.def_member_uid_list[i])
			{
				return this->GetSpecialLogic(schedule.def_member_uid_list[0]);
			}
		}
	}

	return NULL;
}

SpecialLogicUTA* UTASceneManager::GetSpecialLogic(long long schedule_id, long long unique_team_id)
{
	const UTAScheduleShadow* schedule = m_shadow->GetScheduleShadow(schedule_id);
	if (NULL == schedule) return NULL;

	if (unique_team_id == schedule->atk_team_unique_id)
	{
		return this->GetSpecialLogic(schedule->atk_member_uid_list[0]);
	}
	
	if (unique_team_id == schedule->def_team_unique_id)
	{
		return this->GetSpecialLogic(schedule->def_member_uid_list[0]);
	}

	return NULL;
}

bool UTASceneManager::HasCreateScene(int scene_key)
{
	int scene_id = UTAConfig::Instance().GetOtherCfg().scene_id;
	Scene* scene = World::GetInstWorld()->GetSceneManager()->GetSceneById(scene_id, scene_key);
	if (NULL == scene) return false;

	return true;
}

Scene* UTASceneManager::CreateScene(int scene_key)
{
	int scene_id = UTAConfig::Instance().GetOtherCfg().scene_id;
	Scene* scene = World::GetInstWorld()->GetSceneManager()->GetSceneById(scene_id, scene_key);
	if (NULL != scene)
	{
		//scene->GetSpecialLogic()->DelayKickOutAllRole();
		if (scene->GetSceneType() != Scene::SCENE_TYPE_UTA)
		{
			assert(0);
			return NULL;
		}
#ifdef _DEBUG
		SpecialLogicUTA* spl = dynamic_cast<SpecialLogicUTA*>(scene->GetSpecialLogic());
		if (NULL == spl)
		{
			assert(0);
			return NULL;
		}
#else
		SpecialLogicUTA* spl = static_cast<SpecialLogicUTA*>(scene->GetSpecialLogic());
#endif

		spl->ResetForNextBattle();
	}
	else
	{
		bool ret = World::GetInstWorld()->GetSceneManager()->CreateFB(scene_id, scene_key, &scene);
		assert(ret);
	}	

	return scene;
}

