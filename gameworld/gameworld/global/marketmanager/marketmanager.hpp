#ifndef __MARKET_MANAGER_HPP__
#define __MARKET_MANAGER_HPP__

#include "servercommon/struct/global/marketmanagerparamhpp.h"
#include "servercommon/internalprotocal/crossgameprotocal.h"
#include "other/shop/shopconfig.h"

class Role;
class MarketManager
{
	friend class RMIInitMarketItemDataBackObjectImpl;

public:
	static MarketManager & Instance();

	void Update(unsigned long interval, time_t now_second);
	void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);
	void OnHiddenDayChange();	//跨服daychange
	void OnGameDayChange();		//原服daychagne

	void OnServerStart();
	void OnServerStop();
	bool LoadMarketAllItemData(long long id_from);		
	bool IsLoadFinish() { return m_load_flag; }

	void Init(const MarketManagerParam & param);
	void GetInitParam(MarketManagerParam * param);
	void ClearDirtyMark(MarketManagerParam *param);
	void OnServerLoadDataFinish();			//全部读取完后再检测之前记录
	void OnReloadConfig();

	bool OnBuyItem(Role * role, int item_id, int num);
	void OnMarketItemPurchased(int item_id, int num);
	void OnSaleItem(Role * role, int column, int index, int num);
	bool GetItemPrice(int item_id, int num, ARG_OUT int* price_type, ARG_OUT int* total_price);
	int GetItemPriceFloat(int business_amount, int float_range, const MarketOtherCfg & market_other_cfg, int transaction_count, bool is_sale);

	void OnSaleFuShi(Role* role, int index);

	void OnConnectCrossSucc();
	void OnDisconnectFromCrossServer();

	void OnGameHiddenBuyItemRecordSyn(TransactionRecordInfo record);		//游戏服同步该条购买记录到隐藏服
	void OnHiddenReceGameBuyItemRecord(crossgameprotocal::GameHiddenMarketBuyItemRecordSynReq * msg);		//隐藏服收到游戏服的购买记录
	void OnHiddenGameBuyItemRecordSyn(const MarketItemInfo & item_info, int server_id, bool is_has_change_price);	//隐藏服同步该条购买记录到所有游戏服 / 隐藏服收到游戏服同步的购买记录后再转发出去
	void OnGameRecvHiddenBuyItemRecord(crossgameprotocal::HiddenGameMarketBuyItemRecordSynReq *msg);

	void OnGameHiddenAllRecordSyn();										//游戏服所有记录同步到隐藏服
	void OnGameRecvHiddenAllRecord(crossgameprotocal::HiddenGameMarketAllTransactionRecordSynReq & msg);	//游戏服收到了隐藏服的记录数据
	void OnHiddenRecvGameAllRecord(crossgameprotocal::GameHiddenMarketAllTransactionRecordSynReq & msg);	//隐藏服收到了游戏服的记录数据,计算最新汇总回后转发回各个服

	void SendAllInfo(Role * role);
	void SendSingleInfo(Role * role, MarketItemInfo & item_info);

private:
	MarketManager();
	~MarketManager();

	void GetNextRefreshTimestamp();
	void SetNewPrice(const MarketItemCfg * market_item_cfg, const MarketOtherCfg & market_other_cfg, MarketItemInfo & market_info);

	void CheckAllItemData();
	void Save();
	void LoadMarketAllItemDataSucc();
	void SetDirty(ItemID item_id);
	bool IsDirty() { return m_change_flag; }

	unsigned int next_refresh_timestamp;		// 下次交易记录重置时间戳

	std::map<ItemID, MarketItemInfo> m_cross_gather_tran_record_list;			//跨服汇总的交易记录
	std::map<ItemID, int> m_disconnect_cross_add_record_list;					//原服与跨服断开连接的这段时间添加的记录

	std::map<long long, char> m_dirty_mark;
	std::map<long long, char> m_old_state;

	bool m_change_flag;
	bool m_load_flag;
	unsigned int m_next_save_db_timestamp;								// 下次保存到数据库的时间戳
};


#endif