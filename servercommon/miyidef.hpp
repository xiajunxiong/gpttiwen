#ifndef __MI_YI_DEF_H__
#define __MI_YI_DEF_H__

#include "servercommon/serverdef.h"
#include "servercommon/servercommon.h"
#include "servercommon/struct/battle/battle_def.hpp"
#include "servercommon/struct/itemlistparam.h"
#include "servercommon/struct/global/drawdef.hpp"

#pragma pack(push, 4)

const int static MI_YI_MAX_MONSTER_NUM = 10;		//��ҽ����������
const int static MI_YI_MAX_LEVEL = 10;		//��ҽ
const int static MI_YI_MAX_REWARD_PROPORTION_NUM = 3;
enum MI_YI_PASS_BIT_INFO
{
	MI_YI_PASS_BIT_INFO_FIRST_PASS,			//���û�ɱ
	MI_YI_PASS_BIT_INFO_PASS,				//�ܻ�ɱ
};
struct MiYiPokerFixedReward		// ���ƹ̶�����
{
	MiYiPokerFixedReward()
	{
		this->Reset();
	}

	void Reset()
	{
		reward_id = 0;
		num = 0;
		is_bind = 0;
	}

	int reward_id;
	int num;
	int is_bind;
};

struct MiYiPoker     // ����֮�� ����  �ýṹ�õ����
{
	MiYiPoker()
	{
		this->Reset();
	}
	bool Serialize(TLVSerializer2 & outstream) const;
	void Reset()
	{
		index = -1;
		role_uid = 0;
		memset(role_name, '\0', sizeof(role_name));
		fixed_reward.Reset();
		poker_info.Reset();
	}

	int index;						// �������Ƶ�index
	int role_uid;					// UID
	GameName role_name;				// ��ɫ����
	MiYiPokerFixedReward fixed_reward;	// �̶�����
	ItemDrawInfo poker_info;		//���ƽ���
};

struct MiYiParam
{
	MiYiParam()
	{
		this->Reset();
	}

	void Reset()
	{
		memset(pass_info, 0, sizeof(pass_info));
		ArrayFillZero(discard);
		weekly_pass_count = 0;
		day_pass_count = 0;

		for (int i = 0; i < ARRAY_LENGTH(fetch_flag); ++i)
		{
			fetch_flag[i] = 0;
		}
	}

	unsigned char pass_info[MI_YI_MAX_LEVEL][MI_YI_MAX_MONSTER_NUM];		//bit 1 �Ƿ��Ѿ���ɱ����bit2 ��������Ƿ��Ѿ�ͨ�ع�
	unsigned char discard[628];
	int weekly_pass_count;			//ÿ��ͨ�ش���(ÿ����ɱ������)
	int day_pass_count;
	unsigned short fetch_flag[MI_YI_MAX_LEVEL];				//	ÿ����ɱ����������ȡ��־
};

UNSTD_STATIC_CHECK(MI_YI_MAX_MONSTER_NUM < (int)sizeof(((MiYiParam *)0)->fetch_flag[0]) * 8);

typedef char MiYiParamHex[sizeof(MiYiParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(MiYiParamHex) < 1800);


#pragma pack(pop)

#endif
