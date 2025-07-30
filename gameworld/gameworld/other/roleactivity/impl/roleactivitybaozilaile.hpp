#ifndef __ROLE_ACTIVITY_BAO_ZI_LAI_LE_HPP__
#define __ROLE_ACTIVITY_BAO_ZI_LAI_LE_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/rabozhongyouliparam.hpp"

class RoleActivityBaoZiLaiLe : public RoleActivity
{
public:
	RoleActivityBaoZiLaiLe(int type);
	virtual ~RoleActivityBaoZiLaiLe();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);

	virtual void OnLoginSendInfo();
	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);

	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);
	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);

	
	void ReceiveReward(int model, int is_win);

private:
	void RASendInfo();
	void OnJoinActivity();

	RABaoZiLaiLeParam m_param;
};

#endif