#ifndef __ROLE_ACTIVITY_JING_YAN_BIAO_SHENG_HPP__
#define __ROLE_ACTIVITY_JING_YAN_BIAO_SHENG_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/randactivityroleparam.hpp"

class RoleActivityJingYanBiaoSheng: public RoleActivity
{
public:
	RoleActivityJingYanBiaoSheng(int type);
	virtual ~RoleActivityJingYanBiaoSheng();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);
	virtual void OnLoginSendInfo();
	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);
	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);
	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);
public:
	void SendInfo();

	RAJingYanBiaoShengParam m_param;
};

#endif	//__ROLE_ACTIVITY_LUCKY_BAG_HPP__
