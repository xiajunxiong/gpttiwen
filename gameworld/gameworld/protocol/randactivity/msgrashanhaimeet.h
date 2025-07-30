#ifndef __MSG_RA_SHAN_HAI_MEET_HPP__
#define __MSG_RA_SHAN_HAI_MEET_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/rashanhaimeetparam.hpp"

//////////////////////////////////////////// 2140  山海相逢  //////////////////////////////////////////

#pragma pack(push, 4)

namespace Protocol
{
	// 由协议  3000
	enum RA_SHAN_HAI_MEET_OPERA_TYPE
	{
		RA_SHAN_HAI_MEET_TYPE_GET_INFO = 0,				//请求信息下发
		RA_SHAN_HAI_MEET_OPERA_TYPE_FETCH = 1,			//请求领取奖励 param1 - seq
		RA_SHAN_HAI_MEET_OPERA_TYPE_POKER = 2,			//请求翻牌	   param1 - index0-5
		RA_SHAN_HAI_MEET_OPERA_TYPE_BUY = 3,			//请求购买
		RA_SHAN_HAI_MEET_OPERA_RESET_POKER = 4,			//请求重置翻牌
	};

	struct SCRAShanHaiMeetInfo							//<! 3110 山海相逢信息
	{
	public:
		SCRAShanHaiMeetInfo();
		MessageHeader header;

		char			is_buy;												// 1:已购买 0:未购买
		char			sh_server;
		short			ch_server;

		BitMap<RA_SHAN_HAI_MEET_POKER_MAX_INDEX> poker_idx_flag;			// 翻牌下标标志位 RA_SHAN_HAI_MEET_POKER_MAX_INDEX-6
		BitMap<RA_SHAN_HAI_MEET_TASK_ID_MAX> fetch_flag;					// 领取seq标志位 RA_SHAN_HAI_MEET_TASK_ID_MAX-32

		int poker_item_seq[RA_SHAN_HAI_MEET_POKER_MAX_INDEX];				// 翻牌奖励索引-6

		int				count;
		int				finish_count[RA_SHAN_HAI_MEET_ACT_TYPE_MAX];
	};

}

#pragma pack(pop)

#endif  // __MSG_RA_SHAN_HAI_MEET_HPP__
