#ifndef __ROLE_ACTIVITY_DUAN_YANG_XIANG_NANG_HPP__
#define __ROLE_ACTIVITY_DUAN_YANG_XIANG_NANG_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/raduanyangxiangnangparam.hpp"

class RoleActivityDuanYangXiangNang : public RoleActivity
{
public:
	RoleActivityDuanYangXiangNang(int type);
	virtual ~RoleActivityDuanYangXiangNang();

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);
	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);

	virtual void OnLogin();
	virtual void Update(time_t now_second);
	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);

	// 任务进度
	void AddTaskParam(int task_type, int task_param = 1);
	// 领取奖励
	void ReceiveReward(int seq);
	// 制作香囊
	void MakeSachet();

private:
	void RASendInfo();

	unsigned int m_online_task_timestamp;

	RADuanYangXiangNangParam m_param;
};


#endif
