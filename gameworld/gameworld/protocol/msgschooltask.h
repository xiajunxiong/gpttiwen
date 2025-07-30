#ifndef __MSG_SCHOOL_TASK_H__
#define __MSG_SCHOOL_TASK_H__

#include "servercommon/userprotocal/msgheader.h"
#include "gamedef.h"
#include "servercommon/schooltaskdef.h"

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{
	class SCSchoolTaskAllInfo					//!< 所以信息  登录时下发一次2180
	{
	public:
		SCSchoolTaskAllInfo();
		MessageHeader		header;

		char task_status[SCHOOL_TASK_NUM];				// 状态
		int task_list[SCHOOL_TASK_NUM];					// id
		int task_progress_list[SCHOOL_TASK_NUM];		// 进度
	};

	class SCSchoolTaskSingleInfo				//!< 单条任务信息 有改变时下发一次2181
	{
	public:
		SCSchoolTaskSingleInfo();
		MessageHeader		header;

		int task_status;				// 状态
		int task_list;					// id
		int task_progress_list;			// 进度
		short is_new;					// 是否显示为新任务
		short reserve_sh;
	};

	class CSSchoolTaskReq						//!< 操作任务请求2182
	{
	public:
		CSSchoolTaskReq();
		MessageHeader		header;

		enum SchoolOperatorType
		{
			SCHOOL_TASK_TYPE_GET = 0,			//!< 接取任务
			SCHOOL_TASK_TYPE_QUICK,				//!< 快速完成
		};

		int ret_type;			
		int param_1;							//!< 0 - 任务下标  1 - 消耗金币
		int param_2;							//!<			   1 - 金币类型
	};
}

#pragma pack(pop)

#endif

