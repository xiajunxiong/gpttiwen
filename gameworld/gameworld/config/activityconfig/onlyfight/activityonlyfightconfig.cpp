#include "activityonlyfightconfig.hpp"
#include "world.h"
#include "checkresourcecenter.hpp"
#include "config/logicconfigmanager.hpp"
#include "other/title/titleconfig.hpp"

ActivityOnlyFightConfig::ActivityOnlyFightConfig(): chest_weight(0), m_born_count(0)
{
}

ActivityOnlyFightConfig::~ActivityOnlyFightConfig()
{
	std::vector<OnlyFightRoleLevelRankAward>().swap(m_rank_award);
	for (std::map<int, std::vector<OnlyFightBoxRewardCfg> >::iterator it = m_item_group_cfg.begin(); it != m_item_group_cfg.end(); it++)
	{
		std::vector<OnlyFightBoxRewardCfg>().swap(it->second);
	}
	std::vector<OnlyFightSceneCfg>().swap(m_scene_cfg);
	std::vector<int>().swap(m_rank_reward_title_list_cfg);
	for (std::map<int, std::vector<OnlyFightFindRewardGroupCfg> >::iterator it = m_find_reward_group_map.begin(); it != m_find_reward_group_map.end(); it++)
	{
		std::vector<OnlyFightFindRewardGroupCfg>().swap(it->second);
	}
}

bool ActivityOnlyFightConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG

	LOAD_CONFIG("other", InitOnlyFightOthersCfg);							// ∆‰À¸≈‰÷√
	LOAD_CONFIG("participation_reward", InitOnlyFightParticicpationCfg);	// ≤Œ”ÎΩ±¿¯
	LOAD_CONFIG("rank_reward", InitOnlyFightRankAward);						// ≈≈√˚Ω±¿¯
	LOAD_CONFIG("live_reward", InitOnlyFightSurvAward);						// ¥ÊªÓΩ±¿¯
	LOAD_CONFIG("box_reward", InitOnlyFightBoxRewardCfg);					// Ω±¿¯◊È≈‰÷√
	LOAD_CONFIG("box", InitOnlyFightBoxCfg);								// ±¶œ‰≈‰÷√
	LOAD_CONFIG("match_level", InitOnlyFightScecnCfg);						// ≥°æ∞≈‰÷√ 
	LOAD_CONFIG("random_entrance", InitOnlyFightBornPosiCfg);				// ≥ˆ…˙≈‰÷√
	LOAD_CONFIG("reward_find_group", InitOnlyFightFindAwardGroupCfg);		// ’“ªÿΩ±¿¯◊È
	LOAD_CONFIG("reward_find", InitOnlyFightFindAwardCfg);					// ’“ªÿΩ±¿¯
	
	return true;
}

void ActivityOnlyFightConfig::GetChestIDList(std::vector<int>& out_list)
{
	out_list.clear();
	for (std::map<int, OnlyFightBoxCfg>::iterator loop = m_chest_cfg.begin(); loop != m_chest_cfg.end(); ++loop)
	{
		out_list.push_back(loop->first);
	}
}

void ActivityOnlyFightConfig::GetSurvivalCheckNumList(std::set<int>& out_list)
{
	out_list.clear();
	for (std::map<int, std::map<ParamSection, OnlyFightSurvAward> >::iterator loop = m_survival_award.begin(); loop != m_survival_award.end(); ++loop)
	{
		out_list.insert(loop->first);
	}
}

const OnlyFightParticipationAward * ActivityOnlyFightConfig::GetOnlyFightParCfg(int level)
{
	if(level <= 0 || level > MAX_ROLE_LEVEL) return NULL;

	for (std::map<ParamSection, OnlyFightParticipationAward>::iterator it = m_particicpation_map.begin(); it != m_particicpation_map.end(); it++)
	{
		if (it->first.section_min <= level && it->first.section_max >= level)
		{
			return &it->second;
		}
	}

	return NULL;
}

const OnlyFightRankAward * ActivityOnlyFightConfig::GetOnlyFightRankCfg(int rank, int role_level)
{
	for (int i = 0; i < (int)m_rank_award.size(); ++i)
	{
		const OnlyFightRoleLevelRankAward & curr = m_rank_award[i];
		if (curr.min_role_level <= role_level && role_level <= curr.max_role_level)
		{
			for (std::map<ParamSection, OnlyFightRankAward>::const_iterator it = curr.rank_reward.begin(); it != curr.rank_reward.end(); it++)
			{
				if (it->first.section_min <= rank && it->first.section_max >= rank)
				{
					return &it->second;
				}
			}
		}
	}

	return NULL;
}

const OnlyFightSurvAward * ActivityOnlyFightConfig::GetOnlyFightSurvAward(int level, int check_num)
{
	std::map<int, std::map<ParamSection, OnlyFightSurvAward> >::iterator find_it = m_survival_award.find(check_num);
	if (m_survival_award.end() == find_it) return NULL;

	for (std::map<ParamSection, OnlyFightSurvAward>::iterator level_it = find_it->second.begin(); level_it != find_it->second.end(); ++level_it)
	{
		if (level >= level_it->first.section_min && level <= level_it->first.section_max) return &level_it->second;
	}

	return NULL;
}

const OnlyFightBoxCfg * ActivityOnlyFightConfig::GetOnlyFightBoxCfg(int box_id)
{
	std::map<int, OnlyFightBoxCfg>::iterator it = m_chest_cfg.find(box_id);
	if(it != m_chest_cfg.end()) return &it->second;

	return NULL;
}

bool ActivityOnlyFightConfig::RandAwardListByBoxID(int chest_id, std::vector<ItemConfigData>& out_award_list)
{
	out_award_list.clear();
	std::map<int, OnlyFightBoxCfg>::iterator find_it = m_chest_cfg.find(chest_id);
	if (m_chest_cfg.end() == find_it) return false;

	int award_num = find_it->second.reward_id_num;
	for (int award_idx = 0; award_idx < award_num && award_idx < ONLY_FIGHT_BOX_MAX_AWARD; ++award_idx)
	{
		int award_group_id = find_it->second.reward_id_list[award_idx];
		std::map<int, std::vector<OnlyFightBoxRewardCfg> >::iterator group_it = m_item_group_cfg.find(award_group_id);
		if (m_item_group_cfg.end() == group_it) continue;

		int prob_rate = rand() % RAND_UNIVERSAL_RATIO_BASE_NUM;
		for (std::vector<OnlyFightBoxRewardCfg>::iterator award_it = group_it->second.begin(); award_it != group_it->second.end(); ++award_it)
		{
			if (prob_rate < award_it->rate)
			{
				out_award_list.push_back(award_it->reward_item);
				break;
			}

			prob_rate -= award_it->rate;
		}
	}

	return true;
}

const OnlyFightBoxCfg * ActivityOnlyFightConfig::GetOnlyFightBoxCfg()
{
	int rand_num = rand() % chest_weight;
	for (std::map<int, OnlyFightBoxCfg>::iterator it = m_chest_cfg.begin(); it != m_chest_cfg.end(); it++)
	{
		if (rand_num < it->second.rate)
		{
			return &it->second;
		}
		rand_num -= it->second.rate;
	}
	
	return NULL;
}

const OnlyFightSceneCfg * ActivityOnlyFightConfig::GetOnlyFightSceneCfgByLevel(int level)
{
	if(level <= 0 || level > MAX_ROLE_LEVEL) return NULL;

	for (int i = 0; i < (int)m_scene_cfg.size(); i++)
	{
		if (m_scene_cfg[i].min_level <= level && m_scene_cfg[i].max_level >= level)
		{
			return &m_scene_cfg[i];
		}
	}
	
	return NULL;
}

Posi ActivityOnlyFightConfig::GetRandBornPosi()
{
	if (m_born_count <= 0 || m_born_count > ONLY_FIGHT_BORN_POSI_MAX_NUM) return Posi(0, 0);

	int rand_index = RandomNum(0, m_born_count);

	return m_born_list[rand_index];
}

const OnlyFightFindCfg * ActivityOnlyFightConfig::GetFindCfg(int find_type, int role_level) const
{
	if (find_type < 0 || find_type >= RB_TYPE_MAX) return NULL;

	const OnlyFightFindGroupCfg & node_cfg = m_find_cfg[find_type];
	for (int i = 0; i < (int)node_cfg.node_list.size(); i++)
	{
		if (node_cfg.node_list[i].min_level <= role_level && node_cfg.node_list[i].max_level >= role_level)
		{
			return &node_cfg.node_list[i];
		}
	}
	return NULL;
}

const OnlyFightFindRewardGroupCfg * ActivityOnlyFightConfig::GetFindRewardGroupCfg(int group_id) const
{
	std::map<int, std::vector<OnlyFightFindRewardGroupCfg> >::const_iterator it = m_find_reward_group_map.find(group_id);
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

int ActivityOnlyFightConfig::InitOnlyFightOthersCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	if (!GetSubNodeValue(root_element, "max_fail_num", m_other_cfg.max_fail_times) || m_other_cfg.max_fail_times <= 0)
	{
		return -1;
	}

	if (!GetSubNodeValue(root_element, "max_rounds", m_other_cfg.max_rounds_num) || m_other_cfg.max_rounds_num <= 0)
	{
		return -2;
	}

	if (!GetSubNodeValue(root_element, "win_score", m_other_cfg.wins_score))
	{
		return -3;
	}

	if (!GetSubNodeValue(root_element, "lose_score", m_other_cfg.lose_score))
	{
		return -4;
	}

	if (!GetSubNodeValue(root_element, "draw_score", m_other_cfg.draw_score))
	{
		return -5;
	}

	if (!GetSubNodeValue(root_element, "refresh_time", m_other_cfg.rank_refresh_interval) || m_other_cfg.rank_refresh_interval <= 0)
	{
		return -6;
	}

	if (!GetSubNodeValue(root_element, "time", m_other_cfg.time) || m_other_cfg.time < 0)
	{
		return -7;
	}

	if (!GetSubNodeValue(root_element, "maxnum", m_other_cfg.max_num) || m_other_cfg.max_num <= 0)
	{
		return -8;
	}

	if (!GetSubNodeValue(root_element, "num", m_other_cfg.num) || m_other_cfg.num <= 0)
	{
		return -9;
	}

	if (!GetSubNodeValue(root_element, "countdown", m_other_cfg.countdown) || m_other_cfg.countdown < 0)
	{
		return -10;
	}

	return 0;
}

int ActivityOnlyFightConfig::InitOnlyFightParticicpationCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		ParamSection section; 
		OnlyFightParticipationAward tmp_con;

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


		TiXmlElement * award_list = root_element->FirstChildElement("join_reward_list");
		if (NULL != award_list)
		{
			TiXmlElement * award_node = award_list->FirstChildElement("join_reward");
			if (NULL == award_node) return -100;

			ItemConfigData item;
			while (NULL != award_node)
			{
				if ((int)tmp_con.item_reward.size() >= ONLY_FIGHT_AWARD_MAX_COUNT) return -150;
				if (!item.ReadConfig(award_node))
				{
					return -200;
				}
				tmp_con.item_reward.push_back(item);
				award_node = award_node->NextSiblingElement();
			}
		}
		m_particicpation_map[section] = tmp_con;
		root_element = root_element->NextSiblingElement();
	}
	return 0;
}

int ActivityOnlyFightConfig::InitOnlyFightRankAward(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	int last_section_max = 0;
	int last_max_role_level = 0;
	int last_min_role_level = 0;
	int count = 0;
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
			last_section_max = 0;
			count += 1;
			m_rank_award.push_back(OnlyFightRoleLevelRankAward());
		}

		OnlyFightRoleLevelRankAward & rank_reward = m_rank_award.back();
		rank_reward.min_role_level = min_role_level;
		rank_reward.max_role_level = max_role_level;


		ParamSection rank_sec; 
		OnlyFightRankAward tmp_con; 
		if (!GetSubNodeValue(root_element, "rank_min", rank_sec.section_min) || rank_sec.section_min <= 0)
		{
			return -1;
		}

		if (!GetSubNodeValue(root_element, "rank_max", rank_sec.section_max) || rank_sec.section_max < rank_sec.section_min)
		{
			return -2;
		}
		if (0 != last_section_max && last_section_max + 1 != rank_sec.section_min)
		{
			return -222;
		}
		last_section_max = rank_sec.section_max;

		if (!GetSubNodeValue(root_element, "coin", tmp_con.coin) || tmp_con.coin < 0)
		{
			return -3;
		}

		if (!GetSubNodeValue(root_element, "title", tmp_con.title))
		{
			return -4;
		}
		if (0 != tmp_con.title)
		{
			CheckResourceCenter::Instance().GetTitleCheck()->Add(tmp_con.title, __FUNCTION__);
		}

		TiXmlElement * award_list = root_element->FirstChildElement("reward_list");
		if (NULL != award_list)
		{
			TiXmlElement * award_node = award_list->FirstChildElement("reward");
			if (NULL == award_node) return -100;

			ItemConfigData item;
			while (NULL != award_node)
			{
				if ((int)tmp_con.item_reward.size() >= ONLY_FIGHT_AWARD_MAX_COUNT) return -150;
				if (!item.ReadConfig(award_node))
				{
					return -200;
				}
				tmp_con.item_reward.push_back(item);
				award_node = award_node->NextSiblingElement();
			}
		}

		if (tmp_con.title > 0 && count == 1)
		{
			if((int)m_rank_reward_title_list_cfg.size() >= ONLY_FIGHT_RANK_TITLE_MAX_NUM) return -333;

			m_rank_reward_title_list_cfg.push_back(tmp_con.title);
		}
		rank_reward.rank_reward[rank_sec] = tmp_con;
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int ActivityOnlyFightConfig::InitOnlyFightSurvAward(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		int resi_num = 0; ParamSection section; OnlyFightSurvAward tmp_con;
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

			ItemConfigData item;
			while (NULL != award_node)
			{
				if ((int)tmp_con.item_reward.size() >= ONLY_FIGHT_AWARD_MAX_COUNT) return -150;
				if (!item.ReadConfig(award_node))
				{
					return -200;
				}
				tmp_con.item_reward.push_back(item);
				award_node = award_node->NextSiblingElement();
			}
		}

		m_survival_award[resi_num][section] = tmp_con;
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int ActivityOnlyFightConfig::InitOnlyFightBoxCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;
	chest_weight = 0;
	while (NULL != root_element)
	{
		int chest_id = 0; OnlyFightBoxCfg tmp_gfcc;
		if (!GetSubNodeValue(root_element, "box_id", chest_id) || chest_id < 0)
		{
			return -1;
		}
		tmp_gfcc.box_id = chest_id;
		if (!GetSubNodeValue(root_element, "rate", tmp_gfcc.rate) || tmp_gfcc.rate <= 0)
		{
			return -2;
		}

		TiXmlElement * award_list = root_element->FirstChildElement("reward_group_id");
		if (NULL == award_list) return -100;

		TiXmlElement * award_node = award_list->FirstChildElement("node");
		while (NULL != award_node)
		{
			if (tmp_gfcc.reward_id_num >= ONLY_FIGHT_BOX_MAX_AWARD) return -150;
			if (!GetNodeValue(award_node, tmp_gfcc.reward_id_list[tmp_gfcc.reward_id_num]) || m_item_group_cfg.end() == m_item_group_cfg.find(tmp_gfcc.reward_id_list[tmp_gfcc.reward_id_num]))
			{
				return -200;
			}

			++tmp_gfcc.reward_id_num;
			award_node = award_node->NextSiblingElement();
		}
		chest_weight += tmp_gfcc.rate;
		m_chest_cfg[chest_id] = tmp_gfcc;
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int ActivityOnlyFightConfig::InitOnlyFightBoxRewardCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		int award_group_id = 0; OnlyFightBoxRewardCfg tmp_gfir;
		if (!GetSubNodeValue(root_element, "reward_id", award_group_id) || award_group_id < 0)
		{
			return -1;
		}

		if (!GetSubNodeValue(root_element, "rate", tmp_gfir.rate) || tmp_gfir.rate <= 0 || tmp_gfir.rate > RAND_UNIVERSAL_RATIO_BASE_NUM)
		{
			return -2;
		}

		TiXmlElement * item_node = root_element->FirstChildElement("reward_item");
		if (NULL == item_node) return -100;

		if (!tmp_gfir.reward_item.ReadConfig(item_node))
		{
			return -3;
		}

		std::map<int, std::vector<OnlyFightBoxRewardCfg> >::iterator f_it = m_item_group_cfg.find(award_group_id);
		if (m_item_group_cfg.end() != f_it)		// »Áπ˚’“µΩ
		{
			f_it->second.push_back(tmp_gfir);
		}
		else
		{
			std::vector<OnlyFightBoxRewardCfg> tmp_vec_list; tmp_vec_list.push_back(tmp_gfir);
			m_item_group_cfg[award_group_id] = tmp_vec_list;
		}

		root_element = root_element->NextSiblingElement();
	}

	for (std::map<int, std::vector<OnlyFightBoxRewardCfg> >::iterator loop = m_item_group_cfg.begin(); loop != m_item_group_cfg.end(); ++loop)
	{
		int prob_total = 0;
		for (std::vector<OnlyFightBoxRewardCfg>::iterator vec_it = loop->second.begin(); vec_it != loop->second.end(); ++vec_it)
		{
			prob_total += vec_it->rate;
		}

		if (RAND_UNIVERSAL_RATIO_BASE_NUM != prob_total) return -500;
	}

	return 0;
}

int ActivityOnlyFightConfig::InitOnlyFightScecnCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		OnlyFightSceneCfg scene_cfg;
		if (!GetSubNodeValue(root_element, "min_level", scene_cfg.min_level) || scene_cfg.min_level <= 0) return -1;
		if (!GetSubNodeValue(root_element, "max_level", scene_cfg.max_level) || scene_cfg.max_level < 0 || 
			(scene_cfg.max_level != 0 && scene_cfg.max_level < scene_cfg.min_level)) return -2;
		if (!GetSubNodeValue(root_element, "scene_id", scene_cfg.scene_id) || !World::GetInstWorld()->GetSceneManager()->CheckSceneIsExist(scene_cfg.scene_id)) return -3;

		if (scene_cfg.max_level == 0)
		{
			scene_cfg.max_level = MAX_ROLE_LEVEL;
		}
		if ((int)m_scene_cfg.size() >= ONLY_FIGHT_SCNEE_MAX_NUM)
		{
			return -666;
		}

		m_scene_cfg.push_back(scene_cfg);

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int ActivityOnlyFightConfig::InitOnlyFightBornPosiCfg(TiXmlElement * RootElement)
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
		if (m_born_count >= ONLY_FIGHT_BORN_POSI_MAX_NUM) return -111;

		m_born_list[m_born_count++] = node;
		root_element = root_element->NextSiblingElement();
	}
	if (m_born_count <= 0) return -222;

	return 0;
}

int ActivityOnlyFightConfig::InitOnlyFightFindAwardGroupCfg(TiXmlElement * RootElement)
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
		OnlyFightFindRewardGroupCfg node_cfg;
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

int ActivityOnlyFightConfig::InitOnlyFightFindAwardCfg(TiXmlElement * RootElement)
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
		OnlyFightFindGroupCfg & group_cfg = m_find_cfg[find_type];
		OnlyFightFindCfg node_cfg;
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
