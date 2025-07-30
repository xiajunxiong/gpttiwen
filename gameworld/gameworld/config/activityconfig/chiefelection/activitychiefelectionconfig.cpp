#include "activitychiefelectionconfig.hpp"

ActivityChiefElectionConfig::ActivityChiefElectionConfig()
{
	m_birth_pos_index = 0;
}

ActivityChiefElectionConfig::~ActivityChiefElectionConfig()
{
	std::vector<ChiefElectionReadySceneCfg>().swap(m_ready_scene_cfg);
	for (std::map<int, std::vector<ActivityChiefElectionBattleRewardCfg> >::iterator it = m_battle_reward_cfg.begin(); it != m_battle_reward_cfg.end(); it++)
	{
		std::vector<ActivityChiefElectionBattleRewardCfg>().swap(it->second);
	}
	std::vector<ActivityChiefElectionSeaRewardCfg>().swap(m_sea_election_reward_cfg);
	std::vector<Posi >().swap(m_birth_pos);
}

bool ActivityChiefElectionConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG("waiting_room", InitSceneCfg);
	LOAD_CONFIG("title", InitRewardCfg);
	LOAD_CONFIG("other", InitOtherCfg);
	LOAD_CONFIG("battle_reward", InitBattleRewardCfg);
	LOAD_CONFIG("participation_reward", InitSeaElectionRewardCfg);
	LOAD_CONFIG("random_entrance", InitSeaElectionPosiCfg);

	return true;
}

int ActivityChiefElectionConfig::GetTitleId(int prof_base)
{
	std::map<int, ChiefElectionRewardCfg>::iterator iter = m_reward_cfg.find(prof_base);
	if (iter != m_reward_cfg.end())
	{
		return iter->second.title_id;
	}

	return 0;
}

const ChiefElectionRewardCfg * ActivityChiefElectionConfig::GetRewardCfg(int prof_base)
{
	std::map<int, ChiefElectionRewardCfg>::iterator iter = m_reward_cfg.find(prof_base);
	if (iter != m_reward_cfg.end())
	{
		return &iter->second;
	}

	return NULL;
}

int ActivityChiefElectionConfig::GetReadySceneId(int round)
{
	int index = round - 1;

	if (index < 0 || index >= (int)m_ready_scene_cfg.size())
	{
		return 0;
	}

	return m_ready_scene_cfg[index].scene_id;
}

const ChiefElectionOtherCfg & ActivityChiefElectionConfig::GetOhterCfg()
{
	return m_other_cfg;
}

const ActivityChiefElectionBattleRewardCfg * ActivityChiefElectionConfig::GetPerBattleRewardCfg(int role_level, int match_round)
{
	std::map<int, std::vector<ActivityChiefElectionBattleRewardCfg> >::iterator iter = m_battle_reward_cfg.find(match_round);
	if (iter == m_battle_reward_cfg.end())
	{
		return NULL;
	}


	for (int i = 0; i < (int)iter->second.size(); ++i)
	{
		ActivityChiefElectionBattleRewardCfg & cfg = iter->second[i];
		if (cfg.min_role_level <= role_level && role_level <= cfg.max_role_level)
		{
			return &cfg;
		}
	}

	return NULL;
}

const ActivityChiefElectionSeaRewardCfg * ActivityChiefElectionConfig::GetSeaRewardCfg(int role_level)
{
	for (int i = 0; i < (int)m_sea_election_reward_cfg.size(); ++i)
	{
		ActivityChiefElectionSeaRewardCfg & cfg = m_sea_election_reward_cfg[i];
		if (cfg.min_role_level <= role_level && role_level <= cfg.max_role_level)
		{
			return &cfg;
		}
	}

	return NULL;
}

bool ActivityChiefElectionConfig::IsElectionChiefScene(int scene_id)
{
	for (int i = 0; i < (int)m_ready_scene_cfg.size(); ++i)
	{
		if (m_ready_scene_cfg[i].scene_id == scene_id)
		{
			return true;
		}
	}

	return false;
}

Posi ActivityChiefElectionConfig::GetSeaElectionPos()
{
	int index = m_birth_pos_index++ % m_birth_pos.size();

	return m_birth_pos[index];
}

int ActivityChiefElectionConfig::InitSceneCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	while (NULL != root_element)
	{
		ChiefElectionReadySceneCfg cfg;
		if (!GetSubNodeValue(root_element, "scene_id", cfg.scene_id))
		{
			return -1;
		}

		m_ready_scene_cfg.push_back(cfg);
		root_element = root_element->NextSiblingElement();
	}

	if (m_ready_scene_cfg.size() == 0)
	{
		return -889;
	}

	return 0;
}

int ActivityChiefElectionConfig::InitRewardCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	while (NULL != root_element)
	{
		int base_prof = 0;
		if (!GetSubNodeValue(root_element, "type", base_prof))
		{
			return -1;
		}

		ChiefElectionRewardCfg & cfg = m_reward_cfg[base_prof];
		if (!GetSubNodeValue(root_element, "title_id", cfg.title_id))
		{
			return -2;
		}

		TiXmlElement * reward_elements = root_element->FirstChildElement("reward_list");
		if (NULL != reward_elements)
		{
			TiXmlElement * reward_item_element = reward_elements->FirstChildElement("reward");
			while (reward_item_element)
			{
				ItemConfigData reward;
				if (!reward.ReadConfig(reward_item_element))
				{
					return -200;
				}

				cfg.rewards.push_back(reward);
				reward_item_element = reward_item_element->NextSiblingElement();
			}
		}

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int ActivityChiefElectionConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	if (!GetSubNodeValue(root_element, "max_round", m_other_cfg.max_round) || m_other_cfg.max_round <= 0)
	{
		return -__LINE__;
	}

	if (!GetSubNodeValue(root_element, "countdown", m_other_cfg.first_ready_time) || m_other_cfg.first_ready_time <= 0)
	{
		return -__LINE__;
	}

	std::set<int> check_repeat_scene_id;
	for (int i = 1; i < ARRAY_LENGTH(m_other_cfg.sea_election_scene_id); ++i)
	{
		std::string competition_scene = STRING_SPRINTF("competition_scene_%d", i);
		if (!GetSubNodeValue(root_element, competition_scene, m_other_cfg.sea_election_scene_id[i]))
		{
			return -__LINE__;
		}

		if (check_repeat_scene_id.find(m_other_cfg.sea_election_scene_id[i]) != check_repeat_scene_id.end())
		{
			return -__LINE__;
		}

		check_repeat_scene_id.insert(m_other_cfg.sea_election_scene_id[i]);
	}

	if (!GetSubNodeValue(root_element, "ready_time", m_other_cfg.sea_election_ready_time) || m_other_cfg.ready_time <= 0)
	{
		return -__LINE__;
	}

	if (!GetSubNodeValue(root_element, "win_score", m_other_cfg.win_score))
	{
		return -__LINE__;
	}

	if (!GetSubNodeValue(root_element, "lose_score", m_other_cfg.lose_score))
	{
		return -__LINE__;
	}

	if (!GetSubNodeValue(root_element, "draw_score", m_other_cfg.draw_score))
	{
		return -__LINE__;
	}

	if (!GetSubNodeValue(root_element, "max_fail_num", m_other_cfg.lose_times_kickout))
	{
		return -__LINE__;
	}

	if (!GetSubNodeValue(root_element, "field_buff_id", m_other_cfg.field_buff_id))
	{
		return -__LINE__;
	}

	if (!GetSubNodeValue(root_element, "competition_time", m_other_cfg.sea_election_fight_time))
	{
		return -__LINE__;
	}

	return 0;
}

int ActivityChiefElectionConfig::InitBattleRewardCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}


	while (NULL != root_element)
	{
		int match_round = 0;
		if (!GetSubNodeValue(root_element, "reward_type", match_round))
		{
			return -__LINE__;
		}

		std::vector<ActivityChiefElectionBattleRewardCfg> & round_reward_cfg = m_battle_reward_cfg[match_round]; //Ã¿Ò»ÂÖÕ½¶·½±Àø
		int last_max_role_level = 0;

		if (!round_reward_cfg.empty())
		{
			last_max_role_level = round_reward_cfg.back().max_role_level;
		}

		ActivityChiefElectionBattleRewardCfg cfg;
		if (!GetSubNodeValue(root_element, "min_level", cfg.min_role_level) || cfg.min_role_level <= last_max_role_level)
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(root_element, "max_level", cfg.max_role_level) || cfg.max_role_level < cfg.min_role_level)
		{
			return -__LINE__;
		}

		last_max_role_level = cfg.max_role_level;

		if (!GetSubNodeValue(root_element, "winner_coin", cfg.winner_coin) || cfg.winner_coin < 0)
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(root_element, "winner_exp", cfg.winner_exp) || cfg.winner_exp < 0)
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(root_element, "loser_coin", cfg.loser_coin) || cfg.loser_coin < 0)
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(root_element, "loser_exp", cfg.loser_exp) || cfg.loser_exp < 0)
		{
			return -__LINE__;
		}

		TiXmlElement * item_list_element = root_element->FirstChildElement("loser_reward_list");
		if (NULL != item_list_element)
		{
			TiXmlElement * item_element = item_list_element->FirstChildElement("loser_reward");
			while (NULL != item_element)
			{
				ItemConfigData item;
				if (!item.ReadConfig(item_element))
				{
					return -__LINE__;
				}

				cfg.loser_rewards.push_back(item);
				item_element = item_element->NextSiblingElement();
			}
		}

		round_reward_cfg.push_back(cfg);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int ActivityChiefElectionConfig::InitSeaElectionRewardCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	int last_max_role_level = 0;
	while (NULL != root_element)
	{
		ActivityChiefElectionSeaRewardCfg cfg;
		if (!GetSubNodeValue(root_element, "level_min", cfg.min_role_level) || cfg.min_role_level <= last_max_role_level)
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(root_element, "level_max", cfg.max_role_level) || cfg.max_role_level < cfg.min_role_level)
		{
			return -__LINE__;
		}

		last_max_role_level = cfg.max_role_level;

		if (!GetSubNodeValue(root_element, "exp", cfg.exp))
		{
			return -__LINE__;
		}

		TiXmlElement * reward_list_element = root_element->FirstChildElement("join_reward_list");
		if (reward_list_element != NULL)
		{
			TiXmlElement * reward_element = reward_list_element->FirstChildElement("join_reward");

			while (reward_element)
			{
				ItemConfigData reward;

				if (!reward.ReadConfig(reward_element))
				{
					return -104;
				}

				cfg.rewards.push_back(reward);
				reward_element = reward_element->NextSiblingElement();
			}
		}

		m_sea_election_reward_cfg.push_back(cfg);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int ActivityChiefElectionConfig::InitSeaElectionPosiCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	while (NULL != root_element)
	{
		Posi pos;
		if (!GetSubNodeValue(root_element, "x", pos.x) || pos.x < 0)
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(root_element, "y", pos.y) || pos.y < 0)
		{
			return -__LINE__;
		}

		m_birth_pos.push_back(pos);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}
