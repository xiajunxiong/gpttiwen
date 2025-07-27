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
	BATTLE_MODE_NORMAL_MONSTER,				//!< 2 ��ͨ����
	BATTLE_MODE_MAZE,						//!< 3 �Թ�
	BATTLE_MODE_DEMON_CAVE,					//!< 4 ħ�ߣ������ȣ�
	BATTLE_MODE_MINE_MONSTER,				//!< 5 ���׹�
	BATTLE_MODE_FIELD_BOSS,					//!< 6 Ұ��BOSS����Ұ�⣬������������ģ�
	BATTLE_MODE_WORLD_BOSS,					//!< 7 ����BOSS���ַ��淨��
	BATTLE_MODE_ZHEN_ZHONG,					//!< 8 ���е��о��������ħ��
	BATTLE_MODE_COURAGE_CHALLENGE,			//!< 9 ��������ս (��ħ֮����
	BATTLE_MODE_GUILD_BOSS,					//!< 10 ��������BOSS  p1: boss_seq
	BATTLE_MODE_CRYSTAL_FB,					//!< 11 ˮ��������Ҫ��¼Ѫ�������֣�
	BATTLE_MODE_SINGLE_PVP_PRACTICE,		//!< 12 ����PVP�д�
	BATTLE_MODE_TREASURE_MAP,				//!< 13 ��ͼ����
	BATTLE_MODE_TASK_CHAIN,					//!< 14 ������
	BATTLE_MODE_ESCORT,						//!< 15 ����
	BATTLE_MODE_MEDAL,						//!< 16 ѫ�� ������
	BATTLE_MODE_TREASURE_ITEM,				//!< 17 �ر�ͼ ��Ʒʹ������
	BATTLE_MODE_MI_YI,						//!< 18 ��ҽ��ı[Զ��ħڣ]
	BATTLE_MODE_SQUARD_FIGHT,				//!< 19 С�Ӷ���
	BATTLE_MODE_BRAVE_GROUND,				//!< 20 �´��ع�
	BATTLE_MODE_MAZE_RACER,					//!< 21 �Թ�����
	BATTLE_MODE_MARRIAGE_FB,				//!< 22 ��Ե����
	BATTLE_MODE_GUILD_FIGHT,				//!< 23 ������Ҷ�
	BATTLE_MODE_SINGLE_ARENA,				//!< 24 ������
	BATTLE_MODE_CHIEF_ELECTION,				//!< 25 ��ϯ��ѡ
	BATTLE_MODE_NORMAL_PRACTICE,			//!< 26 ͨ���д� - ��ӻ���˶��п���
	BATTLE_MODE_GUILD_HONOR_BATTLE,			//!< 27 ������ҫս
	BATTLE_MODE_CHIEF_PEAK_BATTLE,			//!< 28 ��ϯ�۷�ս
	BATTLE_MODE_COURSE_OF_GROWTH,			//!< 29 �ɳ�֮·BOSS
	BATTLE_MODE_PLATFORM_BATTLE,			//!< 30 ��̨ս
	BATTLE_MODE_CHALLENGE_FB,				//!< 31 ��ս����
	BATTLE_MODE_ADVANCE_FB,					//!< 32 ���׸���
	BATTLE_MODE_GUIDE_FB,					//!< 33 ָ������
	BATTLE_MODE_WORLD_BOSS_2,				//!< 34 ����BOSS2������boss��
	BATTLE_MODE_NPC_FIGHT,					//!< 35 ѡ��NPC����ս��
	BATTLE_MODE_STORY,						//!< 36 ����ս��
	BATTLE_MODE_MEDAL_FB,					//!< 37 ѫ�¸���
	BATTLE_MODE_TEST_BACKUP,				//!< 38 ���������Ԯ
	BATTLE_MODE_FANTASY_FB,					//!< 39 ����þ�(��¼Ѫ��)
	BATTLE_MODE_EXPEL_MONSTER,				//!< 40 ��������
	BATTLE_MODE_JOY,						//!< 41 ����Ѱ�������淨
	BATTLE_MODE_JIAN_DAN_QIN_XIN,			//!< 42 ��������
	BATTLE_MODE_ONLY_FIGHT,					//!< 43 ��һ�а�(���˰������Ҷ�)
	BATTLE_MODE_REALIVE_GUIDE,				//!< 44 ����ָ��
	BATTLE_MODE_HUAN_JIE_ZHAN_CHANG_TIAN_TI,//!< 45 �ý�ս��
	BATTLE_MODE_ONE_DRAGON,					//!< 46 һ��������������ӹ��ܣ��������κ��淨��
	BATTLE_MODE_SHI_TU_FB,					//!< 47 ʦͽ����
	BATTLE_MODE_CHIEF_ELECTION_SEA,			//!< 48 ��ϯ��ѡ��ս
	BATTLE_MODE_SHAN_HAI_BOSS,				//!< 49 ɽ������
	BATTLE_MODE_DU_CHUANG_ZEI_YING,			//!< 50 ������Ӫ
	BATTLE_MODE_CATCH_FOX,					//!< 51 ץ�ú���
	BATTLE_MODE_MONPOLY_GIFT,				//!< 52 ţ�˵�ͷ
	BATTLE_MODE_DREAM_NOTES_CHALLENGE,		//!< 53 ��Ԩ��¼
	BATTLE_MODE_SCHOOL_EXERCISE,			//!< 54 У������
	BATTLE_MODE_RA_MIJINGQIWEN,				//!< 55 �ؾ�����
	BATTLE_MODE_REALM_BREAK,				//!< 56 ����ͻ��
	BATTLE_MODE_MATERIAL,					//!< 57 ���龳(���ϸ���)
	BATTLE_MODE_PET_MONOPOLY,				//!< 58 ����ؾ�
	BATTLE_MODE_LUN_HUI_WANG_CHUAN,			//!< 59 �ֻ�����
	BATTLE_MODE_CLOUD_ARENA,				//!< 60 �ƶ˾�����񷶥֮����
	BATTLE_MODE_INITIAL_FIGHT,				//!< 61 ��ʼս��
	BATTLE_MODE_NETHER_WORLD_PURGATORY,		//!< 62 ��ڤ����
	BATTLE_MODE_PET_GOD_FB,					//!< 63 ��ӡ֮��
	BATTLE_MODE_HUNDRED_GHOST,				//!< 64 �ٹ�ҹ��
	BATTLE_MODE_BIG_DIPPER,					//!< 65 ��������
	BATTLE_MODE_SUMMER_LI_LIAN,				//!< 66 ��������
	BATTLE_MODE_MAZE_SUMMER,				//!< 67 �����Թ�
	BATTLE_MODE_DUEL,						//!< 68 ����
	BATTLE_MODE_DU_JIE,						//!< 69 �ɽ�
	BATTLE_MODE_YIN_YANG_EYE,				//!< 70 ������
	BATTLE_MODE_REALM_BREAK_FIGHT,			//!< 71 ����ս��������֮����
	BATTLE_MODE_RA_MIJINGQIWEN_2,			//!< 72 �ؾ�����2 (��������)
	BATTLE_MODE_ENDLESS_TOWER,				//!< 73 �޾�֮��
	BATTLE_MODE_ZAI_ZHAN_YUN_ZE,			//!< 74 ��ս����	p_1:boss_index
	BATTLE_MODE_MO_ZU_ZAI_NA_LI,			//!< 75 ħ��������
	BATTLE_MODE_SHAN_HAI_ZHENG_FENG,		//!< 76 ɽ������
	BATTLE_MODE_WAN_LING_FB,				//!< 77 ���黰��
	BATTLE_MODE_PARTNER_PVE,				//!< 78 ���PVE(�쳽�׾�)
	BATTLE_MODE_WEDDING_FB,					//!< 79 ����
	BATTLE_MODE_WORLD_ARENA,				//!< 80 �۽�����
	BATTLE_MODE_WEDDING_FB_BAO_WEI_XIU_QIU,	//!< 81 ����-С��Ϸ ��������
	BATTLE_MODE_FALLEN_GOD,					//!< 82 ����֮��
	BATTLE_MODE_WORLD_TEAM_ARENA,			//!< 83	Ӣ�ۻ���
	BATTLE_MODE_QING_YUAN_SHENG_HUI,		//!< 84 ��Եʢ��
	BATTLE_MODE_YAO_XING_SHI_LIAN,			//!< 85 ��������
	BATTLE_MODE_LANTERN_RIDDIR,				//!< 86 ɽ���ƻ�
	BATTLE_MODE_UNIVERSE_TEAM_ARENA,		//!< 87 ���µ�һ
	BATTLE_MODE_HONGMENG_TIANJIE,			//!< 88 �������
	BATTLE_MODE_KE_JU_EXAM,					//!< 89 �ƾٿ���
	BATTLE_MODE_GOTO_SCHOOL,				//!< 90 ��Ҫ��ѧ
	BATTLE_MODE_ANCIENT_RUINS,				//!< 91 �Ϲ��ż�
	BATTLE_MODE_ELITE_TRAILS,				//!< 92 ��Ӣ����
	BATTLE_MODE_ANCIENT_RUINS_NORMAL_MONSTER, //!< 93 �Ϲ��ż�С�� (Ϊ���������ף���������
	BATTLE_MODE_ELITE_TRAILS_NORMAL_MONSTER,//!< 94 ��Ӣ����С�� (Ϊ���������ף���������
	BATTLE_MODE_NEW_COURAGE_CHALLENGE,		//!< 95 ߱��֮��
	BATTLE_MODE_MINI_GAME_BATTLE,			//!< 96 С��Ϸս��
	BATTLE_MODE_WORLD_BOSS_3,				//!< 97 ����BOSS3(������)
	BATTLE_MODE_SPECIAL_ANECDOTE_TASK,		//!< 98 ��ս����������
	BATTLE_MODE_TEAM_CHALLENGE_TASK,		//!< 99 �����ս����
	BATTLE_MODE_TASK_ROLE_MONSTER,		//!< 100 ������˹�
	BATTLE_MODE_MAX,
};

bool IsBattleModeMustUseMaxHpMp(int battle_mode);

enum BATTLE_STATUS
{
	BATTLE_STATUS_PREPARE = 0,		// ׼���׶�
	BATTLE_STATUS_FIGHTING,			// ս���׶�
	BATTLE_STATUS_CALCULATE,		// ����׶�
};

/**
* \note ׼��->���Ŷ���->�ȴ�����->����������->���Ŷ���
*/
enum ROUND_PHASE
{
	ROUND_PHASE_PREPARE = 0,			// ׼���׶Σ��˽׶δ��������¼���
	ROUND_PHASE_WAIT_INPUT = 1,			// �ȴ�����׶� ����������׶ε�˲�����˳����Ԥ����AI�Ķ�����ע��Ӧ����ҵ�����Ϊ���ȣ�
	ROUND_PHASE_COM_CALCULATE = 2,		// ����������׶�
	ROUND_PHASE_WATCH_ANIMATION = 3,	// ���Ŷ����׶�
};

enum RESULT_TYPE
{
	RESULT_TYPE_INVALID = 0,
	RESULT_TYPE_ATTACKER_WIN,		//!< 1 ����ʤ
	RESULT_TYPE_DEFENDER_WIN,		//!< 2 �ط�ʤ
	RESULT_TYPE_DRAW,				//!< 3 �;�(����ͬʱ��������
	RESULT_TYPE_FORCED_FINISH,		//!< 4 ս��ǿ�ƽ���
	RESULT_TYPE_EXCEED_MAX_ROUND,	//!< 5 �������غ���
	RESULT_TYPE_EXCEED_MAX_TIME,	//!< 6 �������ʱ������
	RESULT_TYPE_RUNAWAY,			//!< 7 ����
	RESULT_TYPE_MAX,
};

enum AUDIENCE_TYPE
{
	AUDIENCE_TYPE_NONE = 0,			//!< 0 �ǹ�ս
	AUDIENCE_TYPE_FLY_OB = 1,		//!< 1 �����˼�����ս
	AUDIENCE_TYPE_ALWAYS_OB = 2,	//!< 2 һֱ��ս 
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
	BATTLE_MOVE_TYPE_PERFORM_SKILL,		//!< 1 ʹ�ü��� grid_idx Ŀ��λ��  param1 Ŀ�꼼����� param2 Ŀ�꼼�ܵȼ�
	BATTLE_MOVE_TYPE_NORMAL_ATTACK,		//!< 2 �չ� grid_idx Ŀ��λ��
	BATTLE_MOVE_TYPE_NORMAL_DEFENCE,	//!< 3 ����  
	BATTLE_MOVE_TYPE_USE_ITEM,			//!< 4 ʹ����Ʒ grid_idx Ŀ��λ��  param1 ��Ʒ����[0:��Ѫҩ 1:����] (�ҵ�������������Ʒ��Ϣ��ʹ��֮)
	BATTLE_MOVE_TYPE_RUN_AWAY,			//!< 5 ����   
	BATTLE_MOVE_TYPE_CHANGE_POSITION,	//!< 6 ��λ�� 
	BATTLE_MOVE_TYPE_CHANGE_PET,		//!< 7 ������	param1 ����idx (-1Ϊ�ٻأ�
	BATTLE_MOVE_TYPE_SEAL_PET,			//!< 8 ��ӡ���� grid_idx Ŀ��λ��
	BATTLE_MOVE_TYPE_USE_FABAO,			//!< 9 ʹ�÷��� grid_idx Ŀ��λ��  param1 ������������ param2 �������ܲ���1 param3 �������ܲ���2
	BATTLE_MOVE_TYPE_INSTANT_NORMAL_ATTACK, //!< 10 ˲���չ� grid_idx Ŀ��λ��
	BATTLE_MOVE_TYPE_AI_SKILL,			//!< 11 ����ʩ��
	BATTLE_MOVE_TYPE_PERFORM_HEART_SKILL = 12,	//!< 12 ʹ���ķ�����
	BATTLE_MOVE_TYPE_PERFORM_CHANGE_CARD_SKILL = 13,	//!< 13 ʹ�ñ�������
	BATTLE_MOVE_TYPE_PERFORM_HUANSHOU_SKILL = 14,	//!< 14 ʹ�û��޼���

	BATTLE_MOVE_TYPE_ROLE_ACTION_MAX,

	BATTLE_MOVE_TYPE_MONSTER_ACTION_MIN = 20,  //!< -----------�˴��Ƿָ���, ���µĶ������ڹ������Ϊ------------
	BATTLE_MOVE_TYPE_SUMMON_MONSTER,	//!<21 �ٻ����� param1 �ٻ��߼� param2 �ٻ�id
	BATTLE_MOVE_TYPE_SELF_DESTRUCT,		//!<22 �Ա�		param1 �Ա��˺�
	BATTLE_MOVE_TYPE_DO_NOTHING,		//!<23 ר�Ÿ����������õģ���ֹ�����
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
	BATTLE_SKILL_TYPE_COMBO = 5,	// ����N��
	BATTLE_SKILL_TYPE_LINE = 6,		// ֱ�߼���
	BATTLE_SKILL_TYPE_COMMON_RANGE = 7,	// ͨ�÷�Χ����
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
	BATTLE_CHARACTER_TYPE_ROLE = 0,			//!< 0 ����
	BATTLE_CHARACTER_TYPE_PET,				//!< 1 ����
	BATTLE_CHARACTER_TYPE_MONSTER,			//!< 2 ����
	BATTLE_CHARACTER_TYPE_PARTNER,			//!< 3 ���
	BATTLE_CHARACTER_TYPE_ROLE_SHADOW,		//!< 4 ����Ӱ��
	BATTLE_CHARACTER_TYPE_PET_SHADOW,		//!< 5 ����Ӱ��
	BATTLE_CHARACTER_TYPE_PARTNER_ROBOT,	//!< 6 ��������
	BATTLE_CHARACTER_TYPE_NPC_HELPER,		//!< 7 ��սNPC
	BATTLE_CHARACTER_TYPE_MAX,
};

enum RACE_TYPE
{
	RACE_TYPE_MIN = 0,
	RACE_TYPE_HUMAN = RACE_TYPE_MIN,	// 0 ����
	RACE_TYPE_BEAST,					// 1 Ұ��
	RACE_TYPE_NATURE,					// 2 ��Ȼ
	RACE_TYPE_UNDEAD,					// 3 ����
	RACE_TYPE_INSECT,					// 4 ���
	RACE_TYPE_MYSTERY,					// 5 ����
	RACE_TYPE_GHOST,					// 6 ����
	RACE_TYPE_DRAGON,					// 7 ����
	RACE_TYPE_FLY,						// 8 ����
	RACE_TYPE_ANCIENT,					// 9 �Ϲ�
	
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
	BATTLE_MODIFY_MP_REASON_SKILL_CONSUME = 0,	//!< 0 ��������
	BATTLE_MODIFY_MP_REASON_DRUNK_STATE,		//!< 1 ����״̬
	BATTLE_MODIFY_MP_REASON_SELF_DESTRUCT,		//!< 2 �Ա�����
	BATTLE_MODIFY_MP_REASON_SPECIAL_EFFECT,		//!< 3 ��Ч�ı�����
	BATTLE_MODIFY_MP_REASON_BURN_MP_SKILL,		//!< 4 ��������
	BATTLE_MODIFY_MP_REASON_SYSTEM_SCRIPT,		//!< 5 ϵͳ�ű�
	BATTLE_MODIFY_MP_REASON_USE_MP_ITEM,		//!< 6 ʹ�ü�����Ʒ
	BATTLE_MODIFY_MP_REASON_ABSORB_MP,			//!< 7 ����
	BATTLE_MODIFY_MP_REASON_BUFF,				//!< 8 BUFF����
	BATTLE_MODIFY_MP_REASON_HALLOW_ADD_MP,		//!< 9 ʥ������
};

enum BATTLE_MODIFY_RAGE_POINT_REASON
{
	BATTLE_MODIFY_RAGE_POINT_REASON_ROUND = 0,			//!< 0 �غϿ�ʼ
	BATTLE_MODIFY_RAGE_POINT_REASON_NORMAL_ATTACK = 1,	//!< 1 �չ�
	BATTLE_MODIFY_RAGE_POINT_REASON_NORMAL_DEFENSE = 2,	//!< 2 ����
	BATTLE_MODIFY_RAGE_POINT_REASON_PERFORM_SKILL = 3,	//!< 3 �ͷż���
	BATTLE_MODIFY_RAGE_POINT_REASON_SPECIAL_EFFECT = 4, //!< 4 ս����Ч
	BATTLE_MODIFY_RAGE_POINT_REASON_SKILL_EFFECT = 5,	//!< 5 ����Ч��
};

enum BATTLE_MODIFY_SPIRIT_POINT_REASON
{
	BATTLE_MODIFY_SPIRIT_POINT_REASON_DIE_REDUCE = 0,	//!< 0 ��������
	BATTLE_MODIFY_SPIRIT_POINT_REASON_ROUND_ADD = 1,	//!< 1 ÿ�غ�����
	BATTLE_MODIFY_SPIRIT_POINT_REASON_SKILL = 2,		//!< 2 ��������
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

enum BATTLE_HP_DESC  //!< Ѫ���ı�ʱ���������
{
	BATTLE_HP_DESC_NORMAL,				//!< 0 ������
	BATTLE_HP_DESC_CRITICAL,			//!< 1 ����
	BATTLE_HP_DESC_RESERVE_1,			//!< 2 ������
	BATTLE_HP_DESC_HP_SHIELD,			//!< 3 ��������
	BATTLE_HP_DESC_POISON,				//!< 4 ��
	BATTLE_HP_DESC_COMBINE_ATTACK,		//!< 5 �ϻ�
	BATTLE_HP_DESC_ADD_HP,				//!< 6 ��Ѫ
	BATTLE_HP_DESC_DAMAGE_ABSORB_HP,	//!< 7 ��Ѫ
	BATTLE_HP_DESC_DAMAGE_BOUNCE,		//!< 8 �����˺�
	BATTLE_HP_DESC_IN_VAIN,				//!< 9 ��Ч
	BATTLE_HP_DESC_CRACK_DEFENSE,		//!< 10 �Ʒ�
	BATTLE_HP_DESC_RESERVE_3,			//!< 11 ������
	BATTLE_HP_DESC_ADD_HP_CRITCAL,		//!< 12 ������Ѫ
	BATTLE_HP_DESC_REAL_DAMAGE,			//!< 13 ��ʵ�˺�
	BATTLE_HP_DESC_SACRIFICE_HP,		//!< 14 �׼�����Ѫ��
	BATTLE_HP_DESC_DEATH_SICKLE,		//!< 15 ����֮��
	BATTLE_HP_DESC_DESPERATE,			//!< 16 ������
	BATTLE_HP_DESC_MAX
};

enum BATTLE_BE_HIT_DESC	//!< ����ʱ���������
{
	BATTLE_BE_HIT_DESC_NORMAL = 0,		//!< 0 ��
	BATTLE_BE_HIT_DESC_DEFENSE = 1,		//!< 1 ����
	BATTLE_BE_HIT_DESC_SHIELD = 2,		//!< 2 ����
	BATTLE_BE_HIT_DESC_BLOCK = 3,		//!< 3 ��
	BATTLE_BE_HIT_DESC_GUARD = 4,		//!< 4 ����
	BATTLE_BE_HIT_DESC_SHARE_DAMAGE = 5,//!< 5 �ֵ��˺�
	BATTLE_BE_HIT_DESC_DEATH_SICKLE = 6,//!< 6 ����֮��
	BATTLE_BE_HIT_DESC_BLOCK_1 = 7,		//!< 7 ��(α)(��������Ч����)
	BATTLE_BE_HIT_DESC_MAX,
};

enum BATTLE_ATTR
{
	BATTLE_ATTR_MIN = 0,
	BATTLE_ATTR_NORMAL_VALUE_BEGIN = BATTLE_ATTR_MIN,			// ��������ֵ��ʼ

	BATTLE_ATTR_ATTACK = BATTLE_ATTR_NORMAL_VALUE_BEGIN,		// 0 ���� 
	BATTLE_ATTR_DEFENSE,										// 1 ���� 
	BATTLE_ATTR_AGILITY,										// 2 ���� 
	BATTLE_ATTR_RECOVERY,										// 3 �ظ� 
	BATTLE_ATTR_MENTAL,											// 4 ���� 
	BATTLE_ATTR_CUR_HP,											// 5 ��ǰѪ��
	BATTLE_ATTR_MAX_HP,											// 6 ���Ѫ��
	BATTLE_ATTR_CUR_MP,											// 7 ��ǰħ��
	BATTLE_ATTR_MAX_MP,											// 8 ���ħ��
	BATTLE_ATTR_PANEL_END = BATTLE_ATTR_MAX_MP,					// �������

	BATTLE_ATTR_PHYSICAL_DEC_DAMAGE,							// 9 ���⣨ǧ�ֱ�)
	BATTLE_ATTR_MAGIC_DEC_DAMAGE,								// 10 ���⣨ǧ�ֱ�)
	BATTLE_ATTR_PHYSICAL_INC_DAMAGE,							// 11 ���ˣ�ǧ�ֱ�)
	BATTLE_ATTR_MAGIC_INC_DAMAGE,								// 12 ���ˣ�ǧ�ֱ�)

	BATTLE_ATTR_NORMAL_VALUE_END,								// ��������ֵ����
	BATTLE_ATTR_CORRECTED_VALUE_BEGIN = 15,						// ����ֵ��ʼ

	BATTLE_ATTR_CRITICAL = BATTLE_ATTR_CORRECTED_VALUE_BEGIN,	// 15 ��ɱ�ʣ�ǧ�ֱ�)
	BATTLE_ATTR_HIT,											// 16 �����ʣ�ǧ�ֱ�)
	BATTLE_ATTR_DODGE,											// 17 �����ʣ�ǧ�ֱȣ�
	BATTLE_ATTR_COUNTER_ATTACK,									// 18 �����ʣ�ǧ�ֱ�)
	BATTLE_ATTR_MAGIC_ATTACK,									// 19 ħ��
	BATTLE_ATTR_MAGIC_DEFENSE,									// 20 ħ��
	BATTLE_ATTR_CRITICAL_INC_VALUE,								// 21 ��ɱ�˺�ֵ��ǧ�ֱ�)
	BATTLE_ATTR_CRITICAL_DEC_VALUE,								// 22 ����ֵ��ǧ�ֱ�)
	BATTLE_ATTR_CONTROL_HIT,									// 23 ��������(ǧ�ֱ�)
	BATTLE_ATTR_REAL_DAMAGE,									// 24 ��ʵ�˺�ֵ
	BATTLE_ATTR_MAGIC_PENETRATION_RATE,							// 25 ������͸��ǧ�ֱ�)
	BATTLE_ATTR_ANTI_CONTROL_RATE,								// 26 ���Ƶֿ��ʣ�ǧ�ֱȣ�
	BATTLE_ATTR_MAGIC_CRIRICAL,									// 27 ������ɱ�ʣ�ǧ�ֱȣ�
	BATTLE_ATTR_MAGIC_CRIRICAL_INC_DAMAGE,						// 28 ������ɱ�˺�ֵ(ǧ�ֱ�)


	BATTLE_ATTR_CORRECTED_VALUE_END,							// ����ֵ����
	BATTLE_ATTR_ANTI_VALUE_BEGIN = 30,							// �ֿ�ֵ��ʼ

	BATTLE_ATTR_ANTI_POISON = BATTLE_ATTR_ANTI_VALUE_BEGIN,		// 30 �ж��ֿ���ǧ�ֱȣ�
	BATTLE_ATTR_ANTI_FREEZE,									// 31 �����ֿ���ǧ�ֱȣ�
	BATTLE_ATTR_ANTI_PETRIFY,									// 32 ʯ���ֿ���ǧ�ֱȣ�
	BATTLE_ATTR_ANTI_CHAOS,										// 33 ���ҵֿ���ǧ�ֱȣ�
	BATTLE_ATTR_ANTI_DRUNK,										// 34 ����ֿ���ǧ�ֱȣ�
	BATTLE_ATTR_ANTI_SILENCE,									// 35 ��Ĭ�ֿ���ǧ�ֱȣ�
	BATTLE_ATTR_ANTI_CURSE,										// 36 ����ֿ���ǧ�ֱȣ�
	BATTLE_ATTR_ANTI_SINK_HOLE,									// 37 ���ݵֿ���ǧ�ֱȣ�
	BATTLE_ATTR_ANTI_SEVERE_INJURE,								// 38 ���˵ֿ���ǧ�ֱȣ�
	BATTLE_ATTR_ANTI_DEFENCE_CRACKED,							// 39 �Ʒ��ֿ���ǧ�ֱȣ�

	BATTLE_ATTR_ANTI_VALUE_END,									// �ֿ�ֵ����
	BATTLE_ATTR_ELEMENT_BEGIN = 40,								// Ԫ��ֵ��ʼ

	BATTLE_ATTR_ELEMENT_WATER = BATTLE_ATTR_ELEMENT_BEGIN,		// ˮԪ��ֵ Ԫ��ȡֵ��Χ[0,10]������
	BATTLE_ATTR_ELEMENT_FIRE,									// ��
	BATTLE_ATTR_ELEMENT_WIND,									// ��
	BATTLE_ATTR_ELEMENT_EARTH,									// ��
	BATTLE_ATTR_ELEMENT_END,									// Ԫ��ֵ����

	BATTLE_ADD_POINT_TYPE_BEGIN,//����Ԫ�� - ��������Լӵ�
	BATTLE_ADD_POINT_TYPE_VITALITY = BATTLE_ADD_POINT_TYPE_BEGIN,	//!< ���� 45
	BATTLE_ADD_POINT_TYPE_STRENGTH,									//!< ���� 46 
	BATTLE_ADD_POINT_TYPE_POWER,									//!< ǿ�� 47 
	BATTLE_ADD_POINT_TYPE_AGILITY,									//!< ���� 48
	BATTLE_ADD_POINT_TYPE_MAGIC,									//!< ħ�� 49
	BATTLE_ADD_POINT_TYPE_END = BATTLE_ADD_POINT_TYPE_MAGIC,

	BATTLE_ATTR_MAX = 50,
};
UNSTD_STATIC_CHECK(BATTLE_ATTR_NORMAL_VALUE_END <= BATTLE_ATTR_CORRECTED_VALUE_BEGIN);
UNSTD_STATIC_CHECK(BATTLE_ATTR_CORRECTED_VALUE_END <= BATTLE_ATTR_ANTI_VALUE_BEGIN);
UNSTD_STATIC_CHECK(BATTLE_ATTR_ANTI_VALUE_END <= BATTLE_ATTR_ELEMENT_BEGIN);
UNSTD_STATIC_CHECK(BATTLE_ATTR_ELEMENT_END <= BATTLE_ATTR_MAX);
UNSTD_STATIC_CHECK(BATTLE_ATTR_MAX <= 50);

static const int BATTLE_BASE_ATTR_NUM = 5;		// д�����������������ء���
static const int BATTLE_AMEN_ATTR_NUM = BATTLE_ATTR_CORRECTED_VALUE_END - BATTLE_ATTR_CORRECTED_VALUE_BEGIN;
static const int BATTLE_RESI_ATTR_NUM = BATTLE_ATTR_ANTI_VALUE_END - BATTLE_ATTR_ANTI_VALUE_BEGIN;
static const int BATTLE_ELEMENT_ATTR_NUM = BATTLE_ATTR_ELEMENT_END - BATTLE_ATTR_ELEMENT_BEGIN;

enum BATTLE_SKILL_MAGIC_TYPE
{
	BATTLE_SKILL_MAGIC_TYPE_INVALID = 0,
	BATTLE_SKILL_MAGIC_TYPE_WATER = 1,				//!< 1 ˮϵħ��
	BATTLE_SKILL_MAGIC_TYPE_FIRE = 2,				//!< 2 ��ϵħ��
	BATTLE_SKILL_MAGIC_TYPE_WIND = 3,				//!< 3 ��ϵħ��
	BATTLE_SKILL_MAGIC_TYPE_EARTH = 4,				//!< 4 ��ϵħ��
	BATTLE_SKILL_MAGIC_TYPE_HOLY = 5,				//!< 5 ��ʥħ�� 
	BATTLE_SKILL_MAGIC_TYPE_ENERGY = 6,				//!< 6 ����ħ��
	BATTLE_SKILL_MAGIC_TYPE_CHAOS = 7,				//!< 7 ����ħ��
	BATTLE_SKILL_MAGIC_TYPE_MAX = 8,	
};

enum BATTLE_TARGET_DEAD_TYPE
{
	BATTLE_TARGET_ALIVE = 0,
	BATTLE_TARGET_DEAD = 1,
	BATTLE_TARGET_ALIVE_OR_DEAD = 2,
};

static const short USE_BATTLE_SERVER_INDEX = 0;		//!< ʹ��ս����index1
static const short MAX_CHARACTER_NUM = 20;			//!< ս��������ɫ��
static const short SIDE_CHARACTER_NUM = 10;
static const short NORMAL_SPEED_WAIT_TIME_S = 5 * 60; // X1�ٶȵȴ�����ʱ���300��   X2��150��
static const short FIRST_FINISH_ANIMATION_CONTINUE_WAIT_TIME_S = 10;
static const short WAIT_OTHER_ANIMATION_NOTICE_TIME_S = 4;	// �ȴ������˲��Ŷ���������4���δ������֪ͨ��������ĵȴ�
static const short WAIT_INPUT_TIME_S = 31; // 30����ֶ��������ӳٵ�ԭ�򣬸���31��
static const short WAIT_AUTO_INPUT_TIME_S = 1; // 1����Զ�
static const short FIRST_ROUND_WAIT_AUTO_INPUT_TIME_S = 4;
static const short MAX_ROUND_NUM = 300;
static const short ROW_CHARACTER_NUM = 5;
static const short CLOSE_BATTLE_RANGE_ROW = 2; // ��ս���루��λ���У�
static const short INVALID_GRID_IDX = -1;
static const int BATTLE_PERCENT_NUM = 100;
static const int BATTLE_THOUSAND_PERCENT_NUM = 1000;
static const int BATTLE_ATTR_PERCENT_NUM = 1000; // ������ǧ�ֱ�
static const int BATTLE_TEN_THOUSAND_PERCENT_NUM = 10000;	//��ֱ�
static const int BATTLE_FABAO_PERCENT_NUM = 100000; // �ٷֱȣ��ұ���С�����3λ������������ʮ��ֱ�
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
static const int BATTLE_SCRIPT_CONDITION_NUM = 10;  // ��Ȼ����10��������ò�����3������
static const int BATTLE_SCRIPT_EVENT_NUM = 10;	// ��Ȼ����10��������ò�����3���¼�
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

static const int BATTLE_MINE_MONSTER_GROUP = 65534;	// ���׹�ר��monster_group_id
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

static const int BATTLE_MAGIC_CRIRICAL_BASE_DAMAGE_PERCENT = 1300;	//������������ϵ��(ǧ�ֱ�)
static const int BATTLE_BURN_MP_DEEP_BLOW_LOW_MP_PERCENT = 65;		//����Ч��9���һ���۳��ķ���ֵ�ٷֱ�
static const int BATTLE_OTHER_EFFECT_SACRIFICE_SELF_ADD_QINGYUAN_PERCENT = 50;	;//����ս����Ч93,���Ѫ����������Ҫ���ڵ��ڸðٷֱ�

static const int BATTLE_CONDITION_MAX_NUM = 6;
UNSTD_STATIC_CHECK(BATTLE_CONDITION_MAX_NUM < (int)(sizeof(char) * 8));

static const int BATTLE_JIEYI_MAX_NUM = 4;
static const int BATTLE_JIEYI_SKILL_MAX_NUM = 4;

static const int BATTLE_QINGYUAN_SKILL_MAX_NUM = 4;

//Ҳ����ͨ�������������ڵ��б�������Ҫ��չÿ�������Ļ�
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

	short fabao_id;				//!< ����ID
	short fabao_level;			//!< �����ȼ� 
	short fabao_skill_type;		//!< ������������ \see BATTLE_FABAO_SKILL_TYPE
	short fabao_grade;			//!< �����׼�
	int fabao_effect_percent_num; //!< ����Ч���ٷֱ� * 1000
	int fabao_effect_percent_num_2; //!< ����Ч��2 �ٷֱ� * 1000
};

enum BATTLE_FABAO_SKILL_TYPE
{
	BATTLE_FABAO_SKILL_TYPE_INVALID = 100,
	BATTLE_FABAO_SKILL_TYPE_FORBID_ENEMY_FABAO_SKILL = 101,		//!< 1 ��ֹĿ���ͷŷ�������
	BATTLE_FABAO_SKILL_TYPE_TARGET_PET_RUNAWAY = 102,			//!< 2 ����ʹĿ���������
	BATTLE_FABAO_SKILL_TYPE_BOUNCE_NEXT_PHYSICAL_DAMAGE = 103,	//!< 3 ������һ�������˺��İٷֱ�
	BATTLE_FABAO_SKILL_TYPE_BOUNCE_NEXT_MAGIC_DAMAGE = 104,		//!< 4 ������һ��ħ���˺��İٷֱ�
	BATTLE_FABAO_SKILL_TYPE_ABSORB_NEXT_PHYSICAL_DAMAGE = 105,	//!< 5 ������һ�������˺��İٷֱ�
	BATTLE_FABAO_SKILL_TYPE_ABSORB_NEXT_MAGIC_DAMAGE = 106,		//!< 6 ������һ��ħ���˺��İٷֱ�
	BATTLE_FABAO_SKILL_TYPE_REAL_DAMAGE = 107,					//!< 7 ��ʵ�˺�
	BATTLE_FABAO_SKILL_TYPE_PHYSICAL_SHIELD = 108,				//!< 8 ������
	BATTLE_FABAO_SKILL_TYPE_MAGIC_SHIELD = 109,					//!< 9 ħ������
	BATTLE_FABAO_SKILL_TYPE_ELEMENT_TURNOVER = 110,				//!< 10 Ԫ�ط�ת
	BATTLE_FABAO_SKILL_TYPE_ELEMENT_CHAOS = 111,				//!< 11 Ԫ�ػ���1
	BATTLE_FABAO_SKILL_TYPE_PURIFY_AND_HEAL = 112,				//!< 12 �����ӻ�Ѫ
	BATTLE_FABAO_SKILL_TYPE_DETECT_EMNEMY_HP = 113,				//!< 13 �鿴�з�ȫ��Ŀ������ֵ1
	BATTLE_FABAO_SKILL_TYPE_SPEED_MODIFY = 114,					//!< 14 ���������ٶ�
	BATTLE_FABAO_SKILL_TYPE_REALIVE_BUFF = 115,					//!< 15 ����BUFF
	BATTLE_FABAO_SKILL_TYPE_MINE_MONSTER_DAMAGE_UP = 116,		//!< 16 ÿ������ӵ���߶�Ұ�ⰵ���˺��ӳ�%s
	BATTLE_FABAO_SKILL_TYPE_MONSTER_INJURE_DOWN = 117,			//!< 17 ÿ������ӵ�����ܵ������˺�����%s
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

	short skill_id;				//!< �ķ�����ID
	short level;				//!< �ķ����ܵȼ�
	short remain_times;			//!< ʣ��ʹ�ô���
	short reserve_sh;
};

enum BATTLE_SIDE_TYPE
{
	BATTLE_SIDE_TYPE_ATTACKER = 0,
	BATTLE_SIDE_TYPE_DEFENDER = 1,
	BATTLE_SIDE_NUM = 2,
};

// ����һ�������еĶ��Ч��������󴫵���ֵ
enum SKILL_EFFECT_STATISTIC 
{
	SKILL_EFFECT_STATISTIC_MIN = 0,
	SKILL_EFFECT_STATISTIC_DAMAGE = SKILL_EFFECT_STATISTIC_MIN,
	SKILL_EFFECT_STATISTIC_TARGET_NUM = 1,		//Ŀ����
	SKILL_EFFECT_STATISTIC_MAIN_TARGET_GRID_IDX = 2, // ��Ŀ����
	SKILL_EFFECT_STATISTIC_BURN_RAGE_POINT_NUM = 3,  // ȼ��Ԫ������
	SKILL_EFFECT_STATISTIC_PURIFY_NUM = 4,			 // �����������쳣״̬����
	SKILL_EFFECT_STATISTIC_MAX
};

enum IMMUNE_INJURE_TYPE
{
	IMMUNE_INJURE_TYPE_NONE = 0,
	IMMUNE_INJURE_TYPE_ALL = 1,		//!< 1. ���������˺�
	IMMUNE_INJURE_TYPE_PHYSIC = 2,	//!< 2. ���������˺�
	IMMUNE_INJURE_TYPE_MAGIC = 3,	//!< 3. ����ħ���˺�
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
	BATTLE_DAMAGE_TYPE_PHYSIC = 0,			//�����˺�
	BATTLE_DAMAGE_TYPE_MAGIC = 1,			//�����˺�
	BATTLE_DAMAGE_TYPE_HOLY_MAGIC = 2,		//�ظ�/����/��ʥ�˺�

	BATTLE_DAMAGE_TYPE_MAX
};

enum BATTLE_DAMAGE_EFFECT_TYPE
{
	BATTLE_DAMAGE_EFFECT_TYPE_PHYSIC = 0,			//�����˺�
	BATTLE_DAMAGE_EFFECT_TYPE_MAGIC_BOTH = 1,		//ħ���˺�(3+4)
	BATTLE_DAMAGE_EFFECT_TYPE_BOTH = 2,				//�˺�(0+3+4)
	BATTLE_DAMAGE_EFFECT_TYPE_MAGIC = 3,			//�����˺�
	BATTLE_DAMAGE_EFFECT_TYPE_HOLY_MAGIC = 4,		//�ظ�/����/��ʥ�˺�

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

	short grid_idx;	// Ŀ��λ��
	short move_type;
	bool has_done;  // �ѽ���
	int param1;
	int param2;
	int param3;
	int param4;
};

bool IsSpecialMove(int move_type);

enum BATTLE_EFFECT_TARGET
{
	BATTLE_EFFECT_TARGET_EVERYONE = 1,					// ������Ŀ����Ч
	BATTLE_EFFECT_TARGET_ONLY_MAIN = 2,					// ֻ����Ŀ����Ч
	BATTLE_EFFECT_TARGET_SOURCE = 3,					// ��ʩ����
	BATTLE_EFFECT_TARGET_SOURCE_FOR_EACH_TARGET = 4,	// ���ٸ�target���Ͷ�ʩ�������ö��ٴ�
	BATTLE_EFFECT_TARGET_EXCEPT_MAIN = 5,				// ������Ŀ�� ����Ŀ�궼��Ч
};

enum BATTLE_SUMMON_RULE
{
	BATTLE_SUMMON_RULE_RANDOM = 0,
	BATTLE_SUMMON_RULE_LEFT_RIGHT = 1,
	BATTLE_SUMMON_RULE_FIX_POSITION = 2,
	BATTLE_SUMMON_RULE_RANDOM_ON_FRONT_ROW = 3,	// ��ǰ������ٻ�Xֻ
	BATTLE_SUMMON_RULE_RANDOM_ON_BACK_ROW = 4,	// �ں�������ٻ�Xֻ
	BATTLE_SUMMON_RULE_MAX,
};

enum BATTLE_DIE_TYPE
{
	BATTLE_DIE_TYPE_NORMAL = 0,    //!< ��������
	BATTLE_DIE_TYPE_DISAPPEAR = 1, //!< ����ԭ����ʧ
	BATTLE_DIE_TYPE_FLYAWAY = 2,   //!< ����������ʧ
	BATTLE_DIE_TYPE_REALIVE = 3,   //!< ��������
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
	short cd_start;			//�볡cd
	short cd_perform;		//�ͷ�cd
};

struct BattleCharacterInfo
{
	int character_id;				//!< id 
	short character_type;			//!< [0:���� 1:���� 2:���� 3:���]		
	short level;					//!< �ȼ�
	int prof;						//!< ְҵ
	GameName name;					//!< ����
	char weapon_type;				//!< �������� [0:���� 1:�� 2:�� 3:�� 4:�� 5:ǹ 6:ȭ��]
	char is_chaos_element;			//!< �Ƿ���л���Ԫ�� [0:�� 1:��]������Ԫ��Ĭ��Ϊ10������������4��Ԫ�أ�
	ItemID weapon_id;				//!< ����ID
	int maxhp;						//!< ���Ѫ��
	int maxmp;						//!< ���ħ��
	int curhp;						//!< ��ǰѪ��
	int curmp;						//!< ��ǰħ��

	short element_water;			//!< ˮԪ��ֵ
	short element_fire;				//!< ��Ԫ��ֵ
	short element_wind;				//!< ��Ԫ��ֵ
	short element_earth;			//!< ��Ԫ��ֵ

	char race;						//!< �������� [0:���� 1:Ұ�� 2:ֲ�� 3:���� 4:���� 5:���� 6:���� 7:�� 8:����]
	char param;						//!< ��ɫ����Ϊ����ʱ������ǿ���ȣ�Ϊ���ʱ�����ǿ����
	short grid_idx;					//!< ��ɫ���ڸ��ӱ��

	Appearance appearance;			//!< ��ۣ��������ǣ�

	int skin_id;					//!< Ƥ��ID�����ڳ���ͻ�飬�Լ�����Ӱ�ӣ�
	int top_level;					//!< �۷�ȼ�
	int dec_reamin_hp_shield;		//!< ʣ�໤��ֵ
	int remain_freeze_shield;		//!< ʣ���������ֵ
	int origin_freeze_shield;		//!< ԭ���ı�������ֵ
	int spirit_point;				//!< ŭ��ֵSP  -1������ʾŭ��ֵ	
	int fly_flag;					//!< �����׶�/����(���ݽ�ɫ/����/���)�ֶζ���		
	
	short name_color;				//!< 0 ������ɫ  1 ��ɫ
	short reserve_sh;				//!< ������
	int reserve_int;				//!< ������
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

	int character_id;								// ��ɫID
	int id_param;									// ���ڸ�����id��������ʱֻ�г����õ���
	char character_type;							// ��ɫ����		\see BATTLE_CHARACTER_TYPE
	char is_use_cur_hp_mp;							// �Ƿ�ʹ�õ�ǰѪ����ħ��
	short level;									// �ȼ�
	GameName name;									// ����
	int quality;									// Ʒ��

	int profession;									// ְҵ			\see PROF_TYPE
	char race;										// ����			\see RACE_TYPE
	char weapon_type;								// ��������		\see WEAPON_TYPE
	ItemID weapon_id;								// ������ƷID

	BattleAttr attr_list[BATTLE_ATTR_MAX];			// �����б�

	struct SkillItem
	{
		unsigned short skill_id;					// ����ID
		unsigned short skill_level;					// ���ܵȼ�
	};

	char param;										// Ϊ����ʱ����ʾ�����ǿ���ȼ�	
	char is_long_range_unit;						// �Ƿ�Զ�̵�λ

	short skill_num;								// ��������
	SkillItem skill_list[MAX_BATTLE_SKILL_NUM];		// �����б�

	int capability;
	short fly_flag;									// �����׶�/����(���ݽ�ɫ/����/���)�ֶζ���	

	short wear_element_type;						// Я����Ԫ��ˮ������ \see ELEMENT_TYPE_MAX
	int reserve_int;
	long long reserve_ll[2];						// ������
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
	short exclusive_passive_skill_OBSOLETE;			//�ѷ����ֶ�

	int skin_id; // ����Ƥ��ID
	int other_sp_list[BATTLE_OTHER_EFFECT_NUM];		//��Ч�б�

	long long unique_id;							// �����ΨһID
	int lianyao_pet_id;
	int lianyao_pet_str_lv;		
	UNSTD_STATIC_CHECK(BATTLE_PET_GIFT_NUM == 20); // �޸Ĵ˴��ᵼ�¹��񱦿�Ļ����������쳣�������ز���

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

	short battle_row;		// ��ս����վλ 0:���� 1:ǰ��
	short battle_column;	// ��ս����վλ ��
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
	int skin_id;	// ���Ƥ��ID

	short battle_row;		// ���վλ 0:���� 1:ǰ��
	short battle_column;	// ���վλ ��

	int other_sp_num;
	int other_sp_list[BATTLE_PARTNER_OTHER_EFFECT_NUM];	//��Ч�б�

	int str_lv;				// ǿ���ȼ�
};


static const int BATTLE_POS_RELATION[ROW_CHARACTER_NUM] = { 2, 1, 3, 0, 4 };
static const int REVERSE_BATTLE_POS_RELATION[ROW_CHARACTER_NUM] = { 3, 1, 0, 2, 4 };


enum BATTLE_ROLE_OPERATE_TYPE
{
	BATTLE_ROLE_OPERATE_TYPE_INVALID = 0,
	BATTLE_ROLE_OPERATE_TYPE_SET_AUTO,					 //!< 1 �����Զ�    \param param1 �Ƿ��Զ�[0:�� 1:��]
	BATTLE_ROLE_OPERATE_TYPE_SET_AUTO_NORMAL_ATTACK,	 //!< 2 �����Զ��չ� 	\param param1 ��Ϊ��[0,1]
	BATTLE_ROLE_OPERATE_TYPE_SET_AUTO_DEFENSE,			 //!< 3 �����Զ�����		\param param1 ��Ϊ��[0,1]
	BATTLE_ROLE_OPERATE_TYPE_SET_AUTO_PERFORM_SKILL,	 //!< 4 �����Զ��ż���   \param param1 ��Ϊ��[0,1] \param param2 ����ID  \param param3 ���ܵȼ�
	BATTLE_ROLE_OPERATE_TYPE_SET_AUTO_RUNAWAY,			 //!< 5 �����Զ�����		\param param1 ��Ϊ��[0,1]
	BATTLE_ROLE_OPERATE_TYPE_SET_AUTO_CHANGE_POSITION,	 //!< 6 �����Զ���λ		\param param1 ��Ϊ��[0,1]	
	BATTLE_ROLE_OPERATE_TYPE_SET_AUTO_AI_SKILL,			 //!< 7 ����ʩ��

	BATTLE_ROLE_OPERATE_TYPE_SET_MOVE_NORMAL_ATTACK = 10,		//!< 10 �չ�  \param param1 ��Ϊ��[0,1]  \param param2 Ŀ����[0,20)
	BATTLE_ROLE_OPERATE_TYPE_SET_MOVE_DEFENSE,			 //!< 11 ����  \param param1 ��Ϊ��[0,1]
	BATTLE_ROLE_OPERATE_TYPE_SET_MOVE_PERFORM_SKILL,	 //!< 12 �ż��� \param param1 ��Ϊ��[0,1] \param param2 Ŀ����[0,20) \param param3 ����id  \param param4 ���ܵȼ�
	BATTLE_ROLE_OPERATE_TYPE_SET_MOVE_CHANGE_POSITION,	 //!< 13 ��λ  \param param1 ��Ϊ��[0,1]
	BATTLE_ROLE_OPERATE_TYPE_SET_MOVE_RUNAWAY,			 //!< 14 ����  \param param1 ��Ϊ��[0,1]
	BATTLE_ROLE_OPERATE_TYPE_SET_MOVE_CHANGE_PET,		 //!< 15 ����  \param param1 ��Ϊ��[0,1]  \param param2 ���ϵĳ������[0, 3)��[-1, 0) ����-1Ϊֻ�³費�ϳ�
	BATTLE_ROLE_OPERATE_TYPE_SET_MOVE_USE_ITEM,			 //!< 16 ʹ����Ʒ  \param param1 ��Ϊ��[0,1]  \param param2 Ŀ����[0,20) \param param3 ��Ʒ����column  \param param4 ��Ʒ���index
	BATTLE_ROLE_OPERATE_TYPE_SET_MOVE_SEAL_PET,			 //!< 17 ��ӡ���� \param param1 ��Ϊ��[0,1]  \param param2 Ŀ����[0,20)  \param param3 ��ӡ������[0:�տ� 1:���� 2:��]  \param param4 ��ӡ�Ĺ���ID��������ȡ����ID�Լ��ɹ����ϴ���ս����)
	BATTLE_ROLE_OPERATE_TYPE_SET_MOVE_USE_FABAO,		 //!< 18 ʹ�÷������� \param param1 ��Ϊ��[0,1] \param param2 Ŀ����[0,20) \param param3 �������[0,fabao_num)
	BATTLE_ROLE_OPERATE_TYPE_SET_MOVE_USE_INST_KILL_ITEM,		//!< 19 ʹ�ý����� \param param1 ��Ϊ��[0,1] \param param2 Ŀ����[0,20) \param param3 ˲ɱ�Ĺ���ID�����ڳɹ����ϴ���ս����)

	BATTLE_ROLE_OPERATE_TYPE_SET_MOVE_USE_FAKE_POTION,				//!< 20 ʹ�ü�ҩ \param param1 ��Ϊ��[0,1] \param param2 Ŀ����[0,20) \param param3 ��ҩitem_id 
	BTALLE_ROLE_OPERATE_TYPE_SET_MOVE_PERFORM_HEART_SKILL,			//!< 21 ʩ���ķ�����  \param param1 ��Ϊ��[0,1] \param param2 Ŀ����[0,20)  \param param3 ����ID  \param param4  ���ܵȼ�
	BTALLE_ROLE_OPERATE_TYPE_SET_MOVE_PERFORM_CHANGE_CARD_SKILL,	//!< 22 �ͷű������� \param param1 ��Ϊ��[0,1] \param param2 Ŀ����[0,20)  \param param3 ����ID  \param param4  ���ܵȼ�
	BTALLE_ROLE_OPERATE_TYPE_SET_MOVE_PERFORM_HUAN_SHOU_SKILL,		//!< 23 �ͷ� ���޼��� \param param1 ��Ϊ��[0,1] \param param2 Ŀ����[0,20)  \param param3 ���ܵȼ� 

	BATTLE_ROLE_OPERATE_TYPE_MAX
};

enum BATTLE_SEAL_CARD_TYPE // ���ñ��� 1 2 3
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

struct BattleData_StatisticRole // ͳ�Ƹ���ս������
{
	BattleData_StatisticRole() : total_damage_to_enemy(0LL), total_injure_from_enemy(0LL), total_heal_other(0LL) {}

	long long total_damage_to_enemy;
	long long total_injure_from_enemy;
	long long total_heal_other;
};

struct BattleData_BusinessRole // ����ս��ҵ������
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

struct BattleData_Statistic		// ͳ��ս������
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

	short round_num;			// �غ���
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

	// ͳ��ս���еĹ���
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

struct BattleData_Business		// ҵ�����ݣ�����ԭ�����ظ�gameworld��ҵ����
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

	int monster_group_id;				// ���ڱ�ʶ�����ֹ���
	int monster_gen_id;					// ���ڱ�ʶ����ֻ���� ������boss��Ҫ��
	short courage_challenge_level;		// ��������ս���� (��ʱ����)
	short deamon_cave_beat_waves;		// ħ�ߴ���˶��ٲ�
	int team_idx;						// ��ӵ��������Ҫ
	short guild_boss_seq;				// ��������boss  ����õ����ĸ�boss
	short guild_boss_chapter;			// ��������boss�½�
	int guild_boss_guild_id;			// ����ID
	short guild_boss_monster_remain_hp_percent_list[SIDE_CHARACTER_NUM];	// ��������ÿֻ��ʣ��Ѫ������ֱȣ�

	int param1;
	int param2;
	int param3;

	int world_boss_max_hp;				// ����boss���Ѫ��
	int world_boss_cur_hp;				// ����boss��ǰѪ��
	
	int limit_max_time;					// �������ս��ʱ�䣨�룩
	short limit_max_round;				// �������غ���
	char until_one_side_all_dead;		// ǿ��ս����һ��ȫ����Ž���
	char role_runaway_always_fail;		// ��ɫ���ܱض�ʧ��
	int colosseum_target_uid;			// ������Ŀ��uid
	int cloud_arena_target_uid;			// �ƶ˾���Ŀ��uid

	char can_battle_pause_by_client;	// �ͻ����Ƿ�������ͣս�������ڵȴ�����׶�������ͣ)
	char is_real_mine;					// �Ƿ���İ��׹֣�0:��   1:�ǣ�
	short monster_list_size;
	int monster_id_list[SIDE_CHARACTER_NUM];
	short monster_level_list[SIDE_CHARACTER_NUM];

	int auto_catch_monster_id;			// �Զ�ץ�����ID
	int auto_catch_pet_id;				// �Զ�ץ�����ID
	short available_common_card_num;	// �����տ���
	short available_silver_card_num;	// ����������
	short available_gold_card_num;		// ���ý���
	short pet_catch_type;				// ��׽����
	short succ_rate_common;				// �տ���׽�ɹ���
	short succ_rate_silver;				// ������׽�ɹ���
	short succ_rate_gold;				// �𿨲�׽�ɹ���
	short pr_up_on_fail;				// ʧ�ܺ�ɹ��ʼӳ�
	short must_succ_times;				// ���д���
	ItemID common_card_item_id;			// �տ���ƷID
	ItemID silver_card_item_id;			// ������ƷID
	ItemID gold_card_item_id;			// ����ƷID
	char is_catch_bind_pet;				// ��׽���Ƿ�ض��ǰ󶨵ĳ���
	char is_no_auto;					// �Ƿ�ʼʱ���Զ�
	char is_never_flyaway;				// �Ƿ�����������
	char no_sync_auto_move_setting;		// ��ͬ���Զ���Ϊ�趨��gameworld
	short no_auto_level;				// ������ǵȼ��������������ǿ�Ƹ�Ϊ�ֶ�ģʽ
	char only_sync_pet_auto_move;		// ֻͬ��������Զ���Ϊ
	char is_allow_skip_battle;			// �Ƿ���������ս��

	char robot_run_away_before_start;	
	char is_record_battle;				// �Ƿ񱣴�¼��
	char is_record_first_kill;			// �Ƿ񱣴���ɱ¼��
	char is_never_critical;				// �Ƿ�������������		
	int record_score;					// ��¼���� ����Խ��Խ�л�����¼��
	int expect_score_upper_bound;		// �������ֵ��Ͻ磨�����ָ��ڸ�ֵ���򲻱ر���ս���ļ��ˣ�
	long long record_file_id;			// ��¼���ļ�ID

	char condition_num;								// ��ʱ����ս���������������
	unsigned char condition_list[BATTLE_CONDITION_MAX_NUM];	// ��Ϸ������ս������ʱΪ��Щ���� 
	unsigned char condition_flag;					// ս�����ش���Ϸ���õ���ʱ�ļ����������(����������)

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

	short monster_attr_weaken_percent;	// ����ȫ���������ٷֱ�
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
	BATTLE_INJURE_TYPE_PHYSIC = 0,  //!< �����˺�
	BATTLE_INJURE_TYPE_MAGIC = 1,	//!< ħ���˺�
	BATTLE_INJURE_TYPE_BOTH = 2,	//!< ������ħ���˺�
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

	short is_auto;			// �Ƿ��Զ��� 0���� 1���ǡ�
	short role_stand_row;   // 1:ǰ��  0:����
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

	// ����Զ�����
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

	int weapon_sp_list[BATTLE_WEAPON_EFFECT_NUM];				// ������Ч (��Ϊ���Ի����������԰�������Ч�ֿ��ţ�
	int other_sp_list[BATTLE_OTHER_EQUIP_EFFECT_NUM];			// ������Ч
	BattleFabao fabao_list[BATTLE_FABAO_NUM_MAX];

	Appearance appearance;

	int wear_title_id;											// �����ĳƺ�

	short guild_buff_list[BATTLE_GUILD_BUFF_NUM];				// ����BUFF_ID
	int top_level;												// �۷�ȼ�
	
	char reserve_ch;
	char is_robot;												// �Ƿ������ [1:�� 0:��]

	char heart_skill_is_vaild;									// �ķ������Ƿ���Ч 0:ʧЧ 1:��Ч
	char heart_skill_count;		
	BattleHeartSkill heart_skill_list[BATTLE_HEART_SKILL_NUM];	// �ķ�����

	int jieyi_num;
	int jieyi_uid_list[BATTLE_JIEYI_MAX_NUM];					// �������Uid�б�

	int jieyi_skill_num;
	int jieyi_skill_list[BATTLE_JIEYI_SKILL_MAX_NUM];			// ������Ч�б�

	UInt16 soaring_skill_id;									// ��������ID
	short soaring_skill_lv;										// �������ܵȼ�

	int qingyuan_target_uid;
	int qingyuan_skill_num;
	int qingyuan_skill_list[BATTLE_QINGYUAN_SKILL_MAX_NUM];		// ��Ե��Ч�б�

	int career_suppress_value;									// ְҵѹ��
	int change_card_skill_id;									// ��������ID				
	int change_card_seq;										// ��������							

	int role_extra_capability;	// ��ɫ��ս����������Ķ���ս�������������ϻ�õ�ս���������ԣ�

	short hallow_num;
	short fight_hallow_idx;
	BattleHallowInfo hallow_list[BATTLE_HALLOW_NUM];

	BattleHuanShouSkillInfo huanshou_skill_info;
};

////////////////////////////////////////////////////// ս��ͳ�� /////////////////////////////////////////////////////

enum BATTLE_STAT_TYPE
{
	BATTLE_STAT_TYPE_INVALID = 0,	
	BATTLE_STAT_TYPE_MONSTER_REMAIN_HP_PERCENT = 1,		//ͳ�ƹ����ʣ��Ѫ���ٷֱ�  p1 ʣ��Ѫ���ٷֱ�  p2 monster_id
	BATTLE_STAT_TYPE_MONSTER_REINFORCE_TIMES = 2,		//���﷽��Ԯ�˶��ٴΣ�����ħ�ߣ�
	BATTLE_STAT_TYPE_MAX,
};


///////////////////////////////////////// ����ս��ҵ������ //////////////////////////////////////////////
enum BATTLE_BUSINESS_DATA_TYPE
{
	BATTLE_BUSINESS_DATA_TYPE_INVALID = 0,
	BATTLE_BUSINESS_DATA_TYPE_WORLD_BOSS = 1,
	BATTLE_BUSINESS_DATA_TYPE_FB = 2,
	BATTLE_BUSINESS_DATA_TYPE_SQUAD_FIGHT = 3,
	BATTLE_BUSINESS_DATA_TYPE_PLATFORM_FIGHT = 4,
	BATTLE_BUSINESS_DATA_TYPE_COURAGE_CHALLENGE = 5,	// ��ħ֮��
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
		int pet_origin_index;	// ��gameworld�е�index
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

/////////////////////////////////////////////// ����ս��ģʽ-����Ѱ������/////////////////////////////////////

const static int MAX_JOY_SEEKS_FAVORS_MAP_LONG = 5;//��
const static int MAX_PLAYER_COUNT = 2;				//���2��

enum BATTLE_JOY_SEEKS_FAVORS_OPERATE_TYPE
{
	BATTLE_JOY_SEEKS_FAVORS_OPERATE_TYPE_INVALID = 0,

	BATTLE_JOY_SEEKS_FAVORS_OPERATE_TYPE_OPEN = 1,  //Ѱ��һ��λ�ã�param1 = open_map	param2 = x	param3 = y
	BATTLE_JOY_SEEKS_FAVORS_OPERATE_TYPE_ITEM = 2,	//ʹ�õ���		param1 = open_map	param2 = x	param3 = y	param4 = face_pos	param5= item_seq
	BATTLE_JOY_SEEKS_FAVORS_OPERATE_TYPE_SET = 3,	//����			param1 = open_map	param2 = x	param3 = y	param4 = face_pos	param5 = joy_type
	BATTLE_JOY_SEEKS_FAVORS_OPERATE_TYPE_SHOW = 4,	//չʾ			�޲���

	BATTLE_JOY_SEEKS_FAVORS_OPERATE_TYPE_MAX,
};

enum JOY_SEEKS_FAVORS_TYPE//��������
{
	JOY_SEEKS_FAVORS_TYPE_NO = 0,		//��
	JOY_SEEKS_FAVORS_TYPE_LITTLE = 1,	//С
	JOY_SEEKS_FAVORS_TYPE_MIDDLE = 2,	//��
	JOY_SEEKS_FAVORS_TYPE_BIG = 3,		//��
	JOY_SEEKS_FAVORS_TYPE_MAX,
};

enum JOY_SEEKS_FAVORS_INDEX		//���ﲿ��
{
	JOY_SEEKS_FAVORS_INDEX_NO = 0,		//��
	JOY_SEEKS_FAVORS_INDEX_LITTLE = 1,	//С -- ��������Ϊ��ռ1��
	JOY_SEEKS_FAVORS_INDEX_MIDDLE = 2,	//�� -- ��������Ϊ��ռ2�񣬷�2����
	JOY_SEEKS_FAVORS_INDEX_BIG = 3,		//�� -- ��������Ϊ��ռ3�񣬷�3����
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
����С�ͳ���ֻռ1�����Ը��������ⰲ��
���ͳ���ռ2�������ж�����λ�����׸�����λ���첿�ֲ��ɳ�Խ�߽磬��������0����ͷ�����ڲ�λ�������ǣ�0��0����0��1����0��2����0��3����0��4��
���ͳ���ռ3�磬�������ͳ��ﲻ�������������ͳ���Ҳ��������ͬʱ��������0����ͷ��Ҳ�������ǣ�1��0����1��1����1��2����1��3����1��4��
*/


enum JOY_SEEKS_FAVORS_FACE_POS		//����
{
	JOY_SEEKS_FAVORS_FACE_POS_TOP = 0,		//��
	JOY_SEEKS_FAVORS_FACE_POS_RIGHT = 1,	//��
	JOY_SEEKS_FAVORS_FACE_POS_DOWN = 2,		//��
	JOY_SEEKS_FAVORS_FACE_POS_LEFT = 3,		//��
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

const static int MAX_JOY_SEEKS_FAVORS_MAP_RULE_MIDDLE_X_END[JOY_SEEKS_FAVORS_FACE_POS_MAX] = { -1,0,1,0 };//���ݹ���֪��������������ͳ���ĩ��X��ϵ
const static int MAX_JOY_SEEKS_FAVORS_MAP_RULE_MIDDLE_Y_END[JOY_SEEKS_FAVORS_FACE_POS_MAX] = { 0,1,0,-1 };//���ݹ���֪��������������ͳ���ĩ��Y��ϵ

const static int MAX_JOY_SEEKS_FAVORS_MAP_RULE_BIG_X_END[JOY_SEEKS_FAVORS_FACE_POS_MAX] = { -2,0,2,0 };//���ݹ���֪�������������ͳ���ĩ��X��ϵ
const static int MAX_JOY_SEEKS_FAVORS_MAP_RULE_BIG_Y_END[JOY_SEEKS_FAVORS_FACE_POS_MAX] = { 0,2,0,-2 };//���ݹ���֪�������������ͳ���ĩ��Y��ϵ

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
	short				m_avatar_type;									// ������������
	char				find_pet;										// ���߼�
	char				find_item;										// ������Ʒ
	long long			m_avatar_timestamp;								// ������ϴ�ͷ��
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
	ANIMATION_SPEED_NORMAL = 0,	//!< 0 Ĭ���ٶ� ��1���٣�
	ANIMATION_SPEED_DOUBLE = 1,	//!< 1 ˫����
	ANIMATION_SPEED_HALF = 2,	//!< 2 �뱶��
	ANIMATION_SPEED_MAX = 3,
};

int GetPercentNumByAttrType(int attr_type);
bool IsRatioAttrType(int attr_type); // �ж����������Ƿ��ñ�������
BattleAttr GetAttrChangeNum(int attr_type, BattleAttr cur_attr, int percent);


enum FAKE_POTION_TYPE
{
	FAKE_POTION_TYPE_HP = 0, // ��Ѫҩ   param1 ��Ѫ�ٷֱ�  param2 �����ٷֱ�
	FAKE_POTION_TYPE_MP = 1, // ����ҩ   param1 �����ٷֱ�
	FAKE_POTION_TYPE_REALIVE = 2, // ����ҩ  param1 ��Ѫ�ٷֱ�
	FAKE_POTION_TYPE_MAX
};

enum BATTLE_LINK_TYPE
{
	BATTLE_LINK_TYPE_DROP_HP_TOGETHER = 0,   // �������� һ���Ѫ
	BATTLE_LINK_TYPE_HEART_DEVIL_LINK = 1,	 // ��ħ����
	BATTLE_LINK_TYPE_HP_SYMBIOSIS = 2,		 // ��������
	BATTLE_LINK_TYPE_MAX,
};

enum BATTLE_PURIFY_REASON
{
	BATTLE_PURIFY_SKILL = 0,			// ���ܽ��
	BATTLE_PURIFY_FABAO = 1,			// �������
	BATTLE_PURIFY_MEDICINE = 2,			// ҩƷ���
	BATTLE_PURIFY_SPECIAL_EFFECT = 3,	// ��Ч���
	BATTLE_PURIFY_MAX
};

enum BATTLE_EXTRA_PERFORM_SKILL_TARGET_TYPE
{
	BATTLE_EXTRA_PERFORM_SKILL_TARGET_ORIGINAL = 0, // ��ԭĿ��Ϊ��Ŀ��
	BATTLE_EXTRA_PERFORM_SKILL_TARGET_RANDOM_ON_ORIGINAL_DEAD = 1, // ��ԭĿ������ʱ���
	BATTLE_EXTRA_PERFORM_SKILL_TARGET_RANDOM_EXCEPT_ORIGIN = 2,	// ���һ����Ŀ��(���������ԭĿ��)
	BATTLE_EXTRA_PERFORM_SKILL_TARGET_MAX,
};

enum BATTLE_SEED_TYPE
{
	BATTLE_SEED_TYPE_INVALID = 0,
	BATTLE_SEED_TYPE_WATER = 1,				//!< ˮ
	BATTLE_SEED_TYPE_FIRE = 2,				//!< ��
	BATTLE_SEED_TYPE_WIND = 3,				//!< ��
	BATTLE_SEED_TYPE_EARTH = 4,				//!< ��
	BATTLE_SEED_TYPE_MAX
};

#endif
