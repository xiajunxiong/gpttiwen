#ifndef __SMART_MOUNTS_SYSTEM_DEF_HPP__
#define __SMART_MOUNTS_SYSTEM_DEF_HPP__

#include "servercommon/servercommon.h"

#pragma pack(push, 4)

static const int MAX_SMART_MOUNTS_SYSTEM_NUM = 10;						// 灵骑数量最大值
static const int MAX_SMART_MOUNTS_SYSTEM_ATTR_NUM = 5;					// 灵骑属性数量

static const int MAX_SMART_MOUNTS_SYSTEM_EQUIP_NUM = 4;					// 灵骑装备最大数量 (目前2个)
static const int MAX_SMART_MOUNTS_SYSYEM_BASE_SKILL_NUM = 6;			// 灵骑基础技能最大值	(目前3个)
//static const int MAX_SMART_MOUNTS_SYSTEM_EXCLUSIVE_SKILL_NUM = 4;		// 灵骑专属技能最大值 (目前2个)  预留双倍
static const int MAX_SMART_MOUNTS_SYSTEM_DOMINANCE_PET_NUM = 4;			// 灵骑统御宠物最大值 (目前2个)

static const int MAX_SMART_MOUNTS_SYSTEM_EQUIP_CUR_NUM = 2;				// 灵骑装备当前数量				//作用于人物
static const int MAX_SMART_MOUNTS_SYSYEM_BASE_SKILL_CUR_NUM = 3;		// 灵骑基础技能当前最大值			//基础技能作用宠物
//static const int MAX_SMART_MOUNTS_SYSTEM_EXCLUSIVE_SKILL_CUR_NUM = 2;	// 灵骑专属技能当前最大值			//专属技能作用人物
static const int MAX_SMART_MOUNTS_SYSTEM_DOMINANCE_PET_CUR_NUM = 2;		// 灵骑统御宠物当前最大值

static const int MAX_SMART_MOUNTS_SYSTEM_CUR_SKILL_NUM = MAX_SMART_MOUNTS_SYSYEM_BASE_SKILL_CUR_NUM; //+ MAX_SMART_MOUNTS_SYSTEM_EXCLUSIVE_SKILL_CUR_NUM;

UNSTD_STATIC_CHECK(MAX_SMART_MOUNTS_SYSYEM_BASE_SKILL_CUR_NUM <= MAX_SMART_MOUNTS_SYSYEM_BASE_SKILL_NUM);
//UNSTD_STATIC_CHECK(MAX_SMART_MOUNTS_SYSTEM_EXCLUSIVE_SKILL_CUR_NUM <= MAX_SMART_MOUNTS_SYSTEM_EXCLUSIVE_SKILL_NUM);
UNSTD_STATIC_CHECK(MAX_SMART_MOUNTS_SYSTEM_DOMINANCE_PET_CUR_NUM <= MAX_SMART_MOUNTS_SYSTEM_DOMINANCE_PET_NUM);

static const int MAX_SMART_MOUNTS_SYSTEM_JEWELRY_LEVEL_NUM = 10;		//灵饰强化最大等级
static const int MAX_SMART_MOUNTS_SYSTEM_REVEAL_LEVEL_NUM = 20;			//灵骑点化最高等级

enum SMART_MOUNTS_SYSTEM_SPECIAL_FLAG
{
	SMART_MOUNTS_SYSTEM_SPECIAL_FLAG_BIT_BASE = 0,
	SMART_MOUNTS_SYSTEM_SPECIAL_FLAG_BIT_SKILL = 1,

	SMART_MOUNTS_SYSTEM_SPECIAL_FLAG_BIT_BASE_MAX
};
UNSTD_STATIC_CHECK(SMART_MOUNTS_SYSTEM_SPECIAL_FLAG_BIT_BASE_MAX < sizeof(char) * 8);

struct SmartMountsSystemBaseSkillParam
{
	SmartMountsSystemBaseSkillParam() { this->Reset(); }
	SmartMountsSystemBaseSkillParam(UInt16 _skill_id, char _skill_level): skill_id(_skill_id), skill_level(_skill_level), is_unlock(0) {}

	void Reset()
	{
		skill_id = 0;
		skill_level = 0;
		is_unlock = 0;
	}
	UInt16 skill_id;
	char skill_level;
	char is_unlock;			// 0:未解锁 1:已解锁
};

struct SmartMountsSystemItemData
{
	SmartMountsSystemItemData() { this->Reset(); }

	void Reset()
	{
		is_active_flag = 0;
		strength_level = 0;
		reserve_sh = 0;
	}

	char is_active_flag;				//激活标识 0:未激活 1:已激活 
	unsigned char strength_level;		//强化等级
	short reserve_sh;
};

struct SmartMountsSystemSingleParam
{
	SmartMountsSystemSingleParam() { this->Reset(); }

	void Reset()
	{
		bicycle_id = 0;
		level = 0;
		remain_skill_point = 0;
		energy_value = 0;
		is_use_advanced_flag = 0;
		holy_strength_level = 0;
		is_breach_flag = 0;
		is_refine_flag = 0;
		capability = 0;
		reveal_level = 0;
		reserve_sh = 0;
		for (int i = 0; i < MAX_SMART_MOUNTS_SYSTEM_DOMINANCE_PET_NUM; i++)
		{
			dominance_pet_index_list[i] = -1;
		}
		//memset(exclusive_level_list, 0, sizeof(exclusive_level_list));
		for (int i = 0; i < MAX_SMART_MOUNTS_SYSYEM_BASE_SKILL_NUM; i++)
		{
			base_skill_list[i].Reset();
		}
		memset(new_base_skill_list, 0, sizeof(new_base_skill_list));
		jewelry_data.Reset();
		for (int i = 0; i < MAX_SMART_MOUNTS_SYSTEM_EQUIP_NUM; i++)
		{
			equip_list[i].Reset();
		}
		memset(reserve_ll, 0, sizeof(reserve_ll));
	}

	int GetReachSkillLevelCount(int skill_level)
	{
		if(skill_level <= 0) return 0;

		int reach_count = 0;
		/*	for (int i = 0; i < MAX_SMART_MOUNTS_SYSTEM_EXCLUSIVE_SKILL_NUM; i++)
			{
				if (exclusive_level_list[i] >= skill_level)
				{
					reach_count++;
				}
			}*/
		for (int i = 0; i < MAX_SMART_MOUNTS_SYSYEM_BASE_SKILL_NUM; i++)
		{
			if (base_skill_list[i].skill_level >= skill_level)
			{
				reach_count++;
			}
		}
		return reach_count;
	}

	short bicycle_id;										
	UInt16 level;																				// 灵骑等级
	short remain_skill_point;																	// 剩余技能点(废弃)
	short energy_value;																			// 精力值
	char is_breach_flag;																		// 是否经过突破 1:完成突破
	char is_refine_flag;																		// 是否存在洗练 1:有洗练内容(废弃)
	char is_use_advanced_flag;																	// 形象使用标识 0:未使用进阶形象 1:使用进阶形象
	char holy_strength_level;																	// 圣物强化等级
	int capability;																				// 评分
	UInt16 reveal_level;																		// 点化等级
	short reserve_sh;
	char dominance_pet_index_list[MAX_SMART_MOUNTS_SYSTEM_DOMINANCE_PET_NUM];					// 统御宠物index
	//short exclusive_level_list[MAX_SMART_MOUNTS_SYSTEM_EXCLUSIVE_SKILL_NUM];					// 专属技能等级
	SmartMountsSystemBaseSkillParam base_skill_list[MAX_SMART_MOUNTS_SYSYEM_BASE_SKILL_NUM];	// 基础技能
	UInt16 new_base_skill_list[MAX_SMART_MOUNTS_SYSYEM_BASE_SKILL_NUM];							// 洗练基础技能ID(废弃)
	SmartMountsSystemItemData jewelry_data;														// 灵饰数据
	SmartMountsSystemItemData equip_list[MAX_SMART_MOUNTS_SYSTEM_EQUIP_NUM];					// 装备数据

	long long reserve_ll[3];
};

struct SmartMountsSystemParam
{
	SmartMountsSystemParam() { this->Reset(); }

	void Reset()
	{
		is_finish_fun_guide_flag = 0;
		ride_index = -1;
		reserve_sh = 0;
		memset(reserve_ll, 0, sizeof(reserve_ll));
		for (int i = 0; i < MAX_SMART_MOUNTS_SYSTEM_NUM; i++)
		{
			smart_mounts_list[i].Reset();
		}
	}
	char is_finish_fun_guide_flag;			//是否完成功能指引 0:未完成 1:已完成
	char ride_index;						//玩家此时骑乘的灵骑 -1:未骑乘 
	short reserve_sh;

	long long reserve_ll[3];
	SmartMountsSystemSingleParam smart_mounts_list[MAX_SMART_MOUNTS_SYSTEM_NUM];
};
typedef char SmartMountsSystemParamHex[sizeof(SmartMountsSystemParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(SmartMountsSystemParamHex) < 4096);

struct SmartMountsSystemPro
{
	SmartMountsSystemPro() { this->Reset(); }

	void SetData(const SmartMountsSystemSingleParam & data)
	{
		bicycle_id = data.bicycle_id;
		level = data.level;
		holy_strength_level = data.holy_strength_level;
		reveal_level = data.reveal_level;
		capability = data.capability;

		for (int i = 0; i < ARRAY_LENGTH(base_skill_list) && i < ARRAY_LENGTH(data.base_skill_list); i++)
		{
			base_skill_list[i] = data.base_skill_list[i];
		}
		jewelry_data = data.jewelry_data;
		for (int i = 0; i < ARRAY_LENGTH(equip_list) && i < ARRAY_LENGTH(data.equip_list); i++)
		{
			equip_list[i] = data.equip_list[i];
		}
	}

	void Reset()
	{
		bicycle_id = 0;
		level = 0;
		holy_strength_level = 0;
		reveal_level = 0;
		capability = 0;

		memset(attr_list, 0, sizeof(attr_list));
		for (int i = 0; i < ARRAY_LENGTH(base_skill_list); i++)
		{
			base_skill_list[i].Reset();
		}
		jewelry_data.Reset();
		for (int i = 0; i < ARRAY_LENGTH(equip_list); i++)
		{
			equip_list[i].Reset();
		}
	}

	short bicycle_id;					// 灵骑ID
	UInt16 level;						// 灵骑等级
	short holy_strength_level;			// 圣物强化等级
	UInt16 reveal_level;				// 点化等级
	int capability;						// 评分

	AttrInfo attr_list[MAX_SMART_MOUNTS_SYSTEM_ATTR_NUM];			//灵骑属性
	SmartMountsSystemBaseSkillParam base_skill_list[MAX_SMART_MOUNTS_SYSYEM_BASE_SKILL_CUR_NUM];	// 基础技能
	SmartMountsSystemItemData jewelry_data;											// 灵饰数据
	SmartMountsSystemItemData equip_list[MAX_SMART_MOUNTS_SYSTEM_EQUIP_CUR_NUM];	// 装备数据
};

#pragma pack(pop)

#endif