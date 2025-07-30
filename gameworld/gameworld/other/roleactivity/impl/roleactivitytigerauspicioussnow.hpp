#ifndef __ROLE_ACTIVITY_TIGER_AUSPICIOUS_SNOW_HPP__
#define __ROLE_ACTIVITY_TIGER_AUSPICIOUS_SNOW_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/ratigerauspicioussnowparam.hpp"
#include "protocol/randactivity/msgratigerauspicioussnow.hpp"

class RoleActivityTigerAuspiciousSnow : public RoleActivity
{
public:
	RoleActivityTigerAuspiciousSnow(int type);
	virtual ~RoleActivityTigerAuspiciousSnow();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);

	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);
	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);
	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);
	virtual void OnLoginSendInfo();


	void RATigerSpSnowAddTimes();
	int GetRATigerSpSnowTimes() { return m_param.times; }

	void RASendInfo();
private:	
	void SnowBallLottery();
	void Exchange();
	bool ConsumeAllItem();


	RATigerAuspiciousSnowParam m_param;
};



#endif