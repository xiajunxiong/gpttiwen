#ifndef __MARKET_MANAGER_PARAM_HPP__
#define __MARKET_MANAGER_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "common/tlvprotocol.h"

static const int MAX_MARKET_ITEM_NUM = 1000;
static const int MAX_MARKET_DATA_SAVE_DB_TIME = 60;		//存库时间间隔

#pragma pack(push) 
#pragma pack(4)

struct TransactionRecordInfo
{
	TransactionRecordInfo(ItemID _item_id, int _transaction_num) : item_id(_item_id), reserve_sh(0), transaction_num(_transaction_num) {}

	TransactionRecordInfo() : item_id(0), reserve_sh(0), transaction_num(0)
	{}

	ItemID item_id;
	short reserve_sh;
	int transaction_num;
};

struct MarketItemInfo
{
	MarketItemInfo() { this->Reset(); }
	
	void Reset()
	{
		item_id = 0;
		reserve_sh = 0;
		cur_price = 0;
		yesterday_price = 0;
		transaction_num = 0;
		transaction_count = 0;
		version = 0;
	}

	bool IsVaild() const
	{
		return item_id > 0;
	}
	
	ItemID item_id;
	short reserve_sh;				
	int cur_price;				//当前价格(这里保存的是购买的,售卖需要再衰减计算一下)
	int yesterday_price;		//昨天价格
	int transaction_num;		//当天贸易数量(可为负数)
	long long transaction_count;//总共贸易数量
	int version;				//版号
};

struct MarketItemSimpleInfo
{
	MarketItemSimpleInfo() { this->Reset(); }
	MarketItemSimpleInfo(const MarketItemInfo & info)
	{
		item_id = info.item_id;
		reserve_sh = info.reserve_sh;
		cur_price = info.cur_price;
		yesterday_price = info.yesterday_price;
		transaction_num = info.transaction_num;
	}
	void Reset()
	{
		item_id = 0;
		reserve_sh = 0;
		cur_price = 0;
		yesterday_price = 0;
		transaction_num = 0;
	}

	ItemID item_id;
	short reserve_sh;
	int cur_price;				//当前价格
	int yesterday_price;		//昨天价格
	int transaction_num;		//当天贸易数量(可为负数)
};

#pragma pack(pop)

class MarketManagerParam
{
public:
	bool Serialize(TLVSerializer &outstream) const;
	bool Unserialize(TLVUnserializer &instream);

	struct MarketItemSaveEntry
	{
		MarketItemSaveEntry() { this->Reset(); }

		void Reset()
		{
			change_state = 0;
			market_info.Reset();
		}

		char change_state;
		MarketItemInfo market_info;
	};


	int count;
	MarketItemSaveEntry market_list[MAX_MARKET_ITEM_NUM];
};


#endif