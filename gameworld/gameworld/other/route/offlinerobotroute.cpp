#include "offlinerobotroute.hpp"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "servercommon/internalprotocal/crossgameprotocal.h"
#include "internalcomm.h"
#include "global/offlinebattledata/offlinebattledata.hpp"
#include "global/usercache/usercache.hpp"
#include "servercommon/serverconfig/localconfig.hpp"
#include "global/team/teammatch.hpp"

void OfflineRobotRoute::RequestAddOfflineRoleRobotToTeam(int team_id, int leader_level, int robot_level_min, int robot_level_max, const std::set<int>& exclude_uid_set, int best_level_min, int best_level_max)
{
	crossgameprotocal::GameHiddenQueryOfflineBattleDataForRobot msg;
	AssembleQueryMsg(&msg, team_id, leader_level, robot_level_min, robot_level_max, 
		exclude_uid_set, best_level_min, best_level_max);
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		InternalComm::Instance().SendToHiddenThroughCross((const char*)&msg, sizeof(msg));
	}
	else
	{
		OfflineBattleDataMgr::Instance().OnQueryOfflineBattleDataForRobot(&msg, false);
	}
}

void OfflineRobotRoute::ResponseOfflineBattleDataForRobot(int server_id, int team_id, UserCacheNode* robot_user, const RoleBattleData& robot_rbd, bool is_from_origin_server)
{
	crossgameprotocal::HiddenGameQueryOfflineBattleDataForRobot msg;
	msg.team_id = team_id;
	msg.uid = robot_user->uid;
	msg.rbd = robot_rbd;

	if (is_from_origin_server)
	{
		InternalComm::Instance().SendSerializeMsgToGameThroughCross(&msg, server_id);
	}
	else
	{
		TeamMatch::Instance().OnOfflineBattleDataForRobotResp(&msg);
	}
}

void OfflineRobotRoute::NotifyOfflineBattleDataForRobotResult(int uid, bool is_succ)
{
	crossgameprotocal::GameHiddenNotifyOfflineBattleDataForRobotResult msg;
	msg.uid = uid;
	msg.result = is_succ ?
		crossgameprotocal::GameHiddenNotifyOfflineBattleDataForRobotResult::SUCC :
		crossgameprotocal::GameHiddenNotifyOfflineBattleDataForRobotResult::FAIL ;

	if (!CrossConfig::Instance().IsHiddenServer())
	{
		InternalComm::Instance().SendToHiddenThroughCross((const char*)&msg, sizeof(msg));
	}
	else
	{
		OfflineBattleDataMgr::Instance().OnNotifyOfflineBattleDataForRobotResult(&msg);
	}
}

void OfflineRobotRoute::NotifyRealRoleRobotLeaveTeam(int uid)
{
	crossgameprotocal::GameHiddenRealRoleRobotLeaveTeamNotice msg;
	msg.uid = uid;

	if (!CrossConfig::Instance().IsHiddenServer())
	{
		InternalComm::Instance().SendToHiddenThroughCross((const char*)&msg, sizeof(msg));
	}
	else
	{
		OfflineBattleDataMgr::Instance().OnRealRoleRobotLeaveTeamNotice(&msg);
	}
}

void OfflineRobotRoute::AssembleQueryMsg(crossgameprotocal::GameHiddenQueryOfflineBattleDataForRobot* msg, int team_id, int leader_level, int robot_level_min, int robot_level_max, const std::set<int>& exclude_uid_set, int best_level_min, int best_level_max)
{
	msg->team_id = team_id;
	msg->server_id = LocalConfig::Instance().GetMergeServerId();
	msg->level_min = robot_level_min;
	msg->level_max = robot_level_max;
	msg->leader_level = leader_level;
	msg->best_level_min = best_level_min;
	msg->best_level_max = best_level_max;
	msg->exclude_uid_num = 0;
	memset(msg->exclude_uid_list, 0, sizeof(msg->exclude_uid_list));

	std::set<int>::const_iterator it = exclude_uid_set.begin();
	for (; it != exclude_uid_set.end() 
		&& msg->exclude_uid_num < ARRAY_ITEM_COUNT(msg->exclude_uid_list); ++it)
	{
		msg->exclude_uid_list[msg->exclude_uid_num++] = *it;
	}
}

