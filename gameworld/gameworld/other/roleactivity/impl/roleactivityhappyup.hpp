#ifndef __ROLE_ACTIVITY_HAPPY_UP_HPP__
#define __ROLE_ACTIVITY_HAPPY_UP_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/rahappyupparam.hpp"

class RoleActivityHappyUp : public RoleActivity
{
public:
	RoleActivityHappyUp(int type);
	virtual ~RoleActivityHappyUp();

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);

	virtual void OnLoginSendInfo();
	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);

	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);

	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);

public:
	void SendRAHappyUpInfo();

	void OnRAHappyUpExchange(int item_seq, int count);

	RAHappyUpParam m_param;
};

#endif	//__ROLE_ACTIVITY_HAPPY_UP_HPP__
