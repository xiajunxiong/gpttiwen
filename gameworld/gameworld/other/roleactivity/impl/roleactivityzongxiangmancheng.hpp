#ifndef __ROLE_ACTIVITY_ZONG_XIANG_MAN_CHENG_HPP__
#define __ROLE_ACTIVITY_ZONG_XIANG_MAN_CHENG_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/razongxiangparam.hpp"
#include "global/randactivity/randactivityimpl/randactivityzongxiangmancheng.hpp"

class RoleActivityZongXiangManCheng : public RoleActivity
{
public:
	RoleActivityZongXiangManCheng(int type);
	virtual ~RoleActivityZongXiangManCheng();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);

	virtual void OnLoginSendInfo();
	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);

	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);
	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);

	void ActiveSeq(int seq);
private:
	void RASendInfo();

	RAZongXiangManChengParam m_param;
};

#endif