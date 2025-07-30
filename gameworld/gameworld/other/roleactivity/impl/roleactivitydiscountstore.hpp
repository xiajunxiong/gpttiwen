#ifndef __ROLE_ACTIVITY_DISCOUNT_STORE_HPP__
#define __ROLE_ACTIVITY_DISCOUNT_STORE_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/radiscountstoreparam.hpp"

class RoleActivityDiscountStore : public RoleActivity
{
public:
	RoleActivityDiscountStore(int type);
	virtual ~RoleActivityDiscountStore();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);

	virtual void OnLoginSendInfo();

	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);

	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);

	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);

public:
	void SendRADiscountStoreInfo();
	void OnRADiscountStoreBuy(int seq, int count);

	RADiscountStoreParam m_param;
};

#endif	//__ROLE_ACTIVITY_DISCOUNT_STORE_HPP__
