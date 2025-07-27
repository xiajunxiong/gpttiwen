#include "internalcommmonitor.hpp"
#include "utility/logagent.h"
#include "servercommon/logdef.h"

void InternalCommMonitor::SetMsgTypeRange(int start_msg_type, int end_msg_type)
{
	start_msg_type = GetMax(0, start_msg_type);
	end_msg_type = GetMin(end_msg_type, ARRAY_ITEM_COUNT(m_msg_num_list));
}

void InternalCommMonitor::OnRecvMsg(int msg_type)
{
	if (msg_type < 0 || msg_type >= ARRAY_ITEM_COUNT(m_msg_num_list)) return;

	m_msg_num_list[msg_type] += 1;
}

void InternalCommMonitor::Update(unsigned int now, LogAgent* log_agent)
{
	if (NULL == log_agent) return;

	if (0u == m_next_print_time)
	{
		m_next_print_time = now + 10 * 60;
	}

	if (now >= m_next_print_time)
	{
		m_next_print_time = now + 10 * 60;
		this->Print(log_agent);

		memset(m_msg_num_list, 0, sizeof(m_msg_num_list));
	}
}

InternalCommMonitor::InternalCommMonitor() : m_start_msg_type(0), 
	m_end_msg_type(ARRAY_ITEM_COUNT(m_msg_num_list)), m_next_print_time(0u)
{
	memset(m_msg_num_list, 0, sizeof(m_msg_num_list));
}

InternalCommMonitor::~InternalCommMonitor()
{

}

void InternalCommMonitor::Print(LogAgent* logagent)
{
	logagent->buff_printf("InternalCommMonitor::Print\n");
	for (int i = m_start_msg_type; i < m_end_msg_type; ++i)
	{
		if (0 == m_msg_num_list[i])
		{
			continue;
		}
		logagent->buff_printf("msg type[%d] times[%d]\n", i, m_msg_num_list[i]);
	}
	logagent->buff_print("--------------------------------------\n\n\n");
	logagent->commit_buff(LL_INFO);
}

