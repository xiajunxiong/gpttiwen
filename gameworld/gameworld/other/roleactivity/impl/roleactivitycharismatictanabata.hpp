#ifndef __ROLE_ACTIVITY_CHARISMATIC_TANABATA_HPP__
#define __ROLE_ACTIVITY_CHARISMATIC_TANABATA_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/racharismatictanabataparam.hpp"

class RoleActivityCharismaticTanabata : public RoleActivity
{
public:
	RoleActivityCharismaticTanabata(int type);
	virtual ~RoleActivityCharismaticTanabata();

	void * operator new(size_t c);
	void operator delete(void * m);

	virtual void Init(RoleModuleManager * mgr, const RoleActivityRoleData & p);
	virtual void GetInitParam(RoleActivityRoleData * p);
	virtual void OnLoginSendInfo();
	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);
	virtual void OnRAClose();

	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq * raor);

	void GetReward(int reward_type, int seq);

	int GetMeiLiValue() { return m_param.charm_value; }
	void OnRoleSendFlows(int add_value);
	void OnSendRoleFlasw(int add_value);
private:
	void SendInfo();

	RACharismaticTanabataParam m_param;
};

#endif	

