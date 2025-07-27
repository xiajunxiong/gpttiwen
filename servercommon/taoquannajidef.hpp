#ifndef __TAO_QUAN_NA_JI_DEF_HPP__
#define __TAO_QUAN_NA_JI_DEF_HPP__

#include <string.h>
#include "servercommon/servercommon.h"

#pragma pack(push, 4)

const int TAO_QUAN_NA_JI_X_NUM = 6;//套圈纳吉行
const int TAO_QUAN_NA_JI_Y_NUM = 5;//套圈纳吉列

struct TaoQuanNaJiParam
{
	TaoQuanNaJiParam() { this->Reset(); }

	void Reset()
	{
		map_init = 0;
		memset(play_map, 0, sizeof(play_map));
		sroce = 0;
		item_1_count = 0;
		item_2_count = 0;
		item_3_count = 0;
		item_4_count = 0;
		can_play_times = 0;
		today_play_times = 0;
		flag_title = 0;
	}

	void DayReset()
	{
		map_init = 0;
		memset(play_map, 0, sizeof(play_map));
		sroce = 0;
		item_1_count = 0;
		item_2_count = 0;
		item_3_count = 0;
		item_4_count = 0;
		can_play_times = 0;
		today_play_times = 0;
	}

	void ResetEndGame()
	{
		map_init = 0;
		memset(play_map, 0, sizeof(play_map));
		sroce = 0;
		item_1_count = 0;
		item_2_count = 0;
		item_3_count = 0;
		item_4_count = 0;
		can_play_times = 0;
	}
	int map_init;
	int play_map[TAO_QUAN_NA_JI_X_NUM][TAO_QUAN_NA_JI_Y_NUM];//地图
	int sroce;
	int item_1_count;
	int item_2_count;
	int item_3_count;
	int item_4_count;
	int can_play_times;//可套圈次数
	int today_play_times;//今天玩耍次数
	int flag_title;
};

typedef char TaoQuanNaJiParamHex[sizeof(TaoQuanNaJiParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(TaoQuanNaJiParamHex) < 512);

#pragma pack(pop)

#endif  // __MENTALITY_DEF_HPP__

