#ifndef __ROLE_ACTIVITY_TIGER_FIND_SPRING_HPP__
#define __ROLE_ACTIVITY_TIGER_FIND_SPRING_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/ratigerfindspringparam.hpp"
#include "protocol/randactivity/msgratigerfindspring.hpp"

class RoleActivityTigerFindSpring : public RoleActivity
{
public:
	RoleActivityTigerFindSpring(int type);
	virtual ~RoleActivityTigerFindSpring();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);

	virtual void OnLoginSendInfo();
	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);
	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);
	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);

private:
	void RASendInfo();
	void ApplyFinishTask();
	void ReceiveReward();

	RATigerFindSpringParam m_param;
};


#endif