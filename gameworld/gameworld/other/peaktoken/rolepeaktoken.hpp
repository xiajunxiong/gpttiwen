#ifndef __ROLE_PEAK_TOKEN_HPP__
#define __ROLE_PEAK_TOKEN_HPP__

#include "servercommon/peaktokendef.hpp"
class RoleModuleManager;

class RolePeakToken
{
public:
	RolePeakToken();
	~RolePeakToken();

	void* operator new(size_t c);
	void operator delete(void* m);

	void Init(RoleModuleManager *module_mgr, const RolePeakTokenParam & param);
	void GetInitParam(RolePeakTokenParam * param);

	void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);

	int GetToDayUseTimes() { return m_param.today_use_times; };
	void SetToDayUseTimes(int today_use_times) { m_param.today_use_times = today_use_times; };

	void SendInfo();

private:

	RoleModuleManager * m_module_mgr;

	RolePeakTokenParam m_param;
};

#endif
