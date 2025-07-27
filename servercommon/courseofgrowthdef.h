#ifndef __COURSE_OF_GROWTH_DEF_HPP__
#define __COURSE_OF_GROWTH_DEF_HPP__

#include "servercommon/servercommon.h"
#include "bitmapfunction.hpp"

#pragma pack(push, 4)

static const int GROWTH_TASK_NUM_PER_STAR = 9;		// �ɳ�֮· - ÿ�Ǽ���������
static const int GROWTH_KEY_MULT = 10000;			// ��ֵ����
UNSTD_STATIC_CHECK(GROWTH_TASK_NUM_PER_STAR > BIT_COUNT_OF_BYTE && GROWTH_TASK_NUM_PER_STAR <= BIT_COUNT_OF_BYTE * 2)

static const int GROWTH_PROG_BEGINNING = 1;
static const int GROWTH_STAR_BEGINNING = 1;

static const int MAX_COG_RECORD_NUM = 511;
static const int MAX_COG_RECORD_LEN = sizeof(short) + (sizeof(int) + sizeof(int)) * MAX_COG_RECORD_NUM;

static const int MAX_COURSE_ENTRY_LIMIT_NUM = 1504;		// ���̱�ǩ��Ŀ����
static const int COURSE_ENTRY_KEY_MULT = 1000000;		// ���̼�ֵ����
static const int MAX_COURSE_LEVEL_NUM = 64;				// ������ߵȼ�
static const int MAX_CHANNEL_CHAT_TODAY_COUNT = 5;		// Ƶ������ÿ������ÿ��������

enum CourseOfGrowthCondition
{
	COG_CON_WIN_FIGHT_TIMES_ACC = 0,			// �� �ۼ�ʤ������	|	 /	  | ָ������		| ��
	COG_CON_REACHS_PRESTIGE_NUM = 1,			// �� ����������ֵ	| ָ������ | ָ������		| ��
	COG_CON_PASS_CRYSTAL_ECTYPE = 2,			// �� ͨ��ˮ������	|	 /	  | ָ������		| ��
	COG_CON_OBTAIN_COIN_NUMBERS = 3,			// �� ��ȡͭ������	|	 /	  | ָ������		| ��
	COG_CON_PET_TRAPS_CREATURES = 4,			// �� �ɹ���ӡ����	|	 /	  | ָ������		| ��
	COG_CON_DEFEAT_RACE_MONSTER = 5,			// �� ������������	| �������� | ָ������		| ��
	COG_CON_REACHS_FB_TIER_NUMB = 6,			// �� ���︱������	| �������� | Ŀ�����		| ��
	COG_CON_OBTAIN_TITLE_NUMBER = 7,			// �� ��óƺ�����	|	 /	  | ָ������		| ��
	COG_CON_DEFEAT_BOSS_MONSTER = 8,			// �� ����BOSS��	| BOSS���� | ָ������		| ��
	COG_CON_STUDY_BATTLE_SKILLS = 9,			// �� ϰ��ս������	|	 /	  | ָ������		| ��
	COG_CON_SKILLS_REACHS_LEVEL = 10,			// �� ���ܵȼ�����	| Ŀ��ȼ� | ָ������		| 
	COG_CON_PUT_ON_EQUIPM_TIMES = 11,			// �� ����װ������	|	 /	  | ָ������		| ��
	COG_CON_PET_ALLOT_ATTRPOINT = 12,			// �� ����������	|	 /	  | ָ������		| ��
	COG_CON_PET_OBTAIN_RACE_NUM = 13,			// �� ����������	| �������� | ָ������		| ��
	COG_CON_PET_NUM_REACH_LEVEL = 14,			// �� ����ȼ�����	| Ŀ��ȼ� | ָ������		| 
	COG_CON_PET_INTENSIFY_TIMES = 15,			// �� ����ǿ������	|	 /	  | ָ������		| ��
	COG_CON_PET_REPLACE_TALENTS = 16,			// �� �����滻�츳	|	 /	  | ָ������		| ��
	COG_CON_ACCOMPLISH_ACTIVITY = 17,			// �� ���ָ���	| ����� | ָ������		| ��
	COG_CON_ACTIVATE_COLLECTION = 18,			// �� �ۼƼ���ͼ��	|	 /	  | ָ������		| ��
	COG_CON_USE_APPOINTED_ITEMS = 19,			// �� ʹ�����͵���	| �������� | ָ������		| ��
	COG_CON_USE_NOREXITEM_TYPES = 20,			// �� ʹ�����ĵ���	| �������� | ָ������		| ��
	COG_CON_REACHS_APPOINTED_LV = 21,			// �� ����ָ���ȼ�	| Ŀ��ȼ�				| 
	COG_CON_APPOINTED_TASK_DONE = 22,			// �� ���ָ������	| ������				| 
	COG_CON_CARRY_OUT_TYPE_TASK = 23,			// �� �����������	| �������� | ָ������		| �� p1(COG_TASK_TYPE_MAX)
	COG_CON_WORKSHOP_CRAFT_ITEM = 24,			// �� ����������Ʒ	| �������� | ָ������		| ��
	COG_CON_GATHER_TIMES_ACCUMU = 25,			// �� �ۼƲɼ�����	| �ɼ����� | ָ������		| ��
	COG_CON_TRADEMARKET_PUTAWAY = 26,			// �� �����ϼܵ���	|	 /	  | ָ������		| ��
	COG_CON_TRADEMARKET_DEALING = 27,			// �� �������״���	|	 /	  | ָ������		| ��
	COG_CON_FINISHED_TASK_CHAIN = 28,			// �� �����������	|	 /	  | ָ������		| ��
	COG_CON_EXECUTES_SEAL_ORDER = 29,			// �� ִ�з���ָ��	|	 /	  | ָ������		| ��
	COG_CON_CONSUME_DOUBLE_EXP  = 30,			// �� ����˫������	|	 /	  | ָ������		| ��
	COG_CON_PET_SCOUR_TALENTS   = 31,			// �� ����ϴ������	|	 /	  | ָ������		| ��
	COG_CON_PARTNER_UP_LEVEL    = 32,           // �� �ۼƻ�����| ������ | ָ������		| ��
	COG_CON_PET_PRACTICE_LEVEL	= 33,           // �� ���������ȼ�| �����ȼ� |  		    | ��
	COG_CON_BOUNTY_TASK_DONE	= 34,           // �� �ͽ��������|    /    | ָ������		| ��
	COG_CON_MIYI_KILL_LAYER_NUM = 35,			// �� Զ����������| Զ������ | ��ͨ����		| ��
	COG_CON_KILL_MONTER_GROUP   = 36,			// �� ��ɱ������  | ������ID | �Ƿ��ɱ    | ��
	COG_CON_PET_MONOPOLY_LEVEL  = 37,			// �� ���޵�����  |    /    |  ָ������   | ��
	COG_CON_TRAIN_SKILL_LEVEL	= 38,			// �� ������������| ���ܵȼ� |  ָ������   | ��
	COG_CON_LUN_HUI_WANG_CHUAN	= 39,			// �� ͨ���ֻ�����|    /    |  ָ������   | ��
	COG_CON_SHAN_HAI_XIANG_YAO	= 40,			// �� ͨ��ɽ������|    /    |  ָ������   | ��
	COG_CON_REALM_BREAK_LEVEL	= 41,			// �� ��ɫ����ȼ�| ����ȼ� |			   | ��
	COG_CON_PET_INTENSIFY_LEVEL = 42,			// �� ����ǿ���ȼ�	| ����Ʒ�� |  ָ���ȼ�   | ��	 param1(0:��ʾû��Ʒ������)
	COG_CON_EQUIPMENT_SMELT		= 43,			// �� ��ɫװ������|    /    |	 ָ������   | ��
	COG_CON_CRYSTAL_FB_XI_YOU	= 44,			// �� ��Ӣ����ͨ��| ָ���ؿ� |		/	   | ��
	COG_CON_PASS_CRYSTAL_XI_YOU = 45,			// �� ��Ӣ����ͨ��|    /    |	 ָ������   | ��
	COG_CON_REAML_BREAK_FIGHT	= 46,			// �� ��������֮��| ָ���ؿ� |		/	   | ��
	COG_CON_REACH_CAPABILITY	= 47,			// �� �ﵽָ������| �������� |	 ָ����ֵ   | ��
	COG_CON_GET_QUALITY_PET     = 48,			// �� ���Ʒ�ʳ���| ����Ʒ�� |	 ָ������   | �� param1(0:��ʾû��Ʒ������)
	COG_CON_SCOUR_TALENTS_MUT	= 49,			// �� �ۼ�ϴ������| ����Ʒ�� |	 ָ������   | �� param1(0:��ʾû��Ʒ������)
	COG_CON_HAS_FA_BAO_COUNT	= 50,			// �� ӵ�з�������| ����Ʒ�� |	 ָ������   | �� param1(0:��ʾû��Ʒ������)
	COG_CON_HAS_ACTIVE_FACE		= 51,			// �� ӵ����ֵ���| ������� |	 ָ������   | �� param1(-1:��ʾû��Ʒ������)
	COG_CON_HAS_SURFACE			= 52,			// �� ӵ��ʱװ����| ������� |	 ָ������   | �� param1(0:��ʾû��Ʒ������)
	COG_CON_GET_MONEY			= 53,			// �� �ۼƻ�û���| �������� |	 ָ������   | ��
	COG_CON_FIRST_ADD_GUILD     = 54,			// �� �״μ������|     /   |	 ָ������   | ��
	COG_CON_GUILD_TEAM_FIGHT	= 55,			// �� ��Ա���ս��|     /   |	 ָ������   | ��
	COG_CON_PARTICIPATE_ACT		= 56,			// �� ������ʱ�| ����� |	 ָ������   | ��
	COG_CON_ACTIVITY_FIGHT_WIN	= 57,			// �� �ս��ʤ��| ����� |	 ָ������   | ��
	COG_CON_GUILD_FIGHT_RANK	= 58,			// �� �����Ҷ�����|   ����   |	    /      | ��
	COG_CON_GUILD_HONOR_RANK	= 59,			// �� ��������ս  |   ����   |	    /      | ��
	COG_CON_FETCH_GUILD_HONGBAO = 60,			// �� ��ȡ������|    /     |��ָ������  | 
	COG_CON_GUILD_ANSWER		= 61,			// �� ��Լ������|    /     |��ָ������  | 
	COG_CON_CHANNEL_CHAT		= 62,			// �� �ۼ�Ƶ������| Ƶ������  |��ָ������  | 
	COG_CON_FETCH_FRIEND_GIFT	= 63,			// �� ��ȡ��������|	 /	   |��ָ������  | ��
	COG_CON_HAS_FRIEND_COUNT	= 64,			// �� ӵ�к�������| ���ܶ�����|��ָ������  | 
	COG_CON_JIE_YI_DAY_NUM		= 65,			// �� ������������|    /     |��ָ������  | ��
	COG_CON_USE_HIGH_TREASURE   = 66,			// �� �ۼ��ڲر�ͼ| ��ͼ����  |��ָ������  | ��
	COG_CON_WIN_CHIEF_ELECTION  = 67,			// �� ���������ϯ|	 /	   |��ָ������  | ��
	COG_CON_CLOUD_ARENA_SCORE	= 68,			// �� 񷶥֮����λ|	 /	   |��ָ������  | ��
	COG_CON_APPRENTICE_COUNT    = 69,			// �� ���ͽ������|    /     |��ָ������  | 
	COG_CON_APPRENTICE_DAY_NUM	= 70,			// �� ��Ұ�ʦ����|    /     |��ָ������  | ��
	COG_CON_YIN_YANG_EYE_REWARD = 71,			// �� �����ۻ���|    /     |��ָ������  | ��
	COG_CON_USE_CRYSTAL_ENERGY	= 72,			// �� �ۼ���������|    /     |��ָ������  | ��
	COG_CON_SHAN_HAI_BOSS		= 73,			// �� �ۼƻ���ɽ��| �Ѷ�����  |  ָ������  | ��
	COG_CON_HUAN_JIE_ZHAN_CHANG = 74,			// �� �ý�ս����λ|    /     |  ָ������  | ��
	COG_CON_ANECDOTE_TASK		= 75,			// �� �����������| ��������  |    /      | 
	COG_CON_CHIVALROUS			= 76,			// �� �ۻ���ȡ����|    /     |  ָ������  | 

	COG_CON_TYPE_MAX
};

enum COG_CON_REACH_CAPABILITY_TYPE
{
	COG_CON_REACH_CAPABILITY_TYPE_COUNT = 0,	//������
	COG_CON_REACH_CAPABILITY_TYPE_ROLE = 1,		//��ɫ����

	COG_CON_REACH_CAPABILITY_TYPE_MAX
};

enum COGBossType
{
	COG_BOSS_WORLD = 0,		// ����BOSS
	COG_BOSS_WILD = 1,		// Ұ��BOSS

	COG_BOSS_TYPE_MAX
};

enum COG_GATHER_TYPE
{
	COG_GATHER_TYPE_MINING = 0,			// �ڿ�
	COG_GATHER_TYPE_LUMBER = 1,			// ��ľ
	COG_GATHER_TYPE_FLOWER = 2,			// �ɻ�
	COG_GATHER_TYPE_COUNT  = 3,			// �ڿ�+��ľ+�ɻ�

	COG_GATHER_TYPE_MAX
};

enum COGAppointedActivity
{
	COG_ACT_TYPE_WAN_SHI_TONG = 0,			// ����ͨ
	COG_ACT_TYPE_WISHING_POOL = 1,			// ��Ը��
	COG_ACT_TYPE_ESCORT_CARGO = 2,			// ����
	COG_ACT_TYPE_ZHENZHO_TASK = 3,			// ���е��о�(�����ħ)
	// COG_ACT_TYPE_MONST_VALLEY = 4,			// �ַ�ħ��(������)[�Ѿ��������ڱ����4 = ����]
	COG_ACT_TYPE_HUA_BEN = 4,				// ����
	COG_ACT_TYPE_MAX
};

enum COGAppointedTaskType
{
	COG_TASK_TYPE_PROF_TASK = 0,		// �������� / ְҵ����
	COG_TASK_TYPE_PRESTIGE_TASK = 1,	// ��������
	COG_TASK_TYPE_ADVENTURES_TASK = 2,	// ��������

	COG_TASK_TYPE_MAX
};

enum COGWorkshopMakesType		// ��������
{
	COG_WORKSHOP_MAKE_EQUIP = 0,		// ����װ��
	COG_WORKSHOP_MAKE_POTION = 1,		// ����ҩ��
	COG_WORKSHOP_MAKE_BOOKS = 2,		// �ϳɼ�����
	COG_WORKSHOP_MAKE_CARD = 3,			// �ϳɷ�����

	COG_WORKSHOP_MAKE_TYPE_MAX
};

struct CourseOfGrowthParam
{
	CourseOfGrowthParam() { this->Reset(); }

	void Reset()
	{
		cur_prog = (UInt16)GROWTH_PROG_BEGINNING;
		cur_star = (UInt16)GROWTH_STAR_BEGINNING;
		cur_reward_prog = (UInt16)GROWTH_PROG_BEGINNING;
		cur_reward_star = (UInt16)GROWTH_STAR_BEGINNING;

		award_fetch_flag.Reset();
		finish_notify_flag.Reset();

		memset(reserve_ll, 0, sizeof(reserve_ll));
		is_give_base_pass_times_flag = 0;
		reserve_ch = 0;
		reserve_sh = 0;
	}

	UInt16 cur_prog;
	UInt16 cur_star;
	UInt16 cur_reward_prog;
	UInt16 cur_reward_star;

	BitMap<GROWTH_TASK_NUM_PER_STAR> award_fetch_flag;
	BitMap<GROWTH_TASK_NUM_PER_STAR> finish_notify_flag;

	long long reserve_ll[3];
	char is_give_base_pass_times_flag;				// ����ҷ��Ż������ݱ�ʶ
	char reserve_ch;
	short reserve_sh;
};
typedef char CourseOfGrowthParamHex[sizeof(CourseOfGrowthParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(CourseOfGrowthParamHex) < 256);

struct RoleCourseParam
{
	RoleCourseParam() { this->Reset(); }

	void Reset()
	{
		memset(reserve_ll, 0, sizeof(reserve_ll));

		course_fraction_count = 0;
		finish_record_flag.Reset();
		level_reward_fetch_flag.Reset();
		memset(course_entry_version_list, 0, sizeof(course_entry_version_list));
		memset(course_level_version_list, 0, sizeof(course_level_version_list));
	}

	long long reserve_ll[3];

	int course_fraction_count;												// �����̵�
	BitMap<MAX_COURSE_ENTRY_LIMIT_NUM> finish_record_flag;					// ������Ŀ��ɼ�¼
	BitMap<MAX_COURSE_LEVEL_NUM> level_reward_fetch_flag;					// ���̵ȼ�������ȡ��¼
	unsigned char course_entry_version_list[MAX_COURSE_ENTRY_LIMIT_NUM];	// ������Ŀ��ż�¼
	unsigned char course_level_version_list[MAX_COURSE_LEVEL_NUM];			// �ȼ�������ȡ���
};
typedef char RoleCourseParamHex[sizeof(RoleCourseParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(RoleCourseParamHex) < 4096);

#pragma pack(pop)
#endif  // __COURSE_OF_GROWTH_DEF_HPP__