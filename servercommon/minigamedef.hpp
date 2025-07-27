#ifndef __MINI_GAME_DEF_HPP__
#define __MINI_GAME_DEF_HPP__

#include "servercommon/bitmapfunction.hpp"

enum MINI_GAME_TYPE
{
	MINI_GAME_NONE = 0,
	MINI_GAME_TOWER = 1,		// ËþÂ¥
	MINI_GAME_NIU_LANG_ZHI_NV = 2, // Å£ÀÉÖ¯Å®
	MINI_GAME_MAX = 3,
};

static const int MINI_GAME_PHASE_REWARD_MAX_SEQ = 32;

#pragma pack(push)
#pragma pack(4)

struct MiniGameParam
{
	MiniGameParam()
	{
		this->Reset();
	}

	void Reset()
	{
		clear_stage_num = 0;
		has_init_for_the_first_time = 0;
		reserve_ch = 0;
		phase_reward_flag.Reset();
		memset(reserve_ll, 0, sizeof(reserve_ll));
	}

	short clear_stage_num;
	char has_init_for_the_first_time;
	char reserve_ch;
	long long reserve_ll[4];

	BitMap<MINI_GAME_PHASE_REWARD_MAX_SEQ> phase_reward_flag;
};
typedef char MiniGameParamHex[sizeof(MiniGameParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(MiniGameParamHex) < 128);

#pragma pack(pop)

#endif