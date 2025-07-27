#include "rmisystemclient.h"
#include "servercommon/darmi/dasystem.h"

#include "servercommon/servercommon.h"
#include "servercommon/struct/system/cmdresult.h"

USE_DATAACCESS_VAR();
OLD_USE_INTERNAL_NETWORK();

static char TLV_BUFF[65536];

bool RMISystemClient::GetPublicNoticeAsyn(rmi::RMIBackObject *backobj)
{
	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	bool ret = m_session.rmi_module->Call(m_session, dataaccessrmi::SYSTEM, dataaccessrmi::system::GET_PUBLIC_NOTICE, s, backobj, false, 5000);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	return ret;
}

bool RMISystemClient::GetCmd(char type, rmi::RMIBackObject *backobj, unsigned int query_timestamp)
{
	if (NULL == backobj) return false;

	UNSTD_STATIC_CHECK(sizeof(char) * 2 < sizeof(TLV_BUFF));

	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	if (!s.Push(type))
	{
		delete backobj;
		return false;
	}

	if (!s.Push(query_timestamp))
	{
		delete backobj;
		return false;
	}
	
	bool ret = true;
	m_session.rmi_module->Call(m_session, dataaccessrmi::SYSTEM, dataaccessrmi::system::GET_CMD, s, backobj, false, 8000);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	return ret;
}

bool RMISystemClient::WriteTimeoutLog(char type, rmi::RMIBackObject* backobj, unsigned int query_timestamp)
{
	if (NULL == backobj) return false;

	UNSTD_STATIC_CHECK(sizeof(char) * 2 < sizeof(TLV_BUFF));

	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	if (!s.Push(type))
	{
		delete backobj;
		return false;
	}

	if (!s.Push(query_timestamp))
	{
		delete backobj;
		return false;
	}

	bool ret = true;
	m_session.rmi_module->Call(m_session, dataaccessrmi::SYSTEM, dataaccessrmi::system::WRITE_TIME_OUT_LOG, s, backobj, false, 8000);
	if (!ret)
	{
		delete backobj;
		return false;
	}
	
	return ret;
}

bool RMISystemClient::WriteCmdResult(char type, rmi::RMIBackObject* backobj, CmdResult* cmd_result)
{
	if (NULL == backobj) return false;

	UNSTD_STATIC_CHECK(sizeof(char) * 2 < sizeof(TLV_BUFF));

	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

#ifdef _DEBUG
	//for (int i = 0; i < cmd_result->count; ++i)
	//{
	//	printf("RMI WriteCmdResult cmd_id[%d] opresult[%s]\n",
	//		cmd_result->result_list[i].cmd_id, cmd_result->result_list[i].opresult);
	//}
#endif

	if (!cmd_result->Serialize(s))
	{
		delete backobj;
		return false;
	}

	bool ret = true;
	m_session.rmi_module->Call(m_session, dataaccessrmi::SYSTEM, dataaccessrmi::system::WRITE_CMD_RESULT, s, backobj, false, 8000);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	return ret;
}

bool RMISystemClient::AddDataaccessAllowIPPrefix(rmi::RMIBackObject* backobj, const std::string& new_ip_prefix)
{
	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	if (!s.Push(new_ip_prefix))
	{
		return false;
	}

	bool ret = m_session.rmi_module->Call(m_session, dataaccessrmi::SYSTEM, dataaccessrmi::system::ADD_DATAACCESS_ALLOW_IP_PREFIX, s, backobj, false, 8000);
	return ret;
}

bool RMIGetPublicNoticeBackObject::__response(TLVUnserializer &in_param)
{
	int ret = -1;

	if (!in_param.Pop(&ret))
	{
		return false;
	}

	static PublicNoticeList notice_list;
	notice_list.count = 0; 

	if (0 == ret)
	{
		if (!notice_list.Unserialize(in_param))
		{
			return false;
		}
	}

	this->GetPublicNoticeRet(ret, notice_list);

	return true;
}

void RMIGetPublicNoticeBackObject::GetPublicNoticeRet(int ret, const PublicNoticeList &notice_list)
{
	printf("GetPublicNoticeRet:%d\n", ret);
}

void RMIGetPublicNoticeBackObject::__free()
{
	delete this;
}

bool RMIGetCmdBackObject::__response(TLVUnserializer &in_param)
{
	int ret = -1;

	if (!in_param.Pop(&ret))
	{
		return false;
	}

	static CmdList cmd_list;
	cmd_list.count = 0;

	if (0 == ret)
	{
		if (!cmd_list.Unserialize(in_param))
		{
			return false;
		}
	}

	this->GetCmdRet(ret, cmd_list);

	return true;
}

void RMIGetCmdBackObject::GetCmdRet(int ret, const CmdList &cmd_list)
{
	printf("GetCmdRet:%d\n", ret);
}

void RMIGetCmdBackObject::__free()
{
	delete this;
}

void RMIGetCmdBackObject::__exception(int error)
{
	printf("GetCmdRet __exception: %d !!!\n", error);
}

void RMIGetCmdBackObject::__timeout()
{
	printf("GetCmdRet __timeout!!!");
}

bool RMIWriteCmdTimeOutLogBackObject::__response(TLVUnserializer &in_param)
{
	int ret = -1;

	if (!in_param.Pop(&ret))
	{
		return false;
	}

	this->WriteLogRet(ret);
	return true;
}

void RMIWriteCmdTimeOutLogBackObject::__exception(int error)
{
	printf("RMIWriteCmdTimeOutLogBackObject::__exception error:%d\n", error);
}

void RMIWriteCmdTimeOutLogBackObject::__timeout()
{
	printf("RMIWriteCmdTimeOutLogBackObject::__timeout \n");
}

void RMIWriteCmdTimeOutLogBackObject::__free()
{
	delete this;
}

void RMIWriteCmdTimeOutLogBackObject::WriteLogRet(int ret)
{
	printf("RMIWriteCmdTimeOutLogBackObject::WriteLogRet ret:%d\n", ret);
}

bool RMIWriteCmdResultBackObject::__response(TLVUnserializer &in_param)
{
	int ret = -1;
	if (!in_param.Pop(&ret))
	{
		return false;
	}
	this->WriteResultRet(ret);
	return true;
}

void RMIWriteCmdResultBackObject::__exception(int error)
{
	printf("RMIWriteCmdResultBackObject::__exception error[%d]\n", error);
}

void RMIWriteCmdResultBackObject::__timeout()
{
	printf("RMIWriteCmdResultBackObject::__timeout \n");
}

void RMIWriteCmdResultBackObject::__free()
{
	delete this;
}

void RMIWriteCmdResultBackObject::WriteResultRet(int ret)
{
	printf("RMIWriteCmdResultBackObject::WriteResultRet ret[%d]\n", ret);
}

bool RMIAddDataaccessAllowIPPrefixBackObject::__response(TLVUnserializer &in_param)
{
	int result = -1;
	if (!in_param.Pop(&result))
	{
		return false;
	}

	this->OnReloadRet(result);

	return true;
}

void RMIAddDataaccessAllowIPPrefixBackObject::__exception(int error)
{
	printf("RMIAddDataaccessAllowIPPrefixBackObject::__exception exception[%d]\n", error);
}

void RMIAddDataaccessAllowIPPrefixBackObject::__timeout()
{
	printf("RMIAddDataaccessAllowIPPrefixBackObject::__timeout\n");
}

void RMIAddDataaccessAllowIPPrefixBackObject::__free()
{
	delete this;
}

void RMIAddDataaccessAllowIPPrefixBackObject::OnReloadRet(int ret)
{
	printf("RMIAddDataaccessAllowIPPrefixBackObject::OnReloadRet ret[%d]\n", ret);
}
