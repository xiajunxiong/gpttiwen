#ifndef __RA_COLLECT_WORD_PARAM_HPP__
#define __RA_COLLECT_WORD_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"


//////////////////////////////////////////// 2068 随机活动 - 集字活动  //////////////////////////////////////////

static const int RA_COLLECT_WORD_MAX_ENTRY_NUM = 20;  //集字好礼最大条目数，配置表条数不能超过这个数。
static const int RA_COLLECT_WORD_MAX_CONSUME_ITEM = 5;  //集字好礼兑换物品 需要最大物品种类
static const int RA_COLLECT_WORD_MAX_DAY_GIFT_REWARD_NUM = 5;	//集字有礼每日礼包最大的道具数

UNSTD_STATIC_CHECK(RA_COLLECT_WORD_MAX_ENTRY_NUM % 2 == 0);

#pragma pack(push, 4)
////////////////////////////////////////////  服务器数据  //////////////////////////////////////////////////////


////////////////////////////////////////////  个人数据  //////////////////////////////////////////////////////

struct RACollectWordParam
{
	RACollectWordParam() { this->Reset(); }
	void Reset()
	{	
		ra_collect_word_begin_timestamp = 0;
		ra_collect_word_day_gift_is_fetch = 0;
		reserve_sh = 0;
		for (int i = 0; i < ARRAY_LENGTH(ra_collect_word_exchange_num); ++i)
		{
			ra_collect_word_exchange_num[i] = 0;
		}

		for (int i = 0; i < ARRAY_LENGTH(ra_collect_word_total_exchange_num); ++i)
		{
			ra_collect_word_total_exchange_num[i] = 0;
		}
	}

	unsigned int ra_collect_word_begin_timestamp;
	short ra_collect_word_day_gift_is_fetch;									//集字有礼每日免费礼包  0:未领取 1:已领取
	short reserve_sh;															//备用
	unsigned short ra_collect_word_exchange_num[RA_COLLECT_WORD_MAX_ENTRY_NUM]; //记录每个条目的兑换次数
	unsigned short ra_collect_word_total_exchange_num[RA_COLLECT_WORD_MAX_ENTRY_NUM]; //
};

UNSTD_STATIC_CHECK(sizeof(RACollectWordParam) <= sizeof(RoleActivityRoleData))

#pragma pack(pop)


#endif	//__RA_COLLECT_WORD_PARAM_HPP__
