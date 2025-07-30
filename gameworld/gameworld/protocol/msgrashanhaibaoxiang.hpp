#ifndef __MSG_RA_SHAN_HAI_BAO_XIANG_HPP__
#define __MSG_RA_SHAN_HAI_BAO_XIANG_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/rashanhaibaoxiangparam.hpp"

//////////////////////////////////////////// 2157  随机活动  山海宝箱  //////////////////////////////////////////

#pragma pack(push, 4)

namespace Protocol
{
	struct SCRAShanHaiBaoXiangInfo						//<! 3431 山海宝箱信息
	{
	public:
		SCRAShanHaiBaoXiangInfo();
		MessageHeader header;

		unsigned short ra_shan_hai_bao_xiang_day_buy_times[RA_SHAN_HAI_BAO_XIANG_MAX_ITEM];			//每种物品每天限购次数
		unsigned short ra_shan_hai_bao_xiang_buy_times[RA_SHAN_HAI_BAO_XIANG_MAX_ITEM];				//每种物品限购次数
		BitMap<RA_SHAN_HAI_BAO_XIANG_BUY_MAX_ITEM> buy_flag;										//购买标识 0:未购买 1:已购买
		int has_free_reward;													//免费宝箱，0-未领取，1-已领取
	};
}

#pragma pack(pop)

#endif  // __MSG_RA_SHAN_HAI_BAO_XIANG_HPP__
