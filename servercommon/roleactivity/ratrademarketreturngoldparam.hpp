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
	long long total_buy_item_consume_gold;			//�ܼƽ����й���������Ԫ����
	long long total_claimable_gold;					//�ܼƿ���Ԫ����
	long long remain_claimable_gold;				//ʣ�����Ԫ����
	double remain_claimable_gold_decimal;			//ʣ�����Ԫ������С�����֣�
};

#pragma pack(pop)

#endif