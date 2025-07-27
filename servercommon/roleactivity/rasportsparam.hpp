#ifndef __RA_SPORTS_PARAM_HPP__
#define __RA_SPORTS_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"

//////////////////////////////////////////// 2079 开服活动 - 竞技场排名奖励  //////////////////////////////////////////


#pragma pack(push, 4)
////////////////////////////////////////////  服务器数据  //////////////////////////////////////////////////////


////////////////////////////////////////////  个人数据  //////////////////////////////////////////////////////

struct RASportsParam
{
	RASportsParam() { this->Reset(); }
	void Reset()
	{
		ra_sports_begin_timestamp = 0;
		ra_sports_histroy_top = -1;
		ra_sports_histroy_reward = 0;
	}

	unsigned int ra_sports_begin_timestamp;
	short ra_sports_histroy_top;			//-1是未上榜
	unsigned short ra_sports_histroy_reward;//二进制
};

UNSTD_STATIC_CHECK(sizeof(RASportsParam) <= sizeof(RoleActivityRoleData))

#pragma pack(pop)


#endif	//__RA_SPORTS_PARAM_HPP__
