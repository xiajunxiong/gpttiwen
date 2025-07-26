#include "rmidaroleclient.h"
#include "servercommon/darmi/darole.h"

#include "servercommon/servercommon.h"
#include <set>
#include <server/loginlog.h>
USE_DATAACCESS_VAR();
OLD_USE_INTERNAL_NETWORK();
USE_DATAACCESS_ROLE_VAR();

static char TLV_BUFF[160 * 1024];

bool RMILRoleClient::GetRoleInfoAsyn(int *role_id, int num, bool is_load_role_to_cache, rmi::RMIBackObject *backobj)
{
	if (NULL == role_id) return false;

	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	bool ret = s.Push(num) && s.Push(is_load_role_to_cache);
	if (! ret)
	{
		delete backobj;
		return false;
	}

	for (int i = 0; i < num; ++i)
	{
		ret = s.Push(role_id[i]);
		if (! ret)
		{
			delete backobj;
			return false;
		}
	}

	ret = m_session.rmi_module->Call(m_session, dataaccessrmi::ROLE, dataaccessrmi::role::GET_ROLE_INFO, s, backobj, true, 5000);
	if (!ret)
	{
		delete backobj;
	}
	return ret;
}


bool RMIGetRoleInfoBackObject::__response(TLVUnserializer &out_param)
{
	int ret = -1;

	if (out_param.Pop(&ret))
	{
		RoleInfoList info;
		if (ret == 0)
		{
			if (!info.Unserialize(&out_param))
			{
				return false;
			}
		}

		GetRoleInfoRet(ret, info);
		return true;
	}
	return false;
}

void RMIGetRoleInfoBackObject::GetRoleInfoRet(int ret, const RoleInfoList &role_info)
{
	printf("GetRoleInfoRet:%d\n", ret);
}
void RMIGetRoleInfoBackObject::__free()
{
	delete this;
}

bool RMILRoleClient::CreateRoleAsyn(const char* pname, int db_index, long long name_strid, const RoleInitParam &p, rmi::RMIBackObject *backobj)
{
	UNSTD_STATIC_CHECK((sizeof(RoleInitParam)) * 2 < sizeof(TLV_BUFF));

	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));
	
	bool ret = s.Push(pname) && s.Push(db_index) && s.Push(name_strid);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	ret = p.Serialize(s);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	ret = m_session.rmi_module->Call(m_session, dataaccessrmi::ROLE, dataaccessrmi::role::CREATE_ROLE, s, backobj, true, 5000);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	return ret;
}
bool RMICreateRoleBackObject::__response(TLVUnserializer& out_param)
{
	int ret = -1, role_id = 0, create_result = 0;
	bool pop_ret = out_param.Pop(&ret);
	bool pop_role_id = out_param.Pop(&role_id);
	bool pop_create_result = out_param.Pop(&create_result);

	// Debug日志：打印每步Pop的结果和参数值
	loginlog::g_log_login.printf(LL_ERROR,
		"RMICreateRoleBackObject::__response debug: pop_ret=%d, ret=%d; pop_role_id=%d, role_id=%d; pop_create_result=%d, create_result=%d",
		pop_ret, ret, pop_role_id, role_id, pop_create_result, create_result);

	if (pop_ret && pop_role_id && pop_create_result)
	{
		CreateRoleRet(ret, role_id, create_result);
		return true;
	}
	else
	{
		Error(CREATE_ROLE_RESULT_TYPE_SERVER_ERROR);
		printf("RMICreateRoleBackObject::__response pop param error! pop_ret=%d, ret=%d; pop_role_id=%d, role_id=%d; pop_create_result=%d, create_result=%d\n",
			pop_ret, ret, pop_role_id, role_id, pop_create_result, create_result);
		loginlog::g_log_login.printf(LL_ERROR,
			"RMICreateRoleBackObject::__response pop param error! pop_ret=%d, ret=%d; pop_role_id=%d, role_id=%d; pop_create_result=%d, create_result=%d",
			pop_ret, ret, pop_role_id, role_id, pop_create_result, create_result);
		return false;
	}
}
void RMICreateRoleBackObject::CreateRoleRet(int ret, int role_id, int create_result)
{
	printf("CreateRoleRet:%d role_id:%d create_result:%d\n", ret, role_id, create_result);
}
void RMICreateRoleBackObject::__free()
{
	delete this;
}

bool RMILRoleClient::DestroyRoleAsyn(int role_id, rmi::RMIBackObject *backobj)
{
	UNSTD_STATIC_CHECK(sizeof(int) * 2 < sizeof(TLV_BUFF));

	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	bool ret = s.Push(role_id);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	ret = m_session.rmi_module->Call(m_session, dataaccessrmi::ROLE, dataaccessrmi::role::DESTROY_ROLE, s, backobj, true, 5000);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	return ret;
}

bool RMIDestroyRoleBackObject::__response(TLVUnserializer &out_param)
{
	int ret = -1;
	if (out_param.Pop(&ret))
	{
		DestroyRoleRet(ret);
		return true;
	}
	return false;
}
void RMIDestroyRoleBackObject::DestroyRoleRet(int ret)
{
	printf("DestroyRoleRet: %d\n", ret);
}
void RMIDestroyRoleBackObject::__free()
{
	delete this;
}

bool RMIInitNameStrIdBackObject::__response(TLVUnserializer &out_param)
{
	int result;
	bool ret = out_param.Pop(&result);
	if (!ret)
	{
		return false;
	}

	if (result)
	{
		return false;
	}
	
	int size = 0;
	ret = out_param.Pop(&size);

	std::set<long long> str_id_list;
	long long str_id = 0;
	if (0 != size)
	{
		for (int i = 0;i < size; ++ i)
		{
			if (!ret)
			{
				return false;
			}

			ret = ret && out_param.Pop(&str_id);
			str_id_list.insert(str_id);
		}
	}

	InitList(str_id_list);
	return true;
}

void RMIInitNameStrIdBackObject::__free()
{
	delete this;
}