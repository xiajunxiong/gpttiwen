#ifndef __BATTLE_HALO_DEF_HPP__
#define __BATTLE_HALO_DEF_HPP__

enum HALO_RANGE_PRIORITY				//!< 注意这个枚举同时用于排序，新增范围类型需要注意优先级
{
	HALO_RANGE_PRIORITY_SELF = 0,				//!< 自身
	HALO_RANGE_PRIORITY_VERTICAL = 1,			//!< 纵排
	HALO_RANGE_PRIORITY_STRONG_POINT = 2,		//!< 强力点
	HALO_RANGE_PRIORITY_HORIZONTAL = 3,			//!< 横排
	HALO_RANGE_PRIORITY_ALLY = 4,				//!< 队友(自身不覆盖）
	HALO_RANGE_PRIORITY_SIDE = 5,				//!< 本方全体

	HALO_RANGE_PRIORITY_NUM,
};

enum HALO_TARGET_SIDE
{
	HALO_TARGET_SIDE_ALLY = 0,
	HALO_TARGET_SIDE_ENEMY = 1,
};

struct HaloEffectParam
{
	HaloEffectParam() : param1(0), param2(0), param3(0), param4(0) { }

	int param1;
	int param2;
	int param3;
	int param4;
};

enum HALO_EFFECT_TYPE
{
	HALO_EFFECT_TYPE_INVALID = 0,
	HALO_EFFECT_TYPE_ADD_PHYSIC_INJURE_REDUCE_RATIO = 1,	//!< 增加物理伤害减免X%  P1：减免千分比
	HALO_EFFECT_TYPE_ADD_HEAL_EFFECT_RATIO = 2,				//!< 增加治疗效果X%      P1：增加千分比
	HALO_EFFECT_TYPE_ADD_DAMAGE_RATIO = 3,					//!< 增加伤害X%			P1：增加千分比	P2:低于X%血量生效(千分比)(0则无视参数2)
	HALO_EFFECT_TYPE_ADD_ANTI_CHAOS_RATIO = 4,				//!< 增加混乱抗性X%      P1：增加千分比
	HALO_EFFECT_TYPE_LOW_SPEED_RATIO = 5,					//!< 降低速度X%			P1：降低千分比
	HALO_EFFECT_TYPE_ADD_DAMAGE_RATIO_BY_TYPE = 6,			//!< 增加伤害X%			P1：增加千分比	P2:\see BATTLE_DAMAGE_EFFECT_TYPE_MAX
	HALO_EFFECT_TYPE_ADD_CONTROL_HIT = 7,					//!< 增加控制命中X%		P1：增加千分比

	HALO_EFFECT_TYPE_MAX
};

#endif