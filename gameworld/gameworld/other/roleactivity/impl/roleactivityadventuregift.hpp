#ifndef __ROLE_ACTIVITY_ADVENTURE_GIFT_HPP__
#define __ROLE_ACTIVITY_ADVENTURE_GIFT_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/raadventuregiftparam.hpp"

class RoleActivityAdventureGift : public RoleActivity
{
public:
	RoleActivityAdventureGift(int type);
	virtual ~RoleActivityAdventureGift();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);

	virtual void OnLoginSendInfo();
	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);
	virtual void OnRoleLevelUp(int old_level, int to_level);

	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);

	void OnPassCourageChallenge(int old_layer, int layer);
	void OnChangeCourseOfGrowthProg(int old_prog, int prog);
public:
	void RaAdventureGiftSendInfo();
	void RaAdventureGiftBuy();

	RAAdventureGiftParam m_param;
};

#endif	//__ROLE_ACTIVITY_ADVENTURE_GIFT_HPP__
