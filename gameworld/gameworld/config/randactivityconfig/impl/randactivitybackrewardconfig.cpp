#include "randactivitybackrewardconfig.hpp"

RandActivityBackRewardConfig::RandActivityBackRewardConfig()
{
}

RandActivityBackRewardConfig::~RandActivityBackRewardConfig()
{
	for (std::map<int, std::vector<RABackRewardDayCfg> >::iterator it = m_day_cfg.begin(); it != m_day_cfg.end(); it++)
	{
		std::vector<RABackRewardDayCfg>().swap(it->second);
	}
}

bool RandActivityBackRewardConfig::Init(TiXmlElement * RootElement, std::string * err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("gift_configure", InitDayCfg);
	LOAD_RA_CONFIG("task_configure", InitTaskCfg);
	LOAD_RA_CONFIG("other", InitOtherCfg);

	return true;
}

const RABackRewardDayCfg * RandActivityBackRewardConfig::GetDayCfg(RandActivityManager * ramgr, int day_index) const
{
	if (NULL == ramgr) return NULL;

	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_BACK_REWARD);
	std::map<int, std::vector<RABackRewardDayCfg> >::const_iterator it = m_day_cfg.upper_bound(act_real_open_day);
	if (it == m_day_cfg.begin())
		return NULL;

	it--;
	if (day_index <= 0 || day_index > (int)it->second.size())
		return NULL;

	return &it->second[day_index - 1];
}

const RABackRewardTaskCfg * RandActivityBackRewardConfig::GetTaskCfg(RandActivityManager * ramgr, int task_index) const
{
	if (NULL == ramgr) return NULL;

	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_BACK_REWARD);
	std::map<int, RABackRewardTaskMap >::const_iterator it = m_task_cfg.upper_bound(act_real_open_day);
	if (it == m_task_cfg.begin())
		return NULL;

	it--;
	if (task_index <= 0 || task_index > (int)it->second.task_cfg.size())
		return NULL;

	return &it->second.task_cfg[task_index - 1];
}

const std::vector<RABackRewardTaskCfg> * RandActivityBackRewardConfig::GetTaskTypeListCfg(RandActivityManager * ramgr, int task_type) const
{
	if (NULL == ramgr) return NULL;

	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_BACK_REWARD);
	std::map<int, RABackRewardTaskMap >::const_iterator it = m_task_cfg.upper_bound(act_real_open_day);
	if (it == m_task_cfg.begin())
		return NULL;

	it--;

	std::map<int, std::vector<RABackRewardTaskCfg> >::const_iterator list_it = it->second.task_type_list.find(task_type);
	if (list_it == it->second.task_type_list.end())
		return NULL;

	return &list_it->second;
}

int RandActivityBackRewardConfig::InitDayCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -10000;
	}

	while (NULL != root_element)
	{
		int section_start = 0;
		if (!GetSubNodeValue(root_element, "section_start", section_start) || section_start <= 0)
			return -1;

		RABackRewardDayCfg cfg;

		TiXmlElement *list_element = root_element->FirstChildElement("reward_item_list");
		if (NULL == list_element)
			return -1000;

		TiXmlElement *item_element = list_element->FirstChildElement("reward_item");
		while (NULL != item_element)
		{
			if (cfg.count >= sizeof(cfg.reward_item) / sizeof(ItemConfigData))
				return -1001;

			if (!cfg.reward_item[cfg.count].ReadConfig(item_element))
				return -1002;

			cfg.count++;
			item_element = item_element->NextSiblingElement();
		}

		m_day_cfg[section_start].push_back(cfg);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityBackRewardConfig::InitTaskCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -10000;
	}

	while (NULL != root_element)
	{
		int section_start = 0;
		if (!GetSubNodeValue(root_element, "section_start", section_start) || section_start <= 0)
			return -1;

		RABackRewardTaskCfg cfg;
		if (!GetSubNodeValue(root_element, "activity_days", cfg.activity_days) || cfg.activity_days <= 0)
			return -2;

		if (!GetSubNodeValue(root_element, "task_id", cfg.task_id) || cfg.task_id <= 0 || cfg.task_id > MAX_RA_BACK_REWARD_TASK)
			return -3;

		if (!GetSubNodeValue(root_element, "task_type", cfg.task_type) || cfg.task_type <= 0 || cfg.task_type >= RA_BACK_REWARD_TYPE_MAX)
			return -4;

		if (!GetSubNodeValue(root_element, "param_1", cfg.param1) || cfg.param1 < 0)
			return -5;

		if (!GetSubNodeValue(root_element, "param_2", cfg.param2) || cfg.param2 < 0)
			return -6;

		TiXmlElement *item_element = root_element->FirstChildElement("reward_item");
		if (NULL == item_element)
			return -1000;

		if (!cfg.reward_item.ReadConfig(item_element))
			return -1001;

		RABackRewardTaskMap *map = &m_task_cfg[section_start];
		map->task_cfg.push_back(cfg);
		map->task_type_list[cfg.task_type].push_back(cfg);

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityBackRewardConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -10000;
	}

	if (!GetSubNodeValue(root_element, "open_level", m_other_cfg.open_level) || m_other_cfg.open_level <= 0 || m_other_cfg.open_level > MAX_ROLE_LEVEL)
		return -1;

	if (!GetSubNodeValue(root_element, "back_days", m_other_cfg.back_days) || m_other_cfg.back_days <= 0)
		return -2;

	return 0;
}
