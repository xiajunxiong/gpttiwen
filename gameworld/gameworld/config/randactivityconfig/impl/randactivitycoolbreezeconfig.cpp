#include "randactivitycoolbreezeconfig.hpp"
#include "servercommon/petdef.h"

RandActivityCoolBreezeConfig::RandActivityCoolBreezeConfig()
{
}

RandActivityCoolBreezeConfig::~RandActivityCoolBreezeConfig()
{
}

bool RandActivityCoolBreezeConfig::Init(TiXmlElement * RootElement, std::string * err)
{	
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("other", InitOtherCfg);
	LOAD_RA_CONFIG("cool_breeze", InitTaskListCfg);

	return true;
}

const RACoolBreezeTaskCfg * RandActivityCoolBreezeConfig::GetTaskCfgByType(int type) const
{
	std::map<int, RACoolBreezeTaskCfg>::const_iterator it = m_task_map.find(type);
	if(it == m_task_map.end()) return NULL;

	return &it->second;
}

void RandActivityCoolBreezeConfig::RandTask(int task_group_id, RACoolBreezeTaskInfo& get_task, std::vector<int>* has_type_list) const
{
	std::map<int, RACoolBreezeTaskListCfg>::const_iterator group_it = m_task_list_map.find(task_group_id);
	if(group_it == m_task_list_map.end()) return ;

	int rate_count = group_it->second.rate_count;
	std::vector<RACoolBreezeTaskCfg> node_list = group_it->second.task_cfg_list;
	if (NULL != has_type_list)
	{
		for (int i = 0; i < (int)has_type_list->size(); i++)
		{
			for (int k = 0; k < (int)node_list.size(); k++)
			{
				if ((*has_type_list)[i] == node_list[k].type)
				{
					rate_count -= node_list[k].rate;
					node_list.erase(node_list.begin() + k);
					break;
				}
			}
		}
	}
	if(node_list.empty()) return ;

	int rate_num = RandomNum(rate_count);
	for (int i = 0; i < (int)node_list.size(); i++)
	{
		if (rate_num < node_list[i].rate)
		{
			get_task.type = node_list[i].type;
			get_task.task_type = node_list[i].task_type;
			return;
		}
		rate_num -= node_list[i].rate;
	}

	return ;
}

int RandActivityCoolBreezeConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}

	if (!GetSubNodeValue(data_element, "refresh", m_other_cfg.refresh_num) || m_other_cfg.refresh_num < 0) return -1;
	if (!GetSubNodeValue(data_element, "fixe_refresh_time", m_other_cfg.fixe_refresh_time)) return -2;
	if (!GetSubNodeValue(data_element, "random_refresh_time", m_other_cfg.random_refresh_time) || m_other_cfg.random_refresh_time < 0) return -3;
	m_other_cfg.fixe_refresh_hour = m_other_cfg.fixe_refresh_time / 100;
	m_other_cfg.fixe_refresh_minute = m_other_cfg.fixe_refresh_time % 100;
	m_other_cfg.random_refresh_hour = m_other_cfg.random_refresh_time / 100;
	m_other_cfg.random_refresh_minute = m_other_cfg.random_refresh_time % 100;
	if (m_other_cfg.fixe_refresh_hour < 0 || m_other_cfg.fixe_refresh_hour >= 24 || m_other_cfg.fixe_refresh_minute < 0 || m_other_cfg.fixe_refresh_minute >= 60)
	{
		return -4;
	}
	if (m_other_cfg.random_refresh_hour < 0 || m_other_cfg.random_refresh_hour >= 24 || m_other_cfg.fixe_refresh_minute < 0 || m_other_cfg.fixe_refresh_minute >= 60)
	{
		return -5;
	}

	if (!GetSubNodeValue(data_element, "gold", m_other_cfg.gold) || m_other_cfg.gold < 0) return -6;


	return 0;
}

int RandActivityCoolBreezeConfig::InitTaskListCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}

	while (NULL != data_element)
	{
		int type = 0;
		if (!GetSubNodeValue(data_element, "type", type) || type <= 0) return -1;
		if (NULL != this->GetTaskCfgByType(type))
		{
			return - 2;
		}
		RACoolBreezeTaskCfg node_cfg;
		node_cfg.type = type;

		if (!GetSubNodeValue(data_element, "task_group", node_cfg.task_group) || node_cfg.task_group <= RA_COOL_BREEZE_TASK_GROUP_TYPE_BEGIN || node_cfg.task_group >= RA_COOL_BREEZE_TASK_GROUP_TYPE_MAX) return -3;
		if (!GetSubNodeValue(data_element, "task_type", node_cfg.task_type) || node_cfg.task_type <= RA_COOL_BREEZE_TASK_TYPE_BEGIN || node_cfg.task_type >= RA_COOL_BREEZE_TASK_TYPE_MAX) return -4;
		if (!GetSubNodeValue(data_element, "param_1", node_cfg.param1)) return -5;
		if (!GetSubNodeValue(data_element, "param_2", node_cfg.param2)) return -6;
		if (!GetSubNodeValue(data_element, "rate", node_cfg.rate)) return -7;

		TiXmlElement * list_node = data_element->FirstChildElement("reward_item_list");
		if (NULL == list_node)
		{
			return -100;
		}
		TiXmlElement * item_node = list_node->FirstChildElement("reward_item");
		while (NULL != item_node)
		{
			ItemConfigData item;
			if (!item.ReadConfig(item_node)) return -110;

			node_cfg.reward_list.push_back(item);
			item_node = item_node->NextSiblingElement();
		}

		UNSTD_STATIC_CHECK(11 == RA_COOL_BREEZE_TASK_TYPE_MAX);
		switch (node_cfg.task_type)
		{
			case RA_COOL_BREEZE_TASK_TYPE_PASS_CRYSTAL:
			case RA_COOL_BREEZE_TASK_TYPE_ACTIVE:
			case RA_COOL_BREEZE_TASK_TYPE_STRENGTHEN_NUM:
			case RA_COOL_BREEZE_TASK_TYPE_CONSUME_COIN:
			case RA_COOL_BREEZE_TASK_TYPE_CONSUME_GOLD:
			case RA_COOL_BREEZE_TASK_TYPE_CONSUME_ENERGY:
			case RA_COOL_BREEZE_TASK_TYPE_XUNBAO_NUM:
			case RA_COOL_BREEZE_TASK_TYPE_SEAL_PET:
			case RA_COOL_BREEZE_TASK_TYPE_PET_SKILL_LEARN:
			{
				if(node_cfg.param1 <= 0) return -22;
			}
			break;
			case RA_COOL_BREEZE_TASK_TYPE_STRENGTHEN_LEVEL:
			{
				if(node_cfg.param1 <= 1 || node_cfg.param1 > PET_STRENGTHEN_MAX_LEVEL) return -33;
			}
			break;
		default:
			return - 55;
		}

		m_task_list_map[node_cfg.task_group].rate_count += node_cfg.rate;
		m_task_list_map[node_cfg.task_group].task_cfg_list.push_back(node_cfg);
		m_task_map[node_cfg.type] = node_cfg;
		data_element = data_element->NextSiblingElement();
	}
	//∑¿÷πÀ¢–¬»ŒŒÒ ß∞‹
	std::map<int, RACoolBreezeTaskListCfg>::iterator it = m_task_list_map.find(RA_COOL_BREEZE_TASK_GROUP_TYPE_FIXE);
	if(it == m_task_list_map.end() || (int)it->second.task_cfg_list.size() < MAX_RA_COOL_BREEZE_FIXE_TASK_NUM + 1) return -120;

	/*for (std::map<int, RACoolBreezeTaskListCfg>::iterator it = m_task_list_map.begin(); it != m_task_list_map.end(); it++)
	{
		switch (it->first)
		{
			case RA_COOL_BREEZE_TASK_GROUP_TYPE_FIXE:
			{
				if ((int)it->second.task_cfg_list.size() < MAX_RA_COOL_BREEZE_FIXE_TASK_NUM + 1)
				{
					return -120;
				}
			}
			break;
			case RA_COOL_BREEZE_TASK_GROUP_TYPE_RANDOM:
			{
				if ((int)it->second.task_cfg_list.size() < MAX_RA_COOL_BREEZE_RANDOM_TASK_NUM + 1)
				{
					return -121;
				}
			}
			break;
		default:
			return -122;
		}
	}*/

	return 0;
}
