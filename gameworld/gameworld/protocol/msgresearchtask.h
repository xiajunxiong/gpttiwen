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
		CSResearchTaskRequest();			//!< 2190�����е��о� ��������
		MessageHeader header;

		enum ResearchTaskOperateType
		{
			RESEARCH_TASK_ACCEPT = 0,		//!< ��ȡ����
			RESEARCH_TASK_COMMIT = 1,		//!< �ύ����

			RESEARCH_TASK_OP_MAX
		};

		int op_type;					//!< ��������
	};

	class SCResearchTaskMessage
	{
	public:
		SCResearchTaskMessage();		//!< 2191�����е��о� ������Ϣ
		MessageHeader header;

		int today_finish_times;			//!< ����������еĴ���
		short reserve_sh;				//!< ������
		short is_new;					//!< �Ƿ�������
		ResearchTaskInfo task_info;		//!< ����������Ϣ
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