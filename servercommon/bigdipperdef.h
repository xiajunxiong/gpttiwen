#ifndef __BIG_DIPPER_DEF_H__
#define __BIG_DIPPER_DEF_H__

#include "servercommon/bitmapfunction.hpp"
#include "servercommon/activitydef.hpp"

#pragma pack(push, 4)
const static int MAX_BIG_DIPPER_MONSTER_REWARD_INDEX = 3;//阶段奖励上限
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
	char first_pass_flag;	//是否首通
	char first_reward_flag;	//首通奖励是否领取
	char pass_flag;			//阶段通关（二进制）
	char pass_reward_flag;	//阶段通关奖励领取（二进制）
	int pass_round_num;		//通关回合数（只有通关3阶段才算回合数）
};
UNSTD_STATIC_CHECK((int)sizeof(char) * 8 > MAX_BIG_DIPPER_MONSTER_REWARD_INDEX);

const static int MAX_BIG_DIPPER_MONSTER_NUM = 8;//最多boss数量
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

	BigDipperMonster monster_list[MAX_BIG_DIPPER_MONSTER_NUM];//星君信息
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

	short help_list[MAX_BIG_DIPPER_MONSTER_NUM];		//每个星君助战次数信息
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
	int monster_list[MAX_BIG_DIPPER_MONSTER_NUM];//开启怪物seq
};


//------------------------------------------殒神之地
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
	BigDipperMonster monster_list[MAX_FALLEN_GOD_MONSTER_NUM];//殒神信息--结构和北斗星君一致
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

	short help_list[MAX_FALLEN_GOD_MONSTER_NUM];		//殒神助战信息--结构和北斗星君一致
};

typedef char FallenGodParamHex[sizeof(FallenGodParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(FallenGodParamHex) < 256);

typedef char FallenGodHelpParamHex[sizeof(FallenGodHelpParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(FallenGodHelpParamHex) < 64);

#pragma pack(pop)
#endif