#include "randactivityyunzecatchelvesconfig.hpp"
#include "item/itempool.h"
#include "servercommon/activitydef.hpp"


RandActivityYunZeCatchElvesConfig::RandActivityYunZeCatchElvesConfig()
	:m_total_rate(0)
{

}


RandActivityYunZeCatchElvesConfig::~RandActivityYunZeCatchElvesConfig()
{
	std::vector<RAYZCatchElvesBasicsCfg>().swap(m_basics_cfg);
	std::vector<RAYZCatchElvesRewardSectionCfg>().swap(m_reward_section_cfg);
}

bool RandActivityYunZeCatchElvesConfig::Init(TiXmlElement *RootElement, std::string *err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("basics",InitBasicsCfg);
	LOAD_RA_CONFIG("reward", InitRewardCfg);
	LOAD_RA_CONFIG("other", InitOtherCfg);

	return true;
}

const RAYZCatchElvesBasicsCfg * RandActivityYunZeCatchElvesConfig::GetBasicsByElvesCfg(int elves_id) const
{
	if (elves_id <= 0) return NULL;

	for (int i = 0; i < (int)m_basics_cfg.size(); ++i)
	{
		if (m_basics_cfg[i].elves_id == elves_id)
		{
			return &m_basics_cfg[i];
		}
	}

	return NULL;
}

const RAYZCatchElvesBasicsCfg * RandActivityYunZeCatchElvesConfig::GetRandRewardCfg() const
{
	int rand_rate = RandomNum(m_total_rate);
	
	for (int i = 0; i < (int)m_basics_cfg.size(); i++)
	{
		if (m_basics_cfg[i].rate > rand_rate)
		{
			return &m_basics_cfg[i];
		}
	}

	return NULL;
}

const RAYZCatchElvesRewardCfg * RandActivityYunZeCatchElvesConfig::GetRewardByGroup(RandActivityManager * ramgr, int reward_group) const
{
	if (NULL == ramgr || reward_group < 0 ) return NULL;

	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_YUN_ZE_CATCH_ELVES);

	for (int j = 0; j < (int)m_reward_section_cfg.size(); ++j)
	{
		if (m_reward_section_cfg[j].section_start <= act_real_open_day && m_reward_section_cfg[j].section_end >= act_real_open_day)
		{
			for (int x = 0; x < (int)m_reward_section_cfg[j].reward_list.size(); ++x)
			{
				if (m_reward_section_cfg[j].reward_list[x].reward_group == reward_group)
				{
					return &m_reward_section_cfg[j].reward_list[x];
				}
			}
		}
	}

	return NULL;
}

int RandActivityYunZeCatchElvesConfig::InitBasicsCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -544;
	}

	while (NULL != data_element)
	{
		RAYZCatchElvesBasicsCfg cfg;

		int rate = 0;
		if (!GetSubNodeValue(data_element, "monster", cfg.elves_id) || cfg.elves_id < 0) return -__LINE__;
		if (!GetSubNodeValue(data_element, "rate", rate) || rate < 0) return -__LINE__;
		if (!GetSubNodeValue(data_element, "reward_group", cfg.reward_group) || cfg.reward_group < 0) return -__LINE__;
		if (!GetSubNodeValue(data_element, "reward_group_added", cfg.extra_reward_group) || cfg.extra_reward_group < 0) return -__LINE__;

		m_total_rate += rate;
		cfg.rate = m_total_rate;

		m_basics_cfg.push_back(cfg);

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityYunZeCatchElvesConfig::InitRewardCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -544;
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
				m_reward_section_cfg.push_back(RAYZCatchElvesRewardSectionCfg());
			}
		}

		if (m_reward_section_cfg.size() == 0)
		{
			if (section_start != 1)
			{
				return -200;
			}
			m_reward_section_cfg.push_back(RAYZCatchElvesRewardSectionCfg());
		}

		RAYZCatchElvesRewardSectionCfg & node_cfg = m_reward_section_cfg[m_reward_section_cfg.size() - 1];
		node_cfg.section_start = section_start;
		node_cfg.section_end = section_end;

		RAYZCatchElvesRewardCfg reward_cfg;

		if (!GetSubNodeValue(data_element, "reward_group", reward_cfg.reward_group) || reward_cfg.reward_group < 0) return -__LINE__;

		TiXmlElement * reward_list_element = data_element->FirstChildElement("reward_item_list");
		if (NULL == reward_list_element) return -__LINE__;

		TiXmlElement * reward_element = reward_list_element->FirstChildElement("reward_item");
		if (NULL == reward_element) return -__LINE__;

		while (NULL != reward_element)
		{
			ItemConfigData item;
			if (!item.ReadConfig(reward_element))
			{
				return -1;
			}

			reward_cfg.reward.push_back(item);
			reward_element = reward_element->NextSiblingElement();
		}

		m_reward_section_cfg[m_reward_section_cfg.size() - 1].reward_list.push_back(reward_cfg);

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityYunZeCatchElvesConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -544;
	}

	if (!GetSubNodeValue(data_element, "cost_item", m_other_cfg.consume_item)) return -__LINE__;
	if (NULL == ITEMPOOL->GetItem(m_other_cfg.consume_item)) return -1;

	if (!GetSubNodeValue(data_element, "quantity_item", m_other_cfg.consume_num)) return -__LINE__;


	return 0;
}
