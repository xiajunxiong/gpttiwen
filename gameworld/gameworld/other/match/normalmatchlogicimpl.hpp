#ifndef __NORMAL_MATCH_LOGIC_IMPL_HPP__
#define __NORMAL_MATCH_LOGIC_IMPL_HPP__

#include "servercommon/matchmanager/imatchlogic.hpp"

class NormalMatchLogicImpl : public IMatchLogic
{
public:
	NormalMatchLogicImpl();
	virtual ~NormalMatchLogicImpl() {}

	virtual bool IsTeamMatchTeam(MatchTeam* mtA, MatchTeam* mtB);
	virtual bool IsTeamMatchEnemy(MatchTeam* mtA, MatchTeam* mtB);

	virtual void RecalcMatchTeamLogic(MatchTeam* mt);

};

#endif