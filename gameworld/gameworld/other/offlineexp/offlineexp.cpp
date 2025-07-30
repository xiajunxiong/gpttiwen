#include "offlineexp.hpp"
#include "offlineexpconfig.hpp"
#include "engineadapter.h"
#include "protocol/msgofflineexp.h"
#include "config/logicconfigmanager.hpp"
#include "other/rolemodulemanager.hpp"
#include "obj/character/role.h"
#include "gameworld/gamelog.h"

OfflineExp::OfflineExp() : m_role_module_mgr(NULL), m_offline_exp_addition(0),m_offline_min(0)
{
}

OfflineExp::~OfflineExp()
{
}

void OfflineExp::Init(RoleModuleManager *role_module_manager, unsigned int offline_exp, int offline_min)
{
	m_role_module_mgr = role_module_manager;
	m_offline_exp_addition = offline_exp;
	m_offline_min = offline_min;
}

void OfflineExp::GetOtherInitParam(unsigned int * offline_exp, int * offline_min)
{
	*offline_exp = m_offline_exp_addition;
	*offline_min = m_offline_min;
}

void OfflineExp::OnRoleLogin(long long last_save_time)
{
	/*unsigned int last_logout = static_cast<unsigned int>(last_save_time);
	if (0 == last_logout) return;	// 上次登出时间为 0，有可能是刚初始化

	long long now_second = (long long)EngineAdapter::Instance().Time();

	const OfflineExpOtherCfg & other_cfg = LOGIC_CONFIG->GetOfflineExpConfig()->GetOtherCfg();
	if(NULL == &other_cfg) return;

	int offline_min = (int)((now_second - last_logout) / SECOND_PER_MIN) + m_offline_min;	// 向下取整
	if (offline_min - other_cfg.oe_start_calc_time <= 0) return;	// 时间是负数，有可能是改了时间

	int real_calc_time = 0;
	m_offline_exp_addition = LOGIC_CONFIG->GetOfflineExpConfig()->CalcOfflineExpAddition(m_role_module_mgr->GetRole()->GetLevel(), offline_min, &real_calc_time);
	m_offline_min = offline_min - other_cfg.oe_start_calc_time;
	
	if (m_offline_exp_addition >= MAX_UINT32)
	{
		m_offline_exp_addition = MAX_UINT32;
	}
	if (m_offline_exp_addition > 0) 
	{
		this->SendOfflineExpInfo(m_offline_min);
		gamelog::g_log_offline_exp.printf(LL_INFO, "OnRoleLogin_Offline role[%d,%s] offline_min[%d] cur_exp[%lld] m_level[%d] add_exp[%d] add_reason[%d] ",
											m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_offline_min,
											m_role_module_mgr->GetRole()->GetExp(), m_role_module_mgr->GetRole()->GetLevel(), 
											m_offline_exp_addition, ADD_EXP_TYPE_NORMAL);
	}*/
}

void OfflineExp::ReceiveExp()
{
	/*if (m_offline_exp_addition <= 0) return;
	
	m_role_module_mgr->GetRole()->AddExp(m_offline_exp_addition, ADD_EXP_TYPE_NORMAL, "OfflineExp");
	char reason_str[] = "OfflineExp";
	gamelog::g_log_offline_exp.printf(LL_INFO, "OnRoleLogin_Offline role[%d,%s] offline_min[%d] cur_exp[%lld] m_level[%d] add_exp[%d] add_reason[%d] reason_str[%s] ",
										m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_offline_min, 
										m_role_module_mgr->GetRole()->GetExp(), m_role_module_mgr->GetRole()->GetLevel(), 
										m_offline_exp_addition, ADD_EXP_TYPE_NORMAL, reason_str);
	m_offline_exp_addition = 0;
	m_offline_min = 0;*/
}

void OfflineExp::SendOfflineExpInfo(int offline_m)
{
/*	static Protocol::SCOfflineExpAddition oea;

	oea.offline_min = offline_m;
	oea.exp_add = m_offline_exp_addition;

	m_role_module_mgr->NetSend((const char *)&oea, sizeof(oea));*/
}