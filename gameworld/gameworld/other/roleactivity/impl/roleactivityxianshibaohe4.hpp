#ifndef __ROLE_ACTIVITY_XIAN_SHI_BAO_HE_4_HPP__
#define __ROLE_ACTIVITY_XIAN_SHI_BAO_HE_4_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/randactivityroleparam.hpp"

class RoleActivityXianShiBaoHe4 : public RoleActivity
{
public:
	static const int BUY_TYPE = BUY_TIME_TYPE_XIAN_SHI_BAO_HE_4;
	static const int COMMON_SAVE_TYPE = ROLE_COMMON_SAVE_DATA_TYPE_XIAN_SHI_BAO_HE_4;

	RoleActivityXianShiBaoHe4(int type);
	virtual ~RoleActivityXianShiBaoHe4();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);
	virtual void OnLoginSendInfo();
	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);
	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);

	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);

	int OnBuyGiftItem(int seq, ARG_OUT int * price);

	int OnBuyItemCheck(int seq, long long ARG_OUT & unique_key);
	int OnBuyItem(int chong_zhi_gold, long long unique_id);
public:
	void SendInfo();
	void ReceiveFreeReward();

	RAXianShiBaoHe4Param m_param;
};

#endif	//__ROLE_ACTIVITY_LUCKY_BAG_HPP__
