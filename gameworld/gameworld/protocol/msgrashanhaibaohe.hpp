#ifndef __MSG_RA_SHAN_HAI_BAO_HE_HPP__
#define __MSG_RA_SHAN_HAI_BAO_HE_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/rashanhaibaoheparam.hpp"

//////////////////////////////////////////// 2158  随机活动  山海宝盒  //////////////////////////////////////////

#pragma pack(push, 4)

namespace Protocol
{
	struct SCRAShanHaiBaoHeInfo						//<! 3432 山海宝盒信息
	{
	public:
		SCRAShanHaiBaoHeInfo();
		MessageHeader header;

		unsigned short day_buy_times[RA_SHAN_HAI_BAO_HE_MAX_ITEM];			//每种物品每天限购次数
		unsigned short buy_times[RA_SHAN_HAI_BAO_HE_MAX_ITEM];				//每种物品限购次数
		BitMap<RA_SHAN_HAI_BAO_HE_BUY_MAX_ITEM> buy_flag;					//购买标识 0:未购买 1:已购买
		int has_free_reward;												//免费宝箱，0-未领取，1-已领取
	};
}

#pragma pack(pop)

#endif  // __MSG_RA_SHAN_HAI_BAO_HE_HPP__
