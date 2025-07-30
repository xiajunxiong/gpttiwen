#ifndef __MSG_RA_GIVE_PET_DRAW_HPP__
#define __MSG_RA_GIVE_PET_DRAW_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/ragivepetdrawparam.hpp"

//////////////////////////////////////////// 2090 随机活动 - 赠30连抽（送寻宠连抽）  //////////////////////////////////////////

#pragma pack(push, 4)

namespace Protocol
{
	// 由协议  3000
	enum RA_GIVE_PET_DRAW_OPERA_TYPE
	{
		RA_GIVE_PET_DRAW_OPERA_TYPE_INFO = 0,					//信息下发	
		RA_GIVE_PET_DRAW_OPERA_TYPE_RECEIVE = 1,				//领取
	};

	struct SCRAGivePetDrawInfo        // 3042 赠30连抽信息
	{
	public:
		SCRAGivePetDrawInfo();
		MessageHeader header;

		int give_mark;				//0:未领取 1:已领取
	};
}

#pragma pack(pop)

#endif  // __MSG_RA_GIVE_PET_DRAW_HPP__



