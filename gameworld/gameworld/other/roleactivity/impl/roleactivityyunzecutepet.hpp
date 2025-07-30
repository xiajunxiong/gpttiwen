#ifndef __ROLE_ACTIVITY_YUN_ZE_CUTE_PET_HPP__
#define __ROLE_ACTIVITY_YUN_ZE_CUTE_PET_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/rayunzecutepetparam.hpp"
#include "protocol/randactivity/msgrayunzecutepet.hpp"

class RoleActivityYunZeCutePet : public RoleActivity
{
public:
	RoleActivityYunZeCutePet(int type);
	virtual ~RoleActivityYunZeCutePet();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);

	virtual void Update(time_t now_second);
 	virtual void OnLoginSendInfo();
  	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);
 	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);
  	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);

	//ÈÎÎñ¼ÇÂ¼
	void AddTaskParam(int task_type, int task_param = 1);
	void EventCutePet();

private:
 	void RASendInfo();
	void Fighting(int monster_id);
	void CountTask(int task_type);

	

	RAYunZeCutePetParam m_param;
};






#endif