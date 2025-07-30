#ifndef __USER_TRADE_INFO_HPP__
#define __USER_TRADE_INFO_HPP__

#include "servercommon/struct/global/trademarketparam.hpp"
#include "usertraderecord.hpp"

class UserTradeRecord;
class UserSaleItem;
class UserTradeInfo
{
public:
	UserTradeInfo(int role_id);
	~UserTradeInfo();

	void * operator new(size_t c);
	void operator delete(void *m);

	void Update(time_t now);
	bool IsChanged() { return m_is_change || m_focus_is_change; }
	void UpdateSaleItemList(unsigned int now);
	void UpdateFocusItemList(unsigned int now);

	bool InitSaleItem(const TradeMarketData::SaleItem &saleitem);
	bool InitFocusItem(const TradeMarketData::SaleItem &focusitem);
	bool InitRoleRecordParam(const RoleTransactionRecordParam& param, const NewUserTradeRecordParam& sold_record, const NewUserTradeRecordParam& buy_record);
	bool SaveDB();
	bool SaveRecord();

	bool OnAddSaleItem(int index, int seller_uid, const char* seller_name, const SaleItemParam& param, bool is_init);
	bool OnAddFocusItem(int index, int seller_uid, const char* seller_name, const SaleItemParam& param, bool is_init);
	bool OnRemoveSaleItem(int sale_index, const char* reason);
	bool OnRemoveFocusItem(int focus_index);

	bool OnCashOutItem(int sale_index, long long* add_gold);
	bool OnItemSold(int sale_index, short sold_num, int buyer_uid, const char* buyer_name, const char* buyer_pname);
	bool OnItemRedoAddOnShelve(int sale_index, int price);

	bool IsItemAlreadyOnShelve(ItemID item_id);  // 物品是否已在货架上
	bool GetEmptyIndex(int* out_sale_index);
	bool GetEmptyFocusIndex(int* out_focus_index);
	const UserSaleItem* GetSaleItem(int sale_index);
	const UserSaleItem* GetFocusItem(int focus_index);
	void UpdateFocusItem(int focus_index, const SaleItemParam& data);

	static bool IsIndexValid(int sale_index);
	static bool IsFocusIndexValid(int focus_index);

	void AddSoldRecord(ItemID item_id, short sold_num, short price_type, int price, const ItemDataWrapper& wrapper);
	void AddBuyRecord(ItemID item_id, short buy_num, short price_type, int price, const ItemDataWrapper& wrapper);
	inline const RoleTransactionRecordParam& GetRoleRecord() const { return m_other_param; }
	inline const UserTradeRecord* GetSoldRecord() const { return m_sold_record; }
	inline const UserTradeRecord* GetBuyRecord() const { return m_buy_record; }

	void GMSetNextStatusTime();
	int GetRoleID() const { return m_role_id; }

	int ItemRecover(int role_id, ItemID item_id);
private:
	void SetDirty(int sale_index);
	void SetFocusDirty(int focus_index);
	void ClearDirtyMark();

	void GetChangeSaleItemList(TradeMarketData* out_data);
	void CheckUserAFKClearTradeRecord(unsigned int now);
	void MakeSoldRecordLog();
	void MakeBuyRecordLog();
	void MakeRecordLog(const char* from, const UserTradeRecord::RecordQueue& record_queue);
	UserSaleItem* MutableSaleItem(int sale_index);

	int m_role_id;
	bool m_old_state[USER_SALE_ITEM_MAX_NUM];
	bool m_dirty_mark[USER_SALE_ITEM_MAX_NUM];
	UserSaleItem* m_sale_list[USER_SALE_ITEM_MAX_NUM];
	bool m_is_change;

	unsigned int m_next_check_status_time;
	unsigned int m_next_save_db_time;

	bool m_focus_old_state[TRADE_MARKET_USER_FOCUS_MAX_NUM];
	bool m_focus_dirty_mark[TRADE_MARKET_USER_FOCUS_MAX_NUM];
	UserSaleItem* m_focus_list[TRADE_MARKET_USER_FOCUS_MAX_NUM];
	bool m_focus_is_change;

	RoleTransactionRecordParam m_other_param;	

	bool m_is_record_change;
	unsigned int m_next_save_record_time;
	bool m_record_old_state;

	UserTradeRecord* m_sold_record;
	UserTradeRecord* m_buy_record;

	unsigned int m_next_check_afk_clear_trade_record_timestamp;
};

#endif