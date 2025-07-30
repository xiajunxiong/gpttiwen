#ifndef __MSG_RA_YUN_ZE_BLOOMING_RECORD_HPP__
#define __MSG_RA_YUN_ZE_BLOOMING_RECORD_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/rayunzebloomingrecordparam.hpp"

#pragma pack(push,4)

namespace Protocol
{

	class SCRAYunZeBloomingRecordInfo			//!< 3115 随机活动-云泽繁花录信息下发
	{
	public:
		SCRAYunZeBloomingRecordInfo();
		MessageHeader header;

		short times;						// 已挑战次数
		short cur_integral;					// 当前玩家积分
		int challenge_start_timestamp;		// 挑战开始时间戳,0-挑战结束
		int reward_list[YUNZE_BLOOMING_RECORD_MAX_REWARD_NUM];	// 奖励是否已领取，0-未领取，1-已领取，长度5（下标0-4代表1-5的盒子）
	};
}

#pragma pack(pop)

#endif // __MSG_RA_YUN_ZE_BLOOMING_RECORD_HPP__
