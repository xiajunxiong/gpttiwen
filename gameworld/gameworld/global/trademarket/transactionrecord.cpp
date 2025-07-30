#include "transactionrecord.hpp"
#include "engineadapter.h"
#include "global/rmibackobjdef.h"
#include "internalcomm.h"
#include "gamelog.h"
#include "protocol/msgtrademarket.h"

TransactionRecord::TransactionRecord() : m_is_changed(false), m_next_check_time(0u)
{
	memset(m_item_records, 0, sizeof(m_item_records));
	m_next_check_time = static_cast<unsigned int>(EngineAdapter::Instance().Time()) + SYSTEM_TRANSACTION_RECORD_SAVE_INTERVAL_S;
}

TransactionRecord::~TransactionRecord()
{
	for (int i = 0; i < MAX_UINT16 + 1; ++i)
	{
		if (NULL != m_item_records[i])
		{
			delete m_item_records[i];
			m_item_records[i] = NULL;
		}
	}
}

void TransactionRecord::Update(unsigned int now)
{
	if (now > m_next_check_time)
	{
		m_next_check_time = now + SYSTEM_TRANSACTION_RECORD_SAVE_INTERVAL_S;
		this->SaveDB();
	}
}

void TransactionRecord::AddTransactionRecord(ItemID item_id, short sold_num, short price_type, int price, short param_length, const NetValueItemParamData param_data)
{
	RecordList* rl = this->GetRecordList(item_id, true);
	if (NULL == rl) return;

	if (rl->IsFull())
	{
		rl->PopFront();
	}

	static Record tmp_record;
	tmp_record.info.item_id = item_id;
	tmp_record.info.price = price;
	tmp_record.info.price_type = price_type;
	tmp_record.info.item_num = sold_num;
	tmp_record.sold_time = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	tmp_record.info.data_len = param_length;
	memcpy(tmp_record.info.data, param_data, sizeof(tmp_record.info.data));

	rl->PushBack(tmp_record);

	m_dirty_set.insert(std::set<int>::value_type(static_cast<int>(item_id)));
}

int TransactionRecord::GetTransactionRecords(ItemID item_id, int max_num, Protocol::ProtocolTransactionItemInfo* out_list)
{
	RecordList* list = this->GetRecordList(item_id, false);
	if (NULL == list || list->IsEmpty()) return 0;

	int count = 0;
	int cur_idx = list->head_idx;
	for (int i = 0; i < list->element_num && count < max_num; ++i)
	{
		const ItemBase* item = ITEMPOOL->GetItem(item_id);
		if (NULL == item) continue;

		out_list[count].item_id = list->record_list[cur_idx].info.item_id;
		out_list[count].item_num = list->record_list[cur_idx].info.item_num;
		out_list[count].price = list->record_list[cur_idx].info.price;
		out_list[count].price_type = list->record_list[cur_idx].info.price_type;

		int param_length = NetValueItemParam::GetDataLengthByItemType(item->GetItemType());
		param_length = GetMin(ARRAY_ITEM_COUNT(out_list[count].data), param_length);

		out_list[count].data_len = param_length;
		memcpy(out_list[count].data, list->record_list[cur_idx].info.data, param_length);
		UNSTD_STATIC_CHECK(sizeof(out_list[count].data) == sizeof(list->record_list[cur_idx].info.data));

		count += 1;
		cur_idx = (cur_idx + 1) % TRANSACTION_RECORD_MAX_NUM;
	}
	
	return count;
}

bool TransactionRecord::InitTransactionRecord(short index, ItemID item_id, short sold_num, short price_type, int price, unsigned int transaction_time, short param_length, const char* param_data)
{
	if (index < 0 || index >= TRANSACTION_RECORD_MAX_NUM) return false;

	RecordList* list = this->GetRecordList(item_id, true);
	if (NULL == list) return false;

	if (list->record_list[index].info.item_id > 0)
	{
#ifdef _DEBUG
		assert(0); // 有重复的 则有BUG，内网环境下应查看一下
#endif
		return false;
	}
	
	list->record_list[index].sold_time = transaction_time;
	list->record_list[index].info.item_id = item_id;
	list->record_list[index].info.item_num = sold_num;
	list->record_list[index].info.price_type = price_type;
	list->record_list[index].info.price = price;
	list->record_list[index].info.data_len = param_length;
	memcpy(list->record_list[index].info.data, param_data, sizeof(list->record_list[index].info.data));
	list->record_list[index].old_state = true;
	
	return true;
}

bool TransactionRecord::OnInitFinish()
{
	for (int i = 0; i <= MAX_UINT16; ++i)
	{
		RecordList* rl = this->GetRecordList(i, false);
		if (NULL == rl) continue;

		unsigned int earliest_time = MAX_UINT32;
		int earliest_record_idx = 0;
		unsigned int latest_time = 0u;
		int latest_record_idx = -1;
		int record_num = 0;
		for (int k = 0; k < TRANSACTION_RECORD_MAX_NUM; ++k)
		{
			if (!rl->record_list[k].old_state) break;

			record_num += 1;

			if (rl->record_list[k].sold_time < earliest_time)
			{
				earliest_time = rl->record_list[k].sold_time;
				earliest_record_idx = k;
			}

			if (rl->record_list[k].sold_time >= latest_time)
			{
				latest_time = rl->record_list[k].sold_time;
				latest_record_idx = k;
			}
		}

		if (0 == earliest_record_idx)
		{
			rl->head_idx = 0;
			rl->tail_idx = (latest_record_idx + 1) % TRANSACTION_RECORD_MAX_NUM;
		}
		else
		{
			rl->head_idx = earliest_record_idx;
			rl->tail_idx = earliest_record_idx;
		}

		rl->element_num = record_num;

#ifdef _DEBUG
		if (0 == earliest_record_idx)
		{
			if (rl->tail_idx == 0 && (rl->element_num != TRANSACTION_RECORD_MAX_NUM && rl->element_num != 0))
			{
				assert(0);
			}
			else if (rl->tail_idx > 0 && rl->element_num != rl->tail_idx)
			{
				assert(0);
			}
		}
		else
		{
			if (rl->element_num != TRANSACTION_RECORD_MAX_NUM)
			{
				assert(0);
			}
			else if (earliest_record_idx != (latest_record_idx + 1) % TRANSACTION_RECORD_MAX_NUM)
			{
				assert(0);
			}
		}

#endif

		gamelog::g_log_trade_market.printf(LL_INFO, "TransactionRecord::OnInitFinish item[%d] head[%d] tail[%d] record_num[%d]",
			i, rl->head_idx, rl->tail_idx, rl->element_num);
	}

	return true;
}

bool TransactionRecord::SaveDB()
{
	if (m_dirty_set.empty())
	{
		return true;
	}

	static SystemTradeRecord record_list;
	record_list.count = 0;

	std::set<int>::iterator it = m_dirty_set.begin();
	for (; it != m_dirty_set.end();)
	{
		ItemID item_id = static_cast<ItemID>(*it);
		RecordList* rl = this->GetRecordList(item_id, false);
		if (NULL == rl)
		{
			++it;
			continue;
		}

		if (rl->element_num + record_list.count > SystemTradeRecord::MAX_SYSTEM_RECORD_NUM)
		{
			// 如果出现需要保存的数量大于上限，则需要缩短下次保存的间隔
			unsigned int now = static_cast<unsigned int>(EngineAdapter::Instance().Time());
			m_next_check_time = now + SYSTEM_TRANSACTION_RECORD_FASTER_SAVE_INTERVAL_S;
			break;
		}

		this->GetChangedRecords(rl, &record_list);
		m_dirty_set.erase(it++);
	}

	if (record_list.count > 0)
	{
		RMISaveSystemTradeRecordBackObjectImpl *impl = new RMISaveSystemTradeRecordBackObjectImpl();

		RMIGlobalClient ac;
		ac.__bind_session(InternalComm::Instance().GetDBHandler().GetGlobalDB());
		return ac.SaveTradeMarketSystemRecordAsync(record_list, impl);
	}

	return true;
}

TransactionRecord::RecordList* TransactionRecord::GetRecordList(ItemID item_id, bool is_create)
{
	if (item_id < 0 || item_id > MAX_UINT16) return NULL;

	if (NULL == m_item_records[item_id])
	{
		if (!is_create)
		{
			return NULL;
		}

		m_item_records[item_id] = new RecordList();
	}

	return m_item_records[item_id];
}

void TransactionRecord::GetChangedRecords(RecordList* rl, SystemTradeRecord* out_data)
{
	if (NULL == rl || NULL == out_data) return;

	if (out_data->count + TRANSACTION_RECORD_MAX_NUM > SystemTradeRecord::MAX_SYSTEM_RECORD_NUM) return;

	int step = 0;
	while (step < rl->element_num && out_data->count < SystemTradeRecord::MAX_SYSTEM_RECORD_NUM)
	{
		int cur_idx = (rl->head_idx + step) % TRANSACTION_RECORD_MAX_NUM;
		if (rl->record_list[cur_idx].is_dirty)
		{
			out_data->record_list[out_data->count].index = cur_idx;
			out_data->record_list[out_data->count].transaction_time = rl->record_list[cur_idx].sold_time;
			out_data->record_list[out_data->count].ti = rl->record_list[cur_idx].info;
			out_data->record_list[out_data->count].old_state = rl->record_list[cur_idx].old_state;
			out_data->count += 1;
		}
		
		rl->record_list[cur_idx].is_dirty = false;
		rl->record_list[cur_idx].old_state = true;

		++step;
	}
}

TransactionRecord::Record* TransactionRecord::RecordList::GetFront()
{
	if (this->IsEmpty()) return NULL;

	return &record_list[head_idx];
}

void TransactionRecord::RecordList::PopFront()
{
	if (this->IsEmpty()) return;

	head_idx = (head_idx + 1) % TRANSACTION_RECORD_MAX_NUM;
	element_num -= 1;
}

void TransactionRecord::RecordList::PushBack(Record& r)
{
	if (this->IsFull()) return;

	bool old_state = record_list[tail_idx].old_state;

	record_list[tail_idx] = r;
	record_list[tail_idx].is_dirty = true;
	record_list[tail_idx].old_state = old_state;

	tail_idx = (tail_idx + 1) % TRANSACTION_RECORD_MAX_NUM;

	element_num += 1;
}

bool TransactionRecord::RecordList::IsEmpty()
{
	return 0 == element_num;
}

bool TransactionRecord::RecordList::IsFull()
{
	return TRANSACTION_RECORD_MAX_NUM == element_num;
}
