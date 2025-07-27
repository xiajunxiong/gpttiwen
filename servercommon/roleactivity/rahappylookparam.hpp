#ifndef __RA_HAPPY_LOOK_PARAM_HPP__
#define __RA_HAPPY_LOOK_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"


//////////////////////////////////////////// 连连乐翻天 //////////////////////////////////////////

#pragma pack(push, 4)
////////////////////////////////////////////  服务器数据  //////////////////////////////////////////////////////


////////////////////////////////////////////  个人数据  //////////////////////////////////////////////////////

struct RAHappyLookParam
{
	RAHappyLookParam() { this->Reset(); }

	void Reset()
	{
		ra_begin_timestamp = 0;
		ra_frequency_num = 0;
		challenge_index = -1;
		challenge_is_succ = -1;
		challenge_end_timestamp = 0;
	}

	void ResetQuency()
	{
		challenge_index = -1;
		challenge_is_succ = -1;
		challenge_end_timestamp = 0;
	}

	unsigned int ra_begin_timestamp;
	short ra_frequency_num;										// 已挑战次数

	char challenge_index;										// 挑战index
	char challenge_is_succ;										// 是否挑战成功
	int challenge_end_timestamp;								// 挑战失败时间戳
};

UNSTD_STATIC_CHECK(sizeof(RAHappyLookParam) <= sizeof(RoleActivityRoleData))

#pragma pack(pop)
#endif	//__RA_HAPPY_LOOK_PARAM_HPP__
