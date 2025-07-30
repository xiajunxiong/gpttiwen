#ifndef __MSG_RA_ANIMAL_RACE_HPP__
#define __MSG_RA_ANIMAL_RACE_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/rahappyupparam.hpp"

//////////////////////////////////////////// 2061  随机活动  神兽径赛  //////////////////////////////////////////

#pragma pack(push, 4)

namespace Protocol
{
	// 由协议  3000
	enum RA_ANIMAL_RACE_OPERA_TYPE
	{
		RA_ANIMAL_RACE_OPERA_TYPE_ROLE_INFO = 0,		//神兽径赛玩家信息下发	
		RA_ANIMAL_RACE_OPERA_TYPE_BET = 1,				//玩家下注 param_1 - 神兽序号[1,4] param_2 - 下注的数量
		RA_ANIMAL_RACE_OPERA_TYPE_ACTIVITY_INFO = 2,	//神兽径赛场景活动信息下发
		RA_ANIMAL_RACE_OPERA_TYPE_ENTER_SCENE = 3,		//玩家进入场景
		RA_ANIMAL_RACE_OPERA_TYPE_EXIT_SCENE = 4,		//玩家退出场景
		RA_ANIMAL_RACE_OPERA_TYPE_ANIMATION_INFO = 5,	//下注时间结束,准备开始播放动画
	};

	struct SCRAAnimalRaceInfo								// 3018 神兽径赛场景信息下发
	{
	public:
		SCRAAnimalRaceInfo();
		MessageHeader header;

		enum ANIMAL_RACE_INFO_TYPE
		{
			ANIMAL_RACE_INFO_TYPE_ROLE_READY = 0,			//玩家信息下发    timestamp - 准备时间: 押注时间结束的时间戳 |  不需要获胜神兽序号
			ANIMAL_RACE_INFO_TYPE_ROLE_DOING = 1,			//玩家信息下发    timestamp - 比赛中:  距离下一次押注时间的时间戳 | 需要获胜神兽序号
			ANIMAL_RACE_INFO_TYPE_NOT_TIME = 2,				//玩家信息下发    timestamp - 空闲状态 下次押注时间开始的时候 | 不需要获胜神兽序号
		};

		int op_type;
		long long  timestamp;
		char rank[MAX_ANIMAL_RACE_NUM];								//本轮获胜的神兽序号 [1,4]
		int count;													//根据这个数量接收
		role_info scene_appearance[MAX_ANIMAL_RACE_SC_NUM];			//场景玩家形象 MAX_ANIMAL_RACE_SC_NUM - 20
	};

	struct SCRAAnimalRaceRoleInfo								// 3019 神兽径赛玩家信息下发
	{
	public:
		SCRAAnimalRaceRoleInfo();
		MessageHeader header;

		short bet_type_count[MAX_ANIMAL_RACE_NUM];					//玩家个人对于每个神兽的押注情况
		int everything_bet_type_count[MAX_ANIMAL_RACE_NUM];			//全服此时每个神兽的押注情况
		int type_response_rate[MAX_ANIMAL_RACE_NUM];				//此时每个神兽的回报率
	};

	struct SCRAAnimalRaceAnimationInfo		//!<　3037 神兽径赛动画数据下发
	{
	public:
		SCRAAnimalRaceAnimationInfo();
		MessageHeader header;

		AnimalRaceData animation_data;				//动画数据
	};
}

#pragma pack(pop)

#endif  // __MSG_RA_ANIMAL_RACE_HPP__


