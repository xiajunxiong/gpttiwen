#ifndef __RA_GOD_BEAST_ADVENT_PARAM_HPP__
#define __RA_GOD_BEAST_ADVENT_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"

#pragma pack(push, 4)

static const int MAX_BITMAP_GOD_BEAST_ADVENT = 32;			// bitmap用
static const int MAX_TASK_NUM_GOD_BEAST_ADVENT = 25;		// 任务种类最大值
static const int MAX_REWARD_GROUP_GOD_BEAST_ADVENT = 20;	// 道具兑换种类最大值

///////////////////////////////////////// 服务器数据 ////////////////////////////////////////////////

struct RAGodBeastAdventBuyParam
{
	RAGodBeastAdventBuyParam() { this->Reset(); }

	void Reset()
	{
		ra_begin_timestamp = 0;
		cycle_flag = 0;
		reward_group = 0;
		activity_seq = 0;
	}

	unsigned int ra_begin_timestamp;		// 活动开始时间戳
	int cycle_flag;							// 活动期数flag
	int reward_group;						// 当前期数奖励组
	int activity_seq;						// 活动期数索引,发给客户端的
};


///////////////////////////////////////// 个人数据 //////////////////////////////////////////////////
struct RAGodBeastAdventParam
{
	RAGodBeastAdventParam() { this->Reset(); }

	void Reset()
	{
		ra_begin_timestamp = 0;
		zhigou_flag = 0;
		reserve_ch = 0;
		cycle_flag = 0;
		cur_like = 0;

		today_task_finish.Reset();
		is_receive_reward.Reset();
		memset(today_task_count, 0, sizeof(today_task_count));
		memset(buy_limit, 0, sizeof(buy_limit));
	}

	void DayReset()
	{
		today_task_finish.Reset();
		is_receive_reward.Reset();
		memset(today_task_count, 0, sizeof(today_task_count));
	}

	void CycleReset()
	{
		zhigou_flag = 0;
		cur_like = 0;

		today_task_finish.Reset();
		is_receive_reward.Reset();
		memset(today_task_count, 0, sizeof(today_task_count));
		memset(buy_limit, 0, sizeof(buy_limit));
	}

	unsigned int ra_begin_timestamp;

	char zhigou_flag;			// 是否购买了活动门票
	char reserve_ch;
	short cycle_flag;			// 期数flag

	unsigned int cur_like;			// 当前好感度

	///////// 任务相关 //////////
	BitMap<MAX_BITMAP_GOD_BEAST_ADVENT> today_task_finish;		// 每日任务是否完成
	BitMap<MAX_BITMAP_GOD_BEAST_ADVENT> is_receive_reward;		// 每日任务奖励是否已领取
	int today_task_count[MAX_TASK_NUM_GOD_BEAST_ADVENT];		// 每日任务进度

	///////// 兑换相关 /////////
	short buy_limit[MAX_REWARD_GROUP_GOD_BEAST_ADVENT];			// 限购道具兑换次数
};
UNSTD_STATIC_CHECK(sizeof(RAGodBeastAdventParam) <= sizeof(RoleActivityRoleData))

#pragma pack(pop)

#endif