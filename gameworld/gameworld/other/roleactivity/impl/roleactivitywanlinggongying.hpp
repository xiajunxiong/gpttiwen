#ifndef __ROLE_ACTIVITY_WAN_LING_GONG_YING_HPP__
#define __ROLE_ACTIVITY_WAN_LING_GONG_YING_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/rawanlinggongyingparam.hpp"
#include "servercommon/rolecommonsavedef.h"

class RoleActivityWanLingGongYing : public RoleActivity
{
public:
	static const int BUY_TYPE = BUY_TIME_TYPE_WAN_LING_GONG_YING;
	static const int COMMON_SAVE_TYPE = ROLE_COMMON_SAVE_DATA_TYPE_WAN_LING_GONG_YING;


	RoleActivityWanLingGongYing(int type);
	virtual ~RoleActivityWanLingGongYing();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);

	virtual void OnLoginSendInfo();
	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);
	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);
	virtual void OnRAClose();

	void OnCompleteTask(int activity_type, int time = 1);

	int OnBuyGiftItem(ARG_OUT int * price);

	int OnBuyItemCheck(long long ARG_OUT & unique_key);
	int OnCmdBuyItem(int chong_zhi_gold, long long unique_id);

public:
	void RASendInfo();
	void OnFetchReward(int task_index);

	RAWanLingGongYingParam m_param;
};

#endif	//__ROLE_ACTIVITY_WAN_LING_GONG_YING_HPP__
