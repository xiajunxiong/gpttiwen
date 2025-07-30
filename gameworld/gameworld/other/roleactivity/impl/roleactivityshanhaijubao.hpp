#ifndef __ROLE_ACTIVITY_SHAN_HAI_JU_BAO_HPP__
#define __ROLE_ACTIVITY_SHAN_HAI_JU_BAO_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/rashanhaijubaoparam.hpp"

class RoleActivityShanHaiJuBao : public RoleActivity
{
public:
	RoleActivityShanHaiJuBao(int type);
	virtual ~RoleActivityShanHaiJuBao();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);
	virtual void OnLoginSendInfo();

	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);
	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);

	//≥ÈΩ±
	void Draw(int phase_seq);
	//¡Ï»°∂ÓÕ‚Ω±¿¯
	void FetchExtraReward(int phase_seq, int extra_reward_index);

	void SendRoleInfo();

public:
	RAShanHaiJuBaoParam m_param;
};

#endif