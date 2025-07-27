#ifndef __RA_LUCKY_BLIND_BOX_PARAM_HPP__
#define __RA_LUCKY_BLIND_BOX_PARAM_HPP__


#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"
#include "servercommon/bitmapfunction.hpp"

#pragma pack(push, 4)

////////////////////////////////////////////  幸运盲盒 个人数据  //////////////////////////////////////////////////////

const static int MAX_LUCKY_BLIND_BOX_X = 4;
const static int MAX_LUCKY_BLIND_BOX_Y = 3;

const static int MAX_LUCKY_BLIND_BOX_SEQ = 8;
const static int MAX_LUCKY_BLIND_BOX_REWARD_GROUP_NUM = 64;

struct RALuckyBlindBoxParam
{
	RALuckyBlindBoxParam()
	{
		this->Reset();
	}

	void Reset()
	{
		ra_begin_timestamp = 0;
		day_chongzhi = 0;

		bax_refresh_tamp = 0;
		memset(box_flag, -1, sizeof(box_flag));

		memset(today_shop_buy_times, 0, sizeof(today_shop_buy_times));
		memset(shop_buy_times, 0, sizeof(shop_buy_times));

		memset(today_recharge_buy_times, 0, sizeof(today_recharge_buy_times));
		memset(recharge_buy_times, 0, sizeof(recharge_buy_times));

		box_rand_times = 0;
		floor_bitmap.Reset();
	}

	void OnDayChange()
	{
		day_chongzhi = 0;
		memset(today_shop_buy_times, 0, sizeof(today_shop_buy_times));
		memset(today_recharge_buy_times, 0, sizeof(today_recharge_buy_times));
	}

	unsigned int ra_begin_timestamp;
	long long day_chongzhi;
	unsigned int bax_refresh_tamp;
	char box_flag[MAX_LUCKY_BLIND_BOX_X][MAX_LUCKY_BLIND_BOX_Y];

	short today_shop_buy_times[MAX_LUCKY_BLIND_BOX_SEQ];//商城今日购买次数
	short shop_buy_times[MAX_LUCKY_BLIND_BOX_SEQ];//商城活动累积购买次数

	short today_recharge_buy_times[MAX_LUCKY_BLIND_BOX_SEQ];//成就今日领取次数
	short recharge_buy_times[MAX_LUCKY_BLIND_BOX_SEQ];//成就活动领取次数

	int box_rand_times;
	BitMap<MAX_LUCKY_BLIND_BOX_REWARD_GROUP_NUM> floor_bitmap;
};
UNSTD_STATIC_CHECK(sizeof(RALuckyBlindBoxParam) % 4 == 0);
UNSTD_STATIC_CHECK(sizeof(RALuckyBlindBoxParam) <= sizeof(RoleActivityRoleData));
#pragma pack(pop)
#endif