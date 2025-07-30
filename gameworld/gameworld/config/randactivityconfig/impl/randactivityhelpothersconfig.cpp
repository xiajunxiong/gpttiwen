#include "randactivityhelpothersconfig.hpp"
#include "checkresourcecenter.hpp"

#include "item/itempool.h"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"

#include "servercommon/maildef.hpp"

RandActivityHelpOthersConfig::RandActivityHelpOthersConfig()
	:m_task_rate_count(0)
{
}

RandActivityHelpOthersConfig::~RandActivityHelpOthersConfig()
{
	std::vector<RAHelpOtherTaskRewardSectionCfg>().swap(m_reward_section_cfg);
}

bool RandActivityHelpOthersConfig::Init(TiXmlElement * RootElement, std::string * err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("other", InitOtherCfg);
	LOAD_RA_CONFIG("bravery_task", InitTaskCfg);
	LOAD_RA_CONFIG("bravery_gift_group", InitRewardGroupCfg);
	LOAD_RA_CONFIG("bravery_configure", InitTaskRewardCfg);	//需要放在奖励组配置与其他配置读取后面

	return true;
}

const RAHelpOthersTaskInfoCfg * RandActivityHelpOthersConfig::GetTaskInfoCfg(int task_id)const
{
	std::map<int, RAHelpOthersTaskInfoCfg>::const_iterator it = m_task_map.find(task_id);
	if(it != m_task_map.end()) return &it->second;

	return NULL;
}

const RAHelpOthersTaskInfoCfg * RandActivityHelpOthersConfig::GetRankTaskCfg(int cur_task_id, bool is_can_rand_pet_task, bool is_first_rank_day) const
{
	if (is_first_rank_day)
	{
		int rand_rate = rand() % m_task_rate_count;
		for (std::map<int, RAHelpOthersTaskInfoCfg>::const_iterator it = m_task_map.begin(); it != m_task_map.end(); it++)
		{
			if (rand_rate <= it->second.rate)
			{
				return &it->second;
			}
			rand_rate -= it->second.rate;
		}
	}
	else
	{
		std::vector<const RAHelpOthersTaskInfoCfg *> snap_task_list;
		int snap_rate_count = 0;
		for (std::map<int, RAHelpOthersTaskInfoCfg>::const_iterator it = m_task_map.begin(); it != m_task_map.end(); it++)
		{
			if (cur_task_id != it->second.task_id && (is_can_rand_pet_task || HELP_OTHERS_TASK_TYPE_DELIVERY_PET != it->second.task_type))
			{
				snap_task_list.push_back(&it->second);
				snap_rate_count += it->second.rate;
			}
		}
		if (snap_rate_count <= 0 || snap_task_list.empty())
		{
			return NULL;
		}

		int rand_rate = rand() % snap_rate_count;
		for (int i = 0; i < (int)snap_task_list.size(); i++)
		{
			if (rand_rate <= snap_task_list[i]->rate)
			{
				return snap_task_list[i];
			}
			rand_rate -= snap_task_list[i]->rate;
		}
	}

	return NULL;
}

const RAHelpOthersTaskRewardCfg * RandActivityHelpOthersConfig::GetTaskRewardCfg(RandActivityManager * ramgr, int pass_count) const
{
	if(NULL == ramgr || pass_count <= 0 || pass_count > m_other_cfg.ring_up) return NULL;

	int act_real_open_day = this->GetActOpenDay(ramgr, RAND_ACTIVITY_TYPE_HELP_OTHERS);
	for (int i = 0; i < (int)m_reward_section_cfg.size(); i++)
	{
		if (m_reward_section_cfg[i].section_start <= act_real_open_day && m_reward_section_cfg[i].section_end >= act_real_open_day)
		{
			for (int k = 0; k < (int)m_reward_section_cfg[i].node_list.size(); k++)
			{
				const RAHelpOthersTaskRewardCfg & node = m_reward_section_cfg[i].node_list[k];
				if (node.ring_down <= pass_count && node.ring_up >= pass_count)
				{
					return &node;
				}
			}
		}
	}

	return NULL;
}

const RAHelpOthersRewardCfg * RandActivityHelpOthersConfig::GetRewardCfg(int group_id) const
{
	std::map<int, RAHelpOthersRewardGroupCfg>::const_iterator it = m_reward_group_cfg.find(group_id);
	if(m_reward_group_cfg.end() == it) return NULL;

	int rand_rate = rand() % it->second.rate_count;
	for (int i = 0; i < (int)it->second.node_list.size(); i++)
	{
		const RAHelpOthersRewardCfg & node = it->second.node_list[i];
		if (rand_rate <= node.rate)
		{
			return &node;
		}
		rand_rate -= node.rate;
	}

	return NULL;
}

int RandActivityHelpOthersConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}

	if (!GetSubNodeValue(data_element, "ring_up", m_other_cfg.ring_up)) return -1;
	if (!GetSubNodeValue(data_element, "space_times", m_other_cfg.space_times)) return -2;
	if (!GetSubNodeValue(data_element, "level_open", m_other_cfg.level_open) || m_other_cfg.level_open <= 0 || m_other_cfg.level_open > MAX_ROLE_LEVEL) return -3;

	return 0;
}

int RandActivityHelpOthersConfig::InitTaskCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}
	
	m_task_rate_count = 0;
	while (NULL != data_element)
	{
		RAHelpOthersTaskInfoCfg info;
		if (!GetSubNodeValue(data_element, "task_type", info.task_type) || info.task_type < HELP_OTHERS_TASK_TYPE_BEGIN || info.task_type > HELP_OTHERS_TASK_TYPE_MAX) return -1;
		if (HELP_OTHERS_TASK_TYPE_BEGIN == info.task_type)	//初始任务不读
		{
			data_element = data_element->NextSiblingElement();
			continue;
		}

		if (!GetSubNodeValue(data_element, "task_id", info.task_id) || info.task_id <= 0) return -2;
		if (!GetSubNodeValue(data_element, "param_1", info.param1)) return -3;
		if (!GetSubNodeValue(data_element, "param_2", info.param2)) return -4;
		if (!GetSubNodeValue(data_element, "accept_npc", info.accept_npc_seq)) return -5;
		if (!GetSubNodeValue(data_element, "commit_npc", info.commit_npc_seq)) return -6;
		if (!GetSubNodeValue(data_element, "rate", info.rate) || info.rate <= 0) return -7;

		switch (info.task_type)
		{
			case HELP_OTHERS_TASK_TYPE_DELIVERY:
			case HELP_OTHERS_TASK_TYPE_INSTEAD_OF_SEND:
			{
				if(NULL == ITEMPOOL->GetItem(info.param1) || info.param2 <= 0) return -(10 + info.task_type);
			}
			break;
			case HELP_OTHERS_TASK_TYPE_BATTLE:
			{
				if(info.param2 <= 0) return -(10 + info.task_type);
			}
			break;
			case HELP_OTHERS_TASK_TYPE_DELIVERY_PET:
			{
				if(info.param2 <= 0) return -(10 + info.task_type);

				CheckResourceCenter::Instance().GetPetCheck()->Add(info.param1, __FUNCTION__);
			}
			break;
			default:
			break;
		}

		m_task_rate_count += info.rate;
		m_task_map[info.task_id] = info;
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityHelpOthersConfig::InitRewardGroupCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}
	while (NULL != data_element)
	{
		int group_id;
		if (!GetSubNodeValue(data_element, "group_id", group_id) || group_id <= 0) return -1;
		
		RAHelpOthersRewardCfg node_cfg;
		if (!GetSubNodeValue(data_element, "rate", node_cfg.rate) || node_cfg.rate <= 0) return -2;
		TiXmlElement * reward_list_element = data_element->FirstChildElement("reward_item_list");
		if (NULL == reward_list_element)
		{
			return -100;
		}
		TiXmlElement * reward_element = reward_list_element->FirstChildElement("reward_item");
		while (reward_element != NULL)
		{
			ItemConfigData reward;
			if (!reward.ReadConfig(reward_element))
			{
				return -101;
			}
			node_cfg.reward_list.push_back(reward);
			reward_element = reward_element->NextSiblingElement();
		}
		if((int)node_cfg.reward_list.size() > MAX_ATTACHMENT_ITEM_NUM) return - 111;

		m_reward_group_cfg[group_id].rate_count += node_cfg.rate;
		m_reward_group_cfg[group_id].node_list.push_back(node_cfg);
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityHelpOthersConfig::InitTaskRewardCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}

	while (NULL != data_element)
	{
		bool has_section_cfg = true;
		int section_start = 0, section_end = 0;
		if (!GetSubNodeValue(data_element, "section_start", section_start))
		{
			has_section_cfg = false;
		}
		if (!GetSubNodeValue(data_element, "section_end", section_end))
		{
			has_section_cfg = false;
		}

		if (!has_section_cfg) // 没配置，默认给一个无限区间
		{
			section_start = 1;
			section_end = INT_MAX;
		}

		if (0 == section_end) // 0 代表无穷
		{
			section_end = INT_MAX;
		}

		if (m_reward_section_cfg.size() > 0)
		{
			int pre_index = m_reward_section_cfg.size() - 1;
			if (section_start != m_reward_section_cfg[pre_index].section_start ||
				section_end != m_reward_section_cfg[pre_index].section_end)
			{
				if (section_start < m_reward_section_cfg[pre_index].section_end)
				{
					return -777;
				}

				m_reward_section_cfg.push_back(RAHelpOtherTaskRewardSectionCfg());
			}
		}
		if (m_reward_section_cfg.size() == 0)
		{
			if (section_start != 1)
			{
				return -11111;
			}
			m_reward_section_cfg.push_back(RAHelpOtherTaskRewardSectionCfg());
		}

		RAHelpOtherTaskRewardSectionCfg & node_cfg = m_reward_section_cfg[m_reward_section_cfg.size() - 1];
		node_cfg.section_start = section_start;
		node_cfg.section_end = section_end;

		RAHelpOthersTaskRewardCfg reward_cfg;
		if (!GetSubNodeValue(data_element, "ring_down", reward_cfg.ring_down) || reward_cfg.ring_down <= 0 || reward_cfg.ring_down >m_other_cfg.ring_up) return -1;
		if (!GetSubNodeValue(data_element, "ring_up", reward_cfg.ring_up) || reward_cfg.ring_up < reward_cfg.ring_down || reward_cfg.ring_up >m_other_cfg.ring_up) return -2;
		if (!GetSubNodeValue(data_element, "group_id", reward_cfg.group_id) || (reward_cfg.group_id != 0 && m_reward_group_cfg.end() == m_reward_group_cfg.find(reward_cfg.group_id))) return -3;
		if (!GetSubNodeValue(data_element, "exp", reward_cfg.exp) || reward_cfg.exp < 0) return -4;
		if (!GetSubNodeValue(data_element, "coin", reward_cfg.coin) || (reward_cfg.coin < 0)) return -5;

		node_cfg.node_list.push_back(reward_cfg);
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}
