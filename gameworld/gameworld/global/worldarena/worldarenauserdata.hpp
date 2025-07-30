#ifndef __WORLD_ARENA_USER_DATA_HPP__
#define __WORLD_ARENA_USER_DATA_HPP__

#include "servercommon/struct/global/worldarenaparam.hpp"

class WorldArenaUserData
{
public:
	WorldArenaUserData(bool old_state);
	~WorldArenaUserData();

	const WorldArenaUserInfo& GetUserData() const;
	WorldArenaUserInfo& MutableUserData();

	void SetDirty(bool is_dirty);
	void SetOldState(bool old_state);

	bool IsDirty() const;
	bool OldState() const;

	void SetDirtyTimestamp_MS(unsigned long long timestamp);
	unsigned long long GetDirtyTimestamp_MS() const;

	void SetLastSaveTimestamp_MS(unsigned long long timestamp);
	unsigned long long GetLastSaveTimestamp_MS() const;
private:
	bool m_is_dirty;
	bool m_old_state;
	unsigned long long m_last_save_timestamp_ms;
	unsigned long long m_dirty_timestamp_ms;
	
	WorldArenaUserInfo m_user_info;
};


#endif