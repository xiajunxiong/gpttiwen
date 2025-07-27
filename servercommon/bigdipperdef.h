#ifndef __BIG_DIPPER_DEF_H__
#define __BIG_DIPPER_DEF_H__

#include "servercommon/bitmapfunction.hpp"
#include "servercommon/activitydef.hpp"

#pragma pack(push, 4)
const static int MAX_BIG_DIPPER_MONSTER_REWARD_INDEX = 3;//�׶ν�������
struct BigDipperMonster
{
	BigDipperMonster() { this->Reset(); }
	void Reset()
	{
		first_pass_flag = 0;
		first_reward_flag = 0;
		pass_flag = 0;
		pass_reward_flag = 0;
		pass_round_num = 0;
	}
	void WeekReset()
	{
		pass_flag = 0;
		pass_reward_flag = 0;
	}
	char first_pass_flag;	//�Ƿ���ͨ
	char first_reward_flag;	//��ͨ�����Ƿ���ȡ
	char pass_flag;			//�׶�ͨ�أ������ƣ�
	char pass_reward_flag;	//�׶�ͨ�ؽ�����ȡ�������ƣ�
	int pass_round_num;		//ͨ�ػغ�����ֻ��ͨ��3�׶β���غ�����
};
UNSTD_STATIC_CHECK((int)sizeof(char) * 8 > MAX_BIG_DIPPER_MONSTER_REWARD_INDEX);

const static int MAX_BIG_DIPPER_MONSTER_NUM = 8;//���boss����
struct BigDipperParam
{
	/*************** **********************/
	BigDipperParam() { this->Reset(); }

	void Reset()
	{
		for (int i = 0; i < MAX_BIG_DIPPER_MONSTER_NUM; i++)
		{
			monster_list[i].Reset();
		}
	}

	void WeekReset()
	{
		for (int i = 0; i < MAX_BIG_DIPPER_MONSTER_NUM; i++)
		{
			monster_list[i].WeekReset();
		}
	}

	BigDipperMonster monster_list[MAX_BIG_DIPPER_MONSTER_NUM];//�Ǿ���Ϣ
};

struct BigDipperHelpParam
{
	/*************** **********************/
	BigDipperHelpParam() { this->Reset(); }

	void Reset()
	{
		memset(help_list, 0, sizeof(help_list));
	}

	void WeekReset()
	{
		memset(help_list, 0, sizeof(help_list));
	}

	short help_list[MAX_BIG_DIPPER_MONSTER_NUM];		//ÿ���Ǿ���ս������Ϣ
};

typedef char BigDipperParamHex[sizeof(BigDipperParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(BigDipperParamHex) < 256);

typedef char BigDipperHelpParamHex[sizeof(BigDipperHelpParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(BigDipperHelpParamHex) < 64);

struct BigDipperMonsterParam
{
	void Reset()
	{
		week_refresh_tamp = 0;
		open_monster_num = 0;
		memset(monster_list, -1, sizeof(monster_list));
	}

	bool HasMonsterSn(int sn)
	{
		for (int i = 0; i < MAX_BIG_DIPPER_MONSTER_NUM && i < open_monster_num; i++)
		{
			if (monster_list[i] == sn)
			{
				return true;
			}
		}
		return false;
	}
	unsigned int week_refresh_tamp;
	int open_monster_num;
	int monster_list[MAX_BIG_DIPPER_MONSTER_NUM];//��������seq
};


//------------------------------------------����֮��
const static int MAX_FALLEN_GOD_MONSTER_NUM = 9;
struct FallenGodParam
{
	/*************** **********************/
	FallenGodParam() { this->Reset(); }

	void Reset()
	{
		top_sn = 0;
		for (int i = 0; i < MAX_BIG_DIPPER_MONSTER_NUM; i++)
		{
			monster_list[i].Reset();
		}
	}

	void WeekReset()
	{
		top_sn = 0;
		for (int i = 0; i < MAX_BIG_DIPPER_MONSTER_NUM; i++)
		{
			monster_list[i].WeekReset();
		}
	}
	int top_sn;
	BigDipperMonster monster_list[MAX_FALLEN_GOD_MONSTER_NUM];//������Ϣ--�ṹ�ͱ����Ǿ�һ��
};

struct FallenGodHelpParam
{
	/*************** **********************/
	FallenGodHelpParam() { this->Reset(); }

	void Reset()
	{
		memset(help_list, 0, sizeof(help_list));
	}

	void WeekReset()
	{
		memset(help_list, 0, sizeof(help_list));
	}

	short help_list[MAX_FALLEN_GOD_MONSTER_NUM];		//������ս��Ϣ--�ṹ�ͱ����Ǿ�һ��
};

typedef char FallenGodParamHex[sizeof(FallenGodParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(FallenGodParamHex) < 256);

typedef char FallenGodHelpParamHex[sizeof(FallenGodHelpParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(FallenGodHelpParamHex) < 64);

#pragma pack(pop)
#endif