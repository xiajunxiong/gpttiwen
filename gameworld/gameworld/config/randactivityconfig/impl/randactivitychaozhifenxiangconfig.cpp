#include "randactivitychaozhifenxiangconfig.hpp"

RandActivityChaoZhiFenXiangConfig::RandActivityChaoZhiFenXiangConfig()
{
}

RandActivityChaoZhiFenXiangConfig::~RandActivityChaoZhiFenXiangConfig()
{
	for (std::map<int, std::vector<RAChaoZhiFenXiangTaskCfg> >::iterator it = m_task_cfg.begin(); it != m_task_cfg.end(); it++)
	{
		std::vector<RAChaoZhiFenXiangTaskCfg>().swap(it->second);
	}
	for (std::map<int, std::vector<RAChaoZhiFenXiangRewardCfg> >::iterator it = m_reward_cfg.begin(); it != m_reward_cfg.end(); it++)
	{
		std::vector<RAChaoZhiFenXiangRewardCfg>().swap(it->second);
	}
}

bool RandActivityChaoZhiFenXiangConfig::Init(TiXmlElement * RootElement, std::string * err)
{
	PRE_LOAD_RA_CONFIG;
	
	LOAD_RA_CONFIG("task", InitTaskCfg);
	LOAD_RA_CONFIG("reward", InitRewardCfg);
	LOAD_RA_CONFIG("other", InitOtherCfg);

	return true;
}

const std::vector<RAChaoZhiFenXiangTaskCfg>* RandActivityChaoZhiFenXiangConfig::GetTaskCfg(RandActivityManager * ramgr, int add_day) const
{
	if (NULL == ramgr) return NULL;
	int act_real_open_day = this->GetActOpenDay(ramgr, RAND_ACTIVITY_TYPE_CHAOZHI_FENXIANG);
	if (add_day != 0) act_real_open_day += add_day;

	std::map<int, std::vector<RAChaoZhiFenXiangTaskCfg> >::const_iterator it = m_task_cfg.find(act_real_open_day);
	if (it == m_task_cfg.end())
		return NULL;

	return &it->second;
}

const RAChaoZhiFenXiangRewardCfg * RandActivityChaoZhiFenXiangConfig::GetRewardCfg(RandActivityManager * ramgr, int seq) const
{
	if (NULL == ramgr) return NULL;
	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_CHAOZHI_FENXIANG);

	std::map<int, std::vector<RAChaoZhiFenXiangRewardCfg> >::const_iterator act_it = m_reward_cfg.upper_bound(act_real_open_day);
	if (act_it == m_reward_cfg.begin())
		return NULL;
	act_it--;

	if (seq < 0 || seq >= (int)act_it->second.size())
		return NULL;

	return &act_it->second[seq];
}

int RandActivityChaoZhiFenXiangConfig::GetRewardNum(RandActivityManager * ramgr) const
{
	if (NULL == ramgr) return NULL;
	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_CHAOZHI_FENXIANG);

	std::map<int, std::vector<RAChaoZhiFenXiangRewardCfg> >::const_iterator act_it = m_reward_cfg.upper_bound(act_real_open_day);
	if (act_it == m_reward_cfg.begin())
		return NULL;
	act_it--;

	return act_it->second.size();
}

int RandActivityChaoZhiFenXiangConfig::InitTaskCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -10000;
	}

	while (NULL != root_element)
	{
		int section = 0;
		if (!GetSubNodeValue(root_element, "section", section) || section <= 0)
			return -1;

		RAChaoZhiFenXiangTaskCfg cfg;
		if (!GetSubNodeValue(root_element, "type", cfg.type) || cfg.type < 0 || cfg.type >= RA_CHAOZHI_FENXIANG_TASK_TYPE_MAX)
			return -2;

		if (!GetSubNodeValue(root_element, "parameter", cfg.parameter) || cfg.parameter <= 0)
			return -3;

		m_task_cfg[section].push_back(cfg);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityChaoZhiFenXiangConfig::InitRewardCfg(TiXmlElement * RootElement)
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

		RAChaoZhiFenXiangRewardCfg cfg;
		if (!GetSubNodeValue(root_element, "task", cfg.task) || cfg.task <= 0)
			return -2;

		TiXmlElement * list_element = root_element->FirstChildElement("reward_item_list");
		if (NULL == list_element)
			return -1000;

		TiXmlElement * item_element = list_element->FirstChildElement("reward_item");
		while (NULL != item_element)
		{
			ItemConfigData item;
			if (!item.ReadConfig(item_element))
				return -1001;

			cfg.reward_item.push_back(item);
			item_element = item_element->NextSiblingElement();
		}

		m_reward_cfg[section_start].push_back(cfg);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityChaoZhiFenXiangConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -10000;
	}

	if (!GetSubNodeValue(root_element, "price_type", m_other_cfg.price_type) || m_other_cfg.price_type < 0)
		return -1;

	if (!GetSubNodeValue(root_element, "price", m_other_cfg.price) || m_other_cfg.price <= 0)
		return -2;

	if (!GetSubNodeValue(root_element, "buy_type", m_other_cfg.buy_type) || m_other_cfg.buy_type < 0)
		return -3;

	TiXmlElement * item_element = root_element->FirstChildElement("reward_item");
	if (NULL == item_element)
		return -1000;

	if (!m_other_cfg.reward_item.ReadConfig(item_element))
		return -1001;

	return 0;
}
