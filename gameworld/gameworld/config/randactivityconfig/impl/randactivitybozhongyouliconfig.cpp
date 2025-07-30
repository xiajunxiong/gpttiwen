#include "randactivitybozhongyouliconfig.hpp"

RandActivityBoZhongYouLiConfig::RandActivityBoZhongYouLiConfig()
{
}

RandActivityBoZhongYouLiConfig::~RandActivityBoZhongYouLiConfig()
{
	std::vector<BZYLBaseRewardSectionCfg>().swap(m_base_section_cfg);
	std::vector<BZYLRandRewardSectionCfg>().swap(m_reward_section_cfg);
}

bool RandActivityBoZhongYouLiConfig::Init(TiXmlElement * RootElement, std::string * err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("reward", InitBaseCfg);
	LOAD_RA_CONFIG("reward_rate", InitRewardCfg);
	LOAD_RA_CONFIG("other", InitOtherCfg);

	return true;
}

const std::vector<ItemConfigData>* RandActivityBoZhongYouLiConfig::GetBaseRewardCfg(RandActivityManager * ramgr) const
{
	if (NULL == ramgr) return NULL;

	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_BO_ZHONG_YOU_LI);

	for (int i = 0; i < (int)m_base_section_cfg.size(); ++i)
	{
		if (m_base_section_cfg[i].section_start <= act_real_open_day && m_base_section_cfg[i].section_end >= act_real_open_day)
		{
			return &m_base_section_cfg[i].reward_list;
		}
	}

	return NULL;
}

const ItemConfigData * RandActivityBoZhongYouLiConfig::GetRandRewardCfg(RandActivityManager * ramgr) const
{
	if (NULL == ramgr) return NULL;

	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_BO_ZHONG_YOU_LI);

	for (int i = 0; i < (int)m_reward_section_cfg.size(); ++i)
	{
		if (m_reward_section_cfg[i].section_start <= act_real_open_day && m_reward_section_cfg[i].section_end >= act_real_open_day)
		{
			int rate = RandomNum(m_reward_section_cfg[i].total_rate);

			for (int j = 0; j < (int)m_reward_section_cfg[i].reward_group.size(); ++j)
			{
				if (m_reward_section_cfg[i].reward_group[j].rate >= rate)
				{
					return &m_reward_section_cfg[i].reward_group[j].reward;
				}
			}
		}
	}

	return NULL;
}

int RandActivityBoZhongYouLiConfig::InitBaseCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element) return -544;

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

		if (m_base_section_cfg.size() > 0)
		{
			int pre_index = m_base_section_cfg.size() - 1;
			if (section_start != m_base_section_cfg[pre_index].section_start ||
				section_end != m_base_section_cfg[pre_index].section_end)
			{
				if (section_start < m_base_section_cfg[pre_index].section_end)
				{
					return -100;
				}
				m_base_section_cfg.push_back(BZYLBaseRewardSectionCfg());
			}
		}

		if (m_base_section_cfg.size() == 0)
		{
			if (section_start != 1)
			{
				return -200;
			}
			m_base_section_cfg.push_back(BZYLBaseRewardSectionCfg());
		}

		BZYLBaseRewardSectionCfg & node_cfg = m_base_section_cfg[m_base_section_cfg.size() - 1];
		node_cfg.section_start = section_start;
		node_cfg.section_end = section_end;

		TiXmlElement* reward_list_element = data_element->FirstChildElement("reward_item_list");
		if (NULL == reward_list_element) return -__LINE__;

		TiXmlElement* reward_element = reward_list_element->FirstChildElement("reward_item");
		if (NULL == reward_element) return -__LINE__;

		while (NULL != reward_element)
		{
			ItemConfigData item;
			if (!item.ReadConfig(reward_element)) return -2;

			node_cfg.reward_list.push_back(item);
			reward_element = reward_element->NextSiblingElement();
		}

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityBoZhongYouLiConfig::InitRewardCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element) return -544;

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
				m_reward_section_cfg.push_back(BZYLRandRewardSectionCfg());
			}
		}

		if (m_reward_section_cfg.size() == 0)
		{
			if (section_start != 1)
			{
				return -200;
			}
			m_reward_section_cfg.push_back(BZYLRandRewardSectionCfg());
		}

		BZYLRandRewardSectionCfg & node_cfg = m_reward_section_cfg[m_reward_section_cfg.size() - 1];
		node_cfg.section_start = section_start;
		node_cfg.section_end = section_end;

		BZYLRandRewardCfg reward_cfg;

		int rate;
		if (!GetSubNodeValue(data_element, "rate", rate) || rate < 0) return -1;
		node_cfg.total_rate += rate;
		reward_cfg.rate = node_cfg.total_rate;

		TiXmlElement* reward_element = data_element->FirstChildElement("reward_item");
		if (NULL == reward_element) return -__LINE__;

		if (!reward_cfg.reward.ReadConfig(reward_element)) return -2;

		node_cfg.reward_group.push_back(reward_cfg);

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityBoZhongYouLiConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element) return -544;

	if (!GetSubNodeValue(data_element, "frequency1", m_other_cfg.sowing_limit) || m_other_cfg.sowing_limit < 0) return -1;
	if (!GetSubNodeValue(data_element, "frequency2", m_other_cfg.watering_limit) || m_other_cfg.watering_limit < 0) return -2;
	if (!GetSubNodeValue(data_element, "state3", m_other_cfg.mature_time) || m_other_cfg.mature_time < 0) return -4;

	return 0;
}
