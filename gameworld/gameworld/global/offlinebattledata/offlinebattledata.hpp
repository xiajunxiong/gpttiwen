#ifndef __OFFLINE_BATTLE_DATA_HPP__
#define __OFFLINE_BATTLE_DATA_HPP__

#include "servercommon/struct/battle/battle_def.hpp"
#include <set>
#include <map>

struct UserOfflineBattleData
{
	UserOfflineBattleData() : is_dirty(false), today_offline_robot_times(0) {}

	bool is_dirty;
	int today_offline_robot_times;
	RoleBattleData rbd;
};

namespace crossgameprotocal
{
	struct GameHiddenQueryOfflineBattleDataForRobot;
	struct GameHiddenNotifyOfflineBattleDataForRobotResult;
	struct GameHiddenRealRoleRobotLeaveTeamNotice;
	struct GameHiddenSyncOfflineBattleData;
	struct HiddenGameQueryBattleDataForOfflineBattleDataSystem;
}

class Role;
struct OfflineUserBattleDataParam;
class OfflineUserBattleDataListForInit;
class OfflineBattleDataMgr
{
public:
	typedef std::map<int, UserOfflineBattleData> UserMap;
	typedef std::set<int> InUseUserSet;
	typedef std::map<int, InUseUserSet> ServerInUseUserSet;

public:
	static OfflineBattleDataMgr& Instance();

	void OnServerStart();
	void OnServerStop();
	void Update(unsigned int now);
	void OnLogin(const UserID& user_id);
	void OnLogout(Role* role);
	void OnRoleLevelUp(Role* role);
	const UserOfflineBattleData* GetUserBattleData(int uid) const;
	void OnDayChange();
	bool IsLoadFinish() const;

	void OnLoadReturn(int ret, const OfflineUserBattleDataListForInit& param);

	void OnQueryOfflineBattleDataForRobot(crossgameprotocal::GameHiddenQueryOfflineBattleDataForRobot* msg, bool is_from_origin_server);
	void OnNotifyOfflineBattleDataForRobotResult(crossgameprotocal::GameHiddenNotifyOfflineBattleDataForRobotResult* msg);
	void OnUserBeingUsedOnRobot(int uid);
	void OnRealRoleRobotLeaveTeamNotice(crossgameprotocal::GameHiddenRealRoleRobotLeaveTeamNotice* msg);
	void OnSyncOfflineBattleData(crossgameprotocal::GameHiddenSyncOfflineBattleData* msg);
	void OnGameServerConnect(const UniqueServerID& mother_server_id, const std::set<int>& server_id_set);
	void OnQueryBattleDataForHiddenOfflineBattleDataSystem(crossgameprotocal::HiddenGameQueryBattleDataForOfflineBattleDataSystem* msg);

	std::vector<const UserOfflineBattleData *> GetAllUserOfflineBattleData() const;
private:
	OfflineBattleDataMgr();
	~OfflineBattleDataMgr();
	OfflineBattleDataMgr& operator= (const OfflineBattleDataMgr&);
	OfflineBattleDataMgr(const OfflineBattleDataMgr&);

	void OnLoadAllFinish();
	UserOfflineBattleData* MutableUserBattleData(int uid);
	void UpdateRoleBattleData(Role* role);
	bool QueryBattleDataFromOtherSystem();
	bool QueryBattleDataFromColosseum();
	bool QueryBattleDataFromCloudArena();
	void QueryBattleDataFromOriginServer(const UniqueServerID& mother_server_id, const std::set<int>& server_id_set);
	void InsertRoleBattleData(int uid, const RoleBattleData& rbd);
	bool InitRoleBattleDataFromDB(const OfflineUserBattleDataParam& param);
	void Save(bool is_save_all);
	void ClearDirtyFlag();
	bool LoadUserData(long long next_id_from);
	void GiveOfflineRobotReward(int uid);
	void SyncOfflineBattleDataToHidden(int uid);

	UserMap m_user_map;
	unsigned int m_query_battle_data_from_other_system_timestamp;
	unsigned int m_next_save_timestamp;
	int m_data_load_status;

	InUseUserSet m_in_use_set;

	bool m_pull_jjc_data_flag;
};

#endif