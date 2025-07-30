#ifndef __MSG_RA_YUAN_QI_HELP_HPP__
#define __MSG_RA_YUAN_QI_HELP_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/rayuanqihelpparam.hpp"


#pragma pack(push, 4)

namespace Protocol
{

	class SCRAYuanQiHelpInfo				//!< 3158 ����-Ԫ������ ��Ϣ�·�
	{
	public:
		SCRAYuanQiHelpInfo();
		MessageHeader header;

		int today_play_times;//���������
		int task_id;
		int pre_task_id;
	};
}


#pragma pack(pop)

#endif
