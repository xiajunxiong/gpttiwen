#ifndef __ROLE_ACTIVITY_CHAOZHI_FENXIANG_HPP__
#define __ROLE_ACTIVITY_CHAOZHI_FENXIANG_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/rachaozhifenxiangparam.hpp"
#include "servercommon/rolecommonsavedef.h"

class RoleActivityChaoZhiFenXiang : public RoleActivity
{
public:
	static const int BUY_TYPE = BUY_TYME_TYPE_CHAOZHI_FENXIANG;
	static const int COMMON_SAVE_TYPE = ROLE_COMMON_SAVE_DATA_TYPE_CHAOZHI_FENXIANG;

	RoleActivityChaoZhiFenXiang(int type);
	virtual ~RoleActivityChaoZhiFenXiang();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);

	virtual void OnLoginSendInfo();
	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);

	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);
	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);
	virtual void OnRAClose();

	int OnBuyGiftItem(ARG_OUT int * price);

	int OnBuyItemCheck(long long ARG_OUT & unique_key);
	int OnCmdBuyItem(int chong_zhi_gold, long long unique_id);
	
	void AddTaskComplete(int task_type, int add_times = 1);

public:
	void RASendInfo();
	void OnFetchReward(int task_seq);
	void OnBuy();
	void OnFetchZhiGouReward();
	void OnRewardPatch();

	RAChaoZhiFenXiangParam m_param;
};

#endif	//__ROLE_ACTIVITY_CHAOZHI_FENXIANG_HPP__
