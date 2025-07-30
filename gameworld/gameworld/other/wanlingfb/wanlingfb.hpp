#ifndef __WAN_LING_FB_HPP__
#define __WAN_LING_FB_HPP__

#include "servercommon/wanlingfbdef.hpp"
class RoleModuleManager;
class RoleWanLingFB
{
public:
	RoleWanLingFB();
	~RoleWanLingFB();

	void* operator new(size_t c);
	void operator delete(void* m);

	void Init(RoleModuleManager *module_mgr, const WanLingFBParam & param);
	void GetInitParam(WanLingFBParam * param);

	void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);

	//角色操作
	void OnReq(int req_type, int param_1, int param_2);

	//可进入副本
	bool CanEnterFB(int fb_index);

	//通关结算
	void FBPass(int fb_index);
	int GetFBPassRewardTimes() { return m_param.day_fetch_pass_reward; }
	//战斗结算
	void FBBattlePass(int fb_index);
	bool HasRewardTimes() const;

	bool IsCanOneClick();	// 是否可以一键完成
	bool OneClickFinishTask(unsigned int& exp, int& coin, int& jade, std::vector<ItemConfigData>& rewards);

	void SendInfo();
private:
	void SetFBFetchFlag(int fb_index, int set_type);

	//场景操作
	//完成任务
	void FinishTask();
	//进行任务战斗
	void TaskFight();

	RoleModuleManager * m_module_mgr;

	WanLingFBParam m_param;
};

#endif