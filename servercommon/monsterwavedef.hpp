#ifndef __MONSTER_WAVE_DEF_H__
#define __MONSTER_WAVE_DEF_H__

#include "servercommon/serverdef.h"
#include "servercommon/servercommon.h"

#pragma pack(push, 4)

struct MonsterWaveParam
{
	MonsterWaveParam()
	{
		this->Reset();
	}

	void Reset()
	{
		play_times = 0;
		today_pass_wave = 0;
	}
	

	int play_times;		//今天玩的次数
	int today_pass_wave;
};

typedef char MonsterWaveParamHex[sizeof(MonsterWaveParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(MonsterWaveParamHex) < 128);


#pragma pack(pop)

#endif
