#ifndef RMIDAROLECLIENT_H
#define RMIDAROLECLIENT_H

#include "rmi/rmibase.h"
#include "servercommon/struct/roleinfolist.h"
#include "servercommon/struct/roleinitparam.h"
#include <set>
#include <server/loginlog.h>

class RMILRoleClient : public rmi::RMIProxyObject
{
public:
	RMILRoleClient(){};
	virtual ~RMILRoleClient(){};

	bool GetRoleInfoAsyn(int *role_id, int num, bool is_load_role_to_cache, rmi::RMIBackObject *backobj);
	bool CreateRoleAsyn(const char* pname, int db_index, long long name_strid, const RoleInitParam &p, rmi::RMIBackObject *backobj);
	bool DestroyRoleAsyn(int role_id, rmi::RMIBackObject *backobj);

protected:
};

class RMIGetRoleInfoBackObject : public rmi::RMIBackObject
{
public:
	RMIGetRoleInfoBackObject(){};
	virtual ~RMIGetRoleInfoBackObject(){};

	virtual bool __response(TLVUnserializer &out_param);
	virtual void __exception(int error){};
	virtual void __timeout(){};
	virtual void __free();
protected:
	virtual void GetRoleInfoRet(int ret, const RoleInfoList &role_info);
};

class RMICreateRoleBackObject : public rmi::RMIBackObject
{
public:
	RMICreateRoleBackObject(){};
	virtual ~RMICreateRoleBackObject(){};

	virtual bool __response(TLVUnserializer &out_param);
	virtual void __exception(int error) { loginlog::g_log_login.printf(LL_ERROR, "__exception错误"); Error(CREATE_ROLE_RESULT_TYPE_SERVER_ERROR); }
	virtual void __timeout(){ loginlog::g_log_login.printf(LL_ERROR, "__timeout错误"); Error(CREATE_ROLE_RESULT_TYPE_SERVER_ERROR); }
	virtual void __free();
protected:
	virtual void CreateRoleRet(int ret, int role_id, int create_result);
	virtual void Error(int create_result){}
};

class RMIDestroyRoleBackObject : public rmi::RMIBackObject
{
public:
	RMIDestroyRoleBackObject(){};
	virtual ~RMIDestroyRoleBackObject(){};

	virtual bool __response(TLVUnserializer &out_param);
	virtual void __exception(int error){};
	virtual void __timeout(){};
	virtual void __free();
protected:
	virtual void DestroyRoleRet(int ret);
};

class RMIInitNameStrIdBackObject :public rmi::RMIBackObject
{
public:
	RMIInitNameStrIdBackObject(){};
	virtual ~RMIInitNameStrIdBackObject(){};

	virtual bool __response(TLVUnserializer &out_param);
	virtual void __exception(int error){};
	virtual void __timeout(){};
	virtual void __free();
protected:
	virtual void InitList(std::set<long long> str_id_list);
};

#endif

