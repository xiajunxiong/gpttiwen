#ifndef __ROLE_ACTIVITY_SECRET_TREASURE_HPP__
#define __ROLE_ACTIVITY_SECRET_TREASURE_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/rasecrettreasureparam.hpp"

class RoleActivitySecretTreasure : public RoleActivity
{
public:
	RoleActivitySecretTreasure(int type);
	virtual ~RoleActivitySecretTreasure();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);

	virtual void OnLoginSendInfo();
	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);
	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);
	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);

	bool OnCmdBuyItem(int type, int chong_zhi_gold);
private:
	void SendInfo();
	void BuyItem(int type, int seq);
	void SetOpenFlag();

	RASecretTreasureParam m_param;
};


#endif