#ifndef __RA_TRADE_MARKET_RETURN_GOLD_PARAM_HPP__
#define __RA_TRADE_MARKET_RETURN_GOLD_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"

#pragma pack(push, 4)

struct RATradeMarketReturnGoldParam
{
	RATradeMarketReturnGoldParam()
	{
		this->Reset();
	}

	void Reset()
	{
		ra_begin_timestamp = 0u;
		total_buy_item_consume_gold = 0LL;
		total_claimable_gold = 0LL;
		remain_claimable_gold = 0LL;
		remain_claimable_gold_decimal = 0.0;
	}

	unsigned int ra_begin_timestamp;
	long long total_buy_item_consume_gold;			//总计交易行购买所消耗元宝数
	long long total_claimable_gold;					//总计可领元宝数
	long long remain_claimable_gold;				//剩余可领元宝数
	double remain_claimable_gold_decimal;			//剩余可领元宝数（小数部分）
};

#pragma pack(pop)

#endif