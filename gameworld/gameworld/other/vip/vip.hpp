#ifndef __VIP_HPP__
#define __VIP_HPP__

#include "servercommon/serverdef.h"
#include "servercommon/vipdef.hpp"


class RoleModuleManager;
class Vip
{
public: 
	Vip();
	~Vip();

	void* operator new(size_t c);
	void operator delete(void* m);

	RoleModuleManager* GetRoleModuleManager();

	void Init(RoleModuleManager *role_module_manager, const VipParam &vip_param); 
	void GetOtherInitParam(VipParam *vip_param);	

	int  GetVipLevel();

private:
	class VipImpl;
	VipImpl* m_pImpl;
};

#endif // __VIP_HPP__



