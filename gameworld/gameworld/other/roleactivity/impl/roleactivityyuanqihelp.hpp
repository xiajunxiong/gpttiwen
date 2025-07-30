#ifndef __ROLE_ACTIVITY_YUAN_QI_HLEP_HPP__
#define __ROLE_ACTIVITY_YUAN_QI_HLEP_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/rayuanqihelpparam.hpp"
#include "protocol/randactivity/msgrayuanqihelp.hpp"

class RoleActivityYuanQiHelp : public RoleActivity
{
public:
	RoleActivityYuanQiHelp(int type);
	virtual ~RoleActivityYuanQiHelp();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);

	virtual void Update(time_t now_second);
 	virtual void OnLoginSendInfo();
	virtual void OnRoleLevelUp(int old_level, int to_level);
 	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);
 	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);
 	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);

	void CommitTask(bool is_finish);
	void SendTaskInfo();
private:
	void RandTask();
	void OnJoinActivity();

	RAYuanQiHelpParam m_param;
};

#endif