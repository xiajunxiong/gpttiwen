#ifndef __ROLE_ACTIVITY_LUCKY_DISCOUNT_HPP__
#define __ROLE_ACTIVITY_LUCKY_DISCOUNT_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/raluckydiscountparam.hpp"

class RoleActivityLuckyDiscount: public RoleActivity
{
public:
	RoleActivityLuckyDiscount(int type);
	virtual ~RoleActivityLuckyDiscount();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);
	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);

	virtual void OnLoginSendInfo();

	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);

	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);

public:
	void SendRALuckyDiscountInfo();
	void OnRALuckyDiscountGetDiscount();
	void OnRALuckyDiscountBuy(int seq, int count);

	RALuckyDiscountParam m_param;
};

#endif	//__ROLE_ACTIVITY_LUCKY_DISCOUNT_HPP__
