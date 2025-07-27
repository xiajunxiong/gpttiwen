#ifndef __RESEARCH_TASK_DEF_H__
#define __RESEARCH_TASK_DEF_H__

#include "servercommon/serverdef.h"
#include "servercommon/servercommon.h"

#pragma pack(push, 4)

struct ResearchTaskInfo
{
	ResearchTaskInfo() { this->Reset(); }

	void Reset()
	{
		task_id = 0;
		progress = 0;
		image_monster_id = 0;
		battle_group_id = 0;
		scene_id = 0;
		posi_x = 0;
		posi_y = 0;
	}

	unsigned short task_id;		// 任务ID
	unsigned short progress;	// 任务进度

	int image_monster_id;		// 显示的怪物ID
	int battle_group_id;		// 战斗的怪物组

	int scene_id;				// 场景ID
	int posi_x;					// 横坐标
	int posi_y;					// 纵坐标
};

struct ResearchTaskParam
{
	ResearchTaskParam() { this->Reset(); }

	void Reset()
	{
		next_refresh_timestamp = 0;
		today_finish_times = 0;
		today_reward_fetch = 0;
		task_info.Reset();
		residual_reward_back_times = 0;
		m_add_exp_per_notice = 0;
	}

	long long next_refresh_timestamp;	// 下次任务刷新的时间戳
	int today_finish_times;				// 今天已完成真中的次数
	int today_reward_fetch;				// 今天已领取真中的标记

	ResearchTaskInfo task_info;

	int residual_reward_back_times;		// 剩余的奖励找回的次数
	long long m_add_exp_per_notice;		// 下发一轮经验获取额
};
typedef char ResearchTaskParamHex[sizeof(ResearchTaskParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(ResearchTaskParamHex) < 128);

#pragma pack(pop)
#endif // !__RESEARCH_TASK_DEF_H__