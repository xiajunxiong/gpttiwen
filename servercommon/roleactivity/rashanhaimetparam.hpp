#ifndef __RA_SHAN_HAI_MET_PARAM_HPP__
#define __RA_SHAN_HAI_MET_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"

//山海初遇
//RAND_ACTIVITY_TYPE_SHAN_HAI_MET

//活动奖励天数 
static const int SHAN_HAI_MET_DAY_MAX = 10;
/*
#include "other/roleactivity/roleactivitymanager.hpp"
#include "other/roleactivity/impl/roleactivityshanhaimet.hpp"

{//山海初遇
RoleActivityShanHaiMet * role_activity = static_cast<RoleActivityShanHaiMet *>
(m_role_module_mgr->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_SHAN_HAI_MET));
if (NULL != role_activity)
{
role_activity->AddTaskParam(RAShanHaiMet::TSAK_TYPE_SHANHAI_QUESTION, 1);
}
}

*/
namespace RAShanHaiMet
{
	//任务类型
	enum TaskType
	{
		TSAK_TYPE_SUMMER_LI_LIAN = 0,	//2124 初夏历练 p:击败首领数	改成 2155
		TSAK_TYPE_COLORFUL_LANTERN,		//2125 多彩河灯 p:完成活动次数  改成 2160
		TSAK_TYPE_HAPPY_LOOK,			//2134 连连乐翻天 p:参与次数
		TSAK_TYPE_SHANHAI_QUESTION,		//2142 山海问答 p:参与次数
		TSAK_TYPE_INTEGRAL_DRAW,		//2140 整点抽奖(山海风月） p:抽奖次数
		TSAK_TYPE_LOTUS_MI_BAO_ACT_ROW,	//2122 荷香觅宝 p:激活行数		改成 2156
		TSAK_TYPE_LOTUS_MI_BAO_ACT_PAGE,//2122 荷香觅宝 p:激活页数		改成 2156

		TSAK_TYPE_MAX,
	};
	//改需求  全部改为不请清空
	// 任务完成还是按照每日统计
	// 达成条件改为活动期间总次数
	////清空类型--每日
	//static const int DAY_RESET_TASK_[] =
	//{
	//	TSAK_TYPE_SUMMER_LI_LIAN,
	//	TSAK_TYPE_COLORFUL_LANTERN,
	//	TSAK_TYPE_HAPPY_LOOK,
	//	TSAK_TYPE_SHANHAI_QUESTION,
	//	TSAK_TYPE_INTEGRAL_DRAW,
	//};
	//static const int DAY_RESET_TASK_CONUT = ARRAY_ITEM_COUNT(DAY_RESET_TASK_);
};

#pragma pack(push, 4)

//////////////////  个人数据  /////////////////////////////////
struct RAShanHaiMetParam
{
	RAShanHaiMetParam() { this->Reset(); }

	void Reset()
	{
		ra_begin_timestamp = 0;

		act_extra_rewaed_flag = 0;
		day_task_finish_flag = 0;
		fetch_reward_flag = 0;
		fetch_extra_rewaed_flag = 0;

		today_task_finish = 0;
		hold_ll = 0;

		memset(task_param_list, 0, sizeof(task_param_list));
	}
	unsigned int ra_begin_timestamp;

	int act_extra_rewaed_flag;	//额外奖励激活标记

	int day_task_finish_flag;	//任务完成标记(只在领取和跨天时校验)
	int fetch_reward_flag;		//奖励发放标记[活动天数index]
	int fetch_extra_rewaed_flag;//额外奖励发放标记[活动天数index]

	int today_task_finish;		//今日任务完成标记

	long long hold_ll;

	//任务参数[任务类型]
	int task_param_list[RAShanHaiMet::TSAK_TYPE_MAX];
};

UNSTD_STATIC_CHECK(sizeof(RAShanHaiMetParam) <= sizeof(RoleActivityRoleData))

#pragma pack(pop)
#endif	//__RA_SHAN_HAI_MET_PARAM_HPP__
