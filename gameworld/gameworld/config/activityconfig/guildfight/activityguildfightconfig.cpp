#include "activityguildfightconfig.hpp"
#include "world.h"
#include "config/logicconfigmanager.hpp"
#include "other/title/titleconfig.hpp"

ActivityGuildFightConfig::~ActivityGuildFightConfig()
{
	std::vector<GuildFightRoleLevelRankAward>().swap(m_rank_award);
	for (std::map<int, std::vector<GuildFightItemRand> >::iterator it = m_item_group_cfg.begin(); it != m_item_group_cfg.end(); it++)
	{
		std::vector<GuildFightItemRand>().swap(it->second);
	}

	std::vector<GuildFightJoinAwardCfg>().swap(m_join_award_vec);

	for (std::map<int, std::vector<GuildFightFindRewardGroupCfg> >::iterator it = m_find_reward_group_map.begin(); it != m_find_reward_group_map.end(); it++)
	{
		std::vector<GuildFightFindRewardGroupCfg>().swap(it->second);
	}
}

bool ActivityGuildFightConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG

	LOAD_CONFIG("other", InitGuildFightOthersCfg);			// 其它配置
	LOAD_CONFIG("rank_reward", InitGuildFightRankAward);	// 排名奖励
	LOAD_CONFIG("live_reward", InitGuildFightSurvAward);	// 存活奖励
	LOAD_CONFIG("box_reward", InitGuildFightItemsRand);		// 奖励组配置
	LOAD_CONFIG("box", InitGuildFightChestsCfg);			// 宝箱配置
	LOAD_CONFIG("random_entrance", InitGuildFightBornPosiCfg);			// 出生配置
	LOAD_CONFIG("participation_reward", InitGuildFightJoinAwardCfg);	// 参与奖励
	LOAD_CONFIG("reward_find_group", InitGuildFightFindAwardGroupCfg);	// 找回奖励组
	LOAD_CONFIG("reward_find", InitGuildFightFindAwardCfg);				// 找回奖励

	return true;
}

void ActivityGuildFightConfig::GetChestIDList(std::vector<int> & out_list)
{
	out_list.clear();
	for (std::map<int, GuildFightChestCfg>::iterator loop = m_chest_cfg.begin(); loop != m_chest_cfg.end(); ++loop)
	{
		out_list.push_back(loop->first);
	}
}

void ActivityGuildFightConfig::GetSurvivalCheckNumList(std::set<int> & out_list)
{
	out_list.clear();
	for (std::map<int, std::map<ParamSection, GuildFightSurvAward> >::iterator loop = m_survival_award.begin(); loop != m_survival_award.end(); ++loop)
	{
		out_list.insert(loop->first);
	}
}

const GuildFightChestCfg * ActivityGuildFightConfig::GetGuildFightChestCfg(int chest_id)
{
	std::map<int, GuildFightChestCfg>::iterator find_it = m_chest_cfg.find(chest_id);
	if (m_chest_cfg.end() != find_it) return & find_it->second;

	return NULL;
}

const GuildFightRankAward * ActivityGuildFightConfig::GetGuildFightActivityAward(int rank_posi, int role_level)
{
	for (int i = 0; i < (int)m_rank_award.size(); ++i)
	{
		const GuildFightRoleLevelRankAward & curr=m_rank_award[i];
		if (curr.min_role_level <= role_level && role_level <= curr.max_role_level)
		{
			const std::map<ParamSection, GuildFightRankAward> & rank_reward = curr.rank_reward;
			for (std::map<ParamSection, GuildFightRankAward>::const_iterator loop_it = rank_reward.begin(); loop_it != rank_reward.end(); ++loop_it)
			{
				if (rank_posi >= loop_it->first.section_min && rank_posi <= loop_it->first.section_max) 
					return &loop_it->second;
			}
		}
	}

	return NULL;
}

const GuildFightSurvAward * ActivityGuildFightConfig::GetGuildFightSurvivalAward(int check_num, int level)
{
	std::map<int, std::map<ParamSection, GuildFightSurvAward> >::iterator find_it = m_survival_award.find(check_num);
	if (m_survival_award.end() == find_it) return NULL;

	for (std::map<ParamSection, GuildFightSurvAward>::iterator level_it = find_it->second.begin(); level_it != find_it->second.end(); ++level_it)
	{
		if (level >= level_it->first.section_min && level <= level_it->first.section_max) return & level_it->second;
	}

	return NULL;
}

bool ActivityGuildFightConfig::RandAwardListByChestID(int chest_id, std::vector<ItemConfigData> & out_award_list, bool is_leader)
{
	out_award_list.clear();
	std::map<int, GuildFightChestCfg>::iterator find_it = m_chest_cfg.find(chest_id);
	if (m_chest_cfg.end() == find_it) return false;

	if(!is_leader)
	{
		int award_num = find_it->second.award_id_num;
		for (int award_idx = 0; award_idx < award_num && award_idx < GUILD_FIGHT_CHEST_MAX_AWARD; ++award_idx)
		{
			int award_group_id = find_it->second.award_id_list[award_idx];
			std::map<int, std::vector<GuildFightItemRand> >::iterator group_it = m_item_group_cfg.find(award_group_id);
			if (m_item_group_cfg.end() == group_it) continue;

			int prob_rate = rand() % RAND_UNIVERSAL_RATIO_BASE_NUM;
			for (std::vector<GuildFightItemRand>::iterator award_it = group_it->second.begin(); award_it != group_it->second.end(); ++award_it)
			{
				if (prob_rate < award_it->prob)
				{
					out_award_list.push_back(award_it->award_item);
					break;
				}

				prob_rate -= award_it->prob;
			}
		}
	}
	else
	{
		int caotain_award_id_num = find_it->second.caotain_award_id_num;
		for (int award_idx = 0; award_idx < caotain_award_id_num && award_idx < GUILD_FIGHT_CHEST_MAX_AWARD; ++award_idx)
		{
			int award_group_id = find_it->second.caotain_award_id_list[award_idx];
			std::map<int, std::vector<GuildFightItemRand> >::iterator group_it = m_item_group_cfg.find(award_group_id);
			if (m_item_group_cfg.end() == group_it) continue;

			int prob_rate = rand() % RAND_UNIVERSAL_RATIO_BASE_NUM;
			for (std::vector<GuildFightItemRand>::iterator award_it = group_it->second.begin(); award_it != group_it->second.end(); ++award_it)
			{
				if (prob_rate < award_it->prob)
				{
					out_award_list.push_back(award_it->award_item);
					break;
				}

				prob_rate -= award_it->prob;
			}
		}
	}

	return true;
}

const GuildFightChestCfg * ActivityGuildFightConfig::GetRandGuildFightChestCfg()
{
	int rand_num = rand() % chest_weight;
	for (std::map<int, GuildFightChestCfg>::iterator it = m_chest_cfg.begin(); it != m_chest_cfg.end(); it++)
	{
		if (rand_num < it->second.rate)
		{
			return &it->second;
		}
		rand_num -= it->second.rate;
	}
	return NULL;
}

const GuildFightJoinAwardCfg * ActivityGuildFightConfig::GetGuildFightJoinAwardCfg(int role_level)
{
	for (int i = 0; i < (int)m_join_award_vec.size(); i++)
	{
		if (m_join_award_vec[i].min_level <= role_level && m_join_award_vec[i].max_level >= role_level)
		{
			return &m_join_award_vec[i];
		}
	}

	return NULL;
}

const GuildFightFindCfg * ActivityGuildFightConfig::GetFindCfg(int find_type, int role_level) const
{
	if (find_type < 0 || find_type >= RB_TYPE_MAX) return NULL;

	const GuildFightFindGroupCfg & node_cfg = m_find_cfg[find_type];
	for (int i = 0; i < (int)node_cfg.node_list.size(); i++)
	{
		if (node_cfg.node_list[i].min_level <= role_level && node_cfg.node_list[i].max_level >= role_level)
		{
			return &node_cfg.node_list[i];
		}
	}
	return NULL;
}

const GuildFightFindRewardGroupCfg * ActivityGuildFightConfig::GetFindRewardGroupCfg(int group_id) const
{
	std::map<int, std::vector<GuildFightFindRewardGroupCfg> >::const_iterator it = m_find_reward_group_map.find(group_id);
	if (it == m_find_reward_group_map.end()) return NULL;

	int rate = rand() & RAND_UNIVERSAL_RATIO_BASE_NUM;
	for (int i = 0; i < (int)it->second.size();i++)
	{
		if (rate < it->second[i].rate)
		{
			return &it->second[i];
		}
		rate -= it->second[i].rate;
	}

	return NULL;
}

Posi ActivityGuildFightConfig::GetRandBornPosi()
{
	if(m_born_count <= 0 || m_born_count > GUILD_FIGHT_BORN_POSI_MAX_NUM) return Posi(0, 0);

	int rand_index = RandomNum(0, m_born_count);

	return m_born_list[rand_index];
}


int ActivityGuildFightConfig::InitGuildFightOthersCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	if (!GetSubNodeValue(root_element, "map", m_other_cfg.scene_id) || !World::GetInstWorld()->GetSceneManager()->CheckSceneIsExist(m_other_cfg.scene_id))
	{
		return -1;
	}

	if (!GetSubNodeValue(root_element, "max_fail_num", m_other_cfg.max_fail_times) || m_other_cfg.max_fail_times <= 0)
	{
		return -2;
	}

	if (!GetSubNodeValue(root_element, "max_rounds", m_other_cfg.max_rounds_num) || m_other_cfg.max_rounds_num <= 0)
	{
		return -3;
	}

	if (!GetSubNodeValue(root_element, "win_score", m_other_cfg.wins_score))
	{
		return -4;
	}

	if (!GetSubNodeValue(root_element, "lose_score", m_other_cfg.lose_score))
	{
		return -5;
	}

	if (!GetSubNodeValue(root_element, "draw_score", m_other_cfg.draw_score))
	{
		return -6;
	}

	if (!GetSubNodeValue(root_element, "refresh_time", m_other_cfg.rank_refresh_interval) || m_other_cfg.rank_refresh_interval <= 0)
	{
		return -7;
	}

	if (!GetSubNodeValue(root_element, "family_coin", m_other_cfg.guild_fund) || m_other_cfg.guild_fund < 0)
	{
		return -8;
	}

	if (!GetSubNodeValue(root_element, "time", m_other_cfg.time) || m_other_cfg.time < 0)
	{
		return -9;
	}

	if (!GetSubNodeValue(root_element, "maxnum", m_other_cfg.max_num) || m_other_cfg.max_num <= 0)
	{
		return -10;
	}

	if (!GetSubNodeValue(root_element, "countdown", m_other_cfg.countdown) || m_other_cfg.countdown < 0)
	{
		return -11;
	}

	if (!GetSubNodeValue(root_element, "num", m_other_cfg.num) || m_other_cfg.num <= 0)
	{
		return -12;
	}

	if (!GetSubNodeValue(root_element, "gongxian", m_other_cfg.join_reward_gongxian) || m_other_cfg.join_reward_gongxian < 0)
	{
		return -13;
	}

	if (!GetSubNodeValue(root_element, "coin", m_other_cfg.join_reward_coin) || m_other_cfg.join_reward_coin < 0)
	{
		return -14;
	}

	TiXmlElement * parti_award_list = root_element->FirstChildElement("join_reward_list");
	if (NULL != parti_award_list)		// 允许参与奖为空
	{
		TiXmlElement * parti_award_node = parti_award_list->FirstChildElement("join_reward");
		if (NULL == parti_award_node) return -100;

		GuildFightAwardCon & read_con = m_other_cfg.pati_award;
		while (NULL != parti_award_node)
		{
			if (read_con.award_num >= GUILD_FIGHT_AWARD_MAX_COUNT) return -150;			// 参与奖数量超过上限
			if (!read_con.award_list[read_con.award_num++].ReadConfig(parti_award_node))
			{
				return -200;
			}

			parti_award_node = parti_award_node->NextSiblingElement();
		}
	}

	return 0;
}

int ActivityGuildFightConfig::InitGuildFightRankAward(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	guild_fight_title_id = 0;
	int last_max_role_level = 0;
	int last_min_role_level = 0;
	while (NULL != root_element)
	{
		int min_role_level = 0;
		if (!GetSubNodeValue(root_element, "level_min", min_role_level) || min_role_level <= 0)
		{
			return -1001;
		}

		int max_role_level = 0;
		if (!GetSubNodeValue(root_element, "level_max", max_role_level) || max_role_level < min_role_level)
		{
			return -1001;
		}

		if (last_max_role_level != max_role_level || last_min_role_level != min_role_level)
		{
			if (min_role_level <= last_max_role_level)
			{
				return -1002;
			}

			last_min_role_level = min_role_level;
			last_max_role_level = max_role_level;
			m_rank_award.push_back(GuildFightRoleLevelRankAward());
		}
				
		GuildFightRoleLevelRankAward & rank_reward = m_rank_award.back();
		rank_reward.min_role_level = min_role_level;
		rank_reward.max_role_level = max_role_level;

		ParamSection rank_sec; GuildFightRankAward tmp_con; GuildFightAwardCon & tmp_gfac = tmp_con.item_reward;
		if (!GetSubNodeValue(root_element, "rank_min", rank_sec.section_min) || rank_sec.section_min <= 0)
		{
			return -1;
		}

		if (!GetSubNodeValue(root_element, "rank_max", rank_sec.section_max) || rank_sec.section_max < rank_sec.section_min)
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "gongxian", tmp_con.contribution) || tmp_con.contribution < 0)
		{
			return -3;
		}
		
		if (!GetSubNodeValue(root_element, "title", tmp_con.title) || (0 != tmp_con.title && NULL == LOGIC_CONFIG->GetTitleConfig()->GetTitleCfg(tmp_con.title)))
		{
			return -4;
		}

		if (!GetSubNodeValue(root_element, "coin", tmp_con.coin) || tmp_con.coin < 0)
		{
			return -5;
		}
		
		TiXmlElement * award_list = root_element->FirstChildElement("reward_list");
		if (NULL != award_list)
		{
			TiXmlElement * award_node = award_list->FirstChildElement("reward");
			if (NULL == award_node) return -100;

			while (NULL != award_node)
			{
				if (tmp_gfac.award_num >= GUILD_FIGHT_AWARD_MAX_COUNT) return -150;
				if (!tmp_gfac.award_list[tmp_gfac.award_num++].ReadConfig(award_node))
				{
					return -200;
				}

				award_node = award_node->NextSiblingElement();
			}
		}
		if (0 != tmp_con.title)
		{
			if (guild_fight_title_id != 0 && guild_fight_title_id != tmp_con.title)
			{
				return -333;
			}
			guild_fight_title_id = tmp_con.title;
		}

		rank_reward.rank_reward[rank_sec] = tmp_con;
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int ActivityGuildFightConfig::InitGuildFightSurvAward(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		int resi_num = 0; ParamSection section; GuildFightSurvAward tmp_con; GuildFightAwardCon & tmp_gfac = tmp_con.item_reward;
		if (!GetSubNodeValue(root_element, "live_num", resi_num) || resi_num <= 0)
		{
			return -1;
		}

		if (!GetSubNodeValue(root_element, "level_min", section.section_min) || section.section_min <= 0)
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "level_max", section.section_max) || section.section_max < section.section_min || section.section_max > MAX_ROLE_LEVEL)
		{
			return -3;
		}

		if (!GetSubNodeValue(root_element, "coin", tmp_con.coin_bind) || tmp_con.coin_bind < 0)
		{
			return -4;
		}

		if (!GetSubNodeValue(root_element, "exp", tmp_con.experience) || tmp_con.experience < 0)
		{
			return -5;
		}

		if (!GetSubNodeValue(root_element, "seq", tmp_con.seq) || tmp_con.seq < 0 || tmp_con.seq >= BIT_COUNT_OF_BYTE)
		{
			return -6;
		}

		TiXmlElement * award_list = root_element->FirstChildElement("reward_list");
		if (NULL != award_list)
		{
			TiXmlElement * award_node = award_list->FirstChildElement("reward");
			if (NULL == award_node) return -100;

			while (NULL != award_node)
			{
				if (tmp_gfac.award_num >= GUILD_FIGHT_AWARD_MAX_COUNT) return -150;
				if (!tmp_gfac.award_list[tmp_gfac.award_num++].ReadConfig(award_node))
				{
					return -200;
				}

				award_node = award_node->NextSiblingElement();
			}
		}

		m_survival_award[resi_num][section] = tmp_con;
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int ActivityGuildFightConfig::InitGuildFightChestsCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;
	chest_weight = 0;
	while (NULL != root_element)
	{
		int chest_id = 0; GuildFightChestCfg tmp_gfcc;
		if (!GetSubNodeValue(root_element, "box_id", chest_id) || chest_id < 0)
		{
			return -1;
		}
		tmp_gfcc.box_id = chest_id;
		if (!GetSubNodeValue(root_element, "rate", tmp_gfcc.rate) || tmp_gfcc.rate <= 0)
		{
			return -2;
		}

		{
			TiXmlElement * award_list = root_element->FirstChildElement("reward_group_id");
			if (NULL == award_list) return -100;

			TiXmlElement * award_node = award_list->FirstChildElement("node");
			while (NULL != award_node)
			{
				if (tmp_gfcc.award_id_num >= GUILD_FIGHT_CHEST_MAX_AWARD) return -150;
				if (!GetNodeValue(award_node, tmp_gfcc.award_id_list[tmp_gfcc.award_id_num]) || m_item_group_cfg.end() == m_item_group_cfg.find(tmp_gfcc.award_id_list[tmp_gfcc.award_id_num]))
				{
					return -200;
				}

				++tmp_gfcc.award_id_num;
				award_node = award_node->NextSiblingElement();
			}
		}

		{
			TiXmlElement * captian_award_list = root_element->FirstChildElement("captainreward_group_id");
			if (NULL == captian_award_list) return -100;

			TiXmlElement * captian_award_node = captian_award_list->FirstChildElement("node");
			while (NULL != captian_award_node)
			{
				if (tmp_gfcc.caotain_award_id_num >= GUILD_FIGHT_CHEST_MAX_AWARD) return -150;
				if (!GetNodeValue(captian_award_node, tmp_gfcc.caotain_award_id_list[tmp_gfcc.caotain_award_id_num]) || m_item_group_cfg.end() == m_item_group_cfg.find(tmp_gfcc.caotain_award_id_list[tmp_gfcc.caotain_award_id_num]))
				{
					return -201;
				}

				++tmp_gfcc.caotain_award_id_num;
				captian_award_node = captian_award_node->NextSiblingElement();
			}
		}

		chest_weight += tmp_gfcc.rate;
		m_chest_cfg[chest_id] = tmp_gfcc;
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int ActivityGuildFightConfig::InitGuildFightItemsRand(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		int award_group_id = 0; GuildFightItemRand tmp_gfir;
		if (!GetSubNodeValue(root_element, "reward_id", award_group_id) || award_group_id < 0)
		{
			return -1;
		}

		if (!GetSubNodeValue(root_element, "rate", tmp_gfir.prob) || tmp_gfir.prob <= 0 || tmp_gfir.prob > RAND_UNIVERSAL_RATIO_BASE_NUM)
		{
			return -2;
		}

		TiXmlElement * item_node = root_element->FirstChildElement("reward_item");
		if (NULL == item_node) return -100;

		if (!tmp_gfir.award_item.ReadConfig(item_node))
		{
			return -3;
		}

		std::map<int, std::vector<GuildFightItemRand> >::iterator f_it = m_item_group_cfg.find(award_group_id);
		if (m_item_group_cfg.end() != f_it)		// 如果找到
		{
			f_it->second.push_back(tmp_gfir);
		}
		else
		{
			std::vector<GuildFightItemRand> tmp_vec_list; tmp_vec_list.push_back(tmp_gfir);
			m_item_group_cfg[award_group_id] = tmp_vec_list;
		}

		root_element = root_element->NextSiblingElement();
	}

	for (std::map<int, std::vector<GuildFightItemRand> >::iterator loop = m_item_group_cfg.begin(); loop != m_item_group_cfg.end(); ++loop)
	{
		int prob_total = 0;
		for (std::vector<GuildFightItemRand>::iterator vec_it = loop->second.begin(); vec_it != loop->second.end(); ++vec_it)
		{
			prob_total += vec_it->prob;
		}

		if (RAND_UNIVERSAL_RATIO_BASE_NUM != prob_total) return -500;
	}

	return 0;
}

int ActivityGuildFightConfig::InitGuildFightBornPosiCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	m_born_count = 0;
	while (NULL != root_element)
	{
		Posi node;
		if (!GetSubNodeValue(root_element, "x", node.x))
		{
			return -1;
		}

		if (!GetSubNodeValue(root_element, "y", node.y))
		{
			return -2;
		}
		if(m_born_count >= GUILD_FIGHT_BORN_POSI_MAX_NUM) return -111;

		m_born_list[m_born_count++] = node;
		root_element = root_element->NextSiblingElement();
	}
	if(m_born_count <= 0) return -222;

	return 0;
}

int ActivityGuildFightConfig::InitGuildFightJoinAwardCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		GuildFightJoinAwardCfg node_cfg;
		if (!GetSubNodeValue(root_element, "level_min", node_cfg.min_level) || node_cfg.min_level < 0 || node_cfg.min_level > MAX_ROLE_LEVEL)
		{
			return -1;
		}
		if (!GetSubNodeValue(root_element, "level_max", node_cfg.max_level) || node_cfg.max_level < node_cfg.min_level || node_cfg.max_level > MAX_ROLE_LEVEL)
		{
			return -2;
		}
		if (!GetSubNodeValue(root_element, "exp", node_cfg.exp) || node_cfg.exp < 0)
		{
			return -3;
		}
		if (!GetSubNodeValue(root_element, "coin", node_cfg.coin) || node_cfg.coin < 0)
		{
			return -4;
		}

		TiXmlElement * award_list = root_element->FirstChildElement("reward_list");
		if (NULL != award_list)
		{
			TiXmlElement * award_node = award_list->FirstChildElement("reward");
			if (NULL == award_node) return -100;

			while (NULL != award_node)
			{
				ItemConfigData item;
				if (!item.ReadConfig(award_node))
				{
					return -200;
				}
				node_cfg.reward_list.push_back(item);
				award_node = award_node->NextSiblingElement();
			}
		}	
		m_join_award_vec.push_back(node_cfg);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int ActivityGuildFightConfig::InitGuildFightFindAwardGroupCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		int group_id = 0;
		if (!GetSubNodeValue(root_element, "group_id", group_id) || group_id < 0)
		{
			return -1;
		}
		GuildFightFindRewardGroupCfg node_cfg;
		if (!GetSubNodeValue(root_element, "rate", node_cfg.rate) || node_cfg.rate < 0)
		{
			return -2;
		}
		TiXmlElement * reward_node = root_element->FirstChildElement("reward");
		if (NULL == reward_node) return -222;

		if (!node_cfg.reward.ReadConfig(reward_node))
		{
			return -3;
		}

		m_find_reward_group_map[group_id].push_back(node_cfg);
		root_element = root_element->NextSiblingElement();
	}
	return 0;
}

int ActivityGuildFightConfig::InitGuildFightFindAwardCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		int find_type = 0;
		if (!GetSubNodeValue(root_element, "find_type", find_type) || find_type < 0 || find_type >= RB_TYPE_MAX)
		{
			return -1;
		}
		GuildFightFindGroupCfg & group_cfg = m_find_cfg[find_type];
		GuildFightFindCfg node_cfg;
		if (!GetSubNodeValue(root_element, "min_level", node_cfg.min_level) || node_cfg.min_level < 0 || node_cfg.min_level > MAX_ROLE_LEVEL)
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "max_level", node_cfg.max_level) || node_cfg.max_level < node_cfg.min_level || node_cfg.max_level > MAX_ROLE_LEVEL)
		{
			return -3;
		}

		if (!GetSubNodeValue(root_element, "exp", node_cfg.exp) || node_cfg.exp < 0)
		{
			return -4;
		}

		if (!GetSubNodeValue(root_element, "coin", node_cfg.coin) || node_cfg.coin < 0)
		{
			return -5;
		}

		if (!GetSubNodeValue(root_element, "family_gongxian", node_cfg.family_gongxian) || node_cfg.family_gongxian < 0)
		{
			return -6;
		}

		if (!GetSubNodeValue(root_element, "group_id", node_cfg.group_id) || (node_cfg.group_id != -1 && m_find_reward_group_map.end() == m_find_reward_group_map.find(node_cfg.group_id)))
		{
			return -6;
		}

		group_cfg.node_list.push_back(node_cfg);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}
