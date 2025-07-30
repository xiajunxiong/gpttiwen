#include "loversmarketconfig.hpp"

#include "item/itempool.h"

LoversMarketConfig::LoversMarketConfig()
{
}

LoversMarketConfig::~LoversMarketConfig()
{
	std::vector<LoversMarketStoreCfg>().swap(m_store_cfg);
}

bool LoversMarketConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG_2("other", InitOtherCfg);
	LOAD_CONFIG_2("store_config", InitStoreCfg);

	return true;
}

const LoversMarketStoreCfg * LoversMarketConfig::GetStoreCfg(int seq)
{
	if (seq < 0 || seq >= (int)m_store_cfg.size())
		return NULL;
	return &m_store_cfg[seq];
}

int LoversMarketConfig::InitOtherCfg(TiXmlElement * RootElement, std::string & errormsg)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	if (!GetSubNodeValue(root_element, "play_coin", m_other_cfg.play_coin) || NULL == ITEMPOOL->GetItem(m_other_cfg.play_coin))
	{
		errormsg = STRING_SPRINTF("[play_coin=%d] error, NULL == ITEMPOOL->GetItem ?", m_other_cfg.play_coin);
		return -1;
	}

	return 0;
}

int LoversMarketConfig::InitStoreCfg(TiXmlElement * RootElement, std::string & errormsg)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		LoversMarketStoreCfg cfg;
		if (!GetSubNodeValue(root_element, "item_id", cfg.item_id) || NULL == ITEMPOOL->GetItem(cfg.item_id))
		{
			errormsg = STRING_SPRINTF("[item_id=%d] error, NULL == ITEMPOOL->GetItem ?", cfg.item_id);
			return -1;
		}

		int is_bind = 0;
		if (!GetSubNodeValue(root_element, "is_bind", is_bind))
		{
			errormsg = STRING_SPRINTF("[is_bind=%d] error, not is_bind ?", is_bind);
			return -2;
		}

		cfg.is_bind = 0 != is_bind;
		
		if (!GetSubNodeValue(root_element, "num", cfg.num) || cfg.num <= 0)
		{
			errormsg = STRING_SPRINTF("[num=%d] error, num <= 0 ?", cfg.num);
			return -3;
		}

		if (!GetSubNodeValue(root_element, "price", cfg.price) || cfg.price <= 0)
		{
			errormsg = STRING_SPRINTF("[price=%d] error, price <= 0 ?", cfg.price);
			return -4;
		}

		if (!GetSubNodeValue(root_element, "limit_convert_count", cfg.limit_convert_count) || cfg.limit_convert_count < 0 || cfg.limit_convert_count > MAX_LOVERS_MARKET_LIMIT_COUNT)
		{
			errormsg = STRING_SPRINTF("[limit_convert_count=%d] error, limit_convert_count < 0 || limit_convert_count > %d ?", cfg.limit_convert_count, MAX_LOVERS_MARKET_LIMIT_COUNT);
			return -5;
		}

		m_store_cfg.push_back(cfg);
		root_element = root_element->NextSiblingElement();
	}

	if ((int)m_store_cfg.size() > MAX_LOVERS_MARKET_STORE_COUNT)
	{
		errormsg = STRING_SPRINTF("[store_num=%d] error, store_num > %d ?", m_store_cfg.size(), MAX_LOVERS_MARKET_STORE_COUNT);
		return -6;
	}

	return 0;
}
