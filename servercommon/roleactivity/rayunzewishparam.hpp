#ifndef __RA_YUN_ZE_WISH_PARAM_HPP__
#define __RA_YUN_ZE_WISH_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"

#pragma pack(push, 4)

static const int YZ_WISH_MAX_REWARD_NUM = 5;			// 最大可能的奖励数

struct RAYunZeWishParam
{
	RAYunZeWishParam() { this->Reset(); }

	void Reset()
	{
		ra_begin_timestamp = 0;
		is_can_lottery = 0;
		next_refresh_timestamp = 0;
		end_lock_timestamp = 0;
		lottery_lock = 0;
		is_send_timestamp = 0;
		is_lock_reward.Reset();
	}

	unsigned int ra_begin_timestamp;
	unsigned int next_refresh_timestamp;		// 下次刷新奖励次数时间戳
	int is_send_timestamp;						// 发给客户端的伪时间戳标志位，0-未发过
	unsigned int end_lock_timestamp;			// 最后一次锁定奖励时间戳
	short is_can_lottery;						// 是否可抽奖，0-不能，1-可以
	short lottery_lock;							// 标志位，标识当前时间段是否抽过奖,0--未抽过，1--抽过
	BitMap<YZ_WISH_MAX_REWARD_NUM> is_lock_reward;	
};
UNSTD_STATIC_CHECK(sizeof(RAYunZeWishParam) <= sizeof(RoleActivityRoleData));

#pragma pack(pop)

#endif