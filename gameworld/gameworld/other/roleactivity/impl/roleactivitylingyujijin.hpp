#ifndef __ROLE_ACTIVITY_LING_YU_JI_JIN_HPP__
#define __ROLE_ACTIVITY_LING_YU_JI_JIN_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/randactivityroleparam.hpp"

class RoleActivityLingYuJiJin : public RoleActivity
{
public:
	static const int BUY_TYPE = BUY_TIME_TYPE_LING_YU_JI_JIN;
	static const int COMMON_SAVE_TYPE = ROLE_COMMON_SAVE_DATA_TYPE_LING_YU_JIN_JI;

	RoleActivityLingYuJiJin(int type);
	virtual ~RoleActivityLingYuJiJin();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);
	virtual void OnLoginSendInfo();
	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);
	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);

	int OnBuyItemCheck(int phase, long long ARG_OUT & unique_key);
	int OnBuyItem(int chong_zhi_gold, long long unique_id);
	int OnFetchReward(int type);
	int OnBuyGiftItem(int phase, ARG_OUT int * price);
public:
	void SendInfo();

	RALingYuJinJiParam m_param;
};

#endif	//__ROLE_ACTIVITY_LUCKY_BAG_HPP__
