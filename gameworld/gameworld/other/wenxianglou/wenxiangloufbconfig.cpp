#include "wenxiangloufbconfig.hpp"
#include "world.h"
#include "gameworld/task/taskpool.h"
#include "item/itempool.h"

WenXiangLouFbConfig::WenXiangLouFbConfig()
{
}

WenXiangLouFbConfig::~WenXiangLouFbConfig()
{
}

bool WenXiangLouFbConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG
	
	LOAD_CONFIG("role_clue", InitRoleClueCfg);
	LOAD_CONFIG("item_clue", InitItemClueCfg);
	LOAD_CONFIG("event_cfg", InitEventCfg);
	LOAD_CONFIG("other", InitOhterCfg);
	
	return true;
}

const RoleClueConfig * WenXiangLouFbConfig::GetRoleClueCfg(int seq)
{
	std::map<int, RoleClueConfig>::iterator it = m_role_clue_cfg.find(seq);
	if (it != m_role_clue_cfg.end())
	{
		return &it->second;
	}
	return NULL;
}

const ItemClueConfig * WenXiangLouFbConfig::GetItemClueCfg(int seq)
{
	std::map<int, ItemClueConfig>::iterator it = m_item_clue_cfg.find(seq);
	if (it != m_item_clue_cfg.end())
	{
		return &it->second;
	}
	return NULL;
}

const EventConfig * WenXiangLouFbConfig::GetEventCfg(int seq)
{
	std::map<int, EventConfig>::iterator it = m_event_cfg.find(seq);
	if (it != m_event_cfg.end())
	{
		return &it->second;
	}
	return NULL;
}

int WenXiangLouFbConfig::InitRoleClueCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		int seq = 0;
		if (!GetSubNodeValue(root_element, "seq", seq))
		{
			return -1;
		}
		std::map<int, RoleClueConfig>::iterator it = m_role_clue_cfg.find(seq);
		if (it != m_role_clue_cfg.end())
		{
			return -2;
		}

		RoleClueConfig &node_cfg = m_role_clue_cfg[seq];
		node_cfg.seq = seq;

		if (!GetSubNodeValue(root_element, "npc_id", node_cfg.npc_id))
		{
			return -3;
		}

		if (!GetSubNodeValue(root_element, "suspicion", node_cfg.suspicion))
		{
			return -4;
		}

		if (!GetSubNodeValue(root_element, "identity", node_cfg.identity))
		{
			return -5;
		}

		if (!GetSubNodeValue(root_element, "add_suspicion", node_cfg.add_suspicion))
		{
			return -6;
		}

		TiXmlElement * directed_element = root_element->FirstChildElement("directed");
		if (NULL != directed_element)		// ÔÊÐíÎª¿Õ
		{
			TiXmlElement * node_element = directed_element->FirstChildElement("node");
			if (NULL != node_element && node_element->FirstChild())
			{
				while (NULL != node_element)
				{
					int node = 0;
					if (!GetNodeValue(node_element, node) || node <= 0)
					{
						return -7;
					}
					node_cfg.m_directed_list.push_back(node);
					node_element = node_element->NextSiblingElement();
				}
			}
		}

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int WenXiangLouFbConfig::InitItemClueCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		int seq = 0;
		if (!GetSubNodeValue(root_element, "seq", seq))
		{
			return -1;
		}
		std::map<int, ItemClueConfig>::iterator it = m_item_clue_cfg.find(seq);
		if (it != m_item_clue_cfg.end())
		{
			return -2;
		}

		ItemClueConfig &node_cfg = m_item_clue_cfg[seq];
		node_cfg.seq = seq;

		if (!GetSubNodeValue(root_element, "x", node_cfg.x) || node_cfg.x < 0)
		{
			return -3;
		}

		if (!GetSubNodeValue(root_element, "y", node_cfg.y) || node_cfg.y < 0)
		{
			return -4;
		}

		if (!GetSubNodeValue(root_element, "directed", node_cfg.directed))
		{
			return -5;
		}

		if (!GetSubNodeValue(root_element, "add_suspicion", node_cfg.add_suspicion))
		{
			return -6;
		}
		root_element = root_element->NextSiblingElement();
	}
	return 0;
}

int WenXiangLouFbConfig::InitEventCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		int stage = 0;
		if (!GetSubNodeValue(root_element, "stage", stage))
		{
			return -1;
		}
		std::map<int, EventConfig>::iterator it = m_event_cfg.find(stage);
		if (it != m_event_cfg.end())
		{
			return -2;
		}

		EventConfig &node_cfg = m_event_cfg[stage];
		node_cfg.stage = stage;

		if (!GetSubNodeValue(root_element, "x", node_cfg.x) || node_cfg.x < 0)
		{
			return -3;
		}

		if (!GetSubNodeValue(root_element, "y", node_cfg.y) || node_cfg.y < 0)
		{
			return -4;
		}

		if (!GetSubNodeValue(root_element, "talk_id", node_cfg.talk_id) || node_cfg.talk_id < 0)
		{
			return -5;
		}

		root_element = root_element->NextSiblingElement();
	}
	return 0;
}

int WenXiangLouFbConfig::InitOhterCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	if (!GetSubNodeValue(root_element, "min_suspicion", m_other_cfg.min_suspicion) || m_other_cfg.min_suspicion < 0)
	{
		return -1;
	}

	if (!GetSubNodeValue(root_element, "prove_error", m_other_cfg.prove_error) || m_other_cfg.prove_error < 0)
	{
		return -2;
	}

	if (!GetSubNodeValue(root_element, "prove_correct", m_other_cfg.prove_correct) || m_other_cfg.prove_correct < 0)
	{
		return -3;
	}

	if (!GetSubNodeValue(root_element, "radius", m_other_cfg.radius) || m_other_cfg.radius < 0)
	{
		return -4;
	}

	if (!GetSubNodeValue(root_element, "item_radius", m_other_cfg.item_radius) || m_other_cfg.item_radius < 0)
	{
		return -5;
	}

	if (!GetSubNodeValue(root_element, "battle_1", m_other_cfg.battle_1) || m_other_cfg.battle_1 < 0)
	{
		return -6;
	}

	if (!GetSubNodeValue(root_element, "battle_2", m_other_cfg.battle_2) || m_other_cfg.battle_2 < 0)
	{
		return -7;
	}

	if (!GetSubNodeValue(root_element, "scene_id", m_other_cfg.scene_id) || m_other_cfg.scene_id < 0)
	{
		return -8;
	}
	
	return 0;
}