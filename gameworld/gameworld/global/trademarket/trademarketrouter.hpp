#ifndef __TRADE_MARKET_ROUTER_HPP__
#define __TRADE_MARKET_ROUTER_HPP__

#include "protocol/msgtrademarket.h"

// 用于把协议转发到TradeMarketManager(global)
class Role;
class TradeMarketRouter
{
public:
	static TradeMarketRouter& Instance();

	void OnUserLogin(const UserID& user_id);

	void OnAddItemOnShelveReq(Role* role, Protocol::CSAddItemOnShelve* req);
	void OnRemoveItemOnShelveReq(Role* role, Protocol::CSRemoveItemOnShelve* req);
	void OnQueryItemRecommendPriceReq(Role* role, Protocol::CSQueryItemRecommendPrice* req);
	void OnCashOutReq(Role* role, Protocol::CSTradeMarketCashOut* req);
	void OnUserShelveInfoReq(Role* role);
	void OnSearchByItemIdReq(Role* role, Protocol::CSTradeMarketSearchByItemID* req);
	void OnSearchByItemSearchTypeReq(Role* role, Protocol::CSTradeMarketSearchByItemSearchType* req);
	void OnSearchByItemSearchBigTypeReq(Role* role, Protocol::CSTradeMarketSearchByItemSearchType* req);
	void OnBuyItemReq(Role* role, Protocol::CSTradeMarketBuyItem* req);
	void OnGetTransactionRecordsReq(Role* role, Protocol::CSRecentTransactionRecords* req);
	void OnGetFocusListReq(Role* role, Protocol::CSTradeMarketGetFocusList* req);
	void OnAddFocusReq(Role* role, Protocol::CSTradeMarketAddFocus* req);
	void OnRemoveFocusReq(Role* role, Protocol::CSTradeMarketRemoveFocus* req);
	void OnGetFocusItemInfoReq(Role* role, Protocol::CSTradeMarketGetFocusItemInfo* req);
	void OnAdvertiseReq(Role* role, Protocol::CSTradeMarketAdvertise* req);
	void OnSearchByAdvertiseReq(Role* role, Protocol::CSTradeMarketSearchAdvertise* req);
	void OnGetSellingListReq(Role* role, Protocol::CSTradeMarketGetSellingList* req);
	void OnGetRoleTransactionRecordsReq(Role* role, Protocol::CSTradeMarketGetRoleTransactionRecord* req);
	void OnRedoAddItemOnShelveReq(Role* role, Protocol::CSTradeMarketRedoAddItemOnShelve* req);
	void OnSearchPetForQuickBuyReq(Role* role, Protocol::CSTradeMarketSearchPetForQuickBuy* req);
	void OnGetMerchandiseNumReq(Role* role, Protocol::CSTradeMarketGetMerchandiseNum* req);
	void OnSearchMedalByEffectTypeAndColorReq(Role* role, Protocol::CSTradeMarketSearchMedalByEffectTypeAndColor* req);
private:
	void QueryUserShelveInfo(Role* role);

	INSTANCE_FORBID_COPY_AND_EXTERIAL_CREATE(TradeMarketRouter);
};

#endif