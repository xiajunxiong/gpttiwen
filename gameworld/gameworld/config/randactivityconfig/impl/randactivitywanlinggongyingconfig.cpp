#include "randactivitywanlinggongyingconfig.hpp"

RandActivityWanLingGongYingConfig::RandActivityWanLingGongYingConfig()
{
}

RandActivityWanLingGongYingConfig::~RandActivityWanLingGongYingConfig()
{
	for (std::map<int, std::vector<RAWanLingGongYingTaskCfg> >::iterator it = m_task_cfg.begin(); it!= m_task_cfg.end(); it++)
	{
		std::vector<RAWanLingGongYingTaskCfg>().swap(it->second);
	}
	for (std::map<int, std::map<int, std::vector<int> > >::iterator it = m_task_index_cfg.begin(); it!= m_task_index_cfg.end(); it++){
		for (std::map<int, std::vector<int> >::iterator nd_it = it->second.begin(); nd_it != it->second.end(); nd_it++){
			std::vector<int>().swap(nd_it->second);
		}
	}
	std::vector<int>().swap(m_activity_cfg);
}

bool RandActivityWanLingGongYingConfig::Init(TiXmlElement * RootElement, std::string * err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("activity_configuration", InitActivityCfg);
	LOAD_RA_CONFIG("other", InitOtherCfg);
	LOAD_RA_CONFIG("basis_configuration", InitTaskCfg);

	return true;
}

const RAWanLingGongYingTaskCfg * RandActivityWanLingGongYingConfig::GeTaskCfg(RandActivityManager * ramgr, int index) const
{
	if (NULL == ramgr) return NULL;

	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_WAN_LING_GONG_YING);

	std::map<int, std::vector<RAWanLingGongYingTaskCfg> >::const_iterator it = m_task_cfg.upper_bound(act_real_open_day);
	if (it == m_task_cfg.begin())
		return NULL;

	it--;
	if (index < 0 || index >= (int)it->second.size() || index >= RA_WAN_LING_GONG_YING_TASK_MAX)
		return NULL;

	return &it->second[index];
}

const std::vector<int>* RandActivityWanLingGongYingConfig::GetTaskIndexCfg(RandActivityManager * ramgr, int activity_type) const
{
	if (NULL == ramgr) return NULL;

	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_WAN_LING_GONG_YING);

	std::map<int, std::map<int, std::vector<int> > >::const_iterator it = m_task_index_cfg.upper_bound(act_real_open_day);
	if (it == m_task_index_cfg.begin())
		return NULL;
	it--;

	std::map<int, std::vector<int> >::const_iterator index_it = it->second.find(activity_type);
	if (index_it == it->second.end())
		return NULL;

	return &index_it->second;
}

int RandActivityWanLingGongYingConfig::GetActivityType(int task_type) const
{
	if (task_type < 0 || task_type >= (int)m_activity_cfg.size())
		return -1;

	return m_activity_cfg[task_type];
}

int RandActivityWanLingGongYingConfig::InitActivityCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -10000;
	}

	while (NULL != root_element)
	{
		int activity_type = 0;
		if (!GetSubNodeValue(root_element, "activity_type", activity_type) || activity_type < RAND_ACTIVITY_TYPE_BEGIN || activity_type >= RAND_ACTIVITY_TYPE_END)
			return -1;

		m_activity_cfg.push_back(activity_type);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityWanLingGongYingConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -10000;
	}

	if (!GetSubNodeValue(root_element, "buy_money", m_other_cfg.buy_gold) || m_other_cfg.buy_gold <= 0)
		return -1;

	return 0;
}

int RandActivityWanLingGongYingConfig::InitTaskCfg(TiXmlElement * RootElement)
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

		RAWanLingGongYingTaskCfg cfg;
		if (!GetSubNodeValue(root_element, "task_id", cfg.index) || cfg.index < 0 || cfg.index >= RA_WAN_LING_GONG_YING_TASK_MAX)
			return -2;

		if (!GetSubNodeValue(root_element, "task_type", cfg.type) || cfg.type < 0 || cfg.type >= (int)m_activity_cfg.size())
			return -3;

		if (!GetSubNodeValue(root_element, "param_1", cfg.param1) || cfg.param1 < 0)
			return -4;
		if (!GetSubNodeValue(root_element, "param_2", cfg.param2) || cfg.param2 < 0)
			return -5;

		{
			TiXmlElement * list_element = root_element->FirstChildElement("reward_item_list");
			if (NULL == list_element)
				return -1000;

			TiXmlElement * item_element = list_element->FirstChildElement("reward_item");
			while (NULL != item_element)
			{
				if (cfg.reward_count > sizeof(cfg.reward_item) / sizeof(ItemConfigData))
					return -1001;

				if (!cfg.reward_item[cfg.reward_count].ReadConfig(item_element))
					return -6;

				cfg.reward_count++;
				item_element = item_element->NextSiblingElement();
			}
		}
		{
			TiXmlElement * list_element = root_element->FirstChildElement("unfinish_reward_item_list");
			if (NULL == list_element)
				return -1000;

			TiXmlElement * item_element = list_element->FirstChildElement("unfinish_reward_item");
			while (NULL != item_element)
			{
				if (cfg.unfinish_count > sizeof(cfg.unfinish_item) / sizeof(ItemConfigData))
					return -1001;

				if (!cfg.unfinish_item[cfg.unfinish_count].ReadConfig(item_element))
					return -6;

				cfg.unfinish_count++;
				item_element = item_element->NextSiblingElement();
			}
		}

		m_task_cfg[section_start].push_back(cfg);

		int activity_type = m_activity_cfg[cfg.type];
		m_task_index_cfg[section_start][activity_type].push_back(cfg.index);

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}