#ifndef __ROLE_ACTIVITY_ONE_YUAN_GOU_HPP__
#define __ROLE_ACTIVITY_ONE_YUAN_GOU_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/randactivityroleparam.hpp"

class RoleActivityOneYuanGou : public RoleActivity
{
public:
	static const int BUY_TYPE = BUY_TIME_TYPE_ONE_YUAN_GOU;
	static const int COMMON_SAVE_TYPE = ROLE_COMMON_SAVE_DATA_TYPE_ONE_YUAN_GOU;

	RoleActivityOneYuanGou(int type);
	virtual ~RoleActivityOneYuanGou();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager * mgr, const RoleActivityRoleData & p);
	virtual void GetInitParam(RoleActivityRoleData * p);
	virtual void OnLoginSendInfo();
	virtual void OnAddChongZhi(long long chongzhi);
	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);
	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq * raor);

	int OnBuyItemCheck(long long ARG_OUT & unique_key);
	int OnBuyItem(int chong_zhi_gold, long long unique_id);
public:
	void SendInfo(bool is_chong_zhi = false);

	RAOneYuanGouParam m_param;
};

#endif
