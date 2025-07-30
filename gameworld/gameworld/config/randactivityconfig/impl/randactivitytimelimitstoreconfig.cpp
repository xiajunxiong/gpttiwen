#include "randactivitytimelimitstoreconfig.hpp"

#include "servercommon/activitydef.hpp"

RandActivityTimeLimitStoreConfig::RandActivityTimeLimitStoreConfig()
{

}

RandActivityTimeLimitStoreConfig::~RandActivityTimeLimitStoreConfig()
{

}

bool RandActivityTimeLimitStoreConfig::Init(TiXmlElement *RootElement, std::string *err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("time_limited_buy", InitCfg);

	return true;
}

const TimeLimitStoreCfg* RandActivityTimeLimitStoreConfig::GetTimeLimitStoreItemCfg(RandActivityManager* ramgr, int seq) const
{
	if (NULL == ramgr) return NULL;
	if (seq < 0 || seq >= RA_TIME_LIMIT_STORE_MAX_NUM) return NULL;

	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_TIME_LIMIT_STORE);
	for (std::vector<TimeLimitStoreSectionCfg>::const_iterator it = m_time_limit_store_sections.begin(); it != m_time_limit_store_sections.end(); ++it)
	{
		if (act_real_open_day >= it->section_start && act_real_open_day <= it->section_end)
		{
			return &it->node_cfg[seq];
		}
	}
	return NULL;
}

int RandActivityTimeLimitStoreConfig::InitCfg(TiXmlElement * RootElement)
{
	if (NULL == RootElement)
	{
		return -10000;
	}

	TiXmlElement *data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -20000;
	}

	while (NULL != data_element)
	{
		bool has_section_cfg = true;

		int section_start = 0;
		if (!GetSubNodeValue(data_element, "section_start", section_start))
		{
			has_section_cfg = false;
		}

		int section_end = 0;
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

		if (m_time_limit_store_sections.size() > 0)
		{
			int pre_index = m_time_limit_store_sections.size() - 1;
			if (section_start != m_time_limit_store_sections[pre_index].section_start ||
				section_end != m_time_limit_store_sections[pre_index].section_end)
			{
				if (section_start < m_time_limit_store_sections[pre_index].section_end)
				{
					return -888;
				}

				m_time_limit_store_sections.push_back(TimeLimitStoreSectionCfg());
			}
		}


		if (m_time_limit_store_sections.size() == 0)
		{
			if (section_start != 1)
			{
				return -10101;
			}

			m_time_limit_store_sections.push_back(TimeLimitStoreSectionCfg());
		}

		TimeLimitStoreSectionCfg & section_cfg = m_time_limit_store_sections[m_time_limit_store_sections.size() - 1];

		section_cfg.section_start = section_start;
		section_cfg.section_end = section_end;

		int seq = 0;
		if (!GetSubNodeValue(data_element, "seq", seq)) return -1;
		TimeLimitStoreCfg& node_cfg = section_cfg.node_cfg[seq];
		node_cfg.seq = seq;

		TiXmlElement *item_element = data_element->FirstChildElement("reward_item");
		if (!node_cfg.item.ReadConfig(item_element)) return -100;

		if (!GetSubNodeValue(data_element, "limit_times", node_cfg.limit_times)) return -2;
		if (!GetSubNodeValue(data_element, "money_type", node_cfg.money_type)) return -3;
		if (!GetSubNodeValue(data_element, "price", node_cfg.price)) return -4;

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}
