#ifndef __ROLE_ACTIVITY_COLORFUL_DOUBLE_HPP__
#define __ROLE_ACTIVITY_COLORFUL_DOUBLE_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/racolorfuldoubleparam.hpp"
#include "servercommon/rolecommonsavedef.h"


class RoleActivityColorfulDouble : public RoleActivity
{
public:
	static const int BUY_TYPE = BUY_TIME_TYPE_COLORFUL_DOUBLE;
	static const int COMMON_SAVE_TYPE = ROLE_COMMON_SAVE_DATA_TYPE_COLORFUL_DOUBLE;


	RoleActivityColorfulDouble(int type);
	virtual ~RoleActivityColorfulDouble();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);
	virtual void OnLoginSendInfo();
	virtual void OnLoginCheck();

	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);

	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);
	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);

	virtual void OnRAClose();

	//直购
	int OnBuyItemCheck(long long ARG_OUT & unique_key, int cfg_ver, int box_seq);
	int OnCmdBuyItem(int chong_zhi_gold, long long unique_id);
	int OnBuyGiftItem(int box_seq, int ARG_OUT * price);

	//任务记录
	void AddTaskParam(int task_type, int task_param, unsigned int now_dayid = 0);

	void CheckGive(unsigned int act_begin_itme);

	//基础信息
	void SendBaseInfo();
	//任务信息
	void SendTaskParam();
	//任务参数改变
	void SendTaskParamChange(int task_type, int task_param);
public:
	//激活宝箱操作
	void OnActBoxReq(int cfg_ver,int box_seq);
	//激活宝箱奖励s
	int ActBoxReward(int box_seq);
	//领取宝箱奖励
	void FetchBoxReaard(int box_seq);

	//领取免费奖励
	void FetchFreeReward();

	//检测任务完成情况
	void ChackTaskFinish(int task_type);

	RAColorfulDoubleParam m_param;
};



#endif