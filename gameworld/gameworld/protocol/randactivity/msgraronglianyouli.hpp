#ifndef __MSG_RA_RONG_LIAN_YOU_LI_HPP__
#define __MSG_RA_RONG_LIAN_YOU_LI_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/randactivityroleparam.hpp"

#pragma pack(push, 4)

namespace Protocol
{

	class SCRARongLianYouLiInfo		//!< 3145(非最终协议号) 随机活动-熔炼有礼所有信息下发
	{
	public:
		SCRARongLianYouLiInfo();
		MessageHeader header;

		short total_task_finish;		// 总完成任务数量
		short total_task;				// 总任务数量
		BitMap<RARongLianYouLi::RONG_LIAN_YOU_LI_MAX_BITMAP> has_task_stage_reward;	// 任务阶段奖励是否已领取
		
		short task_score[RARongLianYouLi::RONG_LIAN_YOU_LI_MAX_TASK_NUM];			// 任务进度[0-11]
		short reward_receive_num[RARongLianYouLi::RONG_LIAN_YOU_LI_MAX_TASK_NUM];	// 奖励已领取数量[0-11]	
	};

}

#pragma  pack(pop)

#endif