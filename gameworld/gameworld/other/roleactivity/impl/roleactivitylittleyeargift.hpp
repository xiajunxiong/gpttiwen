#ifndef __ROLE_ACTIVITY_LITTLE_YEAR_GIFT_HPP__
#define __ROLE_ACTIVITY_LITTLE_YEAR_GIFT_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/ralittleyeargiftparam.hpp"

class RoleActivityLittleYearGift : public RoleActivity
{
public:
	RoleActivityLittleYearGift(int type);
	virtual ~RoleActivityLittleYearGift();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);
	virtual void Update(time_t now_second) {}
	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);

	virtual void OnLoginSendInfo();
	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);

	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);
public:
	void SendInfo();
	void TalkToNpc(int npc_id);

	RALittleYearGiftParam m_param;
};

#endif	//__ROLE_ACTIVITY_LUCKY_BAG_HPP__

