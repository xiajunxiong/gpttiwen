#ifndef __RA_COLORFUL_DOUBLE_PARAM_HPP__
#define __RA_COLORFUL_DOUBLE_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"

//纷享加倍
//RAND_ACTIVITY_TYPE_COLORFUL_DOUBLE
/*
#include "other/roleactivity/impl/roleactivitycolorfuldouble.hpp"

	{
		RoleActivityColorfulDouble * role_activity = static_cast<RoleActivityColorfulDouble *>
			(m_role_module_mgr->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_COLORFUL_DOUBLE));
		if (NULL != role_activity)
		{
			role_activity->AddTaskParam(RAColorfulDouble::xxxxx, 1);
		}
	}
*/
namespace RAColorfulDouble
{
	//宝箱数量
	static const int BOX_COUNT = 10;
	//每日任务数
	static const int DAY_TASK_COUNT = 20;

	
	//任务类型
	enum TaskType
	{
		TSAK_TYPE_LOGIN = 1,		//登录				今日登陆
		TSAK_TYPE_YUN_ZE_TERRITORY,	//云泽画境	2147	探索X次
		TSAK_TYPE_ZAI_ZHAN_YUN_ZE,	//再战云泽	2150	击败首领X次
		TSAK_TYPE_YUN_ZE_ER_SAN_SHI,//云泽二三事2153	完成X次
		TSAK_TYPE_YUN_ZE_BLOOMING_RECORD,//云泽繁花录2148	获得X积分
		TSAK_TYPE_MO_ZU_ZAI_NA_LI,	//魔族在哪里2151	击败X次魔族

		TSAK_TYPE_MAX,
	};

};

#pragma pack(push, 4)

struct RAColorfulDoubleParam
{
	RAColorfulDoubleParam()
	{
		this->Reset();
	}

	void Reset()
	{
		ra_begin_timestamp = 0;

		today_free_fetch_flag = 0;
		today_task_finish_flag = 0;

		act_box_rewaed_flag = 0;
		fetch_box_rewaed_flag = 0;

		act_task_finish_num = 0;

		now_day_id = 0;
		hold_ll = 0;

		memset(task_param_list, 0, sizeof(task_param_list));
	}

	unsigned int ra_begin_timestamp;
	int today_free_fetch_flag;		//今日免费奖励领取标记
	UNSTD_STATIC_CHECK(30 > RAColorfulDouble::DAY_TASK_COUNT)
	int today_task_finish_flag;		//今日任务完成标记[任务index]

	UNSTD_STATIC_CHECK(30 > RAColorfulDouble::BOX_COUNT)//第0位空出来
	int act_box_rewaed_flag;		//宝箱奖励激活标记[宝箱seq]
	int fetch_box_rewaed_flag;		//宝箱奖励发放标记[宝箱seq]

	int act_task_finish_num;		//活动任务完成数
	unsigned int now_day_id;		//今日day_id
	int hold_ll;

	//任务参数[任务类型]
	int task_param_list[RAColorfulDouble::TSAK_TYPE_MAX];
};

UNSTD_STATIC_CHECK(sizeof(RAColorfulDoubleParam) <= sizeof(RoleActivityRoleData))

#pragma pack(pop)
#endif