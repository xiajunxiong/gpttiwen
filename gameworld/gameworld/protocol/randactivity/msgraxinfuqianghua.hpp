#ifndef __MSG_RA_XIN_FU_QIANG_HUA_HPP__
#define __MSG_RA_XIN_FU_QIANG_HUA_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/randactivityroleparam.hpp"
#include "servercommon/roleactivity/raxinfuqianghuaparam.hpp"

#pragma  pack(push, 4)

namespace Protocol
{

	////////////////////////////////  �·�ǿ��  /////////////////////////////////////
	struct SCRAXinFuQiangHua	//!<3188 ����-�·�ǿ��
	{
	public:
		SCRAXinFuQiangHua();
		MessageHeader header;

		int free_reward_flag;		// ÿ��������
		BitMap<MAX_XIN_FU_QIANG_HUA_BITMAP> task_finish[MAX_PET_QUALITY_NUM];		// �����Ƿ������flag
		BitMap<MAX_XIN_FU_QIANG_HUA_BITMAP> reward_receive[MAX_PET_QUALITY_NUM];	// �����Ƿ�����ȡflag
	};


}

#pragma  pack(pop)

#endif
