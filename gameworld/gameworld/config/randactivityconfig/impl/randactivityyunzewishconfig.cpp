#include "randactivityyunzewishconfig.hpp"
#include "servercommon/roleactivity/rayunzewishparam.hpp"


RandActivityYunZeWishConfig::RandActivityYunZeWishConfig()
{

}


RandActivityYunZeWishConfig::~RandActivityYunZeWishConfig()
{
	std::vector<RAYZWishRewardSectionCfg>().swap(m_reward_section_cfg);
}

bool RandActivityYunZeWishConfig::Init(TiXmlElement *RootElement, std::string *err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("reward", InitRewardSectionCfg);
	LOAD_RA_CONFIG("other", InitOtherCfg);

	return true;
}

const RAYZWishRewardSectionCfg * RandActivityYunZeWishConfig::GetRewardCfg(RandActivityManager* ramgr) const
{
	if (NULL == ramgr) return NULL;

	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_YUN_ZE_WISH);

	for (int i = 0; i < (int)m_reward_section_cfg.size(); ++i)
	{
		if (m_reward_section_cfg[i].section_start <= act_real_open_day&&m_reward_section_cfg[i].section_end >= act_real_open_day)
		{
			return &m_reward_section_cfg[i];
		}
	}

	return NULL;
}

int RandActivityYunZeWishConfig::InitRewardSectionCfg(TiXmlElement * RootElement)
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
				m_reward_section_cfg.push_back(RAYZWishRewardSectionCfg());
			}
		}

		if (m_reward_section_cfg.size() == 0)
		{
			if (section_start != 1)
			{
				return -200;
			}
			m_reward_section_cfg.push_back(RAYZWishRewardSectionCfg());
		}

		RAYZWishRewardSectionCfg & node_cfg = m_reward_section_cfg[m_reward_section_cfg.size() - 1];
		node_cfg.section_start = section_start;
		node_cfg.section_end = section_end;

		RAYZWishRewardCfg reward_cfg;

		if (!GetSubNodeValue(data_element, "seq", reward_cfg.seq) || reward_cfg.seq < 0) return -__LINE__;
		if (!GetSubNodeValue(data_element, "rate", reward_cfg.rate) || reward_cfg.rate < 0) return -__LINE__;


		TiXmlElement * reward_element = data_element->FirstChildElement("reward_item");
		if (NULL == reward_element) return -__LINE__;

		ItemConfigData item;
		if (!item.ReadConfig(reward_element))
		{
			return -1;
		}

		reward_cfg.reward = item;
		m_reward_section_cfg[m_reward_section_cfg.size() - 1].reward_list.push_back(reward_cfg);

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityYunZeWishConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -544;
	}

	if (!GetSubNodeValue(data_element, "time", m_other_cfg.time) || m_other_cfg.time <= 0 || m_other_cfg.time > 24) return -__LINE__;
	if (!GetSubNodeValue(data_element, "reward_choice", m_other_cfg.reward_choice) || m_other_cfg.reward_choice<=0 || m_other_cfg.reward_choice > (int)m_reward_section_cfg[0].reward_list.size()) return -__LINE__;
	if (!GetSubNodeValue(data_element, "reward_get", m_other_cfg.reward_get) || m_other_cfg.reward_get <= 0 || m_other_cfg.reward_get > (int)m_reward_section_cfg[0].reward_list.size()) return -__LINE__;
	if (m_other_cfg.reward_get > m_other_cfg.reward_choice) return -__LINE__;

	return 0;
}
