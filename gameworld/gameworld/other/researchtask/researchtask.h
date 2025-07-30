#ifndef __RESEARCH_TASK_H__
#define __RESEARCH_TASK_H__

#include "servercommon/researchtaskdef.h"

#include <map>

class RoleModuleManager;
class ResearchTask
{
public:
	ResearchTask();
	~ResearchTask();

	void* operator new(size_t c);
	void operator delete(void* m);

	void Init(RoleModuleManager * mgr, const ResearchTaskParam & param);
	void GetInitParam(ResearchTaskParam * param);

	void Update(time_t now_second);
	void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);
	void OnFindDataReset();
	void OnResetModuleData(unsigned int old_dayid, unsigned int now_dayid);
	void OnResearchTaskRoleLogin();

	int GetYesterdayUnfinishTimes() { return m_param.residual_reward_back_times; }
	bool OnResearchTaskFindReward(int consume_type, int count);
	bool IsHaveResearchTask() { return 0 != m_param.task_info.task_id; }

	void OnRoleEnterTeam();					// 加入队伍：玩家加入队伍后，如果队长有真中任务，则同步任务
	void OnRoleRejoinTeam();
	void OnTeamLeaderChange();				// TODO 当leader不同了 要不要同步任务？
	void OnRoleLeaveTeam();					// 离开队伍：玩家离开队伍后，直接取消任务

	void OnGainTaskInfo(ResearchTaskInfo * task_con);			// 获取任务信息
	void OnSyncTaskInfo(const ResearchTaskInfo & task_con);		// 同步任务信息

	void OnAcceptResearchTask();
	void OnCommitResearchTask();

	void OnOneKeyFinishZhenZhong();				// 一键完成
	void OnDefeatMonsterGroup();		// 击败真中怪
	void TeamLeaderSyncTaskAfterBattle();
	int GetTodayFinishCount(){ return m_param.today_finish_times; }			// 获取今天完成的次数
	bool HasFinishAllToday() const;
	void OnEnterScene(int scene_id);
	void SendResearchTaskInfo(bool is_new = false);

	bool IsCanOneClick();	// 是否可以一键完成
	void OneClickFinishTask(unsigned int& exp, int& coin, int& jade, std::vector<ItemConfigData>& rewards);

	void GmResearchTaskHandle(int operator_type, int param);

private:
	void CheckCanGainAward(bool _is_onekey_finish = false);		// 检测任务奖励
	bool RandTaskInfoOnce(int team_average_level, ResearchTaskInfo * out_con);	// 获取一次任务进程

	RoleModuleManager * m_mgr;
	ResearchTaskParam m_param;
};


#endif // !__SCHOOL_TASK_H__
