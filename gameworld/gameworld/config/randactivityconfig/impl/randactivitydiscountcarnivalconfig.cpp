#include "randactivitydiscountcarnivalconfig.hpp"

#include "config/randactivityconfig/randactivityconfigmanager.hpp"

RandActivityDiscountCarnivalConfig::RandActivityDiscountCarnivalConfig()
{

}

RandActivityDiscountCarnivalConfig::~RandActivityDiscountCarnivalConfig()
{
	std::vector<RADiscountCarnivalCouponCfg>().swap(m_coupon_cfg);
	std::vector<RADiscountCarnivalSectionCfg>().swap(m_discount_carnival_section_cfg);
}

bool RandActivityDiscountCarnivalConfig::Init(TiXmlElement *RootElement, std::string *err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("reward", InitDiscountCarnivalSectionCfg);
	LOAD_RA_CONFIG("coupon", InitDiscountCarnivalCouponCfg);

	return true;
}

const RADiscountCarnivalCfg * RandActivityDiscountCarnivalConfig::GetDiscountCarnivalCfg(RandActivityManager* ramgr, int seq) const
{
	if (NULL == ramgr)
	{
		return NULL;
	}

	int open_day = LOGIC_CONFIG->GetRandActivityConfigManager()->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_DISCOUNT_CARNIVAL);

	for (std::vector<RADiscountCarnivalSectionCfg>::const_iterator it = m_discount_carnival_section_cfg.begin(); it != m_discount_carnival_section_cfg.end(); ++it)
	{
		if (it->section_start <= open_day && it->section_end >= open_day)
		{
			for (std::vector<RADiscountCarnivalCfg>::const_iterator vit = it->node.begin(); vit != it->node.end(); ++vit)
			{
				if (vit->seq == seq)
				{
					return &(*vit);
				}
			}
		}
	}
	return NULL;
}

const int RandActivityDiscountCarnivalConfig::GetCouponCfg(int item_id) const
{
	for (std::vector<RADiscountCarnivalCouponCfg>::const_iterator it = m_coupon_cfg.begin(); it != m_coupon_cfg.end(); ++it)
	{
		if (item_id == it->item_id)
		{
			return it->discount;
		}
	}
	return -1;
}

int RandActivityDiscountCarnivalConfig::InitDiscountCarnivalSectionCfg(TiXmlElement *RootElement)
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

		if (m_discount_carnival_section_cfg.size() > 0)
		{
			int pre_index = m_discount_carnival_section_cfg.size() - 1;
			if (section_start != m_discount_carnival_section_cfg[pre_index].section_start ||
				section_end != m_discount_carnival_section_cfg[pre_index].section_end)
			{
				if (section_start < m_discount_carnival_section_cfg[pre_index].section_end)
				{
					return -888;
				}

				m_discount_carnival_section_cfg.push_back(RADiscountCarnivalSectionCfg());
			}
		}
		if (m_discount_carnival_section_cfg.size() == 0)
		{
			if (section_start != 1)
			{
				return -11111;
			}
			m_discount_carnival_section_cfg.push_back(RADiscountCarnivalSectionCfg());
		}

		RADiscountCarnivalSectionCfg & node_cfg = m_discount_carnival_section_cfg[m_discount_carnival_section_cfg.size() - 1];
		node_cfg.section_start = section_start;
		node_cfg.section_end = section_end;

		RADiscountCarnivalCfg item_cfg;

		if (!GetSubNodeValue(data_element, "seq1", item_cfg.seq) || item_cfg.seq < 0) return -1;

		TiXmlElement * item_element = data_element->FirstChildElement("reward_item");
		if (NULL != item_element)
		{
			if (!item_cfg.item.ReadConfig(item_element)) return -100;
		}

		if (!GetSubNodeValue(data_element, "buy_times", item_cfg.buy_limit_all) || item_cfg.buy_limit_all < 0) return -2;
		if (!GetSubNodeValue(data_element, "price_type", item_cfg.price_type)) return -3;
		if (!GetSubNodeValue(data_element, "original_price", item_cfg.price)) return -4;
		if (!GetSubNodeValue(data_element, "buy_tepy", item_cfg.buy_type)) return -5;
		if (!GetSubNodeValue(data_element, "buy_seccond_day", item_cfg.buy_limit_day)) return -5;

		node_cfg.node.push_back(item_cfg);
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityDiscountCarnivalConfig::InitDiscountCarnivalCouponCfg(TiXmlElement *RootElement)
{
	TiXmlElement *data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -999;
	}

	while (NULL != data_element)
	{
		RADiscountCarnivalCouponCfg cfg;

		if (!GetSubNodeValue(data_element, "coupon_id", cfg.item_id) || NULL == ITEMPOOL->GetItem(cfg.item_id)) return -1;
		if (!GetSubNodeValue(data_element, "discount", cfg.discount) || cfg.discount <= 0 || cfg.discount > 9) return -2;

		m_coupon_cfg.push_back(cfg);
		data_element = data_element->NextSiblingElement();
	}
	return 0;
}
