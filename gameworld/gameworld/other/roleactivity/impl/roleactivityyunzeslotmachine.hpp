#ifndef __ROLE_ACTIVITY_YUN_ZE_SLOT_MACHINE_HPP__
#define __ROLE_ACTIVITY_YUN_ZE_SLOT_MACHINE_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "protocol/randactivity/msgrayunzeslotmachine.hpp"
#include "servercommon/roleactivity/rayunzeslotmachineparam.hpp"
#include "servercommon/rolecommonsavedef.h"

class RoleActivityYunZeSlotMachine : public RoleActivity
{
public:
	static const int BUY_TYPE = BUY_TIME_TYPE_YUN_ZE_SLOT_MACHINE;
	static const int COMMON_SAVE_TYPE = ROLE_COMMON_SAVE_DATA_TYPE_YUN_ZE_SLOT_MACHINE;

	RoleActivityYunZeSlotMachine(int type);
	virtual ~RoleActivityYunZeSlotMachine();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);

	virtual void Update(time_t now_second);
	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);
 	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);
	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);
	virtual void OnLoginSendInfo();

	int OnBuyGiftItem(ARG_OUT int * price);

	int OnBuyItemCheck(long long ARG_OUT & unique_key);
	int OnCmdBuyItem(int chong_zhi_gold, long long unique_id);

private:
	void RASendInfo();
	int RASendRandResultInfo(int rand_result);

	void Lottery();
	void LockRewardPool(int reward_id);
	void BuyLotteryItem();
	void SendReward(int reward_pool);

	RAYunZeSlotMachineParam m_param;
};



#endif