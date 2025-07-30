#ifndef __MSG_RA_YUAN_QI_SPEED_HPP__
#define __MSG_RA_YUAN_QI_SPEED_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/rayuanqispeedparam.hpp"


#pragma pack(push, 4)

namespace Protocol
{

	class SCRAYuanQiSpeedInfo				//!< 3148 ����-Ԫ������ ��Ϣ�·�
	{
	public:
		SCRAYuanQiSpeedInfo();
		MessageHeader header;

		int today_play_times;
		BitMap<RA_YUAN_QI_SPEED_PASS_FLAG> pass_flag;//ͨ�ؼ�¼
		BitMap<RA_YUAN_QI_SPEED_PASS_FLAG> reward_flag;//������¼
	};

	class SCRAYuanQiSpeedPlayInfo				//!< 3149 ����-Ԫ������ ��Ϸ״̬ ��Ϣ�·�
	{
	public:
		SCRAYuanQiSpeedPlayInfo();
		MessageHeader header;
		enum PLAY_TYPE
		{
			PLAY_TYPE_LOSE = 0,// ��
			PLAY_TYPE_WIN = 1,// Ӯ
			PLAY_TYPE_PLAYING = 2,//��Ϸ��
		};
		int game_type;

		int play_end_tamp;
		int play_seq;
	};
}


#pragma pack(pop)

#endif
