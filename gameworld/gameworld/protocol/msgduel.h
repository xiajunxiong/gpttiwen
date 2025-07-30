#ifndef __MSG_DUEL_H__
#define __MSG_DUEL_H__

#include "servercommon/dueldef.h"
#include "servercommon/userprotocal/msgheader.h"

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{
	class SCDuelBattleInfo						//!<4100 决斗战斗信息信息
	{
	public:
		SCDuelBattleInfo();
		MessageHeader header;

		int duel_type;
		GameName att_name;
		int att_list[MAX_TEAM_MEMBER_NUM];		//MAX_TEAM_MEMBER_NUM - 5
		GameName def_name;
		int def_list[MAX_TEAM_MEMBER_NUM];		//MAX_TEAM_MEMBER_NUM - 5
	};


	class SCRoleDuelKillValueAOINotice			//!<4101 杀戮值改变通知周边玩家
	{
	public:
		SCRoleDuelKillValueAOINotice();
		MessageHeader header;

		ObjID obj_id;
		short reserve_sh;
		int kill_value;							//杀戮值
	};
}

#pragma pack(pop)
#endif



