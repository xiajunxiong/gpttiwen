#ifndef __ROLE_ACTIVITY_NIU_XING_DA_YUN_HPP__
#define __ROLE_ACTIVITY_NIU_XING_DA_YUN_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/raniuxingdayunparam.hpp"

class RoleActivityNiuXingDaYun : public RoleActivity
{
public:
	RoleActivityNiuXingDaYun(int type);
	virtual ~RoleActivityNiuXingDaYun();

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
	void GetReward();

	RANiuXingDaYunParam m_param;
};

#endif	//__ROLE_ACTIVITY_LUCKY_BAG_HPP__

