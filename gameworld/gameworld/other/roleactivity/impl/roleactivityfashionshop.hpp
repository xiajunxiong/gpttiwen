#ifndef __ROLE_ACTIVITY_FASHION_SHOP_HPP__
#define __ROLE_ACTIVITY_FASHION_SHOP_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/randactivityroleparam.hpp"

class RoleActivityFashionShop : public RoleActivity
{
public:
	static const int BUY_TYPE = BUY_TIME_TYPE_FASHION_SHOP;
	static const int COMMON_SAVE_TYPE = ROLE_COMMON_SAVE_DATA_TYPE_FASHION_SHOP;

	RoleActivityFashionShop(int type);
	virtual ~RoleActivityFashionShop();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);
	virtual void OnLoginSendInfo();
	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);
	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);

	int BuyItemByGold(int seq);
	int OnBuyItemCheck(int seq, long long ARG_OUT & unique_key);
	int OnBuyItem(int chong_zhi_gold, long long unique_id);
	int OnBuyGiftItem(int seq, ARG_OUT int * price);
public:
	void SendInfo();

	RAFashionShopParam m_param;
};

#endif	//__ROLE_ACTIVITY_LUCKY_BAG_HPP__
