#ifndef __MSG_RA_DISCOUNT_STORE_HPP__
#define __MSG_RA_DISCOUNT_STORE_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/radiscountstoreparam.hpp"

//////////////////////////////////////////// 2052 开服活动 --  折扣商店  //////////////////////////////////////////

#pragma pack(push, 4)

namespace Protocol
{
	// 由协议  3000
	enum RA_DISCOUNT_STORE_OPERA_TYPE
	{
		RA_DISCOUNT_STORE_OPERA_TYPE_GET_INFO = 0,						// 获取信息
		RA_DISCOUNT_STORE_OPERA_TYPE_BUY = 1,							// 转转盘 param_1: 序号 param_2: 数量	
	};


	class SCRandActivityDiscountStoreInfo									//!<3005  获取信息
	{
	public:
		SCRandActivityDiscountStoreInfo();
		MessageHeader header;

		unsigned short ra_discount_store_buy_count[RA_DISCOUNT_STORE_MAX_NUM];
	};

}

#pragma pack(pop)

#endif  // __MSG_RA_TIME_LIMIT_STORE_HPP__
