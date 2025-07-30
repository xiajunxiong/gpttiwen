#include "randactivityheritagetreasureconfig.hpp"

#include <algorithm>

RandActivityHeritageTreasireConfig::RandActivityHeritageTreasireConfig()
{

}

RandActivityHeritageTreasireConfig::~RandActivityHeritageTreasireConfig()
{
	std::vector<RAHeritageTreasureOtherCfg>().swap(m_heritage_treasure_cfg);
	std::vector<int>().swap(m_clue_cfg);
	std::vector<RAHeritageTreasureRewardSectionCfg>().swap(m_reward_section_cfg);
}

bool RandActivityHeritageTreasireConfig::Init(TiXmlElement *RootElement, std::string *err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("other", InitOtherCfg);
	LOAD_RA_CONFIG("reward_pool", InitRewardCfg);
	LOAD_RA_CONFIG("heritage_treasure", InitClueCfg);

	return true;
}

const RAHeritageTreasureOtherCfg & RandActivityHeritageTreasireConfig::GetHeritageTreasureOtherCfg(RandActivityManager* ramgr) const
{
	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_HERITAGE_TREASURE);
	for (std::vector<RAHeritageTreasureOtherCfg>::const_iterator section_it = m_heritage_treasure_cfg.begin(); m_heritage_treasure_cfg.end() != section_it; ++section_it)
	{
		if (section_it->section_end >= act_real_open_day && act_real_open_day >= section_it->section_start)
		{
			return *section_it;
		}
	}
	return *m_heritage_treasure_cfg.rbegin();
}

const RAHeritageTreasureRewardCfg::ItemCfg * RandActivityHeritageTreasireConfig::GetRandReward(RandActivityManager* ramgr) const
{
	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_HERITAGE_TREASURE);
	for (std::vector<RAHeritageTreasureRewardSectionCfg>::const_iterator section_it = m_reward_section_cfg.begin(); m_reward_section_cfg.end() != section_it; ++section_it)
	{
		if (section_it->section_end >= act_real_open_day && act_real_open_day >= section_it->section_start)
		{
			int r = RandomNum(section_it->m_reward_cfg.count);

			for (int i = 0; i < (int)section_it->m_reward_cfg.group_reward.size(); ++i)
			{
				if (r < section_it->m_reward_cfg.group_reward[i].rate)
				{
					return &section_it->m_reward_cfg.group_reward[i];
				}

				r -= section_it->m_reward_cfg.group_reward[i].rate;
			}
			break;
		}
	}
	
	return NULL;
}

void RandActivityHeritageTreasireConfig::GetRandClueId(unsigned char ARG_OUT(&clue_id)[RA_HERITAGE_TREASURE_MAX_PIECE]) 
{
	std::random_shuffle(m_clue_cfg.begin(), m_clue_cfg.end());

	for (int i = 0; i < (int)m_clue_cfg.size() && i < ARRAY_LENGTH(clue_id); ++i)
	{
		clue_id[i] = m_clue_cfg[i];
	}
}

int RandActivityHeritageTreasireConfig::InitClueCfg(TiXmlElement *RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -__LINE__;
	}

	std::set<int> check_repeat;
	while (NULL != root_element)
	{
		int clue_id = 0;
		if (!GetSubNodeValue(root_element, "type", clue_id) || clue_id <= 0 || clue_id >= RA_HERITAGE_TREASURE_FETCH_CLUE_REWARD_MARD)
		{
			return -__LINE__;
		}

		if (check_repeat.find(clue_id) != check_repeat.end())
		{
			return -__LINE__;
		}

		check_repeat.insert(clue_id);
		m_clue_cfg.push_back(clue_id);
		root_element = root_element->NextSiblingElement();
	}

	if (m_clue_cfg.size() < RA_HERITAGE_TREASURE_MAX_PIECE)
	{
		return -__LINE__;
	}

	return 0;
}

int RandActivityHeritageTreasireConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement *data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -1000;
	}
	int last_section = 0;
	while (NULL != data_element)
	{
		int section_start = 0, section_end = 0;
		if (!GetSubNodeValue(data_element, "section_start", section_start) || section_start <= 0) return -999;
		if (!GetSubNodeValue(data_element, "section_end", section_end) || section_end < 0) return -998;
		if (0 == section_end)
		{
			section_end = MAX_INT;
		}

		if (section_end < section_start) return -997;

		if (section_start != last_section)
		{
			m_heritage_treasure_cfg.push_back(RAHeritageTreasureOtherCfg(section_start, section_end));
			last_section = section_start;
		}

		RAHeritageTreasureOtherCfg& node = *m_heritage_treasure_cfg.rbegin();

		TiXmlElement * reward_element = data_element->FirstChildElement("reward_item");
		ItemConfigData reward;
		if (!reward.ReadConfig(reward_element))
		{
			return -104;
		}
		node.final_reward.push_back(reward);
		//reward_element = reward_element->NextSiblingElement();

		if (!GetSubNodeValue(data_element, "consume_item_id", node.active_clue_item_id) || ITEMPOOL->GetItem(node.active_clue_item_id) == NULL)
		{
			return -3;
		}
		if (!GetSubNodeValue(data_element, "time", node.end_day_time) || node.end_day_time < 0) return -4;
		if (!GetSubNodeValue(data_element, "coin_num", node.coin_num) || node.coin_num <= 0) return -5;
		if (!GetSubNodeValue(data_element, "gold_num", node.gold_num) || node.gold_num <= 0) return -6;
		if (!GetSubNodeValue(data_element, "is_complement", node.is_complement)) return -7;

		data_element = data_element->NextSiblingElement();
	}
	if (0 == m_heritage_treasure_cfg.size())
	{
		return -998;
	}
	
	return 0;
}

int RandActivityHeritageTreasireConfig::InitRewardCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -999;
	}

	int last_section = 0;
	while (NULL != data_element)
	{
		int section_start = 0, section_end = 0;
		if (!GetSubNodeValue(data_element, "section_start", section_start) || section_start <= 0) return -999;
		if (!GetSubNodeValue(data_element, "section_end", section_end) || section_end < 0) return -998;
		if (0 == section_end)
		{
			section_end = MAX_INT;
		}

		if (section_end < section_start) return -997;

		if (section_start != last_section)
		{
			m_reward_section_cfg.push_back(RAHeritageTreasureRewardSectionCfg(section_start, section_end));
			last_section = section_start;
		}

		RAHeritageTreasureRewardCfg::ItemCfg item_cfg;
		if (!GetSubNodeValue(data_element, "weight", item_cfg.rate) || item_cfg.rate <= 0)
		{
			return -2;
		}

		if (!GetSubNodeValue(data_element, "is_hearsay", item_cfg.is_broadcast))
		{
			return -3;
		}

		LOAD_ITEM_VETOR_NODE("reward_item_list", "reward_item", item_cfg.reward);

		m_reward_section_cfg.rbegin()->m_reward_cfg.count += item_cfg.rate;
		m_reward_section_cfg.rbegin()->m_reward_cfg.group_reward.push_back(item_cfg);
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}
