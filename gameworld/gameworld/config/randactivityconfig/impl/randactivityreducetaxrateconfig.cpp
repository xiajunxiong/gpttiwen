#include "randactivityreducetaxrateconfig.hpp"

#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "servercommon/struct/global/trademarketparam.hpp"

RandActivityReduceTaxRateConfig::RandActivityReduceTaxRateConfig()
{

}

RandActivityReduceTaxRateConfig::~RandActivityReduceTaxRateConfig()
{
	std::vector<RAReduceTaxRateSectionCfg>().swap(m_reduce_tax_rate_section_cfg);
}

bool RandActivityReduceTaxRateConfig::Init(TiXmlElement *RootElement, std::string *err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("tax_rate_discount", InitRAReduceTexRateSectionCfg);

	return true;
}

const int RandActivityReduceTaxRateConfig::GetRateFromReduceTaxRateSectionCfg(RandActivityManager* ramgr)  const
{
	int open_day = LOGIC_CONFIG->GetRandActivityConfigManager()->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_REDUCE_TAX_RATE);

	for (std::vector<RAReduceTaxRateSectionCfg>::const_iterator it = m_reduce_tax_rate_section_cfg.begin(); it != m_reduce_tax_rate_section_cfg.end(); ++it)
	{
		if (it->section_start <= open_day && it->section_end >= open_day)
		{
			return it->tax_rate;
		}
	}

	return 0;
}

int RandActivityReduceTaxRateConfig::InitRAReduceTexRateSectionCfg(TiXmlElement *RootElement)
{
	if (NULL == RootElement)
	{
		return -10000;
	}

	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -20000;
	}
	while (NULL != data_element)
	{
		bool has_section_cfg = true;
		RAReduceTaxRateSectionCfg  node_cfg;

		if (!GetSubNodeValue(data_element, "section_start", node_cfg.section_start))
		{
			has_section_cfg = false;
		}
		if (!GetSubNodeValue(data_element, "section_end", node_cfg.section_end))
		{
			has_section_cfg = false;
		}

		if (!has_section_cfg) // 没配置，默认给一个无限区间
		{
			node_cfg.section_start = 1;
			node_cfg.section_end = INT_MAX;
		}

		if (0 == node_cfg.section_end) // 0 代表无穷
		{
			node_cfg.section_end = INT_MAX;
		}

		if (!GetSubNodeValue(data_element, "reduce_tax_rate", node_cfg.tax_rate) || node_cfg.tax_rate < 0 || node_cfg.tax_rate > TRADE_MARKET_PERCENT_NUM)
		{
			return -1;
		}

		if (m_reduce_tax_rate_section_cfg.size() > 0)
		{
			int index = m_reduce_tax_rate_section_cfg.size() - 1;
			if (node_cfg.section_start != m_reduce_tax_rate_section_cfg[index].section_start ||
				node_cfg.section_end != m_reduce_tax_rate_section_cfg[index].section_end)
			{
				if (node_cfg.section_start < m_reduce_tax_rate_section_cfg[index].section_end)
				{
					return -888;
				}

				m_reduce_tax_rate_section_cfg.push_back(node_cfg);
			}
		}
		if (m_reduce_tax_rate_section_cfg.size() == 0)
		{
			if (node_cfg.section_start != 1)
			{
				return -11111;
			}
			m_reduce_tax_rate_section_cfg.push_back(node_cfg);
		}

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

