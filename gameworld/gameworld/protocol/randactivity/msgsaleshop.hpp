#ifndef __MSG_RA_SALE_SHOP_HPP__
#define __MSG_RA_SALE_SHOP_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/rasaleshopparam.hpp"

//////////////////////////////////////////// 2059  随机活动  特卖商店  //////////////////////////////////////////

#pragma pack(push, 4)

namespace Protocol
{
	// 由协议  3000
	enum RA_SALE_SHOP_OPERA_TYPE
	{
		SALE_SHOP_TYPE_INFO = 0,				//信息下发	param1
		SALE_SHOP_TYPE_BUY						//购买道具  param1 param 2 param 3
	};


	struct SCRASaleShopInfo						// 3016 特卖商店信息下发
	{
	public:
		SCRASaleShopInfo();
		MessageHeader header;

		int count;															//当前配置所拥有的个数			
		short sale_shop_buy_count[MAX_SALE_SHOP_TYPE_SEQ_NUM];				//玩家单个栏目中各个道具的已购买数量
	};

}

#pragma pack(pop)

#endif  // __MSG_RA_SALE_SHOP_HPP__
