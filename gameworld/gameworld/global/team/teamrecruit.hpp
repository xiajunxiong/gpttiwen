#ifndef __TEAM_RECRUIT_HPP__
#define __TEAM_RECRUIT_HPP__

#include "servercommon/servercommon.h"

struct RoleRecruitInfo
{
	RoleRecruitInfo() : role_id(0), team_id(0), team_type(0), 
		level_limit_low(0), level_limit_high(0), cur_member_num(0),
		max_member_num(0), is_cross_team(false), auto_reset_timestamp(0u)
	{
		memset(leader_name, 0, sizeof(leader_name));
	}

	int role_id;
	int team_id;
	int team_type;
	GameName leader_name;
	int level_limit_low;
	int level_limit_high;
	int cur_member_num;
	int max_member_num;
	bool is_cross_team;
	unsigned int auto_reset_timestamp;
	std::map<int, unsigned int> refuse_map;
};

namespace Protocol
{
	class CSTeamRoleRecruitAnswer;
}

class Team;
class Role;
class TeamRecruit
{
public:
	typedef std::map<int, RoleRecruitInfo> RoleRecruitMap;

public:
	static TeamRecruit& Instance();

	void Update(unsigned int now);
	void OnRoleLogout(int role_id);

	void OnTeamRecruit(int team_id, int team_type,
		const char* leader_name, int limit_lv_low, int limit_lv_high,
		int cur_member_num, int max_member_num, bool is_from_cross = false);
	void OnRoleAnswerRecruit(Role* role, Protocol::CSTeamRoleRecruitAnswer* req);

	void SyncTeamRoleRecruitToOrigin(int team_id, int team_type,
		const char* leader_name, int limit_lv_low, int limit_lv_high,
		int cur_member_num, int max_member_num);
private:
	TeamRecruit();
	~TeamRecruit();
	TeamRecruit(const TeamRecruit&);
	TeamRecruit& operator= (const TeamRecruit&);

	void AddRoleRecruit(Role* role, int team_id, int team_type, 
		const char* leader_name, int limit_lv_low, int limit_lv_high,
		int cur_member_num, int max_member_num, bool is_from_cross);
	bool CheckTeamTypeRefuse(RoleRecruitInfo* rri, int team_type);
	void CheckAutoRefuse(unsigned int now);
	void CheckErase();

	RoleRecruitInfo* MutableRoleRecruitInfo(int role_id);
	void SendRoleRecruitInfo(Role* role, const RoleRecruitInfo* rri);
	void ResetCurRecruitInfo(RoleRecruitInfo* rri);
	void RemoveRoleRecruit(int role_id);
	bool CheckRoleRemainTimesEnough(Role* role, int team_type, int team_type_target_type);

	RoleRecruitMap m_role_recruit_map;

	unsigned int m_next_check_timestamp;
};

#endif