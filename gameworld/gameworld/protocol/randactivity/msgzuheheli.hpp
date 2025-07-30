#ifndef __MSG_RA_ZU_HE_HE_LI_HPP__
#define __MSG_RA_ZU_HE_HE_LI_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/randactivityroleparam.hpp"

//////////////////////////////////////////// 2085  随机活动  定制宝箱  //////////////////////////////////////////

#pragma pack(push, 4)

namespace Protocol
{
	struct SCRAZuHeHeLiInfo						//<! 3630  组合贺礼
	{
	public:
		SCRAZuHeHeLiInfo();
		MessageHeader header;

		unsigned short ra_day_buy_times[RA_ZU_HE_HE_LI_MAX_ITEM];			//大小50，每种物品每天限购次数
		unsigned short ra_buy_times[RA_ZU_HE_HE_LI_MAX_ITEM];				//每种物品限购次数
	};
}

#pragma pack(pop)

#endif  // __MSG_RA_LUCKY_BAG_HPP__
