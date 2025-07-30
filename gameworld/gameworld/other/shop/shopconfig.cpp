#include "shopconfig.h"
#include "servercommon/configcommon.h"
#include "item/itempool.h"
#include "item/money.h"
#include "servercommon/flyupsystemdef.h"

ShopConfig::ShopConfig()
{
	memset(m_convert_type_item_num_list, 0, sizeof(m_convert_type_item_num_list));
}

ShopConfig::~ShopConfig()
{
}

bool ShopConfig::Init(const std::string &configname, std::string *err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG("shop_npc", InitShopNpcInfoCfg);
	LOAD_CONFIG("shop", InitShopItemInfoCfg);
	LOAD_CONFIG("fast_buy", InitShopFastBuyCfg);
	LOAD_CONFIG("gold_shop", InitGoldShopBuyCfg);
	LOAD_CONFIG("department_store", InitConvertShopCfg);
	LOAD_CONFIG("mourinho", InitNewShopItemInfoCfg);
	LOAD_CONFIG("market", InitMarketItemCfg);
	LOAD_CONFIG("market_other", InitMarketOtherCfg);
	LOAD_CONFIG("other", InitShopOtherCfg);

	return true;
}

const ShopItemCfg * ShopConfig::GetShopItemCfg(int index)
{
	std::map<int, ShopItemCfg>::const_iterator it = m_shop_item_map.find(index);
	if (it == m_shop_item_map.end()) return NULL;
	return &it->second;
}

const GoldShopItemCfg * ShopConfig::GetGoldShopItemCfg(int index)
{
	std::map<int, GoldShopItemCfg>::const_iterator it = m_gold_shop_item_map.find(index);
	if (it == m_gold_shop_item_map.end()) return NULL;
	return &it->second;
}

const FastBuyContainer * ShopConfig::GetFastBuyCfg(int item_id)
{
	std::map<int, FastBuyContainer>::iterator f_it = m_fast_buy_cfg.find(item_id);
	if (m_fast_buy_cfg.end() != f_it) return & f_it->second;

	return NULL;
}

const ConvertShopItemCfg* ShopConfig::GetConvertShopItemCfg(int convert_type, int seq)
{
	if (convert_type < 0 || convert_type >= CONVERT_TYPE_MAX) return NULL;
	if (seq < 0 || seq >= CONVERT_TYPE_MAX_ITEM_COUNT) return NULL;
	if (!m_convert_shop_cfg_list[convert_type][seq].IsValid()) return NULL;

	return &m_convert_shop_cfg_list[convert_type][seq];
}

const ConvertShopItemCfg* ShopConfig::GetConvertShopItemCfgByItemID(int convert_type, ItemID item_id)
{
	if (convert_type < 0 || convert_type >= CONVERT_TYPE_MAX) return NULL;

	for (int seq = 0; seq < CONVERT_TYPE_MAX_ITEM_COUNT; ++seq)
	{
		if (!m_convert_shop_cfg_list[convert_type][seq].IsValid()) return NULL;
	
		if (m_convert_shop_cfg_list[convert_type][seq].item.item_id == item_id)
		{
			return &m_convert_shop_cfg_list[convert_type][seq];
		}
	}

	return NULL;
}

const NewShopItemCfg* ShopConfig::GetNewShopItemCfg(int index)
{
	if (index < 0 || index >= m_new_shop_item_num || index >= MAX_NEW_SHOP_ITEM_NUM) return NULL;

	return &m_new_shop_item_list[index];
}

int ShopConfig::GetConvertTypeItemNum(int convert_type)
{
	if (convert_type < 0 || convert_type >= CONVERT_TYPE_MAX) return 0;

	return m_convert_type_item_num_list[convert_type];
}

void ShopConfig::ResetBuyCount(int type, ShopParam * param)
{
	if (NULL == param) return;
	std::map<int, std::vector<int> >::const_iterator type_it = m_shop_limit_map.find(type);
	if (type_it == m_shop_limit_map.end()) return;

	for (std::vector<int>::const_iterator it = type_it->second.begin(), end = type_it->second.end(); it != end; ++it)
	{
		if (*it < 0 || *it >= SHOP_ITEM_MAX_COUNT) continue;
		param->shop_buy_count[*it] = 0;
	}
}

void ShopConfig::ResetGoldBuyCount(int type, ShopParam * param)
{
	if (NULL == param) return;
	std::map<int, std::vector<int> >::const_iterator type_it = m_gold_shop_limit_map.find(type);
	if (type_it == m_gold_shop_limit_map.end()) return;

	for (std::vector<int>::const_iterator it = type_it->second.begin(), end = type_it->second.end(); it != end; ++it)
	{
		if (*it < 0 || *it >= GOLD_SHOP_ITEM_MAX_COUNT) continue;
		param->gold_shop_buy_count[*it] = 0;
	}
}

const MarketItemCfg * ShopConfig::GetMarketItemCfg(ItemID item_id)
{
	std::map<ItemID, MarketItemCfg>::iterator it = m_market_item_map.find(item_id);
	if(it != m_market_item_map.end()) return &it->second;

	return NULL;
}

int ShopConfig::InitShopItemInfoCfg(TiXmlElement *RootElement)
{
	if (NULL == RootElement) return -10000;
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -20000;

	while (dataElement)
	{
		ShopItemCfg node_cfg;
		if (!GetSubNodeValue(dataElement, "index", node_cfg.index) || node_cfg.index < 0 || node_cfg.index >= SHOP_ITEM_MAX_COUNT) return -1;
		if (!GetSubNodeValue(dataElement, "shop_id", node_cfg.shop_id)) return -2;

		if (m_shop_npc_cfg.find(node_cfg.shop_id) == m_shop_npc_cfg.end()) return -3;

		if (!GetSubNodeValue(dataElement, "item_id", node_cfg.item_id) || NULL == ITEMPOOL->GetItem(node_cfg.item_id)) return -4;

		if (!GetSubNodeValue(dataElement, "price", node_cfg.price) || node_cfg.price <= 0) return -5;

		if (!GetSubNodeValue(dataElement, "money_type", node_cfg.money_type) ||
			node_cfg.money_type < CURRENCY_TYPE_GOLD || node_cfg.money_type > CURRENCY_TYPE_COIN_BIND) return -6;

		if (CURRENCY_TYPE_COIN_BIND == node_cfg.money_type)
		{
			if (!ITEMPOOL->CheckItemLegal(node_cfg.item_id, node_cfg.price, __FUNCTION__))
			{
				return -33;
			}
		}

		if (!GetSubNodeValue(dataElement, "is_bind", node_cfg.is_bind) || node_cfg.is_bind < 0 || node_cfg.is_bind > 1) return -7;

		if (!GetSubNodeValue(dataElement, "limit_type", node_cfg.limit_type) ||
			node_cfg.limit_type < SHOP_BUY_LIMIT_TYPE_INVALID || node_cfg.limit_type >= SHOP_BUY_LIMIT_TYPE_MAX) return -8;

		if (!GetSubNodeValue(dataElement, "limit_times", node_cfg.limit_times) || node_cfg.limit_times < 0 || node_cfg.limit_times >= MAX_UINT8) return -9;

		m_shop_item_map[node_cfg.index] = node_cfg;
		m_shop_limit_map[node_cfg.limit_type].push_back(node_cfg.index);

		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int ShopConfig::InitShopNpcInfoCfg(TiXmlElement * RootElement)
{
	if (NULL == RootElement) return -10000;
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -20000;
	while (dataElement)
	{
		int npc_seq;
		int shop_id;
		if (!GetSubNodeValue(dataElement, "shop_id", shop_id)) return -1;
		if (!GetSubNodeValue(dataElement, "npc_seq", npc_seq)) return -2;

		m_shop_npc_cfg[shop_id] = npc_seq;

		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int ShopConfig::InitShopFastBuyCfg(TiXmlElement * RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -20000;

	while (dataElement)
	{
		int item_id; FastBuyContainer tmp_fbc;
		if (!GetSubNodeValue(dataElement, "item_id", item_id) || NULL == ITEMPOOL->GetItem(item_id) || m_fast_buy_cfg.end() != m_fast_buy_cfg.find(item_id))
		{
			return -1;
		}

		if (!GetSubNodeValue(dataElement, "price", tmp_fbc.unit_price) || tmp_fbc.unit_price <= 0)
		{
			return -2;
		}

		if (!GetSubNodeValue(dataElement, "money_type", tmp_fbc.currency_type) || tmp_fbc.currency_type < 0 || tmp_fbc.currency_type >= MONEY_TYPE_MAX)
		{
			return -3;
		}

		if (MONEY_TYPE_COIN_BIND == tmp_fbc.currency_type)
		{
			if (!ITEMPOOL->CheckItemLegal(item_id, tmp_fbc.unit_price, __FUNCTION__))
			{
				return -33;
			}
		}

		if (!GetSubNodeValue(dataElement, "is_bind", tmp_fbc.is_bind) || (0 != tmp_fbc.is_bind && 1 != tmp_fbc.is_bind))
		{
			return -4;
		}

		m_fast_buy_cfg[item_id] = tmp_fbc;
		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int ShopConfig::InitGoldShopBuyCfg(TiXmlElement * RootElement)
{
	if (NULL == RootElement) return -10000;
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -20000;

	while (dataElement)
	{
		GoldShopItemCfg node_cfg;
		if (!GetSubNodeValue(dataElement, "index", node_cfg.index) || node_cfg.index < 0 || node_cfg.index >= GOLD_SHOP_ITEM_MAX_COUNT) return -1;

		if (!GetSubNodeValue(dataElement, "item_id", node_cfg.item_id) || NULL == ITEMPOOL->GetItem(node_cfg.item_id)) return -4;

		if (!GetSubNodeValue(dataElement, "price", node_cfg.price) || node_cfg.price <= 0) return -5;

		if (!GetSubNodeValue(dataElement, "money_type", node_cfg.money_type) ||
			node_cfg.money_type < CURRENCY_TYPE_GOLD || node_cfg.money_type >CURRENCY_TYPE_COIN_BIND) return -6;
		
		if (!GetSubNodeValue(dataElement, "is_bind", node_cfg.is_bind) || node_cfg.is_bind < 0 || node_cfg.is_bind > 1) return -7;

		if (!GetSubNodeValue(dataElement, "limit_type", node_cfg.limit_type) ||
			node_cfg.limit_type < SHOP_BUY_LIMIT_TYPE_INVALID || node_cfg.limit_type >= SHOP_BUY_LIMIT_TYPE_MAX) return -8;

		if (!GetSubNodeValue(dataElement, "limit_times", node_cfg.limit_times) || node_cfg.limit_times >= MAX_UINT8) return -9;

		if (CURRENCY_TYPE_COIN_BIND == node_cfg.money_type)
		{
			if (!ITEMPOOL->CheckItemLegal(node_cfg.item_id, node_cfg.price, __FUNCTION__))
			{
				return -10;
			}
		}

		m_gold_shop_item_map[node_cfg.index] = node_cfg;
		m_gold_shop_limit_map[node_cfg.limit_type].push_back(node_cfg.index);

		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int ShopConfig::InitConvertShopCfg(TiXmlElement *RootElement)
{
	if (NULL == RootElement) return -10000;
	
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -20000;

	int last_type = -1;
	int last_seq = -1;
	while (dataElement)
	{
		int convert_type = 0;
		if (!GetSubNodeValue(dataElement, "conver_type", convert_type) || convert_type < 0 || convert_type >= CONVERT_TYPE_MAX)
		{
			return -1;
		}
		if (convert_type != last_type + 1 && convert_type != last_type)
		{
			return -111;
		}
		if (last_type != convert_type)
		{
			last_type = convert_type;
			last_seq = -1;
		}
		
		int seq = 0;
		if (!GetSubNodeValue(dataElement, "seq", seq) || seq < 0 || seq >= CONVERT_TYPE_MAX_ITEM_COUNT)
		{
			return -2;
		}
		if (seq != last_seq + 1)
		{
			return -222;
		}
		last_seq = seq;

		m_convert_type_item_num_list[convert_type] = seq + 1;
		ConvertShopItemCfg& cfg = m_convert_shop_cfg_list[convert_type][seq];
		if (!GetSubNodeValue(dataElement, "item_id", cfg.item.item_id) || NULL == ITEMPOOL->GetItem(cfg.item.item_id))
		{
			printf("ShopConfig::InitConvertShopCfg item[%d]\n", cfg.item.item_id);
			return -3;
		}

		if (!GetSubNodeValue(dataElement, "is_bind", cfg.item.is_bind))
		{
			return -4;
		}

		if (!GetSubNodeValue(dataElement, "num", cfg.item.num) || cfg.item.num <= 0)
		{
			return -5;
		}

		if (!GetSubNodeValue(dataElement, "price_type", cfg.price_type) || cfg.price_type < MONEY_TYPE_CONVERT_ITEM || cfg.price_type >= MONEY_TYPE_MAX)
		{
			return -6;
		}

		if (!GetSubNodeValue(dataElement, "price", cfg.price) || cfg.price < 0)
		{
			return -7;
		}

		if (MONEY_TYPE_COIN_BIND == cfg.price_type)
		{
			if (!ITEMPOOL->CheckItemLegal(cfg.item.item_id, cfg.price, __FUNCTION__))
			{
				return -33;
			}
		}

		if (!GetSubNodeValue(dataElement, "conver_item_id", cfg.conver_item_id) || cfg.conver_item_id < 0)
		{
			return -8;
		}

		if (!GetSubNodeValue(dataElement, "require_type", cfg.require_type) || cfg.require_type < 0 || cfg.require_type >= CONVERT_REQUIRE_TYPE_MAX)
		{
			return -9;
		}

		if (!GetSubNodeValue(dataElement, "require_value", cfg.require_value) || cfg.require_value < 0)
		{
			return -10;
		}

		if (!GetSubNodeValue(dataElement, "limit_convert_count", cfg.limit_buy_times) || cfg.limit_buy_times < 0)
		{
			return -11;
		}

		if (!GetSubNodeValue(dataElement, "limit_type", cfg.limit_type) || cfg.limit_type < 0 || cfg.limit_type >= CONVERT_LIMIT_TYPE_MAX)
		{
			return -12;
		}

		if (!GetSubNodeValue(dataElement, "reset", cfg.version) || cfg.version < 0)
		{
			return -13;
		}
		
		if (!GetSubNodeValue(dataElement, "open_day", cfg.open_day) || cfg.open_day < 0 || cfg.open_day > 7)
		{
			return -14;
		}

		if (!GetSubNodeValue(dataElement, "last_seq", cfg.last_seq))
		{
			return -15;
		}
		if (-1 != cfg.last_seq)
		{
			if (seq <= cfg.last_seq)
			{
				return -16;
			}
			if (cfg.last_seq < -1)
			{
				return -17;
			}
		}

		if (!GetSubNodeValue(dataElement, "season_type", cfg.is_cloud_arena_new_shop))
		{
			return -18;
		}

		if (!GetSubNodeValue(dataElement, "feisheng_level", cfg.feisheng_count) || cfg.feisheng_count < 0 || cfg.feisheng_count >= FlyUpSystemParam::FLY_UP_STAG_MAX)
		{
			return -19;
		}

		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int ShopConfig::InitNewShopItemInfoCfg(TiXmlElement *RootElement)
{
	if (NULL == RootElement) return -10000;

	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -20000;

	int last_index = -1;
	while (dataElement)
	{
		int index = 0;
		if (!GetSubNodeValue(dataElement, "index", index) || index != last_index + 1 || index >= MAX_NEW_SHOP_ITEM_NUM)
		{
			return -1;
		}
		last_index = index;

		m_new_shop_item_num = index + 1;
		NewShopItemCfg& cfg = m_new_shop_item_list[index];
		if (!GetSubNodeValue(dataElement, "item_id", cfg.item.item_id) || NULL == ITEMPOOL->GetItem(cfg.item.item_id))
		{
			return -3;
		}

		if (!GetSubNodeValue(dataElement, "is_bind", cfg.item.is_bind))
		{
			return -4;
		}

		if (!GetSubNodeValue(dataElement, "num", cfg.item.num) || cfg.item.num <= 0)
		{
			return -5;
		}

		if (!GetSubNodeValue(dataElement, "price_type", cfg.price_type) || cfg.price_type < 0 || cfg.price_type >= MONEY_TYPE_MAX)
		{
			return -6;
		}

		if (!GetSubNodeValue(dataElement, "price", cfg.price) || cfg.price <= 0)
		{
			return -7;
		}

		if (MONEY_TYPE_COIN_BIND == cfg.price_type)
		{
			if (!ITEMPOOL->CheckItemLegal(cfg.item.item_id, cfg.price, __FUNCTION__))
			{
				return -33;
			}
		}

		if (!GetSubNodeValue(dataElement, "require_type", cfg.require_type) || cfg.require_type < 0 || cfg.require_type >= CONVERT_REQUIRE_TYPE_MAX)
		{
			return -8;
		}

		if (!GetSubNodeValue(dataElement, "require_value", cfg.require_value) || cfg.require_value < 0)
		{
			return -9;
		}
		
		int is_show = 0;
		if (!GetSubNodeValue(dataElement, "is_show", is_show))
		{
			return -10;
		}
		cfg.is_show = is_show > 0;

		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int ShopConfig::InitMarketItemCfg(TiXmlElement * RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -888;

	while (NULL != dataElement)
	{
		ItemID item_id = 0;
		if (!GetSubNodeValue(dataElement, "item_id", item_id) || NULL == ITEMPOOL->GetItem(item_id) || m_market_item_map.end() != m_market_item_map.find(item_id))
		{
			return -1;
		}
		MarketItemCfg & cfg = m_market_item_map[item_id];
		cfg.item.item_id = item_id;
		if (!GetSubNodeValue(dataElement, "is_bind", cfg.item.is_bind))
		{
			return -2;
		}

		if (!GetSubNodeValue(dataElement, "num", cfg.item.num) || cfg.item.num != 1)
		{
			return -3;
		}
		if (!GetSubNodeValue(dataElement, "price_type", cfg.price_type) || cfg.price_type < 0 || cfg.price_type >= MONEY_TYPE_MAX)
		{
			return -4;
		}

		if (!GetSubNodeValue(dataElement, "price", cfg.price) || cfg.price <= 0)
		{
			return -5;
		}

		if (!GetSubNodeValue(dataElement, "business_amount", cfg.business_amount) || cfg.business_amount <= 0)
		{
			return -6;
		}
		if (!GetSubNodeValue(dataElement, "float_range", cfg.float_range) || cfg.float_range <= 0)
		{
			return -7;
		}
		if (!GetSubNodeValue(dataElement, "require_type", cfg.require_type) || cfg.require_type < 0 || cfg.require_type >= CONVERT_REQUIRE_TYPE_MAX)
		{
			return -8;
		}

		if (!GetSubNodeValue(dataElement, "require_value", cfg.require_value) || cfg.require_value < 0)
		{
			return -9;
		}

		if (!GetSubNodeValue(dataElement, "conver_type", cfg.conver_type) || cfg.conver_type < 0)
		{
			return -10;
		}

		if (!GetSubNodeValue(dataElement, "feisheng_level", cfg.feisheng_count) || cfg.feisheng_count < 0 || cfg.feisheng_count >= FlyUpSystemParam::FLY_UP_STAG_MAX)
		{
			return -11;
		}

		if (!GetSubNodeValue(dataElement, "min_price", cfg.min_price) || cfg.min_price <= 0)
		{
			return -12;
		}

		if (!GetSubNodeValue(dataElement, "max_price", cfg.max_price) || cfg.max_price <= 0)
		{
			return -13;
		}

		if (cfg.price < cfg.min_price || cfg.price > cfg.max_price)
		{
			return -14;
		}

		if (!GetSubNodeValue(dataElement, "version", cfg.version) || cfg.version < 0)
		{
			return -15;
		}

		int is_show = 0;
		if (!GetSubNodeValue(dataElement, "is_show", is_show))
		{
			return -16;
		}
		cfg.is_show = is_show == 0 ? false : true;

		dataElement = dataElement->NextSiblingElement();	
	}

	if (m_market_item_map.size() > MAX_MARKET_ITEM_NUM)
	{
		return -55;
	}

	return 0;
}

int ShopConfig::InitMarketOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -888;

	if (!GetSubNodeValue(dataElement, "sell_price_scale", m_market_other_cfg.sell_price_scale) || m_market_other_cfg.sell_price_scale <= 0) return -1;
	if (!GetSubNodeValue(dataElement, "float_limit_max", m_market_other_cfg.float_limit_max) || m_market_other_cfg.float_limit_max <= 0) return -2;
	if (!GetSubNodeValue(dataElement, "float_limit_min", m_market_other_cfg.float_limit_min) || m_market_other_cfg.float_limit_min <= 0) return -3;
	if (m_market_other_cfg.float_limit_max + m_market_other_cfg.sell_price_scale > MarketItemCfg::BASE_NUM || m_market_other_cfg.sell_price_scale <= m_market_other_cfg.float_limit_min)
	{
		return -4;
	}

	if (!GetSubNodeValue(dataElement, "float_time_reset", m_market_other_cfg.float_time_reset) || m_market_other_cfg.float_time_reset <= 0 || m_market_other_cfg.float_time_reset > 24) return -5;
	if (0 != (24 % m_market_other_cfg.float_time_reset))
	{
		return -6;
	}

	return 0;
}

int ShopConfig::InitShopOtherCfg(TiXmlElement *RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -888;

	if (!GetSubNodeValue(dataElement, "cloud_arena_new_shop_timestamp", m_shop_other_cfg.cloud_arena_new_shop_timestamp))
	{
		return -1;
	}

	return 0;
}
