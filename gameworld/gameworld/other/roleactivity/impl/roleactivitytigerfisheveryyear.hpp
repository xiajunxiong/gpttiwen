#ifndef __ROLE_ACTIVITY_TIGER_FISH_EVERY_YEAR_HPP__
#define __ROLE_ACTIVITY_TIGER_FISH_EVERY_YEAR_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/ratigerfisheveryyearparam.hpp"
#include "protocol/randactivity/msgratigerfisheveryyear.hpp"


class RoleActivityTigerFishEveryYear : public RoleActivity
{
public:
	RoleActivityTigerFishEveryYear(int type);
	virtual ~RoleActivityTigerFishEveryYear();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);

	virtual void OnLoginSendInfo();
	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);
	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);
	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);

	//任务记录
	void AddTaskParam(int task_type, int task_param = 1);

	void UpdateLevel();		// 32级时更新等级

private:
	void RASendInfo();
	void FishermanTask();					// 完成渔夫任务
	void ReceiveTaskReward(int seq);		// 获取任务奖励
	void ReceiveFishReward(int seq);		// 获取鱼数奖励
	void UpdateAvailableReward();			// 更新奖励档位

	RATigerFishEveryYearParam m_param;
};






#endif
