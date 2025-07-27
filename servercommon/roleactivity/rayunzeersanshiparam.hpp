#ifndef __RA_YUN_ZE_ER_SAN_SHI_PARAM_HPP__
#define __RA_YUN_ZE_ER_SAN_SHI_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"

#pragma pack(push, 4)

static const int YUNZE_ER_SAN_SHI_MAX_TASK_NUM = 10;

enum RECEIVE_STATUS
{
	RECEIVE_STATUS_EXIST = 0,	// 未领取
	RECEIVE_STATUS_LACK = 1,	// 已领取
};

struct RAYunZeErSanShiParam
{
	RAYunZeErSanShiParam() { this->Reset(); }

	void Reset()
	{
		ra_begin_timestamp = 0;
		times = 0;
		is_receive_reward = 0;
		is_turntable_reward = 0;
		task_id = 0;

		memset(task_list, 0, sizeof(task_list));
	}

	void DayReset()
	{
		times = 0;
		is_receive_reward = 0;
		is_turntable_reward = 0;

		memset(task_list, 0, sizeof(task_list));
	}

	unsigned int ra_begin_timestamp;
	short times;
	short is_receive_reward;			//当日是否领取过转盘奖励， 0-未领取，1-已领取
	short is_turntable_reward;			// 是否可以转盘 1-可以，0-不可以
	short task_id;
	int task_list[YUNZE_ER_SAN_SHI_MAX_TASK_NUM];		//随机出的问题id
};
UNSTD_STATIC_CHECK(sizeof(RAYunZeErSanShiParam) <= sizeof(RoleActivityRoleData));


#pragma pack(pop)

#endif