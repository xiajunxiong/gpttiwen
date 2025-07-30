#include "randactivityyuanqihelpconfig.hpp"
#include "global/randactivity/randactivitymanager.hpp"
#include <set>

RandActivityYuanQiHelpConfig::RandActivityYuanQiHelpConfig()
{
}

RandActivityYuanQiHelpConfig::~RandActivityYuanQiHelpConfig()
{
	std::vector<YuanQiHelpTaskCfg>().swap(m_task_vec);
}

bool RandActivityYuanQiHelpConfig::Init(TiXmlElement * RootElement, std::string * err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("other", InitOtherCfg);
	LOAD_RA_CONFIG("reward", InitRewardCfg);
	LOAD_RA_CONFIG("task", InitTaskCfg);

	return true;
}

const YuanQiHelpRewardCfg * RandActivityYuanQiHelpConfig::GetRewardCfg(int open_day, int seq) const
{
	for (std::map<int, YuanQiHelpRewardListCfg>::const_iterator it = m_reward_map.begin(); it != m_reward_map.end(); it++)
	{
		YuanQiKey key;
		key.key = it->first;
		if (key.section.section_start <= open_day && (key.section.section_end >= open_day || key.section.section_end == 0))
		{
			std::map<int, YuanQiHelpRewardCfg>::const_iterator reward_it = it->second.m_reward_group.find(seq);
			if (it->second.m_reward_group.end() != reward_it)
			{
				return &reward_it->second;
			}
		}
	}
	return NULL;
}

const YuanQiHelpTaskCfg * RandActivityYuanQiHelpConfig::RandTaskCfg(int pre_task_id[MAX_YUAN_QI_HELP_MAX_TASK_LIST]) const
{
	std::vector<const YuanQiHelpTaskCfg*> m_real_vec;

	for (std::vector<YuanQiHelpTaskCfg>::const_iterator it = m_task_vec.begin(); it != m_task_vec.end(); it++)
	{
		bool is_continue = false;
		for (int i = 0; i < MAX_YUAN_QI_HELP_MAX_TASK_LIST; i++)
		{
			if (it->task_id == pre_task_id[i])is_continue = true;
		}
		if (is_continue)continue;
		m_real_vec.push_back(&(*it));
	}

	int real_task_num = m_real_vec.size();
	if (real_task_num > 0)
	{
		int task_idx = rand() % real_task_num;
		return m_real_vec[task_idx];
	}

	return NULL;
}

const YuanQiHelpTaskCfg * RandActivityYuanQiHelpConfig::GetTaskCfg(int task_id) const
{
	for (std::vector<YuanQiHelpTaskCfg>::const_iterator it = m_task_vec.begin(); it != m_task_vec.end(); it++)
	{
		if (it->task_id == task_id)
		{
			return &(*it);
		}
	}
	return NULL;
}

int RandActivityYuanQiHelpConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -1;
	}

	if (!GetSubNodeValue(data_element, "frequency", m_other_cfg.frequency) || m_other_cfg.frequency <= 0 || m_other_cfg.frequency >= MAX_YUAN_QI_HELP_MAX_TASK_LIST)
	{
		return -2;
	}

	return 0;
}

int RandActivityYuanQiHelpConfig::InitRewardCfg(TiXmlElement * RootElement)
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
		YuanQiKey key;
		key.section.section_start = section_start;
		key.section.section_end = section_end;

		YuanQiHelpRewardListCfg &node_list = m_reward_map[key.key];

		int item_id = 0;
		if (!GetSubNodeValue(data_element, "item_id", item_id))
		{
			return -3;
		}

		TiXmlElement * reward_item_list_element = data_element->FirstChildElement("reward_item_list");
		if (reward_item_list_element != NULL)
		{
			TiXmlElement * reward_item_element = reward_item_list_element->FirstChildElement("reward_item");
			while (NULL != reward_item_element)
			{
				ItemConfigData node_reward;
				if (!node_reward.ReadConfig(reward_item_element))
				{
					return -3;
				}
				YuanQiHelpRewardCfg &reward_node = node_list.m_reward_group[item_id];
				reward_node.m_reward_vec.push_back(node_reward);
				reward_item_element = reward_item_element->NextSiblingElement();
			}
		}
		
		
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityYuanQiHelpConfig::InitTaskCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -1;
	}

	while (data_element != NULL)
	{
		YuanQiHelpTaskCfg node;

		if (!GetSubNodeValue(data_element, "task_id", node.task_id))
		{
			return -1;
		}

		if (!GetSubNodeValue(data_element, "item_id", node.item_id))
		{
			return -2;
		}

		m_task_vec.push_back(node);

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}
