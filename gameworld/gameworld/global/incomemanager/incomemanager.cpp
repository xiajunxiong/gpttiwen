#include "incomemanager.hpp"

IncomeManager & IncomeManager::Instance()
{
	static IncomeManager income_manager;
	return income_manager;
}

void IncomeManager::OnIncomeAdd(IncomeAddParam & add_info)
{
	switch (add_info.income_type)
	{
		//平均
	case INCOME_ROLE_TYPE_DAY_EXP:
	case INCOME_ROLE_TYPE_DAY_COIN_BIND:
	case INCOME_ROLE_TYPE_DAY_SILVER_COIN:
		{
			if (0 >= add_info.param)
			{
				return;
			}
			if (0 == add_info.uid)
			{
				return;
			}

			GlobalIncome::RoleIncomeInfo & role_income_info
				= m_global_income_list[add_info.income_type].m_role_info_map[add_info.uid];

			//更新
			long long add_param = add_info.param - role_income_info.m_income_param;
			role_income_info.m_income_param = add_info.param;

			//总数
			m_global_income_list[add_info.income_type].global_param += add_param;
			//平均
			if (m_global_income_list[add_info.income_type].m_role_info_map.empty())
			{
				return;
			}
			m_send_param_list[add_info.income_type] =
				m_global_income_list[add_info.income_type].global_param
				/ (int)m_global_income_list[add_info.income_type].m_role_info_map.size();
		}
		break;
	default:
		return;
	}
}

void IncomeManager::GetSendParam(long long(&send_param_list)[INCOME_ROLE_TYPE_MAX])
{
	memcpy(send_param_list, m_send_param_list, sizeof(send_param_list));
}

void IncomeManager::OnDayChange()
{
	memset(m_send_param_list, 0, sizeof(m_send_param_list));

	UNSTD_STATIC_CHECK(3 == INCOME_ROLE_TYPE_MAX)
	for (int i = 0; i < INCOME_ROLE_TYPE_MAX; i++)
	{
		m_global_income_list[i].Clear();
	}
}

IncomeManager::IncomeManager()
{
}

IncomeManager::~IncomeManager()
{
}

GlobalIncome::GlobalIncome()
{
	global_param = 0;
}

void GlobalIncome::Clear()
{
	global_param = 0;
	m_role_info_map.clear();
}
