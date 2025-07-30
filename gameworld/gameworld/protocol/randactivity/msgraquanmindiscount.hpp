#ifndef __MSG_RA_QUAN_MIN_DISCOUNT_HPP__
#define __MSG_RA_QUAN_MIN_DISCOUNT_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/raquanmindiscountparam.hpp"

//////////////////////////////////////////// 2078  随机活动  全民团购  //////////////////////////////////////////

#pragma pack(push, 4)

namespace Protocol
{
	// 由协议  3000
	enum RA_QUAN_MIN_DISCOUNT_OPERA_TYPE
	{
		RA_QUAN_MIN_DISCOUNT_OPERA_TYPE_GET_INFO = 0,		//获取信息
		RA_QUAN_MIN_DISCOUNT_OPERA_TYPE_BUY_ITEM = 1,		//购买道具 param1 购买seq
	};

	struct QuanMinDiscountCommodity
	{
		QuanMinDiscountCommodity() { this->Reset(); }

		void Reset()
		{
			discount = 0;
			buy_count = 0;
			all_buy_count = 0;
		}
		char discount;
		unsigned char buy_count;				// 玩家购买次数
		short all_buy_count;					// 此商品一共购买次数
	};

	struct SCRAQuanMinDiscountInfo			//<! 3036 全民团购信息
	{
	public:
		SCRAQuanMinDiscountInfo();
		MessageHeader header;

		int count;
		QuanMinDiscountCommodity commodity_info[QUAN_MIN_DISCOUNT_COMMODITY_MAX_NUM];
	};


}

#pragma pack(pop)

#endif  // __MSG_RA_QUAN_MIN_DISCOUNT_HPP__
