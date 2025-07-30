#ifndef __ROLE_ACTIVITY_LOTUS_MI_BAO_HPP__
#define __ROLE_ACTIVITY_LOTUS_MI_BAO_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/racoolsummerparam.hpp"

class RoleActivityLotusMiBao : public RoleActivity
{
public:
	RoleActivityLotusMiBao(int type);
	virtual ~RoleActivityLotusMiBao();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);

	virtual void OnLoginSendInfo();
	/*virtual void OnRAOpenOnlineSend();*/
	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);
	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);

private:
	void SendInfo();
	void OnActiveGrid(int row, int column);
	void OnFetchAllActiveReward();
	void OnCheckCondFinish(int row, int column);
	bool IsFinishCondActive(int cond_type, int param1 = 0);

	RALotusMiBao m_param;
};

#endif