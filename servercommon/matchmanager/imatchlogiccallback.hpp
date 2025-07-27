#ifndef __I_MATCH_LOGIC_CALLBACK_HPP__
#define __I_MATCH_LOGIC_CALLBACK_HPP__

#include "matchdef.hpp"

class IMatchLogicCallback
{
public:
	IMatchLogicCallback() {}
	virtual ~IMatchLogicCallback() {}

	virtual void OnTeamStartMatch(MatchTeam* mt) = 0;
	virtual void OnTeamMatchTeam(MatchTeam* mtA, MatchTeam* mtB) = 0;
	virtual void OnTeamMatchEnemy(MatchTeam* mtA, MatchTeam* mtB) = 0;
	virtual void OnUserLeaveTeam(MatchTeam* mt, int uid) = 0;

	virtual bool IsTeamMatchTeam(MatchTeam* mtA, MatchTeam* mtB) = 0;
	virtual bool IsTeamMatchEnemy(MatchTeam* mtA, MatchTeam* mtB) = 0;

	virtual void RecalcMatchTeamLogic(MatchTeam* mt) = 0;
};


#endif