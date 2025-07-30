#ifndef __MSG_RA_XIN_FU_LING_CHONG_HPP__
#define __MSG_RA_XIN_FU_LING_CHONG_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/randactivityroleparam.hpp"

//////////////////////////////////////////// 2217  随机活动  新服灵宠 //////////////////////////////////////////

#pragma pack(push, 4)

namespace Protocol
{
	// 由协议  3000
	enum RA_XIN_FU_LING_CHONG_OPERA_TYPE
	{
		RA_XIN_FU_LING_CHONG_OPERA_TYPE_INFO = 0,					//获取信息
		RA_XIN_FU_LING_CHONG_OPERA_TYPE_BUY_ITEM = 1,				//购买道具
	};

	struct SCRAXinFuLingChongInfo					//<! 5220 新服灵宠信息下发
	{
	public:
		SCRAXinFuLingChongInfo();
		MessageHeader header;

		int buy_count;								//购买次数
	};
}

#pragma pack(pop)

#endif  // __MSG_RA_LUCKY_BAG_HPP__
