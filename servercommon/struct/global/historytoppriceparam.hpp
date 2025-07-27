#ifndef __HISTORY_TOP_PRICE_PARAM_HPP__
#define __HISTORY_TOP_PRICE_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/struct/itemlistparam.h"

static const int TRADE_MARKET_TOP_PRICE_ITEM_NUM = 6;

#pragma pack(push)
#pragma pack(4)

struct HistoryTopPriceInfo
{
	HistoryTopPriceInfo() : price(0) 
	{
		wrapper.Reset();
	}

	HistoryTopPriceInfo(int _price, const ItemDataWrapper& _wrapper) : price(_price), wrapper(_wrapper) {}
	
	int price;
	ItemDataWrapper wrapper;
};

struct HistoryTopPriceParam
{
	HistoryTopPriceParam()
	{
		this->Reset();
	}

	void Reset()
	{
		memset(reserve_ll, 0, sizeof(reserve_ll));
		memset(price_list, 0, sizeof(price_list));
	}

	long long reserve_ll[4];
	HistoryTopPriceInfo price_list[TRADE_MARKET_TOP_PRICE_ITEM_NUM];
};
typedef char HistoryTopPriceParamHex[sizeof(HistoryTopPriceParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(HistoryTopPriceParamHex) < 13000);

#pragma pack(pop)

#endif