#include "suitlistconfig.hpp"
#include "item/itempool.h"

SuitListConfig::SuitListConfig()
{
}

SuitListConfig::~SuitListConfig()
{
}

bool SuitListConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG("suit_group", InitSuitGroupCfg);
	LOAD_CONFIG("suit_attr", InitSuitValueCfg);
	LOAD_CONFIG("suit_combine", InitOldSuitChangeNewCfg);
	return true;
}

const SuitListGroup * SuitListConfig::GetSuitListGroup(int group_id)
{
	std::map<int, SuitListGroup>::iterator it = m_suit_list_map.find(group_id);
	if (it != m_suit_list_map.end())
	{
		return &it->second;
	}
	return NULL;
}

const SuitEffect * SuitListConfig::GetSuitEffectList(int suit_id, int color, int suit_num)
{
	std::map<int, SuitEffectColorList>::iterator it = m_suit_effect_map.find(suit_id);
	if (it != m_suit_effect_map.end())
	{
		std::map<int, SuitEffectList>::iterator it2 = it->second.m_effect.find(color);
		if (it2 != it->second.m_effect.end())
		{
			for (int idx = 0; idx < it2->second.suit_num && it2->second.suit_num < MAX_SUIT_EFFECT_LIST_NUM; idx++)
			{
				if (it2->second.m_effect_list[idx].suit_compose_1 == suit_num)
				{
					return &it2->second.m_effect_list[idx];
				}
			}
		}
	}
	return NULL;
}

int SuitListConfig::GetNewSuitByOldSuit(int old_suit_id)
{
	std::map<int, int>::iterator it = m_suit_combine_map.find(old_suit_id);
	if (it != m_suit_combine_map.end())
	{
		return it->second;
	}

	return 0;
}

int SuitListConfig::InitSuitGroupCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		int suitgroup_id = 0;
		if (!GetSubNodeValue(root_element, "suitgroup_id", suitgroup_id) || suitgroup_id <= 0)
		{
			return -1;
		}
		SuitListGroup & node_cfg = m_suit_list_map[suitgroup_id];
		
		SuitGroup node;
		
		if (!GetSubNodeValue(root_element, "id", node.id))
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "rate", node.rate))
		{
			return -3;
		}

		node_cfg.rate_weight += node.rate;
		node_cfg.m_suit_group.push_back(node);
		
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int SuitListConfig::InitSuitValueCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		int id = 0;
		if (!GetSubNodeValue(root_element, "id", id) || id <= 0)
		{
			return -1;
		}

		int color = 0;
		if (!GetSubNodeValue(root_element, "color", color) || color <= 0)
		{
			return -2;
		}

		SuitEffectColorList & node_color_cfg = m_suit_effect_map[id];
		SuitEffectList &node_cfg = node_color_cfg.m_effect[color];

		node_cfg.id = id;
		node_cfg.color = color;

		if (node_cfg.suit_num >= MAX_SUIT_EFFECT_LIST_NUM)return -2;
		
		SuitEffect &node = node_cfg.m_effect_list[node_cfg.suit_num++];

		if (!GetSubNodeValue(root_element, "suit_compose_1", node.suit_compose_1) || node.suit_compose_1 < 0)
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "sp_id", node.sp_id) || node.sp_id < 0)
		{
			return -3;
		}

		for (int i = 0; i < MAX_SUIT_EFFECT_VALUE_NUM; i++)
		{
			std::string reader_mate = STRING_SPRINTF("attr_type_%d", i + 1);
			if (!GetSubNodeValue(root_element, reader_mate, node.m_value[i].attr_type) || node.m_value[i].attr_type < 0)
			{
				return -(10 * i + 4);
			}
			reader_mate = STRING_SPRINTF("attr_add_%d", i + 1);
			if (!GetSubNodeValue(root_element, reader_mate, node.m_value[i].attr_add) || node.m_value[i].attr_add < 0)
			{
				return -(10 * i + 5);
			}
		}

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int SuitListConfig::InitOldSuitChangeNewCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		int id = 0;
		if (!GetSubNodeValue(root_element, "id", id) || id <= 0)
		{
			return -1;
		}

		int suitgroup_id = 0;
		if (!GetSubNodeValue(root_element, "suitgroup_id", suitgroup_id) || suitgroup_id <= 0)
		{
			return -2;
		}
		int &new_id = m_suit_combine_map[id];
		new_id = suitgroup_id;

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}
