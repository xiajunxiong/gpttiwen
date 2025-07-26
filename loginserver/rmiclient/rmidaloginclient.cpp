#include "rmidaloginclient.h"

#include "servercommon/darmi/dalogin.h"
#include "servercommon/darmi/darole.h"

#include "servercommon/servercommon.h"
#include "loginserver/randnamemanager/randnamemanager.hpp"
#include "loginserver/server/loginlog.h"

USE_DATAACCESS_VAR();
OLD_USE_INTERNAL_NETWORK();
USE_DATAACCESS_ROLE_VAR();
USE_DATAACCESS_LOGIN_VAR();

static char TLV_BUFF[4 * 1024];

bool RMILoginClient::GetProfNum(rmi::RMIBackObject *backobj)
{
	if (NULL == backobj)
	{
		return false;
	}

	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	bool ret = m_session.rmi_module->Call(m_session, dataaccessrmi::LOGIN, dataaccessrmi::login::GET_PROF_NUM, s, backobj, true, 5000);
	if (!ret)
	{
		delete backobj;
	}

	return ret;
}

bool RMILoginClient::GetAccountInfoAsyn(const char *pname, rmi::RMIBackObject *backobj)
{
	if (NULL == pname) return false;

	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	bool ret = s.Push(pname);
	if (ret)
	{
		ret = m_session.rmi_module->Call(m_session, dataaccessrmi::LOGIN, dataaccessrmi::login::GET_ACCOUNT_INFO, s, backobj, true, 5000);
	}
	if (!ret)
	{
		delete backobj;
	}
	return ret;
}

bool RMIGetProfNumBackObject::__response(TLVUnserializer &out_param)
{
	int ret = -1, prof1_num = 0, prof2_num = 0, prof3_num = 0, prof4_num = 0;

	if (out_param.Pop(&ret))
	{
		if (0 == ret && out_param.Pop(&prof1_num) && out_param.Pop(&prof2_num) && out_param.Pop(&prof3_num) && out_param.Pop(&prof4_num))
		{
			this->GetProfNumRet(ret, prof1_num, prof2_num, prof3_num, prof4_num);
		}
		else
		{
			this->GetProfNumRet(ret, 0, 0, 0, 0);
		}

		return true;
	}

	return false;
}
void RMIGetProfNumBackObject::GetProfNumRet(int ret, int prof1_num, int prof2_num, int prof3_num, int prof4_num)
{
	printf("GetProfNumRet:%d\n", ret);
}

void RMIGetProfNumBackObject::__free()
{
	delete this;
}

bool RMIGetAccountInfoBackObject::__response(TLVUnserializer &in_param)
{
	int ret = -1;

	if (in_param.Pop(&ret))
	{
		AccountInfo tmp_account_info;
		if (ret == 0 && tmp_account_info.Unserialize(in_param))
		{
			GetAccountInfoRet(ret, &tmp_account_info);
		}
		else
		{
			GetAccountInfoRet(ret, NULL);
		}

		return true;
	}

	return false;
}
void RMIGetAccountInfoBackObject::GetAccountInfoRet(int ret, const AccountInfo* account_info)
{
	printf("UserLoginRet:%d\n", ret);
}

void RMIGetAccountInfoBackObject::__free()
{
	delete this;
}

bool RMILoginClient::AddRoleAsyn(const char *pname, int db_index, int role_id, rmi::RMIBackObject *backobj)
{
	if (NULL == pname) return false;

	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	bool ret = s.Pushf("sii", pname, db_index, role_id);
	if (ret)
	{
		ret = m_session.rmi_module->Call(m_session, dataaccessrmi::LOGIN, dataaccessrmi::login::ADD_ROLE, s, backobj, true, 5000);
	}
	if (!ret)
	{
		delete backobj;
	}
	return ret;
}
bool RMIAddRoleBackObject::__response(TLVUnserializer &out_param)
{
	int ret = -1;
	if (!out_param.Pop(&ret))
	{
		Error();
		return false;
	}
	AddRoleRet(ret);
	return true;
}
void RMIAddRoleBackObject::AddRoleRet(int result)
{
	printf("AddRoleRet:%d\n", result);
}
void RMIAddRoleBackObject::__free()
{
	delete this;
}

bool RMILoginClient::RemoveRoleAsyn(const char *pname, int db_index, int role_id, rmi::RMIBackObject *backobj)
{
	if (NULL == pname) return false;

	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	bool ret = s.Pushf("sii", pname, db_index, role_id);
	if (ret)
	{
		ret = m_session.rmi_module->Call(m_session, dataaccessrmi::LOGIN, dataaccessrmi::login::REMOVE_ROLE, s, backobj, true, 5000);
	}
	if (!ret)
	{
		delete backobj;
	}
	return ret;
}
bool RMIRemoveRoleBackObject::__response(TLVUnserializer &out_param)
{
	int ret = -1;
	if (!out_param.Pop(&ret))
	{
		return false;
	}
	RemoveRoleRet(ret);
	return true;
}
void RMIRemoveRoleBackObject::RemoveRoleRet(int result)
{
	printf("RemoveRoleRet:%d\n", result);
}
void RMIRemoveRoleBackObject::__free()
{
	delete this;
}

bool RMIPassAntiWallowBackObject::__response(TLVUnserializer &out_param)
{
	int ret = -1;
	if (!out_param.Pop(&ret))
	{
		return false;
	}

	PassAntiWallowRet(ret);

	return true;
}

void RMIPassAntiWallowBackObject::PassAntiWallowRet(int ret)
{
	printf("PassAntiWallowRet:%d\n", ret);
}

void RMIPassAntiWallowBackObject::__free()
{
	delete this;
}

bool RMILoginClient::FrobidAsyn(const char *pname, unsigned int forbid_time, int role_id, rmi::RMIBackObject *backobj)
{
	if (NULL == pname) return false;

	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	bool ret = s.Push(pname) && s.Push(forbid_time) && s.Push(role_id);
	if (ret)
	{
		ret = m_session.rmi_module->Call(m_session, dataaccessrmi::LOGIN, dataaccessrmi::login::FORBID, s, backobj, true, 5000);
	}
	if (!ret)
	{
		delete backobj;
	}
	return ret;
}

bool RMIForbidBackObject::__response(TLVUnserializer &out_param)
{
	int ret = -1;
	int db_index = 0, role_1 = 0, role_2 = 0, role_3 = 0, role_4 = 0, role_5 = 0;
	if (!out_param.Pop(&ret) || !out_param.Pop(&db_index) || !out_param.Pop(&role_1) || 
		!out_param.Pop(&role_2) || !out_param.Pop(&role_3) || !out_param.Pop(&role_4) ||
		!out_param.Pop(&role_5))
	{
		return false;
	}
	ForbidRet(ret, db_index, role_1, role_2, role_3, role_4, role_5);

	return true;
}

void RMIForbidBackObject::ForbidRet(int result, int db_index, int role_1, int role_2, int role_3, int role_4, int role_5)
{
	printf("ForbidRet:%d\n", result);
}
void RMIForbidBackObject::__free()
{
	delete this;
}

bool RMILoginClient::AddNameInfoAsyn(const char *role_name, const char *pname, long long name_strid, rmi::RMIBackObject *backobj)
{
	if (NULL == role_name || NULL == pname) return false;

	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	bool ret = s.Push(role_name) && s.Push(pname) && s.Push(name_strid);
	if (ret)
	{
		ret = m_session.rmi_module->Call(m_session, dataaccessrmi::LOGIN, dataaccessrmi::login::ADD_NAME_INFO, s, backobj, true, 5000);
	}
	if (!ret)
	{
		delete backobj;
	}

	if (name_strid != 0)
	{
		if (!RandNameManager::Instance().AddNameStrId(name_strid))
		{
			loginlog::g_log_create.printf(LL_ERROR,"[RMILoginClient::AddNameInfoAsyn Error] role_name: %s, pname: %s, name_strid: %lld",
				role_name, pname, name_strid);
			return ret;
		}
	}

	return ret;
}
bool RMIAddNameInfoBackObject::__response(TLVUnserializer &out_param)
{
	int ret = -1;
	if (!out_param.Pop(&ret))
	{
		return false;
	}
	AddNameInfoRet(ret);
	return true;
}
void RMIAddNameInfoBackObject::AddNameInfoRet(int result)
{
	printf("AddNameInfoRet:%d\n", result);
}
void RMIAddNameInfoBackObject::__free()
{
	delete this;
}

bool RMILoginClient::UpdateNameInfoAsyn(const char *role_name, int db_index, int role_id, rmi::RMIBackObject *backobj)
{
	if (NULL == role_name) return false;

	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	bool ret = s.Pushf("sii", role_name, db_index, role_id);
	if (ret)
	{
		ret = m_session.rmi_module->Call(m_session, dataaccessrmi::LOGIN, dataaccessrmi::login::UPDATE_NAME_INFO, s, backobj, true, 5000);
	}
	if (!ret)
	{
		delete backobj;
	}
	return ret;
}
bool RMIUpdateNameInfoBackObject::__response(TLVUnserializer &out_param)
{
	int ret = -1;
	if (!out_param.Pop(&ret))
	{
		return false;
	}
	UpdateNameInfoRet(ret);
	return true;
}
void RMIUpdateNameInfoBackObject::UpdateNameInfoRet(int result)
{
	printf("UpdateNameInfoRet:%d\n", result);
}
void RMIUpdateNameInfoBackObject::__free()
{
	delete this;
}

bool RMILoginClient::DeleteNameInfoAsyn(int db_index, int role_id, const char *pname, rmi::RMIBackObject *backobj)
{
	if (NULL == pname) return false;

	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	bool ret = s.Push(db_index) && s.Push(role_id) && s.Push(pname);
	if (ret)
	{
		ret = m_session.rmi_module->Call(m_session, dataaccessrmi::LOGIN, dataaccessrmi::login::DELETE_NAME_INFO, s, backobj, true, 5000);
	}
	if (!ret)
	{
		delete backobj;
	}
	return ret;
}
bool RMIDeleteNameInfoBackObject::__response(TLVUnserializer &out_param)
{
	int ret = -1;
	if (!out_param.Pop(&ret))
	{
		return false;
	}
	DeleteNameInfoRet(ret);
	return true;
}
void RMIDeleteNameInfoBackObject::DeleteNameInfoRet(int result)
{
	printf("DeleteNameInfoRet:%d\n", result);
}
void RMIDeleteNameInfoBackObject::__free()
{
	delete this;
}
bool RMILoginClient::DeleteNameInfoByNameAsyn(const char *role_name, rmi::RMIBackObject *backobj)
{
	if (NULL == role_name) return false;

	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	bool ret = s.Push(role_name);
	if (ret)
	{
		ret = m_session.rmi_module->Call(m_session, dataaccessrmi::LOGIN, dataaccessrmi::login::DELETE_NAME_INFO_BY_NAME, s, backobj, true, 5000);
	}
	if (!ret)
	{
		delete backobj;
	}
	return ret;
}

bool RMIDeleteNameInfoByNameBackObject::__response(TLVUnserializer &out_param)
{
	int ret = -1;
	if (!out_param.Pop(&ret))
	{
		return false;
	}
	DeleteNameInfoByNameRet(ret);
	return true;
}
void RMIDeleteNameInfoByNameBackObject::__free()
{
	delete this;
}
void RMIDeleteNameInfoByNameBackObject::DeleteNameInfoByNameRet(int result)
{
	printf("DeleteNameInfoByNameRet:%d\n", result);
}

// 用于通知DataAccess获取NameCacheList
bool RMILoginClient::GetNameMappingCacheAsyn(rmi::RMIBackObject *backobj)
{
	if (NULL == backobj) return false;

	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));
	bool ret = true;/*s.Push(db_index);*/
	if (ret)
	{
		ret = m_session.rmi_module->Call(m_session, dataaccessrmi::LOGIN, dataaccessrmi::login::GET_NAME_CACHE, s, backobj, true, 5000);
	}
	if (!ret)
	{
		delete backobj;
	}
	return ret;
}

bool RMILoginClient::GetTotalRoleNumAsyn(rmi::RMIBackObject *backobj)
{
	if (NULL == backobj)
	{
		return false;
	}

	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	bool ret = m_session.rmi_module->Call(m_session, dataaccessrmi::LOGIN, dataaccessrmi::login::GET_TOTAL_ROLE_NUM, s, backobj, true, 5000);
	if (!ret)
	{
		delete backobj;
	}

	return ret;
}

bool RMIGetTotalRoleNumBackObject::__response(TLVUnserializer &out_param)
{
	int ret = 0;
	int total_role_num = 0;
	if (!out_param.Pop(&ret))
	{
		return false;
	}

	if (!out_param.Pop(&total_role_num))
	{
		return false;
	}

	this->GetTotalRoleNumRet(ret, total_role_num);

	return true;
}

void RMIGetTotalRoleNumBackObject::__free()
{
	delete this;
}

void RMIGetTotalRoleNumBackObject::GetTotalRoleNumRet(int ret, int total_role_num)
{
	printf("RMIGetRoleNumBackObject::GetRoleNumRet ret[%d] total_role_num[%d]", ret, total_role_num);
}

bool RMILoginClient::UpdateLastLoginTimeAsyn(const char* pname, unsigned int login_time, rmi::RMIBackObject *backobj)
{
	if (NULL == pname || NULL == backobj) return false;

	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	if (!s.Push(pname) || !s.Push(login_time))
	{
		delete backobj;
		return false;
	}

	bool ret = m_session.rmi_module->Call(m_session, dataaccessrmi::LOGIN, dataaccessrmi::login::UPDATE_LAST_LOGIN_TIME, s, backobj, true, 5000);
	if (!ret)
	{
		delete backobj;
	}

	return ret;
}

bool RMIUpdateLastLoginTimeBackObject::__response(TLVUnserializer &out_param)
{
	int ret = -1;
	if (out_param.Pop(&ret))
	{
		UpdateLastLoginTimeRet(ret);
		return true;
	}
	return false;
}

void RMIUpdateLastLoginTimeBackObject::__free()
{
	delete this;
}

void RMIUpdateLastLoginTimeBackObject::UpdateLastLoginTimeRet(int ret)
{
	printf("RMIUpdateLastLoginTimeBackObject::UpdateLastLoginTimeRet ret[%d]\n", ret);
}
