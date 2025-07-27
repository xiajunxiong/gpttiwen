#ifndef __RA_DUAN_YANG_XIANG_NANG_PARAM_HPP__
#define __RA_DUAN_YANG_XIANG_NANG_PARAM_HPP__

#include "servercommon/roleactivitydef.hpp"


#pragma pack(push,4)

static const int DUAN_WU_ACTIVITY_MAX_BITMAP_NUM = 32;

///////////////////////////// 端阳香囊 ///////////////////////////////////
static const int MAX_TASK_NUM_DUAN_YANG_XIANG_NANG = 25;		// 任务种类最大值
struct RADuanYangXiangNangParam
{
	RADuanYangXiangNangParam() { this->Reset(); }
	void Reset()
	{
		ra_begin_timestamp = 0;

		today_task_finish.Reset();
		is_receive_reward.Reset();
		memset(today_task_count, 0, sizeof(today_task_count));
	}

	void DayReset()
	{
		today_task_finish.Reset();
		is_receive_reward.Reset();
		memset(today_task_count, 0, sizeof(today_task_count));
	}

	unsigned int ra_begin_timestamp;

	BitMap<DUAN_WU_ACTIVITY_MAX_BITMAP_NUM> today_task_finish;		// 每日任务是否完成
	BitMap<DUAN_WU_ACTIVITY_MAX_BITMAP_NUM> is_receive_reward;		// 每日任务奖励是否已领取
	int today_task_count[MAX_TASK_NUM_DUAN_YANG_XIANG_NANG];		// 每日任务进度
};
UNSTD_STATIC_CHECK(sizeof(RADuanYangXiangNangParam) <= sizeof(RoleActivityRoleData))


///////////////////////////// 苍龙赐福 ///////////////////////////////////

static const int CLCF_BITMAP_MAX_VALUE = 32;

struct RACangLongCiFuParam
{
	RACangLongCiFuParam() { this->Reset(); }
	void Reset()
	{
		ra_begin_timestamp = 0;
		all_light_reward_flag = 0;

		light_up_flag.Reset();
	}

	unsigned int ra_begin_timestamp;

	int all_light_reward_flag;		// 全部点亮奖励标识, 0--不可领取, 1--可领取, 2--已领取
	BitMap<CLCF_BITMAP_MAX_VALUE> light_up_flag;
};
UNSTD_STATIC_CHECK(sizeof(RACangLongCiFuParam) <= sizeof(RoleActivityRoleData))


///////////////////////////// 兰汤初沐 ///////////////////////////////////
static const int MAX_TASK_NUM_LAN_TANG_CHU_MU = 25;		// 任务种类最大值
static const int MAX_HERBS_TYPE_LAN_TANG_CHU_MU = 4;	// 最大药材种类
struct RALanTangChuMuParam
{
	RALanTangChuMuParam() { this->Reset(); }
	void Reset()
	{
		ra_begin_timestamp = 0;
		last_reward_flag = 0;

		today_task_finish.Reset();
		is_receive_reward.Reset();
		memset(today_task_count, 0, sizeof(today_task_count));
		memset(add_herbs_num, 0, sizeof(add_herbs_num));
	}

	void DayReset()
	{
		last_reward_flag = 0;

		today_task_finish.Reset();
		is_receive_reward.Reset();
		memset(today_task_count, 0, sizeof(today_task_count));
		memset(add_herbs_num, 0, sizeof(add_herbs_num));
	}

	unsigned int ra_begin_timestamp;

	int last_reward_flag;		// 最终奖励, 0--不可领取, 1--可领取, 2--已领取

	BitMap<DUAN_WU_ACTIVITY_MAX_BITMAP_NUM> today_task_finish;		// 每日任务是否完成
	BitMap<DUAN_WU_ACTIVITY_MAX_BITMAP_NUM> is_receive_reward;		// 每日任务奖励是否已领取
	int today_task_count[MAX_TASK_NUM_LAN_TANG_CHU_MU];				// 每日任务进度

	short add_herbs_num[MAX_HERBS_TYPE_LAN_TANG_CHU_MU];	// 已加入药材数量
};
UNSTD_STATIC_CHECK(sizeof(RALanTangChuMuParam) <= sizeof(RoleActivityRoleData))


#pragma pack(pop)

#endif
