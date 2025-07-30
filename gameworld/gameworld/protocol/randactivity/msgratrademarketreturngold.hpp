#ifndef __MSG_RA_TRADE_MARKET_RETURN_GOLD_HPP__
#define __MSG_RA_TRADE_MARKET_RETURN_GOLD_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/ratrademarketreturngoldparam.hpp"

//////////////////////////////////////////// 2102 随机活动  交易所返还活动  //////////////////////////////////////////

#pragma pack(push, 4)

namespace Protocol
{
	// 由协议  3000
	enum RA_TRADE_MARKET_RETURN_GOLD_OPERA_TYPE
	{
		RA_TRADE_MARKET_RETURN_GOLD_OPERA_TYPE_INFO = 0,		//获取信息	
		RA_TRADE_MARKET_RETURN_GOLD_OPERA_TYPE_CALIM_GOLD = 1,	//领取返还元宝
	};

	class SCRATradeMarketReturnGoldInfo			// 3063 交易所返还活动信息下发 
	{
	public:
		SCRATradeMarketReturnGoldInfo();
		MessageHeader header;

		long long total_buy_item_consume_gold;	//!< 总计购买商品消耗元宝数
		long long total_calaimable_gold;		//!< 总计返还元宝数
		long long remain_calaimable_gold;		//!< 剩余可领取元宝数
	};

	class SCRATradeMarketReturnHotItemInfo : public IMessageSerializer	//!< 3064 交易行热卖物品列表
	{
	public:
		SCRATradeMarketReturnHotItemInfo();
		virtual ~SCRATradeMarketReturnHotItemInfo() {}
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length);

		ItemDataWrapper hot_item_list[6];				//!< 热卖物品ID列表 不足6个的话item_id为0
	};
}

#pragma pack(pop)

#endif  // __MSG_RA_HAPPY_UP_HPP__


