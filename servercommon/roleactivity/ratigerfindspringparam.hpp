#ifndef __RA_TIGER_FIND_SPRING_PARAM_HPP__
#define __RA_TIGER_FIND_SPRING_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"

#pragma pack(push, 4)


struct RATigerFindSpringParam
{
	RATigerFindSpringParam() { this->Reset(); }

	void Reset()
	{
		ra_begin_timestamp = 0;
		is_receive_reward = 0;
	}

	void DayReset()
	{
		is_receive_reward = 0;
	}

	unsigned int ra_begin_timestamp;
	int is_receive_reward;			// 当日是否可领取奖励，0--不可领取，1--可领取，2--已领取
};
UNSTD_STATIC_CHECK(sizeof(RATigerFindSpringParam) <= sizeof(RoleActivityRoleData));

#pragma pack(pop)

#endif