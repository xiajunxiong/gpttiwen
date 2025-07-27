#ifndef __LOGIN_SYSTEM_DEF_HPP__
#define __LOGIN_SYSTEM_DEF_HPP__

#include <string.h>
#include "servercommon/servercommon.h"
#include "servercommon/bitmapfunction.hpp"
#pragma pack(push, 4)

static const int MAX_LOGIN_SYSTEM_NUM = 32;
UNSTD_STATIC_CHECK(sizeof(int) * 8 == MAX_LOGIN_SYSTEM_NUM);

struct LoginSystemParam
{
	LoginSystemParam() { this->Reset(); }

	void Reset()
	{
		login_day = 0;
		next_add_login_day_tamp = 0;
		reward_flag = 0;
		unlock_reward_flag.Reset();
		is_reward_flag.Reset();
	}
	//---------28天累计登陆
	int login_day;//累计登陆天数
	long long next_add_login_day_tamp;//下一次可累计登陆天数时间戳
	int reward_flag;//奖励领取flag - 32位(弃用)
	BitMap<MAX_LOGIN_SYSTEM_NUM> unlock_reward_flag;	// 奖励解锁flag
	BitMap<MAX_LOGIN_SYSTEM_NUM> is_reward_flag;		// 是否已领取奖励
};

typedef char LoginSystemParamHex[sizeof(LoginSystemParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(LoginSystemParamHex) < 256);

#pragma pack(pop)

#endif  // __MENTALITY_DEF_HPP__

