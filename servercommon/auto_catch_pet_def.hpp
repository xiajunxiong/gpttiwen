#ifndef __AUTO_CATCH_PET_DEF_HPP__
#define __AUTO_CATCH_PET_DEF_HPP__

#include "servercommon/servercommon.h"

#pragma pack(push)
#pragma pack(4)

static const int MAX_AUTO_CATCH_NUM = 5;

struct AutoCatchInfo
{
	void Reset()
	{
		monster_id = 0;
		max_catch_num = 0;
		cur_catch_num = 0;
		reserve_ll = 0;
	}

	bool IsValid() const
	{
		return monster_id > 0;
	}

	int monster_id;
	short max_catch_num;
	short cur_catch_num;
	long long reserve_ll;
};

struct AutoCatchPetParam
{
	void Reset()
	{
		max_seal_card_common_num = 0;
		max_seal_card_silver_num = 0;
		max_seal_card_gold_num = 0;
		cur_seal_card_common_num = 0;
		cur_seal_card_silver_num = 0;
		cur_seal_card_gold_num = 0;
		scene_id = 0;
		reserve_int = 0;
		memset(reserve, 0, sizeof(reserve));

		for (int i = 0; i < MAX_AUTO_CATCH_NUM; ++i)
		{
			auto_catch_list[i].Reset();
		}
	}

	int max_seal_card_common_num;
	int max_seal_card_silver_num;
	int max_seal_card_gold_num;
	int cur_seal_card_common_num;
	int cur_seal_card_silver_num;
	int cur_seal_card_gold_num;
	int scene_id;					// 设置场景ID  暂时只用于原样发给客户端做逻辑
	int reserve_int;
	long long reserve[3];

	AutoCatchInfo auto_catch_list[MAX_AUTO_CATCH_NUM];
};

typedef char AutoCatchPetParamHex[sizeof(AutoCatchPetParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(AutoCatchPetParamHex) < 512);

#pragma pack(pop)

#endif