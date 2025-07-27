#ifndef __RA_YUN_ZE_HAO_LI_PARAM_HPP__
#define __RA_YUN_ZE_HAO_LI_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"

#pragma pack(push, 4)

struct RAYunZeHaoLiParam
{
	RAYunZeHaoLiParam() { this->Reset(); }

	void Reset()
	{
		ra_begin_timestamp = 0;
		buy_times = 0;
		prop_free_num = 0;
		prop_free_timestamp = 0;
	}

	void DayReset()
	{
		buy_times = 0;			// 根据限购类型
	}

	unsigned int ra_begin_timestamp;
	short buy_times;
	short prop_free_num;
	unsigned int prop_free_timestamp;			// 当次领取奖励时间段结束戳
};
UNSTD_STATIC_CHECK(sizeof(RAYunZeHaoLiParam) <= sizeof(RoleActivityRoleData));

#pragma pack(pop)

#endif