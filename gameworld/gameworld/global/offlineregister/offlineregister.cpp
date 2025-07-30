#include "offlineregister.hpp"

#include "servercommon/servercommon.h"
#include "config/logicconfigmanager.hpp"
#include "config/activityconfig/chongzhiconfig.hpp"

OfflineRegister::OfflineRegister()
	: m_user_id(INVALID_USER_ID), m_last_divorce_time(0), m_chongzhi_cmd_flag(0)
{
	m_other_param.Reset();
}

OfflineRegister::OfflineRegister(const UserID &user_id) : m_user_id(user_id), m_last_divorce_time(0), m_chongzhi_cmd_flag(0)
{
	m_other_param.Reset();
}

OfflineRegister::~OfflineRegister()
{

}

void OfflineRegister::Init(const OfflineRegisterParam::OfflineRegisterAttr &attr)
{
	m_user_id = IntToUid(attr.role_id);
	m_last_divorce_time = attr.last_divorce_time;
	m_chongzhi_cmd_flag = attr.chongzhi_cmd_flag;
	m_other_param = attr.other_param;
}

void OfflineRegister::GetAttr(OfflineRegisterParam::OfflineRegisterAttr *attr)
{
	attr->role_id = UidToInt(m_user_id);
	attr->last_divorce_time = m_last_divorce_time;
	attr->other_param = m_other_param;
}

void OfflineRegister::AddShiTuRelationBreakInfo(int master_apprentice_uid, int type)
{
	int free_index = -1;
	bool is_has = false;

	for (int i = 0; i < SHITU_MAX_APPRENTICE_COUNT + SHITU_MAX_MASTER_COUNT; ++i)
	{
		if (m_other_param.shi_tu_break_info[i].uid == master_apprentice_uid)
		{
			is_has = true;
			break;
		}

		if (m_other_param.shi_tu_break_info[i].uid == 0 && free_index == -1)
		{
			free_index = i;
		}
	}

	if (!is_has && free_index != -1)
	{
		m_other_param.shi_tu_break_info[free_index].uid = master_apprentice_uid;
		m_other_param.shi_tu_break_info[free_index].break_type = type;
	}
}

void OfflineRegister::ClearShiTuRelationBreakInfo()
{
	for (int i = 0; i < SHITU_MAX_APPRENTICE_COUNT + SHITU_MAX_MASTER_COUNT; ++i)
	{
		m_other_param.shi_tu_break_info[i].Reset();
	}
}

void OfflineRegister::AddGuildGongXian(long long add_num)
{
	if (MAX_INT64 - add_num >= m_other_param.add_guild_gongxian)
	{
		m_other_param.add_guild_gongxian += add_num;
	}
	else
	{
		m_other_param.add_guild_gongxian = MAX_INT64;
	}
}

void OfflineRegister::ClearGuildGongXian()
{
	m_other_param.add_guild_gongxian = 0;
}

bool OfflineRegister::IsTrademarketItemEmpty()
{
	for (int i = 0; i < USER_SALE_ITEM_MAX_NUM; ++i)
	{
		if (0 != m_other_param.sale_item_id[i])
		{
			return false;
		}
	}
	return true;
}

void OfflineRegister::AddTrademarketNum(int add_num, ItemID item_id)
{
	if(add_num <= 0) return;
	m_other_param.trademarket_finish += add_num;
	for (int i = 0; i < USER_SALE_ITEM_MAX_NUM; ++i)
	{
		if (0 == m_other_param.sale_item_id[i])
		{
			m_other_param.sale_item_id[i] = item_id;
			break;
		}
	}
}

void OfflineRegister::ClearTrademarketNum()
{
	m_other_param.trademarket_finish = 0;
	memset(m_other_param.sale_item_id, 0, sizeof(m_other_param.sale_item_id));
}

void OfflineRegister::SetRandSportsEnd()
{
	m_other_param.rand_sports_end = 1;
}

void OfflineRegister::SetGladiatorEveryDayReward(int rank_pos)
{
	/*if (m_other_param.gladiator_everyday_count >= OFFLINE_GLADIATOR_EVERYDAY_REWARD_MAX_LIST)
	{
		//替换顶去最旧的，把新的排到最末尾
		for (int i = 0; i < OFFLINE_GLADIATOR_EVERYDAY_REWARD_MAX_LIST - 1; i++)
		{
			m_other_param.gladiator_everyday_reward[i] = m_other_param.gladiator_everyday_reward[i + 1];
		}
		m_other_param.gladiator_everyday_reward[OFFLINE_GLADIATOR_EVERYDAY_REWARD_MAX_LIST - 1] = rank_pos;
	}
	else
	{
		m_other_param.gladiator_everyday_reward[m_other_param.gladiator_everyday_count++] = rank_pos;
	}*/
	m_other_param.gladiator_everyday_reward[0] = rank_pos;
	m_other_param.gladiator_everyday_count = 1;
}

const int * OfflineRegister::GetGladiatorEveryDayReward(int * count)
{
	if (count != NULL)*count = m_other_param.gladiator_everyday_count;
	return m_other_param.gladiator_everyday_reward;
}

void OfflineRegister::ClearGladiatorEveryDayReward()
{
	m_other_param.gladiator_everyday_count = 0;
	memset(m_other_param.gladiator_everyday_reward, 0, sizeof(m_other_param.gladiator_everyday_reward));
}

void OfflineRegister::SetCrossChatGroupCreate(long long group_id, int invalid_index)
{
	m_other_param.create_cross_group_id = group_id;
	m_other_param.invalid_index = invalid_index;
}

bool OfflineRegister::GetCrossChatGroupCreate(long long * group_id, int * invalid_index)
{
	*group_id = m_other_param.create_cross_group_id;
	*invalid_index = m_other_param.invalid_index;
	return true;
}

void OfflineRegister::ClearCrossChatGroupCreate()
{
	m_other_param.create_cross_group_id = 0;
	m_other_param.invalid_index = 0;
}

void OfflineRegister::SetJoinCrossChatGroup(long long group_id)
{
	m_other_param.join_cross_group_id = group_id;
}

void OfflineRegister::ClearJoinCrossChatGroup()
{
	m_other_param.join_cross_group_id = 0LL;
}

void OfflineRegister::SetLimitTimeActivtyCount(int type)
{
	if (0 != (m_other_param.finish_limit_activity_count & (1 << type)))
	{
		return;
	}

	m_other_param.finish_limit_activity_count |= 1 << type;
}

void OfflineRegister::ClearLimitTimeActivtyCount()
{
	m_other_param.finish_limit_activity_count = 0;
}

void OfflineRegister::SetGuildBossChapterBaodiReward(unsigned int week_id, int baodi_flag)
{
	m_other_param.guild_boss_baodi_reward_flag = baodi_flag;
	m_other_param.fetch_boss_chapter_baodi_reward_week_id = week_id;
}

void OfflineRegister::ClearGuildBossChapterBaodiReward()
{
	m_other_param.guild_boss_baodi_reward_flag = 0;
	m_other_param.fetch_boss_chapter_baodi_reward_week_id = 0;
}

void OfflineRegister::SetGuildBossChapterDamageRankReward(unsigned int day_id)
{
	m_other_param.is_fetch_guild_boss_chapter_damage_rank_reward = 1;
	m_other_param.fetch_boss_chapter_rank_reward_day_id = day_id;
}

void OfflineRegister::ClearGuildBossChapterRankReward()
{
	m_other_param.is_fetch_guild_boss_chapter_damage_rank_reward = 0;
	m_other_param.fetch_boss_chapter_rank_reward_day_id = 0;
}

void OfflineRegister::SetGuildGatherDreamPublishFlag()
{
	m_other_param.today_role_guild_gather_dream_publish = 1;
}

void OfflineRegister::ClearGuildGatherDreamPublishFlag()
{
	m_other_param.today_role_guild_gather_dream_publish = 0;
}

void OfflineRegister::SetHuanJieRankScore(int rank_score, int is_finish_ding_ji)
{
	m_other_param.huan_jie_rank_score = rank_score;
	m_other_param.is_finish_ding_ji = 0 != is_finish_ding_ji ? 1 : 0;
}

void OfflineRegister::ClearHuanJieRankScore()
{
	m_other_param.huan_jie_rank_score = 0;
	m_other_param.is_finish_ding_ji = -1;
}

void OfflineRegister::SetRetrievePetInfo(int cmd_id, int pet_id, long long item_unique_id)
{
	m_other_param.retrieve_pet_cmd_id = cmd_id;
	m_other_param.retrieve_pet_id = pet_id;
	m_other_param.retrieve_pet_unique_id = item_unique_id;
}

void OfflineRegister::ClearRetrievePetInfo()
{
	m_other_param.retrieve_pet_cmd_id = 0;
	m_other_param.retrieve_pet_id = 0;
	m_other_param.retrieve_pet_unique_id = 0;
}

void OfflineRegister::SetGuildFightFetchRewardZeroTime(unsigned int zero_timstamp)
{
	m_other_param.guild_fight_fetch_reward_zero_timestamp = zero_timstamp;
}

void OfflineRegister::ClearGuildFightFetchRewardZeroTime()
{
	m_other_param.guild_fight_fetch_reward_zero_timestamp = 0;
}

void OfflineRegister::AddRetrieveExp(long long retrieve_exp)
{
	m_other_param.retrieve_exp += retrieve_exp;
}

void OfflineRegister::ClearRetrieveExpInfo()
{
	m_other_param.retrieve_exp = 0;
}

void OfflineRegister::SetOnlyFightFetchRewardZeroTime(unsigned int zero_timstamp)
{
	m_other_param.only_fight_fetch_reward_zero_timestamp =  zero_timstamp;
}

void OfflineRegister::ClearOnlyFightFetchRewardZeroTime()
{
	m_other_param.only_fight_fetch_reward_zero_timestamp = 0;
}

void OfflineRegister::SetTeamFightFetchRewardZeroTime(unsigned int zero_timstamp)
{
	m_other_param.team_fight_fetch_reward_zero_timestamp = zero_timstamp;
}

void OfflineRegister::ClearTeamFightFetchRewardZeroTime()
{
	m_other_param.team_fight_fetch_reward_zero_timestamp = 0;
}

void OfflineRegister::SetTradeMarketBuyItemInfo(ItemID item_id, int consume_gold)
{
	m_other_param.trademarket_buy_item_id = item_id;
	m_other_param.trademarket_purchase_succ_consume_gold = consume_gold;
}

void OfflineRegister::ClearTradeMarketBuyItemInfo()
{
	m_other_param.trademarket_purchase_succ_consume_gold = 0;
	m_other_param.trademarket_buy_item_id = 0;
}

void OfflineRegister::SetTradeCreditAddCredit(int credit_num)
{
	m_other_param.credit_num = credit_num;
}

void OfflineRegister::ClearTradeCreditAddCredit()
{
	m_other_param.credit_num = 0;
}

void OfflineRegister::SetCloudArenaAddArenaCoin(int add_num)
{
	m_other_param.add_cloud_arena_coin_num = add_num;
}

void OfflineRegister::ClearCloudArenaCoin()
{
	m_other_param.add_cloud_arena_coin_num = 0;
}

void OfflineRegister::AddColosseumCoin(int add_num)
{
	if (add_num <= 0) return;

	if (m_other_param.add_colosseum_coin > MAX_INT - add_num)
	{
		m_other_param.add_colosseum_coin = MAX_INT;
	}
	else
	{
		m_other_param.add_colosseum_coin += add_num;
	}
	
}

void OfflineRegister::ClearColosseumCoin()
{
	m_other_param.add_colosseum_coin = 0;
}

void OfflineRegister::OnUserGetNormalCrystalFBBossReward(int seq)
{
	m_other_param.has_get_normal_crystal_fb_reward = 1;
	m_other_param.normal_crystal_fb_reward_seq = seq;
}

void OfflineRegister::ClearUserGetNormalCrystalFBBossRewardFlag()
{
	m_other_param.has_get_normal_crystal_fb_reward = 0;
	m_other_param.normal_crystal_fb_reward_seq = -1;
}

void OfflineRegister::AddHongBaoTimes(int _hongbao_type, int _times)
{
	if (_hongbao_type <= HONGBAO_TYPE_INVALID || _hongbao_type >= HONGBAO_TYPE_MAX)
		return;

	m_other_param.hongbao_times[_hongbao_type] += _times;
}

int OfflineRegister::GetHongBaoTimes(int _hongbao_type)
{
	if (_hongbao_type <= HONGBAO_TYPE_INVALID || _hongbao_type >= HONGBAO_TYPE_MAX)
		return 0;

	return m_other_param.hongbao_times[_hongbao_type] ;
}

void OfflineRegister::ClearHongBaoTimes(int _hongbao_type)
{
	if (_hongbao_type < 0 || _hongbao_type >= MAX_HONG_BAO_TYPE_COUNT)
		return;

	m_other_param.hongbao_times[_hongbao_type] = 0;
}

void OfflineRegister::AddHongBaoSilverCoin(int _val)
{
	m_other_param.hongbao_silver_coin += _val;
}

int OfflineRegister::GetHongBaoSilverCoin()
{
	return m_other_param.hongbao_silver_coin; 
}

void OfflineRegister::ClearHongBaoSilverCoin()
{
	m_other_param.hongbao_silver_coin = 0;
}

void OfflineRegister::OnSetRAPaiMaiHangLastBidInfo(int seq, int bid_price)
{
	if(seq < 0 || seq >= ARRAY_LENGTH(m_other_param.ra_last_bid_price)) return;
	
	m_other_param.ra_last_bid_price[seq] = bid_price;
}

void OfflineRegister::ClearRAPaiMaiHangLastBidInfo()
{
	memset(m_other_param.ra_last_bid_price, 0, sizeof(m_other_param.ra_last_bid_price));
}

void OfflineRegister::OnSetDuelFight(char duel_type, char is_attack, char is_win, unsigned char level_diff)
{
	m_other_param.duel_type = duel_type;
	m_other_param.duel_is_attack = is_attack;
	m_other_param.duel_is_win = is_win;
	m_other_param.duel_level_diff = level_diff;
}

void OfflineRegister::ClearDuelFight()
{
	m_other_param.duel_type = 0;
	m_other_param.duel_is_attack = -1;
	m_other_param.duel_is_win = -1;
	m_other_param.duel_level_diff = 0;
}

void OfflineRegister::OnSetGuildFightRank(int guild_fight_rank)
{
	m_other_param.max_guild_fight_rank = guild_fight_rank;
}

void OfflineRegister::ClearGuildFightRank()
{
	m_other_param.max_guild_fight_rank = 0;
}

void OfflineRegister::OnSetGuildHonorRank(int guild_honor_rank)
{
	m_other_param.max_guild_honor_rank = guild_honor_rank;
}

void OfflineRegister::ClearGuildHonorRank()
{
	m_other_param.max_guild_honor_rank = 0;
}

void OfflineRegister::OnSetWinChiefElection(int prof)
{
	m_other_param.win_chief_election_prof = prof;
}

void OfflineRegister::ClearWinChiefElection()
{
	m_other_param.win_chief_election_prof = 0;
}

void OfflineRegister::OnSetJoinJieYiDay(int join_day)
{
	m_other_param.join_jie_yi_day = join_day;
}

void OfflineRegister::ClearJoinJieYiDay()
{
	m_other_param.join_jie_yi_day = 0;
}

void OfflineRegister::OnReceviceFlower()
{
	m_other_param.recevice_flower_count += 1;
}

void OfflineRegister::ClearReceviceFlower()
{
	m_other_param.recevice_flower_count = 0;
}

void OfflineRegister::AddCowReportGoodNewsReturnGoldOfflineEvent(int _val)
{
	m_other_param.cow_report_good_news_return_gold += _val;
}

void OfflineRegister::ClearCowReportGoodNewsReturnGoldOfflineEvent()
{
	m_other_param.cow_report_good_news_return_gold = 0;
}

void OfflineRegister::AddFriendOfflineEvent()
{
	m_other_param.has_add_friend_event = 1;
}

void OfflineRegister::ClearAddFriendOfflineEvent()
{
	m_other_param.has_add_friend_event = 0;
}

void OfflineRegister::AddTigerBringsBlessingReturnGoldOfflineEvent(int _val)
{
	m_other_param.tiger_brings_blessing_return_gold += _val;
}

void OfflineRegister::ClearTigerBringsBlessingReturnGoldOfflineEvent()
{
	m_other_param.tiger_brings_blessing_return_gold = 0;
}

void OfflineRegister::SetQingYuanShiZhuangBuyMark(int buy_mark)
{
	m_other_param.qingyuan_shizhuang_buy_mark = buy_mark;
}

void OfflineRegister::ClearQingYuanShiZhuangBuyMark()
{
	m_other_param.qingyuan_shizhuang_buy_mark = 0;
}

void OfflineRegister::SetQingYuanShiLeiChongGold(int chongzhi_gold)
{
	m_other_param.qingyuan_leichong_gold = chongzhi_gold;
}

void OfflineRegister::ClearQingYuanLeiChongGold()
{
	m_other_param.qingyuan_leichong_gold = 0;
}

void OfflineRegister::SetWorldTeamArenaInfo(int team_rank, unsigned int timestamp, int * title_id)
{
	m_other_param.world_team_arena_rank = team_rank;
	m_other_param.world_team_arena_timestamp = timestamp;
	if (NULL != title_id)
	{
		m_other_param.world_team_arena_title_id = *title_id;
	}
}

void OfflineRegister::ClearWorldTeamArenaInfo()
{
	m_other_param.world_team_arena_rank = 0;
	m_other_param.world_team_arena_title_id = 0;
	m_other_param.world_team_arena_timestamp = 0;
}

void OfflineRegister::SetCloudArenaUseTimesTimestamp(unsigned int timestamp)
{
	m_other_param.cloud_arena_use_times_timestamp = timestamp;
}

void OfflineRegister::ClearCloudArenaUseTimesTimestamp()
{
	m_other_param.cloud_arena_use_times_timestamp = 0u;
}

void OfflineRegister::SetLifeSkillChangeCard(int fight_change_card_seq)
{
	m_other_param.fight_change_card_seq = fight_change_card_seq;
}

void OfflineRegister::ClearLifeSkillChangeCard()
{
	m_other_param.fight_change_card_seq = 0;
}

void OfflineRegister::SetRoleChangeName(GameName new_name, bool is_cost)
{
	m_other_param.new_name_flag = 1;
	m_other_param.new_name_cost_flag = is_cost ? 1 : 0;
	F_STRNCPY(m_other_param.new_name, new_name, sizeof(GameName));
}

void OfflineRegister::ClearRoleChangeName()
{
	m_other_param.new_name_flag = 0;
	m_other_param.new_name_cost_flag = 0;
	memset(m_other_param.new_name, 0, sizeof(m_other_param.new_name));
}

bool OfflineRegister::SetAuthorityType(int effect_type, int effect_param, int effect_param1)
{
	m_other_param.effect_type = effect_type;
	m_other_param.effect_param = effect_param;
	m_other_param.effect_param1 = effect_param1;
	return true;
}

void OfflineRegister::ClearAuthorityType()
{
	m_other_param.effect_type = -1;
	m_other_param.effect_param = 0;
	m_other_param.effect_param1 = 0;
}

bool OfflineRegister::SetHorcruxTianCheng(int times, int floor_back)
{
	m_other_param.horcrux_tian_cheng_times += times;
	m_other_param.floor_back = floor_back;
	return true;
}

void OfflineRegister::ClearHorcruxTianCheng()
{
	m_other_param.horcrux_tian_cheng_times = 0;
	m_other_param.floor_back = 0;
}

bool OfflineRegister::SetJiXingGaoZhao(int times, int floor_back)
{
	m_other_param.jixing_gaozhao_times += times;
	m_other_param.jixing_gaozhao_floor_back = floor_back;
	return true;
}

void OfflineRegister::ClearJiXingGaoZhao()
{
	m_other_param.jixing_gaozhao_times = 0;
	m_other_param.jixing_gaozhao_floor_back = 0;
}

void OfflineRegister::AddFriendIntimacy(int _add_value)
{
	m_other_param.add_value += _add_value;
}

void OfflineRegister::ClearFriendIntimacy()
{
	m_other_param.add_value = 0;
}

bool OfflineRegister::CanRemove(unsigned int now_second)
{
	if ( 0 != m_last_divorce_time  || !m_other_param.CanRemove())
	{
		return false;
	}

	return true;
}

void OfflineRegister::OnChongzhiCmd(int chongzhi_num)
{
	UNSTD_STATIC_CHECK(CHONGZHI_REWARD_CONFIG_MAX_NUM <= (int)sizeof(m_chongzhi_cmd_flag) * 8);

	//G6可以无限次额外奖励，所以不去重了
	//bool is_found = false;
	//for (int i = 0; i < m_other_param.chongzhi_count; ++i)
	//{
	//	if (chongzhi_num == m_other_param.chongzhi_list[i])
	//	{
	//		is_found = true;
	//		break;
	//	}
	//}

	if (m_other_param.chongzhi_count < OFFLINE_CHONGZHI_ARRAY_SIZE) 
	{
		m_other_param.chongzhi_list[m_other_param.chongzhi_count++] = chongzhi_num;
	}
}

int OfflineRegister::GetChongzhiNumByIndex(int index)
{
	if (index < 0 || index >= OFFLINE_CHONGZHI_ARRAY_SIZE || index >= m_other_param.chongzhi_count)
	{
		return 0;
	}

	return m_other_param.chongzhi_list[index];
}

void OfflineRegister::ClearChongzhiList()
{
	m_other_param.chongzhi_count = 0;
	memset(m_other_param.chongzhi_list, 0, sizeof(m_other_param.chongzhi_list));
}

void OfflineRegister::KickOutGuildTamp(time_t now_second)
{
	m_other_param.kick_out_guild_tamp = now_second;
}

void OfflineRegister::ClearKickOutGuildTamp()
{
	m_other_param.kick_out_guild_tamp = 0;
}

void OfflineRegister::SetForbidTalkSeconds(int forbid_talk_seconds, int forbid_talk_type)
{
	m_other_param.forbid_talk_seconds = forbid_talk_seconds;
	m_other_param.forbid_talk_type = forbid_talk_type;
}

void OfflineRegister::ClearForbidTalksSeconds()
{
	m_other_param.forbid_talk_seconds = -1; 
	m_other_param.forbid_talk_type = FORBID_TALK_TYPE_DEFAULT;
}

void OfflineRegister::SetRetrieveParam(Int64 unbind_gold, Int64 bind_gold, Int64 bind_coin, Int64 spirit_jade, int vip_exp, ItemID item_id, short item_num, short retrieve_reason)
{
	m_other_param.retrieve_unbind_gold = unbind_gold;
	m_other_param.retrieve_bind_coin = bind_coin;
	m_other_param.retrieve_item_id = item_id;
	m_other_param.retrieve_item_num = item_num;
	m_other_param.retrieve_reason = retrieve_reason;
	m_other_param.other_currency[MONEY_TYPE_SILVER_COIN] = spirit_jade;
}

void OfflineRegister::SetRetrieveOtherMoneyParam(MoneyOtherParam retrieve_other_currency, short retrieve_reason)
{
	for (int i = 0; i < ARRAY_LENGTH(m_other_param.other_currency) && i < ARRAY_LENGTH(retrieve_other_currency.other_currency); ++i)
	{
		m_other_param.other_currency[i] = retrieve_other_currency.other_currency[i];
	}
	m_other_param.other_currency_retrieve_reason = retrieve_reason;
}

void OfflineRegister::ClearRetrieveOtherMoneyParam()
{
	memset(m_other_param.other_currency, 0, sizeof(m_other_param.other_currency));
	m_other_param.other_currency_retrieve_reason = -1;
}

void OfflineRegister::ClearRetrieveParam()
{
	m_other_param.retrieve_unbind_gold = 0;
	m_other_param.retrieve_bind_coin = 0;
	m_other_param.retrieve_item_id = 0;
	m_other_param.retrieve_item_num = 0;
	m_other_param.retrieve_reason = -1;
	m_other_param.other_currency[MONEY_TYPE_SILVER_COIN] = 0;
}


