#ifndef __ROLE_ACTIVITY_REDUCE_TAX_RATE_HPP__
#define __ROLE_ACTIVITY_REDUCE_TAX_RATE_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/rareducetaxrateparam.hpp"

class RoleActivityReduceTaxRate : public RoleActivity
{
public:
	RoleActivityReduceTaxRate(int type);
	virtual ~RoleActivityReduceTaxRate();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);

	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);

public:

	RAReduceTaxRateParam m_param;
};

#endif	//__ROLE_ACTIVITY_REDUCE_TAX_RATE_HPP__
