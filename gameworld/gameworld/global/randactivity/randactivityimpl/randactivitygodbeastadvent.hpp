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
	void CheckNextCycle();		// ����Ƿ�����һ������

	bool IsNextCycle(short cycle_flag);
	int GetCycleFlag() { return m_param.cycle_flag; }
	int GetRewardGroup() { return m_param.reward_group; }
	bool IsInOpen() { return !!is_open; }

	// �Ƿ���ϼ�
	bool CanSell(unsigned int sell_timestamp);
	// �Ƿ��������޻�ĵ���
	bool IsCycleExchangeItem(int item_id);
	// �Ƿ����ڱ��ڶһ�����
	bool IsCurCycleExchangeItem(int item_id);

	int BuyServerLimitItem(int user_id);
	void SyncHiddenGameInfo();
	void SynHiddenGameReceiveNumInfo(crossgameprotocal::HiddenGameGodBeastAdventNumInfo * msg);

	// ���տ�������еĵ���
	void ReceiveTradeMarket(crossgameprotocal::GameHiddenGodBeastAdventReceiveTradeMarketReq * msg);

	void SendNumAllInfo(RoleModuleManager* rmgr = NULL);
	void SendCycleAllInfo(RoleModuleManager* rmgr = NULL);


private:
	int is_open;		// ��Ƿ���
	int is_first_sync;	// �������ʱͬ��һ������

	int buy_limit;			// ȫ���޹�����,�����ݿ�
	RAGodBeastAdventBuyParam m_param;
};


#endif // !__RAND_ACTIVITY_GOD_BEAST_DEVENT_HPP__
