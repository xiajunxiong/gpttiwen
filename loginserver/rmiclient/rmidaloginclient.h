
#ifndef RMILOGINCLIENT_H
#define RMILOGINCLIENT_H

#include "rmi/rmibase.h"
#include "servercommon/logindef.h"
#include "servercommon/struct/roleinitparam.h"

class RMILoginClient : public rmi::RMIProxyObject
{
public:
	RMILoginClient(){};
	virtual ~RMILoginClient(){};

	bool GetProfNum(rmi::RMIBackObject *backobj);
	bool GetAccountInfoAsyn(const char *pname, rmi::RMIBackObject *backobj);
	bool UpdateLastLoginTimeAsyn(const char* pname, unsigned int login_time, rmi::RMIBackObject *backobj);

	bool AddRoleAsyn(const char *pname, int db_index, int role_id, rmi::RMIBackObject *backobj);
	bool RemoveRoleAsyn(const char *pname, int db_index, int role_id, rmi::RMIBackObject *backobj);
	bool FrobidAsyn(const char *pname, unsigned int forbid_time, int role_id, rmi::RMIBackObject *backobj);
	bool AddNameInfoAsyn(const char *role_name, const char *pname, long long name_strid, rmi::RMIBackObject *backobj);
	bool UpdateNameInfoAsyn(const char *role_name, int db_index, int role_id, rmi::RMIBackObject *backobj);
	bool DeleteNameInfoAsyn(int db_index, int role_id, const char *pname, rmi::RMIBackObject *backobj);
	bool DeleteNameInfoByNameAsyn(const char *role_name, rmi::RMIBackObject *backobj);
	bool AddIdentityAsyn(const char *pname, const char *name, const char *id, bool check_repeat, rmi::RMIBackObject *backobj);
	bool GetNameMappingCacheAsyn(/*int db_index,*/rmi::RMIBackObject *backobj);			// 获得名字映射表，缓存到NameManager中
	bool GetTotalRoleNumAsyn(rmi::RMIBackObject *backobj);
};

class RMIGetProfNumBackObject : public rmi::RMIBackObject
{
public:
	RMIGetProfNumBackObject(){};
	virtual ~RMIGetProfNumBackObject(){};

	virtual bool __response(TLVUnserializer &out_param);
	virtual void __exception(int error){};
	virtual void __timeout(){};
	virtual void __free();
protected:
	virtual void GetProfNumRet(int ret, int prof1_num, int prof2_num, int prof3_num, int prof4_num);
};

class RMIGetTotalRoleNumBackObject : public rmi::RMIBackObject
{
public:
	RMIGetTotalRoleNumBackObject() {};
	virtual ~RMIGetTotalRoleNumBackObject() {};

	virtual bool __response(TLVUnserializer &out_param);
	virtual void __exception(int error) {};
	virtual void __timeout() {};
	virtual void __free();
protected:
	virtual void GetTotalRoleNumRet(int ret, int total_role_num);
};

class RMIGetAccountInfoBackObject : public rmi::RMIBackObject
{
public:
	RMIGetAccountInfoBackObject(){};
	virtual ~RMIGetAccountInfoBackObject(){};

	virtual bool __response(TLVUnserializer &in_param);
	virtual void __exception(int error){};
	virtual void __timeout(){};
	virtual void __free();
protected:
	virtual void GetAccountInfoRet(int ret, const AccountInfo* account_info);
};

class RMIAddRoleBackObject : public rmi::RMIBackObject
{
public:
	RMIAddRoleBackObject(){};
	virtual ~RMIAddRoleBackObject(){};

	virtual bool __response(TLVUnserializer &out_param);
	virtual void __exception(int error){ Error(); }
	virtual void __timeout(){ Error(); }
	virtual void __free();
protected:
	virtual void AddRoleRet(int result);
	virtual void Error(){}
};

class RMIRemoveRoleBackObject : public rmi::RMIBackObject
{
public:
	RMIRemoveRoleBackObject(){};
	virtual ~RMIRemoveRoleBackObject(){};

	virtual bool __response(TLVUnserializer &out_param);
	virtual void __exception(int error){};
	virtual void __timeout(){};
	virtual void __free();
protected:
	virtual void RemoveRoleRet(int result);
};

class RMIPassAntiWallowBackObject : public rmi::RMIBackObject
{
public:
	RMIPassAntiWallowBackObject(){};
	virtual ~RMIPassAntiWallowBackObject(){};

	virtual bool __response(TLVUnserializer &out_param);
	virtual void __exception(int error){};
	virtual void __timeout(){};
	virtual void __free();
protected:
	virtual void PassAntiWallowRet(int ret);
};

class RMIForbidBackObject : public rmi::RMIBackObject
{
public:
	RMIForbidBackObject(){};
	virtual ~RMIForbidBackObject(){};

	virtual bool __response(TLVUnserializer &out_param);
	virtual void __exception(int error){};
	virtual void __timeout(){};
	virtual void __free();
protected:
	virtual void ForbidRet(int result, int db_index, int role_1, int role_2, int role_3, int role_4, int role_5);
};

class RMIAddNameInfoBackObject : public rmi::RMIBackObject
{
public:
	RMIAddNameInfoBackObject(){};
	virtual ~RMIAddNameInfoBackObject(){};

	virtual bool __response(TLVUnserializer &out_param);
	virtual void __exception(int error){};
	virtual void __timeout(){};
	virtual void __free();
protected:
	virtual void AddNameInfoRet(int result);
};

class RMIUpdateNameInfoBackObject : public rmi::RMIBackObject
{
public:
	RMIUpdateNameInfoBackObject(){};
	virtual ~RMIUpdateNameInfoBackObject(){};

	virtual bool __response(TLVUnserializer &out_param);
	virtual void __exception(int error){};
	virtual void __timeout(){};
	virtual void __free();
protected:
	virtual void UpdateNameInfoRet(int result);
};

class RMIDeleteNameInfoBackObject : public rmi::RMIBackObject
{
public:
	RMIDeleteNameInfoBackObject(){};
	virtual ~RMIDeleteNameInfoBackObject(){};

	virtual bool __response(TLVUnserializer &out_param);
	virtual void __exception(int error){};
	virtual void __timeout(){};
	virtual void __free();
protected:
	virtual void DeleteNameInfoRet(int result);
};

class RMIDeleteNameInfoByNameBackObject : public rmi::RMIBackObject
{
public:
	RMIDeleteNameInfoByNameBackObject(){};
	virtual ~RMIDeleteNameInfoByNameBackObject(){};

	virtual bool __response(TLVUnserializer &out_param);
	virtual void __exception(int error){};
	virtual void __timeout(){};
	virtual void __free();
protected:
	virtual void DeleteNameInfoByNameRet(int result);
};

class RMIUpdateLastLoginTimeBackObject : public rmi::RMIBackObject
{
public:
	RMIUpdateLastLoginTimeBackObject() {};
	virtual ~RMIUpdateLastLoginTimeBackObject() {};

	virtual bool __response(TLVUnserializer &out_param);
	virtual void __exception(int error) {};
	virtual void __timeout() {};
	virtual void __free();

protected:
	virtual void UpdateLastLoginTimeRet(int ret);
};

#endif
