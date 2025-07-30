#ifndef __ROLE_ACTIVITY_SERVER_COMPETITION_HPP__
#define __ROLE_ACTIVITY_SERVER_COMPETITION_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/raservercompetitionparam.hpp"
#include "protocol/randactivity/msgraservercompetition.hpp"

class RoleActivityServerCompetition : public RoleActivity
{
public:
	RoleActivityServerCompetition(int type);
	virtual ~RoleActivityServerCompetition();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);
	virtual void Update(time_t now_second);

	virtual void OnLoginSendInfo();
	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);
	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);

	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);

	void OnRankValueChange(int rank_type, int rank_value);

	void SendAllRankInfo();

public:
	void SendAllInfo();
	void SendSingleInfo(int rank_type);
	void SendOtherInfo();
	void OnFetchReward(int rank_type, int seq);
	void OnFetchDayGift();
	void OnCheckTime();

	int GetCurRankValueByType(int rank_type);

	RARoleServerCompetitionParam m_param;
};


#endif