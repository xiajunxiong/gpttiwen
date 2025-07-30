#ifndef __ROLE_ACTIVITY_SUPER_OFF_SHOP_HPP__
#define __ROLE_ACTIVITY_SUPER_OFF_SHOP_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/racoolsummerparam.hpp"

class RoleActivitySuperOffShop : public RoleActivity
{
public:
	RoleActivitySuperOffShop(int type);
	virtual ~RoleActivitySuperOffShop();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);

	virtual void OnLoginSendInfo();
	/*virtual void OnRAOpenOnlineSend();*/
	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);
	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);
	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);

	bool OnCmdBuyItem(long long unique_id, int chong_zhi_gold);
private:
	void SendInfo(bool is_recal_phase = false);
	void SendSingleInfo(int notice_type, int param1, int param2);
	void OnBuyNormalItem(int seq);
	void OnBuyDiscItem(int seq);
	void OnBuyItemChecek(int seq, bool is_normal = true);

	RASuperOffShopParam m_param;
};


#endif 