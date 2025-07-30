#ifndef  __COW_REPORT_GOOD_NEWS_HPP__
#define __COW_REPORT_GOOD_NEWS_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/serverdef.h"
#include "servercommon/commonsavedef.h"
#include "servercommon/internalprotocal/crossgameprotocal.h"
#include "servercommon/struct/cowreportgoodnewsparam.h"
#include "global/commonsave/commonsave.hpp"
#include "protocol/msgcowreportgoodnews.hpp"

#include <map>
#include <set>


enum COW_REPORT_GOOD_NEWS_DATA_STATUS
{
	COW_REPORT_GOOD_NEWS_DATA_STATUS_INVALID = 0,						// 初始状态
	COW_REPORT_GOOD_NEWS_DATA_STATUS_LOADING,							// 数据加载中 
	COW_REPORT_GOOD_NEWS_DATA_STATUS_LOAD_FINISH,						// 数据加载完成 
	COW_REPORT_GOOD_NEWS_DATA_STATUS_MAX,
};

static const int COW_REPORT_GOOD_NEWS_SAVE_INTERVAL = 60;				// 1分钟回写一次


class Role;

// 策划说这个是系统不是活动
class CowReportGoodNewsManager
{
	friend class RMIInitCowReportGoodNewsBuyListBackObjectImpl;
public:
	static CowReportGoodNewsManager & Instance();
	void InitParam(int _ret, CowReportGoodNewsBuyListParam & _param);
	void OnServerStart();
	void OnServerStop();
	bool IsLoadFinish() const;
	void Update(unsigned long interval, time_t now_second);
	void OnLogin(Role * _role);
	void OnCowReportGoodNewsOfflineEventByLogin(Role * _role, int _val);

	// 处理来至隐藏服的信息   玩家的购买请求的前置验证
	void OnCowReportGoodNewsCanBuyFromHidden(crossgameprotocal::HiddenGameCowReportGoodNewsCheckCanBuy * _msg);
	// 处理来至隐藏服的信息   玩家的购买请求
	void OnCowReportGoodNewsBuyItemFromHidden(crossgameprotocal::HiddenGameCowReportGoodNewsBuyItem * _msg);
	// 处理来至隐藏服的信息
	void CowReportGoodNewsGetInfoFromHidden(crossgameprotocal::HiddenGameCowReportGoodNewsGetInfo * _get_info);

	// 处理来至主服的信息  玩家额购买请求的前置验证应答
	void OnCowReportGoodNewsCheckCanBuyAckFromGameWorld(crossgameprotocal::GameHiddenCowReportGoodNewsCheckCanBuyAck * _ack);
	// 处理来至隐藏服的信息   玩家的购买请求应答
	void OnCowReportGoodNewsBuyItemAckFromGameWorld(crossgameprotocal::GameHiddenCowReportGoodNewsBuyItemAck * _ack);

	// 可能是 主服 ->隐藏服  也可能是隐藏服 ->主服 
	void OnSyncCowReportGoodNewsBuyItemOfflineEvent(crossgameprotocal::SyncCowReportGoodNewsBuyItemOfflineEvent * _sync);

	// 客户端操作
	void OnClientReq(Role * _role ,Protocol::CSCowReportGoodNewsClientReq * _req);

	// GM测试用指令 , 清理数据 , 约等于重开活动(受配表限制
	void GmCowReportGoodNewsResetData();

private:
	CowReportGoodNewsManager();
	virtual ~CowReportGoodNewsManager();

	void Save();
	void SaveParam();
	void ClearDirtyMark();
	void SetDirtyMark(int _role_id);
	void CheckLoadFinish();
	void ClearDataByFinish();

	void NoticeStatusChange();
	void NoticeStatusChangeByGameWorld();
	void NoticeStatusChangeToHidden();

	void GetInfoToGameWorld(int _role_id, int _type);

	//  系统信息
	void SendInfo(Role * _role);
	void SendInfoByGameWorld(Role * _role);
	void SendInfoToHidden(int _role_id);

	// 奖励信息
	void SendRewardInfo(Role * _role);
	void SendRewardInfoByGameWorld(Role * _role);
	void SendRewardInfoToHidden(int _role_id);

	// 购买信息
	void SendBuyInfo(Role * _role);
	void SendBuyInfoByGameWorld(int _role_id);
	void GmResetSendBuyInfo();

	void SendCanBuyAckToHidden(int _role_id, int _ret);
	void SendBuyAckToHidden(int _role_id, int _ret);
	void SyncCowReportGoodNewsReturnGoldOfflineEvent(int _role_id, int _val);

	// 计算活动状态
	void CalcStatus(unsigned int now_second);
	// 是否是展示期
	bool IsShowTime();
	// 是否是购买期
	bool IsBuyTimes();
	// 是否是结算期
	bool IsRewardTimes();
	// 是否已经结束
	bool IsFinish();
	// 是否买了
	bool IsBuy(int _role_id);

	//  购买物品
	void OnBuyItem(Role * _role);
	void BuyItemByHidden(Role * _role);
	void BuyItemByGameWorld(Role * _role);
	void BuyItemToGameWorld(Role * _role);

	// 开奖
	void CalcReward();
	// 发奖励
	void GiveReward();
	void GiveWinReward(std::set<int > & _set);
	void GiveLoseReward(const std::set<int > & _set);

private:
	int m_data_state;
	bool m_load_flag;
	// 是否有数据发生过变更
	bool m_change_flag;
	// 有信息变动的玩家列表
	std::set< int > m_dirty_mark;
	std::set< int > m_old_state;

	std::map<int , CowReportGoodNewsBuyInfo> m_buy_map;				// 购买了的玩家信息 <uid , 购买信息>

	CommonSave * GetCommonSave() const;
	CommonSaveCowReportGoodNewsData * GetCommonSaveCowReportGoodNewsData( bool is_auto_create = true);
	void SetCommonSaveDirtyMark();
};

#endif	// __COW_REPORT_GOOD_NEWS_HPP__

