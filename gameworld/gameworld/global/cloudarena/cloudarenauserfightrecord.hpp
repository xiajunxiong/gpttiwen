#ifndef __CLOUD_ARENA_USER_FIGHT_RECORD_HPP__
#define __CLOUD_ARENA_USER_FIGHT_RECORD_HPP__

#include "servercommon/serverdef.h"
#include "servercommon/cloudarenadef.hpp"

class CloudArenaUserFightRecord
{
public:
	CloudArenaUserFightRecord();
	~CloudArenaUserFightRecord();

	void Init(int head, int tail, int record_data_length, const char* record_buffer);
	void GetRecordData(ARG_OUT int* head, ARG_OUT int* tail, ARG_OUT int* record_data_length, ARG_OUT char* record_buffer, int record_buff_size) const;
	inline int GetRecordHead() const { return m_head; }
	inline int GetRecordTail() const { return m_tail; }
	void AddFightRecord(const CloudArenaFightRecord& record);
	void ClearAllRecord();
	void GetFightRecord(int offset, int count, ARG_OUT CloudArenaFightRecord* record_list, ARG_OUT int* record_num, int max_num) const;
private:
	bool IsFull() const;
	bool IsEmpty() const;
	inline int Size() const { return (m_tail - m_head + CLOUD_ARENA_USER_FIGHT_RECORD_NUM) % CLOUD_ARENA_USER_FIGHT_RECORD_NUM; }
	void PopFront();
	void PushBack(const CloudArenaFightRecord& record);

	int m_head;
	int m_tail;
	CloudArenaFightRecord m_record_list[CLOUD_ARENA_USER_FIGHT_RECORD_NUM];
};


#endif