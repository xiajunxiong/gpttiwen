#include "msgratrademarketreturngold.hpp"
#include "item/itembase.h"

bool Protocol::SCRATradeMarketReturnHotItemInfo::Serialize(char* buffer, int buffer_size, int* out_length)
{
	bool result = true;

	result = result && WRITE_BEGIN(header.msg_type);

	for (int i = 0; i < ARRAY_LENGTH(hot_item_list); ++i)
	{
		if (hot_item_list[i].item_id > 0)
		{
			const ItemBase* item = ITEMPOOL->GetItem(hot_item_list[i].item_id);
			if (NULL == item) continue;

			result = result && WRITE_USHORT(hot_item_list[i].item_id);

			short param_length = static_cast<int>(NetValueItemParam::GetDataLengthByItemType(item->GetItemType()));
			result = result && WRITE_SHORT(param_length);

			result = result && WRITE_STRN(hot_item_list[i].param_data, param_length);
		}
		else
		{
			result = result && WRITE_USHORT(0);
			result = result && WRITE_SHORT(0);
		}
	}

	return result;
}
