#ifndef __RA_SHAN_HAI_BAO_HE_PARAM_HPP__
#define __RA_SHAN_HAI_BAO_HE_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/bitmapfunction.hpp"
#include "servercommon/roleactivitydef.hpp"

//////////////////////////////////////////// 2158 随机活动 - 山海宝盒  //////////////////////////////////////////

const int static RA_SHAN_HAI_BAO_HE_MAX_ITEM = 20;
const int static RA_SHAN_HAI_BAO_HE_BUY_MAX_ITEM = 32;
UNSTD_STATIC_CHECK(RA_SHAN_HAI_BAO_HE_BUY_MAX_ITEM >= RA_SHAN_HAI_BAO_HE_MAX_ITEM);

#pragma pack(push, 4)
////////////////////////////////////////////  服务器数据  //////////////////////////////////////////////////////


////////////////////////////////////////////  个人数据  //////////////////////////////////////////////////////

struct RAShanHaiBaoHeParam
{
	RAShanHaiBaoHeParam() { this->Reset(); }
	void Reset()
	{
		ra_begin_timestamp = 0;
		has_free_reward = 0;

		for (int i = 0; i < RA_SHAN_HAI_BAO_HE_MAX_ITEM; ++i)
		{
			ra_day_buy_times[i] = 0;
			ra_buy_times[i] = 0;
		}
		buy_flag.Reset();
	}

	unsigned int ra_begin_timestamp;
	unsigned short ra_day_buy_times[RA_SHAN_HAI_BAO_HE_MAX_ITEM];			//每种物品每天限购次数
	unsigned short ra_buy_times[RA_SHAN_HAI_BAO_HE_MAX_ITEM];				//每种物品限购次数
	BitMap<RA_SHAN_HAI_BAO_HE_BUY_MAX_ITEM> buy_flag;						//购买标识 0:未购买 1:已购买
	int has_free_reward;													//免费宝箱，0-未领取，1-已领取
};

UNSTD_STATIC_CHECK(sizeof(RAShanHaiBaoHeParam) <= sizeof(RoleActivityRoleData))

#pragma pack(pop)

#endif	//__RA_SHAN_HAI_BAO_HE_PARAM_HPP__
