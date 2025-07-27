#ifndef __ROLE_GUILD_INBORN_DEF_HPP__
#define __ROLE_GUILD_INBORN_DEF_HPP__

#include "servercommon/servercommon.h"

#pragma pack(push, 4)

static const int MAX_GUILD_INBORN_SKILL_TYPE_NUM = 8;							//家族灵脉技能类别数量
static const int MAX_GUILD_INBORN_SKILL_TYPE_IN_HAS_SKILL_NUM = 8;				//一个灵脉技能类别中最多拥有几个技能
static const int NAX_GUILD_INBORN_SKILL_LEVEL_NUM = 10;							//家族灵脉技能等级上限
	
static const int MAX_GUILD_INBORN_SKILL_TYPE_CUR_NUM = 6;						//家族灵脉技能类别数量(当前)
static const int MAX_GUILD_INBORN_SKILL_TYPE_IN_HAS_SKILL_CUR_NUM = 6;			//一个灵脉技能类别中最多拥有几个技能(当前)	改这个数量需要

static const int MAX_GUILD_INBORN_SKILL_NUM = MAX_GUILD_INBORN_SKILL_TYPE_NUM * MAX_GUILD_INBORN_SKILL_TYPE_IN_HAS_SKILL_NUM;				//家族灵脉技能数量
static const int MAX_GUILD_INBORN_SKILL_CUR_NUM = MAX_GUILD_INBORN_SKILL_TYPE_CUR_NUM * MAX_GUILD_INBORN_SKILL_TYPE_IN_HAS_SKILL_CUR_NUM;	//家族灵脉当前技能数量

UNSTD_STATIC_CHECK(MAX_GUILD_INBORN_SKILL_TYPE_CUR_NUM <= MAX_GUILD_INBORN_SKILL_TYPE_NUM);
UNSTD_STATIC_CHECK(MAX_GUILD_INBORN_SKILL_TYPE_IN_HAS_SKILL_CUR_NUM <= MAX_GUILD_INBORN_SKILL_TYPE_IN_HAS_SKILL_NUM);

//一个类别中,可能会指向多个目标类型
enum GUILD_INBORN_SKILL_TYPE
{
	GUILD_INBORN_SKILL_TYPE_PET = 0,				//宠物
	GUILD_INBORN_SKILL_TYPE_PARTNER = 1,			//伙伴
	GUILD_INBORN_SKILL_TYPE_MAGIC_ATTACK = 2,		//法攻
	GUILD_INBORN_SKILL_TYPE_PHYSICAL_ATTACK = 3,	//物攻
	GUILD_INBORN_SKILL_TYPE_DEFENSE = 4,			//防御
	GUILD_INBORN_SKILL_TYPE_AGILITY = 5,			//迅猛(目前就这个会指向多个)

	GUILD_INBORN_SKILL_TYPE_MAX
};

UNSTD_STATIC_CHECK(MAX_GUILD_INBORN_SKILL_TYPE_CUR_NUM == GUILD_INBORN_SKILL_TYPE_MAX);

enum GUILD_INBORN_ADD_TARGET_TYPE
{
	GUILD_INBORN_ADD_TARGET_TYPE_ROLE = 0,			//角色
	GUILD_INBORN_ADD_TARGET_TYPE_PET = 1,			//宠物
	GUILD_INBORN_ADD_TARGET_TYPE_PARTNER = 2,		//伙伴

	GUILD_INBORN_ADD_TARGET_TYPE_MAX
};

struct GuildInbornSkill
{
	GuildInbornSkill() { this->Reset(); }

	void Reset()
	{
		is_learn_flag = 0;
		reserve_ch = 0;
		skill_level = 0;
	} 
	bool IsInvaild() { return skill_level <= 0; }

	char is_learn_flag;		//0:未学习 1:已学习 (旧字段,之后直接使用等级字段即可)
	char reserve_ch;
	short skill_level;			//灵脉等级
};

struct RoleGuildInbornParam
{
	RoleGuildInbornParam() { this->Reset(); }

	void Reset()
	{
		max_guild_level = 0;
		use_skill_points = 0;
		is_check_old_skill_flag = 0;
		reserve_ch = 0;
		reserve_sh = 0;
		reserve_int = 0;

		memset(reserve_ll, 0, sizeof(reserve_ll));
		for (int i = 0; i < MAX_GUILD_INBORN_SKILL_TYPE_NUM; i++)
		{
			for (int k = 0; k < MAX_GUILD_INBORN_SKILL_TYPE_IN_HAS_SKILL_NUM; k++)
			{
				inborn_learn_list[i][k].Reset();
			}
		}
	}
		
	short max_guild_level;				//呆过的最高家族等级(废弃)
	short use_skill_points;				//已使用的技能点数(废弃)
	char is_check_old_skill_flag;		//是否检测过技能 0:未检测
	char reserve_ch;
	short reserve_sh;
	int reserve_int;

	long long reserve_ll[2];
	GuildInbornSkill inborn_learn_list[MAX_GUILD_INBORN_SKILL_TYPE_NUM][MAX_GUILD_INBORN_SKILL_TYPE_IN_HAS_SKILL_NUM];
};

typedef char RoleGuildInbornParamHex[sizeof(RoleGuildInbornParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(RoleGuildInbornParamHex) < 1024);

#pragma pack(pop)

#endif