#include "rmihidden.h"
#include "gamelog.h"
#include "servercommon/servercommon.h"
#include "engineadapter.h"
#include "servercommon/darmi/damodule.h"
#include "servercommon/darmi/darole.h"

USE_DATAACCESS_VAR();
USE_DATAACCESS_ROLE_VAR();

static char TLV_BUFF[4 * 1024 * 1024];

bool RMIHiddenClient::GetRoleInfoAsyn(int *role_id, int num, bool is_load_role_to_cache, rmi::RMIBackObject *backobj)
{
	if (NULL == role_id) return false;

	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	bool ret = s.Push(num) && s.Push(is_load_role_to_cache);
	if (!ret)
	{
		delete backobj;
		return false;
	}
	for (int i = 0; i < num; ++i)
	{
		ret = s.Push(role_id[i]);
		if (!ret)
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

void RMIGetRoleInfoBackObject::__exception(int error)
{
	printf("RMIGetRoleInfoBackObject::__exception error[%d]\n", error);
}

void RMIGetRoleInfoBackObject::__timeout()
{
	printf("RMIGetRoleInfoBackObject::__timeout\n");
}

void RMIGetRoleInfoBackObject::__free()
{
	delete this;
}