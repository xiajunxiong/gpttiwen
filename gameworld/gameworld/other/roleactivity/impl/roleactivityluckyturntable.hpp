#ifndef __ROLE_ACTIVITY_LUCKY_TURN_TABLE_HPP__
#define __ROLE_ACTIVITY_LUCKY_TURN_TABLE_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/raluckyturntableparam.hpp"
#include "protocol/randactivity/msgraluckyturntable.hpp"

class RoleActivityLuckyTurnTable: public RoleActivity
{
public:
	RoleActivityLuckyTurnTable(int type);
	virtual ~RoleActivityLuckyTurnTable();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);

	virtual void OnLoginSendInfo();
	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);
	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);
	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);

	void OnRALuckyTurntableFinishType(int type, int num);
	bool OnUseLuckLotteryTicket(int num);

private:
	void SendRALuckyTurntableRewardInfo(int type = Protocol::SCRALuckyTurntableReward::LUCKY_TURN_TABLE_TYPE_START);
	void SendRALuckyTurntableInfo();

	void OnRAStartLuckyTurntable();
	void OnRAGetLuckyTurntableReward(int seq);
	void OnRAStartLuckyTurntableSkip();

	RALuckyTurnTableParam m_param;
};

#endif	//__ROLE_ACTIVITY_LUCKY_TURN_TABLE_HPP__
