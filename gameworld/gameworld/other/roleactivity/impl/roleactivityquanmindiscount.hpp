#ifndef __ROLE_ACTIVITY_QUAN_MIN_DISCOUNT_HPP__
#define __ROLE_ACTIVITY_QUAN_MIN_DISCOUNT_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/raquanmindiscountparam.hpp"

class RoleActivityQuanMinDiscount : public RoleActivity
{
public:
	RoleActivityQuanMinDiscount(int type);
	virtual ~RoleActivityQuanMinDiscount();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);

	virtual void OnLoginSendInfo();
	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);
	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);

	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);

	void SendRAQuanMinDiscountInfo();
private:
	void OnRAQuanMinDiscountBuy(int seq);

	RAQuanMinDiscountParam m_param;
};

#endif	//__ROLE_ACTIVITY_QUAN_MIN_DISCOUNT_HPP__
