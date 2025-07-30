#ifndef __ROLE_ACTIVITY_COOL_BREEZE_HPP__
#define __ROLE_ACTIVITY_COOL_BREEZE_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/racoolsummerparam.hpp"

class RoleActivityCoolBreeze : public RoleActivity
{
public:
	RoleActivityCoolBreeze(int type);
	virtual ~RoleActivityCoolBreeze();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);
	virtual void Update(time_t now_second);

	virtual void OnConsumeGold(long long consume_gold);
	virtual void OnLoginSendInfo();
	/*virtual void OnRAOpenOnlineSend();*/
	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);
	//virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);
	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);

	void OnTaskEventInfoChange(int task_type, int param1, int param2 = 0);
private:
	void SendInfo();
	void OnSendSingleInfo(int index, short task_group_type, RACoolBreezeTaskInfo & task_info);
	void OnCommitTask(int index, bool is_rand_task);
	void OnRefreshFixeTask(int index);
	void OnTimeOutCheckTaskStatus(const RACoolBreezeTaskInfo & task_info);

	RACoolBreezeTaskInfo & GetTaskInfoByIndex(int index);

	RACoolBreezeParam m_param;
};



#endif