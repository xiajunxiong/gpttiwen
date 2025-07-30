#include "randactivityyuexileichongconfig.hpp"

RandActivityYueXiLeiChongConfig::RandActivityYueXiLeiChongConfig()
{
}

RandActivityYueXiLeiChongConfig::~RandActivityYueXiLeiChongConfig()
{
	std::vector<RAYueXiLeiChongRewardSectionCfg>().swap(m_reward_section_cfg);
}

bool RandActivityYueXiLeiChongConfig::Init(TiXmlElement * RootElement, std::string * err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG_2("gift_configure", InitRewardCfg);

	return true;
}

const RAYueXiLeiChongRewardCfg * RandActivityYueXiLeiChongConfig::GetRewardCfg(RandActivityManager * ramgr, int seq) const
{
	if (NULL == ramgr) return NULL;
	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_YUE_XI_LEI_CHONG);

	for (size_t i = 0; i < m_reward_section_cfg.size(); i++)
	{
		if (m_reward_section_cfg[i].IsInclude(act_real_open_day))
		{
			if (seq < 0 || seq >= (int)m_reward_section_cfg[i].reward_cfg.size()) return NULL;
			return &m_reward_section_cfg[i].reward_cfg[seq];
		}
	}

	return NULL;
}

int RandActivityYueXiLeiChongConfig::InitRewardCfg(TiXmlElement * RootElement, std::string & err)
{
	TiXmlElement* data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element) return -544;

	while (data_element)
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
				m_reward_section_cfg.push_back(RAYueXiLeiChongRewardSectionCfg());
			}
		}

		if (m_reward_section_cfg.size() == 0)
		{
			if (section_start != 1)
			{
				return -200;
			}
			m_reward_section_cfg.push_back(RAYueXiLeiChongRewardSectionCfg());
		}

		RAYueXiLeiChongRewardSectionCfg & node_cfg = m_reward_section_cfg[m_reward_section_cfg.size() - 1];
		node_cfg.section_start = section_start;
		node_cfg.section_end = section_end;

		RAYueXiLeiChongRewardCfg cfg;

		if (!GetSubNodeValue(data_element, "acc_recharge_down", cfg.acc_recharge_down) || cfg.acc_recharge_down < 0) return -1;
		if (!GetSubNodeValue(data_element, "gold", cfg.gold_reward) || cfg.gold_reward < 0) return -2;

		{
			TiXmlElement* reward_list_element = data_element->FirstChildElement("reward_item_list");
			if (NULL == reward_list_element) return -101;

			TiXmlElement* reward_element = reward_list_element->FirstChildElement("reward_item");
			if (NULL == reward_element) return -102;

			while (reward_element)
			{
				ItemConfigData item;
				if (!item.ReadConfig(reward_element)) return -2;

				cfg.reward_list.push_back(item);

				reward_element = reward_element->NextSiblingElement();
			}
		}

		node_cfg.reward_cfg.push_back(cfg);

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}
