#ifndef __ROLE_ACTIVITY_SHAN_HAI_ZHENG_FENG_HPP__
#define __ROLE_ACTIVITY_SHAN_HAI_ZHENG_FENG_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/raburningsummerparam.hpp"

class RoleActivityShanHaiZhengFeng : public RoleActivity
{
public:
	RoleActivityShanHaiZhengFeng(int type);
	virtual ~RoleActivityShanHaiZhengFeng();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);

	virtual void OnLoginSendInfo();
	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);
	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);
	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);

	void OnBattleFinish(int activity_time, bool is_win);
private:
	void SendAllInfo();
	void OnChallenge(int activity_time);
	void OnFetchAllPassReward();
	bool IsLevelUnLock(int activity_time);

	RASummerLiLianParam m_param;

};

#endif // __ROLE_ACTIVITY_SUMMER_LI_LIAN_HPP__