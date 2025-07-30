#ifndef __SPECIAL_LOGIC_TEAM_FIGHT_HPP__
#define __SPECIAL_LOGIC_TEAM_FIGHT_HPP__
#include "scene/speciallogic.hpp"

class SpecialLogicTeamFight: public SpecialLogic
{
public:
	SpecialLogicTeamFight(Scene* scene);
	virtual ~SpecialLogicTeamFight();

	virtual bool CanDestroy() { return false; }
	virtual void OnRoleEnterScene(Role *role);
	virtual void OnRoleLeaveScene(Role *role, bool is_logout);
	virtual void OnRoleLeaveTeam(Role * role);
	virtual bool CanEnterTeam(Role * role, Team * team, bool is_from_create);
	virtual void OnFightFinish(battlegameprotocol::BattleGameFightResultAck* ack);
	virtual void OnRoleRunAway(battlegameprotocol::BattleGameRoleRunAway* ack);
	virtual bool CanTeamMemberGotoLeaderFromOtherScene(Role* member, Role* leader) { return true; }

private:


};


#endif