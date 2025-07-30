#include "sanxianguidongconfig.hpp"

#include "checkresourcecenter.hpp"

SanXianConfig::SanXianConfig()
{
	
}

SanXianConfig::~SanXianConfig()
{
	std::vector<SanXianLevelCfg>().swap(m_level_vec);
}

bool SanXianConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;	
	
	LOAD_CONFIG("level", InitLevelCfg);
	//LOAD_CONFIG("reward", InitRewardCfg);
	LOAD_CONFIG("other", InitOtherCfg);
	return true;
}

const SanXianLevelCfg * SanXianConfig::GetLevelCfg(int level)
{
	for (std::vector<SanXianLevelCfg>::iterator it = m_level_vec.begin(); it != m_level_vec.end(); it++)
	{
		if (it->level == level)
		{
			return &(*it);
		}
	}
	return NULL;
}

int SanXianConfig::InitLevelCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;
	int next_level = 0;
	while (NULL != root_element)
	{
		SanXianLevelCfg node;
		if (!GetSubNodeValue(root_element, "level", node.level) || node.level < 0 || ++next_level != node.level)return -1;

		if (!GetSubNodeValue(root_element, "grade", node.grade))return -2;

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
				node.reward_vec.push_back(reward_node);
				item_Element = item_Element->NextSiblingElement();
			}
		}
		m_level_vec.push_back(node);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int SanXianConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement)return -1;

	if (!GetSubNodeValue(dataElement, "chose_time", m_other_cfg.chose_time) || m_other_cfg.chose_time <= 0)return -2;
	if (!GetSubNodeValue(dataElement, "reward_time", m_other_cfg.reward_time) || m_other_cfg.reward_time < 0)return -3;
	if (!GetSubNodeValue(dataElement, "scene_id", m_other_cfg.scene_id) || m_other_cfg.scene_id < 0)return -4;
	if (!GetSubNodeValue(dataElement, "title_item_id", m_other_cfg.title_item_id) || m_other_cfg.title_item_id < 0)return -5;
	if (!GetSubNodeValue(dataElement, "title_level", m_other_cfg.title_level) || m_other_cfg.title_level < 0)return -6;
	if (!GetSubNodeValue(dataElement, "title_id", m_other_cfg.title_id) || m_other_cfg.title_id < 0)return -7;
	TiXmlElement* item_list_Element = dataElement->FirstChildElement("reward_min_list");
	if (NULL != item_list_Element)
	{
		TiXmlElement* item_Element = item_list_Element->FirstChildElement("reward_min");
		while (NULL != item_Element)
		{
			ItemConfigData reward_node;
			if (!reward_node.ReadConfig(item_Element))
			{
				return -99;
			}
			m_other_cfg.reward_vec.push_back(reward_node);
			item_Element = item_Element->NextSiblingElement();
		}
	}
	return 0;
}
