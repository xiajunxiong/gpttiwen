#ifndef __WORLD_STATUS_PARAM_HPP__
#define __WORLD_STATUS_PARAM_HPP__

#include "common/tlvprotocol.h"
#include "servercommon/serverdef.h"
#include "servercommon/basedef.h"
#include "servercommon/servercommon.h"
#include "servercommon/wildbossdef.hpp"
#include "servercommon/couragechallengedef.hpp"
#include "servercommon/activitydef.hpp"
#include "servercommon/colosseumdef.hpp"
#include "servercommon/schooltaskdef.h"
#include "servercommon/shanhaibossdef.hpp"
#include "servercommon/struct/global/historytoppriceparam.hpp"
#include "servercommon/bigdipperdef.h"
#include "servercommon/weddingdef.hpp"
#include "servercommon/struct/global/kuafuyingxiongludef.hpp"
#include "servercommon/huanshoudef.hpp"

static const int TOP_LEVEL_REFRESH_BASE_TIMESTAMP = 1640966400;			//巅峰等级重置刷新的基准时间戳 
static const int TOP_LEVEL_REFRESH_DAY_TIEMS = 90;						//巅峰等级重置天数(如果修改了重置天数,则基准时间与全局flag和玩家flag也要跟着修改,统一重置一遍)
static const int TOP_LEVEL_QUICK_LOG_INTERVAL = 4;						//巅峰等级log间隔(单位:小时)
UNSTD_STATIC_CHECK(TOP_LEVEL_REFRESH_DAY_TIEMS > 0);

static const int MAX_DAY_SPECIAL_LOGIC_ACTIVITY_TYPE_NUM = 10;
enum DAY_SPECIAL_LOGIC_ACTIVITY_TYPE		//本服活动不可加入
{
	DAY_SPECIAL_LOGIC_ACTIVITY_TYPE_GUILD_ANSWER = 0,			//家族答题
	DAY_SPECIAL_LOGIC_ACTIVITY_TYPE_GUILD_FIGHT = 1,			//家族大乱斗
	DAY_SPECIAL_LOGIC_ACTIVITY_TYPE_ONLY_FIGHT = 2,				//以一敌百
	DAY_SPECIAL_LOGIC_ACTIVITY_TYPE_TEAM_FIGHT = 3,				//竞技对抗赛
	DAY_SPECIAL_LOGIC_ACTIVITY_TYPE_SILK_ROADS = 4,				//丝绸之路

	DAY_SPECIAL_LOGIC_ACTIVITY_TYPE_MAX,
};
UNSTD_STATIC_CHECK(DAY_SPECIAL_LOGIC_ACTIVITY_TYPE_MAX <= MAX_DAY_SPECIAL_LOGIC_ACTIVITY_TYPE_NUM);

struct WorldStatusCommonDataParam
{
	WorldStatusCommonDataParam() { this->Reset(); }

	void Reset() 
	{
		last_reset_role_module_dayid = 0;
		last_reset_role_modlue_timestamp = 0;
		platform_battle_settle_accounts_time_old = 0;
		memset(today_school_task_list, 0, sizeof(today_school_task_list));
		open_server_week_num = 0;

		now_question_group_id = 0;
		now_answer_subject = 0;
		now_answer = 0;
		answer_tamp = 0;
		show_answer_tamp = 0;
		next_subject_refresh_tamp = 0;
		answer_end_tamp = 0;
		next_answer_tamp = 0;

		ra_quan_min_discount_first_mark = 0;

		is_reset_complex_cap_rank_flag = 0;
		is_reset_shan_hai_boss_mark = 0;
		is_recalc_top_level_refresh_time_flag = 0;
		two_week_refresh_flag = 0;

		next_week_timestamp = 0u;
		next_month_timestamp = 0u;
		next_week_6_OClock = 0u;
		next_month_6_OClock = 0u;

		memset(can_find_end_timestamp_list, 0, sizeof(can_find_end_timestamp_list));

		guild_fight_open_times = 0;
		colosseum_refresh_tamp = 0;
		lun_hui_wang_chuan_refresh_tamp = 0;
		cloud_arena_cur_season_start_timestamp = 0u;
		cloud_arena_cur_season_end_timestamp = 0u;

		open_cross_server_zero_timestamp = 0;
		next_top_level_refresh_timestamp = 0;

		big_dipper_param.Reset();

		tian_di_dao_hen_open_state = 0;
		tian_di_dao_hen_next_state_time = 0;
		brave_ground_world_level = 0;

		week_refresh_flag = 0;
		platform_battle_settle_accounts_time = 0;
		endless_tower_rank_reward_tamp = 0;
		jie_yi_mod_ver = 0;

		clear_person_rank_flag = 0;
		reserve_uint = 0;
		hidden_open_server_time = 0;

		uta_active_reward_begin_timestamp = 0u;
		uta_active_reward_end_timestamp = 0u;

		is_calc_top_level_season_start_timestamp_flag = 0;
		reserve_ch1 = 0;
		reserve_sh = 0;
		top_level_season_start_timestamp = 0;
		next_quick_log_top_level_timestamp = 0;
	}

	void WorldQuestionNextTimesReset()
	{
		now_question_group_id = 0;
		now_answer_subject = 0;
		now_answer = 0;
		answer_tamp = 0;
		show_answer_tamp = 0;
		next_subject_refresh_tamp = 0;
		answer_end_tamp = 0;
	}

	unsigned int last_reset_role_module_dayid;		// 服务器角色最后重置数据的dayid
	unsigned int last_reset_role_modlue_timestamp;	// 服务器角色最后充值数据的时间
	unsigned int platform_battle_settle_accounts_time_old;		//擂台站结算时间

	int today_school_task_list[SCHOOL_TASK_NUM];	// 今天门派任务
	int open_server_week_num;						// 开服周数（按周一算）

	int now_question_group_id;					//第几题（题目组）
	int now_answer_subject;						//题目id
	int now_answer;								//group_id
	long long answer_tamp;						//结束回答时间戳
	long long show_answer_tamp;					//展示答案时间戳
	long long next_subject_refresh_tamp;		//下一次题目刷新时间

												//活动开启相关
	long long answer_end_tamp;//结束时间戳
	long long next_answer_tamp;//下一次活动开启时间戳

	int ra_quan_min_discount_first_mark;
	char is_reset_complex_cap_rank_flag;		//重置综合战力排行榜 0:未重置 1:已重置
	char is_reset_shan_hai_boss_mark;			//山海改版 重置数据
	char is_recalc_top_level_refresh_time_flag;	//是否重新计算巅峰等级重置时间戳 0:未计算 2:已计算
	char two_week_refresh_flag;			//保留 -》 一次性两周刷新标记

	unsigned int next_week_timestamp;	// 触发OnWeekChange的时间戳
	unsigned int next_month_timestamp;	// 触发OnMonthChange的时间戳
	unsigned int next_week_6_OClock;	// 触发OnWeekResetModuleData的时间戳
	unsigned int next_month_6_OClock;	// 触发OnMonthResetModuleData的时间戳

	unsigned int can_find_end_timestamp_list[MAX_DAY_SPECIAL_LOGIC_ACTIVITY_TYPE_NUM];		//特殊可找回奖励结束时间戳
	int guild_fight_open_times;

	unsigned int colosseum_refresh_tamp;					//竞技场成就刷新时间戳
	unsigned int lun_hui_wang_chuan_refresh_tamp;			//轮回忘川刷新时间戳
	unsigned int next_top_level_refresh_timestamp;			//巅峰等级重置时间戳

	unsigned int cloud_arena_cur_season_start_timestamp; // 穹顶当前赛季开始时间戳
	unsigned int cloud_arena_cur_season_end_timestamp;

	unsigned int open_cross_server_zero_timestamp;		//跨服服务器配置的开服0点时间戳

	BigDipperMonsterParam big_dipper_param;

	int tian_di_dao_hen_open_state;			
	unsigned int tian_di_dao_hen_next_state_time;
	int brave_ground_world_level;  //没用了，需求51012 ，取消了和世界等级挂钩

	int week_refresh_flag;
	unsigned int platform_battle_settle_accounts_time;		//擂台站结算时间
	unsigned int endless_tower_rank_reward_tamp;
	unsigned int jie_yi_mod_ver;

	int clear_person_rank_flag;

	unsigned int reserve_uint;
	unsigned int hidden_open_server_time;

	unsigned int uta_active_reward_begin_timestamp;
	unsigned int uta_active_reward_end_timestamp;

	char is_calc_top_level_season_start_timestamp_flag;		//是否计算过巅峰等级赛季开始时间戳标识 1:已计算
	char reserve_ch1;
	short reserve_sh;										
	unsigned int top_level_season_start_timestamp;			//巅峰等级赛季开始时间戳
	unsigned int next_quick_log_top_level_timestamp;		//下一次打巅峰等级log时间戳		
};

enum HONOR_ROLL_TYPE
{
	HONOR_ROLL_TYPE_MIN = 0,

	HONOR_ROLL_TYPE_COURAGE_CHALLENGE_1 = 6,			// 通关勇气的挑战 第1层
	HONOR_ROLL_TYPE_COURAGE_CHALLENGE_2 = 7,			// 通关勇气的挑战 第2层
	HONOR_ROLL_TYPE_COURAGE_CHALLENGE_3 = 8,			// 通关勇气的挑战 第3层
	HONOR_ROLL_TYPE_COURAGE_CHALLENGE_4 = 9,			// 通关勇气的挑战 第4层
	HONOR_ROLL_TYPE_COURAGE_CHALLENGE_5 = 10,			// 通关勇气的挑战 第5层
	HONOR_ROLL_TYPE_COURAGE_CHALLENGE_6 = 11,			// 通关勇气的挑战 第6层

	HONOR_ROLL_TYPE_MAX,
};

static const int WORLD_HONOR_ROLL_TEAM_COUNT = 3;		// 速杀榜有几个队伍
static const int WORLD_HONOR_ROLL_TYPE_COUNT = 64;		// 排名的类型数量
UNSTD_STATIC_CHECK(HONOR_ROLL_TYPE_MAX < WORLD_HONOR_ROLL_TYPE_COUNT);

struct HonorRollData
{
	HonorRollData() { this->Reset(); }
	void Reset()
	{
		rounds = 0;
		pass_time = 0;
		memset(member_list, 0, sizeof(member_list));
	}

	bool operator ==(HonorRollData a)
	{
		// 传入时按uid大小排序了
		for (int i = 0; i < MAX_TEAM_MEMBER_NUM; ++i)
		{
			if (this->member_list[i] != a.member_list[i])
			{
				return false;
			}
		}

		return true;
	}

	int rounds;					// 回合数
	unsigned int pass_time;		// 通关时间戳
	int member_list[MAX_TEAM_MEMBER_NUM];
	UNSTD_STATIC_CHECK(5 == MAX_TEAM_MEMBER_NUM);
};

struct HonorRollTypeData
{
	HonorRollTypeData() { this->Reset(); }
	void Reset()
	{
		first_data.Reset();
		for (int j = 0; j < WORLD_HONOR_ROLL_TEAM_COUNT; ++j)
		{
			roll_data[j].Reset();
		}
	}

	HonorRollData first_data;
	HonorRollData roll_data[WORLD_HONOR_ROLL_TEAM_COUNT];
	UNSTD_STATIC_CHECK(3 == WORLD_HONOR_ROLL_TEAM_COUNT);
};

struct WorldHonorRollDataParam
{
	WorldHonorRollDataParam() { this->Reset(); }
	void Reset()
	{
		for (int j = 0; j < WORLD_HONOR_ROLL_TYPE_COUNT; ++j)
		{
			roll_type_list[j].Reset();
		}
	}
	
	HonorRollTypeData roll_type_list[WORLD_HONOR_ROLL_TYPE_COUNT];
	UNSTD_STATIC_CHECK(64 == WORLD_HONOR_ROLL_TYPE_COUNT);
};

struct CourageChallengeDataParam
{
	CourageChallengeDataParam() { this->Reset(); }
	void Reset()
	{
		memset(first_pass_role_id, 0, sizeof(first_pass_role_id));
	}

	int first_pass_role_id[COURAGE_CHALLENGE_MAX_LEVEL];
};

struct ChiefElectionDataParam
{
	ChiefElectionDataParam() { this->Reset(); }
	void Reset()
	{
		memset(winner, 0, sizeof(winner));
	}

	bool HasRole(int role_id)
	{
		for (int i = 0; i < ARRAY_LENGTH(winner); ++i)
		{
			if (winner[i].role_id == role_id)
			{
				return true;
			}
		}

		return false;
	}

	ChiefElectionWinnerInfo  winner[ACTIVITY_CHIEF_ELECTION_JOIN_PROF_NUM];
};

struct ChiefPeakBattleDataParam
{
	ChiefPeakBattleDataParam() { this->Reset(); }
	void Reset()
	{
		champion_role_id = 0;
		memset(role_info, 0, sizeof(role_info));
	}

	bool HasRole(int role_id)
	{
		for (int i = 0; i < ARRAY_LENGTH(role_info); ++i)
		{
			if (role_info[i].role_id == role_id)
			{
				return true;
			}
		}

		return false;
	}

	int champion_role_id;
	ChiefPeakBattleInfo  role_info[ACTIVITY_CHIEF_PEAK_BATTLE_FIGTHER_NUM];
};


struct ColosseumManagerParam
{
	ColosseumManagerParam() { this->Reset(); }
	void Reset()
	{
		buff_id_OBSOLETE = 0;	
		buff_num = 0;
		memset(buff_id_list, 0, sizeof(buff_id_list));
	}

	int buff_id_OBSOLETE;
	int buff_num;
	int buff_id_list[COLOSSEUM_DEFENDER_BUFF_NUM];
};

struct OnlyFightDataParam
{
	OnlyFightDataParam() { this->Reset(); }
	void Reset()
	{
		memset(title_id_list, 0, sizeof(title_id_list));
		memset(reserve_int, 0, sizeof(reserve_int));
		count = 0;
		memset(rank, 0, sizeof(rank));
	}

	int title_id_list[ONLY_FIGHT_RANK_TITLE_MAX_NUM];
	int reserve_int[17];
	int count;
	int rank[ONLY_FIGHT_SCNEE_MAX_NUM][ONLY_FIGHT_RANK_TITLE_MAX_NUM];	//记录以一敌百各个等级场排行前三的玩家UID
};

struct WorldStatusGuildFightTopInfo
{
	WorldStatusGuildFightTopInfo() { this->Reset(); }
	void Reset()
	{
		guild_id = 0;
		memset(guild_name, 0, sizeof(guild_name));
	}

	int guild_id;
	GuildName guild_name;
};

struct WorldStatusGuildFightParam
{
	static const int GUILD_FIGHT_BUFF_REWARD_OPEN_TIMES = 2;		//每周要计算buff奖励的开启活动次数,
	static const int GUILD_FIGHT_BUFF_RANK_TOP_NUM = 10;		//每次开启活动前多少名会计算分数，1-10名的公会分别获得10到1分的积分。
	static const int GUILD_FIGHT_BUFF_WIN_GUILD_NUM = 3;		//积分根据每周2次的家族大乱斗，分数最高3个家族获取荣耀之证buff

	WorldStatusGuildFightParam() { this->Reset(); }
	void Reset()
	{
		is_set = 0;
		for (int i = 0; i < ARRAY_LENGTH(top_info); ++i)
		{
			top_info[i].Reset();
		}
	}

	int is_set;
	WorldStatusGuildFightTopInfo top_info[GUILD_FIGHT_BUFF_WIN_GUILD_NUM];
};

struct QingYuanDuiDuiPengRankData
{
	int uid;
	GameName name;
	short jifen;
	short pass_times;
};

struct QingYuanDuiDuiPengRankParam
{
	QingYuanDuiDuiPengRankParam() { this->Reset(); }
	void Reset()
	{
		rank_count = 0;
		for (int i = 0; i < ARRAY_LENGTH(rank_list); ++i)
		{
			memset(&rank_list[i], 0, sizeof(QingYuanDuiDuiPengRankData));
		}
	}

	int rank_count;
	QingYuanDuiDuiPengRankData rank_list[100];
};

class WorldStatusParam
{
public:
	bool Serialize(TLVSerializer &outstream) const;
	bool Unserialize(TLVUnserializer &instream);

	unsigned int start_time;
	unsigned int world_time;
	unsigned int last_update_dayid;

	WorldStatusCommonDataParam common_data;
	WorldHonorRollDataParam honor_roll_data;
	WildBossParam wild_boss_data;
	CourageChallengeDataParam courage_challenge_data;
	ChiefElectionDataParam chief_election_data;
	ChiefPeakBattleDataParam chief_peak_battle_data;
	ColosseumManagerParam colosseum_manager_data;
	int pool_id_list_len;		// 竞技场 - 古神宝库数据
	char pool_player_id_list[CHALLENGE_FIELD_POOL_PLAYER_LIST_LEN];

	OnlyFightDataParam only_fight_data;
	ShanHaiBossParam shan_hai_boss_data;
	HistoryTopPriceParam history_top_price_data;
	WorldStatusGuildFightParam guild_fight_data;
	WeddingFB::HiddenGlobalParam wedding_global_data;
	QingYuanDuiDuiPengRankParam qingyuan_duiduipeng_data;
	int god_beast_advent_data;		// 异兽临世全服限购数据
	KuaFuYingXiongLuActivityParam kua_fu_ying_xiong_lu_data;
	HuanShouAnnouncementParam huan_shou_ann_data;
};

typedef char WorldStatusCommonDataParamHex[sizeof(WorldStatusCommonDataParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(WorldStatusCommonDataParamHex) < 1024);
typedef char WorldHonorRollDataParamHex[sizeof(WorldHonorRollDataParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(WorldHonorRollDataParamHex) < 20480);
typedef char WildBossParamHex[sizeof(WildBossParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(WildBossParamHex) < 5120);
typedef char CourageChallengeDataParamHex[sizeof(CourageChallengeDataParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(CourageChallengeDataParamHex) < 2048);
typedef char ChiefElectionDataParamHex[sizeof(ChiefElectionDataParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(ChiefElectionDataParamHex) < 512);
typedef char ChiefPeakBattleDataParamHex[sizeof(ChiefPeakBattleDataParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(ChiefPeakBattleDataParamHex) < 512);

typedef char ColosseumManagerParamHex[sizeof(ColosseumManagerParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(ColosseumManagerParamHex) < 128);

UNSTD_STATIC_CHECK(CHALLENGE_FIELD_POOL_PLAYER_LIST_LEN * 2 < 8192)

typedef char OnlyFightDataParamHex[sizeof(OnlyFightDataParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(OnlyFightDataParamHex) < 800);
typedef char ShanHaiBossParamHex[sizeof(ShanHaiBossParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(ShanHaiBossParamHex) < 2048);

typedef char WorldStatusGuildFightParamHex[sizeof(WorldStatusGuildFightParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(WorldStatusGuildFightParamHex) < 512);

typedef char WeddingHiddenGlobalParamHex[sizeof(WeddingFB::HiddenGlobalParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(WeddingHiddenGlobalParamHex) < 128);

typedef char QingYuanDuiDuiPengRankParamHex[sizeof(QingYuanDuiDuiPengRankParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(QingYuanDuiDuiPengRankParamHex) < 14417);

#endif // __WORLD_STATUS_PARAM_HPP__