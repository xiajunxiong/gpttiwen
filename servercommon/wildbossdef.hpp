#ifndef __WILD_BOSS_DEF_H__
#define __WILD_BOSS_DEF_H__

#include "servercommon/serverdef.h"
#include "servercommon/servercommon.h"
#include "servercommon/struct/battle/battle_def.hpp"
#include "servercommon/struct/itemlistparam.h"
#include "servercommon/struct/global/drawdef.hpp"

#pragma pack(push, 4)

const static int MAX_WILD_BOSS_NUM = 100;
const static int MAX_PLAY_THIS_MODEL = 150;		// 设当前同时正在同时打野外boss的玩家有150

enum WILD_BOSS_STATE
{
	WILD_BOSS_STATE_KILL,
	WILD_BOSS_STATE_EXIST,
	WILD_BOSS_STATE_NEED_CREATE,  ////保存时会将WILD_BOSS_STATE_EXIST设置成WILD_BOSS_STATE_NEED_CREATE,开服时创建
};

enum PokerReason
{
	POKER_REASON_SELF = 0,		// 玩家主动翻牌
	POKER_ERASON_SYSTEM = 1,		// 系统帮忙翻牌
};

struct WildbossPoker	// 该结构用到库存
{
	WildbossPoker() { this->Reset(); }

	bool Serialize(TLVSerializer2 & outstream) const;
	void Reset()
	{
		index = -1;
		role_uid = 0;
		memset(role_name, '\0', sizeof(role_name));
		luck_coin = 0;
		reward_info.Reset();
	}

	int index;							// 翻到的牌的index -1 未领取
	int role_uid;						// UID
	GameName role_name;					// 角色名字
	//short res;
	//ItemID reward_id;					// 获得的奖励ID
	//int reward_count;					// 奖励数量
	int luck_coin;						// 气运值
	//unsigned int param_len;				// 净值长度
	//unsigned int param_len_shengyu;		// 净值剩余长度
	//NetValueItemParamData param_data;	//  char[256]  翻牌物品物品净值参数
	ItemDrawInfo reward_info;
};

struct TeamWildBossPokerInfo
{
	TeamWildBossPokerInfo()
	{
		this->Reset();
	}

	void Reset()
	{
		for (int i = 0; i < SIDE_MAX_ROLE_NUM; ++i)
		{
			once_info[i].Reset();
		}

		flip_poker_end_time = 0u;
	}

	unsigned int flip_poker_end_time;
	WildbossPoker once_info[SIDE_MAX_ROLE_NUM];
};

struct OfflineList
{
	OfflineList()
	{
		this->Reset();
	}

	void Reset()
	{
		team_id = 0;
		role_id = 0;
	}

	int team_id;
	int role_id;
};

struct PokerIndex
{
	PokerIndex()
	{
		this->Reset();
	}

	void Reset()
	{
		memset(pokker_index, 0, sizeof(pokker_index));
		pokker_index[0] = -1;
	}

	short pokker_index[SIDE_MAX_ROLE_NUM + 1];				// 判断如果都不翻牌的情况避免全部默认选第一张牌，而是按顺序选
};

struct WildBossParam
{
	WildBossParam()
	{
		this->Reset();
	}

	void Reset()
	{
		memset(monster_state, 0, sizeof(monster_state));
		memset(next_refresh_time, 0, sizeof(next_refresh_time));

	}

	unsigned char monster_state[MAX_WILD_BOSS_NUM];
	unsigned int next_refresh_time[MAX_WILD_BOSS_NUM];

};


#pragma pack(pop)

#endif
