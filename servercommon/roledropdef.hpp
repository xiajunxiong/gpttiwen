#ifndef __ROLE_DROP_DEF_HPP__
#define __ROLE_DROP_DEF_HPP__

#include "servercommon/serverdef.h"
#include "servercommon/servercommon.h"

#pragma pack(push, 4)

static const int MAX_ROLE_DROP_LIMIT_ITEM_NUM = 500;
static const int MAX_MONSTER_GROUP_DROP_LIMIT_NUM = 500;
static const int MAX_ITEM_COOL_DOWN_DROP_LIMIT_NUM = 20;

struct RoleDropParam
{
	RoleDropParam()
	{
		this->Reset();
	}

	void Reset()
	{
		reserve_ll1 = 0;
		reserve_ll2 = 0;
		reserve_sh = 0;
		limit_item_num = 0;
		memset(limit_item_list, 0, sizeof(limit_item_list));
	}

	long long reserve_ll1;
	long long reserve_ll2;
	short reserve_sh;
	short limit_item_num;
	ItemID limit_item_list[MAX_ROLE_DROP_LIMIT_ITEM_NUM];
};

typedef char RoleDropParamHex[sizeof(RoleDropParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(RoleDropParamHex) < 2048);

struct MonsterGroupDropParam
{
	struct DropInfo
	{
		DropInfo() : monster_id(0), drop_times(0), reserve_sh(0) {}

		void Reset()
		{
			monster_id = 0;
			drop_times = 0;
			reserve_sh = 0;
		}

		int monster_id;
		unsigned short drop_times;
		short reserve_sh;
	};

	void Reset()
	{
		for (int i = 0; i < MAX_MONSTER_GROUP_DROP_LIMIT_NUM; ++i)
		{
			limit_drop_list[i].Reset();
		}
		reserve_ll1 = 0;
		reserve_ll2 = 0;
		reserve_sh = 0;
		limit_drop_num = 0;
	}

	long long reserve_ll1;
	long long reserve_ll2;
	short reserve_sh;
	short limit_drop_num;
	DropInfo limit_drop_list[MAX_MONSTER_GROUP_DROP_LIMIT_NUM];
};

typedef char MonsterGroupDropParamHex[sizeof(MonsterGroupDropParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(MonsterGroupDropParamHex) < 8192);


struct ItemCoolDownDropParam
{
	struct DropInfo
	{
		DropInfo() : item_id(0), reserve_sh(0), allow_drop_timestamp(0u) {}

		void Reset()
		{
			item_id = 0;
			allow_drop_timestamp = 0u;
			reserve_sh = 0;
		}

		ItemID item_id;
		short reserve_sh;
		unsigned int allow_drop_timestamp;
	};

	void Reset()
	{
		for (int i = 0; i < ARRAY_ITEM_COUNT(limit_drop_list); ++i)
		{
			limit_drop_list[i].Reset();
		}
		reserve_ll1 = 0;
		reserve_ll2 = 0;
		reserve_sh = 0;
		limit_drop_num = 0;
	}

	long long reserve_ll1;
	long long reserve_ll2;
	short reserve_sh;
	short limit_drop_num;
	DropInfo limit_drop_list[MAX_ITEM_COOL_DOWN_DROP_LIMIT_NUM];
};

typedef char ItemCoolDownDropParamHex[sizeof(ItemCoolDownDropParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(ItemCoolDownDropParamHex) < 512);


#pragma pack(pop)

#endif