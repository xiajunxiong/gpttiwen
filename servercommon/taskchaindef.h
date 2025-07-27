#ifndef __TASK_CHAIN_DEF_H__
#define __TASK_CHAIN_DEF_H__

#include "servercommon/serverdef.h"
#include "servercommon/servercommon.h"

#pragma pack(push, 4)

static const int TASK_CHAIN_PARAM_COUNT = 10;		// 存库，慎改；
UNSTD_STATIC_CHECK(10 == TASK_CHAIN_PARAM_COUNT);

static const int TASK_CHAIN_RATE_MAX = 10000;		//概率权重比-万分比

struct TaskChainParam
{
	TaskChainParam() { this->Reset(); }
	void Reset()
	{
		pass_count = 0;
		task_type = 0;
		memset(task_param, 0, sizeof(task_param));
		task_count = 0;
		next_refresh_timestamp = 0LL;
		task_limit_timestamp = 0LL;
		has_fetch_turn_table_reward = 0;
		turn_table_group_id = 0;
		accept_task_level = 0;	
		pre_wait_task = 0;
		next_completion_times = 0;
		turn_table_completion_times_index = -1;
		is_version_change_process_flag = 0;
		is_version_change_week_reset_flag = 0;
		version_change_completion_times = 0;
		task_pass_count = 0;
		cur_task_max_count = 0;
	}

	void ClearTask()
	{
		task_type = 0;
		memset(task_param, 0, sizeof(task_param));
		can_skip = 0;
		reward_double = 0;
		price_type = 0;
		price = 0;
		task_limit_timestamp = 0LL;
	}

	int			pass_count;		// 本轮次的任务进度；
	int			task_type;
	int			task_param[TASK_CHAIN_PARAM_COUNT];
	short		can_skip;
	short		reward_double;
	int			price_type;
	int			price;
	int			task_count;					// 阶段；
	long long	next_refresh_timestamp;		// 下次刷新任务以及周次数的时间戳
	long long	task_limit_timestamp;		// 任务特殊时限 - 0则不限制
	short		has_fetch_turn_table_reward;	// 本周是否领取了转盘 0:未领取 1:已领取(位运算）
	short		turn_table_group_id;			// 本周转盘随机的奖励组ID
	short		accept_task_level;				// 接取每阶段首任务时的等级
	short       pre_wait_task;					// 代送任务间隔
	short		next_completion_times;			// 下一个阶段的环数目标
	short		turn_table_completion_times_index;	// 此时随机的转盘奖励对应是哪一位（对应领取位运算）
	char		is_version_change_process_flag;		// 改版后处理标识  0:未处理 1:已处理 2:新版数据改动
	char		is_version_change_week_reset_flag;	// 改版后是否进行了周刷新 0:否 1:是
	short		version_change_completion_times;	// 改版后经数据处理,此时应该下发的所能完成的环数
	short		task_pass_count;				// 本阶段的任务进度
	short		cur_task_max_count;				// 本阶段的最大环数
};
typedef char TaskChainParamHex[sizeof(TaskChainParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(TaskChainParamHex) < 256);

#pragma pack(pop)
#endif // !__TASK_CHAIN_DEF_H__