#ifndef __RA_SECRET_TREASURE_PARAM_HPP__
#define __RA_SECRET_TREASURE_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"

#pragma pack(push, 4)

static const int MAX_SECRET_TREASURE_SHOP_ITEM_NUM = 100;				// 活动出售道具数目最大值
static const int MAX_SECRET_TREASURE_PHASE_SHOP_ITEM_NUM = 25;			// 阶段出售道具数目最大值

struct RASecretTreasureParam
{
	RASecretTreasureParam() { this->Reset(); }

	void Reset()
	{
		ra_begin_timestamp = 0;
		phase = 0;
		is_open_today_flag = 0;
		memset(today_buy_times, 0, sizeof(today_buy_times));
		memset(activity_buy_times, 0, sizeof(activity_buy_times));
	}

	unsigned int ra_begin_timestamp;
	short phase;															//阶段
	short is_open_today_flag;												//今天是否打开过界面 0:没打开 1：打开过
	short today_buy_times[MAX_SECRET_TREASURE_PHASE_SHOP_ITEM_NUM];			//每日限购
	short activity_buy_times[MAX_SECRET_TREASURE_SHOP_ITEM_NUM];			//活动限购
};
UNSTD_STATIC_CHECK(sizeof(RASecretTreasureParam) <= sizeof(RoleActivityRoleData));

#pragma pack(pop)

#endif	//__RA_SECRET_TREASURE_PARAM_HPP__