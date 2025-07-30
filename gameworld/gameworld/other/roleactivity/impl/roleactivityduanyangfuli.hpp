#ifndef __ROLE_ACTIVITY_DUAN_YANG_FU_LI_HPP__
#define __ROLE_ACTIVITY_DUAN_YANG_FU_LI_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/randactivityroleparam.hpp"

class RoleActivityDuanYangFuLi : public RoleActivity
{
public:
	static const int BUY_TYPE = BUY_TIME_TYPE_DUAN_YANG_FU_LI;			//端阳福礼
	static const int COMMON_SAVE_TYPE = ROLE_COMMON_SAVE_DATA_TYPE_DUAN_YANG_FU_LI;

	RoleActivityDuanYangFuLi(int type);
	virtual ~RoleActivityDuanYangFuLi();

	void * operator new(size_t c);
	void operator delete(void * m);

	virtual void Init(RoleModuleManager * mgr, const RoleActivityRoleData & p);
	virtual void GetInitParam(RoleActivityRoleData * p);
	virtual void OnLoginSendInfo();
	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);
	virtual void OnRAClose();
	virtual void OnLoginCheck();

	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq * raor);

	int FetchReward(int seq);

	int OnBuyItemCheck(int seq, bool is_buy_all, long long ARG_OUT & unique_key);
	int OnBuyItem(int chong_zhi_gold, long long unique_id);

	int OnBuyGiftItem(int seq, bool is_buy_all, ARG_OUT int * price);
private:
	void SendInfo();
	void GiveUnFetchReward();  //活动结束未领取的奖励自动发放

	RADuanYangFuLiParam m_param;
};

#endif	

