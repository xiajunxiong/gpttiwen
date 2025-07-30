#ifndef __MSG_RA_LUCKY_DISCOUNT_HPP__
#define __MSG_RA_LUCKY_DISCOUNT_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/raluckydiscountparam.hpp"

//////////////////////////////////////////// 2071  随机活动  幸运折扣  //////////////////////////////////////////

#pragma pack(push, 4)

namespace Protocol
{
	// 由协议  3000
	enum RA_LUCKY_DISCOUNT_OPERA_TYPE
	{
		RA_LUCKY_DISCOUNT_OPERA_TYPE_INFO = 0,					//信息下发	
		RA_LUCKY_DISCOUNT_OPERA_TYPE_GET_DISCOUNT = 1,			//抽取折扣
		RA_LUCKY_DISCOUNT_OPERA_TYPE_BUY_ITEM = 2,				//购买道具 param_1-购买seq param_2-num
	};

	struct SCRALuckyDiscountInfo        // 3031 幸运折扣信息
	{
	public:
		SCRALuckyDiscountInfo();
		MessageHeader header;

		int this_discount;											//此次折扣
		char bug_count[LUCKY_DISCOUNT_COMMODITY_MAX_NUM];			//购买次数 LUCKY_DISCOUNT_COMMODITY_MAX_NUM 24 
		char today_buy_count[LUCKY_DISCOUNT_COMMODITY_MAX_NUM];				//每日购买次数
	};
}

#pragma pack(pop)

#endif  // __MSG_RA_LUCKY_DISCOUNT_HPP__


