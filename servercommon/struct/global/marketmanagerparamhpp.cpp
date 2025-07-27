#include "marketmanagerparamhpp.h"

bool MarketManagerParam::Serialize(TLVSerializer & outstream) const
{
	bool ret = true;
	ret = outstream.Push(count);
	if (!ret)
	{
		return false;
	}
	for (int i = 0; i < count && i < ARRAY_LENGTH(market_list); i++)
	{
		ret = outstream.Push(market_list[i].change_state);
		if (!ret)
		{
			return false;
		}

		const MarketItemInfo & market_item_data = market_list[i].market_info;
		ret = outstream.Pushf(
			"hiiiki",
			market_item_data.item_id, market_item_data.cur_price, market_item_data.yesterday_price, market_item_data.transaction_num, market_item_data.transaction_count, market_item_data.version
		);
		if (!ret)
		{
			return false;
		}
	}
	return ret;
}

bool MarketManagerParam::Unserialize(TLVUnserializer & instream)
{
	bool ret = true;
	ret = instream.Pop(&count);
	if (!ret)
	{
		return false;
	}
	for (int i = 0; i < count && i < ARRAY_LENGTH(market_list); i++)
	{
		ret = instream.Pop(&market_list[i].change_state);
		if (!ret)
		{
			return false;
		}

		MarketItemInfo & market_item_data = market_list[i].market_info;
		ret = instream.Popf(
			"hiiiki",
			&market_item_data.item_id, &market_item_data.cur_price, &market_item_data.yesterday_price, &market_item_data.transaction_num, 
			&market_item_data.transaction_count, &market_item_data.version
		);
		if (!ret)
		{
			return false;
		}
	}

	return ret;
}
