#ifndef __RA_YUN_ZE_BLOOMING_RECORD_PARAM_HPP__
#define __RA_YUN_ZE_BLOOMING_RECORD_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"

#pragma pack(push, 4)

static const int YUNZE_BLOOMING_RECORD_MAX_REWARD_NUM = 5;		// 可领取箱子数量

struct RAYunZeBloomingRecordParam
{
	RAYunZeBloomingRecordParam() { this->Reset(); }

	void Reset()
	{
		ra_begin_timestamp = 0;
		times = 0;
		challenge_end_timestamp = 0;
		cur_integral = 0;
		is_available_integral = 0;

		memset(reward_list, 0, sizeof(reward_list));
	}

	void DayReset()
	{
		times = 0;
	}

	unsigned int ra_begin_timestamp;
	short times;						// 已挑战次数
	short cur_integral;					// 当前玩家积分
	int challenge_end_timestamp;		// 挑战结束时间戳
	int is_available_integral;			// 积分是否可用，1-可用，0-不可用
	int reward_list[YUNZE_BLOOMING_RECORD_MAX_REWARD_NUM];					// 奖励是否已领取，0-未领取，1-已领取
};
UNSTD_STATIC_CHECK(sizeof(RAYunZeBloomingRecordParam) <= sizeof(RoleActivityRoleData));



#pragma pack(pop)

#endif 
