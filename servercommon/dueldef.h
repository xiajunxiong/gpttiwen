#ifndef __DUEL_DEF_H__
#define __DUEL_DEF_H__

#include "servercommon/serverdef.h"
#include "servercommon/servercommon.h"

#pragma pack(push, 4)

enum DUEL_BATTLE_TYPE
{
	DUEL_BATTLE_TYPE_BEGIN = 0,

	DUEL_BATTLE_TYPE_DUEL = 1,					// 决斗（红、白打白名）
	DUEL_BATTLE_TYPE_ADJUDICATION = 2,			// 裁决 (红、白打红名)
	DUEL_BATTLE_TYPE_ESCORT = 3,				// 劫镖（红、白打运镖玩家（运镖玩家无论红白，都视为白名玩家））

	DUEL_BATTLE_TYPE_END,
};

struct DuelParam
{
	DuelParam()
	{
		this->Reset();
	}

	void Reset()
	{
		kill_value = 0;
		use_reduce_item_num = 0;
		kill_title_id = 0;
		kill_titel_stage = -1;

		last_reduce_time = 0;

		escort_task_seq = 0;
		escort_npc_id = 0;
	}

	int kill_value;							// 杀戮值(罪恶值)
	int use_reduce_item_num;				// 使用减少杀戮道具次数
	int kill_title_id;						// 当前佩戴的杀戮称号
	int kill_titel_stage;					// 当前佩戴的杀戮称号阶段（有可能称号ID相同，阶段不同，但要降属性）

	unsigned int last_reduce_time;

	//劫镖
	int escort_task_seq;
	int escort_npc_id;
};

typedef char DuelParamHex[sizeof(DuelParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(DuelParamHex) < 256);

#pragma pack(pop)
#endif
