#ifndef __ROLE_ACTIVITY_EXPEL_MONSTER_HPP__
#define __ROLE_ACTIVITY_EXPEL_MONSTER_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/raexpelmonsterparam.hpp"

class RoleActivityExpelMonster : public RoleActivity
{
public:
	RoleActivityExpelMonster(int type);
	virtual ~RoleActivityExpelMonster();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);

	virtual void OnLoginSendInfo();
	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);

	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);

	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);

	void OnExpelMonster();
	int GetExpelRoleLevel() { return m_param.ra_expel_role_level; }
	int GetExpelMonsterNum() { return m_param.ra_expel_monster_num; }
public:
	void SendExpelMonsterInfo();
	void ExpelMonsterBuy(int seq);

	RAExpelMonsterParam m_param;
};

#endif	//__ROLE_ACTIVITY_EXPEL_MONSTER_HPP__
