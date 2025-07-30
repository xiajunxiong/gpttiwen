#ifndef __ROLE_ACTIVITY_JOY_SEEKS_FAVORS_HPP__
#define __ROLE_ACTIVITY_JOY_SEEKS_FAVORS_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/rajoyseeksfavorsparam.hpp"

class RoleActivityJoySeeksFavors : public RoleActivity
{
public:
	RoleActivityJoySeeksFavors(int type);
	virtual ~RoleActivityJoySeeksFavors();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);

	virtual void OnLoginSendInfo();
	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);

	int GetJoySeeksFavorsRoleLevel();
private:
	void SendRAJoySeeksFavorsInfo();

	RAJoySeeksFavorsParam m_param;
};

#endif	//__ROLE_ACTIVITY_JOY_SEEKS_FAVORS_HPP__
