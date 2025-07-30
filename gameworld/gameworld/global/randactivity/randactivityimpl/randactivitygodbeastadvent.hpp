#ifndef __RAND_ACTIVITY_GOD_BEAST_ADVENT_HPP__
#define __RAND_ACTIVITY_GOD_BEAST_ADVENT_HPP__

#include "global/randactivity/randactivity.hpp"
#include "gameworld/other/rolemodulemanager.hpp"
#include "servercommon/internalprotocal/crossgameprotocal.h"
#include "gameworld/internalcomm.h"
#include "protocol/randactivity/msgragodbeastadvent.hpp"
#include "servercommon/roleactivity/ragodbeastadventparam.hpp"

class RandActivityGodBeastAdvent : public RandActivity
{
public:
	RandActivityGodBeastAdvent(RandActivityManager * activity_manager);
	~RandActivityGodBeastAdvent();

	virtual void Init(const ActivityData &arg_data);
	virtual void GetInitParam(ActivityData & arg_data);

	void InitBuy(int buy_num);
	void GetInitBuyParam(int& buy_num);

	virtual void Update(unsigned long interval, time_t now_second);
	void CheckNextCycle();		// 检查是否到了下一个周期

	bool IsNextCycle(short cycle_flag);
	int GetCycleFlag() { return m_param.cycle_flag; }
	int GetRewardGroup() { return m_param.reward_group; }
	bool IsInOpen() { return !!is_open; }

	// 是否可上架
	bool CanSell(unsigned int sell_timestamp);
	// 是否属于异兽活动的道具
	bool IsCycleExchangeItem(int item_id);
	// 是否属于本期兑换道具
	bool IsCurCycleExchangeItem(int item_id);

	int BuyServerLimitItem(int user_id);
	void SyncHiddenGameInfo();
	void SynHiddenGameReceiveNumInfo(crossgameprotocal::HiddenGameGodBeastAdventNumInfo * msg);

	// 回收跨服交易行的道具
	void ReceiveTradeMarket(crossgameprotocal::GameHiddenGodBeastAdventReceiveTradeMarketReq * msg);

	void SendNumAllInfo(RoleModuleManager* rmgr = NULL);
	void SendCycleAllInfo(RoleModuleManager* rmgr = NULL);


private:
	int is_open;		// 活动是否开启
	int is_first_sync;	// 跨服启动时同步一次数量

	int buy_limit;			// 全服限购数量,存数据库
	RAGodBeastAdventBuyParam m_param;
};


#endif // !__RAND_ACTIVITY_GOD_BEAST_DEVENT_HPP__
