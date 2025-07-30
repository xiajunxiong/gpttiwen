#ifndef __MSG_MONSTER_WAVE_H__
#define __MSG_MONSTER_WAVE_H__

#include "servercommon/userprotocal/msgheader.h"


#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{
	class SCMonsterWaveInfo						//!< 2145 魔窟信息
	{
	public:
		SCMonsterWaveInfo();
		MessageHeader		header;

		int play_times;		//!< 玩的次数
		int today_pass_wave;
	};

	class SCMonsterWaveFinishInfo						//!< 2146 完成信息
	{
	public:
		SCMonsterWaveFinishInfo();
		MessageHeader		header;

		int role_level;
		int pass_wave;
	};
}

#pragma pack(pop)

#endif

