
#ifndef TASKRECORDER_H
#define TASKRECORDER_H

#include <map>
#include <set>

#include "gamedef.h"
#include "servercommon/taskdef.h"

/*
	��¼��ɫ�����������¼���ֱ��¼���������Ĵ����Լ����������Ĵ���
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

	void TaskComplete(int task_id);		// ������ɣ�д��������ɼ�¼��
	bool HaveFinished(int task_id);		// ��������Ƿ������
	void TaskRecordRemove(int task_id);	// �����¼ɾ��

	void OnRoleLogin();		// ����ʱ�·�һ������ɵ�������Ϣ
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
