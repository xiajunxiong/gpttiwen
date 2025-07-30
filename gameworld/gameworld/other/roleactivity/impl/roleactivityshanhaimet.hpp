#ifndef __ROLE_ACTIVITY_SHAN_HAI_MET_HPP__
#define __ROLE_ACTIVITY_SHAN_HAI_MET_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/rashanhaimetparam.hpp"
#include "servercommon/rolecommonsavedef.h"

class RoleActivityShanHaiMet : public RoleActivity
{
public:
	static const int BUY_TYPE = BUY_TIME_TYPE_SHAN_HAI_MET;
	static const int COMMON_SAVE_TYPE = ROLE_COMMON_SAVE_DATA_TYPE_SHAN_HAI_MET;


	RoleActivityShanHaiMet(int type);
	virtual ~RoleActivityShanHaiMet();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);
	virtual void OnLoginSendInfo();
	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);

	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);
	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);

	//直购
	int OnBuyItemCheck(long long ARG_OUT & unique_key, int cfg_ver);
	int OnCmdBuyItem(int chong_zhi_gold, long long unique_id);
	int OnBuyGiftItem(ARG_OUT int * price);

	//任务记录
	void AddTaskParam(int task_type, int task_param);


	//基础信息
	void SendBaseInfo();
	//任务信息
	void SendTaskParam();
	//任务参数改变
	void SendTaskParamChange(int task_type, int task_param);
public:
	//奖励发放
	//通常奖励
	void FetchNormal(const int fetch_index);
	//未完成奖励
	void FetchUnfinish(const int fetch_index);
	//额外奖励
	void FetchExtra(const int fetch_index);

	void CheckToDayTaskFinish(const int now_act_open_day);

	RAShanHaiMetParam m_param;
};

#endif
