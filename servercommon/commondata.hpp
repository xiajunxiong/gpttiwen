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
	NEW_FOLLOW_INDEX = 0,		// �������index
	NEW_FIGHT_INDEX_BEGIN,	 		// 
	NEW_FIGHT_INDEX= NEW_FIGHT_INDEX_BEGIN,	 		// ��ս����index
	NEW_STANDBY_INDEX1,	 		// ��������index1
	NEW_STANDBY_INDEX2,	 		// ��������index2
	NEW_STANDBY_INDEX3,	 		// ��������index3
	NEW_STANDBY_INDEX4,			// ��������index4
	NEW_STANDBY_INDEX5,			// ��������index5
	NEW_FIGHT_INDEX_END= NEW_STANDBY_INDEX5,			// 
	NEW_RESERVED,				// ����
	NEW_PET_STATUS_MAX,
};

enum PET_STATUS_OBSOLETE			// �ѷ�������NEW_PET_STATUS
{
	FOLLOW_INDEX_OBSOLETE = 0,		// �������index
	FIGHT_INDEX_OBSOLETE,	 		// ��ս����index
	STANDBY_INDEX1_OBSOLETE,	 	// ��������index1
	STANDBY_INDEX2_OBSOLETE,	 	// ��������index2
	STANDBY_INDEX3_OBSOLETE,	 	// ��������index3
	PET_STATUS_MAX_OBSOLETE,
};

enum HOLY_EQUIP_STATUS
{
	HOLY_EQUIP_STATUS_BEGIN = 0,

	HOLY_EQUIP_STATUS_INDEX0 = HOLY_EQUIP_STATUS_BEGIN,	// ��ս/����ʥ��index
	HOLY_EQUIP_STATUS_INDEX1,		// ��ս/����ʥ��index1
	HOLY_EQUIP_STATUS_INDEX2,		// ��ս/����ʥ��index2
	HOLY_EQUIP_STATUS_INDEX3,		// ��ս/����ʥ��index3
	HOLY_EQUIP_STATUS_INDEX4,		// ��ս/����ʥ��index4

	HOLY_EQUIP_STATUS_MAX = 5,
};

static const int MAX_DAY_ACTIVITY_TYPE_NUM = 32;
enum DAY_ACTIVITY_TYPE
{
	DAY_ACTIVITY_TYPE_MIN = 0,

	DAY_ACTIVITY_TYPE_ZHEN_ZHONG				= 1,					//	���е��о� / �����ħ
	DAY_ACTIVITY_TYPE_TREASURE_MAP_TASK		    = 2,					//	��ͼ���� / ��������
	DAY_ACTIVITY_TYPE_SCHOOL_TASK				= 3,					//	ѧԺ���� / ��������
	DAY_ACTIVITY_TYPE_WST_QUESTION				= 4,					//	����ͨ�Ŀ��� / �����ӵĿ���
	DAY_ACTIVITY_TYPE_TREVI_FOUNTAIN			= 5,					//	��������Ը�� / ��Ը��
	DAY_ACTIVITY_TYPE_BOUNTY_TASK				= 6,					//  �ͽ�����
	DAY_ACTIVITY_TYPE_FANTASY_FB				= 7,					//  ����þ�
	DAY_ACTIVITY_TYPE_TRANSACTION_TASK			= 8,					//  ˿��֮·(�ѷ���, Զ��ó��/��������)
	DAY_ACTIVITY_TYPE_GUILD_ANSWER				= 9,					//	������}
	DAY_ACTIVITY_TYPE_GUILD_FIGHT				= 10,					//	������Ҷ�
	DAY_ACTIVITY_TYPE_ONLY_FIGHT				= 11,					//	��һ�а�
	DAY_ACTIVITY_TYPE_TEAM_FIGHT				= 12,					//  �����Կ���
	DAY_ACTIVITY_TYPE_OVERFLOW_ENERGY			= 13,					//	�ؾ��������
	DAY_ACTIVITY_TYPE_WAN_LING_FB_ACTIVE		= 14,					//  ���黰��(��Ծ)
	DAY_ACTIVITY_TYPE_CRYSTAL_XI_YOU_ACTIVE		= 15,					//  ��Ӣ����(��Ծ)
	DAY_ACTIVITY_TYPE_CLOUD_ARENA_ACTIVE		= 16,					//  񷶥֮��(��Ծ)
	DAY_ACTIVITY_TYPE_ZHEN_ZHONG_ACTIVE			= 17,					//	���е��о� / �����ħ(��Ծ)
	DAY_ACTIVITY_TYPE_BOUNTY_TASK_ACTIVE		= 18,					//  �ͽ�����(��Ծ)
	DAY_ACTIVITY_TYPE_SILK_ROADS_ACTIVE			= 19,					//  �� ˿��֮·

	DAY_ACTIVITY_TYPE_MAX,
};
UNSTD_STATIC_CHECK(DAY_ACTIVITY_TYPE_MAX == 20 && DAY_ACTIVITY_TYPE_MAX <= MAX_DAY_ACTIVITY_TYPE_NUM);

enum REWARD_BACK_FIND_TYPE
{
	RB_TYPE_COIN = 0,		// ͭ���һ�
	RB_TYPE_GOLD = 1,		// Ԫ���һ�

	RB_TYPE_MAX
};

static const int MAX_CLIENT_BEHAVIOR_TYPE_NUM = 64;
enum CLIENT_BEHAVIOR_TYPE
{
	CLIENT_BEHAVIOR_TYPE_OPEN_CHONGZHI = 0,		//�ͻ��˴򿪳�ֵ����

	CLIENT_BEHAVIOR_TYPE_MAX
};

UNSTD_STATIC_CHECK(MAX_CLIENT_BEHAVIOR_TYPE_NUM >= CLIENT_BEHAVIOR_TYPE_MAX);

static const int MAX_TREVI_FOUNTION_TODAY_BUFF_NUM = 3;
static const int LAYER_MAX_NUM = 4;						//��ǰ������(����þ�)

static const int SHI_TU_PREFERENCES_NUM = 4;						//ʦͽƫ������ѡ����
static const int CHAT_CORD_MAX_LIST = 8;

static const int  CHONG_ZHI_RECORD_MAX_NUM = 5;

static const int QUICK_SPEECH_MAX_NUM = 5;// ��ݷ���

const static int PER_DAY_SECOND = 3600 * 24;	//һ�������
static const int MAX_XUN_BAO_BLESS_PET_NUM = 5;	

const static int MAX_COLOSSEUM_TASK_NUM = 256;	// �����ɾ��������

const static int SINGLE_CHAT_NUM_LIMIT_MAX = 10;  //˽�������������

static const int MAX_ROLE_TOP_LEVEL_DB_NUM = 1024;		// ����������
static const int MAX_ROLE_TOP_LEVEL_NUM = 999;		// �۷�ȼ�����
static const int MAX_ROLE_TOP_LEVEL_ACHIEVE_NUM = 32;	// �۷�ȼ��ɾͽ�����������
UNSTD_STATIC_CHECK(MAX_ROLE_TOP_LEVEL_DB_NUM >= MAX_ROLE_TOP_LEVEL_NUM);

static const int ONE_DAY_QUIT_GUILD_LIMIT_NUM = 2;	//ÿ���˳�����������ƣ���ʼCD��
static const int ONE_DAY_QUIT_GUILD_LIMIT_TIME = 8 * 3600;	//��������CDʱ��

static const int DAY_REWARD_DAY_FOOD_SEQ_COUNT = 32;

static const int MAX_SCORE_TASK_NUM = 32;
static const int PET_STRENGTHEN_REWARD_FLAG = 32;

const int static BRAVE_GROUND_ROLE_REWARD_REISSUE_VER = 2;
const int static DISLOCATION_RESET_VER_FLAG = 1;

const static int ZAO_HUA_SUIT_APPEARANCE_MAX = 80;	//�컯װ��������

const static int ACCESSORIES_SPECIAL_EQUIP_LEVEL = 2;// ���� - ����

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

		is_give_task_item_flag = 0;			// 0:δ���� 1:�Ѹ��� 
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
		capacity_target_can_start = 0;			//�����������Ϊ1
		capacity_target_is_finish = 0;			//��ʱ������Ϊ1
		capacity_target_is_fetch_reward = 0;				//�Ƿ���ȡ�˽���
		capacity_target_value = 0;				//ʱ�䵽���ս��

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

	int			plat_spid;									// ƽ̨����id

	short		pet_status_index_OBSOLETE[PET_STATUS_MAX_OBSOLETE]; // �ѷ���  ��new_pet_status_index
	unsigned char reward_first_flag;						// ����-���׳���
	char		battle_is_auto;								// ս���Ƿ��Զ�
	char		initial_battle_flag;						// ��ʼս�����
	char		randactivity_compensation_flag;				// ����������
	char		gm_check_waigua_flag;
	char		reserve_ch;

	// ����ͨ�Ŀ���(����)
	int					wst_questions_id;								// ��Ŀid
	char				wst_question_today_buy_exp_count;				// �ܹ���Ľ��ս�������
	char				wst_question_yesterday_buy_exp_count;			// �ܹ�������ս�������
	unsigned short		wst_answer_num;									// ����Ĵ�������

	short		battle_auto_first_move_type;
	UInt16		battle_auto_first_skill_id;
	short		battle_auto_first_skill_level;
	short		battle_auto_second_move_type;
	UInt16		battle_auto_second_skill_id;
	short		battle_auto_second_skill_level;

	unsigned short		trevi_fountain_today_use_count;												//!< ��Ը�ؽ���ʹ�ô���
	unsigned short		trevi_fountain_buff_id[MAX_TREVI_FOUNTION_TODAY_BUFF_NUM];					//!< ��Ը�ػ�õ�buff_id

	short online_reward_fetch_flag;			// ���߽�����ȡflag ÿ��0�����
	short sign_count;						// ÿ��ǩ������ ǩ������ ÿ��1�����
	int sign_reward_fetch_flag;				// ÿ��ǩ������ ��ȡflag ÿ��1�����
	int level_reward_fetch_flag;			// �ȼ����� ��ȡflag

	char battle_position;					// ս��ʱ��վλ��0������   1��ǰ�ţ�
	char is_open_exp_crytal;				// �Ƿ�����ˮ��

	short wst_answer_yesterday_num;			// ����ͨ�Ĵ��� ����û�������

	int system_notice_fetch_flag;			// ����Ԥ�潱����ȡ���

	long long encounter_monster_interval_ms;// ʣ����������ʱ�䣨���룩
	char encounter_monster_flag;			// �Ƿ���������

	char is_in_no_recover_fb;				// �Ƿ���Ѫ��ħ�����ܻظ��ĸ���
	short change_career_times;				// ְҵ�任����

	int maxhp_in_scene;						// �ڳ����е����Ѫ��
	int maxmp_in_scene;						// �ڳ����е����ħ��
	int curhp_in_scene;						// ��ǰ�ڳ����е�Ѫ��
	int curmp_in_scene;						// ��ǰ�ڳ����е�ħ��

	Attribute gm_attr_list[BATTLE_ATTR_MAX];// gm�����б�

	int wild_boss_get_reward_times;			// ��ÿ��Ұ��boss��ȡ���������� �ĳ�ÿ��Ұ��boss��ȡ��������
	BitMap<PET_COUNT_MAX> get_pet_list;
	GuildID m_guild_id;
	int m_guild_positon;

	unsigned int last_logout_timestamp;		// �ϴεǳ���ʱ���

	int				escort_today_count;		// ���ڽ���ʹ�ô���
	int				escort_task_seq;
	int				escort_npc_id;

	int				trevi_fountain_can_find_count;			// ��Ը�ؿ��һش���
	int				chongzhi_count;			// ��ֵ����
	int				encounter_escort_monster_ms;

	char last_encounter_monster_flag;			// ֮ǰ�Ƿ���������
	char show_currency_type;					// �����ʾ�ĳ�ֵ�������ͣ�Ҫ����������Ϊ�˱�֤��ֵ��ʱ�����׼ȷ�ض�ȡ���ã�
	short role_create_count_day;				// ��ǰΪ��ɫ���ǵڼ���(�˴���0�����)

	long long wst_question_add_coin;
	long long wst_question_add_exp;
	long long last_encounter_monster_ms;		// ֮ǰ�������ֵ���ʱ

	int trap_creature_times;				// �ɹ���������
	int pet_join_fight_times;				// �����ս����
	long long today_gold_shop_cost;			// ����Ԫ���̳�����Ԫ����
	int open_treasure_chest_times;			// ˮ�����������������
	int treasure_hunt_times;				// Ѱ������
	unsigned int offline_exp;				// ���߾���
	int offline_min;						// ����ʱ��
	int client_spid;						// �ͻ����³�ֵ��Ҫ��spid

	int exp_crytal_remain_times;			// ��ˮ��ʣ�����
	char today_patrol_task;				    // �����Ƿ���ȡ��Ѳ������
	char patrol_task_reward;				// �Ƿ���ȡ��������
	short patrol_task_progress;				// Ѳ���������
	short patrol_task_status;				// ��ȡ״̬  0:δ��������  1:������δ��ȡ 2:����ȡ
	unsigned int patrol_task_coin;			// ��ȡ����
	int enter_brave_ground_times;			// �����´��ع�����
	int partner_pro_level;					// ���ĵȽ�
	Int64 gold_unbind_in_debt;				// Ƿ��ϵͳ��Ԫ���� �������˿bug��ԭ����Ҫ����ʱ��������Ƿ�ѣ�
	Int64 coin_bind_in_debt;				// Ƿ��ϵͳ�İ�ͭ��
	BitMap<LAYER_MAX_NUM> fantasy_fb_flag;	// ��ҽ����������þ��������ļ���(λ����1-4Ϊ1-4��)
	int wst_questions_group;				// ��Ŀ��  ������-��
	char today_open_find_fun;				// �����Ƿ��ڽ�����������һع���
	char is_first_create_find_fantasy;		// �״δ�����ɫ�������һش���������Ϊ0  0:δ���� 1:������
	char is_fetch_boss_chapter_baodi_reward_OBSOLETE;		//��ҽ����Ƿ�����ȡ���׽���	0:δ��ȡ 1:����ȡ
	char is_fetch_boss_chapter_damage_rank_reward;	//��ҽ����Ƿ�����ȡ�������˺��������� 0:δ��ȡ 1:����ȡ
	unsigned long long xun_bao_first_draw_flag_old;	 //ÿ��Ѱ���׳��ǲ��Ǳ�־(û��)
	int offine_day;							// ��ұ��ε�½ʱ������ϴ����߹��˼���(����0��Ϊ����)
	short world_ans;						// ������--�޸�Ϊ������
	short bounty_task_find_num;				// �ͽ�����ʣ���һش���
	unsigned int trevi_fountain_buff_expiration_time[MAX_TREVI_FOUNTION_TODAY_BUFF_NUM];		//!< ��Ը��buff�ĵ���ʱ��
	unsigned int escort_end_time;			// �ճ�����ڵ���ʱ������ʱ���
	//long long	chat_group_list[MAX_ROLE_GROUP];	// ��Ҽ����Ⱥ��id
	long long local_chat_group_list_OBSOLETE[3];	// ����Ⱥ�� 
	long long cross_chat_group_list_OBSOLETE[2];	// ���Ⱥ��id
	long long chat_group_reserve[5];				// ����5��Ԥ����չ
	BitMap<LAYER_MAX_NUM> find_fantasy_flag;	// ��������һر�ʶ(λ����1-4Ϊ1-4��)
	char shi_tu_preferences[SHI_TU_PREFERENCES_NUM];				//ʦͽƫ������
	int gladiator_everyday_reward_count;
	int gladiator_everyday_reward_list[OFFLINE_GLADIATOR_EVERYDAY_REWARD_MAX_LIST];

	unsigned int sign_reward_next_month_reset_time;	// ��ǩ�������¸��µ�ˢ��ʱ���
	unsigned short xun_bao_group_draw_times[XUN_BAO_DRAW_CUMULATIVE_TIMES_GROUP_MAX_NUM];
	unsigned int fetch_boss_chapter_baodi_reward_day_id_OBSOLETE;	//�����������׽�����ȡ��ID
	int brave_ground_max_reached_level;
	int huan_jie_season_num;

	long long chat_id_list[CHAT_CORD_MAX_LIST];

	int shan_hai_boss_challenge_time;		//ɽ������������ս����
	unsigned char shan_hai_boss_soul_fragment[SHAN_HAI_BOSS_MAX_GRADE][SHAN_HAI_BOSS_MAX_TYPE];		//ɽ������boss�����Ƭ�����ã�

	unsigned int world_ans_tamp;//���������Ŀˢ��ʱ�����Ϊ��Ҽ������޸��������������Ŀˢ��ʱ���10s��
	BitMap<DAY_ACTIVITY_TYPE_MAX> today_find_reward_flag;		//�����һر�ʶ (λ����)��һλ��1:����ʹ�øû�һ� 0:����û��ʹ�øû�һ�
	char colosseum_set_flag;									//��ǰ����������δ���  2:������
	char use_new_pet_status_array;
	char is_reward_sign_month_resert_flag;			//��ǩ��ʱ������ 0:δ���� 1:������
	char is_check_train_uplevel_item;

	int next_chong_zhi_record_index;
	int chong_zhi_record[CHONG_ZHI_RECORD_MAX_NUM];

	GameName simple_record_list[QUICK_SPEECH_MAX_NUM];

	short new_pet_status_index[NEW_PET_STATUS_MAX];
	int train_uplevel_comsume_item_num;	 //	�����������ĵ�������תְʱ�򷵻�
	unsigned int fetch_boss_chapter_rank_reward_day_id;		//������������������ȡ����ID
	unsigned char xun_bao_first_draw_times[XUN_BAO_TREASURE_ID_MAX];			//��¼ÿ������Ѱ��δ�ﵽ�׳齱���ĳ齱����
	unsigned long long xun_bao_first_draw_flag;	 //ÿ��Ѱ���׳��ǲ��Ǳ�־
	Attribute user_attr[BATTLE_ATTR_MAX];		//���������(����BUFF)

	unsigned int next_week_timestamp;	// ����OnWeekChange��ʱ���
	unsigned int next_month_timestamp;	// ����OnMonthChange��ʱ���
	unsigned int next_week_6_OClock;	// ����OnWeekResetModuleData��ʱ���
	unsigned int next_month_6_OClock;	// ����OnMonthResetModuleData��ʱ���

	char is_give_task_item_flag;			// 0:δ���� 1:�Ѹ��� 
	char level_complement_flag;				// �·�����
	short world_answer_idx;					// ����->>���������Ҵ𰸱��
	unsigned short brave_ground_sao_dang_reward_level;
	unsigned short brave_ground_sao_dang_is_fetch_sao_dang_reward;

	long long accumulated_exp;
	unsigned int m_refresh_accumulated_tamp;
	unsigned int m_end_accumulate_tamp;

	int m_day_difference;
	int m_add_exp_times;

	short guild_answer_reply_count;		// �������������
	short guild_answer_find_count;		// ���������һش���
	unsigned int fetch_special_logic_activity_reward_zero_timestamp[MAX_DAY_SPECIAL_LOGIC_ACTIVITY_TYPE_NUM];	//�����ȡ����Ļ���뽱����������0��ʱ���
	char fetch_guild_fight_join_reward_flag;	//������Ҷ����뽱����ȡ��ʶ
	char guild_fight_find_count;				//������Ҷ����һش���
	char fetch_only_fight_join_reward_flag;		//��һ�аٲ��뽱����ȡ��ʶ
	char only_fight_find_count;					//��һ�аٿ��һش���
	char fetch_team_fight_join_reward_flag;		//�����Կ������뽱����ȡ��ʶ
	char team_fight_find_count;					//�����Կ������һش���
	char animation_speed;						// �ۿ�ս���������ٶ�
	char is_transfer_chat_group_data;			// ת�Ʊ���Ⱥ�����ݵ����Ⱥ��ı��
	long long cross_chat_group_list[MAX_ROLE_CROSS_GROUP];
	unsigned int create_chat_group_cd_timestamp;// ����Ⱥ��CDʱ���

	long long m_alread_get_exp;
	long long m_save_exp;

	BitMap<MAX_ACTIVITY_TYPE_NUM> participate_activity_flag;		// ÿ����ʱ������ʶ

	long long m_today_accumulated_exp;
	unsigned int brave_ground_sao_dang_cal_reward_time;
	int xun_bao_bless_pet_id_list[MAX_XUN_BAO_BLESS_PET_NUM];

	BitMap<MAX_COLOSSEUM_TASK_NUM> colosseum_task_flag;		// �����������ʾ

	unsigned int shan_hai_boss_next_reset_time;				//�´�����һЩɽ�����ݵ�ʱ��
	unsigned int shan_hai_boss_challenge_day_id;			//��ǰ��ս��ɽ��dayID
	short shan_hai_boss_week_challenge_time;				//ɽ������ÿ����ս����
	unsigned char shan_hai_boss_is_challenge_seq[SHAN_HAI_BOSS_MAXNUM];		//��ǰ����ս����ɽ�������������ظ���ս������ˢ�£�
	int shan_hai_boss_pass_times;							//��ɫ������ս�ɹ�����
	int cloud_arena_season_highest_score;					// �ƶ˾���������߻���
	int cloud_arena_season_highest_dan_id;					// �ƶ˾���������߶�λ
	int cloud_arena_season_highest_rank;					// �ƶ˾��������������
	int cloud_arena_last_season_rank;						// �ƶ˾�������������
	unsigned int  cloud_arena_season_timestamp;				// �ƶ˾�������ʱ���
	unsigned int chief_election_login_broadcast_time;			
	unsigned int guild_honor_login_broadcast_time;				
	unsigned short reserved_sh_505[2];

	unsigned int single_chat_num_last_time;					// �ϴ�����˽��ʱ���
	int single_chat_num_list[SINGLE_CHAT_NUM_LIMIT_MAX];	// ����˽���б�
	
	unsigned int cloud_arena_new_shop_timestamp;			// �ƶ˾������̵�ʱ���

	int fabao_tranf;										// �����޸ı��
	
	int guild_day_active_reward_flag;						// �����ջ�Ծ�Ƚ�����Ϣ[guildmember�¼�¼��Ҫ����ÿ�ս���δ��ȡ��]

	char is_refresh_first_notice_top_level_flag;				// ˢ�º��״ε��� 0:δ�� 1:�ѵ�(���ú�Ҫ����ҵ㿪����ſ�ʼ�µ�)
	char today_quit_guild_count;								//�����˳��������
	short overflow_find_count;									// ���տ��һ��������
	BitMap<MAX_ROLE_TOP_LEVEL_DB_NUM> top_level_reward_fetch_flag;	// �۷�ȼ�������ȡ��ʶ

	unsigned int next_join_guild_timestamp;						//�´�����������ʱ���

	int hongbao_times[MAX_HONG_BAO_TYPE_COUNT];					// ��¼���ʣ��ɷ��ĵĺ������

	char fetch_resource_download_reward_flag;					// ��ȡ��Դ���ؽ������
	char is_brave_ground_reset_flag;							// �´��ع���ǰ���ñ�ʶ 0:δ���� 1:������
	char fetch_fang_fraud_reward_flag;							// ��թƭ������ȡ���
	char single_arena_animation_speed;							// �������Ŀ������ٶ�

	int svip_client_custom_data_1;								// svip�ͻ��˱�������1
	int svip_client_custom_data_2;								// svip�ͻ��˱�������2
	int use_crystal_energy_num;									// ʹ�����������������������ص��۵���

	int today_activity_type_find_count[MAX_DAY_ACTIVITY_TYPE_NUM];// �������һش���
	int god_equip_err_flag;
	BitMap<MAX_ROLE_TOP_LEVEL_ACHIEVE_NUM> top_level_achieve_flag;	//�۷�ȼ��ɾͽ�����ȡ��ʶ

	int get_hongbao_day_times[MAX_HONG_BAO_TYPE_COUNT];			// ÿ�ջ�ú������(�ɷ��ͺ������)[ÿ������]
	int get_hongbao_total_times[MAX_HONG_BAO_TYPE_COUNT];		// �ۼƻ�ú������(�ɷ��ͺ������)[��������]

	unsigned int next_can_guild_recommend_time;
	unsigned long long xun_bao_second_draw_flag;				// ÿ��Ѱ���׳��ǲ��Ǳ�־
					
	int colosseum_rank;											// ����������
	GameName  capacity_target_robot_role_name;
	int  capacity_target_robot_advance_times;
	int  capacity_target_robot_avatar_type;
	int  capacity_target_robot_headshot_id;
	int  capacity_target_robot_color;
	int  capacity_target_robot_prof;
	int  capacity_target_robot_weapon_id;
	int  capacity_target_robot_sub_weapon_id;
	unsigned int  capacity_target_end_time;			//����ʱ��
	char  capacity_target_can_start;			//�����������Ϊ1
	char  capacity_target_is_finish;			//��ʱ������Ϊ1
	short  capacity_target_is_fetch_reward;				//�Ƿ���ȡ�˽���
	int capacity_target_value;				//ʱ�䵽���ս��
	int capacity_target_robot_value;				//ʱ�䵽��Ķ���ս��

	int week_refresh_flag;
	int god_turn_bag_flag;

	char is_pet_refine_flag;						// �Ƿ��Ѿ�ϴ�������� 1:��ϴ��
	char is_set_auto_talk_groups_flag;				// �Ƿ����ù��Զ��Ի���ID 0:δ���� 1:������
	char is_again_set_top_level_refresh_time_flag;	// �Ƿ������۷�ȼ�����ʱ��� 0:δ���� 1:������
	char is_move_prestige_data_flag;				// �����ṹ��Ķ�,����Ǩ�Ʊ�ʶ 0:δǨ�� 1:��Ǩ��
	int pet_refine_count;							// ����ϴ������

	int endless_tower_battle_seq;
	short max_guild_fight_rank;					// �����ļ������ﵽ�ļ�����Ҷ��������
	short max_guild_honor_rank;					// �����ļ������ﵽ�ļ�������ս�������
	int guild_hong_bao_fetch_count;				// ��������ȡ����
	int guild_answer_correct_count;				// ��������Դ���
	
	unsigned char today_channel_chat_count_list[chatdef::MAX_CHANNEL_TYPE_COUNT]; //����Ƶ���������������¼
	short endless_refresh_flag;//����-���޾�֮�����ñ��
	unsigned short channel_chat_count_list[chatdef::MAX_CHANNEL_TYPE_COUNT];	//����Ƶ�����������¼,MAX_CHANNEL_CHAT_TODAY_COUNTÿ��Ƶ��ÿ���������������
	int reserve_int_1_DO_NOT_USE;		// �������ֵû�б���ʼ����ʹ�û��з���
	
	BitMap<DAY_REWARD_DAY_FOOD_SEQ_COUNT> day_food_flag;		//��������ȡ��ʶ
	int auto_talk_groups_id;				//�Զ��Ի���ID
	int auto_talk_id;						//�Զ��Ի�ID


	UInt16 gm_soaring_skill_id;
	short gm_soaring_skill_lv;

	int guild_boss_baodi_reward_flag_OBSOLETE;
	unsigned int fetch_boss_chapter_baodi_reward_week_id; // ����boss���׽�����ID

	unsigned int endless_tower_battle_tamp;

	BitMap<MAX_SCORE_TASK_NUM> task_finish;			// ������ɱ��
	short reserve_sh11[30];
	int brave_ground_max_reached_level_by_week;	//����ͨ�ص���߲���
	
	int world_team_arena_rank;					//Ӣ�ۻ����ɫս������(0:û������)
	int world_team_arena_title_id;				//Ӣ�ۻ���ƺ�

	int equip_refresh_voc_times;

	short pet_strengthen_reawrd_flag;		//�������ǿ���ȼ�
	short sanxian_title_flag;// ����-�����ɹ鶴�ƺű��
	BitMap<PET_STRENGTHEN_REWARD_FLAG> has_pet_strengthen_flag;			//����ǿ�������Ƿ�����ȡ
	char brave_ground_reward_reissue_ver;						// �ع�����������־
	char dislocation_reset_ver_flag;	//һЩ��λ���ð汾��ʶ
	char qingyuan_duiduipeng_times;				// ��Ե�Զ��� ÿ�ջ�ý�����
	char is_day_fetch_where_is_the_fairy_flag;	// �����Ƿ���ȡ������ȥ�Ķ��Ľ��� 0:δ��ȡ

	int san_xian_today_play_times;
	int san_xian_pass_idx;
	unsigned int choose_tamp;//ѡ������ʱ��

	int tian_gong_xun_bao_cur_grid;		// �칬Ѱ����ǰ���
	unsigned int tian_gong_xun_bao_start_timestamp;	//�칬Ѱ������ʱ���

	int tu_zi_kuai_pao_reward_times;

	int tian_gong_xun_bao_free_flag;	// �칬Ѱ�����flag

	int guild_build_join_speed_up_count;	// ���彨��������
	char is_old_user_hundred_god_boss_flag;			// ����Ϻ�����Ƿ����ʸ� 0:δ��� 1:�Ѽ��
	char is_finish_hundred_god_boss_flag;			// �Ƿ�����˰���֮ʦ 0:δ��� 1:�����
	short fetch_hundred_god_boss_flag;				// ����֮ʦ��ɽ�����ʾ(������) 1:����ȡ
	short hundred_god_boss_fin_bounty_count;		// ����֮ʦ���ʮ�������������
	short zao_hua_show_mo_zhuang;					// ���� -�� ���ħװ����
	unsigned int hundred_god_boss_end_timestamp;	// ����֮ʦ����ʱ���

	int autority_type_test_day_buy_limit;		// �ڲ���ҽ����й�������

	char pingjia_reward_mark;				// ����������ȡ���
	char re_ch;	// ������

	char silk_roads_find_count;	// ˿��֮·���һش���
	char silk_roads_count;		// ˿��֮·��ɴ���

	int zao_hua_wear_app_id;
	BitMap<ZAO_HUA_SUIT_APPEARANCE_MAX> zao_hua_app_flag;

	int holy_equip_status_index[HOLY_EQUIP_STATUS_MAX];	// ʥ����ս/����״̬,��ʼ��Ϊ-1

	char nether_refresh_flag;
	char is_first_manual_pet_xun_bao_flag;				// �Ƿ����״��ֶ�������ͨ�������Ѱ�� 
	char is_old_user_first_manual_pet_xun_bao_flag;		// ����Ϻ��Ƿ����ʸ�����״���ͨ�������Ѱ��
	char team_animation_speed;	
	int zhou_qi_num;
	unsigned int zhou_qi_end_time;

	unsigned int last_logout_timestamp_2;		// �ϴεǳ���ʱ���2 (����˫��У�������ţ�
	int goto_school_get_reward_times;

	int guild_create_consume_coin_bind;		//��������ֵ���һ��ͭ�Ҵ����������flag,���ý�һ��(0-δʹ��, 1-��ʹ��)
	unsigned int last_set_world_team_aren_timestamp;	//��һ��������߷����³ƺŵ�ʱ���(Ӣ�ۻ���)

	int accessories_special_equip_list[ACCESSORIES_SPECIAL_EQUIP_LEVEL];

	int today_add_guaji_exp;		// ÿ�չһ��ѻ�õľ���
	int big_dipper_fight_succ_times;		//��������սʤ����(�Ǿ�+�۾�)

	int receive_real_name_reward_flag;		// ��ȡʵ������flag
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
	short avatar_type;				// ������������
	short headshot_id;				// ͷ��ID					
	short top_level;				// �۷�ȼ�		
	unsigned int next_top_level_refresh_timestamp;
	int uid;
	int profession;
	int capability;
	unsigned int last_online_timestamp;
	GameName role_name;
	long long avatar_timestamp;		// �û����ϴ���ͷ��
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


