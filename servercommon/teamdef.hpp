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
static const int TEAM_TYPE_RESEARCH_TASK = 2;	// 2在配置内代表三界任务，这个常量用于特殊处理一些需求
static const int TEAM_TYPE_SCHOOL_TASK = 4;		// 4在配置内代表门派任务，这个常量用于特殊处理一些需求
static const int TEAM_TYPE_WILD_BOSS = 8;		// 8在配置内代表野外首领，这个常量用于特殊处理一些需求
static const int TEAM_TYPE_ONE_DRAGON = 10;		// 10在配置内代表一条龙，这个常量用于特殊处理一些需求
static const int TEAM_TYPE_MONSTER_WAVE = 5;	// 5在配置内代表万妖谷，这个常量用于特殊处理一些需求
static const int TEAM_TYPE_FANTASY_FB = 11;		// 11在配置内代表四象幻境，这个常量用于特殊处理一些需求
static const int TEAM_TYPE_GUILD_FIGHT = 165;	// 165在配置内代表家族大乱斗，这个常量用于特殊处理一些需求
static const int TEAM_TYPE_RELIC_LOOTING = 13;	// 13在配置内代表遗迹夺宝，这个常量用于特殊处理一些需求
static const int TEAM_TYPE_HUAN_JIE_ZHAN_CHANG = 180; //180 幻界战场，这个常量用于特殊处理一些需求
static const int TEAM_TYPE_QINGYUAN_DUIDUIPENG = 243;	// 243在配置内代表情缘对对碰，这个常量用于特殊处理一些需求
static const int TEAM_TYPE_TU_ZI_KUAI_PAO = 244;	// 兔子快跑
static const int TEAM_TYPE_SHAN_HAI_DENG_HUI = 245;		// 245在配置内代表山海灯会，这个常量用于特殊处理一些需求
static const int TEAM_TYPE_CONCENTTRIC_KONT_1 = 246;	// 246在配置内代表缘牵同心结-情义同心结，这个常量用于特殊处理一些需求
static const int TEAM_TYPE_CONCENTTRIC_KONT_2 = 247;	// 247在配置内代表缘牵同心结-如意同心结，这个常量用于特殊处理一些需求
static const int TEAM_TYPE_CONCENTTRIC_KONT_3 = 248;	// 248在配置内代表缘牵同心结-永结同心结，这个常量用于特殊处理一些需求
static const int TEAM_TYPE_KE_JU_EXAM = 269;	// 科举考试


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
	TEAM_TYPE_TARGET_TYPE_CONQUEST_TASK = 6,	//远征任务
	TEAM_TYPE_TARGET_TYPE_LILIAN = 7,			//秘宝阁(历练)
	TEAM_TYPE_TARGET_TYPE_ELITE_TRAIL = 8,		//精英试炼
	TEAM_TYPE_TARGET_TYPE_ACTIVITY = 9,		
	TEAM_TYPE_TARGET_TYPE_SHANHAI_BOSS = 10,	//山海降妖
	TEAM_TYPE_TARGET_TYPE_SHANHAI_BOSS_HARD = 11, // 山海困难
	TEAM_TYPE_TARGET_TYPE_SHANHAI_BOSS_HELL = 12, // 山海地狱
	TEAM_TYPE_TARGET_TYPE_SHANHAI_RELIC_LOOTING = 13, // 遗迹夺宝
	TEAM_TYPE_TARGET_TYPE_MIYI = 14,				// 远征魔冢（密医）
	TEAM_TYPE_TARGET_TYPE_BIG_DIPPER = 15,			// 北斗七星
	TEAM_TYPE_TARGET_TYPE_DU_JIE = 16,			// 渡劫
	TEAM_TYPE_TARGET_TYPE_WAN_LING_HUA_BEN = 17,// 万灵话本
	TEAM_TYPE_TARGET_TYPE_FALLEN_GOD = 18,		// 殒神之地
	TEAM_TYPE_TARGET_TYPE_HUASHENG_MIJING = 19,	// 化圣秘境
	TEAM_TYPE_TARGET_TYPE_RELIC_LOOTING = 20,// 遗迹夺宝
	TEAM_TYPE_TARGET_TYPE_CYCLE_ACTIVITY = 21,	// 周期活动
	TEAM_TYPE_TARGET_TYPE_HONGMENG_TIANJIE = 22,	// 鸿蒙天界
	TEAM_TYPE_TARGET_TYPE_TEAM_CHALLENGE = 23,		// 组队挑战任务
	TEAM_TYPE_TARGET_TYPE_ROLE_GUILD_TASK = 24,	// 家族任务组队打怪

	TEAM_TYPE_TARGET_TYPE_MAX
};

//这个结构要对齐协议用到
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