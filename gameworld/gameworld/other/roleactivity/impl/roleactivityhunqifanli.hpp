#ifndef __ROLE_ACTIVITY_HUN_QI_FAN_LI_HPP__
#define __ROLE_ACTIVITY_HUN_QI_FAN_LI_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/rahunqifanliparam.hpp"
#include "servercommon/rolecommonsavedef.h"

class RoleActivityHunQiFanLi : public RoleActivity
{
public:
	RoleActivityHunQiFanLi(int type);
	virtual ~RoleActivityHunQiFanLi();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);

	virtual void OnLoginSendInfo();
	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);
	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);
	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);

	virtual void OnAddChongZhi(long long chongzhi);
	
private:
	// 抽奖
	void Lottery();
	// 领取抽奖次数奖励
	void ReceiveLotteryCountReward(int seq);

	void RASendInfo();
private:
	RAHunQiFanLiParam m_param;
};

#endif