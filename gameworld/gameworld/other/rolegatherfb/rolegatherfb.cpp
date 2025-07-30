#include "config/logicconfigmanager.hpp"
#include "other/rolemodulemanager.hpp"
#include "obj/character/role.h"
#include "engineadapter.h"
#include "servercommon/string/gameworldstr.h"
#include "world.h"

#include "other/rolegatherfb/gatherfbconfig.hpp"
#include "protocol/msggatherfb.h"
#include "rolegatherfb.hpp"

RoleGatherFb::RoleGatherFb() :m_role_module_mgr(NULL)
{
}

RoleGatherFb::~RoleGatherFb()
{
}

void RoleGatherFb::Init(RoleModuleManager * mgr, const RoleGatherFbParam & param)
{
	m_role_module_mgr = mgr;
	m_param = param;

	m_param.gather_power = GetMin(m_param.gather_power, LOGIC_CONFIG->GetRoleGatherFbConfig()->GetOtherCfg().gather_power_max);
}

void RoleGatherFb::GetInitParam(RoleGatherFbParam * param)
{
	*param = m_param;
}

void RoleGatherFb::OnResetModuleData(unsigned old_dayid, unsigned int now_dayid)
{
	/*if(old_dayid == now_dayid) return;

	int day = 0;
	int offline_day = m_role_module_mgr->GetCommonData().offine_day;
	if (offline_day <= 0)
	{
		day = 1;
	}
	else 
	{
		day = offline_day;
	}

	if (m_param.is_first == 0)
	{
		day = 1;
		m_param.is_first = 1;
	}
	this->AddGatherPower(LOGIC_CONFIG->GetRoleGatherFbConfig()->GetOtherCfg().every_add_gather_power_value * day);*/
}

void RoleGatherFb::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	if (old_dayid == now_dayid) return;

	int day = 0;
	int offline_day = m_role_module_mgr->GetCommonData().offine_day;
	if (offline_day <= 0)
	{
		day = 1;
	}
	else
	{
		day = offline_day;
	}

	if (m_param.is_first == 0)
	{
		day = 1;
		m_param.is_first = 1;
	}
	this->AddGatherPower(LOGIC_CONFIG->GetRoleGatherFbConfig()->GetOtherCfg().every_add_gather_power_value * day);
}

int RoleGatherFb::GetGatherPower()
{
	return m_param.gather_power;
}

bool RoleGatherFb::AddGatherPower(int add_power, bool is_notice /* = false */)
{
	const RoleGatherFbOtherCfg & other_cfg = LOGIC_CONFIG->GetRoleGatherFbConfig()->GetOtherCfg();

	if (add_power > 0 && m_param.gather_power >= other_cfg.gather_power_max)
	{
		return false;
	}

	if (add_power < 0 && m_param.gather_power <= 0)
	{
		return false;
	}

	m_param.gather_power += add_power;

	if (m_param.gather_power > other_cfg.gather_power_max)
	{
		m_param.gather_power = other_cfg.gather_power_max;
	}

	if (m_param.gather_power < 0)
	{
		m_param.gather_power = 0;
	}

	this->SendInfo();
	if (is_notice && add_power < 0)
	{
		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_life_skill_consume_gather_power, -add_power);
		if (length > 0)
		{
			World::GetInstWorld()->GetSceneManager()->SystemMsgPerson2(m_role_module_mgr->GetRole(), gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT));
		}
	}

	return true;
}

void RoleGatherFb::SendInfo()
{
	Protocol::SCGatherFbInfo protocol;
	protocol.gather_power = m_param.gather_power;

	EngineAdapter::Instance().NetSend(m_role_module_mgr->GetRole()->GetNetId(), &protocol, sizeof(protocol));
}























