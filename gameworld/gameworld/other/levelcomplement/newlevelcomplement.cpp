#include "newlevelcomplement.hpp"
#include "gamelog.h"

#include "item/money.h"
#include "obj/character/role.h"
#include "protocol/msglevelcomplement.h"

#include "global/rank/rankmanager.hpp"

#include "other/funopen/funopen.hpp"
#include "other/rolecross/rolecross.hpp"
#include "other/roleattribute/roleexpconfig.hpp"
#include "other/levelcomplement/levelcomplementconfig.hpp"
#include "other/flyupsystem/flyupsystem.hpp"
#include "config/logicconfigmanager.hpp"

#include "servercommon/errornum.h"


RoleNewLevelComplement::RoleNewLevelComplement()
{
}

RoleNewLevelComplement::~RoleNewLevelComplement()
{
}

void RoleNewLevelComplement::Init(RoleModuleManager * mgr, const RoleNewLevelComplementParam & param)
{
	m_module_mgr = mgr;
	m_param = param;
}

void RoleNewLevelComplement::GetInitParam(RoleNewLevelComplementParam * param)
{
	*param = m_param;
}

void RoleNewLevelComplement::OnRoleLogin()
{
	if (this->IsOpen())
	{
		if (0 == m_param.is_first_achieve_fun_open_flag)
		{
			this->ComplementHelpExp();
		}
		else
		{
			this->CheckAccExpConform();
		}
	}
}

void RoleNewLevelComplement::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	if (old_dayid == now_dayid) return;

	m_param.day_buy_exp_count = 0;

	int role_fly_limit_level = m_module_mgr->GetFlyUpSystem()->IsAddExpFlyUpFlag();
	bool role_fly_limit_level_flag = m_module_mgr->GetRole()->GetLevel() >= role_fly_limit_level;
	if (role_fly_limit_level_flag)return;
	
	if (!this->IsOpen())
	{
		m_param.today_last_get_daily_exp_timestamp = 0;
		m_param.last_get_daily_exp_timestamp = 0;
		this->SendInfo();
		gamelog::g_log_levelcomplement.printf(LL_INFO, "%s user[%d, %s]", __FUNCTION__, m_module_mgr->GetUid(), m_module_mgr->GetRoleName());
		return;
	}

	m_param.last_get_daily_exp_timestamp = m_param.today_last_get_daily_exp_timestamp;
	this->ComplementHelpExp();
}

void RoleNewLevelComplement::OnRoleLevelUp()
{
	if (this->IsOpen())
	{
		if (0 == m_param.is_first_achieve_fun_open_flag)
		{
			this->ComplementHelpExp();
		}
		else
		{
			this->CheckAccExpConform();
		}
	}
}

void RoleNewLevelComplement::OnAddExp(long long add_exp, const char * reason)
{
	if (m_param.accumulation_exp <= 0 || add_exp <= 0 || !this->IsOpen())
	{
		return;
	}

	int level_difference = this->GetLevelDifference();
	const RoleNewLevelComplementCfg * level_cfg = LOGIC_CONFIG->GetRoleLevelComplementConfig()->GetNewLevelCfg(level_difference);
	if (NULL == level_cfg)
	{
		return;
	}
	int multiple_index = this->GetMultipleIndex();
	if (multiple_index < 0 || multiple_index >= ARRAY_LENGTH(level_cfg->multiple_coefficient_list))
	{
		multiple_index = LEVEL_COMPLEMENT_MULTIPLE_TYPE_0;
	}
	long long extra_add_exp = add_exp * level_cfg->multiple_coefficient_list[multiple_index] / RAND_UNIVERSAL_RATIO_BASE_NUM;
	if (extra_add_exp > m_param.accumulation_exp)
	{
		extra_add_exp = m_param.accumulation_exp;
	}
	if (extra_add_exp > 0)
	{
		m_module_mgr->GetRole()->AddExp(extra_add_exp, ADD_EXP_TYPE_NEW_LEVEL_COMPLEMENT, __FUNCTION__, false);
	}
	m_param.accumulation_exp -= extra_add_exp;
	m_param.today_last_get_daily_exp_timestamp = (unsigned int)EngineAdapter::Instance().Time();
	this->SendInfo();

	gamelog::g_log_levelcomplement.printf(LL_INFO, "%s reason[%s] user[%d, %s] accumulation_exp[%lld] add_exp[%lld] extra_add_exp[%lld] buy_count[%d] level_diff[%d] ",
		__FUNCTION__, NULL != reason ? reason : "NULL", m_module_mgr->GetUid(), m_module_mgr->GetRoleName(), m_param.accumulation_exp, add_exp, extra_add_exp, 
		m_param.day_buy_exp_count, level_difference);
}

void RoleNewLevelComplement::OnBuyExp()
{
	if (m_param.accumulation_exp <= 0)
	{
		m_module_mgr->NoticeNum(errornum::EN_LEVEL_COMPLEMENT_NOT_HAS_EXP_BUY_FAILED);
		return;
	}

	const RoleLevelComplementOtherCfg & other_cfg = LOGIC_CONFIG->GetRoleLevelComplementConfig()->GetOtherCfg();
	if (m_param.day_buy_exp_count >= other_cfg.day_buy_num)
	{
		m_module_mgr->NoticeNum(errornum::EN_LEVEL_COMPLEMENT_BUY_EXP_COUNT_NOT_ENOUGH);
		return;
	}
	int level_difference = this->GetLevelDifference();
	const RoleNewLevelComplementCfg * level_cfg = LOGIC_CONFIG->GetRoleLevelComplementConfig()->GetNewLevelCfg(level_difference);
	if (NULL == level_cfg)
	{
		return;
	}
	if (!m_module_mgr->GetMoney()->MoneyTypeMoreThan(level_cfg->money_type, level_cfg->price))
	{
		return;
	}
	if (!m_module_mgr->GetMoney()->UseMoney(level_cfg->money_type, level_cfg->price, __FUNCTION__))
	{
		return;
	}

	long long add_exp = m_param.day_supplement_exp * level_cfg->buy_exp_coefficient / RAND_UNIVERSAL_RATIO_BASE_NUM;
	if (add_exp > m_param.accumulation_exp)
	{
		add_exp = m_param.accumulation_exp;
	}
	if (add_exp > 0)
	{
		m_module_mgr->GetRole()->AddExp(add_exp, ADD_EXP_TYPE_NEW_LEVEL_COMPLEMENT, __FUNCTION__, false);
	}
	m_param.accumulation_exp -= add_exp;
	m_param.day_buy_exp_count += 1;
	this->SendInfo();

	gamelog::g_log_levelcomplement.printf(LL_INFO, "%s user[%d, %s] accumulation_exp[%lld] add_exp[%lld] buy_count[%d] level_diff[%d] ", __FUNCTION__, 
		m_module_mgr->GetUid(), m_module_mgr->GetRoleName(), m_param.accumulation_exp, add_exp, m_param.day_buy_exp_count, level_difference);
}

void RoleNewLevelComplement::SendInfo()
{
	Protocol::SCRoleNewLevelComplementInfo info;
	info.accumulation_exp = m_param.accumulation_exp;
	info.day_supplement_exp = m_param.day_supplement_exp;
	info.last_get_daily_exp_timestamp = m_param.last_get_daily_exp_timestamp;
	info.day_buy_exp_count = m_param.day_buy_exp_count;
	info.level_difference = this->GetLevelDifference();

	m_module_mgr->NetSend(&info, sizeof(info));
}

int RoleNewLevelComplement::GetLevelDifference()
{
	return this->GetAverageLevel() - LOGIC_CONFIG->GetRoleLevelComplementConfig()->GetOtherCfg().rank_level_difference - m_module_mgr->GetRole()->GetLevel();
}

int RoleNewLevelComplement::GetAverageLevel()
{
	return RankManager::Instance().GetAverageByLevelComplement(m_module_mgr->GetRoleCross()->GetOriginServerId());
}

//已有外部判断满足条件
void RoleNewLevelComplement::ComplementHelpExp()
{
	int level_difference = this->GetLevelDifference();
	const RoleNewLevelComplementCfg * level_cfg = LOGIC_CONFIG->GetRoleLevelComplementConfig()->GetNewLevelCfg(level_difference);
	if (NULL == level_cfg)
	{
		return;
	}

	int end_level = this->GetAverageLevel() - LOGIC_CONFIG->GetRoleLevelComplementConfig()->GetOtherCfg().rank_level_difference;
	m_param.day_supplement_exp = level_cfg->day_add_exp_coefficient * LOGIC_CONFIG->GetRoleExpConfig()->GetNeedExpByLevel(
		m_module_mgr->GetRole()->GetLevel(), end_level) / RAND_UNIVERSAL_RATIO_BASE_NUM;
	m_param.accumulation_exp += m_param.day_supplement_exp;
	m_param.is_first_achieve_fun_open_flag = 1;
	this->CheckAccExpConform();
	this->SendInfo();

	gamelog::g_log_levelcomplement.printf(LL_INFO, "%s user[%d, %s] level_difference[%d] accumulation_exp[%lld] day_supplement_exp[%lld] time[%u, %u] buy_count[%d]",
		 __FUNCTION__, m_module_mgr->GetUid(), m_module_mgr->GetRoleName(), level_difference, m_param.accumulation_exp, m_param.day_supplement_exp, 
		 m_param.today_last_get_daily_exp_timestamp, m_param.last_get_daily_exp_timestamp, m_param.day_buy_exp_count);
}

void RoleNewLevelComplement::CheckAccExpConform()
{
	int level_difference = this->GetLevelDifference();
	const RoleNewLevelComplementCfg * level_cfg = LOGIC_CONFIG->GetRoleLevelComplementConfig()->GetNewLevelCfg(level_difference);
	if (NULL == level_cfg)
	{
		return;
	}
	long long exp_limit = level_cfg->exp_limit_coefficient * m_param.day_supplement_exp / RAND_UNIVERSAL_RATIO_BASE_NUM;
	if (m_param.accumulation_exp > exp_limit)
	{
		gamelog::g_log_levelcomplement.printf(LL_INFO, "%s user[%d, %s] accumulation_exp[%lld]  level_diff[%d] exp_limit[%lld]", __FUNCTION__, 
		m_module_mgr->GetUid(), m_module_mgr->GetRoleName(), m_param.accumulation_exp, level_difference, exp_limit);
		m_param.accumulation_exp = exp_limit;
		this->SendInfo();
	}
}

bool RoleNewLevelComplement::IsOpen()
{
	if (!FunOpen::IsFunOpen(m_module_mgr->GetRole(), FUN_TYPE_NEW_LEVEL_COMPLEMENT, false) || this->GetLevelDifference() <= 0)
	{
		return false;
	}

	return true;
}

int RoleNewLevelComplement::GetMultipleIndex()
{
	if(m_param.last_get_daily_exp_timestamp <= 0) return LEVEL_COMPLEMENT_MULTIPLE_TYPE_0;

	int interval_day = GetDayIndex((time_t)m_param.last_get_daily_exp_timestamp, EngineAdapter::Instance().Time()); 
	if (interval_day <= 0)
	{
		interval_day = 1;
	}
	int multiple_index = 0;
	switch (interval_day)
	{
		case 1:
		{
			multiple_index =  LEVEL_COMPLEMENT_MULTIPLE_TYPE_0;
		}
		break;
		case 2:
		{
			multiple_index = LEVEL_COMPLEMENT_MULTIPLE_TYPE_1;
		}
		break;
		default:
		{
			multiple_index = LEVEL_COMPLEMENT_MULTIPLE_TYPE_2;
		}
		break;
	}

	return multiple_index;
}
