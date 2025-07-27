#ifndef __BATTLE_DEF_HPP__
#define __BATTLE_DEF_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/msgserialize.hpp"
#include "servercommon/struct/battle/battle_hallow_def.hpp"

typedef Attribute BattleAttr;
struct BattleAttrPair
{
	BattleAttrPair() : attr_type(0), attr_value(0) {}
	BattleAttrPair(int _attr_type, int _attr_value) : attr_type(_attr_type), attr_value(_attr_value) {}

	int attr_type;
	int attr_value;
};

enum BATTLE_MODE
{
	BATTLE_MODE_INVALID = 0,
	BATTLE_MODE_DEMO,						//!< 1 DEMO
	BATTLE_MODE_NORMAL_MONSTER,				//!< 2 普通怪物
	BATTLE_MODE_MAZE,						//!< 3 迷宫
	BATTLE_MODE_DEMON_CAVE,					//!< 4 魔窟（万妖谷）
	BATTLE_MODE_MINE_MONSTER,				//!< 5 暗雷怪
	BATTLE_MODE_FIELD_BOSS,					//!< 6 野外BOSS（在野外，可以锁定来打的）
	BATTLE_MODE_WORLD_BOSS,					//!< 7 世界BOSS（讨伐玩法）
	BATTLE_MODE_ZHEN_ZHONG,					//!< 8 真中的研究（三界除魔）
	BATTLE_MODE_COURAGE_CHALLENGE,			//!< 9 勇气的挑战 (锢魔之塔）
	BATTLE_MODE_GUILD_BOSS,					//!< 10 家族试炼BOSS  p1: boss_seq
	BATTLE_MODE_CRYSTAL_FB,					//!< 11 水晶副本（要记录血量的那种）
	BATTLE_MODE_SINGLE_PVP_PRACTICE,		//!< 12 单人PVP切磋
	BATTLE_MODE_TREASURE_MAP,				//!< 13 宝图任务
	BATTLE_MODE_TASK_CHAIN,					//!< 14 任务链
	BATTLE_MODE_ESCORT,						//!< 15 运镖
	BATTLE_MODE_MEDAL,						//!< 16 勋章 废弃了
	BATTLE_MODE_TREASURE_ITEM,				//!< 17 藏宝图 物品使用遇怪
	BATTLE_MODE_MI_YI,						//!< 18 密医阴谋[远征魔冢]
	BATTLE_MODE_SQUARD_FIGHT,				//!< 19 小队斗技
	BATTLE_MODE_BRAVE_GROUND,				//!< 20 勇闯地宫
	BATTLE_MODE_MAZE_RACER,					//!< 21 迷宫竞速
	BATTLE_MODE_MARRIAGE_FB,				//!< 22 情缘副本
	BATTLE_MODE_GUILD_FIGHT,				//!< 23 家族大乱斗
	BATTLE_MODE_SINGLE_ARENA,				//!< 24 竞技场
	BATTLE_MODE_CHIEF_ELECTION,				//!< 25 首席竞选
	BATTLE_MODE_NORMAL_PRACTICE,			//!< 26 通常切磋 - 组队或个人都有可能
	BATTLE_MODE_GUILD_HONOR_BATTLE,			//!< 27 工会荣耀战
	BATTLE_MODE_CHIEF_PEAK_BATTLE,			//!< 28 首席巅峰战
	BATTLE_MODE_COURSE_OF_GROWTH,			//!< 29 成长之路BOSS
	BATTLE_MODE_PLATFORM_BATTLE,			//!< 30 擂台战
	BATTLE_MODE_CHALLENGE_FB,				//!< 31 挑战副本
	BATTLE_MODE_ADVANCE_FB,					//!< 32 进阶副本
	BATTLE_MODE_GUIDE_FB,					//!< 33 指引副本
	BATTLE_MODE_WORLD_BOSS_2,				//!< 34 世界BOSS2（巨龙boss）
	BATTLE_MODE_NPC_FIGHT,					//!< 35 选项NPC操作战斗
	BATTLE_MODE_STORY,						//!< 36 剧情战斗
	BATTLE_MODE_MEDAL_FB,					//!< 37 勋章副本
	BATTLE_MODE_TEST_BACKUP,				//!< 38 测试玩家增援
	BATTLE_MODE_FANTASY_FB,					//!< 39 四象幻境(记录血量)
	BATTLE_MODE_EXPEL_MONSTER,				//!< 40 驱除妖兽
	BATTLE_MODE_JOY,						//!< 41 欢乐寻宠特殊玩法
	BATTLE_MODE_JIAN_DAN_QIN_XIN,			//!< 42 剑胆琴心
	BATTLE_MODE_ONLY_FIGHT,					//!< 43 以一敌百(个人版家族大乱斗)
	BATTLE_MODE_REALIVE_GUIDE,				//!< 44 复活指引
	BATTLE_MODE_HUAN_JIE_ZHAN_CHANG_TIAN_TI,//!< 45 幻界战场
	BATTLE_MODE_ONE_DRAGON,					//!< 46 一条龙（仅用于组队功能，不属于任何玩法）
	BATTLE_MODE_SHI_TU_FB,					//!< 47 师徒副本
	BATTLE_MODE_CHIEF_ELECTION_SEA,			//!< 48 首席竞选混战
	BATTLE_MODE_SHAN_HAI_BOSS,				//!< 49 山海降妖
	BATTLE_MODE_DU_CHUANG_ZEI_YING,			//!< 50 独闯贼营
	BATTLE_MODE_CATCH_FOX,					//!< 51 抓拿狐妖
	BATTLE_MODE_MONPOLY_GIFT,				//!< 52 牛运当头
	BATTLE_MODE_DREAM_NOTES_CHALLENGE,		//!< 53 梦渊笔录
	BATTLE_MODE_SCHOOL_EXERCISE,			//!< 54 校场演练
	BATTLE_MODE_RA_MIJINGQIWEN,				//!< 55 秘境奇闻
	BATTLE_MODE_REALM_BREAK,				//!< 56 境界突破
	BATTLE_MODE_MATERIAL,					//!< 57 玉虚境(材料副本)
	BATTLE_MODE_PET_MONOPOLY,				//!< 58 灵宠秘境
	BATTLE_MODE_LUN_HUI_WANG_CHUAN,			//!< 59 轮回忘川
	BATTLE_MODE_CLOUD_ARENA,				//!< 60 云端竞技（穹顶之争）
	BATTLE_MODE_INITIAL_FIGHT,				//!< 61 初始战斗
	BATTLE_MODE_NETHER_WORLD_PURGATORY,		//!< 62 幽冥炼狱
	BATTLE_MODE_PET_GOD_FB,					//!< 63 神印之地
	BATTLE_MODE_HUNDRED_GHOST,				//!< 64 百鬼夜行
	BATTLE_MODE_BIG_DIPPER,					//!< 65 北斗七星
	BATTLE_MODE_SUMMER_LI_LIAN,				//!< 66 初夏历练
	BATTLE_MODE_MAZE_SUMMER,				//!< 67 夏日迷宫
	BATTLE_MODE_DUEL,						//!< 68 决斗
	BATTLE_MODE_DU_JIE,						//!< 69 渡劫
	BATTLE_MODE_YIN_YANG_EYE,				//!< 70 阴阳眼
	BATTLE_MODE_REALM_BREAK_FIGHT,			//!< 71 境界战斗（心灵之境）
	BATTLE_MODE_RA_MIJINGQIWEN_2,			//!< 72 秘境奇闻2 (万灵奇闻)
	BATTLE_MODE_ENDLESS_TOWER,				//!< 73 无尽之塔
	BATTLE_MODE_ZAI_ZHAN_YUN_ZE,			//!< 74 再战云泽	p_1:boss_index
	BATTLE_MODE_MO_ZU_ZAI_NA_LI,			//!< 75 魔族在哪里
	BATTLE_MODE_SHAN_HAI_ZHENG_FENG,		//!< 76 山海争锋
	BATTLE_MODE_WAN_LING_FB,				//!< 77 万灵话本
	BATTLE_MODE_PARTNER_PVE,				//!< 78 伙伴PVE(天辰蜃境)
	BATTLE_MODE_WEDDING_FB,					//!< 79 婚宴
	BATTLE_MODE_WORLD_ARENA,				//!< 80 论剑九州
	BATTLE_MODE_WEDDING_FB_BAO_WEI_XIU_QIU,	//!< 81 婚宴-小游戏 保卫绣球
	BATTLE_MODE_FALLEN_GOD,					//!< 82 殒神之地
	BATTLE_MODE_WORLD_TEAM_ARENA,			//!< 83	英雄会武
	BATTLE_MODE_QING_YUAN_SHENG_HUI,		//!< 84 情缘盛会
	BATTLE_MODE_YAO_XING_SHI_LIAN,			//!< 85 妖行试炼
	BATTLE_MODE_LANTERN_RIDDIR,				//!< 86 山海灯会
	BATTLE_MODE_UNIVERSE_TEAM_ARENA,		//!< 87 天下第一
	BATTLE_MODE_HONGMENG_TIANJIE,			//!< 88 鸿蒙天界
	BATTLE_MODE_KE_JU_EXAM,					//!< 89 科举考试
	BATTLE_MODE_GOTO_SCHOOL,				//!< 90 我要上学
	BATTLE_MODE_ANCIENT_RUINS,				//!< 91 上古遗迹
	BATTLE_MODE_ELITE_TRAILS,				//!< 92 精英试炼
	BATTLE_MODE_ANCIENT_RUINS_NORMAL_MONSTER, //!< 93 上古遗迹小怪 (为了区分天雷）（废弃）
	BATTLE_MODE_ELITE_TRAILS_NORMAL_MONSTER,//!< 94 精英试炼小怪 (为了区分天雷）（废弃）
	BATTLE_MODE_NEW_COURAGE_CHALLENGE,		//!< 95 弑神之塔
	BATTLE_MODE_MINI_GAME_BATTLE,			//!< 96 小游戏战斗
	BATTLE_MODE_WORLD_BOSS_3,				//!< 97 世界BOSS3(天神降临)
	BATTLE_MODE_SPECIAL_ANECDOTE_TASK,		//!< 98 挑战类奇闻任务
	BATTLE_MODE_TEAM_CHALLENGE_TASK,		//!< 99 组队挑战任务
	BATTLE_MODE_TASK_ROLE_MONSTER,		//!< 100 任务个人怪
	BATTLE_MODE_MAX,
};

bool IsBattleModeMustUseMaxHpMp(int battle_mode);

enum BATTLE_STATUS
{
	BATTLE_STATUS_PREPARE = 0,		// 准备阶段
	BATTLE_STATUS_FIGHTING,			// 战斗阶段
	BATTLE_STATUS_CALCULATE,		// 结算阶段
};

/**
* \note 准备->播放动画->等待输入->服务器计算->播放动画
*/
enum ROUND_PHASE
{
	ROUND_PHASE_PREPARE = 0,			// 准备阶段（此阶段触发场景事件）
	ROUND_PHASE_WAIT_INPUT = 1,			// 等待输入阶段 （进入这个阶段的瞬间可以顺便做预计算AI的动作，注意应按玩家的输入为优先）
	ROUND_PHASE_COM_CALCULATE = 2,		// 服务器计算阶段
	ROUND_PHASE_WATCH_ANIMATION = 3,	// 播放动画阶段
};

enum RESULT_TYPE
{
	RESULT_TYPE_INVALID = 0,
	RESULT_TYPE_ATTACKER_WIN,		//!< 1 攻方胜
	RESULT_TYPE_DEFENDER_WIN,		//!< 2 守方胜
	RESULT_TYPE_DRAW,				//!< 3 和局(两边同时被打死）
	RESULT_TYPE_FORCED_FINISH,		//!< 4 战斗强制结束
	RESULT_TYPE_EXCEED_MAX_ROUND,	//!< 5 超过最大回合数
	RESULT_TYPE_EXCEED_MAX_TIME,	//!< 6 超过最大时间限制
	RESULT_TYPE_RUNAWAY,			//!< 7 逃跑
	RESULT_TYPE_MAX,
};

enum AUDIENCE_TYPE
{
	AUDIENCE_TYPE_NONE = 0,			//!< 0 非观战
	AUDIENCE_TYPE_FLY_OB = 1,		//!< 1 击飞了继续观战
	AUDIENCE_TYPE_ALWAYS_OB = 2,	//!< 2 一直观战 
};

enum BATTLE_PHASE
{
	BATTLE_PHASE_FIRST = 0,
	BATTLE_PHASE_SECOND = 1,
	BATTLE_PHASE_NUM = 2
};

enum BATTLE_MOVE_TYPE
{
	BATTLE_MOVE_TYPE_INVALID = 0,
	BATTLE_MOVE_TYPE_PERFORM_SKILL,		//!< 1 使用技能 grid_idx 目标位置  param1 目标技能序号 param2 目标技能等级
	BATTLE_MOVE_TYPE_NORMAL_ATTACK,		//!< 2 普攻 grid_idx 目标位置
	BATTLE_MOVE_TYPE_NORMAL_DEFENCE,	//!< 3 防御  
	BATTLE_MOVE_TYPE_USE_ITEM,			//!< 4 使用物品 grid_idx 目标位置  param1 物品类型[0:回血药 1:武器] (找到缓存下来的物品信息并使用之)
	BATTLE_MOVE_TYPE_RUN_AWAY,			//!< 5 逃跑   
	BATTLE_MOVE_TYPE_CHANGE_POSITION,	//!< 6 换位置 
	BATTLE_MOVE_TYPE_CHANGE_PET,		//!< 7 换宠物	param1 宠物idx (-1为召回）
	BATTLE_MOVE_TYPE_SEAL_PET,			//!< 8 封印宠物 grid_idx 目标位置
	BATTLE_MOVE_TYPE_USE_FABAO,			//!< 9 使用法宝 grid_idx 目标位置  param1 法宝技能类型 param2 法宝技能参数1 param3 法宝技能参数2
	BATTLE_MOVE_TYPE_INSTANT_NORMAL_ATTACK, //!< 10 瞬发普攻 grid_idx 目标位置
	BATTLE_MOVE_TYPE_AI_SKILL,			//!< 11 智能施法
	BATTLE_MOVE_TYPE_PERFORM_HEART_SKILL = 12,	//!< 12 使用心法技能
	BATTLE_MOVE_TYPE_PERFORM_CHANGE_CARD_SKILL = 13,	//!< 13 使用变身卡技能
	BATTLE_MOVE_TYPE_PERFORM_HUANSHOU_SKILL = 14,	//!< 14 使用幻兽技能

	BATTLE_MOVE_TYPE_ROLE_ACTION_MAX,

	BATTLE_MOVE_TYPE_MONSTER_ACTION_MIN = 20,  //!< -----------此处是分割线, 以下的都是用于怪物的行为------------
	BATTLE_MOVE_TYPE_SUMMON_MONSTER,	//!<21 召唤怪物 param1 召唤逻辑 param2 召唤id
	BATTLE_MOVE_TYPE_SELF_DESTRUCT,		//!<22 自爆		param1 自爆伤害
	BATTLE_MOVE_TYPE_DO_NOTHING,		//!<23 专门给伙伴机器人用的，防止其二动
	BATTLE_MOVE_TYPE_NUM,
};

UNSTD_STATIC_CHECK(BATTLE_MOVE_TYPE_ROLE_ACTION_MAX < BATTLE_MOVE_TYPE_MONSTER_ACTION_MIN);

enum USE_ITEM_TYPE
{
	USE_ITEM_HEAL = 0,
	USE_ITEM_EQUIPMENT = 1,
	USE_ITEM_INST_KILL_MONSTER = 2,
	USE_ITEM_MP = 3,
};

enum BATTLE_SKILL_TYPE
{
	BATTLE_SKILL_TYPE_INVALID = 0,
	BATTLE_SKILL_TYPE_SINGLE = 1,
	BATTLE_SKILL_TYPE_POWER = 2,
	BATTLE_SKILL_TYPE_SUPER_POWER = 3,
	BATTLE_SKILL_TYPE_RANDOM = 4,
	BATTLE_SKILL_TYPE_COMBO = 5,	// 攻击N次
	BATTLE_SKILL_TYPE_LINE = 6,		// 直线技能
	BATTLE_SKILL_TYPE_COMMON_RANGE = 7,	// 通用范围技能
	BATTLE_SKILL_TYPE_MAX = 8,
};

enum BATTLE_SKILL_TARGET_SIDE_TYPE
{
	BATTLE_SKILL_TARGET_SIDE_TYPE_INVALID = -1,
	BATTLE_SKILL_TARGET_SIDE_TYPE_NO_LIMIT,
	BATTLE_SKILL_TARGET_SIDE_TYPE_FRIENDLY_ONLY,
	BATTLE_SKILL_TARGET_SIDE_TYPE_ENEMY_ONLY,
	BATTLE_SKILL_TARGET_SIDE_TYPE_MAX,
};

enum BATTLE_CHARACTER_TYPE
{
	BATTLE_CHARACTER_TYPE_INVALID = -1,
	BATTLE_CHARACTER_TYPE_ROLE = 0,			//!< 0 主角
	BATTLE_CHARACTER_TYPE_PET,				//!< 1 宠物
	BATTLE_CHARACTER_TYPE_MONSTER,			//!< 2 怪物
	BATTLE_CHARACTER_TYPE_PARTNER,			//!< 3 伙伴
	BATTLE_CHARACTER_TYPE_ROLE_SHADOW,		//!< 4 主角影子
	BATTLE_CHARACTER_TYPE_PET_SHADOW,		//!< 5 宠物影子
	BATTLE_CHARACTER_TYPE_PARTNER_ROBOT,	//!< 6 伙伴机器人
	BATTLE_CHARACTER_TYPE_NPC_HELPER,		//!< 7 助战NPC
	BATTLE_CHARACTER_TYPE_MAX,
};

enum RACE_TYPE
{
	RACE_TYPE_MIN = 0,
	RACE_TYPE_HUMAN = RACE_TYPE_MIN,	// 0 人形
	RACE_TYPE_BEAST,					// 1 野兽
	RACE_TYPE_NATURE,					// 2 自然
	RACE_TYPE_UNDEAD,					// 3 复生
	RACE_TYPE_INSECT,					// 4 异虫
	RACE_TYPE_MYSTERY,					// 5 神秘
	RACE_TYPE_GHOST,					// 6 妖灵
	RACE_TYPE_DRAGON,					// 7 古龙
	RACE_TYPE_FLY,						// 8 飞行
	RACE_TYPE_ANCIENT,					// 9 上古
	
	RACE_TYPE_MAX,							
};

enum BATTLE_MODIFY_HP_REASON
{
	BATTLE_MODIFY_HP_REASON_PHYSIC_SKILL_INJURE = 0,
	BATTLE_MODIFY_HP_REASON_MAGIC_SKILL_INJURE,
	BATTLE_MODIFY_HP_REASON_ADD_HP,
	BATTLE_MODIFY_HP_REASON_COMBINE_ATTACK_INJURE,
	BATTLE_MODIFY_HP_REASON_BUFF,
	BATTLE_MODIFY_HP_REASON_SELF_DESTRUCT_INJURE,
	BATTLE_MODIFY_HP_REASON_MOVE_SCRIPT,
	BATTLE_MODIFY_HP_REASON_DAMAGE_ABSORB_HP,
	BATTLE_MODIFY_HP_REASON_BOUND_DAMAGE,
	BATTLE_MODIFY_HP_REASON_SCRIPT_DAMAGE,	
	BATTLE_MODIFY_HP_REASON_FABAO_REAL_DAMAGE,
	BATTLE_MODIFY_HP_REASON_DAMAGE_BACKFIRE,
	BATTLE_MODIFY_HP_REASON_REAL_DAMAGE,
	BATTLE_MODIFY_HP_REASON_SACRIFICE_HP,
	BATTLE_MODIFY_HP_REASON_SHARE_DAMAGE,
	BATTLE_MODIFY_HP_REASON_DAMAGE_TOGETHER,
	BATTLE_MODIFY_HP_REASON_HOLY_MAGIC_SKILL_INJURE,
	BATTLE_MODIFY_HP_REASON_ACCUMULATE_DAMAGE_BUFF_BURUST,

	BATTLE_MODIFY_HP_REASON_MAX
};

enum BATTLE_MODIFY_MP_REASON
{
	BATTLE_MODIFY_MP_REASON_SKILL_CONSUME = 0,	//!< 0 技能消耗
	BATTLE_MODIFY_MP_REASON_DRUNK_STATE,		//!< 1 酒醉状态
	BATTLE_MODIFY_MP_REASON_SELF_DESTRUCT,		//!< 2 自爆耗蓝
	BATTLE_MODIFY_MP_REASON_SPECIAL_EFFECT,		//!< 3 特效改变蓝量
	BATTLE_MODIFY_MP_REASON_BURN_MP_SKILL,		//!< 4 技能削蓝
	BATTLE_MODIFY_MP_REASON_SYSTEM_SCRIPT,		//!< 5 系统脚本
	BATTLE_MODIFY_MP_REASON_USE_MP_ITEM,		//!< 6 使用加蓝物品
	BATTLE_MODIFY_MP_REASON_ABSORB_MP,			//!< 7 吸蓝
	BATTLE_MODIFY_MP_REASON_BUFF,				//!< 8 BUFF加蓝
	BATTLE_MODIFY_MP_REASON_HALLOW_ADD_MP,		//!< 9 圣器加蓝
};

enum BATTLE_MODIFY_RAGE_POINT_REASON
{
	BATTLE_MODIFY_RAGE_POINT_REASON_ROUND = 0,			//!< 0 回合开始
	BATTLE_MODIFY_RAGE_POINT_REASON_NORMAL_ATTACK = 1,	//!< 1 普攻
	BATTLE_MODIFY_RAGE_POINT_REASON_NORMAL_DEFENSE = 2,	//!< 2 防御
	BATTLE_MODIFY_RAGE_POINT_REASON_PERFORM_SKILL = 3,	//!< 3 释放技能
	BATTLE_MODIFY_RAGE_POINT_REASON_SPECIAL_EFFECT = 4, //!< 4 战斗特效
	BATTLE_MODIFY_RAGE_POINT_REASON_SKILL_EFFECT = 5,	//!< 5 技能效果
};

enum BATTLE_MODIFY_SPIRIT_POINT_REASON
{
	BATTLE_MODIFY_SPIRIT_POINT_REASON_DIE_REDUCE = 0,	//!< 0 死亡减少
	BATTLE_MODIFY_SPIRIT_POINT_REASON_ROUND_ADD = 1,	//!< 1 每回合增加
	BATTLE_MODIFY_SPIRIT_POINT_REASON_SKILL = 2,		//!< 2 技能消耗
};

enum BATTLE_MODIFY_CD_REASON
{
	BATTLE_MODIFY_CD_REASON_PERFORM_SKILL = 0,
	BATTLE_MODIFY_CD_REASON_ROUND_END,
};

enum BATTLE_INSTANT_KILL_REASON
{
	BATTLE_INSTANT_KILL_REASON_SKILL = 0,
	BATTLE_INSTANT_KILL_REASON_SCRIPT = 1,
	BATTLE_INSTANT_KILL_REASON_ITEM = 2,
	BATTLE_INSTANT_KILL_REASON_EFFECT = 3,
};

enum BATTLE_ATTACK_FLAG
{
	BATTLE_ATTACK_FLAG_PHYSICAL_SKILL = 0,
	BATTLE_ATTACK_FLAG_MAGIC_SKILL,
	BATTLE_ATTACK_FLAG_HOLY_MAGIC_SKILL,
	BATTLE_ATTACK_FLAG_CRACK_ATTACK,
	BATTLE_ATTACK_FLAG_COUNTER_ATTACK,
	BATTLE_ATTACK_FLAG_SKILL_CRITICAL,
};

enum BATTLE_RESET_CHARACTER_REASON
{
	BATTLE_RESET_CHARACTER_REASON_NORMAL = 0,
	BATTLE_RESET_CHARACTER_REASON_RUN_AWAY = 1,
	BATTLE_RESET_CHARACTER_REASON_RETRIEVE_PET = 2,
	BATTLE_RESET_CHARACTER_REASON_SUMMON_PET = 3,
	BATTLE_RESET_CHARACTER_REASON_SEAL_PET = 4,
	BATTLE_RESET_CHARACTER_REASON_SELF_DESTRUCT = 5,
	BATTLE_RESET_CHARACTER_REASON_MONSTER_DIE = 6,
	BATTLE_RESET_CHARACTER_REASON_FLY_AWAY = 7,
	BATTLE_RESET_CHARACTER_REASON_DISAPPEAR = 8,
};

enum BATTLE_HP_DESC  //!< 血量改变时的特殊表现
{
	BATTLE_HP_DESC_NORMAL,				//!< 0 无特殊
	BATTLE_HP_DESC_CRITICAL,			//!< 1 暴击
	BATTLE_HP_DESC_RESERVE_1,			//!< 2 保留字
	BATTLE_HP_DESC_HP_SHIELD,			//!< 3 生命护盾
	BATTLE_HP_DESC_POISON,				//!< 4 毒
	BATTLE_HP_DESC_COMBINE_ATTACK,		//!< 5 合击
	BATTLE_HP_DESC_ADD_HP,				//!< 6 加血
	BATTLE_HP_DESC_DAMAGE_ABSORB_HP,	//!< 7 吸血
	BATTLE_HP_DESC_DAMAGE_BOUNCE,		//!< 8 反弹伤害
	BATTLE_HP_DESC_IN_VAIN,				//!< 9 无效
	BATTLE_HP_DESC_CRACK_DEFENSE,		//!< 10 破防
	BATTLE_HP_DESC_RESERVE_3,			//!< 11 保留字
	BATTLE_HP_DESC_ADD_HP_CRITCAL,		//!< 12 暴击加血
	BATTLE_HP_DESC_REAL_DAMAGE,			//!< 13 真实伤害
	BATTLE_HP_DESC_SACRIFICE_HP,		//!< 14 献祭自身血量
	BATTLE_HP_DESC_DEATH_SICKLE,		//!< 15 死神之镰
	BATTLE_HP_DESC_DESPERATE,			//!< 16 摄魂夺魄
	BATTLE_HP_DESC_MAX
};

enum BATTLE_BE_HIT_DESC	//!< 被击时的特殊表现
{
	BATTLE_BE_HIT_DESC_NORMAL = 0,		//!< 0 无
	BATTLE_BE_HIT_DESC_DEFENSE = 1,		//!< 1 防御
	BATTLE_BE_HIT_DESC_SHIELD = 2,		//!< 2 护盾
	BATTLE_BE_HIT_DESC_BLOCK = 3,		//!< 3 格挡
	BATTLE_BE_HIT_DESC_GUARD = 4,		//!< 4 护卫
	BATTLE_BE_HIT_DESC_SHARE_DAMAGE = 5,//!< 5 分担伤害
	BATTLE_BE_HIT_DESC_DEATH_SICKLE = 6,//!< 6 死神之镰
	BATTLE_BE_HIT_DESC_BLOCK_1 = 7,		//!< 7 格挡(伪)(连击超有效次数)
	BATTLE_BE_HIT_DESC_MAX,
};

enum BATTLE_ATTR
{
	BATTLE_ATTR_MIN = 0,
	BATTLE_ATTR_NORMAL_VALUE_BEGIN = BATTLE_ATTR_MIN,			// 基础属性值起始

	BATTLE_ATTR_ATTACK = BATTLE_ATTR_NORMAL_VALUE_BEGIN,		// 0 攻击 
	BATTLE_ATTR_DEFENSE,										// 1 防御 
	BATTLE_ATTR_AGILITY,										// 2 敏捷 
	BATTLE_ATTR_RECOVERY,										// 3 回复 
	BATTLE_ATTR_MENTAL,											// 4 精神 
	BATTLE_ATTR_CUR_HP,											// 5 当前血量
	BATTLE_ATTR_MAX_HP,											// 6 最大血量
	BATTLE_ATTR_CUR_MP,											// 7 当前魔量
	BATTLE_ATTR_MAX_MP,											// 8 最大魔量
	BATTLE_ATTR_PANEL_END = BATTLE_ATTR_MAX_MP,					// 面板属性

	BATTLE_ATTR_PHYSICAL_DEC_DAMAGE,							// 9 物免（千分比)
	BATTLE_ATTR_MAGIC_DEC_DAMAGE,								// 10 法免（千分比)
	BATTLE_ATTR_PHYSICAL_INC_DAMAGE,							// 11 物伤（千分比)
	BATTLE_ATTR_MAGIC_INC_DAMAGE,								// 12 法伤（千分比)

	BATTLE_ATTR_NORMAL_VALUE_END,								// 基础属性值结束
	BATTLE_ATTR_CORRECTED_VALUE_BEGIN = 15,						// 修正值起始

	BATTLE_ATTR_CRITICAL = BATTLE_ATTR_CORRECTED_VALUE_BEGIN,	// 15 必杀率（千分比)
	BATTLE_ATTR_HIT,											// 16 命中率（千分比)
	BATTLE_ATTR_DODGE,											// 17 闪避率（千分比）
	BATTLE_ATTR_COUNTER_ATTACK,									// 18 反击率（千分比)
	BATTLE_ATTR_MAGIC_ATTACK,									// 19 魔攻
	BATTLE_ATTR_MAGIC_DEFENSE,									// 20 魔防
	BATTLE_ATTR_CRITICAL_INC_VALUE,								// 21 必杀伤害值（千分比)
	BATTLE_ATTR_CRITICAL_DEC_VALUE,								// 22 坚韧值（千分比)
	BATTLE_ATTR_CONTROL_HIT,									// 23 控制命中(千分比)
	BATTLE_ATTR_REAL_DAMAGE,									// 24 真实伤害值
	BATTLE_ATTR_MAGIC_PENETRATION_RATE,							// 25 法术穿透（千分比)
	BATTLE_ATTR_ANTI_CONTROL_RATE,								// 26 控制抵抗率（千分比）
	BATTLE_ATTR_MAGIC_CRIRICAL,									// 27 法术必杀率（千分比）
	BATTLE_ATTR_MAGIC_CRIRICAL_INC_DAMAGE,						// 28 法术必杀伤害值(千分比)


	BATTLE_ATTR_CORRECTED_VALUE_END,							// 修正值结束
	BATTLE_ATTR_ANTI_VALUE_BEGIN = 30,							// 抵抗值开始

	BATTLE_ATTR_ANTI_POISON = BATTLE_ATTR_ANTI_VALUE_BEGIN,		// 30 中毒抵抗（千分比）
	BATTLE_ATTR_ANTI_FREEZE,									// 31 冰冻抵抗（千分比）
	BATTLE_ATTR_ANTI_PETRIFY,									// 32 石化抵抗（千分比）
	BATTLE_ATTR_ANTI_CHAOS,										// 33 混乱抵抗（千分比）
	BATTLE_ATTR_ANTI_DRUNK,										// 34 酒醉抵抗（千分比）
	BATTLE_ATTR_ANTI_SILENCE,									// 35 沉默抵抗（千分比）
	BATTLE_ATTR_ANTI_CURSE,										// 36 诅咒抵抗（千分比）
	BATTLE_ATTR_ANTI_SINK_HOLE,									// 37 地陷抵抗（千分比）
	BATTLE_ATTR_ANTI_SEVERE_INJURE,								// 38 重伤抵抗（千分比）
	BATTLE_ATTR_ANTI_DEFENCE_CRACKED,							// 39 破防抵抗（千分比）

	BATTLE_ATTR_ANTI_VALUE_END,									// 抵抗值结束
	BATTLE_ATTR_ELEMENT_BEGIN = 40,								// 元素值开始

	BATTLE_ATTR_ELEMENT_WATER = BATTLE_ATTR_ELEMENT_BEGIN,		// 水元素值 元素取值范围[0,10]的整数
	BATTLE_ATTR_ELEMENT_FIRE,									// 火
	BATTLE_ATTR_ELEMENT_WIND,									// 风
	BATTLE_ATTR_ELEMENT_EARTH,									// 地
	BATTLE_ATTR_ELEMENT_END,									// 元素值结束

	BATTLE_ADD_POINT_TYPE_BEGIN,//特殊元素 - 玩家类属性加点
	BATTLE_ADD_POINT_TYPE_VITALITY = BATTLE_ADD_POINT_TYPE_BEGIN,	//!< 体力 45
	BATTLE_ADD_POINT_TYPE_STRENGTH,									//!< 力量 46 
	BATTLE_ADD_POINT_TYPE_POWER,									//!< 强度 47 
	BATTLE_ADD_POINT_TYPE_AGILITY,									//!< 敏捷 48
	BATTLE_ADD_POINT_TYPE_MAGIC,									//!< 魔法 49
	BATTLE_ADD_POINT_TYPE_END = BATTLE_ADD_POINT_TYPE_MAGIC,

	BATTLE_ATTR_MAX = 50,
};
UNSTD_STATIC_CHECK(BATTLE_ATTR_NORMAL_VALUE_END <= BATTLE_ATTR_CORRECTED_VALUE_BEGIN);
UNSTD_STATIC_CHECK(BATTLE_ATTR_CORRECTED_VALUE_END <= BATTLE_ATTR_ANTI_VALUE_BEGIN);
UNSTD_STATIC_CHECK(BATTLE_ATTR_ANTI_VALUE_END <= BATTLE_ATTR_ELEMENT_BEGIN);
UNSTD_STATIC_CHECK(BATTLE_ATTR_ELEMENT_END <= BATTLE_ATTR_MAX);
UNSTD_STATIC_CHECK(BATTLE_ATTR_MAX <= 50);

static const int BATTLE_BASE_ATTR_NUM = 5;		// 写死，攻、防、敏、回、精
static const int BATTLE_AMEN_ATTR_NUM = BATTLE_ATTR_CORRECTED_VALUE_END - BATTLE_ATTR_CORRECTED_VALUE_BEGIN;
static const int BATTLE_RESI_ATTR_NUM = BATTLE_ATTR_ANTI_VALUE_END - BATTLE_ATTR_ANTI_VALUE_BEGIN;
static const int BATTLE_ELEMENT_ATTR_NUM = BATTLE_ATTR_ELEMENT_END - BATTLE_ATTR_ELEMENT_BEGIN;

enum BATTLE_SKILL_MAGIC_TYPE
{
	BATTLE_SKILL_MAGIC_TYPE_INVALID = 0,
	BATTLE_SKILL_MAGIC_TYPE_WATER = 1,				//!< 1 水系魔法
	BATTLE_SKILL_MAGIC_TYPE_FIRE = 2,				//!< 2 火系魔法
	BATTLE_SKILL_MAGIC_TYPE_WIND = 3,				//!< 3 风系魔法
	BATTLE_SKILL_MAGIC_TYPE_EARTH = 4,				//!< 4 土系魔法
	BATTLE_SKILL_MAGIC_TYPE_HOLY = 5,				//!< 5 神圣魔法 
	BATTLE_SKILL_MAGIC_TYPE_ENERGY = 6,				//!< 6 能量魔法
	BATTLE_SKILL_MAGIC_TYPE_CHAOS = 7,				//!< 7 混沌魔法
	BATTLE_SKILL_MAGIC_TYPE_MAX = 8,	
};

enum BATTLE_TARGET_DEAD_TYPE
{
	BATTLE_TARGET_ALIVE = 0,
	BATTLE_TARGET_DEAD = 1,
	BATTLE_TARGET_ALIVE_OR_DEAD = 2,
};

static const short USE_BATTLE_SERVER_INDEX = 0;		//!< 使用战斗服index1
static const short MAX_CHARACTER_NUM = 20;			//!< 战斗中最大角色数
static const short SIDE_CHARACTER_NUM = 10;
static const short NORMAL_SPEED_WAIT_TIME_S = 5 * 60; // X1速度等待动画时间最长300秒   X2则150秒
static const short FIRST_FINISH_ANIMATION_CONTINUE_WAIT_TIME_S = 10;
static const short WAIT_OTHER_ANIMATION_NOTICE_TIME_S = 4;	// 等待其他人播放动画结束，4秒后未结束则发通知让玩家耐心等待
static const short WAIT_INPUT_TIME_S = 31; // 30秒的手动，考虑延迟的原因，给到31秒
static const short WAIT_AUTO_INPUT_TIME_S = 1; // 1秒的自动
static const short FIRST_ROUND_WAIT_AUTO_INPUT_TIME_S = 4;
static const short MAX_ROUND_NUM = 300;
static const short ROW_CHARACTER_NUM = 5;
static const short CLOSE_BATTLE_RANGE_ROW = 2; // 近战距离（单位：行）
static const short INVALID_GRID_IDX = -1;
static const int BATTLE_PERCENT_NUM = 100;
static const int BATTLE_THOUSAND_PERCENT_NUM = 1000;
static const int BATTLE_ATTR_PERCENT_NUM = 1000; // 属性用千分比
static const int BATTLE_TEN_THOUSAND_PERCENT_NUM = 10000;	//万分比
static const int BATTLE_FABAO_PERCENT_NUM = 100000; // 百分比，且保留小数点后3位，所以这里用十万分比
static const short BATTLE_BUFF_PARAM_NUM = 7;
static const short BATTLE_BUFF_MAX_LEVEL = 42;
static const short LEAST_COMBINE_ATTACK_CHARACTER_NUM = 2;
static const short RUN_AWAY_SUCC_RATE_HIGH = 80;
static const short RUN_AWAY_SUCC_RATE_LOW = 30;
static const int ROLE_BATTLE_PET_NUM = 5;
static const int ROLE_BATTLE_PET_HELPER_NUM = 4;
static const int BATTLE_MONSTER_MOVE_NUM = 10;
static const int BATTLE_MONSTER_SPECIAL_EFFECT_NUM = 3;
static const int MAX_BATTLE_SKILL_NUM = 25;
static const int BATTLE_SCRIPT_CONDITION_NUM = 10;  // 虽然定了10个，但最好不超过3个条件
static const int BATTLE_SCRIPT_EVENT_NUM = 10;	// 虽然定了10个，但最好不超过3个事件
static const int BATTLE_MAX_ROLE_NUM = 10;
static const int SIDE_MAX_ROLE_NUM = BATTLE_MAX_ROLE_NUM / 2;
UNSTD_STATIC_CHECK(SIDE_MAX_ROLE_NUM * 2 == BATTLE_MAX_ROLE_NUM);
UNSTD_STATIC_CHECK(SIDE_MAX_ROLE_NUM == ROW_CHARACTER_NUM);

static const int BATTLE_NORMAL_ATTACK_SKILL_ID = 1;
static const int BATTLE_NORMAL_DEFEND_SKILL_ID = 2;
static const int BATTLE_INSTANT_KILL_SKILL_ID = 158;
static const int BATTLE_RESURRECT_ENEMY_SKILL_ID = 165;
static const int BATTLE_RAMPAGE_BUFF_ID = 333;
static const int INVALID_BATTLE_ID = -1;

static const int MAX_BATTLE_SKILL_LEVEL = 42;
static const int MAX_BATTLE_SKILL_EFFECT_NUM = 4;
static const UInt16 INVALID_BATTLE_SKILL_ID = -1;
static const int MAX_BATTLE_SKILL_ID = MAX_UINT16 + 1;


static const int BATTLE_WEAPON_ATTR_NUM = 6;
static const int BATTLE_WEAPON_EFFECT_NUM = 1;
static const int BATTLE_OTHER_EQUIP_EFFECT_NUM = 40;

static const int BATTLE_MINE_MONSTER_GROUP = 65534;	// 暗雷怪专用monster_group_id
static const int BATTLE_BUSINESS_DATA_BUFFER_LEN = 8192;
static const int BATTLE_DISMISS_TIME = 3 * 60;
static const int BATTLE_FABAO_CD = 2;
static const int BATTLE_FABAO_NUM_MAX = 16;
static const int BATTLE_NPC_DEMO_FABAO_NUM = 3;
static const int BATTLE_NPC_DEMO_POTION_NUM = 3;

static const int BATTLE_ELEMENT_MAX = 10;

static const int BATTLE_PET_GIFT_NUM = 20;
static const int BATTLE_OTHER_EFFECT_NUM = 30;
static const int BATTLE_PARTNER_OTHER_EFFECT_NUM = 15;
static const int BATTLE_OBSERVER_MAX_NUM = 200;
static const int BATTLE_COMBINE_ATTACK_PROB = 50;

static const int BATTLE_COUNTER_ATTACK_MAX_TIMES = 2;

static const int BATTLE_STORY_ROUND_NUM = 2;

static const int BATTLE_ROUND_ADD_RAGE_TIMES = 2;

static const int BATTLE_BUSINESS_BUFF_MAX_NUM = 4;

static const int BATTLE_GUILD_BUFF_NUM = 2;
static const int BATTLE_HEART_SKILL_NUM = 2;

static const int BATTLE_MAGIC_CRIRICAL_BASE_DAMAGE_PERCENT = 1300;	//法术暴击基础系数(千分比)
static const int BATTLE_BURN_MP_DEEP_BLOW_LOW_MP_PERCENT = 65;		//技能效果9深度一击扣除的法力值百分比
static const int BATTLE_OTHER_EFFECT_SACRIFICE_SELF_ADD_QINGYUAN_PERCENT = 50;	;//其它战斗特效93,玩家血量和蓝量需要大于等于该百分比

static const int BATTLE_CONDITION_MAX_NUM = 6;
UNSTD_STATIC_CHECK(BATTLE_CONDITION_MAX_NUM < (int)(sizeof(char) * 8));

static const int BATTLE_JIEYI_MAX_NUM = 4;
static const int BATTLE_JIEYI_SKILL_MAX_NUM = 4;

static const int BATTLE_QINGYUAN_SKILL_MAX_NUM = 4;

//也可以通过代码生成相邻的列表，假如需要扩展每行人数的话
static const short ADJACENT_GRID_ARRAY[MAX_CHARACTER_NUM][3] = {
	{ 5, 1, -1 },
	{ 6, 0, 2 },
	{ 7, 1, 3 },
	{ 8, 2, 4 },
	{ 9, 3, -1 },
	{ 0, 6, -1 },
	{ 1, 5, 7 },
	{ 2, 6, 8 },
	{ 3, 7, 9 },
	{ 4, 8, -1 },
	{ 15, 11, -1 },
	{ 16, 10, 12 },
	{ 17, 11, 13 },
	{ 18, 12, 14 },
	{ 19, 13, -1 },
	{ 10, 16, -1 },
	{ 11, 15, 17 },
	{ 12, 16, 18 },
	{ 13, 17, 19 },
	{ 14, 18, -1 }
};

struct BattleFabao
{
	BattleFabao() : fabao_id(0), fabao_level(0), fabao_skill_type(0), 
		fabao_grade(0), fabao_effect_percent_num(0) {}

	short fabao_id;				//!< 法宝ID
	short fabao_level;			//!< 法宝等级 
	short fabao_skill_type;		//!< 法宝技能类型 \see BATTLE_FABAO_SKILL_TYPE
	short fabao_grade;			//!< 法宝阶级
	int fabao_effect_percent_num; //!< 法宝效果百分比 * 1000
	int fabao_effect_percent_num_2; //!< 法宝效果2 百分比 * 1000
};

enum BATTLE_FABAO_SKILL_TYPE
{
	BATTLE_FABAO_SKILL_TYPE_INVALID = 100,
	BATTLE_FABAO_SKILL_TYPE_FORBID_ENEMY_FABAO_SKILL = 101,		//!< 1 禁止目标释放法宝技能
	BATTLE_FABAO_SKILL_TYPE_TARGET_PET_RUNAWAY = 102,			//!< 2 几率使目标宠物逃跑
	BATTLE_FABAO_SKILL_TYPE_BOUNCE_NEXT_PHYSICAL_DAMAGE = 103,	//!< 3 反弹下一次物理伤害的百分比
	BATTLE_FABAO_SKILL_TYPE_BOUNCE_NEXT_MAGIC_DAMAGE = 104,		//!< 4 反弹下一次魔法伤害的百分比
	BATTLE_FABAO_SKILL_TYPE_ABSORB_NEXT_PHYSICAL_DAMAGE = 105,	//!< 5 吸收下一次物理伤害的百分比
	BATTLE_FABAO_SKILL_TYPE_ABSORB_NEXT_MAGIC_DAMAGE = 106,		//!< 6 吸收下一次魔法伤害的百分比
	BATTLE_FABAO_SKILL_TYPE_REAL_DAMAGE = 107,					//!< 7 真实伤害
	BATTLE_FABAO_SKILL_TYPE_PHYSICAL_SHIELD = 108,				//!< 8 物理护盾
	BATTLE_FABAO_SKILL_TYPE_MAGIC_SHIELD = 109,					//!< 9 魔法护盾
	BATTLE_FABAO_SKILL_TYPE_ELEMENT_TURNOVER = 110,				//!< 10 元素反转
	BATTLE_FABAO_SKILL_TYPE_ELEMENT_CHAOS = 111,				//!< 11 元素混沌1
	BATTLE_FABAO_SKILL_TYPE_PURIFY_AND_HEAL = 112,				//!< 12 净化加回血
	BATTLE_FABAO_SKILL_TYPE_DETECT_EMNEMY_HP = 113,				//!< 13 查看敌方全体目标生命值1
	BATTLE_FABAO_SKILL_TYPE_SPEED_MODIFY = 114,					//!< 14 调整出手速度
	BATTLE_FABAO_SKILL_TYPE_REALIVE_BUFF = 115,					//!< 15 复活BUFF
	BATTLE_FABAO_SKILL_TYPE_MINE_MONSTER_DAMAGE_UP = 116,		//!< 16 每级提升拥有者对野外暗怪伤害加成%s
	BATTLE_FABAO_SKILL_TYPE_MONSTER_INJURE_DOWN = 117,			//!< 17 每级提升拥有者受到怪物伤害减免%s
	BATTLE_FABAO_SKILL_TYPE_MAX,
};

struct BattleHeartSkill
{
	BattleHeartSkill() { this->Reset(); }

	void Reset()
	{
		skill_id = 0;
		level = 0;
		remain_times = 0;
		reserve_sh = 0;
	}

	short skill_id;				//!< 心法技能ID
	short level;				//!< 心法技能等级
	short remain_times;			//!< 剩余使用次数
	short reserve_sh;
};

enum BATTLE_SIDE_TYPE
{
	BATTLE_SIDE_TYPE_ATTACKER = 0,
	BATTLE_SIDE_TYPE_DEFENDER = 1,
	BATTLE_SIDE_NUM = 2,
};

// 用于一个技能中的多个效果可以向后传递数值
enum SKILL_EFFECT_STATISTIC 
{
	SKILL_EFFECT_STATISTIC_MIN = 0,
	SKILL_EFFECT_STATISTIC_DAMAGE = SKILL_EFFECT_STATISTIC_MIN,
	SKILL_EFFECT_STATISTIC_TARGET_NUM = 1,		//目标数
	SKILL_EFFECT_STATISTIC_MAIN_TARGET_GRID_IDX = 2, // 主目标格点
	SKILL_EFFECT_STATISTIC_BURN_RAGE_POINT_NUM = 3,  // 燃烧元气数量
	SKILL_EFFECT_STATISTIC_PURIFY_NUM = 4,			 // 净化禁锢、异常状态数量
	SKILL_EFFECT_STATISTIC_MAX
};

enum IMMUNE_INJURE_TYPE
{
	IMMUNE_INJURE_TYPE_NONE = 0,
	IMMUNE_INJURE_TYPE_ALL = 1,		//!< 1. 免疫所有伤害
	IMMUNE_INJURE_TYPE_PHYSIC = 2,	//!< 2. 免疫物理伤害
	IMMUNE_INJURE_TYPE_MAGIC = 3,	//!< 3. 免疫魔法伤害
	IMMUNE_INJURE_TYPE_MAX
};

enum RACE_RELATION_TYPE
{
	RACE_RELATION_TYPE_MIN = 0,
	RACE_RELATION_TYPE_NORMAL = RACE_RELATION_TYPE_MIN,
	RACE_RELATION_TYPE_SUPPRESS,
	RACE_RELATION_TYPE_HALF_SUPPRESS,	
	RACE_RELATION_TYPE_BEING_HALF_SUPPRESSED,
	RACE_RELATION_TYPE_BEING_SUPPRESSED,
	RACE_RELATION_TYPE_MAX,
};

enum BATTLE_DAMAGE_TYPE
{
	BATTLE_DAMAGE_TYPE_PHYSIC = 0,			//物理伤害
	BATTLE_DAMAGE_TYPE_MAGIC = 1,			//法术伤害
	BATTLE_DAMAGE_TYPE_HOLY_MAGIC = 2,		//回复/能量/神圣伤害

	BATTLE_DAMAGE_TYPE_MAX
};

enum BATTLE_DAMAGE_EFFECT_TYPE
{
	BATTLE_DAMAGE_EFFECT_TYPE_PHYSIC = 0,			//物理伤害
	BATTLE_DAMAGE_EFFECT_TYPE_MAGIC_BOTH = 1,		//魔法伤害(3+4)
	BATTLE_DAMAGE_EFFECT_TYPE_BOTH = 2,				//伤害(0+3+4)
	BATTLE_DAMAGE_EFFECT_TYPE_MAGIC = 3,			//法术伤害
	BATTLE_DAMAGE_EFFECT_TYPE_HOLY_MAGIC = 4,		//回复/能量/神圣伤害

	BATTLE_DAMAGE_EFFECT_TYPE_MAX
};

enum BATTLE_PLAYER_TYPE
{
	BATTLE_PLAYER_TYPE_ALL = 0,				
	BATTLE_PLAYER_TYPE_PVP = 1,			
	BATTLE_PLAYER_TYPE_PVE = 2,

	BATTLE_PLAYER_TYPE_MAX
};

extern bool IsConformDamageEffectType(int damage_type, int damage_effect_type);
extern bool IsConformDamageEffectTypeByReason(int modify_reason, int damage_effect_type);

static const double RACE_FACTOR_ARRAY[RACE_RELATION_TYPE_MAX] = { 0.0, 0.2, 0.1, -0.15, -0.2 };

extern short RACE_RELATION_ARRAY[RACE_TYPE_MAX][RACE_TYPE_MAX];
extern void RaceRelationInit();

extern short ELEMENT_RELATION_ARRAY[ELEMENT_TYPE_MAX][ELEMENT_TYPE_MAX];
extern void ElementRelationInit();

struct BattleMove
{
	BattleMove()
	{
		this->Reset();
	}

	void Reset()
	{
		grid_idx = 0;
		move_type = BATTLE_MOVE_TYPE_INVALID;
		has_done = false;
		param1 = 0;
		param2 = 0;
		param3 = 0;
		param4 = 0;
	}

	short grid_idx;	// 目标位置
	short move_type;
	bool has_done;  // 已结算
	int param1;
	int param2;
	int param3;
	int param4;
};

bool IsSpecialMove(int move_type);

enum BATTLE_EFFECT_TARGET
{
	BATTLE_EFFECT_TARGET_EVERYONE = 1,					// 对所有目标生效
	BATTLE_EFFECT_TARGET_ONLY_MAIN = 2,					// 只对主目标生效
	BATTLE_EFFECT_TARGET_SOURCE = 3,					// 对施法者
	BATTLE_EFFECT_TARGET_SOURCE_FOR_EACH_TARGET = 4,	// 多少个target，就对施法者作用多少次
	BATTLE_EFFECT_TARGET_EXCEPT_MAIN = 5,				// 除了主目标 其他目标都生效
};

enum BATTLE_SUMMON_RULE
{
	BATTLE_SUMMON_RULE_RANDOM = 0,
	BATTLE_SUMMON_RULE_LEFT_RIGHT = 1,
	BATTLE_SUMMON_RULE_FIX_POSITION = 2,
	BATTLE_SUMMON_RULE_RANDOM_ON_FRONT_ROW = 3,	// 在前排随机召唤X只
	BATTLE_SUMMON_RULE_RANDOM_ON_BACK_ROW = 4,	// 在后排随机召唤X只
	BATTLE_SUMMON_RULE_MAX,
};

enum BATTLE_DIE_TYPE
{
	BATTLE_DIE_TYPE_NORMAL = 0,    //!< 死亡倒地
	BATTLE_DIE_TYPE_DISAPPEAR = 1, //!< 死亡原地消失
	BATTLE_DIE_TYPE_FLYAWAY = 2,   //!< 死亡击飞消失
	BATTLE_DIE_TYPE_REALIVE = 3,   //!< 死亡复活
};

struct BattleRoleList
{
	BattleRoleList() { this->Reset(); }

	void Reset()
	{
		role_count = 0;
		memset(role_id_list, 0, sizeof(role_id_list));
	}

	int role_count;
	int role_id_list[SIDE_MAX_ROLE_NUM];
};

struct BattleBuffSimpleInfo
{
	BattleBuffSimpleInfo(int _buff_type, unsigned int _unique_key) : buff_type(_buff_type), unique_key(_unique_key) {}
	BattleBuffSimpleInfo() { this->Reset(); }
	
	void Reset()
	{
		buff_type = 0;
		unique_key = 0;
	}

	int buff_type;
	unsigned int unique_key;
};

enum BATTLE_NAME_COLOR
{
	BATTLE_NAME_COLOR_NONE = 0,
	BATTLE_NAME_COLOR_PURPLE = 1,
	BATTLE_NAME_COLOR_MAX
};

#pragma pack(push)
#pragma pack(4)

struct BattleHallowInfo
{
	BattleHallowInfo() { this->Reset(); }

	void Reset()
	{
		hallow_id = 0;
		hallow_lv = 0;
		attr_active = 0;
		attr_active_max = 1;
		attr_damage = 0;
		attr_recover = 0;
		attr_knock_off = 0;
		attr_anti_knock_off = 0;
		attr_intervene = 0;
		attr_speed = 0;

		skill_num = 0;
		memset(skill_id_list, 0, sizeof(skill_id_list));
		memset(skill_lv_list, 0, sizeof(skill_lv_list));

		gift_num = 0;
		memset(gift_id_list, 0, sizeof(gift_id_list));
	}

	int hallow_id;
	int hallow_lv;
	double attr_active;
	double attr_active_max;
	double attr_damage;
	double attr_recover;
	double attr_knock_off;
	double attr_anti_knock_off;
	double attr_intervene;
	double attr_speed;

	int skill_num;
	unsigned short skill_id_list[BATTLE_HALLOW_SKILL_NUM];
	short skill_lv_list[BATTLE_HALLOW_SKILL_NUM];

	int gift_num;
	short gift_id_list[BATTLE_HALLOW_GIFT_NUM];
};

struct BattleHuanShouSkillInfo
{
	BattleHuanShouSkillInfo() { this->Reset(); }

	void Reset()
	{
		skill_id = 0;
		skill_level = 0;
		cd_start = 0;
		cd_perform = 0;
	}

	UInt16 skill_id;
	short skill_level;
	short cd_start;			//入场cd
	short cd_perform;		//释放cd
};

struct BattleCharacterInfo
{
	int character_id;				//!< id 
	short character_type;			//!< [0:主角 1:宠物 2:怪物 3:伙伴]		
	short level;					//!< 等级
	int prof;						//!< 职业
	GameName name;					//!< 名字
	char weapon_type;				//!< 武器类型 [0:空手 1:弓 2:剑 3:斧 4:杖 5:枪 6:拳套]
	char is_chaos_element;			//!< 是否带有混沌元素 [0:否 1:是]（混沌元素默认为10，不带有其他4种元素）
	ItemID weapon_id;				//!< 武器ID
	int maxhp;						//!< 最大血量
	int maxmp;						//!< 最大魔量
	int curhp;						//!< 当前血量
	int curmp;						//!< 当前魔量

	short element_water;			//!< 水元素值
	short element_fire;				//!< 火元素值
	short element_wind;				//!< 风元素值
	short element_earth;			//!< 土元素值

	char race;						//!< 种族类型 [0:人形 1:野兽 2:植物 3:不死 4:昆虫 5:特殊 6:金属 7:龙 8:飞行]
	char param;						//!< 角色类型为宠物时：宠物强化度，为伙伴时：伙伴强化度
	short grid_idx;					//!< 角色所在格子编号

	Appearance appearance;			//!< 外观（用于主角）

	int skin_id;					//!< 皮肤ID（用于宠物和伙伴，以及宠物影子）
	int top_level;					//!< 巅峰等级
	int dec_reamin_hp_shield;		//!< 剩余护盾值
	int remain_freeze_shield;		//!< 剩余冰冻护盾值
	int origin_freeze_shield;		//!< 原来的冰冻护盾值
	int spirit_point;				//!< 怒气值SP  -1代表不显示怒气值	
	int fly_flag;					//!< 飞升阶段/次数(根据角色/宠物/伙伴)字段定义		
	
	short name_color;				//!< 0 正常颜色  1 紫色
	short reserve_sh;				//!< 保留字
	int reserve_int;				//!< 保留字
};

class PB_BattleCharacterData;
struct BattleCharacterData
{
	BattleCharacterData()
	{
		this->Reset();
	}

	void Reset()
	{
		character_id = 0;
		id_param = 0;
		character_type = 0;
		is_use_cur_hp_mp = 0;
		level = 0;
		memset(name, 0, sizeof(name));
		quality = 0;

		profession = 0;
		race = 0;
		weapon_type = 0;
		weapon_id = 0;

		memset(attr_list, 0, sizeof(attr_list));

		skill_num = 0;
		memset(skill_list, 0, sizeof(skill_list));

		is_long_range_unit = 0;

		capability = 0;
		fly_flag = 0;
		wear_element_type = 0;
		reserve_int = 0;
		memset(reserve_ll, 0, sizeof(reserve_ll));
	}

	bool Serialize_Ver0(TLVSerializer& outstream) const;
	bool Deserialize_Ver0(TLVUnserializer& instream);

	bool Serialize(TLVSerializer& outstream) const;
	bool Deserialize(TLVUnserializer& instream);

	bool GetPBData(ARG_OUT PB_BattleCharacterData* out_data) const;
	bool SetDataFromPB(const PB_BattleCharacterData& pb_data);

	int character_id;								// 角色ID
	int id_param;									// 用于辅助的id参数（暂时只有宠物用到）
	char character_type;							// 角色类型		\see BATTLE_CHARACTER_TYPE
	char is_use_cur_hp_mp;							// 是否使用当前血量和魔量
	short level;									// 等级
	GameName name;									// 名字
	int quality;									// 品质

	int profession;									// 职业			\see PROF_TYPE
	char race;										// 种族			\see RACE_TYPE
	char weapon_type;								// 武器类型		\see WEAPON_TYPE
	ItemID weapon_id;								// 武器物品ID

	BattleAttr attr_list[BATTLE_ATTR_MAX];			// 属性列表

	struct SkillItem
	{
		unsigned short skill_id;					// 技能ID
		unsigned short skill_level;					// 技能等级
	};

	char param;										// 为宠物时，表示宠物的强化等级	
	char is_long_range_unit;						// 是否远程单位

	short skill_num;								// 技能数量
	SkillItem skill_list[MAX_BATTLE_SKILL_NUM];		// 技能列表

	int capability;
	short fly_flag;									// 飞升阶段/次数(根据角色/宠物/伙伴)字段定义	

	short wear_element_type;						// 携带的元素水晶类型 \see ELEMENT_TYPE_MAX
	int reserve_int;
	long long reserve_ll[2];						// 保留字
};

class PB_PetBattleData;
struct PetBattleData
{
	PetBattleData() : auto_move_type(-1), auto_skill_id(0), auto_skill_level(0), skin_id(0)
	{
		this->Reset();
	}

	void Reset()
	{
		memset(passive_list, 0, sizeof(passive_list));
		memset(passive_level_list, 0, sizeof(passive_level_list));
		auto_move_type = 0;
		auto_skill_id = 0;
		auto_skill_level = 0;
		exclusive_passive_skill_OBSOLETE = 0;
		battle_data.Reset();
		skin_id = 0;
		memset(other_sp_list, 0, sizeof(other_sp_list));
		unique_id = 0LL;
		lianyao_pet_id = 0;
		lianyao_pet_str_lv = 0;
		pet_exclusive_skill_disable_flag = 0;
		lian_yao_exclusive_skill_disable_flag = 0;
	}

	bool GetPBData(ARG_OUT PB_PetBattleData* out_data) const;
	bool SetDataFromPB(const PB_PetBattleData& pb_data);

	BattleCharacterData battle_data;
	short passive_list[BATTLE_PET_GIFT_NUM];
	short passive_level_list[BATTLE_PET_GIFT_NUM];
	short auto_move_type;
	short auto_skill_id;
	short auto_skill_level;
	short exclusive_passive_skill_OBSOLETE;			//已废弃字段

	int skin_id; // 宠物皮肤ID
	int other_sp_list[BATTLE_OTHER_EFFECT_NUM];		//特效列表

	long long unique_id;							// 宠物的唯一ID
	int lianyao_pet_id;
	int lianyao_pet_str_lv;		
	UNSTD_STATIC_CHECK(BATTLE_PET_GIFT_NUM == 20); // 修改此处会导致古神宝库的机器人数据异常，请慎重操作

	unsigned char pet_exclusive_skill_disable_flag;
	unsigned char lian_yao_exclusive_skill_disable_flag;
};

struct PetHelperBattleData
{
	PetHelperBattleData() : battle_row(0), battle_column(0) {}

	void Reset()
	{
		battle_row = 0;
		battle_column = 0;
		pet_data.Reset();
	}

	short battle_row;		// 助战宠物站位 0:后排 1:前排
	short battle_column;	// 助战宠物站位 列
	PetBattleData pet_data;
};

class PB_PartnerBattleData;
struct PartnerBattleData
{
	PartnerBattleData() { this->Reset(); }
	void Reset()
	{
		battle_data.Reset();
		skin_id = 0;
		battle_row = 0;
		battle_column = -1;
		other_sp_num = 0;
		memset(other_sp_list, 0, sizeof(other_sp_list));
		str_lv = 0;
	}

	bool GetPBData(ARG_OUT PB_PartnerBattleData* out_data) const;
	bool SetDataFromPB(const PB_PartnerBattleData& pb_data);

	BattleCharacterData battle_data;
	int skin_id;	// 伙伴皮肤ID

	short battle_row;		// 伙伴站位 0:后排 1:前排
	short battle_column;	// 伙伴站位 列

	int other_sp_num;
	int other_sp_list[BATTLE_PARTNER_OTHER_EFFECT_NUM];	//特效列表

	int str_lv;				// 强化等级
};


static const int BATTLE_POS_RELATION[ROW_CHARACTER_NUM] = { 2, 1, 3, 0, 4 };
static const int REVERSE_BATTLE_POS_RELATION[ROW_CHARACTER_NUM] = { 3, 1, 0, 2, 4 };


enum BATTLE_ROLE_OPERATE_TYPE
{
	BATTLE_ROLE_OPERATE_TYPE_INVALID = 0,
	BATTLE_ROLE_OPERATE_TYPE_SET_AUTO,					 //!< 1 设置自动    \param param1 是否自动[0:否 1:是]
	BATTLE_ROLE_OPERATE_TYPE_SET_AUTO_NORMAL_ATTACK,	 //!< 2 设置自动普攻 	\param param1 行为格[0,1]
	BATTLE_ROLE_OPERATE_TYPE_SET_AUTO_DEFENSE,			 //!< 3 设置自动防御		\param param1 行为格[0,1]
	BATTLE_ROLE_OPERATE_TYPE_SET_AUTO_PERFORM_SKILL,	 //!< 4 设置自动放技能   \param param1 行为格[0,1] \param param2 技能ID  \param param3 技能等级
	BATTLE_ROLE_OPERATE_TYPE_SET_AUTO_RUNAWAY,			 //!< 5 设置自动逃跑		\param param1 行为格[0,1]
	BATTLE_ROLE_OPERATE_TYPE_SET_AUTO_CHANGE_POSITION,	 //!< 6 设置自动换位		\param param1 行为格[0,1]	
	BATTLE_ROLE_OPERATE_TYPE_SET_AUTO_AI_SKILL,			 //!< 7 智能施法

	BATTLE_ROLE_OPERATE_TYPE_SET_MOVE_NORMAL_ATTACK = 10,		//!< 10 普攻  \param param1 行为格[0,1]  \param param2 目标格点[0,20)
	BATTLE_ROLE_OPERATE_TYPE_SET_MOVE_DEFENSE,			 //!< 11 防御  \param param1 行为格[0,1]
	BATTLE_ROLE_OPERATE_TYPE_SET_MOVE_PERFORM_SKILL,	 //!< 12 放技能 \param param1 行为格[0,1] \param param2 目标格点[0,20) \param param3 技能id  \param param4 技能等级
	BATTLE_ROLE_OPERATE_TYPE_SET_MOVE_CHANGE_POSITION,	 //!< 13 换位  \param param1 行为格[0,1]
	BATTLE_ROLE_OPERATE_TYPE_SET_MOVE_RUNAWAY,			 //!< 14 逃跑  \param param1 行为格[0,1]
	BATTLE_ROLE_OPERATE_TYPE_SET_MOVE_CHANGE_PET,		 //!< 15 换宠  \param param1 行为格[0,1]  \param param2 换上的宠物序号[0, 3)∪[-1, 0) 其中-1为只下宠不上宠
	BATTLE_ROLE_OPERATE_TYPE_SET_MOVE_USE_ITEM,			 //!< 16 使用物品  \param param1 行为格[0,1]  \param param2 目标格点[0,20) \param param3 物品栏号column  \param param4 物品序号index
	BATTLE_ROLE_OPERATE_TYPE_SET_MOVE_SEAL_PET,			 //!< 17 封印宠物 \param param1 行为格[0,1]  \param param2 目标格点[0,20)  \param param3 封印卡类型[0:普卡 1:银卡 2:金卡]  \param param4 封印的怪物ID（用于提取宠物ID以及成功率上传到战斗服)
	BATTLE_ROLE_OPERATE_TYPE_SET_MOVE_USE_FABAO,		 //!< 18 使用法宝技能 \param param1 行为格[0,1] \param param2 目标格点[0,20) \param param3 法宝序号[0,fabao_num)
	BATTLE_ROLE_OPERATE_TYPE_SET_MOVE_USE_INST_KILL_ITEM,		//!< 19 使用降妖鞭 \param param1 行为格[0,1] \param param2 目标格点[0,20) \param param3 瞬杀的怪物ID（用于成功率上传到战斗服)

	BATTLE_ROLE_OPERATE_TYPE_SET_MOVE_USE_FAKE_POTION,				//!< 20 使用假药 \param param1 行为格[0,1] \param param2 目标格点[0,20) \param param3 假药item_id 
	BTALLE_ROLE_OPERATE_TYPE_SET_MOVE_PERFORM_HEART_SKILL,			//!< 21 施放心法技能  \param param1 行为格[0,1] \param param2 目标格点[0,20)  \param param3 技能ID  \param param4  技能等级
	BTALLE_ROLE_OPERATE_TYPE_SET_MOVE_PERFORM_CHANGE_CARD_SKILL,	//!< 22 释放变身卡技能 \param param1 行为格[0,1] \param param2 目标格点[0,20)  \param param3 技能ID  \param param4  技能等级
	BTALLE_ROLE_OPERATE_TYPE_SET_MOVE_PERFORM_HUAN_SHOU_SKILL,		//!< 23 释放 幻兽技能 \param param1 行为格[0,1] \param param2 目标格点[0,20)  \param param3 技能等级 

	BATTLE_ROLE_OPERATE_TYPE_MAX
};

enum BATTLE_SEAL_CARD_TYPE // 配置表是 1 2 3
{
	BATTLE_SEAL_CARD_TYPE_COMMON = 0,
	BATTLE_SEAL_CARD_TYPE_SILVER = 1,
	BATTLE_SEAL_CARD_TYPE_GOLD = 2,
};

enum BATTLE_ROW_TYPE
{
	BATTLE_ROW_TYPE_BACK = 0,
	BATTLE_ROW_TYPE_FRONT = 1,
	BATTLE_ROW_TYPE_MAX
};

struct BattleData_StatisticRole // 统计个人战斗数据
{
	BattleData_StatisticRole() : total_damage_to_enemy(0LL), total_injure_from_enemy(0LL), total_heal_other(0LL) {}

	long long total_damage_to_enemy;
	long long total_injure_from_enemy;
	long long total_heal_other;
};

struct BattleData_BusinessRole // 个人战斗业务数据
{
	BattleData_BusinessRole() : is_leave_team_after_battle(0), reserve_sh(0) {}

	void Reset()
	{
		is_leave_team_after_battle = 0;
		reserve_sh = 0;
	}

	short is_leave_team_after_battle;
	short reserve_sh;
};

struct BattleData_Statistic		// 统计战斗数据
{
	BattleData_Statistic() : round_num(0), attacker_partner_num(0), defender_partner_num(0), total_attacker_partner_damage_to_enemy(0), total_defender_partner_damage_to_enemy(0), monster_list_size(0)
	{
		this->Reset(); 
	}

	void Reset()
	{
		round_num = 0;
		
		attacker_partner_num = 0;
		defender_partner_num = 0;
		
		total_attacker_partner_damage_to_enemy = 0;
		total_defender_partner_damage_to_enemy = 0;

		monster_list_size = 0;
		memset(monster_id_list, 0, sizeof(monster_id_list));
		memset(monster_level_list, 0, sizeof(monster_level_list));
		memset(attacker_parner_id_list, 0, sizeof(attacker_parner_id_list));
		memset(defender_parner_id_list, 0, sizeof(defender_parner_id_list));

		attacker_role_num = 0;
		defender_role_num = 0;

		attacker_alive_role_num = 0;
		defender_alive_role_num = 0;

		attacker_role_high_level = 0;
		defender_role_high_level = 0;

		attacker_role_low_level = 0;
		defender_role_low_level = 0;

		attacker_role_average_level = 0;
		defender_role_average_level = 0;

		attacker_total_damage = 0;
		defender_total_damage = 0;

		attacker_use_medicine_times = 0;
		defender_use_medicine_times = 0;

		attacker_realive_times = 0;
		defender_realive_times = 0;

		total_attacker_pet_help_damage_to_enemy = 0;
		total_defender_pet_help_damage_to_enemy = 0;

		fight_last_seconds = 0;
	}

	short round_num;			// 回合数
	short reserve_sh;
	short attacker_partner_num;
	short defender_partner_num;

	short attacker_role_num;
	short defender_role_num;

	short attacker_alive_role_num;
	short defender_alive_role_num;

	short attacker_role_high_level;
	short defender_role_high_level;
	
	short attacker_role_low_level;
	short defender_role_low_level;

	short attacker_role_average_level;
	short defender_role_average_level;

	int attacker_parner_id_list[BATTLE_MAX_ROLE_NUM - 1];
	int defender_parner_id_list[BATTLE_MAX_ROLE_NUM - 1];

	long long total_attacker_partner_damage_to_enemy;
	long long total_defender_partner_damage_to_enemy;

	// 统计战斗中的怪物
	short monster_list_size;
	int monster_id_list[SIDE_CHARACTER_NUM];
	short monster_level_list[SIDE_CHARACTER_NUM];

	long long attacker_total_damage;
	long long defender_total_damage;

	short attacker_use_medicine_times;
	short defender_use_medicine_times;

	short attacker_realive_times;
	short defender_realive_times;

	long long total_attacker_pet_help_damage_to_enemy;
	long long total_defender_pet_help_damage_to_enemy;

	int fight_last_seconds;
};

struct BattleData_Business		// 业务数据，用于原样返回给gameworld做业务处理
{
	BattleData_Business() { this->Reset(); }

	void Reset()
	{
		monster_group_id = -1;
		monster_gen_id = -1;
		courage_challenge_level = -1;
		deamon_cave_beat_waves = 0;
		team_idx = -1;

		guild_boss_seq = 0;
		guild_boss_guild_id = 0;
		guild_boss_chapter = 0;
		memset(guild_boss_monster_remain_hp_percent_list, 0, sizeof(guild_boss_monster_remain_hp_percent_list));
		param1 = 0;
		param2 = 0;
		param3 = 0;

		limit_max_round = 0;
		limit_max_time = 0;
		until_one_side_all_dead = 0;
		role_runaway_always_fail = 0;

		colosseum_target_uid = 0;
		cloud_arena_target_uid = 0;

		monster_list_size = 0;
		is_real_mine = 0;
		can_battle_pause_by_client = 0;
		memset(monster_id_list, 0, sizeof(monster_id_list));
		memset(monster_level_list, 0, sizeof(monster_level_list));

		world_boss_max_hp = 0;
		world_boss_cur_hp = 0;

		pop_offset = 0;
		data_size = 0;
		memset(data, 0, sizeof(data));

		robot_run_away_before_start = 0;
		is_record_battle = 0;
		is_record_first_kill = 0;
		is_never_critical = 0;
		record_file_id = 0;

		condition_num = 0;
		for (int i = 0; i < BATTLE_CONDITION_MAX_NUM; i++)
		{
			condition_list[i] = 0;
		}
		condition_flag = 0;

		attacker_buff_id = 0;
		attacker_buff_lv = 0;
		defender_buff_id = 0;
		defender_buff_lv = 0;
		attacker_sidebuff_id = 0;
		defender_sidebuff_id = 0;
		notice_num = 0;

		colosseum_target_uid = 0;

		memset(launch_fun_name, 0, sizeof(launch_fun_name));
		memset(launch_role_name, 0, sizeof(launch_role_name));
		launch_scene_id = 0;
		launch_scene_key = 0;
		launch_scene_posx = 0;
		launch_scene_posy = 0;
		memset(launch_stack, 0, sizeof(launch_stack));

		auto_catch_monster_id = 0;	
		auto_catch_pet_id = 0;
		available_common_card_num = 0;
		available_silver_card_num = 0;
		available_gold_card_num = 0;	
		pet_catch_type = 0;				
		succ_rate_common = 0;				
		succ_rate_silver = 0;				
		succ_rate_gold = 0;		
		pr_up_on_fail = 0;
		must_succ_times = 0;
		common_card_item_id = 0;
		silver_card_item_id = 0;
		gold_card_item_id = 0;
		is_catch_bind_pet = 0;
		is_no_auto = 0;
		is_never_flyaway = 0;
		no_sync_auto_move_setting = 0;
		no_auto_level = 0;
		only_sync_pet_auto_move = 0;
		is_allow_skip_battle = 0;

		is_force_speed = 0;
		is_use_level_change = 0;
		use_level = 0;

		is_use_system_food = 0;
		skip_waiting_time = 0;

		allow_fast_forward_round_num = 0;

		attacker_buff_num = 0;
		defender_buff_num = 0;

		memset(attacker_buff_id_list, 0, sizeof(attacker_buff_id_list));
		memset(attacker_buff_lv_list, 0, sizeof(attacker_buff_lv_list));
		memset(defender_buff_id_list, 0, sizeof(defender_buff_id_list));
		memset(defender_buff_lv_list, 0, sizeof(defender_buff_lv_list));

		uta_schedule_id = 0LL;

		monster_attr_weaken_percent = 0;
		gen_element_elf_monster = 0;
		has_kill_element_elf_monster = 0;
		element_elf_monster_id = 0;
		element_elf_reward_group = -1;
		reserve_ch1 = 0;
		allow_surrender_round_num = 0;

		mini_game_has_pet = 0;
		mini_game_has_partner = 0;
		mini_game_has_skill = 0;
		reserve_ch2 = 0;
	}

	bool IsUseSystemFood() const
	{
		return is_use_system_food > 0;
	}

	bool Push(void * src, unsigned int length)
	{
		if (data_size + length > MAX_DATA_SIZE)
		{
			return false;
		}

		memcpy(data + data_size, src, length);
		data_size += length;

		return true;
	}

	bool Pop(void * dst, unsigned int length)
	{
		if (pop_offset + length > data_size)
		{
			return false;
		}

		memcpy(dst, data + pop_offset, length);
		pop_offset += length;

		return true;
	}

	int monster_group_id;				// 用于标识是哪种怪物
	int monster_gen_id;					// 用于标识是哪只怪物 （世界boss需要）
	short courage_challenge_level;		// 勇气的挑战层数 (暂时无用)
	short deamon_cave_beat_waves;		// 魔窟打败了多少波
	int team_idx;						// 组队的情况下需要
	short guild_boss_seq;				// 家族试炼boss  标记用的是哪个boss
	short guild_boss_chapter;			// 家族试炼boss章节
	int guild_boss_guild_id;			// 家族ID
	short guild_boss_monster_remain_hp_percent_list[SIDE_CHARACTER_NUM];	// 家族试炼每只怪剩余血量（万分比）

	int param1;
	int param2;
	int param3;

	int world_boss_max_hp;				// 世界boss最大血量
	int world_boss_cur_hp;				// 世界boss当前血量
	
	int limit_max_time;					// 限制最大战斗时间（秒）
	short limit_max_round;				// 限制最大回合数
	char until_one_side_all_dead;		// 强制战斗在一边全死后才结束
	char role_runaway_always_fail;		// 角色逃跑必定失败
	int colosseum_target_uid;			// 竞技场目标uid
	int cloud_arena_target_uid;			// 云端竞技目标uid

	char can_battle_pause_by_client;	// 客户端是否允许暂停战斗（仅在等待输入阶段允许暂停)
	char is_real_mine;					// 是否真的暗雷怪（0:否   1:是）
	short monster_list_size;
	int monster_id_list[SIDE_CHARACTER_NUM];
	short monster_level_list[SIDE_CHARACTER_NUM];

	int auto_catch_monster_id;			// 自动抓宠怪物ID
	int auto_catch_pet_id;				// 自动抓宠宠物ID
	short available_common_card_num;	// 可用普卡数
	short available_silver_card_num;	// 可用银卡数
	short available_gold_card_num;		// 可用金卡数
	short pet_catch_type;				// 捕捉类型
	short succ_rate_common;				// 普卡捕捉成功率
	short succ_rate_silver;				// 银卡捕捉成功率
	short succ_rate_gold;				// 金卡捕捉成功率
	short pr_up_on_fail;				// 失败后成功率加成
	short must_succ_times;				// 必中次数
	ItemID common_card_item_id;			// 普卡物品ID
	ItemID silver_card_item_id;			// 银卡物品ID
	ItemID gold_card_item_id;			// 金卡物品ID
	char is_catch_bind_pet;				// 捕捉的是否必定是绑定的宠物
	char is_no_auto;					// 是否开始时不自动
	char is_never_flyaway;				// 是否永不被击飞
	char no_sync_auto_move_setting;		// 不同步自动行为设定到gameworld
	short no_auto_level;				// 如果主角等级低于这个数，则强制改为手动模式
	char only_sync_pet_auto_move;		// 只同步宠物的自动行为
	char is_allow_skip_battle;			// 是否允许跳过战斗

	char robot_run_away_before_start;	
	char is_record_battle;				// 是否保存录像
	char is_record_first_kill;			// 是否保存首杀录像
	char is_never_critical;				// 是否永不触发暴击		
	int record_score;					// 记录评分 评分越低越有机会上录像
	int expect_score_upper_bound;		// 期望评分的上界（若评分高于该值，则不必保存战报文件了）
	long long record_file_id;			// 记录的文件ID

	char condition_num;								// 此时传进战斗的完成条件数量
	unsigned char condition_list[BATTLE_CONDITION_MAX_NUM];	// 游戏服传入战斗服此时为哪些条件 
	unsigned char condition_flag;					// 战斗服回传游戏服得到此时哪几个条件完成(二进制运算)

	short attacker_buff_id;
	short attacker_buff_lv;
	short defender_buff_id;
	short defender_buff_lv;
	short attacker_sidebuff_id;
	short defender_sidebuff_id;

	short attacker_buff_num;
	short defender_buff_num;

	short attacker_buff_id_list[BATTLE_BUSINESS_BUFF_MAX_NUM];
	short attacker_buff_lv_list[BATTLE_BUSINESS_BUFF_MAX_NUM];
	short defender_buff_id_list[BATTLE_BUSINESS_BUFF_MAX_NUM];
	short defender_buff_lv_list[BATTLE_BUSINESS_BUFF_MAX_NUM];

	int notice_num;
	char is_force_speed;
	char is_use_level_change;
	short use_level;

	char is_use_system_food;
	char skip_waiting_time;
	short allow_fast_forward_round_num;

	long long uta_schedule_id;

	short monster_attr_weaken_percent;	// 怪物全属性削弱百分比
	char gen_element_elf_monster;
	char has_kill_element_elf_monster;
	int element_elf_monster_id;
	char element_elf_reward_group;
	char reserve_ch1;
	short allow_surrender_round_num;
	char mini_game_has_pet;
	char mini_game_has_partner;
	char mini_game_has_skill;
	char reserve_ch2;
	
	char launch_fun_name[64];
	GameName launch_role_name;
	int launch_scene_id;
	int launch_scene_key;
	short launch_scene_posx;
	short launch_scene_posy;
	char launch_stack[512];

	const unsigned int static MAX_DATA_SIZE = 8192;
	unsigned int pop_offset;
	unsigned int data_size;
	char data[MAX_DATA_SIZE];
};

enum BATTLE_INJURE_TYPE
{
	BATTLE_INJURE_TYPE_PHYSIC = 0,  //!< 物理伤害
	BATTLE_INJURE_TYPE_MAGIC = 1,	//!< 魔法伤害
	BATTLE_INJURE_TYPE_BOTH = 2,	//!< 物理与魔法伤害
};

class PB_RoleBattleData;
struct RoleBattleData
{
	RoleBattleData() : is_auto(0), role_stand_row(0), huanshou_level(0), partner_num(0), pet_num(0), fight_pet_idx(0), pet_helper_num(0),
		auto_first_move_type(0), auto_first_skill_id(0), auto_first_skill_level(0), auto_second_move_type(0), 
		auto_second_skill_id(0), auto_second_skill_level(0), weapon_attr_num(0), weapon_sp_num(0),
		other_sp_num(0), fabao_num(0), wear_title_id(0), top_level(0), reserve_ch(0), is_robot(0), 
		heart_skill_is_vaild(1), heart_skill_count(0),
		jieyi_num(0), jieyi_skill_num(0), soaring_skill_id(0), soaring_skill_lv(0), 
		qingyuan_target_uid(0), qingyuan_skill_num(0), 
		career_suppress_value(0), change_card_skill_id(0), 
		change_card_seq(0), role_extra_capability(0),
		hallow_num(0), fight_hallow_idx(-1)
	{
		memset(weapon_attr_list, 0, sizeof(weapon_attr_list));
		memset(weapon_sp_list, 0, sizeof(weapon_sp_list));
		memset(other_sp_list, 0, sizeof(other_sp_list));
		memset(guild_buff_list, 0, sizeof(guild_buff_list));
		for (int i = 0; i < ARRAY_LENGTH(heart_skill_list); i++)
		{
			heart_skill_list[i].Reset();
		}
		memset(jieyi_uid_list, 0, sizeof(jieyi_uid_list));
		memset(qingyuan_skill_list, 0, sizeof(qingyuan_skill_list));
		huanshou_skill_info.Reset();
	}

	void Reset();

	bool Serialize(char* buffer, int buffer_size, int* out_length);
	bool Deserialize(const char* data, int data_length, int* out_offset);

	bool GetPBData(ARG_OUT PB_RoleBattleData* out_data) const;
	bool SetDataFromPB(const PB_RoleBattleData& pb_data);

	const char* GetRoleCharacterAttrStr() const;

	short is_auto;			// 是否自动【 0：否 1：是】
	short role_stand_row;   // 1:前排  0:后排
	BattleCharacterData role_character_data;

	short huanshou_level;
	short partner_num;
	PartnerBattleData partner_character_data[SIDE_MAX_ROLE_NUM - 1];

	UNSTD_STATIC_CHECK(4 == SIDE_MAX_ROLE_NUM - 1);

	short pet_num;
	short fight_pet_idx;
	PetBattleData pet_character_data[ROLE_BATTLE_PET_NUM];

	short pet_helper_num;
	PetHelperBattleData pet_helper_character_data[ROLE_BATTLE_PET_HELPER_NUM];

	// 玩家自动设置
	short auto_first_move_type;
	short auto_first_skill_id;
	short auto_first_skill_level;
	short auto_second_move_type;
	short auto_second_skill_id;
	short auto_second_skill_level;

	short weapon_attr_num;
	short weapon_sp_num;
	short other_sp_num;
	short fabao_num;

	AttrInfo weapon_attr_list[BATTLE_WEAPON_ATTR_NUM];
	UNSTD_STATIC_CHECK(6 == BATTLE_WEAPON_ATTR_NUM);

	int weapon_sp_list[BATTLE_WEAPON_EFFECT_NUM];				// 武器特效 (因为可以换武器，所以把武器特效分开放）
	int other_sp_list[BATTLE_OTHER_EQUIP_EFFECT_NUM];			// 其他特效
	BattleFabao fabao_list[BATTLE_FABAO_NUM_MAX];

	Appearance appearance;

	int wear_title_id;											// 穿戴的称号

	short guild_buff_list[BATTLE_GUILD_BUFF_NUM];				// 家族BUFF_ID
	int top_level;												// 巅峰等级
	
	char reserve_ch;
	char is_robot;												// 是否机器人 [1:是 0:否]

	char heart_skill_is_vaild;									// 心法技能是否生效 0:失效 1:生效
	char heart_skill_count;		
	BattleHeartSkill heart_skill_list[BATTLE_HEART_SKILL_NUM];	// 心法技能

	int jieyi_num;
	int jieyi_uid_list[BATTLE_JIEYI_MAX_NUM];					// 结义对象Uid列表

	int jieyi_skill_num;
	int jieyi_skill_list[BATTLE_JIEYI_SKILL_MAX_NUM];			// 结义特效列表

	UInt16 soaring_skill_id;									// 飞升技能ID
	short soaring_skill_lv;										// 飞升技能等级

	int qingyuan_target_uid;
	int qingyuan_skill_num;
	int qingyuan_skill_list[BATTLE_QINGYUAN_SKILL_MAX_NUM];		// 情缘特效列表

	int career_suppress_value;									// 职业压制
	int change_card_skill_id;									// 变身卡技能ID				
	int change_card_seq;										// 变身卡索引							

	int role_extra_capability;	// 角色除战斗数据以外的额外战斗力（从配置上获得的战力，非属性）

	short hallow_num;
	short fight_hallow_idx;
	BattleHallowInfo hallow_list[BATTLE_HALLOW_NUM];

	BattleHuanShouSkillInfo huanshou_skill_info;
};

////////////////////////////////////////////////////// 战斗统计 /////////////////////////////////////////////////////

enum BATTLE_STAT_TYPE
{
	BATTLE_STAT_TYPE_INVALID = 0,	
	BATTLE_STAT_TYPE_MONSTER_REMAIN_HP_PERCENT = 1,		//统计怪物的剩余血量百分比  p1 剩余血量百分比  p2 monster_id
	BATTLE_STAT_TYPE_MONSTER_REINFORCE_TIMES = 2,		//怪物方增援了多少次（用于魔窟）
	BATTLE_STAT_TYPE_MAX,
};


///////////////////////////////////////// 特殊战斗业务数据 //////////////////////////////////////////////
enum BATTLE_BUSINESS_DATA_TYPE
{
	BATTLE_BUSINESS_DATA_TYPE_INVALID = 0,
	BATTLE_BUSINESS_DATA_TYPE_WORLD_BOSS = 1,
	BATTLE_BUSINESS_DATA_TYPE_FB = 2,
	BATTLE_BUSINESS_DATA_TYPE_SQUAD_FIGHT = 3,
	BATTLE_BUSINESS_DATA_TYPE_PLATFORM_FIGHT = 4,
	BATTLE_BUSINESS_DATA_TYPE_COURAGE_CHALLENGE = 5,	// 锢魔之塔
	BATTLE_BUSINESS_DATA_TYPE_UTA = 6,
	BATTLE_BUSINESS_DATA_TYPE_MAX
};

static const int BATTLE_WORLD_BOSS_GRID_IDX = BATTLE_SIDE_TYPE_DEFENDER * SIDE_CHARACTER_NUM + 2;

struct BattleWorldBossData
{
	BattleWorldBossData() : boss_cur_hp(0) {}

	int boss_cur_hp;
};
UNSTD_STATIC_CHECK(BATTLE_BUSINESS_DATA_BUFFER_LEN > static_cast<int>(sizeof(BattleWorldBossData)));


struct BattleFBData
{
	struct PetInfo
	{
		PetInfo() : pet_id(0), pet_origin_index(0), pet_cur_hp(0), pet_cur_mp(0) {}

		int pet_id;
		int pet_origin_index;	// 在gameworld中的index
		int pet_cur_hp;
		int pet_cur_mp;
	};

	struct PartnerInfo
	{
		PartnerInfo() : partner_id(0), partner_cur_hp(0), partner_cur_mp(0) {}

		int partner_id;
		int partner_cur_hp;
		int partner_cur_mp;
	};

	struct RoleInfo
	{
		RoleInfo() : role_id(0), role_cur_hp(0), role_cur_mp(0), pet_count(0) {}

		int role_id;
		int role_cur_hp;
		int role_cur_mp;

		int pet_count;
		PetInfo pet_list[ROLE_BATTLE_PET_NUM];
	};

	BattleFBData() : role_count(0), partner_belong_uid(0), partner_count(0) {}

	int role_count;
	RoleInfo role_list[SIDE_MAX_ROLE_NUM];

	int partner_belong_uid;
	int partner_count;
	PartnerInfo partner_list[SIDE_MAX_ROLE_NUM - 1];
};
UNSTD_STATIC_CHECK(BATTLE_BUSINESS_DATA_BUFFER_LEN > static_cast<int>(sizeof(BattleFBData)));

struct BattleSquardFightData
{
	BattleSquardFightData() : attacker_alive_num(0), defender_alive_num(0) {}

	short attacker_alive_num;
	short defender_alive_num;
};

struct BattlePlatformFightData
{
	BattlePlatformFightData() : atker_uid(0), pet_num(0), partner_num(0) {}

	int atker_uid;
	int pet_num;
	PetBattleData pet_data_list[ROW_CHARACTER_NUM];
	int partner_num;
	PartnerBattleData partner_data_list[ROW_CHARACTER_NUM];
};

struct BattleUTAData
{
	BattleUTAData() : uta_schedule_id(0){}

	long long uta_schedule_id;
};

/////////////////////////////////////////////// 特殊战斗模式-欢乐寻宠数据/////////////////////////////////////

const static int MAX_JOY_SEEKS_FAVORS_MAP_LONG = 5;//格长
const static int MAX_PLAYER_COUNT = 2;				//最多2人

enum BATTLE_JOY_SEEKS_FAVORS_OPERATE_TYPE
{
	BATTLE_JOY_SEEKS_FAVORS_OPERATE_TYPE_INVALID = 0,

	BATTLE_JOY_SEEKS_FAVORS_OPERATE_TYPE_OPEN = 1,  //寻宠一个位置，param1 = open_map	param2 = x	param3 = y
	BATTLE_JOY_SEEKS_FAVORS_OPERATE_TYPE_ITEM = 2,	//使用道具		param1 = open_map	param2 = x	param3 = y	param4 = face_pos	param5= item_seq
	BATTLE_JOY_SEEKS_FAVORS_OPERATE_TYPE_SET = 3,	//设置			param1 = open_map	param2 = x	param3 = y	param4 = face_pos	param5 = joy_type
	BATTLE_JOY_SEEKS_FAVORS_OPERATE_TYPE_SHOW = 4,	//展示			无参数

	BATTLE_JOY_SEEKS_FAVORS_OPERATE_TYPE_MAX,
};

enum JOY_SEEKS_FAVORS_TYPE//宠物种类
{
	JOY_SEEKS_FAVORS_TYPE_NO = 0,		//空
	JOY_SEEKS_FAVORS_TYPE_LITTLE = 1,	//小
	JOY_SEEKS_FAVORS_TYPE_MIDDLE = 2,	//中
	JOY_SEEKS_FAVORS_TYPE_BIG = 3,		//大
	JOY_SEEKS_FAVORS_TYPE_MAX,
};

enum JOY_SEEKS_FAVORS_INDEX		//宠物部分
{
	JOY_SEEKS_FAVORS_INDEX_NO = 0,		//空
	JOY_SEEKS_FAVORS_INDEX_LITTLE = 1,	//小 -- 宠物种类为大，占1格
	JOY_SEEKS_FAVORS_INDEX_MIDDLE = 2,	//中 -- 宠物种类为大，占2格，分2部分
	JOY_SEEKS_FAVORS_INDEX_BIG = 3,		//大 -- 宠物种类为大，占3格，分3部分
	JOY_SEEKS_FAVORS_INDEX_MAX,
};

/*
			  0
		*  *  *  *  *
		*  *  *  *  *
	3   *  *  *  *  *  1
		*  *  *  *  *
		*  *  *  *  *
			  2
规则：小型宠物只占1格，所以格子内随意安插
中型宠物占2格，所以判断面向方位，即首格往方位延伸部分不可超越边界，例，面向0，则头部所在部位不可以是（0，0）（0，1）（0，2）（0，3）（0，4）
大型宠物占3哥，所以中型宠物不成立的条件大型宠物也不成立，同时例如面向0，则头部也不可以是（1，0）（1，1）（1，2）（1，3）（1，4）
*/


enum JOY_SEEKS_FAVORS_FACE_POS		//面向
{
	JOY_SEEKS_FAVORS_FACE_POS_TOP = 0,		//上
	JOY_SEEKS_FAVORS_FACE_POS_RIGHT = 1,	//右
	JOY_SEEKS_FAVORS_FACE_POS_DOWN = 2,		//下
	JOY_SEEKS_FAVORS_FACE_POS_LEFT = 3,		//左
	JOY_SEEKS_FAVORS_FACE_POS_MAX,
};

struct RandJoySeeksFavorsList
{
	RandJoySeeksFavorsList() : little_pos(0, 0), middle_pos(0, 0), middle_face(0), big_pos(0, 0), big_face(0) {}
	Posi little_pos;

	Posi middle_pos;
	int middle_face;

	Posi big_pos;
	int big_face;
};

const static int MAX_JOY_SEEKS_FAVORS_MAP_RULE_MIDDLE_X_END[JOY_SEEKS_FAVORS_FACE_POS_MAX] = { -1,0,1,0 };//根据规则知道面向规则与中型宠物末端X关系
const static int MAX_JOY_SEEKS_FAVORS_MAP_RULE_MIDDLE_Y_END[JOY_SEEKS_FAVORS_FACE_POS_MAX] = { 0,1,0,-1 };//根据规则知道面向规则与中型宠物末端Y关系

const static int MAX_JOY_SEEKS_FAVORS_MAP_RULE_BIG_X_END[JOY_SEEKS_FAVORS_FACE_POS_MAX] = { -2,0,2,0 };//根据规则知道面向规则与大型宠物末端X关系
const static int MAX_JOY_SEEKS_FAVORS_MAP_RULE_BIG_Y_END[JOY_SEEKS_FAVORS_FACE_POS_MAX] = { 0,2,0,-2 };//根据规则知道面向规则与大型宠物末端Y关系

struct JoySeeksFavorsMapPoint
{
	JoySeeksFavorsMapPoint() : is_open(0), joy_index(JOY_SEEKS_FAVORS_INDEX_NO), joy_seq(JOY_SEEKS_FAVORS_TYPE_NO) {}

	JoySeeksFavorsMapPoint& operator=(const JoySeeksFavorsMapPoint &data)
	{
		is_open = data.is_open;
		joy_index = data.joy_index;
		joy_seq = data.joy_seq;

		return *this;
	}

	char is_open;
	char joy_index;
	short joy_seq;
};

struct SpecialBattleRoleHandInfo
{
	SpecialBattleRoleHandInfo() :role_id(0), bg_netid(INVALID_BGNETID),m_avatar_type(0), find_pet(0), find_item(0),m_avatar_timestamp(0){}

	SpecialBattleRoleHandInfo& operator=(const SpecialBattleRoleHandInfo &data)
	{
		role_id = data.role_id;
		bg_netid = data.bg_netid;
		F_STRNCPY(role_name,data.role_name,sizeof(GameName));
		m_avatar_type = data.m_avatar_type;
		find_pet = data.find_pet;
		find_item = data.find_item;
		m_avatar_timestamp = data.m_avatar_timestamp;

		return *this;
	}

	int					role_id;
	BGNetID				bg_netid;
	GameName			role_name;
	short				m_avatar_type;									// 虚拟人物类型
	char				find_pet;										// 道具鸡
	char				find_item;										// 道具物品
	long long			m_avatar_timestamp;								// 玩家自上传头像
};

struct JoySeeksFavorsMap
{
	JoySeeksFavorsMap() : map_index(0){}
	int map_index;
	SpecialBattleRoleHandInfo role_info;
	JoySeeksFavorsMapPoint player_joy_map[MAX_JOY_SEEKS_FAVORS_MAP_LONG][MAX_JOY_SEEKS_FAVORS_MAP_LONG];
};
#pragma pack(pop)


enum ANIMATION_SPEED
{
	ANIMATION_SPEED_NORMAL = 0,	//!< 0 默认速度 （1倍速）
	ANIMATION_SPEED_DOUBLE = 1,	//!< 1 双倍速
	ANIMATION_SPEED_HALF = 2,	//!< 2 半倍速
	ANIMATION_SPEED_MAX = 3,
};

int GetPercentNumByAttrType(int attr_type);
bool IsRatioAttrType(int attr_type); // 判断属性类型是否用比例计算
BattleAttr GetAttrChangeNum(int attr_type, BattleAttr cur_attr, int percent);


enum FAKE_POTION_TYPE
{
	FAKE_POTION_TYPE_HP = 0, // 回血药   param1 回血百分比  param2 净化百分比
	FAKE_POTION_TYPE_MP = 1, // 回蓝药   param1 回蓝百分比
	FAKE_POTION_TYPE_REALIVE = 2, // 复活药  param1 回血百分比
	FAKE_POTION_TYPE_MAX
};

enum BATTLE_LINK_TYPE
{
	BATTLE_LINK_TYPE_DROP_HP_TOGETHER = 0,   // 铁索连环 一起扣血
	BATTLE_LINK_TYPE_HEART_DEVIL_LINK = 1,	 // 心魔连接
	BATTLE_LINK_TYPE_HP_SYMBIOSIS = 2,		 // 生命共生
	BATTLE_LINK_TYPE_MAX,
};

enum BATTLE_PURIFY_REASON
{
	BATTLE_PURIFY_SKILL = 0,			// 技能解控
	BATTLE_PURIFY_FABAO = 1,			// 法宝解控
	BATTLE_PURIFY_MEDICINE = 2,			// 药品解控
	BATTLE_PURIFY_SPECIAL_EFFECT = 3,	// 特效解控
	BATTLE_PURIFY_MAX
};

enum BATTLE_EXTRA_PERFORM_SKILL_TARGET_TYPE
{
	BATTLE_EXTRA_PERFORM_SKILL_TARGET_ORIGINAL = 0, // 以原目标为新目标
	BATTLE_EXTRA_PERFORM_SKILL_TARGET_RANDOM_ON_ORIGINAL_DEAD = 1, // 当原目标死亡时随机
	BATTLE_EXTRA_PERFORM_SKILL_TARGET_RANDOM_EXCEPT_ORIGIN = 2,	// 随机一个新目标(不会随机到原目标)
	BATTLE_EXTRA_PERFORM_SKILL_TARGET_MAX,
};

enum BATTLE_SEED_TYPE
{
	BATTLE_SEED_TYPE_INVALID = 0,
	BATTLE_SEED_TYPE_WATER = 1,				//!< 水
	BATTLE_SEED_TYPE_FIRE = 2,				//!< 火
	BATTLE_SEED_TYPE_WIND = 3,				//!< 风
	BATTLE_SEED_TYPE_EARTH = 4,				//!< 土
	BATTLE_SEED_TYPE_MAX
};

#endif
