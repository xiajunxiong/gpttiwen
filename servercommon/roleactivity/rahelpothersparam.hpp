#ifndef __RA_HELP_OTHERS_PARAM_HPP__
#define __RA_HELP_OTHERS_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"

#pragma pack(push, 4)

enum HELP_OTHERS_TASK_STATUS
{
	HELP_OTHERS_TASK_STATUS_NOT_ACCEPT = 0,		//δ��ȡ
	HELP_OTHERS_TASK_STATUS_ACCEPT = 1,			//�ѽ�ȡ�����������
	HELP_OTHERS_TASK_STATUS_FINISH = 2,			//���
};

struct RAHelpOthersParam
{
	RAHelpOthersParam() { this->Reset(); }

	void Reset()
	{
		ra_begin_timestamp = 0;
		pass_count = 0;
		task_id = 0;
		task_status = 0;
		progress = 0;
		task_type = 0;
		next_rank_pet_task_type = 0;
		accept_npc_seq = 0;
		commit_npc_seq = 0;
		param1 = 0;
		param2 = 0;
	}

	void DayReset()
	{
		pass_count = 0;
		task_id = 0;
		task_status = 0;
		progress = 0;
		task_type = 0;
		next_rank_pet_task_type = 0;
		accept_npc_seq = 0;
		commit_npc_seq = 0;
		param1 = 0;
		param2 = 0;
	}
	void ClearTaskInfo()
	{
		task_id = 0;
		task_status = 0;
		progress = 0;
		task_type = 0;
		accept_npc_seq = 0;
		commit_npc_seq = 0;
		param1 = 0;
		param2 = 0;
	}

	unsigned int ra_begin_timestamp;
	short pass_count;
	short task_id;
	short task_status;
	short progress;			//�����������ʱ��Ҫ
	short task_type;		
	short next_rank_pet_task_type;	//��һ�ο��Կ�ʼ���Ѱ����������� > 
	int accept_npc_seq;				//��ȡNPC_seq
	int commit_npc_seq;				//�ύNPC_seq
	int param1;
	int param2;

};
UNSTD_STATIC_CHECK(sizeof(RAHelpOthersParam) <= sizeof(RoleActivityRoleData));

#pragma pack(pop)

#endif	//__RA_HELP_OTHERS_PARAM_HPP__