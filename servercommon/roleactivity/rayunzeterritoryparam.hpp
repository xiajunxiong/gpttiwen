#ifndef __RA_YUN_ZE_TERRITORY_PARAM_HPP__
#define  __RA_YUN_ZE_TERRITORY_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"


static const int PICTURE_FRAGMENTS_NUM = 7;		// 拼图碎片最大数量
static const int PICTURE_NUM = 3;				// 图片数量

#pragma pack(push, 4)

enum REWARD_STATUS
{
	REWARD_STATUS_EXIST = 0,	// 奖励未领取
	REWARD_STATUS_LACK = 1,		// 奖励已领取
};

struct RAYunZeTerritoryParam
{
	RAYunZeTerritoryParam() { this->Reset(); }

	void Reset()
	{
		is_get_reward = 0;
		times = 0;
		ra_begin_timestamp = 0;

		memset(light_up,0,sizeof(light_up));
	}

	void OnDayReset()
	{
		is_get_reward = 0;
		times = 0;

		memset(light_up, 0, sizeof(light_up));
	}

	unsigned int ra_begin_timestamp;
	int light_up[PICTURE_FRAGMENTS_NUM];	//点亮的碎片,1-点亮，0-未点亮
	short is_get_reward;	// 是否领取奖励 1-已领取，0-未领取
	short times;		// 当日已探索次数

};
UNSTD_STATIC_CHECK(sizeof(RAYunZeTerritoryParam) <= sizeof(RoleActivityRoleData));


#pragma pack(pop)

#endif