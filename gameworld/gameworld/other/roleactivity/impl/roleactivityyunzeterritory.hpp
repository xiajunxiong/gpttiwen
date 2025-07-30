#ifndef __ROLE_ACTIVITY_YUN_ZE_TERRITORY_HPP__
#define __ROLE_ACTIVITY_YUN_ZE_TERRITORY_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/rayunzeterritoryparam.hpp"
#include "protocol/randactivity/msgrayunzeterritory.hpp"

class RoleActivityYunZeTerritory : public RoleActivity
{
public:
	RoleActivityYunZeTerritory(int type);
	virtual ~RoleActivityYunZeTerritory();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);

	virtual void OnLoginSendInfo();
	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);
	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);
	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);

private:
	void SendInfo();
	void Explore(int picture_piece);
	void SendReward();

	RAYunZeTerritoryParam m_param;
};

#endif