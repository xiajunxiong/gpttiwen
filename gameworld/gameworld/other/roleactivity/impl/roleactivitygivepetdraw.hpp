#ifndef __ROLE_ACTIVITY_GIVE_PET_DRAW_HPP__
#define __ROLE_ACTIVITY_GIVE_PET_DRAW_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/ragivepetdrawparam.hpp"

class RoleActivityGivePetDraw : public RoleActivity
{
public:
	RoleActivityGivePetDraw(int type);
	virtual ~RoleActivityGivePetDraw();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);

	virtual void OnLoginSendInfo();
	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);

	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);

public:
	void SendInfo();
	void OnReveive();

	RAGivePetDrawParam m_param;
};

#endif	//__ROLE_ACTIVITY_LUCKY_BAG_HPP__

