#include "randactivityyunzeterritoryconfig.hpp"

RandActivityYunZeTerritoryConfig::RandActivityYunZeTerritoryConfig()
{

}


RandActivityYunZeTerritoryConfig::~RandActivityYunZeTerritoryConfig()
{
	std::vector<RAYZTerritoryRewardSectionCfg>().swap(m_reward_section_cfg);
}

bool RandActivityYunZeTerritoryConfig::Init(TiXmlElement *RootElement, std::string *err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("basics", InitBasicsCfg);
	LOAD_RA_CONFIG("reward", InitTaskRrwardCfg);

	return true;
}

int RandActivityYunZeTerritoryConfig::InitTaskRrwardCfg(TiXmlElement *RootElement)
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
					return -100;
				}
				m_reward_section_cfg.push_back(RAYZTerritoryRewardSectionCfg());
			}
		}

		if (m_reward_section_cfg.size() == 0)
		{
			if (section_start != 1)
			{
				return -200;
			}
			m_reward_section_cfg.push_back(RAYZTerritoryRewardSectionCfg());
		}

		RAYZTerritoryRewardSectionCfg & node_cfg = m_reward_section_cfg[m_reward_section_cfg.size() - 1];
		node_cfg.section_start = section_start;
		node_cfg.section_end = section_end;

		RAYZTerritoryTaskRewardCfg reward_cfg;

		if (!GetSubNodeValue(data_element, "section_day", reward_cfg.section_day) || reward_cfg.section_day < 0)
		{
			return -1;
		}

		if (!GetSubNodeValue(data_element, "picture", reward_cfg.picture) || reward_cfg.picture <= 0)
		{
			return -2;
		}

		TiXmlElement * reward_list_element = data_element->FirstChildElement("reward_item_list");
		if (NULL == reward_list_element)
		{
			return -300;
		}

		TiXmlElement * reward_element = reward_list_element->FirstChildElement("reward_item");
		while (NULL != reward_element)
		{
			ItemConfigData t_item;
			if (!t_item.ReadConfig(reward_element))
			{
				return -2;
			}
			
			reward_cfg.item_list.push_back(t_item);
			reward_element = reward_element->NextSiblingElement();
		}

		m_reward_section_cfg[m_reward_section_cfg.size() - 1].node_list.push_back(reward_cfg);

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityYunZeTerritoryConfig::InitBasicsCfg(TiXmlElement *RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -777;
	}

	if (!GetSubNodeValue(data_element, "frequency", m_basics_cfg.frequency)||m_basics_cfg.frequency<=0) return -1;
	if (!GetSubNodeValue(data_element, "frequency_total", m_basics_cfg.frequency_total)||m_basics_cfg.frequency_total<=0) return - 2;

	return 0;
}

const RAYZTerritoryTaskRewardCfg * RandActivityYunZeTerritoryConfig::GetTaskRewardCfg(RandActivityManager* ramgr)const
{
	if (NULL == ramgr ) return NULL;

	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_YUN_ZE_TERRITORY);
	int open_day = this->GetActOpenDay(ramgr, RAND_ACTIVITY_TYPE_YUN_ZE_TERRITORY);

	for (int i = 0; i < (int)m_reward_section_cfg.size(); ++i)
	{
		if (m_reward_section_cfg[i].section_start <= act_real_open_day&&m_reward_section_cfg[i].section_end >= act_real_open_day)
		{
			for (int j = 0; j < m_reward_section_cfg[i].node_list.size(); ++j)
			{
				if (m_reward_section_cfg[i].node_list[j].section_day == open_day)
				{
					const RAYZTerritoryTaskRewardCfg & node = m_reward_section_cfg[i].node_list[j];
					return &node;
				}			
			}
		}
	}

	return NULL;
}