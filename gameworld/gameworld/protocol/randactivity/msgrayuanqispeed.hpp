#ifndef __MSG_RA_YUAN_QI_SPEED_HPP__
#define __MSG_RA_YUAN_QI_SPEED_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/rayuanqispeedparam.hpp"


#pragma pack(push, 4)

namespace Protocol
{

	class SCRAYuanQiSpeedInfo				//!< 3148 随机活动-元气竞速 信息下发
	{
	public:
		SCRAYuanQiSpeedInfo();
		MessageHeader header;

		int today_play_times;
		BitMap<RA_YUAN_QI_SPEED_PASS_FLAG> pass_flag;//通关记录
		BitMap<RA_YUAN_QI_SPEED_PASS_FLAG> reward_flag;//奖励记录
	};

	class SCRAYuanQiSpeedPlayInfo				//!< 3149 随机活动-元气竞速 游戏状态 信息下发
	{
	public:
		SCRAYuanQiSpeedPlayInfo();
		MessageHeader header;
		enum PLAY_TYPE
		{
			PLAY_TYPE_LOSE = 0,// 输
			PLAY_TYPE_WIN = 1,// 赢
			PLAY_TYPE_PLAYING = 2,//游戏中
		};
		int game_type;

		int play_end_tamp;
		int play_seq;
	};
}


#pragma pack(pop)

#endif
