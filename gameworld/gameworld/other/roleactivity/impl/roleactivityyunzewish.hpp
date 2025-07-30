#ifndef __ROLE_ACTIVITY_YUN_ZE_WISH_HPP__
#define __ROLE_ACTIVITY_YUN_ZE_WISH_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/rayunzewishparam.hpp"
#include "protocol/randactivity/msgrayunzewish.hpp"

class RoleActivityYunZeWish : public RoleActivity
{
public:
	RoleActivityYunZeWish(int type);
	virtual ~RoleActivityYunZeWish();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);

	virtual void Update(time_t now_second);
 	virtual void OnLoginSendInfo();
 	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);
 	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);

private:
	void RASendInfo();
	void LockReward(int reward_seq);
	void UnlockReward(int reward_seq);  // ÔÝÊ±ÆúÓÃ
	void Lottery();

	RAYunZeWishParam m_param;
};


#endif