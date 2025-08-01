#ifndef __MONITOR_HPP__
#define __MONITOR_HPP__

#include "servercommon/monitordef.hpp"

class RoleModuleManager;
class Monitor
{
public:
	Monitor();
	~Monitor();

	void* operator new(size_t c);
	void operator delete(void* m);

	void Init(RoleModuleManager* module_mgr, const MonitorParam &monitor_param);
	void GetMonitorParam(MonitorParam *monitor_param);

	void Update(time_t now_second);
	void OnResetData(unsigned int old_dayid, unsigned int now_dayid);

	void AddMonitorNum(short monitor_type, int add_num);

private:
	void MonitorLog();
	bool CanMonitorLog();

	RoleModuleManager* m_module_mgr;

	unsigned int m_next_monitor_time;
	int m_monitor_num_list[MONITOR_TYPE_MAX];
};

#endif // __MONITOR_HPP__

