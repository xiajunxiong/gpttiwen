#ifndef __TREASURE_TASK_DEF_H__
#define __TREASURE_TASK_DEF_H__

#include "servercommon/serverdef.h"
#include "servercommon/servercommon.h"

#pragma pack(push, 4)

static const int TREASURE_TASK_POOL_NNM = 8;				// 奖池大小
static const int TREASURE_TASK_TOTAL_ITEM_NNM = 40;			// 累积物品数量

struct TreasureTaskItem
{
	unsigned short item_id;
	unsigned short num;
};

struct TreasureTaskParam
{
	TreasureTaskParam() { this->Reset(); }
	void Reset()
	{
		level = 0;
		order_id = 0;

		route = 0;	
		pos_id = 0;
		baodi_time = 0;
		day_total_num = 0;

		memset(pool, 0, sizeof(pool));
		memset(day_total_item, 0, sizeof(day_total_item));
	}

	short level;
	short order_id;

	short route;
	short pos_id;

	short baodi_time;
	short day_total_num;

	short pool[TREASURE_TASK_POOL_NNM];

	TreasureTaskItem day_total_item[TREASURE_TASK_TOTAL_ITEM_NNM];
};
typedef char TreasureTaskParamHex[sizeof(TreasureTaskParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(TreasureTaskParamHex) < 512);

#pragma pack(pop)
#endif // !__TREASURE_TASK_DEF_H__