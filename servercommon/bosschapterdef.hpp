#ifndef __BOSS_CHAPTER_DEF_HPP__
#define __BOSS_CHAPTER_DEF_HPP__

#include "servercommon/servercommon.h"

#pragma pack(push, 4)

enum BOSS_CHAPTER_REWARD_STATUS
{
	BOSS_CHAPTER_REWARD_STATUS_INVALID = 0,		//!< 0 不可领取
	BOSS_CHAPTER_REWARD_STATUS_CLAIMABLE = 1,	//!< 1 可领取
	BOSS_CHAPTER_REWARD_STATUS_CLAIMED = 2,		//!< 2 已领取
};

static const int MAX_BOSS_CHAPTER = 30;
static const int MAX_BOSS_NUM_EACH_CHAPTER = 10;
static const int BOSS_CHAPTER_MAX_BOSS_NUM = MAX_BOSS_CHAPTER * MAX_BOSS_NUM_EACH_CHAPTER;

struct BossChapterInfo
{
	BossChapterInfo() { this->Reset(); }

	void Reset()
	{
		chapter = 0;
		reward_status = BOSS_CHAPTER_REWARD_STATUS_INVALID;
		reserve_ch = 0;
		monster_group_id = 0;
		reserve_ll = 0;
	}

	short chapter;
	char reward_status;
	char reserve_ch;
	int monster_group_id;
	long long reserve_ll;
};

struct BossChapterParam
{
	BossChapterParam() { this->Reset(); }
	void Reset()
	{
		info_num = 0;
		chapter_reward_flag = 0;
		memset(reserve_ll, 0, sizeof(reserve_ll));
		for (int i = 0; i < BOSS_CHAPTER_MAX_BOSS_NUM; ++i)
		{
			info_list[i].Reset();
		}
	}

	int info_num;
	long long chapter_reward_flag;
	long long reserve_ll[3];

	BossChapterInfo info_list[BOSS_CHAPTER_MAX_BOSS_NUM];
};

typedef char BossChapterParamHex[sizeof(BossChapterParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(BossChapterParamHex) < 16384);

#pragma pack(pop)

#endif