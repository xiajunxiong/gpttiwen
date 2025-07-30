#ifndef __ROLE_HUAN_JIE_HPP__
#define __ROLE_HUAN_JIE_HPP__

#include "servercommon/rolehuanjiedef.hpp"

class RoleModuleManager;
class Role;

class RoleHuanJie
{
public:
	RoleHuanJie();
	~RoleHuanJie();

	void Init(RoleModuleManager * mgr, const RoleHuanJieParam & param);
	void GetInitParam(RoleHuanJieParam * param);

	void SendInfo();
	void AddTianTiRecord(const CommonSaveHuanJieTianTiReceordData & record_data);
	
	void* operator new(size_t c);
	void operator delete(void* m);
private:
	RoleModuleManager * m_role_module_mgr;

	RoleHuanJieParam m_param;
};

#endif