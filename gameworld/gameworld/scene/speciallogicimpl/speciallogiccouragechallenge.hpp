#ifndef __SPECIAL_LOGIC_COURAGE_CHALLENGE_HPP__
#define __SPECIAL_LOGIC_COURAGE_CHALLENGE_HPP__

#include "scene/speciallogic.hpp"

class SpecialLogicCourageChallenge : public SpecialLogic
{
public:
	SpecialLogicCourageChallenge(Scene* scene);
	virtual ~SpecialLogicCourageChallenge();

	virtual void OnRoleEnterScene(Role *role);

	virtual bool CanDestroy() { return false; }
	virtual bool IsLevelOneMonsterOccur() { return false; }
	virtual bool IsDropCard() { return false; }

	virtual bool CanEnterTeam(Role * role, Team * team, bool is_from_create) { return false; }
	virtual bool CanInvitationRole(Role *role, int uid) { return false; }

private:
};

#endif