#ifndef __ROLE_ACTIVITY_TRUN_TABLE_HPP__
#define __ROLE_ACTIVITY_TRUN_TABLE_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/ratruntableparam.hpp"

class RoleActivityTrunTable : public RoleActivity
{
public:
	RoleActivityTrunTable(int type);
	virtual ~RoleActivityTrunTable();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);

	virtual void OnLoginSendInfo();

	virtual void Update(time_t now_second);

	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);

	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);

	void OnRaTurnTableAddKillBossNum();
	void GMAddTimes(int free_num, int kill_boss_num);

public:
	void SendRATurnTableInfo();
	void OnRAStartTurnTable(int turn_type);
	void OnRAGetTurnTableReward(int index);

	RATrunTableParam m_param;
};

#endif	//__ROLE_ACTIVITY_TRUN_TABLE_HPP__
