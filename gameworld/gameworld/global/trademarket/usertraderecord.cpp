#include "usertraderecord.hpp"
#include "protocol/msgtrademarket.h"
#include "item/itembase.h"
#include "item/itempool.h"

UserTradeRecord::UserTradeRecord()
{

}

UserTradeRecord::~UserTradeRecord()
{

}

void UserTradeRecord::InitTradeRecord(const NewUserTradeRecordParam& param)
{
	int slot_idx = (param.next_slot_idx - 1 + ARRAY_ITEM_COUNT(param.record_list)) % ARRAY_ITEM_COUNT(param.record_list);
	
	assert(slot_idx >= 0 && slot_idx < ARRAY_ITEM_COUNT(param.record_list));
	if (slot_idx < 0 || slot_idx >= ARRAY_ITEM_COUNT(param.record_list)) return;
	
	int record_num = param.record_num;
	while (record_num-- > 0 && m_record_queue.size() < ARRAY_ITEM_COUNT(param.record_list))
	{
		m_record_queue.push_back(param.record_list[slot_idx]);
		slot_idx = (slot_idx - 1 + ARRAY_ITEM_COUNT(param.record_list)) % ARRAY_ITEM_COUNT(param.record_list);
	}
}

void UserTradeRecord::GetTradeRecordParam(NewUserTradeRecordParam* out_param)
{
	out_param->next_slot_idx = 0;
	out_param->record_num = 0;
	RecordQueue::iterator it = m_record_queue.begin();
	for (; it != m_record_queue.end() && out_param->record_num < ARRAY_ITEM_COUNT(out_param->record_list); ++it)
	{
		out_param->record_list[out_param->record_num++] = *it;
		out_param->next_slot_idx = (out_param->next_slot_idx + 1) % ARRAY_ITEM_COUNT(out_param->record_list);
	}
}

void UserTradeRecord::AddRecord(ItemID item_id, short num, short price_type, int price, const ItemDataWrapper& wrapper)
{
	if (m_record_queue.size() >= ROLE_TRANSACTION_RECORD_MAX_NUM)
	{
		m_record_queue.pop_front();
	}
	TransactionInfo info;
	info.item_id = item_id;
	info.item_num = num;
	info.price_type = price_type;
	info.price = price;
	info.wrapper = wrapper;
	m_record_queue.push_back(info);
}

void UserTradeRecord::AssembleRecordsForMsg(short max_num, Protocol::ProtocolTransactionItemInfo* info_list, short* out_num) const
{
	if (NULL == info_list || NULL == out_num) return;

	int num = 0;

	RecordQueue::const_iterator it = m_record_queue.begin();
	for (; it != m_record_queue.end() && num < max_num; ++it)
	{
		const TransactionInfo& info = *it;
		const ItemBase* item = ITEMPOOL->GetItem(info.item_id);
		if (NULL == item) continue;
		
		info_list[num].Reset();
		info_list[num].item_id = info.item_id;
		info_list[num].item_num = info.item_num;
		info_list[num].price = info.price;
		info_list[num].price_type = info.price_type;
		info_list[num].data_len = NetValueItemParam::GetDataLengthByItemType(item->GetItemType());
		memcpy(info_list[num].data, info.wrapper.param_data, sizeof(info_list[num].data));

		UNSTD_STATIC_CHECK(sizeof(info_list[num].data) == sizeof(info.wrapper.param_data));

		num += 1;
	}
	*out_num = num;
}

