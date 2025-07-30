#include "income.hpp"

#include "protocol/msgincome.hpp"
#include "global/incomemanager/incomemanager.hpp"

#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"

RoleIncome::RoleIncome()
{
}

RoleIncome::~RoleIncome()
{
}

void RoleIncome::Init(RoleModuleManager * module_mgr, const RoleIncomeParam & param)
{
	m_module_mgr = module_mgr;
	m_param = param;
}

void RoleIncome::GetInitParam(RoleIncomeParam * param)
{
	*param = m_param;
}

void RoleIncome::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	if (old_dayid == now_dayid)
	{
		return;
	}
	memset(m_param.income_info_list_, 0, sizeof(m_param.income_info_list_));
}

void RoleIncome::OnRoleLogin()
{
	//同步一下
	for (int i = 0; i < INCOME_ROLE_TYPE_MAX; i++)
	{
		this->OnIncomeAdd(i, 0);
	}
}

void RoleIncome::OnReq(int type)
{
	switch (type)
	{
	case Protocol::CSIncomeRoleReq::REQ_TYPE_ROLE_INFO:
		{
			this->SendInfo();
		}
		break;
	default:
		return;
	}
}

void RoleIncome::SendInfo()
{
	Protocol::SCIncomeRoleInfo info;
	info.role_count = INCOME_ROLE_TYPE_MAX;
	memcpy(info.role_param_list_, m_param.income_info_list_, sizeof(info.role_param_list_));

	info.global_count = INCOME_ROLE_TYPE_MAX;
	IncomeManager::Instance().GetSendParam(info.global_param_list_);

	m_module_mgr->NetSend(&info, sizeof(info));
}

void RoleIncome::OnIncomeAdd(int income_type, long long add_param)
{
	switch (income_type)
	{
	//累加
	case INCOME_ROLE_TYPE_DAY_EXP:
	case INCOME_ROLE_TYPE_DAY_COIN_BIND:
	case INCOME_ROLE_TYPE_DAY_SILVER_COIN:
		{
			if (0 >= add_param)
			{
				return;
			}
			//为0则代表新增
			bool is_first = (0 == m_param.income_info_list_[income_type]);

			//记录
			m_param.income_info_list_[income_type] += add_param;

			IncomeManager::IncomeAddParam add_info;
			add_info.uid = m_module_mgr->GetUid();
			add_info.income_type = income_type;
			add_info.param = m_param.income_info_list_[income_type];
			IncomeManager::Instance().OnIncomeAdd(add_info);
		}
		break;
	default:
		return;
	}
}
