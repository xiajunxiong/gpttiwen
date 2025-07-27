#include "uniqueidgenerator.hpp"
#include "servercommon/servercommon.h"
#include "iclockmodule.h"
#include "servercommon/serverconfig/crossconfig.hpp"

UniqueIDGenerator::UniqueIDGenerator() : m_has_init(false), m_server_type(SERVER_TYPE_INVALID),
	m_server_id(0), m_server_index(0), m_last_gen_time(0), m_inc_id(0), m_clock(NULL)
{

}

UniqueIDGenerator::~UniqueIDGenerator()
{

}

UniqueIDGenerator* UniqueIDGenerator::Instance()
{
	static UniqueIDGenerator uidgen;
	return &uidgen;
}

bool UniqueIDGenerator::Init(SERVER_TYPE server_type, int server_id, int server_index, IClockModule* clock)
{
	if (NULL == clock) return false;

	assert(server_id > 0);

	if (CrossConfig::Instance().IsHiddenServer())
	{
		server_id += CrossConfig::Instance().GetHiddenServerIDX();
	}

	m_has_init = true;
	m_server_type = server_type;
	m_server_id = server_id;
	m_server_index = server_index;
	m_inc_id = 0;
	m_clock = clock;
	m_last_gen_time = clock->Time();

	return true;
}

bool UniqueIDGenerator::GenUniqueID(ARG_OUT long long* unique_id)
{
	if (!m_has_init)
	{
#ifdef _DEBUG
		assert(0);
#endif
		return false;
	}

	long long cur_time = m_clock->Time();
	if (m_last_gen_time < cur_time)
	{
		// 此处忽略服务器时间回退的问题
		m_last_gen_time = cur_time;
		m_inc_id = 0;
	}

	UniqueIDUnion tmp;
	tmp.inc_id = m_inc_id;
	tmp.timestamp_diff = (m_last_gen_time - ZERO_TIMESTAMP);
	tmp.server_index = m_server_index;
	tmp.server_id = m_server_id;
	tmp.server_type = m_server_type;
	tmp.sign_bit = 0;

	/*
	long long uuid = ((long long)m_server_type << SERVER_TYPE_SHIFT)
		+ ((long long)m_server_id << SERVER_ID_SHIFT)
		+ ((long long)m_server_index << SERVER_INDEX_SHIFT)
		+ ((long long)(m_last_gen_time - ZERO_TIMESTAMP) << TIMESTAMP_SHIFT)
		+ ((long long)m_inc_id << INC_ID_SHIFT);
	*/

	++m_inc_id;
	if (m_inc_id == (1LL << INC_ID_BIT)) // 如果自增ID已满，则强制让时间+1
	{
		m_last_gen_time += 1;
		m_inc_id = 0;
	}

	if (NULL != unique_id)
	{
		*unique_id = tmp.unique_id;
	}

#ifdef _DEBUG
	assert(m_history_unique_id_set.find(tmp.unique_id) == m_history_unique_id_set.end());
	m_history_unique_id_set.insert(tmp.unique_id);

	assert(tmp.unique_id > 0);
#endif
	return true;
}

