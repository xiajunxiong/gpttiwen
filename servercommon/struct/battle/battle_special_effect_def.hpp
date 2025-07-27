#ifndef __BATTLE_SPECIAL_EFFECT_DEF_HPP__
#define __BATTLE_SPECIAL_EFFECT_DEF_HPP__

////////////////////////////////////////// 特效 /////////////////////////////////////////////
static const int BATTLE_SPECIAL_EFFECT_TYPE_NUM = 1000; // 对应特效表.xlsx 特效列表sheet的sp_type字段


enum SPECIAL_EFFECT_DAMAGE_CONTROL
{
	SPE_DAMAGE_INVALID = 0,
	SPE_DAMAGE_ELEMENT_MAGIC_UP = 1,			//!< 1. 四元素法术伤害up	p1 技能类型(单体，强力，超强等)  p2 提高百分比
	SPE_DAMAGE_SKILL_UP_1 = 2,					//!< 2. 技能伤害up(按技能id)    p1 技能id(0:不限制)  p2 提高千分比 P3:是否需要处于异常状态(0:否 1:是)(禁锢+异常)
	SPE_DAMAGE_SKILL_UP_2 = 3,					//!< 3. 技能伤害up(按技能类型)  p1 技能类型 p2 提高千分比
	SPE_DAMAGE_PHYSICAL_SKILL_IGNORE_DEF = 4,	//!< 4. 物理技能忽略物理防御  p1 忽略百分比 P2:技能ID(0:则表示所有技能都行)
	SPE_DAMAGE_MAGIC_SKILL_IGNORE_DEF = 5,		//!< 5. 法术技能忽略法术防御  p1 忽略百分比 P2:0(魔法) 1(法术(精神)) 2(回复)
	SPE_DAMAGE_RACE_UP = 6,						//!< 6. 对种族伤害提升  p1 种族   p2 提升百分比
	SPE_DAMAGE_RACE_DOWN = 7,					//!< 7. 受到种族伤害减少 p1 种族  p2 减少百分比
	SPE_DAMAGE_ELEMENT_MAGIC_DOWN = 8,			//!< 8. 受到元素法术伤害减少 p1 元素  p2 减少百分比
	SPE_DAMAGE_DOWN = 9,						//!< 9. 最终伤害减免  p1 伤害类型 \see BATTLE_INJURE_TYPE  p2 伤害减免百分比
	SPE_DAMAGE_COUNTER_ATTACK_UP = 10,			//!< 10.反击伤害提升 p1 反击类型(0:自然反击 1:技能反击 2:包括0和1） p2 提高百分比 
	SPE_DAMAGE_MAGIC_FLOAT = 11,				//!< 11.法术最终伤害浮动 p1 浮动百分比低位  p2 浮动百分比高位
	SPE_DAMAGE_FIRE_MAGIC = 12,					//!< 12.火元素法术伤害UP  p1:提高百分比
	SPE_DAMAGE_WATER_MAGIC = 13,				//!< 13.水元素法术伤害UP  p1:提高百分比
	SPE_DAMAGE_WIND_MAGIC = 14,					//!< 14.风元素法术伤害UP  p1:提高百分比
	SPE_DAMAGE_EARTH_MAGIC = 15,				//!< 15.土元素法术伤害UP  p1:提高百分比
	SPE_DAMAGE_SKILL_ATTACH_ELEMENT = 16,		//!< 16.技能附带元素属性  p1:附带元素[1:水 2:火 3:风 4:土 5:混沌]  p2:提高百分比
	SPE_DAMAGE_IMMUNE = 17,						//!< 17.免疫伤害 p1 免疫伤害类型  \see BATTLE_INJURE_TYPE
	SPE_DAMAGE_DOWN_PHYSIC = 18,				//!< 18.物理伤害减免 p1:伤害减免百分比
	SPE_DAMAGE_DOWN_MAGIC = 19,					//!< 19.法术伤害减免 p1:伤害减免百分比
	SPE_DAMAGE_DOWN_IN_PATRIFY_STATE = 20,		//!< 20.石化状态下伤害减免 p1 伤害类型 \see BATTLE_INJURE_TYPE  p2 伤害减免百分比
	SPE_DAMAGE_SKILL_CRITICAL_INC = 21,			//!< 21.技能暴击时,X%概率增伤 p1:提高百分比 p2:技能ID (0:不限制) P3:X(百分比, 0:表示触发)
	SPE_DAMAGE_DOWN_BY_LEVEL = 22,				//!< 22.最终伤害按等级减少 p1:X    最终伤害减少等级*X的数值（群伤效果只有33%)
	SPE_DAMAGE_UP_BY_LEVEL = 23,				//!< 23.最终伤害按等级增加 p1:X    最终伤害增加等级*X的数值（群伤效果只有33%)
	SPE_DAMAGE_UP_BY_ENEMY_HP_PERCENT = 24,		//!< 24.敌方生命低于X%时，伤害增加Y%  p1:X  p2:Y
	SPE_DAMAGE_UP_TO_REAL_MINE_MONSTER = 25,	//!< 25.提升对野外暗怪伤害加成 p1:加成比例（十万分比，法宝专用）
	SPE_DAMAGE_DOWN_FROM_MONSTER = 26,			//!< 26.受到怪物伤害减免 p1:减免比例（十万分比，法宝专用）
	SPE_DAMAGE_UP_FIXED = 27,					//!< 27.物理与魔法伤害伤害结果提升固定值 P1:伤害值  
	SPE_DAMAGE_PHYSICAL_UP_FIXED = 28,			//!< 28.物理伤害结果提升固定值 P1:伤害值
	SPE_DAMAGE_MAGIC_UP_FIXED = 29,				//!< 29.魔法伤害结果提升固定值 P1:伤害值
	SPE_DAMAGE_PHYSICAL_IGNORE_DEF_IFXED = 30,	//!< 30.无视目标防御固定值 P1:无视防御值
	SPE_DAMAGE_INJURE_DOWN_FIXED = 31,			//!< 31.伤害结果减少固定值 P1:减少伤害结果(减少后最少-1) 
	SPE_DAMAGE_LOWER_HP_HIGHER = 32,			//!< 32.血量低于X%时,伤害增加Y% p1:X P2:Y P3:(0:物理伤害 1：魔法伤害 2：物理与魔法伤害)
	SPE_DAMAGE_UP_BY_TARGET_BUFF = 33,			//!< 33.对目标造成伤害时,有X%触发造成(P1)伤害,对带有增益BUFF目标概率提升Y% P1:造成伤害百分比 P2:(0:物理伤害 1：魔法伤害 2：物理与魔法伤害)  P3:X P4:Y (最高概率X+Y)
	SPE_DAMAGE_UP_BY_TAGET_SPC_STATUS = 34,		//!< 34.对处于控制状态的目标提升X%伤害  P1:提高百分比(X) P2:(0:物理伤害 1：魔法伤害 2：物理与魔法伤害)
	SPE_DAMAGE_UP_BY_RATE = 35,					//!< 35.几率提高伤害 P1:几率百分比  P2:伤害提高百分比
	SPE_DAMAGE_DOWN_BY_HP_DROP_1 = 36,			//!< 36.血量每降低X%, 伤害就降低Y%   P1:X (0, 1000)  P2:Y (0, 1000)
	SPE_DAMAGE_UP_PERCENT = 37,					//!< 37.最终伤害按比例上升 p1:上升千分比 P2:自身是否需要处于异常状态(0:否 1:是)(禁锢+异常)
	SPE_DAMAGE_INC_FINALY_DAMAGE_ADD_BUFF = 38,	//!< 38.X%概率提高最终伤害Y%,并给目标添加BUFF P1:X(千分比) P2:Y(千分比) P3:buff_id P4:buff_level
	SPE_DAMAGE_UP_AGAINST_STEALTH = 39,			//!< 39.对隐身目标伤害提高X%  p1:提高百分比
	SPE_DAMAGE_SKILL_UP_BY_RATE = 40,			//!< 40.技能伤害有几率up(按技能id)    p1 技能id  p2 提高千分比  p3 概率千分比
	SPE_DAMAGE_UP_ON_HIGH_HP_PERCENT = 41,		//!< 41.血量高于X%时，伤害增加Y%  p1:X(百分比) p2:Y(百分比)
	SPE_DAMAGE_UP_ON_STEALTH_OR_ACTION_UNDER_STEALTH = 42, //!< 42.处于隐身状态（被动）或行动开始时处于隐身状态（主动）伤害提升千分比 P1: 伤害提升千分比 
	SPE_DAMAGE_UP_BY_JOB = 43,					//!< 43.根据职业增加伤害  p1:职业ID  p2:增伤千分比   职业ID取值[拳师100 剑客200 斧头300 铁卫400 弓手500 法师600 医师700 巫师800]
	SPE_DAMAGE_INC_BY_HP_DROP_PERCENT = 44,		//!< 44.血量每降低X%，提升自身伤害Y%  p1:X(千分比） p2:Y(千分比）
	SPE_DAMAGE_DOWN_BY_JOB = 45,				//!< 45.根据职业降低伤害 P1:职业ID P2:减伤千分比 职业ID取值[拳师100 剑客200 斧头300 铁卫400 弓手500 法师600 医师700 巫师800]
	SPE_DAMAGE_UP_BY_BOTH_SIDES_HP = 46,		//!< 46.对方当前生命高于自身,则伤害提升X% P1:X(千分比) P2:几回合内(0:不受回合限制)  仅PVP
	SPE_DAMAGE_INC_UP = 47,						//!< 47.%X概率提高对敌方伤害 P1:(BATTLE_DAMAGE_EFFECT_TYPE_MAX) P2:增伤千分比 P3:X(百分比,0:表示触发)
	SPE_DAMAGE_INC_UP_BY_SKILL_EFFECT_TYPE = 48,//!< 48.提升65技能效果类型参数3系数 P1:提升千分比
	SPE_DAMAGE_INC_UP_BY_TARGET_NUM = 49,		//!< 49.技能分摊单位越多,伤害提高越多 P1:X(千分比) P2:提高伤害上限Y%(千分比) P3:BATTLE_DAMAGE_EFFECT_TYPE_MAX 公式:X*分担单位数量
	SPE_DAMAGE_INC_UP_BY_BOSS = 50,				//!< 50.提高对BOSS单位的伤害X% P1:X(千分比) (仅PVE)
	SPE_DAMAGE_REAL_DAMAGE_UP_BY_TYPE = 51,		//!< 51.造成某种类型伤害时,提高真实伤害 P1.BATTLE_DAMAGE_EFFECT_TYPE_MAX P2:提高伤害值 P3:群伤提高的伤害值
	SPE_DAMAGE_CENTER_TARGET_UP = 52,			//!< 52.中心目标单位伤害提高 P1:X(千分比)
	SPE_DAMAGE_INC_UP_BY_ATTR_TYPE = 53,		//!< 53.属性比目标高/低提高X%伤害 P1:X(千分比) P2:(0:低 1:高) P3:属性类型(\see BATTLE_ATTR_MAX)
	SPE_DAMAGE_SPECIAL_REAL_DAMAGE_UP = 54,		//!< 54.提高真实伤害 P1:X(千分比) P2:Y(千分比) P3:Z(千分比) 公式:攻击*X + 回复*Y + 精神*Z
	SPE_DAMAGE_DOWN_SKILL_CRITICAL = 55,		//!< 55.受到暴击时,X%概率减伤 p1:减伤(千分比)  P2:X(千分比, 0:表示触发)
	SPE_DAMAGE_REAL_DAMAGE_UP_BY_LEVEL = 56,	//!< 56.提高真实伤害 P1:X(千分比) 公式:等级*X
	SPE_DAMAGE_REAL_DAMAGE_DOWN_BY_LEVEL = 57,	//!< 57.减少真实伤害 P1:X(千分比) 公式:等级*X
	SPE_DAMAGE_INC_UP_AND_DEC_UP = 58,			//!< 58.提高X伤害,增加Y伤害减免 P1:X(千分比) P2:Y(千分比) P3:单位类型(\see BATTLE_CHARACTER_TYPE_MAX)  提高(增加)公式:我方场上该单位类型存活数量*X(Y)
	SPE_DAMAGE_REAL_DAMAGE_UP_HUANSHOU_LEVEL = 59,	//!< 59.造成某种类型伤害时,提高真实伤害 P1.BATTLE_DAMAGE_EFFECT_TYPE_MAX P2:X(千分比) 提高公式:幻兽等级*X
	SPE_DAMAGE_SKILL_IGNORE_DEF_HUANSHOU_LEVEL = 60,//!< 60.技能忽视物防/法防  P1:X(千分比) P2:技能ID(0:则表示所有技能都行) P3.BATTLE_DAMAGE_EFFECT_TYPE_MAX 忽视公式:幻兽等级*X
	SPE_DAMAGE_INC_UP_WITH_WEAR_ELEMENT = 61,	//!< 61.佩戴指定元素水晶时,伤害提高X P1:\see ELEMENT_TYPE_MAX P2:X(千分比)
	SPE_DAMAGE_INC_UP_BY_TARGET_ROLE = 62,		//!< 62.提高对角色目标的伤害 P1:X(万分比)  提高公式:X * 双方幻兽等级差  (仅限PVP)
	SPE_DAMAGE_DOWN_BY_TARGET_ROLE = 63,		//!< 63.降低目标对自己造成的伤害 P1:X(万分比) 降低公式:X * 双方幻兽等级差  (仅限PVP)

	SPE_DAMAGE_MAX,

};

enum SPECIAL_EFFECT_ATTR_CHANGE
{
	SPE_ATTR_INVALID = 0,
	SPE_ATTR_ADD_ATTR = 1,						//!< 1. 加属性      p1 属性类型  p2 属性变化值
	SPE_ATTR_ADD_SKILL_LEVEL = 2,				//!< 2. 加技能等级  p1 技能id  p2 技能等级变化值
	SPE_ATTR_ADD_ATTR_BY_LEVEL = 3,				//!< 3. 按人物等级加属性  p1 属性类型  p2 等级系数(百分比）P3: 增加的属性值  (P2 * level + P3)
	SPE_ATTR_ADD_ATTR_BY_PET_LEVEL = 4,			//!< 4. 按宠物等级加属性  p1 属性类型  p2 等级系数(百分比）P3: 增加的属性值  (P2 * level + P3)
	SPE_ATTR_ADD_ATTR_BY_ANOTHER_ATTR = 5,		//!< 5. 某属性值增加另一属性值  p1 被加属性类型  p2 参照属性类型  p3 系数(百分比）     p1属性加的值=p2属性的值/p3系数 
	SPE_ATTR_ADD_ATTR_BY_PARTNER_LEVEL = 6,		//!< 6. 按伙伴等级加属性 p1 属性类型  p2 等级系数(百分比）P3: 增加的属性值  (P2 * level + P3)
	SPE_ATTR_ADD_ATTR_BY_HUANSHOU_LEVEL = 7,	//!< 7. 按幻兽等级加属性 p1 属性类型  p2 等级系数(百分比）P3: 增加的属性值  (P2 * level + P3)

	SPE_ATTR_MAX,
};

enum SPECIAL_EFFECT_TYPE
{
	SPECIAL_EFFECT_TYPE_INVALID = 0,
	SPECIAL_EFFECT_TYPE_TIMING = 1,				//!< 1.基于触发时机的特效
	SPECIAL_EFFECT_TYPE_DAMAGE_CONTROL = 2,		//!< 2.控制伤害的特效
	SPECIAL_EFFECT_TYPE_ADD_ATTR = 3,			//!< 3.加属性的特效
	SPECIAL_EFFECT_TYPE_SKILL_HIT = 4,			//!< 4.技能命中的特效(带条件)
	SPECIAL_EFFECT_TYPE_OTHER = 5,				//!< 5.其他的特效
	SPECIAL_EFFECT_TYPE_HALO = 6,				//!< 6.光环特效
	SPECIAL_EFFECT_TYPE_MAX,
};

enum SPECIAL_OTHER_EFFECT
{
	SPE_OTHER_INVALID = 0,
	SPE_OTHER_AVOID_BEING_ABSORB_HP = 1,	//!< 1.防止被吸血
	SPE_OTHER_ADD_HP_UP = 2,				//!< 2.受到治疗效果增加 p1 增加百分比
	SPE_OTHER_IMMUNE_BOUNCE_DAMAGE = 3,		//!< 3.免疫反弹伤害 p1 伤害类型(0:物理 1:魔法)
	SPE_OTHER_POISON_PROB_UP = 4,			//!< 4.中毒概率提高 p1 增加千分比
	SPE_OTHER_PETRIFY_PROB_UP = 5,			//!< 5.石化概率提高 p1 增加千分比
	SPE_OTHER_CHAOS_PROB_UP = 6,			//!< 6.混乱概率提高 p1 增加千分比
	SPE_OTHER_SLEEP_PROB_UP = 7,			//!< 7.昏睡概率提高 p1 增加千分比
	SPE_OTHER_DRUNK_PROB_UP = 8,			//!< 8.酒醉概率提高 p1 增加千分比
	SPE_OTHER_GUARD_DEFENCE_UP = 9,			//!< 9.护卫防御加成 p1 增加百分比
	SPE_OTHER_SHIELD_DAMAGE_DOWN = 10,		//!< 10.圣盾受伤减少 p1 减少百分比
	SPE_OTHER_PERFORM_UPGRADED_SKILL = 11,	//!< 11.释放增强版技能 p1:概率百分比  p2:替换前技能ID  p3:替换后技能ID 
	SPE_OTHER_SKILL_CONSUME_LESS_MP = 12,	//!< 12.释放技能蓝耗降低 p1:降低百分比
	SPE_OTHER_LOWER_HP_HIGHER_DEF = 13,		//!< 13.血越少防越高（只在计算伤害时生效） （等待公式中）
	SPE_OTHER_LOWER_HP_HIGHER_MAGIC_DEF = 14,//!< 14.血越少魔防越高（只在计算伤害时生效）（等待公式中）
	SPE_OTHER_LOWER_HP_HIGHER_DAMAGE_DEC_PHYSIC = 15,	//!< 15.血越少物理减伤越高 p1 减伤系数百分比  公式：减伤百分比 =（1-血量百分比）*减伤系数
	SPE_OTHER_LOWER_HP_HIGHER_DAMAGE_DEC_MAGIC = 16,	//!< 16.血越少魔法减伤越高 p1 减伤系数百分比  公式：减伤百分比 =（1-血量百分比）*减伤系数
	SPE_OTHER_IMMUNE_PATRIFY = 17,			//!< 17.免疫石化
	SPE_OTHER_IMMUNE_CHAOS = 18,			//!< 18.免疫混乱
	SPE_OTHER_USE_ITEM_ADD_HP_UP = 19,		//!< 19.吃红药回复HP效果增加  p1 增加百分比
	SPE_OTHER_USE_ITEM_ADD_MP_UP = 20,		//!< 20.吃蓝药回复MP效果增加  p1 增加百分比
	SPE_OTHER_CATCH_BABY_PROB_UP = 21,		//!< 21.抓宝宝几率增加 p1:增加几率千分比
	SPE_OTHER_SKILL_HEAL_UP = 22,			//!< 22.技能治疗效果提升 p1:提升治疗百分比  p2:指定技能ID（为0则不指定技能）
	SPE_OTHER_SKILL_HEAL_CRITICAL = 23,		//!< 23.技能治疗可触发暴击 p1:提升治疗百分比 p2:暴击几率千分比
	SPE_OTHER_FEAR_PROB_UP = 24,			//!< 24.怯战概率提高 p1 增加千分比
	SPE_OTHER_ADD_SKILL_SUCC_RATE = 25,		//!< 25.增加技能成功率 p1 增加百分比  p2 技能ID
	SPE_OTHER_ANTI_ABNORMAL_IMPRISON = 26,	//!< 26.增加异常、禁锢状态抗性千分比  p1 增加千分比
	SPE_OTHER_SKILL_SPLASH_FACTOR_UP = 27,	//!< 27.技能溅射伤害系数增加 p1 增加千分比
	SPE_OTHER_SACRIFICE_FACTOR_DOWN = 28,	//!< 28.技能牺牲血量系数减低 p1 减少千分比
	SPE_OTHER_COMBO_SAME_TARGET_NOT_DEC = 29, //!< 29.连击技能打击相同目标衰减比例降低千分比 p1：技能ID p2：降低衰减比例千分比 
	SPE_OTHER_COMBO_HIT_TIMES_INC = 30,		//!< 30.连击技能打击次数增加 p1 技能ID p2 增加次数 p3:概率（千分比）(为0则必定触发）
	SPE_OTHER_SKILL_ADD_MAX_TARGET_NUM = 31,//!< 31.技能最大目标数增加 p1 技能ID  p2 增加次数  p3:概率（千分比）(为0则必定触发）
	SPE_OTHER_SKILL_NO_COST_RAGE_POINT = 32,//!< 32.释放技能有几率不消耗怒气 p1 概率百分比
	SPE_OTHER_FIX_DAMAGE_TO_OTHER = 33,		//!< 33.对敌人固定伤害 p1 固定伤害值
	SPE_OTHER_FIX_INJURE_FROM_OTHER = 34,	//!< 34.承受敌人的伤害为固定值 p1 固定伤害值
	SPE_OTHER_SKILL_NO_RESTRAIN_BY_ELEMENT = 35, //!< 35.技能不受元素水晶克制 p1 技能ID（为0则不限技能）
	SPE_OTHER_MAGIC_CRITICAL_OBSOLETE = 36,			//!< 36.释放法术技能时，有概率暴击 p1 暴击概率百分比 p2 法术暴击系数百分比 P3:0(魔法) 1(法术(精神)) 2(回复) --废弃
	SPE_OTHER_MAGIC_CRITICAL_SINGLE_SKILL_OBSOLETE = 37, //!< 37. 单体法术有概率暴击 p1 暴击概率百分比 p2 法术暴击系数百分比   -- 废弃
	SPE_OTHER_SUSTAIN_HEAL_UP = 38,			//!< 38.提升持续回血BUFF的回血效果 p1 提升百分比  p2 来自技能ID（指定来自哪个技能，填0则不指定）
	SPE_OTHER_SEVERE_INJURE_PROB_UP = 39,	//!< 39.重伤概率提高 p1 增加千分比
	SPE_OTHER_IMMUNE_TAUNT = 40,			//!< 40.免疫嘲讽
	SPE_OTHER_USE_OTHER_SKILL_EFFECT = 41,	//!< 41.使用其他技能的技能效果 p1:概率百分比  p2:替换前技能ID  p3:替换后技能ID （就是把技能的效果替换掉，但释放的仍是那个技能） 
	
	SPE_OTHER_ADD_REALIVE_SUCC_RATE = 42,	//!< 42.增加复活的成功率 p1 成功率增加千分比
	SPE_OTHER_ONLY_INJURE_PARTIAL = 43,		//!< 43.承受伤害时有概率只受到部分伤害  p1 概率百分比  p2 伤害百分比
	SPE_OTHER_TRANSFORM_ENEMY_INJURE_TO_HEAL = 44,//!< 44.概率转化敌方伤害为治疗  p1 概率千分比  p2 伤害转化千分比
	SPE_OTHER_STEAL_DEFENSE_ON_DAMAGE_ABSORB_HP = 45, //!< 45.吸血时概率偷取敌方防御 p1 概率百分比  p2 偷取防御百分比  p3 偷取防御BUFF_ID  p4 被偷取防御DEBUFF_ID 
	SPE_OTHER_SKILL_KILL_SOMEONE_EXTRA_NORMAL_ATTACK_RANDOM_TARGET = 46, //!< 46.释放技能击杀后，有几率额外对随机一个敌人发起一次普攻 p1:概率百分比
	SPE_OTHER_INJURE_SKILL_EXTRA_PERFORM = 47,	//!< 47.伤害技能额外释放一次  p1 概率百分比  p2 物理技能填0, 法术技能填1  p3:技能ID（填0就是不限技能）
	SPE_OTHER_TRIGGER_SPID_ADD_HP = 48,		//!< 48.当触发一次某特效时，有几率额外加血   p1:触发特效ID   p2:概率百分比   p3:参数X(百分比)   p4:参数Y    触发特效p1时，有p2的概率额外回血(等级*p3/1000+p4)
	SPE_OTHER_UPGRADE_SKILL_POSOBILITY_UP = 49, //!< 49.提高释放增强版技能的概率 p1 提高百分比 p2 技能ID p3 增强版技能ID 
	SPE_OTHER_ADD_HP_FROM_DAMAGE = 50,		//!< 50.造成伤害加血 p1 伤害转化千分比 P2 伤害类型(BATTLE_DAMAGE_EFFECT_TYPE_MAX)
	SPE_OTHER_BUFF_SUBSTITUTION = 51,		//!< （暂时作废） 51.通过技能加BUFF时，替换BUFF_ID和BUFF_LV  p1:技能ID  p2:被替换的buff_id  p3:替换成新的buff_id  p4:替换成新的buff_lv  
	SPE_OTHER_SPECIAL_REALIVE_BUFF_UPGRADE = 52,//!< 52.特殊复活Buff强化 p1 复活血量增加千分比  
	SPE_OTHER_RESIST_IMPRISON_STATE_BOUNCE = 53,	//!< 53.抵抗控制效果成功,有X%概率反弹给来源目标(目标如果没有免疫该效果则必定命中,无法抵抗) P1: X(百分比)
	SPE_OTHER_PET_FATAL_INJURE_ADD_HP = 54,			//!< 54 宠物受到致命伤害后,X%添加血量上限Y%(触发时不倒下)(此处特效默认人物身上) P1:X(千分比) P2:Y(千分比)
	SPE_OTHER_PROB_CONTROL_IMMUNE = 55,				//!< 55 X%概率免疫控制 P1:X(千分比) P2:第几回合(0则不限制)
	SPE_OTHER_ANTI_IMPRISON_BOUNCE = 56,			//!< 56 提升抵抗禁锢(控制)状态X%,Y%概率回击技能 P1:X(千分比) P2:Y(千分比) (触发回击概率,目标如果没有免疫该效果则必定命中,无法抵抗)
	SPE_OTHER_MP_LOW_LOSS_HP = 57,					//!< 57 敌方单位法力值低于X%，则立刻扣除当前生命值Y% P1:X(千分比) P2:Y(千分比)
	SPE_OTHER_RAGE_POINT_CONSUME_LOW = 58,			//!< 58 降低自身技能怒气值消耗 P1:降低的点数
	SPE_OTHER_ABSORB_HP_RATE_UP = 59,				//!< 59 吸血比例提升 p1:提升千分比
	SPE_OTHER_INSTANT_KILL_BEFORE_SKILL_HIT = 60,	//!< 60 技能可触发秒杀 p1:概率千分比 p2:技能ID  p3:通知客户端播放新的技能ID
	SPE_OTHER_SKILL_ADD_CRITICAL_RATE = 61,			//!< 61 技能提升暴击率 p1:提升千分比 p2:技能ID
	SPE_OTHER_CHANGE_SKILL_ID = 62,					//!< 62 新的替换技能ID  p1:旧技能ID_1  p2:新技能ID_1  p3:旧技能ID_2  p4:新技能ID_2
	SPE_OTHER_COUNTER_ATTACK_RATE_UP = 63,			//!< 63 反击几率提升 p1:提升千分比
	SPE_OTHER_ADD_BUFF_SUCC_ADD_BUFF = 64,			//!< 64 加BUFF成功后继续加BUFF  p1:条件BuffID  p2:追加BuffID  p3:追加Buff等级
	SPE_OTHER_ADD_EXTRA_PERFORM_SKILL_RATE = 65,	//!< 65 给可以额外释放的技能，增加额外释放的概率  p1:技能ID  p2:增加的概率 千分比
	SPE_OTHER_SKILL_DAMAGE_NO_DECLINE_ON_NEXT_TARGET = 66, //!< 66 伤害不递减 p1:技能ID
	SPE_OTHER_SKILL_REAL_DAMAGE_RATE_UP = 67,		//!< 67 提高技能效果19和效果12和效果48的真实伤害比例  p1:根据各自效果的百分比/千分比
	SPE_OTHER_HALO_ADD_DAMAGE_RATIO_UP = 68,		//!< 68 光环特效3.增加伤害X%  P1：增加千分比  P2:低于Y%血量生效(千分比)(0则无视参数2)
	SPE_OTHER_SKILL_IGNORE_DEFENSE_AND_SHIELD = 69,	//!< 69 技能无视防御和圣盾效果  P1:技能ID（0则不限技能）
	SPE_OTHER_DEBUFF_COUNTER = 70,					//!< 70 自身受到异常、控制效果时，使对方也承受这个效果    P1:概率千分比
	SPE_OTHER_SKILL_FACTOR_UP = 71,					//!< 71 增加技能系数（不算普攻）  P1:技能系数增加千分比  P2:技能ID （填0即任意技能）
	SPE_OTHER_SKILL_EXTRA_PERFORM = 72,				//!< 72.技能有概率额外释放一次  p1 概率百分比  p2 物理技能填0, 法术技能填1  p3:技能ID（填0就是不限技能） P4:技能系数折损百分比（注意是乘法，不是减法）
	SPE_OTHER_BURN_MP_DEEP_BLOW_MP_PERCENT_UP = 73,	//!< 73.提高技能效果9深度一击扣的法力值 P1:X百分比(原先+X)
	SPE_OTHER_SILENT_PROB_UP = 74,					//!< 74.沉默概率提高 P1:提高千分比
	SPE_OTHER_ADD_CRITICAL_RATE_ON_LOWER_HP = 75,	//!< 75 血量低于X%时提升暴击率 p1:提升暴击率千分比 p2:血量阈值X（百分比）
	SPE_OTHER_SHIELD_CONEFFICIENT_UP = 76,			//!< 76 提高护盾BUFF类型37,46,48护盾值比例 P1:根据各个效果的系数比例(百分比/千分比)
	SPE_OTHER_MAGIC_CRITICAL_2 = 77,				//!< 77.释放法术技能时，临时增加暴击概率,若目标有增益buff，则概率额外增加 p1 暴击概率千分比  p2 增益BUFF目标暴击概率额外增加千分比 p3:BATTLE_DAMAGE_EFFECT_TYPE_MAX
	SPE_OTHER_ADD_BUFF_TO_TARGET_ON_EXPLODE_EFFECT = 78, //!< 78.自爆特效触发时，给目标加BUFF   P1:buff_id   P2:buff_lv   P3:概率千分比
	SPE_OTHER_SKILL_EXTRA_PERFORM_ON_CRITICAL = 79,	//!< 79.技能释放触发必杀，有概率额外释放一次  p1 概率百分比  p2 物理技能填0, 法术技能填1  p3:技能ID（填0就是不限技能） P4:技能系数折损百分比（注意是乘法，不是减法）
	SPE_OTHER_GUARD_BY_JIEYI_MEMBER = 80,			//!< 80.结义护卫  p1:护卫者血量百分比阈值（超过血量百分比才会护卫）
	SPE_OTHER_ADD_HP_UP_BY_JIEYI_MEMBER = 81,		//!< 81.受到来自结义成员的治疗效果提升 p1:增加百分比
	SPE_OTHER_EXTRA_PERFORM_ANOTHER_SKILL = 82,		//!< 82.成功释放技能P1后，有概率P3释放技能P2   P1:原技能ID    P2:额外释放的技能ID    P3:概率千分比
	SPE_OTHER_SKILL_DAMAGE_REDUCE_DECLINE_ON_NEXT_TARGET = 83, //!< 83 伤害递减系数降低 p1:技能ID  p2:伤害递减系数降低千分比
	SPE_OTHER_ENEMY_DEAD_RECOVER_HP_DROP_PERCENT = 84,	//!< 84 敌方死亡时恢复自身X%的已损失生命值  p1:X （千分比）
	SPE_OTHER_SKILL_EXTRA_PERFORM_ON_KILL = 85,		//!< 85.技能释放击杀后，有概率额外释放一次  p1 概率百分比  p2 物理技能填0, 法术技能填1  p3:技能ID（填0就是不限技能） P4:技能系数折损百分比（注意是乘法，不是减法）
	SPE_OTHER_HEAL_HP_OVERFLOW = 86,	//!< 86.受到治疗并溢出生命上限添加可叠加生命护盾BUFF P1:X (百分比) P2:buff_id P3:buff_level P4:几回合内(0:没有回合限制) 护盾值: X * 溢出治疗值 / 百分比
	SPE_OTHER_ADD_BUFF_BY_ONLY_INJURE_PERCENT = 87,	//!< 87.单次受到最大生命X%伤害,添加BUFF P1:X(百分比) P2:buff_id P3:buff_level P4:几回合内(0:没有回合限制)
	SPE_OTHER_ADD_MORE_ATTR_BY_TRIGGER_SPE_44 = 88,	//!< 88.由时机触发特效44加的属性值增加X%， P1:X(千分比)
	SPE_OTHER_TAUNT_UPGRADE_1 = 89,					//!< 89.强化嘲讽（增加命中率和反击系数）  P1:命中率(百分比)  P2:反击系数(百分比）
	SPE_OTHER_WIND_SEED_CONTROL_POSIBILITY_UP = 90,	//!< 90.增加飓风种子的控制概率 p1:增加的概率千分比
	SPE_OTHER_REAL_DAMAGE_SKILL_EFFECT_UP = 91,		//!< 91.真伤技能效果强化（技能效果12）  p1:提升X  p2:提升Y 
	SPE_OTHER_GUARD_BY_QING_YUAN_TARGET = 92,		//!< 92.情缘护卫 气血低于X,则另一方有Y概率来护卫,护卫时添加BUFF P1:X(百分比) P2:Y(百分比) P3:buff_id P4:buff_level
	SPE_OTHER_SACRIFICE_SELF_ADD_QINGYUAN = 93,		//!< 93.牺牲自身当前X%血量与蓝量,给情缘目标加可叠加生命护盾BUFF并避免致命伤害 P1:X(百分比) P2:Y(百分比) P3:buff_id P4:buff_level 护盾值:Y*最大血量(情缘目标)/百分比 前提角色血量和蓝量需要大于50%
	SPE_OTHER_ADD_BUFF_QING_YUAN_BY_SELF_DIE = 94,	//!< 94.自身阵亡,给情缘目标添加BUFF  P1:buff_id P2:buff_level

	SPE_OTHER_ATTR_PHYSICAL_DEC_DAMAGE = 95,		//!< 95.物免属性改变（效果应等同于改属性） P1:改变千分比
	SPE_OTHER_ATTR_MAGIC_DEC_DAMAGE = 96,			//!< 96.法免属性改变（效果应等同于改属性） P1:改变千分比
	SPE_OTHER_ATTR_PHYSICAL_INC_DAMAGE = 97,		//!< 97.物伤属性改变（效果应等同于改属性） P1:改变千分比
	SPE_OTHER_ATTR_MAGIC_INC_DAMAGE = 98,			//!< 98.法伤属性改变（效果应等同于改属性） p1:改变千分比
	SPE_OTHER_ATTR_CRTICAL = 99,					//!< 99.必杀属性改变 (效果应等同于改属性)  p1:改变千分比
	SPE_OTHER_ATTR_COUNTER_ATTACK = 100,			//!< 100.反击率属性改变(效果应等同于改属性) p1:改变千分比
	SPE_OTHER_HURT_ME_HP_LESS_INSTANT_KILL = 101,	//!< 101.目标受到伤害前,血量低于X%,则触发秒杀 P1:X(千分比)    -- (限制条件:无法对队员,PVP)
	SPE_OTHER_ADD_DAMAGE_HP_SHILED_BUFF_HEAL = 102,	//!< 102.添加伤害生命护盾BUFF单位回复X%血Y%蓝(最大) P1:X(千分比) P2:Y(千分比)  (仅限技能效果52触发)
	SPE_OTHER_REMOVE_BUFF = 103,					//!< 103.删除自身BUFF P1:buff_id P2: 0.任何类型技能 1.伤害型技能
	SPE_OTHER_COMBO_SAME_TARGET_EXTRA_DAMAGE = 104,	//!< 104.连击技能打击相同目标造成额外X%最大生命值的真实伤害 p1：技能ID p2：X(百分比) p3: BATTLE_PLAYER_TYPE_MAX
	SPE_OTHER_ADD_BUFF_BY_ME_SIDE_OTHER_GRID_DIE = 105,	//!< 105.己方单位阵亡时,给自身添加BUFF P1:buff_id P2:buff_level P3:概率(百分比)
	SPE_OTHER_DELIVER_CHAR_DIE_OR_ADD_BUFF_BY_DIE = 106,	//!< 106.单位真的死亡,则有X概率让造成该单位的玩家死亡,否则加buff P1:X概率(百分比) P2:buff_id P3:buff_level
	SPE_OTHER_ABSORB_CONVERT_BUFF_EFFECT_UP = 107,	//!< 107.提升吸收转换生命buff类型吸收上限/低于Y%血量吸收 P1:X(百分比) P2:buff_id(0:不限制) P3:Y(百分比)  公式 : 最大生命值 * X /100 
	SPE_OTHER_PERFORM_SKILL_BY_LAST_SKILL_KILL_BC = 108,	//!< 108.上个技能击杀单位,概率会释放一个技能 P1:skill_id P2:skill_level P3:概率(千分比)
	SPE_OTHER_ADD_HP_UP_2 = 109,					//!< 109.受到治疗效果增加  p1 增加千分比分比
	SPE_OTHER_SKILL_HEAL_UP_2 = 110,				//!< 110.技能治疗效果提升 p1:提升治疗千分比  p2:指定技能ID（为0则不指定技能）
	SPE_OTHER_SKILL_SPLASH_DAMAGE_TARGET_ADD_BUFF = 111,	//!< 111.给受到溅射伤害的单位添加BUFF(仅限技能效果18) P1:buff_id P2:buff_level P3:概率(千分比) P4:技能ID(0:不限制)

	SPE_OTHER_ELEMENT_CRYSTAL_DAMAGE_ONLY = 112,	//!< 112 指定灵珠及混沌灵珠的攻击可以秒杀，否则只受1点伤害  P1:水晶类型[1水 2火 3风 4土]
	SPE_OTHER_GAMBLE_STRIKE_SUCC_SPIKE = 113,		//!< 113 暗杀成功概率转为秒杀 P1:X(千分比) P2:(0:对boss无效)

	SPE_OTHER_HALO_STILL_ACTIVE_ON_DIE = 114,		//!< 114.光环死后也不会消失（但击飞会消失） 
	SPE_OTHER_LOCK_HP_AND_BUFF_ON_DEADLY_INJURE = 115,	//!< 115.受到致命伤害时锁1点血,并加Buff   P1 buff_id  P2 buff_lv
	SPE_OTHER_BOUNCE_DAMAGE_ON_DEADLY_INJURE = 116,	//!< 116. 受到致命伤害时有几率反弹全额伤害(不超过自身最大血量) P1 概率百分比
	SPE_OTHER_SKILL_INSTANT_PERFORM = 117,			//!< 117. 技能有几率瞬发 p1:技能ID  p2:概率百分比 
	SPE_OTHER_HEAL_SKILL_EXTRA_PERFORM = 118,		//!< 118.回血技能额外释放一次  p1 概率百分比  p2:技能ID（填0就是不限技能）
	SPE_OTHER_IMMUNE_ALL_BUFF = 119,				//!< 119.免疫正面和负面BUFF
	SPE_OTHER_RUN_AWAY_ON_ROUND_END = 120,			//!< 120.在指定回合结束时逃跑 p1:回合数
	SPE_OTHER_FATAL_INJURE_ADD_HP = 121,			//!< 121 受到致命伤害后,X%添加血量上限Y%(触发时不倒下) P1:X(千分比) P2:Y(千分比)
	SPE_OTHER_WITHSTAND_INSTANT_KILL = 122,			//!< 122 概率抵挡某些秒杀效果 P1:概率(千分比) (目前仅限技能效果7,45,战斗其它特效60,113)
	SPE_OTHER_ADD_HP_UP_WITH_HUANSHOU_LEVEL = 123,	//!< 123 处于控制状态时,增加受到的治疗效果 P1:X(千分比) 增加公式:幻兽等级*X
	SPE_OTHER_ADD_DEF_WITH_HUANSHOU_LEVEL = 124,	//!< 124 处于控制状态时,增加防御/法防 P1:X(千分比) P2:BATTLE_DAMAGE_EFFECT_TYPE_MAX 增加公式:幻兽等级*X
	SPE_OTHER_ADD_BUFF_BY_CONSUME_RAGE_POINTS = 125,//!< 125 消耗元气,添加BUFF P1:buff_id P2:buff_level (消耗的元气数值等同buff层数)

	SPE_OTHER_MAX,
};

enum SPECIAL_EFFECT_TIMING //!< 触发时机
{
	SPE_TIMING_INVALID = 0,
	SPE_TIMING_ON_BATTLE_START = 1,			//!< 1  战斗开始时
	SPE_TIMING_BEFORE_PERFORM_SKILL = 2,	//!< 2  技能释放或攻击前
	SPE_TIMING_BEFORE_SKILL_HIT_OTHER = 3,	//!< 3  技能命中前
	SPE_TIMING_BEFORE_SKILL_HIT_ME = 4,		//!< 4  被技能命中前
	SPE_TIMING_BEFORE_HURT_OTHER = 5,		//!< 5  造成伤害前
	SPE_TIMING_BEFORE_HURT_ME = 6,			//!< 6  受到伤害前 (废弃)
	SPE_TIMING_ON_HP_CHANGE = 7,			//!< 7  HP变化时 (废弃)
	SPE_TIMING_ON_MP_CHANGE = 8,			//!< 8  MP变化时 (废弃)
	SPE_TIMING_ON_DIE = 9,					//!< 9  死亡触发
	SPE_TIMING_AFTER_HURT_OTHER = 10,		//!< 10 造成伤害后
	SPE_TIMING_AFTER_HURT_ME = 11,			//!< 11 受到伤害后
	SPE_TIMING_AFTER_SKILL_HIT_OTHER = 12,	//!< 12 技能命中后
	SPE_TIMING_AFTER_SKILL_HIT_ME = 13,		//!< 13 被技能命中后
	SPE_TIMING_AFTER_PERFORM_SKILL = 14,	//!< 14 技能释放或攻击后 (废弃)
	SPE_TIMING_CRITICAL_HIT_OTHER = 15,		//!< 15 造成暴击后
	SPE_TIMING_CRITICAL_HIT_ME = 16,		//!< 16 受到暴击后
	SPE_TIMING_ON_DODGE = 17,				//!< 17 闪避时
	SPE_TIMING_ON_COUNTER_ATTACK = 18,		//!< 18 反击时
	SPE_TIMING_ON_SACRIFICE_EFFECT = 19,	//!< 19 牺牲效果触发时 (废弃)
	SPE_TIMING_ON_GUARD_OTHER = 20,			//!< 20 护卫其他人时
	SPE_TIMING_ADD_HP_TO_OTHER = 21,		//!< 21 回复其他角色血量时
	SPE_TIMING_ADD_HP_TO_ME = 22,			//!< 22 受到加血时
	SPE_TIMING_ACTION_START = 23,			//!< 23 行动开始时
	SPE_TIMING_ACTION_END = 24,				//!< 24 行动结束时
	SPE_TIMING_DEFENSE_SUCC = 25,			//!< 25 成功防御时
	SPE_TIMING_SUFFER_DEADLY_INJURE = 26,	//!< 26 受到致命伤害前
	SPE_TIMING_ROUND_END = 27,				//!< 27 回合结束时
	SPE_TIMING_TAUNT_SUCC = 28,				//!< 28 嘲讽成功后
	SPE_TIMING_ON_KILL_SOMEONE = 29,		//!< 29 击杀时
	SPE_TIMING_AFTER_SUFFER_DEADLY_INJURE = 30, //!< 30 死亡前触发 （比类型9死亡触发更早一个瞬间）(用于那些需要比复活更早触发的死亡特效）
	SPE_TIMING_ON_SUMMON = 31,				//!< 31 被召唤进入战斗时
	SPE_TIMING_ON_RESIST_DEBUFF = 32,		//!< 32 抵抗触发时
	SPE_TIMING_ON_ROUND_PREPARE = 33,		//!< 33 321准备前
	SPE_TIMING_ON_GAMBLE_STRIKE_FAILED = 34,//!< 34 暗杀失败时
	SPE_TIMING_ON_GAMBLE_STRIKE_SUCC = 35,	//!< 35 暗杀成功时
	SPE_TIMING_ON_PURIFY_OTHER_SUCC = 36,	//!< 36 主动成功解除他人不良状态时（代指使用技能、法宝、药品的净化时，不计算自身特效）
	SPE_TIMING_ON_ACTIVE_SUMMON_PET = 37,	//!< 37 主动召唤宠物进入战斗时
	SPE_TIMING_ON_ACTIVE_STEALTH_BREAK = 38,//!< 38 主动破除隐身时
	SPE_TIMING_ON_GUARD_OTHER_END = 39,		//!< 39 护卫其他人结束后
	SPE_TIMING_ON_ROUND_END_BEFORE_BUFF = 40,//!< 40 回合结束前触发(比buff回合结束早一个瞬间)

	SPE_TIMING_MAX,
};

enum SPE_TRIGGER_EFFECT
{
	SPE_TRIGGER_EFFECT_INVALID = 0,
	SPE_TRIGGER_EFFECT_ADD_HP = 1,			//!< 1. 加血  P1 目标(0:自身  1:目标) P2 加血数值（必须大于0，因为约定了不提供直接扣血的特效） p3 是否千分比加血(0:否 1:是）p4:概率(千分比,0和1000都代表必定成功)
	SPE_TRIGGER_EFFECT_MODIFY_MP = 2,		//!< 2. 改变蓝量  P1 目标(0:自身  1:目标) P2 改变数值（可为负）  P3 是否百分比(0:否 1:是)  
	SPE_TRIGGER_EFFECT_ADD_BUFF = 3,		//!< 3. 加buff  P1 作用对象(0:自身  1:目标  2:自身位置取反) P2 buff_id  P3 buff等级  P4 概率（千分比, 0和1000都代表必定成功）
	SPE_TRIGGER_EFFECT_ABSORB_HP_FROM_DAMAGE = 4,		//!< 4. 造成伤害吸血  P1 吸血千分比   P2 伤害类型(0:物理  1:魔法  2:物理和魔法)
	SPE_TRIGGER_EFFECT_REALIVE = 5,			//!< 5. 复活  P1 目标(0:自身  1:目标)   P2 加血数值（必须大于0，因为约定了不提供直接扣血的特效） p3 是否百分比加血(0:否 1:是） P4 概率（百分比）
	SPE_TRIGGER_EFFECT_ADD_HP_BY_LEVEL = 6, //!< 6. 按自身等级回血  P1 目标(0:自身  1:目标) P2 加血数值基数千分比分比  P3 一个固定值  总加血量等于 P2 / 1000 * level + P3
	SPE_TRIGGER_EFFECT_MODIFY_MP_BY_LEVEL = 7, //!< 7. 按自身等级改变蓝量  P1 目标(0:自身  1:目标) P2 蓝量数值基数百分比   总蓝量改变值等于 P2 / 100 * level
	SPE_TRIGGER_EFFECT_CLEAR_NEGATIVE_STATE = 8, //!< 8. 清除自身负面状态（只清异常和禁锢状态） P1：概率百分比
	SPE_TRIGGER_EFFECT_ADD_HP_BY_ATK = 9,	//!< 9. 按攻击力加血 P1 目标（0:自身  1:目标） P2:加血数值基数百分比  P3 一个固定值   总加血量等于 P2 / 100 * atk + P3
	SPE_TRIGGER_EFFECT_FRIEND_ADD_HP = 10,	//!< 10. 给友军加血  P1 加血数值  P2 是否千分比加血(0:否 1:是）
	SPE_TRIGGER_EFFECT_EXPLODE = 11,					//!< 11. 对敌方所有单位造成真实伤害 P1 伤害系数百分比  伤害=攻击力*P1/100  P2:概率(百分比 0和100都代表必定成功)
	SPE_TRIGGER_EFFECT_REALIVE_ON_ALLY_ALIVE = 12, //!< 12. 当死亡时，如果友方还有其他单位存活，则满血复活
	SPE_TRIGGER_EFFECT_REALIVE_2 = 13,		//!< 13. 复活回血并加BUFF P1 加血百分比  P2 BUFF_ID  P3 BUFF等级  P4 概率（百分比）
	SPE_TRIGGER_EFFECT_ADD_ATK_BY_RECOVER_ATTR = 14, //!< 14. 给自身加一个根据回复值计算的加攻BUFF  P1 BUFF_ID   P2 转化百分比   加攻数值=回复值*转化百分比
	SPE_TRIGGER_EFFECT_FRIEND_ADD_MP = 15,				//!< 15. 给友军加蓝  P1 加蓝数值  P2 是否千分比加蓝(0:否 1:是）
	SPE_TRIGGER_EFFECT_CLEAR_FRIEND_NEGATIVE = 16,		//!< 16. 清除随机若干个友军的控制  P1 概率百分比  P2 随机X个玩家 
	SPE_TRIGGER_EFFECT_PERFORM_FIRST_SKILL = 17,		//!< 17. 几率释放第一个技能  P1 概率百分比
	SPE_TRIGGER_EFFECT_PERFORM_SKILL = 18,				//!< 18. 几率释放一个技能（允许角色没有这个技能，目标随机） P1 概率百分比 P2 技能ID P3 技能等级
	SPE_TRIGGER_EFFECT_REMOVE_BUFF = 19,				//!< 19. 清除一个BUFF  P1 作用对象(0:自身  1:目标  2:自身位置取反) P2 buff_id
	
	SPE_TRIGGER_EFFECT_DEC_HP_ADD_BUFF = 20,			//!< 20. 上个回合受到血量上限X%伤害时,有Y%概率添加BUFF P1 buff_id P2 buff等级 P3 X P4 Y  概率为百分比
	SPE_TRIGGER_EFFECT_NOT_USE_INJURE_SKILL = 21,		//!< 21. 上个回合未施放伤害类技能(不包括普攻),加BUFF P1 buff_id P2 buff等级 
	SPE_TRIGGER_EFFECT_DEBUFF_BY_HIGHEST_ATTR_1 = 22,	//!< 22. 敌方攻击、精神、回复属性最高的角色，加DEBUFF  P1 buff_id P2 buff等级
	SPE_TRIGGER_EFFECT_ADD_SELF_SIDE_BUFF = 23,			//!< 23. 给我方添加side_buff P1:side_buff_id
	SPE_TRIGGER_EFFECT_ADD_BUFF_BY_ATTR = 24,			//!< 24. 根据属性值来给我方角色添加BUFF P1:buff_id P2:buff等级 P3:属性类型 P4:0:最低 1:最高
	SPE_TRIGGER_EFFECT_ADD_TWO_BUFF = 25,				//!< 25. 添加2个BUFF P1.BUFF_1 P2.BUFF_2 P3.BUFF等级 P4.概率(百分比) (目标为自身)
	SPE_TRIGGER_EFFECT_HEART_SKILL_INVAILD = 26,		//!< 26. 心法技能失效 P1:千分比(失效概率) P2:作用对象(0:自身  1:目标) (当前仅限敌方目标)
	SPE_TRIGGER_EFFECT_ADD_BUF_AND_SET_HP = 27,			//!< 27. 给自身添加BUFF并设置血量 P1:buff_id P2:buff_level P3:血量值(<=0则默认设置1滴血)
	SPE_TRIGGER_EFFECT_REALIVE_BEFORE_BUFF_CLEAR = 28,	//!< 28. 在清buff前复活（只能配合时机30使用） P1 目标(0:自身  1:目标)   P2 加血数值（必须大于0，因为约定了不提供直接扣血的特效） p3 是否百分比加血(0:否 1:是） P4 概率（百分比）
	SPE_TRIGGER_EFFECT_CLEAR_TARGET_NEGATIVE_STATE = 29,//!< 29. 净化 P1 作用对象(0:自身  1:目标  2:自身位置取反) P2 概率（千分比, 0和1000都代表必定成功）
	SPE_TRIGGER_EFFECT_LOW_DAMAGE_ADD_BUFF = 30,		//!< 30. 上个回合受到的伤害小于或等于X,则给自身添加BUFF P1:buff_id P2:buff_level P3:X(伤害值)
	SPE_TRIGGER_EFFECT_ADD_SP_BY_HAS_SP = 31,			//!< 31. 判断是否有参数中的特效,没有则随机一个添加 P1:sp_id_1 P2:sp_id_2
	SPE_TRIGGER_EFFECT_CHANGE_SP_BY_OTHER_SP = 32,		//!< 32. 特效循环 P1:sp_id_1 P2:sp_id_2 P3:触发次数(每隔触发次数删除已有添加另一个)
	SPE_TRIGGER_EFFECT_OUR_ADD_BUFF = 33,				//!< 33. 给我方X个单位Y概率添加BUFF P1:buff_id P2:buff_level P3:X P4:Y概率(千分比) (包括自身)
	SPE_TRIGGER_EFFECT_MODIFY_RAGE_POINT = 34,			//!< 34. 修改怒气值 P1:改变的值（可为负） P2:概率千分比
	SPE_TRIGGER_EFFECT_MAXHP_PERCENT_DAMAGE = 35,		//!< 35. 对目标造成其气血上限的X%伤害 P1:最大血量千分比  P2:需要处于异常或禁锢状态吗？(1:是 0:否)
	SPE_TRIGGER_EFFECT_SIDE_ADD_BUFF = 36,				//!< 36. 给一方加BUFF  P1:[0=本方 1=敌方]  P2 buff_id  P3 buff等级  P4 概率（千分比, 0和1000都代表必定成功）
	SPE_TRIGGER_EFFECT_REAL_DAMAGE_TO_ALL_ENEMY = 37,	//!< 37. 对敌方全体造成真伤  P1:系数千分比   伤害=角色等级*系数
	SPE_TRIGGER_EFFECT_ADD_BUFF_TO_ALL_ENEMY = 38,		//!< 38. 对敌方全体加BUFF  P1:Buff_id  P2:Buff等级   P3:概率千分比
	SPE_TRIGGER_EFFECT_ADD_BUFF_TO_RANDOM_ENEMEY = 39,	//!< 39. 对敌方随机一个单位加BUFF  P1:Buff_id  P2:Buff等级   P3:概率千分比
	SPE_TRIGGER_EFFECT_SUMMON_MONSTER = 40,				//!< 40. 召唤怪物  P1:召唤seq
	SPE_TRIGGER_EFFECT_REALIVE_BY_JIEYI_MEMBER = 41,	//!< 41. 死亡时由结义成员进行复活  p1:总牺牲血量百分比   p2:允许参与复活的血量百分比  p3:复活血量百分比

	SPE_TRIGGER_EFFECT_ADD_ATTR_BUFF_BY_LEVEL = 42,		//!< 42. 按目标的等级*系数，给目标加属性BUFF  P1:等级系数千分比  P2:属性类型   P3:buff_id  P4:buff等级 （BUFF必须使用类型29：按数值改变属性的BUFF）
	SPE_TRIGGER_EFFECT_ADJACENT_GRID_DAMAGE_BY_SKILL_DAMAGE = 43, //!< 43. 周围一格的敌方单位受到本次技能伤害的千分比伤害 P1:千分比
	SPE_TRIGGER_EFFECT_ADD_ATTR_BUFF_BY_LAST_ROUND_INJURE = 44, //!< 44. 根据上回合受到伤害值获得属性BUFF  P1:伤害值转化系数千分比 （获得属性值=受伤数值*P1/1000) P2:属性类型   P3:buff_id  P4:buff等级 （BUFF必须使用类型29：按数值改变属性的BUFF）
	SPE_TRIGGER_EFFECT_ADD_BUFF_IF_CUR_HP_PERCENT_LOWER_THAN = 45, //!< 45. 若生命值低于X%时，有Y%概率获得BUFF  P1:X(千分比)  P2:Y(千分比)  P3:BUFF_ID  P4:BUFF等级
	SPE_TRIGGER_EFFECT_ADD_CAN_MERGE_HP_SHIELD_BUFF = 46,	//!< 46.消耗自身最大X%血量添加可叠加生命护盾BUFF P1:X(百分比) P2:Y(百分比) P3:buff_id P4:buff_level 护盾值 : Y * 最大血量 / 百分比
	SPE_TRIGGER_EFFECT_BURN_AND_ABSORB_TARGET_MP = 47,		//!< 47.燃烧目标最大蓝量，并吸收燃烧的部分  P1:燃烧千分比  P2:吸收千分比
	SPE_TRIGGER_EFFECT_ADD_BUFF_CERTAINLY = 48,				//!< 48.添加BUFF(不算概率,除非免疫) P1 作用对象(0:自身  1:目标  2:自身位置取反) P2 buff_id  P3 buff等级

	SPE_TRIGGER_EFFECT_CAN_DIE_GRID_ADD_BUFF = 49,			//!< 49.加buff(可以给死亡单位添加)  P1 作用对象(0:自身  1:目标  2:自身位置取反) P2 buff_id  P3 buff等级  P4 概率（千分比, 0和1000都代表必定成功）
	SPE_TRIGGER_EFFECT_REMOVE_BUFF_QINGYUAN_TARGET = 50,	//!< 50.X概率清除情缘目标身上随机Y个负面状态 P1:X(百分比) P2:Y
	SPE_TRIGGER_EFFECT_ADD_BUFF_AND_ADD_HP = 51,			//!< 51.加BUFF并回复自身血量 P1:X概率触发(百分比) P2:buff_id P3:buff_level P4:Y(百分比) 回复值:Y * 最大血量/100
	SPE_TRIGGER_EFFECT_MP_LESS_RECOVER_MP = 52,				//!< 52.法力低于X%,则回复Y%最大法力值  P1:X(千分比)  P2:Y(千分比)

	SPE_TRIGGER_EFFECT_RECORD_BURN_MP_ADD_MP_NUM = 53,		//!< 53.特效54的燃烧目标最大蓝量前置特效  P1:燃烧千分比  P2:吸收千分比
	SPE_TRIGGER_EFFECT_BURN_MP_ADD_MP_BASE_ON_RECORD = 54,	//!< 54.燃烧目标蓝量（按特效53记录的数值）
	SPE_TRIGGER_EFFECT_REVIVE_OR_HEAL = 55,					//!< 55.随机复活一个死亡单位,回复X%与Y%蓝量,若没有死亡单位则随机回复一个单位Z%血量和F%蓝量 P1:X P2:Y P3:Z P4:F (都是百分比,都是最大血蓝为基础)(同阵营,可随机自身)
	SPE_TRIGGER_EFFECT_ADD_BUFF_BY_ADJACENT_GRID = 56,		//!< 56.给自身周围一格单位添加BUFF P1:buff_id  P2 buff等级 P3 概率（千分比, 0和1000都代表必定成功）(包括自身)
	SPE_TRIGGER_EFFECT_ADD_CAN_MERGE_HP_SHIELD_BUFF_1 = 57,	//!< 57.给自身添加可叠加生命护盾BUFF P1 buff_id  P2 buff等级  P3 概率（千分比, 0和1000都代表必定成功）P4 系数   护盾公式:自身等级 *　P4
	SPE_TRIGGER_EFFECT_PERFORM_SKILL_1 = 58,				//!< 58.概率释放一个技能（允许角色没有这个技能，查找优先己方死亡单位,无死亡单位则存活(可自身)） P1 概率百分比 P2 技能ID P3 技能等级
	SPE_TRIGGER_EFFECT_ADD_BUFF_BY_TARGET_ENEMY_SIDE = 59,	//!< 59.因为敌方而触发,则加buff  P1 作用对象(0:自身  1:目标  2:自身位置取反) P2 buff_id  P3 buff等级  P4 概率（千分比, 0和1000都代表必定成功）
	SPE_TRIGGER_EFFECT_POSIBLE_BURN_MANA = 60,				//!< 60.概率燃烧蓝量  P1 概率百分比   P2 燃烧数值   
	SPE_TRIGGER_EFFECT_EXPLODE_2 = 61,						//!< 61.死亡时对敌方所有单位造成真实伤害  P1 固定点数
	SPE_TRIGGER_EFFECT_ADD_SHIELD_BY_MAXHP = 62,			//!< 62.加可叠加生命护盾BUFF（数值与最大生命值相关） P1 buff_id  P2 buff等级  P3 最大生命值系数百分比   护盾值=最大生命值*P3/100
	SPE_TRIGGER_EFFECT_PERFORM_SKILL_2 = 63,				//!< 63.概率释放一个技能（允许角色没有这个技能，查找优先己方异常状态单位,无异常状态单位则随机一个队友） P1 概率百分比 P2 技能ID P3 技能等级
	SPE_TRIGGER_EFFECT_CLEAR_NEGATIVE_STATE_1 = 64,			//!< 64.清除X个自身负面状态（只清异常和禁锢状态） P1：概率百分比 P2:X
	SPE_TRIGGER_EFFECT_LAST_ROUND_NOT_CAUSE_DAMAGE = 65,	//!< 65.上个回合如果没有造成伤害,则加BUFF P1 buff_id P2 buff等级 P3 概率(千分比)
	SPE_TRIGGER_EFFECT_ADD_ATTR_BUFF_BY_ROLE_ATTR = 66,		//!< 66.添加属性BUFF,属性值基于角色 P1:buff_id P2:buff_level P3:attr_type P4:基于属性(千分比)(属性值:角色属性*P4/1000)（BUFF必须使用类型29：按数值改变属性的BUFF）
	SPE_TRIGGER_EFFECT_ADD_BUFF_ENEMY_ALL_BC_FAKE_SKILL = 67,//!< 67.给敌方全体X概率释放假技能,Y概率添加BUFF(表现为假技能) P1:X(千分比) P2:Y(千分比) P3:buff_id P4:skill_id


	SPE_TRIGGER_EFFECT_MAX,
};

enum SPE_EFFECT_COND						//!<  特效触发的条件 
{
	SPE_SKILL_COND_INVALID = 0,
	SPE_SKILL_COND_SKILL_ID,				//!<  1. 按技能ID
	SPE_SKILL_COND_SKILL_TYPE,				//!<  2. 按技能类型
	SPE_SKILL_COND_SKILL_MAGIC_TYPE,		//!<  3. 按技能的法术类型
	SPE_SKILL_COND_NORMAL_ATTACK_CRITICAL,	//!<  4. 普攻且暴击
	SPE_SKILL_COND_SKILL_CRITICAL,			//!<  5. 按技能ID，且触发暴击  p1 技能ID
	SPE_SKILL_COND_KILL_SOMEONE,			//!<  6. 按技能ID，且造成击杀  p1 技能ID
	SPE_SKILL_COND_MAGIC_OR_PHYSIC_SKILL,	//!<  7. 按技能是法术还是物理 p1 填1就是法术，填0就是物理 
	SPE_SKILL_COND_IS_HEAL_SKILL,			//!<  8. 判断技能是否治疗技能
	SPE_SKILL_COND_ON_STEALTH_OR_ACTION_UNDER_STEALTH_TRIGGER_CRITICAL, //!< 9. 处于隐身状态下（被动），或行动开始时处于隐身状态下（主动） 触发暴击时
	SPE_SKILL_COND_MAX,
};


#endif