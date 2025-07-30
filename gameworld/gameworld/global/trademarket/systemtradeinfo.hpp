#ifndef __SYSTEM_TRADE_INFO_HPP__
#define __SYSTEM_TRADE_INFO_HPP__

#include "servercommon/struct/global/trademarketparam.hpp"


class SystemSaleItem;
struct SystemTradeItemInfo
{
	SystemTradeItemInfo() : is_dirty(false), old_state(false), sale_item_ptr(NULL) {}

	bool IsValid() { return NULL != sale_item_ptr; }

	bool is_dirty;
	bool old_state;
	SystemSaleItem* sale_item_ptr;
};
class SystemTradeInfo
{
public:
	typedef std::map<long long, SystemTradeItemInfo> SystemTradeItemMap;

public:
	SystemTradeInfo();
	~SystemTradeInfo();

	void * operator new(size_t c);
	void operator delete(void *m);

	void Update(time_t now);
	bool IsChanged() { return m_is_change; }
	void UpdateSaleItemList(unsigned int now);

	bool InitSaleItem(const TradeMarketData::SaleItem &saleitem);
	
	bool SaveDB();

	bool OnAddSaleItem(long long unique_key, int seller_uid, const char* seller_name, const SaleItemParam& param, bool is_init);
	bool OnRemoveSaleItem(long long unique_key, const char* reason);
	bool OnItemSold(long long unique_key, short sold_num, int buyer_uid, const char* buyer_name, const char* buyer_pname);

	//bool IsItemAlreadyOnShelve(ItemID item_id);  // 物品是否已在货架上
	SystemTradeItemInfo* GetTradeItemInfo(long long unique_key);

	void GetSystemBuyList(ARG_OUT std::vector<long long>* unique_key_vec);
	void GetSystemOffshelveList(ARG_OUT std::vector<long long>* unique_key_vec);
private:
	void GetChangeSaleItemList(ARG_OUT TradeMarketData* data);
	void ClearDirtyFlag(const TradeMarketData& data);
	void RemoveOffShelveItems();

	SystemTradeItemMap m_trade_item_map;
	
	bool m_is_change;
	unsigned int m_next_save_db_time;
	unsigned int m_next_check_status_time;

};

#endif