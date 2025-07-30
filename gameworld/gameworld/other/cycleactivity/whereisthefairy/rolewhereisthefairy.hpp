#ifndef __ROLE_WHERE_IS_THE_FAIRY_HPP__
#define __ROLE_WHERE_IS_THE_FAIRY_HPP__

#include "servercommon/servercommon.h"

class RoleModuleManager;
class RoleWhereIsTheFairy
{
public:
	RoleWhereIsTheFairy();
	~RoleWhereIsTheFairy();

	void* operator new(size_t c);
	void operator delete(void* m);

	void Init(RoleModuleManager * module_mgr);

	void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);

	void OnFinishGame();
		
	void SendAllInfo();

private:
	RoleModuleManager * m_role_module_mgr;

};


#endif
