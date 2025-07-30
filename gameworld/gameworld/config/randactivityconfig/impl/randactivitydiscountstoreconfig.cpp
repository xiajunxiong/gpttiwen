#include "randactivitydiscountstoreconfig.hpp"

#include "servercommon/activitydef.hpp"

RandActivityDiscountStoreConfig::RandActivityDiscountStoreConfig()
{

}

RandActivityDiscountStoreConfig::~RandActivityDiscountStoreConfig()
{

}

bool RandActivityDiscountStoreConfig::Init(TiXmlElement *RootElement, std::string *err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("open_discount_store", InitCfg);

	return true;
}

const RADiscountStoreCfg * RandActivityDiscountStoreConfig::GetDiscountStoreItemCfg(RandActivityManager* ramgr, int seq) const
{
	if (NULL == ramgr) return NULL;
	if (seq < 0 || seq >= RA_DISCOUNT_STORE_MAX_NUM) return NULL;

	int act_real_open_day = this->GetActOpenDay(ramgr, RAND_ACTIVITY_TYPE_DISCOUNT_STORE);
	for (std::vector<RADiscountStoreSectionCfg>::const_iterator it = m_discount_store_sections.begin(); it != m_discount_store_sections.end(); ++it)
	{
		if (act_real_open_day >= it->section_start && act_real_open_day <= it->section_end)
		{
			return &it->node_cfg[seq];
		}
	}
	return NULL;
}

const RADiscountStoreSectionCfg * RandActivityDiscountStoreConfig::GetDiscountStoreSectionCfg(RandActivityManager* ramgr) const
{
	if (NULL == ramgr) return NULL;

	int act_real_open_day = this->GetActOpenDay(ramgr, RAND_ACTIVITY_TYPE_DISCOUNT_STORE);
	for (std::vector<RADiscountStoreSectionCfg>::const_iterator it = m_discount_store_sections.begin(); it != m_discount_store_sections.end(); ++it)
	{
		if (act_real_open_day >= it->section_start && act_real_open_day <= it->section_end)
		{
			return &(*it);
		}
	}

	return NULL;
}

int RandActivityDiscountStoreConfig::InitCfg(TiXmlElement * RootElement)
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

		if (m_discount_store_sections.size() > 0)
		{
			int pre_index = m_discount_store_sections.size() - 1;
			if (section_start != m_discount_store_sections[pre_index].section_start ||
				section_end != m_discount_store_sections[pre_index].section_end)
			{
				if (section_start < m_discount_store_sections[pre_index].section_end)
				{
					return -888;
				}

				m_discount_store_sections.push_back(RADiscountStoreSectionCfg());
			}
		}


		if (m_discount_store_sections.size() == 0)
		{
			if (section_start != 1)
			{
				return -10101;
			}

			m_discount_store_sections.push_back(RADiscountStoreSectionCfg());
		}

		RADiscountStoreSectionCfg &section_cfg = m_discount_store_sections[m_discount_store_sections.size() - 1];

		section_cfg.section_start = section_start;
		section_cfg.section_end = section_end;

		if (section_cfg.count >= RA_DISCOUNT_STORE_MAX_NUM) return -101;

		int seq = 0;
		if (!GetSubNodeValue(data_element, "seq", seq) || seq >= RA_DISCOUNT_STORE_MAX_NUM) return -1;
		RADiscountStoreCfg &node_cfg = section_cfg.node_cfg[seq];
		node_cfg.seq = seq;

		TiXmlElement *item_element = data_element->FirstChildElement("item");
		if (!node_cfg.item.ReadConfig(item_element)) return -100;

		if (!GetSubNodeValue(data_element, "need_gold", node_cfg.need_gold)) return -2;
		if (!GetSubNodeValue(data_element, "buy_times", node_cfg.buy_times)) return -3;
		if (!GetSubNodeValue(data_element, "total_exchange", node_cfg.total_exchange)) return -4;

		section_cfg.count++;

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}
