#include "drawdef.hpp"

bool ItemDrawInfo::Serialize(TLVSerializer2 & outstream) const
{
	bool ret = true;
	
	ret = ret && outstream.Push(item_info.item_id);
	ret = ret && outstream.Push(item_info.is_market_buy_flag);
	ret = ret && outstream.Push(item_info.reserve_ch);
	ret = ret && outstream.Push(item_info.num);
	ret = ret && outstream.Push(item_info.is_bind);
	ret = ret && outstream.Push(item_info.has_param);
	ret = ret && outstream.Push(item_info.invalid_time);
	ret = ret && outstream.Push(item_info.gold_price);
	if (0 != item_info.has_param && param_len > 0 && (unsigned int)param_len < sizeof(item_info.param_data))
	{
		TLVSerializer2 param_data;
		param_data.Reset((void *)&item_info.param_data, param_len);
		param_data.MoveCurPos(param_len);
		ret = ret && outstream.Push(param_data);
	}

	ret = ret && outstream.Push(item_info.item_unique_id);

	return ret;
}
