#ifndef __CONCENTRIC_KNOT_PARAM_HPP__
#define __CONCENTRIC_KNOT_PARAM_HPP__

#include "servercommon/serverdef.h"
#include "servercommon/servercommon.h"

#pragma pack(push, 4)


struct ConcentricKnotParam
{
	ConcentricKnotParam()
	{
		this->Reset();
	}

	void Reset()
	{
		is_get_concentric_knot = 0;
		reward_times = 0;
		memset(re_ch, 0, sizeof(re_ch));

		use_item = 0;
		game_end_times = 0;
	}

	char is_get_concentric_knot;			// 今天是否获得过同心结
	char reward_times;						// 今天是否完成游戏获得过奖励
	char re_ch[2];

	int use_item;
	unsigned int game_end_times;			// 游戏结束时间戳
};

typedef char ConcentricKnotParamHex[sizeof(ConcentricKnotParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(ConcentricKnotParamHex) < 64);

#pragma pack(pop)
#endif
