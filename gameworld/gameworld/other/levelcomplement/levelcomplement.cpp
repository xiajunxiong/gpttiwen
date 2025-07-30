#include "levelcomplement.h"
#include "protocol/msglevelcomplement.h"
#include "engineadapter.h"
#include "other/rolemodulemanager.hpp"
#include "obj/character/role.h"
#include "config/logicconfigmanager.hpp"
#include "other/levelcomplement/levelcomplementconfig.hpp"
#include "config/sharedconfig/sharedconfig.h"
#include "item/money.h"
#include "servercommon/errornum.h"
#include "other/funopen/funopen.hpp"
#include "global/cross/crossmanager.h"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "gameworld/gamelog.h"

LevelComplement::LevelComplement() :m_role_module_mgr(NULL)
{
	level_complement_flag = 0;
	accumulated_exp = 0;
	m_refresh_accumulated_tamp = 0;
	m_end_accumulate_tamp = 0;
	m_day_difference = 0;
	m_add_exp_times = 0;
	m_alread_get_exp = 0;
	m_save_exp = 0;
}

LevelComplement::~LevelComplement()
{
}

void LevelComplement::Init(RoleModuleManager * mgr, const CommonDataParam & param)
{
	m_role_module_mgr = mgr;
	level_complement_flag = param.level_complement_flag;
	accumulated_exp = param.accumulated_exp;
	m_refresh_accumulated_tamp = param.m_refresh_accumulated_tamp;
	m_end_accumulate_tamp = param.m_end_accumulate_tamp;
	m_day_difference = param.m_day_difference;
	m_add_exp_times = param.m_add_exp_times;
	m_alread_get_exp = param.m_alread_get_exp;
	m_save_exp = param.m_save_exp;
	m_today_accumulated_exp = param.m_today_accumulated_exp;
	//检查创角时间和跨服时间的天数差，看一下是否获取过经验，没有则补偿
	if (m_today_accumulated_exp <= 0)
	{
		m_today_accumulated_exp = accumulated_exp;//修复未赋值的
	}
	unsigned int real_tamp = CrossManager::Instance().GetCrossOldestServerOpenTamp();
	if (real_tamp <= 0)return;

	long long  m_create_tamp = m_role_module_mgr->GetRole()->GetRoleCreateTime();
	int open_day = GetDayIndex(real_tamp, (time_t)m_create_tamp);
	const RoleLevelComplementCfg * cfg = LOGIC_CONFIG->GetRoleLevelComplementConfig()->GetLevelCfg(open_day);
	if (NULL != cfg && m_save_exp <= 0 && level_complement_flag == 1)
	{
		long long old_accumulated_exp = accumulated_exp;
		long long old_save_exp = m_save_exp;
		accumulated_exp = cfg->exp;
		m_refresh_accumulated_tamp = static_cast<unsigned int>(EngineAdapter::Instance().Time() + EngineAdapter::Instance().NextDayInterval(0, 0, 0));
		m_end_accumulate_tamp = m_refresh_accumulated_tamp + (PER_DAY_SECOND * cfg->complement_day);
		m_day_difference = cfg->day_difference;
		m_add_exp_times = 0;
		m_save_exp += cfg->exp;
		m_today_accumulated_exp = accumulated_exp;
		gamelog::g_log_levelcomplement.printf(LL_INFO, "LevelComplement::Init user[%d,%s] old_accumulated_exp[%lld] old_save_exp[%lld] new_accumulated_exp[%lld] m_refresh_accumulated_tamp[%d] m_end_accumulate_tamp[%d] m_day_difference[%d] m_add_exp_times[%d] new_save_exp[%lld]",
			m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), old_accumulated_exp, old_save_exp, accumulated_exp, m_refresh_accumulated_tamp, m_end_accumulate_tamp, m_day_difference, m_add_exp_times, m_save_exp);
	}
}

void LevelComplement::GetInitParam(CommonDataParam * param)
{
	param->level_complement_flag = level_complement_flag;
	param->accumulated_exp = accumulated_exp;
	param->m_refresh_accumulated_tamp = m_refresh_accumulated_tamp;
	param->m_end_accumulate_tamp = m_end_accumulate_tamp;
	param->m_day_difference = m_day_difference;
	param->m_add_exp_times = m_add_exp_times;
	param->m_alread_get_exp = m_alread_get_exp;
	param->m_save_exp = m_save_exp;
	param->m_today_accumulated_exp = m_today_accumulated_exp;
}

void LevelComplement::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
}

void LevelComplement::Update(time_t now_second)
{
	/*if (!FunOpen::IsFunOpen(m_role_module_mgr->GetRole(), FUN_TYPE_LEVEL_COMPLEMENT))return;
	if (level_complement_flag < 1)
	{
		//请求跨服开服天数，如果没有则返回等待
		unsigned int real_tamp = CrossManager::Instance().GetCrossOldestServerOpenTamp();
		if (real_tamp <= 0)return;

		level_complement_flag = 1;
		//首次刷新设置初始
		long long  m_create_tamp = m_role_module_mgr->GetRole()->GetRoleCreateTime();
		int open_day = GetDayIndex(real_tamp, (time_t)m_create_tamp);
		const RoleLevelComplementCfg * cfg = LOGIC_CONFIG->GetRoleLevelComplementConfig()->GetLevelCfg(open_day);
		if (cfg != NULL)
		{
			long long old_accumulated_exp = accumulated_exp;
			long long old_save_exp = m_save_exp;
			accumulated_exp = cfg->first_day_exp;
			m_refresh_accumulated_tamp = EngineAdapter::Instance().Time() + EngineAdapter::Instance().NextDayInterval(0, 0, 0);
			m_end_accumulate_tamp = m_refresh_accumulated_tamp + (PER_DAY_SECOND * cfg->complement_day);
			m_day_difference = cfg->day_difference;
			m_add_exp_times = 0;
			m_save_exp += cfg->first_day_exp;
			m_today_accumulated_exp = accumulated_exp;
			gamelog::g_log_levelcomplement.printf(LL_INFO, "LevelComplement::Update user[%d,%s] old_accumulated_exp[%lld] old_save_exp[%lld] new_accumulated_exp[%lld] m_refresh_accumulated_tamp[%d] m_end_accumulate_tamp[%d] m_day_difference[%d] m_add_exp_times[%d] new_save_exp[%lld]",
				m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), old_accumulated_exp, old_save_exp, accumulated_exp, m_refresh_accumulated_tamp, m_end_accumulate_tamp, m_day_difference, m_add_exp_times, m_save_exp);
		}
	}

	if (m_end_accumulate_tamp > now_second && m_refresh_accumulated_tamp <= now_second && level_complement_flag < 2)
	{
		//请求跨服开服天数，如果没有则返回等待
		unsigned int real_tamp = CrossManager::Instance().GetCrossOldestServerOpenTamp();
		if (real_tamp <= 0)return;

		level_complement_flag = 2;
		//第二天
		long long  m_create_tamp = m_role_module_mgr->GetRole()->GetRoleCreateTime();
		int open_day = GetDayIndex(real_tamp, (time_t)m_create_tamp);
		const RoleLevelComplementCfg * cfg = LOGIC_CONFIG->GetRoleLevelComplementConfig()->GetLevelCfg(open_day);
		if (cfg != NULL)
		{
			long long old_accumulated_exp = accumulated_exp;
			long long old_save_exp = m_save_exp;
			accumulated_exp += cfg->second_day_exp;
			m_refresh_accumulated_tamp = static_cast<unsigned int>(EngineAdapter::Instance().Time() + EngineAdapter::Instance().NextDayInterval(0, 0, 0));
			m_save_exp += cfg->second_day_exp;
			m_today_accumulated_exp = accumulated_exp;
			gamelog::g_log_levelcomplement.printf(LL_INFO, "LevelComplement::Update user[%d,%s] old_accumulated_exp[%lld] old_save_exp[%lld] new_accumulated_exp[%lld] m_refresh_accumulated_tamp[%d] m_end_accumulate_tamp[%d] m_day_difference[%d] m_add_exp_times[%d] new_save_exp[%lld]",
				m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), old_accumulated_exp, old_save_exp, accumulated_exp, m_refresh_accumulated_tamp, m_end_accumulate_tamp, m_day_difference, m_add_exp_times, m_save_exp);
		}
	}

	if (m_end_accumulate_tamp > now_second)
	{
		if (m_refresh_accumulated_tamp <= now_second)
		{
			const RoleLevelComplementCfg * cfg = LOGIC_CONFIG->GetRoleLevelComplementConfig()->GetLevelCfg(m_day_difference);
			if (cfg != NULL)
			{
				long long old_accumulated_exp = accumulated_exp;
				long long old_save_exp = m_save_exp;
				if (m_add_exp_times >= cfg->add_exp_num)
				{
					return;
				}

				long long real_add_save = 0;
				if (accumulated_exp + cfg->exp > cfg->exp_limit)
				{
					real_add_save = cfg->exp_limit - accumulated_exp;
					accumulated_exp = cfg->exp_limit;
				}
				else
				{
					real_add_save = cfg->exp;
					accumulated_exp += cfg->exp;
				}
				m_save_exp += real_add_save;
				m_today_accumulated_exp = accumulated_exp;
				m_add_exp_times += 1;
				m_refresh_accumulated_tamp = m_refresh_accumulated_tamp + PER_DAY_SECOND;
				gamelog::g_log_levelcomplement.printf(LL_INFO, "LevelComplement::Update user[%d,%s] old_accumulated_exp[%lld] old_save_exp[%lld] new_accumulated_exp[%lld] m_refresh_accumulated_tamp[%d] m_end_accumulate_tamp[%d] m_day_difference[%d] m_add_exp_times[%d] new_save_exp[%lld]",
					m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), old_accumulated_exp, old_save_exp, accumulated_exp, m_refresh_accumulated_tamp, m_end_accumulate_tamp, m_day_difference, m_add_exp_times, m_save_exp);
			}
		}
	}*/
}

void LevelComplement::OnAddExp(long long add_exp)
{
	/*if (!FunOpen::IsFunOpen(m_role_module_mgr->GetRole(), FUN_TYPE_LEVEL_COMPLEMENT))return;
	if (accumulated_exp <= 0)return;
	if (EngineAdapter::Instance().Time() > m_end_accumulate_tamp)return;
	const RoleLevelComplementCfg * cfg = LOGIC_CONFIG->GetRoleLevelComplementConfig()->GetLevelCfg(m_day_difference);
	if (cfg != NULL)
	{
		long long ex_add_exp = add_exp * (cfg->multiple / RoleLevelComplementCfg::MULTIPLE_PERSENT);
		if (ex_add_exp <= 0)
		{
			return;
		}

		ex_add_exp -= add_exp;
		if (ex_add_exp <= accumulated_exp)
		{
			accumulated_exp -= ex_add_exp;
		}
		else
		{
			ex_add_exp = accumulated_exp;
			accumulated_exp = 0;
		}
		m_alread_get_exp += ex_add_exp;
		m_role_module_mgr->GetRole()->AddExp(ex_add_exp, ADD_EXP_TYPE_LEVEL_COMPLEMENT);
		this->SendInfo();
	}*/
}

void LevelComplement::BuyExp()
{
	/*if (accumulated_exp <= 0)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_LEVELCOMPLEMENT_EXP_NOT_ENOUGH_ERR);
		return;
	}
	const RoleLevelComplementCfg * cfg = LOGIC_CONFIG->GetRoleLevelComplementConfig()->GetLevelCfg(m_day_difference);
	if (cfg != NULL)
	{
		long long ex_add_exp = cfg->buy_exp;
		if (ex_add_exp > accumulated_exp)
		{
			ex_add_exp = accumulated_exp;
		}

		//消耗
		if (cfg->price_type == 0)
		{
			if (!m_role_module_mgr->GetMoney()->UseGold(cfg->price, __FUNCTION__))
			{
				m_role_module_mgr->NoticeNum(errornum::EN_GOLD_NOT_ENOUGH);
				return;
			}
		}
		else
		{
			return;
		}

		accumulated_exp -= ex_add_exp;
		m_alread_get_exp += ex_add_exp;
		m_role_module_mgr->GetRole()->AddExp(ex_add_exp, ADD_EXP_TYPE_LEVEL_COMPLEMENT);
		this->SendInfo();
	}*/
}

void LevelComplement::SendInfo()
{
	/*Protocol::SCLevelComplementInfo protocol;

	protocol.accumulated_exp = accumulated_exp;
	protocol.m_refresh_accumulated_tamp = m_refresh_accumulated_tamp;
	protocol.m_end_accumulate_tamp = m_end_accumulate_tamp;
	protocol.m_day_difference = m_day_difference;
	protocol.m_add_exp_times = m_add_exp_times;
	protocol.m_alread_get_exp = m_alread_get_exp;
	protocol.m_save_exp = m_save_exp;
	protocol.m_today_accumulated_exp = m_today_accumulated_exp;
	m_role_module_mgr->NetSend((const char *)&protocol, sizeof(protocol));*/
}
