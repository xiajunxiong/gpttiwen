#include "randactivityjixinggaozhaoconfig.hpp"

#include "item/itembase.h"

RandActivityJiXingGaoZhaoConfig::RandActivityJiXingGaoZhaoConfig()
{
}

RandActivityJiXingGaoZhaoConfig::~RandActivityJiXingGaoZhaoConfig()
{
	for (std::map<int, std::vector<RAJiXingGaoZhaoShopCfg> >::iterator it = m_shop_cfg.begin(); it != m_shop_cfg.end(); it++)
	{
		std::vector<RAJiXingGaoZhaoShopCfg>().swap(it->second);
	}

	for (std::map<int, std::vector<RAJiXingGaoZhaoPhaseCfg> >::iterator it = m_phase_cfg.begin(); it != m_phase_cfg.end(); it++)
	{
		std::vector<RAJiXingGaoZhaoPhaseCfg>().swap(it->second);
	}
}

bool RandActivityJiXingGaoZhaoConfig::Init(TiXmlElement * RootElement, std::string * err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("basis_configuration", InitBaseCfg);
	LOAD_RA_CONFIG("gift_configuration", InitJackpotCfg);
	LOAD_RA_CONFIG("buy_configuration", InitShopCfg);
	LOAD_RA_CONFIG("phase_reward", InitPhaseCfg);
	LOAD_RA_CONFIG("other", InitOtherCfg);

	return true;
}

const RAJiXingGaoZhaoBaseCfg * RandActivityJiXingGaoZhaoConfig::GetBaseCfg(RandActivityManager * ramgr, int check_floor, const BitMap<MAX_JI_XING_GAO_ZHAO_REWARD_GROUP_NUM>* bit_map, bool *floor_back) const
{
	if (NULL == ramgr) return NULL;
	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_JIXING_GAOZHAO);

	std::map<int, RAJiXingGaoZhaoBaseMapCfg>::const_iterator map_it = m_base_map.upper_bound(act_real_open_day);
	if (map_it == m_base_map.begin())
		return NULL;
	map_it--;

	//保底
	if (check_floor > 0)
	{
		//先检查是否触发保底
		std::map<int, std::map<int, RAJiXingGaoZhaoBaseCfg*> >::const_iterator it = m_cfg_floor_map.upper_bound(act_real_open_day);
		if (it != m_cfg_floor_map.begin())
		{
			it--;
			std::map<int, RAJiXingGaoZhaoBaseCfg*>::const_iterator floor_map = it->second.find(check_floor);
			if (floor_map != it->second.end())
			{
				BitMap<MAX_JI_XING_GAO_ZHAO_REWARD_GROUP_NUM> check_bit_map;
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

	int rate = RandomNum(map_it->second.rate_max);
	std::map<int, RAJiXingGaoZhaoBaseCfg>::const_iterator it = map_it->second.base_cfg.upper_bound(rate);
	if (it == map_it->second.base_cfg.end())
		return NULL;

	return &it->second;
}

const RAJiXingGaoZhaoJackpotCfg * RandActivityJiXingGaoZhaoConfig::GetJackpotCfg(RandActivityManager * ramgr) const
{
	if (NULL == ramgr) return NULL;
	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_JIXING_GAOZHAO);

	std::map<int, RAJiXingGaoZhaoJackpotCfg>::const_iterator it = m_jackpot_cfg.upper_bound(act_real_open_day);
	if (it == m_jackpot_cfg.begin())
		return NULL;
	it--;

	return &it->second;
}

const RAJiXingGaoZhaoShopCfg * RandActivityJiXingGaoZhaoConfig::GetShopCfg(RandActivityManager * ramgr, int shop_index) const
{
	if (NULL == ramgr) return NULL;
	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_JIXING_GAOZHAO);

	std::map<int, std::vector<RAJiXingGaoZhaoShopCfg> >::const_iterator map_it = m_shop_cfg.upper_bound(act_real_open_day);
	if (map_it == m_shop_cfg.begin())
		return NULL;
	map_it--;

	if (shop_index < 0 || shop_index >= (int)map_it->second.size())
		return NULL;

	return &map_it->second[shop_index];
}

const RAJiXingGaoZhaoPhaseCfg * RandActivityJiXingGaoZhaoConfig::GetPhaseCfg(RandActivityManager * ramgr, int phase_index) const
{
	if (NULL == ramgr) return NULL;
	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_JIXING_GAOZHAO);

	std::map<int, std::vector<RAJiXingGaoZhaoPhaseCfg> >::const_iterator map_it = m_phase_cfg.upper_bound(act_real_open_day);
	if (map_it == m_phase_cfg.begin())
		return NULL;
	map_it--;

	if (phase_index < 0 || phase_index >= (int)map_it->second.size())
		return NULL;

	return &map_it->second[phase_index];
}

const std::vector<RAJiXingGaoZhaoShopCfg>* RandActivityJiXingGaoZhaoConfig::GetShopMapCfg(RandActivityManager * ramgr) const
{
	if (NULL == ramgr) return NULL;
	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_JIXING_GAOZHAO);

	std::map<int, std::vector<RAJiXingGaoZhaoShopCfg> >::const_iterator map_it = m_shop_cfg.upper_bound(act_real_open_day);
	if (map_it == m_shop_cfg.begin())
		return NULL;
	map_it--;

	return &map_it->second;
}

int RandActivityJiXingGaoZhaoConfig::InitBaseCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -10000;
	}
	m_cfg_floor_map.clear();
	while (NULL != root_element)
	{
		int section_start = 0;
		if (!GetSubNodeValue(root_element, "section_start", section_start) || section_start <= 0)
			return -1;

		RAJiXingGaoZhaoBaseMapCfg &map_cfg = m_base_map[section_start];
		RAJiXingGaoZhaoBaseCfg cfg;
		if (!GetSubNodeValue(root_element, "seq", cfg.seq) || cfg.seq < 0)
			return -2;
		if (!GetSubNodeValue(root_element, "reward_type", cfg.reward_type) || cfg.reward_type < 0 || cfg.reward_type > 2)
			return -3;

		if (cfg.reward_type == 0)
		{
			TiXmlElement * item_element = root_element->FirstChildElement("reward_item");
			if (NULL == item_element)
				return -1000;

			if (!cfg.reward_item.ReadConfig(item_element))
				return -1001;
		}

		if (!GetSubNodeValue(root_element, "gold_num", cfg.gold_num) || cfg.gold_num < 0)
			return -4;
		if (!GetSubNodeValue(root_element, "reward_per", cfg.reward_per) || cfg.reward_per < 0)
			return -5;

		int rate = 0;
		if (!GetSubNodeValue(root_element, "rate", rate) || rate <= 0)
			return -6;

		if (!GetSubNodeValue(root_element, "hearsay", cfg.hearsay) || cfg.hearsay < 0)
			return -7;

		if (!GetSubNodeValue(root_element, "guarantee_times", cfg.guarantee_times) || cfg.guarantee_times < 0)
			return -8;

		map_cfg.rate_max += rate;
		map_cfg.base_cfg[map_cfg.rate_max] = cfg;

		root_element = root_element->NextSiblingElement();
	}

	for (std::map<int, RAJiXingGaoZhaoBaseMapCfg>::iterator it = m_base_map.begin(); it != m_base_map.end(); it++)
	{
		for (std::map<int, RAJiXingGaoZhaoBaseCfg>::iterator floor_it = it->second.base_cfg.begin(); floor_it != it->second.base_cfg.end(); floor_it++)
		{
			if (floor_it->second.guarantee_times <= 0)continue;
			std::map<int, RAJiXingGaoZhaoBaseCfg*> &floor_map = m_cfg_floor_map[it->first];
			if (floor_map.find(floor_it->second.guarantee_times) != floor_map.end())//一个保底次数有多个对应配置问题
			{
				return -99;
			}
			floor_map.insert(std::pair<int, RAJiXingGaoZhaoBaseCfg*>(floor_it->second.guarantee_times, &floor_it->second));
		}
	}

	return 0;
}

int RandActivityJiXingGaoZhaoConfig::InitJackpotCfg(TiXmlElement * RootElement)
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

		RAJiXingGaoZhaoJackpotCfg cfg;
		if (!GetSubNodeValue(root_element, "jackpot_item_id", cfg.jackpot_item_id) || 
			(0 != cfg.jackpot_item_id && NULL == ITEMPOOL->GetItem(cfg.jackpot_item_id)))
			return -2;

		if (!GetSubNodeValue(root_element, "jackpot_item_num", cfg.jackpot_item_num) || cfg.jackpot_item_num < 0)
			return -3;
		if (!GetSubNodeValue(root_element, "jackpot_gold_num", cfg.jackpot_gold_num) || cfg.jackpot_gold_num < 0)
			return -4;

		if (!GetSubNodeValue(root_element, "all_draw_times", cfg.all_draw_times) || cfg.all_draw_times <= 0)
			return -5;
		if (!GetSubNodeValue(root_element, "add_item_num", cfg.add_item_num) || cfg.add_item_num < 0)
			return -6;
		if (!GetSubNodeValue(root_element, "add_gold", cfg.add_gold) || cfg.add_gold < 0)
			return -7;

		m_jackpot_cfg[section_start] = cfg;
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityJiXingGaoZhaoConfig::InitShopCfg(TiXmlElement * RootElement)
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

		RAJiXingGaoZhaoShopCfg cfg;
		if (!GetSubNodeValue(root_element, "limit_type", cfg.limit_type) || (cfg.limit_type != 1 && cfg.limit_type != 2))
			return -3;

		if (!GetSubNodeValue(root_element, "buy_times", cfg.limit_times) || cfg.limit_times <= 0)
			return -4;

		if (!GetSubNodeValue(root_element, "price_type", cfg.price_type) || cfg.price_type < 0)
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

		m_shop_cfg[section_start].push_back(cfg);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityJiXingGaoZhaoConfig::InitPhaseCfg(TiXmlElement * RootElement)
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

		RAJiXingGaoZhaoPhaseCfg cfg;
		if (!GetSubNodeValue(root_element, "lucky_draw_num", cfg.lucky_draw_num) || cfg.lucky_draw_num <= 0)
			return -2;

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

		m_phase_cfg[section_start].push_back(cfg);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityJiXingGaoZhaoConfig::InitOtherCfg(TiXmlElement * RootElement)
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

	return 0;
}
