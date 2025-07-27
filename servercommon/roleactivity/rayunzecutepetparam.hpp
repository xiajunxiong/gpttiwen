#ifndef __RA_YUN_ZE_CUTE_PET_PARAM_HPP__
#define __RA_YUN_ZE_CUTE_PET_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"

namespace RAYZCutePet
{
	//每日任务数
	static const int DAY_TASK_COUNT_MAX = 10;
	//活动持续最大时长
	static const int ACTIVITY_DAY_MAX = 12;
	//宠物最大挑战个数
	static const int MONSTER_MAX_NUM = 10;

	//任务类型
	enum TaskType
	{
		TASK_TYPE_BEGIN,

		TASK_TYPE_LOGIN = 1,				// 登录
		TASK_TYPE_YZ_TERRITORY = 2,			// 云泽画境
		TASK_TYPE_YZ_ZAI_ZHAN = 3,			// 再战云泽
		TASK_TYPE_YZ_ER_SAN_SHI = 4,		// 云泽二三事
		TASK_TYPE_YZ_BLOOMING_RECORD = 5,	// 云泽繁花录
		TASK_TYPE_MO_ZU_ZAI_NA_LI = 6,		// 魔族在哪里
		TASK_TYPE_ACTIVE = 7,				// 活跃度

		TASK_TYPE_MAX,
	};


};



#pragma pack(push, 4)

struct RAYunZeCutePetParam
{
	RAYunZeCutePetParam() { this->Reset(); }

	void Reset()
	{
		ra_begin_timestamp = 0;
		pet_combat_num = 0;
		ra_login_timestamp = 0;

		memset(today_fraction, 0, sizeof(today_fraction));
		memset(task_param_list, 0, sizeof(task_param_list));
		task_status.Reset();
		challenge_status.Reset();
	}

	void DayReset()
	{

		memset(task_param_list, 0, sizeof(task_param_list));
		task_status.Reset();
	}

	unsigned int ra_begin_timestamp;
	unsigned int ra_login_timestamp;
	int pet_combat_num;					// 宠物战斗力

	//历史每日获得战力[活动最长持续时间--12]
	int today_fraction[RAYZCutePet::ACTIVITY_DAY_MAX];

	//任务参数[任务类型], 每日任务得分
	int task_param_list[RAYZCutePet::TASK_TYPE_MAX];

	//任务完成状态
	BitMap<RAYZCutePet::DAY_TASK_COUNT_MAX> task_status;

	//萌宠挑战状态[1-10]
	BitMap<RAYZCutePet::MONSTER_MAX_NUM+1> challenge_status;
};
UNSTD_STATIC_CHECK(sizeof(RAYunZeCutePetParam) <= sizeof(RoleActivityRoleData));

#pragma pack(pop)

#endif