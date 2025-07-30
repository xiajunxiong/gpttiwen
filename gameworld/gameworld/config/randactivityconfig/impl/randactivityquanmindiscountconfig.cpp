#include "randactivityquanmindiscountconfig.hpp"

#include "config/randactivityconfig/randactivityconfigmanager.hpp"

RandActivityQuanMinDiscountConfig::RandActivityQuanMinDiscountConfig()
{

}

RandActivityQuanMinDiscountConfig::~RandActivityQuanMinDiscountConfig()
{
	std::vector<RAQuanMinDiscountSectionCfg>().swap(m_section_cfg);
	for (std::map<int, std::vector<RAQuanMinDiscountDiscountCfg> >::iterator it = m_discount_cfg.begin(); it != m_discount_cfg.end(); it++)
	{
		std::vector<RAQuanMinDiscountDiscountCfg>().swap(it->second);
	}
}

bool RandActivityQuanMinDiscountConfig::Init(TiXmlElement *RootElement, std::string *err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("group_buying", InitQuanMinDiscountSectionCfg);
	LOAD_RA_CONFIG("discount_form", InitQuanMinDiscountDiscountCfg);

	return true;
}

const RAQuanMinDiscountCfg * RandActivityQuanMinDiscountConfig::GetQuanMinDiscountCfg(RandActivityManager* ramgr, int seq) const
{
	int open_day = LOGIC_CONFIG->GetRandActivityConfigManager()->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_QUAN_MIN_DISCOUNT);

	for (std::vector<RAQuanMinDiscountSectionCfg>::const_iterator it = m_section_cfg.begin(); it != m_section_cfg.end(); ++it)
	{
		if (it->section_start <= open_day && it->section_end >= open_day)
		{
			for (std::vector<RAQuanMinDiscountCfg>::const_iterator vit = it->node.begin(); vit != it->node.end(); ++vit)
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

double RandActivityQuanMinDiscountConfig::GetDiscountCfg(int discount_id, int discount_num) const
{
	std::map<int, std::vector<RAQuanMinDiscountDiscountCfg> >::const_iterator it = m_discount_cfg.find(discount_id);
	if (m_discount_cfg.end() == it) return 10;

	double ret_discount = 10;

	for (std::vector<RAQuanMinDiscountDiscountCfg>::const_iterator vit = it->second.begin(); vit != it->second.end(); ++vit)
	{
		if (discount_num >= vit->discount_number)
		{
			ret_discount = vit->discount_proportion;
		}
		else
		{
			break;
		}
	}
	return ret_discount;
}

double RandActivityQuanMinDiscountConfig::GetMaxDiscountCfg(int discount_id) const
{
	std::map<int, std::vector<RAQuanMinDiscountDiscountCfg> >::const_iterator it = m_discount_cfg.find(discount_id);
	if (m_discount_cfg.end() == it)return -1;

	double ret_discount = 10;
	for (std::vector<RAQuanMinDiscountDiscountCfg>::const_iterator vit = it->second.begin(); vit != it->second.end(); ++vit)
	{
		if (ret_discount > vit->discount_proportion)
		{
			ret_discount = vit->discount_proportion;
		}
	}

	return ret_discount;
}

int RandActivityQuanMinDiscountConfig::GetMaxSeq(RandActivityManager* ramgr) const
{
	int open_day = LOGIC_CONFIG->GetRandActivityConfigManager()->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_QUAN_MIN_DISCOUNT);

	for (std::vector<RAQuanMinDiscountSectionCfg>::const_iterator it = m_section_cfg.begin(); it != m_section_cfg.end(); ++it)
	{
		if (it->section_start <= open_day && it->section_end >= open_day)
		{
			return (int)it->node.size();
		}
	}
	return QUAN_MIN_DISCOUNT_COMMODITY_MAX_NUM;
}

int RandActivityQuanMinDiscountConfig::GetMinOrMaxBuyNeedNum(int discount_id, bool is_max) const
{
	std::map<int, std::vector<RAQuanMinDiscountDiscountCfg> >::const_iterator it = m_discount_cfg.find(discount_id);
	if (m_discount_cfg.end() == it || it->second.empty()) return 1;

	int number = 0;
	if (is_max)
	{
		for (std::vector<RAQuanMinDiscountDiscountCfg>::const_reverse_iterator vit = it->second.rbegin(); vit != it->second.rend(); ++vit)
		{
			number = GetMax(number, vit->discount_number);
		}
	}
	else
	{
		number = MAX_INT8;
		for (std::vector<RAQuanMinDiscountDiscountCfg>::const_iterator vit = it->second.begin(); vit != it->second.end(); ++vit)
		{
			number = GetMin(number, vit->discount_number);
		}
	}

	if (number <= 0)
	{
		number = 1;
	}

	return number;
}

int RandActivityQuanMinDiscountConfig::InitQuanMinDiscountSectionCfg(TiXmlElement *RootElement)
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

				m_section_cfg.push_back(RAQuanMinDiscountSectionCfg());
			}
		}
		if (m_section_cfg.size() == 0)
		{
			if (section_start != 1)
			{
				return -11111;
			}
			m_section_cfg.push_back(RAQuanMinDiscountSectionCfg());
		}

		RAQuanMinDiscountSectionCfg & node_cfg = m_section_cfg[m_section_cfg.size() - 1];
		node_cfg.section_start = section_start;
		node_cfg.section_end = section_end;

		RAQuanMinDiscountCfg item_cfg;

		if (!GetSubNodeValue(data_element, "type", item_cfg.seq) || item_cfg.seq < 0) return -1;
		if (!GetSubNodeValue(data_element, "price_type", item_cfg.money_type)) return -2;

		TiXmlElement * item_element = data_element->FirstChildElement("reward_item");
		if (NULL != item_element)
		{
			if (!item_cfg.item.ReadConfig(item_element)) return -100;
		}

		if (!GetSubNodeValue(data_element, "iscount_id", item_cfg.discount_id) || item_cfg.discount_id <= 0) return -3;
		if (!GetSubNodeValue(data_element, "original_price", item_cfg.price)) return -4;
		if (!GetSubNodeValue(data_element, "hour", item_cfg.hour) || item_cfg.hour < 0) return -5;
		if (!GetSubNodeValue(data_element, "support_number", item_cfg.support_number) || item_cfg.support_number < 0) return -6;
		if (!GetSubNodeValue(data_element, "buy_tepy", item_cfg.buy_type) || item_cfg.buy_type <= 0) return -7;
		if (!GetSubNodeValue(data_element, "buy_seccond_day", item_cfg.buy_limit_day) || item_cfg.buy_limit_day < 0) return -8;
		if (!GetSubNodeValue(data_element, "buy_seccond", item_cfg.buy_limit_all) || item_cfg.buy_limit_all < 0) return -9;

		node_cfg.node.push_back(item_cfg);
		data_element = data_element->NextSiblingElement();
	}
	return 0;
}

int RandActivityQuanMinDiscountConfig::InitQuanMinDiscountDiscountCfg(TiXmlElement *RootElement)
{
	TiXmlElement *data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -999;
	}

	while (NULL != data_element)
	{
		RAQuanMinDiscountDiscountCfg discount_cfg;

		int seq = 0;
		if (!GetSubNodeValue(data_element, "seq", seq) || seq < 0) return -1;
		if (!GetSubNodeValue(data_element, "discount_proportion", discount_cfg.discount_proportion)) return -2;
		if (!GetSubNodeValue(data_element, "discount_number", discount_cfg.discount_number) || discount_cfg.discount_number < 0) return -3;

		discount_cfg.discount_proportion += 1e-6;

		m_discount_cfg[seq].push_back(discount_cfg);
		data_element = data_element->NextSiblingElement();
	}
	return 0;
}
