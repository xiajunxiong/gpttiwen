#ifndef __RA_SHANHAI_HUADENG_PARAM_HPP__
#define __RA_SHANHAI_HUADENG_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/bitmapfunction.hpp"
#include "servercommon/roleactivitydef.hpp"

#pragma pack(push, 4)

static const int MAX_RA_SHANHAI_HUADENG_TASK_NUM = 32;				//山海花灯任务数量
static const int MAX_RA_SHANHAI_HUADENG_LIGHT_TYPE_NUM = 10;		//山海花灯类型

//山海花灯
struct RAShanHaiHuaDengParam
{
	RAShanHaiHuaDengParam() { this->Reset(); }

	void Reset()
	{
		ra_begin_timestamp = 0;
		last_start_day_id = 0;
		game_end_timestamp = 0;
		single_score_count = 0;
		last_game_score_count = 0;
		day_single_score_count = 0;
		activity_single_score_count = 0;
		day_join_times = 0;
		activity_join_times = 0;
		day_score_count = 0;
		reserve_sh = 0;
		activity_score_count = 0;
		m_task_list_flag.Reset();
	}

	unsigned int ra_begin_timestamp;
	unsigned int last_start_day_id;									//开始游戏时的天数ID
	unsigned int game_end_timestamp;								//游戏结束时间戳
	short single_score_count;										//本次游戏积分
	short last_game_score_count;									//上一场游戏获得的积分
	short day_single_score_count;									//今天单次积分最大值
	short activity_single_score_count;								//活动内单次积分最大值
	short day_join_times;											//今日参与次数
	short activity_join_times;										//活动内参与次数
	short day_score_count;											//今日获得的总分
	short reserve_sh;
	int activity_score_count;										//活动内获得的总积分
	BitMap<MAX_RA_SHANHAI_HUADENG_TASK_NUM> m_task_list_flag;		//任务列表标识 0:未完成 1:已完成
};
UNSTD_STATIC_CHECK(sizeof(RAShanHaiHuaDengParam) <= sizeof(RoleActivityRoleData));

#pragma pack(pop)

#endif