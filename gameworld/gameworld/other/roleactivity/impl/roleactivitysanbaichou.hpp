#ifndef __ROLE_ACTIVITY_SANBAI_CHOU_HPP__
#define __ROLE_ACTIVITY_SANBAI_CHOU_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/rasanbaichouparam.hpp"

class RoleActivitySanBaiChou : public RoleActivity
{
public:
	RoleActivitySanBaiChou(int type);
	virtual ~RoleActivitySanBaiChou();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);

	virtual void OnLoginSendInfo();
	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);
	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);
	virtual void OnRAClose();
	virtual void OnLoginCheck();

public:
	void RASendAllInfo();
	void OnDraw(int type);
	void OnSetSelect(int type, int index);
	void OnFetchReward(int type);
	void RASendInfo(int type);

	void OnRewardBuFa();

	RASanBaiChouParam m_param;
};

#endif	//__ROLE_ACTIVITY_WAN_LING_GONG_YING_HPP__
