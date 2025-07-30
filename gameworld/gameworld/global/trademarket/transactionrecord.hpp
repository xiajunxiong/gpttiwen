#ifndef __TRANSACTION_RECORD_HPP__
#define __TRANSACTION_RECORD_HPP__

#include <set>

#include "servercommon/serverdef.h"
#include "servercommon/basedef.h"
#include "servercommon/struct/global/trademarketparam.hpp"

namespace Protocol
{
	struct ProtocolTransactionItemInfo;
}

class TransactionRecord
{
public:
	struct Record
	{
		Record() : old_state(false), is_dirty(false), sold_time(0u) {}

		bool old_state;
		bool is_dirty;

		SystemTransactionInfo info;
		unsigned int sold_time;
	};

	struct RecordList
	{
		RecordList() : head_idx(0), tail_idx(0), element_num(0) {}

		Record* GetFront();
		void PopFront();
		void PushBack(Record& r);
		bool IsEmpty();
		bool IsFull();

		short head_idx;
		short tail_idx;
		short element_num;
		Record record_list[TRANSACTION_RECORD_MAX_NUM];
	};

public:
	TransactionRecord();
	~TransactionRecord();

	void Update(unsigned int now);

	void AddTransactionRecord(ItemID item_id, short sold_num, short price_type, int price, short param_length, const NetValueItemParamData param_data);
	int GetTransactionRecords(ItemID item_id, int max_num, Protocol::ProtocolTransactionItemInfo* out_list);

	bool InitTransactionRecord(short index, ItemID item_id, short sold_num, short price_type, int price, unsigned int transaction_time, short param_length, const char* param_data);
	bool OnInitFinish();
	bool SaveDB();
private:
	RecordList* GetRecordList(ItemID item_id, bool is_create);
	void GetChangedRecords(RecordList* rl, SystemTradeRecord* out_data);

	RecordList* m_item_records[MAX_UINT16 + 1];
	std::set<int> m_dirty_set;

	bool m_is_changed;
	unsigned int m_next_check_time;
};

#endif