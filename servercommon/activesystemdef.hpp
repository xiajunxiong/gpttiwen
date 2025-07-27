#ifndef __ACTIVE_SYSTEM_DEF_HPP__
#define __ACTIVE_SYSTEM_DEF_HPP__

#include "servercommon/servercommon.h"
#include "bitmapfunction.hpp"

#pragma pack(push, 4)

static const int ACTIVE_SYSTEM_LEVEL_GROUP_MAX_NUM = 5;
static const int ACTIVE_SYSTEM_REWARD_MAX_NUM = 2;
static const int ACTIVE_SYSTEM_REWARD_ITEM_MAX_NUM = 10;

enum ACTIVE_SYSTEM_TYPE
{
	ACTIVE_SYSTEM_TYPE_BEGIN = 0,	
	ACTIVE_SYSTEM_TYPE_SCHOOL_TASK, 				// 1  ѧԺ������������
	ACTIVE_SYSTEM_TYPE_ZHEN_ZHONG,					// 2  ���е��о�(�����ħ)
	ACTIVE_SYSTEM_TYPE_ARRESTING_THIEVES,			// 3  ׷������
	ACTIVE_SYSTEM_TYPE_WST_QUESTION,				// 4  ����ͨ�Ŀ��� / �����ӵĿ���
	ACTIVE_SYSTEM_TYPE_TREVI_FOUNTAIN,				// 5  ��������Ը�� / ��Ը��
	ACTIVE_SYSTEM_TYPE_DEMON_CAVE,					// 6  ħ�ߣ�������)
	ACTIVE_SYSTEM_TYPE_ESCORT,						// 7  ����
	ACTIVE_SYSTEM_TYPE_BOUNTY_TASK,					// 8  �ͽ�����
	ACTIVE_SYSTEM_TYPE_TRANSACTION_TASK,			// 9  �������� - ˿��֮·
	ACTIVE_SYSTEM_TYPE_FANTASY_FB,					// 10 ����þ�
	ACTIVE_SYSTEM_TYPE_JINGJICHANG,					// 11 ���뾺����
	ACTIVE_SYSTEM_TYPE_COURAGE_CHALLENGE,			// 12 ������ħ֮��
	ACTIVE_SYSTEM_TYPE_MIYI,						// 13 Զ��ħڣ(����֮��)
	ACTIVE_SYSTEM_TYPE_ENERGY,						// 14 ��������
	ACTIVE_SYSTEM_TYPE_CLOUD_ARENA,					// 15 �ƶ˾���
	ACTIVE_SYSTEM_TYPE_LUNHUIWANGCHUAN,				// 16 �ֻ�����
	ACTIVE_SYSTEM_TYPE_DREAM_NOTES_CHALLENGE,		// 17 ��Ԩ��¼
	ACTIVE_SYSTEM_TYPE_SHAN_HAI_BOSS,				// 18 ɽ������
	ACTIVE_SYSTEM_TYPE_CRYSTAL_XIYOU,				// 19 ��Ӣ����
	ACTIVE_SYSTEM_TYPE_NETHER_WORLD_PURGATORY,		// 20 ��ڤ����
	ACTIVE_SYSTEM_TYPE_PRESTIGE_TASK,				// 21 ��������
	ACTIVE_SYSTEM_TYPE_BIG_DIPPER,					// 22 ��������
	ACTIVE_SYSTEM_TYPE_WAN_LING_FB,					// 23 ���黰��
	ACTIVE_SYSTEM_TYPE_TREASURE_TASK,				// 24 �����ر�
	ACTIVE_SYSTEM_TYPE_FLY_UP_TASK,					// 25 ��������

	ACTIVE_SYSTEM_TYPE_MAX,
};

static const int ACTIVE_SYSTEM_TYPE_MAX_IN_DB = 32;	// ����Ծ����������ʱ��ActiveSystemParam�е����ݽ����ָ���������Ҫ������������չ
UNSTD_STATIC_CHECK(26 == ACTIVE_SYSTEM_TYPE_MAX);
UNSTD_STATIC_CHECK(ACTIVE_SYSTEM_TYPE_MAX <= ACTIVE_SYSTEM_TYPE_MAX_IN_DB);

enum ACTIVE_SYSTEM_IS_EVERY_TIME
{
	ACTIVE_SYSTEM_IS_EVERY_TIME_ALL_FINISH = 0,		// ��Ҫ������д����ŷ��Ż���
	ACTIVE_SYSTEM_IS_EVERY_TIME_SINGLE_FINISH,	    // ������ɼ��ɷ��Ż���
};

enum ACTIVITY_SYSTEM_SPECIAL_TYPE
{
	ACTIVITY_SYSTEM_SPECIAL_TYPE_BEGIN = 0,
	ACTIVITY_SYSTEM_SPECIAL_TYPE_WILD_BOSS = 1,		// Ұ������

	ACTIVITY_SYSTEM_SPECIAL_TYPE_MAX
};
static const int ACTIVE_SYSTEM_SPECIAL_TYPE_MAX_IN_DB = 32;	// ����Ծ����������ʱ��ActiveSystemParam�е����ݽ����ָ���������Ҫ������������չ
UNSTD_STATIC_CHECK(ACTIVITY_SYSTEM_SPECIAL_TYPE_MAX <= ACTIVE_SYSTEM_TYPE_MAX_IN_DB);

struct ActiveSystemParam
{
	ActiveSystemParam(){ this->Reset(); }
	void Reset()
	{
		level = 0;
		integral_active = 0;
		active_flag.Reset();
		type_flag.Reset();
		memset(progress, 0, sizeof(progress));
		type_reward_fetch_flag.Reset();
		add_guess_coin_active_num = 0;
		special_active_reward_flag.Reset();
		memset(reserve_ll, 0, sizeof(reserve_ll));
		memset(can_find_count_list, 0, sizeof(can_find_count_list));
	}

	void DayReset()
	{
		level = 0;
		integral_active = 0;
		active_flag.Reset();
		type_flag.Reset();
		memset(progress, 0, sizeof(progress));
		type_reward_fetch_flag.Reset();
		add_guess_coin_active_num = 0;
	}

	short level;											// ��ҵ����һ�ε�½�ĵȼ�
	short integral_active;									// ��ҵ�ǰ�Ļ�Ծ��
	BitMap<ACTIVE_SYSTEM_LEVEL_GROUP_MAX_NUM> active_flag;	// ��Ծ��ȡ��־
	BitMap<ACTIVE_SYSTEM_TYPE_MAX_IN_DB> type_flag;				// ��Ӧ�����ͻ�Ծ��ȫ��ȡ���л���ʶ
	BitMap<ACTIVE_SYSTEM_TYPE_MAX_IN_DB> type_reward_fetch_flag;	// �������ͻ�ȡ���⽱��
	int progress[ACTIVE_SYSTEM_TYPE_MAX_IN_DB];					// �������͵Ľ���
	int add_guess_coin_active_num;

	BitMap<ACTIVE_SYSTEM_SPECIAL_TYPE_MAX_IN_DB> special_active_reward_flag;	//�����Ծ���ͽ�����ȡ��ʶ

	long long reserve_ll[3];
	short can_find_count_list[ACTIVE_SYSTEM_TYPE_MAX_IN_DB];		//��Ծ���һش��� 
};

typedef char ActiveSystemParamHex[sizeof(ActiveSystemParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(ActiveSystemParamHex) < 512);

//��Ծ�� ��������
enum ACTIVE_DEGREE_TYPE
{
	ACTIVE_DEGREE_TYPE_BEGIN = -1,

	ACTIVE_DEGREE_TYPE_ZHEN_ZHONG,				// 0  ���е��о�(�����ħ)
	ACTIVE_DEGREE_TYPE_BOUNTY_TASK, 			// 1  �ͽ�������������
	ACTIVE_DEGREE_TYPE_ENERGY,					// 2  ��������
	ACTIVE_DEGREE_TYPE_WORLD_BOSS,				// 3  ħ������
	ACTIVE_DEGREE_TYPE_WORLD_BOSS_2,			// 4  ������Ϯ
	ACTIVE_DEGREE_TYPE_GUILD_ANSWER,			// 5  �������
	ACTIVE_DEGREE_TYPE_GUILD_FIGHT,				// 6  ������Ҷ�
	ACTIVE_DEGREE_TYPE_TEAM_FIGHT,				// 7  �����Կ���
	ACTIVE_DEGREE_TYPE_MAZE_RACER,				// 8  �Թ�����
	ACTIVE_DEGREE_TYPE_ONLY_FIGHT,				// 9  ��һ�а�
	ACTIVE_DEGREE_TYPE_GUILD_HONOR_BATTLE,		// 10 ��������ս
	ACTIVE_DEGREE_TYPE_CHIEF_ELECTION,			// 11 ��ϯ��ѡ
	ACTIVE_DEGREE_TYPE_HUNDRED_GHOST,			// 12 �ٹ�ҹ��
	ACTIVE_DEGREE_TYPE_BRAVE_GROUND,			// 13 �´��ع�
	ACTIVE_DEGREE_TYPE_SILK_ROADS,				// 14 ˿��֮·
	ACTIVE_DEGREE_TYPE_RELIC_LOOTING,			// 15 �ż��ᱦ

	ACTIVE_DEGREE_TYPE_MAX,
};


static const int MAX_ACTIVE_DEGREE_WEEK_REWARD_NUM = 10;
static const int MAX_ACTIVE_DEGREE_TASK_NUM = 30;

// ��Ծת�� ��������
enum ACTIVE_TURNTABLE_TYPE
{
	ACTIVE_TURNTABLE_TYP_BEGIN = 0,
	ACTIVE_TURNTABLE_TYPE_JOIN_ACTIVITY,		// 1 ������ʱ� ����:�ID
	ACTIVE_TURNTABLE_TYPE_WEEK_ACTIVE,			// 2 �ܻ�Ծ�ﵽһ��ֵ ����:�ܻ�Ծֵ

	ACTIVE_TURNTABLE_TYPE_MAX,
};


struct ActiveDegreeBaseParam
{
	ActiveDegreeBaseParam() { this->Reset(); }
	void Reset()
	{
		hold_ll = 0;
		week_fetch_flag.Reset();
		week_task_today_finish_flag.Reset();
		week_active_degree = 0;
	}

	long long hold_ll;
	UNSTD_STATIC_CHECK(BIT_COUNT_OF_LONG_64 > MAX_ACTIVE_DEGREE_WEEK_REWARD_NUM);
	UNSTD_STATIC_CHECK(BIT_COUNT_OF_LONG_64 > MAX_ACTIVE_DEGREE_TASK_NUM);
	UNSTD_STATIC_CHECK(BIT_COUNT_OF_LONG_64 > ACTIVE_DEGREE_TYPE_MAX);

	BitMap<BIT_COUNT_OF_LONG_64> week_fetch_flag;		//��-��Ծ������ȡ���[����index]
	BitMap<BIT_COUNT_OF_LONG_64> week_task_today_finish_flag;	//��-ÿ�������ɱ��[����index]
	int week_active_degree;			//�ܻ�Ծ��
};

struct ActiveDegreeTaskParam
{
	ActiveDegreeTaskParam() { this->Reset(); }
	void Reset()
	{
		hold_ll = 0;
		memset(task_info_list, 0, sizeof(task_info_list));
	}

	struct TaskInfo
	{
		int task_param;		//������ɲ���
		int hold_i;			//ռλ
	};
	long long hold_ll;

	//������Ϣ[��������]
	TaskInfo task_info_list[ACTIVE_DEGREE_TYPE_MAX];

};

struct ActiveTurntableParam
{
	ActiveTurntableParam() { this->Reset(); }
	void Reset()
	{
		turntable_time = 0;
		turntable_day_task_flag.Reset();
		turntable_week_task_flag.Reset();
	}

	int turntable_time;												// ��Ծת�̴���
	BitMap<BIT_COUNT_OF_LONG_64> turntable_day_task_flag;			// ��Ծת�� ÿ��������ɱ��
	BitMap<BIT_COUNT_OF_LONG_64> turntable_week_task_flag;			// ��Ծת�� ÿ��������ɱ��
};

typedef char ActiveDegreeBaseParamHex[sizeof(ActiveDegreeBaseParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(ActiveDegreeBaseParamHex) < 128);

typedef char ActiveDegreeTaskParamHex[sizeof(ActiveDegreeTaskParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(ActiveDegreeTaskParamHex) < 384);

typedef char ActiveTurntableParamHex[sizeof(ActiveTurntableParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(ActiveTurntableParamHex) < 128);

#pragma pack(pop)

#endif