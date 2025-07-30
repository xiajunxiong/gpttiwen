#ifndef __ROLE_ACTIVITY_SEAL_PROMOTE_HPP__
#define __ROLE_ACTIVITY_SEAL_PROMOTE_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/rasealpromoteparam.hpp"

class RoleActivitySealPromote : public RoleActivity
{
public:
	RoleActivitySealPromote(int type);
	virtual ~RoleActivitySealPromote();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);

	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);

public:

	RASealPromoteParam m_param;
};

#endif	//__ROLE_ACTIVITY_SEAL_PROMOTE_HPP__
