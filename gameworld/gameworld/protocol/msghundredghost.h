#ifndef __MSG_HUNDRED_GHOST_H__
#define __MSG_HUNDRED_GHOST_H__

#include "servercommon/activitydef.hpp"
#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/struct/global/drawdef.hpp"
#include "servercommon/struct/battle/battle_def.hpp"

static const int HUNDRED_GHOST_BOSS_MAXNUM = 50;

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{
	struct CSHundredGhostReq					// 3940 百鬼夜行npc信息请求
	{
		CSHundredGhostReq();
		MessageHeader header;

		enum REQ_TYPE
		{
			REQ_TYPE_BOSS_INFO = 0,			//请求BOSS信息
		};

		short req_type;
		short param;
	};

	class SCHundredGhostInfo					//!<3941 百鬼夜行npc信息
	{
	public:
		SCHundredGhostInfo();
		MessageHeader header;

		struct NpcInfo
		{
			int npc_id;
			int scene_id;
		};

		short surplus_num;			//剩余boss数量
		short count;
		NpcInfo boss_info[HUNDRED_GHOST_BOSS_MAXNUM];
	};

	struct HundredGhostPoker
	{
		HundredGhostPoker() { this->Reset(); }

		void Reset()
		{
			index = -1;
			role_uid = 0;
			memset(role_name, '\0', sizeof(role_name));
			reward_info.Reset();
		}

		int index;							// 翻到的牌的index
		int role_uid;						// UID
		GameName role_name;					// 角色名字

		ItemDrawInfo reward_info;
	};
	struct SCHundredGhostPokerInfo			// 3942	百鬼夜行翻牌奖励信息
	{
		SCHundredGhostPokerInfo();
		MessageHeader header;

		int draw_group_id;
		unsigned timeout;
		HundredGhostPoker poker_data[SIDE_MAX_ROLE_NUM];		//SIDE_MAX_ROLE_NUM - 5
	};
}

#pragma pack(pop)
#endif


