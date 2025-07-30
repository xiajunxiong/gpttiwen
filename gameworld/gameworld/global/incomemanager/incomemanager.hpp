#ifndef __INCOME_MANAGER_HPP__
#define __INCOME_MANAGER_HPP__


#include "servercommon/incomedef.hpp"

class GlobalIncome
{
public:
	GlobalIncome();

	struct RoleIncomeInfo
	{
		RoleIncomeInfo()
		{
			m_income_param = 0;
		}

		long long m_income_param;
	}; 

	void Clear();

	long long global_param;
	//角色数据map <uid, 信息>
	typedef std::map<int, RoleIncomeInfo> RoleInfoMap;
	RoleInfoMap m_role_info_map;
};

class IncomeManager
{
public:
	static IncomeManager & Instance();

	struct IncomeAddParam
	{
		IncomeAddParam()
		{
			this->Reset();
		}
		void Reset()
		{
			uid = 0;

			income_type = INCOME_ROLE_TYPE_MAX;
			param = 0;
		}
		int uid;

		int income_type;			//收益类型
		long long param;			//收益参数
	};
	void OnIncomeAdd(IncomeAddParam & add_param);

	void GetSendParam(long long(&send_param_list)[INCOME_ROLE_TYPE_MAX]);

	void OnDayChange();
private:
	long long m_send_param_list[INCOME_ROLE_TYPE_MAX];
private:
	GlobalIncome m_global_income_list[INCOME_ROLE_TYPE_MAX];
private:
	IncomeManager();
	~IncomeManager();
};


#endif