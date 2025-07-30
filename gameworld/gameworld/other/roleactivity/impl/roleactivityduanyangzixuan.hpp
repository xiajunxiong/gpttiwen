#ifndef __ROLE_ACTIVITY_DUAN_YANG_ZI_XUAN_HPP__
#define __ROLE_ACTIVITY_DUAN_YANG_ZI_XUAN_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/randactivityroleparam.hpp"

class RoleActivityDuanYangZiXuan : public RoleActivity
{
public:
	static const int BUY_TYPE = BUY_TIME_TYPE_DUAN_YANG_ZI_XUAN;			//端阳自选
	static const int COMMON_SAVE_TYPE = ROLE_COMMON_SAVE_DATA_TYPE_DUAN_YANG_ZI_XUAN;

	RoleActivityDuanYangZiXuan(int type);
	virtual ~RoleActivityDuanYangZiXuan();

	void * operator new(size_t c);
	void operator delete(void * m);

	virtual void Init(RoleModuleManager * mgr, const RoleActivityRoleData & p);
	virtual void GetInitParam(RoleActivityRoleData * p);
	virtual void OnLoginSendInfo();
	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);
	virtual void OnRAClose();
	virtual void OnLoginCheck();

	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq * raor);

	int FetchReward(int day, int seq);

	int OnBuyItemCheck(long long ARG_OUT & unique_key);
	int OnBuyItem(int chong_zhi_gold, long long unique_id);

	int OnBuyGiftItem(ARG_OUT int * price);
private:
	void SendInfo();
	void GiveUnFetchReward();  //活动结束未领取的奖励自动发放

	RADuanYangZiXuanParam m_param;
};

#endif	

