#ifndef __ACTIVITY_TEAM_FIGHT_MATCH_LOGIC_IMPL_HPP__
#define __ACTIVITY_TEAM_FIGHT_MATCH_LOGIC_IMPL_HPP__

#include "servercommon/matchmanager/imatchlogic.hpp"

class ActivityTeamFightMatchLogicImpl : public IMatchLogic
{
public:
	ActivityTeamFightMatchLogicImpl();
	virtual ~ActivityTeamFightMatchLogicImpl() {}

	virtual bool IsTeamMatchTeam(MatchTeam* mtA, MatchTeam* mtB);
	virtual bool IsTeamMatchEnemy(MatchTeam* mtA, MatchTeam* mtB);

	virtual void RecalcMatchTeamLogic(MatchTeam* mt);

};

#endif