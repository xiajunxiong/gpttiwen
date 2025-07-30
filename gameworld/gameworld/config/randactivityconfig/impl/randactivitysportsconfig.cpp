#include "randactivitysportsconfig.hpp"

RandActivitySportsConfig::RandActivitySportsConfig()
{

}

RandActivitySportsConfig::~RandActivitySportsConfig()
{

}

bool RandActivitySportsConfig::Init(TiXmlElement *RootElement, std::string *err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("sports_history", InitSportsHistoryCfg);
	LOAD_RA_CONFIG("sports_settlement", InitSportsSettlementCfg);

	return true;
}

const RASportsHistoryCfg * RandActivitySportsConfig::GetSportsHistoryCfg(int seq) const
{
	std::map<int, RASportsHistoryCfg>::const_iterator it = m_sports_history_map.find(seq);
	if (it != m_sports_history_map.end())
	{
		return &it->second;
	}
	return NULL;
}

const std::map<int, RASportsHistoryCfg>* RandActivitySportsConfig::GetSportsHistoryList() const
{
	return &m_sports_history_map;
}

const RASportsSettlementCfg * RandActivitySportsConfig::GetSportsSettlementCfg(int role_rank) const
{
	for (std::map<int, RASportsSettlementCfg>::const_iterator it = m_sports_settlement_map.begin(); it != m_sports_settlement_map.end(); it++)
	{
		if (role_rank <= it->second.ranking)
		{
			return &it->second;
		}
	}
	return NULL;
}

int RandActivitySportsConfig::InitSportsHistoryCfg(TiXmlElement * RootElement)
{
	TiXmlElement *data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -999;
	}
	while (NULL != data_element)
	{
		int seq = 0;
		if (!GetSubNodeValue(data_element, "seq", seq) || seq < 0)
		{
			return -1;
		}

		RASportsHistoryCfg &node_cfg = m_sports_history_map[seq];
		node_cfg.seq = seq;

		if (!GetSubNodeValue(data_element, "historical_ranking", node_cfg.historical_ranking) || node_cfg.historical_ranking <= 0)
		{
			return -2;
		}

		TiXmlElement * item_list_node = data_element->FirstChildElement("reward_item_list");
		if (NULL != item_list_node)
		{
			TiXmlElement * item_node = item_list_node->FirstChildElement("reward_item");
			while (NULL != item_node)
			{
				ItemConfigData node_reward;
				if (!node_reward.ReadConfig(item_node)) return -3;
				node_cfg.itemlist.push_back(node_reward);
				item_node = item_node->NextSiblingElement();
			}

		}

		data_element = data_element->NextSiblingElement();
	}
	return 0;
}

int RandActivitySportsConfig::InitSportsSettlementCfg(TiXmlElement * RootElement)
{
	TiXmlElement *data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -999;
	}
	while (NULL != data_element)
	{
		int seq = 0;
		if (!GetSubNodeValue(data_element, "seq", seq) || seq < 0)
		{
			return -1;
		}

		RASportsSettlementCfg &node_cfg = m_sports_settlement_map[seq];
		node_cfg.seq = seq;

		if (!GetSubNodeValue(data_element, "ranking", node_cfg.ranking) || node_cfg.ranking < 0)
		{
			return -2;
		}

		TiXmlElement * item_list_node = data_element->FirstChildElement("reward_item_list");
		if (NULL != item_list_node)
		{
			TiXmlElement * item_node = item_list_node->FirstChildElement("reward_item");
			while (NULL != item_node)
			{
				ItemConfigData node_reward;
				if (!node_reward.ReadConfig(item_node)) return -3;
				node_cfg.itemlist.push_back(node_reward);
				item_node = item_node->NextSiblingElement();
			}

		}

		data_element = data_element->NextSiblingElement();
	}
	return 0;
}
