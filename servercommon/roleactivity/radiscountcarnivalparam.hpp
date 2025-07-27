#ifndef __RA_DISCOUNT_CARNIVAL_PARAM_HPP__
#define __RA_DISCOUNT_CARNIVAL_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"

//////////////////////////////////////////// 2069 随机活动 - 折扣狂欢  //////////////////////////////////////////

static const int DISCOUNT_CARNIVAL_COMMODITY_MAX_NUM = 16;			//商品最大数量

#pragma pack(push, 4)
////////////////////////////////////////////  服务器数据  //////////////////////////////////////////////////////


////////////////////////////////////////////  个人数据  //////////////////////////////////////////////////////

struct RADiscountCarnivalParam
{
	RADiscountCarnivalParam() { this->Reset(); }
	void Reset()
	{
		ra_discount_carnival_begin_timestamp = 0;
		ra_discount_carnival_buy_flag.Reset();
		memset(ra_discount_carnival_buy_count, 0, sizeof(ra_discount_carnival_buy_count));
	}

	unsigned int ra_discount_carnival_begin_timestamp;
	BitMap<DISCOUNT_CARNIVAL_COMMODITY_MAX_NUM> ra_discount_carnival_buy_flag;				//购买标志(弃用 改用下面购买次数)
	unsigned char ra_discount_carnival_buy_count[DISCOUNT_CARNIVAL_COMMODITY_MAX_NUM];		//商品购买次数
};

UNSTD_STATIC_CHECK(sizeof(RADiscountCarnivalParam) <= sizeof(RoleActivityRoleData))

#pragma pack(pop)


#endif	//__RA_DISCOUNT_CARNIVAL_PARAM_HPP__
