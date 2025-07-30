#ifndef __MSG_RA_FASHION_SHOP_HPP__
#define __MSG_RA_FASHION_SHOP_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/randactivityroleparam.hpp"

#pragma pack(push, 4)

namespace Protocol
{
	struct SCRAFashionShopInfo						//<! 3820  ʱװ�̵�
	{
	public:
		SCRAFashionShopInfo();
		MessageHeader header;

		int ra_buy_times[RA_FASHION_SHOP_MAX_ITEM];				//ÿ����Ʒ�޹�����
	};
}

#pragma pack(pop)

#endif  // __MSG_RA_LUCKY_BAG_HPP__
