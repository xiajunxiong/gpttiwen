#ifndef __ROLE_ACTIVITY_SIGN_HAPPY_HPP__
#define __ROLE_ACTIVITY_SIGN_HAPPY_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/rasignhappyparam.hpp"

class RoleActivitySignHappy : public RoleActivity
{
public:
	RoleActivitySignHappy(int type);
	virtual ~RoleActivitySignHappy();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);

	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);
	virtual void OnLoginSendInfo();
	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);
	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);

public:
	void RASendInfo();
	void OnFetchReward(int index);
	void OnFetchAllReward();

	RASignHappyParam m_param;
};

#endif	//__ROLE_ACTIVITY_SIGN_HAPPY_HPP__
