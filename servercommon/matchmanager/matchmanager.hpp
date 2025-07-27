#ifndef __MATCH_MANAGER_HPP__
#define __MATCH_MANAGER_HPP__

#include "matchdef.hpp"
#include <map>
#include <vector>

class IMatchLogic;
class MatchEngine;

struct MatchManagerInitParam
{
	MatchManagerInitParam() : team_member_num(1), business_type(MATCH_BUSINESS_NONE),
		unfull_team_match_interval_ms(3000), full_team_match_interval_ms(3000), 
		logic_ptr(NULL) 
	{
	
	}

	int team_member_num;
	int business_type;
	unsigned long unfull_team_match_interval_ms;
	unsigned long full_team_match_interval_ms;
	IMatchLogic* logic_ptr;
};

class MatchEngine;
class MatchManager
{
public:
	typedef std::vector<MatchTeam*> FullTeamPool;
	typedef std::vector<MatchTeam*> UnfullTeamPool;
	typedef std::map<int, long long> UserTeamHash;

public:
	MatchManager();
	virtual ~MatchManager();

	void Init(MatchEngine* match_engine, MatchManagerInitParam& param);
	void Reset();

	virtual bool OnUserStartMatchReq(int user_num, int logic_type, MatchUser** user_list); 
	virtual bool OnUserStopMatchReq(int user_num, int* uid_list);

	void Update(unsigned long interval_ms); // Ö´ÐÐÆ¥ÅäÂß¼­
	
	virtual void OnTeamStartMatch(MatchTeam* mt);
	virtual void OnTeamMatchTeam(MatchTeam* mtA, MatchTeam* mtB);
	virtual void OnTeamMatchEnemy(MatchTeam* mtA, MatchTeam* mtB);
	virtual void OnUserLeaveTeam(MatchTeam* mt, int uid);

	inline MatchEngine* GetMatchEngine() const { return m_engine; }

protected:
	MatchEngine* m_engine;
	bool m_has_init;
};

#endif