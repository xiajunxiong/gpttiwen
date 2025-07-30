#ifndef __ROLE_ACTIVITY_SIGN_IN_CONTINUITY_HPP__
#define __ROLE_ACTIVITY_SIGN_IN_CONTINUITY_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/rasignincontinuityparam.hpp"

class RoleActivitySignInContinuity : public RoleActivity
{
public:
	RoleActivitySignInContinuity(int type);
	virtual ~RoleActivitySignInContinuity();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);

	virtual void OnLogin();
	virtual void OnLoginSendInfo();
	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);
	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);
	virtual void OnRAOpenOnlineSend();
	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);

private:
	void OnRASignInContinuityDayReset();

	void SendRASignInContinuityInfo();
	void OnRASignInContinuityReceive(int seq);


	RASignInContinuityParam m_param;
};

#endif	//__ROLE_ACTIVITY_SIGN_IN_CONTINUITY_HPP__
