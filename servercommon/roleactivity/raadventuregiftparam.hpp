#ifndef __RA_ADVENTURE_GIFT_PARAM_HPP__
#define __RA_ADVENTURE_GIFT_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"

//////////////////////////////////////////// 2074 随机活动 - 奇遇礼包  //////////////////////////////////////////


#pragma pack(push, 4)
////////////////////////////////////////////  服务器数据  //////////////////////////////////////////////////////


////////////////////////////////////////////  个人数据  //////////////////////////////////////////////////////

struct RAAdventureGiftParam
{
	RAAdventureGiftParam() { this->Reset(); }
	void Reset()
	{
		ra_adventure_gift_begin_timestamp = 0;
		ra_adventure_gift_seq = 0;
		ra_adventure_gift_end_time = 0;
	}

	unsigned int ra_adventure_gift_begin_timestamp;
	int  ra_adventure_gift_seq;
	unsigned int  ra_adventure_gift_end_time;
};

UNSTD_STATIC_CHECK(sizeof(RAAdventureGiftParam) <= sizeof(RoleActivityRoleData))

#pragma pack(pop)


#endif	//__RA_ADVENTURE_GIFT_PARAM_HPP__
