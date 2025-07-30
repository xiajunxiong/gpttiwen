#include "taskrecorder.h"
#include "engineadapter.h"
#include "protocol/msgtask.h"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"

#include "task/taskpool.h"

TaskRecorder::TaskRecorder() : m_role_module_mgr(NULL), m_max_finished_main_task_id(0)
{

}

TaskRecorder::~TaskRecorder()
{
}

bool TaskRecorder::Init(RoleModuleManager * role_module_mgr, int buff_len, const char * record_buff, const TaskChosedParam & param)
{
	m_role_module_mgr = role_module_mgr;
	
	if (NULL != record_buff && buff_len >= (int)sizeof(short))
	{
		short total_count = *(short *)record_buff;
		record_buff += sizeof(short);
		buff_len -= sizeof(short);

		for (int i = 0; i < total_count; ++i)
		{
			if (buff_len <= 0) break;

			short task_id = *(short *)record_buff;
			record_buff += sizeof(short);
			buff_len -= sizeof(short);

			m_record_set.insert(task_id);

			this->UpdateMaxFinishedMainTask(task_id);
		}
	}

	m_chosed_param.chosed_count = param.chosed_count;
	for (int idx = 0;idx < param.chosed_count;idx++)
	{
		m_chosed_param.chosed_list[idx].index = param.chosed_list[idx].index;
		m_chosed_param.chosed_list[idx].param = param.chosed_list[idx].param;
	}

	return true;
}

void TaskRecorder::GetInitParam(char * record_buff, int * record_buff_len, TaskChosedParam * param)
{
	if (NULL == record_buff || NULL == record_buff_len) return;

	*record_buff_len = 0;

	short total_count = (short)(m_record_set.size());
	if (total_count > MAX_TASK_RECORD_NUM) total_count = MAX_TASK_RECORD_NUM;

	*(short *)record_buff = total_count;
	record_buff += sizeof(short);
	*record_buff_len += sizeof(short);

	for (std::set<TaskID>::iterator iter = m_record_set.begin(); iter != m_record_set.end(); ++iter)
	{
		*(short *)record_buff = *iter;
		record_buff += sizeof(short);
		*record_buff_len += sizeof(short);
	}

	param->chosed_count = m_chosed_param.chosed_count;
	for (int idx = 0; idx < m_chosed_param.chosed_count; idx++)
	{
		param->chosed_list[idx].index = m_chosed_param.chosed_list[idx].index;
		param->chosed_list[idx].param = m_chosed_param.chosed_list[idx].param;
	}
}

void TaskRecorder::OnTaskDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	if (old_dayid == now_dayid) return;
}

void TaskRecorder::OnTaskWeekChange()
{
}

void TaskRecorder::TaskComplete(int task_id)
{
	m_record_set.insert(task_id);

	this->UpdateMaxFinishedMainTask(task_id);
}

bool TaskRecorder::HaveFinished(int task_id)
{
	std::set<TaskID>::iterator s_it = m_record_set.find(task_id);
	if (s_it != m_record_set.end()) return true;
	
	return false;
}

void TaskRecorder::TaskRecordRemove(int task_id)
{
	m_record_set.erase(task_id);
}

void TaskRecorder::OnRoleLogin()
{
	this->SendTaskCompleteList();
}

void TaskRecorder::ClearAppoiontedTypeTaskRecord(int clear_task_type)
{
	if (clear_task_type < 0 || clear_task_type >= TASK_TYPE_MAX) return;

	for (std::set<TaskID>::iterator it = m_record_set.begin(); it != m_record_set.end();)
	{
		const TaskInfomaConfig * task_cfg = TASKPOOL->GetTaskCfgByID(*it);
		if (NULL != task_cfg && clear_task_type == task_cfg->task_type)
		{
			m_record_set.erase(it++);
		}
		else
		{
			++it;
		}
	}
}

void TaskRecorder::SetTaskChosed(unsigned short task_index, unsigned short param)
{
	bool has_idx = false;
	for(int i = 0; i < m_chosed_param.chosed_count;i++)
	{
		if (m_chosed_param.chosed_list[i].index == task_index)
		{
			m_chosed_param.chosed_list[i].param = param;
			has_idx = true;
		}
	}
	if (!has_idx)
	{
		m_chosed_param.chosed_list[m_chosed_param.chosed_count].index = task_index;
		m_chosed_param.chosed_list[m_chosed_param.chosed_count].param = param;
		m_chosed_param.chosed_count++;
	}
	//this->SendTaskChosedInfo();
}

void TaskRecorder::SendTaskChosedInfo()
{
	Protocol::SCTaskChosedInfo info;
	info.info_count = 0;
	for (int i = 0; i < m_chosed_param.chosed_count && i < MAX_TASK_CHOSED_RECORD_NUM;i++)
	{
		info.chosed_list[i].index = m_chosed_param.chosed_list[i].index;
		info.chosed_list[i].param = m_chosed_param.chosed_list[i].param;
		info.info_count++;
	}
	m_role_module_mgr->NetSend((const char*)&info, sizeof(info));
}

void TaskRecorder::ClearPetFeiShengRecord()
{
	this->ClearAppoiontedTypeTaskRecord(TASK_TYPE_PET_FEI_SHENG_TASK);
}

void TaskRecorder::ClearRoleGuildTaskRecord()
{
	this->ClearAppoiontedTypeTaskRecord(TASK_TYPE_ROLE_GUILD_TASK);
}

void TaskRecorder::SendTaskCompleteList()
{
	Protocol::SCTaskCompleteTaskList tctl;
	tctl.list_count = 0;

	for (std::set<TaskID>::iterator loop = m_record_set.begin(); loop != m_record_set.end(); ++loop)
	{
		if (tctl.list_count >= MAX_TASK_RECORD_NUM) break;
		tctl.task_list[tctl.list_count++] = *loop;
	}
	
	int send_len = sizeof(tctl) - (MAX_TASK_RECORD_NUM - tctl.list_count) * sizeof(TaskID);
	if (send_len > 0) m_role_module_mgr->NetSend((const void *)&tctl, send_len);
}

void TaskRecorder::UpdateMaxFinishedMainTask(TaskID task_id)
{
	const TaskInfomaConfig* cfg = TASKPOOL->GetTaskCfgByID(task_id);
	if (NULL == cfg || cfg->task_type != TASK_TYPE_MAIN_QUEST) return;

	m_max_finished_main_task_id = GetMax(m_max_finished_main_task_id, task_id);
}
