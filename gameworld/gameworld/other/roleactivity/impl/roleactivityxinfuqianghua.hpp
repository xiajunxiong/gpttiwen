#ifndef __ROLE_ACTIVITY_XIN_FU_QIANG_HUA_HPP__
#define __ROLE_ACTIVITY_XIN_FU_QIANG_HUA_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/raxinfuqianghuaparam.hpp"

class RoleActivityXinFuQiangHua : public RoleActivity
{
public:
	RoleActivityXinFuQiangHua(int type);
	virtual ~RoleActivityXinFuQiangHua();

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);
	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);

	virtual void OnLogin();
	virtual void OnLoginCheck();
	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);

	void StrengthenTaskFinish(int pet_quality, int str_level);

	void ReceiveFreeReward();		// 领取每日免费奖励
	void ReceiveStrengthenReward(int pet_quality, int seq);	// 领取强化奖励
	void ReissueAllReward();		// 活动结束补发全部奖励

private:
	void RASendInfo();

	RAXinFuQiangHuaParam m_param;
};


#endif