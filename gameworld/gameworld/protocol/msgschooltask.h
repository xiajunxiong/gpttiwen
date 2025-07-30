#ifndef __MSG_SCHOOL_TASK_H__
#define __MSG_SCHOOL_TASK_H__

#include "servercommon/userprotocal/msgheader.h"
#include "gamedef.h"
#include "servercommon/schooltaskdef.h"

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{
	class SCSchoolTaskAllInfo					//!< ������Ϣ  ��¼ʱ�·�һ��2180
	{
	public:
		SCSchoolTaskAllInfo();
		MessageHeader		header;

		char task_status[SCHOOL_TASK_NUM];				// ״̬
		int task_list[SCHOOL_TASK_NUM];					// id
		int task_progress_list[SCHOOL_TASK_NUM];		// ����
	};

	class SCSchoolTaskSingleInfo				//!< ����������Ϣ �иı�ʱ�·�һ��2181
	{
	public:
		SCSchoolTaskSingleInfo();
		MessageHeader		header;

		int task_status;				// ״̬
		int task_list;					// id
		int task_progress_list;			// ����
		short is_new;					// �Ƿ���ʾΪ������
		short reserve_sh;
	};

	class CSSchoolTaskReq						//!< ������������2182
	{
	public:
		CSSchoolTaskReq();
		MessageHeader		header;

		enum SchoolOperatorType
		{
			SCHOOL_TASK_TYPE_GET = 0,			//!< ��ȡ����
			SCHOOL_TASK_TYPE_QUICK,				//!< �������
		};

		int ret_type;			
		int param_1;							//!< 0 - �����±�  1 - ���Ľ��
		int param_2;							//!<			   1 - �������
	};
}

#pragma pack(pop)

#endif

