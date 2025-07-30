#ifndef __CLOUD_ARENA_RANK_MANAGER_HPP__
#define __CLOUD_ARENA_RANK_MANAGER_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/cloudarenadef.hpp"

namespace Protocol
{
	class SCCloudArenaRankList;
}

class CloudArenaUser;
class CloudArenaRank;
struct CloudArenaRankItem;
class CloudArenaRankManager
{
public:
	typedef std::map<UniqueServerID, CloudArenaRank*> ServerRankMap;
	typedef std::map<int, CloudArenaRankItem> UnknownMotherServerUserMap;

public:
	CloudArenaRankManager();
	~CloudArenaRankManager();

	void OnUserUpdateRankInfo(const CloudArenaUser* user);
	void OnUserUpdateDanRank(const CloudArenaUser* user, int old_dan, int new_dan);
	void OnRobotUpdateServerRankInfo(const CloudArenaUser* user, const UniqueServerID& usid);
	void AssembleTotalRankMsg(int uid, Protocol::SCCloudArenaRankList* msg);
	void AssembleDanRankMsg(int uid, int dan_type, Protocol::SCCloudArenaRankList* msg);
	void AssembleServerRankMsg(int uid, Protocol::SCCloudArenaRankList* msg);
	int GetUserRank(int uid);
	void OnGameRegisterCross(const UniqueServerID& usid);
	void OnStartNewSeason();
private:
	void ConstructRankItem(ARG_OUT CloudArenaRankItem* item, const CloudArenaUser* user);
	CloudArenaRank* GetServerRankInstance(const UniqueServerID& unique_server_id);
	void TryUpdateServerRank(int uid, const CloudArenaRankItem& rank_item);
	void RobotUpdateAllServerRank(const CloudArenaRankItem& rank_item);
	void UpdateUnknownMotherServerUserRankItem(int uid, const CloudArenaRankItem& rank_item);
	void RetryUnknownMotherServerUserRankItemUpdate(const UniqueServerID& mother_server_uid);
	void Release();

	void AssembleRankMsg(CloudArenaRank* rank_inst, int uid, Protocol::SCCloudArenaRankList* msg);

	CloudArenaRank* m_total_rank;		// 总榜
	ServerRankMap m_server_rank_map;	// 本服榜
	UnknownMotherServerUserMap m_unknown_mother_server_user_map; // 用于刚开服时游戏服未连接，不知道原服的玩家数据

	CloudArenaRank* m_dan_rank_list[CLOUD_ARENA_DAN_TYPE_MAX];
};

#endif