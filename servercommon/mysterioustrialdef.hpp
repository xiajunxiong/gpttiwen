#ifndef __MYSTERIOUS_STRIAL_DEF_HPP__
#define __MYSTERIOUS_STRIAL_DEF_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/bitmapfunction.hpp"


//-----------------------------神秘试炼(模块功能)------------------------------------
static const int RA_MYSTERIOUS_TRIAL_MAX_DAY_NUM = 7;								//奖励天数
static const int RA_MYSTERIOUS_TRIAL_DAY_TASK_MAX_NUM = 5;							//一天的任务数
static const int RA_NEW_MYSTERIOUS_TRIAL_DAY_TASK_MAX_NUM = 7;						//新一天的任务数
static const int RA_MYSTERIOUS_TRIAL_FINALLY_TASK_MAX_NUM = 5;						//最终任务数量
static const int RA_MYSTERIOUS_TRIAL_FINALLY_MAX_NUM = 32;							//最终任务红点标识
static const int RA_MYSTERIOUS_TRIAL_OPEN_DAY_MAX_NUM = 7;							//持续的天数
static const int RA_MYSTERIOUS_TRAIL_CMD_BUY_START_NUM = 10;						//直购起始 + day(start_time)	 兼容外网老玩家数据
UNSTD_STATIC_CHECK(RA_MYSTERIOUS_TRIAL_OPEN_DAY_MAX_NUM <= RA_MYSTERIOUS_TRAIL_CMD_BUY_START_NUM);		

enum MysteriousStrialCondition
{
	 MSC_CON_LOGIN_DAY    = 0,				// √ 累计登陆天数	|   累积   | 指定天数		| 存
	 MSC_CON_ROLE_LEVEL   = 1,				// √ 玩家当前等级	| 直接判断 | 指定等级		| 存
	 MSC_CON_EQPM_FORG    = 2,				// √ 玩家装备锻造	|   累积   | 指定次数		| 大于等于品阶(0:不限制品质) | 大于等于等级(0不限制等级) |存
	 MSC_CON_PROF_PRO     = 3,				// √ 玩家职业进阶	| 直接判断 | 指定级别		| 存
	 MSC_CON_PET_INTEN	  = 4,				// √ 玩家强化宠物	| 直接判断 | 指定级别		| 大于等于品质(0:不限制品质) | 存
	 MSC_CON_COUR_CHAG    = 5,				// √ 锢魔之塔星数| 直接判断 | 指定星数		| 指定等级 |存
	 MSC_CON_KILL_BOSS    = 6,				// √ 击杀BOSS数量| 单次计算 | 指定数量		| 存(限定野外)
	 MSC_CON_COMP_TREA    = 7,				// √ 参加竞技宝库	|   累积   | 指定数量		| 存
	 MSC_CON_COMPLETE_FB  = 8,				// √ 完成任意副本	|   累积   | 指定次数		| 存
	 MSC_CON_PARTNER_NUM  = 9,				// √ 解锁伙伴数量	| 直接判断 | 指定数量		| 存
	 MSC_CON_PET_CAPABIL  = 10,				// √ 玩家宠物战力	| 直接判断 | 指定战力		| 存
	 MSC_CON_GEMS_MOSAIC  = 11,				// √ 玩家宝石镶嵌| 直接判断 | 指定数量		| 达到品阶 |存
	 MSC_CON_ONLINE_TIME  = 12,				// √ 玩家在线时长|   累积   | 指定时长		| 存
	 MSC_CON_PARTNER_PRO  = 13,				// √ 玩家伙伴进阶| 直接判断 | 指定数量		| 指定阶数 |存
	 MSC_CON_JOIN_FAMILY  = 14,				// √ 玩家加入家族	| 直接判断 | 指定次数		| 存
	 MSC_CON_TEMP_COMP_FB = 15,				// √ 组队完成副本	|   累积   | 指定次数		| 存
	 MSC_CON_PET_USE_BOOK = 16,				// √ 宠物用技能书	|   累积   | 指定次数		| 大于等于宠物品质(0:不限制品质) | 大于等于书等级(0:不限制品质) |存
	 MSC_CON_TRIAL_BOSS   = 17,				// √ 试炼之地首领	|   累积   | 指定次数		| 存
	 MSC_CON_TRA_MAR_PUR  = 18,				// √ 交易行买或卖	|   累积   | 指定次数		| 存
	 MSC_CON_KILL_YW_BOSS = 19,				// √ 击杀野外BOSS|   累积   | 指定次数	| 存(限定野外)
	 MSC_CON_USER_CAPABIL = 20,				// √ 玩家人物评分	| 直接判断 | 指定战力		| 存
	 MSC_CON_ACT_COLLECT  = 21,				// √ 玩家激活图鉴	|   累积   | 指定数量		| 存
	 MSC_CON_SHOP_CONSUME = 22,				// √ 玩家商会消费	|   累积   | 指定次数		| 存
	 MSC_CON_JOIN_XIYOUFB = 23,				// √ 参与稀有副本	|   累积   | 指定次数		| 存

	 MSC_CON_COURAGE_CHALLENGE = 24,		// √ 锢魔之塔星数	| 直接判断 | 指定数量		| 存
	 MSC_CON_COLOSSEUM = 25,				// √ 竞技场排名	| 直接判断 | 指定名次		| 存
	 MSC_CON_CATCH_PET = 26,				// √ 获得一只宠物	| 直接判断 | 指定品质		| 存
	 MSC_CON_ROLE_SKILL_LEVEL = 27,			// √ 角色技能升级	| 直接判断 | 指定等级		| 存
	 MSC_CON_GET_MEDAL = 28,				// √ 穿戴令牌	| 直接判断 | 指定数量		| 指定品质		| 存
	 MSC_CON_PARTNER_FETTERS = 29,			// √ 伙伴羁绊数量	| 直接判断 | 指定数量		| 存
	 MSC_CON_MI_YI = 30,					// √ 通关远征魔冢	| 直接判断 | 固定1		| 指定层数 |存
	 MSC_CON_SPECIAL_PET_INTEN = 31,		// √ 特殊宠物强化	| 直接判断 | 指定品质		| 达到强化等级	| 存
	 MSC_CON_PRESTIG_LEVEL = 32,			// √ 村庄声望	| 直接判断 | 指定等级		| 存
	 MSC_CON_PET_SKILL_NUM = 33,			// √ 宠物技能数量	| 直接判断 | 指定数量		| 存
	 MSC_CON_JOIN_XIYOUFB_LEVEL = 34,		// √ 稀有副本指定关卡	|  直接等于  | 指定数量		| 存
	 MSC_CON_GET_FA_BAO = 35,				// √ 角色获得法宝| 直接判断 | 指定数量    | 指定等级  | 指定品质(0:不限) | 存 
	 MSC_CON_UNLOCK_SMART_MOUNTS = 36,		// √ 解锁或进阶灵骑| 直接判断 | 指定数量(次数) | 0:解锁 1:进阶 | 存
	 MSC_CON_ACT_COLLECT_QUAILTY = 37,		// √ 激活品质图鉴| 直接判断 |　指定数量　 | 指定品质(0:不限) | 存
	 MSC_CON_DREAM_NOTES_CHALLENGE = 38,	// √ 通关梦渊笔录| 直接判断 |  指定关卡   | 存
	 MSC_CON_LUN_HUI_WANG_CHUAN = 39,		// √ 通关轮回忘川| 直接判断 |  指定关卡   | 存
	 MSC_CON_CLOUD_ARENA = 40,				// √ 穹顶之争段位| 直接判断 |  指定段位   | 存
	 MSC_CON_KILL_SHAN_HAI_BOSS = 41,		// √ 击杀山海降妖|  累积    |  指定数量  |	 山海类型(难度 0:不限) |存
	 MSC_CON_BRAVE_GROUD_PASS = 42,			// √ 通关勇闯地宫| 直接判断 |  指定层数   | 存
	 MSC_CON_HUAN_JIE_ZHAN_CHANG = 43,		// √ 幻界战场段位| 直接判断 |  指定段位   | 存
	 MSC_CON_PARTNER_EQUIPMENT = 44,		// √ 伙伴一套命格	| 直接判断 |  指定数量   | 存
	 MSC_CON_PARTNER_QUALITY = 45,			// √ 伙伴觉醒    | 直接判断 |　指定数量　 | 达标觉醒次数 |存
	 MSC_CON_NTHER_WORLD_PURGATORY = 46,	// √ 通关某七星  |  累积	 |	指定数量	 |　指定七星（-1:不限制）　 |存
	 MSC_CON_NTHER_WORLD_PURGATORY_ALL = 47,// √ 通关北斗七星|  累积	 |	指定数量  |	星君类型（0:所有,1:星君,2:帝君） |存
	 MSC_CON_SPECIAL_PET_ABANDON = 48,		// √ 宠物放生	|  累积	 |  指定数量  | 指定品质(0:不限)	| 存
	 MSC_CON_PET_REFINED = 49,				// √ 宠物洗练次数|  累积	 |  指定次数 | 大于等于品质(0:不限制品质) |存
	 MSC_CON_PARTNER_BREAK = 50,			// √ 伙伴晋升次数|  累积	 |  指定次数 | 大于等于品质(0:不限制品质) |存
	 MSC_CON_FAST_PASS_DREAM_NOTES = 51,	// √ 扫荡梦渊笔录|  累积	 |  指定次数 | 存
	 MSC_CON_EQUIP_FULL_SET_STR = 52,		// √ 全套装备强化| 直接判断|  指定等级 | 存
	 MSC_CON_PAR_LIMIT_ACTIVITY = 53,		// √ 参与限时活动|  累积	 |  指定次数 | 存  
	 MSC_CON_PET_INTEN_NUMBER	= 54,		// √ 强化宠物次数	|  累积	 |  指定次数 | 大于等于品质(0:不限制品质) | 存
	 MSC_CON_PET_MONOPOLY		= 55,		// √ 灵兽岛层数	| 直接判断|  指定层数 | 存
	 NSC_CON_REALM_BREAK_FIGHT	= 56,		// √ 心灵之境关卡| 直接判断|  指定关卡 | 存
	 MSC_CON_MEDAL_UP_GRADE		= 57,		// √ 角色勋章进阶|  累积	 |  指定次数 | 大于等于品质(0:不限制品质) | 存	
	 MSC_CON_MARKET_SHOPPING	= 58,		// √ 市场灵玉购物	|  累积	 |  指定数量 | 存
	 MSC_CON_PASS_MI_YI_BOSS	= 59,		// √ 通关远征首领| 直接判断|　 固定1   | 指定层数 | 指定怪物序号 | 存
	 MSC_CON_PAR_CLOUD_ARENA	= 60,		// √ 参与穹顶之争|  累积	 |  指定次数 | 存 
	 MSC_CON_PET_LIAN_YAO		= 61,		// √ 宠物炼妖	|  累积	 |  指定次数 | 大于等于品质(0:不限制宠物) | 存

	 MSC_CON_MAX,	
}; 
UNSTD_STATIC_CHECK(MSC_CON_MAX == 62);

enum MYSTERIOUS_TRIAL_TYPE
{
	MYSTERIOUS_TRIAL_TYPE_YUSHEN = 0,			// 狱神再世
	MYSTERIOUS_TRIAL_TYPE_BA_SHE = 1,			// 巴蛇问世

	MYSTERIOUS_TRIAL_TYPE_MAX
};

#pragma pack(push, 4)

struct ActivityFlagSchedule
{
	void Reset()
	{
		schedule = 0;
		reserve_sh = 0;
		flag = 0;
	}
	unsigned short flag;	// 标志     0:未完成   1:已完成未领取   2:已领取	
	short reserve_sh;		//保留字				
	int schedule;			// 进度
};

struct MysteriousTrialParam
{ 
	MysteriousTrialParam(){this->Reset();}
	void Reset()
	{
		open_day = 0;
		task = 0;
		for(int i = 0; i<RA_MYSTERIOUS_TRIAL_MAX_DAY_NUM; i++)
			for(int j =0;j<RA_MYSTERIOUS_TRIAL_DAY_TASK_MAX_NUM;j++)
				mysterious_trial_info[i][j].Reset();
		activity_flag.Reset();
		type = MYSTERIOUS_TRIAL_TYPE_YUSHEN;
		change_reset_flag = 0;
		transfer = 0;
		for (int i = 0; i < RA_MYSTERIOUS_TRIAL_MAX_DAY_NUM; i++)
			for (int j = 0; j < RA_NEW_MYSTERIOUS_TRIAL_DAY_TASK_MAX_NUM; j++)
				new_mysterious_trial_info[i][j].Reset();
		active_open_day_id = 0;
	}

	void ActivityExpiredReset()
	{
		open_day = 0;
		task = 0;
		for (int i = 0; i < RA_MYSTERIOUS_TRIAL_MAX_DAY_NUM; i++)
			for (int j = 0;j < RA_MYSTERIOUS_TRIAL_DAY_TASK_MAX_NUM;j++)
				mysterious_trial_info[i][j].Reset();
		activity_flag.Reset();
		for (int i = 0; i < RA_MYSTERIOUS_TRIAL_MAX_DAY_NUM; i++)
			for (int j = 0; j < RA_NEW_MYSTERIOUS_TRIAL_DAY_TASK_MAX_NUM; j++)
				new_mysterious_trial_info[i][j].Reset();
		active_open_day_id = 0;
	}

	int open_day;					//活动开启的天数
	int task;						//已完成的任务个数(不包含最终任务)
	ActivityFlagSchedule mysterious_trial_info[RA_MYSTERIOUS_TRIAL_MAX_DAY_NUM][RA_MYSTERIOUS_TRIAL_DAY_TASK_MAX_NUM];					//七天的活动信息
	BitMap<RA_MYSTERIOUS_TRIAL_FINALLY_MAX_NUM> activity_flag;																			//最终任务的信息
	short type;						//当前阶段 MYSTERIOUS_TRIAL_TYPE	
	char change_reset_flag;			//进度数据长度修改重置 0:未重置 1:已重置
	char transfer;					//数据转移
	ActivityFlagSchedule new_mysterious_trial_info[RA_MYSTERIOUS_TRIAL_MAX_DAY_NUM][RA_NEW_MYSTERIOUS_TRIAL_DAY_TASK_MAX_NUM];					//七天的活动信息
	unsigned int active_open_day_id;
};
typedef char MysteriousTrialParamHex[sizeof(MysteriousTrialParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(MysteriousTrialParamHex) < 2048);

#pragma pack(pop)

#endif
