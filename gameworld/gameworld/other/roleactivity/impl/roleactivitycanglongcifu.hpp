#ifndef __ROLE_ACTIVITY_CANG_LONG_CI_FU_HPP__
#define __ROLE_ACTIVITY_CANG_LONG_CI_FU_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/raduanyangxiangnangparam.hpp"

class RoleActivityCangLongCiFu : public RoleActivity
{
public:
	RoleActivityCangLongCiFu(int type);
	virtual ~RoleActivityCangLongCiFu();

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);
	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);

	virtual void OnLoginSendInfo();
	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);

	// 点亮
	void LightUp(int seq);
	// 领取全部点亮奖励
	void ReceiveAllLinghtUp();
	// 补发未领取的奖励
	void ReissueReward();

private:
	void RASendInfo();
	
	RACangLongCiFuParam m_param;
};

#endif