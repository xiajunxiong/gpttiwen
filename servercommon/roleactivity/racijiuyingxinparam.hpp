#ifndef __RA_CI_JIU_YING_XIN_PARAM_HPP__
#define __RA_CI_JIU_YING_XIN_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"

#pragma pack(push, 4)
////////////////////////////////////////////  服务器数据  //////////////////////////////////////////////////////


////////////////////////////////////////////  个人数据  //////////////////////////////////////////////////////

struct RACiJiuYingXinParam
{
	RACiJiuYingXinParam() { this->Reset(); }

	void Reset()
	{
		open_day = 0;
		today_answer = 0;
		id = 0;
		has_answer_count = 0;
		memset(has_answer_list, 0, sizeof(has_answer_list));
		begin_time = 0;
	}

	void OnDayReset()
	{
		today_answer = 0;
		id = 0;
		has_answer_count = 0;
		memset(has_answer_list, 0, sizeof(has_answer_list));
	}

	int open_day;
	int today_answer;
	int id;
	int has_answer_count;
	int has_answer_list[2];
	unsigned int begin_time;
};

UNSTD_STATIC_CHECK(sizeof(RACiJiuYingXinParam) <= sizeof(RoleActivityRoleData))

#pragma pack(pop)


#endif	//__RA_DISCOUNT_STORE_PARAM_HPP__
