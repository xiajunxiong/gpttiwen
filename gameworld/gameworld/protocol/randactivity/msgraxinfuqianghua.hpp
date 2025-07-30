#ifndef __MSG_RA_XIN_FU_QIANG_HUA_HPP__
#define __MSG_RA_XIN_FU_QIANG_HUA_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/randactivityroleparam.hpp"
#include "servercommon/roleactivity/raxinfuqianghuaparam.hpp"

#pragma  pack(push, 4)

namespace Protocol
{

	////////////////////////////////  新服强化  /////////////////////////////////////
	struct SCRAXinFuQiangHua	//!<3188 随机活动-新服强化
	{
	public:
		SCRAXinFuQiangHua();
		MessageHeader header;

		int free_reward_flag;		// 每日免费礼包
		BitMap<MAX_XIN_FU_QIANG_HUA_BITMAP> task_finish[MAX_PET_QUALITY_NUM];		// 任务是否已完成flag
		BitMap<MAX_XIN_FU_QIANG_HUA_BITMAP> reward_receive[MAX_PET_QUALITY_NUM];	// 奖励是否已领取flag
	};


}

#pragma  pack(pop)

#endif
