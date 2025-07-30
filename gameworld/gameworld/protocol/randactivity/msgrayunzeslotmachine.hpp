#ifndef __MSG_RA_YUN_ZE_SLOT_MACHINE_HPP__
#define __MSG_RA_YUN_ZE_SLOT_MACHINE_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/rayunzeslotmachineparam.hpp"

#pragma pack(push, 4)

namespace Protocol
{
	// 由协议  3000
	enum RA_YUN_ZE_SLOT_MACHINE_OPERA_TYPE
	{
		RA_YUN_ZE_SLOT_MACHINE_TYPE_GET_INFO = 0,		// 请求信息下发
		RA_YUN_ZE_SLOT_MACHINE_TYPE_LOTTERY = 1,		// 请求抽奖
		RA_YUN_ZE_SLOT_MACHINE_TYPE_LOCK_REWARD = 2,	// 请求锁定奖励 param1 - 奖励编号
		RA_YUN_ZE_SLOT_MACHINE_TYPE_BUY = 3,			// 请求购买
	};

	class SCRAYunZeSlotMachineInfo			//!< 3118 随机活动-纷享同花礼信息下发
	{
	public:
		SCRAYunZeSlotMachineInfo();				
		MessageHeader header;

		short buy_times;				// 抽奖道具已购买次数
		short surplus_key;				// 玩家钥匙剩余数量
		int lock_reward[YUN_ZE_SLOT_NACHINE_LOCK_REWARD_TYPE];		// MAX为3，每个位置存一个玩家锁定的奖励位
	};

	class SCRAYunZeSlotMachineResultInfo	//!< 3119 随机活动-纷享同花礼摇奖结果信息下发
	{
	public:
		SCRAYunZeSlotMachineResultInfo();
		MessageHeader header;

		int rand_result;			//摇奖结果1-4
	};

}

#pragma pack(pop)

#endif