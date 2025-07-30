#include "randactivitymonpolygiftconfig.hpp"
#include "servercommon/activitydef.hpp"
#include "global/randactivity/randactivityadapter.hpp"

RandActivityMonpolyGiftConfig::RandActivityMonpolyGiftConfig()
{
}

RandActivityMonpolyGiftConfig::~RandActivityMonpolyGiftConfig()
{
	std::vector<RAMonpolyGiftMapSectionCfg>().swap(m_play_section_cfg);
}

bool RandActivityMonpolyGiftConfig::Init(TiXmlElement *RootElement, std::string *err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("activity_configure", InitMonpolyGiftCfg);
	LOAD_RA_CONFIG("gift_configure", InitMonpolyGiftRewardCfg);
	LOAD_RA_CONFIG("num_exchange", InitMonpolyGiftExchangeCfg);
	LOAD_RA_CONFIG("other", InitMonpolyGiftOhterCfg);

	return true;
}

const RAMonpolyGiftMapCfg * RandActivityMonpolyGiftConfig::GetMapInfoCfg(RandActivityManager * ramgr, int map_id, int map_index) const
{
	if (map_id - 1 < 0 || map_id - 1 >= MAX_MAP_COUNT)return NULL;
	if (map_index < 0 || map_index >= MAX_ONE_MAP_INDEX)return NULL;
	
	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_MONPOLY_GIFT);
	for (std::vector<RAMonpolyGiftMapSectionCfg>::const_iterator it = m_play_section_cfg.begin(); m_play_section_cfg.end() != it; ++it)
	{
		if (it->section_end >= act_real_open_day && act_real_open_day >= it->section_start) 
		{
			return &it->play_map[map_id - 1][map_index];;
		}

	}
	return NULL;
}

const RAMonpolyGiftRewardCfg * RandActivityMonpolyGiftConfig::GetRewardCfg(int reward_group_id) const
{
	std::map<int, RAMonpolyGiftRewardCfg>::const_iterator it = m_index_map.find(reward_group_id);
	if (it != m_index_map.end())
	{
		return &it->second;
	}
	return NULL;
}

const int RandActivityMonpolyGiftConfig::GetRandPoint() const
{
	if (m_other_cfg.rate_count > 0)
	{
		int rand_num = rand() % m_other_cfg.rate_count;
		if (rand_num < m_other_cfg.rate1)
		{
			return m_other_cfg.points1;
		}
		rand_num -= m_other_cfg.rate1;
		if (rand_num < m_other_cfg.rate2)
		{
			return m_other_cfg.points2;
		}
		rand_num -= m_other_cfg.rate2;
		if (rand_num < m_other_cfg.rate3)
		{
			return m_other_cfg.points3;
		}
	}
	return 0;
}

const RAMonpolyGiftExchangeCfg * RandActivityMonpolyGiftConfig::GetExchangeCfg(int item_seq) const
{
	std::map<int, RAMonpolyGiftExchangeCfg>::const_iterator it = m_exchange_map.find(item_seq);
	if (it != m_exchange_map.end())
	{
		return &it->second;
	}
	return NULL;
}

int RandActivityMonpolyGiftConfig::InitMonpolyGiftCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}
	
	int last_section = 0;
	while (NULL != data_element)
	{
		int type = 0;
		if (!GetSubNodeValue(data_element, "type", type) || type < 0) return -1;
		int seq = 0;
		if (!GetSubNodeValue(data_element, "seq", seq) || seq >= MAX_ONE_MAP_INDEX || seq < 0) return -2;
		int layer_number = 0;
		if (!GetSubNodeValue(data_element, "layer_number", layer_number) || layer_number - 1 >= MAX_MAP_COUNT || layer_number - 1 < 0) return -3;

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
			m_play_section_cfg.push_back(RAMonpolyGiftMapSectionCfg(section_start, section_end));
			last_section = section_start;
		}

		RAMonpolyGiftMapCfg &node_cfg = m_play_section_cfg.rbegin()->play_map[layer_number - 1][seq];
		node_cfg.type = type;
		node_cfg.seq = seq;
		node_cfg.layer_number = layer_number;
		if (!GetSubNodeValue(data_element, "layer_number_type", node_cfg.layer_number_type) ||
			node_cfg.layer_number_type < RAMonpolyGiftMapCfg::MAP_TYPE_FIGHT ||
			node_cfg.layer_number_type >= RAMonpolyGiftMapCfg::MAP_TYPE_MAX) return -4;

		if (!GetSubNodeValue(data_element, "parameter", node_cfg.parameter)) return -5;

		if (!GetSubNodeValue(data_element, "exp", node_cfg.exp)) return -6;

		TiXmlElement * item_list_element = data_element->FirstChildElement("reward_item_list");
		if (NULL != item_list_element)
		{
			TiXmlElement * item_element = item_list_element->FirstChildElement("reward_item");
			while (NULL != item_element)
			{
				ItemConfigData node_reward_cfg;
				if (!node_reward_cfg.ReadConfig(item_element)) return -7;
				node_cfg.reward_vec.push_back(node_reward_cfg);
				item_element = item_element->NextSiblingElement();
			}
		}

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityMonpolyGiftConfig::InitMonpolyGiftRewardCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}
	weight_count = 0;
	while (NULL != data_element)
	{
		int type = 0;
		if (!GetSubNodeValue(data_element, "type", type) || type < 0) return -1;

		RAMonpolyGiftRewardCfg &node_cfg = m_index_map[type];
		node_cfg.type = type;

		if (!GetSubNodeValue(data_element, "rate", node_cfg.rate)) return -2;
		weight_count += node_cfg.rate;

		TiXmlElement * item_element = data_element->FirstChildElement("reward_item");
		if (NULL != item_element)
		{
			ItemConfigData node_reward_cfg;
			if (!node_cfg.reward.ReadConfig(item_element)) return -3;
		}

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityMonpolyGiftConfig::InitMonpolyGiftExchangeCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}
	weight_count = 0;
	while (NULL != data_element)
	{
		int item_seq = 0;
		if (!GetSubNodeValue(data_element, "item_seq", item_seq) || item_seq < 0 || item_seq >= MAX_COW_LOG_TIMES) return -1;

		RAMonpolyGiftExchangeCfg &node_cfg = m_exchange_map[item_seq];
		node_cfg.item_seq = item_seq;

		if (!GetSubNodeValue(data_element, "today_times", node_cfg.today_times)) return -2;

		TiXmlElement * item_element = data_element->FirstChildElement("exchange_item");
		if (NULL != item_element)
		{
			if (!node_cfg.exchange_item.ReadConfig(item_element)) return -3;
		}

		TiXmlElement * item_list_element = data_element->FirstChildElement("consume_item_list");
		if (NULL != item_list_element)
		{
			TiXmlElement * item_element = item_list_element->FirstChildElement("consume_item");
			while (NULL != item_element)
			{
				ItemConfigData node_reward_cfg;
				if (!node_reward_cfg.ReadConfig(item_element)) return -6;
				node_cfg.reward_vec.push_back(node_reward_cfg);
				if (static_cast<int>(node_cfg.reward_vec.size()) > MAX_COST_ITEM_COUNT)
				{
					return -7;
				}
				item_element = item_element->NextSiblingElement();
			}
		}

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityMonpolyGiftConfig::InitMonpolyGiftOhterCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}
	
	if (!GetSubNodeValue(data_element, "points1", m_other_cfg.points1) || m_other_cfg.points1 < 0) return -1;
	if (!GetSubNodeValue(data_element, "rate1", m_other_cfg.rate1)) return -2;
	if (!GetSubNodeValue(data_element, "points2", m_other_cfg.points2) || m_other_cfg.points2 < 0) return -3;
	if (!GetSubNodeValue(data_element, "rate2", m_other_cfg.rate2)) return -4;
	if (!GetSubNodeValue(data_element, "points3", m_other_cfg.points3) || m_other_cfg.points3 < 0) return -5;
	if (!GetSubNodeValue(data_element, "rate3", m_other_cfg.rate3)) return -6;
	if (!GetSubNodeValue(data_element, "rate_count", m_other_cfg.rate_count) || m_other_cfg.rate_count != m_other_cfg.rate1 + m_other_cfg.rate2 + m_other_cfg.rate3) return -7;
	if (!GetSubNodeValue(data_element, "cost_item", m_other_cfg.cost_item)) return -8;

	return 0;
}