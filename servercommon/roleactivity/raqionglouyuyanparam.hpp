#ifndef __RA_QIONG_LOU_YU_YAN_PARAM_HPP__
#define __RA_QIONG_LOU_YU_YAN_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"

#pragma pack(push, 4)
////////////////////////////////////////////  服务器数据  //////////////////////////////////////////////////////

enum QIONG_LOU_YU_YAN_STATUS_TYPE
{
	QIONG_LOU_YU_YAN_STATUS_TYPE_NEXT_BLANK = 0,		//空白期（24:00 -》 筹办）
	QIONG_LOU_YU_YAN_STATUS_TYPE_NEXT_READY = 1,		//准备期
	QIONG_LOU_YU_YAN_STATUS_TYPE_NEXT_BEGAIN = 2,		//宴会期
	QIONG_LOU_YU_YAN_STATUS_TYPE_NEXT_PREPARATION = 3,	//筹办期
};

enum BANQUET_TYPE
{
	BANQUET_TYPE_NO = 0,	//未开宴会
	BANQUET_TYPE_1 = 1,		//青玉宴
	BANQUET_TYPE_2 = 2,		//琼玉宴
	BANQUET_TYPE_3 = 3,		//瑶玉宴
};

struct RAQiongLouYuYanParam
{
	RAQiongLouYuYanParam() { this->Reset(); }

	void Reset()
	{
		accumulate_num = 0;
		now_status = QIONG_LOU_YU_YAN_STATUS_TYPE_NEXT_BLANK;
		next_status_tamp = 0u;

		banquet_type = BANQUET_TYPE_NO;
		open_times = 0u;
	}

	void OnPartEnd()
	{
		accumulate_num = 0;
		banquet_type = BANQUET_TYPE_NO;
	}

	int accumulate_num;

	int now_status;
	unsigned int next_status_tamp;

	int banquet_type;
	unsigned int open_times;
};

UNSTD_STATIC_CHECK(sizeof(RAQiongLouYuYanParam) <= sizeof(RandActivityData));

////////////////////////////////////////////  个人数据  //////////////////////////////////////////////////////

struct RAQiongLouYuYanRoleParam
{
	RAQiongLouYuYanRoleParam() { this->Reset(); }

	void Reset()
	{
		begin_tamp = 0;
		join_activity = 0;
		reward_flag = 0;
		eat_food_times = 0;
		next_eat_food_tamp = 0;
		open_times = 0;
	}

	void OnPartEnd()
	{
		join_activity = 0;
		reward_flag = 0;
		eat_food_times = 0;
		next_eat_food_tamp = 0;
	}

	unsigned int begin_tamp;
	int join_activity;
	int reward_flag;
	int eat_food_times;
	unsigned int next_eat_food_tamp;
	unsigned int open_times;
};

UNSTD_STATIC_CHECK(sizeof(RAQiongLouYuYanRoleParam) <= sizeof(RoleActivityRoleData));

#pragma pack(pop)


#endif	//__RA_DISCOUNT_STORE_PARAM_HPP__
