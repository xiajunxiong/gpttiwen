#include "peaktokenconfig.hpp"

PeakTokenConfig::PeakTokenConfig()
{
}

PeakTokenConfig::~PeakTokenConfig()
{
}

bool PeakTokenConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG("reward", InitRewardGroupCfg);
	LOAD_CONFIG("other", InitOtherCfg);

	return true;
}

const PeakTokenConfig::RewardCfg * PeakTokenConfig::GetGroupRandCfg(int group_id) const
{
	GroupIDMap::const_iterator group_it = m_group_id_map.find(group_id);
	if (m_group_id_map.end() == group_it)
	{
		return NULL;
	}

	int rand_num = RandomNum(group_it->second.total_power);

	RewardPool::const_iterator rand_it
		= group_it->second.item_pool.upper_bound(rand_num);
	if (group_it->second.item_pool.begin() == rand_it)
	{
		return NULL;
	}
	rand_it--;
	return &rand_it->second;
}

int PeakTokenConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -10000;
	}

	if (!GetSubNodeValue(root_element, "activity_duration", m_other_cfg.act_duration_s)
		|| 0 >= m_other_cfg.act_duration_s)
	{
		return -1;
	}

	if (!GetSubNodeValue(root_element, "notice_interval", m_other_cfg.notice_interval_s)
		|| 0 >= m_other_cfg.notice_interval_s)
	{
		return -2;
	}

	if (!GetSubNodeValue(root_element, "game_duration", m_other_cfg.game_time_s)
		|| 0 >= m_other_cfg.game_time_s)
	{
		return -3;
	}

	if (!GetSubNodeValue(root_element, "reward_num", m_other_cfg.role_reward_times)
		|| 0 >= m_other_cfg.role_reward_times)
	{
		return -4;
	}

	if (!GetSubNodeValue(root_element, "use_frequency", m_other_cfg.item_can_use_times)
		|| 0 >= m_other_cfg.item_can_use_times)
	{
		return -4;
	}
	
	return 0;
}

int PeakTokenConfig::InitRewardGroupCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -10000;
	}
	while (NULL != root_element)
	{
		int group_id = 0;
		if (!GetSubNodeValue(root_element, "box_type", group_id)
			|| 0 >= group_id)
		{
			return -1;
		}

		GroupCfg & group_cfg = m_group_id_map[group_id];
		group_cfg.group_id = group_id;

		RewardCfg & item_cfg = group_cfg.item_pool[group_cfg.total_power];
		item_cfg.group_id = group_id;

		if (!GetSubNodeValue(root_element, "rate", item_cfg.power)
			|| 0 >= item_cfg.power)
		{
			return -2;
		}
		group_cfg.total_power += item_cfg.power;

		{
			TiXmlElement * node_element = root_element->FirstChildElement("reward_item");
			if (NULL != node_element)
			{
				if (!item_cfg.item.ReadConfig(node_element))
				{
					return -102;
				}
			}
		}

		if (!GetSubNodeValue(root_element, "is_broadcast", item_cfg.is_broadcast)
			|| 0 > item_cfg.is_broadcast)
		{
			return -3;
		}

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}
