#ifndef __MSG_RA_FASHION_SHOP_HPP__
#define __MSG_RA_FASHION_SHOP_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/randactivityroleparam.hpp"

#pragma pack(push, 4)

namespace Protocol
{
	struct SCRAFashionShopInfo						//<! 3820  时装商店
	{
	public:
		SCRAFashionShopInfo();
		MessageHeader header;

		int ra_buy_times[RA_FASHION_SHOP_MAX_ITEM];				//每种物品限购次数
	};
}

#pragma pack(pop)

#endif  // __MSG_RA_LUCKY_BAG_HPP__
