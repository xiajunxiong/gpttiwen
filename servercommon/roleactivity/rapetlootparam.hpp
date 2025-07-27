#ifndef __RA_PET_LOOT_PARAM_HPP__
#define __RA_PET_LOOT_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"

#pragma pack(push, 4)
////////////////////////////////////////////  服务器数据  //////////////////////////////////////////////////////

const static int MAX_PET_LOOT_TASK_LIST_COUNT = 15;//单区块任务数量最大值

////////////////////////////////////////////  个人数据  //////////////////////////////////////////////////////

struct PetLootTask
{
	PetLootTask() : param(0), can_get_reward_times(0), has_get_reward_times(0) {}
	int param;
	short can_get_reward_times;
	short has_get_reward_times;
};

struct RAPetLootParam
{
	RAPetLootParam() { this->Reset(); }

	void Reset()
	{
		begin_time = 0;
		end_time = 0;
		open_day = 0;
		memset(task_info, 0, sizeof(task_info));
		task_finish_times = 0;
		task_final_reward_flag = 0;
	}

	unsigned int begin_time;
	unsigned int end_time;
	int open_day;
	PetLootTask task_info[MAX_PET_LOOT_TASK_LIST_COUNT];

	int task_finish_times;
	int task_final_reward_flag;//二进制累计奖励领取标记
};

UNSTD_STATIC_CHECK(sizeof(RAPetLootParam) <= sizeof(RoleActivityRoleData));

#pragma pack(pop)


#endif	//__RA_PET_Loot_PARAM_HPP__
