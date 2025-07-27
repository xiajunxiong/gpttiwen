#ifndef __RA_XIN_FU_LING_CHONG_PARAM_HPP__
#define __RA_XIN_FU_LING_CHONG_PARAM_HPP__

#include "servercommon/roleactivitydef.hpp"

#pragma pack(push,4)


static const int MAX_RA_XIN_FU_LING_CHONG_BUY_COUNT = 1;		//目前新服灵宠最大购买次数

///////////////////////////// 新服灵宠 ///////////////////////////////////
struct RAXinFuLingChongParam
{
	RAXinFuLingChongParam() { this->Reset(); }
	void Reset()
	{
		ra_begin_timestamp = 0;
		buy_count = 0;
	}

	unsigned int ra_begin_timestamp;
	int buy_count;
};
UNSTD_STATIC_CHECK(sizeof(RAXinFuLingChongParam) <= sizeof(RoleActivityRoleData))

#pragma pack(pop)

#endif
