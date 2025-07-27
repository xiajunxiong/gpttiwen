#ifndef __PRESTIGE_TASK_DEF_HPP__
#define __PRESTIGE_TASK_DEF_HPP__

#include "servercommon/servercommon.h"

static const int MAX_PRESTIGE_TASK_NUM_PER_DAY = 10;

#pragma pack(push, 4)

struct PrestigeTaskParam
{
	PrestigeTaskParam()
	{
		this->Reset();
	}

	void Reset()
	{
		village_id = 0;
		cur_task_id = 0;
		today_finish_task_num = 0;
		today_task_num = 0;
		memset(today_task_list, 0, sizeof(today_task_list));
		memset(reserve_int, 0, sizeof(reserve_int));
		today_total_prestige = 0;
		baodi_refresh_num = 0;
		re_sh = 0;
		memset(today_star_list, 0, sizeof(today_star_list));
	}

	short village_id;			//!< 选择的村庄id
	UInt16 cur_task_id;			//!< 当前任务ID
	short today_finish_task_num;//!< 今天已完成任务数
	short today_task_num;		//!< 今天总计任务数
	int today_total_prestige;	//!< 今天声望任务总计获得声望数
	short baodi_refresh_num;	//!< 保底刷新次数
	short re_sh;
	int reserve_int[2];			//!< 保留字

	UInt16 today_task_list[MAX_PRESTIGE_TASK_NUM_PER_DAY];	//!< 今日任务列表
	char today_star_list[MAX_PRESTIGE_TASK_NUM_PER_DAY];
};

typedef char PrestigeTaskParamHex[sizeof(PrestigeTaskParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(PrestigeTaskParamHex) < 256);

#pragma pack(pop)


#endif