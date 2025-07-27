#ifndef __RA_SPRING_FESTIVAL_STORE_PARAM_HPP__
#define __RA_SPRING_FESTIVAL_STORE_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"

#pragma pack(push, 4)
////////////////////////////////////////////  服务器数据  //////////////////////////////////////////////////////

const static int MAX_LOG_TIMES = 20;//最大兑换种类

////////////////////////////////////////////  个人数据  //////////////////////////////////////////////////////

struct RASpringFesticalStoreParam
{
	RASpringFesticalStoreParam() { this->Reset(); }

	void Reset()
	{
		open_day = 0;
		phase = 0;
		memset(today_buy_times, 0, sizeof(today_buy_times));
		memset(activity_buy_times, 0, sizeof(activity_buy_times));
		begin_time = 0;
	}

	void DayReset()
	{
		memset(today_buy_times, 0, sizeof(today_buy_times));
	}

	int open_day;
	int phase;
	short today_buy_times[MAX_LOG_TIMES];
	short activity_buy_times[MAX_LOG_TIMES];
	unsigned int begin_time;
};

UNSTD_STATIC_CHECK(sizeof(RASpringFesticalStoreParam) <= sizeof(RoleActivityRoleData));

#pragma pack(pop)


#endif	//__RA_DISCOUNT_STORE_PARAM_HPP__
