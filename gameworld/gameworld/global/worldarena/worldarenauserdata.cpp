#include "worldarenauserdata.hpp"
#include "engineadapter.h"

WorldArenaUserData::WorldArenaUserData(bool old_state) : m_is_dirty(false), m_old_state(old_state), 
	m_last_save_timestamp_ms(0ULL), m_dirty_timestamp_ms(0ULL)
{
	if (!old_state)
	{
		m_is_dirty = true; // 如果以前没有现在有，则说明是新增的，所以要setdirty
	}
}

WorldArenaUserData::~WorldArenaUserData()
{

}

const WorldArenaUserInfo& WorldArenaUserData::GetUserData() const
{
	return m_user_info;
}

WorldArenaUserInfo& WorldArenaUserData::MutableUserData()
{
	this->SetDirty(true);
	return m_user_info;
}

void WorldArenaUserData::SetDirty(bool is_dirty)
{
	m_is_dirty = is_dirty;
	if (is_dirty)
	{
		this->SetDirtyTimestamp_MS(GetNowTime_MS());
	}
	else
	{
		this->SetDirtyTimestamp_MS(0ULL);
	}
}

void WorldArenaUserData::SetOldState(bool old_state)
{
	m_old_state = old_state;
}

bool WorldArenaUserData::IsDirty() const
{
	return m_is_dirty;
}

bool WorldArenaUserData::OldState() const
{
	return m_old_state;
}

void WorldArenaUserData::SetDirtyTimestamp_MS(unsigned long long timestamp)
{
	m_dirty_timestamp_ms = timestamp;
}

unsigned long long WorldArenaUserData::GetDirtyTimestamp_MS() const
{
	return m_dirty_timestamp_ms;
}

void WorldArenaUserData::SetLastSaveTimestamp_MS(unsigned long long timestamp)
{
	m_last_save_timestamp_ms = timestamp;
}

unsigned long long WorldArenaUserData::GetLastSaveTimestamp_MS() const
{
	return m_last_save_timestamp_ms;
}

