#ifndef __TEAM_MATCH_HPP__
#define __TEAM_MATCH_HPP__

#include <map>
#include "servercommon/servercommon.h"

enum MATCH_STATUS
{
	MATCH_STATUS_INVALID = 0,
	MATCH_STATUS_MATCHING,		// 正在匹配
	MATCH_STATUS_SUSPEND,		// 暂停匹配
};

static const int MATCH_INTERVAL_S = 1;

class Team;
class Role;
struct TeamTypeRobotCfg;
struct MatchTeamInfo
{
	MatchTeamInfo() : is_auto_add_robot(true), team_index(0), team(NULL), match_status(MATCH_STATUS_INVALID),
		start_match_time(0u), suspend_end_time(0u), next_add_robot_time(0u), 
		try_real_role_robot_time(0u), creator_guild_id(0){}

	bool IsSuspend();
	bool IsFullForMatch();
	MatchTeamInfo* Clone();
	void AddUserToEnteringList(const UniqueUserID& uuid);
	void RemoveUserFromEnteringList(const UniqueUserID& uuid);

	void * operator new(size_t c);
	void operator delete(void *m);

	bool is_auto_add_robot;				// 是否自动加机器人
	int team_index;						// 队伍标识
	Team* team;							// 队伍指针
	int match_status;					// 匹配状态
	unsigned int start_match_time;		// 记录开始匹配的时间
	unsigned int suspend_end_time;		// 解除暂停匹配的时间
	unsigned int next_add_robot_time;	// 下次加入机器人时间
	unsigned int try_real_role_robot_time;	// 开始尝试加真实玩家机器人时间
	int creator_guild_id;				// 队伍家族id
	std::vector<UniqueUserID> entering_list; // 记录正在进入的玩家列表
};

struct MatchUserInfo
{
	MatchUserInfo() : match_status(MATCH_STATUS_INVALID), start_match_time(0u), 
		start_enter_team_time(0u), enter_team_index(0), match_team_type(0),
		user_level(0), user_guild_id(0), is_cross_matching(false), is_auto_create(false) {}

	bool IsSuspend();

	void * operator new(size_t c);
	void operator delete(void *m);

	UniqueUserID uuid;					// 要区分好uuid来自隐藏服还是本服
	int match_status;					// 玩家匹配状态
	unsigned int start_match_time;		// 记录开始匹配的时间
	unsigned int start_enter_team_time; // 记录开始进队的时间
	int enter_team_index;				// 记录要进入
	int match_team_type;				// 要匹配的队伍目标类型
	int user_level;						// 玩家的等级
	int user_guild_id;					// 玩家家族ID
	bool is_cross_matching;				// 是否正在跨服匹配（仅原服用）
	bool is_auto_create;				// 一定时间匹配不到人是否自动创建队伍[0:否 1 : 是]
};

namespace crossgameprotocal
{
	struct HiddenGameQueryOfflineBattleDataForRobot;
}

struct RoleBattleData;
class TeamMatch
{
public:
	typedef std::map<int, MatchTeamInfo*> MatchTeamInfoMap;
	typedef std::map<UniqueUserID, MatchUserInfo*> MatchUserInfoMap;	

public:
	static TeamMatch& Instance();

	void Update(long long intervals, unsigned int now);

	void OnUserLogin(const UserID& user_id);
	void OnUserLogout(Role* role);

	bool OnTeamStartMatch(Team* team);
	bool OnTeamStopMatch(Team* team);

	bool OnUserStartMatch(const UniqueUserID& uuid, int user_level, int match_team_type, GuildID user_guild_id, bool is_auto_create = false);
	bool OnUserStartCrossMatch(const UniqueUserID& uuid, int user_level, int match_team_type, GuildID user_guild_id, bool is_auto_create = false);
	void OnUserStopMatch(const UniqueUserID& uuid);
	void OnUserStopCrossMatch(const UniqueUserID& uuid);
	void OnUserStopLocalMatch(const UniqueUserID& uuid);

	MatchTeamInfo* GetMatchTeamInfo(int team_index);
	MatchUserInfo* GetMatchUserInfo(const UniqueUserID& uuid);
	
	void OnEnterTeamSucc(const UniqueUserID& uuid, int team_index);
	void OnEnterTeamFail(const UniqueUserID& uuid, bool continue_match = true);

	void SendMatchInfo(Role* user, int is_matching);
	void GmAddUserToMatch();

	void OnOfflineBattleDataForRobotResp(crossgameprotocal::HiddenGameQueryOfflineBattleDataForRobot* msg);
private:
	void RemoveMatchTeamInfo(MatchTeamInfo** mti);
	void RemoveMatchUserInfo(MatchUserInfo** mui);
	void Release();
	void DoMatch();
	void DoAutoAddRobot();
	void DoSuspendEndRestartMatch();
	void OnMatchSucc(MatchUserInfo* mui);
	bool OnAddRobotToTeam(MatchTeamInfo* mti);
	bool OnAddRobotToUser(MatchUserInfo* mui);
	bool TryMatchTeamWithRobot(MatchUserInfo* mui);
	void CheckExpireMatchUser();
	void AddNormalRobotToTeam(int add_robot_num, Team* team, Role* leader_role, const TeamTypeRobotCfg* cfg);
	void TryAddRealRoleRobotToTeam(int add_robot_num, Team* team, Role* leader_role, const TeamTypeRobotCfg* cfg);
	bool OnAddRealRoleRobotToTeam(MatchTeamInfo* mti, int uid, const RoleBattleData& rbd);
	bool IsWaitTooLongForRealRoleRobot(const MatchTeamInfo* mti, unsigned int now) const;
	void ResetTryRealRoleRobotTime(MatchTeamInfo* mti);
	void SetTryRealRoleRobotTime(MatchTeamInfo* mti, unsigned int now);

	MatchUserInfoMap m_matching_user_map;	// 正在匹配的玩家列表
	MatchUserInfoMap m_suspend_user_map;	// 已经找到队伍，正在进队的玩家列表

	MatchTeamInfoMap m_matching_team_map;	// 正在匹配的队伍列表
	MatchTeamInfoMap m_suspend_team_map;	// 暂时已加满人的列表，在人进来之前，先等几秒才能继续去匹配



	unsigned int m_next_match_time;

	INSTANCE_FORBID_COPY_AND_EXTERIAL_CREATE(TeamMatch);
};

#endif