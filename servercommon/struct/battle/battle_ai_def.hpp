#ifndef __BATTLE_AI_DEF_HPP__
#define __BATTLE_AI_DEF_HPP__

#include "servercommon/servercommon.h"

////////////////////////////////////////////////////// BATTLE_AI ///////////////////////////////////////////////////
enum BATTLE_AI_TYPE
{
	BATTLE_AI_TYPE_INVALID = 0,
	BATTLE_AI_TYPE_TEST = 1,
	BATTLE_AI_TYPE_GOBLIN_PET = 2,					// 会变狂暴的哥布林宠物
	BATTLE_AI_TYPE_RESURRECT_WITCH = 3,				// 会复活玩家的女巫
	BATTLE_AI_TYPE_TARGET_LOWEST_HP_PERCENT = 4,	// 只打血量最少的目标
	BATTLE_AI_TYPE_PARTNER = 5,						// 伙伴
	BATTLE_AI_TYPE_AUTO_CHARACTER = 6,				// 自动的角色（主角、宠物、主角影子、宠物影子）
	BATTLE_AI_TYPE_NPC_HELPER = 7,					// 助战NPC
	BATTLE_AI_TYPE_JIANG_ZI_YA = 8,					// 姜子牙伙伴AI
	BATTLE_AI_TYPE_TARGET_HIGH_MP_PERCENT = 9,		// 只打蓝量最高的目标(查找顺序:先宠物,没有宠物再随机查找,默认敌方) 
	BATTLE_AI_TYPE_DA_SI_MING = 10,					// 大司命伙伴AI
	BATTLE_AI_TYPE_MAX,
};

static const int BATTLE_AI_MOVE_MAX_NUM = 20;

struct BattleAIMove
{
	BattleAIMove() : priority(0), move_type(0), move_cd(0), move_cd_in_config(0), move_param1(0), move_param2(0), move_param3(0), no_reduce_cd_flag(0), is_attack_move(false) {}

	bool IsValid() { return 0 != move_type; }

	short priority;
	short move_type;
	short move_cd;
	short move_cd_in_config;

	int move_param1;
	int move_param2;
	int move_param3;

	short no_reduce_cd_flag;   // 值为1时， 本回合不减CD

	bool is_attack_move;	// 是否一个攻击行为
};

enum ASSIGN_TARGET_TYPE // 指定目标类型
{
	ASSIGN_TARGET_TYPE_INVALID = 0,
	ASSIGN_TARGET_TYPE_ALL = 1,					// 全体随机一个目标
	ASSIGN_TARGET_TYPE_ALL_ENEMY = 2,			// 全体敌人随机一个目标
	ASSIGN_TARGET_TYPE_ONE_ENEMY = 3,			// 指定敌人一个目标
	ASSIGN_TARGET_TYPE_ALL_FRIEND = 4,			// 全体友军随机一个目标
	ASSIGN_TARGET_TYPE_ONE_FRIEND = 5,			// 指定友军一个目标
	ASSIGN_TARGET_TYPE_ALL_FRIEND_BUT_SELF = 6, // 全体友军随机一个目标但自己除外
	ASSIGN_TARGET_TYPE_SELF = 7,				// 指定自己为目标
	ASSIGN_TARGET_TYPE_FRIEND_MONSTER = 8,		// 指定友军怪物
	ASSIGN_TARGET_TYPE_NOT_SAME_ID_WITH_SELF = 9,//随机一个与自己ID不一样的目标
	ASSIGN_TARGET_TYPE_ENEMY_IN_CHAOS = 10,		// 随机一个处于混乱状态的敌人
	ASSIGN_TARGET_TYPE_MAX,
};

enum BATTLE_STRATEGY
{
	BATTLE_STRATEGY_INVALID = -1,		//!< -1 无
	BATTLE_STRATEGY_OTHER = 0,			//!< 0 其他
	BATTLE_STRATEGY_FOCUS_FIRE = 1,		//!< 1 集火
	BATTLE_STRATEGY_HEAL = 2,			//!< 2 治疗
	BATTLE_STRATEGY_REALIVE = 3,		//!< 3 复活
	BATTLE_STRATEGY_CONTROL = 4,		//!< 4 控制
	BATTLE_STRATEGY_BREAK_DEFENCE = 5,	//!< 5 破防
	BATTLE_STRATEGY_BURN_MANA = 6,		//!< 6 打蓝
	BATTLE_STRATEGY_DEFENSE = 7,		//!< 7 防御
	BATTLE_STRATEGY_PURIFY = 8,			//!< 8 净化
	BATTLE_STRATEGY_MAX = 9,
};

extern int STRATEGY_PRIORITY_ARRAY[BATTLE_STRATEGY_MAX];

#endif
