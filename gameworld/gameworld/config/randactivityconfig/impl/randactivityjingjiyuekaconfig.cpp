#include "randactivityjingjiyuekaconfig.hpp"

RandActivityJingJiYueKaConfig::RandActivityJingJiYueKaConfig()
{
}

RandActivityJingJiYueKaConfig::~RandActivityJingJiYueKaConfig()
{
}

bool RandActivityJingJiYueKaConfig::Init(TiXmlElement * RootElement, std::string * err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG_2("other", InitOtherCfg);
	LOAD_RA_CONFIG_2("basis_configuration", InitBaseCfg);
	LOAD_RA_CONFIG_2("task_configuration", InitTaskCfg);
	LOAD_RA_CONFIG_2("task_num", InitTaskCountCfg);

	return true;
}

const RandActivityJingJiYueKaBaseCfg * RandActivityJingJiYueKaConfig::GetBaseCfg(int active_role_level, int card_level)const
{
	for (std::map<long long, std::map<int, RandActivityJingJiYueKaBaseCfg> >::const_iterator base_it = m_base_cfg.begin(); base_it != m_base_cfg.end(); ++base_it)
	{
		int level_min = 0, level_max = 0;
		this->GetInfoByKey(base_it->first, &level_min, &level_max);
		if (level_min <= active_role_level && active_role_level <= level_max)
		{
			std::map<int, RandActivityJingJiYueKaBaseCfg>::const_iterator it = base_it->second.find(card_level);
			if (it != base_it->second.end())
			{
				return &it->second;
			}
		}
	}

	return NULL;
}

const std::map<int, RandActivityJingJiYueKaBaseCfg>* RandActivityJingJiYueKaConfig::GetBaseMapCfg(int active_role_level) const
{
	for (std::map<long long, std::map<int, RandActivityJingJiYueKaBaseCfg> >::const_iterator base_it = m_base_cfg.begin(); base_it != m_base_cfg.end(); ++base_it)
	{
		int level_min = 0, level_max = 0;
		this->GetInfoByKey(base_it->first, &level_min, &level_max);
		if (level_min <= active_role_level && active_role_level <= level_max && !base_it->second.empty())
		{
			return &base_it->second;
		}
	}
	return NULL;
}

int RandActivityJingJiYueKaConfig::GetMaxCardLevel(int active_role_level)const
{
	for (std::map<long long, std::map<int, RandActivityJingJiYueKaBaseCfg> >::const_iterator base_it = m_base_cfg.begin(); base_it != m_base_cfg.end(); ++base_it)
	{
		int level_min = 0, level_max = 0;
		this->GetInfoByKey(base_it->first, &level_min, &level_max);
		if (level_min <= active_role_level && active_role_level <= level_max && !base_it->second.empty())
		{
			return base_it->second.rbegin()->first;
		}
	}
	return 0;
}

const RandActivityJingJiYueKaTaskCfg * RandActivityJingJiYueKaConfig::GetTaskCfg(int task_groupd_type, int task_id)const
{
	if(task_groupd_type < RA_JING_JI_YUE_KA_TASK_GROUP_TYPE_BEGIN || task_groupd_type >= RA_JING_JI_YUE_KA_TASK_GROUP_TYPE_MAX) return NULL;

	if(m_task_group_list[task_groupd_type].task_list.empty() || task_id < 0 || task_id >= (int)m_task_group_list[task_groupd_type].task_list.size()) return NULL;

	return &m_task_group_list[task_groupd_type].task_list[task_id];
}

bool RandActivityJingJiYueKaConfig::GetSingleRandDayTask(std::set<int> accept_task_list, int & new_task_id, int & new_task_type)const
{
	RandActivityJingJiYueKaTaskGroupCfg task_group_cfg = m_task_group_list[RA_JING_JI_YUE_KA_TASK_GROUP_TYPE_DAY];
	std::vector<RandActivityJingJiYueKaTaskCfg> & day_task_list = task_group_cfg.task_list;
	if (!accept_task_list.empty())
	{
		int delete_num = 0;
		for (std::set<int>::const_iterator it = accept_task_list.begin(); it != accept_task_list.end(); ++it)
		{
			int index = (*it - delete_num);
			if(index >= (int)day_task_list.size()) break;		//可能是配置修改了,此时已接列表可能是之前的配置,任务数量可能遭到缩减

			if (day_task_list[index].is_can_rand)
			{
				task_group_cfg.can_rand_rate_count -= day_task_list[index].rate;
				task_group_cfg.can_rand_task_count -= 1;
			}
			task_group_cfg.rate_count -= day_task_list[index].rate;
			day_task_list.erase(day_task_list.begin() + index);	
			delete_num++;
		}
	}
	if (day_task_list.empty())
	{
		return false;
	}

	int rate_num = RandomNum(task_group_cfg.can_rand_rate_count);
	for (int i = 0; i < (int)day_task_list.size(); i++)
	{
		if(!task_group_cfg.task_list[i].is_can_rand) continue;

		if (rate_num < day_task_list[i].rate)
		{
			new_task_id = day_task_list[i].task_id;
			new_task_type = day_task_list[i].task_type;
			return true;
		}
		rate_num -= day_task_list[i].rate;
	}

	return false;
}

void RandActivityJingJiYueKaConfig::GetRandTaskByType(int task_type, int active_role_level, short & has_task_count, int max_count, RAJingJiYueKaTaskInfo * task_info_list) const
{
	if(max_count <= 0 || NULL == task_info_list || task_type < RA_JING_JI_YUE_KA_TASK_GROUP_TYPE_BEGIN || task_type >= RA_JING_JI_YUE_KA_TASK_GROUP_TYPE_MAX) return;

	has_task_count = 0;
	int refresh_task_count = 0;
	for (std::map<long long, RandActivityJingJiYueKaTaskCountCfg>::const_iterator it = m_task_count_cfg.begin(); it != m_task_count_cfg.end(); ++it)
	{
		int level_min = 0, level_max = 0;
		this->GetInfoByKey(it->first, &level_min, &level_max);
		if (level_min <= active_role_level && active_role_level <= level_max)
		{
			refresh_task_count = it->second.task_num_list[task_type];
		}
	}
	refresh_task_count = GetMin(refresh_task_count, max_count);
	if(refresh_task_count <= 0) return;

	RandActivityJingJiYueKaTaskGroupCfg task_group_cfg = m_task_group_list[task_type];
	for (int i = 0; i < refresh_task_count; i++)
	{
		int rate_num = RandomNum(task_group_cfg.can_rand_rate_count);
		for (int k = 0; k < (int)task_group_cfg.task_list.size(); k++)
		{
			if(!task_group_cfg.task_list[k].is_can_rand) continue;

			if (rate_num < task_group_cfg.task_list[k].rate)
			{
				task_info_list[has_task_count].task_id = task_group_cfg.task_list[k].task_id;
				has_task_count++;
 				task_group_cfg.rate_count -= task_group_cfg.task_list[k].rate;
				task_group_cfg.can_rand_rate_count -= task_group_cfg.task_list[k].rate;
				task_group_cfg.can_rand_task_count -= 1; 
				task_group_cfg.task_list.erase(task_group_cfg.task_list.begin() + k);
				break;
			}
			rate_num -= task_group_cfg.task_list[k].rate;
		}
	}
}

int RandActivityJingJiYueKaConfig::GetConfigVer() const
{
	return m_other_cfg.cfg_ver;
}

int RandActivityJingJiYueKaConfig::InitOtherCfg(TiXmlElement * RootElement, std::string & errormsg)
{
	TiXmlElement *data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -999;
	}

	if (!GetSubNodeValue(data_element, "buy_gold", m_other_cfg.buy_gold) || m_other_cfg.buy_gold <= 0)
	{
		errormsg = STRING_SPRINTF("buy_gold[%d] <= 0?", m_other_cfg.buy_gold);
		return -1;
	}
	if (!GetSubNodeValue(data_element, "task_gold", m_other_cfg.day_task_refresh_gold) || m_other_cfg.day_task_refresh_gold <= 0)
	{
		errormsg = STRING_SPRINTF("task_gold[%d] <= 0?", m_other_cfg.day_task_refresh_gold);
		return -2;
	}
	int is_open = 0;
	if (!GetSubNodeValue(data_element, "is_open", is_open))
	{
		errormsg = STRING_SPRINTF("is_open[%d] not find data?", is_open);
		return -3;
	}
	m_other_cfg.is_open = (0 != is_open) ? true : false;
	if (!GetSubNodeValue(data_element, "open_day", m_other_cfg.open_day) || m_other_cfg.open_day < 0)
	{
		errormsg = STRING_SPRINTF("open_day[%d] < 0?", m_other_cfg.open_day);
		return -4;
	}
	if (!GetSubNodeValue(data_element, "buy_price", m_other_cfg.buy_price) || m_other_cfg.buy_price <= 0)
	{
		errormsg = STRING_SPRINTF("buy_price[%d] <= 0?", m_other_cfg.buy_price);
		return -5;
	}
	if (!GetSubNodeValue(data_element, "cfg_ver", m_other_cfg.cfg_ver) || m_other_cfg.cfg_ver < 0)
	{
		errormsg = STRING_SPRINTF("cfg_ver[%d] < 0?", m_other_cfg.cfg_ver);
		return -6;
	}
	if (!GetSubNodeValue(data_element, "open_level", m_other_cfg.open_role_level) || m_other_cfg.open_role_level <= 0 || m_other_cfg.open_role_level > MAX_ROLE_LEVEL)
	{
		errormsg = STRING_SPRINTF("open_level[%d] <= 0 || > ?", m_other_cfg.cfg_ver, MAX_ROLE_LEVEL);
		return -7;
	}

	return 0;
}

int RandActivityJingJiYueKaConfig::InitBaseCfg(TiXmlElement * RootElement, std::string & errormsg)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}
	
	while (NULL != root_element)
	{
		int level_down = 0, level_up = 0;
		if (!GetSubNodeValue(root_element, "level_down", level_down) || level_down <= 0 || level_down > MAX_ROLE_LEVEL)
		{
			errormsg = STRING_SPRINTF("level_down[%d] <= 0 || > %d?", level_down, MAX_ROLE_LEVEL);
			return -1;
		}
		if (!GetSubNodeValue(root_element, "level_up", level_up) || level_up < 0)
		{
			errormsg = STRING_SPRINTF("level_up[%d] < 0?", level_up);
			return -2;
		}
		if (0 == level_up)
		{
			level_up = MAX_INT;
		}
		if (level_down > level_up)
		{
			errormsg = STRING_SPRINTF("level_down[%d] > level_up[%d]?", level_down, level_up);
			return -3;
		}
		long long base_key = this->GetKey(level_down, level_up);
		int card_level = 0;
		if (!GetSubNodeValue(root_element, "level_card", card_level) || card_level < 0 || card_level >= MAX_RA_JING_JI_YUE_KA_LEVEL_NUM)
		{
			errormsg = STRING_SPRINTF("level_down[%d] > level_up[%d]?", level_down, level_up);
			return -4;
		}
		std::map<long long, std::map<int, RandActivityJingJiYueKaBaseCfg> >::const_iterator base_it = m_base_cfg.find(base_key);
		if (base_it != m_base_cfg.end() && base_it->second.end() != base_it->second.find(card_level))
		{
			errormsg = STRING_SPRINTF("[level_down:%d , level_up:%d] card_level[%d] repeat?", level_down, level_up, card_level);
			return -5;
		}
		RandActivityJingJiYueKaBaseCfg node_cfg;
		if (!GetSubNodeValue(root_element, "upgrade_exp", node_cfg.upgrade_exp) || node_cfg.upgrade_exp < 0)
		{
			errormsg = STRING_SPRINTF("[level_down:%d , level_up:%d, card_level:%d] upgrade_exp[%d] < 0?", level_down, level_up, card_level, node_cfg.upgrade_exp);
			return -6;
		}
		
		int is_reward_item = 0;
		if (!GetSubNodeValue(root_element, "is_reward_item", is_reward_item))
		{
			errormsg = STRING_SPRINTF("[level_down:%d , level_up:%d, card_level:%d] [is_reward_item] not find data?", level_down, level_up, card_level);
			return -7;
		}
		node_cfg.is_has_base_reward = (0 != is_reward_item) ? true : false;
		if (node_cfg.is_has_base_reward)
		{
			TiXmlElement *con_reward_element = root_element->FirstChildElement("con_reward_item");
			if (NULL == con_reward_element)
			{
				errormsg = STRING_SPRINTF("[level_down:%d , level_up:%d, card_level:%d] [con_reward_item] not find data?", level_down, level_up, card_level);
				return -8;
			}
			if (!node_cfg.base_item.ReadConfig(con_reward_element))
			{
				errormsg = STRING_SPRINTF("[level_down:%d , level_up:%d, card_level:%d] con_reward_item[%d, %d] check not pass?", level_down, level_up, card_level,
					node_cfg.base_item.item_id, node_cfg.base_item.num);
				return -9;
			}
		}
		
		TiXmlElement *extra_item_list_element = root_element->FirstChildElement("add_reward_item_list");
		if (NULL != extra_item_list_element)
		{
			TiXmlElement *extra_item_element = extra_item_list_element->FirstChildElement("add_reward_item");
			while (NULL != extra_item_element)
			{
				ItemConfigData item;		
				if (!item.ReadConfig(extra_item_element))
				{
					errormsg = STRING_SPRINTF("[level_down:%d , level_up:%d, card_level:%d] add_reward_item[%d, %d] check not pass?", level_down, level_up, card_level,
						item.item_id, item.num);
					return -10;
				}
				extra_item_element = extra_item_element->NextSiblingElement();
			}
		}

		m_base_cfg[base_key][card_level] = node_cfg;
		root_element = root_element->NextSiblingElement();
	}
	return 0;
}

int RandActivityJingJiYueKaConfig::InitTaskCfg(TiXmlElement * RootElement, std::string & errormsg)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	while (NULL != root_element)
	{
		int task_group_type = 0;
		if (!GetSubNodeValue(root_element, "task_group", task_group_type) || task_group_type < RA_JING_JI_YUE_KA_TASK_GROUP_TYPE_BEGIN || 
			task_group_type >= RA_JING_JI_YUE_KA_TASK_GROUP_TYPE_MAX)
		{
			errormsg = STRING_SPRINTF("task_group[%d] < %d || >= %d?", task_group_type, RA_JING_JI_YUE_KA_TASK_GROUP_TYPE_BEGIN, RA_JING_JI_YUE_KA_TASK_GROUP_TYPE_MAX);
			return -1;
		}
		RandActivityJingJiYueKaTaskCfg node_cfg;
		if (!GetSubNodeValue(root_element, "task_id", node_cfg.task_id) || node_cfg.task_id < 0)
		{
			errormsg = STRING_SPRINTF("[task_group:%d] task_id[%d] < 0?", task_group_type, node_cfg.task_id);
			return -2;
		}
		if (!GetSubNodeValue(root_element, "task_type", node_cfg.task_type) || node_cfg.task_type < RA_JING_JI_YUE_KA_TASK_TYPE_BEGIN || 
			node_cfg.task_type >= RA_JING_JI_YUE_KA_TASK_TYPE_MAX)
		{
			errormsg = STRING_SPRINTF("[task_group:%d, task_id:%d] task_type[%d] < %d || >= %d?", task_group_type, node_cfg.task_id, node_cfg.task_type, 
				RA_JING_JI_YUE_KA_TASK_TYPE_BEGIN, RA_JING_JI_YUE_KA_TASK_TYPE_MAX);
			return -3;
		}
		if (!GetSubNodeValue(root_element, "parameter", node_cfg.param1) || node_cfg.param1 <= 0)
		{
			errormsg = STRING_SPRINTF("[task_group:%d, task_id:%d] parameter_1[%d] < 0?", task_group_type, node_cfg.task_id, node_cfg.param1);
			return -4;
		}
		if (!GetSubNodeValue(root_element, "task_exp", node_cfg.task_exp) || node_cfg.task_exp <= 0)
		{
			errormsg = STRING_SPRINTF("[task_group:%d, task_id:%d] task_exp[%d] < 0?", task_group_type, node_cfg.task_id, node_cfg.task_exp);
			return -5;
		}
		if (!GetSubNodeValue(root_element, "rate", node_cfg.rate) || node_cfg.rate <= 0)
		{
			errormsg = STRING_SPRINTF("[task_group:%d, task_id:%d] rate[%d] <= 0?", task_group_type, node_cfg.task_id, node_cfg.rate);
			return -6;
		}
		int is_cand_rand_task = 0;
		if (!GetSubNodeValue(root_element, "task_confirm", is_cand_rand_task))
		{
			errormsg = STRING_SPRINTF("[task_group:%d, task_id:%d] [task_confirm] data not find?", task_group_type, node_cfg.task_id);
			return -7;
		}
		node_cfg.is_can_rand = (0 != is_cand_rand_task) ? true : false;
		if (m_task_group_list[task_group_type].task_list.size() != node_cfg.task_id)
		{
			errormsg = STRING_SPRINTF("[task_group:%d, task_id:%d] [task_id] sequence error?", task_group_type, node_cfg.task_id);
			return -8;
		}

		m_task_group_list[task_group_type].task_list.push_back(node_cfg);
		m_task_group_list[task_group_type].rate_count += node_cfg.rate;
		if (node_cfg.is_can_rand)
		{
			m_task_group_list[task_group_type].can_rand_rate_count += node_cfg.rate;
			m_task_group_list[task_group_type].can_rand_task_count += 1;
		}
		root_element = root_element->NextSiblingElement();
	}
	return 0;
}

int RandActivityJingJiYueKaConfig::InitTaskCountCfg(TiXmlElement * RootElement, std::string & errormsg)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	while (NULL != root_element)
	{
		int level_down = 0, level_up = 0;
		if (!GetSubNodeValue(root_element, "level_down", level_down) || level_down <= 0 || level_down > MAX_ROLE_LEVEL)
		{
			errormsg = STRING_SPRINTF("level_down[%d] <= 0 || > %d?", level_down, MAX_ROLE_LEVEL);
			return -1;
		}
		if (!GetSubNodeValue(root_element, "level_up", level_up) || level_up < 0)
		{
			errormsg = STRING_SPRINTF("level_up[%d] < 0?", level_up);
			return -2;
		}
		if (0 == level_up)
		{
			level_up = MAX_INT;
		}
		if (level_down > level_up)
		{
			errormsg = STRING_SPRINTF("level_down[%d] > level_up[%d]?", level_down, level_up);
			return -3;
		}
		long long key = this->GetKey(level_down, level_up);
		if (m_task_count_cfg.end() != m_task_count_cfg.find(key))
		{
			errormsg = STRING_SPRINTF("[level_down:%d , level_up:%d] repeat?", level_down, level_up);
			return -4;
		}

		RandActivityJingJiYueKaTaskCountCfg node_cfg;
		if (!GetSubNodeValue(root_element, "season_task_num", node_cfg.task_num_list[RA_JING_JI_YUE_KA_TASK_GROUP_TYPE_SEASON]) || 
			node_cfg.task_num_list[RA_JING_JI_YUE_KA_TASK_GROUP_TYPE_SEASON] <= 0 || node_cfg.task_num_list[RA_JING_JI_YUE_KA_TASK_GROUP_TYPE_SEASON] > MAX_RA_JING_JI_YUE_KA_SEASON_TASK_NUM)
		{
			errormsg = STRING_SPRINTF("[level_down:%d , level_up:%d] season_task_num[%d] <= 0 || > %d?", level_down, level_up, 
				node_cfg.task_num_list[RA_JING_JI_YUE_KA_TASK_GROUP_TYPE_SEASON], MAX_RA_JING_JI_YUE_KA_SEASON_TASK_NUM);
			return -5;
		}
		if (!GetSubNodeValue(root_element, "week_task_num", node_cfg.task_num_list[RA_JING_JI_YUE_KA_TASK_GROUP_TYPE_WEEK]) || 
			node_cfg.task_num_list[RA_JING_JI_YUE_KA_TASK_GROUP_TYPE_WEEK] <= 0 || node_cfg.task_num_list[RA_JING_JI_YUE_KA_TASK_GROUP_TYPE_WEEK] > MAX_RA_JING_JI_YUE_KA_WEEK_TASK_NUM)
		{
			errormsg = STRING_SPRINTF("[level_down:%d , level_up:%d] week_task_num[%d] <= 0 || > %d?", level_down, level_up, 
				node_cfg.task_num_list[RA_JING_JI_YUE_KA_TASK_GROUP_TYPE_WEEK], MAX_RA_JING_JI_YUE_KA_WEEK_TASK_NUM);
			return -6;
		}
		if (!GetSubNodeValue(root_element, "day_task_num", node_cfg.task_num_list[RA_JING_JI_YUE_KA_TASK_GROUP_TYPE_DAY]) || 
			node_cfg.task_num_list[RA_JING_JI_YUE_KA_TASK_GROUP_TYPE_DAY] <= 0 || node_cfg.task_num_list[RA_JING_JI_YUE_KA_TASK_GROUP_TYPE_DAY] > MAX_RA_JING_JI_YUE_KA_DAY_TASK_NUM)
		{
			errormsg = STRING_SPRINTF("[level_down:%d , level_up:%d] day_task_num[%d] <= 0 || > %d?", level_down, level_up, 
				node_cfg.task_num_list[RA_JING_JI_YUE_KA_TASK_GROUP_TYPE_DAY], MAX_RA_JING_JI_YUE_KA_DAY_TASK_NUM);
			return -7;
		}
		//需要保证此时任务组数量大于等于刷新任务数量,防止检测任务删除重新刷新失败
		//每日任务可以刷新,所以任务组数量需要大于 
		if (node_cfg.task_num_list[RA_JING_JI_YUE_KA_TASK_GROUP_TYPE_DAY] >= (int)m_task_group_list[RA_JING_JI_YUE_KA_TASK_GROUP_TYPE_DAY].can_rand_task_count)	
		{
			errormsg = STRING_SPRINTF("[level_down:%d , level_up:%d] day_task_num[%d] >= [task_configuration] page day task count:%d?", level_down, level_up, 
				node_cfg.task_num_list[RA_JING_JI_YUE_KA_TASK_GROUP_TYPE_DAY], (int)m_task_group_list[RA_JING_JI_YUE_KA_TASK_GROUP_TYPE_DAY].can_rand_task_count);
			return -8;
		}
		if (node_cfg.task_num_list[RA_JING_JI_YUE_KA_TASK_GROUP_TYPE_WEEK] > (int)m_task_group_list[RA_JING_JI_YUE_KA_TASK_GROUP_TYPE_WEEK].can_rand_task_count)
		{
			errormsg = STRING_SPRINTF("[level_down:%d , level_up:%d] week_task_num[%d] > [task_configuration] page week task count:%d?", level_down, level_up,
				node_cfg.task_num_list[RA_JING_JI_YUE_KA_TASK_GROUP_TYPE_WEEK], (int)m_task_group_list[RA_JING_JI_YUE_KA_TASK_GROUP_TYPE_WEEK].can_rand_task_count);
			return -9;
		}
		if (node_cfg.task_num_list[RA_JING_JI_YUE_KA_TASK_GROUP_TYPE_SEASON] > (int)m_task_group_list[RA_JING_JI_YUE_KA_TASK_GROUP_TYPE_SEASON].can_rand_task_count)
		{
			errormsg = STRING_SPRINTF("[level_down:%d , level_up:%d] season_task_num[%d] > [task_configuration] page season task count:%d?", level_down, level_up,
				node_cfg.task_num_list[RA_JING_JI_YUE_KA_TASK_GROUP_TYPE_SEASON], (int)m_task_group_list[RA_JING_JI_YUE_KA_TASK_GROUP_TYPE_SEASON].can_rand_task_count);
			return -10;
		}

		m_task_count_cfg[key] = node_cfg;
		root_element = root_element->NextSiblingElement();
	}
	return 0;
}

long long RandActivityJingJiYueKaConfig::GetKey(int level_down, int level_up) const
{
	RandActivityJingJiYueKaKey base_key;
	base_key.param[0] = level_down;
	base_key.param[1] = level_up;

	return base_key.unique_key;
}

void RandActivityJingJiYueKaConfig::GetInfoByKey(long long key, int * level_down, int * level_up) const
{
	if(NULL == level_down || NULL == level_up) return;

	RandActivityJingJiYueKaKey base_key;
	base_key.unique_key = key;
	*level_down = base_key.param[0];
	*level_up = base_key.param[1];
}
