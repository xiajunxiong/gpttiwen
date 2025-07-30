#ifndef __ROLE_ACTIVITY_CINIU_YINGXINHU_HPP__
#define __ROLE_ACTIVITY_CINIU_YINGXINHU_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/rayinhuactivityparam.hpp"
#include "servercommon/rolecommonsavedef.h"

class RoleActivityCiNiuYingXinHu : public RoleActivity
{
public:
	RoleActivityCiNiuYingXinHu(int type);
	virtual ~RoleActivityCiNiuYingXinHu();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);

	virtual void OnLoginSendInfo();
	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);
	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);

public:
	void RASendInfo();
	void RAAddTigerAir(int type);
	void RAFetchReward(int type, int index);
	void CheckAwaken(int type);
	void RAAwaken(int type);
	void RADrawAwakenReward();

	RACiNiuYingXinHuParam m_param;
};

#endif	//__ROLE_ACTIVITY_YINHU_WENSHI_HPP__
