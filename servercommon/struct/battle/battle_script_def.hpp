#ifndef __BATTLE_SCRIPT_DEF_HPP__
#define __BATTLE_SCRIPT_DEF_HPP__

#include "servercommon/struct/battle/battle_def.hpp"

////////////////////////////////////////////////////// BATTLE_MOVE_SCRIPT 行为脚本//////////////////////////////////////////////
enum MOVE_SCRIPT_CHECK_POINT
{
	MOVE_SCRIPT_CHECK_POINT_MIN = 0,
	MOVE_SCRIPT_CHECK_POINT_ON_ROUND_START = MOVE_SCRIPT_CHECK_POINT_MIN,
	MOVE_SCRIPT_CHECK_POINT_ON_CHARACTER_DIE = 1,
	MOVE_SCRIPT_CHECK_POINT_ON_ONE_SIDE_ALL_DEAD = 2,
	MOVE_SCRIPT_CHECK_POINT_ON_ROUND_END = 3,
	MOVE_SCRIPT_CHECK_POINT_ON_ROUND_PREPARE = 4, // 回合准备阶段
	//MOVE_SCRIPT_CHECK_POINT_ON_CHARACTER_START_MOVE = 5, // 角色开始行动时
	MOVE_SCRIPT_CHECK_POINT_MAX,
};

enum SCRIPT_EXCUTE_TYPE
{
	SCRIPT_EXCUTE_TYPE_MIN = 0,
	SCRIPT_EXCUTE_TYPE_NORMAL = SCRIPT_EXCUTE_TYPE_MIN, // 0 正常逻辑
	SCRIPT_EXCUTE_TYPE_DISABLE_ON_CONDITION_FALSE,		// 1 当条件不成立时停用脚本
	SCRIPT_EXCUTE_TYPE_COUNTER_SCRIPT_EXCUTE,			// 2 当条件不成立时调用反脚本事件
	SCRIPT_EXCUTE_TYPE_MAX,
};

enum MOVE_SCRIPT_COMPARE_TYPE
{
	MOVE_SCRIPT_COMPARE_TYPE_GREATER,
	MOVE_SCRIPT_COMPARE_TYPE_LESS,
	MOVE_SCRIPT_COMPARE_TYPE_EQUAL,
	MOVE_SCRIPT_COMPARE_TYPE_NOT_EQUAL,
	MOVE_SCRIPT_COMPARE_TYPE_GTEATER_EQUAL,
	MOVE_SCRIPT_COMPARE_TYPE_LESS_EQUAL,
	MOVE_SCRIPT_COMPARE_TYPE_DEVIDED_BY,     // 整除
	MOVE_SCRIPT_COMPARE_TYPE_NOT_DEVIDED_BY, // 不整除 
	MOVE_SCRIPT_COMPARE_TYPE_MAX,
};

enum MOVE_SCRIPT_CHECK_COND_TYPE	// 我方：怪物方    敌方：玩家方 
{
	MOVE_SCRIPT_COND_TYPE_ROUND_NUM,			// 比较现时的回合数  compare_num：回合数
	MOVE_SCRIPT_COND_TYPE_MONSTER_ID,			// 比较我方某单位的怪物ID  compare_num: 怪物ID   param1:格点
	MOVE_SCRIPT_COND_TYPE_MONSTER_HP_PERCENT,	// 比较我方某单位(可指定怪物)的血量百分比  compare_num: 百分比  param1:格点  param2:monster_id(可选)
	MOVE_SCRIPT_COND_TYPE_ENEMY_SPECIAL_STATE,	// 比较敌方某单位是否正在某状态  compare_num:1或0（是或否)   param1:格点  param2：状态值
	MOVE_SCRIPT_COND_TYPE_MONSTER_NUM,			// 比较我方怪物数  compare_num:怪物数  param1:指定怪物ID（可选）
	MOVE_SCRIPT_COND_TYPE_MONSTER_SPECIAL_STATE,// 比较我方某单位是否正在某状态  compare_num:1或0 (是或否)   param1:格点  param2: 状态值
	MOVE_SCRIPT_COND_TYPE_HP_PERCENT,			// 比较某单位的血量百分比  compare_num: 百分比  param1:side[0,1] param2:格点[0,10)
	MOVE_SCRIPT_COND_TYPE_ENEMY_NUM,			// 比较敌方单位数  compare_num: 单位数  param1:是否计算宠物  param2:是否计算伙伴
	MOVE_SCRIPT_COND_TYPE_RAND_NUM,				// 比较一个随机数  compare_num: 随机数  param1:随机数上限X (0, X]
	MOVE_SCRIPT_COND_TYPE_MP_PERCENT,			// 比较某单位的蓝量百分比  compare_num: 百分比  param1:side[0,1] param2:格点[0,10)
	MOVE_SCRIPT_COND_TYPE_CHARA_IMPRISON_STATE,	// 比较某单位是否正处于禁锢状态 compare_num:1或0  param1:side[0,1] param2:格点[0,10)
	MOVE_SCRIPT_COND_TYPE_CHARA_ABNORMAL_STATE, // 比较某单位是否正处于异常状态 compare_num:1或0  param1:side[0,1] param2:格点[0,10)
	MOVE_SCRIPT_COND_TYPE_CHARACTER_NUM_ON_ROW,	// 比较某排的单位数 compare_num: 单位数  param1:side[0:玩家方,1:怪物方] param2:[0:后排,1:前排] 
	MOVE_SCRIPT_COND_TYPE_SIDE_HAS_IMPRISON_STATE,	// 比较某方是否存在处于禁锢状态的单位 compare_num:填1  param1:side[0:玩家方,1:怪物方] 
	MOVE_SCRIPT_COND_TYPE_SIDE_HAS_ABNORMAL_STATE,	// 比较某方是否存在处于异常状态的单位 compare_num:填1  param1:side[0:玩家方,1:怪物方] 
	MOVE_SCRIPT_COND_TYPE_CHARACTER_ELEMENT_10,		// 比较玩家方某单位是否10水或10火或10风或10地  compare_num:填1  param1:格点[0,10)  param2:元素类型[1水 2火 3土 4风]
	MOVE_SCRIPT_COND_TYPE_MONSTER_DIE_ORDER,		// 比较第X个阵亡的怪物方格点 compare_num:格点[0,10) param1：第X个阵亡的，就填X，取值范围[0,10)
	MOVE_SCRIPT_COND_TYPE_MONSTER_HP,		// 比较我方怪物血量数值 compare_num:血量 param1:格点[0,10)  param2:monster_id（可选）  ps:此脚本只查找怪物方   
	MOVE_SCRIPT_COND_TYPE_IN_SHIELD,		// 比较单位是否在生命护盾下（拥有类型为37的buff) compare_num:填1  param1:side[0,1] param2:格点[0,10)
	MOVE_SCRIPT_COND_TYPE_PET_NUM,			// 比较玩家方场上宠物数量  compare_num:宠物数
	MOVE_SCRIPT_COND_TYPE_MAX,
};

enum MOVE_SCRIPT_EVENT
{
	MOVE_SCRIPT_EVENT_INVALID = 0,
	MOVE_SCRIPT_EVENT_SUMMON = 1,						// 召唤  param1:召唤seq  param2:格点[0,10)或-1（指定格点作为源，-1表示没有源）
	MOVE_SCRIPT_EVENT_SET_MONSTER_MOVE_PRIORITY = 2,	// 设置怪物行为优先度  param1:格点[0,10)  param2:monster_id(可选)  param3:行为序号  param4:新的优先度
	MOVE_SCRIPT_EVENT_SIDE_HEAL = 3,					// 某一方全体回血  param1:side[0,1]  param2:0为百分比，1为数值  param3:回复数值
	MOVE_SCRIPT_EVENT_SIDE_DAMAGE = 4,					// 某一方全体扣血  param1:side[0,1]  param2:0为百分比，1为数值  param3:回复数值
	MOVE_SCRIPT_EVENT_SET_MONSTER_NEXT_MOVE = 5,		// 设置怪物下一次的行为  param1:格点[0,10)  param2:monster_id(可选) param3:行为类型  param4:行为参数p1,  param5:行为参数p2   （PS：p1,p2的含义与怪物行为的p1 p2一样）
	MOVE_SCRIPT_EVENT_ADD_BUFF_TO_CHARACTER = 6,		// 加buff给某个角色  param1:side[0,1] param2:格点[0,10) param3:buff_id  param4:buff_level
	MOVE_SCRIPT_EVENT_SET_MONSTER_ASSIGN_MOVE = 7,		// 设置怪物指定行为与目标 param1:格点[0,10) param2:monster_id(可选) param3:指定行为序号[0, move_num) param4:指定目标类型  param5:指定目标
	MOVE_SCRIPT_EVENT_REINFORCE = 8,					// 增援  param1:新的怪物组ID
	MOVE_SCRIPT_EVENT_KILL_ALL_MONSTER = 9,				// 杀死所有怪物
	MOVE_SCRIPT_EVENT_MONSTER_COMBINE_ATTACK = 10,		// 怪物合击(让怪物集中攻击某个格点） param1:side[0,1] param2:格点[0,10) param3:monster_id (为0则是全体怪物，否则指定怪物）
	MOVE_SCRIPT_EVENT_CANCEL_MONSTER_ASSIGN_MOVE = 11,	// 取消怪物指定行为 param1:格点[0,10) param2:monster_id(可选) 
	MOVE_SCRIPT_EVENT_LOWEST_HP_SUFFER_DAMAGE = 12,		// 血量最低的单位受到伤害 param1:side[0,1] param2:血量类型[0:百分比 1:数值] param3:受到伤害数 param4:伤害数类型[0:最大生命百分比 1:当前生命百分比 2:数值]
	MOVE_SCRIPT_EVENT_FIND_TARGET_SUFFER_DAMAGE = 13,	// 使目标受到伤害 param1:目标配置seq  param2:受到伤害数  param3:伤害数类型[0:最大生命百分比 1:当前生命百分比 2:数值] 
	MOVE_SCRIPT_EVENT_SWITCH_POSITION = 14,				// 使目标前后位置调换 param1:side[0,1] param2:格点[0,10)
	MOVE_SCRIPT_EVENT_FIND_TARGET_ADD_BUFF = 15,		// 加buff给目标 param1:目标配置seq  param2:buff_id  param3:buff_level param4:概率百分比[0,100]  填0或100都是必中（为了兼容旧的脚本配置）
	MOVE_SCRIPT_EVENT_FIND_TARGET_COMBINE_ATTACK = 16,	// 怪物查找目标并合击 param1:目标配置seq  param2:monster_id （为0则全体怪物都会合击，否则指定怪物进行合击）
	MOVE_SCRIPT_EVENT_DISABLE_SCRIPT = 17,				// 令当前脚本失效 param1:随意
	MOVE_SCRIPT_EVENT_FIND_TARGET_AND_KILL = 18,		// 系统查找目标并处死  param1:目标配置seq  param2:是否击飞(1:是 0:否)
	MOVE_SCRIPT_EVENT_ENABLE_OR_DISABLE_SCRIPT = 19,	// 令脚本有效或无效 param1:脚本ID  param2:[0:无效 1:生效]
	MOVE_SCRIPT_EVENT_FIND_TARGET_MODIFY_HP = 20,		// 查找目标并修改血量 param1:目标配置seq param2:修改的血量（正数加血，负数减血）
	MOVE_SCRIPT_EVENT_FIND_TARGET_MODIFY_MP = 21,		// 查找目标并修改蓝量 param1:目标配置seq param2:修改的蓝量（正数加蓝，负数减蓝）
	MOVE_SCRIPT_EVENT_FIND_TARGET_MONSTER_ATTACK = 22,	// 查找目标并释放攻击行为（普攻或技能） param1:目标配置seq  
	MOVE_SCRIPT_EVENT_FIND_TARGET_ADD_HP_BY_ATK = 23,	// 查找目标并加血，血量为攻击力数值 param1:目标配置seq 
	MOVE_SCRIPT_EVENT_FIND_TARGET_AND_BIANSHEN = 24,	// 查找目标并变身（目标必须是怪物) param1:目标配置seq   param2：变身成怪物ID
	MOVE_SCRIPT_EVENT_SUMMON_BY_ROLE_NUM = 25,			// 按照玩家数量召唤  param1:召唤seq  param2:源格点[0,10)或-1（指定格点作为源，-1表示没有源）param3:最少召唤数量
	MOVE_SCRIPT_EVENT_SET_NPC_HELPER_NEXT_MOVE = 26,	// 设置助战NPC下一次的行为  param1:格点[0,10)  param2:助战npc_id(可选) param3:行为类型  param4:行为参数p1,  param5:行为参数p2  
	MOVE_SCRIPT_EVENT_SET_MONSTER_NEXT_SECOND_MOVE = 27,// 设置怪物下一次的二动行为 param1:格点[0,10)  param2:monster_id(可选) param3:行为类型  param4:行为参数p1,  param5:行为参数p2   （PS：p1,p2的含义与怪物行为的p1 p2一样）
	MOVE_SCRIPT_EVENT_SUMMON_NPC_HELPER = 28,			// 召唤助战NPC param1:召唤的助战NPC_ID param2:格点[0,10)
	MOVE_SCRIPT_EVENT_CLIENT_SPECIAL_NEED = 29,			// 客户端特殊需求 param1:类型(1:对话框 2:切镜头 3:指引)  param2:对应参数(若为对话框，则填对话ID，若为切镜头，则填镜头特效ID，若为指引，则填指引ID）
	MOVE_SCRIPT_EVENT_CLIENT_CUSTOM_EVENT = 30,			// 客户端自定义脚本需求 param1 param2 param3 param4 param5 param6 将原样返回给客户端，就是配置填什么下发的就是什么
	MOVE_SCRIPT_EVENT_SIDE_HEAL_BY_FAKE_SKILL = 31,		// 某一方全体回血(客户端表现为某个角色释放技能）  param1:side[0,1]  param2:0为百分比，1为数值  param3:回复数值 param4:释放技能者格点[0, 10) param5:释放技能ID param6:释放技能等级
	MOVE_SCRIPT_EVENT_SIDE_DAMAGE_BY_FAKE_SKILL = 32,	// 某一方全体扣血(客户端表现为某方被技能击中扣血）param1:side[0,1]  param2:0为百分比，1为数值  param3:扣血数值 param4:释放技能者格点[0, 20) 或 -1 (-1即没有释放者） param5:释放技能ID param6:释放技能等级 
	MOVE_SCRIPT_EVENT_SET_MONSTER_NEXT_SECOND_MOVE_2 = 33, // 设置怪物下一次的二动行为（从行为配置中选择） param1:格点[0,10)  param2:monster_id(可选)  param3:行为序号 param4:指定目标类型  param5:指定目标
	MOVE_SCRIPT_EVENT_FIND_TARGET_MODIFY_HP_PERCENT = 34,  // 查找目标并按百分比修改血量 param1:目标配置seq  param2:修改的血量百分比（正数加血，负数减血）
	MOVE_SCRIPT_EVENT_SUMMON_HEART_DEVIL = 35,			// 前排召唤心魔并附带 param1 召唤心魔seq  param2 灵魂链buff_id  param3 灵魂链buff_lv
	MOVE_SCRIPT_EVENT_FIND_TARGET_SUFFER_DAMAGE_FAKE_SKILL = 36,	// 使目标受到伤害(表现为假的技能) param1:目标配置seq  param2:受到伤害数  param3:伤害数类型[0:最大生命百分比 1:当前生命百分比 2:数值]  param4:释放技能格点[0,20)或-1代表无   param5:代表释放技能ID  param6:代表释放技能等级
	MOVE_SCRIPT_EVENT_FIND_TARGET_MONSTER_ADD_SPECIAL_EFFECT = 37,	// 查找怪物目标并给目标加上特效 param1: 目标配置seq param2:特效ID 
	MOVE_SCRIPT_EVENT_FIND_TARGET_AND_BIANSHEN_2 = 38,	// 查找目标并变身,保留当前血量（目标必须是怪物) param1:目标配置seq   param2：变身成怪物ID
	MOVE_SCRIPT_EVENT_FIND_TARGET_MONSTER_REMOVE_SPECIAL_EFFECT = 39, // 查找怪物目标并移除目标身上特效 param1: 目标配置seq param2:特效ID 

	MOVE_SCRIPT_EVENT_MAX,
};

enum MOVE_SCRIPT_TARGET //!< 脚本目标类型 \see（结合side_type --> [0:攻方、1:守方、2:双方]）
{
	MOVE_SCRIPT_TARGET_INVALID = 0,
	MOVE_SCRIPT_TARGET_ALL_CHARACTER = 1,						//!< 所有角色
	MOVE_SCRIPT_TARGET_RANDOM_ONE_CHARACTER = 2,				//!< 随机一个角色
	MOVE_SCRIPT_TARGET_ALL_CHARACTER_IN_STATE = 3,				//!< 所有处于某状态的角色 param1: 状态号 \see BATTLE_SPECIAL_STATE
	MOVE_SCRIPT_TARGET_ALL_CHARACTER_IN_ABNORMAL_DEBUFF = 4,	//!< 所有处于异常状态下的角色 
	MOVE_SCRIPT_TARGET_ALL_CHARACTER_IN_IMPRISON_DEBUFF = 5,	//!< 所有处于禁锢状态下的角色
	MOVE_SCRIPT_TARGET_ONE_CHARACTER = 6,						//!< 查找指定一个角色 param1:side[0:攻方 1:守方] param2:grid_idx[0,10) 
	MOVE_SCRIPT_TARGET_ONE_ROW = 7,								//!< 指定一排角色 param1:side[0:玩家方 1:怪物方] param2:row[0:后排 1:前排]
	MOVE_SCRIPT_TARGET_ONE_SIDE = 8,							//!< 指定一边角色 param1:side[0:玩家方 1:怪物方]
	MOVE_SCRIPT_TARGET_RANDOM_ONE_CHARACTER_IN_ABNORMAL_DEBUFF = 9,	//!< 随机一个处于异常状态下的角色
	MOVE_SCRIPT_TARGET_RANDOM_ONE_CHARACTER_IN_IMPRISON_DEBUFF = 10,//!< 随机一个处于禁锢状态下的角色
	MOVE_SCRIPT_TARGET_RANDOM_ONE_CHARACTER_IN_ONE_ROW = 11,	//!< 随机某排一个角色 param1:side[0:玩家方 1:怪物方] param2:row[0:后排 1:前排]
	MOVE_SCRIPT_TARGET_LOWEAST_DEFENSE_CHARACTER = 12,			//!< 查找最低防御的角色 param1:side[0:玩家方 1:怪物方]
	MOVE_SCRIPT_TARGET_LOWEAST_DAMAGE_LAST_ROUND_CHARACTER = 13,//!< 查找上回合造成最低伤害的角色 param1:side[0:玩家方 1:怪物方]

	MOVE_SCRIPT_TARGET_MAX
};

struct BattleCheckCondition
{
	BattleCheckCondition() : condition_type(0), compare_type(0), compare_num(0), cond_param1(0), cond_param2(0) {}

	short condition_type;
	short compare_type;
	int compare_num;

	int cond_param1;
	int cond_param2;
};

struct BattleScriptEvent
{
	BattleScriptEvent() : move_script_idx(0), script_event_type(0), param1(0), param2(0), param3(0), param4(0), param5(0), param6(0) {}

	short move_script_idx;
	short script_event_type;
	int param1;
	int param2;
	int param3;
	int param4;
	int param5;
	int param6;
};

struct BattleMoveScript // 行为脚本
{
	BattleMoveScript() : script_id(0), is_disable(false), check_point_type(0), condition_excute_type(0), check_condition_num(0), script_event_num(0), counter_script_event_num(0) {}

	bool IsDisable() { return is_disable; }

	int script_id;
	bool is_disable;
	short check_point_type;
	short condition_excute_type;

	short check_condition_num;
	BattleCheckCondition check_condition_list[BATTLE_SCRIPT_CONDITION_NUM];

	short script_event_num;
	BattleScriptEvent script_event_list[BATTLE_SCRIPT_EVENT_NUM];

	short counter_script_event_num;
	BattleScriptEvent counter_script_event_list[BATTLE_SCRIPT_EVENT_NUM];
};

#endif