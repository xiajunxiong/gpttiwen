#ifndef __MSG_RA_HAPPY_UP_HPP__
#define __MSG_RA_HAPPY_UP_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/rahappyupparam.hpp"

//////////////////////////////////////////// 2060  随机活动  欣欣向荣|印记兑换  //////////////////////////////////////////

#pragma pack(push, 4)

namespace Protocol
{
	// 由协议  3000
	enum RA_HAPPY_UP_OPERA_TYPE
	{
		RA_HAPPY_UP_OPERA_TYPE_INFO = 0,				//信息下发	
		RA_HAPPY_UP_OPERA_TYPE_EXCHANGE_ITEM = 1,		//商店兑换道具 param_1:seq   param_2:count
	};


	struct SCRAHappyUpInfo								// 3017 欣欣向荣信息下发
	{
	public:
		SCRAHappyUpInfo();
		MessageHeader header;

		int count;															//当前配置所拥有的个数			
		short happy_up_exchange_count[MAX_HAPPY_UP_ITEM_SEQ_NUM];			//玩家兑换各个道具的数量
	};

}

#pragma pack(pop)

#endif  // __MSG_RA_HAPPY_UP_HPP__
