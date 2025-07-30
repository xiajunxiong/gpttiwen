#ifndef __MSG_RA_GOD_TOKEN_HPP__
#define __MSG_RA_GOD_TOKEN_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/ragodtokenparam.hpp"

//////////////////////////////////////////// 2081 随机活动  封神战令  //////////////////////////////////////////

#pragma pack(push, 4)

namespace Protocol
{
	// 由协议  3000
	enum RA_GOD_TOKEN_OPERA_TYPE
	{
		RA_GOD_TOKEN_OP_TYPE_INFO = 0,					// 信息下发
		RA_GOD_TOKEN_OP_TYPE_FETCH_COMMON = 1,			// 领取普通奖励 param1 - seq
		RA_GOD_TOKEN_OP_TYPE_FETCH_SENIOR = 2,			// 领取高级奖励 param1 - seq
		RA_GOD_TOKEN_OP_TYPE_BUY_EXP = 3,				// 购买经验		param1 - count
		RA_GOD_TOKEN_OP_TYPE_EXCHANGE = 4,				// 兑换			param1 - seq
		RA_GOD_TOKEN_OP_TYPE_FETCH_SURPLUS = 5,			// 领取溢出奖励
		RA_GOD_TOKEN_OP_TYPE_FETCH_DAILY_TASK = 6,		// 领取每日任务奖励 param1 - seq
		RA_GOD_TOKEN_OP_TYPE_FETCH_WEEKLY_TASK = 7,		// 领取每周任务奖励 param1 - seq
	};


	struct SCRAGodTokenInfo					//!<　3039 封神战令数据下发
	{
	public:
		SCRAGodTokenInfo();
		MessageHeader header;

		char time_seq;
		char is_open;
		char is_buy_token;
		char sh_reserver; //无用

		short token_level;
		short token_exp;

		int time_level;

		BitMap<RA_GOD_TOKEN_LEVEL_BIT_INFO> fetch_common_flag;				//普通奖励领取标志位（RA_GOD_TOKEN_LEVEL_BIT_INFO 64）
		BitMap<RA_GOD_TOKEN_LEVEL_BIT_INFO> fetch_senior_flag;				//高级奖励领取标志位（RA_GOD_TOKEN_LEVEL_BIT_INFO 64）

		short daily_level;
		char daily_type[RA_GOD_TOKEN_DAILY_TASK_NUM];						//每日任务类型 RA_GOD_TOKEN_DAILY_TASK_NUM-6
		int daily_type_finish_count[RA_GOD_TOKEN_DAILY_TASK_NUM];			//每日任务类型完成次数 RA_GOD_TOKEN_DAILY_TASK_NUM-6  -1代表已领取

		short weekly_level;
		char weekly_type[RA_GOD_TOKEN_WEEKLY_TASK_NUM];						//每周任务类型 RA_GOD_TOKEN_WEEKLY_TASK_NUM-10
		int weekly_type_finish_count[RA_GOD_TOKEN_WEEKLY_TASK_NUM];			//每周任务类型完成次数 RA_GOD_TOKEN_WEEKLY_TASK_NUM-10  -1代表已领取

		short surplus_exp;													//溢出经验
		short surplus_receive_count;										//溢出奖励可领取次数

		int count;
		int exchange_count[RA_GOD_TOKEN_EXCHANGE_MAXNUM];					//兑换次数
	};

}

#pragma pack(pop)

#endif  // __MSG_RA_GOD_TOKEN_HPP__
