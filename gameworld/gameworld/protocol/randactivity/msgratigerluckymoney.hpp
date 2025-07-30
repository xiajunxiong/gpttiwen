#ifndef __MSG_RA_TIGER_LUCKY_MONEY_HPP__
#define __MSG_RA_TIGER_LUCKY_MONEY_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/ratigerluckymoneyparam.hpp"


#pragma pack(push, 4)

namespace Protocol
{

	class SCRATigerLuckyMoneyInfo		//!< 3146 随机活动-寅虎压岁
	{
	public:
		SCRATigerLuckyMoneyInfo();
		MessageHeader header;

		// 压岁钱累计数量
		int lucky_money_num;

		// 压岁钱是否可领取,0-不可领取
		short is_can_receive;

		// 玩家当日登录时等级
		short role_level;

		// 每日任务是否完成 [0-RATLM_TASK_TYPE_MAX(9)]
		BitMap<RATLM::RATLM_TASK_TYPE_MAX> task_status;

		// 每日任务奖励是否已领取[0-RATLM_TASK_TYPE_MAX(9)]
		BitMap<RATLM::RATLM_TASK_TYPE_MAX> task_reward_status;

		// 每日任务进度 [0-RATLM_TASK_TYPE_MAX(9)]
		int task_extent[RATLM::RATLM_TASK_TYPE_MAX];
	};

}


#pragma pack(pop)

#endif