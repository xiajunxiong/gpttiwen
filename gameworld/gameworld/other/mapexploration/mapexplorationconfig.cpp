#include "mapexplorationconfig.hpp"
#include "servercommon/servercommon.h"
#include "servercommon/struct/itemlistparam.h"

MapExplorationConfig::MapExplorationConfig()
{
}

MapExplorationConfig::~MapExplorationConfig()
{
}

bool MapExplorationConfig::Init(const std::string &configname, std::string *err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG("npc_xy", InitMapExplorationCfg);
	LOAD_CONFIG("reward", InitMapExplorationRewardCfg);

	return true;
}

const MapExplorationCfg * MapExplorationConfig::GetExplorationCfg(int seq)
{
	std::map<int, MapExplorationCfg>::iterator it = m_exploration_map.find(seq);
	if (it != m_exploration_map.end())
	{
		return &it->second;
	}
	return NULL;
}

const MapExplorationRewardCfg * MapExplorationConfig::GetExplorationRewardCfg(int reward_id)
{
	std::map<int, MapExplorationRewardCfg>::iterator it = m_reward_map.find(reward_id);
	if (it != m_reward_map.end())
	{
		return &it->second;
	}
	return NULL;
}

int MapExplorationConfig::InitMapExplorationCfg(TiXmlElement *RootElement)
{
	TiXmlElement* dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -10000;
	m_exploration_map.clear();
	while (dataElement)
	{
		int seq = 0;
		if (!GetSubNodeValue(dataElement, "seq", seq) || seq < 0)
		{
			return -1;
		}
		MapExplorationCfg &node = m_exploration_map[seq];
		
		if (!GetSubNodeValue(dataElement, "scene_id", node.scene_id))
		{
			return -2;
		}

		if (!GetSubNodeValue(dataElement, "x", node.npc_pos.x))
		{
			return -3;
		}

		if (!GetSubNodeValue(dataElement, "y", node.npc_pos.y))
		{
			return -4;
		}

		if (!GetSubNodeValue(dataElement, "reward_group_id", node.reward_group_id))
		{
			return -5;
		}

		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int MapExplorationConfig::InitMapExplorationRewardCfg(TiXmlElement * RootElement)
{
	TiXmlElement* dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -10000;
	m_reward_map.clear();
	while (dataElement)
	{
		int reward_group = 0;
		if (!GetSubNodeValue(dataElement, "reward_group", reward_group) || reward_group < 0)
		{
			return -1;
		}
		MapExplorationRewardCfg &node = m_reward_map[reward_group];
		node.reward_group = reward_group;

		TiXmlElement* reward_list_Element = dataElement->FirstChildElement("reward_item_list");
		if (reward_list_Element != NULL)
		{
			TiXmlElement* reward_Element = reward_list_Element->FirstChildElement("reward_item");
			while (reward_Element != NULL)
			{
				ItemConfigData reward_node;
				if (!reward_node.ReadConfig(reward_Element))
				{
					return -2;
				}
				node.item_list.push_back(reward_node);
				reward_Element = reward_Element->NextSiblingElement();
			}
		}

		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}
