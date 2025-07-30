#ifndef __MSG_RESEARCH_TASK_H__
#define __MSG_RESEARCH_TASK_H__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/researchtaskdef.h"

#pragma pack(push, 4)

namespace Protocol
{
	class CSResearchTaskRequest
	{
	public:
		CSResearchTaskRequest();			//!< 2190：真中的研究 操作请求
		MessageHeader header;

		enum ResearchTaskOperateType
		{
			RESEARCH_TASK_ACCEPT = 0,		//!< 领取任务
			RESEARCH_TASK_COMMIT = 1,		//!< 提交任务

			RESEARCH_TASK_OP_MAX
		};

		int op_type;					//!< 操作类型
	};

	class SCResearchTaskMessage
	{
	public:
		SCResearchTaskMessage();		//!< 2191：真中的研究 所有信息
		MessageHeader header;

		int today_finish_times;			//!< 今天完成真中的次数
		short reserve_sh;				//!< 保留字
		short is_new;					//!< 是否新任务
		ResearchTaskInfo task_info;		//!< 真中任务信息
	};

	class CSResearchTaskAutoStart
	{
	public:
		CSResearchTaskAutoStart();
		MessageHeader header;
	};
}

#pragma pack(pop)
#endif