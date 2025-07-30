#ifndef __ROLE_ACTIVITY_QIYUAN_KONGMINGDENG_HPP__
#define __ROLE_ACTIVITY_QIYUAN_KONGMINGDENG_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/raqiyuankongmingdengparam.hpp"

class RoleActivityQiYuanKongMingDeng : public RoleActivity
{
public:
	RoleActivityQiYuanKongMingDeng(int type);
	virtual ~RoleActivityQiYuanKongMingDeng();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);

	virtual void OnLoginSendInfo();
	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);
	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);

	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);

	void OnCompleteTask(int task_type, int add_param = 1, bool is_reward = true);

public:
	void RASendInfo();
	void RADianDeng();
	void OnJoinActivity();

	void GMAddHuoZhong(int add_huozhong);


	RAQiYuanKongMingDengParam m_param;
};

#endif	//__ROLE_ACTIVITY_QIYUAN_KONGMINGDENG_HPP__
