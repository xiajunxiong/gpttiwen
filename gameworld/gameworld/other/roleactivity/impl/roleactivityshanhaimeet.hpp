#ifndef __ROLE_ACTIVITY_SHAN_HAI_MEET_HPP__
#define __ROLE_ACTIVITY_SHAN_HAI_MEET_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/rashanhaimeetparam.hpp"
#include "servercommon/rolecommonsavedef.h"

class RoleActivityShanHaiMeet : public RoleActivity
{
public:

	static const int BUY_TYPE = BUY_TIME_TYPE_SHAN_HAI_MEET;
	static const int COMMON_SAVE_TYPE = ROLE_COMMON_SAVE_DATA_TYPE_SHAN_HAI_MEET;

	RoleActivityShanHaiMeet(int type);
	virtual ~RoleActivityShanHaiMeet();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);
	virtual void OnLogin();
	virtual void OnLoginSendInfo();
	virtual void Update(time_t now_second);
	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);
	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);

	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);

	int OnBuyItemCheck(long long ARG_OUT & unique_key);
	int OnCmdBuyItem(int chong_zhi_gold, long long unique_id);
	int OnBuyGiftItem(ARG_OUT int * price);

	void OnFetch(int seq);
	void OnPoker(int index);
	void OnResetPoker();
	void OnFinishType(int type, int count);
public:
	void SendAllInfo();

	RAShanHaiMeetParam m_param;
};

#endif	//__ROLE_ACTIVITY_SHAN_HAI_MEET_HPP__
