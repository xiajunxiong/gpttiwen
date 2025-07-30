#include "randactivitytigerteasenewyearconfig.hpp"


RandActivityTigerTeaseNewYearConfig::RandActivityTigerTeaseNewYearConfig()
{

}


RandActivityTigerTeaseNewYearConfig::~RandActivityTigerTeaseNewYearConfig()
{
	std::vector<RATTNYBasisSectionCfg>().swap(m_basis_cfg);
	std::vector<RATTNYRewardGroupCfg>().swap(m_reward_cfg);
}

bool RandActivityTigerTeaseNewYearConfig::Init(TiXmlElement *RootElement, std::string *err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("basic_configure", InitBasisCfg);
	LOAD_RA_CONFIG("gift_group", InitRewardCfg);
	LOAD_RA_CONFIG("other", InitOtherCfg);

	return true;
}

const RATTNYBasisCfg * RandActivityTigerTeaseNewYearConfig::GetBasisCfg(RandActivityManager * ramgr) const
{
	if (NULL == ramgr) return NULL;

	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_TIGER_TEASE_NEWYEAR);

	for (int i = 0; i < (int)m_basis_cfg.size(); ++i)
	{
		if (m_basis_cfg[i].section_start <= act_real_open_day && m_basis_cfg[i].section_end >= act_real_open_day)
		{
			return &m_basis_cfg[i].reward_cfg;
		}
	}

	return NULL;
}

const RATTNYRewardCfg * RandActivityTigerTeaseNewYearConfig::GetRandRewardCfg(int reward_group) const
{
	if (reward_group < 0) return NULL;

	for (int i = 0; i < (int)m_reward_cfg.size(); ++i)
	{
		if (m_reward_cfg[i].reward_group == reward_group)
		{
			int rate_rand = RandomNum(m_reward_cfg[i].total_rate);

			for (int j = 0; j < (int)m_reward_cfg[i].reward_cfg.size(); ++j)
			{
				if (rate_rand < m_reward_cfg[i].reward_cfg[j].rate)
				{
					return &m_reward_cfg[i].reward_cfg[j];
				}
			}
		}
	}

	return NULL;
}

int RandActivityTigerTeaseNewYearConfig::InitBasisCfg(TiXmlElement * RootElement)
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

		if (m_basis_cfg.size() > 0)
		{
			int pre_index = m_basis_cfg.size() - 1;
			if (section_start != m_basis_cfg[pre_index].section_start ||
				section_end != m_basis_cfg[pre_index].section_end)
			{
				if (section_start < m_basis_cfg[pre_index].section_end)
				{
					return -100;
				}
				m_basis_cfg.push_back(RATTNYBasisSectionCfg());
			}
		}

		if (m_basis_cfg.size() == 0)
		{
			if (section_start != 1)
			{
				return -200;
			}
			m_basis_cfg.push_back(RATTNYBasisSectionCfg());
		}

		RATTNYBasisSectionCfg & node_cfg = m_basis_cfg[m_basis_cfg.size() - 1];
		node_cfg.section_start = section_start;
		node_cfg.section_end = section_end;

		RATTNYBasisCfg reward_cfg;

		if (!GetSubNodeValue(data_element, "win_rate_group", reward_cfg.win_rate_group) || reward_cfg.win_rate_group < 0) return -__LINE__;
		if (!GetSubNodeValue(data_element, "fail_rate_group", reward_cfg.fail_rate_group) || reward_cfg.fail_rate_group < 0) return -__LINE__;

		TiXmlElement * win_reward_list_element = data_element->FirstChildElement("win_reward_item_list");
		if (NULL == win_reward_list_element) return -__LINE__;

		TiXmlElement * win_reward_element = win_reward_list_element->FirstChildElement("win_reward_item");
		if (NULL == win_reward_element) return -__LINE__;

		while (NULL != win_reward_element)
		{
			ItemConfigData item;
			if (!item.ReadConfig(win_reward_element))
			{
				return -__LINE__;
			}

			reward_cfg.win_reward_list.push_back(item);
			win_reward_element = win_reward_element->NextSiblingElement();
		}

		TiXmlElement * fail_reward_list_element = data_element->FirstChildElement("fail_reward_item_list");
		if (NULL == fail_reward_list_element) return -__LINE__;

		TiXmlElement * fail_reward_element = fail_reward_list_element->FirstChildElement("fail_reward_item");
		if (NULL == fail_reward_element) return -__LINE__;

		while (NULL != fail_reward_element)
		{
			ItemConfigData item;
			if (!item.ReadConfig(fail_reward_element))
			{
				return -__LINE__;
			}

			reward_cfg.fail_reward_list.push_back(item);
			fail_reward_element = fail_reward_element->NextSiblingElement();
		}

		node_cfg.reward_cfg = reward_cfg;

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityTigerTeaseNewYearConfig::InitRewardCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -544;
	}

	while (NULL != data_element)
	{
		
		int reward_group = -1;
		if (!GetSubNodeValue(data_element, "reward_group", reward_group) || reward_group < 0) return -__LINE__;

		if (m_reward_cfg.size() > 0)
		{
			if (reward_group != m_reward_cfg[m_reward_cfg.size() - 1].reward_group)
			{
				for (int i = 0; i < (int)m_reward_cfg.size(); ++i)
				{
					if (m_reward_cfg[i].reward_group == reward_group) return -__LINE__;
				}
				m_reward_cfg.push_back(RATTNYRewardGroupCfg());
			}
		}
		else
		{
			m_reward_cfg.push_back(RATTNYRewardGroupCfg());
		}
		

		RATTNYRewardGroupCfg & node_cfg = m_reward_cfg[m_reward_cfg.size() - 1];
		node_cfg.reward_group = reward_group;

		RATTNYRewardCfg reward_cfg;

		int rate = 0;
		if (!GetSubNodeValue(data_element, "rate", rate) || rate < 0) return -__LINE__;
		node_cfg.total_rate += rate;
		reward_cfg.rate = node_cfg.total_rate;

		TiXmlElement * win_reward_list_element = data_element->FirstChildElement("win_reward_item_list");
		if (NULL != win_reward_list_element)
		{
			TiXmlElement * win_reward_element = win_reward_list_element->FirstChildElement("win_reward_item");
			if (NULL == win_reward_element) return -__LINE__;

			while (NULL != win_reward_element)
			{
				ItemConfigData item;
				if (!item.ReadConfig(win_reward_element))
				{
					return -__LINE__;
				}

				reward_cfg.reward_list.push_back(item);
				win_reward_element = win_reward_element->NextSiblingElement();
			}
		}

		node_cfg.reward_cfg.push_back(reward_cfg);

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityTigerTeaseNewYearConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -544;
	}

	if (!GetSubNodeValue(data_element, "day_num", m_other_cfg.day_num) || m_other_cfg.day_num < 0) return -__LINE__;

	return 0;
}
