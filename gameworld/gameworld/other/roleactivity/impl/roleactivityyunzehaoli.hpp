#ifndef __ROLE_ACTIVITY_YUN_ZE_HAO_LI_HPP__
#define __ROLE_ACTIVITY_YUN_ZE_HAO_LI_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/rayunzehaoliparam.hpp"
#include "protocol/randactivity/msgrayunzehaoli.hpp"

class RoleActivityYunZeHaoLi : public RoleActivity
{
public:
	RoleActivityYunZeHaoLi(int type);
	virtual ~RoleActivityYunZeHaoLi();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);

	virtual void Update(time_t now_second);
 	virtual void OnLoginSendInfo();
 	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);
 	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);
 	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);

private:
	void RASendInfo();
	time_t ForHourMinuteGetTimestamp(int hour_minute);

	void SmashEgg(int is_advanced_item);
	void BuyGoldHammer();

	RAYunZeHaoLiParam m_param;
};






#endif