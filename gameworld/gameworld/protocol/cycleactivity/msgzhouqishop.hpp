#ifndef __MSG_ZHOU_QI_SHOP_HPP__
#define __MSG_ZHOU_QI_SHOP_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/cycleactivity/zhouqishopparam.hpp"

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{
	enum ZHOU_QI_SHOP_REQ_TYPE
	{
		ZHOU_QI_SHOP_REQ_TYPE_BUY,	//<! p1 seq
	};

	class CSZhouQiShopReq				// 5160
	{
	public:
		CSZhouQiShopReq();
		MessageHeader header;

		int req_type;
		int p1;
	};

	class SCZhouQiShopInfo			//!< 5161
	{
	public:
		SCZhouQiShopInfo();
		MessageHeader header;

		unsigned short store_list[ZHOU_QI_SHOP_ITEM_MAX_NUM];
	};
}
#pragma pack(pop)

#endif