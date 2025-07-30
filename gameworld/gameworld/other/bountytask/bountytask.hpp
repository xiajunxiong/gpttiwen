#ifndef __BOUNTY_TASK_HPP__
#define __BOUNTY_TASK_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/bountytaskdef.hpp"
#include "servercommon/struct/moneyotherdef.h"
#include "servercommon/activesystemdef.hpp"
#include "gamedef.h"

class RoleModuleManager;
class BountyTask
{
public:
	BountyTask();
	~BountyTask();

	void* operator new(size_t c);
	void operator delete(void* m);

	void Init(RoleModuleManager* mgr, const BountyTaskParam& param);
	void GetInitParam(ARG_OUT BountyTaskParam* out_param);

	void OnTurnTableRewardFetchReq();
	void GMFastFinishTask();
	void OnLevelUp();
	void OnRoleFirstAdavanceCareer();
	void OnRoleChangeProf();		//转职
	void OnResetModuleData(unsigned int old_dayid, unsigned int now_dayid);
	void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);
	void OnTaskCommit(int task_id, bool _is_onekey_finish = false);
	void OnAddDayExp(int task_id, int add_exp);

	void OneKeyFinishBountyTask();				
	void GetReward(bool _is_onekey_finish = false);						//获取奖励

	bool OnBountyTaskFindReward(int consume_type, int count);
	int GetTodayFinish() { return m_param.cur_finish_task_group_num; }
	bool HasFinishTodayTask() { return this->GetTodayFinish() >= this->GetTaskCount(); }
	int GetTaskCount();			//一天任务量
	void SendInfo();
	void SendTaskInfo();

	void OnRoleLogin();

	bool IsCanOneClick();	// 是否可以一键完成
	void OneClickFinishTask(unsigned int& exp, int& coin, int& jade, std::vector<ItemConfigData>& rewards);
private:
	void RefreshTask(const char* caller);
	void RefreshTaskSkipFirstTask(const char* caller);
	void RefreshFindCount();
	void RemoveYestodayTask();
	void ResetTurnTable();
	void AcceptBountyTask(TaskID task_id, bool _is_onekey_finish = false);
	void FillItemVec(std::vector<ItemConfigData> & item_list);
	void DepositItem(int item_num, const ItemConfigData * item_list);
	void GetReward(TaskID task_id, unsigned int& exp, int& coin, int& jade, std::vector<ItemConfigData>& rewards);

	RoleModuleManager* m_mgr;
	BountyTaskParam m_param;

	TaskID m_task_id;

};

#endif