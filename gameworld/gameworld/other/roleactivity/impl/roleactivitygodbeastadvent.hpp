#ifndef __ROLE_ACTIVITY_GOD_BEAST_ADVENT_HPP__
#define __ROLE_ACTIVITY_GOD_BEAST_ADVENT_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/ragodbeastadventparam.hpp"
#include "servercommon/rolecommonsavedef.h"

#include "servercommon/errornum.h"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "servercommon/internalprotocal/crossgameprotocal.h"
#include "gameworld/internalcomm.h"

class RoleActivityGodBeastAdvent : public RoleActivity
{
public:
	static const int BUY_TYPE = BUY_TIME_TYPE_GOD_BEAST_ADVENT;
	static const int COMMON_SAVE_TYPE = ROLE_COMMON_SAVE_DATA_TYPE_GOD_BEAST_ADVENT;

	RoleActivityGodBeastAdvent(int type);
	virtual ~RoleActivityGodBeastAdvent();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);

	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);
	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);

	virtual void OnLogin();
	virtual void Update(time_t now_second);
	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);

	// 任务进度
	void AddTaskParam(int task_type, int task_param = 1);
	// 领取奖励
	void ReceiveReward(int seq);
	// 补发前一天奖励
	void ReissueReward();
	// 兑换
	void ExchangeReward(int seq);
	void OnHiddenGameBuyRecviceReq(crossgameprotocal::HiddenGameGodBeastAdventInfo* info);		// 处理跨服返回的ret

	// 回收上一次周期的道具
	void RecoverItem();

	// gm增加好感度
	void GmAddLike(int add_num);

	// 是否购买了门票
	bool IsCanReceive() { return !!m_param.zhigou_flag; }

	int OnBuyItemCheck(long long ARG_OUT & unique_key);
	int OnCmdBuyItem(int chong_zhi_gold, long long unique_id);
private:
	void RASendInfo();

	unsigned int online_task_timestamp;			// 在线时长任务时间戳

	RAGodBeastAdventParam m_param;
};

#endif