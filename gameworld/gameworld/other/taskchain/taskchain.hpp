#ifndef __TASK_CHAIN_HPP__
#define __TASK_CHAIN_HPP__
#pragma once

#include "servercommon/taskchaindef.h"

class RoleModuleManager;
class TaskChain
{
public:
	TaskChain();
	~TaskChain();

	void* operator new(size_t c);
	void operator delete(void* m);

	void Init(RoleModuleManager * mgr, const TaskChainParam& param);
	void GetInitParam(TaskChainParam* param);

	void OnLogin();
	void OnRoleLevelUp();

	void TaskChainUpdate(time_t now);
	void RefreshData();
	void OnEnterScene(int scene_id);

	void OnTalkToNpc();
	void OnKillMonster();
	void OnDelivery(int column, int index);
	void OnKillMineMonster();
	void OnSkipFight();
	void OnCommitPet(int pet_index);
	void OnGetItemByWait();//从等待NPC手上获得对应任务道具

	void OnAcceptTask();
	void OnCommitTask(bool is_double = false);
	void OnGiveupTask();
	void OnGiveTurnTableReward();

	int GetMonsterGroupId();

	void SendTaskInfo(bool is_new = false);
	void SendTurnTableInfo();

	void GmTaskChainOrder(int op_type, int param);
	void ClearTemporaryPreTask();

	void GMTaskChainAccept(int task_type);
	void GMRefreshTaskChain();
private:
	bool RandTask(bool is_notice = true);
	void SendTaskChainMonsterInfo();
	void ResetTurnTable();
	void RandTurnTable(bool is_notice = true);

	RoleModuleManager * m_role_module_mgr;
	TaskChainParam m_param;

	//临时存储上一次环任务信息
	int			task_type;
	int			task_param[TASK_CHAIN_PARAM_COUNT];
};

#endif // !__TASK_CHAIN_HPP__
