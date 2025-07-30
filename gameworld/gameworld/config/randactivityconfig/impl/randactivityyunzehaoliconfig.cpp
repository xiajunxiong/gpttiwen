#include "randactivityyunzehaoliconfig.hpp"



RandActivityYunZeHaoLiConfig::RandActivityYunZeHaoLiConfig()
{

}


RandActivityYunZeHaoLiConfig::~RandActivityYunZeHaoLiConfig()
{
	std::vector<RAYZHaoLiRewardSectionCfg>().swap(m_reward_section_cfg);
}

bool RandActivityYunZeHaoLiConfig::Init(TiXmlElement *RootElement, std::string *err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("reward", InitRewardCfg);
	LOAD_RA_CONFIG("other", InitOtherCfg);

	return true;
}

const RAYZHaoLiRewardCfg * RandActivityYunZeHaoLiConfig::GetRandRewardCfg(RandActivityManager* ramgr, bool is_advanced_item) const
{
	if (NULL == ramgr) return NULL;

	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_YUN_ZE_HAO_LI);

	for (int i = 0; i < (int)m_reward_section_cfg.size(); ++i)
	{
		if (m_reward_section_cfg[i].section_start <= act_real_open_day && m_reward_section_cfg[i].section_end >= act_real_open_day)
		{
			int total_rate = 0;
			std::vector<int> reward_rate;

			for (int j = 0; j < (int)m_reward_section_cfg[i].reward.size(); ++j)
			{
				reward_rate.push_back((m_reward_section_cfg[i].reward[j].rate + total_rate));

				if (is_advanced_item && m_reward_section_cfg[i].reward[j].is_rate_added == 1)
				{
					reward_rate[j] += m_other_cfg.rate_added;
				}

				total_rate = reward_rate[j];
			}

			int rand_rate = RandomNum(total_rate);

			for (int x = 0; x < (int)reward_rate.size(); ++x)
			{
				if (rand_rate < reward_rate[x])
				{
					return &m_reward_section_cfg[i].reward[x];
				}
			}
		}
	}

	return NULL;
}

int RandActivityYunZeHaoLiConfig::InitRewardCfg(TiXmlElement * RootElement)
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
				m_reward_section_cfg.push_back(RAYZHaoLiRewardSectionCfg());
			}
		}

		if (m_reward_section_cfg.size() == 0)
		{
			if (section_start != 1)
			{
				return -200;
			}
			m_reward_section_cfg.push_back(RAYZHaoLiRewardSectionCfg());
		}

		RAYZHaoLiRewardSectionCfg & node_cfg = m_reward_section_cfg[m_reward_section_cfg.size() - 1];
		node_cfg.section_start = section_start;
		node_cfg.section_end = section_end;

		RAYZHaoLiRewardCfg reward_cfg;

		if (!GetSubNodeValue(data_element, "seq", reward_cfg.seq) || reward_cfg.seq < 0) return -__LINE__;
		if (!GetSubNodeValue(data_element, "rate", reward_cfg.rate) || reward_cfg.rate < 0) return -__LINE__;
		if (!GetSubNodeValue(data_element, "rate_added", reward_cfg.is_rate_added) ||(reward_cfg.is_rate_added != 0 && reward_cfg.is_rate_added != 1)) return -__LINE__;

		TiXmlElement * reward_element = data_element->FirstChildElement("reward_item");
		if (NULL == reward_element) return -__LINE__;

		ItemConfigData item;
		if (!item.ReadConfig(reward_element))
		{
			return -1;
		}

		reward_cfg.reward = item;
		m_reward_section_cfg[m_reward_section_cfg.size()-1].reward.push_back(reward_cfg);

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityYunZeHaoLiConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -544;
	}

	if (!GetSubNodeValue(data_element, "prop_added", m_other_cfg.prop_added_id)) return -__LINE__;
	if (NULL == ITEMPOOL->GetItem(m_other_cfg.prop_added_id)) return -2;

	if (!GetSubNodeValue(data_element, "rate_added", m_other_cfg.rate_added) || m_other_cfg.rate_added < 0) return -__LINE__;
	if (!GetSubNodeValue(data_element, "price_type", m_other_cfg.price_type) || (m_other_cfg.price_type != RAYZ_HAO_LI_BUY_TYPE_GOLD && m_other_cfg.price_type != RAYZ_HAO_LI_BUY_TYPE_JADE)) return -__LINE__;
	if (!GetSubNodeValue(data_element, "price", m_other_cfg.price) || m_other_cfg.price < 0) return -__LINE__;
	if (!GetSubNodeValue(data_element, "buy_type", m_other_cfg.buy_limit) || (m_other_cfg.buy_limit != RAYZ_HAO_LI_LIMIT_BUY_TYPE_TODAY && m_other_cfg.buy_limit != RAYZ_HAO_LI_LIMIT_BUY_TYPE_TOTAL) ) return -__LINE__;
	if (!GetSubNodeValue(data_element, "quantity", m_other_cfg.quantity) || m_other_cfg.quantity < 0) return -__LINE__;

	TiXmlElement * reward_element = data_element->FirstChildElement("reward_item");
	if (NULL == reward_element) return -__LINE__;

	ItemConfigData item;
	if (!item.ReadConfig(reward_element))
	{
		return -3;
	}

	m_other_cfg.limit_item = item;

	while (NULL != data_element)
	{
		int time = 0;
		if (!GetSubNodeValue(data_element, "time", time) || time < 0) return -__LINE__;

		m_other_cfg.time.push_back(time);

		data_element = data_element->NextSiblingElement();
	}


	return 0;
}
