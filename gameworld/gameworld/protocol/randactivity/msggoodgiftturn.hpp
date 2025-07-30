#ifndef __MSG_GOOD_GIFT_TURN_HPP__
#define __MSG_GOOD_GIFT_TURN_HPP__

#include "servercommon/userprotocal/msgheader.h"

#pragma pack(push, 4)

namespace Protocol
{
	// 由协议 3000
	enum RA_GOOD_GIFT_TURN_REQ_TYPE
	{
		RA_GOOD_GIFT_TURN_REQ_TYPE_INFO = 0,			// 请求信息下发
		RA_GOOD_GIFT_TURN_REQ_TYPE_FETCH = 1,			// 请求领取奖励
	};

	class SCRAGoodGiftTurnInfo				//!< 3070 随机活动-好礼转动信息下发
	{
	public:
		SCRAGoodGiftTurnInfo();
		MessageHeader header;

		short fetch_num;					// 剩余领取次数
		short fetch_flag;					// 领取奖励标识(二进制) 
	};

	class SCRAGoodGiftTurnRewardInfo		//!< 3071 随机活动-好礼转动抽取信息
	{
	public:
		SCRAGoodGiftTurnRewardInfo();
		MessageHeader header;

		int index;						//对应配置的seq1
	};
}

#pragma pack(pop)

#endif	// __MSG_GOOD_GIFT_TURN_HPP__