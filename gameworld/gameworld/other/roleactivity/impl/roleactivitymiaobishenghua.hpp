#ifndef __ROLE_ACTIVITY_MIAO_BI_SHENG_HUA_HPP__
#define __ROLE_ACTIVITY_MIAO_BI_SHENG_HUA_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/rabozhongyouliparam.hpp"

class RoleActivityMiaoBiShengHua : public RoleActivity
{
public:
	RoleActivityMiaoBiShengHua(int type);
	virtual ~RoleActivityMiaoBiShengHua();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);

	virtual void OnLogin();
	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);

	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);
	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);


	void ReceiveReward();

private:
	void OnJoinActivity();
	void RASendInfo();

	RAMiaoBiShengHuaParam m_param;
};

#endif