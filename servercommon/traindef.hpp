#ifndef __TRAIN_DEF_HPP__
#define __TRAIN_DEF_HPP__

#include "servercommon/servercommon.h"

#pragma pack(push, 4)

enum TRAIN_TYPE
{
	TRAIN_BEGIN = 0 ,
	TRAIN_ATTACK = TRAIN_BEGIN,
	TRAIN_MENTAL = 1,
	TRAIN_PHYSICAL_DEC = 2,
	TRAIN_MAGIC_DEC = 3,
	TRAIN_MAX_HP = 4,
	TRAIN_MAX_MP = 5,
	TRAIN_MAIN_ATTR = 6,		// ����������
	TRAN_DEFENSE = 7,			// ��������
	TRAN_AGILITY = 8,			// �ٶ�����
	TRAN_MAGIC_DEFENSE = 9,		// ��������
	TRAIN_ENG,
};

static const int TARIN_TYPE_MAX = 10;
static const int TRAIN_TYPE_RESERVE_MAX = 6;
UNSTD_STATIC_CHECK(TRAIN_ENG <= TARIN_TYPE_MAX);
UNSTD_STATIC_CHECK(TARIN_TYPE_MAX + TRAIN_TYPE_RESERVE_MAX <= sizeof(short) * BIT_COUNT_OF_BYTE);

enum TRAIN_CONSUME_TYPE
{
	TRAIN_CONSUME_TYPE_BIND_COIN = 0,
	TRAIN_CONSUME_TYPE_ITEM = 1,
};

static const int TRAIN_TYPE_PET = 1000;

struct TrainData
{
	TrainData() { this->Reset(); }
	void Reset()
	{
		level= 0;
		reserve_ush = 0;
		exp = 0;
	}

	unsigned short		level;
	unsigned short		reserve_ush;
	int					exp;
};

struct TrainParam
{
	TrainParam() { this->Reset(); }
	void Reset()
	{
		pet_train.Reset();
		for (int i = 0; i < TARIN_TYPE_MAX; ++i)
		{
			train_list[i].Reset();
		}
		for (int i = 0; i < TRAIN_TYPE_RESERVE_MAX; i++)
		{
			reserve_list[i].Reset();
		}
		pet_train_red_dot_flag = 1;
		train_red_dot_flag |= ~0;
		reserve_sh = 0;
		reserve_ch = 0;
		is_set_flag = 0;
	}

	TrainData pet_train;
	TrainData train_list[TARIN_TYPE_MAX];
	TrainData reserve_list[TRAIN_TYPE_RESERVE_MAX];		//��ֹ�Ժ���չ,Ԥ��6��,�������ʶλ������Ӧ

	short pet_train_red_dot_flag;						//0 ����ʾ 1����Ҫ��ʾ
	unsigned short train_red_dot_flag;					//����ʶ(λ����) 0 ����ʾ 1����Ҫ��ʾ
	short reserve_sh;
	char reserve_ch;
	char is_set_flag;									//1: ������
};
typedef char TrainParamHex[sizeof(TrainParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(TrainParamHex) < 4096);

#pragma pack(pop)
#endif // !__COLLECTION_DEF_HPP__