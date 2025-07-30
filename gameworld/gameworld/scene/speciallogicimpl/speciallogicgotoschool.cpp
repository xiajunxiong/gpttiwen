#include "engineadapter.h"
#include "obj/character/role.h"
#include "scene/scene.h"
#include "config/logicconfigmanager.hpp"
#include "other/rolemodulemanager.hpp"
#include "item/knapsack.h"
#include "gamecommon.h"
#include "gamelog.h"
#include "battle/battle_manager_local.hpp"

#include "other/cycleactivity/tuzikuaipao/tuzikuaipaoconfig.hpp"
#include "protocol/cycleactivity/msggotoschool.hpp"
#include "speciallogicgotoschool.hpp"

SpecialLogicGotoSchool::SpecialLogicGotoSchool(Scene * scene) : SpecialLogic(scene)
{
}

SpecialLogicGotoSchool::~SpecialLogicGotoSchool()
{
}

void SpecialLogicGotoSchool::Update(unsigned long interval, time_t now_second)
{
}

void SpecialLogicGotoSchool::OnRoleEnterScene(Role * role)
{
}

void SpecialLogicGotoSchool::OnRoleLeaveScene(Role * role, bool is_logout)
{
}


bool SpecialLogicGotoSchool::CanDestroy()
{
	return true;
}

void SpecialLogicGotoSchool::Fight(Role * role, int monster_group_id)
{
	BattleManagerLocal::GetInstance().StartBattleModeFightReq(role, BATTLE_MODE_GOTO_SCHOOL, monster_group_id, 0, 0, true);
}
