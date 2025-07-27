#ifndef __OFFLINE_REGISTER_PARAM_HPP__
#define __OFFLINE_REGISTER_PARAM_HPP__

#include "common/tlvprotocol.h"
#include "servercommon/serverdef.h"
#include "servercommon/servercommon.h"
#include "servercommon/shitudef.h"
#include "servercommon/miyidef.hpp"
#include "servercommon/struct/global/trademarketparam.hpp"
#include "servercommon/hongbaodef.hpp"
#include "servercommon/roleactivity/rapaimaihangparam.hpp"
#include "servercommon/struct/moneyotherdef.h"

#pragma pack(push, 4)

static const int OFFLINE_CHONGZHI_ARRAY_SIZE = 10;
static const int OFFLINE_GLADIATOR_EVERYDAY_REWARD_MAX_LIST = 3;
static const int MAX_MONEY_TYPE_RESERVED_OFFLINE_NUM = 30;
UNSTD_STATIC_CHECK(MAX_MONEY_TYPE_RESERVED_OFFLINE_NUM >= MAX_MONEY_TYPE_RESERVED);
struct ShiTuBreakRelationInfo
{
	ShiTuBreakRelationInfo()
	{
		this->Reset();
	}

	void Reset()
	{
		uid = 0;
		break_type = 0;
	}

	int uid;
	int break_type;
};

class OfflineRegisterParam
{
public:
	bool Serialize(TLVSerializer &outstream) const;
	bool Unserialize(TLVUnserializer &instream);

	struct OfflineRegisterOtherParam
	{
		OfflineRegisterOtherParam() { this->Reset(); }

		void Reset()
		{
			for (int i = 0; i < SHITU_MAX_APPRENTICE_COUNT + SHITU_MAX_MASTER_COUNT; ++i)
			{
				shi_tu_break_info[i].Reset();			
			}

			memset(reserve_II, 0, sizeof(reserve_II));
			title_obtain = 0;
			add_guild_gongxian = 0;
			chongzhi_count = 0;
			memset(chongzhi_list, 0, sizeof(chongzhi_list));
			kick_out_guild_tamp = 0;
			forbid_talk_seconds = -1;
			retrieve_reason = -1;
			retrieve_unbind_gold = 0;
			retrieve_bind_coin = 0;
			retrieve_item_id = 0;
			retrieve_item_num = 0;
			trademarket_finish = 0;
			colosseum_new_report_flag = 0;
			rand_sports_end = 0;
			gladiator_everyday_count = 0;
			memset(gladiator_everyday_reward, 0, sizeof(gladiator_everyday_reward));
			finish_limit_activity_count = 0;
			memset(sale_item_id, 0, sizeof(sale_item_id));
			create_cross_group_id = 0;
			invalid_index = 0;
			is_fetch_guild_boss_chapter_baodi_reward_OBSOLETE = 0;
			is_fetch_guild_boss_chapter_damage_rank_reward = 0;
			today_role_guild_gather_dream_publish = 0;
			is_finish_ding_ji = -1;
			fetch_boss_chapter_baodi_reward_day_id_OBSOLETE = 0;
			huan_jie_rank_score = 0;
			fetch_boss_chapter_rank_reward_day_id = 0;

			retrieve_pet_cmd_id = 0;
			retrieve_pet_id = 0;
			retrieve_pet_unique_id = 0;
			guild_fight_fetch_reward_zero_timestamp = 0;

			retrieve_exp = 0;
			only_fight_fetch_reward_zero_timestamp = 0;
			team_fight_fetch_reward_zero_timestamp = 0;
			trademarket_purchase_succ_consume_gold = 0;
			trademarket_buy_item_id = 0;
			has_add_friend_event = 0;
			has_get_normal_crystal_fb_reward = 0;
			join_cross_group_id = 0LL;
			credit_num = 0;
			add_cloud_arena_coin_num = 0;
			new_name_cost_flag = 0;
			normal_crystal_fb_reward_seq = -1;
			title_times = 0;
			memset(hongbao_times, 0, sizeof(hongbao_times));
			hongbao_silver_coin = 0;
			memset(ra_last_bid_price, 0, sizeof(ra_last_bid_price));

			duel_type = 0;
			duel_is_attack = -1;
			duel_is_win = -1;
			duel_level_diff = 0;

			max_guild_fight_rank = 0;
			max_guild_honor_rank = 0;

			win_chief_election_prof = 0;
			join_jie_yi_day = 0;
			recevice_flower_count = 0;

			cow_report_good_news_return_gold = 0;

			add_colosseum_coin = 0;
			guild_boss_baodi_reward_flag = 0;
			fetch_boss_chapter_baodi_reward_week_id = 0;
			tiger_brings_blessing_return_gold = 0;

			other_currency_retrieve_reason = -1;
			memset(other_currency, 0, sizeof(other_currency));
			qingyuan_shizhuang_buy_mark = 0;
			new_name_flag = 0;
			qingyuan_leichong_gold = 0;

			world_team_arena_rank = 0;
			world_team_arena_title_id = 0;
			fight_change_card_seq = 0;

			memset(new_name, 0, sizeof(new_name));

			effect_type = -1;
			effect_param = 0;
			effect_param1 = 0;

			floor_back = 0;
			horcrux_tian_cheng_times = 0;

			jixing_gaozhao_floor_back = 0;
			jixing_gaozhao_times = 0;
			add_value = 0;

			forbid_talk_type = FORBID_TALK_TYPE_DEFAULT;
			world_team_arena_timestamp = 0;

			cloud_arena_use_times_timestamp = 0u;
		}

		bool CanRemove()
		{
			for (int i = 0; i < SHITU_MAX_APPRENTICE_COUNT + SHITU_MAX_MASTER_COUNT; ++i)
			{
				if (shi_tu_break_info[i].uid != 0) return false;
			}
			
			for (int i = 0; i < USER_SALE_ITEM_MAX_NUM; ++i)
			{
				if (0 != sale_item_id[i]) return false;
			}

			for (int i = 0; i < MAX_HONG_BAO_TYPE_COUNT && i < HONGBAO_TYPE_MAX; ++i)
			{
				if (0 != hongbao_times[i]) return false;
			}
			
			for (int i = 0; i < ARRAY_LENGTH(ra_last_bid_price); i++)
			{
				if(0 != ra_last_bid_price[i]) return false;
			}

			for (int i = 0; i < ARRAY_LENGTH(other_currency); ++i)
			{
				if (0 != other_currency[i]) return false;
			}

			if (0 != title_obtain ||
				0 != add_guild_gongxian ||
				0 != chongzhi_count ||
				0 != kick_out_guild_tamp ||
				0 != trademarket_finish ||
				0 != colosseum_new_report_flag ||
				-1 != forbid_talk_seconds ||
				0 != retrieve_unbind_gold ||
				0 != retrieve_bind_coin ||
				0 != retrieve_item_id ||
				0 != rand_sports_end ||
				0 != gladiator_everyday_count ||
				0 != finish_limit_activity_count ||
				0 != create_cross_group_id ||
				0 != invalid_index ||
				0 != is_fetch_guild_boss_chapter_damage_rank_reward ||
				0 != today_role_guild_gather_dream_publish ||
				-1 != is_finish_ding_ji ||
				0 != huan_jie_rank_score ||
				0 != fetch_boss_chapter_rank_reward_day_id ||
				0 != retrieve_pet_cmd_id ||
				0 != retrieve_pet_id ||
				0 != retrieve_pet_unique_id ||
				0 != guild_fight_fetch_reward_zero_timestamp ||
				0 != retrieve_exp ||
				0 != only_fight_fetch_reward_zero_timestamp ||
				0 != team_fight_fetch_reward_zero_timestamp || 
				0 != trademarket_purchase_succ_consume_gold || 
				0 != trademarket_buy_item_id || 
				0LL != join_cross_group_id ||
				0 != credit_num ||
				0 != add_cloud_arena_coin_num ||
				0 != has_get_normal_crystal_fb_reward ||
				-1 != normal_crystal_fb_reward_seq ||
				0 != hongbao_silver_coin ||
				0 != duel_type ||
				-1 != duel_is_attack ||
				-1 != duel_is_win ||
				0 != duel_level_diff ||
				0 != max_guild_fight_rank ||
				0 != max_guild_honor_rank || 
				0 != win_chief_election_prof ||
				0 != join_jie_yi_day ||
				0 != recevice_flower_count ||
				0 != add_colosseum_coin ||
				0 != guild_boss_baodi_reward_flag ||
				0 != fetch_boss_chapter_baodi_reward_week_id||
				0 != has_add_friend_event ||
				0 != cow_report_good_news_return_gold ||
				0 != tiger_brings_blessing_return_gold ||
				0 != qingyuan_shizhuang_buy_mark ||
				0 != qingyuan_leichong_gold ||
				0 != world_team_arena_rank ||
				0 != world_team_arena_title_id ||
				0 != fight_change_card_seq ||
				0 != new_name_flag ||
				-1 != effect_type ||
				0 != floor_back ||
				0 != horcrux_tian_cheng_times ||
				0 != jixing_gaozhao_floor_back ||
				0 != jixing_gaozhao_times ||
				0 != add_value || 
				FORBID_TALK_TYPE_DEFAULT != forbid_talk_type ||
				0 != world_team_arena_timestamp ||
				0 != cloud_arena_use_times_timestamp
				) return false;

			return true;
		}

		ShiTuBreakRelationInfo shi_tu_break_info[SHITU_MAX_APPRENTICE_COUNT + SHITU_MAX_MASTER_COUNT];
		int title_obtain;
		long long add_guild_gongxian;
		int chongzhi_count;
		int chongzhi_list[OFFLINE_CHONGZHI_ARRAY_SIZE];
		long long kick_out_guild_tamp;
		int forbid_talk_seconds;
		short retrieve_reason;
		Int64 retrieve_unbind_gold;
		Int64 retrieve_bind_coin;
		ItemID retrieve_item_id;
		short retrieve_item_num;
		short trademarket_finish;
		char colosseum_new_report_flag;
		char rand_sports_end;
		short gladiator_everyday_count;
		int gladiator_everyday_reward[OFFLINE_GLADIATOR_EVERYDAY_REWARD_MAX_LIST];
		char reserve_II[92 * 5];		// 原先的字节数保留
		int finish_limit_activity_count;
		ItemID sale_item_id[USER_SALE_ITEM_MAX_NUM];
		long long create_cross_group_id;
		int invalid_index;
		char is_fetch_guild_boss_chapter_baodi_reward_OBSOLETE;
		char is_fetch_guild_boss_chapter_damage_rank_reward;
		char today_role_guild_gather_dream_publish;
		char is_finish_ding_ji;
		unsigned int fetch_boss_chapter_baodi_reward_day_id_OBSOLETE;	
		int huan_jie_rank_score;
		unsigned int fetch_boss_chapter_rank_reward_day_id;

		int retrieve_pet_cmd_id;
		int retrieve_pet_id;
		long long retrieve_pet_unique_id;
		unsigned int guild_fight_fetch_reward_zero_timestamp;

		long long retrieve_exp;
		unsigned int only_fight_fetch_reward_zero_timestamp;
		unsigned int team_fight_fetch_reward_zero_timestamp;

		char has_add_friend_event;		// 是否有添加好友事件
		char has_get_normal_crystal_fb_reward;
		ItemID trademarket_buy_item_id;
		int trademarket_purchase_succ_consume_gold;

		long long join_cross_group_id;

		int credit_num;
		int add_cloud_arena_coin_num;

		short normal_crystal_fb_reward_seq;
		short new_name_cost_flag;//保留 -》 cmd修改玩家名字是否需要消耗对应道具

		unsigned int title_times;

		int hongbao_times[MAX_HONG_BAO_TYPE_COUNT];	
		int hongbao_silver_coin;

		int ra_last_bid_price[MAX_PAI_MAI_HANG_DAY_SHOP_ITEM_NUM];

		char duel_type;
		char duel_is_attack;
		char duel_is_win;
		unsigned char duel_level_diff;

		short max_guild_fight_rank;					// 呆过的家族所达到的家族大乱斗最高排名
		short max_guild_honor_rank;					// 呆过的家族所达到的家族荣誉战最高排名

		short win_chief_election_prof;				
		short join_jie_yi_day;
		int recevice_flower_count;					// 收到好友的花次数

		int cow_report_good_news_return_gold;

		int add_colosseum_coin;
		int guild_boss_baodi_reward_flag;
		unsigned int fetch_boss_chapter_baodi_reward_week_id;

		int tiger_brings_blessing_return_gold;

		int other_currency_retrieve_reason;
		long long other_currency[MAX_MONEY_TYPE_RESERVED_OFFLINE_NUM];		// 回收其它货币

		short qingyuan_shizhuang_buy_mark;
		short new_name_flag;//保留-》cmd修改名字标记
		
		int qingyuan_leichong_gold;
		int world_team_arena_rank;
		int world_team_arena_title_id;
		int fight_change_card_seq;

		GameName new_name;

		int effect_type;
		int effect_param;
		int effect_param1;

		int floor_back;
		int horcrux_tian_cheng_times;

		int jixing_gaozhao_floor_back;
		int jixing_gaozhao_times;

		int add_value;
		int forbid_talk_type;		//禁言模式\see FORBID_TALK_TYPE_MAX
		int world_team_arena_timestamp;

		unsigned int cloud_arena_use_times_timestamp;
	};

	struct OfflineRegisterAttr
	{
		long long id_offline_register;			// 仅仅初始化的时候有用 
		char change_state; 

		int role_id;
		unsigned int last_divorce_time;
		int chongzhi_cmd_flag;
		OfflineRegisterOtherParam other_param;
	};

	static const int MAX_OFFLINE_REGISTER_COUNT = 256;

	int count;
	OfflineRegisterAttr offline_register_list[MAX_OFFLINE_REGISTER_COUNT];
};

#pragma pack(pop)

typedef char OfflineRegisterOtherParamHex[sizeof(OfflineRegisterParam::OfflineRegisterOtherParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(OfflineRegisterOtherParamHex) < 4096);

#endif  // __OFFLINE_REGISTER_PARAM_HPP__