#ifndef __ROLE_ACTIVITY_TIGER_LUCKY_MONEY_HPP__
#define __ROLE_ACTIVITY_TIGER_LUCKY_MONEY_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/ratigerluckymoneyparam.hpp"
#include "protocol/randactivity/msgratigerluckymoney.hpp"

class RoleActivityTigerLuckyMoney : public RoleActivity
{
public:
	RoleActivityTigerLuckyMoney(int type);
	virtual ~RoleActivityTigerLuckyMoney();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);

	virtual void Update(time_t now_second);
	virtual void OnLoginSendInfo();
	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);
	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);
	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);

	//�����¼
	void AddTaskParam(int task_type, int task_param = 1);
	void LevelUpToFirstTask(int cur_level);			// ��������һ���ɽ�����׶�

private:
	void RASendInfo();
	void ReceiveTaskReward(int seq);	// ��ȡ���������
	void ReceiveLuckyMoney();			// ��ȡѹ��Ǯ����

	RATigerLuckyMoneyParam m_param;
};


#endif