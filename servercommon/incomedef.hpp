#ifndef __INCOME_DEF__
#define __INCOME_DEF__

#include "servercommon/servercommon.h"

///////////////////////收益统计///////////////////
//收益类型
enum IncomeRoleType						//			角色			全局
{										//			param			p_ll	p			协议参数
	INCOME_ROLE_TYPE_DAY_EXP = 0,		//每日-经验	当日收获		总数	参加人数	平均数
	INCOME_ROLE_TYPE_DAY_COIN_BIND,		//每日-铜币	当日收获		总数	参加人数	平均数
	INCOME_ROLE_TYPE_DAY_SILVER_COIN,	//每日-灵玉	当日收获		总数	参加人数	平均数

	INCOME_ROLE_TYPE_MAX,
};

#pragma pack(push, 4)
//角色参数
struct RoleIncomeParam
{
	RoleIncomeParam()
	{
		this->Reset();
	}
	void Reset()
	{
		memset(hold_ll_list, 0, sizeof(hold_ll_list));
		memset(income_info_list_, 0, sizeof(income_info_list_));
	}

	long long hold_ll_list[2];

	long long income_info_list_[INCOME_ROLE_TYPE_MAX];

};
typedef char RoleIncomeParamHex[sizeof(RoleIncomeParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(RoleIncomeParamHex) < 256);

#pragma pack(pop)

#endif