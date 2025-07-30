#ifndef __MSG_RA_HERITAGE_TREASURE_HPP__
#define __MSG_RA_HERITAGE_TREASURE_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/raheritagetreasureparam.hpp"

//////////////////////////////////////////// 2057 随机活动  -- 传承瑰宝  //////////////////////////////////////////

#pragma pack(push, 4)

namespace Protocol
{
	// 由协议  3000
	enum RA_HERITAGE_TREASURE_OPERA_TYPE
	{
		RA_HERITAGE_TREASURE_OPERA_TYPE_GET_INFO = 0,							// 获取信息
		RA_HERITAGE_TREASURE_OPERA_TYPE_ACTIVE_CLUES = 1,						// 激活线索 ,param1: 碎片index
		RA_HERITAGE_TREASURE_OPERA_TYPE_FETCH_REWARD = 2,						// 领取奖励 ,param1: 碎片index
		RA_HERITAGE_TREASURE_OPERA_TYPE_FETCH_FINALE_REWARD = 3,				// 领取最后奖励奖励 

	};


	struct SCRAHeritageTreasureInfo			//<! 3013 传承瑰宝信息
	{
	public:
		SCRAHeritageTreasureInfo();
		MessageHeader header;

		unsigned char ra_heritage_treasure_piece[RA_HERITAGE_TREASURE_MAX_PIECE];	 //对应线索seq
		unsigned char ra_heritage_treasure_clue[RA_HERITAGE_TREASURE_MAX_PIECE];	 //激活了多少个线索，255是领取了奖励
		unsigned char ra_heritage_treasure_is_fetch_final_reward;					//最终奖励领取标志
		unsigned char ra_heritage_treasure_reserved_ch;
	};
}

#pragma pack(pop)

#endif  // __MSG_RA_HERITAGE_TREASURE_HPP__
