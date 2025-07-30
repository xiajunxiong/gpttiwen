#include "randactivityyuanqijishibuconfig.hpp"
#include "servercommon/activitydef.hpp"
#include "servercommon/roleactivity/rayuanqijishibuparam.hpp"

RandActivityYuanQiJiShiBuConfig::RandActivityYuanQiJiShiBuConfig()
{
}

RandActivityYuanQiJiShiBuConfig::~RandActivityYuanQiJiShiBuConfig()
{
	for (std::map<short, std::vector<RAYuanQiJiShiBuTaskCfg> >::iterator it = m_task_cfg.begin(); it!= m_task_cfg.end(); it++)
	{
		std::vector<RAYuanQiJiShiBuTaskCfg>().swap(it->second);
	}

	for (std::map<short, std::vector<RAYuanQiJiShiBuRewardCfg> >::iterator it = m_reward_cfg.begin(); it!= m_reward_cfg.end(); it++)
	{
		std::vector<RAYuanQiJiShiBuRewardCfg>().swap(it->second);
	}
}

bool RandActivityYuanQiJiShiBuConfig::Init(TiXmlElement * RootElement, std::string * err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("task", InitTaskCfg);
	LOAD_RA_CONFIG("reward", InitRewardCfg);
	LOAD_RA_CONFIG("other", InitOtherCfg);

	return true;
}

const std::vector<RAYuanQiJiShiBuTaskCfg>* RandActivityYuanQiJiShiBuConfig::GetTaskListCfg(RandActivityManager * ramgr) const
{
	if (NULL == ramgr) return NULL;
	int act_real_open_day = this->GetActOpenDay(ramgr, RAND_ACTIVITY_TYPE_YUAN_QI_JI_SHI_BU);

	std::map<short, std::vector<RAYuanQiJiShiBuTaskCfg> >::const_iterator day_it = m_task_cfg.find(act_real_open_day);
	if (day_it == m_task_cfg.end())
		return NULL;

	return &day_it->second;
}

const RAYuanQiJiShiBuRewardCfg * RandActivityYuanQiJiShiBuConfig::GetRewardCfg(RandActivityManager * ramgr, int seq) const
{
	if (NULL == ramgr) return NULL;
	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_YUAN_QI_JI_SHI_BU);

	std::map<short, std::vector<RAYuanQiJiShiBuRewardCfg> >::const_iterator it = m_reward_cfg.upper_bound(act_real_open_day);
	if (it == m_reward_cfg.begin())
		return NULL;
	it--;

	if (seq < 0 || seq >= (int)it->second.size())
		return NULL;

	return &it->second[seq];
}

const std::vector<RAYuanQiJiShiBuRewardCfg>* RandActivityYuanQiJiShiBuConfig::GetRewardMap(RandActivityManager * ramgr) const
{
	if (NULL == ramgr) return NULL;
	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_YUAN_QI_JI_SHI_BU);

	std::map<short, std::vector<RAYuanQiJiShiBuRewardCfg> >::const_iterator it = m_reward_cfg.upper_bound(act_real_open_day);
	if (it == m_reward_cfg.begin())
		return NULL;
	it--;

	return &it->second;
}


int RandActivityYuanQiJiShiBuConfig::InitTaskCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -10000;
	}

	while (NULL != data_element)
	{
		int section = 0;
		if (!GetSubNodeValue(data_element, "section", section) || section <= 0)
			return -1;

		RAYuanQiJiShiBuTaskCfg cfg;
		if (!GetSubNodeValue(data_element, "type", cfg.task_type) || cfg.task_type <= 0 || cfg.task_type >= RA_YUAN_QI_JISHIBU_TASK_TYPE_MAX)
			return -2;
		if (!GetSubNodeValue(data_element, "parameter", cfg.param) || cfg.param <= 0)
			return -3;
		if (!GetSubNodeValue(data_element, "combat", cfg.combat) || cfg.combat <= 0)
			return -4;

		m_task_cfg[section].push_back(cfg);
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityYuanQiJiShiBuConfig::InitRewardCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -10000;
	}

	while (NULL != data_element)
	{
		int section_start = 0;
		if (!GetSubNodeValue(data_element, "section_start", section_start) || section_start <= 0)
			return -1;

		RAYuanQiJiShiBuRewardCfg cfg;
		if (!GetSubNodeValue(data_element, "combat", cfg.combat) || cfg.combat <= 0)
			return -2;

		{
			TiXmlElement * item_element = data_element->FirstChildElement("reward1_item");
			if (NULL == item_element)
				return -3;

			if (!cfg.reward.ReadConfig(item_element))
				return -4;
		}
		
		{
			TiXmlElement * list_element = data_element->FirstChildElement("reward2_item_list");
			if (NULL == list_element)
				return -5;

			TiXmlElement * item_element = list_element->FirstChildElement("reward2_item");
			while (NULL != item_element)
			{
				ItemConfigData item;
				if (!item.ReadConfig(item_element))
					return -6;

				cfg.reward_2.push_back(item);
				item_element = item_element->NextSiblingElement();
			}
		}
	
		m_reward_cfg[section_start].push_back(cfg);
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityYuanQiJiShiBuConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -10000;
	}

	if (!GetSubNodeValue(data_element, "price", m_other_cfg.price) || m_other_cfg.price <= 0)
		return -1;

	return 0;
}
