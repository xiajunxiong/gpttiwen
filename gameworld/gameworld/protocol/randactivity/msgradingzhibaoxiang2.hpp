#ifndef __MSG_RA_DING_ZHI_BAO_XIANG_2_HPP__
#define __MSG_RA_DING_ZHI_BAO_XIANG_2_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/randactivityroleparam.hpp"

//////////////////////////////////////////// 2161  随机活动  定制宝箱  //////////////////////////////////////////

#pragma pack(push, 4)

namespace Protocol
{
	struct SCRADingZhiBaoXiang2Info						//<! 3477 定制宝箱信息
	{
	public:
		SCRADingZhiBaoXiang2Info();
		MessageHeader header;

		unsigned short ra_ding_zhi_bao_xiang_day_buy_times[RA_DING_ZHI_BAO_XIANG_2_MAX_ITEM];			//每种物品每天限购次数
		unsigned short ra_ding_zhi_bao_xiang_buy_times[RA_DING_ZHI_BAO_XIANG_2_MAX_ITEM];				//每种物品限购次数
		BitMap<RA_DING_ZHI_BAO_XIANG_2_BUY_MAX_ITEM> buy_flag;										//购买标识 0:未购买 1:已购买
		int has_free_reward;													//免费宝箱，0-未领取，1-已领取
	};
}

#pragma pack(pop)

#endif  // __MSG_RA_LUCKY_BAG_HPP__
