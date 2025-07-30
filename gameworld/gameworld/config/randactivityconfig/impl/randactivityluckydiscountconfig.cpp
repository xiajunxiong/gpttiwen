#include "randactivityluckydiscountconfig.hpp"

RandActivityLuckyDiscountConfig::RandActivityLuckyDiscountConfig()
	:m_all_weight(0)
{

}

RandActivityLuckyDiscountConfig::~RandActivityLuckyDiscountConfig()
{
	std::vector<RALuckyDiscountDiscountCfg>().swap(m_discount_cfg);
	std::vector<RALuckyDiscountRewardSectionCfg>().swap(m_reward_section_cfg);
}

bool RandActivityLuckyDiscountConfig::Init(TiXmlElement *RootElement, std::string *err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("discount", InitRALuckyDiscountDiscoutnCfg);
	LOAD_RA_CONFIG("reward", InitRALuckyDiscountRewardCfg);

	return true;
}

const int RandActivityLuckyDiscountConfig::GetDiscountCfg() const
{
	if (0 >= m_all_weight) return 0;

	int rand_weight = rand() % m_all_weight;
	for (std::vector<RALuckyDiscountDiscountCfg>::const_iterator it = m_discount_cfg.begin(); it != m_discount_cfg.end(); ++it)
	{
		if (it->weight > rand_weight)
		{
			return it->proportion;
		}
		rand_weight -= it->weight;
	}

	return 0;
}

const RALuckyDiscountRewardCfg * RandActivityLuckyDiscountConfig::GetRewardCfg(int seq, RandActivityManager * ramgr) const
{
	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_LUCKY_DISCOUNT);
	for (std::vector<RALuckyDiscountRewardSectionCfg>::const_iterator it = m_reward_section_cfg.begin(); m_reward_section_cfg.end() != it; ++it)
	{
		if (it->section_end >= act_real_open_day && act_real_open_day >= it->section_start)
		{
			if (0 > seq || (int)it->reward_cfg.size() <= seq) return NULL;

			return &it->reward_cfg[seq];
		}
	}
	return NULL;
}

int RandActivityLuckyDiscountConfig::InitRALuckyDiscountDiscoutnCfg(TiXmlElement *RootElement)
{
	TiXmlElement *data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -999;
	}

	while (NULL != data_element)
	{
		RALuckyDiscountDiscountCfg discount_cfg;

		if (!GetSubNodeValue(data_element, "seq", discount_cfg.seq) || discount_cfg.seq < 0) return -1;
		if (!GetSubNodeValue(data_element, "proportion", discount_cfg.proportion)) return -2;
		if (!GetSubNodeValue(data_element, "weight", discount_cfg.weight) || discount_cfg.weight < 0) return -3;
		m_all_weight += discount_cfg.weight;

		m_discount_cfg.push_back(discount_cfg);
		data_element = data_element->NextSiblingElement();
	}
	return 0;
}

int RandActivityLuckyDiscountConfig::InitRALuckyDiscountRewardCfg(TiXmlElement *RootElement)
{
	TiXmlElement *data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -999;
	}

	int last_section = 0;
	while (NULL != data_element)
	{
		int section_start = 0, section_end = 0;
		if (!GetSubNodeValue(data_element, "section_start", section_start) || section_start <= 0) return -999;
		if (!GetSubNodeValue(data_element, "section_end", section_end) || section_end < 0) return -998;
		if (0 == section_end)
		{
			section_end = MAX_INT;
		}

		if (section_end < section_start) return -997;

		if (section_start != last_section)
		{
			m_reward_section_cfg.push_back(RALuckyDiscountRewardSectionCfg(section_start, section_end));
			last_section = section_start;
		}
		
		RALuckyDiscountRewardCfg reward_cfg;

		if (!GetSubNodeValue(data_element, "seq", reward_cfg.seq) || reward_cfg.seq != (int)m_reward_section_cfg.rbegin()->reward_cfg.size()) return -1;
		if (reward_cfg.seq >= LUCKY_DISCOUNT_COMMODITY_MAX_NUM)return -998;
		if (!GetSubNodeValue(data_element, "type", reward_cfg.money_type)) return -2;
		if (!GetSubNodeValue(data_element, "original_price", reward_cfg.price) || reward_cfg.price < 0) return -3;
		if (!GetSubNodeValue(data_element, "frequency", reward_cfg.limit_count) || reward_cfg.limit_count < 0 || reward_cfg.limit_count >= MAX_INT8) return -4;
		if (!GetSubNodeValue(data_element, "limit_type", reward_cfg.limit_type) || reward_cfg.limit_type <= RALuckyDiscountRewardCfg::LIMIT_TYPE_TOTAL_MIN || reward_cfg.limit_type >= RALuckyDiscountRewardCfg::LIMIT_TYPE_TOTAL_MAX) return -5;

		TiXmlElement * item_element = data_element->FirstChildElement("reward_item");
		if (NULL != item_element)
		{
			if (!reward_cfg.item.ReadConfig(item_element)) return -100;
		}

		m_reward_section_cfg.rbegin()->reward_cfg.push_back(reward_cfg);
		data_element = data_element->NextSiblingElement();
	}
	return 0;
}
