#ifndef __ROLE_ACTIVITY_HU_XING_DA_YUN_HPP__
#define __ROLE_ACTIVITY_HU_XING_DA_YUN_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/rayinhuactivityparam.hpp"
#include "servercommon/rolecommonsavedef.h"

class RoleActivityHuXingDaYun : public RoleActivity
{
public:
	RoleActivityHuXingDaYun(int type);
	virtual ~RoleActivityHuXingDaYun();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);

	virtual void OnLoginSendInfo();
	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);
	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);

	virtual void Update(time_t now_second);
	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);

	bool IsStartTask();
	unsigned int OnStartTask();
	int GetTaskPosId() { return m_param.task_pos_id; }

	bool IsBeInvited();
	int GetBeInvitedTimes() { return m_param.be_invited_times; }

	void YaoQingTongXing(int uid);

public:
	void RASendInfo();
	void RASendListInfo();
	void RAYaoQing(int invite_uid);
	void RAAcceptYaoQing(int uid);
	void RAFetchReward();
	void RAFetchInvuteReward();

	RAHuXingDaYunParam m_param;
};

#endif	//__ROLE_ACTIVITY_HU_XING_DA_YUN_HPP__
