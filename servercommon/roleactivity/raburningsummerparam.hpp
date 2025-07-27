#ifndef __RA_BURNING_SUMMER_PARAM_HPP__
#define __RA_BURNING_SUMMER_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/bitmapfunction.hpp"
#include "servercommon/roleactivitydef.hpp"

#pragma pack(push, 4)

static const int MAX_RA_SUMMER_LI_LIAN_BOSS_NUM = 31;				//初夏历练BOSS数量 (一天解锁一个)
static const int MAX_RA_SUMMER_LI_LIAN_MONSTER_NUM = 5;				//初夏历练单个配置首领个数
static const int MAX_RA_COLORFUL_LANTERN_TASK_NUM = 32;				//多彩花灯任务数量
static const int MAX_RA_COLORFUL_LANTERN_LIGHT_TYPE_NUM = 10;		//多彩花灯类型
static const int MAX_RA_SUMMER_STORE_SHOP_ITEM_NUM = 100;			//夏日商店活动内出售商品数量最大值
static const int MAX_RA_SUMMER_STORE_PHASE_SHOP_ITEM_NUM = 30;		//夏日商店阶段内出售商品数量最大值


//初夏历练
UNSTD_STATIC_CHECK(MAX_RA_SUMMER_LI_LIAN_BOSS_NUM <= (int)sizeof(int) * 8);
struct RASummerLiLianParam
{
	RASummerLiLianParam() { this->Reset(); }

	void Reset()
	{
		ra_begin_timestamp = 0;
		open_day = 0;
		is_fetch_all_pass_reward_flag = 0;
		reserve_ch = 0;
		m_first_flag.Reset();
	}

	unsigned int ra_begin_timestamp;
	short open_day;												//活动开启天数
	char is_fetch_all_pass_reward_flag;							//是否领取全通奖励标识 0:未领取 1:已领取
	char reserve_ch;
	BitMap<MAX_RA_SUMMER_LI_LIAN_BOSS_NUM> m_first_flag;		//是否首杀标识  [1,31]
};
UNSTD_STATIC_CHECK(sizeof(RASummerLiLianParam) <= sizeof(RoleActivityRoleData));


//多彩花灯
struct RAColorfulLanternParam
{
	RAColorfulLanternParam() { this->Reset(); }

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
	BitMap<MAX_RA_COLORFUL_LANTERN_TASK_NUM> m_task_list_flag;		//任务列表标识 0:未完成 1:已完成
};
UNSTD_STATIC_CHECK(sizeof(RAColorfulLanternParam) <= sizeof(RoleActivityRoleData));

//夏日商店
struct RASummerStoreParam
{
	RASummerStoreParam() { this->Reset(); }

	void Reset()
	{
		ra_begin_timestamp = 0;
		is_open_today_flag = 0;
		reserve_sh = 0;
		memset(today_buy_times, 0, sizeof(today_buy_times));
		memset(activity_buy_times, 0, sizeof(activity_buy_times));
	}

	unsigned int ra_begin_timestamp;
	short is_open_today_flag;													//今天是否打开过界面 0:没打开 1：打开过
	short reserve_sh;															
	short today_buy_times[MAX_RA_SUMMER_STORE_PHASE_SHOP_ITEM_NUM];				//每日限购
	short activity_buy_times[MAX_RA_SUMMER_STORE_SHOP_ITEM_NUM];				//活动限购
};
UNSTD_STATIC_CHECK(sizeof(RASummerStoreParam) <= sizeof(RoleActivityRoleData));

//好运花签
struct RALuckySignParam
{
	RALuckySignParam() { this->Reset(); }

	void Reset()
	{
		ra_begin_timestamp = 0;
		is_base_show_flag = 0;
		is_fetch_reward_flag = 0;
		sign_id = 0;
		reward_type_seq = 0;
	}
	void DayReset()
	{
		is_base_show_flag = 0;
		is_fetch_reward_flag = 0;
		sign_id = 0;
		reward_type_seq = 0;
	}

	unsigned int ra_begin_timestamp;
	char is_base_show_flag;			//0:签文界面 1:解签界面
	char is_fetch_reward_flag;		//0:未领取	1: 已领取
	short sign_id;
	int reward_type_seq;
};
UNSTD_STATIC_CHECK(sizeof(RALuckySignParam) <= sizeof(RoleActivityRoleData));

#pragma pack(pop)

#endif