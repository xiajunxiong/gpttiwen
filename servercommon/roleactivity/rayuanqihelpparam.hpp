#ifndef __RA_YUAN_QI_HELP_PARAM_HPP__
#define __RA_YUAN_QI_HELP_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"
#include "servercommon/bitmapfunction.hpp"

#pragma pack(push, 4)

const static int MAX_YUAN_QI_HELP_MAX_TASK_LIST = 10;

struct RAYuanQiHelpParam
{
	RAYuanQiHelpParam() { this->Reset(); }

	void Reset()
	{
		ra_begin_timestamp = 0;
		today_play_times = 0;
		task_id = 0;
		pre_task_id = 0;
		memset(pre_task_id_list, 0, sizeof(pre_task_id_list));
	}

	void DayReset()
	{
		today_play_times = 0;
		memset(pre_task_id_list, 0, sizeof(pre_task_id_list));
		pre_task_id = 0;
		task_id = 0;
	}

	void PlayEnd()
	{
		task_id = 0;
	}

	unsigned int ra_begin_timestamp;
	int today_play_times;
	int task_id;
	int pre_task_id;
	int pre_task_id_list[MAX_YUAN_QI_HELP_MAX_TASK_LIST];
	
};
UNSTD_STATIC_CHECK(sizeof(RAYuanQiHelpParam) <= sizeof(RoleActivityRoleData));

#pragma pack(pop)

#endif