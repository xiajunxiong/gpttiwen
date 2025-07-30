#include "randactivitysaleshopconfig.hpp"

#include "servercommon/roleactivitydef.hpp"
#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"

#include "servercommon/struct/moneyotherdef.h"

RandActivitySaleShopConfig::RandActivitySaleShopConfig()
{

}

RandActivitySaleShopConfig::~RandActivitySaleShopConfig()
{
	std::vector<RASaleShopSectionCfg>().swap(m_sale_shop_section_cfg);
}

bool RandActivitySaleShopConfig::Init(TiXmlElement *RootElement, std::string *err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("sale_shop", InitCfg);

	return true;
}

const RASaleShopCfg* RandActivitySaleShopConfig::GetSaleShopItemCfg(RandActivityManager* ramgr, int item_type, int item_seq)const
{
	if (item_type < SALE_SHOP_ITEM_TYPE_RECOMMEND || item_type >= SALE_SHOP_ITEM_TYPE_MAX || item_seq < 0 || item_seq >= MAX_SALE_SHOP_TYPE_SEQ_NUM) return NULL;

	int act_real_open_day = LOGIC_CONFIG->GetRandActivityConfigManager()->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_SALE_SHOP);
	for (std::vector<RASaleShopSectionCfg>::const_iterator it = m_sale_shop_section_cfg.begin(); it != m_sale_shop_section_cfg.end(); it++)
	{
		if (it->section_start <= act_real_open_day && it->section_end >= act_real_open_day)
		{
			if ((int)it->node_cfg[item_type].item_list.size() <= item_seq) break;

			return &it->node_cfg[item_type].item_list[item_seq];
		}
	}
	return NULL;
}

int RandActivitySaleShopConfig::GetSaleShopTypeItemCount(RandActivityManager* ramgr, int item_type)const
{
	if (item_type < SALE_SHOP_ITEM_TYPE_RECOMMEND || item_type >= SALE_SHOP_ITEM_TYPE_MAX) return 0;

	int act_real_open_day = LOGIC_CONFIG->GetRandActivityConfigManager()->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_SALE_SHOP);
	for (std::vector<RASaleShopSectionCfg>::const_iterator it = m_sale_shop_section_cfg.begin(); it != m_sale_shop_section_cfg.end(); it++)
	{
		if (it->section_start <= act_real_open_day && it->section_end >= act_real_open_day)
		{
			return (int)it->node_cfg[item_type].item_list.size();
		}
	}
	return 0;
}

const RASaleShopSectionCfg* RandActivitySaleShopConfig::GetSaleShopSectionCfg(RandActivityManager* ramgr) const
{
	int act_real_open_day = LOGIC_CONFIG->GetRandActivityConfigManager()->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_SALE_SHOP);
	for (std::vector<RASaleShopSectionCfg>::const_iterator it = m_sale_shop_section_cfg.begin(); it != m_sale_shop_section_cfg.end(); it++)
	{
		if (it->section_start <= act_real_open_day && it->section_end >= act_real_open_day)
		{
			return &(*it);
		}
	}
	return NULL;
}

int RandActivitySaleShopConfig::InitCfg(TiXmlElement *RootElement)
{
	TiXmlElement *data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -999;
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

		if (m_sale_shop_section_cfg.size() > 0)
		{
			int pre_index = m_sale_shop_section_cfg.size() - 1;
			if (section_start != m_sale_shop_section_cfg[pre_index].section_start ||
				section_end != m_sale_shop_section_cfg[pre_index].section_end)
			{
				if (section_start < m_sale_shop_section_cfg[pre_index].section_end)
				{
					return -888;
				}

				m_sale_shop_section_cfg.push_back(RASaleShopSectionCfg());
			}
		}
		if (m_sale_shop_section_cfg.size() == 0)
		{
			if (section_start != 1)
			{
				return -11111;
			}
			m_sale_shop_section_cfg.push_back(RASaleShopSectionCfg());
		}

		RASaleShopSectionCfg & node_cfg = m_sale_shop_section_cfg[m_sale_shop_section_cfg.size() - 1];
		node_cfg.section_start = section_start;
		node_cfg.section_end = section_end;

		int type = 0;
		if (!GetSubNodeValue(data_element, "type", type) || type < SALE_SHOP_ITEM_TYPE_RECOMMEND || type >= SALE_SHOP_ITEM_TYPE_MAX) return -1;

		RASaleShopSectionCfg::RASaleShopTypeCfg & type_cfg = node_cfg.node_cfg[type];
		type_cfg.item_type = type;

		int item_seq = 0;
		if (!GetSubNodeValue(data_element, "item_seq", item_seq) || item_seq < 0 || item_seq >= MAX_SALE_SHOP_TYPE_SEQ_NUM) return -2;

		RASaleShopCfg shop_cfg;
		shop_cfg.item_seq = item_seq;

		TiXmlElement* item_element = data_element->FirstChildElement("reward_item");
		if (NULL != item_element)
		{
			if (!shop_cfg.item.ReadConfig(item_element)) return -100;
		}

		if (!GetSubNodeValue(data_element, "price_type", shop_cfg.price_type) || shop_cfg.price_type < MONEY_TYPE_GOLD || shop_cfg.price_type >= MONEY_TYPE_MAX) return -3;
		if (!GetSubNodeValue(data_element, "price", shop_cfg.price) || shop_cfg.price < 0) return -4;
		if (!GetSubNodeValue(data_element, "buy_times", shop_cfg.buy_times) || shop_cfg.buy_times < 0) return -5;
		if (!GetSubNodeValue(data_element, "today_times", shop_cfg.today_times) || shop_cfg.today_times < 0) return -6;
		if (!GetSubNodeValue(data_element, "limit_type", shop_cfg.limit_type) || shop_cfg.limit_type <= SALE_SHOP_LIMIT_TYPE_BEGIN
			|| shop_cfg.limit_type >= SALE_SHOP_LIMIT_TYPE_MAX) return -7;

		type_cfg.item_list.push_back(shop_cfg);
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

