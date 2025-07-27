#ifndef __SCHOOL_EXERCISE_DEF_HPP__
#define __SCHOOL_EXERCISE_DEF_HPP__

#include "servercommon/servercommon.h"

static const int MAX_SCHOOL_EXERCISE_NUM = 60;

#pragma pack(push)
#pragma pack(4)

struct SchoolExerciseParam
{
	SchoolExerciseParam()
	{
		this->Reset();
	}

	void Reset()
	{
		memset(reserve_ll, 0, sizeof(reserve_ll));
		clear_flag = 0LL;
		has_open_ui_ever = 0;
		reserve_ch = 0;
		reserve_sh = 0;
		reserve_int = 0;
	}

	bool IsClearSeq(int seq)
	{
		return 0 != (clear_flag & (1LL << seq));
	}

	void SetSeqClear(int seq)
	{
		clear_flag |= (1LL << seq);
	}

	long long clear_flag;
	char has_open_ui_ever;
	char reserve_ch;
	short reserve_sh;
	int reserve_int;
	long long reserve_ll[3];
};

typedef char SchoolExerciseParamHex[sizeof(SchoolExerciseParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(SchoolExerciseParamHex) < 128);

#pragma pack(pop)

#endif