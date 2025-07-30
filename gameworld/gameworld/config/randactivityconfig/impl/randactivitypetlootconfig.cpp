#include "randactivitypetlootconfig.hpp"
#include "servercommon/roleactivity/rapetlootparam.hpp"
#include "randactivitypetlootconfig.hpp"

RandActivityPetLootConfig::RandActivityPetLootConfig()
{

}

RandActivityPetLootConfig::~RandActivityPetLootConfig()
{
	for (std::map<int, std::vector<RamdGroup> >::iterator it = m_rand_group.begin(); it!= m_rand_group.end(); it++)
	{
		std::vector<RamdGroup>().swap(it->second);
	}
}

bool RandActivityPetLootConfig::Init(TiXmlElement *RootElement, std::string *err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("gift_configure", InitPetLootCfg);
	LOAD_RA_CONFIG("task_gift_configure", InitTaskGiftConfigureCfg);
	LOAD_RA_CONFIG("reward_group", InitRewardGroupCfg);

	return true;
}



const PetLootReward * RandActivityPetLootConfig::GetRewardCfg(int open_day,int seq)const
{
	for (std::map<int, PetLootReward>::const_iterator it = m_cfg_map.begin(); it != m_cfg_map.end(); it++)
	{
		if (open_day >= it->second.section_start && open_day <= it->second.section_end)
		{
			if (it->second.seq == seq)
			{
				return &it->second;
			}
		}
	}
	return NULL;
}

const TaskGiftConfigure * RandActivityPetLootConfig::GetTaskGiftCfg(int type) const
{
	std::map<int, TaskGiftConfigure>::const_iterator it = m_gift_map.find(type);
	if (it != m_gift_map.end())
	{
		return &it->second;
	}
	return NULL;
}

const RamdGroup * RandActivityPetLootConfig::GetRandRewardCfg(int group_id) const
{
	std::map<int, int>::const_iterator it = m_rand_weight.find(group_id);
	std::map<int, std::vector<RamdGroup> >::const_iterator it_rand = m_rand_group.find(group_id);
	if (it != m_rand_weight.end() && it_rand != m_rand_group.end())
	{
		if (it->second > 0)
		{
			int rate = rand() % it->second;
			for (std::vector<RamdGroup>::const_iterator rand_it_group = it_rand->second.begin(); rand_it_group != it_rand->second.end(); rand_it_group++)
			{
				if (rand_it_group->rate >= rate)
				{
					return &(*rand_it_group);
				}
				rate -= rand_it_group->rate;
			}
		}
	}
	return NULL;
}

int RandActivityPetLootConfig::InitPetLootCfg(TiXmlElement *RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;

	while (NULL != root_element)
	{
		int type = 0;
		if (!GetSubNodeValue(root_element, "type", type) || type < 0) return -1;
		PetLootReward &node_cfg = m_cfg_map[type];
		node_cfg.type = type;
		if (!GetSubNodeValue(root_element, "section_start", node_cfg.section_start)) return -2;
		if (!GetSubNodeValue(root_element, "section_end", node_cfg.section_end)) return -3;
		if (node_cfg.section_end == 0)
		{
			node_cfg.section_end = MAX_INT;
		}
		if (!GetSubNodeValue(root_element, "seq", node_cfg.seq) || node_cfg.seq >= MAX_PET_LOOT_TASK_LIST_COUNT) return -4;
		
		TiXmlElement * node_list_element = root_element->FirstChildElement("pet_type");
		if (node_list_element != NULL)
		{
			TiXmlElement * node_element = node_list_element->FirstChildElement("node");
			while (node_element)
			{
				int node = 0;
				if (!GetNodeValue(node_element, node))
				{
					return -5;
				}
				node_cfg.pet_type.push_back(node);
				node_element = node_element->NextSiblingElement();
			}
		}

		if (!GetSubNodeValue(root_element, "int_type", node_cfg.int_type) || node_cfg.int_type < PetLootReward::INT_TYPE_0 || node_cfg.int_type >= PetLootReward::INT_TYPE_MAX) return -6;
		if (!GetSubNodeValue(root_element, "parameter", node_cfg.parameter)) return -7;
		if (!GetSubNodeValue(root_element, "reward_times", node_cfg.reward_times)) return -8;

		TiXmlElement *item_element = root_element->FirstChildElement("reward_item");
		if (item_element != NULL)
		{
			if (!node_cfg.reward_item.ReadConfig(item_element))
			{
				return -9;
			}
		}
		root_element = root_element->NextSiblingElement();
	}
	return 0;
}

int RandActivityPetLootConfig::InitTaskGiftConfigureCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;

	while (NULL != root_element)
	{
		int type = 0;
		if (!GetSubNodeValue(root_element, "type", type) || type < 0) return -1;
		TaskGiftConfigure &node_cfg = m_gift_map[type];
		node_cfg.type = type;
		if (!GetSubNodeValue(root_element, "task_num", node_cfg.task_num)) return -2;
		if (!GetSubNodeValue(root_element, "group_id", node_cfg.group_id)) return -3;

		TiXmlElement *item_element = root_element->FirstChildElement("regular_item");
		if (item_element != NULL)
		{
			if (!node_cfg.reward_item.ReadConfig(item_element))
			{
				return -9;
			}
		}
		root_element = root_element->NextSiblingElement();
	}
	return 0;
}

int RandActivityPetLootConfig::InitRewardGroupCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;

	while (NULL != root_element)
	{
		int group_id = 0;
		if (!GetSubNodeValue(root_element, "group_id", group_id) || group_id < 0) return -1;
		RamdGroup node_cfg;;
		node_cfg.group_id = group_id;
		if (!GetSubNodeValue(root_element, "rate", node_cfg.rate)) return -2;
		TiXmlElement *item_list_element = root_element->FirstChildElement("reward_item_list");
		if (item_list_element == NULL)return -3;
		TiXmlElement *item_element = item_list_element->FirstChildElement("reward_item");
		while(item_element != NULL)
		{
			ItemConfigData reward_cfg;
			if (!reward_cfg.ReadConfig(item_element))
			{
				return -9;
			}
			node_cfg.reward_list.push_back(reward_cfg);
			item_element = item_element->NextSiblingElement();
		}
		m_rand_group[group_id].push_back(node_cfg);
		if (m_rand_weight.find(group_id) != m_rand_weight.end())
		{
			m_rand_weight[group_id] += node_cfg.rate;
		}
		else
		{
			m_rand_weight[group_id] = node_cfg.rate;
		}
		root_element = root_element->NextSiblingElement();
	}
	return 0;
}
