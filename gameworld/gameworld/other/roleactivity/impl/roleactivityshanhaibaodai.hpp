#ifndef __ROLE_ACTIVITY_SHAN_HAI_BAO_DAI_HPP__
#define __ROLE_ACTIVITY_SHAN_HAI_BAO_DAI_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/rashanhaibaodaiparam.hpp"
#include "servercommon/rolecommonsavedef.h"

class RoleActivityShanHaiBaoDai : public RoleActivity
{
public:
	static const int BUY_TYPE = BUY_TIME_TYPE_SHAN_HAI_BAO_DAI;
	static const int COMMON_SAVE_TYPE = ROLE_COMMON_SAVE_DATA_TYPE_SHAN_HAI_BAO_DAI;

	RoleActivityShanHaiBaoDai(int type);
	virtual ~RoleActivityShanHaiBaoDai();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);
	virtual void OnLoginSendInfo();
	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);
	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);
	virtual bool IsFinishBonusCond(int type = 0);

	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);

	int OnBuyGiftItem(int seq, ARG_OUT int * price);

	int OnBuyItemCheck(int seq, long long ARG_OUT & unique_key);
	int OnBuyItem(int chong_zhi_gold, long long unique_id);
public:
	void SendInfo();
	void ReceiveFreeReward();

	RAShanHaiBaoDaiParam m_param;
};

#endif	//__ROLE_ACTIVITY_SHAN_HAI_BAO_DAI_HPP__
