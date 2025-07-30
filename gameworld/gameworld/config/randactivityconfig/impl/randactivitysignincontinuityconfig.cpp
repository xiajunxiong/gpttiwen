#include "randactivitysignincontinuityconfig.hpp"

#include "config/randactivityconfig/randactivityconfigmanager.hpp"

RandActivitySignInContinuityConfig::RandActivitySignInContinuityConfig()
{

}

RandActivitySignInContinuityConfig::~RandActivitySignInContinuityConfig()
{
	std::vector<RASignInContinuitySectionCfg>().swap(m_section_cfg);
}

bool RandActivitySignInContinuityConfig::Init(TiXmlElement *RootElement, std::string *err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("reward", InitSignInContinuityOtherCfg);

	return true;
}

const RASignInContinuityCfg * RandActivitySignInContinuityConfig::GetSignInContinuityCfg(RandActivityManager* ramgr, int seq) const
{
	if (NULL == ramgr) return NULL;
	if (0 > seq) return NULL;

	int open_day = LOGIC_CONFIG->GetRandActivityConfigManager()->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_SIGN_IN_CONTINUITY);

	for (std::vector<RASignInContinuitySectionCfg>::const_iterator it = m_section_cfg.begin(); it != m_section_cfg.end(); ++it)
	{
		if (it->section_start <= open_day && it->section_end >= open_day)
		{
			if (seq >= (int)it->node.size()) return NULL;

			return &it->node[seq];
		}
	}

	return NULL;
}

int RandActivitySignInContinuityConfig::InitSignInContinuityOtherCfg(TiXmlElement *RootElement)
{
	if (NULL == RootElement) return -10000;

	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element) return -20000;

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

		if (m_section_cfg.size() > 0)
		{
			int pre_index = m_section_cfg.size() - 1;
			if (section_start != m_section_cfg[pre_index].section_start ||
				section_end != m_section_cfg[pre_index].section_end)
			{
				if (section_start < m_section_cfg[pre_index].section_end)
				{
					return -888;
				}

				m_section_cfg.push_back(RASignInContinuitySectionCfg());
			}
		}
		if (m_section_cfg.size() == 0)
		{
			if (section_start != 1)
			{
				return -11111;
			}
			m_section_cfg.push_back(RASignInContinuitySectionCfg());
		}

		RASignInContinuitySectionCfg & node_cfg = m_section_cfg[m_section_cfg.size() - 1];
		node_cfg.section_start = section_start;
		node_cfg.section_end = section_end;

		RASignInContinuityCfg item_cfg;

		if (!GetSubNodeValue(data_element, "seq", item_cfg.seq) || item_cfg.seq < 0) return -1;
		if (!GetSubNodeValue(data_element, "day", item_cfg.day) || item_cfg.day < 0) return -2;

		TiXmlElement * reward_list_element = data_element->FirstChildElement("reward_item_list");
		if (reward_list_element != NULL)
		{
			TiXmlElement * reward_element = reward_list_element->FirstChildElement("reward_item");
			while (reward_element)
			{
				ItemConfigData reward;
				if (!reward.ReadConfig(reward_element))
				{
					return -__LINE__;
				}
				item_cfg.item.push_back(reward);
				reward_element = reward_element->NextSiblingElement();
			}
		}

		node_cfg.node.push_back(item_cfg);
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}
