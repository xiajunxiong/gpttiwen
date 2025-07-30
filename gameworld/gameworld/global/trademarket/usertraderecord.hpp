#ifndef __USER_TRADE_RECORD_HPP__
#define __USER_TRADE_RECORD_HPP__

#include <deque>
#include "servercommon/struct/global/trademarketparam.hpp"

namespace Protocol
{
	struct ProtocolTransactionItemInfo;
}

class UserTradeRecord
{
public:
	typedef std::deque<TransactionInfo> RecordQueue;

public:
	UserTradeRecord();
	~UserTradeRecord();

	void InitTradeRecord(const NewUserTradeRecordParam& param);
	void GetTradeRecordParam(NewUserTradeRecordParam* out_param);

	void AddRecord(ItemID item_id, short num, short price_type, int price, const ItemDataWrapper& wrapper);

	void AssembleRecordsForMsg(short max_num, Protocol::ProtocolTransactionItemInfo* info_list, short* out_num) const;
	const RecordQueue& GetRecordQueue() const { return m_record_queue; }

	void * operator new(size_t c);
	void operator delete(void *m);

private:
	RecordQueue m_record_queue;
};

#endif