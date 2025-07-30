#ifndef __OFFLINE_ROBOT_ROUTE_HPP__
#define __OFFLINE_ROBOT_ROUTE_HPP__

#include <set>

namespace crossgameprotocal
{
	struct GameHiddenQueryOfflineBattleDataForRobot;
}

struct UserCacheNode;
struct RoleBattleData;
class OfflineRobotRoute
{
public:
	static void RequestAddOfflineRoleRobotToTeam(int team_id, int leader_level, int robot_level_min, int robot_level_max, const std::set<int>& exclude_uid_set, int best_level_min, int best_level_max);
	static void ResponseOfflineBattleDataForRobot(int server_id, int team_id, UserCacheNode* robot_user, const RoleBattleData& robot_rbd, bool is_from_origin_server);
	static void NotifyOfflineBattleDataForRobotResult(int uid, bool is_succ);
	static void NotifyRealRoleRobotLeaveTeam(int uid);
private:
	static void AssembleQueryMsg(crossgameprotocal::GameHiddenQueryOfflineBattleDataForRobot* msg, 
		int team_id, int leader_level, int robot_level_min, int robot_level_max, 
		const std::set<int>& exclude_uid_set, int best_level_min, int best_level_max);
};


#endif