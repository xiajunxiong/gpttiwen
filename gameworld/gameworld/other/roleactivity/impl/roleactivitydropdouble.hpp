#ifndef __ROLE_ACTIVITY_DROP_DOUBLE_HPP__
#define __ROLE_ACTIVITY_DROP_DOUBLE_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/radropdoubleparam.hpp"

class RoleActivityDropDouble : public RoleActivity
{
public:
	RoleActivityDropDouble(int type);
	virtual ~RoleActivityDropDouble();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);

	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);

public:

	RADropDoubleParam m_param;
};

#endif	//__ROLE_ACTIVITY_DROP_DOUBLE_HPP__
