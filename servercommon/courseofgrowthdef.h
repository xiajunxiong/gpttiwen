#ifndef __COURSE_OF_GROWTH_DEF_HPP__
#define __COURSE_OF_GROWTH_DEF_HPP__

#include "servercommon/servercommon.h"
#include "bitmapfunction.hpp"

#pragma pack(push, 4)

static const int GROWTH_TASK_NUM_PER_STAR = 9;		// 成长之路 - 每星级的任务数
static const int GROWTH_KEY_MULT = 10000;			// 键值倍率
UNSTD_STATIC_CHECK(GROWTH_TASK_NUM_PER_STAR > BIT_COUNT_OF_BYTE && GROWTH_TASK_NUM_PER_STAR <= BIT_COUNT_OF_BYTE * 2)

static const int GROWTH_PROG_BEGINNING = 1;
static const int GROWTH_STAR_BEGINNING = 1;

static const int MAX_COG_RECORD_NUM = 511;
static const int MAX_COG_RECORD_LEN = sizeof(short) + (sizeof(int) + sizeof(int)) * MAX_COG_RECORD_NUM;

static const int MAX_COURSE_ENTRY_LIMIT_NUM = 1504;		// 历程标签条目数量
static const int COURSE_ENTRY_KEY_MULT = 1000000;		// 历程键值倍率
static const int MAX_COURSE_LEVEL_NUM = 64;				// 历程最高等级
static const int MAX_CHANNEL_CHAT_TODAY_COUNT = 5;		// 频道聊天每个类型每天最多次数

enum CourseOfGrowthCondition
{
	COG_CON_WIN_FIGHT_TIMES_ACC = 0,			// √ 累计胜利次数	|	 /	  | 指定次数		| 存
	COG_CON_REACHS_PRESTIGE_NUM = 1,			// √ 到达声望数值	| 指定城镇 | 指定数量		| 存
	COG_CON_PASS_CRYSTAL_ECTYPE = 2,			// √ 通关水晶副本	|	 /	  | 指定数量		| 存
	COG_CON_OBTAIN_COIN_NUMBERS = 3,			// √ 获取铜币数量	|	 /	  | 指定数量		| 存
	COG_CON_PET_TRAPS_CREATURES = 4,			// √ 成功封印宠物	|	 /	  | 指定次数		| 存
	COG_CON_DEFEAT_RACE_MONSTER = 5,			// √ 击败种族妖兽	| 种族类型 | 指定数量		| 存
	COG_CON_REACHS_FB_TIER_NUMB = 6,			// √ 到达副本层数	| 副本类型 | 目标层数		| 存
	COG_CON_OBTAIN_TITLE_NUMBER = 7,			// √ 获得称号数量	|	 /	  | 指定数量		| 存
	COG_CON_DEFEAT_BOSS_MONSTER = 8,			// √ 击败BOSS怪	| BOSS类型 | 指定次数		| 存
	COG_CON_STUDY_BATTLE_SKILLS = 9,			// √ 习得战斗技能	|	 /	  | 指定数量		| 存
	COG_CON_SKILLS_REACHS_LEVEL = 10,			// √ 技能等级到达	| 目标等级 | 指定数量		| 
	COG_CON_PUT_ON_EQUIPM_TIMES = 11,			// √ 穿戴装备件数	|	 /	  | 指定数量		| 存
	COG_CON_PET_ALLOT_ATTRPOINT = 12,			// √ 分配宠物点数	|	 /	  | 指定数量		| 存
	COG_CON_PET_OBTAIN_RACE_NUM = 13,			// √ 获得种族宠物	| 种族类型 | 指定数量		| 存
	COG_CON_PET_NUM_REACH_LEVEL = 14,			// √ 宠物等级到达	| 目标等级 | 指定数量		| 
	COG_CON_PET_INTENSIFY_TIMES = 15,			// √ 宠物强化次数	|	 /	  | 指定次数		| 存
	COG_CON_PET_REPLACE_TALENTS = 16,			// √ 宠物替换天赋	|	 /	  | 指定次数		| 存
	COG_CON_ACCOMPLISH_ACTIVITY = 17,			// √ 完成指定活动	| 活动类型 | 指定次数		| 存
	COG_CON_ACTIVATE_COLLECTION = 18,			// √ 累计激活图鉴	|	 /	  | 指定数量		| 存
	COG_CON_USE_APPOINTED_ITEMS = 19,			// √ 使用类型道具	| 道具类型 | 指定次数		| 存
	COG_CON_USE_NOREXITEM_TYPES = 20,			// √ 使用消耗道具	| 消耗类型 | 指定次数		| 存
	COG_CON_REACHS_APPOINTED_LV = 21,			// √ 到达指定等级	| 目标等级				| 
	COG_CON_APPOINTED_TASK_DONE = 22,			// √ 完成指定任务	| 任务编号				| 
	COG_CON_CARRY_OUT_TYPE_TASK = 23,			// √ 完成类型任务	| 任务类型 | 指定次数		| 存 p1(COG_TASK_TYPE_MAX)
	COG_CON_WORKSHOP_CRAFT_ITEM = 24,			// √ 工坊制作物品	| 制造类型 | 指定数量		| 存
	COG_CON_GATHER_TIMES_ACCUMU = 25,			// √ 累计采集次数	| 采集类型 | 指定数量		| 存
	COG_CON_TRADEMARKET_PUTAWAY = 26,			// √ 拍卖上架道具	|	 /	  | 指定次数		| 存
	COG_CON_TRADEMARKET_DEALING = 27,			// √ 拍卖交易次数	|	 /	  | 指定次数		| 存
	COG_CON_FINISHED_TASK_CHAIN = 28,			// √ 完成任务链条	|	 /	  | 指定次数		| 存
	COG_CON_EXECUTES_SEAL_ORDER = 29,			// √ 执行封妖指令	|	 /	  | 指定次数		| 存
	COG_CON_CONSUME_DOUBLE_EXP  = 30,			// √ 消耗双倍次数	|	 /	  | 指定次数		| 存
	COG_CON_PET_SCOUR_TALENTS   = 31,			// √ 宠物洗练次数	|	 /	  | 指定次数		| 存
	COG_CON_PARTNER_UP_LEVEL    = 32,           // √ 累计伙伴进阶| 伙伴阶数 | 指定数量		| 存
	COG_CON_PET_PRACTICE_LEVEL	= 33,           // √ 宠物修炼等级| 修炼等级 |  		    | 存
	COG_CON_BOUNTY_TASK_DONE	= 34,           // √ 赏金任务次数|    /    | 指定次数		| 存
	COG_CON_MIYI_KILL_LAYER_NUM = 35,			// √ 远征层数次数| 远征层数 | 首通次数		| 存
	COG_CON_KILL_MONTER_GROUP   = 36,			// √ 首杀怪物组  | 怪物组ID | 是否击杀    | 存
	COG_CON_PET_MONOPOLY_LEVEL  = 37,			// √ 灵兽岛层数  |    /    |  指定层数   | 存
	COG_CON_TRAIN_SKILL_LEVEL	= 38,			// √ 人物修炼技能| 技能等级 |  指定数量   | 存
	COG_CON_LUN_HUI_WANG_CHUAN	= 39,			// √ 通关轮回忘川|    /    |  指定次数   | 存
	COG_CON_SHAN_HAI_XIANG_YAO	= 40,			// √ 通关山海降妖|    /    |  指定次数   | 存
	COG_CON_REALM_BREAK_LEVEL	= 41,			// √ 角色境界等级| 境界等级 |			   | 存
	COG_CON_PET_INTENSIFY_LEVEL = 42,			// √ 宠物强化等级	| 宠物品质 |  指定等级   | 存	 param1(0:表示没有品质限制)
	COG_CON_EQUIPMENT_SMELT		= 43,			// √ 角色装备熔炼|    /    |	 指定次数   | 存
	COG_CON_CRYSTAL_FB_XI_YOU	= 44,			// √ 精英试炼通关| 指定关卡 |		/	   | 存
	COG_CON_PASS_CRYSTAL_XI_YOU = 45,			// √ 精英试炼通关|    /    |	 指定次数   | 存
	COG_CON_REAML_BREAK_FIGHT	= 46,			// √ 境界心灵之境| 指定关卡 |		/	   | 存
	COG_CON_REACH_CAPABILITY	= 47,			// √ 达到指定评分| 评分类型 |	 指定数值   | 存
	COG_CON_GET_QUALITY_PET     = 48,			// √ 获得品质宠物| 宠物品质 |	 指定数量   | 存 param1(0:表示没有品质限制)
	COG_CON_SCOUR_TALENTS_MUT	= 49,			// √ 累计洗练变异| 宠物品质 |	 指定数量   | 存 param1(0:表示没有品质限制)
	COG_CON_HAS_FA_BAO_COUNT	= 50,			// √ 拥有法宝数量| 法宝品质 |	 指定数量   | 存 param1(0:表示没有品质限制)
	COG_CON_HAS_ACTIVE_FACE		= 51,			// √ 拥有颜值外观| 外观类型 |	 指定数量   | 存 param1(-1:表示没有品质限制)
	COG_CON_HAS_SURFACE			= 52,			// √ 拥有时装数量| 外观类型 |	 指定数量   | 存 param1(0:表示没有品质限制)
	COG_CON_GET_MONEY			= 53,			// √ 累计获得货币| 货币类型 |	 指定数量   | 存
	COG_CON_FIRST_ADD_GUILD     = 54,			// √ 首次加入家族|     /   |	 指定次数   | 存
	COG_CON_GUILD_TEAM_FIGHT	= 55,			// √ 成员组队战斗|     /   |	 指定次数   | 存
	COG_CON_PARTICIPATE_ACT		= 56,			// √ 参与限时活动| 活动类型 |	 指定次数   | 存
	COG_CON_ACTIVITY_FIGHT_WIN	= 57,			// √ 活动战斗胜利| 活动类型 |	 指定次数   | 存
	COG_CON_GUILD_FIGHT_RANK	= 58,			// √ 家族乱斗排名|   名次   |	    /      | 存
	COG_CON_GUILD_HONOR_RANK	= 59,			// √ 家族荣誉战  |   名次   |	    /      | 存
	COG_CON_FETCH_GUILD_HONGBAO = 60,			// √ 领取家族红包|    /     |　指定次数  | 
	COG_CON_GUILD_ANSWER		= 61,			// √ 答对家族答题|    /     |　指定次数  | 
	COG_CON_CHANNEL_CHAT		= 62,			// √ 累计频道聊天| 频道类型  |　指定次数  | 
	COG_CON_FETCH_FRIEND_GIFT	= 63,			// √ 领取好友赠礼|	 /	   |　指定次数  | 存
	COG_CON_HAS_FRIEND_COUNT	= 64,			// √ 拥有好友数量| 亲密度数量|　指定数量  | 
	COG_CON_JIE_YI_DAY_NUM		= 65,			// √ 金兰结义天数|    /     |　指定天数  | 存
	COG_CON_USE_HIGH_TREASURE   = 66,			// √ 累计挖藏宝图| 宝图类型  |　指定次数  | 存
	COG_CON_WIN_CHIEF_ELECTION  = 67,			// √ 夺得门派首席|	 /	   |　指定次数  | 存
	COG_CON_CLOUD_ARENA_SCORE	= 68,			// √ 穹顶之争段位|	 /	   |　指定积分  | 存
	COG_CON_APPRENTICE_COUNT    = 69,			// √ 玩家徒弟数量|    /     |　指定数量  | 
	COG_CON_APPRENTICE_DAY_NUM	= 70,			// √ 玩家拜师天数|    /     |　指定天数  | 存
	COG_CON_YIN_YANG_EYE_REWARD = 71,			// √ 阴阳眼获奖励|    /     |　指定次数  | 存
	COG_CON_USE_CRYSTAL_ENERGY	= 72,			// √ 累计消耗体力|    /     |　指定数量  | 存
	COG_CON_SHAN_HAI_BOSS		= 73,			// √ 累计击败山海| 难度类型  |  指定数量  | 存
	COG_CON_HUAN_JIE_ZHAN_CHANG = 74,			// √ 幻界战场段位|    /     |  指定积分  | 存
	COG_CON_ANECDOTE_TASK		= 75,			// √ 完成奇闻任务| 奇闻类型  |    /      | 
	COG_CON_CHIVALROUS			= 76,			// √ 累积获取侠义|    /     |  指定数量  | 

	COG_CON_TYPE_MAX
};

enum COG_CON_REACH_CAPABILITY_TYPE
{
	COG_CON_REACH_CAPABILITY_TYPE_COUNT = 0,	//总评分
	COG_CON_REACH_CAPABILITY_TYPE_ROLE = 1,		//角色评分

	COG_CON_REACH_CAPABILITY_TYPE_MAX
};

enum COGBossType
{
	COG_BOSS_WORLD = 0,		// 世界BOSS
	COG_BOSS_WILD = 1,		// 野外BOSS

	COG_BOSS_TYPE_MAX
};

enum COG_GATHER_TYPE
{
	COG_GATHER_TYPE_MINING = 0,			// 挖矿
	COG_GATHER_TYPE_LUMBER = 1,			// 伐木
	COG_GATHER_TYPE_FLOWER = 2,			// 采花
	COG_GATHER_TYPE_COUNT  = 3,			// 挖矿+伐木+采花

	COG_GATHER_TYPE_MAX
};

enum COGAppointedActivity
{
	COG_ACT_TYPE_WAN_SHI_TONG = 0,			// 万事通
	COG_ACT_TYPE_WISHING_POOL = 1,			// 许愿池
	COG_ACT_TYPE_ESCORT_CARGO = 2,			// 运镖
	COG_ACT_TYPE_ZHENZHO_TASK = 3,			// 真中的研究(三界除魔)
	// COG_ACT_TYPE_MONST_VALLEY = 4,			// 讨伐魔窟(万妖谷)[已经废弃现在变成了4 = 话本]
	COG_ACT_TYPE_HUA_BEN = 4,				// 话本
	COG_ACT_TYPE_MAX
};

enum COGAppointedTaskType
{
	COG_TASK_TYPE_PROF_TASK = 0,		// 门派任务 / 职业任务
	COG_TASK_TYPE_PRESTIGE_TASK = 1,	// 声望任务
	COG_TASK_TYPE_ADVENTURES_TASK = 2,	// 奇遇任务

	COG_TASK_TYPE_MAX
};

enum COGWorkshopMakesType		// 制造类型
{
	COG_WORKSHOP_MAKE_EQUIP = 0,		// 打造装备
	COG_WORKSHOP_MAKE_POTION = 1,		// 制作药剂
	COG_WORKSHOP_MAKE_BOOKS = 2,		// 合成技能书
	COG_WORKSHOP_MAKE_CARD = 3,			// 合成封妖卡

	COG_WORKSHOP_MAKE_TYPE_MAX
};

struct CourseOfGrowthParam
{
	CourseOfGrowthParam() { this->Reset(); }

	void Reset()
	{
		cur_prog = (UInt16)GROWTH_PROG_BEGINNING;
		cur_star = (UInt16)GROWTH_STAR_BEGINNING;
		cur_reward_prog = (UInt16)GROWTH_PROG_BEGINNING;
		cur_reward_star = (UInt16)GROWTH_STAR_BEGINNING;

		award_fetch_flag.Reset();
		finish_notify_flag.Reset();

		memset(reserve_ll, 0, sizeof(reserve_ll));
		is_give_base_pass_times_flag = 0;
		reserve_ch = 0;
		reserve_sh = 0;
	}

	UInt16 cur_prog;
	UInt16 cur_star;
	UInt16 cur_reward_prog;
	UInt16 cur_reward_star;

	BitMap<GROWTH_TASK_NUM_PER_STAR> award_fetch_flag;
	BitMap<GROWTH_TASK_NUM_PER_STAR> finish_notify_flag;

	long long reserve_ll[3];
	char is_give_base_pass_times_flag;				// 给玩家发放基础数据标识
	char reserve_ch;
	short reserve_sh;
};
typedef char CourseOfGrowthParamHex[sizeof(CourseOfGrowthParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(CourseOfGrowthParamHex) < 256);

struct RoleCourseParam
{
	RoleCourseParam() { this->Reset(); }

	void Reset()
	{
		memset(reserve_ll, 0, sizeof(reserve_ll));

		course_fraction_count = 0;
		finish_record_flag.Reset();
		level_reward_fetch_flag.Reset();
		memset(course_entry_version_list, 0, sizeof(course_entry_version_list));
		memset(course_level_version_list, 0, sizeof(course_level_version_list));
	}

	long long reserve_ll[3];

	int course_fraction_count;												// 总历程点
	BitMap<MAX_COURSE_ENTRY_LIMIT_NUM> finish_record_flag;					// 历程条目完成记录
	BitMap<MAX_COURSE_LEVEL_NUM> level_reward_fetch_flag;					// 历程等级奖励领取记录
	unsigned char course_entry_version_list[MAX_COURSE_ENTRY_LIMIT_NUM];	// 历程条目版号记录
	unsigned char course_level_version_list[MAX_COURSE_LEVEL_NUM];			// 等级奖励领取版号
};
typedef char RoleCourseParamHex[sizeof(RoleCourseParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(RoleCourseParamHex) < 4096);

#pragma pack(pop)
#endif  // __COURSE_OF_GROWTH_DEF_HPP__