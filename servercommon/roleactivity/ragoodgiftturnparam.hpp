#ifndef __RA_GOOD_GIFT_TURN_PARAM_HPP__
#define __RA_GOOD_GIFT_TURN_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"

#pragma pack(push, 4)

static const int MAX_GOOD_GIFT_TURN_NUM = 5;			//转盘格子最大数
UNSTD_STATIC_CHECK(MAX_GOOD_GIFT_TURN_NUM <= (int)(BIT_COUNT_OF_BYTE * sizeof(short)));

struct RAGoodGiftTurnParam
{
	RAGoodGiftTurnParam() {  this->Reset(); }

	void Reset()
	{
		ra_begin_time = 0;
		fetch_num = 0;
		fetch_flag = 0;
		last_add_fetch_num_day = 0;
	}

	void ActReset()
	{
		fetch_num = 0;
		fetch_flag = 0;
		last_add_fetch_num_day = 0;
	}

	unsigned int ra_begin_time;
	short fetch_num;					// 剩余领取次数
	short fetch_flag;					// 领取奖励标识(二进制) 
	int last_add_fetch_num_day;			// 上次领取标识时活动天数(活动开启天数)
};

UNSTD_STATIC_CHECK(sizeof(RAGoodGiftTurnParam) <= sizeof(RoleActivityRoleData));

#pragma pack(pop)

#endif