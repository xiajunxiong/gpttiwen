#ifndef __ROLE_ACTIVITY_YUAN_QI_JISHIBU_HPP__
#define __ROLE_ACTIVITY_YUAN_QI_JISHIBU_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/rayuanqijishibuparam.hpp"
#include "servercommon/rolecommonsavedef.h"

class RoleActivityYuanQiJiShiBu : public RoleActivity
{
public:
	static const int BUY_TYPE = BUY_TIME_TYPE_YUAN_QI_JISHIBU;
	static const int COMMON_SAVE_TYPE = ROLE_COMMON_SAVE_DATA_TYPE_YUAN_QI_JISHIBU;

	RoleActivityYuanQiJiShiBu(int type);
	virtual ~RoleActivityYuanQiJiShiBu();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);

	virtual void OnLoginSendInfo();
	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);
	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);

	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);
	virtual void OnRAClose();
	virtual void OnLoginCheck();

	void OnCompleteTask(int task_type, int add_param = 1, bool is_reward = true);

	int OnBuyItemCheck(long long ARG_OUT & unique_key);
	int OnCmdBuyItem(int chong_zhi_gold, long long unique_id);

	int OnBuyGiftItem(ARG_OUT int * price);

public:
	void RASendInfo();
	void RAFetchReward(int index);
	void RAFetchJingJieReward(int index);
	void OnRewardBuFa();

	RAYuanQiJiShiBuParam m_param;
};

#endif	//__ROLE_ACTIVITY_YUAN_QI_JISHIBU_HPP__
