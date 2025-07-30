#ifndef __ROLE_ACTIVITY_SALTY_SWEET_BATTLE_HPP__
#define __ROLE_ACTIVITY_SALTY_SWEET_BATTLE_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/razongxiangparam.hpp"

class RoleActivitySaltySweetBattle : public RoleActivity
{
public:
	RoleActivitySaltySweetBattle(int type);
	virtual ~RoleActivitySaltySweetBattle();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);
	virtual void OnRAClose();
	virtual void OnLoginSendInfo();
	virtual void OnLoginCheck();
	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);

	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);
	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);

	void GetReward();
private:
	void RASendInfo();

	RASaltySweetBattleParam m_param;
};

#endif