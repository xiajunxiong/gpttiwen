#ifndef __MSG_RA_TRUN_TABLE_HPP__
#define __MSG_RA_TRUN_TABLE_HPP__

#include "servercommon/userprotocal/msgheader.h"

//////////////////////////////////////////// 2051 随机活动  开服活动 -- 转盘活动  //////////////////////////////////////////

#pragma pack(push, 4)

namespace Protocol
{
	// 由协议  3000
	enum RA_TRUN_TABLE_OPERA_TYPE
	{
		RA_TRUN_TABLE_OPERA_TYPE_GET_INFO = 0,						// 获取信息
		RA_TRUN_TABLE_OPERA_TYPE_FREE_TRUN_TABLE = 1,				// 免费转盘 	
		RA_TRUN_TABLE_OPERA_TYPE_KILL_BOSS_TRUN_TABLE = 2,			// 消耗BOSS击杀数转盘
		RA_TRUN_TABLE_OPERA_TYPE_GET_REWARD = 3,					// 获取转盘奖励  param_1:   param_2:  
	};

	class SCRandActivityTurnTableInfo								//!<3003 随机活动-开服转盘
	{
	public:
		SCRandActivityTurnTableInfo();
		MessageHeader header;

		int ra_has_free_turn_table_num;								// 免费转盘次数    次数最高为1,消耗完才继续计时
		int kill_boss_num;											// 当前BOSS击杀数
		unsigned int next_refresh_time;								// 下次免费转盘刷新时间
	};

	class SCRandActivityTurnTableReward								//!<3004 随机活动-开服转盘-请求奖励
	{
	public:
		SCRandActivityTurnTableReward();
		MessageHeader header;

		int index;
	};
}

#pragma pack(pop)

#endif  // __MSG_RA_TRUN_TABLE_HPP__
