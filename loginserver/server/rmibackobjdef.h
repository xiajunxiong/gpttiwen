
#ifndef RMIBACKOBJDEF_H
#define RMIBACKOBJDEF_H

#include "rmiclient/rmidaloginclient.h"
#include "rmiclient/rmidaroleclient.h"
#include "servercommon/darmi/rmisystemclient.h"
#include "servercommon/logindef.h"
#include "inetworkmodule.h"

class LoginServer;

class RMIGetProfBackObjectImpl : public RMIGetProfNumBackObject
{
public:
	RMIGetProfBackObjectImpl()
	{

	}

	void *	operator new(size_t c);
	void	operator delete(void *m);

protected:
	virtual void GetProfNumRet(int ret, int prof1_num, int prof2_num, int prof3_num, int prof4_num);
};

class RMIGetTotalRoleNumBackObjectImpl : public RMIGetTotalRoleNumBackObject
{
public:
	RMIGetTotalRoleNumBackObjectImpl()
	{

	}

	void *	operator new(size_t c);
	void	operator delete(void *m);

protected:
	virtual void GetTotalRoleNumRet(int ret, int total_role_num);
};

class RMILoginToAccountBackObjectImpl : public RMIGetAccountInfoBackObject
{
public:
	RMILoginToAccountBackObjectImpl() : netid(-1), ip(0)
	{
		memset(pname, 0, sizeof(pname));
		memset(original_pname, 0, sizeof(original_pname));
	}

	virtual void __exception(int error);
	virtual void __timeout();

	NetID netid;
	PlatName pname;
	PlatName original_pname;
	IP ip;

	void *	operator new(size_t c);
	void	operator delete(void *m);

protected:
	virtual void GetAccountInfoRet(int ret, const AccountInfo* account_info);
};

class RMIAddRoleBackObjectImpl : public RMIAddRoleBackObject
{
public:
	RMIAddRoleBackObjectImpl() : netid(-1), avatar(0), sex(0), prof(0), level(0), plat_spid(0)
	{
		memset(pname, 0, sizeof(pname));
		memset(role_name, 0, sizeof(role_name));
	}

	NetID netid;
	UserID user_id;
	PlatName pname;
	GameName role_name;
	char avatar;
	char sex;
	char prof;
	unsigned short level;
	int plat_spid;
	long long create_role_time;

	void *	operator new(size_t c);
	void	operator delete(void *m);

protected:
	virtual void AddRoleRet(int result);
	virtual void Error();
};

class RMIRemoveRoleBackObjectImpl : public RMIRemoveRoleBackObject
{
public:
	RMIRemoveRoleBackObjectImpl() : netid(-1)
	{
		memset(pname, 0, sizeof(pname));
	}

	UserID user_id;
	int netid;
	PlatName pname;

	void *	operator new(size_t c);
	void	operator delete(void *m);

protected:
	virtual void RemoveRoleRet(int result);
};

class RMIPassAntiWallowBackObjectImpl : public RMIPassAntiWallowBackObject
{
public:
	void *	operator new(size_t c);
	void	operator delete(void *m);

protected:
	virtual void PassAntiWallowRet(int ret);
};

class RMIForbidBackObjectImpl : public RMIForbidBackObject
{
public:
	RMIForbidBackObjectImpl() : login_server(NULL), cmd_id(0)
	{
		memset(pname, 0, sizeof(pname));
	}

	void *	operator new(size_t c);
	void	operator delete(void *m);

	LoginServer *login_server;
	PlatName pname;

	int cmd_id;
	std::string creator;
	std::string cmd;
	int forbid_time_s;

protected:
	virtual void ForbidRet(int result, int db_index, int role_1, int role_2, int role_3, int role_4, int role_5);
};

class RMIDeleteNameInfoBackObjectImpl : public RMIDeleteNameInfoBackObject
{
public:
	void *	operator new(size_t c);
	void	operator delete(void *m);

protected:
	virtual void DeleteNameInfoRet(int result);
};

class RMIGetRoleInfoBackObjectImpl : public RMIGetRoleInfoBackObject
{
public:
	RMIGetRoleInfoBackObjectImpl() : db_index(0), netid(-1), result(0)
	{
		memset(pname, 0, sizeof(pname));
		memset(original_pname, 0, sizeof(original_pname));
	}

	int db_index;
	NetID netid;
	PlatName pname;
	PlatName original_pname;
	int result;

	void *	operator new(size_t c);
	void	operator delete(void *m);

protected:
	virtual void GetRoleInfoRet(int ret, const RoleInfoList &role_info);
};

class LoginServer;

class RMILoginToRoleBackObjectImpl : public RMIGetAccountInfoBackObject
{
public:
	RMILoginToRoleBackObjectImpl() : game_thread(0), ip(0), netid(-1), min_login_interval_time(0)
	{
		memset(pname, 0, sizeof(pname));
	}

	int game_thread;
	UserID user_id;
	IP ip;
	NetID netid;
	PlatName pname;
	int min_login_interval_time;

	void *	operator new(size_t c);
	void	operator delete(void *m);

protected:
	virtual void GetAccountInfoRet(int ret, const AccountInfo* account_info);
};

class RMIGetRoleInfoBackObjectImplRoleReq : public RMIGetRoleInfoBackObject
{
public:
	RMIGetRoleInfoBackObjectImplRoleReq() : ip(0), netid(-1), min_login_interval_time(0)
	{
		memset(pname, 0, sizeof(pname));
	}

	UserID user_id;
	IP ip;
	NetID netid;
	PlatName pname;

	int min_login_interval_time;

	void *	operator new(size_t c);
	void	operator delete(void *m);

protected:
	virtual void GetRoleInfoRet(int ret, const RoleInfoList &role_info);
};

class RMICreateRoleBackObjectImpl : public RMICreateRoleBackObject
{
public:
	RMICreateRoleBackObjectImpl() : db_index(0), netid(-1), avatar_type(AVATAR_TYPE_INVALID), color(0), plat_spid(0), job_id(0)
	{
		memset(pname, 0, sizeof(pname));
		memset(role_name, 0, sizeof(role_name));
	}

	int db_index;
	NetID netid;
	PlatName pname;

	GameName role_name;
	char avatar_type;
	char color;
	int plat_spid;
	long long create_role_time;
	int job_id;

	void *	operator new(size_t c);
	void	operator delete(void *m);

protected:
	virtual void CreateRoleRet(int ret, int role_id, int create_result);
	virtual void Error(int create_result);
};

class RMIDestroyRoleBackObjectImpl : public RMIDestroyRoleBackObject
{
public:
	RMIDestroyRoleBackObjectImpl() : netid(-1)
	{
		memset(pname, 0, sizeof(pname));
	}

	UserID user_id;
	int netid;
	PlatName pname;

	void *	operator new(size_t c);
	void	operator delete(void *m);

protected:
	virtual void DestroyRoleRet(int ret);
};

class RMILoginGetCmdBackObjectImpl : public RMIGetCmdBackObject
{
public:
	RMILoginGetCmdBackObjectImpl() : login_server(NULL) {}

	LoginServer *login_server;

protected:
	virtual void GetCmdRet(int ret, const CmdList &cmd_list);
};

class RMIInitNameStrIdBackObjectImpl : public RMIInitNameStrIdBackObject
{
public:
	RMIInitNameStrIdBackObjectImpl(){};

	std::set<int> name_str_id_list;
protected:
	virtual void InitListRet(std::set<long long> name_str_id_list);
};

class RMIUpdateLastLoginTimeBackObjectImpl : public RMIUpdateLastLoginTimeBackObject
{
public:
	RMIUpdateLastLoginTimeBackObjectImpl() {};

	PlatName pname;
	int role_id;
	unsigned int login_timestamp;
	GameName role_name;
protected:
	virtual void UpdateLastLoginTimeRet(int ret);
};

#endif

