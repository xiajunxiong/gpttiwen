#ifndef __ROLE_ACTIVITY_YUAN_QI_TURNING_HPP__
#define __ROLE_ACTIVITY_YUAN_QI_TURNING_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/rayuanqiturningparam.hpp"

class RoleActivityYuanQiTurning : public RoleActivity
{
public:
	RoleActivityYuanQiTurning(int type);
	virtual ~RoleActivityYuanQiTurning();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);

	virtual void OnLoginSendInfo();
	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);
	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);

	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);
private:
	void Turning(int open_idx);
	void SendInfo();
	RAYuanQiTurningParam m_param;
};


#endif 