#ifndef __ROLE_ACTIVITY_TIME_LIMIT_STORE_HPP__
#define __ROLE_ACTIVITY_TIME_LIMIT_STORE_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/ratimelimitstoreparam.hpp"

class RoleActivityTimeLimitStore : public RoleActivity
{
public:
	RoleActivityTimeLimitStore(int rand_type);
	virtual ~RoleActivityTimeLimitStore();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);

	virtual void OnLoginSendInfo();
	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);

	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);

private:
	void SendRATimeLimitStoreInfo();
	void OnRATimeLimitStoreBuy(int seq, int count);

	RATimeLimitStoreParam m_param;
};


#endif	//__ROLE_ACTIVITY_TIME_LIMIT_STORE_HPP__
