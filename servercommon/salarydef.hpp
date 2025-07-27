#ifndef __SALARY_DEF_HPP__
#define __SALARY_DEF_HPP__

#include "servercommon/servercommon.h"

#pragma pack(push)
#pragma pack(4)

static const int MAX_SALARY_INFO_NUM = 20;
static const int SALARY_MIN_TEAM_MEMBER = 2;

enum SALARY_REWARD_TYPE
{
	SALARY_REWARD_TYPE_WIN_BATTLE = 0,
	SALARY_REWARD_TYPE_CLEAR_FB = 1,
	SALARY_REWARD_TYPE_KILL_TASK_MONSTER_GROUP = 2,
	SALARY_REWARD_TYPE_CLEAR_ONE_FB_LAYER = 3,
	SALARY_REWARD_TYPE_DEMON_CAVE_WAVE = 4,
	SALARY_REWARD_TYPE_KILL_WILD_BOSS = 5,
	SALARY_REWARD_TYPE_KILL_CHALLENGE_TASK_MONSTER_SUCC = 6,
	SALARY_REWARD_TYPE_KILL_BRANCH_TASK_MONSTER_SUCC = 7,
	SALARY_REWARD_TYPE_KILL_ADVANCED_TASK_MONSTER_SUCC = 8,

	SALARY_REWARD_TYPE_MAX
};

enum SALARY_TYPE
{
	SALARY_TYPE_CAPTAIN_SALARY = 0,
	SALARY_TYPE_CHIVAROUS_REWARD = 1,
};

struct SalaryInfo
{
	SalaryInfo() : accumulate_times(0), total_salary(0)
	{
		this->Reset();
	}

	void Reset()
	{
		accumulate_times = 0;
		version = 0;
		reserve_ch = 0;
		total_salary = 0;
	}

	short accumulate_times;
	char version;
	char reserve_ch;	
	int total_salary;
};

struct SalaryParam
{
	SalaryParam()
	{
		this->Reset();
	}

	void Reset()
	{
		memset(reserve_ll, 0, sizeof(reserve_ll));
		memset(reserve_ll2, 0, sizeof(reserve_ll2));
		for (int i = 0; i < MAX_SALARY_INFO_NUM; ++i)
		{
			captain_salary_list[i].Reset();
			chivalrous_reward_list[i].Reset();
		}
		captain_salary = 0;
		chivalrous_salary = 0;
		history_chivalrous = 0;
		history_chivalrous_reward_flag = 0;
	}
	int captain_salary;			//每天队长工资
	int chivalrous_salary;		//每天侠义值
	int history_chivalrous;		//历史侠义值
	int history_chivalrous_reward_flag;	//历史侠义值成就奖励领取标记
	long long reserve_ll[4];
	SalaryInfo captain_salary_list[MAX_SALARY_INFO_NUM];
	
	long long reserve_ll2[5];
	SalaryInfo chivalrous_reward_list[MAX_SALARY_INFO_NUM];
};

typedef char SalaryParamHex[sizeof(SalaryParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(SalaryParamHex) < 1024);

#pragma pack(pop)

#endif