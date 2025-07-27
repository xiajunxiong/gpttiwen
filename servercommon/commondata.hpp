#ifndef __COMMON_DATA_HPP__
#define __COMMON_DATA_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/struct/battle/battle_def.hpp"
#include "bitmapfunction.hpp"
#include "servercommon/struct/global/offlineregisterparam.hpp"
#include "servercommon/shanhaibossdef.hpp"
#include "servercommon/xunbaodef.hpp"
#include "gameworld/gameworld/other/attributelist.hpp"
#include "servercommon/struct/global/worldstatusparam.hpp"
#include "servercommon/activitydef.hpp"
#include "hongbaodef.hpp"

#pragma pack(push) 
#pragma pack(4)
enum NEW_PET_STATUS
{
	NEW_FOLLOW_INDEX = 0,		// 跟随宠物index
	NEW_FIGHT_INDEX_BEGIN,	 		// 
	NEW_FIGHT_INDEX= NEW_FIGHT_INDEX_BEGIN,	 		// 出战宠物index
	NEW_STANDBY_INDEX1,	 		// 待机宠物index1
	NEW_STANDBY_INDEX2,	 		// 待机宠物index2
	NEW_STANDBY_INDEX3,	 		// 待机宠物index3
	NEW_STANDBY_INDEX4,			// 待机宠物index4
	NEW_STANDBY_INDEX5,			// 待机宠物index5
	NEW_FIGHT_INDEX_END= NEW_STANDBY_INDEX5,			// 
	NEW_RESERVED,				// 保留
	NEW_PET_STATUS_MAX,
};

enum PET_STATUS_OBSOLETE			// 已废弃，见NEW_PET_STATUS
{
	FOLLOW_INDEX_OBSOLETE = 0,		// 跟随宠物index
	FIGHT_INDEX_OBSOLETE,	 		// 出战宠物index
	STANDBY_INDEX1_OBSOLETE,	 	// 待机宠物index1
	STANDBY_INDEX2_OBSOLETE,	 	// 待机宠物index2
	STANDBY_INDEX3_OBSOLETE,	 	// 待机宠物index3
	PET_STATUS_MAX_OBSOLETE,
};

enum HOLY_EQUIP_STATUS
{
	HOLY_EQUIP_STATUS_BEGIN = 0,

	HOLY_EQUIP_STATUS_INDEX0 = HOLY_EQUIP_STATUS_BEGIN,	// 出战/待机圣器index
	HOLY_EQUIP_STATUS_INDEX1,		// 出战/待机圣器index1
	HOLY_EQUIP_STATUS_INDEX2,		// 出战/待机圣器index2
	HOLY_EQUIP_STATUS_INDEX3,		// 出战/待机圣器index3
	HOLY_EQUIP_STATUS_INDEX4,		// 出战/待机圣器index4

	HOLY_EQUIP_STATUS_MAX = 5,
};

static const int MAX_DAY_ACTIVITY_TYPE_NUM = 32;
enum DAY_ACTIVITY_TYPE
{
	DAY_ACTIVITY_TYPE_MIN = 0,

	DAY_ACTIVITY_TYPE_ZHEN_ZHONG				= 1,					//	真中的研究 / 三界除魔
	DAY_ACTIVITY_TYPE_TREASURE_MAP_TASK		    = 2,					//	宝图任务 / 缉捕盗贼
	DAY_ACTIVITY_TYPE_SCHOOL_TASK				= 3,					//	学院任务 / 门派任务
	DAY_ACTIVITY_TYPE_WST_QUESTION				= 4,					//	万事通的考验 / 神算子的考验
	DAY_ACTIVITY_TYPE_TREVI_FOUNTAIN			= 5,					//	法兰城许愿池 / 许愿池
	DAY_ACTIVITY_TYPE_BOUNTY_TASK				= 6,					//  赏金任务
	DAY_ACTIVITY_TYPE_FANTASY_FB				= 7,					//  四象幻境
	DAY_ACTIVITY_TYPE_TRANSACTION_TASK			= 8,					//  丝绸之路(已废弃, 远洋贸易/购买任务)
	DAY_ACTIVITY_TYPE_GUILD_ANSWER				= 9,					//	家族答題
	DAY_ACTIVITY_TYPE_GUILD_FIGHT				= 10,					//	家族大乱斗
	DAY_ACTIVITY_TYPE_ONLY_FIGHT				= 11,					//	以一敌百
	DAY_ACTIVITY_TYPE_TEAM_FIGHT				= 12,					//  竞技对抗赛
	DAY_ACTIVITY_TYPE_OVERFLOW_ENERGY			= 13,					//	秘境体力溢出
	DAY_ACTIVITY_TYPE_WAN_LING_FB_ACTIVE		= 14,					//  万灵话本(活跃)
	DAY_ACTIVITY_TYPE_CRYSTAL_XI_YOU_ACTIVE		= 15,					//  精英试炼(活跃)
	DAY_ACTIVITY_TYPE_CLOUD_ARENA_ACTIVE		= 16,					//  穹顶之争(活跃)
	DAY_ACTIVITY_TYPE_ZHEN_ZHONG_ACTIVE			= 17,					//	真中的研究 / 三界除魔(活跃)
	DAY_ACTIVITY_TYPE_BOUNTY_TASK_ACTIVE		= 18,					//  赏金任务(活跃)
	DAY_ACTIVITY_TYPE_SILK_ROADS_ACTIVE			= 19,					//  真 丝绸之路

	DAY_ACTIVITY_TYPE_MAX,
};
UNSTD_STATIC_CHECK(DAY_ACTIVITY_TYPE_MAX == 20 && DAY_ACTIVITY_TYPE_MAX <= MAX_DAY_ACTIVITY_TYPE_NUM);

enum REWARD_BACK_FIND_TYPE
{
	RB_TYPE_COIN = 0,		// 铜币找回
	RB_TYPE_GOLD = 1,		// 元宝找回

	RB_TYPE_MAX
};

static const int MAX_CLIENT_BEHAVIOR_TYPE_NUM = 64;
enum CLIENT_BEHAVIOR_TYPE
{
	CLIENT_BEHAVIOR_TYPE_OPEN_CHONGZHI = 0,		//客户端打开充值界面

	CLIENT_BEHAVIOR_TYPE_MAX
};

UNSTD_STATIC_CHECK(MAX_CLIENT_BEHAVIOR_TYPE_NUM >= CLIENT_BEHAVIOR_TYPE_MAX);

static const int MAX_TREVI_FOUNTION_TODAY_BUFF_NUM = 3;
static const int LAYER_MAX_NUM = 4;						//当前最大层数(四象幻境)

static const int SHI_TU_PREFERENCES_NUM = 4;						//师徒偏好设置选择数
static const int CHAT_CORD_MAX_LIST = 8;

static const int  CHONG_ZHI_RECORD_MAX_NUM = 5;

static const int QUICK_SPEECH_MAX_NUM = 5;// 快捷发言

const static int PER_DAY_SECOND = 3600 * 24;	//一天的秒数
static const int MAX_XUN_BAO_BLESS_PET_NUM = 5;	

const static int MAX_COLOSSEUM_TASK_NUM = 256;	// 竞技成就最大数量

const static int SINGLE_CHAT_NUM_LIMIT_MAX = 10;  //私聊限制最大人数

static const int MAX_ROLE_TOP_LEVEL_DB_NUM = 1024;		// 用来存库对齐
static const int MAX_ROLE_TOP_LEVEL_NUM = 999;		// 巅峰等级上限
static const int MAX_ROLE_TOP_LEVEL_ACHIEVE_NUM = 32;	// 巅峰等级成就奖励数量上限
UNSTD_STATIC_CHECK(MAX_ROLE_TOP_LEVEL_DB_NUM >= MAX_ROLE_TOP_LEVEL_NUM);

static const int ONE_DAY_QUIT_GUILD_LIMIT_NUM = 2;	//每日退出家族次数限制（开始CD）
static const int ONE_DAY_QUIT_GUILD_LIMIT_TIME = 8 * 3600;	//超过限制CD时间

static const int DAY_REWARD_DAY_FOOD_SEQ_COUNT = 32;

static const int MAX_SCORE_TASK_NUM = 32;
static const int PET_STRENGTHEN_REWARD_FLAG = 32;

const int static BRAVE_GROUND_ROLE_REWARD_REISSUE_VER = 2;
const int static DISLOCATION_RESET_VER_FLAG = 1;

const static int ZAO_HUA_SUIT_APPEARANCE_MAX = 80;	//造化装备外观最大

const static int ACCESSORIES_SPECIAL_EQUIP_LEVEL = 2;// 配饰 - 仙饰

UNSTD_STATIC_CHECK(8 == NEW_PET_STATUS_MAX);
UNSTD_STATIC_CHECK(5 == PET_STATUS_MAX_OBSOLETE);
struct CommonDataParam
{
	CommonDataParam()
	{
		this->Reset();
	}

	void Reset()
	{
		plat_spid = 0;
		for (int i = 0; i < PET_STATUS_MAX_OBSOLETE; ++i)
		{
			pet_status_index_OBSOLETE[i] = -1;
		}

		for (int i = 0; i < NEW_PET_STATUS_MAX; ++i)
		{
			new_pet_status_index[i] = -1;
		}

		reward_first_flag = 0;
		battle_is_auto = 0;

		wst_questions_id = 0;
		wst_question_today_buy_exp_count = 0;
		wst_question_yesterday_buy_exp_count = 0;
		wst_answer_num = 0;

		battle_auto_first_move_type = BATTLE_MOVE_TYPE_NORMAL_ATTACK;
		battle_auto_first_skill_id = 0;
		battle_auto_first_skill_level = 0;
		battle_auto_second_move_type = BATTLE_MOVE_TYPE_NORMAL_ATTACK;
		battle_auto_second_skill_id = 0;
		battle_auto_second_skill_level = 0;

		trevi_fountain_today_use_count = 0;
		memset(trevi_fountain_buff_id, 0, sizeof(trevi_fountain_buff_id));
		memset(trevi_fountain_buff_expiration_time, 0, sizeof(trevi_fountain_buff_expiration_time));

		online_reward_fetch_flag = 0;
		sign_count = 0;
		sign_reward_fetch_flag = 0;
		level_reward_fetch_flag = 0;

		battle_position = BATTLE_ROW_TYPE_BACK;
		is_open_exp_crytal = 0;
		wst_answer_yesterday_num = 0;

		system_notice_fetch_flag = 0;

		encounter_monster_interval_ms = 0;
		encounter_monster_flag = 0;
		is_in_no_recover_fb = 0;
		change_career_times = 0;

		maxhp_in_scene = 0;
		maxmp_in_scene = 0;
		curhp_in_scene = 0;
		curmp_in_scene = 0;

		memset(gm_attr_list, 0, sizeof(gm_attr_list));

		wild_boss_get_reward_times = 0;
		get_pet_list.Reset();
		m_guild_id = 0;
		m_guild_positon = 0;

		last_logout_timestamp = 0;

		escort_today_count = 0;
		escort_task_seq = 0;
		escort_npc_id = 0;
		trevi_fountain_can_find_count = 0;
		chongzhi_count = 0;
		encounter_escort_monster_ms = 0;
		last_encounter_monster_flag = 0;
		show_currency_type = 0;
		role_create_count_day = 0;

		wst_question_add_coin = 0;
		wst_question_add_exp = 0;
		last_encounter_monster_ms = 0;

		trap_creature_times = 0;
		pet_join_fight_times = 0;
		today_gold_shop_cost = 0;
		open_treasure_chest_times = 0;
		treasure_hunt_times = 0;
		offline_exp = 0;
		offline_min = 0;
		client_spid = 0;
		exp_crytal_remain_times = 0;
		today_patrol_task = 0;
		patrol_task_reward = 0;
		patrol_task_progress = 0;
		patrol_task_status = 0;
		patrol_task_coin = 0;
		enter_brave_ground_times = 0;
		partner_pro_level = 0;
		gold_unbind_in_debt = 0;
		coin_bind_in_debt = 0;
		fantasy_fb_flag.Reset();
		wst_questions_group = 0;
		today_open_find_fun = 0;
		is_first_create_find_fantasy = 0;
		is_fetch_boss_chapter_baodi_reward_OBSOLETE = 0;
		is_fetch_boss_chapter_damage_rank_reward = 0;
		xun_bao_first_draw_flag_old = 0;
		offine_day = 0;
		world_ans = 0;
		bounty_task_find_num = 0;
		escort_end_time = 0;
		find_fantasy_flag.Reset();
		memset(shi_tu_preferences, 0, sizeof(shi_tu_preferences));
		gladiator_everyday_reward_count = 0;
		memset(gladiator_everyday_reward_list, 0, sizeof(gladiator_everyday_reward_list));
		//memset(chat_group_list, 0, sizeof(chat_group_list));
		memset(local_chat_group_list_OBSOLETE, 0, sizeof(local_chat_group_list_OBSOLETE));
		memset(cross_chat_group_list_OBSOLETE, 0, sizeof(cross_chat_group_list_OBSOLETE));
		memset(cross_chat_group_list, 0, sizeof(cross_chat_group_list));
		memset(chat_group_reserve, 0, sizeof(chat_group_reserve));
		sign_reward_next_month_reset_time = 0u;

		for (int i = 0; i < ARRAY_LENGTH(xun_bao_group_draw_times); ++i)
		{
			xun_bao_group_draw_times[i] = 0;
		}
		fetch_boss_chapter_baodi_reward_day_id_OBSOLETE = 0;
		brave_ground_max_reached_level = 0;
		huan_jie_season_num = 0;

		memset(chat_id_list, 0, sizeof(chat_id_list));
		shan_hai_boss_challenge_time = 0;
		memset(shan_hai_boss_soul_fragment, 0, sizeof(shan_hai_boss_soul_fragment));
		world_ans_tamp = 0;
		today_find_reward_flag.Reset();
		colosseum_set_flag = 0;
		use_new_pet_status_array = 0;
		is_reward_sign_month_resert_flag = 0;
		is_check_train_uplevel_item = 0;
		level_complement_flag = 0;

		next_chong_zhi_record_index = 0;
		for (int i = 0; i < ARRAY_LENGTH(chong_zhi_record); ++i)
		{
			chong_zhi_record[i] = 0;
		}

		memset(simple_record_list, 0, sizeof(simple_record_list));

		train_uplevel_comsume_item_num = 0;
		fetch_boss_chapter_rank_reward_day_id = 0;

		for (int i = 0; i < ARRAY_LENGTH(xun_bao_first_draw_times); ++i)
		{
			xun_bao_first_draw_times[i] = 0;
		}
		xun_bao_first_draw_flag = 0;

		memset(user_attr, 0, sizeof(user_attr));

		next_month_timestamp = 0u;
		next_week_timestamp = 0u;
		next_week_6_OClock = 0u;
		next_month_6_OClock = 0u;

		is_give_task_item_flag = 0;			// 0:未给予 1:已给予 
		world_answer_idx = 0;
		level_complement_flag = 0;
		brave_ground_sao_dang_reward_level = 0;
		brave_ground_sao_dang_is_fetch_sao_dang_reward = 0;

		accumulated_exp = 0;
		m_refresh_accumulated_tamp = 0;
		m_end_accumulate_tamp = 0;
		m_day_difference = 0;
		m_add_exp_times = 0;
		guild_answer_reply_count = 0;
		guild_answer_find_count = 0;
		memset(fetch_special_logic_activity_reward_zero_timestamp, 0, sizeof(fetch_special_logic_activity_reward_zero_timestamp));
		fetch_guild_fight_join_reward_flag = 0;
		guild_fight_find_count = 0;
		fetch_only_fight_join_reward_flag = 0;
		only_fight_find_count = 0;
		fetch_team_fight_join_reward_flag = 0;
		team_fight_find_count = 0;
		animation_speed = 0;
		is_transfer_chat_group_data = 0;
		create_chat_group_cd_timestamp = 0u;

		m_alread_get_exp = 0;
		m_save_exp = 0;

		participate_activity_flag.Reset();
		m_today_accumulated_exp = 0;

		brave_ground_sao_dang_cal_reward_time = 0;
		memset(xun_bao_bless_pet_id_list, 0, sizeof(xun_bao_bless_pet_id_list));

		shan_hai_boss_next_reset_time = 0;
		shan_hai_boss_challenge_day_id = 0; 
		shan_hai_boss_week_challenge_time = 0;
		for (int i = 0; i < SHAN_HAI_BOSS_MAXNUM; i++)
		{
			shan_hai_boss_is_challenge_seq[i] = MAX_UINT8;
		}
		shan_hai_boss_pass_times = 0;

		cloud_arena_season_highest_score = 0;
		cloud_arena_season_highest_dan_id = -1;
		cloud_arena_season_highest_rank = -1;
		cloud_arena_last_season_rank = -1;
		cloud_arena_season_timestamp = 0;
		cloud_arena_new_shop_timestamp = 0u;

		initial_battle_flag = 0;
		randactivity_compensation_flag = 0;
		gm_check_waigua_flag = 0;
		reserve_ch = 0;

		chief_election_login_broadcast_time = 0;
		guild_honor_login_broadcast_time = 0;
		for (int i = 0; i < ARRAY_LENGTH(reserved_sh_505); ++i)
		{
			reserved_sh_505[i] = 0;
		}
		single_chat_num_last_time = 0;
		memset(single_chat_num_list, 0, sizeof(single_chat_num_list));
		fabao_tranf = 0;
		guild_day_active_reward_flag = 0;

		is_refresh_first_notice_top_level_flag = 0;
		today_quit_guild_count = 0;
		overflow_find_count = 0;
		top_level_reward_fetch_flag.Reset();
		next_join_guild_timestamp = 0;

		memset(hongbao_times, 0, sizeof(hongbao_times));

		fetch_resource_download_reward_flag = 0;
		is_brave_ground_reset_flag = 0;
		fetch_fang_fraud_reward_flag = 0;
		single_arena_animation_speed = 0;

		svip_client_custom_data_1 = 0;
		svip_client_custom_data_2 = 0;
		
		use_crystal_energy_num = 0;
		memset(today_activity_type_find_count, 0, sizeof(today_activity_type_find_count));
		god_equip_err_flag = 0;
		top_level_achieve_flag.Reset();

		memset(get_hongbao_day_times, 0, sizeof(get_hongbao_day_times));
		memset(get_hongbao_total_times, 0, sizeof(get_hongbao_total_times));

		next_can_guild_recommend_time = 0;

		xun_bao_second_draw_flag = 0;
		colosseum_rank = 0;

		capacity_target_end_time = 0;
		capacity_target_can_start = 0;			//完成任务设置为1
		capacity_target_is_finish = 0;			//到时间设置为1
		capacity_target_is_fetch_reward = 0;				//是否领取了奖励
		capacity_target_value = 0;				//时间到后的战力

		week_refresh_flag = 0;
		god_turn_bag_flag = 0;

		is_pet_refine_flag = 0;
		is_set_auto_talk_groups_flag = 0;
		is_again_set_top_level_refresh_time_flag = 0;
		is_move_prestige_data_flag = 0;
		pet_refine_count = 0;				

		endless_tower_battle_seq = 0;

		max_guild_fight_rank = 0;
		max_guild_honor_rank = 0;
		guild_hong_bao_fetch_count = 0;
		guild_answer_correct_count = 0;

		memset(today_channel_chat_count_list, 0, sizeof(today_channel_chat_count_list));
		endless_refresh_flag = 0;
		memset(channel_chat_count_list, 0, sizeof(channel_chat_count_list));

		day_food_flag.Reset();

		auto_talk_groups_id = 0;
		auto_talk_id = 0;

		gm_soaring_skill_id = 0;
		gm_soaring_skill_lv = 0;

		guild_boss_baodi_reward_flag_OBSOLETE = 0;
		fetch_boss_chapter_baodi_reward_week_id = 0;

		reserve_int_1_DO_NOT_USE = 0;

		endless_tower_battle_tamp = 0;

		memset(reserve_sh11, 0, sizeof(reserve_sh11));
		task_finish.Reset();

		brave_ground_max_reached_level_by_week = 0;
		brave_ground_reward_reissue_ver = 0;
		dislocation_reset_ver_flag = 0;
		qingyuan_duiduipeng_times = 0;
		is_day_fetch_where_is_the_fairy_flag = 0;

		world_team_arena_rank = 0;
		world_team_arena_title_id = 0;

		equip_refresh_voc_times = 0;

		pet_strengthen_reawrd_flag = 0;
		sanxian_title_flag = 0;
		has_pet_strengthen_flag.Reset();

		san_xian_today_play_times = 0;
		san_xian_pass_idx = 0;
		choose_tamp = 0;

		tian_gong_xun_bao_cur_grid = 0;
		tian_gong_xun_bao_start_timestamp = 0;
		tu_zi_kuai_pao_reward_times = 0;
		tian_gong_xun_bao_free_flag = 0;
		guild_build_join_speed_up_count = 0;

		is_old_user_hundred_god_boss_flag = 0;
		is_finish_hundred_god_boss_flag = 0;
		fetch_hundred_god_boss_flag = 0;
		hundred_god_boss_fin_bounty_count = 0;
		zao_hua_show_mo_zhuang = 0;
		hundred_god_boss_end_timestamp = 0;
		autority_type_test_day_buy_limit = 0;

		pingjia_reward_mark = 0;
		re_ch = 0;

		silk_roads_find_count = 0;
		silk_roads_count = 0;

		zao_hua_wear_app_id = 0;
		zao_hua_app_flag.Reset();

		memset(holy_equip_status_index, -1, sizeof(holy_equip_status_index));

		nether_refresh_flag = 0;
		is_first_manual_pet_xun_bao_flag = 0;
		is_old_user_first_manual_pet_xun_bao_flag = 0;
		team_animation_speed = 0;

		zhou_qi_num = 0;
		zhou_qi_end_time = 0;

		last_logout_timestamp_2 = 0u;
		goto_school_get_reward_times = 0;

		guild_create_consume_coin_bind = 0;
		last_set_world_team_aren_timestamp = 0;

		memset(accessories_special_equip_list, 0, sizeof(accessories_special_equip_list));

		today_add_guaji_exp = 0;
		big_dipper_fight_succ_times = 0;

		receive_real_name_reward_flag = 0;
	}

	int			plat_spid;									// 平台渠道id

	short		pet_status_index_OBSOLETE[PET_STATUS_MAX_OBSOLETE]; // 已废弃  用new_pet_status_index
	unsigned char reward_first_flag;						// 保留-》首充标记
	char		battle_is_auto;								// 战斗是否自动
	char		initial_battle_flag;						// 初始战斗标记
	char		randactivity_compensation_flag;				// 随机活动补差标记
	char		gm_check_waigua_flag;
	char		reserve_ch;

	// 万事通的考验(答题)
	int					wst_questions_id;								// 题目id
	char				wst_question_today_buy_exp_count;				// 能购买的今日奖励次数
	char				wst_question_yesterday_buy_exp_count;			// 能购买的昨日奖励次数
	unsigned short		wst_answer_num;									// 今天的答题数量

	short		battle_auto_first_move_type;
	UInt16		battle_auto_first_skill_id;
	short		battle_auto_first_skill_level;
	short		battle_auto_second_move_type;
	UInt16		battle_auto_second_skill_id;
	short		battle_auto_second_skill_level;

	unsigned short		trevi_fountain_today_use_count;												//!< 许愿池今日使用次数
	unsigned short		trevi_fountain_buff_id[MAX_TREVI_FOUNTION_TODAY_BUFF_NUM];					//!< 许愿池获得的buff_id

	short online_reward_fetch_flag;			// 在线奖励领取flag 每日0点清空
	short sign_count;						// 每月签到奖励 签到次数 每月1号清空
	int sign_reward_fetch_flag;				// 每月签到奖励 领取flag 每月1号清空
	int level_reward_fetch_flag;			// 等级奖励 领取flag

	char battle_position;					// 战斗时的站位（0：后排   1：前排）
	char is_open_exp_crytal;				// 是否开启打卡水晶

	short wst_answer_yesterday_num;			// 万事通的答题 昨天没答的数量

	int system_notice_fetch_flag;			// 功能预告奖励领取标记

	long long encounter_monster_interval_ms;// 剩余遭遇暗怪时间（毫秒）
	char encounter_monster_flag;			// 是否遭遇暗怪

	char is_in_no_recover_fb;				// 是否在血量魔量不能回复的副本
	short change_career_times;				// 职业变换次数

	int maxhp_in_scene;						// 在场景中的最大血量
	int maxmp_in_scene;						// 在场景中的最大魔量
	int curhp_in_scene;						// 当前在场景中的血量
	int curmp_in_scene;						// 当前在场景中的魔量

	Attribute gm_attr_list[BATTLE_ATTR_MAX];// gm属性列表

	int wild_boss_get_reward_times;			// （每日野外boss获取奖励次数） 改成每周野外boss获取奖励次数
	BitMap<PET_COUNT_MAX> get_pet_list;
	GuildID m_guild_id;
	int m_guild_positon;

	unsigned int last_logout_timestamp;		// 上次登出的时间戳

	int				escort_today_count;		// 运镖进入使用次数
	int				escort_task_seq;
	int				escort_npc_id;

	int				trevi_fountain_can_find_count;			// 许愿池可找回次数
	int				chongzhi_count;			// 充值次数
	int				encounter_escort_monster_ms;

	char last_encounter_monster_flag;			// 之前是否遭遇暗怪
	char show_currency_type;					// 玩家显示的充值货币类型（要保存下来是为了保证充值的时候可以准确地读取配置）
	short role_create_count_day;				// 当前为角色创角第几天(此处以0点计算)

	long long wst_question_add_coin;
	long long wst_question_add_exp;
	long long last_encounter_monster_ms;		// 之前遭遇暗怪倒计时

	int trap_creature_times;				// 成功封妖次数
	int pet_join_fight_times;				// 宠物参战次数
	long long today_gold_shop_cost;			// 今天元宝商场消耗元宝数
	int open_treasure_chest_times;			// 水晶副本开启宝箱次数
	int treasure_hunt_times;				// 寻宝次数
	unsigned int offline_exp;				// 离线经验
	int offline_min;						// 离线时长
	int client_spid;						// 客户端新充值需要的spid

	int exp_crytal_remain_times;			// 打卡水晶剩余次数
	char today_patrol_task;				    // 今天是否领取过巡逻任务
	char patrol_task_reward;				// 是否领取过奖励了
	short patrol_task_progress;				// 巡逻任务进度
	short patrol_task_status;				// 领取状态  0:未满足条件  1:已满足未领取 2:已领取
	unsigned int patrol_task_coin;			// 领取经验
	int enter_brave_ground_times;			// 参与勇闯地宫次数
	int partner_pro_level;					// 伙伴的等阶
	Int64 gold_unbind_in_debt;				// 欠下系统的元宝数 （由于退款、bug等原因需要回收时所产生的欠费）
	Int64 coin_bind_in_debt;				// 欠下系统的绑铜数
	BitMap<LAYER_MAX_NUM> fantasy_fb_flag;	// 玩家今天参与四象幻境副本的哪几层(位运算1-4为1-4层)
	int wst_questions_group;				// 题目组  神算子-补
	char today_open_find_fun;				// 今天是否在奖励界面打开了找回功能
	char is_first_create_find_fantasy;		// 首次创建角色后四象找回次数需设置为0  0:未设置 1:已设置
	char is_fetch_boss_chapter_baodi_reward_OBSOLETE;		//玩家今天是否已领取保底奖励	0:未领取 1:已领取
	char is_fetch_boss_chapter_damage_rank_reward;	//玩家今天是否已领取了试炼伤害排名奖励 0:未领取 1:已领取
	unsigned long long xun_bao_first_draw_flag_old;	 //每个寻宝首抽是不是标志(没用)
	int offine_day;							// 玩家本次登陆时间距离上次离线过了几天(此以0点为计算)
	short world_ans;						// 保留字--修改为答题标记
	short bounty_task_find_num;				// 赏金任务剩余找回次数
	unsigned int trevi_fountain_buff_expiration_time[MAX_TREVI_FOUNTION_TODAY_BUFF_NUM];		//!< 许愿池buff的到期时间
	unsigned int escort_end_time;			// 日常活动运镖倒计时结束的时间戳
	//long long	chat_group_list[MAX_ROLE_GROUP];	// 玩家加入的群组id
	long long local_chat_group_list_OBSOLETE[3];	// 本地群聊 
	long long cross_chat_group_list_OBSOLETE[2];	// 跨服群聊id
	long long chat_group_reserve[5];				// 保留5个预留拓展
	BitMap<LAYER_MAX_NUM> find_fantasy_flag;	// 玩家四象找回标识(位运算1-4为1-4层)
	char shi_tu_preferences[SHI_TU_PREFERENCES_NUM];				//师徒偏好设置
	int gladiator_everyday_reward_count;
	int gladiator_everyday_reward_list[OFFLINE_GLADIATOR_EVERYDAY_REWARD_MAX_LIST];

	unsigned int sign_reward_next_month_reset_time;	// 月签到奖励下个月的刷新时间戳
	unsigned short xun_bao_group_draw_times[XUN_BAO_DRAW_CUMULATIVE_TIMES_GROUP_MAX_NUM];
	unsigned int fetch_boss_chapter_baodi_reward_day_id_OBSOLETE;	//家族试炼保底奖励领取天ID
	int brave_ground_max_reached_level;
	int huan_jie_season_num;

	long long chat_id_list[CHAT_CORD_MAX_LIST];

	int shan_hai_boss_challenge_time;		//山海降妖今日挑战次数
	unsigned char shan_hai_boss_soul_fragment[SHAN_HAI_BOSS_MAX_GRADE][SHAN_HAI_BOSS_MAX_TYPE];		//山海降妖boss灵魂碎片（弃用）

	unsigned int world_ans_tamp;//世界答题题目刷新时间戳，为玩家检查答题修复（比世界答题题目刷新时间快10s）
	BitMap<DAY_ACTIVITY_TYPE_MAX> today_find_reward_flag;		//今天活动找回标识 (位运算)哪一位置1:今天使用该活动找回 0:今天没有使用该活动找回
	char colosseum_set_flag;									//此前竞技场排名未存库  2:已设置
	char use_new_pet_status_array;
	char is_reward_sign_month_resert_flag;			//月签到时间重置 0:未重置 1:已重置
	char is_check_train_uplevel_item;

	int next_chong_zhi_record_index;
	int chong_zhi_record[CHONG_ZHI_RECORD_MAX_NUM];

	GameName simple_record_list[QUICK_SPEECH_MAX_NUM];

	short new_pet_status_index[NEW_PET_STATUS_MAX];
	int train_uplevel_comsume_item_num;	 //	修炼技能消耗的数量，转职时候返回
	unsigned int fetch_boss_chapter_rank_reward_day_id;		//家族试炼排名奖励领取天数ID
	unsigned char xun_bao_first_draw_times[XUN_BAO_TREASURE_ID_MAX];			//记录每种类型寻宝未达到首抽奖励的抽奖次数
	unsigned long long xun_bao_first_draw_flag;	 //每个寻宝首抽是不是标志
	Attribute user_attr[BATTLE_ATTR_MAX];		//玩家总属性(不加BUFF)

	unsigned int next_week_timestamp;	// 触发OnWeekChange的时间戳
	unsigned int next_month_timestamp;	// 触发OnMonthChange的时间戳
	unsigned int next_week_6_OClock;	// 触发OnWeekResetModuleData的时间戳
	unsigned int next_month_6_OClock;	// 触发OnMonthResetModuleData的时间戳

	char is_give_task_item_flag;			// 0:未给予 1:已给予 
	char level_complement_flag;				// 新服助力
	short world_answer_idx;					// 保留->>世界答题玩家答案标记
	unsigned short brave_ground_sao_dang_reward_level;
	unsigned short brave_ground_sao_dang_is_fetch_sao_dang_reward;

	long long accumulated_exp;
	unsigned int m_refresh_accumulated_tamp;
	unsigned int m_end_accumulate_tamp;

	int m_day_difference;
	int m_add_exp_times;

	short guild_answer_reply_count;		// 家族答题参与次数
	short guild_answer_find_count;		// 家族答题可找回次数
	unsigned int fetch_special_logic_activity_reward_zero_timestamp[MAX_DAY_SPECIAL_LOGIC_ACTIVITY_TYPE_NUM];	//玩家领取特殊的活动参与奖励当天晚上0点时间戳
	char fetch_guild_fight_join_reward_flag;	//家族大乱斗参与奖励领取标识
	char guild_fight_find_count;				//家族大乱斗可找回次数
	char fetch_only_fight_join_reward_flag;		//以一敌百参与奖励领取标识
	char only_fight_find_count;					//以一敌百可找回次数
	char fetch_team_fight_join_reward_flag;		//竞技对抗赛参与奖励领取标识
	char team_fight_find_count;					//竞技对抗赛可找回次数
	char animation_speed;						// 观看战斗动画的速度
	char is_transfer_chat_group_data;			// 转移本地群组数据到跨服群组的标记
	long long cross_chat_group_list[MAX_ROLE_CROSS_GROUP];
	unsigned int create_chat_group_cd_timestamp;// 创建群组CD时间戳

	long long m_alread_get_exp;
	long long m_save_exp;

	BitMap<MAX_ACTIVITY_TYPE_NUM> participate_activity_flag;		// 每日限时活动参与标识

	long long m_today_accumulated_exp;
	unsigned int brave_ground_sao_dang_cal_reward_time;
	int xun_bao_bless_pet_id_list[MAX_XUN_BAO_BLESS_PET_NUM];

	BitMap<MAX_COLOSSEUM_TASK_NUM> colosseum_task_flag;		// 竞技场任务标示

	unsigned int shan_hai_boss_next_reset_time;				//下次重置一些山海数据的时间
	unsigned int shan_hai_boss_challenge_day_id;			//当前挑战的山海dayID
	short shan_hai_boss_week_challenge_time;				//山海降妖每周挑战次数
	unsigned char shan_hai_boss_is_challenge_seq[SHAN_HAI_BOSS_MAXNUM];		//当前已挑战过的山海索引（不能重复挑战，除非刷新）
	int shan_hai_boss_pass_times;							//角色生涯挑战成功次数
	int cloud_arena_season_highest_score;					// 云端竞技赛季最高积分
	int cloud_arena_season_highest_dan_id;					// 云端竞技赛季最高段位
	int cloud_arena_season_highest_rank;					// 云端竞技赛季最高排名
	int cloud_arena_last_season_rank;						// 云端竞技上赛季排名
	unsigned int  cloud_arena_season_timestamp;				// 云端竞技赛季时间戳
	unsigned int chief_election_login_broadcast_time;			
	unsigned int guild_honor_login_broadcast_time;				
	unsigned short reserved_sh_505[2];

	unsigned int single_chat_num_last_time;					// 上次限制私聊时间戳
	int single_chat_num_list[SINGLE_CHAT_NUM_LIMIT_MAX];	// 限制私聊列表
	
	unsigned int cloud_arena_new_shop_timestamp;			// 云端竞技新商店时间戳

	int fabao_tranf;										// 法宝修改标记
	
	int guild_day_active_reward_flag;						// 帮派日活跃度奖励信息[guildmember下记录主要用于每日结算未领取的]

	char is_refresh_first_notice_top_level_flag;				// 刷新后首次弹出 0:未弹 1:已弹(重置后要有玩家点开这个才开始新的)
	char today_quit_guild_count;								//今日退出家族次数
	short overflow_find_count;									// 今日可找回溢出体力
	BitMap<MAX_ROLE_TOP_LEVEL_DB_NUM> top_level_reward_fetch_flag;	// 巅峰等级奖励领取标识

	unsigned int next_join_guild_timestamp;						//下次允许加入家族时间戳

	int hongbao_times[MAX_HONG_BAO_TYPE_COUNT];					// 记录玩家剩余可发的的红包次数

	char fetch_resource_download_reward_flag;					// 获取资源下载奖励标记
	char is_brave_ground_reset_flag;							// 勇闯地宫提前重置标识 0:未重置 1:已重置
	char fetch_fang_fraud_reward_flag;							// 防诈骗奖励领取标记
	char single_arena_animation_speed;							// 竞技场的看动画速度

	int svip_client_custom_data_1;								// svip客户端保存数据1
	int svip_client_custom_data_2;								// svip客户端保存数据2
	int use_crystal_energy_num;									// 使用体力的数量，用来随机天地道痕道具

	int today_activity_type_find_count[MAX_DAY_ACTIVITY_TYPE_NUM];// 今天已找回次数
	int god_equip_err_flag;
	BitMap<MAX_ROLE_TOP_LEVEL_ACHIEVE_NUM> top_level_achieve_flag;	//巅峰等级成就奖励领取标识

	int get_hongbao_day_times[MAX_HONG_BAO_TYPE_COUNT];			// 每日获得红包次数(可发送红包次数)[每日重置]
	int get_hongbao_total_times[MAX_HONG_BAO_TYPE_COUNT];		// 累计获得红包次数(可发送红包次数)[终身不重置]

	unsigned int next_can_guild_recommend_time;
	unsigned long long xun_bao_second_draw_flag;				// 每个寻宝首抽是不是标志
					
	int colosseum_rank;											// 竞技场排名
	GameName  capacity_target_robot_role_name;
	int  capacity_target_robot_advance_times;
	int  capacity_target_robot_avatar_type;
	int  capacity_target_robot_headshot_id;
	int  capacity_target_robot_color;
	int  capacity_target_robot_prof;
	int  capacity_target_robot_weapon_id;
	int  capacity_target_robot_sub_weapon_id;
	unsigned int  capacity_target_end_time;			//结束时间
	char  capacity_target_can_start;			//完成任务设置为1
	char  capacity_target_is_finish;			//到时间设置为1
	short  capacity_target_is_fetch_reward;				//是否领取了奖励
	int capacity_target_value;				//时间到后的战力
	int capacity_target_robot_value;				//时间到后的对手战力

	int week_refresh_flag;
	int god_turn_bag_flag;

	char is_pet_refine_flag;						// 是否已经洗练过变异 1:已洗练
	char is_set_auto_talk_groups_flag;				// 是否设置过自动对话组ID 0:未设置 1:已设置
	char is_again_set_top_level_refresh_time_flag;	// 是否设置巅峰等级重置时间戳 0:未设置 1:已设置
	char is_move_prestige_data_flag;				// 声望结构体改动,数据迁移标识 0:未迁移 1:已迁移
	int pet_refine_count;							// 宠物洗练次数

	int endless_tower_battle_seq;
	short max_guild_fight_rank;					// 呆过的家族所达到的家族大乱斗最高排名
	short max_guild_honor_rank;					// 呆过的家族所达到的家族荣誉战最高排名
	int guild_hong_bao_fetch_count;				// 家族红包领取次数
	int guild_answer_correct_count;				// 家族答题答对次数
	
	unsigned char today_channel_chat_count_list[chatdef::MAX_CHANNEL_TYPE_COUNT]; //各个频道当天聊天次数记录
	short endless_refresh_flag;//保留-》无尽之塔重置标记
	unsigned short channel_chat_count_list[chatdef::MAX_CHANNEL_TYPE_COUNT];	//各个频道聊天次数记录,MAX_CHANNEL_CHAT_TODAY_COUNT每个频道每天受这个次数限制
	int reserve_int_1_DO_NOT_USE;		// 这个保留值没有被初始化，使用会有风险
	
	BitMap<DAY_REWARD_DAY_FOOD_SEQ_COUNT> day_food_flag;		//午餐晚餐领取标识
	int auto_talk_groups_id;				//自动对话组ID
	int auto_talk_id;						//自动对话ID


	UInt16 gm_soaring_skill_id;
	short gm_soaring_skill_lv;

	int guild_boss_baodi_reward_flag_OBSOLETE;
	unsigned int fetch_boss_chapter_baodi_reward_week_id; // 家族boss保底奖励周ID

	unsigned int endless_tower_battle_tamp;

	BitMap<MAX_SCORE_TASK_NUM> task_finish;			// 任务完成标记
	short reserve_sh11[30];
	int brave_ground_max_reached_level_by_week;	//本周通关的最高层数
	
	int world_team_arena_rank;					//英雄会武角色战队排名(0:没有排名)
	int world_team_arena_title_id;				//英雄会武称号

	int equip_refresh_voc_times;

	short pet_strengthen_reawrd_flag;		//宠物最高强化等级
	short sanxian_title_flag;// 保留-》三仙归洞称号标记
	BitMap<PET_STRENGTHEN_REWARD_FLAG> has_pet_strengthen_flag;			//宠物强化奖励是否已领取
	char brave_ground_reward_reissue_ver;						// 地宫补发奖励标志
	char dislocation_reset_ver_flag;	//一些错位重置版本标识
	char qingyuan_duiduipeng_times;				// 情缘对对碰 每日获得奖励数
	char is_day_fetch_where_is_the_fairy_flag;	// 当天是否领取过仙子去哪儿的奖励 0:未领取

	int san_xian_today_play_times;
	int san_xian_pass_idx;
	unsigned int choose_tamp;//选择限制时间

	int tian_gong_xun_bao_cur_grid;		// 天宫寻宝当前格点
	unsigned int tian_gong_xun_bao_start_timestamp;	//天宫寻宝开启时间戳

	int tu_zi_kuai_pao_reward_times;

	int tian_gong_xun_bao_free_flag;	// 天宫寻宝免费flag

	int guild_build_join_speed_up_count;	// 家族建造参与次数
	char is_old_user_hundred_god_boss_flag;			// 检测老号玩家是否还有资格 0:未检测 1:已检测
	char is_finish_hundred_god_boss_flag;			// 是否完成了百神之师 0:未完成 1:已完成
	short fetch_hundred_god_boss_flag;				// 百神之师达成奖励表示(二进制) 1:已领取
	short hundred_god_boss_fin_bounty_count;		// 百神之师完成十环门派任务次数
	short zao_hua_show_mo_zhuang;					// 保留 -》 标记魔装开启
	unsigned int hundred_god_boss_end_timestamp;	// 百神之师结束时间戳

	int autority_type_test_day_buy_limit;		// 内部玩家交易行购买限制

	char pingjia_reward_mark;				// 评价有礼领取标记
	char re_ch;	// 保留字

	char silk_roads_find_count;	// 丝绸之路可找回次数
	char silk_roads_count;		// 丝绸之路完成次数

	int zao_hua_wear_app_id;
	BitMap<ZAO_HUA_SUIT_APPEARANCE_MAX> zao_hua_app_flag;

	int holy_equip_status_index[HOLY_EQUIP_STATUS_MAX];	// 圣器出战/待机状态,初始化为-1

	char nether_refresh_flag;
	char is_first_manual_pet_xun_bao_flag;				// 是否是首次手动进行普通宠物池子寻宝 
	char is_old_user_first_manual_pet_xun_bao_flag;		// 检测老号是否还有资格进行首次普通宠物池子寻宝
	char team_animation_speed;	
	int zhou_qi_num;
	unsigned int zhou_qi_end_time;

	unsigned int last_logout_timestamp_2;		// 上次登出的时间戳2 (用来双重校验内玩封号）
	int goto_school_get_reward_times;

	int guild_create_consume_coin_bind;		//玩家任意充值获得一次铜币创建家族机会flag,永久仅一次(0-未使用, 1-已使用)
	unsigned int last_set_world_team_aren_timestamp;	//上一次清除或者发放新称号的时间戳(英雄会武)

	int accessories_special_equip_list[ACCESSORIES_SPECIAL_EQUIP_LEVEL];

	int today_add_guaji_exp;		// 每日挂机已获得的经验
	int big_dipper_fight_succ_times;		//北斗七星战胜次数(星君+帝君)

	int receive_real_name_reward_flag;		// 领取实名奖励flag
};
UNSTD_STATIC_CHECK(8 == NEW_PET_STATUS_MAX);
UNSTD_STATIC_CHECK(chatdef::MAX_CHANNEL_TYPE_COUNT >= chatdef::CHANNEL_TYPE_MAX);
UNSTD_STATIC_CHECK(chatdef::MAX_CHANNEL_TYPE_COUNT % 4 == (int)sizeof(short));
UNSTD_STATIC_CHECK(32 == DAY_REWARD_DAY_FOOD_SEQ_COUNT);

static const int MAX_INIT_USERCACHE_COUNT = 512;
class UserCacheParam
{
public:
	UserCacheParam() : level(0), avatar_type(0), headshot_id(0), top_level(0), next_top_level_refresh_timestamp(0), uid(0), profession(0), capability(0),
		last_online_timestamp(0u), avatar_timestamp(0LL)
	{
		memset(role_name, 0, sizeof(role_name));
	}

	short level;
	short avatar_type;				// 虚拟人物类型
	short headshot_id;				// 头像ID					
	short top_level;				// 巅峰等级		
	unsigned int next_top_level_refresh_timestamp;
	int uid;
	int profession;
	int capability;
	unsigned int last_online_timestamp;
	GameName role_name;
	long long avatar_timestamp;		// 用户自上传的头像
	Appearance appearance;

	void GetName(GameName name) const
	{
		if (NULL != name)
		{
			F_STRNCPY(name, role_name, sizeof(GameName));
		}
	}
};

#pragma pack(pop)
typedef char CommonDataParamHex[sizeof(CommonDataParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(CommonDataParamHex) < 8192);
#endif  // __COMMON_DATA_HPP__


