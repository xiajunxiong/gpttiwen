#ifndef __ROLE_ACTIVITY_LINKAGE_GIFT_HPP__
#define __ROLE_ACTIVITY_LINKAGE_GIFT_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/ralinkagegiftparam.hpp"

class RoleActivityLinkageGift : public RoleActivity
{
public:
	RoleActivityLinkageGift(int type);
	virtual ~RoleActivityLinkageGift();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);

	virtual void OnLoginSendInfo();
	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);
	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);

	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);
private:
	void OnCheckLinkageGiftCondition();
	void SendRALinkageGiftInfo();
	void OnRALinkageGiftFetchReward();

	RALinkageGiftParam m_param;
};

#endif	//__ROLE_ACTIVITY_LINKAGE_GIFT_HPP__
