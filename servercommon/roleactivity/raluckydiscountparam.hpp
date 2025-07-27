#ifndef __RA_LUCKY_DISCOUNT_PARAM_HPP__
#define __RA_LUCKY_DISCOUNT_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"

//////////////////////////////////////////// 2071 随机活动 - 幸运折扣  //////////////////////////////////////////

static const int LUCKY_DISCOUNT_COMMODITY_MAX_NUM = 24;
static const int LUCKY_DISCOUNT_BEGIN_DISCOUNT = 10;			//默认初始折扣 10折（不打折）

#pragma pack(push, 4)
////////////////////////////////////////////  服务器数据  //////////////////////////////////////////////////////


////////////////////////////////////////////  个人数据  //////////////////////////////////////////////////////

struct RALuckyDiscountParam
{
	RALuckyDiscountParam() { this->Reset(); }
	void Reset()
	{
		ra_lucky_discount_begin_timestamp = 0;
		ra_lucky_discount_discount = LUCKY_DISCOUNT_BEGIN_DISCOUNT;
		memset(ra_lucky_discount_buy_count, 0, sizeof(ra_lucky_discount_buy_count));
		memset(ra_lucky_discount_today_buy_count, 0, sizeof(ra_lucky_discount_today_buy_count));
	}

	unsigned int ra_lucky_discount_begin_timestamp;
	int ra_lucky_discount_discount;													//本次折扣
	char ra_lucky_discount_buy_count[LUCKY_DISCOUNT_COMMODITY_MAX_NUM];				//购买次数
	char ra_lucky_discount_today_buy_count[LUCKY_DISCOUNT_COMMODITY_MAX_NUM];				//购买次数
};

UNSTD_STATIC_CHECK(sizeof(RALuckyDiscountParam) <= sizeof(RoleActivityRoleData))

#pragma pack(pop)


#endif	//__RA_LUCKY_DISCOUNT_PARAM_HPP__
