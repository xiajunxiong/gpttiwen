#ifndef __WILD_BOSS_DEF_H__
#define __WILD_BOSS_DEF_H__

#include "servercommon/serverdef.h"
#include "servercommon/servercommon.h"
#include "servercommon/struct/battle/battle_def.hpp"
#include "servercommon/struct/itemlistparam.h"
#include "servercommon/struct/global/drawdef.hpp"

#pragma pack(push, 4)

const static int MAX_WILD_BOSS_NUM = 100;
const static int MAX_PLAY_THIS_MODEL = 150;		// �赱ǰͬʱ����ͬʱ��Ұ��boss�������150

enum WILD_BOSS_STATE
{
	WILD_BOSS_STATE_KILL,
	WILD_BOSS_STATE_EXIST,
	WILD_BOSS_STATE_NEED_CREATE,  ////����ʱ�ὫWILD_BOSS_STATE_EXIST���ó�WILD_BOSS_STATE_NEED_CREATE,����ʱ����
};

enum PokerReason
{
	POKER_REASON_SELF = 0,		// �����������
	POKER_ERASON_SYSTEM = 1,		// ϵͳ��æ����
};

struct WildbossPoker	// �ýṹ�õ����
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

	int index;							// �������Ƶ�index -1 δ��ȡ
	int role_uid;						// UID
	GameName role_name;					// ��ɫ����
	//short res;
	//ItemID reward_id;					// ��õĽ���ID
	//int reward_count;					// ��������
	int luck_coin;						// ����ֵ
	//unsigned int param_len;				// ��ֵ����
	//unsigned int param_len_shengyu;		// ��ֵʣ�೤��
	//NetValueItemParamData param_data;	//  char[256]  ������Ʒ��Ʒ��ֵ����
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

	short pokker_index[SIDE_MAX_ROLE_NUM + 1];				// �ж�����������Ƶ��������ȫ��Ĭ��ѡ��һ���ƣ����ǰ�˳��ѡ
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
