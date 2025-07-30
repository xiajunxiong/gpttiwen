#include "randactivityyuanqispeedconfig.hpp"
#include "global/randactivity/randactivitymanager.hpp"
#include <set>
#include "randactivityyuanqispeedconfig.hpp"

RandActivityYuanQiSpeedConfig::RandActivityYuanQiSpeedConfig()
{
}

RandActivityYuanQiSpeedConfig::~RandActivityYuanQiSpeedConfig()
{
}

bool RandActivityYuanQiSpeedConfig::Init(TiXmlElement * RootElement, std::string * err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("other", InitOtherCfg);
	LOAD_RA_CONFIG("reward", InitRewardCfg);

	return true;
}

const YuanQiSpeedReward * RandActivityYuanQiSpeedConfig::GetRewardCfg(int open_day, int seq) const
{
	for (std::map<int, YuanQiSpeedRewardCfg>::const_iterator it = m_reward_cfg.begin(); it != m_reward_cfg.end(); it++)
	{
		YuanQiKey key;
		key.key = it->first;
		if (key.section.section_start <= open_day && (key.section.section_end >= open_day || key.section.section_end == 0))
		{
			std::map<int, YuanQiSpeedReward>::const_iterator reward_it = it->second.m_reward_vec.find(seq);
			if (it->second.m_reward_vec.end() != reward_it)
			{
				return &reward_it->second;
			}
		}
	}
	return NULL;
}

int RandActivityYuanQiSpeedConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -1;
	}

	if (!GetSubNodeValue(data_element, "frequency", m_other_cfg.frequency) || m_other_cfg.frequency <= 0)
	{
		return -2;
	}

	if (!GetSubNodeValue(data_element, "time", m_other_cfg.time) || m_other_cfg.time <= 0)
	{
		return -3;
	}

	return 0;
}

int RandActivityYuanQiSpeedConfig::InitRewardCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -1;
	}

	while (data_element != NULL)
	{
		int section_start = 0;
		int section_end = 0;
		if (!GetSubNodeValue(data_element, "section_start", section_start) || section_start <= 0)
		{
			return -1;
		}

		if (!GetSubNodeValue(data_element, "section_end", section_end) || section_end < 0)
		{
			return -2;
		}
		if (!GetSubNodeValue(data_element, "section_end", section_end) || section_end < 0)
		{
			return -2;
		}
		YuanQiKey key;
		key.section.section_start = section_start;
		key.section.section_end = section_end;

		std::map<int, YuanQiSpeedRewardCfg>::iterator it = m_reward_cfg.find(key.key);
		if (it == m_reward_cfg.end())
		{
			YuanQiSpeedRewardCfg &node_list = m_reward_cfg[key.key];
		}
		YuanQiSpeedRewardCfg &node_list = m_reward_cfg[key.key];

		YuanQiSpeedReward node;

		TiXmlElement * reward_item_list_element = data_element->FirstChildElement("reward1_item_list");
		if (reward_item_list_element != NULL)
		{
			TiXmlElement * reward_item_element = reward_item_list_element->FirstChildElement("reward1_item");
			while (NULL != reward_item_element)
			{
				ItemConfigData node_reward;
				if (!node_reward.ReadConfig(reward_item_element))
				{
					return -3;
				}
				node.m_pass_reward.push_back(node_reward);
				reward_item_element = reward_item_element->NextSiblingElement();
			}
		}

		TiXmlElement * reward_item2_list_element = data_element->FirstChildElement("reward2_item_list");
		if (reward_item2_list_element != NULL)
		{
			TiXmlElement * reward_item_element = reward_item2_list_element->FirstChildElement("reward2_item");
			while (NULL != reward_item_element)
			{
				ItemConfigData node_reward;
				if (!node_reward.ReadConfig(reward_item_element))
				{
					return -3;
				}
				node.m_fail_reward.push_back(node_reward);
				reward_item_element = reward_item_element->NextSiblingElement();
			}
		}

		TiXmlElement * reward_item_element = data_element->FirstChildElement("reward3_item");
		if (NULL != reward_item_element)
		{
			ItemConfigData node_reward;
			if (!node_reward.ReadConfig(reward_item_element))
			{
				return -3;
			}
			node.m_fisrt_reward.push_back(node_reward);
			reward_item_element = reward_item_element->NextSiblingElement();
		}

		int seq = 0;
		if (!GetSubNodeValue(data_element, "seq", seq) || seq < 0)
		{
			return -999;
		}

		node_list.m_reward_vec[seq] = node;

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}