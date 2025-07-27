#ifndef __FORMATION_DEF_HPP__
#define __FORMATION_DEF_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/struct/battle/battle_def.hpp"

#pragma pack(push)
#pragma pack(4)

struct FormationGrid // 阵容中的每个格点
{
	FormationGrid()
	{
		this->Reset();
	}

	void Reset()
	{
		character_type = -1;
		reserve_ch = 0;
		reserve_sh = 0;
		character_id = 0;
		unique_id = 0;
	}

	bool IsValid() const
	{
		return character_id > 0;
	}

	char character_type; 
	char reserve_ch;
	short reserve_sh;
	int character_id;
	long long unique_id;	// 主要用于区分宠物
};

struct FormationInfo
{
	FormationInfo()
	{
		this->Reset();
	}

	void Reset()
	{
		is_valid = 0;
		reserve_ch = 0;
		reserve_sh = 0;
		
		for (int i = 0; i < SIDE_CHARACTER_NUM; ++i)
		{
			grid_list[i].Reset();
		}

		memset(formation_name, 0, sizeof(formation_name));
	}

	char is_valid;
	char reserve_ch;
	short reserve_sh;
	GameName formation_name;

	FormationGrid grid_list[SIDE_CHARACTER_NUM];
};

static const int MAX_FORMATION_NUM = 10;

struct FormationAutoInfo
{
	FormationAutoInfo() { this->Reset(); }

	void Reset()
	{
		battle_auto_first_move_type = BATTLE_MOVE_TYPE_NORMAL_ATTACK;
		battle_auto_first_skill_id = 0;
		battle_auto_first_skill_level = 0;
		battle_auto_second_move_type = BATTLE_MOVE_TYPE_NORMAL_ATTACK;
		battle_auto_second_skill_id = 0;
		battle_auto_second_skill_level = 0;
	}

	short		battle_auto_first_move_type;
	UInt16		battle_auto_first_skill_id;
	short		battle_auto_first_skill_level;
	short		battle_auto_second_move_type;
	UInt16		battle_auto_second_skill_id;
	short		battle_auto_second_skill_level;
};

struct FormationParam
{
	FormationParam()
	{
		this->Reset();
	}

	void Reset()
	{
		memset(reserve_ll, 0, sizeof(reserve_ll));
		valid_formation_num = 0;

		for (int i = 0; i < MAX_FORMATION_NUM; ++i)
		{
			info_list[i].Reset();
		}
		for (int i = 0; i < ARRAY_LENGTH(info_auto_list); i++)
		{
			info_auto_list[i].Reset();
		}

		cur_formation_idx = -1;
		reserve_sh = 0;
	}

	long long reserve_ll[4];
	int valid_formation_num;
	short cur_formation_idx;
	short reserve_sh;
	FormationInfo info_list[MAX_FORMATION_NUM];
	FormationAutoInfo info_auto_list[MAX_FORMATION_NUM];
};

typedef char FormationParamHex[sizeof(FormationParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(FormationParamHex) < 8192);

#pragma pack(pop)

#endif