#ifndef __RMISYSTEMCLIENT_H__
#define __RMISYSTEMCLIENT_H__

#include "rmi/rmibase.h"
#include "servercommon/struct/system/publicnoticelist.h"
#include "servercommon/struct/system/cmdlist.h"
#include "servercommon/struct/global/trademarketparam.hpp"

class CmdResult;
class RMISystemClient : public rmi::RMIProxyObject
{
public:
	RMISystemClient(){};
	virtual ~RMISystemClient(){};

	bool GetPublicNoticeAsyn(rmi::RMIBackObject *backobj);
	
	// 从后台数据库取得的命令的类型 其值对应command表的type字段
	enum CMD_TYPE
	{
		CMD_NONE = 0,
		CMD_TYPE_LOGIN,						// 登陆服务器处理的命令
		CMD_TYPE_GLOBAL,					// 全局服务器处理的命令
	};

	bool GetCmd(char type, rmi::RMIBackObject *backobj, unsigned int query_timestamp);
	bool WriteTimeoutLog(char type, rmi::RMIBackObject* backobj, unsigned int query_timestamp);
	bool WriteCmdResult(char type, rmi::RMIBackObject* backobj, CmdResult* cmd_result);
	bool AddDataaccessAllowIPPrefix(rmi::RMIBackObject* backobj, const std::string& new_ip_prefix);
};

class RMIGetPublicNoticeBackObject : public rmi::RMIBackObject
{
public:
	RMIGetPublicNoticeBackObject() {}
	virtual ~RMIGetPublicNoticeBackObject() {}

	virtual bool __response(TLVUnserializer &in_param);
	virtual void __exception(int error){};
	virtual void __timeout(){};
	virtual void __free();

protected:
	virtual void GetPublicNoticeRet(int ret, const PublicNoticeList &notice_list);
};

class RMIGetCmdBackObject : public rmi::RMIBackObject
{
public:
	RMIGetCmdBackObject(){};
	virtual ~RMIGetCmdBackObject(){};

	virtual bool __response(TLVUnserializer &in_param);
	virtual void __exception(int error);
	virtual void __timeout();
	virtual void __free();
protected:
	virtual void GetCmdRet(int ret, const CmdList &cmd_list);
};

class RMIWriteCmdTimeOutLogBackObject : public rmi::RMIBackObject
{
public:
	RMIWriteCmdTimeOutLogBackObject() {};
	virtual ~RMIWriteCmdTimeOutLogBackObject() {};

	virtual bool __response(TLVUnserializer &in_param);
	virtual void __exception(int error);
	virtual void __timeout();
	virtual void __free();

protected:
	virtual void WriteLogRet(int ret);
};

class RMIAddDataaccessAllowIPPrefixBackObject : public rmi::RMIBackObject
{
public:
	RMIAddDataaccessAllowIPPrefixBackObject() {};
	virtual ~RMIAddDataaccessAllowIPPrefixBackObject() {};

	virtual bool __response(TLVUnserializer &in_param);
	virtual void __exception(int error);
	virtual void __timeout();
	virtual void __free();

protected:
	virtual void OnReloadRet(int ret);
};

class RMIWriteCmdResultBackObject : public rmi::RMIBackObject
{
public:
	RMIWriteCmdResultBackObject() {};
	virtual ~RMIWriteCmdResultBackObject() {};

	virtual bool __response(TLVUnserializer &in_param);
	virtual void __exception(int error);
	virtual void __timeout();
	virtual void __free();

protected:
	virtual void WriteResultRet(int ret);
};

class RMIGetCmdBackObjectImpl : public RMIGetCmdBackObject
{
public:
	unsigned int query_timestamp; // 记录本次请求的时间戳，并记录到command表中，如触发__timeout()或__exception()，则再次向dataaccess请求RMIWriteCmdTimeOutLogBackObjectImpl，并在dataaccess记录log

protected:
	virtual void GetCmdRet(int ret, const CmdList &cmd_list);
	virtual void __exception(int error);
	virtual void __timeout();
};

class RMIWriteCmdTimeOutLogBackObjectImpl : public RMIWriteCmdTimeOutLogBackObject
{
public:
	unsigned int query_timestamp;

protected:
	virtual void WriteLogRet(int ret);
	virtual void __exception(int error);
	virtual void __timeout();
};

class RMIAddDataaccessAllowIPPrefixBackObjImpl : public RMIAddDataaccessAllowIPPrefixBackObject
{
public:
	RMIAddDataaccessAllowIPPrefixBackObjImpl() {}
	~RMIAddDataaccessAllowIPPrefixBackObjImpl() {}
};

class RMIWriteCmdResultBackObjectImpl : public RMIWriteCmdResultBackObject
{
public:
	void *	operator new(size_t c);
	void	operator delete(void *m);

protected:
	virtual void WriteResultRet(int ret);
	virtual void __exception(int error);
	virtual void __timeout();
};

#endif // __RMISYSTEMCLIENT_H__



