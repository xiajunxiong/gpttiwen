#ifndef __BATTLE_CONDITION_DEF_HPP__
#define __BATTLE_CONDITION_DEF_HPP__

enum BATTLE_CONDITION_TYPE
{
	BATTLE_COND_WIN = 0,					// 通关 
	BATTLE_COND_WIN_ROUND_BELOW_X,			// X回合结束战斗 param1 回合数
	BATTLE_COND_DEAD_CHARACTER_BELOW_X,		// 阵亡角色小于X名 param1 为0时：每个回合都要判断 为1时：战斗结束时判断  param2：阵亡数量
	BATTLE_COND_CHARACTER_HP_PERCENT,		// 战斗结束时生命百分比大于X% param1 为0时：全队 为1时：仅主角  param2:生命值百分比
	BATTLE_COND_KILL_MONSTER,				// X回合内击杀怪物 param1 回合数  param2 格点  param3 怪物ID
	BATTLE_COND_WIN_WITH_PARTNER,			// 携带伙伴通关 param1 伙伴ID
	BATTLE_COND_WIN_WITH_PARTNER_AND_PET,	// 携带伙伴与宠物通关 param1 伙伴ID param2 宠物ID
	BATTLE_COND_WIN_WITH_PET_QUALITY,		// 携带X个Y品质宠物通关  param1 宠物个数 param2 品质
	BATTLE_COND_WIN_WITH_PARTNER_PROF,		// 携带X个Y职业伙伴通关  param1 伙伴个数 param2 职业
	BATTLE_COND_WIN_WITH_PET_LEVEL,			// 携带至少X个Y强化等级宠物通关  param1 宠物个数 param2 强化等级
	BATTLE_COND_WIN_WITH_PARTNER_QUALITY,	// 携带X个Y品质伙伴通关	 param1 伙伴数量  param2 品质
	BATTLE_COND_WIN_WITH_PARTNER_SEX,		// 携带X个Y性别伙伴通关  param1 伙伴数量  param2 性别
	BATTLE_COND_WIN_WITH_PETID_STR_LEVEL,	// 携带+X强化等级的Y宠物ID通关  param1 宠物id  param2强化等级
	BATTLE_COND_WIN_WITH_NUM_LEVEL_QUA,		// 携带X个 + Y强化等级的z品质宠物 param1 宠物个数  param2强化等级  param3 品质

	BATTLE_COND_TYPE_NUM
};

struct BattleCond
{
	BattleCond() : cond_type(-1), param1(0), param2(0), param3(0) {}

	int cond_type;
	int param1;
	int param2;
	int param3;
};

/*
struct CondData_WinWithPartner
{

};

union BattleCondData
{

};*/

#endif