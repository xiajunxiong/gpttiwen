#ifndef __RA_YINHU_ACTIVITY_PARAM_HPP__
#define __RA_YINHU_ACTIVITY_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"


//////////////////////////////////////////// 寅虎活动 //////////////////////////////////////////
static const int RA_YINHU_WENSHI_DAY_MAX = 6;			// 寅虎问世奖励天数
static const int RA_CINIU_YINGXINHU_TYPE_MAX = 5;		// 辞牛迎新虎 类型
static const int RA_HU_XING_DA_YUN_USER_NUM = 5;		// 虎行大运 被邀请人列表上限

struct RACiNiuYinXinHuTypeData
{
	short reward_mark;
	unsigned short param;
};

struct RAHuXingDaYunBeInvitedData
{
	int uid;
	int prof_id;
	short	avatar_type;							// 体型类型
	short	headshot_id;							// 头像ID	
	GameName role_name;								// 玩家名称
};

#pragma pack(push, 4)
////////////////////////////////////////////  服务器数据  //////////////////////////////////////////////////////



////////////////////////////////////////////  个人数据  //////////////////////////////////////////////////////


struct RAYinHuWenShiParam
{
	RAYinHuWenShiParam() { this->Reset(); }

	void Reset()
	{
		ra_begin_timestamp = 0;
		zhigou_flag = 0;
		reward_fetch_mark = 0;
		memset(reward_type, -1, sizeof(reward_type));
	}

	unsigned int ra_begin_timestamp;
	short zhigou_flag;
	short reward_fetch_mark;

	short reward_type[RA_YINHU_WENSHI_DAY_MAX];
};

UNSTD_STATIC_CHECK(sizeof(RAYinHuWenShiParam) <= sizeof(RoleActivityRoleData))

struct RACiNiuYingXinHuParam
{
	RACiNiuYingXinHuParam() { this->Reset(); }

	void Reset()
	{
		ra_begin_timestamp = 0;
		awaken_mark = 0;
		awaken_reward_time = 0;

		memset(awaken_param, 0, sizeof(awaken_param));
	}

	unsigned int ra_begin_timestamp;

	short awaken_mark;
	short awaken_reward_time;
	RACiNiuYinXinHuTypeData awaken_param[RA_CINIU_YINGXINHU_TYPE_MAX];
};

UNSTD_STATIC_CHECK(sizeof(RACiNiuYingXinHuParam) <= sizeof(RoleActivityRoleData))

struct RAHuXingDaYunParam
{
	RAHuXingDaYunParam() { this->Reset(); }

	void Reset()
	{
		ra_begin_timestamp = 0;
		complete_time = 0;
		task_pos_id = 0;
		task_first_pass = 0;
		is_reward = 0;

		is_be_invited_reward = 0;
		be_invited_times = 0;
		be_invited_complete_time = 0;

		memset(be_invited_list, 0, sizeof(be_invited_list));

		be_invited_bos_id = 0;
	}

	unsigned int ra_begin_timestamp;
	unsigned int complete_time;						// 游戏完成时间戳
	short task_pos_id;								// 当前任务点
	short task_first_pass;							// 是否首通
	char is_reward;									// 是否走到任务点 可以领取奖励

	char is_be_invited_reward;						// 被邀请奖励是否可领取
	short be_invited_times;							// 被邀请次数
	unsigned int be_invited_complete_time;			// 被邀请同行 完成时间戳

	RAHuXingDaYunBeInvitedData be_invited_list[RA_HU_XING_DA_YUN_USER_NUM];

	int be_invited_bos_id;							// 被邀请开始的任务点
};

UNSTD_STATIC_CHECK(sizeof(RAHuXingDaYunParam) <= sizeof(RoleActivityRoleData))

struct RAYinHuBaiSuiParam
{
	RAYinHuBaiSuiParam() { this->Reset(); }

	void Reset()
	{
		ra_begin_timestamp = 0;
		zhigou_flag = 0;
		reward_fetch_mark = 0;
	}

	unsigned int ra_begin_timestamp;
	short zhigou_flag;
	short reward_fetch_mark;
};

UNSTD_STATIC_CHECK(sizeof(RAYinHuBaiSuiParam) <= sizeof(RoleActivityRoleData))

#pragma pack(pop)
#endif	//__RA_YINHU_ACTIVITY_PARAM_HPP__
