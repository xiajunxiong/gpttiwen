#include "config/otherconfig/guildconfig.hpp"
#include "item/itempool.h"
#include "servercommon/commondata.hpp"
#include "servercommon/guilddef.hpp"
#include "gameworld/checkresourcecenter.hpp"
#include "monster/monster_group_pool.hpp"
#include "world.h"
#include "servercommon/gatherdreamdef.hpp"
#include "skill/skillpool.hpp"
#include "servercommon/rankdef.hpp"
#include "global/guild/guild.hpp"

GuildConfig::GuildConfig() : m_guild_build_init(0), m_hall_max_level(0), m_other_build_max_level(0)
{

}

GuildConfig::~GuildConfig()
{
	std::vector<GuildJournalCfg>().swap(m_journal_cfg);
	std::vector<GuildActiveGiftCfg >().swap(m_active_gift_cfg);
	std::vector<GuildActiveRankRewardCfg >().swap(m_active_rank_reward_cfg);
	std::vector<GuildActivePersonRankRewardCfg >().swap(m_active_person_rank_reward_cfg);

	for (std::map<int, std::vector<int> >::iterator it = m_guild_inborn_relation_map.begin(); it != m_guild_inborn_relation_map.end(); it++)
	{
		std::vector<int>().swap(it->second);
	}

	for (std::map<int, std::vector<HallOfFameRank> >::iterator it = m_hof_sort_map.begin(); it != m_hof_sort_map.end(); it++)
	{
		std::vector<HallOfFameRank>().swap(it->second);
	}

	std::vector<GuildConstructCfg>().swap(m_guild_construct_cfg);
}

bool GuildConfig::Init(const std::string &configname, std::string *err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG("other", InitOtherCfg);
	LOAD_CONFIG("family_room", InitRoomCfg);
	LOAD_CONFIG("member_type", InitMemberTypeCfg);
	LOAD_CONFIG("train_up", InitPetTrainingCfg);
	LOAD_CONFIG("exp", InitTrainingExpCfg);
	LOAD_CONFIG("family_shop", InitGuildShopCfg);
	LOAD_CONFIG("family_boss", InitGuildBossCfg);
	LOAD_CONFIG("boss_reward", InitGuildBossChapterRewardCfg);
	LOAD_CONFIG("family_journal", InitGuildJournalCfg);
	LOAD_CONFIG("injury_reward", InitGuildRankRewardCfg);
	LOAD_CONFIG("family_pray", InitGuildGatherDreamCfg);
	LOAD_CONFIG("family_inborn", InitGuildInbornCfg);

	LOAD_CONFIG("active_gift", InitGuildActiveGiftCfg);
	LOAD_CONFIG("family_active_rank", InitGuildActiveRankRewardCfg);
	LOAD_CONFIG("personal_active_rank", InitGuildActivePersonRankRewardCfg);
	LOAD_CONFIG("redbag", InitGuildHongbaoCfg);
	LOAD_CONFIG("build_up", InitGuildConstructCfg);

	LOAD_CONFIG("hof_sort", InitGuildHofSortCfg);
	LOAD_CONFIG("warth_get", InitGuildWarthGetCfg);
	LOAD_CONFIG("bless_base", InitGuildBlessBaseCfg);
	LOAD_CONFIG("bless_reward", InitGuildBlessRewardCfg);

	return true; 	
}

const GuildRoomCfg * GuildConfig::GetRoomCfg(int level)
{
	std::map<int, GuildRoomCfg>::const_iterator it = m_room_map.find(level);
	if (it == m_room_map.end()) return NULL;
	return &it->second;
}

const GuildRoomCfg * GuildConfig::GetRoomCfgByLevel(int level)
{
	if(m_room_map.empty()) return NULL;

	if (m_room_map.rbegin()->first < level)
	{
		return &m_room_map.rbegin()->second;
	}
	std::map<int, GuildRoomCfg>::const_iterator it = m_room_map.find(level);
	if (it == m_room_map.end()) return NULL;

	return &it->second;
}

const GuildMemberTypeCfg * GuildConfig::GetMemberTypeCfg(int type)
{
	std::map<int, GuildMemberTypeCfg>::const_iterator it = m_member_type_map.find(type);
	if (it == m_member_type_map.end()) return NULL;
	return &it->second;
}

const GuildPetTrainingCfg * GuildConfig::GetPetTrainingCfg(int type, int level)
{
	std::map<int, std::map<int, GuildPetTrainingCfg> >::const_iterator type_it = m_pet_training_map.find(type);
	if (type_it == m_pet_training_map.end()) return NULL;
	std::map<int, GuildPetTrainingCfg>::const_iterator level_it = type_it->second.find(level);
	if (level_it == type_it->second.end()) return NULL;
	return &level_it->second;
}

const GuildJournalCfg * GuildConfig::GetJournalCfg(int type)
{
	if (type < GUILD_EVENTHANDLER_ADD_MEMBER || type >= GUILD_EVENTHANDLER_TYPE_MAX || type >(int)m_journal_cfg.size()) return NULL;

	return &m_journal_cfg[type - 1];
}

const GuildBaseConstructCfg * GuildConfig::GetConstructCfg(int build_type, int build_level)
{
	if (build_type <= 0 || build_type >= GUILD_BUILD_TYPE_MAX || build_level < 0) return NULL;

	for (int i = 0; i < (int)m_guild_construct_cfg.size(); ++i)
	{
		if (build_type == m_guild_construct_cfg[i].build_type)
		{
			std::map<int, GuildBaseConstructCfg>::const_iterator it_level = m_guild_construct_cfg[i].base_list.find(build_level);
			if (m_guild_construct_cfg[i].base_list.end() != it_level)
			{
				return &it_level->second;
			}
		}

	}
	return NULL;
}

const GuildBossCfg::GuildBossCfgItem * GuildConfig::GetBossCfg(int chapter_id)
{
	std::map<int, GuildBossCfg>::iterator iter = m_boss_cfg_list.find(chapter_id);
	if (iter == m_boss_cfg_list.end())
	{
		return NULL;
	}

	return &iter->second.guild_boss_cfg;
}

const GuildBossCfg* GuildConfig::GetBossChapterCfg(int chapter_id)
{
	std::map<int, GuildBossCfg>::iterator iter = m_boss_cfg_list.find(chapter_id);
	if (iter == m_boss_cfg_list.end())
	{
		return NULL;
	}

	return &iter->second;
}

int GuildConfig::GetBossChapterLimitWorldLevel(int chapter_id)
{
	std::map<int, GuildBossCfg>::iterator iter = m_boss_cfg_list.find(chapter_id);
	if (iter == m_boss_cfg_list.end())
	{
		return 0;
	}

	return iter->second.world_level;
}

int GuildConfig::GetBossChapterByWorldLevel(int world_level)
{
	std::map<int, GuildBossCfg>::iterator it = m_boss_cfg_list.begin();
	int ret_chapter_id = it->first;

	for (; it != m_boss_cfg_list.end(); ++it)
	{
		if (world_level <= it->second.world_level)
		{
			ret_chapter_id = it->first;
			break;
		}
	}

	return ret_chapter_id;
}

bool GuildConfig::IsVaildGuildBossChapterId(int chapter_id)
{
	std::map<int, GuildBossCfg>::iterator iter = m_boss_cfg_list.find(chapter_id);
	if (iter == m_boss_cfg_list.end())
	{
		return false;
	}

	return true;
}

const GuildBossChapterRewardCfg::ItemCfg * GuildConfig::GetGuildBossChapterRankReward(int reward_group_id)
{
	std::map<int, GuildBossChapterRewardCfg>::iterator iter = m_guild_boss_chapter_item_reward_map.find(reward_group_id);
	if (iter == m_guild_boss_chapter_item_reward_map.end())
	{
		return NULL;
	}

	if (iter->second.group_reward.empty())
	{
		return NULL;
	}

	int rate = rand() % iter->second.rate_count;
	for (int i = 0; i < (int)iter->second.group_reward.size(); i++)
	{
		if (rate < iter->second.group_reward[i].rate)
		{
			return &iter->second.group_reward[i];
		}
		rate -= iter->second.group_reward[i].rate;
	}

	return NULL;
}

const GuildRankRewardCfg * GuildConfig::GetRankRewardCfg(int chapter_id, int rank)
{
	if (chapter_id < 0 || chapter_id >= MAX_GUILD_CHAPTER_BOSS_NUM || rank < 0 || rank > GUILD_MAX_MEMBER_COUNT) return NULL;

	for (int i = 0; i < MAX_GUILD_BOSS_CHAPTER_RANK_NUM; i++)
	{
		if (m_rank_reward_cfg[chapter_id][i].min_rank <= rank && m_rank_reward_cfg[chapter_id][i].max_rank >= rank)
		{
			return &m_rank_reward_cfg[chapter_id][i];
		}
	}

	return NULL;
}

const GuildGatherDreamCfg * GuildConfig::GetGatherDreamCfg(int quality, int type)
{
	if (0 >= quality || quality >= MAX_GATHER_DREAM_TYPE_NUM || type < GUILD_GATHER_DREAM_ITEM_TYPE_PET || type >= GUILD_GATHER_DREAM_ITEM_TYPE_MAX) return NULL;

	return &m_gather_dream_cfg[type][quality];
}

int GuildConfig::GetPetTrainingExp(int level)
{
	std::map<int, int>::const_iterator it = m_training_exp_map.find(level);
	if (it == m_training_exp_map.end()) return 0;
	return it->second;
}

int GuildConfig::GetPetTrainingGongXian(int level)
{
	std::map<int, int>::const_iterator it = m_training_gongxian_map.find(level);
	if (it == m_training_gongxian_map.end()) return 0;
	return it->second;
}

void GuildConfig::ResetBuyCount(int type, unsigned short shop_buy_list[GUILD_SHOP_COUNT_MAX])
{
	if (NULL == shop_buy_list) return;
	for (std::map<int, GuildShopCfg>::const_iterator it = m_shop_map.begin(), end = m_shop_map.end(); it != end; ++it)
	{
		if (it->second.seq < 0 || it->second.seq >= GUILD_SHOP_COUNT_MAX) break;
		if (type != it->second.buy_type) continue;
		shop_buy_list[it->second.seq] = 0;
	}
}

const GuildShopCfg * GuildConfig::GetShopCfg(int seq)
{
	std::map<int, GuildShopCfg>::const_iterator it = m_shop_map.find(seq);
	if (it == m_shop_map.end()) return NULL;
	return &it->second;
}

const GuildActiveGiftCfg::GiftCfg * GuildConfig::GetGuildActiveGiftCfg(int _seq, int _level)
{
	for (unsigned int i = 0; i < m_active_gift_cfg.size(); ++i)
	{
		if (_level >= m_active_gift_cfg[i].level_min && _level <= m_active_gift_cfg[i].level_max)
		{
			if (_seq < 0 || _seq >= static_cast<int>(m_active_gift_cfg[i].gift_cfg_vec.size()))
			{
				return NULL;
			}

			return &m_active_gift_cfg[i].gift_cfg_vec[_seq];
		}
	}

	return NULL;
}

const GuildActiveRankRewardCfg * GuildConfig::GetGuildActiveRankRewardCfg(int _rank)
{
	for (unsigned int i = 0; i < m_active_rank_reward_cfg.size(); ++i)
	{
		if (_rank >= m_active_rank_reward_cfg[i].rank_min && _rank <= m_active_rank_reward_cfg[i].rank_max)
		{
			return &m_active_rank_reward_cfg[i];
		}
	}

	return NULL;
}

const GuildActivePersonRankRewardCfg * GuildConfig::GetGuildActivePersonRankRewardCfg(int _rank)
{
	for (unsigned int i = 0; i < m_active_person_rank_reward_cfg.size(); ++i)
	{
		if (_rank >= m_active_person_rank_reward_cfg[i].rank_min && _rank <= m_active_person_rank_reward_cfg[i].rank_max)
		{
			return &m_active_person_rank_reward_cfg[i];
		}
	}

	return NULL;
}

const GuildInbornLevelCfg * GuildConfig::GetGuildInbornCfg(int inborn_type, int type_seq, int level)
{
	if (inborn_type < 0 || inborn_type >= ARRAY_LENGTH(m_guild_inborn_list) ||
		type_seq < 0 || type_seq >= ARRAY_LENGTH(m_guild_inborn_list[inborn_type]) ||
		level <= 0 || level > m_guild_inborn_list[inborn_type][type_seq].max_level)
	{
		return NULL;
	}

	return &m_guild_inborn_list[inborn_type][type_seq].level_list[level - 1];
}

int GuildConfig::GetMaxGuildInbornLevel(int inborn_type, int type_seq) const
{
	if (inborn_type < 0 || inborn_type >= ARRAY_LENGTH(m_guild_inborn_list) ||
		type_seq < 0 || type_seq >= ARRAY_LENGTH(m_guild_inborn_list[inborn_type]))
	{
		return 0;
	}

	return m_guild_inborn_list[inborn_type][type_seq].max_level;
}

void GuildConfig::GetGuildInbornResetSkill(int inborn_type, int seq, GuildInbornSkill * learn_list, int learn_count, ARG_OUT std::vector<ItemConfigData> & return_item_list) const
{
	if(NULL == learn_list || learn_count <= 0 || seq >= learn_count) return;

	int guild_inborn_key = this->GetGuildInbornKey(inborn_type, seq);
	std::map<ItemID, int> return_item_map;
	//先重置原始目标
	this->GetGuildInbornReturnItem(guild_inborn_key, learn_list[seq].skill_level, return_item_map);
	learn_list[seq].Reset();
	//再查看目标是否存在后置链
	std::map<int, std::vector<int> >::const_iterator it = m_guild_inborn_relation_map.find(guild_inborn_key);
	if(it != m_guild_inborn_relation_map.end() && !it->second.empty()) 
	{
		for (int i = 0; i < (int)it->second.size(); i++)
		{
			int type_seq = it->second[i] % 100;
			if (type_seq < 0 || type_seq >= learn_count || learn_list[type_seq].IsInvaild()) continue;

			this->GetGuildInbornReturnItem(it->second[i], learn_list[type_seq].skill_level, return_item_map);
			this->CalcGuildInbornResetSkill(it->second[i], learn_list, learn_count, return_item_map);
			learn_list[type_seq].Reset();
		}
	}

	for (std::map<ItemID, int>::const_iterator it = return_item_map.begin(); it != return_item_map.end(); ++it)
	{
		return_item_list.push_back(ItemConfigData(it->first, true, it->second));
	}
}

const GuildHongBaoCfg::HongBaoCfg * GuildConfig::GetGuildHongBaoCfgByType(int _hong_bao_type)
{
	if (_hong_bao_type <= HONGBAO_TYPE_INVALID || _hong_bao_type >= HONGBAO_TYPE_MAX)
	{
		return NULL;
	}

	return &m_hongbao_cfg.hong_bao_cfg[_hong_bao_type];
}

const HallOfFameRank * GuildConfig::GetRankTypeCfg(int type, int rank_idx)
{
	std::map<int, std::vector<HallOfFameRank> >::iterator it = m_hof_sort_map.find(type);
	if (it != m_hof_sort_map.end())
	{
		for (std::vector<HallOfFameRank>::iterator it_2 = it->second.begin(); it_2 != it->second.end(); it_2++)
		{
			if (it_2->rank_min <= rank_idx && it_2->rank_max >= rank_idx)
			{
				return &(*it_2);
			}
		}
	}
	return NULL;
}

const WarthAddInfo* GuildConfig::GetWarthAdd(int battle_mode)
{
	std::map<int, WarthAddInfo>::iterator it = m_warth_map.find(battle_mode);
	if (it != m_warth_map.end())
	{
		return &it->second;
	}
	return 0;
}

const BlessTimesList * GuildConfig::GetBlessBaseCfg(int times)
{
	if (times - 1 < 0 || times - 1 >= MAX_BLESS_TIMES)return NULL;
	return &bless_time_config[times - 1];
}

const BlessTimesAttr * GuildConfig::GetBlessAttrCfg(int world_level)
{
	std::map<int, BlessTimesAttr>::iterator it = m_bless_attr_map.find(world_level);
	if (it != m_bless_attr_map.end())
	{
		return &it->second;
	}
	return NULL;
}

int GuildConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -1000;

	while (dataElement)
	{
		if (!GetSubNodeValue(dataElement, "create_cost", m_other_cfg.create_cost) || m_other_cfg.create_cost < 0)return -1;
		if (!GetSubNodeValue(dataElement, "level", m_other_cfg.level) || m_other_cfg.level < 0)return -2;
		if (!GetSubNodeValue(dataElement, "exp_time", m_other_cfg.exp_time) || m_other_cfg.exp_time < 0)return -3;
		if (!GetSubNodeValue(dataElement, "train_time", m_other_cfg.training_max_time) || m_other_cfg.training_max_time < 0)return -4;
		if (!GetSubNodeValue(dataElement, "protect_time", m_other_cfg.protect_time) || m_other_cfg.protect_time < 0)return -5;
		if (!GetSubNodeValue(dataElement, "max_coin", m_other_cfg.max_coin) || m_other_cfg.max_coin < 0)return -6;
		if (!GetSubNodeValue(dataElement, "challenge", m_other_cfg.guild_boss_play_times) || m_other_cfg.guild_boss_play_times < 0)return -7;
		if (!GetSubNodeValue(dataElement, "compensate_percent", m_other_cfg.fund_return_rate) || m_other_cfg.fund_return_rate < 0 || m_other_cfg.fund_return_rate > 100) return -8;
		if (!GetSubNodeValue(dataElement, "first_coin", m_other_cfg.initial_funding) || m_other_cfg.initial_funding < 0) return -9;
		if (!GetSubNodeValue(dataElement, "family_scene", m_other_cfg.station_scene_id) || !World::GetInstWorld()->GetSceneManager()->CheckSceneIsExist(m_other_cfg.station_scene_id)) return -10;
		if (!GetSubNodeValue(dataElement, "change_name_item", m_other_cfg.guild_rename_need_itemid)) return -11;
		if (!GetSubNodeValue(dataElement, "change_badge_coin", m_other_cfg.change_badge_coin) || m_other_cfg.change_badge_coin < 0) return -12;
		if (!GetSubNodeValue(dataElement, "pray_family_level", m_other_cfg.pray_family_level) || m_other_cfg.pray_family_level < 0) return -13;
		if (!GetSubNodeValue(dataElement, "pray_give_maxnum", m_other_cfg.pray_give_maxnum) || m_other_cfg.pray_give_maxnum < 0 || m_other_cfg.pray_give_maxnum > MAX_GATHER_DREAM_GIFT_NUM) return -14;
		if (!GetSubNodeValue(dataElement, "patriarch_linetime", m_other_cfg.tan_he_zu_zhang_logout_time_length)) return -15;
		if (!GetSubNodeValue(dataElement, "impeach_time", m_other_cfg.tan_he_zu_zhang_continued_time)) return -16;
		if (!GetSubNodeValue(dataElement, "redbag_time", m_other_cfg.redbag_time_limit) || m_other_cfg.redbag_time_limit <= 0) return -17;
		if (!GetSubNodeValue(dataElement, "boss_round_limit", m_other_cfg.boss_round_limit) || m_other_cfg.boss_round_limit <= 0) return -18;
		if (!GetSubNodeValue(dataElement, "bless_consume", m_other_cfg.bless_consume) || m_other_cfg.bless_consume <= 0) return -19;
		if (!GetSubNodeValue(dataElement, "bless_all_add", m_other_cfg.bless_all_add) || m_other_cfg.bless_all_add <= 0) return -20;
		if (!GetSubNodeValue(dataElement, "rank_amount", m_other_cfg.rank_amount) || m_other_cfg.rank_amount <= 0) return -21;
		if (!GetSubNodeValue(dataElement, "bless_open_level", m_other_cfg.bless_open_level) || m_other_cfg.bless_open_level <= 0) return -22;
		if (!GetSubNodeValue(dataElement, "bless_open_fund", m_other_cfg.bless_open_fund) || m_other_cfg.bless_open_fund <= 0) return -23;
		if (!GetSubNodeValue(dataElement, "serve_build_capital", m_other_cfg.serve_build_capital) || m_other_cfg.serve_build_capital < 0) return -24;
		if (!GetSubNodeValue(dataElement, "expend_coin", m_other_cfg.expend_coin) || m_other_cfg.expend_coin < 0) return -25;
		if (!GetSubNodeValue(dataElement, "get_contribution", m_other_cfg.get_contribution) || m_other_cfg.get_contribution < 0) return -26;
		if (!GetSubNodeValue(dataElement, "speed_up", m_other_cfg.speed_up) || m_other_cfg.speed_up < 0) return -27;
		if (!GetSubNodeValue(dataElement, "speed_times", m_other_cfg.speed_times) || m_other_cfg.speed_times < 0) return -28;
		if (!GetSubNodeValue(dataElement, "person_times", m_other_cfg.person_times) || m_other_cfg.person_times < 0) return -29;
		if (!GetSubNodeValue(dataElement, "build_times", m_other_cfg.building_limit) || m_other_cfg.building_limit < 0 || m_other_cfg.building_limit < m_other_cfg.speed_times) return -30;
		if (!GetSubNodeValue(dataElement, "create_cost_type", m_other_cfg.create_cost_type) || m_other_cfg.create_cost_type < MONEY_TYPE_GOLD || m_other_cfg.create_cost_type >= MONEY_TYPE_MAX) return -31;
		if (!GetSubNodeValue(dataElement, "first_charge_create_cost_type", m_other_cfg.first_charge_create_cost.first) || m_other_cfg.first_charge_create_cost.first < MONEY_TYPE_GOLD || m_other_cfg.first_charge_create_cost.first >= MONEY_TYPE_MAX) return -31;
		if (!GetSubNodeValue(dataElement, "first_charge_create_cost", m_other_cfg.first_charge_create_cost.second) || m_other_cfg.first_charge_create_cost.second < 0) return -32;
		if (!GetSubNodeValue(dataElement, "title_id", m_other_cfg.post_title_id) || m_other_cfg.post_title_id <= 0) return -33;

		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int GuildConfig::InitRoomCfg(TiXmlElement * RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -1000;

	int last_level = 0;
	int last_skill_points = 0;
	while (dataElement)
	{
		GuildRoomCfg node;
		if (!GetSubNodeValue(dataElement, "level", node.level) || node.level != last_level + 1)return -1;
		if (!GetSubNodeValue(dataElement, "exp", node.exp) || node.exp < 0)return -2;
		if (!GetSubNodeValue(dataElement, "max_member", node.max_member) || node.max_member < 0 || node.max_member > GUILD_MAX_MEMBER_COUNT)return -4;
		if (!GetSubNodeValue(dataElement, "cost", node.cost) || node.cost < 0)return -5;
		if (!GetSubNodeValue(dataElement, "hearsay", node.hearsay) || node.hearsay < 0)return -6;
		if (!GetSubNodeValue(dataElement, "skill_points", node.skill_points) || node.skill_points < 0 || node.skill_points < last_skill_points)return -7;
		if (!GetSubNodeValue(dataElement, "skill_level", node.life_skill_level_limit) || node.life_skill_level_limit < 0)return -8;

		last_level = node.level;
		last_skill_points = node.skill_points;

		m_room_map[last_level] = node;
		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int GuildConfig::InitMemberTypeCfg(TiXmlElement * RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -1000;

	while (dataElement)
	{
		GuildMemberTypeCfg node;
		if (!GetSubNodeValue(dataElement, "member_type", node.member_type) || node.member_type < GUILD_POST_PATRIARCH || node.member_type >= GUILD_POST_MAX)return -1;
		if (!GetSubNodeValue(dataElement, "max_num", node.max_num))return -2;

		char str[PRIVILEGE_TYPE_MAX][128] = { "approval", "appoint", "kick", "notice", "impeach" ,"mission_remind" };
		for (int i = 0; i < PRIVILEGE_TYPE_MAX; ++i)
		{
			if (!GetSubNodeValue(dataElement, str[i], node.privilege[i]))return -10 - i;
		}

		if (!GetSubNodeValue(dataElement, "modify_target", node.modify_target))return -1000;
		if (!GetSubNodeValue(dataElement, "level_up", node.level_up))return -1001;
		if (!GetSubNodeValue(dataElement, "build_limit", node.build_limit)) return -1002;

		m_member_type_map[node.member_type] = node;
		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int GuildConfig::InitPetTrainingCfg(TiXmlElement * RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -1000;

	while (dataElement)
	{
		GuildPetTrainingCfg node;
		if (!GetSubNodeValue(dataElement, "type", node.type) || node.type < 0 || node.type >= PET_TRAINING_TYPE_MAX)return -1;
		if (!GetSubNodeValue(dataElement, "level", node.level))return -2;
		if (!GetSubNodeValue(dataElement, "family_level", node.guild_level))return -3;
		if (!GetSubNodeValue(dataElement, "gongxian_cost", node.gongxian_cost))return -4;
		if (!GetSubNodeValue(dataElement, "coin_type", node.coin_type))return -5;
		if (!GetSubNodeValue(dataElement, "coin_cost", node.coin_cost))return -6;
		if (!GetSubNodeValue(dataElement, "addition", node.add_param))return -7;

		m_pet_training_map[node.type][node.level] = node;
		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int GuildConfig::InitTrainingExpCfg(TiXmlElement * RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -1000;

	int last_level = 0;
	while (dataElement)
	{
		int level = 0;
		if (!GetSubNodeValue(dataElement, "level", level) || level != last_level + 1)return -1;
		last_level = level;
		int add_exp = 0;
		if (!GetSubNodeValue(dataElement, "add_exp", add_exp) || add_exp < 0)return -2;
		int gongxian_cost = 0;
		if (!GetSubNodeValue(dataElement, "gongxian_cost", gongxian_cost) || gongxian_cost < 0)return -3;

		m_training_exp_map[level] = add_exp;
		m_training_gongxian_map[level] = gongxian_cost;
		dataElement = dataElement->NextSiblingElement();
	}
	if (last_level != MAX_PET_LEVEL) return -3;
	return 0;
}

int GuildConfig::InitGuildShopCfg(TiXmlElement * RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -1000;

	while (dataElement)
	{
		GuildShopCfg node_cfg;
		if (!GetSubNodeValue(dataElement, "seq", node_cfg.seq) || node_cfg.seq < 0 || node_cfg.seq >= GUILD_SHOP_COUNT_MAX)return -1;
		if (!GetSubNodeValue(dataElement, "level", node_cfg.level) || node_cfg.level < 0)return -2;
		if (!GetSubNodeValue(dataElement, "item_id", node_cfg.item_id) || node_cfg.item_id < 0)return -3;
		if (!GetSubNodeValue(dataElement, "present_cost", node_cfg.present_cost) || node_cfg.present_cost < 0)return -4;
		if (!GetSubNodeValue(dataElement, "gongxian_cost", node_cfg.gongxian_cost) || node_cfg.gongxian_cost < 0)return -5;
		if (!GetSubNodeValue(dataElement, "buy_type", node_cfg.buy_type) || node_cfg.buy_type < BUY_LIMIT_TYPE_UNLIMITED || node_cfg.buy_type >= BUY_LIMIT_TYPE_MAX)return -6;
		if (!GetSubNodeValue(dataElement, "buy_num", node_cfg.buy_num) || node_cfg.buy_num < 0)return -7;
		if (!GetSubNodeValue(dataElement, "cost_type", node_cfg.cost_type) || node_cfg.cost_type < BUY_COST_TYPE_GOLD || node_cfg.cost_type > BUY_COST_TYPE_BIND_COIN)return -8;

		m_shop_map[node_cfg.seq] = node_cfg;
		dataElement = dataElement->NextSiblingElement();
	}
	return 0;
}

int GuildConfig::InitGuildBossCfg(TiXmlElement* RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -1000;

	int max_chapter_id = 0;
	int last_boss_index = 0;
	while (dataElement)
	{
		int chapter_id = 0;
		if (!GetSubNodeValue(dataElement, "chapter_id", chapter_id) || chapter_id < 0 || chapter_id >= MAX_GUILD_CHAPTER_BOSS_NUM)
		{
			return -1;
		}
		if (max_chapter_id != chapter_id)
		{
			last_boss_index = 0;
		}
		max_chapter_id = GetMax(chapter_id, max_chapter_id);

		GuildBossCfg & chapter_cfg = m_boss_cfg_list[chapter_id];

		if (!GetSubNodeValue(dataElement, "world_levels", chapter_cfg.world_level) || chapter_cfg.world_level <= 0 || chapter_cfg.world_level > MAX_ROLE_LEVEL)
		{
			return -22;
		}

		GuildBossCfg::GuildBossCfgItem& cfg = chapter_cfg.guild_boss_cfg;
		if (!GetSubNodeValue(dataElement, "boss_seq", cfg.boss_index) || cfg.boss_index != last_boss_index + 1)
		{
			return -23;
		}
		last_boss_index = cfg.boss_index;

		if (!GetSubNodeValue(dataElement, "boss_id", cfg.boss_group_id) || (NULL == MonsterGroupPool::GetInstance().GetMonsterGroupCfg(cfg.boss_group_id)))
		{
			return -2;
		}

		if (!GetSubNodeValue(dataElement, "join_coin", cfg.join_coin) || cfg.join_coin < 0)
		{
			return -3;
		}
		if (!GetSubNodeValue(dataElement, "join_gongxian", cfg.join_gongxian) || cfg.join_coin < 0)
		{
			return -4;
		}
		if (!GetSubNodeValue(dataElement, "family_coin", cfg.family_coin) || cfg.family_coin < 0)
		{
			return -5;
		}
		if (!GetSubNodeValue(dataElement, "immortal", cfg.immortal) || cfg.immortal < 0)
		{
			return -6;
		}

		TiXmlElement *list_element = dataElement->FirstChildElement("kill_item_list");
		if (NULL != list_element)
		{
			TiXmlElement *item_element = list_element->FirstChildElement("kill_item");
			while (NULL != item_element)
			{
				ItemConfigData item;
				if (!item.ReadConfig(item_element)) return -100;

				if (cfg.item_count >= MAX_GUILD_REWARD_GROUP_ITEM_NUM) return -222;

				cfg.kill_reward_list[cfg.item_count++] = item;
				item_element = item_element->NextSiblingElement();
			}
		}

		dataElement = dataElement->NextSiblingElement();
	}

	for (int chapter_id = 0; chapter_id <= max_chapter_id; ++chapter_id)
	{
		const GuildBossCfg* boss_cfg = this->GetBossChapterCfg(chapter_id);
		if (NULL == boss_cfg)
		{
			// chapter_id没有连续
			return -10086;
		}
	}

	return 0;
}

int GuildConfig::InitGuildBossChapterRewardCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	while (NULL != root_element)
	{
		int reward_group_id;
		if (!GetSubNodeValue(root_element, "group_id", reward_group_id))
		{
			return -1;
		}

		GuildBossChapterRewardCfg::ItemCfg item_cfg;
		if (!GetSubNodeValue(root_element, "probability", item_cfg.rate) || item_cfg.rate <= 0)
		{
			return -2;
		}

		TiXmlElement *list_element = root_element->FirstChildElement("reward_item_list");
		if (NULL != list_element)
		{
			TiXmlElement *item_element = list_element->FirstChildElement("reward_item");
			while (NULL != item_element)
			{
				ItemConfigData item;
				if (!item.ReadConfig(item_element)) return -100;

				if (item_cfg.item_count >= MAX_GUILD_REWARD_GROUP_ITEM_NUM) return -222;

				item_cfg.reward_list[item_cfg.item_count++] = item;
				item_element = item_element->NextSiblingElement();
			}
		}
		if (!GetSubNodeValue(root_element, "coin", item_cfg.coin) || item_cfg.coin < 0)
		{
			return -3;
		}

		if (!GetSubNodeValue(root_element, "gongxian", item_cfg.gongxian) || item_cfg.gongxian < 0)
		{
			return -4;
		}


		if (!GetSubNodeValue(root_element, "family_coin", item_cfg.guild_coin_per_join) || item_cfg.guild_coin_per_join < 0)
		{
			return -5;
		}

		m_guild_boss_chapter_item_reward_map[reward_group_id].rate_count += item_cfg.rate;
		m_guild_boss_chapter_item_reward_map[reward_group_id].group_reward.push_back(item_cfg);
		root_element = root_element->NextSiblingElement();
	}

	for (std::map<int, GuildBossChapterRewardCfg>::iterator iter = m_guild_boss_chapter_item_reward_map.begin(); iter != m_guild_boss_chapter_item_reward_map.end(); ++iter)
	{
		int count = 0;
		for (int i = 0; i < (int)iter->second.group_reward.size(); ++i)
		{
			GuildBossChapterRewardCfg::ItemCfg & t = iter->second.group_reward[i];
			count += t.rate;
		}
		if (count <= 0)
		{
			return -10086;
		}

	}

	return 0;
}

int GuildConfig::InitGuildJournalCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	while (NULL != root_element)
	{
		GuildJournalCfg node_cfg;
		if (!GetSubNodeValue(root_element, "type", node_cfg.type) || node_cfg.type < GUILD_EVENTHANDLER_ADD_MEMBER || node_cfg.type >= GUILD_EVENTHANDLER_TYPE_MAX)
		{
			return -1;
		}
		if (!GetSubNodeValue(root_element, "condition", node_cfg.condition) || node_cfg.condition < 0)
		{
			return -2;
		}

		m_journal_cfg.push_back(node_cfg);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int GuildConfig::InitGuildRankRewardCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	while (NULL != root_element)
	{
		int chapter_id = 0, seq = 0;
		if (!GetSubNodeValue(root_element, "chapter_id", chapter_id) || chapter_id < 0 || chapter_id >= MAX_GUILD_CHAPTER_BOSS_NUM)
		{
			return -1;
		}
		if (!GetSubNodeValue(root_element, "seq", seq) || seq < 0 || seq >= MAX_GUILD_BOSS_CHAPTER_RANK_NUM)
		{
			return -2;
		}

		GuildRankRewardCfg node_cfg;
		if (!GetSubNodeValue(root_element, "min_rank", node_cfg.min_rank) || node_cfg.min_rank < 0 || node_cfg.min_rank > GUILD_MAX_MEMBER_COUNT)
		{
			return -3;
		}
		if (!GetSubNodeValue(root_element, "max_rank", node_cfg.max_rank) || node_cfg.max_rank < node_cfg.min_rank || node_cfg.max_rank > GUILD_MAX_MEMBER_COUNT)
		{
			return -3;
		}
		if (!GetSubNodeValue(root_element, "exp", node_cfg.exp) || node_cfg.exp < 0)
		{
			return -3;
		}
		if (!GetSubNodeValue(root_element, "bind_coin", node_cfg.bind_coin) || node_cfg.bind_coin < 0)
		{
			return -4;
		}

		if (!GetSubNodeValue(root_element, "reward_group_id", node_cfg.reward_group_id) || m_guild_boss_chapter_item_reward_map.find(node_cfg.reward_group_id) == m_guild_boss_chapter_item_reward_map.end())
		{
			return -5;
		}
		if (!GetSubNodeValue(root_element, "immortal", node_cfg.immortal) || node_cfg.immortal < 0)
		{
			return -6;
		}

		m_rank_reward_cfg[chapter_id][seq] = node_cfg;
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int GuildConfig::InitGuildGatherDreamCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	while (NULL != root_element)
	{
		int quality = 0, type = 0;
		if (!GetSubNodeValue(root_element, "quality", quality) || quality <= 0 || quality >= MAX_GATHER_DREAM_TYPE_NUM)
		{
			return -1;
		}

		if (!GetSubNodeValue(root_element, "type", type) || type < GUILD_GATHER_DREAM_ITEM_TYPE_PET || type >= GUILD_GATHER_DREAM_ITEM_TYPE_MAX)
		{
			return -2;
		}

		GuildGatherDreamCfg node_cfg;
		if (!GetSubNodeValue(root_element, "is_can_pray", node_cfg.is_can_pray) || node_cfg.is_can_pray < 0)
		{
			return -3;
		}

		if (!GetSubNodeValue(root_element, "pray_patch_num", node_cfg.pray_patch_num) || node_cfg.pray_patch_num < 0)
		{
			return -4;
		}

		if (!GetSubNodeValue(root_element, "coin", node_cfg.coin) || node_cfg.coin < 0)
		{
			return -5;
		}

		if (!GetSubNodeValue(root_element, "gongxian", node_cfg.gongxian) || node_cfg.gongxian < 0)
		{
			return -6;
		}

		if (!GetSubNodeValue(root_element, "exp", node_cfg.guild_exp) || node_cfg.guild_exp < 0)
		{
			return -7;
		}

		TiXmlElement * reward_element = root_element->FirstChildElement("reward_item");
		if (NULL == reward_element)
		{
			return -8;
		}
		if (!node_cfg.reward_item.ReadConfig(reward_element))
		{
			return -9;
		}

		m_gather_dream_cfg[type][quality] = node_cfg;
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int GuildConfig::InitGuildActiveGiftCfg(TiXmlElement *RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -101010;
	}

	int last_seq = -1;
	int last_level_min = -1, last_level_max = -1;
	GuildActiveGiftCfg temp_cfg;
	while (NULL != root_element)
	{
		int level_min = 0, level_max = 0;

		if (!GetSubNodeValue(root_element, "level_min", level_min) || level_min < 1 )
		{
			return -1;
		}

		if (!GetSubNodeValue(root_element, "level_max", level_max) || level_max < 1)
		{
			return -2;
		}

		if (level_min> level_max)
			return -201;

		if (level_min != last_level_min &&  level_min < last_level_max)
		{
			return -202;
		}

		if (-1!= last_level_min &&  last_level_min != level_min)
		{
			m_active_gift_cfg.push_back(temp_cfg);
			temp_cfg.Reset();
			last_seq = -1;
		}

		temp_cfg.level_min = level_min;
		temp_cfg.level_max = level_max;

		int seq = 0;
		GuildActiveGiftCfg::GiftCfg _cfg;
		if (!GetSubNodeValue(root_element, "seq", seq) || seq < 0 || seq >= 32) //礼包记录是用int按位标记 只有0~31
		{
			return -3;
		}

		if (-1 == last_seq && 0 != seq)
		{
			return -301;
		}

		if (-1 != last_seq && last_seq + 1 != seq)
		{
			return -302;
		}

		if (!GetSubNodeValue(root_element, "active_limit", _cfg.active_limit) || _cfg.active_limit < 0)
		{
			return -4;
		}

		if (!GetSubNodeValue(root_element, "exp", _cfg.exp) || _cfg.exp < 0)
		{
			return -5;
		}

		// 这样判断可以方便策划哪天想删除奖励
		TiXmlElement *list_element = root_element->FirstChildElement("reward_list");
		if (NULL != list_element)
		{
			TiXmlElement *item_element = list_element->FirstChildElement("reward");
			_cfg.count = 0;
			while (NULL != item_element)
			{
				if (_cfg.count >= MAX_GUILD_REWARD_GROUP_ITEM_NUM)
					return -301;

				ItemConfigData item;
				if (!_cfg.reward_list[_cfg.count++].ReadConfig(item_element))
					return -302;

				item_element = item_element->NextSiblingElement();
			}
		}

		temp_cfg.gift_cfg_vec.push_back(_cfg);
		last_seq = seq;
		last_level_min = temp_cfg.level_min;
		last_level_max = temp_cfg.level_max;
		root_element = root_element->NextSiblingElement();
	}

	m_active_gift_cfg.push_back(temp_cfg);

	return 0;
}

int GuildConfig::InitGuildActiveRankRewardCfg(TiXmlElement *RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -101010;
	}

	int last_min = -1, last_max = -1;
	while (NULL != root_element)
	{
		GuildActiveRankRewardCfg temp_cfg;
		if (!GetSubNodeValue(root_element, "rank_min", temp_cfg.rank_min) || temp_cfg.rank_min <= 0) 
		{
			return -1;
		}

		if (!GetSubNodeValue(root_element, "rank_max", temp_cfg.rank_max) || temp_cfg.rank_max <= 0)
		{
			return -2;
		}

		if (temp_cfg.rank_max < temp_cfg.rank_min)
		{
			return -201;
		}

		if (-1 == last_min && 1 != temp_cfg.rank_min)
		{
			return -202;
		}

		if (-1 == last_max && 1 != temp_cfg.rank_max)
		{
			return -203;
		}

		if ((-1 != last_min || -1 != last_max) && temp_cfg.rank_min <= last_max)
		{
			return -204;
		}

		if (temp_cfg.rank_max > CROSS_GUILD_ACTIVE_RANK_REWARD_COUNT)
		{
			return -205;
		}

		// 这样判断可以方便策划哪天想删除奖励
		TiXmlElement *list_element = root_element->FirstChildElement("reward_list");
		if (NULL != list_element)
		{
			TiXmlElement *item_element = list_element->FirstChildElement("reward");
			temp_cfg.count = 0;
			while (NULL != item_element)
			{
				if (temp_cfg.count >= MAX_GUILD_REWARD_GROUP_ITEM_NUM)
					return -301;

				ItemConfigData item;
				if (!temp_cfg.reward_list[temp_cfg.count++].ReadConfig(item_element))
					return -302;

				item_element = item_element->NextSiblingElement();
			}
		}

		if (!GetSubNodeValue(root_element, "gongxian", temp_cfg.gongxian) || temp_cfg.gongxian < 0)
		{
			return -4;
		}

		if (!GetSubNodeValue(root_element, "coin", temp_cfg.coin) || temp_cfg.coin < 0)
		{
			return -5;
		}

		m_active_rank_reward_cfg.push_back(temp_cfg);
		last_min = temp_cfg.rank_min;
		last_max = temp_cfg.rank_max;
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int GuildConfig::InitGuildActivePersonRankRewardCfg(TiXmlElement *RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -101010;
	}

	int last_min = -1, last_max = -1;
	while (NULL != root_element)
	{
		GuildActivePersonRankRewardCfg temp_cfg;
		if (!GetSubNodeValue(root_element, "rank_min", temp_cfg.rank_min) || temp_cfg.rank_min <= 0)
		{
			return -1;
		}

		if (!GetSubNodeValue(root_element, "rank_max", temp_cfg.rank_max) || temp_cfg.rank_max <= 0)
		{
			return -2;
		}

		if (temp_cfg.rank_max < temp_cfg.rank_min)
		{
			return -201;
		}

		if (-1 == last_min && 1 != temp_cfg.rank_min)
		{
			return -202;
		}

		if (-1 == last_max && 1 != temp_cfg.rank_max)
		{
			return -203;
		}

		if ((-1 != last_min || -1 != last_max) && temp_cfg.rank_min <= last_max)
		{
			return -204;
		}

		if (temp_cfg.rank_max > GUILD_ACTIVE_PERSON_RANK_REWARD_COUNT)
		{
			return -205;
		}

		// 这样判断可以方便策划哪天想删除奖励
		TiXmlElement *list_element = root_element->FirstChildElement("reward_list");
		if (NULL != list_element)
		{
			TiXmlElement *item_element = list_element->FirstChildElement("reward");
			temp_cfg.count = 0;
			while (NULL != item_element)
			{
				if (temp_cfg.count >= MAX_GUILD_REWARD_GROUP_ITEM_NUM)
					return -301;

				ItemConfigData item;
				if (!temp_cfg.reward_list[temp_cfg.count++].ReadConfig(item_element))
					return -302;

				item_element = item_element->NextSiblingElement();
			}
		}

		if (!GetSubNodeValue(root_element, "title", temp_cfg.title) || temp_cfg.title < 0)
		{
			return -4;
		}

		if (!GetSubNodeValue(root_element, "gongxian", temp_cfg.gongxian) || temp_cfg.gongxian < 0)
		{
			return -5;
		}

		if (!GetSubNodeValue(root_element, "coin", temp_cfg.coin) || temp_cfg.coin < 0)
		{
			return -6;
		}

		m_active_person_rank_reward_cfg.push_back(temp_cfg);
		last_min = temp_cfg.rank_min;
		last_max = temp_cfg.rank_max;
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int GuildConfig::InitGuildInbornCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}
	int gain_target_max = 0;
	for (int i = 0; i < GUILD_INBORN_ADD_TARGET_TYPE_MAX; i++)
	{
		gain_target_max |= (1 << i);
	}


	int last_type = 0, last_seq = 0, last_level = 0;
	while (NULL != root_element)
	{
		int inborn_type = 0, type_seq = 0, skill_level = 0;
		if (!GetSubNodeValue(root_element, "inborn_type", inborn_type) || inborn_type < 0 || inborn_type >= ARRAY_ITEM_COUNT(m_guild_inborn_list)) return -1;
		if (!GetSubNodeValue(root_element, "type_seq", type_seq) || type_seq < 0 || type_seq >= ARRAY_ITEM_COUNT(m_guild_inborn_list[inborn_type])) return -2;
		if (!GetSubNodeValue(root_element, "skill_level", skill_level) || skill_level <= 0 || skill_level > NAX_GUILD_INBORN_SKILL_LEVEL_NUM) return -3;

		GuildInbornCfg & cfg = m_guild_inborn_list[inborn_type][type_seq];
		if (last_type == inborn_type)
		{
			if (last_seq == type_seq)
			{
				if (last_level + 1 != skill_level)
				{
					return -4;
				}
			}
			else if (last_seq + 1 == type_seq)
			{
				if(1 != skill_level)
				{
					return -5;
				}
			}
			else 
			{
				return -6;
			}
		}
		else if (last_type + 1 == inborn_type)
		{
			if (0 != type_seq)
			{
				return -7;
			}
			else if (1 != skill_level)
			{
				return -8;
			}
		}
		else
		{
			return -9;
		}

		

		cfg.max_level = GetMax(cfg.max_level, skill_level);
		GuildInbornLevelCfg & node_cfg = cfg.level_list[skill_level - 1];

		if (!GetSubNodeValue(root_element, "inborn_skill", node_cfg.inborn_skill_id) || NULL == SkillPool::Instance().GetPassiveSkillCfg(node_cfg.inborn_skill_id)) return -10;
		if (!GetSubNodeValue(root_element, "prepose_seq", node_cfg.prepose_seq))  return -11;
		if (!GetSubNodeValue(root_element, "level_limit", node_cfg.level_limit) || node_cfg.level_limit <= 0 || node_cfg.level_limit > MAX_ROLE_LEVEL)  return -12;
		if (!GetSubNodeValue(root_element, "family_level_limit", node_cfg.guild_level_limit) || node_cfg.guild_level_limit <= 0)  return -13;
		if (!GetSubNodeValue(root_element, "reset_consume", node_cfg.reset_consume) || node_cfg.reset_consume <= 0)  return -14;
		if (!GetSubNodeValue(root_element, "skill_grade", node_cfg.skill_grade) || node_cfg.skill_grade <= 0)  return -15;
		if (!GetSubNodeValue(root_element, "gain_target", node_cfg.gain_target) || node_cfg.gain_target <= 0 || node_cfg.gain_target > gain_target_max)  return -16;

		TiXmlElement * item_list_element = root_element->FirstChildElement("inborn_consume_item_list");
		if (NULL == item_list_element)
		{
			return -100;
		}

		TiXmlElement * item_element = item_list_element->FirstChildElement("inborn_consume_item");
		if (NULL == item_element)
		{
			return -101;
		}
		while (NULL != item_element)
		{
			ItemConfigData item;
			if (!item.ReadConfig(item_element))
			{
				return -102;
			}

			node_cfg.consume_list.push_back(item);
			item_element = item_element->NextSiblingElement();
		}

		int inborn_key = this->GetGuildInbornKey(inborn_type, type_seq);
		if (node_cfg.prepose_seq != -1 && (last_type != inborn_type || last_seq != type_seq))
		{
			int pre_inborn_key = this->GetGuildInbornKey(inborn_type, node_cfg.prepose_seq);
			if (NULL == this->GetGuildInbornCfg(inborn_type, node_cfg.prepose_seq, 1))
			{
				return -50;
			}

			//组建关系链,用于重置技能时重置该技能的后置技能,重置时用到递归,构建这里如果修改需要谨慎
			m_guild_inborn_relation_map[pre_inborn_key].push_back(inborn_key);
		}

		last_type = inborn_type;
		last_seq = type_seq;
		last_level = skill_level;
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int GuildConfig::InitGuildBlessBaseCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}
	while (NULL != root_element)
	{
		int bless_list = 0;
		if (!GetSubNodeValue(root_element, "bless_list", bless_list) || bless_list - 1 < 0 || bless_list - 1 >= MAX_BLESS_TIMES) return -1;
		BlessTimesList & node = bless_time_config[bless_list - 1];
		node.bless_list = bless_list;
		if (!GetSubNodeValue(root_element, "gongxian_reward", node.gongxian_reward)) return -2;
		if (!GetSubNodeValue(root_element, "cost_type", node.cost_type)) return -3;
		if (!GetSubNodeValue(root_element, "bless_consume", node.bless_consume)) return -4;
		if (!GetSubNodeValue(root_element, "bless_add_once", node.bless_add_once)) return -5;
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int GuildConfig::InitGuildBlessRewardCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}
	while (NULL != root_element)
	{
		int world_level = 0;
		if (!GetSubNodeValue(root_element, "world_level", world_level)) return -1;
		BlessTimesAttr & node = m_bless_attr_map[world_level];
		node.world_level = world_level;
		
		for (int i = 0; i < BlessTimesAttr::MAX_ATTR_LIST_NUM; i++)
		{
			std::string read_path = STRING_SPRINTF("attr_type_%d", i + 1);
			BlessAttrInfo &attr_node = node.attr_list[i];
			if (!GetSubNodeValue(root_element, read_path.c_str(), attr_node.attr_type))return -2;
			read_path.clear();
			read_path = STRING_SPRINTF("attr_add_%d", i + 1);
			if (!GetSubNodeValue(root_element, read_path.c_str(), attr_node.attr_value))return -3;
		}

		root_element = root_element->NextSiblingElement();
	}

	return 0; 
}

int GuildConfig::GetGuildInbornKey(int inborn_type, int type_seq) const
{
	return inborn_type * 100 + type_seq;
}

void GuildConfig::CalcGuildInbornResetSkill(int guild_inborn_key, GuildInbornSkill * learn_list, int learn_count, ARG_OUT std::map<ItemID, int> & return_item_map) const
{
	std::map<int, std::vector<int> >::const_iterator it = m_guild_inborn_relation_map.find(guild_inborn_key);
	if (it == m_guild_inborn_relation_map.end() || it->second.empty()) return;

	for (int i = 0; i < (int)it->second.size(); i++)
	{
		int type_seq = it->second[i] % 100;
		if (type_seq < 0 || type_seq >= learn_count || learn_list[type_seq].IsInvaild()) continue;
		
		this->GetGuildInbornReturnItem(it->second[i], learn_list[type_seq].skill_level, return_item_map);
		this->CalcGuildInbornResetSkill(it->second[i], learn_list, learn_count, return_item_map);
		learn_list[type_seq].Reset();
	}
}

void GuildConfig::GetGuildInbornReturnItem(int guild_inborn_key, int skill_level, ARG_OUT std::map<ItemID, int>& return_item_map) const
{
	int inborn_type = guild_inborn_key / 100;
	int type_seq = guild_inborn_key % 100;
	if (inborn_type < 0 || inborn_type >= ARRAY_LENGTH(m_guild_inborn_list) ||
		type_seq < 0 || type_seq >= ARRAY_LENGTH(m_guild_inborn_list[inborn_type]))
	{
		return;
	}

	int max_level = GetMin(skill_level, m_guild_inborn_list[inborn_type][type_seq].max_level);
	if (max_level <= 0)
	{
		return;
	}
	
	const GuildInbornCfg & cfg = m_guild_inborn_list[inborn_type][type_seq];
	for (int i = 0; i < max_level && i < ARRAY_LENGTH(cfg.level_list); ++i)
	{
		if(cfg.level_list[i].consume_list.empty()) continue;

		for (int item_index = 0; item_index < (int)cfg.level_list[i].consume_list.size(); ++item_index)
		{
			const ItemConfigData & item = cfg.level_list[i].consume_list[item_index];
			return_item_map[item.item_id] += item.num;
		}
	}
}

int GuildConfig::InitGuildHongbaoCfg(TiXmlElement *RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -101010;
	}

	while (NULL != root_element)
	{
		int type = 0;

		if (!GetSubNodeValue(root_element, "redbag_type", type) || type <= HONGBAO_TYPE_INVALID || type >= HONGBAO_GUILD_TYPE_MAX)
		{
			return -1;
		}

		GuildHongBaoCfg::HongBaoCfg & temp_cfg = m_hongbao_cfg.hong_bao_cfg[type];
		if (temp_cfg.lingyu_sum != 0)
		{
			return -101;
		}

		if (!GetSubNodeValue(root_element, "redbag_sum", temp_cfg.lingyu_sum ) || temp_cfg.lingyu_sum <= 0)
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "redbag_num", temp_cfg.redbag_num) || temp_cfg.redbag_num <= 0)
		{
			return -3;
		}

		if (!GetSubNodeValue(root_element, "redbag_min", temp_cfg.lingyu_min) || temp_cfg.lingyu_min < 1)
		{
			return -4;
		}

		if (!GetSubNodeValue(root_element, "redbag_max", temp_cfg.lingyu_max) )
		{
			return -5;
		}

		if (temp_cfg.lingyu_max < temp_cfg.lingyu_min)
		{
			return -501;
		}

		if (!GetSubNodeValue(root_element, "redbag_change", temp_cfg.lingyu_change) || temp_cfg.lingyu_change < 0)
		{
			return -6;
		}

		if (temp_cfg.lingyu_max > temp_cfg.lingyu_sum - temp_cfg.redbag_num * temp_cfg.lingyu_min - ( temp_cfg.redbag_num - 1) * temp_cfg.lingyu_change)
		{
			return -601;
		}

		if (!GetSubNodeValue(root_element, "is_take_effect", temp_cfg.is_take_effect) )
		{
			return -7;
		}

		if (!GetSubNodeValue(root_element, "times_limit", temp_cfg.day_times_limit) || temp_cfg.day_times_limit < 0)
		{
			return -8;
		}

		if (!GetSubNodeValue(root_element, "top_up_num", temp_cfg.need_chong_zhi) || temp_cfg.need_chong_zhi < 0)
		{
			return -9;
		}

		if (!GetSubNodeValue(root_element, "redbag_heroism", temp_cfg.redbag_heroism) || temp_cfg.redbag_heroism < 0)
		{
			return -10;
		}

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int GuildConfig::InitGuildHofSortCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	while (NULL != root_element)
	{
		int type = 0;

		if (!GetSubNodeValue(root_element, "type", type) || type < Guild::HALLOFFAME_TYPE_BATTLE || type >= Guild::HALLOFFAME_TYPE_MAX)
		{
			return -1;
		}

		std::vector<HallOfFameRank> & cfg = m_hof_sort_map[type];
		HallOfFameRank node;
		node.type = type;
		if (!GetSubNodeValue(root_element, "rank_min", node.rank_min) || node.rank_min <= 0)
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "rank_max", node.rank_max) || node.rank_max <= 0)
		{
			return -3;
		}

		TiXmlElement * title_element = root_element->FirstChildElement("title");
		if (NULL != title_element && title_element->FirstChild())
		{
			if (!GetSubNodeValue(root_element, "title", node.title) || NULL == ITEMPOOL->GetItem(node.title))
			{
				return -4;
			}
		}

		if (!GetSubNodeValue(root_element, "gongxian", node.gongxian))
		{
			return -5;
		}

		if (!GetSubNodeValue(root_element, "coin", node.coin) || node.coin < 0)
		{
			return -6;
		}

		cfg.push_back(node);

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int GuildConfig::InitGuildWarthGetCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -1;
	}

	while (NULL != root_element)
	{
		int type = 0;

		if (!GetSubNodeValue(root_element, "act_type", type) || type <= BATTLE_MODE_INVALID || type >= BATTLE_MODE_MAX)
		{
			return -1;
		}

		WarthAddInfo & node = m_warth_map[type];
		
		if (!GetSubNodeValue(root_element, "once_warth_add", node.once_warth_add))
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "captain_warth_add", node.captain_warth_add))
		{
			return -3;
		}

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int GuildConfig::InitGuildConstructCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)  return -554;

	while (NULL != root_element)
	{
		int build_type = 0, build_level = 0;	
		if (!GetSubNodeValue(root_element, "build_type", build_type) || build_type <= 0 || build_type >= GUILD_BUILD_TYPE_MAX) return -__LINE__;

		if (m_guild_construct_cfg.empty())
		{
			m_guild_construct_cfg.push_back(GuildConstructCfg());
			m_guild_construct_cfg[m_guild_construct_cfg.size() - 1].build_type = build_type;
		}
		else
		{
			if (m_guild_construct_cfg[m_guild_construct_cfg.size() - 1].build_type != build_type)
			{
				m_guild_construct_cfg.push_back(GuildConstructCfg());
				m_guild_construct_cfg[m_guild_construct_cfg.size() - 1].build_type = build_type;
			}
		}
		GuildConstructCfg& gc_cfg = m_guild_construct_cfg[m_guild_construct_cfg.size() - 1];
 		GuildBaseConstructCfg gbc_cfg;

		if (!GetSubNodeValue(root_element, "build_level", build_level) || build_level < 0) return -__LINE__;
		if (!GetSubNodeValue(root_element, "up_expend_capital", gbc_cfg.up_expend_capital) || gbc_cfg.up_expend_capital < 0) return -__LINE__;
		if (!GetSubNodeValue(root_element, "up_expend_time", gbc_cfg.up_expend_time) || gbc_cfg.up_expend_time < 0) return -__LINE__;
		if (!GetSubNodeValue(root_element, "paramet_1", gbc_cfg.paramet_1) || gbc_cfg.paramet_1 < 0) return -__LINE__;

		if (build_type == GUILD_BUILD_TYPE_HALL)		// 获取建筑最高等级
		{
			m_hall_max_level = build_level > m_hall_max_level ? build_level : m_hall_max_level;
		}
		else
		{
			m_other_build_max_level = build_level > m_other_build_max_level ? build_level : m_other_build_max_level;
		}

		gc_cfg.base_list[build_level] = gbc_cfg;

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

