#include "trademarketparam.hpp"

bool TradeMarketData::Serialize(TLVSerializer& outstream) const
{
	bool ret = outstream.Push(is_system_shelve) && outstream.Push(count);
	if (!ret) return false;

	for (int i = 0; i < count && i < MAX_SALE_ITEM_COUNT; ++i)
	{
		ret = outstream.Push(sale_list[i].id_saleitem) &&
			outstream.Push(sale_list[i].change_state) &&
			outstream.Push(sale_list[i].is_focus) &&
			outstream.Push(sale_list[i].index) &&
			outstream.Push(sale_list[i].owner_uid) &&
			outstream.Push(sale_list[i].unique_key);
					
		if (!ret) return false;

		if (sale_list[i].change_state == structcommon::CS_DELETE) continue;

		const char* seller_name = sale_list[i].seller_name;
		ret = outstream.Push(seller_name) &&
			outstream.Push(sale_list[i].seller_uid) &&
			outstream.Push(sale_list[i].salitem_param.sale_index);
		outstream.Push(sale_list[i].salitem_param.immortal_coin_price) &&
			outstream.Push(sale_list[i].salitem_param.gold_price) &&
			outstream.Push(sale_list[i].salitem_param.num_per_group) &&
			outstream.Push(sale_list[i].salitem_param.sold_num) &&
			outstream.Push(sale_list[i].salitem_param.cur_status) &&
			outstream.Push(sale_list[i].salitem_param.next_status_time) &&
			outstream.Push(sale_list[i].salitem_param.is_need_announcement) &&
			outstream.Push(sale_list[i].salitem_param.item_search_type) &&
			outstream.Push(sale_list[i].salitem_param.put_on_shelve_time) &&
			outstream.Push(sale_list[i].salitem_param.unique_key) &&
			outstream.Push(sale_list[i].salitem_param.system_buy_minute_num) &&
			outstream.Push(sale_list[i].salitem_param.system_offshelve_minute_num) &&
			outstream.Push(sale_list[i].salitem_param.system_buy_timestamp) &&
			outstream.Push(sale_list[i].salitem_param.system_offshelve_timestamp);

		if (!ret) return false;

		ret = sale_list[i].salitem_param.item_wrapper.Serialize(outstream);
		if (!ret) return false;
	}

	return true;
}

bool TradeMarketData::UnSerialize(TLVUnserializer& instream)
{
	bool ret = instream.Pop(&is_system_shelve) && instream.Pop(&count);
	if (!ret) return false;

	for (int i = 0; i < count && i < MAX_SALE_ITEM_COUNT; ++i)
	{
		ret = instream.Pop(&sale_list[i].id_saleitem) &&
			instream.Pop(&sale_list[i].change_state) &&
			instream.Pop(&sale_list[i].is_focus) &&
			instream.Pop(&sale_list[i].index) &&
			instream.Pop(&sale_list[i].owner_uid) &&
			instream.Pop(&sale_list[i].unique_key);
		if (!ret) return false;

		if (sale_list[i].change_state == structcommon::CS_DELETE) continue;

		const char * seller_name = 0;

		ret = instream.Pop(&seller_name) &&
			instream.Pop(&sale_list[i].seller_uid) &&
			instream.Pop(&sale_list[i].salitem_param.sale_index) &&
			instream.Pop(&sale_list[i].salitem_param.immortal_coin_price) &&
			instream.Pop(&sale_list[i].salitem_param.gold_price) &&
			instream.Pop(&sale_list[i].salitem_param.num_per_group) &&
			instream.Pop(&sale_list[i].salitem_param.sold_num) &&
			instream.Pop(&sale_list[i].salitem_param.cur_status) &&
			instream.Pop(&sale_list[i].salitem_param.next_status_time) &&
			instream.Pop(&sale_list[i].salitem_param.is_need_announcement) &&
			instream.Pop(&sale_list[i].salitem_param.item_search_type) &&
			instream.Pop(&sale_list[i].salitem_param.put_on_shelve_time) &&
			instream.Pop(&sale_list[i].salitem_param.unique_key) &&
			instream.Pop(&sale_list[i].salitem_param.system_buy_minute_num) &&
			instream.Pop(&sale_list[i].salitem_param.system_offshelve_minute_num) &&
			instream.Pop(&sale_list[i].salitem_param.system_buy_timestamp) &&
			instream.Pop(&sale_list[i].salitem_param.system_offshelve_timestamp);

		if (!ret) return false;

		ret = sale_list[i].salitem_param.item_wrapper.Unserialize(instream);
		if (!ret) return false;

		STRNCPY(sale_list[i].seller_name, seller_name, sizeof(GameName));
	}

	return true;
}

bool UserTradeRecordData::Serialize(TLVSerializer& outstream) const
{
	bool ret = outstream.Push(count);
	if (!ret) return false;

	for (int i = 0; i < count && i < MAX_USER_RECORD_NUM; ++i)
	{
		ret = outstream.Push(record_list[i].id) &&
			outstream.Push(record_list[i].role_id) &&
			outstream.Push(record_list[i].old_state);
		if (!ret) return false;

		{
			TLVSerializer serializer;
			serializer.Reset((void*)&record_list[i].other_param, sizeof(record_list[i].other_param));
			serializer.MoveCurPos(sizeof(record_list[i].other_param));

			ret = outstream.Push(serializer);
			if (!ret) return false;
		}
		
		{
			TLVSerializer serializer;
			serializer.Reset((void*)&record_list[i].sold_record, sizeof(record_list[i].sold_record));
			serializer.MoveCurPos(sizeof(record_list[i].sold_record));

			ret = outstream.Push(serializer);
			if (!ret) return false;
		}

		{
			TLVSerializer serializer;
			serializer.Reset((void*)&record_list[i].buy_record, sizeof(record_list[i].buy_record));
			serializer.MoveCurPos(sizeof(record_list[i].buy_record));

			ret = outstream.Push(serializer);
			if (!ret) return false;
		}
	}

#ifdef _DEBUG
	printf("UserTradeRecordData::Serialize count=[%d]\n", count);
#endif

	return true;
}

bool UserTradeRecordData::UnSerialize(TLVUnserializer& instream)
{
	bool ret = instream.Pop(&count);
	if (!ret) return false;

	for (int i = 0; i < count && i < MAX_USER_RECORD_NUM; ++i)
	{
		ret = instream.Pop(&record_list[i].id) &&
			instream.Pop(&record_list[i].role_id) &&
			instream.Pop(&record_list[i].old_state);
		if (!ret) return false;

		{
			// 读取record_param数据
			TLVUnserializer unserializer;
			ret = instream.Pop(&unserializer);
			if (!ret) return false;

			if (unserializer.Size() <= sizeof(record_list[i].other_param))
			{
				memcpy(&record_list[i].other_param, unserializer.Ptr(), unserializer.Size());
			}
			else
			{
				return false;
			}
		}

		{
			// 读取sold_record数据
			TLVUnserializer unserializer;
			ret = instream.Pop(&unserializer);
			if (!ret) return false;

			if (unserializer.Size() <= sizeof(record_list[i].sold_record))
			{
				memcpy(&record_list[i].sold_record, unserializer.Ptr(), unserializer.Size());
			}
			else
			{
				return false;
			}
		}

		{
			// 读取buy_record数据
			TLVUnserializer unserializer;
			ret = instream.Pop(&unserializer);
			if (!ret) return false;

			if (unserializer.Size() <= sizeof(record_list[i].buy_record))
			{
				memcpy(&record_list[i].buy_record, unserializer.Ptr(), unserializer.Size());
			}
			else
			{
				return false;
			}
		}
	}

#ifdef _DEBUG
	printf("UserTradeRecordData::UnSerialize count=[%d]\n", count);
#endif

	return true;
}

bool SystemTradeRecord::Serialize(TLVSerializer& outstream) const
{
	bool ret = outstream.Push(count);
	if (!ret) return false;

	for (int i = 0; i < count && i < MAX_SYSTEM_RECORD_NUM; ++i)
	{
		ret = outstream.Push(record_list[i].id) &&
			outstream.Push(record_list[i].old_state) &&
			outstream.Push(record_list[i].index) &&
			outstream.Push(record_list[i].transaction_time);

		if (!ret) return false;

		TLVSerializer serializer;
		serializer.Reset((void*)&record_list[i].ti, sizeof(record_list[i].ti));
		serializer.MoveCurPos(sizeof(record_list[i].ti));

		ret = outstream.Push(serializer);
		if (!ret) return false;
	}

	return true;
}

bool SystemTradeRecord::UnSerialize(TLVUnserializer& instream)
{
	bool ret = instream.Pop(&count);
	if (!ret) return false;

	for (int i = 0; i < count && i < MAX_SYSTEM_RECORD_NUM; ++i)
	{
		ret = instream.Pop(&record_list[i].id) &&
			instream.Pop(&record_list[i].old_state) &&
			instream.Pop(&record_list[i].index) &&
			instream.Pop(&record_list[i].transaction_time);

		if (!ret) return false;

		TLVUnserializer unserializer;
		ret = instream.Pop(&unserializer);
		if (!ret) return false;
		if (unserializer.Size() <= sizeof(record_list[i].ti))
		{
			memcpy(&record_list[i].ti, unserializer.Ptr(), unserializer.Size());
		}
#ifdef _DEBUG
		else
		{
			return false;
		}
#endif
	}

	return true;
}

bool SystemRecycleRuleList::Serialize(TLVSerializer& outstream) const
{
	bool ret = outstream.Push(count);
	if (!ret) return false;

	for (int i = 0; i < count && i < MAX_RULE_NUM; ++i)
	{
		ret = outstream.Push(rule_list[i].id) &&
			outstream.Push(rule_list[i].change_state) &&
			outstream.Push(rule_list[i].rule_id);

		if (!ret) return false;

		const RecycleRuleParam& param = rule_list[i].param;
		ret = outstream.Push(param.rule_id) &&
			outstream.Push(param.begin_timestamp) &&
			outstream.Push(param.end_timestamp) &&
			outstream.Push(param.recycle_num_limit) &&
			outstream.Push(param.recycle_price_limit) &&
			outstream.Push(param.item_id) &&
			outstream.Push(param.reserve_sh) &&
			outstream.Push(param.cur_recycle_num);
		
		if (!ret)
		{
			return false;
		}

		TLVSerializer itemparam_data;
		itemparam_data.Reset((void*)param.data, sizeof(param.data));
		itemparam_data.MoveCurPos(sizeof(param.data));

		ret = outstream.Push(itemparam_data);
		if (!ret)
		{
			return false;
		}
	}

	return true;
}

bool SystemRecycleRuleList::UnSerialize(TLVUnserializer& instream)
{
	bool ret = instream.Pop(&count);
	if (!ret) return false;

	for (int i = 0; i < count && i < MAX_RULE_NUM; ++i)
	{
		ret = instream.Pop(&rule_list[i].id) &&
			instream.Pop(&rule_list[i].change_state) &&
			instream.Pop(&rule_list[i].rule_id);

		if (!ret) return false;

		RecycleRuleParam& param = rule_list[i].param;
		ret = instream.Pop(&param.rule_id) &&
			instream.Pop(&param.begin_timestamp) &&
			instream.Pop(&param.end_timestamp) &&
			instream.Pop(&param.recycle_num_limit) &&
			instream.Pop(&param.recycle_price_limit) &&
			instream.Pop(&param.item_id) &&
			instream.Pop(&param.reserve_sh) &&
			instream.Pop(&param.cur_recycle_num);

		if (!ret)
		{
			return false;
		}
		
		static TLVUnserializer itemparam_data;
		if (!instream.Pop(&itemparam_data)) return false;

		if (itemparam_data.Size() <= sizeof(param.data))
		{
			memcpy(param.data, itemparam_data.Ptr(), itemparam_data.Size());
		}
		else
		{
			return false;
		}
	}

	return true;
}

bool AutoUpshelveDataList::Serialize(TLVSerializer& outstream) const
{
	bool ret = outstream.Push(count);
	if (!ret) return false;

	for (int i = 0; i < count && i < MAX_AUTO_UPSHELVE_ITEM_NUM; ++i)
	{
		ret = outstream.Push(data_list[i].id) &&
			outstream.Push(data_list[i].change_state) &&
			outstream.Push(data_list[i].unique_id);

		if (!ret) return false;

		const AutoUpshelveParam& param = data_list[i].param;
		ret = outstream.Push(param.item_id) &&
			outstream.Push(param.upshelve_num) &&
			outstream.Push(param.upshelve_price) &&
			outstream.Push(param.upshelve_interval_m) &&
			outstream.Push(param.auto_offshelve_time_m) &&
			outstream.Push(param.auto_upshelve_limit_num) &&
			outstream.Push(param.price_float_percent) &&
			outstream.Push(param.price_float_limit_percent) &&
			outstream.Push(param.begin_timestamp) &&
			outstream.Push(param.end_timestamp) &&
			outstream.Push(param.unique_id) &&
			outstream.Push(param.stop_upshelve_world_level) &&
			outstream.Push(param.start_upshelve_world_level);

		if (!ret)
		{
			return false;
		}
	}

	return true;
}

bool AutoUpshelveDataList::UnSerialize(TLVUnserializer& instream)
{
	bool ret = instream.Pop(&count);
	if (!ret) return false;

	for (int i = 0; i < count && i < MAX_AUTO_UPSHELVE_ITEM_NUM; ++i)
	{
		ret = instream.Pop(&data_list[i].id) &&
			instream.Pop(&data_list[i].change_state) &&
			instream.Pop(&data_list[i].unique_id);

		if (!ret) return false;

		AutoUpshelveParam& param = data_list[i].param;
		ret = instream.Pop(&param.item_id) &&
			instream.Pop(&param.upshelve_num) &&
			instream.Pop(&param.upshelve_price) &&
			instream.Pop(&param.upshelve_interval_m) &&
			instream.Pop(&param.auto_offshelve_time_m) &&
			instream.Pop(&param.auto_upshelve_limit_num) &&
			instream.Pop(&param.price_float_percent) &&
			instream.Pop(&param.price_float_limit_percent) &&
			instream.Pop(&param.begin_timestamp) &&
			instream.Pop(&param.end_timestamp) &&
			instream.Pop(&param.unique_id) &&
			instream.Pop(&param.stop_upshelve_world_level) &&
			instream.Pop(&param.start_upshelve_world_level);

		if (!ret)
		{
			return false;
		}
	}

	return true;
}
