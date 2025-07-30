#ifndef __MSG_RA_PET_EXCHANGE_HPP__
#define __MSG_RA_PET_EXCHANGE_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/rapetexchangeparam.hpp"

//////////////////////////////////////////// 2072  随机活动  宠物兑换  //////////////////////////////////////////

#pragma pack(push, 4)

namespace Protocol
{
	// 由协议  3000
	enum RA_PET_EXCHANGE_OPERA_TYPE
	{
		RA_PET_EXCHANGE_OPERA_TYPE_GET_INFO = 0,		//获取信息
		RA_PET_EXCHANGE_OPERA_TYPE_EXCHANGE = 1,		//兑换 param1:seq
		RA_PET_EXCHANGE_OPERA_TYPE_BUY_ITEM = 2,		//商店购物消费 param1:seq
		RA_PET_EXCHANGE_OPERA_TYPE_BUY_ITEM_CHECK = 3,	//商店购买直购物品检测 param1:seq
	};


	class SCRAActivityPetExchangeInfo			//!< 1234 宠物兑换活动下发
	{
	public:
		SCRAActivityPetExchangeInfo();
		MessageHeader header;

		unsigned short ra_pet_exchange_times[MAX_RA_PET_NUM];			//活动已兑换次数
		unsigned short ra_pet_exchange_today_times[MAX_RA_PET_NUM];		//每日已兑换次数
		int ra_shop_buy_times[MAX_RA_PET_EXCHANGE_SHOP_ITEM_NUM];		//商店已购买次数(每日与活动共用)
	};

	class SCRAPetExchangeSingleInfo				//!< 3192 随机活动-宠物兑换单条信息下发
	{
	public:
		SCRAPetExchangeSingleInfo();
		MessageHeader header;

		enum INFO_TYPE
		{
			INFO_TYPE_EXCHANGE = 0,			//兑换		param1:活动已兑换次数 param2:每日已兑换次数
			INFO_TYPE_BUY_ITEM = 1,			//商店购买	param1:已购买次数
		};

		short type;
		short seq;
		int param1;
		int param2;
	};

}

#pragma pack(pop)

#endif  // __MSG_RA_PET_EXCHANGE_HPP__
