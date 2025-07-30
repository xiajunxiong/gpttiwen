#include "speciallogiccouragechallenge.hpp"
#include "obj/character/role.h"
#include "other/funopen/funopen.hpp"
#include "servercommon/errornum.h"

SpecialLogicCourageChallenge::SpecialLogicCourageChallenge(Scene* scene) : SpecialLogic(scene)
{

}

SpecialLogicCourageChallenge::~SpecialLogicCourageChallenge()
{

}

void SpecialLogicCourageChallenge::OnRoleEnterScene(Role * role)
{
	if (NULL == role) return;

	if (!FunOpen::IsFunOpen(role, FUN_TYPE_COURAGE_CHALLENGE, false))
	{
		role->NoticeNum(errornum::EN_CURR_LEVEL_NOT_ENTER_SCENE);
		this->DelayKickOutRole(role);
		return;
	}

	if (NULL != role->GetMyTeam())
	{
		role->NoticeNum(errornum::EN_ENTER_SCENE_NOT_TEAM);
		this->DelayKickOutRole(role);
		return;
	}

}

