#ifndef __RA_YUAN_QI_TURNING_PARAM_HPP__
#define __RA_YUAN_QI_TURNING_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/bitmapfunction.hpp"
#include "servercommon/roleactivitydef.hpp"

#pragma pack(push, 4)

struct RAYuanQiTurningParam
{
	RAYuanQiTurningParam() { this->Reset(); }

	void Reset()
	{
		ra_begin_timestamp = 0;
		open_index = 0;
		info_id = 0;
		reward_id = 0;
	}

	void OnDayChange()
	{
		open_index = 0;
		info_id = 0;
		reward_id = 0;
	}

	unsigned int ra_begin_timestamp;							
	int open_index;
	int info_id;
	int reward_id;
};
UNSTD_STATIC_CHECK(sizeof(RAYuanQiTurningParam) <= sizeof(RoleActivityRoleData));

#pragma pack(pop)

#endif