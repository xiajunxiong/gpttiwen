#ifndef __RANDACTIVITY_JOYSEEKSFAVORS_MATCH_MANAGER_HPP__
#define __RANDACTIVITY_JOYSEEKSFAVORS_MATCH_MANAGER_HPP__

#include "other/match/normalmatchmanager.hpp"

class RandactivityJoyseeksfavors;
class RandActivityJoyseeksfavorsMatchManager : public NormalMatchMgr
{
public:
	RandActivityJoyseeksfavorsMatchManager(RandactivityJoyseeksfavors* activity);
	virtual ~RandActivityJoyseeksfavorsMatchManager();

	void Init();

	virtual void RoleStartMatchReq(Role* role);

	virtual void OnTeamStartMatch(MatchTeam* mt);
	virtual void OnTeamMatchTeam(MatchTeam* mtA, MatchTeam* mtB);
	virtual void OnTeamMatchEnemy(MatchTeam* mtA, MatchTeam* mtB);
	virtual void OnUserLeaveTeam(MatchTeam* mt, int uid);

private:
	RandactivityJoyseeksfavors* m_activity;
};

#endif