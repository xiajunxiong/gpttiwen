#ifndef __TRADE_MARKET_MANAGER_HPP__
#define __TRADE_MARKET_MANAGER_HPP__

#include <map>
#include "servercommon/servercommon.h"
#include "global/rmibackobjdef.h"
#include "servercommon/internalprotocal/gglobalprotocal.h"
#include "trademarketitemindex.hpp"
#include "transactionrecord.hpp"
#include "protocol/msgtrademarket.h"
#include "systemtradeinfo.hpp"
#include "systemrecycle.hpp"
#include "autoupshelve.hpp"
#include "servercommon/internalprotocal/crossgameprotocal.h"
#include "ordermanager.hpp"
#include "historytopprice.hpp"

class UserTradeInfo;
class Role;
union NetValueItemParam;
class Equipment;
class Jewelry;
struct HistoryTopPriceParam;
class TradeMarketManager
{
public:
	typedef std::map<int, UserTradeInfo*> UserTradeInfoMap;
	friend class RMIInitTradeMarketDataBackObjectImpl;
	friend class RMIInitUserTradeRecordBackObjectImpl;
	friend class RMIInitSystemTradeRecordBackObjectImpl;
	friend class RMIInitSystemRecycleRuleBackObjectImpl;
	friend class RMIInitSystemAutoUpshelveBackObjectImpl;

public:
	static TradeMarketManager& Instance();

	void OnServerStart();
	void OnServerStop();
	bool IsLoadFinish();

	void OnUserLogin(const UserID& user_id);

	UserTradeInfo* GetUserTradeInfo(int role_id, bool is_create = false);
	long long GetUniqueItemKey();

	void Update(unsigned long interval, time_t now_second); 

	bool OnAddItemOnShelveReq(Role* role, SaleItemParam& param, int used_coin, int used_credit);
	bool OnRemoveItemOnShelveReq(Role* role, int sale_index);
	bool OnCashOutReq(Role* role, int sale_index);
	bool OnUserShelveDataReq(Role* role);
	bool OnSearchByItemIDReq(Role* role, ItemID item_id, short query_page, bool is_search_announce, short order_type);
	bool OnSearchByItemTypeReq(Role* role, short item_search_type, short query_page, bool is_search_announce, short order_type);
	bool OnSearchByItemBigTypeReq(Role* role, short item_search_big_type, short query_page, bool is_search_announce, short order_type);
	bool OnSearchByItemTypeWithLevelReq(Role* role, short level, short item_search_type, short query_page, bool is_search_announce, short order_type);
	bool OnBuyItemReq(Role* role, int seller_uid, short sale_index, short buy_num, unsigned int upshelve_time, bool is_pet, long long unique_key, ARG_OUT int* consume_gold);
	bool OnBuySystemItemReq(Role* role, short buy_num, unsigned int upshelve_time, long long unique_key, ARG_OUT int* consume_gold);
	bool OnAutoBuyItemReq(Role* role, ItemID buy_item_id, short buy_num, int expect_price, ARG_OUT int* consume_gold);
	bool OnGetTransactionRecordsReq(Role* role, ItemID item_id);
	bool OnAddFocusReq(Role* role, int seller_uid, int sale_index, unsigned int upshelve_time);
	bool OnRemoveFocusReq(Role* role, int focus_index, short cur_page, bool is_announce);
	bool OnGetFocusListReq(Role* role);
	bool OnGetFocusItemInfoReq(Role* role, int focus_index, int page, bool is_announce);
	bool OnAdvertiseReq(Role* role, int sale_index);
	bool OnSearchByAdvertiseReq(Role* role, int seller_uid, short sale_index, unsigned int upshelve_time, int price, ItemID item_id, short item_sub_type, long long unique_key);
	bool OnSearchSystemByAdvertiseReq(Role* role, int seller_uid, short sale_index, unsigned int upshelve_time, int price, ItemID item_id, short item_sub_type, long long unique_key);
	bool OnGetSellingListReq(Role* role, ItemID item_id, short special_effect_type);
	bool OnGetRoleTradeRecordReq(Role* role);
	bool OnRedoAddItemOnShelveReq(Role* role, short sale_index, int price);
	bool OnSearchPetForQuickBuyReq(Role* role, short order_type, ItemID item_id, short quality, int last_seller_uid, int last_price, unsigned int last_put_on_shelve_time, short last_str_level);
	bool OnSearchMedalWithColor(Role* role, short medal_color, short query_page, bool is_search_announce, short order_type, int prof_param = -1);
	bool OnGetMerchandiseNumReq(Role* role, short big_search_type, bool is_announce);
	bool OnSearchMedalByEffectTypeAndColorReq(Role* role, short sp_type, short medal_color, short query_page, bool is_search_announce, short order_type);

	TradeMarketItemIndex& GetSaleItemIndex() { return m_sale_item_index; }
	void SendShelveChangeNotice(int uid, int sale_index, short reason);

	/////////////////////////////////////// 原服请求跨服数据 ///////////////////////////////////////
	void OnOriginUserGetShelveDataReq(int plat_type, int server_id, long long uuid);
	void OnOriginUserSearchItemByIdReq(int plat_type, int server_id, long long uuid, ItemID item_id, short query_page, bool is_search_announce, short order_type);
	void OnOriginUserSearchItemByTypeReq(int plat_type, int server_id, long long uuid, short item_search_type, short query_page, bool is_search_announce, short order_type);
	void OnOriginUserSearchItemByBigTypeReq(int plat_type, int server_id, long long uuid, short item_search_big_type, short query_page, bool is_search_announce, short order_type);
	void OnOriginUserSearchItemByTypeWithLevelReq(int plat_type, int server_id, long long uuid, short level, short item_search_type, short query_page, bool is_search_announce, short order_type);
	void OnOriginUserSearchMedalByColorReq(int plat_type, int server_id, long long uuid, short medal_color, short query_page, bool is_search_announce, short order_type, int prof_param = -1);
	void OnOriginUserSearchMedalByEffectTypeAndColor(int plat_type, int server_id, long long uuid, short sp_type, short medal_color, short query_page, bool is_search_announce, short order_type);
	void OnOriginUserGetTransactionRecordReq(int plat_type, int server_id, long long uuid, ItemID item_id);
	void OnOriginUserGetRoleTradeRecordReq(int plat_type, int server_id, long long uuid);
	void OnOriginUserGetRoleFocusListReq(int plat_type, int server_id, long long uuid);
	void OnOriginUserGetFocusItemInfoReq(int plat_type, int server_id, long long uuid, int focus_index, int page, bool is_announce);
	void OnOriginUserRemoveFocusReq(int plat_type, int server_id, long long uuid, int focus_index, short cur_page, bool is_announce);
	void OnOriginUserAddFocusReq(int plat_type, int server_id, long long uuid, int seller_uid, int sale_index, unsigned int upshelve_time);
	void OnOriginUserGetMerchandiseNum(int plat_type, int server_id, long long uuid, short big_search_type, bool is_announce);
	void OnOriginUserGetSellingListReq(int plat_type, int server_id, long long uuid, ItemID item_id, short medal_effect_type);
	void OnOriginUserSearchPetForQuickBuyReq(int plat_type, int server_id, int role_id, short order_type, ItemID item_id, short quality, int last_seller_uid, int last_price, unsigned int last_put_on_shelve_time, short last_str_level);
	void OnOriginUserSearchAdvertiseItemReq(crossgameprotocal::GameHiddenTradeMarketSearchAdvertiseItemReq* msg);
	void OnOriginUserRequestBuy(crossgameprotocal::GameHiddenTradeMarketRequestBuy* msg);
	void OnOriginUserRequestSystemBuy(crossgameprotocal::GameHiddenTradeMarketRequestBuy* msg);
	void OnOriginUserRequestBuyResp(crossgameprotocal::HiddenGameTradeMarketRequestBuyResp* msg);
	void OnOriginUserRequestBuyConfirm(crossgameprotocal::GameHiddenTradeMarketRequestBuyConfirm* msg);
	void OnOriginUserRequestSystemBuyConfirm(crossgameprotocal::GameHiddenTradeMarketRequestBuyConfirm* msg);
	void OnOriginUserRequestBuyConfirmResp(crossgameprotocal::HiddenGameTradeMarketRequestBuyConfirmResp* msg);
	void OnOriginUserRequestBuyConfirmErrorResp(crossgameprotocal::HiddenGameTradeMarketRequestBuyConfirmErrorResp* msg);
	void OnOriginUserRequestUpshelve(crossgameprotocal::GameHiddenTradeMarketRequestUpshelve* msg);
	void OnOriginUserRequestUpshelveResp(crossgameprotocal::HiddenGameTradeMarketRequestUpshelveResp* msg);
	void OnOriginUserOffshelveReq(crossgameprotocal::GameHiddenTradeMarketOffshelveReq* msg);
	void OnOriginUserOffshelveResp(crossgameprotocal::HiddenGameTradeMarketOffshelveResp* msg);
	void OnOriginUserRedoUpshelveReq(crossgameprotocal::GameHiddenTradeMarketRedoUpshelveReq* msg);
	void OnOriginUserRedoUpshelveResp(crossgameprotocal::HiddenGameTradeMarketRedoUpshelveResp* msg);
	bool OnOriginUserCashOutReq(crossgameprotocal::GameHiddenTradeMarketCashOutReq* msg);
	bool OnOriginUserCashOutResp(crossgameprotocal::HiddenGameTradeMarketCashOutResp* msg);
	bool OnOriginUserAdvertiseReq(crossgameprotocal::GameHiddenTradeMarketAdvertiseReq* msg);
	bool OnOriginUserAdvertiseResp(crossgameprotocal::HiddenGameTradeMarketAdvertiseResp* msg);
	bool OnOriginUserAdvertiseConfirmBoardcast(crossgameprotocal::GameHiddenTradeMarketAdvertiseConfirmBoardcast* msg);
	bool OnOriginUserQueryItemRecommendPriceReq(crossgameprotocal::GameHiddenQueryItemRecommendPrice* msg);
	void SendOriginUserRequestUpshelveResp(int plat_type, int server_id, int role_id, int result, const ItemDataWrapper& wrapper, int service_fee, int trade_credit, long long order_id);
	void SendOriginUserRedoUpshelveResp(int plat_type, int server_id, int role_id, int sale_index, int result);
	void SendOriginUserCashOutResp(int plat_type, int server_id, int role_id, int add_gold, int result);
	void SendOriginUserConfirmBuyErrorResp(int plat_type, int server_id, int role_id, int seller_uid, int sale_index, int buy_num, ItemID buy_item_id, UniqueKeyStr unique_key, long long order_id, int error_num, const char* error_msg);

	////////////////////////////////// 交易行后台接口与功能 /////////////////////////
	bool AddItemOnSystemShelve(SaleItemParam& param, std::string& error_msg);
	bool CheckSystemSaleItemPet(NetValueItemParam& param, std::string& error_msg);
	bool CheckSystemSaleItemEquip(const Equipment* equip, NetValueItemParam& param, std::string& error_msg);
	bool CheckSystemSaleItemJewelry(const Jewelry* jewelry, NetValueItemParam& param, std::string& error_msg);
	bool AddRecycleRule(const RecycleRuleParam& param);
	bool RemoveRecycleRule(long long rule_id);
	void DoRecycle();

	void OnItemSold(ItemID item_id, int price, int sold_num, const ItemDataWrapper& item_wrapper, int seller_uid, const char* seller_name, const char* buyer_name, int buyer_uid, const char* buyer_pname);
	void LogDeal(ItemID item_id, int price, int sold_num, int seller_uid, const char* seller_name, const char* buyer_name, int buyer_uid, const char* buyer_pname);
	void GetLogParam(ItemID item_id, ARG_OUT int* storage_num, ARG_OUT int* low_price, ARG_OUT int* high_price);
	int GetItemStorage(ItemID item_id);
	AutoUpshelve& GetAutoUpshelve() { return m_auto_upshelve; }
	void SendSystemNotice(bool is_announce, SaleItemBase* sib);
	void GmGetMerchandiseNum(int big_type, bool is_announce);

	void GetHistoryTopPriceItemList(int max_list_size, ARG_OUT int* num, ARG_OUT ItemDataWrapper* item_list);
	void InitHistoryTopPrice(const HistoryTopPriceParam& param);
	void GetHistoryTopPriceInitParam(ARG_OUT HistoryTopPriceParam& param);

	static int GetTradeMinPrice(const ItemBase* item, const ItemDataWrapper* wrapper);
	static int GetTradeMaxPrice(const ItemBase* item, const ItemDataWrapper* wrapper);
	static int GetTradeRecommendPrice(const ItemBase* item, const ItemDataWrapper* wrapper);
	static void QueryItemTradePrice(const ItemBase* item, const ItemDataWrapper* wrapper, ARG_OUT int* min_price, ARG_OUT int* max_price, ARG_OUT int* recommend_price);

	bool ItemRecover(int role_id, ItemID item_id, ARG_OUT int* recover_num);
	bool CanSellItem(int uid, const SaleItemParam& param, int* errnum);
private:
	TradeMarketManager();
	~TradeMarketManager();

	//禁止拷贝复制（请不要公有化以下这两个函数， 除非做好深拷贝， 因为在析构时会delete掉某些成员变量）
	TradeMarketManager(const TradeMarketManager&);
	TradeMarketManager& operator=(const TradeMarketManager &);

	void NewVersionAutoOffshelve(const UserID& user_id);

	bool GiveItemToRole(Role* role, const ItemDataWrapper& item_data, const char* reason, bool is_from_buy = false);
	bool GiveMoneyToRole(Role* role, long long gold, long long immortal_coin, long long bind_coin, const char* reason);
	bool GiveResourceToUser(const UserID& user_id, const ItemDataWrapper* item_data, int gold, int bind_coin, int trade_credit, const char* reason, bool is_from_buy = false);

	bool LoadData(long long id_offset);
	bool LoadSystemShelve(long long id_offset);
	bool LoadUserRecord(long long id_offset);
	bool LoadSystemRecord(long long id_offset);
	bool LoadSystemRecycleRule(long long id_offset);
	bool LoadSystemAutoUpshelve(long long id_offset);
	void OnLoadDataSucc();
	void OnLoadUserRecordSucc();
	void OnLoadSystemRecordSucc();
	void OnLoadSystemShelveSucc();
	void OnLoadSystemRecycleRuleSucc();
	void OnLoadSystemAutoUpshelveSucc();
	void OnAllLoadSucc();

	void Release();
	void SaveAll();
	void SendNoticeNum(const GSNetID& netid, int notice_num);
	void SendNoticeNumToOriginRole(int plat_type, int server_id, long long uuid, int notice_num);
	void SendNoticeNumToOriginRole(int uid, int notice_num);
	void SendShelveData(Role* role);
	void SendShelveDataToOriginRole(int uid);
	void SendRefreshNotice(Role* role, int uid);

	bool SendSearchResult(Role* role, int count, SaleItemBase** item_list, int page, int total_page, int order_type, bool is_search_announce, int cursor_index);
	bool SendSearchPetForQuickBuyResult(Role* role, short order_type, int count, SaleItemBase** item_list);
	void AssembleSearchPetForQuickBuyResult(ARG_OUT Protocol::SCTradeMarketSearchPetForQuickBuy* result, short order_type, int count, SaleItemBase** item_list);

	UserTradeInfo* CreateUserTradeInfo(int seller_uid);

	void MakeShelveData(int uid, Protocol::SCTradeMarketUserShelveData* data);
	void MakeSearchResult(Protocol::SCTradeMarketSearchResult* result, int count, SaleItemBase** item_list, int page, int total_page, int order_type, bool is_search_announce, int cursor_index);
	void MakeUserTradeRecordData(int role_id, Protocol::SCTradeMarketGetRoleTransactionRecord* data);
	void MakeUserFocusListData(int role_id, Protocol::SCTradeMarketGetFocusList* data);
	void MakeFocusItemInfoData(const UserSaleItem* usi, int focus_index, Protocol::SCTradeMarketGetFocusItemInfo* data);

	static bool IsSystemSeller(int seller_uid);

	void DoSystemBuy();
	void DoSystemOffshelve();
	
	bool CheckRAGodBeaseAdventCanSellItem(int uid, const SaleItemParam& param, int* errnum);

	int m_load_data_state;
	int m_load_user_record_state;
	int m_load_system_record_state;
	int m_load_system_shelve_state;
	int m_load_system_recycle_rule_state;
	int m_load_system_auto_upshelve_state;

	UserTradeInfoMap m_user_trade_info_map;
	SystemTradeInfo m_sys_trade_info;
	TradeMarketItemIndex m_sale_item_index;
	TransactionRecord m_transaction_record;
	SystemRecycle m_system_recycle;
	AutoUpshelve m_auto_upshelve;
	HistoryTopPrice m_history_top_price;
	
	unsigned int m_next_check_user_time;
	unsigned int m_next_recycle_time;
};

#endif