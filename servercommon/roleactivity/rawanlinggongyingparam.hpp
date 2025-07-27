#ifndef __RA_WAN_LING_GONG_YING_PARAM_HPP__
#define __RA_WAN_LING_GONG_YING_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"


//////////////////////////////////////////// 万灵共盈 //////////////////////////////////////////
const static int RA_WAN_LING_GONG_YING_TASK_MAX = 10;				// 任务上限


#pragma pack(push, 4)
////////////////////////////////////////////  服务器数据  //////////////////////////////////////////////////////


////////////////////////////////////////////  个人数据  //////////////////////////////////////////////////////
struct RAWLGYTaskData
{
	RAWLGYTaskData() : task_flag(0), param1(0), param2(0) {}

	short task_flag;
	char param1;
	char param2;
};


struct RAWanLingGongYingParam
{
	RAWanLingGongYingParam() { this->Reset(); }

	void Reset()
	{
		ra_begin_timestamp = 0;
		ra_buy_reward_timestamp = 0;
		
		memset(task_list, 0, sizeof(task_list));
	}

	unsigned int ra_begin_timestamp;
	unsigned int ra_buy_reward_timestamp;

	RAWLGYTaskData task_list[RA_WAN_LING_GONG_YING_TASK_MAX];
};

UNSTD_STATIC_CHECK(sizeof(RAWanLingGongYingParam) <= sizeof(RoleActivityRoleData))

#pragma pack(pop)
#endif	//__RA_WAN_LING_GONG_YING_PARAM_HPP__
