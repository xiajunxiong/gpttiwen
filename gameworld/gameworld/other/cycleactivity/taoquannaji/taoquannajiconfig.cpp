#include "taoquannajiconfig.hpp"
#include "item/itempool.h"

TaoQuanNaJiConfig::TaoQuanNaJiConfig()
{
	
}

TaoQuanNaJiConfig::~TaoQuanNaJiConfig()
{
	std::vector<NaJiItem>().swap(m_item_list);
	std::vector<NaJiReward>().swap(m_reward_vec);
}

bool TaoQuanNaJiConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;	
	
	LOAD_CONFIG("item", InitItemList);
	LOAD_CONFIG("item_group", InitMapListCfg);
	LOAD_CONFIG("reward", InitRewardCfg);
	LOAD_CONFIG("other", InitOtherCfg);
	return true;
}

const NaJiItem * TaoQuanNaJiConfig::GetItemCfg(int item_id)
{
	for (std::vector<NaJiItem>::iterator it = m_item_list.begin(); it != m_item_list.end(); it++)
	{
		if (it->item_id == item_id)
		{
			return &(*it);
		}
	}
	return NULL;
}

const NaJiReward * TaoQuanNaJiConfig::GetRewardCfg(int score)
{
	for (std::vector<NaJiReward>::reverse_iterator it = m_reward_vec.rbegin(); it != m_reward_vec.rend(); it++)
	{
		if (it->score <= score)
		{
			return &(*it);
		}
	}
	return NULL;
}

const NaJiMapList * TaoQuanNaJiConfig::RandMapCfg()
{
	if (m_map_group_vec.size() <= 0)return NULL;

	int map_group = rand() % m_map_group_vec.size() + 1;

	std::map<int, NaJiMapList>::iterator it = m_map_group_vec.find(map_group);
	if (it != m_map_group_vec.end())
	{
		return  &it->second;
	}
	return NULL;
}

int TaoQuanNaJiConfig::InitItemList(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;
	
	while (NULL != root_element)
	{
		NaJiItem node;
		if (!GetSubNodeValue(root_element, "item_id", node.item_id))return -1;

		if (!GetSubNodeValue(root_element, "item_type", node.item_type))return -2;

		if (!GetSubNodeValue(root_element, "score", node.score))return -3;

		if (!GetSubNodeValue(root_element, "sp_type", node.sp_type) || NaJiItem::ITEM_EFFECT_TYPE_0 > node.sp_type || node.sp_type > NaJiItem::ITEM_EFFECT_TYPE_2)return -4;
		m_item_list.push_back(node);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int TaoQuanNaJiConfig::InitMapListCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;
	
	while (NULL != root_element)
	{
		int group = 0;
		if (!GetSubNodeValue(root_element, "group", group))return -1;

		NaJiMapList & map_list_node = m_map_group_vec[group];
		map_list_node.group = group;
		if ((int)m_map_group_vec.size() != group)return -2;
		NaJiMapNode node;
		if (!GetSubNodeValue(root_element, "item_id", node.item_id))return -3;
		if (!GetSubNodeValue(root_element, "item_num", node.item_num))return -4;

		map_list_node.m_map_list.push_back(node);
		map_list_node.map_point_num += node.item_num;
		if (map_list_node.map_point_num > TAO_QUAN_NA_JI_Y_NUM * TAO_QUAN_NA_JI_X_NUM)
		{
			return -4;
		}
		
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int TaoQuanNaJiConfig::InitRewardCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;
	
	while (NULL != root_element)
	{
		NaJiReward node;
		if (!GetSubNodeValue(root_element, "score", node.score) || node.score < 0)return -1;

		TiXmlElement* item_list_Element = root_element->FirstChildElement("final_reward_list");
		if (NULL != item_list_Element)
		{
			TiXmlElement* item_Element = item_list_Element->FirstChildElement("final_reward");
			while (NULL != item_Element)
			{
				ItemConfigData reward_node;
				if (!reward_node.ReadConfig(item_Element))
				{
					return -7;
				}
				node.m_reward_vec.push_back(reward_node);
				item_Element = item_Element->NextSiblingElement();
			}
		}
		m_reward_vec.push_back(node);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int TaoQuanNaJiConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement)return -1;

	if (!GetSubNodeValue(dataElement, "circle_num", m_other_cfg.circle_num) || m_other_cfg.circle_num <= 0)return -1;
	if (!GetSubNodeValue(dataElement, "scene_id", m_other_cfg.scene_id) || m_other_cfg.scene_id < 0)return -2;
	if (!GetSubNodeValue(dataElement, "reward_time", m_other_cfg.reward_time) || m_other_cfg.reward_time < 0)return -3;
	if (!GetSubNodeValue(dataElement, "title_item_id", m_other_cfg.title_item_id) || m_other_cfg.title_item_id < 0)return -4;
	if (!GetSubNodeValue(dataElement, "title_score", m_other_cfg.title_score) || m_other_cfg.title_score < 0)return -5;
	return 0;
}
