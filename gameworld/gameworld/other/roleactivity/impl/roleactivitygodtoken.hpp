#ifndef __ROLE_ACTIVITY_GOD_TOKEN_HPP__
#define __ROLE_ACTIVITY_GOD_TOKEN_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/ragodtokenparam.hpp"

struct GodTokenItemData
{
	GodTokenItemData() : is_bind_num(0), no_bind_num(0) {}

	int is_bind_num;
	int no_bind_num;
};

class RoleActivityGodToken : public RoleActivity
{
public:
	RoleActivityGodToken(int type);
	virtual ~RoleActivityGodToken();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);

	virtual void OnLogin();
	virtual void OnLoginSendInfo();
	virtual void Update(time_t now_second);
	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);
	virtual void OnWeekChange();
	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);

	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);
	virtual void OnRAOpenOnlineSend();
	virtual void OnRoleLevelUp(int old_level, int to_level);

	void OnRAGodTokenAddCount(int type, int count = 1, int param = 0);			//每日任务
	void OnRAGodTokenWeekAddCount(int type, int count = 1, int param = 0);		//每周任务

	bool OnRAGodTokenBuyToken(bool is_gm = false);												//购买战令
	int OnBuyGiftItem(ARG_OUT int * price);

	void OnRAGodTokenCaclTimeSeq();
	void CheckRoleGodTokenIsOpen();

	bool IsRoleraGodTokenOpen() { return m_param.ra_god_token_is_open == 1 ? true : false; }
	void OnReSetRAGodToken(bool is_gm = false);

	void OnRAGodTokenPatchLevelReward();					//赛季结束或活动关闭补发没领取的等级奖励以及溢出奖励（在回收物品之前）
	void OnRAGodTokenReclaimItem();							//赛季结束或活动关闭回收物品（补发奖励之后）
private:
	void SendRAGodTokenInfo();
	void OnTimeSeqChange();

	//任务
	bool IsRAGodTokenTaskType(int task_type, int type);
	void OnRAGodTokenRefreshTask(int task_type);

	//等级奖励
	void OnRAGodTokenFetchCommonReward(int seq);
	void OnRAGodTokenFetchSeniorReward(int seq);

	void OnRAGodTokenExchange(int seq);

	//经验
	void OnRAGodTokenBuyExp(int count);
	bool OnRAGodTokenAddExp(int add_exp);

	//溢出奖励
	void OnRAGodTokenFetchSurplusReward();
	void OnRAGodTokenPatchSurplusSeniorReward();			//邮件补发溢出奖励的高级奖励

	//任务奖励
	void OnRAGodTokenFetchDailyReward(int seq); //(废弃)
	void OnRAGodTokenFetchWeeklyReward(int seq); //(废弃)
	void CheckDailyReward(int type);
	void CheckWeekReward(int type);

	void AddWeeklyTask();
	void ChangeWeelyTask();

	RAGodTokenParam m_param;
	unsigned int ra_god_token_cacl_time;

	int m_mail_reclaim_num;
};

#endif	//__ROLE_ACTIVITY_GOD_TOKEN_HPP__
