#ifndef __ROLE_ACTIVITY_DING_ZHI_BAO_XIANG_3_HPP__
#define __ROLE_ACTIVITY_DING_ZHI_BAO_XIANG_3_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/randactivityroleparam.hpp"

class RoleActivityDingZhiBaoXiang3: public RoleActivity
{
public:
	static const int BUY_TYPE = BUY_TIME_TYPE_DING_ZHI_BAO_XIANG_3;
	static const int COMMON_SAVE_TYPE = ROLE_COMMON_SAVE_DATA_TYPE_DING_ZHI_BAO_XIANG_3;

	RoleActivityDingZhiBaoXiang3(int type);
	virtual ~RoleActivityDingZhiBaoXiang3();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);
	virtual void OnLoginSendInfo();
	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);
	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);

	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);

	bool OnBuyItemOld(int seq, int param1, int chong_zhi_gold);
	int OnBuyGiftItem(int seq, ARG_OUT int * price);		//使用道具获取,不选哪几个,直接给

	int OnBuyItemCheck(int seq, int param1, long long ARG_OUT & unique_key);
	int OnBuyItem(int chong_zhi_gold, long long unique_id);
public:
	void SendInfo();
	void ReceiveFreeReward();

	RADingZhiBaoXiang3Param m_param;
};

#endif	//__ROLE_ACTIVITY_LUCKY_BAG_HPP__
