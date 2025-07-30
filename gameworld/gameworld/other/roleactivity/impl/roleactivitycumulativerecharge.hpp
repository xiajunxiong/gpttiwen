#ifndef __ROLE_ACTIVITY_CUNULATIVE_RECHARGE_HPP__
#define __ROLE_ACTIVITY_CUNULATIVE_RECHARGE_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/racumulativerechargeparam.hpp"

class RoleActivityCumulativeRecharge : public RoleActivity
{
public:
	RoleActivityCumulativeRecharge(int type);
	virtual ~RoleActivityCumulativeRecharge();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);

	virtual void OnLoginSendInfo();
	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);

	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);

	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);
	virtual void OnAddChongZhi(long long chongzhi);
	void OnBuyItemCfgCheck();
	bool OnBuyItem(int chong_zhi_gold, int type);
	int OnBuyGiftItem(ARG_OUT int * price);
public:
	void SendInfo();
	void GetReward();
	void ReceiveFreeGift();
	int GetEquipScore();

	RACumulativeChargeParam m_param;
};

#endif	//__ROLE_ACTIVITY_CUNULATIVE_RECHARGE_HPP__
