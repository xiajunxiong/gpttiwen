#include "itemgriddata.h"

#include "item/itembase.h"
#include "item/itempool.h"
#include "item/itemextern.hpp"

#include "equipment/equipment.h"
#include "engineadapter.h"

#include "servercommon/configcommon.h"
#include "gameworld/gamelog.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool ItemConfigData::ReadConfig(TiXmlElement *element)
{
	if (!GetSubNodeValue(element, "item_id", item_id))
	{	
		gamelog::g_log_world.printf(LL_ERROR,"ItemConfigData::ReadConfig Error GetSubNodeValue item[id:%d]" , item_id);
		return false;
	}

	if (!GetSubNodeValue(element, "is_bind", is_bind))
	{	
		gamelog::g_log_world.printf(LL_ERROR, "ItemConfigData::ReadConfig Error GetSubNodeValue item[id:%d is_bind:%d ]", item_id,is_bind);
		return false;
	}

	if (!GetSubNodeValue(element, "num", num) || num <= 0)
	{	
		gamelog::g_log_world.printf(LL_ERROR, "ItemConfigData::ReadConfig Error GetSubNodeValue item[id:%d is_bind:%d num:%d]", item_id, is_bind, num);
		return false;
	}
	
	std::string errormsg;
	if (!this->CheckConfig(errormsg))
	{
		gamelog::g_log_world.printf(LL_ERROR, "ItemConfigData::ReadConfig CheckConfig Failed! item[id:%d is_bind:%d num:%d] reason[%s]", item_id, is_bind, num, errormsg.c_str());
		return false;
	}

	return true;
}

bool ItemConfigData::CheckConfig(std::string& errmsg) const
{
	const ItemBase* item = ITEMPOOL->GetItem(item_id);
	if (NULL == item)
	{
		errmsg = STRING_SPRINTF("Item[%d] Not Found In ItemPool", item_id);
		return false;
	}

	if (num <= 0)
	{
		errmsg = STRING_SPRINTF("Item[%d] Num <= 0 ", item_id);
		return false;
	}

	if (!item->CanMerge() && num > 1)
	{
		errmsg = STRING_SPRINTF("Item[id:%d is_bind:%d num:%d] CanMerge()==False And Num > 1 ", item_id, is_bind, num);
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ItemGridData::Clear()
{
	if (NULL != param)
	{
		delete param;
		param = NULL;
	}
	this->Reset();
}

bool ItemGridData::Set(const ItemDataWrapper &item_data_wrapper)
{
	if (item_data_wrapper.num <= 0) return false;
	if (NULL != param) return false;		// 绝对不允许

	const ItemBase *item = ITEMPOOL->GetItem(item_data_wrapper.item_id);
	if (NULL == item) return false;

	item_id = item_data_wrapper.item_id;
	is_bind = (0 != item_data_wrapper.is_bind);
	invalid_time = item_data_wrapper.invalid_time;
	gold_price = item_data_wrapper.gold_price;
	item_unique_id = item_data_wrapper.item_unique_id;
	is_market_buy_flag = item_data_wrapper.is_market_buy_flag;
	
	if (!is_bind && item->IsBind()) is_bind = true;

	if (item->GetItemType() < I_TYPE_NET_VALUE_ITEM_BEGIN)
	{
		if (item_data_wrapper.num >= MAX_INT16)
		{
			num = MAX_INT16;
		}
		else
		{
			num = item_data_wrapper.num;
		}
	}
	else
	{
		num = 1; // 净值物品不允许叠加 

		if (!item_data_wrapper.has_param)
		{
			param = item->CreateParam();
		}
		else
		{
			param = item->CreateParamFromParamData(item_data_wrapper.param_data);
		}
	}

	return true;
}

bool ItemGridData::WrapTo(ItemDataWrapper *item_data_wrapper) const
{
	if (NULL == item_data_wrapper) return false;

	if (NULL != param)
	{
		const ItemBase* item = ITEMPOOL->GetItem(item_id);
		if (NULL == item || item->GetItemType() < I_TYPE_NET_VALUE_ITEM_BEGIN) return false;

		memcpy(item_data_wrapper->param_data, param->data, sizeof(NetValueItemParamData));
	}
	else
	{
		memset(item_data_wrapper->param_data, 0, sizeof(item_data_wrapper->param_data));
	}

	item_data_wrapper->item_id = item_id;
	item_data_wrapper->num = num;
	item_data_wrapper->is_bind = (short)is_bind;
	item_data_wrapper->invalid_time = invalid_time;
	item_data_wrapper->has_param = (NULL != param) ? 1 : 0;
	item_data_wrapper->gold_price = gold_price;
	item_data_wrapper->item_unique_id = item_unique_id;
	item_data_wrapper->is_market_buy_flag = is_market_buy_flag;

	return true;
}

bool ItemGridData::WrapTo(ARG_OUT ItemGridDataGetter* getter) const
{
	if (NULL == getter) return false;

	if (NULL != param)
	{
		const ItemBase* item = ITEMPOOL->GetItem(item_id);
		if (NULL == item || item->GetItemType() < I_TYPE_NET_VALUE_ITEM_BEGIN) return false;

		getter->param = *param;
	}
	else
	{
		getter->param.Reset();
	}

	getter->item_id = item_id;
	getter->num = num;
	getter->is_bind = (short)is_bind;
	getter->invalid_time = invalid_time;
	getter->gold_price = gold_price;
	getter->item_unique_id = item_unique_id;
	getter->is_market_buy_flag = is_market_buy_flag;

	return true;
}

bool ItemGridData::CanBeMerge(bool ignore_invalid_time /* = false */) const
{
	const ItemBase *itembase = ITEMPOOL->GetItem(item_id);
	if (NULL == itembase) return false;

	if (num <= 0 || num >= itembase->GetPileLimit() || !itembase->CanMerge() || NULL != param || (!ignore_invalid_time && invalid_time > 0))
	{
		return false;
	}

	return true;
}

int ItemGridData::Merge(const ItemDataWrapper &item_data_wrapper, int *rest_num, int *merged_gold_price, bool ignore_bind)
{
	const ItemBase *itembase = ITEMPOOL->GetItem(item_id);
	if (NULL == itembase) return false;

	if (item_id != item_data_wrapper.item_id
		|| !this->CanMergeWith(item_data_wrapper)
		|| (!ignore_bind && (is_bind != (0 != item_data_wrapper.is_bind)))
		|| is_market_buy_flag != item_data_wrapper.is_market_buy_flag)
	{
		return ITEM_MERGE_CANNOT;
	}

	int ret = ITEM_MERGE_NOT_COMPLETE;
	int tmp_rest_num = 0;

	num += item_data_wrapper.num;
	if (num <= itembase->GetPileLimit()) 
	{
		tmp_rest_num = 0;
		ret = ITEM_MERGE_COMPLETE;
	}
	else
	{
		tmp_rest_num = num - itembase->GetPileLimit();
		num = itembase->GetPileLimit();
	}

	if (NULL != rest_num) *rest_num = tmp_rest_num;

	if (item_data_wrapper.gold_price > 0)
	{
		int tmp_merge_gold_price = ItemExtern::GetGoldPriceByNum(item_data_wrapper.item_id, item_data_wrapper.num, item_data_wrapper.gold_price, item_data_wrapper.num - tmp_rest_num);
		gold_price += tmp_merge_gold_price;

		if (NULL != merged_gold_price) *merged_gold_price = tmp_merge_gold_price;
	}

#ifdef TRACK_ITEM_FLAG
	gamelog::g_log_item_track.printf(LL_INFO, "TRACK_ITEM_FLAG ItemGridData::Merge item_id[%d] unique_id[%lld] Merge With unique_id[%lld]", item_id, item_unique_id, item_data_wrapper.item_unique_id);
#endif

	return ret;
}

int ItemGridData::Merge(const ItemGridData &itemdata, int *rest_num, int *merged_gold_price, bool ignore_bind)
{
	if (NULL != param || NULL != itemdata.param) return ITEM_MERGE_CANNOT; 

	static ItemDataWrapper item_wrapper;
	item_wrapper.item_id = itemdata.item_id;
	item_wrapper.num = itemdata.num;
	item_wrapper.is_bind = itemdata.is_bind;
	item_wrapper.gold_price = itemdata.gold_price;
	item_wrapper.invalid_time = itemdata.invalid_time;
	item_wrapper.is_market_buy_flag = itemdata.is_market_buy_flag;

	return Merge(item_wrapper, rest_num, merged_gold_price, ignore_bind);
}

bool ItemGridData::CanMergeWith(const ItemDataWrapper &item_data_wrapper) const
{
	if (!this->CanBeMerge(true)) return false;

	if (invalid_time != item_data_wrapper.invalid_time) return false;

	return true;
}