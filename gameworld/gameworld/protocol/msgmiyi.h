#ifndef __MSG_MI_YI_HPP__
#define __MSG_MI_YI_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "gamedef.h"
#include "servercommon/miyidef.hpp"

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{
	class SCMiYiInfo					//!< 3830 密医阴谋信息
	{
	public:
		SCMiYiInfo();
		MessageHeader header;
		
		char pass_info[MI_YI_MAX_LEVEL][MI_YI_MAX_MONSTER_NUM];	//bit 1 是否已经首杀过，bit2 这个星期是否已经通关过
		unsigned short fetch_flag[MI_YI_MAX_LEVEL];				//	每周首杀奖励奖励领取标志
	};

	struct SCMiYiPoker			//!< 3831 密医阴谋周常奖励翻牌下发 
	{
		SCMiYiPoker();
		MessageHeader header;

		int draw_group_id;
		unsigned int timeout;
		MiYiPoker mypoker[SIDE_MAX_ROLE_NUM];
	};

	struct CSMiYiPokerReq				//!< 3832 密医阴谋翻牌请求  [弃用，改为 2260]
	{
		CSMiYiPokerReq();
		MessageHeader header;

		int role_uid;
		int index;			//[1,5]
	};

	class SCMiYiPokerInfo			//!< 3833 密医阴谋信息下发
	{
	public:
		SCMiYiPokerInfo();
		MessageHeader header;

		int weekly_pass_count;				//本周通关的次数
		int day_pass_count;
	};

	class SCMiYiSceneInfo			//!< 3834 密医阴谋场景信息下发
	{
	public:
		SCMiYiSceneInfo();
		MessageHeader header;

		unsigned int kill_flag;		//场景中怪物击杀标识(二进制) 0:未击杀  1:已击杀
	};

	class SCMiYiKillPassInfo //!< 3835 通知客户端此次通关是不是首通信息
	{
	public:
		SCMiYiKillPassInfo();
		MessageHeader header;

		unsigned char is_first_kill;
		unsigned char is_week_kill;
		unsigned short day_pass_count;
	};


	enum MI_YI_REQ_TYPE
	{
		MI_YI_REQ_TYPE_FETCH_WEEK_KILL_REWARD,		//领取周首杀奖励  p1 配置scene_id p2 配置 npc_id
		MI_YI_REQ_TYPE_SAO_DANG,		//扫荡  p1 配置scene_id 
	};

	struct CSMiYiReq				//!< 3836 密医阴谋请求
	{
		CSMiYiReq();
		MessageHeader header;

		int req_type;
		int param1;
		int param2;
	};
}

#pragma pack(pop)

#endif

