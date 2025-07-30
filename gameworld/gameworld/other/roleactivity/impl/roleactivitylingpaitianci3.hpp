#ifndef __ROLE_ACTIVITY_LING_PAI_TIAN_CI_3_HPP__
#define __ROLE_ACTIVITY_LING_PAI_TIAN_CI_3_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/randactivityroleparam.hpp"

class RoleActivityLingPaiTianCi3 : public RoleActivity
{
public:
	RoleActivityLingPaiTianCi3(int type);
	virtual ~RoleActivityLingPaiTianCi3();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);
	virtual void OnLoginSendInfo();
	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);
	virtual void OnAddChongZhi(long long chongzhi);
	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);

	int FetchRewards(int  seq);
public:
	void SendInfo();

	RALingPaiTianCi3Param m_param;
};

#endif	//__ROLE_ACTIVITY_LUCKY_BAG_HPP__
