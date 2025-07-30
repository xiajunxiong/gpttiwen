#include "randactivitytrademarketreturngoldconfig.hpp"

RandActivityTradeMarketReturnGoldConfig::RandActivityTradeMarketReturnGoldConfig() : m_section_count(0)
{

}

RandActivityTradeMarketReturnGoldConfig::~RandActivityTradeMarketReturnGoldConfig()
{

}

bool RandActivityTradeMarketReturnGoldConfig::Init(TiXmlElement *RootElement, std::string *err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("deal_discount", InitReturnGoldCfg);

	return true;
}

double RandActivityTradeMarketReturnGoldConfig::GetReturnGoldNum(RandActivityManager* ramgr, int consume_gold) const
{
	if (NULL == ramgr) return 0;

	int act_real_open_day = this->GetActOpenDay(ramgr, RAND_ACTIVITY_TYPE_TRADEMARKET_GOLD_RETURN);
	for (int i = 0; i < m_section_count && i < ARRAY_LENGTH(m_return_gold_section_cfg_list); ++i)
	{
		if (act_real_open_day >= m_return_gold_section_cfg_list[i].section_start &&
			act_real_open_day < m_return_gold_section_cfg_list[i].section_end)
		{
			double temp = 1.0 * consume_gold * m_return_gold_section_cfg_list[i].return_gold_percent / 100 + 1e-6;
			return temp;
		}
	}

	return 0;
}

int RandActivityTradeMarketReturnGoldConfig::InitReturnGoldCfg(TiXmlElement * RootElement)
{
	TiXmlElement* dataElement = RootElement->FirstChildElement("data");

	m_section_count = 0;
	while (dataElement)
	{
		int section_start = 0;
		if (!GetSubNodeValue(dataElement, "section_start", section_start) || section_start <= 0)
		{
			return -1;
		}

		int section_end = 0;
		if (!GetSubNodeValue(dataElement, "section_end", section_end) || section_end < 0 || (section_end > 0 && section_end <= section_start))
		{
			return -2;
		}

		int reduce_discount = 0;
		if (!GetSubNodeValue(dataElement, "reduce_discount", reduce_discount) || reduce_discount <= 0 || reduce_discount > 80) // 不太可能定到比80高 防止运营填错数值
		{
			return -3;
		}

		RATradeMarketReturnGoldSectionCfg cfg;
		cfg.section_start = section_start;
		cfg.section_end = section_end;
		cfg.return_gold_percent = reduce_discount;

		m_return_gold_section_cfg_list[m_section_count++] = cfg;

		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}
