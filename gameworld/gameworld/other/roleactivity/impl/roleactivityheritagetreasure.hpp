#ifndef __ROLE_ACTIVITY_HERITAGE_TREASURE_HPP__
#define __ROLE_ACTIVITY_HERITAGE_TREASURE_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/raheritagetreasureparam.hpp"

class RoleActivityHeritageTreasure : public RoleActivity
{
public:
	RoleActivityHeritageTreasure(int type);
	virtual ~RoleActivityHeritageTreasure();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);

	virtual void OnLoginSendInfo();
	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);

	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);
	virtual void OnRandActivityMakeUpReq(Protocol::CSRandActivityMakeUpReq * req);

public:
	void RandActivityHeritageTreasureActiveClues(int index);
	void RandActivityHeritageTreasureFetchReward(int index);
	void RandActivityHeritageTreasureFetchLastReward();
	void RandActivityHeritageTreasureSendInfo();

	RAHeritageTreasureParam m_param;
};

#endif	//__ROLE_ACTIVITY_HERITAGE_TREASURE_HPP__
