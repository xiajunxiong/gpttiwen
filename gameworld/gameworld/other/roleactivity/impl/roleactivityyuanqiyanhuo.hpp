#ifndef __ROLE_ACTIVITY_YUAN_QI_YAN_HUO_HPP__
#define __ROLE_ACTIVITY_YUAN_QI_YAN_HUO_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/randactivityroleparam.hpp"

class RoleActivityYuanQiYanHuo: public RoleActivity
{
public:
	static const int BUY_TYPE = BUY_TIME_TYPE_YUAN_QI_YAN_HUO;
	static const int COMMON_SAVE_TYPE = ROLE_COMMON_SAVE_DATA_TYPE_YUAN_QI_YAN_HUO;

	RoleActivityYuanQiYanHuo(int type);
	virtual ~RoleActivityYuanQiYanHuo();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);
	virtual void OnLoginSendInfo();
	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);
	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);
	virtual void OnRAClose();
	virtual void OnLoginCheck();

	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);

	int FetchReward(int seq);
	int FetchFreeReward();
	int OpenYanHuo();

	int OnBuyItemCheck(long long ARG_OUT & unique_key);
	int OnBuyItem(int chong_zhi_gold, long long unique_id);

	int OnBuyGiftItem(ARG_OUT int * price);

private:
	void SendInfo();
	void GiveUnFetchReward();  //活动结束未领取的奖励自动发放

	RAYuanQiYanHuoParam m_param;
};

#endif	
