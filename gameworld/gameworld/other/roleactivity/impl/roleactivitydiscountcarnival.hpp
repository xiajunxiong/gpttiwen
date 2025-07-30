#ifndef __ROLE_ACTIVITY_DISCOUNT_CARNIVAL_HPP__
#define __ROLE_ACTIVITY_DISCOUNT_CARNIVAL_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/radiscountcarnivalparam.hpp"

class RoleActivityDiscountCarnival : public RoleActivity
{
public:
	RoleActivityDiscountCarnival(int type);
	virtual ~RoleActivityDiscountCarnival();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);

	virtual void OnLoginSendInfo();
	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);
	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);

	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);

public:
	void SendRADiscountCarniavalInfo();
	void OnRADiscountCarniavalBuy(int seq, int item_id);

	RADiscountCarnivalParam m_param;
};

#endif	//__ROLE_ACTIVITY_DISCOUNT_CARNIVAL_HPP__
