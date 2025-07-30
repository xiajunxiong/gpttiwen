#ifndef __RELIC_LOOTING_MATCH_MANAGER_HPP__
#define __RELIC_LOOTING_MATCH_MANAGER_HPP__

#include "other/match/normalmatchmanager.hpp"

class ActivityRelicLooting;
class ActivityRelicLootingMatchManager : public NormalMatchMgr
{
public:
	ActivityRelicLootingMatchManager(ActivityRelicLooting* activity);
	virtual ~ActivityRelicLootingMatchManager();

	void Init();

	virtual void RoleStartMatchReq(Role* role);

	virtual void OnTeamStartMatch(MatchTeam* mt);
	virtual void OnTeamMatchTeam(MatchTeam* mtA, MatchTeam* mtB);
	virtual void OnTeamMatchEnemy(MatchTeam* mtA, MatchTeam* mtB);
	virtual void OnUserLeaveTeam(MatchTeam* mt, int uid);

private:
	ActivityRelicLooting* m_activity;
};

#endif