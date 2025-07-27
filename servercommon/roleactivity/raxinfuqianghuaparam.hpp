#ifndef __RA_XIN_FU_QIANG_HUA_PARAM_HPP__
#define __RA_XIN_FU_QIANG_HUA_PARAM_HPP__

#include "servercommon/roleactivitydef.hpp"
#include "servercommon/petdef.h"

#pragma pack(push,4)


///////////////////////////// 新服强化 ///////////////////////////////////
static const int MAX_XIN_FU_QIANG_HUA_BITMAP = 32;
struct RAXinFuQiangHuaParam
{
	RAXinFuQiangHuaParam() { this->Reset(); }
	void Reset()
	{
		ra_begin_timestamp = 0;
		free_reward_flag = 0;

		for (size_t i = 0; i < MAX_PET_QUALITY_NUM; i++)
		{
			task_finish[i].Reset();
			reward_receive[i].Reset();
		}
	}

	void DayReset()
	{
		free_reward_flag = 0;
	}

	unsigned int ra_begin_timestamp;

	int free_reward_flag;		// 每日免费礼包
	BitMap<MAX_XIN_FU_QIANG_HUA_BITMAP> task_finish[MAX_PET_QUALITY_NUM];		// 任务是否已完成flag
	BitMap<MAX_XIN_FU_QIANG_HUA_BITMAP> reward_receive[MAX_PET_QUALITY_NUM];	// 奖励是否已领取flag
};
UNSTD_STATIC_CHECK(sizeof(RAXinFuQiangHuaParam) <= sizeof(RoleActivityRoleData))
UNSTD_STATIC_CHECK(MAX_PET_QUALITY_NUM == 6)

#pragma pack(pop)

#endif
