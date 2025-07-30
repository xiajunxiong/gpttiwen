#ifndef __MSG_ZHEN_PIN_SHANG_PU_HPP__
#define __MSG_ZHEN_PIN_SHANG_PU_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/randactivityroleparam.hpp"

//////////////////////////////////////////// 2128  随机活动  珍品商铺  //////////////////////////////////////////

#pragma pack(push, 4)

namespace Protocol
{
	struct SCRAZhenPinShangPu						//<! 4080 珍品商铺信息
	{
	public:
		SCRAZhenPinShangPu();
		MessageHeader header;

		RAZhenPinShangPuBuyTimes day_buy_times[RA_ZHEN_PIN_SHANG_PU_MAX_ITEM];			//每种物品每天限购次数
		RAZhenPinShangPuBuyTimes buy_times[RA_ZHEN_PIN_SHANG_PU_MAX_ITEM];				//每种物品限购次数
	};

	struct SCRAZhenPinShangPuOpenDay						//<! 4081 珍品商铺信息
	{
	public:
		SCRAZhenPinShangPuOpenDay();
		MessageHeader header;

		int day;
	};
}

#pragma pack(pop)

#endif  // __MSG_RA_LUCKY_BAG_HPP__
