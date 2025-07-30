#include "randactivityhorcruxtianchengconfig.hpp"

#include "servercommon/struct/moneyotherdef.h"

RandActivityHorcruxTianChengConfig::RandActivityHorcruxTianChengConfig()
{
}

RandActivityHorcruxTianChengConfig::~RandActivityHorcruxTianChengConfig()
{
	for (std::map<int, std::vector<RAHorcruxTianChengShopCfg> >::iterator it = m_shop_cfg.begin(); it!= m_shop_cfg.end(); it++)
	{
		std::vector<RAHorcruxTianChengShopCfg>().swap(it->second);
	}
	for (std::map<int, std::vector<RAHorcruxTianChengExchangeCfg> >::iterator it = m_exchang_cfg.begin(); it != m_exchang_cfg.end(); it++)
	{
		std::vector<RAHorcruxTianChengExchangeCfg>().swap(it->second);
	}
	for (std::map<int, std::vector<RAHorcruxTianChengConsumeCfg> >::iterator it = m_consume_cfg.begin(); it != m_consume_cfg.end(); it++)
	{
		std::vector<RAHorcruxTianChengConsumeCfg>().swap(it->second);
	}
}

bool RandActivityHorcruxTianChengConfig::Init(TiXmlElement * RootElement, std::string * err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("gift_configure", InitGiftCfg);
	LOAD_RA_CONFIG("jackpot_configure", InitJackpotCfg);
	LOAD_RA_CONFIG("shop_configure", InitShopCfg);
	LOAD_RA_CONFIG("exchange_configure", InitExchangeCfg);
	LOAD_RA_CONFIG("other", InitOtherCfg);
	LOAD_RA_CONFIG("consume_configuration", InitConsumeCfg);

	return true;
}

const RAHorcruxTianChangGiftCfg * RandActivityHorcruxTianChengConfig::GetGiftCfg(RandActivityManager * ramgr, int check_floor, const BitMap<MAX_HORCRUX_TIANCHENG_REWARD_GROUP_NUM>* bit_map, bool *floor_back) const
{
	if (NULL == ramgr) return NULL;
	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_HORCRUX_TIANCHENG);

	std::map<int, RAHorcruxTianChengCfgMap>::const_iterator map_it = m_cfg_map.upper_bound(act_real_open_day);
	if (map_it == m_cfg_map.begin())
		return NULL;
	map_it--;

	//保底
	if (check_floor > 0)
	{
		//先检查是否触发保底
		std::map<int, std::map<int, RAHorcruxTianChangGiftCfg*> >::const_iterator it = m_cfg_floor_map.upper_bound(act_real_open_day);
		if (it != m_cfg_floor_map.begin())
		{
			it--;
			std::map<int, RAHorcruxTianChangGiftCfg*>::const_iterator floor_map = it->second.find(check_floor);
			if (floor_map != it->second.end())
			{
				BitMap<MAX_HORCRUX_TIANCHENG_REWARD_GROUP_NUM> check_bit_map;
				check_bit_map.Reset();
				check_bit_map = *bit_map;
				if (!check_bit_map.IsBitSet(floor_map->second->seq))//没保底过才能触发
				{
					if (NULL != floor_back) *floor_back = true;
					return floor_map->second;//保底直接返回
				}
			}
		}

	}

	int rate = RandomNum(map_it->second.rete_max);
	std::map<int, RAHorcruxTianChangGiftCfg>::const_iterator it = map_it->second.gift_cfg.upper_bound(rate);
	if (it == map_it->second.gift_cfg.end())
		return NULL;

	return &it->second;
}

const RAHorcruxTianChengJackpotCfg * RandActivityHorcruxTianChengConfig::GetJackpotCfg(RandActivityManager * ramgr) const
{
	if (NULL == ramgr) return NULL;
	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_HORCRUX_TIANCHENG);

	std::map<int, RAHorcruxTianChengJackpotCfg>::const_iterator it = m_jackpot_cfg.upper_bound(act_real_open_day);
	if (it == m_jackpot_cfg.begin())
		return NULL;
	it--;

	return &it->second;
}

const RAHorcruxTianChengShopCfg * RandActivityHorcruxTianChengConfig::GetShopCfg(RandActivityManager * ramgr, int shop_index) const
{
	if (NULL == ramgr) return NULL;
	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_HORCRUX_TIANCHENG);

	std::map<int, std::vector<RAHorcruxTianChengShopCfg> >::const_iterator map_it = m_shop_cfg.upper_bound(act_real_open_day);
	if (map_it == m_shop_cfg.begin())
		return NULL;
	map_it--;

	if (shop_index < 0 || shop_index >= (int)map_it->second.size())
		return NULL;

	return &map_it->second[shop_index];
}

const RAHorcruxTianChengExchangeCfg * RandActivityHorcruxTianChengConfig::GetExchangeCfg(RandActivityManager * ramgr, int exchange_index) const
{
	if (NULL == ramgr) return NULL;
	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_HORCRUX_TIANCHENG);

	std::map<int, std::vector<RAHorcruxTianChengExchangeCfg> >::const_iterator map_it = m_exchang_cfg.upper_bound(act_real_open_day);
	if (map_it == m_exchang_cfg.begin())
		return NULL;
	map_it--;

	if (exchange_index < 0 || exchange_index >= (int)map_it->second.size())
		return NULL;

	return &map_it->second[exchange_index];
}

const std::vector<RAHorcruxTianChengShopCfg>* RandActivityHorcruxTianChengConfig::GetShopMapCfg(RandActivityManager * ramgr) const
{
	if (NULL == ramgr) return NULL;
	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_HORCRUX_TIANCHENG);

	std::map<int, std::vector<RAHorcruxTianChengShopCfg> >::const_iterator map_it = m_shop_cfg.upper_bound(act_real_open_day);
	if (map_it == m_shop_cfg.begin())
		return NULL;
	map_it--;

	return &map_it->second;
}

const std::vector<RAHorcruxTianChengExchangeCfg>* RandActivityHorcruxTianChengConfig::GetExchangeMapCfg(RandActivityManager * ramgr) const
{
	if (NULL == ramgr) return NULL;
	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_HORCRUX_TIANCHENG);

	std::map<int, std::vector<RAHorcruxTianChengExchangeCfg> >::const_iterator map_it = m_exchang_cfg.upper_bound(act_real_open_day);
	if (map_it == m_exchang_cfg.begin())
		return NULL;
	map_it--;

	return &map_it->second;
}

const RAHorcruxTianChengConsumeCfg * RandActivityHorcruxTianChengConfig::GetConsumeCfg(RandActivityManager * ramgr, int seq) const
{
	if (NULL == ramgr) return NULL;
	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_HORCRUX_TIANCHENG);

	std::map<int, std::vector<RAHorcruxTianChengConsumeCfg> >::const_iterator act_it = m_consume_cfg.upper_bound(act_real_open_day);
	if (act_it == m_consume_cfg.begin())
		return NULL;
	act_it--;

	if (seq < 0 || seq >= (int)act_it->second.size())
		return NULL;

	return &act_it->second[seq];
}

int RandActivityHorcruxTianChengConfig::GetConsumeSize(RandActivityManager * ramgr) const
{
	if (NULL == ramgr) return NULL;
	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_HORCRUX_TIANCHENG);

	std::map<int, std::vector<RAHorcruxTianChengConsumeCfg> >::const_iterator act_it = m_consume_cfg.upper_bound(act_real_open_day);
	if (act_it == m_consume_cfg.begin())
		return NULL;
	act_it--;

	return act_it->second.size();
}

int RandActivityHorcruxTianChengConfig::InitGiftCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -10000;
	}

	while (NULL != root_element)
	{
		int section_start = 0;
		if (!GetSubNodeValue(root_element, "section_start", section_start) || section_start <= 0)
			return -1;

		RAHorcruxTianChengCfgMap &map_cfg = m_cfg_map[section_start];

		RAHorcruxTianChangGiftCfg cfg;
		if (!GetSubNodeValue(root_element, "seq", cfg.seq) || cfg.seq < 0)
			return -2;

		if (!GetSubNodeValue(root_element, "reward_type", cfg.reward_type) || cfg.reward_type < 0 || cfg.reward_type > 2)
			return -3;

		if (!GetSubNodeValue(root_element, "gold_num", cfg.gold_num) || cfg.gold_num < 0)
			return -4;

		if (cfg.reward_type == 0)
		{
			TiXmlElement * item_element = root_element->FirstChildElement("reward_item");
			if (NULL == item_element)
				return -1000;

			if (!cfg.reward_item.ReadConfig(item_element))
				return -1001;
		}

		if (!GetSubNodeValue(root_element, "reward_per", cfg.reward_per) || cfg.reward_per < 0)
			return -5;

		int rate = 0;
		if (!GetSubNodeValue(root_element, "rate", rate) || rate <= 0)
			return -6;

		if (!GetSubNodeValue(root_element, "hearsay", cfg.hearsay) || cfg.hearsay < 0)
			return -7;

		if (!GetSubNodeValue(root_element, "guarantee_times", cfg.guarantee_times) || cfg.guarantee_times < 0)
			return -8;

		map_cfg.rete_max += rate;
		map_cfg.gift_cfg[map_cfg.rete_max] = cfg;

		root_element = root_element->NextSiblingElement();
	}

	for (std::map<int, RAHorcruxTianChengCfgMap>::iterator it = m_cfg_map.begin(); it != m_cfg_map.end(); it++)
	{
		for (std::map<int, RAHorcruxTianChangGiftCfg>::iterator floor_it = it->second.gift_cfg.begin(); floor_it != it->second.gift_cfg.end(); floor_it++)
		{
			if (floor_it->second.guarantee_times <= 0)continue;
			std::map<int, RAHorcruxTianChangGiftCfg*> &floor_map = m_cfg_floor_map[it->first];
			if (floor_map.find(floor_it->second.guarantee_times) != floor_map.end())//一个保底次数有多个对应配置问题
			{
				return -99;
			}
			floor_map.insert(std::pair<int, RAHorcruxTianChangGiftCfg*>(floor_it->second.guarantee_times, &floor_it->second));
		}
	}

	return 0;
}

int RandActivityHorcruxTianChengConfig::InitJackpotCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -10000;
	}

	while (NULL != root_element)
	{
		int section_start = 0;
		if (!GetSubNodeValue(root_element, "section_start", section_start) || section_start <= 0)
			return -1;

		RAHorcruxTianChengJackpotCfg cfg;
		if (!GetSubNodeValue(root_element, "jackpot_type", cfg.jackpot_type) || cfg.jackpot_type < -1 || cfg.jackpot_type >= MONEY_TYPE_MAX)
			return -2;

		if (cfg.jackpot_type == -1)
		{
			if (!GetSubNodeValue(root_element, "item_id", cfg.item_id) || NULL == ITEMPOOL->GetItem(cfg.item_id))
				return -3;
		}

		if (!GetSubNodeValue(root_element, "start_num", cfg.start_num) || cfg.start_num <= 0)
			return -4;

		if (!GetSubNodeValue(root_element, "all_draw_times", cfg.all_draw_times) || cfg.all_draw_times <= 0)
			return -5;

		if (!GetSubNodeValue(root_element, "add_item_num", cfg.add_item_num) || cfg.add_item_num <= 0)
			return -6;

		m_jackpot_cfg[section_start] = cfg;
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityHorcruxTianChengConfig::InitShopCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -10000;
	}

	while (NULL != root_element)
	{
		int section_start = 0;
		if (!GetSubNodeValue(root_element, "section_start", section_start) || section_start <= 0)
			return -1;

		std::vector<RAHorcruxTianChengShopCfg> &map_cfg = m_shop_cfg[section_start];
		if (map_cfg.size() >= MAX_RA_HORCRUX_TIANCHENG_SHOP_INDEX)
			return -2;

		RAHorcruxTianChengShopCfg cfg;
		if (!GetSubNodeValue(root_element, "limit_type", cfg.limit_type) || (cfg.limit_type != 1 && cfg.limit_type != 2))
			return -3;

		if (!GetSubNodeValue(root_element, "today_times", cfg.limit_times) || cfg.limit_times <= 0)
			return -4;

		if (!GetSubNodeValue(root_element, "price_type", cfg.price_type) || cfg.price_type < -1 || cfg.price_type >= MONEY_TYPE_MAX)
			return -5;

		if (!GetSubNodeValue(root_element, "buy_money", cfg.buy_money) || cfg.buy_money <= 0)
			return -6;

		{
			TiXmlElement * list_element = root_element->FirstChildElement("reward_item_list");
			if (NULL == list_element)
				return -1000;

			TiXmlElement * item_element = list_element->FirstChildElement("reward_item");
			while (NULL != item_element)
			{
				ItemConfigData item;
				if (!item.ReadConfig(item_element))
					return -1001;

				cfg.reward_item.push_back(item);
				item_element = item_element->NextSiblingElement();
			}
		}

		map_cfg.push_back(cfg);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityHorcruxTianChengConfig::InitExchangeCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -10000;
	}

	while (NULL != root_element)
	{
		int section_start = 0;
		if (!GetSubNodeValue(root_element, "section_start", section_start) || section_start <= 0)
			return -1;

		std::vector<RAHorcruxTianChengExchangeCfg> &map_cfg = m_exchang_cfg[section_start];
		if (map_cfg.size() >= MAX_RA_HORCRUX_TIANCHENG_EXCHANGE_INDEX)
			return -2;

		RAHorcruxTianChengExchangeCfg cfg;
		{
			TiXmlElement * item_element = root_element->FirstChildElement("exchange_item");
			if (NULL == item_element)
				return -1001;

			if (!cfg.exchange_item.ReadConfig(item_element))
				return -1002;
		}
		{
			TiXmlElement * list_element = root_element->FirstChildElement("consume_item_list");
			if (NULL != list_element)
			{
				TiXmlElement * item_element = list_element->FirstChildElement("consume_item");
				while (NULL != item_element)
				{
					ItemConfigData item;
					if (!item.ReadConfig(item_element))
						return -2000;

					cfg.consume_item.push_back(item);
					item_element = item_element->NextSiblingElement();
				}
			}
		}
		if (!GetSubNodeValue(root_element, "gold", cfg.gold) || cfg.gold < 0)
			return -3;

		if (!GetSubNodeValue(root_element, "jade", cfg.jade) || cfg.jade < 0)
			return -4;

		if (!GetSubNodeValue(root_element, "buy_type", cfg.buy_type) || (cfg.buy_type != 1 && cfg.buy_type != 2))
			return -5;

		if (!GetSubNodeValue(root_element, "today_times", cfg.limit_times) || cfg.limit_times <= 0)
			return -6;

		map_cfg.push_back(cfg);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityHorcruxTianChengConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -10000;
	}

	if (!GetSubNodeValue(root_element, "item_id", m_other_cfg.item_id) || NULL == ITEMPOOL->GetItem(m_other_cfg.item_id))
		return -1;

	if (!GetSubNodeValue(root_element, "item_num", m_other_cfg.item_num) || m_other_cfg.item_num <= 0)
		return -2;

	short is_open = 0;
	if (!GetSubNodeValue(root_element, "is_open", is_open))
		return -3;
	m_other_cfg.is_open_exchange = 0 != is_open;

	return 0;
}

int RandActivityHorcruxTianChengConfig::InitConsumeCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -10000;
	}

	while (NULL != root_element)
	{
		int section_start = 0;
		if (!GetSubNodeValue(root_element, "section_start", section_start) || section_start <= 0)
			return -1;

		RAHorcruxTianChengConsumeCfg cfg;
		if (!GetSubNodeValue(root_element, "consume_num", cfg.consume_num) || cfg.consume_num <= 0)
			return -2;

		{
			TiXmlElement * list_element = root_element->FirstChildElement("reward_item_list");
			if (NULL != list_element)
			{
				TiXmlElement * item_element = list_element->FirstChildElement("reward_item");
				while (NULL != item_element)
				{
					ItemConfigData item;
					if (!item.ReadConfig(item_element))
						return -2000;

					cfg.item.push_back(item);
					item_element = item_element->NextSiblingElement();
				}
			}
		}

		m_consume_cfg[section_start].push_back(cfg);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}
