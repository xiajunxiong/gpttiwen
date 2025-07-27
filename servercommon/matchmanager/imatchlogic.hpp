#ifndef __I_MATCH_LOGIC_HPP__
#define __I_MATCH_LOGIC_HPP__

#include "matchdef.hpp"

class IMatchLogic
{
public:
	IMatchLogic() {}
	virtual ~IMatchLogic() {}

	virtual bool IsTeamMatchTeam(MatchTeam* mtA, MatchTeam* mtB) = 0;
	virtual bool IsTeamMatchEnemy(MatchTeam* mtA, MatchTeam* mtB) = 0;

	virtual void RecalcMatchTeamLogic(MatchTeam* mt) = 0;
};


#endif