#ifndef __ACTIVE_SYSTEM_DEF_HPP__
#define __ACTIVE_SYSTEM_DEF_HPP__

#include "servercommon/servercommon.h"
#include "bitmapfunction.hpp"

#pragma pack(push, 4)

static const int ACTIVE_SYSTEM_LEVEL_GROUP_MAX_NUM = 5;
static const int ACTIVE_SYSTEM_REWARD_MAX_NUM = 2;
static const int ACTIVE_SYSTEM_REWARD_ITEM_MAX_NUM = 10;

enum ACTIVE_SYSTEM_TYPE
{
	ACTIVE_SYSTEM_TYPE_BEGIN = 0,	
	ACTIVE_SYSTEM_TYPE_SCHOOL_TASK, 				// 1  学院任务（门派任务）
	ACTIVE_SYSTEM_TYPE_ZHEN_ZHONG,					// 2  真中的研究(三界除魔)
	ACTIVE_SYSTEM_TYPE_ARRESTING_THIEVES,			// 3  追缉盗贼
	ACTIVE_SYSTEM_TYPE_WST_QUESTION,				// 4  万事通的考验 / 神算子的考验
	ACTIVE_SYSTEM_TYPE_TREVI_FOUNTAIN,				// 5  法兰城许愿池 / 许愿池
	ACTIVE_SYSTEM_TYPE_DEMON_CAVE,					// 6  魔窟（万妖谷)
	ACTIVE_SYSTEM_TYPE_ESCORT,						// 7  运镖
	ACTIVE_SYSTEM_TYPE_BOUNTY_TASK,					// 8  赏金任务
	ACTIVE_SYSTEM_TYPE_TRANSACTION_TASK,			// 9  购买任务 - 丝绸之路
	ACTIVE_SYSTEM_TYPE_FANTASY_FB,					// 10 四象幻境
	ACTIVE_SYSTEM_TYPE_JINGJICHANG,					// 11 参与竞技场
	ACTIVE_SYSTEM_TYPE_COURAGE_CHALLENGE,			// 12 参与锢魔之塔
	ACTIVE_SYSTEM_TYPE_MIYI,						// 13 远征魔冢(试炼之地)
	ACTIVE_SYSTEM_TYPE_ENERGY,						// 14 体力消耗
	ACTIVE_SYSTEM_TYPE_CLOUD_ARENA,					// 15 云端竞技
	ACTIVE_SYSTEM_TYPE_LUNHUIWANGCHUAN,				// 16 轮回忘川
	ACTIVE_SYSTEM_TYPE_DREAM_NOTES_CHALLENGE,		// 17 梦渊笔录
	ACTIVE_SYSTEM_TYPE_SHAN_HAI_BOSS,				// 18 山海降妖
	ACTIVE_SYSTEM_TYPE_CRYSTAL_XIYOU,				// 19 精英试炼
	ACTIVE_SYSTEM_TYPE_NETHER_WORLD_PURGATORY,		// 20 幽冥炼狱
	ACTIVE_SYSTEM_TYPE_PRESTIGE_TASK,				// 21 声望任务
	ACTIVE_SYSTEM_TYPE_BIG_DIPPER,					// 22 北斗七星
	ACTIVE_SYSTEM_TYPE_WAN_LING_FB,					// 23 万灵话本
	ACTIVE_SYSTEM_TYPE_TREASURE_TASK,				// 24 九州秘宝
	ACTIVE_SYSTEM_TYPE_FLY_UP_TASK,					// 25 飞升任务

	ACTIVE_SYSTEM_TYPE_MAX,
};

static const int ACTIVE_SYSTEM_TYPE_MAX_IN_DB = 32;	// 当活跃项大于这个数时，ActiveSystemParam中的数据将出现覆盖现象，需要重置数据再扩展
UNSTD_STATIC_CHECK(26 == ACTIVE_SYSTEM_TYPE_MAX);
UNSTD_STATIC_CHECK(ACTIVE_SYSTEM_TYPE_MAX <= ACTIVE_SYSTEM_TYPE_MAX_IN_DB);

enum ACTIVE_SYSTEM_IS_EVERY_TIME
{
	ACTIVE_SYSTEM_IS_EVERY_TIME_ALL_FINISH = 0,		// 需要完成所有次数才发放积分
	ACTIVE_SYSTEM_IS_EVERY_TIME_SINGLE_FINISH,	    // 单次完成即可发放积分
};

enum ACTIVITY_SYSTEM_SPECIAL_TYPE
{
	ACTIVITY_SYSTEM_SPECIAL_TYPE_BEGIN = 0,
	ACTIVITY_SYSTEM_SPECIAL_TYPE_WILD_BOSS = 1,		// 野外首领

	ACTIVITY_SYSTEM_SPECIAL_TYPE_MAX
};
static const int ACTIVE_SYSTEM_SPECIAL_TYPE_MAX_IN_DB = 32;	// 当活跃项大于这个数时，ActiveSystemParam中的数据将出现覆盖现象，需要重置数据再扩展
UNSTD_STATIC_CHECK(ACTIVITY_SYSTEM_SPECIAL_TYPE_MAX <= ACTIVE_SYSTEM_TYPE_MAX_IN_DB);

struct ActiveSystemParam
{
	ActiveSystemParam(){ this->Reset(); }
	void Reset()
	{
		level = 0;
		integral_active = 0;
		active_flag.Reset();
		type_flag.Reset();
		memset(progress, 0, sizeof(progress));
		type_reward_fetch_flag.Reset();
		add_guess_coin_active_num = 0;
		special_active_reward_flag.Reset();
		memset(reserve_ll, 0, sizeof(reserve_ll));
		memset(can_find_count_list, 0, sizeof(can_find_count_list));
	}

	void DayReset()
	{
		level = 0;
		integral_active = 0;
		active_flag.Reset();
		type_flag.Reset();
		memset(progress, 0, sizeof(progress));
		type_reward_fetch_flag.Reset();
		add_guess_coin_active_num = 0;
	}

	short level;											// 玩家当天第一次登陆的等级
	short integral_active;									// 玩家当前的活跃度
	BitMap<ACTIVE_SYSTEM_LEVEL_GROUP_MAX_NUM> active_flag;	// 活跃领取标志
	BitMap<ACTIVE_SYSTEM_TYPE_MAX_IN_DB> type_flag;				// 对应的类型活跃度全领取完切换标识
	BitMap<ACTIVE_SYSTEM_TYPE_MAX_IN_DB> type_reward_fetch_flag;	// 各种类型获取额外奖励
	int progress[ACTIVE_SYSTEM_TYPE_MAX_IN_DB];					// 各种类型的进度
	int add_guess_coin_active_num;

	BitMap<ACTIVE_SYSTEM_SPECIAL_TYPE_MAX_IN_DB> special_active_reward_flag;	//特殊活跃类型奖励领取标识

	long long reserve_ll[3];
	short can_find_count_list[ACTIVE_SYSTEM_TYPE_MAX_IN_DB];		//活跃可找回次数 
};

typedef char ActiveSystemParamHex[sizeof(ActiveSystemParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(ActiveSystemParamHex) < 512);

//活跃度 任务类型
enum ACTIVE_DEGREE_TYPE
{
	ACTIVE_DEGREE_TYPE_BEGIN = -1,

	ACTIVE_DEGREE_TYPE_ZHEN_ZHONG,				// 0  真中的研究(三界除魔)
	ACTIVE_DEGREE_TYPE_BOUNTY_TASK, 			// 1  赏金任务（门派任务）
	ACTIVE_DEGREE_TYPE_ENERGY,					// 2  体力消耗
	ACTIVE_DEGREE_TYPE_WORLD_BOSS,				// 3  魔将入侵
	ACTIVE_DEGREE_TYPE_WORLD_BOSS_2,			// 4  巨龙来袭
	ACTIVE_DEGREE_TYPE_GUILD_ANSWER,			// 5  家族答题
	ACTIVE_DEGREE_TYPE_GUILD_FIGHT,				// 6  家族大乱斗
	ACTIVE_DEGREE_TYPE_TEAM_FIGHT,				// 7  竞技对抗赛
	ACTIVE_DEGREE_TYPE_MAZE_RACER,				// 8  迷宫竞速
	ACTIVE_DEGREE_TYPE_ONLY_FIGHT,				// 9  以一敌百
	ACTIVE_DEGREE_TYPE_GUILD_HONOR_BATTLE,		// 10 家族荣誉战
	ACTIVE_DEGREE_TYPE_CHIEF_ELECTION,			// 11 首席竞选
	ACTIVE_DEGREE_TYPE_HUNDRED_GHOST,			// 12 百鬼夜行
	ACTIVE_DEGREE_TYPE_BRAVE_GROUND,			// 13 勇闯地宫
	ACTIVE_DEGREE_TYPE_SILK_ROADS,				// 14 丝绸之路
	ACTIVE_DEGREE_TYPE_RELIC_LOOTING,			// 15 遗迹夺宝

	ACTIVE_DEGREE_TYPE_MAX,
};


static const int MAX_ACTIVE_DEGREE_WEEK_REWARD_NUM = 10;
static const int MAX_ACTIVE_DEGREE_TASK_NUM = 30;

// 活跃转盘 任务类型
enum ACTIVE_TURNTABLE_TYPE
{
	ACTIVE_TURNTABLE_TYP_BEGIN = 0,
	ACTIVE_TURNTABLE_TYPE_JOIN_ACTIVITY,		// 1 参与限时活动 参数:活动ID
	ACTIVE_TURNTABLE_TYPE_WEEK_ACTIVE,			// 2 周活跃达到一定值 参数:周活跃值

	ACTIVE_TURNTABLE_TYPE_MAX,
};


struct ActiveDegreeBaseParam
{
	ActiveDegreeBaseParam() { this->Reset(); }
	void Reset()
	{
		hold_ll = 0;
		week_fetch_flag.Reset();
		week_task_today_finish_flag.Reset();
		week_active_degree = 0;
	}

	long long hold_ll;
	UNSTD_STATIC_CHECK(BIT_COUNT_OF_LONG_64 > MAX_ACTIVE_DEGREE_WEEK_REWARD_NUM);
	UNSTD_STATIC_CHECK(BIT_COUNT_OF_LONG_64 > MAX_ACTIVE_DEGREE_TASK_NUM);
	UNSTD_STATIC_CHECK(BIT_COUNT_OF_LONG_64 > ACTIVE_DEGREE_TYPE_MAX);

	BitMap<BIT_COUNT_OF_LONG_64> week_fetch_flag;		//周-活跃奖励领取标记[奖励index]
	BitMap<BIT_COUNT_OF_LONG_64> week_task_today_finish_flag;	//周-每日任务达成标记[任务index]
	int week_active_degree;			//周活跃数
};

struct ActiveDegreeTaskParam
{
	ActiveDegreeTaskParam() { this->Reset(); }
	void Reset()
	{
		hold_ll = 0;
		memset(task_info_list, 0, sizeof(task_info_list));
	}

	struct TaskInfo
	{
		int task_param;		//任务完成参数
		int hold_i;			//占位
	};
	long long hold_ll;

	//任务信息[任务类型]
	TaskInfo task_info_list[ACTIVE_DEGREE_TYPE_MAX];

};

struct ActiveTurntableParam
{
	ActiveTurntableParam() { this->Reset(); }
	void Reset()
	{
		turntable_time = 0;
		turntable_day_task_flag.Reset();
		turntable_week_task_flag.Reset();
	}

	int turntable_time;												// 活跃转盘次数
	BitMap<BIT_COUNT_OF_LONG_64> turntable_day_task_flag;			// 活跃转盘 每日任务完成标记
	BitMap<BIT_COUNT_OF_LONG_64> turntable_week_task_flag;			// 活跃转盘 每周任务完成标记
};

typedef char ActiveDegreeBaseParamHex[sizeof(ActiveDegreeBaseParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(ActiveDegreeBaseParamHex) < 128);

typedef char ActiveDegreeTaskParamHex[sizeof(ActiveDegreeTaskParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(ActiveDegreeTaskParamHex) < 384);

typedef char ActiveTurntableParamHex[sizeof(ActiveTurntableParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(ActiveTurntableParamHex) < 128);

#pragma pack(pop)

#endif