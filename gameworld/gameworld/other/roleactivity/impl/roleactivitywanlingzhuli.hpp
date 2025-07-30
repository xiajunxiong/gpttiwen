#ifndef __ROLE_ACTIVITY_WANLING_ZHULI_HPP__
#define __ROLE_ACTIVITY_WANLING_ZHULI_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/rawanlingzhuliparam.hpp"
#include "servercommon/rolecommonsavedef.h"

class RoleActivityWanLingZhuLi : public RoleActivity
{
public:
	static const int BUY_TYPE = BUY_TIME_TYPE_WAN_LING_ZHU_LI;
	static const int COMMON_SAVE_TYPE = ROLE_COMMON_SAVE_DATA_TYPE_WAN_LING_ZHU_LI;

	RoleActivityWanLingZhuLi(int type);
	virtual ~RoleActivityWanLingZhuLi();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);

	virtual void OnLoginSendInfo();
	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);

	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);
	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);
	virtual void OnRAClose();
	virtual void OnLoginCheck();

	int OnBuyItemCheck(long long ARG_OUT & unique_key);
	int OnCmdBuyItem(int chong_zhi_gold, long long unique_id);

	int OnBuyGiftItem(ARG_OUT int * price);
public:
	void RASendInfo();
	void OnFetchDayReward();
	void OnFetchReward(int day_index, bool is_zhongji);
	void OnRewardBuFa();

	RAWanLingZhuLiParam m_param;
};

#endif	//__ROLE_ACTIVITY_WANLING_ZHULI_HPP__
