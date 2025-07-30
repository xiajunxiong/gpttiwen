#ifndef __MSG_RA_DISCOUNT_CARNIVAL_HPP__
#define __MSG_RA_DISCOUNT_CARNIVAL_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/radiscountcarnivalparam.hpp"

//////////////////////////////////////////// 2069  随机活动  折扣狂欢  //////////////////////////////////////////

#pragma pack(push, 4)

namespace Protocol
{
	// 由协议  3000
	enum RA_DISCOUNT_CARNIVAL_OPERA_TYPE
	{
		RA_DISCOUNT_CARNIVAL_OPERA_TYPE_INFO = 0,		//信息下发	
		RA_DISCOUNT_CARNIVAL_OPERA_TYPE_BUY_ITEM = 1,	//商店购买道具 param_1 购买seq   param_2 使用券ID
	};

	struct SCRADiscountCarnivalInfo        // 3030 折扣狂欢信息
	{
	public:
		SCRADiscountCarnivalInfo();
		MessageHeader header;

		unsigned char buy_count[DISCOUNT_CARNIVAL_COMMODITY_MAX_NUM];		//购买次数 DISCOUNT_CARNIVAL_COMMODITY_MAX_NUM 16  
	};

}

#pragma pack(pop)

#endif  // __MSG_RA_DISCOUNT_CARNIVAL_HPP__


