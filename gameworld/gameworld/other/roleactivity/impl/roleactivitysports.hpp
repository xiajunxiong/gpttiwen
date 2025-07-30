#ifndef __ROLE_ACTIVITY_SPORTS_HPP__
#define __ROLE_ACTIVITY_SPORTS_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/rasportsparam.hpp"

class RoleActivitySports: public RoleActivity
{
public:
	RoleActivitySports(int type);
	virtual ~RoleActivitySports();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);

	virtual void OnLoginSendInfo();
	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);

	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);
	short & GetHistroyTop() { return m_param.ra_sports_histroy_top; }
	void OnRaSportsHistroyEnd();
private:

	void SendRASportsHistroyInfo();
	void GetRaSportHistroyReward(int seq);

	RASportsParam m_param;
};

#endif	//__ROLE_ACTIVITY_SPORTS_HPP__
