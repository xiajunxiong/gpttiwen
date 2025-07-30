#ifndef __MSG_RA_TIGER_FISH_EVERY_YEAR_HPP__
#define __MSG_RA_TIGER_FISH_EVERY_YEAR_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/ratigerfisheveryyearparam.hpp"

#pragma pack(push,4)

namespace Protocol
{

	class SCRATigerFishEveryYearInfo	//!< 3138 随机活动-年年有鱼 信息下发
	{
	public:
		SCRATigerFishEveryYearInfo();
		MessageHeader header;

		// 每日是否接取过渔夫任务，0--未接取，1--已接取
		short is_accept_task;

		// 玩家当日接任务时等级
		short role_level;

		// 玩家拥有的鱼数
		int fish_num;

		// 每日任务是否完成 [0-TIRGER_FEY_TYPE_MAX(9)]
		BitMap<RATIGERFEY::TIRGER_FEY_TYPE_MAX> task_status;

		// 每日任务奖励是否已领取[0-TIRGER_FEY_TYPE_MAX(9)]
		BitMap<RATIGERFEY::TIRGER_FEY_TYPE_MAX> task_reward_status;

		// 每日任务进度 [0-TIRGER_FEY_TYPE_MAX(9)]
		int task_extent[RATIGERFEY::TIRGER_FEY_TYPE_MAX];

		// 奖励档位是否可领取[0-31]
		BitMap<RATIGERFEY::MAX_REWARD_STAGE> is_receive_reward;

		// 奖励档位是否已领取[0-31]
		BitMap<RATIGERFEY::MAX_REWARD_STAGE> reward_stage_status;
	};

}


#pragma pack(pop)

#endif