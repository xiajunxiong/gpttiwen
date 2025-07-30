#include "cloudarenauserfightrecord.hpp"
#include "servercommon/protobufmanager.hpp"
#include "servercommon/protobuf/cloud_arena_fight_record_list.pb.h"
#include "gamelog.h"
#include "cloudarena.hpp"

CloudArenaUserFightRecord::CloudArenaUserFightRecord() : m_head(0), m_tail(0)
{

}

CloudArenaUserFightRecord::~CloudArenaUserFightRecord()
{

}

void CloudArenaUserFightRecord::Init(int head, int tail, int record_data_length, const char* record_buffer)
{
	if (head < 0 || head >= CLOUD_ARENA_USER_FIGHT_RECORD_NUM) return;
	if (tail < 0 || tail >= CLOUD_ARENA_USER_FIGHT_RECORD_NUM) return;

	PB_CloudArenaFightRecordList * pb_data = (PB_CloudArenaFightRecordList *)ProtobufMgr::Instance()->GetProtoMessage(PROTO_CLOUD_ARENA_FIGHT_RECORD_LIST_DATA);
	if (NULL == pb_data) return;

	if (!pb_data->ParseFromArray(record_buffer, record_data_length))
	{
		gamelog::g_log_serious_error.printf(LL_ERROR, "CloudArenaUserFightRecord::Init parse protobuf failed!");
		return;
	}

	for (int i = 0, idx = head; i < pb_data->record_list_size() && i < ARRAY_ITEM_COUNT(m_record_list); ++i, idx = (idx + 1) % ARRAY_ITEM_COUNT(m_record_list))
	{
		m_record_list[idx].SetDataFromPB(pb_data->record_list(i));

		m_record_list[idx].attacker_info.is_robot = 0;
		m_record_list[idx].defender_info.is_robot = CloudArena::IsRobot(m_record_list[idx].defender_info.uid) ? 1 : 0;
	}
	
	m_head = head;
	m_tail = tail;
}

void CloudArenaUserFightRecord::GetRecordData(ARG_OUT int* head, ARG_OUT int* tail, ARG_OUT int* record_data_length, ARG_OUT char* record_buffer, int record_buff_size) const
{
	PB_CloudArenaFightRecordList* pb_data = (PB_CloudArenaFightRecordList*)ProtobufMgr::Instance()->GetProtoMessage(PROTO_CLOUD_ARENA_FIGHT_RECORD_LIST_DATA);
	if (NULL == pb_data)
	{
		return;
	}

	int size = this->Size();
	pb_data->mutable_record_list()->Clear();
	pb_data->mutable_record_list()->Reserve(size);
	for (int idx = m_head; idx != m_tail; idx = (idx + 1) % CLOUD_ARENA_USER_FIGHT_RECORD_NUM)
	{
		const CloudArenaFightRecord& record = m_record_list[idx];
		record.GetPBData(pb_data->add_record_list());
	}

	if (!pb_data->SerializeToArray(record_buffer, record_buff_size))
	{
#ifdef _DEBUG
		assert(0);
#endif
		return;
	}

	*record_data_length = pb_data->GetCachedSize();

	*head = m_head;
	*tail = m_tail;
}

void CloudArenaUserFightRecord::AddFightRecord(const CloudArenaFightRecord& record)
{
	if (this->IsFull())
	{
		this->PopFront();
	}

	this->PushBack(record);
}

void CloudArenaUserFightRecord::ClearAllRecord()
{
	while (!this->IsEmpty())
	{
		this->PopFront();
	}
}

void CloudArenaUserFightRecord::GetFightRecord(int offset, int count, ARG_OUT CloudArenaFightRecord* record_list, ARG_OUT int* record_num, int max_num) const
{
	if (this->IsEmpty()) return;

	int tmp_tail = m_tail;
	int tmp_head = m_head;
	int idx = 0;
	int cnt = 50;
	do 
	{
		tmp_tail = (tmp_tail - 1 + CLOUD_ARENA_USER_FIGHT_RECORD_NUM) % CLOUD_ARENA_USER_FIGHT_RECORD_NUM;
		if (idx >= offset)
		{
			record_list[(*record_num)++] = m_record_list[tmp_tail];
		}
		idx += 1;
	} while (tmp_tail != tmp_head && --cnt > 0 && *record_num < max_num && *record_num < count);

#ifdef _DEBUG
	assert(cnt > 0);
#endif
}

bool CloudArenaUserFightRecord::IsFull() const
{
	return (m_tail + 1) % CLOUD_ARENA_USER_FIGHT_RECORD_NUM == m_head;
}

bool CloudArenaUserFightRecord::IsEmpty() const
{
	return m_head == m_tail;
}

void CloudArenaUserFightRecord::PopFront()
{
#ifdef _DEBUG
	assert(!this->IsEmpty());
#endif

	if (this->IsEmpty()) return;

	m_head = (m_head + 1) % CLOUD_ARENA_USER_FIGHT_RECORD_NUM;
}

void CloudArenaUserFightRecord::PushBack(const CloudArenaFightRecord& record)
{
#ifdef _DEBUG
	assert(!this->IsFull());
#endif

	if (this->IsFull()) return;

	m_record_list[m_tail] = record;
	m_tail = (m_tail + 1) % CLOUD_ARENA_USER_FIGHT_RECORD_NUM;
}

