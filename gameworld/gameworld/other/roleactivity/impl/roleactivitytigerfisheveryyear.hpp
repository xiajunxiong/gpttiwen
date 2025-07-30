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

	//�����¼
	void AddTaskParam(int task_type, int task_param = 1);

	void UpdateLevel();		// 32��ʱ���µȼ�

private:
	void RASendInfo();
	void FishermanTask();					// ����������
	void ReceiveTaskReward(int seq);		// ��ȡ������
	void ReceiveFishReward(int seq);		// ��ȡ��������
	void UpdateAvailableReward();			// ���½�����λ

	RATigerFishEveryYearParam m_param;
};






#endif
