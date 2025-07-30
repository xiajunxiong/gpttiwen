
#ifndef TASKRECORDER_H
#define TASKRECORDER_H

#include <map>
#include <set>

#include "gamedef.h"
#include "servercommon/taskdef.h"

/*
	记录角色做过的任务记录，分别记录任务做过的次数以及今天做过的次数
*/

class RoleModuleManager;

class TaskRecorder
{
public:
	TaskRecorder();
	~TaskRecorder();

	void* operator new(size_t c);
	void operator delete(void* m);

	bool Init(RoleModuleManager * role_module_mgr, int buff_len, const char *record_buff,const TaskChosedParam& param);
	void GetInitParam(char *record_buff, int *record_buff_len, TaskChosedParam * param);

	void OnTaskDayChange(unsigned int old_dayid, unsigned int now_dayid);
	void OnTaskWeekChange();

	void TaskComplete(int task_id);		// 任务完成，写进任务完成记录里
	bool HaveFinished(int task_id);		// 检测任务是否已完成
	void TaskRecordRemove(int task_id);	// 任务记录删除

	void OnRoleLogin();		// 上线时下发一次已完成的任务信息
	void ClearAppoiontedTypeTaskRecord(int clear_task_type);

	void SetTaskChosed(unsigned short task_index, unsigned short param);
	void SendTaskChosedInfo();
	inline TaskID GetMaxFinishedMainTaskID() const { return m_max_finished_main_task_id; }

	void ClearPetFeiShengRecord();
	void ClearRoleGuildTaskRecord();
protected:
	void SendTaskCompleteList();
	void UpdateMaxFinishedMainTask(TaskID task_id);

	RoleModuleManager * m_role_module_mgr;
	TaskChosedParam m_chosed_param;
	std::set<TaskID> m_record_set;
	TaskID m_max_finished_main_task_id;

	
};

#endif
