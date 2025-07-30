#ifndef __HISTORY_TOP_PRICE_HPP__
#define __HISTORY_TOP_PRICE_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/struct/global/historytoppriceparam.hpp"

class HistoryTopPrice
{
public:
	HistoryTopPrice();
	~HistoryTopPrice();

	void Init(const HistoryTopPriceParam& param);
	void GetInitParam(ARG_OUT HistoryTopPriceParam& param);

	void OnItemSold(int price, const ItemDataWrapper& wrapper);
	void GetTopPriceItemList(int max_list_size, ARG_OUT int* num, ARG_OUT ItemDataWrapper* item_list);

private:
	bool IsListFull();
	bool ReplaceLowestPriceItem(int price, const ItemDataWrapper& wrapper);
	void SetDirty();

	bool m_is_change;
	int m_info_num;
	HistoryTopPriceInfo m_info_list[TRADE_MARKET_TOP_PRICE_ITEM_NUM];
};

#endif