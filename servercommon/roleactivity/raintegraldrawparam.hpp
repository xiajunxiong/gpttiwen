#ifndef __RA_INTEGRAL_DRAW_PARAM_HPP__
#define __RA_INTEGRAL_DRAW_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"


//////////////////////////////////////////// 整点抽奖 //////////////////////////////////////////
static const int MAX_RA_INTEGRAL_DRAW_COLOR_NUM = 4;				// 花色种类
static const int MAX_RA_INTEGRAL_DRAW_LINE_NUM = 4;					// 抽奖行数

#pragma pack(push, 4)
////////////////////////////////////////////  服务器数据  //////////////////////////////////////////////////////


////////////////////////////////////////////  个人数据  //////////////////////////////////////////////////////


struct RAIntegralDrawParam
{
	RAIntegralDrawParam() { this->Reset(); }

	void Reset()
	{
		ra_begin_timestamp = 0;
		draw_times = 0;
		memset(reward_list, 0, sizeof(reward_list));
	}

	unsigned int ra_begin_timestamp;
	int draw_times;
	short reward_list[MAX_RA_INTEGRAL_DRAW_LINE_NUM];
};

UNSTD_STATIC_CHECK(sizeof(RAIntegralDrawParam) <= sizeof(RoleActivityRoleData))

#pragma pack(pop)
#endif	//__RA_HORCRXU_TIANCHENG_PARAM_HPP__
