#ifndef __ROLE_GUILD_INBORN_DEF_HPP__
#define __ROLE_GUILD_INBORN_DEF_HPP__

#include "servercommon/servercommon.h"

#pragma pack(push, 4)

static const int MAX_GUILD_INBORN_SKILL_TYPE_NUM = 8;							//�������������������
static const int MAX_GUILD_INBORN_SKILL_TYPE_IN_HAS_SKILL_NUM = 8;				//һ������������������ӵ�м�������
static const int NAX_GUILD_INBORN_SKILL_LEVEL_NUM = 10;							//�����������ܵȼ�����
	
static const int MAX_GUILD_INBORN_SKILL_TYPE_CUR_NUM = 6;						//�������������������(��ǰ)
static const int MAX_GUILD_INBORN_SKILL_TYPE_IN_HAS_SKILL_CUR_NUM = 6;			//һ������������������ӵ�м�������(��ǰ)	�����������Ҫ

static const int MAX_GUILD_INBORN_SKILL_NUM = MAX_GUILD_INBORN_SKILL_TYPE_NUM * MAX_GUILD_INBORN_SKILL_TYPE_IN_HAS_SKILL_NUM;				//����������������
static const int MAX_GUILD_INBORN_SKILL_CUR_NUM = MAX_GUILD_INBORN_SKILL_TYPE_CUR_NUM * MAX_GUILD_INBORN_SKILL_TYPE_IN_HAS_SKILL_CUR_NUM;	//����������ǰ��������

UNSTD_STATIC_CHECK(MAX_GUILD_INBORN_SKILL_TYPE_CUR_NUM <= MAX_GUILD_INBORN_SKILL_TYPE_NUM);
UNSTD_STATIC_CHECK(MAX_GUILD_INBORN_SKILL_TYPE_IN_HAS_SKILL_CUR_NUM <= MAX_GUILD_INBORN_SKILL_TYPE_IN_HAS_SKILL_NUM);

//һ�������,���ܻ�ָ����Ŀ������
enum GUILD_INBORN_SKILL_TYPE
{
	GUILD_INBORN_SKILL_TYPE_PET = 0,				//����
	GUILD_INBORN_SKILL_TYPE_PARTNER = 1,			//���
	GUILD_INBORN_SKILL_TYPE_MAGIC_ATTACK = 2,		//����
	GUILD_INBORN_SKILL_TYPE_PHYSICAL_ATTACK = 3,	//�﹥
	GUILD_INBORN_SKILL_TYPE_DEFENSE = 4,			//����
	GUILD_INBORN_SKILL_TYPE_AGILITY = 5,			//Ѹ��(Ŀǰ�������ָ����)

	GUILD_INBORN_SKILL_TYPE_MAX
};

UNSTD_STATIC_CHECK(MAX_GUILD_INBORN_SKILL_TYPE_CUR_NUM == GUILD_INBORN_SKILL_TYPE_MAX);

enum GUILD_INBORN_ADD_TARGET_TYPE
{
	GUILD_INBORN_ADD_TARGET_TYPE_ROLE = 0,			//��ɫ
	GUILD_INBORN_ADD_TARGET_TYPE_PET = 1,			//����
	GUILD_INBORN_ADD_TARGET_TYPE_PARTNER = 2,		//���

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

	char is_learn_flag;		//0:δѧϰ 1:��ѧϰ (���ֶ�,֮��ֱ��ʹ�õȼ��ֶμ���)
	char reserve_ch;
	short skill_level;			//�����ȼ�
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
		
	short max_guild_level;				//��������߼���ȼ�(����)
	short use_skill_points;				//��ʹ�õļ��ܵ���(����)
	char is_check_old_skill_flag;		//�Ƿ�������� 0:δ���
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