#ifndef __ROLE_ACTIVITY_ZAI_ZHAN_YUN_ZE_HPP__
#define __ROLE_ACTIVITY_ZAI_ZHAN_YUN_ZE_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/razaizhanyunzeparam.hpp"
#include "servercommon/rolecommonsavedef.h"

class RoleActivityZaiZhanYunZe : public RoleActivity
{
public:
	RoleActivityZaiZhanYunZe(int type);
	virtual ~RoleActivityZaiZhanYunZe();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);
	virtual void OnLoginSendInfo();

	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);
	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);

	//挑战
	void BossBattle(int boss_index);
	//通关
	void OnBarrleFinish(int boss_index);

	//基础信息
	void SendInfo();
public:

	RAZaiZhanYunZeParam m_param;
};

#endif