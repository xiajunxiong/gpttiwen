#ifndef __MATCH_ENGINE_HPP__
#define __MATCH_ENGINE_HPP__

#include "matchdef.hpp"
#include "servercommon/servercommon.h"
#include <map>
#include <vector>


class IMatchLogic;
struct MatchEngineInitParam
{
	MatchEngineInitParam() : team_member_num(1), business_type(MATCH_BUSINESS_NONE),
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

struct MatchTeam;
class MatchManager;
class MatchEngine
{
public:
	typedef std::vector<MatchTeam*> FullTeamPool;
	typedef std::vector<MatchTeam*> UnfullTeamPool;
	typedef std::map<int, long long> UserTeamHash;

public:
	MatchEngine();
	~MatchEngine();

	void Init(MatchManager* mgr, const MatchEngineInitParam& param);
	void Reset();

	void Update(unsigned long interval_ms);
	int GetMatchLogicType() const { return m_business_type; }

	bool OnUserStartMatchReq(int user_num, int logic_type, MatchUser** user_list);
	bool OnUserStopMatchReq(int user_num, int* uid_list);


	bool IsUserMatching(int uid);
	MatchTeam* CreateMatchTeam(int user_num, int logic_type, const MatchTeam::MatchUserList& mu_list);
private:
	long long GenerateTeamID();
	void RecalcMatchTeamLogic(MatchTeam* mt);
	void DoTeamMatchTeam();
	void DoClearUnfullTeam();
	void DoTeamMatchEnemy();
	void DoClearFullTeam();
	void DoTeamCombine(MatchTeam* mtA, MatchTeam* mtB);
	void DoClearTeamUserHash(MatchTeam* mt, int uid = 0);
	void DoRemoveUserOnTeam(MatchTeam* mt, int uid);
	MatchTeam* GetTeamByTeamID(long long team_id);
	
	MatchManager* m_mgr;
	bool m_has_init;
	long long m_team_id_generator;
	int m_team_member_num;
	int m_business_type;
	unsigned long m_unfull_team_match_interval_ms;
	unsigned long m_full_team_match_interval_ms;
	unsigned long m_match_unfull_pool_countdown_ms;
	unsigned long m_match_full_pool_countdown_ms;
	IMatchLogic* m_match_logic_ptr;

	//MatchQueue m_team_queue[];

	UnfullTeamPool m_unfull_team_pool;
	FullTeamPool m_full_team_pool;	// 满员的队伍等待匹配在一起加入战斗

	UserTeamHash m_user_team_hash;
};

#endif