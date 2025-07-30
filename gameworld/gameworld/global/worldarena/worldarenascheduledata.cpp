#include "worldarenascheduledata.hpp"
#include "engineadapter.h"

WorldArenaScheduleData::WorldArenaScheduleData(bool old_state) : m_is_dirty(false), m_old_state(old_state), 
	m_last_save_timestamp_ms(0ULL), m_dirty_timestamp_ms(0ULL)
{
	if (!old_state)
	{
		m_is_dirty = true; // 如果以前没有现在有，则说明是新增的，所以要setdirty
	}
}

WorldArenaScheduleData::~WorldArenaScheduleData()
{

}

const WorldArenaScheduleInfo& WorldArenaScheduleData::GetScheduleData() const
{
	return m_schedule_info;
}

WorldArenaScheduleInfo& WorldArenaScheduleData::MutableScheduleData()
{
	this->SetDirty(true);
	return m_schedule_info;
}

void WorldArenaScheduleData::SetDirty(bool is_dirty)
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

void WorldArenaScheduleData::SetOldState(bool old_state)
{
	m_old_state = old_state;
}

bool WorldArenaScheduleData::IsDirty() const
{
	return m_is_dirty;
}

bool WorldArenaScheduleData::OldState() const
{
	return m_old_state;
}

void WorldArenaScheduleData::SetDirtyTimestamp_MS(unsigned long long timestamp)
{
	m_dirty_timestamp_ms = timestamp;
}

unsigned long long WorldArenaScheduleData::GetDirtyTimestamp_MS() const
{
	return m_dirty_timestamp_ms;
}

void WorldArenaScheduleData::SetLastSaveTimestamp_MS(unsigned long long timestamp)
{
	m_last_save_timestamp_ms = timestamp;
}

unsigned long long WorldArenaScheduleData::GetLastSaveTimestamp_MS() const
{
	return m_last_save_timestamp_ms;
}

long long WorldArenaScheduleData::GetScheduleId() const
{
	return m_schedule_info.schedule_id;
}

