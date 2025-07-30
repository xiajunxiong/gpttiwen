#ifndef __ROLE_ACTIVITY_TIAN_MING_DIVINATION_HPP__
#define __ROLE_ACTIVITY_TIAN_MING_DIVINATION_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/ratianmingdivinationparam.hpp"

class RoleActivityTianMingDivination : public RoleActivity
{
public:
	RoleActivityTianMingDivination(int type);
	virtual ~RoleActivityTianMingDivination();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void OnLogin();
	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);
	virtual void OnLoginSendInfo();
	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);
	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);
	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);

private:
	void SendRATianMingDivinationActivityInfo();
	void OnRATianMingDivinationAddLotTimes(int lot_index);
	int GetRATianMingDivinationTotalAddLotTimes();
	void OnRATianMingDivinationResetAddLotTimes();
	void OnRATianMingDivinationStartChou();
	void SendRATianMingDivinationStartChouResult(int reward_index);

	RATianMingDivinationParam m_param;
};

#endif	//__ROLE_ACTIVITY_TIAN_MING_DIVINATION_HPP__
