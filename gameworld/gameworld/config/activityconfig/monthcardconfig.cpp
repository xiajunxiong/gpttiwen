#include "monthcardconfig.hpp"

MonthCardConfig::MonthCardConfig()
{
}

MonthCardConfig::~MonthCardConfig()
{
}

bool MonthCardConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG("other", InitOtherCfg);
	LOAD_CONFIG("month_card_configuration", InitMonthCardCfg);
	LOAD_CONFIG("shop_configuration", InitMonthShopCfg);

	
	return true;
}

const MonthCardCfg * MonthCardConfig::GetCardCfg(int level, int card_type)
{
	if(card_type < 0 || card_type >= MONTH_CARD_TYPE_MAX) return NULL;
	MonthCardListCfg &node = m_month_card_list[card_type];
	for (std::vector<MonthCardCfg>::iterator it = node.list_vec.begin(); it != node.list_vec.end(); it++)
	{
		if (level >= it->level_down && level <= it->level_up)
		{
			return &(*it);
		}
	}
	return NULL;
}

const MonthShopCfg * MonthCardConfig::GetShopCfg(int level, int seq)
{
	for (std::map<int, MonthShopListCfg>::iterator it = m_shop_list.begin(); it != m_shop_list.end(); it++)
	{
		if (it->second.level_down <= level && it->second.level_up >= level)
		{
			if (seq < 0 || seq >= MAX_MONTH_CARD_SHOP_ITEM_NUM || seq >= it->second.m_shop_count) return NULL;

			return &it->second.shop_list[seq];
		}
	}

	return NULL;
}

void MonthCardConfig::OnRefreshBuyTimes(int level, int * buy_times_list, int max_count, int limit_type)
{
	if(NULL == buy_times_list || max_count <= 0 || limit_type <= MONTH_CARD_LIMIT_TYPE_INVAILD || limit_type >= MONTH_CARD_LIMIT_TYPE_MAX) return;

	for (std::map<int, MonthShopListCfg>::iterator it = m_shop_list.begin(); it != m_shop_list.end(); it++)
	{
		if (it->second.level_down <= level && it->second.level_up >= level)
		{
			for (int i = 0; i < max_count && i < it->second.m_shop_count && i < MAX_MONTH_CARD_SHOP_ITEM_NUM; ++i)
			{
				if (it->second.shop_list[i].limit_type == limit_type)
				{
					buy_times_list[i] = 0;
				}
			}
			return;
		}
	}
}

int MonthCardConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	if (!GetSubNodeValue(root_element, "cfg_ver", m_other_cfg.cfg_ver) || m_other_cfg.cfg_ver <= 0)
	{
		return -1;
	}

	return 0;
}

int MonthCardConfig::InitMonthCardCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (root_element != NULL)
	{
		int card_type = 0;
		if (!GetSubNodeValue(root_element, "card_type", card_type) || card_type < 0 || card_type >= MONTH_CARD_TYPE_MAX) return -1;
		MonthCardListCfg & node_list = m_month_card_list[card_type];
		MonthCardCfg node_cfg;
		if (!GetSubNodeValue(root_element, "card_days", node_cfg.card_day) || node_cfg.card_day <= 0) return -2;
		if (!GetSubNodeValue(root_element, "buy_reward_gold", node_cfg.buy_reward_gold) || node_cfg.buy_reward_gold < 0) return -3;
		if (!GetSubNodeValue(root_element, "buy_reward_immortal", node_cfg.buy_reward_immortal) || node_cfg.buy_reward_immortal < 0) return -4;
		if (!GetSubNodeValue(root_element, "buy_money", node_cfg.buy_money) || node_cfg.buy_money <= 0) return -5;
		if (!GetSubNodeValue(root_element, "level_down", node_cfg.level_down) || node_cfg.level_down <= 0) return -6;
		if (!GetSubNodeValue(root_element, "level_up", node_cfg.level_up) || node_cfg.level_up <= 0) return -7;

		TiXmlElement *buy_reward_element = root_element->FirstChildElement("buy_reward_item");
		if (buy_reward_element != NULL)
		{
			node_cfg.buy_reward.ReadConfig(buy_reward_element);
		}

		TiXmlElement *day_reward_item_list = root_element->FirstChildElement("day_reward_item_list");
		if (NULL == day_reward_item_list)
		{
			return -100;
		}
		TiXmlElement * day_reward_item = day_reward_item_list->FirstChildElement("day_reward_item");
		if (NULL == day_reward_item)
		{
			return -101;
		}
		while (NULL != day_reward_item)
		{
			ItemConfigData item;
			if (!item.ReadConfig(day_reward_item))
			{
				return -102;
			}

			node_cfg.day_reward_list.push_back(item);
			day_reward_item = day_reward_item->NextSiblingElement();
		}

		node_list.list_vec.push_back(node_cfg);
		root_element = root_element->NextSiblingElement();
	}
	return 0;
}

int MonthCardConfig::InitMonthShopCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	int next_seq = 0;
	int next_type = 0;
	while (root_element != NULL)
	{
		int type = 0;
		if (!GetSubNodeValue(root_element, "type", type) || type < 0) return -1;

		MonthShopListCfg & node_cfg = m_shop_list[type];
		if (type != node_cfg.type)
		{
			node_cfg.m_shop_count = 0;
			next_seq = 0;
		}
		node_cfg.type = type;
		if (!GetSubNodeValue(root_element, "level_down", node_cfg.level_down) || node_cfg.level_down < 0 || node_cfg.level_down > MAX_ROLE_LEVEL) return -2;

		if (!GetSubNodeValue(root_element, "level_up", node_cfg.level_up) || node_cfg.level_up < 0 || node_cfg.level_up > MAX_ROLE_LEVEL)
		{
			return -3;
		}

		int seq = 0;
		if (!GetSubNodeValue(root_element, "seq", seq) || seq < 0 || seq >= MAX_MONTH_CARD_SHOP_ITEM_NUM || seq != next_seq)
		{
			return -4;
		}

		next_seq++;

		MonthShopCfg &node = node_cfg.shop_list[seq];
		node.type = type;
		if (!GetSubNodeValue(root_element, "limit_type", node.limit_type) || node.limit_type <= MONTH_CARD_LIMIT_TYPE_INVAILD || node.limit_type >= MONTH_CARD_LIMIT_TYPE_MAX) return -5;
		if (!GetSubNodeValue(root_element, "buy_times", node.week_buy_times) || node.week_buy_times < 0) return -6;
		if (!GetSubNodeValue(root_element, "today_times", node.today_buy_times) || node.today_buy_times < 0) return -7;
		switch (node.limit_type)
		{
			case MONTH_CARD_LIMIT_TYPE_WEEK:
			{
				if(node.week_buy_times <= 0) return -8;
			}
			break;
			case MONTH_CARD_LIMIT_TYPE_DAY:
			{
				if(node.today_buy_times <= 0) return -9;
			}
			break;
			default:
			return -10;
		}
		if (!GetSubNodeValue(root_element, "price_type", node.price_type) || node.price_type < MONTH_CARD_PRICE_TYPE_GOLD || node.price_type >= MONTH_CARD_PRICE_TYPE_MAX) return -11;
		if (!GetSubNodeValue(root_element, "price", node.price) || node.price < 0) return -12;

		TiXmlElement *reward_item_element = root_element->FirstChildElement("reward_item");
		if (reward_item_element != NULL)
		{
			if (!node.item.ReadConfig(reward_item_element))
			{
				return -13;
			}
		}

		node_cfg.m_shop_count++;
		root_element = root_element->NextSiblingElement();
		if (node_cfg.m_shop_count > MAX_MONTH_CARD_SHOP_ITEM_NUM)
		{
			return -99;
		}
	}
	return 0;
}
