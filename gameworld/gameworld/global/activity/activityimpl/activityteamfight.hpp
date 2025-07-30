#ifndef __ACTIVITY_TEAM_FIGHT_HPP__
#define __ACTIVITY_TEAM_FIGHT_HPP__
#include <queue>
#include <vector>
#include "global/activity/activity.hpp"
#include "other/match/activityteamfightmatchmanager.hpp"
struct RoleInfo
{
	RoleInfo() :_uid(0), _score(0), _victory_count(0), _defeat_count(0), _continue_victory_count(0), 
		_box_fetch_flag(0), _level(0), _join_award_flag(0), _battle_count(0){}

	bool operator == (const int& uid) const
	{
		return _uid == uid;
	}

	bool operator == (const RoleInfo& info) const
	{
		return _uid == info._uid;
	}

	int _uid;
	int _score;
	int _victory_count;
	int _defeat_count;
	int _continue_victory_count;
	int _box_fetch_flag;
	int _level;
	char _join_award_flag;	// 参与奖励识标 0:未领取 1:已领取 
	int _battle_count;
};

typedef std::vector<RoleInfo> RoleList;

struct FightRecordData
{
	int user_list1[MAX_TEAM_MEMBER_NUM];
	int user_list2[MAX_TEAM_MEMBER_NUM];
};

typedef std::vector<FightRecordData> FightRecordList;

struct ActTeamFightMatchGroup
{
	ActTeamFightMatchGroup() : fight_start_time(0) {}

	unsigned int fight_start_time;
	std::vector<int> attacker_list;
	std::vector<int> defender_list;
};

class Role;
struct MatchTeam;
class ActivityTeamFight : public Activity
{
public:
	ActivityTeamFight(ActivityManager *activity_manager);
	~ActivityTeamFight();

	virtual void OnActivityStatusChange(int from_status, int to_status);
	virtual void Update(unsigned long interval, time_t now_second);
	virtual void OnMonthChange();

	void OnMatchReq(Role* role);
	void OnCancelReq(Role* role);

	virtual void OnUserLogin(Role *role);
	virtual void OnUserLogout(Role *role);

	void FightRet(int uid, bool win, int score, bool is_draw);
	void FightRecord(int user_list1[MAX_TEAM_MEMBER_NUM], int user_list2[MAX_TEAM_MEMBER_NUM]);
	void OnFetchBoxReward(Role * role, int box_type);

	void OnAdavanceFetch(Role * role, short op_type, int param1);
	void SendAdvanceFetchInfo(Role * role);

	void SyncRoleScoreToActivityData(Role* role);
	void SyncActivityDataToRoleScore(Role* role);
	void SyncActivityDataToRoleScore(const RoleInfo& info, Role* role);
	void SendRoleInfo(Role* role);
	void SendRankInfo(Role* role);
	void SendRecordInfo(Role* role, bool only_me);
	void OnMatchEnemy(MatchTeam* mtA, MatchTeam* mtB);
	bool StartBattle(const ActTeamFightMatchGroup& mg);
	void CheckStartBattle(unsigned int now);

	void OnRoleFightRet(int uid, bool is_attacker, int battle_result, int atk_alive_num, int def_alive_num);
private:
	RoleList m_role_list;
	FightRecordList m_record_list;
	ActivityTeamFightMatchManager* m_match_mgr;
	std::queue<ActTeamFightMatchGroup> m_match_group;
	unsigned int m_check_start_battle_time;
};

#endif // !__ACTIVITY_TEAM_FIGHT_HPP__
