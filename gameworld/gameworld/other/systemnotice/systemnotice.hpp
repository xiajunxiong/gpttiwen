#ifndef __SYSTEM_NOTICE_HPP__
#define __SYSTEM_NOTICE_HPP__

#include "servercommon/serverdef.h"

class RoleModuleManager;
class SystemNotice
{
public:
	SystemNotice();
	~SystemNotice();

	RoleModuleManager * GetRoleModuleManager() { return m_role_module_mgr; }

	void Init(RoleModuleManager * role_module_manager);

	void GetAward(int seq);
	void SendFetchFlagInfo();

	//void* operator new(size_t c);
	//void operator delete(void* m);
private:
	RoleModuleManager * m_role_module_mgr;
};

#endif
