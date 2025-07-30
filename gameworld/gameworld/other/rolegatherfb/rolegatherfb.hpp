#ifndef __ROLE_GATHER_FB_HPP__
#define __ROLE_GATHER_FB_HPP__

#include "servercommon/rolegatherfbdef.h"
#include "other/attributelist.hpp"

class RoleModuleManager;
class Role;

class RoleGatherFb
{
public:
	RoleGatherFb();
	~RoleGatherFb();

	void* operator new(size_t c);
	void operator delete(void* m);

	void Init(RoleModuleManager * mgr, const RoleGatherFbParam & param);
	void GetInitParam(RoleGatherFbParam * param);

	void OnResetModuleData(unsigned old_dayid, unsigned int now_dayid);
	void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);

	int GetGatherPower();
	bool AddGatherPower(int add_power, bool is_notice = false);

	void SendInfo();
private:
	RoleModuleManager * m_role_module_mgr;

	RoleGatherFbParam m_param;
};

#endif