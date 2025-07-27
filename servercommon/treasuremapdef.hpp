#ifndef __TREASURE_MAP_DEF_H__
#define __TREASURE_MAP_DEF_H__

#include "servercommon/servercommon.h"

#pragma pack(push, 4)

struct TreasureMapParam
{
	TreasureMapParam()
	{
		this->Reset();
	}

	void Reset()
	{
		play_times = 0;
		get_reward_times = 0;
		monster_pos_x = 0;
		monster_pos_y = 0;
		monster_id = 0;
		scene_id = 0;
		task_id = 0;
		find_back_times = 0;
		reserve_sh = 0;
	}

	short play_times;
	short get_reward_times;
	int monster_pos_x;
	int monster_pos_y;
	int monster_id;
	int scene_id;
	int task_id;
	short find_back_times;		// 奖励找回的次数
	short reserve_sh;
};

typedef char TreasureMapParamHex[sizeof(TreasureMapParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(TreasureMapParamHex) < 256);

#pragma pack(pop)
#endif