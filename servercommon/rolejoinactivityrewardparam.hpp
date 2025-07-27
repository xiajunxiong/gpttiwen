#ifndef __ROLE_JOIN_ACTIVITY_REWARD_PARAM_H__
#define __ROLE_JOIN_ACTIVITY_REWARD_PARAM_H__

#include "servercommon/serverdef.h"
#include "servercommon/servercommon.h"
#include "servercommon/commonsavedef.h"

#pragma pack(push, 4)

const long long static MAX_JOIN_ACTIVITY_REWARD_COUNT = 32;			

struct RoleJoinActivityRewardParam
{
	RoleJoinActivityRewardParam()
	{
		this->Reset();
	}

	void Reset()
	{
		last_join_time = 0;
		join_flag = 0;
		get_reward_flag = 0;
	}

	unsigned int last_join_time;			// 最后参与活动的时间
	int join_flag;							// 参与活动的标记 [按位]
	int get_reward_flag;					// 已领取的标记	  [按位]
};

typedef char RoleJoinActivityRewardParamHex[sizeof(RoleJoinActivityRewardParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(RoleJoinActivityRewardParamHex) < 64);


#pragma pack(pop)

#endif	// __ROLE_JOIN_ACTIVITY_REWARD_PARAM_H__
