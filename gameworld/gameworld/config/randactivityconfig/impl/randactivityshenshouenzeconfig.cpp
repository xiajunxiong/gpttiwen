#include "config/randactivityconfig/randactivityconfigmanager.hpp"

#include "randactivityshenshouenzeconfig.hpp"

RandActivityShenShouEnZeConfig::RandActivityShenShouEnZeConfig()
{
}

RandActivityShenShouEnZeConfig::~RandActivityShenShouEnZeConfig()
{
	std::vector<RandActivityShenShouEnZeSectionCfg>().swap(m_section_cfg);
	std::vector<RandActivityShenShouEnZeSelectRewardSectionCfg>().swap(m_times_reward_section_cfg);
	std::vector<RandActivityShenShouEnZeBuySectionCfg>().swap(m_buy_section_cfg);
}

bool RandActivityShenShouEnZeConfig::Init(TiXmlElement * RootElement, std::string * err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("other", InitOtherfg);
	LOAD_RA_CONFIG("reward", InitRandActivityShenShouEnZeCfg);
	LOAD_RA_CONFIG("get_reward", InitRandActivityShenShouEnZeSelectRewardCfg);
	LOAD_RA_CONFIG("prop", InitRandActivityShenShouEnZeBuyItemCfg);

	return true;
}

const RandActivityShenShouEnZeOtherCfg & RandActivityShenShouEnZeConfig::GetOtherCfg() const
{
	return m_other_cfg;
}

const RandActivityShenShouEnZeSectionCfg * RandActivityShenShouEnZeConfig::GetRandActivityShenShouEnZeSectionCfg(RandActivityManager * ramgr) const
{
	int act_real_open_day = LOGIC_CONFIG->GetRandActivityConfigManager()->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_SHEN_SHOU_EN_ZE);

	for (std::vector<RandActivityShenShouEnZeSectionCfg>::const_iterator it = m_section_cfg.begin(); it != m_section_cfg.end(); it++)
	{
		const RandActivityShenShouEnZeSectionCfg & curr = *it;
		if (curr.section_start <= act_real_open_day && act_real_open_day <= curr.section_end)
		{
			return &curr;
		}
	}

	return NULL;
}

const RandActivityShenShouEnZeSelectRewardCfg * RandActivityShenShouEnZeConfig::GetRandActivityShenShouEnZeSelectRewardCfg(RandActivityManager * ramgr, int reward_seq) const
{
	int act_real_open_day = LOGIC_CONFIG->GetRandActivityConfigManager()->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_SHEN_SHOU_EN_ZE);

	for (std::vector<RandActivityShenShouEnZeSelectRewardSectionCfg>::const_iterator it = m_times_reward_section_cfg.begin(); it != m_times_reward_section_cfg.end(); it++)
	{
		const RandActivityShenShouEnZeSelectRewardSectionCfg & curr = *it;
		if (curr.section_start <= act_real_open_day && act_real_open_day <= curr.section_end)
		{
			return MapValuesConstPtr(curr.item_list, reward_seq);
		}
	}

	return NULL;
}

const RandActivityShenShouEnZeBuyItemCfg * RandActivityShenShouEnZeConfig::GetRandActivityShenShouEnZeBuyItemCfg(RandActivityManager * ramgr, int reward_seq) const
{
	int act_real_open_day = LOGIC_CONFIG->GetRandActivityConfigManager()->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_SHEN_SHOU_EN_ZE);

	for (std::vector<RandActivityShenShouEnZeBuySectionCfg>::const_iterator it = m_buy_section_cfg.begin(); it != m_buy_section_cfg.end(); it++)
	{
		const RandActivityShenShouEnZeBuySectionCfg & curr = *it;
		if (curr.section_start <= act_real_open_day && act_real_open_day <= curr.section_end)
		{
			return MapValuesConstPtr(curr.item_list, reward_seq);
		}
	}

	return NULL;
}

int RandActivityShenShouEnZeConfig::GetConfigVer() const
{
	return m_other_cfg.cfg_ver;
}

int RandActivityShenShouEnZeConfig::InitOtherfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}

	if (!GetSubNodeValue(data_element, "consume_item", m_other_cfg.draw_need_item_id)) return -__LINE__;
	if (!GetSubNodeValue(data_element, "num", m_other_cfg.draw_need_item_num)) return -__LINE__;
	if (!GetSubNodeValue(data_element, "cfg_ver", m_other_cfg.cfg_ver)) return -__LINE__;

	return 0;
}

int RandActivityShenShouEnZeConfig::InitRandActivityShenShouEnZeCfg(TiXmlElement * RootElement)
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

				m_section_cfg.push_back(RandActivityShenShouEnZeSectionCfg());
			}
		}
		if (m_section_cfg.size() == 0)
		{
			if (section_start != 1)
			{
				return -11111;
			}
			m_section_cfg.push_back(RandActivityShenShouEnZeSectionCfg());
		}

		RandActivityShenShouEnZeSectionCfg & node_cfg = m_section_cfg[m_section_cfg.size() - 1];
		node_cfg.section_start = section_start;
		node_cfg.section_end = section_end;

		RandActivityShenShouEnZeCfg item_cfg;
		if (!GetSubNodeValue(data_element, "seq", item_cfg.seq)) return -__LINE__;
		if (!GetSubNodeValue(data_element, "hearsay", item_cfg.is_broadcast)) return -__LINE__;
		if (!GetSubNodeValue(data_element, "rate", item_cfg.rate)) return -__LINE__;
		if (ReadItemConfigData2(data_element, "reward_item", item_cfg.rewards) != 0) return -__LINE__;

		node_cfg.rate_count += item_cfg.rate;
		node_cfg.item_list.push_back(item_cfg);
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityShenShouEnZeConfig::InitRandActivityShenShouEnZeSelectRewardCfg(TiXmlElement * RootElement)
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

		std::vector<RandActivityShenShouEnZeSelectRewardSectionCfg> & section_cfg = m_times_reward_section_cfg;
		if (section_cfg.size() > 0)
		{
			int pre_index = section_cfg.size() - 1;
			if (section_start != section_cfg[pre_index].section_start ||
				section_end != section_cfg[pre_index].section_end)
			{
				if (section_start < section_cfg[pre_index].section_end)
				{
					return -888;
				}

				section_cfg.push_back(RandActivityShenShouEnZeSelectRewardSectionCfg());
			}
		}
		if (section_cfg.size() == 0)
		{
			if (section_start != 1)
			{
				return -11111;
			}
			section_cfg.push_back(RandActivityShenShouEnZeSelectRewardSectionCfg());
		}

		RandActivityShenShouEnZeSelectRewardSectionCfg & node_cfg = section_cfg.back();
		node_cfg.section_start = section_start;
		node_cfg.section_end = section_end;

		RandActivityShenShouEnZeSelectRewardCfg item_cfg;
		if (!GetSubNodeValue(data_element, "seq", item_cfg.seq)) return -__LINE__;
		if (!GetSubNodeValue(data_element, "hearsay", item_cfg.is_broadcast)) return -__LINE__;
		if (!GetSubNodeValue(data_element, "times", item_cfg.need_draw_times)) return -__LINE__;
		if (ReadItemConfigData2(data_element, "reward_item", item_cfg.rewards) != 0) return -__LINE__;

		node_cfg.item_list[item_cfg.seq] = item_cfg;
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityShenShouEnZeConfig::InitRandActivityShenShouEnZeBuyItemCfg(TiXmlElement * RootElement)
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

		std::vector<RandActivityShenShouEnZeBuySectionCfg> & section_cfg = m_buy_section_cfg;
		if (section_cfg.size() > 0)
		{
			int pre_index = section_cfg.size() - 1;
			if (section_start != section_cfg[pre_index].section_start ||
				section_end != section_cfg[pre_index].section_end)
			{
				if (section_start < section_cfg[pre_index].section_end)
				{
					return -888;
				}

				section_cfg.push_back(RandActivityShenShouEnZeBuySectionCfg());
			}
		}
		if (section_cfg.size() == 0)
		{
			if (section_start != 1)
			{
				return -11111;
			}
			section_cfg.push_back(RandActivityShenShouEnZeBuySectionCfg());
		}

		RandActivityShenShouEnZeBuySectionCfg & node_cfg = section_cfg.back();
		node_cfg.section_start = section_start;
		node_cfg.section_end = section_end;

		RandActivityShenShouEnZeBuyItemCfg item_cfg;
		if (!GetSubNodeValue(data_element, "seq", item_cfg.seq)) return -__LINE__;
		if (!GetSubNodeValue(data_element, "limit_type", item_cfg.limit_type)) return -__LINE__;

		if (item_cfg.limit_type == ACTVITY_BUY_LIMIT_TYPE_ALL)
		{
			if (!GetSubNodeValue(data_element, "limit_times", item_cfg.limit_buy_times)) return -__LINE__;
		}

		if (item_cfg.limit_type == ACTVITY_BUY_LIMIT_TYPE_DAY)
		{
			if (!GetSubNodeValue(data_element, "limit_times", item_cfg.limit_buy_times_day)) return -__LINE__;
		}

		if (!GetSubNodeValue(data_element, "cfg_or", item_cfg.is_need_chong_zhi)) return -__LINE__;

		if (item_cfg.is_need_chong_zhi)
		{
			if (!GetSubNodeValue(data_element, "price", item_cfg.need_chong_zhi_gold)) return -__LINE__;
		}

		if (!GetSubNodeValue(data_element, "price_type", item_cfg.money_type)) return -__LINE__;
		if (!GetSubNodeValue(data_element, "price", item_cfg.money_value)) return -__LINE__;


		if (ReadItemConfigData2(data_element, "reward_item", item_cfg.rewards) != 0) return -__LINE__;

		node_cfg.item_list[item_cfg.seq] = item_cfg;
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}
