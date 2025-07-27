#ifndef __RA_TIGER_FISH_EVERY_YEAR_PARAM_HPP__
#define __RA_TIGER_FISH_EVERY_YEAR_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"

namespace RATIGERFEY
{
	static const int MAX_REWARD_STAGE = 32;
	static const int MAX_LEVEL = 120;

	enum RA_TIRGER_FISH_EVERY_YEAR_TYPE
	{
		TIRGER_FEY_TYPE_JING_JI_CHANG = 0,			//!< 竞技场
		TIRGER_FEY_TYPE_QIONG_DING_ZHI_ZHENG = 1,	//!< 穹顶之争
		TIRGER_FEY_TYPE_WU_FANG_LEI_TAI = 2,		//!< 五方擂台
		TIRGER_FEY_TYPE_XIAN_SHI_HUO_DONG = 3,		//!< 任意限时活动	[魔将入侵,家族答题,以一敌百,百鬼夜行,巨龙来袭,迷宫竞速,家族荣誉战,幻界战场,首席竞选,家族大乱斗,竞技对抗赛]
		TIRGER_FEY_TYPE_CONSUME_TI_LI = 4,			//!< 使用体力
		TIRGER_FEY_TYPE_JING_YING_SHI_LIAN = 5,		//!< 精英试炼
		TIRGER_FEY_TYPE_YE_WAI_SHOU_LING = 6,		//!< 野外首领
		TIRGER_FEY_TYPE_YUAN_ZHENG_MO_ZHONG = 7,	//!< 远征魔冢
		TIRGER_FEY_TYPE_BEI_DOU_QI_XING = 8,		//!< 北斗七星

		TIRGER_FEY_TYPE_MAX,
	};
	UNSTD_STATIC_CHECK(TIRGER_FEY_TYPE_MAX <= 32);

};

#pragma pack(push,4)

struct RATigerFishEveryYearParam
{
	RATigerFishEveryYearParam() { this->Reset(); }

	void Reset()
	{
		ra_begin_timestamp = 0;
		is_accept_task = 0;
		fish_num = 0;
		role_level = 0;
		open_activity_level = 0;

		task_status.Reset();
		reward_stage_status.Reset();
		is_receive_reward.Reset();
		task_reward_status.Reset();
		memset(task_extent, 0, sizeof(task_extent));
	}

	void DayReset()
	{
		is_accept_task = 0;
		role_level = 0;

		task_status.Reset();
		task_reward_status.Reset();
		memset(task_extent, 0, sizeof(task_extent));
	}

	unsigned int ra_begin_timestamp;

	// 每日是否接取过渔夫任务，0--未接取，1--已接取
	short is_accept_task;

	// 玩家当日接任务时等级
	short role_level;

	// 玩家拥有的鱼数
	int fish_num;

	// 每日任务是否完成 [0-TIRGER_FEY_TYPE_MAX(9)]
	BitMap<RATIGERFEY::TIRGER_FEY_TYPE_MAX> task_status;

	// 每日任务奖励是否已领取[0-TIRGER_FEY_TYPE_MAX(9)]
	BitMap<RATIGERFEY::TIRGER_FEY_TYPE_MAX> task_reward_status;

	// 每日任务进度 [0-TIRGER_FEY_TYPE_MAX(9)]
	int task_extent[RATIGERFEY::TIRGER_FEY_TYPE_MAX];

	// 奖励档位是否可领取[0-31]
	BitMap<RATIGERFEY::MAX_REWARD_STAGE> is_receive_reward;

	// 奖励档位是否已领取[0-31]
	BitMap<RATIGERFEY::MAX_REWARD_STAGE> reward_stage_status;

	int open_activity_level;	//活动开启时玩家等级
};
UNSTD_STATIC_CHECK(sizeof(RATigerFishEveryYearParam) <= sizeof(RoleActivityRoleData));

#pragma pack(pop)

#endif