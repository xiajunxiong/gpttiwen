#ifndef __BATTLE_EVENT_DEF_HPP__
#define __BATTLE_EVENT_DEF_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/struct/battle/battle_def.hpp"

////////////////////////////////////////////////////// BATTLE_EVENT ////////////////////////////////////////////////

enum BATTLE_EVENT
{
	BATTLE_EVENT_INVALID = 0,					// 
	BATTLE_EVENT_PERFORM_SKILL,					//!< 1 释放技能 
	BATTLE_EVENT_DODGE,							//!< 2 闪避
	BATTLE_EVENT_MODIFY_HP,						//!< 3 HP变化
	BATTLE_EVENT_COMBINE_ATTACK,				//!< 4 合击		\see BattleEventCombineAttack
	BATTLE_EVENT_RUN_AWAY,						//!< 5 逃跑		\see BattleEventRunAway
	BATTLE_EVENT_REPLACE_BUFF,					//!< 6 BUFF替换
	BATTLE_EVENT_MERGE_BUFF,					//!< 7 BUFF合并
	BATTLE_EVENT_ADD_BUFF_TO_LIST,				//!< 8 BUFF新增到角色上
	BATTLE_EVENT_REMOVE_BUFF,					//!< 9 BUFF去除
	BATTLE_EVENT_SELF_DESTRUCT,					//!< 10 自爆
	BATTLE_EVENT_CHARACTER_DIE,					//!< 11 角色死亡 \see BattleEventCharacterDie
	BATTLE_EVENT_COUNTER_ATTACK,				//!< 12 反击
	BATTLE_EVENT_BE_GUARD,						//!< 13 被攻击时，被其他角色护卫 \see BattleEventBeGuard
	BATTLE_EVENT_CHANGE_POSITION,				//!< 14 换位
	BATTLE_EVENT_USE_ITEM,						//!< 15 使用物品
	BATTLE_EVENT_CHANGE_EQUIPMENT,				//!< 16 替换装备
	BATTLE_EVENT_MODIFY_CD,						//!< 17 技能CD时间改
	BATTLE_EVENT_ROUND_END,						//!< 18 回合结束
	BATTLE_EVENT_SUMMON_BATTLE_CHARACTER,		//!< 19 召唤战斗单位
	BATTLE_EVENT_SUMMON,						//!< 20 用于播放召唤动作
	BATTLE_EVENT_RETRIEVE_PET,					//!< 21 用于播放召回动作
	BATTLE_EVENT_COMBINE_ATTACK_PERFORM_ONE_HIT,//!< 22 合击，播放一次攻击   \note 没有data数据  grid_idx表示当前要出击的角色
	BATTLE_EVENT_MODIFY_MP,						//!< 23 MP变化
	BATTLE_EVENT_COMBINE_ATTACK_TOTAL_DAMAGE,	//!< 24 合击总伤害结点  \see BattleEventCombineAttackTotalDamage
	BATTLE_EVENT_PERFORM_COMBO_SKILL,			//!< 25 释放连击技能  \see BattleEventPerformComboSkill
	BATTLE_EVENT_COMBO_SKILL_ONE_HIT,			//!< 26 连击技能的一次攻击  \see BattleEventComboSkillOneHit
	BATTLE_EVENT_SEAL_PET,						//!< 27 封印宠物 \see BattleEventSealPet
	BATTLE_EVENT_PERFORM_FABAO_SKILL,			//!< 28 释放法宝技能 \see BattleEventPerformFabaoSkill
	BATTLE_EVENT_ELEMENT_CHANGE,				//!< 29 元素改变 \see BattleEventElementChange
	BATTLE_EVENT_VANISH,						//!< 30 原地消失
	BATTLE_EVENT_PRE_SKILL_EFFECT,				//!< 31 技能前置效果事件
	BATTLE_EVENT_POST_SKILL_EFFECT,				//!< 32 技能后置效果事件
	BATTLE_EVENT_SKILL_EFFECT,					//!< 33 技能效果事件
	BATTLE_EVENT_ABSORB_DAMAGE,					//!< 34 伤害吸收事件
	BATTLE_EVENT_SPECIAL_EFFECT_ACTIVE,			//!< 35 特效触发事件
	BATTLE_EVENT_SKILL_HIT,						//!< 36 技能命中事件
	BATTLE_EVENT_INEFFETIVE,					//!< 37 无效(飘字）
	BATTLE_EVENT_CLIENT_DIALOG,					//!< 38 客户端显示对话框(剧情战报用）
	BATTLE_EVENT_CLIENT_SHOT_EFFECT,			//!< 39 客户端切镜头(剧情战报用）	
	BATTLE_EVENT_CLIENT_GUIDE,					//!< 40 客户端指引（剧情战报用）
	BATTLE_EVENT_MONSTER_TRANSFORM,             //!< 41 怪物变身
	BATTLE_EVENT_BUFF_MISS,						//!< 42 Buff不中事件
	BATTLE_EVENT_USE_MP_ITEM,					//!< 43 使用蓝药事件
	BATTLE_EVENT_BUFF_SUSTAIN_NUM_CHANGE,		//!< 44 BUFF持续时间改变事件
	BATTLE_EVENT_SKILL_PERFORM_FAIL,			//!< 45 技能释放失败 （飘字：“释放失败”)
	BATTLE_EVENT_MODIFY_RAGE_POINTS,			//!< 46 改变怒气值
	BATTLE_EVENT_NOTICE_NUM,					//!< 47 飘字
	BATTLE_EVENT_CLIENT_CUSTOM,					//!< 48 客户端自定义事件
	BATTLE_EVENT_SIDE_SHOW_HP,					//!< 49 全队显示HP
	BATTLE_EVENT_FABAO_INFO_CHANGE,				//!< 50 法宝数据改变
	BATTLE_EVENT_ADD_HALO,						//!< 51 新增光环
	BATTLE_EVENT_REMOVE_HALO,					//!< 52 删除光环
	BATTLE_EVENT_HALO_CHANGE_POSITION,			//!< 53 光环换位
	BATTLE_EVENT_IMPRISON_PERFORM_FAILED,		//!< 54 被禁锢了释放失败（飘字：“XX中，无法释放”）
	BATTLE_EVENT_SKILL_REMAIN_CAST_TIMES,		//!< 55 技能剩余次数改变
	BATTLE_EVENT_HEART_SKILL_INFO_CHANGE,		//!< 56 心法数据改变
	BATTLE_EVENT_LINK_TOGETHER,					//!< 57 链接在一起
	BATTLE_EVENT_LINK_BREAK,					//!< 58 链接断开
	BATTLE_EVENT_INSTANT_KILL,					//!< 59 秒杀事件（用于改变客户端技能的表现）
	BATTLE_EVENT_CHARACTER_SPECIAL_EFFECT_LIST,	//!< 60 角色特效列表下发（用于新召唤的角色）
	BATTLE_EVENT_ADD_SINGLE_SP,					//!< 61 角色增加单个特效
	BATTLE_EVENT_REMOVE_SINGLE_SP,				//!< 62 角色移除单个特效
	BATTLE_EVENT_SPIRIT_POINT_CHANGE,			//!< 63 怒气值sp改变
	BATTLE_EVENT_SEED_EXPLODE,					//!< 64 爆种事件
	BATTLE_EVENT_SPECIAL_EFFECT_TARGET,			//!< 65 特效作用目标事件
	BATTLE_EVENT_SPREAD_SKILL,					//!< 66 扩散技能事件
	BATTLE_EVENT_HALLOW_PERFORM_SKILL,			//!< 67 圣器释放技能
	BATTLE_EVENT_HALLOW_SKILL_HIT,				//!< 68 圣器技能命中
	BATTLE_EVENT_HALLOW_SKILL_EFFECT,			//!< 69 圣器技能效果事件
	BATTLE_EVENT_HALLOW_FLY_AWAY,				//!< 70 圣器被打飞
	BATTLE_EVENT_HALLOW_FLY_WORDS,				//!< 71 圣器飘字
	BATTLE_EVENT_HALLOW_ACTIVATE_LIST,			//!< 72 圣器激活列表下发
	BATTLE_EVENT_HALLOW_INTERVENE,				//!< 73 圣器援护上场
	BATTLE_EVENT_HALLOW_GIFT_TRIGGER,			//!< 74 圣器天赋触发
	BATTLE_EVENT_HUANSHOU_MODIFY_CD,			//!< 75 幻兽技能CD时间改

	BATTLE_EVENT_MAX,
};

extern UInt16 BATTLE_EVENT_VERSION[BATTLE_EVENT_MAX];

extern void BattleEventVersionInit();

typedef std::map<int, UInt16> EventVersionMap;

#pragma pack(push)
#pragma pack(4)

// 协议结构 按4对齐 尽量不要浪费空间
struct BattleEventPerformSkill				//!< 释放技能 
{
	BattleEventPerformSkill() : skill_id(0), target_num(0)
	{
		memset(target_list, 0, sizeof(target_list));
	}

	void* operator new(size_t c);
	void operator delete(void *m);

	UInt16 skill_id;						//!< 技能id
	short target_num;						//!< 目标数
	short target_list[MAX_CHARACTER_NUM];	//!< 目标列表（按目标数来读）
};

struct BattleEventPerformSkillFailed
{
	BattleEventPerformSkillFailed() : skill_id(0), reserve_sh(0) {}

	void* operator new(size_t c);
	void operator delete(void *m);

	UInt16 skill_id;
	short reserve_sh;
};

struct BattleEventPerformFabaoSkill
{
	BattleEventPerformFabaoSkill() : fabao_skill_type(0), target_grid_idx(0) {}

	void* operator new(size_t c);
	void operator delete(void *m);

	short fabao_skill_type;					//!< 法宝技能类型
	short target_grid_idx;					//!< 目标格点
};

struct BattleEventModifyHP					//!< 血量改变
{
	BattleEventModifyHP() : modify_num(0), modify_desc(0), behit_desc(0), attr_injure_num(0), 
		dec_reamin_hp_shield(0), remain_freeze_shield(0), origin_freeze_shield(0) {}

	void* operator new(size_t c);
	void operator delete(void *m);

	int modify_num;							//!< 改变的数值
	int cur_num;							//!< 改变后的数值
	short modify_desc;						//!< 改变的原因
	short behit_desc;						//!< 被击表现
	int attr_injure_num;					//!< 克制/被克制的数值 （不等于0时显示出来）

	short behit_buff_id;					//!< 被击buffid  0则没有
	UInt16 behit_buff_source_skill;			//!< 被击buff来源技能ID
	int dec_reamin_hp_shield;				//!< 剩余护盾值
	int remain_freeze_shield;				//!< 剩余冰冻护盾值
	int origin_freeze_shield;				//!< 原有冰冻护盾值
};

struct BattleEventModifyMP					//!< 魔量改变
{
	BattleEventModifyMP() {}

	void* operator new(size_t c);
	void operator delete(void *m);

	int modify_num;							//!< 改变的数值
	int cur_num;							//!< 改变后的数值
	short modify_desc;						//!< 改变的原因
	short reserve_sh;						//!< 保留字
};

struct BattleEventModifyRagePoints			//!< 怒气变化
{
	BattleEventModifyRagePoints() {}

	void* operator new(size_t c);
	void operator delete(void *m);

	int modify_num;							//!< 改变的数值
	int cur_num;							//!< 改变后的数值
	short modify_desc;						//!< 改变的原因
	short reserve_sh;						//!< 保留字
};

struct BattleEventCombineAttack				//!< 合击结点
{
	BattleEventCombineAttack() : target_grid_idx(0), combine_num(0)
	{
		memset(combine_list, 0, sizeof(combine_list));
	}

	void* operator new(size_t c);
	void operator delete(void *m);

	short target_grid_idx;					//!< 合击目标 
	short combine_num;						//!< 参与合击的角色数
	short combine_list[SIDE_CHARACTER_NUM];	//!< 参与合击的角色格点列表
};

struct BattleEventRunAway					//!< 逃跑结点
{
	BattleEventRunAway() : is_run_away_succ(0), reserve_ch(0), reserve_sh(0) {}

	void* operator new(size_t c);
	void operator delete(void *m);

	char is_run_away_succ;					//!< 是否成功
	char reserve_ch;						//!< 保留字
	short reserve_sh;						//!< 保留字
};

struct BattleEventReplaceBuff
{
	BattleEventReplaceBuff() : new_buff_id(0), new_buff_level(0), new_buff_key(0), replaced_key(0), sustain_num(0), buff_layer(0) {}

	void* operator new(size_t c);
	void operator delete(void *m);

	short new_buff_id;
	short new_buff_level;
	unsigned int new_buff_key;
	unsigned int replaced_key;
	short sustain_num;
	short buff_layer;
};

struct BattleEventMergeBuff
{
	BattleEventMergeBuff() : new_buff_id(0), new_buff_level(0), merge_buff_key(0), sustain_num(0), buff_layer(0){}

	void* operator new(size_t c);
	void operator delete(void *m);

	short new_buff_id;
	short new_buff_level;
	unsigned int merge_buff_key;
	short sustain_num;
	short buff_layer;
};

struct BattleEventAddBuffToList
{
	BattleEventAddBuffToList() : new_buff_id(0), new_buff_level(0), new_buff_key(0), sustain_num(0), buff_layer(0) {}

	void* operator new(size_t c);
	void operator delete(void *m);

	short new_buff_id;
	short new_buff_level;
	unsigned int new_buff_key;
	short sustain_num;
	short buff_layer;
};

struct BattleEventRemoveBuff
{
	BattleEventRemoveBuff() : removed_buff_key(0) {}

	void* operator new(size_t c);
	void operator delete(void *m);

	unsigned int removed_buff_key;
};

struct BattleEventCharacterDie		//!< 角色死亡结点
{
	BattleEventCharacterDie() : die_type(0), reserve_sh(0) {}

	void* operator new(size_t c);
	void operator delete(void *m);

	short die_type;					//!< 死亡类型  \see BATTLE_DIE_TYPE
	short reserve_sh;				//!< 保留字
};

struct BattleEventBeGuard
{
	BattleEventBeGuard() : guardian_grid_idx(INVALID_GRID_IDX), reserve_sh(0) {}

	void* operator new(size_t c);
	void operator delete(void *m);

	short guardian_grid_idx;		//!< 护卫者格点
	short reserve_sh;				//!< 保留字
};

struct BattleEventUseItem
{
	BattleEventUseItem() : target_grid_idx(INVALID_GRID_IDX) {}

	void* operator new(size_t c);
	void operator delete(void *m);

	short target_grid_idx;			//!< 目标格点
	ItemID item_id;					//!< 物品ID
	short is_use_fake_potion;		//!< 是否用假药
	short reserve_sh;				//!< 保留字
};

struct BattleEventChangeEquipment
{
	BattleEventChangeEquipment() : before_equip_id(-1), after_equip_id(-1) {}

	void* operator new(size_t c);
	void operator delete(void *m);

	ItemID before_equip_id;
	ItemID after_equip_id;
};

struct BattleEventModifyCD
{
	BattleEventModifyCD() : skill_idx(0), modify_num(0), cur_num(0), modify_reason(0) {}

	void* operator new(size_t c);
	void operator delete(void *m);

	short skill_idx;
	short modify_num;
	short cur_num;
	short modify_reason;
};

struct BattleEventSummonBattleCharacter
{
	BattleEventSummonBattleCharacter() {}

	void* operator new(size_t c);
	void operator delete(void *m);

	int summon_pet_index; //!< 召唤宠物的序号[-1,3) (-1为非召唤宠物， 0到2为召唤宠物的序号)   

	BattleCharacterInfo character_info;
};

struct BattleEventCombineAttackTotalDamage
{
	BattleEventCombineAttackTotalDamage() {}

	void* operator new(size_t c);
	void operator delete(void *m);

	int total_damage;		// 总伤害
};

struct BattleEventCounterAttack
{
	BattleEventCounterAttack() {}

	void* operator new(size_t c);
	void operator delete(void *m);

	short target_grid_idx;			//!< 反击的目标格点
	short reserve_sh;				//!< 保留字
};

struct BattleEventPerformComboSkill
{
	BattleEventPerformComboSkill() {}

	void* operator new(size_t c);
	void operator delete(void *m);

	UInt16 skill_id;				//!< 技能id
	short reserve_sh;
};

struct BattleEventComboSkillOneHit
{
	BattleEventComboSkillOneHit() {}

	void* operator new(size_t c);
	void operator delete(void *m);

	short target_grid_idx;			//!< 目标格点
	UInt16 instant_kill_skill_id;	//!< 秒杀技能ID   为0则没有秒杀
};

struct BattleEventSealPet
{
	BattleEventSealPet() {}

	void* operator new(size_t c);
	void operator delete(void *m);

	short target_grid_idx;			//!< 目标格点
	char is_succ;					//!< 是否成功 (0:否  1：是）
	char reserve_ch;				//!< 保留字
	ItemID seal_card_item_id;		//!< 封印卡物品
	short reserve_sh;				//!< 保留字
};

struct BattleEventBuffMiss
{
	BattleEventBuffMiss() {}

	void* operator new(size_t c);
	void operator delete(void *m);

	short miss_reason;				//!< buff不中的原因 \see BATTLE_BUFF_MISS_REASON
	short reserve_sh;				//!< 保留字
};

struct BattleEventElementChange
{
	BattleEventElementChange() : element_water(0), element_fire(0), element_wind(0), element_earth(0) {}

	void* operator new(size_t c);
	void operator delete(void *m);

	short element_water;
	short element_fire;
	short element_wind;
	short element_earth;
};

enum BATTLE_BUFF_MISS_REASON
{
	BATTLE_BUFF_MISS_REASON_IMEFFECTIVE = 0, //!< 0 未生效
	BATTLE_BUFF_MISS_REASON_IMMUNE,			 //!< 1 免疫
	BATTLE_BUFF_MISS_REASON_RESIST,		     //!< 2 抵抗成功
};

enum BATTLE_SUMMON_REASON
{
	BATTLE_SUMMON_REASON_ROLE_SUMMON_PET,		//!< 0 召唤宠物
	BATTLE_SUMMON_REASON_MONSTER_SUMMON,		//!< 1 怪物召唤
	BATTLE_SUMMON_REASON_SCRIPT_SUMMON,			//!< 2 脚本召唤
	BATTLE_SUMMON_REASON_MONSTER_REINFORCE,		//!< 3 怪物增援
	BATTLE_SUMMON_REASON_ROLE_REINFORCE,		//!< 4 主角增援
	BATTLE_SUMMON_REASON_PET_REINFORCE,			//!< 5 宠物增援
	BATTLE_SUMMON_REASON_PET_SUMMON_PET,		//!< 6 宠物自己召唤宠物
};

struct BattleEventSummon
{
	BattleEventSummon() : summon_reason(0) {}

	void* operator new(size_t c);
	void operator delete(void *m);

	short summon_reason;				//!< \see BATTLE_SUMMON_REASON
	short is_summon_mutiple;			//!< 是否召唤多个  1:是 0:否
};

struct BattleEventAbsorbDamage
{
	BattleEventAbsorbDamage() : buff_id(0), source_skill_id(0) {}

	void* operator new(size_t c);
	void operator delete(void *m);

	short buff_id;
	UInt16 source_skill_id;
};

struct BattleEventSpecialEffectActive
{
	BattleEventSpecialEffectActive() : sp_id(0) {}

	void* operator new(size_t c);
	void operator delete(void *m);

	int sp_id;			// 特效ID
};

struct BattleEventSkillHit
{
	BattleEventSkillHit() : target_grid_idx(0), skill_id(0) {}

	void* operator new(size_t c);
	void operator delete(void *m);

	short target_grid_idx;
	UInt16 skill_id;
};

struct BattleEventHallowSkillHit	// 68 圣器技能命中目标
{
	BattleEventHallowSkillHit() : target_grid_idx(0), skill_id(0) {}

	void* operator new(size_t c);
	void operator delete(void *m);

	short target_grid_idx;
	short skill_id;
};

struct BattleEventClientDialog
{
	BattleEventClientDialog() : dialog_id(0) {}

	void* operator new(size_t c);
	void operator delete(void *m);

	int dialog_id;
};

struct BattleEventClientShotEffect
{
	BattleEventClientShotEffect() : shot_effect_id(0) {}

	void* operator new(size_t c);
	void operator delete(void *m);

	int shot_effect_id;
};

struct BattleEventClientGuide
{
	BattleEventClientGuide() {}

	void* operator new(size_t c);
	void operator delete(void *m);

	int guide_id;
};

struct BattleEventMonsterTransform
{
	BattleEventMonsterTransform() {}

	void* operator new(size_t c);
	void operator delete(void *m);

	BattleCharacterInfo character_info;
};

struct BattleEventUseMpItem
{
	BattleEventUseMpItem() : used_times(0), max_times(0) {}

	void* operator new(size_t c);
	void operator delete(void *m);

	short used_times;		//!< 当前已使用次数
	short max_times;		//!< 最大可使用次数
};

struct BattleEventBuffSustainNumChange
{
	BattleEventBuffSustainNumChange() : new_sustain_num(0), unique_key(0u) {}

	void* operator new(size_t c);
	void operator delete(void *m);

	int new_sustain_num;			//!< 新的持续时间
	unsigned int unique_key;		//!< 唯一标识
};

struct BattleEventRetrievePet
{
	BattleEventRetrievePet() : caller_grid_idx(0) {}

	void* operator new(size_t c);
	void operator delete(void *m);

	int caller_grid_idx;
};

struct BattleEventNoticeNum
{
	BattleEventNoticeNum() : notice_num(0) {}

	void* operator new(size_t c);
	void operator delete(void *m);

	int notice_num;
};

struct BattleEventClientCustom
{
	BattleEventClientCustom() : param1(0), param2(0), param3(0), param4(0), param5(0), param6(0) {}

	void* operator new(size_t c);
	void operator delete(void *m);

	int param1;
	int param2;
	int param3;
	int param4;
	int param5;
	int param6;
};

struct BattleEventSideShowHP
{
	BattleEventSideShowHP() : side(0), is_show(0) {}

	void* operator new(size_t c);
	void operator delete(void *m);

	short side;	   //!< 0攻方  1守方
	short is_show; //!< 0关	   1开
};

struct BattleEventSelfDestruct
{
	BattleEventSelfDestruct() : type(0), reserve_sh(0), param1(0) {}

	enum EXPLODE_TYPE
	{
		EXPLODE_TYPE_NORMAL,			//!< 0 普通自爆
		EXPLODE_TYPE_SPECIAL_EFFECT,	//!< 1 特效自爆
	};

	void* operator new(size_t c);
	void operator delete(void *m);

	short type;
	short reserve_sh;
	int param1;
};

struct BattleEventFabaoInfoChange
{
	BattleEventFabaoInfoChange() {}

	void* operator new(size_t c);
	void operator delete(void *m);

	int role_id;
	short cd_num;			// cd大于0时不可释放
	short remain_times;		// 剩余法宝使用次数 （全法宝统一使用这个次数）
};

struct BattleEventAddHalo						//!< 新增光环
{
	BattleEventAddHalo() {}

	void* operator new(size_t c);
	void operator delete(void *m);

	int halo_sp_id;								//!< 光环SP_ID
	char cover_grid_flag[MAX_CHARACTER_NUM];	//!< 格点覆盖 flag[x] = y, x表示格点[0,20) y表示是否被覆盖[0否,1是]
};

struct BattleEventRemoveHalo					//!< 删除光环
{
	BattleEventRemoveHalo() {}

	void* operator new(size_t c);
	void operator delete(void *m);

	int halo_sp_id;								//!< 光环SP_ID
};

struct BattleEventHaloChangePosition			//!< 光环所有者移位
{
	BattleEventHaloChangePosition() {}

	void* operator new(size_t c);
	void operator delete(void *m);

	int old_grid_idx;							//!< 旧的位置的格点
	int halo_sp_id;								//!< 光环SP_ID
	char old_cover_grid_flag[MAX_CHARACTER_NUM];//!< 旧的格点覆盖 flag[x] = y, x表示格点[0,20) y表示是否被覆盖[0否,1是]
	char new_cover_grid_flag[MAX_CHARACTER_NUM];//!< 新的格点覆盖 flag[x] = y, x表示格点[0,20) y表示是否被覆盖[0否,1是]
};

struct BattleEventImprisonPerformFailed	//!< 角色因特殊状态导致释放技能、行动失败
{
	BattleEventImprisonPerformFailed() {}

	void* operator new(size_t c);
	void operator delete(void *m);

	int special_state_type;			//!< 特殊状态类型
};

struct BattleEventModifySkillRemainCastTimes	//!< 当技能剩余释放次数改变时下发
{
	BattleEventModifySkillRemainCastTimes() {}

	void* operator new(size_t c);
	void operator delete(void *m);

	UInt16 skill_id;
	short remain_cast_times;
};

struct BattleEventHeartSkillInfoChange			//!< 心法信息修改
{
	BattleEventHeartSkillInfoChange() {}

	void* operator new(size_t c);
	void operator delete(void *m);

	int role_id;
	short remain_times;		// 剩余心法技能使用次数 
	char is_vaild;			// 是否生效	0:失效 1:生效
	char reserve_ch;
};

struct BattleEventLinkTogether		//事件57 链接在一起
{
	BattleEventLinkTogether() {}

	void* operator new(size_t c);
	void operator delete(void *m);

	short link_target_idx;	//链接的目标格点
	short link_type;	// 链接类型（目前只有类型0：铁索连环)
};

struct BattleEventLinkBreak			//事件58 断开链接
{
	BattleEventLinkBreak() {}

	void* operator new(size_t c);
	void operator delete(void *m);

	short link_target_idx; // 断开的目标格点
	short link_type;	// 链接类型  （目前只有类型0：铁索连环)
};

struct BattleEventSkillInstantKill	//!< 事件59 秒杀事件
{
	BattleEventSkillInstantKill() {}

	void* operator new(size_t c);
	void operator delete(void *m);

	short is_instant_kill;		//!< 是否触发秒杀（0否  1是）
	short instant_kill_grid_idx;//!< 秒杀的目标格点
};

struct BattleEventCharacterSpecialEffectList
{
	BattleEventCharacterSpecialEffectList() {}

	void* operator new(size_t c);
	void operator delete(void *m);

	short special_effect_num;
	short reserve_sh;

	int special_effect_list[20];	//!< 最多支持20个特效
};

struct BattleEventAddSingleSP	//!< 事件61 战斗添加单个特效
{
	BattleEventAddSingleSP() : sp_id(0) {}

	void* operator new(size_t c);
	void operator delete(void *m);

	int sp_id;
};

struct BattleEventRemoveSingleSP	//!< 事件62 战斗移除单个特效
{
	BattleEventRemoveSingleSP() : sp_id(0) {}

	void* operator new(size_t c);
	void operator delete(void *m);

	int sp_id;
};

struct BattleEventSpiritPointChange	//!< 事件63 怒气值sp变化事件
{
	BattleEventSpiritPointChange() : change_spirit_point(0), remain_spirit_point(0){}

	void* operator new(size_t c);
	void operator delete(void *m);

	enum CHANGE_REASON
	{
		CHANGE_REASON_DIE_REDUCE = 0,	//!< 死亡减少怒气值
		CHANGE_REASON_ROUND_ADD = 1,	//!< 回合结束加
	};

	int change_spirit_point;	//!< 变化的值
	int remain_spirit_point;	//!< 剩余的值
	int reason;					//!< 变化原因
};

struct BattleEventSeedExplode	//!< 事件64 爆种
{
	BattleEventSeedExplode() : seed_type(0), buff_id(0), buff_key(0) {}

	void* operator new(size_t c);
	void operator delete(void *m);

	short seed_type;		//!< 爆种类型 [1:水  2:火  3:风  4:土]
	short buff_id;			//!< 爆种的buff_id
	unsigned int buff_key;	//!< 爆种的buff_key
};

struct BattleEventSpecialEffectTarget	//!< 事件65 特效作用目标事件
{
	BattleEventSpecialEffectTarget() : deliver_grid_idx(0) {}

	void* operator new(size_t c);
	void operator delete(void *m);

	int deliver_grid_idx;			// 特效作用目标
};

struct BattleEventSpreadSkill		//!< 事件66 扩散技能事件
{
	BattleEventSpreadSkill() : source_grid_idx(0), spread_times(0) {}

	void* operator new(size_t c);
	void operator delete(void *m);
	
	int source_grid_idx;		// 扩散技能释放者
	int spread_times;			// 扩散次数 起始0
};

struct BattleEventHallowPerformSkill	// 事件67 圣器释放技能
{
	BattleEventHallowPerformSkill() : hallow_id(0), hallow_skill_id(0), 
		hallow_skill_lv(0), target_num(0) 
	{
		memset(target_list, 0, sizeof(target_list));
	}

	void* operator new(size_t c);
	void operator delete(void *m);

	short hallow_id;
	short hallow_skill_id;
	short hallow_skill_lv;
	short assist_num;
	short reserve_sh;
	short target_num;						//!< 目标数
	short target_list[MAX_CHARACTER_NUM];	//!< 目标列表（按目标数来读）
};

struct BattleEventHallowFlyAway	// 事件70 圣器被打飞
{
	BattleEventHallowFlyAway() : hallow_id(0), hallow_grid_idx(0) {}

	void* operator new(size_t c);
	void operator delete(void *m);

	short hallow_id;
	short hallow_grid_idx;
};

struct BattleEventHallowFlyWords // 事件71 圣器飘字
{
	BattleEventHallowFlyWords() : fly_words_type(0){}

	enum FLY_WORDS_TYPE
	{
		FLY_WORDS_TYPE_INVALID = 0,
		FLY_WORDS_TYPE_DODGE = 1,		// 闪避
		FLY_WORDS_TYPE_TARGET_NOT_FOUND_ABORT = 2, // 找不到目标，放弃释放
		FLY_WORDS_TYPE_ALL_IN_CD_ABORT = 3,	// 所有技能在CD中，放弃释放
		FLY_WORDS_TYPE_MASTER_DEAD_ABORT = 4, // 主人死亡，放弃释放
		FLY_WORDS_TYPE_NUM
	};

	void* operator new(size_t c);
	void operator delete(void *m);

	int fly_words_type;
	int param1;
};

struct BattleEventHallowActivateList	//事件72 圣器激活列表
{
	BattleEventHallowActivateList() : hallow_num(0) {}

	void* operator new(size_t c);
	void operator delete(void *m);

	enum STATUS
	{
		STATUS_NOT_ACTIVATE = 0,   //不激活
		STATUS_ACTIVATE_FIGHT = 1, //主战 (本回合要放技能）
		STATUS_ACTIVATE_ASSIST = 2,//助战（本回合不放技能）
	};

	struct HallowInfo
	{
		short hallow_grid_idx;
		short status;			// 见STATUS枚举
	};

	int hallow_num;
	HallowInfo hallow_list[BATTLE_MAX_ROLE_NUM]; // 最多10个
};

struct BattleEventHallowIntervene	//事件73 圣器援护上场
{
	BattleEventHallowIntervene() : hallow_id(0), owner_uid(0){}

	void* operator new(size_t c);
	void operator delete(void *m);

	int hallow_id;	//!< 圣器ID
	int owner_uid;	//!< 拥有者ID
};

struct BattleEventHallowGiftTrigger	//事件74 圣器天赋触发
{
	BattleEventHallowGiftTrigger() : hallow_id(0), hallow_gift_id(0) {}

	void* operator new(size_t c);
	void operator delete(void *m);

	int hallow_id;	//!< 圣器ID
	int hallow_gift_id; //!< 圣器天赋ID
};

struct BattleEventHuanShouModifyCD
{
	BattleEventHuanShouModifyCD() : role_id(0), cooldown_num(0), reserve_sh(0) {}

	void* operator new(size_t c);
	void operator delete(void *m);

	int role_id;
	short cooldown_num;		//冷却时间
	short reserve_sh;
};


#pragma pack(pop)

#endif