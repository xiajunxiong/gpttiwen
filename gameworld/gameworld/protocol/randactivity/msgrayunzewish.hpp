#ifndef __MSG_RA_YUN_ZE_WISH_HPP__
#define __MSG_RA_YUN_ZE_WISH_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/rayunzewishparam.hpp"


#pragma pack(push, 4)

namespace Protocol
{

	class SCRAYunZeWishInfo				//!< 3132 随机活动-云泽祈愿 信息下发
	{
	public:
		SCRAYunZeWishInfo();
		MessageHeader header;

		unsigned int next_refresh_timestamp;		// 下次刷新奖励次数时间戳
		int is_can_lottery;							// 是否可抽奖，0-不能，1-可以
		BitMap<YZ_WISH_MAX_REWARD_NUM> is_lock_reward;		// [0-4] 锁定奖励, YZ_WISH_MAX_REWARD_NUM = 5
	};

}


#pragma pack(pop)

#endif
