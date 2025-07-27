#ifndef __DAY_REWARD_DEF_HPP__
#define __DAY_REWARD_DEF_HPP__

#include "servercommon/serverdef.h"
#include "servercommon/servercommon.h"
#include "servercommon/bitmapfunction.hpp"

#pragma pack(push, 4)

static const int DAY_REWARD_ONLINE_GIFT_NUM = 32;		//在线礼包最大数量

struct RoleDayRewardParam
{
	RoleDayRewardParam()
	{
		this->Reset();
	}

	void Reset()
	{
		memset(reserve_ll, 0, sizeof(reserve_ll));
		is_old_user_flag = 0;
		is_can_join_online_gift_flag = 0;
		reserve_sh = 0;

		online_gift_join_begin_timestamp = 0;
		online_gift_record_begin_timestamp = 0;
		online_gift_times = 0;
		online_gift_flag.Reset();
	}
	long long reserve_ll[3];
	char is_old_user_flag;								// 是否判断过老号标识 0:未判断 1:已判断
	char is_can_join_online_gift_flag;					// 是否具有资格参加在线礼包 0:木有资格 
	short reserve_sh;

	unsigned int online_gift_join_begin_timestamp;		// 开始参与在线礼包时间戳
	unsigned int online_gift_record_begin_timestamp;	// 开始记录在线礼包时间戳
	unsigned int online_gift_times;						// 累计在线时长　　总累计时间为：(now-online_gift_record_begin_timestamp +online_gift_times)

	BitMap<DAY_REWARD_ONLINE_GIFT_NUM> online_gift_flag;// 在线礼包领取标识
};

typedef char RoleDayRewardParamHex[sizeof(RoleDayRewardParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(RoleDayRewardParamHex) < 128);

#pragma pack(pop)
#endif
