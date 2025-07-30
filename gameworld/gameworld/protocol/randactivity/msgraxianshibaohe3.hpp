#ifndef __MSG_RA_XIAN_SHI_BAO_HE_3_HPP__
#define __MSG_RA_XIAN_SHI_BAO_HE_3_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/randactivityroleparam.hpp"

//////////////////////////////////////////// 2211  随机活动  限时宝盒3  //////////////////////////////////////////

#pragma pack(push, 4)

namespace Protocol
{
	struct SCRAXianShiBaoHe3Info						//<! 5140 限时宝盒3 四季花牌 
	{
	public:
		SCRAXianShiBaoHe3Info();
		MessageHeader header;

		unsigned short day_buy_times[RA_XIAN_SHI_BAO_HE_3_MAX_ITEM];		//<! 每种物品每天限购次数
		unsigned short buy_times[RA_XIAN_SHI_BAO_HE_3_MAX_ITEM];			//<! 每种物品限购次数
		int has_free_reward;						//<! 免费宝箱，0-未领取，1-已领取
	};
}

#pragma pack(pop)

#endif  // __MSG_RA_LUCKY_BAG_HPP__
