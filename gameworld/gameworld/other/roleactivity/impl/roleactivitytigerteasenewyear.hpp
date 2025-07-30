#ifndef __ROLE_ACTIVITY_TIGER_TEASE_NEW_YEAR_HPP__
#define __ROLE_ACTIVITY_TIGER_TEASE_NEW_YEAR_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/ratigerteasenewyearparam.hpp"
#include "protocol/randactivity/msgratigerteasenewyear.hpp"

class RoleActivityTigerTeaseNewYear : public RoleActivity
{
public:
	RoleActivityTigerTeaseNewYear(int type);
	virtual ~RoleActivityTigerTeaseNewYear();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);

	virtual void OnLoginSendInfo();
	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);
	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);
	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);

private:
	void RASendInfo(int is_win = 0);
	void Mora();

	RATigerTeaseNewYearParam m_param;
};


#endif