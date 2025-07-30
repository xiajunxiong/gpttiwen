#ifndef __NORMAL_MATCH_MANAGER_HPP__
#define __NORMAL_MATCH_MANAGER_HPP__

#include "servercommon/matchmanager/matchmanager.hpp"

//#define TEST_MODE



class IMatchLogic;
class Role;
class NormalMatchMgr : public MatchManager
{
public:
	NormalMatchMgr();
	virtual ~NormalMatchMgr();

	void Init(int team_member_num, IMatchLogic* logic, int business_type);

	virtual void RoleStartMatchReq(Role* role);
	virtual void RoleStopMatchReq(Role* role);

	// 以下是回调
	virtual void OnTeamStartMatch(MatchTeam* mt);
	virtual void OnTeamMatchTeam(MatchTeam* mtA, MatchTeam* mtB);
	virtual void OnTeamMatchEnemy(MatchTeam* mtA, MatchTeam* mtB);
	virtual void OnUserLeaveTeam(MatchTeam* mt, int uid);

protected:
	void SendTeamStartMatchNotice(MatchTeam* mt);
	void SendTeamMatchTeamNotice(MatchTeam* mtA, MatchTeam* mtB);
	void SendTeamMatchEnemyNotice(MatchTeam* mtA, MatchTeam* mtB);
	void SendUserLeaveTeamNotice(MatchTeam* mt, int uid);
};

#endif