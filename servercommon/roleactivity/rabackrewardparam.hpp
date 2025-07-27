#ifndef __RA_BACK_REWARD_PARAM_HPP__
#define __RA_BACK_REWARD_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"

////////////////////////////////////////////  回归豪礼 //////////////////////////////////////////////////////

static const int MAX_RA_BACK_REWARD_DAY = 8;					// 回归奖励天数上限
static const int MAX_RA_BACK_REWARD_TASK = 64;					// 回归奖励任务上限

enum RA_BACK_REWARD_TYPE
{
	RA_BACK_REWARD_TYPE_BEGIN = 0,
	RA_BACK_REWARD_TYPE_PET_INTENSI,				// 1 宠物强化次数
	RA_BACK_REWARD_TYPE_PARTNER_UPGRADE,			// 2 伙伴进阶
	RA_BACK_REWARD_TYPE_PASS_FB,					// 3 通关普通秘境
	RA_BACK_REWARD_TYPE_PASS_LHWC,					// 4 通关轮回忘川
	RA_BACK_REWARD_TYPE_PURPLE_TOKEN,				// 5 穿戴紫色令牌数量
	RA_BACK_REWARD_TYPE_BIG_DIPPER,					// 6 参与北斗七星
	RA_BACK_REWARD_TYPE_WEAR_EQUIPNUM,				// 7 伙伴穿戴命格卡的数量
	RA_BACK_REWARD_TYPE_PASS_ELITEFB,				// 8 通关精英秘境
	RA_BACK_REWARD_TYPE_PET_INTENSI_LEVEL,			// 9 宠物强化等级

	RA_BACK_REWARD_TYPE_MAX
};

enum RA_BACK_REWARD_FLAG_TYPE
{
	RA_BACK_REWARD_FLAG_TYPE_NOT_FINISH = 0,		// 未完成
	RA_BACK_REWARD_FLAG_TYPE_FINISH,				// 已完成
	RA_BACK_REWARD_FLAG_TYPE_FETCH,					// 已领取

	RA_BACK_REWARD_FLAG_TYPE_MAX
};

#pragma pack(push, 4)
////////////////////////////////////////////  服务器数据  //////////////////////////////////////////////////////


////////////////////////////////////////////  个人数据  //////////////////////////////////////////////////////
struct RABackRewardTaskData
{
	char flag;
	unsigned char param;
};


struct RABackRewardParam
{
	RABackRewardParam() { this->Reset(); }

	void Reset()
	{
		ra_begin_timestamp = 0;
		huigui_timestamp = 0;
		memset(day_flag, 0, sizeof(day_flag));
		memset(task_list, 0, sizeof(task_list));
	}

	unsigned int ra_begin_timestamp;					// 是否上一次参与活动时间戳
	unsigned int huigui_timestamp;						// 回归活动开始时间戳

	char day_flag[MAX_RA_BACK_REWARD_DAY];
	RABackRewardTaskData task_list[MAX_RA_BACK_REWARD_TASK];
};

UNSTD_STATIC_CHECK(sizeof(RABackRewardParam) <= sizeof(RoleActivityRoleData))

#pragma pack(pop)
#endif	//__RA_BACK_REWARD_PARAM_HPP__
