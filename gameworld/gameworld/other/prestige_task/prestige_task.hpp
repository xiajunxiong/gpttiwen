#ifndef __PRESTIGE_TASK_HPP__
#define __PRESTIGE_TASK_HPP__

#include "servercommon/prestigetaskdef.hpp"

class RoleModuleManager;
class PrestigeTask
{
public:
	PrestigeTask();
	~PrestigeTask();

	void* operator new(size_t c);
	void operator delete(void* m);

	void Init(RoleModuleManager * mgr, const PrestigeTaskParam& param);
	void GetInitParam(PrestigeTaskParam* param);
	void OnDayChange();

	void OnSelectVillageReq(int village_id);
	void OnReq(void * msg);
	void OnRandTaskStar();
	void OnTaskAccept();
	void OnTaskCommit(int task_id);
	inline int GetCurTaskID() const { return m_param.cur_task_id; }

	bool IsCanOneClick();	// 是否可以一键完成
	void OneClickFinishTask(unsigned int& exp, int& coin, int& jade, std::vector<ItemConfigData>& rewards);

	void SendAllInfo();
private:
	void IssuePrestigeTaskReward(int task_id);
	void IssuePrestigeTaskEndReward(int village_id);

	void SendPrestigeTaskInfo();
	bool AcceptPrestigeTask(int task_id);
	void ClearTask(int task_id);
	void SendLastNotice();

	RoleModuleManager* m_mgr;
	PrestigeTaskParam m_param;
};


#endif