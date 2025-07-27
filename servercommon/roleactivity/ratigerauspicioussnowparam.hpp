#ifndef __RA_TIGER_AUSPICIOUS_SNOW_PARAM_HPP__
#define __RA_TIGER_AUSPICIOUS_SNOW_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"

#pragma pack(push, 4)


struct RATigerAuspiciousSnowParam
{
	RATigerAuspiciousSnowParam() { this->Reset(); }

	void Reset()
	{
		ra_begin_timestamp = 0;
		times = 0;
	}

	void DayReset()
	{
		times = 0;
	}

	unsigned int ra_begin_timestamp;
	int times;		// 已拾取数量

};
UNSTD_STATIC_CHECK(sizeof(RATigerAuspiciousSnowParam) <= sizeof(RoleActivityRoleData));

#pragma pack(pop)

#endif