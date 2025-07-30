#ifndef __WORLD_ARENA_BATTLE_MGR_HPP__
#define __WORLD_ARENA_BATTLE_MGR_HPP__

#include <vector>
#include <map>

struct WorldArenaScheduleInfo;
class WorldArena;
class SpecialLogic;
class Role;
class WorldArenaBattleMgr
{
public:
	struct ScheduleInfo
	{
		long long schedule_id;
		long long attacker_uuid;
		long long defender_uuid;
		int fb_key;
	};

	typedef std::map<long long, ScheduleInfo> SchduleMap;
	typedef std::map<long long, int> UserSceneKeyHash;
public:
	WorldArenaBattleMgr(WorldArena* wa);
	~WorldArenaBattleMgr();

	int GetCurRoundIndex() const { return m_cur_round_index; }
	bool HasCreateScenesForToday() const;
	void ResetCreateScenesFlag();
	void ResetRoundIndex();
	void CreateScenesForSchedulesToday(int round_index, const std::vector<const WorldArenaScheduleInfo*>& today_schedules);
	int GenFBKey();
	SpecialLogic* GetSpecialLogicForUser(long long uuid);
	void StandbyForFight(Role* role, bool is_ready);
private:
	void ResetLocalData();

	WorldArena* m_world_arena;
	SchduleMap m_today_schedules;
	int m_room_key_generator;
	UserSceneKeyHash m_user_scene_key_hash;
	bool m_has_create_scenes_for_today;
	int m_cur_round_index;
};

#endif