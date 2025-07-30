#ifndef __ROLE_ACTIVITY_SALE_SHOP_HPP__
#define __ROLE_ACTIVITY_SALE_SHOP_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/rasaleshopparam.hpp"

class RoleActivitySaleShop : public RoleActivity
{
public:
	RoleActivitySaleShop(int type);
	virtual ~RoleActivitySaleShop();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);

	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);

	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);

	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);

public:
	void SendRASaleShopInfo(int item_type);

	void OnRASaleShopBuy(int item_type, int item_seq, int count);


	RASaleShopParam m_param;
};

#endif	//__ROLE_ACTIVITY_SALE_SHOP_HPP__
