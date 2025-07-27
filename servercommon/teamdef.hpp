#ifndef __TEAM_DEF_HPP__
#define __TEAM_DEF_HPP__

#include "servercommon/servercommon.h"

#pragma pack(push)
#pragma pack(4)

enum TEAM_CHECK_TYPE
{
	TEAM_CHECK_TYPE_REFUSE_ALL = 0,
	TEAM_CHECK_TYPE_NEED_CHECK = 1,
	TEAM_CHECK_TYPE_AUTO_JOIN = 2,
	TEAM_CHECK_TYPE_MAX,
};

static const int TEAM_TYPE_INVALID = 0;
static const int TEAM_TYPE_DEFAULT = 1;
static const int TEAM_TYPE_RESEARCH_TASK = 2;	// 2�������ڴ�������������������������⴦��һЩ����
static const int TEAM_TYPE_SCHOOL_TASK = 4;		// 4�������ڴ�������������������������⴦��һЩ����
static const int TEAM_TYPE_WILD_BOSS = 8;		// 8�������ڴ���Ұ�����죬��������������⴦��һЩ����
static const int TEAM_TYPE_ONE_DRAGON = 10;		// 10�������ڴ���һ��������������������⴦��һЩ����
static const int TEAM_TYPE_MONSTER_WAVE = 5;	// 5�������ڴ��������ȣ���������������⴦��һЩ����
static const int TEAM_TYPE_FANTASY_FB = 11;		// 11�������ڴ�������þ�����������������⴦��һЩ����
static const int TEAM_TYPE_GUILD_FIGHT = 165;	// 165�������ڴ��������Ҷ�����������������⴦��һЩ����
static const int TEAM_TYPE_RELIC_LOOTING = 13;	// 13�������ڴ����ż��ᱦ����������������⴦��һЩ����
static const int TEAM_TYPE_HUAN_JIE_ZHAN_CHANG = 180; //180 �ý�ս������������������⴦��һЩ����
static const int TEAM_TYPE_QINGYUAN_DUIDUIPENG = 243;	// 243�������ڴ�����Ե�Զ�������������������⴦��һЩ����
static const int TEAM_TYPE_TU_ZI_KUAI_PAO = 244;	// ���ӿ���
static const int TEAM_TYPE_SHAN_HAI_DENG_HUI = 245;		// 245�������ڴ���ɽ���ƻᣬ��������������⴦��һЩ����
static const int TEAM_TYPE_CONCENTTRIC_KONT_1 = 246;	// 246�������ڴ���Եǣͬ�Ľ�-����ͬ�Ľᣬ��������������⴦��һЩ����
static const int TEAM_TYPE_CONCENTTRIC_KONT_2 = 247;	// 247�������ڴ���Եǣͬ�Ľ�-����ͬ�Ľᣬ��������������⴦��һЩ����
static const int TEAM_TYPE_CONCENTTRIC_KONT_3 = 248;	// 248�������ڴ���Եǣͬ�Ľ�-����ͬ�Ľᣬ��������������⴦��һЩ����
static const int TEAM_TYPE_KE_JU_EXAM = 269;	// �ƾٿ���


static const int TEAM_TYPE_MAX = 300;
static const int TEAM_INDEX_MAX = 100000;
UNSTD_STATIC_CHECK(TEAM_INDEX_MAX < MAX_ROLE_ID / 10);

enum TEAM_TYPE_TARGET_TYPE
{
	TEAM_TYPE_TARGET_TYPE_INVALID = 0,
	TEAM_TYPE_TARGET_TYPE_DAILY_ACTIVITY = 1,
	TEAM_TYPE_TARGET_TYPE_LIMIT_TIME_ACTIVITY = 2,
	TEAM_TYPE_TARGET_TYPE_BRANCH_TASK = 3,
	TEAM_TYPE_TARGET_TYPE_CHALLENGE_TASK = 4,
	TEAM_TYPE_TARGET_TYPE_JINJIE_TASK = 5,
	TEAM_TYPE_TARGET_TYPE_CONQUEST_TASK = 6,	//Զ������
	TEAM_TYPE_TARGET_TYPE_LILIAN = 7,			//�ر���(����)
	TEAM_TYPE_TARGET_TYPE_ELITE_TRAIL = 8,		//��Ӣ����
	TEAM_TYPE_TARGET_TYPE_ACTIVITY = 9,		
	TEAM_TYPE_TARGET_TYPE_SHANHAI_BOSS = 10,	//ɽ������
	TEAM_TYPE_TARGET_TYPE_SHANHAI_BOSS_HARD = 11, // ɽ������
	TEAM_TYPE_TARGET_TYPE_SHANHAI_BOSS_HELL = 12, // ɽ������
	TEAM_TYPE_TARGET_TYPE_SHANHAI_RELIC_LOOTING = 13, // �ż��ᱦ
	TEAM_TYPE_TARGET_TYPE_MIYI = 14,				// Զ��ħڣ����ҽ��
	TEAM_TYPE_TARGET_TYPE_BIG_DIPPER = 15,			// ��������
	TEAM_TYPE_TARGET_TYPE_DU_JIE = 16,			// �ɽ�
	TEAM_TYPE_TARGET_TYPE_WAN_LING_HUA_BEN = 17,// ���黰��
	TEAM_TYPE_TARGET_TYPE_FALLEN_GOD = 18,		// ����֮��
	TEAM_TYPE_TARGET_TYPE_HUASHENG_MIJING = 19,	// ��ʥ�ؾ�
	TEAM_TYPE_TARGET_TYPE_RELIC_LOOTING = 20,// �ż��ᱦ
	TEAM_TYPE_TARGET_TYPE_CYCLE_ACTIVITY = 21,	// ���ڻ
	TEAM_TYPE_TARGET_TYPE_HONGMENG_TIANJIE = 22,	// �������
	TEAM_TYPE_TARGET_TYPE_TEAM_CHALLENGE = 23,		// �����ս����
	TEAM_TYPE_TARGET_TYPE_ROLE_GUILD_TASK = 24,	// ����������Ӵ��

	TEAM_TYPE_TARGET_TYPE_MAX
};

//����ṹҪ����Э���õ�
struct TeamItem
{
	struct RoleInfo
	{
		int uid;
		int level;
		int prof;
		short avatar_type;
		short headshot_id;

		GameName name;
	};

	void Reset()
	{
		team_index = 0;
		limit_level_low = 0;
		limit_level_high = 0;
		cur_member_num = 0;
		team_type = 0;
		is_cross = 0;
		guild_id = 0;
		create_team_timestamp = 0;
		memset(role_info, 0, sizeof(role_info));
	}

	int GetMinLevel() const
	{
		int level = MAX_ROLE_LEVEL;
		for (int i = 0; i < cur_member_num && i < MAX_TEAM_MEMBER_NUM; ++i)
		{
			level = GetMin(level, role_info[i].level);
		}

		return level;
	}

	int		team_index;
	short	limit_level_low;
	short	limit_level_high;
	int		cur_member_num;
	short	team_type;
	short	is_cross;
	int		guild_id;
	unsigned int create_team_timestamp;
	RoleInfo role_info[MAX_TEAM_MEMBER_NUM];
};

UNSTD_STATIC_CHECK(5 == MAX_TEAM_MEMBER_NUM);

static const int MAX_TEAM_LIST_NUM = 128;
static const int MAX_TEAM_CHANNEL_ITEM_NUM = 30;

#pragma pack(pop)

#endif 