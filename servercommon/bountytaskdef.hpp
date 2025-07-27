#ifndef __BOUNTY_TASK_DEF_HPP__
#define __BOUNTY_TASK_DEF_HPP__

#include "servercommon/servercommon.h"

#pragma pack(push, 4)

static const int BOUNTY_TASK_MAX_PER_DAY = 100;
static const int BOUNTY_TASK_REWARD_COUNT = 30;

struct ItemReward
{
	ItemReward() { this->Reset(); }

	void Reset()
	{
		is_bind = 0;
		item_id = 0;
		count = 0;
	}

	short is_bind;
	ItemID item_id;
	int count;
};

struct BountyTaskParam
{
	BountyTaskParam() { this->Reset(); }
	void Reset()
	{
		memset(reserve_ll, 0, sizeof(reserve_ll));
		first_task_count = 0;
		is_first_task = 0;
		is_special_task = 0;
		today_first_task_id = 0;

		today_task_group_num = 0;
		cur_task_idx = -1;
		memset(today_task_list, 0, sizeof(today_task_list));

		today_turn_table_reward_idx = -1;
		has_fetch_turn_table_reward = 0;
		has_finish_first_task = 0;
		today_turn_table_group_id = 0;
		cur_finish_task_group_num = 0;

		last_task_id = 0;
		last_is_one_key = 0;
		is_first_turn_table = 0;
		day_add_exp = 0;
		day_add_coin = 0;
		item_count = 0;
		for (int i = 0; i < ARRAY_LENGTH(day_item_reward_list); ++i)
		{
			day_item_reward_list[i].Reset();
		}
	}
	void DayItemRewardReset()
	{
		day_add_exp = 0;
		day_add_coin = 0;
		for (int i = 0; i < item_count && i < ARRAY_LENGTH(day_item_reward_list); i++)
		{
			day_item_reward_list[i].Reset();
		}
		item_count = 0;
	}

	long long reserve_ll[2];
	int first_task_count;	// 第一次任务计数器 
	char is_first_task;		// 第一次任务特殊逻辑
	char is_special_task;	// 走到特殊逻辑
	unsigned short today_first_task_id;
	short today_turn_table_reward_idx;
	char has_fetch_turn_table_reward;
	char has_finish_first_task;
	short today_turn_table_group_id;
	short cur_finish_task_group_num;
	short today_task_group_num;
	short cur_task_idx;
	unsigned short today_task_list[BOUNTY_TASK_MAX_PER_DAY];
	unsigned short last_task_id;	 // 最后一次接的任务
	char last_is_one_key;
	char is_first_turn_table; 
	int day_add_exp;
	int day_add_coin;
	int item_count;
	ItemReward day_item_reward_list[BOUNTY_TASK_REWARD_COUNT];			//暂存今天的任务奖励
};

typedef char BountyTaskParamHex[sizeof(BountyTaskParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(BountyTaskParamHex) < 1024);

#pragma pack(pop)

#endif // !__BOUNTY_TASK_DEF_H__
