#ifndef __ROLE_ACTIVITY_INTEGRAL_DRAW_HPP__
#define __ROLE_ACTIVITY_INTEGRAL_DRAW_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/raintegraldrawparam.hpp"

class RoleActivityIntegralDraw : public RoleActivity
{
public:
	RoleActivityIntegralDraw(int type);
	virtual ~RoleActivityIntegralDraw();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);

	virtual void OnLoginSendInfo();
	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);
	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);

	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);

public:
	void RASendInfo();
	void RADraw();
	void RAFetchReward(int index);

	RAIntegralDrawParam m_param;
};

#endif	//__ROLE_ACTIVITY_INTEGRAL_DRAW_HPP__
