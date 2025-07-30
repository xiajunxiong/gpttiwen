#ifndef __MSG_MONSTER_WAVE_H__
#define __MSG_MONSTER_WAVE_H__

#include "servercommon/userprotocal/msgheader.h"


#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{
	class SCMonsterWaveInfo						//!< 2145 ħ����Ϣ
	{
	public:
		SCMonsterWaveInfo();
		MessageHeader		header;

		int play_times;		//!< ��Ĵ���
		int today_pass_wave;
	};

	class SCMonsterWaveFinishInfo						//!< 2146 �����Ϣ
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

