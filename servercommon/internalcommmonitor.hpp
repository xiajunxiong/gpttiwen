#ifndef __INTERNAL_COMM_MONITOR_HPP__
#define __INTERNAL_COMM_MONITOR_HPP__

#include "servercommon/servercommon.h"

class LogAgent;
class InternalCommMonitor
{
public:
	InternalCommMonitor();
	~InternalCommMonitor();

	void SetMsgTypeRange(int start_msg_type, int end_msg_type);
	void OnRecvMsg(int msg_type);
	void Update(unsigned int now, LogAgent* log_agent);
private:
	void Print(LogAgent* logagent);

	int m_start_msg_type;
	int m_end_msg_type;
	unsigned int m_next_print_time;
	int m_msg_num_list[MAX_UINT16];
};

#endif