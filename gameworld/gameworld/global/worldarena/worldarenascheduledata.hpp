#ifndef __WORLD_ARENA_SCHEDULE_DATA_HPP__
#define __WORLD_ARENA_SCHEDULE_DATA_HPP__

#include "servercommon/struct/global/worldarenaparam.hpp"

class WorldArenaScheduleData
{
public:
	WorldArenaScheduleData(bool old_state);
	~WorldArenaScheduleData();

	const WorldArenaScheduleInfo& GetScheduleData() const;
	WorldArenaScheduleInfo& MutableScheduleData();

	void SetDirty(bool is_dirty);
	void SetOldState(bool old_state);

	bool IsDirty() const;
	bool OldState() const;

	void SetDirtyTimestamp_MS(unsigned long long timestamp);
	unsigned long long GetDirtyTimestamp_MS() const;

	void SetLastSaveTimestamp_MS(unsigned long long timestamp);
	unsigned long long GetLastSaveTimestamp_MS() const;

	long long GetScheduleId() const;
private:
	bool m_is_dirty;
	bool m_old_state;
	unsigned long long m_last_save_timestamp_ms;
	unsigned long long m_dirty_timestamp_ms;
	
	WorldArenaScheduleInfo m_schedule_info;
};


#endif