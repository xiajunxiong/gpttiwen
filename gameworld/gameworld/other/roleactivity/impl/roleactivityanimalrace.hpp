#ifndef __ROLE_ACTIVITY_ANIMAL_RACE_HPP__
#define __ROLE_ACTIVITY_ANIMAL_RACE_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/raanimalraceparam.hpp"

class RoleActivityAnimalRace : public RoleActivity
{
public:
	RoleActivityAnimalRace(int type);
	virtual ~RoleActivityAnimalRace();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);

	virtual void OnLoginSendInfo();
	virtual void Update(time_t now_second);
	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);

	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);

	void OnRAAnimalRaceEnterScene();
	void OnRAAnimalRaceExitScene();

	void OnRAAnimalRaceRoleComment(const char* msg, unsigned int msg_length);
public:
	void SendRAAnimalRaceInfo();
	void SendRAAnimalRaceActivityInfo();
	void SendRAAnimalRaceAnimationInfo();

	void OnRAAnimalRaceBet(int animal_race_type, int count);


	RAAnimalRaceParam m_param;
};

#endif	//__ROLE_ACTIVITY_ANIMAL_RACE_HPP__
