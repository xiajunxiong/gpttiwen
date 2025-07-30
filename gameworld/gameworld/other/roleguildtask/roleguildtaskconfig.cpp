#include "task/taskpool.h"

#include "roleguildtaskconfig.hpp"

RoleGuildTaskConfig::RoleGuildTaskConfig()
{
	m_task_rate_count = 0;
}

RoleGuildTaskConfig::~RoleGuildTaskConfig()
{
}

bool RoleGuildTaskConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG("other", InitOtherCfg);
	LOAD_CONFIG("mission_msg", InitRoleGuildTaskItemCfg);
	LOAD_CONFIG("reward_group", InitRoleGuildTaskRewardGroupCfg);

	return true;
}

const RoleGuildTaskItemCfg * RoleGuildTaskConfig::GetRoleGuildTaskItemCfg(int task_id)
{
	return MapValuesConstPtr(m_task_cfg, task_id);
}

const RoleGuildTaskItemCfg * RoleGuildTaskConfig::GetRandRoleGuildTaskItemCfg(int role_level)
{
	
	int rate_count = 0;
	std::vector<const RoleGuildTaskItemCfg *> rand_list;
	for (std::map<int, RoleGuildTaskItemCfg>::iterator iter = m_task_cfg.begin(); iter != m_task_cfg.end(); ++iter)
	{
		const RoleGuildTaskItemCfg & curr = iter->second;
		if (role_level >= curr.need_role_level)
		{
			rate_count += curr.rate;
			rand_list.push_back(&curr);
		}
	}

	int r = RandomNum(m_task_rate_count);
	for (int i = 0; i < (int)rand_list.size(); ++i)
	{
		const RoleGuildTaskItemCfg * curr = rand_list[i];
		if (r < curr->rate)
		{
			return curr;
		}

		r -= curr->rate;

	}

	return NULL;
}

const RoleGuildTaskRewardGroupItemCfg * RoleGuildTaskConfig::RandGuildTaskRewardGroupItemCfg(int group_id)
{
	const RoleGuildTaskRewardGroupCfg * cfg = MapValuesConstPtr(m_reward_group_cfg, group_id);
	if (!cfg)
	{
		return NULL;
	}

	int r = RandomNum(cfg->rate_count);

	for (int i = 0; i < (int)cfg->group_list.size(); ++i)
	{
		const RoleGuildTaskRewardGroupItemCfg & curr = cfg->group_list[i];
		if (r<curr.rate)
		{
			return &curr;
		}

		r -= curr.rate;
	}

	return NULL;
}

const RoleGuildTaskOtherCfg & RoleGuildTaskConfig::GetOtherCfg() const
{
	return m_other_cfg;
}

int RoleGuildTaskConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}

	if (!GetSubNodeValue(data_element, "tasknum_every_day", m_other_cfg.max_finish_times))
	{
		ret_line;
	}

	if (!GetSubNodeValue(data_element, "refresh_time", m_other_cfg.finish_cd))
	{
		ret_line;
	}

	if (!GetSubNodeValue(data_element, "delete_time", m_other_cfg.give_up_cd))
	{
		ret_line;
	}

	if (!GetSubNodeValue(data_element, "refresh_cost", m_other_cfg.refresh_comsume_value))
	{
		ret_line;
	}

	return 0;
}

int RoleGuildTaskConfig::InitRoleGuildTaskItemCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	m_task_rate_count = 0;
	while (NULL != root_element)
	{
		RoleGuildTaskItemCfg cfg;
		if (!GetSubNodeValue(root_element, "task_id", cfg.task_id))
		{
			ret_line;
		}

		if (!GetSubNodeValue(root_element, "end_task_id", cfg.finish_task_id))
		{
			ret_line;
		}

		if (!TASKPOOL->GetTaskCfgByID(cfg.task_id) || !TASKPOOL->GetTaskCfgByID(cfg.finish_task_id))
		{
			ret_line;
		}

		if (!GetSubNodeValue(root_element, "need_level", cfg.need_role_level))
		{
			ret_line;
		}

		if (!GetSubNodeValue(root_element, "sub_type", cfg.type))
		{
			ret_line;
		}

		if (!GetSubNodeValue(root_element, "change", cfg.rate) || cfg.rate <= 0)
		{
			ret_line;
		}

		if (!GetSubNodeValue(root_element, "is_help", cfg.can_help))
		{
			ret_line;
		}

		if (!GetSubNodeValue(root_element, "family_coin", cfg.guild_exp))
		{
			ret_line;
		}


		if (!GetSubNodeValue(root_element, "gongxian", cfg.guild_gong_xian))
		{
			ret_line;
		}


		if (!GetSubNodeValue(root_element, "reward_id", cfg.reward_group_id))
		{
			ret_line;
		}

		if (!GetSubNodeValue(root_element, "help_family_coin", cfg.give_guild_exp))
		{
			ret_line;
		}


		if (!GetSubNodeValue(root_element, "help_gongxian", cfg.give_guild_gong_xian))
		{
			ret_line;
		}


		if (!GetSubNodeValue(root_element, "help_reward_id", cfg.give_reward_group_id))
		{
			ret_line;
		}

		m_task_rate_count += cfg.rate;
		m_task_cfg[cfg.task_id] = cfg;
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int RoleGuildTaskConfig::InitRoleGuildTaskRewardGroupCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	while (NULL != root_element)
	{
		RoleGuildTaskRewardGroupItemCfg item_cfg;
		if (!GetSubNodeValue(root_element, "seq", item_cfg.group_id))
		{
			ret_line;
		}

		if (!GetSubNodeValue(root_element, "change", item_cfg.rate) || item_cfg.rate <= 0)
		{
			ret_line;
		}

		if (ReadItemConfigDataList(root_element, "item_list", item_cfg.rewards) != 0)
		{
			ret_line;
		}

		RoleGuildTaskRewardGroupCfg & cfg = m_reward_group_cfg[item_cfg.group_id];
		cfg.group_id = item_cfg.group_id;
		cfg.rate_count += item_cfg.rate;
		cfg.group_list.push_back(item_cfg);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}
