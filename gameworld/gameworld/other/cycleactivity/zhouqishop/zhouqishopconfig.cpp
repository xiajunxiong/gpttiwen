#include "item/itempool.h"

#include "servercommon/cycleactivity/zhouqishopparam.hpp"
#include "zhouqishopconfig.hpp"


ZhouQiShopConfig::ZhouQiShopConfig()
{
}

ZhouQiShopConfig::~ZhouQiShopConfig()
{
}

bool ZhouQiShopConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG_2("store_config", InitStoreCfg);

	return true;
}

const ZhouQiShopItemCfg * ZhouQiShopConfig::GetZhouQiShopItemCfg(int zhou_qi_num, int item_seq)
{
	const ZhouQiShopCfg * shop_cfg = MapValuesConstPtr(m_shop_cfg, zhou_qi_num);
	if (!shop_cfg)
	{
		return NULL;
	}

	return MapValuesConstPtr(shop_cfg->item_list, item_seq);
}

int ZhouQiShopConfig::InitStoreCfg(TiXmlElement * RootElement, std::string & errormsg)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	ZhouQiShopCfg & shop_cfg = m_shop_cfg[3];

	while (NULL != root_element)
	{
		ZhouQiShopItemCfg cfg;

		if (!GetSubNodeValue(root_element, "seq", cfg.seq) || cfg.seq >= ZHOU_QI_SHOP_ITEM_MAX_NUM)
		{
			ret_line;
		}

		if (ReadItemConfigData2(root_element, "reward", cfg.rewards) != 0)
		{		
			ret_line;
		}

		if (!GetSubNodeValue(root_element, "conver_item_id", cfg.consume_item_id))
		{
			ret_line;
		}

		if (!GetSubNodeValue(root_element, "price", cfg.consume_item_num) || cfg.consume_item_num <= 0)
		{
			errormsg = STRING_SPRINTF("[price=%d] error, price <= 0 ?", cfg.consume_item_num);
			ret_line;
		}

		if (!GetSubNodeValue(root_element, "price", cfg.consume_item_num) || cfg.consume_item_num <= 0)
		{
			errormsg = STRING_SPRINTF("[price=%d] error, price <= 0 ?", cfg.consume_item_num);
			ret_line;
		}

		if (!GetSubNodeValue(root_element, "limit_convert_count", cfg.limit_buy_num) || cfg.limit_buy_num < 0 || cfg.limit_buy_num > ZHOU_QI_SHOP_ITEM_LIMIT_BUY_NUM)
		{
			errormsg = STRING_SPRINTF("[limit_convert_count=%d] error, limit_convert_count < 0 || limit_convert_count > %d ?", cfg.limit_buy_num, ZHOU_QI_SHOP_ITEM_LIMIT_BUY_NUM);
			ret_line;
		}

		shop_cfg.item_list[cfg.seq] = cfg;
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

