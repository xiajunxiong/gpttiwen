#ifndef __BATTLE_HALLOW_GIFT_DEF_HPP__
#define __BATTLE_HALLOW_GIFT_DEF_HPP__

enum HALLOW_GIFT_EFFECT_TYPE
{
	HALLOW_GIFT_EFFECT_TYPE_INVALID = 0,
	HALLOW_GIFT_EFFECT_TYPE_ADD_ATTR = 1,	//!< 1.加属性的特效			
	HALLOW_GIFT_EFFECT_TYPE_TIMING = 2,		//!< 2.基于触发时机的特效
	HALLOW_GIFT_EFFECT_TYPE_OTHER = 3,		//!< 3.其他的特效
	HALLOW_GIFT_EFFECT_TYPE_MAX,
};

enum HALLOW_GIFT_TIMING
{
	HG_TIMING_INVALID = 0,
	HG_TIMING_ROUND_START = 1,				//!< 1 回合开始时
	HG_TIMING_BEING_KNOCK_OFF = 2,			//!< 2 圣器被落宝时
	HG_TIMING_HALLOW_PERFORM_SKILL = 3,		//!< 3 圣器发动技能时 
	HG_TIMING_MASTER_SUFFER_INJURE = 4,		//!< 4 主人受到伤害时
	HG_TIMING_NUM,
};

enum HALLOW_GIFT_TIMING_EFFECT
{
	HG_TIMING_EFFECT_INVALID = 0,
	HG_TIMING_EFFECT_MASTER_ADD_HP = 1,		//!< 1 主人回血  p1:回血百分比
	HG_TIMING_EFFECT_MASTER_ADD_MP = 2,		//!< 2 主人回蓝  p1:回蓝百分比
	HG_TIMING_EFFECT_MASTER_ADD_HP_MP = 3,	//!< 3 主人回血且回蓝  p1:回血百分比  p2:回蓝百分比
	HT_TIMING_EFFECT_ADD_NEXT_ROUND_ATTR_ACTIVE = 4, //!< 4 下回合圣器活跃度提高 p1:百分比
	HT_TIMING_EFFECT_ADD_CUR_ROUND_ATTR_ACTIVE = 5,  //!< 5 本回合圣器活跃度提高 p1:百分比  p2:主人血量百分比需低于该值 [0,100]
	HT_TIMING_EFFECT_REALIVE_ONE_ALLY = 6,	//!< 6 复活一个友方单位 p1:回血量系数   回血量=p1*圣器等级
	HG_TIMING_EFFECT_NUM,
};

enum HALLOW_GIFT_OTHER_EFFECT
{
	HG_OTHER_EFFECT_INVALID = 0,
	HG_OTHER_EFFECT_CAN_PERFORM_SKILL_IF_MASTER_DEAD = 1, //!< 1 主人死后仍可释放技能 p1 几率百分比[0,100]
	HG_OTHER_EFFECT_NUM,
};

enum HALLOW_GIFT_ADD_ATTR_EFFECT
{
	HG_ADD_ATTR_INVALID = 0,
	HG_ADD_ATTR_PERCENT = 1,					//!< 1 百分比加属性   p1 圣器属性类型  p2 圣器加属性百分比
	HG_ADD_ATTR_BASE_ON_MASTER_ADD_POINT = 2,	//!< 2 根据主人加点属性而加属性  p1 主人加点属性类型  p2 加点属性提高点数 p3 圣器属性类型  p4 圣器属性增加千分比
	HG_ADD_ATTR_VALUE = 3,						//!< 3 增加属性点数   p1 圣器属性类型  p2 圣器加属性点数
	HG_ADD_ATTR_NUM
};

struct HallowTimingParam_RoundStart
{};

struct HallowTimingParam_HallowPerformSkill
{
	int hallow_id;
	int hallow_skill_id;
	int hallow_skill_lv;
};

struct HallowTimingParam_BeingKnockOff
{};

struct HallowTimingParam_MasterSufferInjure
{
	short deliver_grid_idx;
	int injure;
};

union HallowTimingParam
{
	HallowTimingParam_RoundStart round_start;
	HallowTimingParam_HallowPerformSkill hallow_perform_skill;
	HallowTimingParam_BeingKnockOff being_knock_off;
	HallowTimingParam_MasterSufferInjure master_suffer_injure;
};

#endif