#ifndef __MSG_RA_ELEMENT_CARDS_HPP__
#define __MSG_RA_ELEMENT_CARDS_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "protocol/randactivity/msgradiscountstore.hpp"

//////////////////////////////////////////// 2082 开服活动 --  元素翻牌  //////////////////////////////////////////

#pragma pack(push, 4)

namespace Protocol
{
	// 由协议  3000
	enum RA_ELEMENT_CARDS_OP_TYPE
	{
		RA_ELEMENT_CARDS_OP_TYPE_INFO = 0,						// 信息下发
		RA_ELEMENT_CARDS_OP_TYPE_FLOP = 1,						// 翻牌
	};

	struct SCRAElementCardsInfo									//!<　3040 元素翻牌数据下发
	{
	public:
		SCRAElementCardsInfo();
		MessageHeader header;

		short flop_seq;
		short flop_count;
	};
}

#pragma pack(pop)

#endif  // __MSG_RA_ELEMENT_CARDS_HPP__
