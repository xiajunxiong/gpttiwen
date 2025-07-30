#ifndef __WORLD_ARENA_SCHEDULE_HPP__
#define __WORLD_ARENA_SCHEDULE_HPP__

#include "worldarenascheduledata.hpp"

class WorldArena;
class WorldArenaUser;
class WorldArenaSchedule
{
public:
	static const int ROUND_NUM = WORLD_ARENA_ROUND_NUM;
	static const int FIGHT_NUM_PER_ROUND = WORLD_ARENA_FIGHT_NUM_PER_ROUND;
	UNSTD_STATIC_CHECK(WORLD_ARENA_FIGHTER_NUM_PER_GROUP % 2 == 0 && WORLD_ARENA_FIGHTER_NUM_PER_GROUP > 0);

	typedef std::map<long long, WorldArenaScheduleData> ScheduleMap; // key是一个全局唯一的schedule_id
public:
	WorldArenaSchedule(WorldArena* arena_ptr);
	~WorldArenaSchedule();

	void Update(unsigned int now);

	bool AddScheduleFromInit(const WorldArenaScheduleInfo& info);

	void MakeScheduleForGroup(long long group_id, std::vector<WorldArenaUser*>& user_vec);
	std::vector<const WorldArenaScheduleInfo*> GetScheduleInfoListByRoundIndex(int round_index);
	std::vector<long long> GetAllScheduleIds();
	std::vector<long long> GetAllGroupIds();
	void SetFightResult(long long schedule_id, int result);
	int GetFightResult(long long schedule_id);

	const WorldArenaScheduleData* GetScheduleInfo(long long schedule_id);
	void OnSeasonRestart();
	bool IsLoadFinish();
	bool LoadSchedule(long long id_from);
	void OnLoadScheduleSucc();

	bool Save(bool is_save_all);
	void ClearDirtyMark();
	void ClearDeletedSchedule(const std::vector<long long>& delete_list);

	void ModifyScheduleCheckResultTimestamp(long long schedule_id, unsigned int timestamp);

private:
	void CalculateScheduleArrangement();
	WorldArenaScheduleData* MutableScheduleInfo(long long schedule_id);
	bool ValidateArrangementArray();
	void SetScheduleLastSaveTimestampMS(const std::vector<WorldArenaScheduleInfo>& save_schedule_list);
	void ClearDirtyMark(WorldArenaScheduleData* schedule_data);
	void CheckResult(unsigned int now, WorldArenaScheduleData* schedule);

	WorldArena* m_arena_ptr;
	short m_schedule_arrangement[ROUND_NUM][FIGHT_NUM_PER_ROUND][2];

	ScheduleMap m_schedule_map;
	int m_data_load_status;

	std::set<long long> m_to_delete_schedule_map;
};

#endif