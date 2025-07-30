#include "rolepeaktoken.hpp"

#include "protocol/msgpeaktoken.hpp"

#include "servercommon/serverconfig/crossconfig.hpp"
#include "servercommon/errornum.h"

#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"

RolePeakToken::RolePeakToken()
{
	m_module_mgr = NULL;
}

RolePeakToken::~RolePeakToken()
{
}

void RolePeakToken::Init(RoleModuleManager * module_mgr, const RolePeakTokenParam & param)
{
	m_module_mgr = module_mgr;
	m_param = param;
}

void RolePeakToken::GetInitParam(RolePeakTokenParam * param)
{
	*param = m_param;
}

void RolePeakToken::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	if (old_dayid == now_dayid)
	{
		return;
	}

	m_param.today_use_times = 0;

	this->SendInfo();
}

void RolePeakToken::SendInfo()
{
	Protocol::SCPeaktokenRoleInfo info;
	info.today_use_times = m_param.today_use_times;

	m_module_mgr->NetSend(&info, sizeof(info));
}
