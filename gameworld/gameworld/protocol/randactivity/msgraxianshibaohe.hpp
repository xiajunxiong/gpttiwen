#ifndef __MSG_RA_XIAN_SHI_BAO_HE_HPP__
#define __MSG_RA_XIAN_SHI_BAO_HE_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/randactivityroleparam.hpp"

//////////////////////////////////////////// 2088  随机活动  限时宝盒  //////////////////////////////////////////

#pragma pack(push, 4)

namespace Protocol
{
	struct SCRAXianShiBaoHeInfo						//<! 3420 限时宝盒
	{
	public:
		SCRAXianShiBaoHeInfo();
		MessageHeader header;

		unsigned short day_buy_times[RA_XIAN_SHI_BAO_HE_MAX_ITEM];			//每种物品每天限购次数
		unsigned short buy_times[RA_XIAN_SHI_BAO_HE_MAX_ITEM];				//每种物品限购次数
		BitMap<RA_XIAN_SHI_BAO_HE_BUY_MAX_ITEM> buy_flag;					//购买标识 0:未购买 1:已购买
		int has_free_reward;													//免费宝箱，0-未领取，1-已领取
	};
}

#pragma pack(pop)

#endif  // __MSG_RA_LUCKY_BAG_HPP__
