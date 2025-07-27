#ifndef __ROLE_HEART_SKILL_DEF_HPP__
#define __ROLE_HEART_SKILL_DEF_HPP__

#include "servercommon/servercommon.h"

#pragma pack(push, 4)

static const int MAX_HEART_SKILL_INITIATIVE_NUM = 30;		//主动技能数量
static const int MAX_HEART_SKILL_PASSIVE_NUM = 40;			//被动技能数量
static const int MAX_HEART_SKILL_STATUS_NUM = 8;			//同时使用的技能数量(主动+被动)
static const int MAX_HEART_SKILL_RESONANCE_ATTR_NUM = 4;	//共鸣最大属性数量
static const int MAX_HEART_SKILL_CONVERT_GROUP_NUM = 3;		
static const int MAX_HEART_SKILL_BASE_NUM = 100;
static const int MAX_HEART_SKILL_DISPLACE_NEED_NUM = 5;		//暂时限制为5本
static const int MAX_HEART_SKILL_ATTR_NUM = 2;				//心法属性数量

enum HEART_SKILL_SKILL_TYPE
{
	HEART_SKILL_SKILL_TYPE_BEGIN = 0,
	HEART_SKILL_SKILL_TYPE_INITATIVE = 1,		//主动技能
	HEART_SKILL_SKILL_TYPE_PASSIVE = 2,			//被动技能

	HEART_SKILL_SKILL_TYPE_MAX
};

enum HEART_SKILL_STATUS
{
	HEART_SKILL_INITIATIVE_INDEX = 0,			//主动技能index
	HEART_SKILL_PASSIVE_INDEX = 1,				//被动技能index
	HEART_SKILL_PASSIVE_INDEX1 = 2,				//被动技能index1
	HEART_SKILL_PASSIVE_INDEX2 = 3,				//被动技能index2
	HEART_SKILL_PASSIVE_INDEX3 = 4,				//被动技能index3
	HEART_SKILL_PASSIVE_END,

	HEART_SKILL_INITIATIVE_INDEX1 = HEART_SKILL_PASSIVE_END,	//主动技能index1

	HEART_SKILL_STATUS_MAX
};
UNSTD_STATIC_CHECK(HEART_SKILL_STATUS_MAX <= MAX_HEART_SKILL_STATUS_NUM);
UNSTD_STATIC_CHECK(MAX_HEART_SKILL_STATUS_NUM <= BIT_COUNT_OF_BYTE); //修改时,请注意下数据结构(解锁与装备栏)

enum HEART_SKILL_LEARN_TYPE
{
	HEART_SKILL_LEARN_TYPE_COIN = 0,			//心法学习消耗铜币
	HEART_SKILL_LEARN_TYPE_GOLD = 1,			//心法学习消耗元宝
	HEART_SKILL_LEARN_TYPE_ITEM = 2,			//心法学习消耗道具

	HEART_SKILL_LEARN_TYPE_MAX
};

struct HeartSkillDisplaceInfo
{
	HeartSkillDisplaceInfo() { this->Reset(); }

	void Reset()
	{
		column  = 0;
		index = 0;
	}

	short column;
	short index;
};

struct HeartSkillReasonanceInfo
{
	HeartSkillReasonanceInfo() : count(0), min_level(MAX_INT16)
	{}

	short count;
	short min_level;
};

struct HeartSkillInfo
{
	HeartSkillInfo() { this->Reset(); }

	void Reset()
	{
		heart_skill_seq = -1;
		skill_level = 0;
		reserve_int = 0;
	}

	short heart_skill_seq;
	short skill_level;
	int reserve_int;
};

struct HeartSkillParam
{
	HeartSkillParam() { this->Reset(); }

	void Reset()
	{
		memset(reserve_ll, 0, sizeof(reserve_ll));
		reserve_int = 0;
		reserve_sh = 0;
		is_old_user_active_flag = 0;
		for (int i = 0; i < MAX_HEART_SKILL_STATUS_NUM; i++)
		{
			equip_skill_list[i] = -1;
		}
		initiative_skill_count = 0;
		passive_skill_count = 0;
		for (int i = 0; i < MAX_HEART_SKILL_INITIATIVE_NUM; i++)
		{
			initiative_skill_list[i].Reset();
		}
		for (int i = 0; i < MAX_HEART_SKILL_PASSIVE_NUM; i++)
		{
			passive_skill_list[i].Reset();
		}
	}

	long long reserve_ll[2];
	int reserve_int;
	short reserve_sh;
	char is_old_user_active_flag;												//老玩家检测解锁触发关联系统 0:未触发 1:已触发
	unsigned char equip_skill_unlock_flag;										//解锁标识 --MAX_HEART_SKILL_STATUS_NUM 0:未解锁 1:已解锁(二进制)
	short equip_skill_list[MAX_HEART_SKILL_STATUS_NUM];							//此时装备的技能列表	下列列表的index
	short initiative_skill_count;
	short passive_skill_count;						
	HeartSkillInfo initiative_skill_list[MAX_HEART_SKILL_INITIATIVE_NUM];		//主动技能列表
	HeartSkillInfo passive_skill_list[MAX_HEART_SKILL_PASSIVE_NUM];				//被动技能列表
};

typedef char HeartSkillParamHex[sizeof(HeartSkillParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(HeartSkillParamHex) < 2048);

#pragma pack(pop)

#endif