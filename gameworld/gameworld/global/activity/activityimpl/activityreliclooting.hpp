#ifndef __ACTIVITY_RELIC_LOOTING_HPP__
#define __ACTIVITY_RELIC_LOOTING_HPP__

#include "global/activity/activity.hpp"
#include "other/match/reliclootingmathmanager.hpp"
#include "servercommon/reliclootingdef.h"
#include "servercommon/serverdef.h"

#include <map>
#include <vector>

const static int RELIC_LOOT_MAX_ONE_SIDE_PLAYER_NUM = 3;//单边最大人数

class Role;
struct MatchTeam;

struct RelicLootingUser
{
	RelicLootingUser() : win_times(0), reward_get_bit(0) {}
	int win_times;
	int reward_get_bit;
};

class ActivityRelicLooting : public Activity
{
public:
	ActivityRelicLooting(ActivityManager *activity_manager);
	~ActivityRelicLooting();

	virtual void OnActivityStatusChange(int from_status, int to_status);
	virtual void Update(unsigned long interval, time_t now_second);
	virtual void OnUserLogin(Role *role);
	virtual void OnUserLogout(Role *role);
	int IsUserPlay(int role_id);
	void OnUserPlayEnd(int role_id);

	void OnMatchReq(Role* role);
	void OnCancelReq(Role* role);

	void OnMatchEnemy(MatchTeam* mtA, MatchTeam* mtB);

	void OnGetRoleReward(Role *role, int seq);
	void OnRoleRank(Role * role, int jifen, bool is_win);
	void OnUserIdRank(int uid, int jifen, bool is_win);
	void OnRoleCheckRankList(Role * role);//玩家查询排行榜
	void OnRoleCheckAchievementReward(Role * role);//玩家查询成就奖励
	void SendRankListToUID(int uid);
	void SendRewardAchievementToUID(int uid);
	void GetAchievementReward(int uid, int seq);
	void SetShow(int uid);

	//跨服返回操作
	void CrossBack(int uid,int seq);
	bool IsMatch(int uid);
private:
	void NextRankPosi();//下次排名刷新时间戳
	void RefreshRank();
	void RewardRank();
	void ResetRoleActivityData();
	void OnActivityEndReward();

	time_t rank_refresh_tamp;
	time_t m_next_match_time;
	MatchLogic m_match_logic;
	ActivityRelicLootingMatchManager * m_match_mgr;
	std::vector<RelicLootRankData> activity_rank;
	std::map<int, RelicLootingUser> role_win_times;
	std::map<int, int> player_map;
};

#endif //__ACTIVITY_RELIC_LOOTING_HPP__