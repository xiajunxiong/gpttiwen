#ifndef __ESCORT_TASK_H__
#define __ESCORT_TASK_H__
#pragma once
#include "servercommon/commondata.hpp"

class RoleModuleManager;
class Escort
{
public:
	Escort();
	~Escort();

	void Init(RoleModuleManager *role_module_manager);

	void OnResetModuleData(unsigned int old_dayid, unsigned int now_dayid);
	void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);
	void OnRoleLogin(long long last_save_time);

	void AcceptTask(int task_type, int task_id = 0);
	void CommitTask();
	void GiveupTask();
	void GiveUpAdvTask(int task_id);  //放弃奇遇运镖任务

	void OnRoleFinishFight(int is_win);
	bool HaveEscortTask();

	// 获取运镖的任务id 不为0时正在运镖
	int GetEscortSeq();
	int GetSpeedChangeRate();

	void SendAllInfo();
	void SendResultNotic(bool is_win);

	void* operator new(size_t c);
	void operator delete(void* m);
private:
	RoleModuleManager * m_role_module_mgr;

	void ResetEscortStatus();
	void SendEscortNoticeArea();
};

#endif // !__ESCORT_TASK_H__