#include "speciallogicanimalrace.hpp"

#include "other/rolemodulemanager.hpp"
#include "servercommon/activitydef.hpp"
#include "gameworld/obj/character/role.h"
#include "global/usercache/usercache.hpp"
#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/struct/global/animalracedef.hpp"
#include "other/roleactivity/roleactivitymanager.hpp"
#include "other/roleactivity/impl/roleactivityanimalrace.hpp"

SpecialLogicAnimalRace::SpecialLogicAnimalRace(Scene * scene) : SpecialLogic(scene)
{
	//activity_scene_role_appearance.clear();
	//all_role_appearance.clear();
}

SpecialLogicAnimalRace::~SpecialLogicAnimalRace()
{
}

void SpecialLogicAnimalRace::OnRoleEnterScene(Role * role)
{
	if(role == NULL) return;

	RoleActivityAnimalRace * role_activity = static_cast<RoleActivityAnimalRace * >
		(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_ANIMAL_RACE));
	if (NULL == role_activity)
	{
		return;
	}

	role_activity->OnRAAnimalRaceEnterScene();

	/*
	UserCacheNode * user = UserCacheManager::Instance().GetUserNode(role->GetUID());
	if(user == NULL) return;

	std::map<int, Appearance>::iterator all_it = all_role_appearance.find(user->uid);
	if (all_it == all_role_appearance.end())
	{
		all_role_appearance[user->uid] = user->appearance;
	}

	std::map<int, Appearance>::iterator it = activity_scene_role_appearance.find(user->uid);
	if (it == activity_scene_role_appearance.end())
	{
		if (activity_scene_role_appearance.size() < MAX_ANIMAL_RACE_SCENE_NUM)
		{
			activity_scene_role_appearance[user->uid] = user->appearance;
		}
	}
	role->GetRoleModuleManager()->GetRoleActivity()->OnRAAnimalRaceOp();*/

}

void SpecialLogicAnimalRace::OnRoleLeaveScene(Role * user, bool is_logout)
{
	if(NULL == user) return;

	RoleActivityAnimalRace * role_activity = static_cast<RoleActivityAnimalRace *>
		(user->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_ANIMAL_RACE));
	if (NULL == role_activity)
	{
		return;
	}

	role_activity->OnRAAnimalRaceExitScene();
	/*std::map<int, Appearance>::iterator all_it = all_role_appearance.find(user->GetUID());
	if (all_it != all_role_appearance.end())
	{
		all_role_appearance.erase(all_it);
	}
	std::map<int, Appearance>::iterator it = activity_scene_role_appearance.find(user->GetUID());
	if (it != activity_scene_role_appearance.end())
	{
		activity_scene_role_appearance.erase(it);
	}
	if ((int)activity_scene_role_appearance.size() < MAX_ANIMAL_RACE_SCENE_NUM && (int)all_role_appearance.size() > (int)activity_scene_role_appearance.size())
	{
		for (all_it = all_role_appearance.begin(); all_it != all_role_appearance.end(); all_it++)
		{
			it = activity_scene_role_appearance.find(all_it->first);
			if (it != activity_scene_role_appearance.end())
			{
				activity_scene_role_appearance[it->first] = it->second;
				if ((int)activity_scene_role_appearance.size() >= MAX_ANIMAL_RACE_SCENE_NUM || (int)all_role_appearance.size() <= (int)activity_scene_role_appearance.size())
				{
					break;
				}
			}
		}
	}*/
}

void SpecialLogicAnimalRace::OnActivityClose()
{
	this->DelayKickOutAllRole();
}
