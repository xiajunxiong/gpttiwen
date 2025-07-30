#ifndef __ROLE_ACTIVITY_BACK_REWARD_HPP__
#define __ROLE_ACTIVITY_BACK_REWARD_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/rabackrewardparam.hpp"

class RoleActivityBackReward : public RoleActivity
{
public:
	RoleActivityBackReward(int type);
	virtual ~RoleActivityBackReward();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);

	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);
	virtual void OnLoginSendInfo();
	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);

	void OnReq(int type, int param);
	void CheckTaskRate(int task_type, int add_param = 0, bool is_send = true);

	void GmSetActivity(int type,  int param);

public:
	void RASendInfo();
	void RAFetchDayReward(int day_index);
	void RAFetchTaskReward(int task_index);
	bool IsHaveActivity();

	RABackRewardParam m_param;
};

#endif	//__ROLE_ACTIVITY_BACK_REWARD_HPP__
