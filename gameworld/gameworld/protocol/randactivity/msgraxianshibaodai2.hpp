#ifndef __MSG_RA_XIAN_SHI_BAO_DAI_2_HPP__
#define __MSG_RA_XIAN_SHI_BAO_DAI_2_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/randactivityroleparam.hpp"

//////////////////////////////////////////// 2163  随机活动  限时宝袋  //////////////////////////////////////////

#pragma pack(push, 4)

namespace Protocol
{
	struct SCRAXianShiBaoDai2Info						//<! 3479 限时宝袋信息
	{
	public:
		SCRAXianShiBaoDai2Info();
		MessageHeader header;

		int can_buy_seq;
		unsigned short ra_day_buy_times[RA_XIAN_SHI_BAO_DAI_MAX_ITEM];			//每种物品每天限购次数
		unsigned short ra_buy_times[RA_XIAN_SHI_BAO_DAI_MAX_ITEM];				//每种物品限购次数
		BitMap<RA_XIAN_SHI_BAO_DAI_BUY_MAX_ITEM> buy_flag;						//购买标识 0:未购买 1:已购买
		int has_free_reward;													//免费宝箱，0-未领取，1-已领取
	};
}

#pragma pack(pop)

#endif  // __MSG_RA_LUCKY_BAG_HPP__
