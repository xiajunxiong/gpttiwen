#ifndef __ACTIVITY_TEAM_FIGHT_MATCH_MANAGER_HPP__
#define __ACTIVITY_TEAM_FIGHT_MATCH_MANAGER_HPP__

#include "other/match/normalmatchmanager.hpp"

class ActivityTeamFight;
class ActivityTeamFightMatchManager : public NormalMatchMgr
{
public:
	ActivityTeamFightMatchManager(ActivityTeamFight* activity);
	virtual ~ActivityTeamFightMatchManager();

	void Init();

	virtual void OnTeamStartMatch(MatchTeam* mt);
	virtual void OnTeamMatchTeam(MatchTeam* mtA, MatchTeam* mtB);
	virtual void OnTeamMatchEnemy(MatchTeam* mtA, MatchTeam* mtB);
	virtual void OnUserLeaveTeam(MatchTeam* mt, int uid);

private:
	ActivityTeamFight* m_activity;
};

#endif