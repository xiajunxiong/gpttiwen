#ifndef __RMI_HIDDEN_H__
#define __RMI_HIDDEN_H__

#include "rmi/rmibase.h"
#include "servercommon/struct/roleinfolist.h"

class RMIHiddenClient : public rmi::RMIProxyObject
{
public:
	RMIHiddenClient() {}
	virtual ~RMIHiddenClient() {}

	bool GetRoleInfoAsyn(int *role_id, int num, bool is_load_role_to_cache, rmi::RMIBackObject *backobj);
};

class RMIGetRoleInfoBackObject : public rmi::RMIBackObject
{
public:
	RMIGetRoleInfoBackObject() {};
	virtual ~RMIGetRoleInfoBackObject() {};

	virtual bool __response(TLVUnserializer &out_param);
	virtual void __exception(int error);
	virtual void __timeout();
	virtual void __free();
protected:
	virtual void GetRoleInfoRet(int ret, const RoleInfoList &role_info);
};

#endif

