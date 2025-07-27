#ifndef __PET_HELPER_FORMATION_DEF_HPP__
#define __PET_HELPER_FORMATION_DEF_HPP__

#include "servercommon/formationdef.hpp"

#pragma pack(push)
#pragma pack(4)

static const int FORMATION_STANDBY_PET_NUM = 5;

enum PET_HELPER_FORMATION_TYPE
{
	PET_HELPER_FORMATION_CLOUD_ARENA = 0,			//!< 0 云端竞技（穹顶）
	PET_HELPER_FORMATION_COURAGE_CHALLENGE = 1,		//!< 1 锢魔之塔
	PET_HELPER_FORMATION_NETHERWORLD = 2,			//!< 2 幽冥炼狱
	PET_HELPER_FORMATION_COLOSSEUM = 3,				//!< 3 竞技场
	PET_HELPER_FORMATION_ENDLESS_TOWER = 4,			//!< 4 无尽之塔
	PET_HELPER_FORMATION_GUILD_BOSS = 5,			//!< 5 家族试炼BOSS
	PET_HELPER_FORMATION_WORLD_ARENA = 6,			//!< 6 论剑九州
	PET_HELPER_FORMATION_YAO_XING_SHI_LIAN = 7,		//!< 7 妖行试炼
	PET_HELPER_FORMATION_REALM_BREAK_FIGHT = 8,		//!< 8 心灵之境

	PET_HELPER_FORMATION_TYPE_MAX
};

enum PET_HELPER_FORMATION_GRID_TYPE
{
	PET_HELPER_FORMATION_GRID_INVALID = 0,
	PET_HELPER_FORMATION_GRID_ROLE = 1,
	PET_HELPER_FORMATION_GRID_PET = 2,
	PET_HELPER_FORMATION_GRID_PARTNER = 3,

	PET_HELPER_FORMATION_GRID_MAX
};

struct PetHelperFormationGrid // 阵容中的每个格点
{
	PetHelperFormationGrid()
	{
		this->Reset();
	}

	void Reset()
	{
		character_type = 0;
		reserve_ch = 0;
		pet_exclusive_skill_disable_flag = 0;
		lian_yao_exclusive_skill_disable_flag = 0;
		character_id = 0;
		unique_id = 0;
	}

	bool IsValid() const
	{
		return character_id > 0;
	}

	char character_type;
	char reserve_ch;
	unsigned char pet_exclusive_skill_disable_flag;
	unsigned char lian_yao_exclusive_skill_disable_flag;
	int character_id;
	long long unique_id;	// 主要用于区分宠物
};

struct StandbyPetData
{
	StandbyPetData() : pet_id(0), pet_unique_id(0LL)
	{
		this->Reset();
	}

	void Reset()
	{
		pet_id = 0;
		pet_exclusive_skill_disable_flag = 0;
		lian_yao_exclusive_skill_disable_flag = 0;
		reserve_sh = 0;
		pet_unique_id = 0;
	}

	int pet_id;
	unsigned char pet_exclusive_skill_disable_flag;
	unsigned char lian_yao_exclusive_skill_disable_flag;
	short reserve_sh;
	long long pet_unique_id;
};

struct PetHelperFormationInfo
{
	PetHelperFormationInfo()
	{
		this->Reset();
	}

	void Reset()
	{
		is_valid_formation = 0;
		reserve_ch = 0;
		reserve_sh = 0;
		reserve_int = 0;
		battle_auto_second_move_type = BATTLE_MOVE_TYPE_NORMAL_ATTACK;
		battle_auto_second_skill_id = 0;
		battle_auto_second_skill_level = 0;
		for (int i = 0; i < SIDE_CHARACTER_NUM; ++i)
		{
			grid_list[i].Reset();
		}

		for (int i = 0; i < FORMATION_STANDBY_PET_NUM; ++i)
		{
			standby_list[i].Reset();
		}

	}

	char is_valid_formation;
	char reserve_ch;
	short reserve_sh;
	int reserve_int;
	char		battle_auto_second_move_type;
	char		battle_auto_second_skill_level;
	UInt16		battle_auto_second_skill_id;
	PetHelperFormationGrid grid_list[SIDE_CHARACTER_NUM];
	StandbyPetData standby_list[FORMATION_STANDBY_PET_NUM];
};

struct PetHelperFormationParam
{
	PetHelperFormationParam()
	{
		this->Reset();
	}

	void Reset()
	{
		memset(reserve_ll, 0, sizeof(reserve_ll));
		for (int i = 0; i < PET_HELPER_FORMATION_TYPE_MAX; ++i)
		{
			formation_list[i].Reset();
		}

		reserve_int = 0;
		battle_auto_first_move_type = BATTLE_MOVE_TYPE_NORMAL_ATTACK;
		battle_auto_first_skill_level = 0;
		battle_auto_first_skill_id = 0;
	}

	long long reserve_ll[7];
	int reserve_int;
	char		battle_auto_first_move_type;
	char		battle_auto_first_skill_level;
	UInt16		battle_auto_first_skill_id;
	PetHelperFormationInfo formation_list[PET_HELPER_FORMATION_TYPE_MAX];
};

typedef char PetHelperFormationParamHex[sizeof(PetHelperFormationParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(PetHelperFormationParamHex) < 6144);

#pragma pack(pop)

#endif