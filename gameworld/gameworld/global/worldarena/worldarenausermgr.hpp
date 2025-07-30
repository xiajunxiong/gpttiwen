#ifndef __WORLD_ARENA_USER_MGR_HPP__
#define __WORLD_ARENA_USER_MGR_HPP__

#include <map>
#include <vector>

class WorldArenaUser;
class WorldArena;
struct RoleBattleData;
struct WorldArenaSignUpInfo;
struct WorldArenaUserInfo;
class WorldArenaUserMgr
{
public:
	typedef std::map<long long, WorldArenaUser*> UserMap;

public:
	WorldArenaUserMgr(WorldArena* wa);
	~WorldArenaUserMgr();

	void OnDayChange();

	bool AddUserFromInit(const WorldArenaUserInfo& info);
	bool OnUserSignUp(const WorldArenaSignUpInfo& signup_info, const RoleBattleData& rbd);

	WorldArenaUser* CreateAndInsertUser(long long uuid);
	WorldArenaUser* CreateAndInsertRobot(long long uuid, int level);
	WorldArenaUser* GetWorldArenaUser(long long uuid);

	void OnSeasonRestart();

	bool Save(bool is_save_all);
	void ClearDirtyMark();
	void ClearDeletedUser(const std::vector<long long>& delete_list);

	bool LoadUser(long long id_from);
	void OnLoadUserSucc();
	bool IsLoadFinish();
	std::vector<WorldArenaUser*> GetUsersInGroup(long long group_id);

	void NoticeAllUsersStartEnterScene(int start_round_index, unsigned int end_timestamp);
	void SendScheduleReleaseNoticeMailToAllUsers();
private:
	void SetUserLastSaveTimestampMS(const std::vector<WorldArenaUserInfo>& save_user_list);

	WorldArena* m_arena;
	UserMap m_user_map;
	UserMap m_to_delete_user_map;

	int m_data_load_status;
};


#endif