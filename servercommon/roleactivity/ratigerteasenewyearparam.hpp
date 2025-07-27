#ifndef __RA_TIGER_TEASE_NEW_YEAR_PARAM_HPP__
#define __RA_TIGER_TEASE_NEW_YEAR_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"

#pragma pack(push, 4)


struct RATigerTeaseNewYearParam
{
	RATigerTeaseNewYearParam() { this->Reset(); }

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
	int times;
};
UNSTD_STATIC_CHECK(sizeof(RATigerTeaseNewYearParam) <= sizeof(RoleActivityRoleData));

#pragma pack(pop)

#endif