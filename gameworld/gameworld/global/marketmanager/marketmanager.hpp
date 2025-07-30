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
	void OnHiddenDayChange();	//���daychange
	void OnGameDayChange();		//ԭ��daychagne

	void OnServerStart();
	void OnServerStop();
	bool LoadMarketAllItemData(long long id_from);		
	bool IsLoadFinish() { return m_load_flag; }

	void Init(const MarketManagerParam & param);
	void GetInitParam(MarketManagerParam * param);
	void ClearDirtyMark(MarketManagerParam *param);
	void OnServerLoadDataFinish();			//ȫ����ȡ����ټ��֮ǰ��¼
	void OnReloadConfig();

	bool OnBuyItem(Role * role, int item_id, int num);
	void OnMarketItemPurchased(int item_id, int num);
	void OnSaleItem(Role * role, int column, int index, int num);
	bool GetItemPrice(int item_id, int num, ARG_OUT int* price_type, ARG_OUT int* total_price);
	int GetItemPriceFloat(int business_amount, int float_range, const MarketOtherCfg & market_other_cfg, int transaction_count, bool is_sale);

	void OnSaleFuShi(Role* role, int index);

	void OnConnectCrossSucc();
	void OnDisconnectFromCrossServer();

	void OnGameHiddenBuyItemRecordSyn(TransactionRecordInfo record);		//��Ϸ��ͬ�����������¼�����ط�
	void OnHiddenReceGameBuyItemRecord(crossgameprotocal::GameHiddenMarketBuyItemRecordSynReq * msg);		//���ط��յ���Ϸ���Ĺ����¼
	void OnHiddenGameBuyItemRecordSyn(const MarketItemInfo & item_info, int server_id, bool is_has_change_price);	//���ط�ͬ�����������¼��������Ϸ�� / ���ط��յ���Ϸ��ͬ���Ĺ����¼����ת����ȥ
	void OnGameRecvHiddenBuyItemRecord(crossgameprotocal::HiddenGameMarketBuyItemRecordSynReq *msg);

	void OnGameHiddenAllRecordSyn();										//��Ϸ�����м�¼ͬ�������ط�
	void OnGameRecvHiddenAllRecord(crossgameprotocal::HiddenGameMarketAllTransactionRecordSynReq & msg);	//��Ϸ���յ������ط��ļ�¼����
	void OnHiddenRecvGameAllRecord(crossgameprotocal::GameHiddenMarketAllTransactionRecordSynReq & msg);	//���ط��յ�����Ϸ���ļ�¼����,�������»��ܻغ�ת���ظ�����

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

	unsigned int next_refresh_timestamp;		// �´ν��׼�¼����ʱ���

	std::map<ItemID, MarketItemInfo> m_cross_gather_tran_record_list;			//������ܵĽ��׼�¼
	std::map<ItemID, int> m_disconnect_cross_add_record_list;					//ԭ�������Ͽ����ӵ����ʱ����ӵļ�¼

	std::map<long long, char> m_dirty_mark;
	std::map<long long, char> m_old_state;

	bool m_change_flag;
	bool m_load_flag;
	unsigned int m_next_save_db_timestamp;								// �´α��浽���ݿ��ʱ���
};


#endif