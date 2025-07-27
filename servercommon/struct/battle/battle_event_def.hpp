#ifndef __BATTLE_EVENT_DEF_HPP__
#define __BATTLE_EVENT_DEF_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/struct/battle/battle_def.hpp"

////////////////////////////////////////////////////// BATTLE_EVENT ////////////////////////////////////////////////

enum BATTLE_EVENT
{
	BATTLE_EVENT_INVALID = 0,					// 
	BATTLE_EVENT_PERFORM_SKILL,					//!< 1 �ͷż��� 
	BATTLE_EVENT_DODGE,							//!< 2 ����
	BATTLE_EVENT_MODIFY_HP,						//!< 3 HP�仯
	BATTLE_EVENT_COMBINE_ATTACK,				//!< 4 �ϻ�		\see BattleEventCombineAttack
	BATTLE_EVENT_RUN_AWAY,						//!< 5 ����		\see BattleEventRunAway
	BATTLE_EVENT_REPLACE_BUFF,					//!< 6 BUFF�滻
	BATTLE_EVENT_MERGE_BUFF,					//!< 7 BUFF�ϲ�
	BATTLE_EVENT_ADD_BUFF_TO_LIST,				//!< 8 BUFF��������ɫ��
	BATTLE_EVENT_REMOVE_BUFF,					//!< 9 BUFFȥ��
	BATTLE_EVENT_SELF_DESTRUCT,					//!< 10 �Ա�
	BATTLE_EVENT_CHARACTER_DIE,					//!< 11 ��ɫ���� \see BattleEventCharacterDie
	BATTLE_EVENT_COUNTER_ATTACK,				//!< 12 ����
	BATTLE_EVENT_BE_GUARD,						//!< 13 ������ʱ����������ɫ���� \see BattleEventBeGuard
	BATTLE_EVENT_CHANGE_POSITION,				//!< 14 ��λ
	BATTLE_EVENT_USE_ITEM,						//!< 15 ʹ����Ʒ
	BATTLE_EVENT_CHANGE_EQUIPMENT,				//!< 16 �滻װ��
	BATTLE_EVENT_MODIFY_CD,						//!< 17 ����CDʱ���
	BATTLE_EVENT_ROUND_END,						//!< 18 �غϽ���
	BATTLE_EVENT_SUMMON_BATTLE_CHARACTER,		//!< 19 �ٻ�ս����λ
	BATTLE_EVENT_SUMMON,						//!< 20 ���ڲ����ٻ�����
	BATTLE_EVENT_RETRIEVE_PET,					//!< 21 ���ڲ����ٻض���
	BATTLE_EVENT_COMBINE_ATTACK_PERFORM_ONE_HIT,//!< 22 �ϻ�������һ�ι���   \note û��data����  grid_idx��ʾ��ǰҪ�����Ľ�ɫ
	BATTLE_EVENT_MODIFY_MP,						//!< 23 MP�仯
	BATTLE_EVENT_COMBINE_ATTACK_TOTAL_DAMAGE,	//!< 24 �ϻ����˺����  \see BattleEventCombineAttackTotalDamage
	BATTLE_EVENT_PERFORM_COMBO_SKILL,			//!< 25 �ͷ���������  \see BattleEventPerformComboSkill
	BATTLE_EVENT_COMBO_SKILL_ONE_HIT,			//!< 26 �������ܵ�һ�ι���  \see BattleEventComboSkillOneHit
	BATTLE_EVENT_SEAL_PET,						//!< 27 ��ӡ���� \see BattleEventSealPet
	BATTLE_EVENT_PERFORM_FABAO_SKILL,			//!< 28 �ͷŷ������� \see BattleEventPerformFabaoSkill
	BATTLE_EVENT_ELEMENT_CHANGE,				//!< 29 Ԫ�ظı� \see BattleEventElementChange
	BATTLE_EVENT_VANISH,						//!< 30 ԭ����ʧ
	BATTLE_EVENT_PRE_SKILL_EFFECT,				//!< 31 ����ǰ��Ч���¼�
	BATTLE_EVENT_POST_SKILL_EFFECT,				//!< 32 ���ܺ���Ч���¼�
	BATTLE_EVENT_SKILL_EFFECT,					//!< 33 ����Ч���¼�
	BATTLE_EVENT_ABSORB_DAMAGE,					//!< 34 �˺������¼�
	BATTLE_EVENT_SPECIAL_EFFECT_ACTIVE,			//!< 35 ��Ч�����¼�
	BATTLE_EVENT_SKILL_HIT,						//!< 36 ���������¼�
	BATTLE_EVENT_INEFFETIVE,					//!< 37 ��Ч(Ʈ�֣�
	BATTLE_EVENT_CLIENT_DIALOG,					//!< 38 �ͻ�����ʾ�Ի���(����ս���ã�
	BATTLE_EVENT_CLIENT_SHOT_EFFECT,			//!< 39 �ͻ����о�ͷ(����ս���ã�	
	BATTLE_EVENT_CLIENT_GUIDE,					//!< 40 �ͻ���ָ��������ս���ã�
	BATTLE_EVENT_MONSTER_TRANSFORM,             //!< 41 �������
	BATTLE_EVENT_BUFF_MISS,						//!< 42 Buff�����¼�
	BATTLE_EVENT_USE_MP_ITEM,					//!< 43 ʹ����ҩ�¼�
	BATTLE_EVENT_BUFF_SUSTAIN_NUM_CHANGE,		//!< 44 BUFF����ʱ��ı��¼�
	BATTLE_EVENT_SKILL_PERFORM_FAIL,			//!< 45 �����ͷ�ʧ�� ��Ʈ�֣����ͷ�ʧ�ܡ�)
	BATTLE_EVENT_MODIFY_RAGE_POINTS,			//!< 46 �ı�ŭ��ֵ
	BATTLE_EVENT_NOTICE_NUM,					//!< 47 Ʈ��
	BATTLE_EVENT_CLIENT_CUSTOM,					//!< 48 �ͻ����Զ����¼�
	BATTLE_EVENT_SIDE_SHOW_HP,					//!< 49 ȫ����ʾHP
	BATTLE_EVENT_FABAO_INFO_CHANGE,				//!< 50 �������ݸı�
	BATTLE_EVENT_ADD_HALO,						//!< 51 �����⻷
	BATTLE_EVENT_REMOVE_HALO,					//!< 52 ɾ���⻷
	BATTLE_EVENT_HALO_CHANGE_POSITION,			//!< 53 �⻷��λ
	BATTLE_EVENT_IMPRISON_PERFORM_FAILED,		//!< 54 ���������ͷ�ʧ�ܣ�Ʈ�֣���XX�У��޷��ͷš���
	BATTLE_EVENT_SKILL_REMAIN_CAST_TIMES,		//!< 55 ����ʣ������ı�
	BATTLE_EVENT_HEART_SKILL_INFO_CHANGE,		//!< 56 �ķ����ݸı�
	BATTLE_EVENT_LINK_TOGETHER,					//!< 57 ������һ��
	BATTLE_EVENT_LINK_BREAK,					//!< 58 ���ӶϿ�
	BATTLE_EVENT_INSTANT_KILL,					//!< 59 ��ɱ�¼������ڸı�ͻ��˼��ܵı��֣�
	BATTLE_EVENT_CHARACTER_SPECIAL_EFFECT_LIST,	//!< 60 ��ɫ��Ч�б��·����������ٻ��Ľ�ɫ��
	BATTLE_EVENT_ADD_SINGLE_SP,					//!< 61 ��ɫ���ӵ�����Ч
	BATTLE_EVENT_REMOVE_SINGLE_SP,				//!< 62 ��ɫ�Ƴ�������Ч
	BATTLE_EVENT_SPIRIT_POINT_CHANGE,			//!< 63 ŭ��ֵsp�ı�
	BATTLE_EVENT_SEED_EXPLODE,					//!< 64 �����¼�
	BATTLE_EVENT_SPECIAL_EFFECT_TARGET,			//!< 65 ��Ч����Ŀ���¼�
	BATTLE_EVENT_SPREAD_SKILL,					//!< 66 ��ɢ�����¼�
	BATTLE_EVENT_HALLOW_PERFORM_SKILL,			//!< 67 ʥ���ͷż���
	BATTLE_EVENT_HALLOW_SKILL_HIT,				//!< 68 ʥ����������
	BATTLE_EVENT_HALLOW_SKILL_EFFECT,			//!< 69 ʥ������Ч���¼�
	BATTLE_EVENT_HALLOW_FLY_AWAY,				//!< 70 ʥ�������
	BATTLE_EVENT_HALLOW_FLY_WORDS,				//!< 71 ʥ��Ʈ��
	BATTLE_EVENT_HALLOW_ACTIVATE_LIST,			//!< 72 ʥ�������б��·�
	BATTLE_EVENT_HALLOW_INTERVENE,				//!< 73 ʥ��Ԯ���ϳ�
	BATTLE_EVENT_HALLOW_GIFT_TRIGGER,			//!< 74 ʥ���츳����
	BATTLE_EVENT_HUANSHOU_MODIFY_CD,			//!< 75 ���޼���CDʱ���

	BATTLE_EVENT_MAX,
};

extern UInt16 BATTLE_EVENT_VERSION[BATTLE_EVENT_MAX];

extern void BattleEventVersionInit();

typedef std::map<int, UInt16> EventVersionMap;

#pragma pack(push)
#pragma pack(4)

// Э��ṹ ��4���� ������Ҫ�˷ѿռ�
struct BattleEventPerformSkill				//!< �ͷż��� 
{
	BattleEventPerformSkill() : skill_id(0), target_num(0)
	{
		memset(target_list, 0, sizeof(target_list));
	}

	void* operator new(size_t c);
	void operator delete(void *m);

	UInt16 skill_id;						//!< ����id
	short target_num;						//!< Ŀ����
	short target_list[MAX_CHARACTER_NUM];	//!< Ŀ���б���Ŀ����������
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

	short fabao_skill_type;					//!< ������������
	short target_grid_idx;					//!< Ŀ����
};

struct BattleEventModifyHP					//!< Ѫ���ı�
{
	BattleEventModifyHP() : modify_num(0), modify_desc(0), behit_desc(0), attr_injure_num(0), 
		dec_reamin_hp_shield(0), remain_freeze_shield(0), origin_freeze_shield(0) {}

	void* operator new(size_t c);
	void operator delete(void *m);

	int modify_num;							//!< �ı����ֵ
	int cur_num;							//!< �ı�����ֵ
	short modify_desc;						//!< �ı��ԭ��
	short behit_desc;						//!< ��������
	int attr_injure_num;					//!< ����/�����Ƶ���ֵ ��������0ʱ��ʾ������

	short behit_buff_id;					//!< ����buffid  0��û��
	UInt16 behit_buff_source_skill;			//!< ����buff��Դ����ID
	int dec_reamin_hp_shield;				//!< ʣ�໤��ֵ
	int remain_freeze_shield;				//!< ʣ���������ֵ
	int origin_freeze_shield;				//!< ԭ�б�������ֵ
};

struct BattleEventModifyMP					//!< ħ���ı�
{
	BattleEventModifyMP() {}

	void* operator new(size_t c);
	void operator delete(void *m);

	int modify_num;							//!< �ı����ֵ
	int cur_num;							//!< �ı�����ֵ
	short modify_desc;						//!< �ı��ԭ��
	short reserve_sh;						//!< ������
};

struct BattleEventModifyRagePoints			//!< ŭ���仯
{
	BattleEventModifyRagePoints() {}

	void* operator new(size_t c);
	void operator delete(void *m);

	int modify_num;							//!< �ı����ֵ
	int cur_num;							//!< �ı�����ֵ
	short modify_desc;						//!< �ı��ԭ��
	short reserve_sh;						//!< ������
};

struct BattleEventCombineAttack				//!< �ϻ����
{
	BattleEventCombineAttack() : target_grid_idx(0), combine_num(0)
	{
		memset(combine_list, 0, sizeof(combine_list));
	}

	void* operator new(size_t c);
	void operator delete(void *m);

	short target_grid_idx;					//!< �ϻ�Ŀ�� 
	short combine_num;						//!< ����ϻ��Ľ�ɫ��
	short combine_list[SIDE_CHARACTER_NUM];	//!< ����ϻ��Ľ�ɫ����б�
};

struct BattleEventRunAway					//!< ���ܽ��
{
	BattleEventRunAway() : is_run_away_succ(0), reserve_ch(0), reserve_sh(0) {}

	void* operator new(size_t c);
	void operator delete(void *m);

	char is_run_away_succ;					//!< �Ƿ�ɹ�
	char reserve_ch;						//!< ������
	short reserve_sh;						//!< ������
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

struct BattleEventCharacterDie		//!< ��ɫ�������
{
	BattleEventCharacterDie() : die_type(0), reserve_sh(0) {}

	void* operator new(size_t c);
	void operator delete(void *m);

	short die_type;					//!< ��������  \see BATTLE_DIE_TYPE
	short reserve_sh;				//!< ������
};

struct BattleEventBeGuard
{
	BattleEventBeGuard() : guardian_grid_idx(INVALID_GRID_IDX), reserve_sh(0) {}

	void* operator new(size_t c);
	void operator delete(void *m);

	short guardian_grid_idx;		//!< �����߸��
	short reserve_sh;				//!< ������
};

struct BattleEventUseItem
{
	BattleEventUseItem() : target_grid_idx(INVALID_GRID_IDX) {}

	void* operator new(size_t c);
	void operator delete(void *m);

	short target_grid_idx;			//!< Ŀ����
	ItemID item_id;					//!< ��ƷID
	short is_use_fake_potion;		//!< �Ƿ��ü�ҩ
	short reserve_sh;				//!< ������
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

	int summon_pet_index; //!< �ٻ���������[-1,3) (-1Ϊ���ٻ���� 0��2Ϊ�ٻ���������)   

	BattleCharacterInfo character_info;
};

struct BattleEventCombineAttackTotalDamage
{
	BattleEventCombineAttackTotalDamage() {}

	void* operator new(size_t c);
	void operator delete(void *m);

	int total_damage;		// ���˺�
};

struct BattleEventCounterAttack
{
	BattleEventCounterAttack() {}

	void* operator new(size_t c);
	void operator delete(void *m);

	short target_grid_idx;			//!< ������Ŀ����
	short reserve_sh;				//!< ������
};

struct BattleEventPerformComboSkill
{
	BattleEventPerformComboSkill() {}

	void* operator new(size_t c);
	void operator delete(void *m);

	UInt16 skill_id;				//!< ����id
	short reserve_sh;
};

struct BattleEventComboSkillOneHit
{
	BattleEventComboSkillOneHit() {}

	void* operator new(size_t c);
	void operator delete(void *m);

	short target_grid_idx;			//!< Ŀ����
	UInt16 instant_kill_skill_id;	//!< ��ɱ����ID   Ϊ0��û����ɱ
};

struct BattleEventSealPet
{
	BattleEventSealPet() {}

	void* operator new(size_t c);
	void operator delete(void *m);

	short target_grid_idx;			//!< Ŀ����
	char is_succ;					//!< �Ƿ�ɹ� (0:��  1���ǣ�
	char reserve_ch;				//!< ������
	ItemID seal_card_item_id;		//!< ��ӡ����Ʒ
	short reserve_sh;				//!< ������
};

struct BattleEventBuffMiss
{
	BattleEventBuffMiss() {}

	void* operator new(size_t c);
	void operator delete(void *m);

	short miss_reason;				//!< buff���е�ԭ�� \see BATTLE_BUFF_MISS_REASON
	short reserve_sh;				//!< ������
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
	BATTLE_BUFF_MISS_REASON_IMEFFECTIVE = 0, //!< 0 δ��Ч
	BATTLE_BUFF_MISS_REASON_IMMUNE,			 //!< 1 ����
	BATTLE_BUFF_MISS_REASON_RESIST,		     //!< 2 �ֿ��ɹ�
};

enum BATTLE_SUMMON_REASON
{
	BATTLE_SUMMON_REASON_ROLE_SUMMON_PET,		//!< 0 �ٻ�����
	BATTLE_SUMMON_REASON_MONSTER_SUMMON,		//!< 1 �����ٻ�
	BATTLE_SUMMON_REASON_SCRIPT_SUMMON,			//!< 2 �ű��ٻ�
	BATTLE_SUMMON_REASON_MONSTER_REINFORCE,		//!< 3 ������Ԯ
	BATTLE_SUMMON_REASON_ROLE_REINFORCE,		//!< 4 ������Ԯ
	BATTLE_SUMMON_REASON_PET_REINFORCE,			//!< 5 ������Ԯ
	BATTLE_SUMMON_REASON_PET_SUMMON_PET,		//!< 6 �����Լ��ٻ�����
};

struct BattleEventSummon
{
	BattleEventSummon() : summon_reason(0) {}

	void* operator new(size_t c);
	void operator delete(void *m);

	short summon_reason;				//!< \see BATTLE_SUMMON_REASON
	short is_summon_mutiple;			//!< �Ƿ��ٻ����  1:�� 0:��
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

	int sp_id;			// ��ЧID
};

struct BattleEventSkillHit
{
	BattleEventSkillHit() : target_grid_idx(0), skill_id(0) {}

	void* operator new(size_t c);
	void operator delete(void *m);

	short target_grid_idx;
	UInt16 skill_id;
};

struct BattleEventHallowSkillHit	// 68 ʥ����������Ŀ��
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

	short used_times;		//!< ��ǰ��ʹ�ô���
	short max_times;		//!< ����ʹ�ô���
};

struct BattleEventBuffSustainNumChange
{
	BattleEventBuffSustainNumChange() : new_sustain_num(0), unique_key(0u) {}

	void* operator new(size_t c);
	void operator delete(void *m);

	int new_sustain_num;			//!< �µĳ���ʱ��
	unsigned int unique_key;		//!< Ψһ��ʶ
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

	short side;	   //!< 0����  1�ط�
	short is_show; //!< 0��	   1��
};

struct BattleEventSelfDestruct
{
	BattleEventSelfDestruct() : type(0), reserve_sh(0), param1(0) {}

	enum EXPLODE_TYPE
	{
		EXPLODE_TYPE_NORMAL,			//!< 0 ��ͨ�Ա�
		EXPLODE_TYPE_SPECIAL_EFFECT,	//!< 1 ��Ч�Ա�
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
	short cd_num;			// cd����0ʱ�����ͷ�
	short remain_times;		// ʣ�෨��ʹ�ô��� ��ȫ����ͳһʹ�����������
};

struct BattleEventAddHalo						//!< �����⻷
{
	BattleEventAddHalo() {}

	void* operator new(size_t c);
	void operator delete(void *m);

	int halo_sp_id;								//!< �⻷SP_ID
	char cover_grid_flag[MAX_CHARACTER_NUM];	//!< ��㸲�� flag[x] = y, x��ʾ���[0,20) y��ʾ�Ƿ񱻸���[0��,1��]
};

struct BattleEventRemoveHalo					//!< ɾ���⻷
{
	BattleEventRemoveHalo() {}

	void* operator new(size_t c);
	void operator delete(void *m);

	int halo_sp_id;								//!< �⻷SP_ID
};

struct BattleEventHaloChangePosition			//!< �⻷��������λ
{
	BattleEventHaloChangePosition() {}

	void* operator new(size_t c);
	void operator delete(void *m);

	int old_grid_idx;							//!< �ɵ�λ�õĸ��
	int halo_sp_id;								//!< �⻷SP_ID
	char old_cover_grid_flag[MAX_CHARACTER_NUM];//!< �ɵĸ�㸲�� flag[x] = y, x��ʾ���[0,20) y��ʾ�Ƿ񱻸���[0��,1��]
	char new_cover_grid_flag[MAX_CHARACTER_NUM];//!< �µĸ�㸲�� flag[x] = y, x��ʾ���[0,20) y��ʾ�Ƿ񱻸���[0��,1��]
};

struct BattleEventImprisonPerformFailed	//!< ��ɫ������״̬�����ͷż��ܡ��ж�ʧ��
{
	BattleEventImprisonPerformFailed() {}

	void* operator new(size_t c);
	void operator delete(void *m);

	int special_state_type;			//!< ����״̬����
};

struct BattleEventModifySkillRemainCastTimes	//!< ������ʣ���ͷŴ����ı�ʱ�·�
{
	BattleEventModifySkillRemainCastTimes() {}

	void* operator new(size_t c);
	void operator delete(void *m);

	UInt16 skill_id;
	short remain_cast_times;
};

struct BattleEventHeartSkillInfoChange			//!< �ķ���Ϣ�޸�
{
	BattleEventHeartSkillInfoChange() {}

	void* operator new(size_t c);
	void operator delete(void *m);

	int role_id;
	short remain_times;		// ʣ���ķ�����ʹ�ô��� 
	char is_vaild;			// �Ƿ���Ч	0:ʧЧ 1:��Ч
	char reserve_ch;
};

struct BattleEventLinkTogether		//�¼�57 ������һ��
{
	BattleEventLinkTogether() {}

	void* operator new(size_t c);
	void operator delete(void *m);

	short link_target_idx;	//���ӵ�Ŀ����
	short link_type;	// �������ͣ�Ŀǰֻ������0����������)
};

struct BattleEventLinkBreak			//�¼�58 �Ͽ�����
{
	BattleEventLinkBreak() {}

	void* operator new(size_t c);
	void operator delete(void *m);

	short link_target_idx; // �Ͽ���Ŀ����
	short link_type;	// ��������  ��Ŀǰֻ������0����������)
};

struct BattleEventSkillInstantKill	//!< �¼�59 ��ɱ�¼�
{
	BattleEventSkillInstantKill() {}

	void* operator new(size_t c);
	void operator delete(void *m);

	short is_instant_kill;		//!< �Ƿ񴥷���ɱ��0��  1�ǣ�
	short instant_kill_grid_idx;//!< ��ɱ��Ŀ����
};

struct BattleEventCharacterSpecialEffectList
{
	BattleEventCharacterSpecialEffectList() {}

	void* operator new(size_t c);
	void operator delete(void *m);

	short special_effect_num;
	short reserve_sh;

	int special_effect_list[20];	//!< ���֧��20����Ч
};

struct BattleEventAddSingleSP	//!< �¼�61 ս����ӵ�����Ч
{
	BattleEventAddSingleSP() : sp_id(0) {}

	void* operator new(size_t c);
	void operator delete(void *m);

	int sp_id;
};

struct BattleEventRemoveSingleSP	//!< �¼�62 ս���Ƴ�������Ч
{
	BattleEventRemoveSingleSP() : sp_id(0) {}

	void* operator new(size_t c);
	void operator delete(void *m);

	int sp_id;
};

struct BattleEventSpiritPointChange	//!< �¼�63 ŭ��ֵsp�仯�¼�
{
	BattleEventSpiritPointChange() : change_spirit_point(0), remain_spirit_point(0){}

	void* operator new(size_t c);
	void operator delete(void *m);

	enum CHANGE_REASON
	{
		CHANGE_REASON_DIE_REDUCE = 0,	//!< ��������ŭ��ֵ
		CHANGE_REASON_ROUND_ADD = 1,	//!< �غϽ�����
	};

	int change_spirit_point;	//!< �仯��ֵ
	int remain_spirit_point;	//!< ʣ���ֵ
	int reason;					//!< �仯ԭ��
};

struct BattleEventSeedExplode	//!< �¼�64 ����
{
	BattleEventSeedExplode() : seed_type(0), buff_id(0), buff_key(0) {}

	void* operator new(size_t c);
	void operator delete(void *m);

	short seed_type;		//!< �������� [1:ˮ  2:��  3:��  4:��]
	short buff_id;			//!< ���ֵ�buff_id
	unsigned int buff_key;	//!< ���ֵ�buff_key
};

struct BattleEventSpecialEffectTarget	//!< �¼�65 ��Ч����Ŀ���¼�
{
	BattleEventSpecialEffectTarget() : deliver_grid_idx(0) {}

	void* operator new(size_t c);
	void operator delete(void *m);

	int deliver_grid_idx;			// ��Ч����Ŀ��
};

struct BattleEventSpreadSkill		//!< �¼�66 ��ɢ�����¼�
{
	BattleEventSpreadSkill() : source_grid_idx(0), spread_times(0) {}

	void* operator new(size_t c);
	void operator delete(void *m);
	
	int source_grid_idx;		// ��ɢ�����ͷ���
	int spread_times;			// ��ɢ���� ��ʼ0
};

struct BattleEventHallowPerformSkill	// �¼�67 ʥ���ͷż���
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
	short target_num;						//!< Ŀ����
	short target_list[MAX_CHARACTER_NUM];	//!< Ŀ���б���Ŀ����������
};

struct BattleEventHallowFlyAway	// �¼�70 ʥ�������
{
	BattleEventHallowFlyAway() : hallow_id(0), hallow_grid_idx(0) {}

	void* operator new(size_t c);
	void operator delete(void *m);

	short hallow_id;
	short hallow_grid_idx;
};

struct BattleEventHallowFlyWords // �¼�71 ʥ��Ʈ��
{
	BattleEventHallowFlyWords() : fly_words_type(0){}

	enum FLY_WORDS_TYPE
	{
		FLY_WORDS_TYPE_INVALID = 0,
		FLY_WORDS_TYPE_DODGE = 1,		// ����
		FLY_WORDS_TYPE_TARGET_NOT_FOUND_ABORT = 2, // �Ҳ���Ŀ�꣬�����ͷ�
		FLY_WORDS_TYPE_ALL_IN_CD_ABORT = 3,	// ���м�����CD�У������ͷ�
		FLY_WORDS_TYPE_MASTER_DEAD_ABORT = 4, // ���������������ͷ�
		FLY_WORDS_TYPE_NUM
	};

	void* operator new(size_t c);
	void operator delete(void *m);

	int fly_words_type;
	int param1;
};

struct BattleEventHallowActivateList	//�¼�72 ʥ�������б�
{
	BattleEventHallowActivateList() : hallow_num(0) {}

	void* operator new(size_t c);
	void operator delete(void *m);

	enum STATUS
	{
		STATUS_NOT_ACTIVATE = 0,   //������
		STATUS_ACTIVATE_FIGHT = 1, //��ս (���غ�Ҫ�ż��ܣ�
		STATUS_ACTIVATE_ASSIST = 2,//��ս�����غϲ��ż��ܣ�
	};

	struct HallowInfo
	{
		short hallow_grid_idx;
		short status;			// ��STATUSö��
	};

	int hallow_num;
	HallowInfo hallow_list[BATTLE_MAX_ROLE_NUM]; // ���10��
};

struct BattleEventHallowIntervene	//�¼�73 ʥ��Ԯ���ϳ�
{
	BattleEventHallowIntervene() : hallow_id(0), owner_uid(0){}

	void* operator new(size_t c);
	void operator delete(void *m);

	int hallow_id;	//!< ʥ��ID
	int owner_uid;	//!< ӵ����ID
};

struct BattleEventHallowGiftTrigger	//�¼�74 ʥ���츳����
{
	BattleEventHallowGiftTrigger() : hallow_id(0), hallow_gift_id(0) {}

	void* operator new(size_t c);
	void operator delete(void *m);

	int hallow_id;	//!< ʥ��ID
	int hallow_gift_id; //!< ʥ���츳ID
};

struct BattleEventHuanShouModifyCD
{
	BattleEventHuanShouModifyCD() : role_id(0), cooldown_num(0), reserve_sh(0) {}

	void* operator new(size_t c);
	void operator delete(void *m);

	int role_id;
	short cooldown_num;		//��ȴʱ��
	short reserve_sh;
};


#pragma pack(pop)

#endif