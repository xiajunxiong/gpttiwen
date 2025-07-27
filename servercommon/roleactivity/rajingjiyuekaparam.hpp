#ifndef __RA_JING_JI_YUE_KA_PARAM_HPP__
#define __RA_JING_JI_YUE_KA_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"

#pragma pack(push, 4)

static const int MAX_RA_JING_JI_YUE_KA_DAY_NUM = 30;			//竞技月卡持续天数
static const int MAX_RA_JING_JI_YUE_KA_DAY_TASK_NUM = 20;		//竞技月卡每日任务数量
static const int MAX_RA_JING_JI_YUE_KA_WEEK_TASK_NUM = 20;		//竞技月卡每周任务数量
static const int MAX_RA_JING_JI_YUE_KA_SEASON_TASK_NUM = 20;	//竞技月卡赛季任务数量
static const int MAX_RA_JING_JI_YUE_KA_TASK_TYPE_NUM = 75;		//竞技月卡任务类型数量
static const int MAX_RA_JING_JI_YUE_KA_TASK_GROUP_TYPE_NUM = 3;	//竞技月卡任务组类型数量
static const int MAX_RA_JING_JI_YUE_KA_LEVEL_NUM = 64;			//竞技月卡等级
static const int MAX_RA_JING_JI_YUE_KA_DAY_TASK_REFRESH_NUM = 1;//竞技月卡每日任务可刷新次数
static const int MAX_RA_JING_JI_YUE_KA_TASK_COUNT = 20;		

UNSTD_STATIC_CHECK(MAX_RA_JING_JI_YUE_KA_TASK_COUNT >= MAX_RA_JING_JI_YUE_KA_DAY_TASK_NUM);
UNSTD_STATIC_CHECK(MAX_RA_JING_JI_YUE_KA_TASK_COUNT >= MAX_RA_JING_JI_YUE_KA_WEEK_TASK_NUM);
UNSTD_STATIC_CHECK(MAX_RA_JING_JI_YUE_KA_TASK_COUNT >= MAX_RA_JING_JI_YUE_KA_SEASON_TASK_NUM);

enum RA_JING_JI_YUE_KA_TASK_GROUP_TYPE
{
	RA_JING_JI_YUE_KA_TASK_GROUP_TYPE_BEGIN = 0,
	RA_JING_JI_YUE_KA_TASK_GROUP_TYPE_SEASON =  RA_JING_JI_YUE_KA_TASK_GROUP_TYPE_BEGIN,	//赛季任务
	RA_JING_JI_YUE_KA_TASK_GROUP_TYPE_DAY = 1,		//每日任务
	RA_JING_JI_YUE_KA_TASK_GROUP_TYPE_WEEK = 2,		//每周任务

	RA_JING_JI_YUE_KA_TASK_GROUP_TYPE_MAX
};
UNSTD_STATIC_CHECK(RA_JING_JI_YUE_KA_TASK_GROUP_TYPE_MAX == MAX_RA_JING_JI_YUE_KA_TASK_GROUP_TYPE_NUM);

enum RA_JING_JI_YUE_KA_TASK_TYPE
{
	RA_JING_JI_YUE_KA_TASK_TYPE_BEGIN = 0,
	RA_JING_JI_YUE_KA_TASK_TYPE_CLOUD_ARENA_PAR = RA_JING_JI_YUE_KA_TASK_TYPE_BEGIN,	// 穹顶之争 P1:参与次数
	RA_JING_JI_YUE_KA_TASK_TYPE_CLOUD_ARENA_WIN = 1,			// 穹顶之争	P1:胜利次数		//胜利次数是得自己主动发起战斗计入
	RA_JING_JI_YUE_KA_TASK_TYPE_PLATFORM_BATTLE_PAR = 2,		// 五方擂台	P1:参与次数 
	RA_JING_JI_YUE_KA_TASK_TYPE_PLATFORM_BATTLE_WIN = 3,		// 五方擂台	P1:胜利次数
	RA_JING_JI_YUE_KA_TASK_TYPE_HUAN_JIE_ZHAN_CHANG_PAR = 4,	// 幻界战场	P1:参与次数
	RA_JING_JI_YUE_KA_TASK_TYPE_HUAN_JIE_ZHAN_CHANG_WIN = 5,	// 幻界战场	P1:胜利次数
	RA_JING_JI_YUE_KA_TASK_TYPE_COLOSSENUM_PAR = 6,				// 竞技场	P1:参与次数
	RA_JING_JI_YUE_KA_TASK_TYPE_COLOSSENUM_WIN = 7,				// 竞技场	P1:胜利次数
	RA_JING_JI_YUE_KA_TASK_TYPE_DREAM_NOTES_CHALLENGE = 8,		// 梦渊笔录	P1:通关次数
	RA_JING_JI_YUE_KA_TASK_TYPE_MATERIAL = 9,					// 玉虚境	P1:通关次数
	RA_JING_JI_YUE_KA_TASK_TYPE_PET_GOD_FB = 10,				// 神印之地	P1:通关次数
	RA_JING_JI_YUE_KA_TASK_TYPE_CRYSTAL = 11,					// 秘宝阁	P1:通关次数
	RA_JING_JI_YUE_KA_TASK_TYPE_LOGIN = 12,						// 登陆		P1:登陆天数
	RA_JING_JI_YUE_KA_TASK_TYPE_USE_CRYSTAL_FB_ENERGY = 13,		// 体力消耗	P1:消耗数值
	RA_JING_JI_YUE_KA_TASK_TYPE_BIG_DIPPER = 14,				// 北斗七星	P1:通关次数
	RA_JING_JI_YUE_KA_TASK_TYPE_LUN_HUI_WANG_CHUAN = 15,		// 轮回忘川	P1:通关次数
	RA_JING_JI_YUE_KA_TASK_TYPE_LEARN_PET_SKILL	= 16,			// 学习宠物技能	P1:学习技能书数量 (领悟不算)
	RA_JING_JI_YUE_KA_TASK_TYPE_COLOSSENUM_RANK = 17,			// 竞技场排名	P1:竞技场排名(直接取当前,不管以前最高排名)
	RA_JING_JI_YUE_KA_TASK_TYPE_COURAGE_CHALLENGE_STAR = 18,	// 锢魔之塔	P1:星数(直接取玩家模块累计)
	RA_JING_JI_YUE_KA_TASK_TYPE_ONLINE_TIME = 19,				// 在线时间	P1:在线时间(秒)
	RA_JING_JI_YUE_KA_TASK_TYPE_PET_INTEN = 20,					// 宠物强化	P1:强化次数
	RA_JING_JI_YUE_KA_TASK_TYPE_KILL_WILD_BOSS = 21,			// 击杀野外BOSS	P1:击杀次数
	RA_JING_JI_YUE_KA_TASK_TYPE_BOUNTY_TASK = 22,				// 门派任务	P1:完成天数	(赏金任务)
	RA_JING_JI_YUE_KA_TASK_TYPE_BRAVE_GROUND = 23,				// 勇闯地宫	P1:通关最高层(取本周通关最高)
	RA_JING_JI_YUE_KA_TASK_TYPE_ACTIVE = 24,					// 日常活跃	P1:活跃数值
	RA_JING_JI_YUE_KA_TASK_TYPE_CONSUME_COIN = 25,				// 消耗铜币	P1:消耗数量
	RA_JING_JI_YUE_KA_TASK_TYPE_CONSUME_GOLD = 26,				// 消耗元宝	P1:消耗数量
	RA_JING_JI_YUE_KA_TASK_TYPE_CONSUME_SILVER_COIN = 27,		// 消耗灵玉	P1:消耗数量

	RA_JING_JI_YUE_KA_TASK_TYPE_MAX
};
UNSTD_STATIC_CHECK(MAX_RA_JING_JI_YUE_KA_TASK_TYPE_NUM >= RA_JING_JI_YUE_KA_TASK_TYPE_MAX);

struct RAJingJiYueKaTaskInfo
{
	RAJingJiYueKaTaskInfo() { this->Reset(); }

	void Reset()
	{
		task_id = -1;
		finish_flag = 0;
		refresh_num = 0;
	}

	short task_id;				//对应任务组的任务序号
	char finish_flag;			//0:未完成 1:已完成
	unsigned char refresh_num;	//任务刷新次数
};
UNSTD_STATIC_CHECK(MAX_RA_JING_JI_YUE_KA_DAY_TASK_REFRESH_NUM <= MAX_UINT8);

struct RAJingJiYueKaParam 
{
	RAJingJiYueKaParam () { this->Reset(); }

	void Reset()
	{
		ra_begin_timestamp = 0;
		next_add_login_day_timestamp = 0;
		reserve_ll_1 = 0; 
		active_role_level = 0;
		next_switch_timestamp = 0;
		last_active_role_level = 0;
		last_role_card_level = 0;
		last_switch_timestamp = 0;
		is_active_buy_flag = 0;
		reserve_ch = 0;
		role_card_level = 0;
		role_card_exp = 0;
		fetch_common_flag.Reset();
		reserve_ll_2 = 0;
		fetch_adv_flag.Reset();
		reserve_ll_3 = 0;
		day_task_count = 0;
		week_task_count = 0;
		season_task_count = 0;
		reserve_sh = 0;
		for (int i = 0; i < ARRAY_LENGTH(day_task_list); ++i)
		{
			day_task_list[i].Reset();
		}
		for (int i = 0; i < ARRAY_LENGTH(week_task_list); ++i)
		{
			week_task_list[i].Reset();
		}
		for (int i = 0; i < ARRAY_LENGTH(season_task_list); ++i)
		{
			season_task_list[i].Reset();
		}
		memset(reserve_ll, 0, sizeof(reserve_ll));
		memset(progress_list, 0, sizeof(progress_list));
	}

	void SeasonReset()
	{
		reserve_ll_1 = 0;
		active_role_level = 0;
		next_switch_timestamp = 0;
		is_active_buy_flag = 0;
		reserve_ch = 0;
		role_card_level = 0;
		role_card_exp = 0;
		fetch_common_flag.Reset();
		reserve_ll_2 = 0;
		fetch_adv_flag.Reset();
		reserve_ll_3 = 0;
		day_task_count = 0;
		week_task_count = 0;
		season_task_count = 0;
		reserve_sh = 0;
		for (int i = 0; i < ARRAY_LENGTH(day_task_list); ++i)
		{
			day_task_list[i].Reset();
		}
		for (int i = 0; i < ARRAY_LENGTH(week_task_list); ++i)
		{
			week_task_list[i].Reset();
		}
		for (int i = 0; i < ARRAY_LENGTH(season_task_list); ++i)
		{
			season_task_list[i].Reset();
		}
		memset(reserve_ll, 0, sizeof(reserve_ll));
		memset(progress_list[RA_JING_JI_YUE_KA_TASK_GROUP_TYPE_SEASON], 0, sizeof(progress_list[RA_JING_JI_YUE_KA_TASK_GROUP_TYPE_SEASON]));
	}

	unsigned int ra_begin_timestamp;
	unsigned int next_add_login_day_timestamp;	//下次添加登陆天数时间戳

	long long reserve_ll_1;
	int active_role_level;					//激活该赛季时角色等级
	unsigned int next_switch_timestamp;		//下次切换状态时间戳	

	int last_active_role_level;				//上赛季激活时角色等级
	int last_role_card_level;				//上赛季月卡等级
	unsigned int last_switch_timestamp;		//上次切换状态时间戳
	
	char is_active_buy_flag;				//是否直购激活了额外奖励(江湖引) 0:未激活 1:已激活
	char reserve_ch;
	short role_card_level;					//月卡等级
	int role_card_exp;						//月卡经验

	BitMap<MAX_RA_JING_JI_YUE_KA_LEVEL_NUM> fetch_common_flag;				//普通等级奖励领取标志位
	long long reserve_ll_2;
	BitMap<MAX_RA_JING_JI_YUE_KA_LEVEL_NUM> fetch_adv_flag;					//高级等级奖励领取标志位
	long long reserve_ll_3;
			
	short day_task_count;
	short week_task_count;
	short season_task_count;
	short reserve_sh;									
	RAJingJiYueKaTaskInfo day_task_list[MAX_RA_JING_JI_YUE_KA_DAY_TASK_NUM];
	RAJingJiYueKaTaskInfo week_task_list[MAX_RA_JING_JI_YUE_KA_WEEK_TASK_NUM];
	RAJingJiYueKaTaskInfo season_task_list[MAX_RA_JING_JI_YUE_KA_SEASON_TASK_NUM];

	long long reserve_ll[3];
	
	unsigned int progress_list[MAX_RA_JING_JI_YUE_KA_TASK_GROUP_TYPE_NUM][MAX_RA_JING_JI_YUE_KA_TASK_TYPE_NUM];
};
UNSTD_STATIC_CHECK(sizeof(RAJingJiYueKaParam) <= sizeof(RoleActivityRoleData));


#pragma pack(pop)

#endif