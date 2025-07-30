#ifndef __ROLE_ACTIVITY_LAN_TANG_CHU_MU_HPP__
#define __ROLE_ACTIVITY_LAN_TANG_CHU_MU_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/raduanyangxiangnangparam.hpp"

class RoleActivityLanTangChuMu : public RoleActivity
{
public:
	RoleActivityLanTangChuMu(int type);
	virtual ~RoleActivityLanTangChuMu();

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
	// 加入材料  需要道具的索引,数量
	void AddHerbs(int seq, int num);
	// 开始沐浴
	void StartBathe();

private:
	void RASendInfo();

	unsigned int m_online_task_timestamp;

	RALanTangChuMuParam m_param;
};


#endif