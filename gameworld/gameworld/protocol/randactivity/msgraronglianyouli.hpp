#ifndef __MSG_RA_RONG_LIAN_YOU_LI_HPP__
#define __MSG_RA_RONG_LIAN_YOU_LI_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/randactivityroleparam.hpp"

#pragma pack(push, 4)

namespace Protocol
{

	class SCRARongLianYouLiInfo		//!< 3145(������Э���) ����-��������������Ϣ�·�
	{
	public:
		SCRARongLianYouLiInfo();
		MessageHeader header;

		short total_task_finish;		// �������������
		short total_task;				// ����������
		BitMap<RARongLianYouLi::RONG_LIAN_YOU_LI_MAX_BITMAP> has_task_stage_reward;	// ����׶ν����Ƿ�����ȡ
		
		short task_score[RARongLianYouLi::RONG_LIAN_YOU_LI_MAX_TASK_NUM];			// �������[0-11]
		short reward_receive_num[RARongLianYouLi::RONG_LIAN_YOU_LI_MAX_TASK_NUM];	// ��������ȡ����[0-11]	
	};

}

#pragma  pack(pop)

#endif