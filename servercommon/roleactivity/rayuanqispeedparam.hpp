#ifndef __RA_YUAN_QI_SPEED_PARAM_HPP__
#define __RA_YUAN_QI_SPEED_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"
#include "servercommon/bitmapfunction.hpp"

#pragma pack(push, 4)

const int RA_YUAN_QI_SPEED_PASS_FLAG = 32;//通关seq

struct RAYuanQiSpeedParam
{
	RAYuanQiSpeedParam() { this->Reset(); }

	void Reset()
	{
		ra_begin_timestamp = 0;
		play_end_tamp = 0;
		play_seq = 0;
		times = 0;
		pass_flag.Reset();
		reward_flag.Reset();
	}

	void DayReset()
	{
		times = 0;
		play_seq = 0;
		play_end_tamp = 0;
	}

	void PlayEnd()
	{
		play_seq = 0;
		play_end_tamp = 0;
	}

	unsigned int ra_begin_timestamp;
	unsigned int play_end_tamp;
	int play_seq;
	int times;
	BitMap<RA_YUAN_QI_SPEED_PASS_FLAG> pass_flag;//通关记录
	BitMap<RA_YUAN_QI_SPEED_PASS_FLAG> reward_flag;//奖励记录
};
UNSTD_STATIC_CHECK(sizeof(RAYuanQiSpeedParam) <= sizeof(RoleActivityRoleData));

#pragma pack(pop)

#endif