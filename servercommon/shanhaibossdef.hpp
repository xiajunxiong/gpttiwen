#ifndef __SHAN_HAI_BOSS_DEF_H__
#define __SHAN_HAI_BOSS_DEF_H__

#include "servercommon/serverdef.h"
#include "servercommon/servercommon.h"
#include "servercommon/struct/battle/battle_def.hpp"
#include "servercommon/struct/global/drawdef.hpp"

#pragma pack(push, 4)

static const int SHAN_HAI_BOSS_MAXNUM = 50;

static const int SHAN_HAI_BOSS_MAX_GRADE = 5;		//山海降妖五星(弃用)
static const int SHAN_HAI_BOSS_MAX_TYPE = 12;		//山海降妖12个类型(弃用)


/*struct ShanHaiBossPoker
{
	ShanHaiBossPoker() { this->Reset(); }

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

struct TeamShanHaiBossPoker
{
	TeamShanHaiBossPoker() { this->Reset(); }

	void Reset()
	{
		end_timestamp = 0;
		for (int i = 0; i < SIDE_MAX_ROLE_NUM; ++i)
		{
			poker_list[i].Reset();
		}
	}

	unsigned int end_timestamp;
	ShanHaiBossPoker poker_list[SIDE_MAX_ROLE_NUM];
};
*/

struct ShanHaiNpcSaveInfo
{
	ShanHaiNpcSaveInfo() { this->Reset(); }

	void Reset()
	{
		scene_id = 0;
		npc_id = 0;
		npc_pos = Posi(0, 0);
		death_num = 0;
		sh_reserver = 0;
	}	

	int scene_id;
	int npc_id;
	Posi npc_pos;
	short death_num;
	short sh_reserver;
};

struct ShanHaiBossParam
{
	ShanHaiBossParam() { this->Reset(); }

	void Reset()
	{
		next_refresh_timestamp = 0;
		for (int i = 0; i < SHAN_HAI_BOSS_MAXNUM; ++i)
		{
			npc_info[i].Reset();
		}
	}

	unsigned int next_refresh_timestamp;
	ShanHaiNpcSaveInfo npc_info[SHAN_HAI_BOSS_MAXNUM];
};

#pragma pack(pop)
#endif
