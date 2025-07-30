#ifndef __ROLE_ACTIVITY_CHONG_JI_FAN_LI_HPP__
#define __ROLE_ACTIVITY_CHONG_JI_FAN_LI_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/rachongjifanliparam.hpp"

class RoleActivityChongJiFanLi : public RoleActivity
{
public:
	RoleActivityChongJiFanLi(int type);
	virtual ~RoleActivityChongJiFanLi();

	void * operator new(size_t c);
	void operator delete(void * m);

	virtual void Init(RoleModuleManager * mgr, const RoleActivityRoleData & p);
	virtual void GetInitParam(RoleActivityRoleData * p);
	virtual void OnLoginSendInfo();
	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);
	virtual void OnRAClose();
	virtual void OnLoginCheck();
	virtual void OnAddChongZhi(long long chongzhi);
	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);

	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq * raor);

	void OnFetchReward(int seq);

private:
	void SendInfo();
	void GiveUnFetchReward();  //活动结束未领取的奖励自动发放

	RAChongJiFanLiParam m_param;
};

#endif	

