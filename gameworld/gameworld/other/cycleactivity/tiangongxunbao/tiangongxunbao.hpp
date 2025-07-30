#ifndef __TIAN_GONG_XUN_BAO_HPP__
#define __TIAN_GONG_XUN_BAO_HPP__

#include "servercommon/servercommon.h"

class RoleModuleManager;
class RoleTianGongXunBao
{
public:
	RoleTianGongXunBao();
	~RoleTianGongXunBao();

	void* operator new(size_t c);
	void operator delete(void* m);

	void Init(RoleModuleManager * module_mgr);

	void Update(unsigned int now_second);
	void OnRoleLogin();
	void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);

	void StartAction();
	void SendInfo();
private:


	RoleModuleManager * m_module_mgr;
};

#endif
