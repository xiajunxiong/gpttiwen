#include "randactivitylaodonghaoliconfig.hpp"

RandActivityLaoDongHaoLiConfig::RandActivityLaoDongHaoLiConfig()
	:m_all_weight(0)
{

}

RandActivityLaoDongHaoLiConfig::~RandActivityLaoDongHaoLiConfig()
{
	std::vector<RALaoDongHaoLiPointCfg>().swap(m_point_cfg);
	std::vector<RALaoDongHaoLiRewardCfg>().swap(m_reward_cfg);
}

bool RandActivityLaoDongHaoLiConfig::Init(TiXmlElement *RootElement, std::string *err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("other", InitOtherCfg);
	LOAD_RA_CONFIG("case_coordinate", InitPointCfg);
	LOAD_RA_CONFIG("case_reward", InitRewardCfg);

	return true;
}

const RALaoDongHaoLiPointCfg * RandActivityLaoDongHaoLiConfig::GetPointCfg(int seq)const
{
	if (seq < 0 || seq >= (int)m_point_cfg.size())
	{
		Posi pos(-1, -1);
		return NULL;
	}
	return &m_point_cfg[seq];
}

const RALaoDongHaoLiRewardCfg * RandActivityLaoDongHaoLiConfig::GetRewardCfg()const
{
	if (0 >= m_all_weight) return NULL;

	int rand_weight = rand() % m_all_weight;
	for (std::vector<RALaoDongHaoLiRewardCfg>::const_iterator it = m_reward_cfg.begin(); it != m_reward_cfg.end(); ++it)
	{
		if (rand_weight < it->weight)
		{
			return &(*it);
		}
		rand_weight -= it->weight;
	}

	return NULL;
}

int RandActivityLaoDongHaoLiConfig::InitOtherCfg(TiXmlElement *RootElement)
{
	if (NULL == RootElement)
	{
		return -10000;
	}

	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -20000;
	}
	while (NULL != data_element)
	{
		int h_time = 0;
		int m_time = 0;
		if (!GetSubNodeValue(data_element, "reward_frequency", m_other_cfg.reward_num) || m_other_cfg.reward_num < 0)
		{
			return -1;
		}
		if (!GetSubNodeValue(data_element, "map_case_number", m_other_cfg.map_case_num) || m_other_cfg.map_case_num < 0)
		{
			return -2;
		}
		if (!GetSubNodeValue(data_element, "time_start", m_other_cfg.start_time))
		{
			return -3;
		}
		h_time = m_other_cfg.start_time / 100;
		m_time = m_other_cfg.start_time % 100;
		if (0 > h_time || 23 < h_time || 0 > m_time || 59 < m_time)return -100;

		if (!GetSubNodeValue(data_element, "time_end", m_other_cfg.end_time))
		{
			return -4;
		}
		h_time = m_other_cfg.end_time / 100;
		m_time = m_other_cfg.end_time % 100;
		if (0 > h_time || 23 < h_time || 0 > m_time || 59 < m_time)return -200;

		if (!GetSubNodeValue(data_element, "time_the", m_other_cfg.refresh_time) || m_other_cfg.refresh_time < 0)
		{
			return -5;
		}

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityLaoDongHaoLiConfig::InitPointCfg(TiXmlElement *RootElement)
{
	if (NULL == RootElement)
	{
		return -10000;
	}

	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -20000;
	}
	while (NULL != data_element)
	{
		RALaoDongHaoLiPointCfg node;
		if (!GetSubNodeValue(data_element, "scene_id", node.scene_id) || m_other_cfg.reward_num < 0)
		{
			return -1;
		}
		if (!GetSubNodeValue(data_element, "treasure_chest_id", node.npc_gather_id))
		{
			return -2;
		}

		m_point_cfg.push_back(node);
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityLaoDongHaoLiConfig::InitRewardCfg(TiXmlElement *RootElement)
{
	if (NULL == RootElement)
	{
		return -10000;
	}

	TiXmlElement *data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -20000;
	}
	while (NULL != data_element)
	{
		RALaoDongHaoLiRewardCfg node;

		TiXmlElement * item_node = data_element->FirstChildElement("reward_item");
		if (NULL != item_node)
		{
			if (!node.reward_item.ReadConfig(item_node)) return -100;
		}

		if (!GetSubNodeValue(data_element, "weight", node.weight) || node.weight < 0)
		{
			return -1;
		}
		if (!GetSubNodeValue(data_element, "hearsay", node.hearsay))
		{
			return -2;
		}

		m_all_weight += node.weight;
		m_reward_cfg.push_back(node);
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}
