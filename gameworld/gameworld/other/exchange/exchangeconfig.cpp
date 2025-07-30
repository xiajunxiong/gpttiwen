#include "exchangeconfig.hpp"
#include "item/itempool.h"

ExchangeConfig::ExchangeConfig()
{
}

ExchangeConfig::~ExchangeConfig()
{
}

bool ExchangeConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG("exchange_page", InitItemCfg);

	return true;
}

const ExchangeItemCfg * ExchangeConfig::GetItemCfg(int seq)
{
	std::map<int, ExchangeItemCfg>::const_iterator it = m_exchange_item_map.find(seq);
	if (it == m_exchange_item_map.end()) return NULL;
	return &it->second;
}

int ExchangeConfig::InitItemCfg(TiXmlElement * RootElement)
{
	if (NULL == RootElement) return -10000;
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -20000;

	int limit_buy_count = 0;
	while (dataElement)
	{
		ExchangeItemCfg node_cfg;
		if (!GetSubNodeValue(dataElement, "seq", node_cfg.seq) || node_cfg.seq < 0) return -1;
		if (!GetSubNodeValue(dataElement, "integral_type", node_cfg.score_type) || node_cfg.score_type < 0 || node_cfg.score_type >= SCORE_TYPE_MAX) return -2;
		if (!GetSubNodeValue(dataElement, "integral_price", node_cfg.score_price) || node_cfg.score_price < 0) return -3;
		if (!GetSubNodeValue(dataElement, "item_id", node_cfg.item_id) || NULL == ITEMPOOL->GetItem(node_cfg.item_id)) return -4;
		if (!GetSubNodeValue(dataElement, "is_bind", node_cfg.is_bind) || node_cfg.is_bind < 0 || node_cfg.is_bind > 1) return -11;
		if (!GetSubNodeValue(dataElement, "limit_type", node_cfg.limit_type) || node_cfg.limit_type < 0 || node_cfg.limit_type >= SHOP_BUY_LIMIT_TYPE_MAX) return -5;
		if (!GetSubNodeValue(dataElement, "limit_times", node_cfg.limit_times) || node_cfg.limit_times < 0) return -6;
		if (!GetSubNodeValue(dataElement, "open_time", node_cfg.open_time) ) return -7;
		if (!GetSubNodeValue(dataElement, "sale_time", node_cfg.sale_time) ) return -8;
		if (!GetSubNodeValue(dataElement, "price", node_cfg.price)) return -9;
		if (!GetSubNodeValue(dataElement, "money_type", node_cfg.money_type)) return -10;

		if (node_cfg.limit_type > 0)
		{
			limit_buy_count++;
		}

		m_exchange_item_map[node_cfg.seq] = node_cfg;
		dataElement = dataElement->NextSiblingElement();
	}

	if (limit_buy_count >= LIMIT_DATA_NUM) return -999;

	return 0;
}
