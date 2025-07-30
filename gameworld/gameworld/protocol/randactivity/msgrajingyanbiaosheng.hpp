#ifndef __MSG_RA_JING_YAN_BIAO_SHENG_HPP__
#define __MSG_RA_JING_YAN_BIAO_SHENG_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/randactivityroleparam.hpp"

//////////////////////////////////////////// 2087  随机活动  经验飙升  //////////////////////////////////////////

#pragma pack(push, 4)

namespace Protocol
{
	struct SCRAJingYanBiaoShengInfo						//<! 3410 经验飙升
	{
	public:
		SCRAJingYanBiaoShengInfo();
		MessageHeader header;

		int buy_times;			//<! 购买次数
		int day_buy_times;		//<! 每日购买次数
	};
}

#pragma pack(pop)

#endif  // __MSG_RA_LUCKY_BAG_HPP__
