#ifndef __SCHOOL_TASK_H__
#define __SCHOOL_TASK_H__

#pragma once
#include "servercommon/schooltaskdef.h"
#include "servercommon/struct/moneyotherdef.h"
#include <map>

class RoleModuleManager;
class SchoolTask
{
public:
	SchoolTask();
	~SchoolTask();

	void Init(RoleModuleManager * mgr, const SchoolTaskParam& param);
	void GetInitParam(SchoolTaskParam* param);

	void OnResetModuleData(unsigned int old_dayid, unsigned int now_dayid);
	void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);
	void OnFindDataReset();
	bool OnKillMonster(int monster_id);

	void OnRoleEnterTeam();
	void OnTeamLeaderChange();

	void OnTaskReq(int index);
	void OnQuickFinish(int index);
	void OnLevelUp(int cur_level);
	bool OnSchoolTaskFindReward(int consume_type, int count);

	/************************************************************************/
	/*    MONEY_TYPE_GOLD = 0    MONEY_TYPE_COIN_BIND = 2                   */
	/************************************************************************/
	//void OneKeyFinish(int _sonsume_money,int _money_type = MONEY_TYPE_GOLD);

	void SetIndexByMonster();
	void RefreshTaskList(int level);

	void SendAllInfo();
	void SendSingleInfo(int task_index, bool is_new = false);
	int GetFindRewardCount() { return m_param.reward_find_count; }
	int GetFinishCount();

	void* operator new(size_t c);
	void operator delete(void* m);

	void GmSchoolTaskOrder(int op_type);
	bool IsInTaskScene();
private:
	void FinishTask(int index);
	bool CheckIsAllFinish();
	void SyncTeamLeaderTask(int index);
	void OnAcceptTask(int index);
	int GetTaskStatus(int index);

	RoleModuleManager * m_role_module_mgr;
	SchoolTaskParam m_param;
	std::map<int, int> m_index_by_monster;
};


#endif // !__SCHOOL_TASK_H__
