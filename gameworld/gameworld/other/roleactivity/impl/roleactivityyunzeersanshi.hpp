#ifndef __ROLE_ACTIVITY_YUN_ZE_ER_SAN_SHI_HPP__
#define __ROLE_ACTIVITY_YUN_ZE_ER_SAN_SHI_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/rayunzeersanshiparam.hpp"
#include "protocol/randactivity/msgrayunzeersanshi.hpp"

class RoleActivityYunZeErSanShi : public RoleActivity
{
public:
	RoleActivityYunZeErSanShi(int type);
	virtual ~RoleActivityYunZeErSanShi();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);

 	virtual void OnLoginSendInfo();
 	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);
 	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);
 	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);

private:
	void RASendInfo();
	void SendReward(int task_id);
	void SendTurntableReward();
	void RandTask();

	RAYunZeErSanShiParam m_param;
};


#endif