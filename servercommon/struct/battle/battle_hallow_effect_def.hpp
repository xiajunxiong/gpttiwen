#ifndef __BATTLE_HALLOW_EFFECT_DEF_HPP__
#define __BATTLE_HALLOW_EFFECT_DEF_HPP__

enum HALLOW_EFFECT
{
	HALLOW_EFFECT_INVALID = 0,
	HALLOW_EFFECT_REAL_DAMAGE = 1,		//1 对目标造成圣器等级*X的真实伤害  param1:X 
	HALLOW_EFFECT_REAL_DAMAGE_FLOAT = 2, //2 对目标造成圣器等级*X的真实伤害（伤害浮动在Y%~Z%之间浮动)  param1:X  param2:Y  param3:Z
	HALLOW_EFFECT_REAL_DAMAGE_MP_CONSUMED = 3, //3 对目标造成圣器等级*{p1}*已消耗法力比例的真实伤害 法力比例取值范围[0.0,1.0]
	HALLOW_EFFECT_REAL_DAMAGE_MP_REMAIN = 4, //4 对目标造成圣器等级*{p1}*剩余法力比例的真实伤害 法力比例取值范围[0.0,1.0]
	HALLOW_EFFECT_REAL_DAMAGE_TWICE_1 = 5, //5 对目标造成圣器等级*{p1}的真实伤害，伤害后若目标血量【大于】{p2}%, 则造成{p3}%的二次伤害
	HALLOW_EFFECT_REAL_DAMAGE_TWICE_2 = 6, //6 对目标造成圣器等级*{p1}的真实伤害，伤害后若目标血量【小于】{p2}%, 则造成{p3}%的二次伤害
	HALLOW_EFFECT_BURN_MAX_MP = 7,	//7 减少目标最大蓝量的{p1}%蓝量	
	HALLOW_EFFECT_ADD_HP = 8,	//8 回复目标圣器等级*{p1}的血量
	HALLOW_EFFECT_ADD_HP_TWICE = 9, //9 回复目标圣器等级*{p1}的血量 其后若目标血量【小于】{p2}%,则再次回复初次回复血量的{p3}%
	HALLOW_EFFECT_CLEAR_NEGATIVE_STATE = 10, //10 {p1}%概率清除所有负面状态(禁锢状态和异常状态)
	HALLOW_EFFECT_ADD_BUFF = 11,	//11 给目标加Buff  p1:buff_id  p2:buff_lv  p3:概率百分比
	HALLOW_EFFECT_KNOCKOFF_HALLOW = 12, //12 击落目标法宝
	HALLOW_EFFECT_REAL_DAMAGE_EQUALLY_SHARED = 13, //13 造成圣器等级*{p1}的固定伤害，平均分配给所有目标
	HALLOW_EFFECT_PET_RUN_AWAY = 14,	//14 使宠物有{p1}%几率逃跑 [0,100]
	HALLOW_EFFECT_REALIVE = 15,		//15 复活  气血增加圣器等级*{p1}
	HALLOW_EFFECT_ADD_MP = 16,		//16 回复目标{p1}%蓝量 如果目标当前蓝量低于{p2}%,则改为回复{p3}%蓝量
	HALLOW_EFFECT_ADD_SHIELD_BUFF = 17,	//17 获得生命护盾(buff类型63)  p1:buff_id   p2:护盾值系数  p3:破盾伤害系数  p4:buff等级（护盾值=p2*圣器等级  伤害值=p3*圣器等级）
	HALLOW_EFFECT_ADD_DAMAGE_AND_HEAL_BUFF = 18, //18 获得持续伤害并给敌人回血BUFF(buff类型67)  p1:buff_id   p2:伤害值系数  p3:buff等级 （持续伤害值=p2*圣器等级)
	HALLOW_EFFECT_ADD_MAXHP_PERCENT = 19,	//19 加最大血量百分比的血  p1:回血百分比[0,100]
	HALLOW_EFFECT_MAX
};

#endif