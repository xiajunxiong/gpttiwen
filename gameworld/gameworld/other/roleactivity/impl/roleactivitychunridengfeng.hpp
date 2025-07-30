#ifndef __ROLE_ACTIVITY_CHUN_RI_DENG_FENG_HPP__
#define __ROLE_ACTIVITY_CHUN_RI_DENG_FENG_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/randactivityroleparam.hpp"

class RoleActivityChunRiDengFeng: public RoleActivity
{
public:
	RoleActivityChunRiDengFeng(int type);
	virtual ~RoleActivityChunRiDengFeng();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);
	virtual void OnLoginSendInfo();
	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);
	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);

	int Explore();
public:
	void SendInfo();

	RAChunRiDengFengParam m_param;
};

#endif	//__ROLE_ACTIVITY_LUCKY_BAG_HPP__
