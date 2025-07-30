#ifndef __ROLE_ACTIVITY_PET_LOOT_HPP__
#define __ROLE_ACTIVITY_PET_LOOT_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/rapetlootparam.hpp"

class RoleActivityPetLoot : public RoleActivity
{
public:
	RoleActivityPetLoot(int type);
	virtual ~RoleActivityPetLoot();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);
	virtual void Update(time_t now_second);
	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);

	virtual void OnLoginSendInfo();
	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);

	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);
	
	void OnPetIntensi(int pet_type,int str_level);
public:
	void GetTaskReward(int seq);
	void GetFinalTaskReward(int seq);
	void SendInfo();

	RAPetLootParam m_param;
};

#endif	//__ROLE_ACTIVITY_PET_LOOT_HPP__

