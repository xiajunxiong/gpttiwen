#include "randactivityluckyblindboxconfig.hpp"
#include "item/itempool.h"
#include <algorithm>
#include "servercommon/struct/moneyotherdef.h"

RandActivityLuckyBlindBoxConfig::RandActivityLuckyBlindBoxConfig()
{
}

RandActivityLuckyBlindBoxConfig::~RandActivityLuckyBlindBoxConfig()
{
	std::vector<LuckyBlindBoxBaseCfg>().swap(base_cfg_vec);
	std::vector<LuckyBlindBoxShopCfg>().swap(shop_cfg_vec);
	std::vector<LuckyBlindBoxRechargeCfg>().swap(recharge_cfg_vec);
}

bool RandActivityLuckyBlindBoxConfig::Init(TiXmlElement * RootElement, std::string * err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("basis_configuration", InitBaseCfg);
	LOAD_RA_CONFIG("shop_configuration", InitShopCfg);
	LOAD_RA_CONFIG("add_recharge_configuration", InitRechargeCfg);
	LOAD_RA_CONFIG("reward_configuration", InitRewardCfg);
	LOAD_RA_CONFIG("other", InitOtherCfg);
	return true;
}

int RandActivityLuckyBlindBoxConfig::GetConfigVer() const
{
	return other_cfg.cfg_ver;
}

const LuckyBlindBoxBaseCfg * RandActivityLuckyBlindBoxConfig::GetBaseCfg(int open_day) const
{
	for (std::vector<LuckyBlindBoxBaseCfg>::const_iterator it = base_cfg_vec.begin(); it != base_cfg_vec.end(); it++)
	{
		if (it->section_start <= open_day && it->section_end >= open_day)
		{
			return &(*it);
		}
	}
	return NULL;
}

const LuckyBlindBoxShopCfg * RandActivityLuckyBlindBoxConfig::GetShopCfg(int open_day, int seq) const
{
	for (std::vector<LuckyBlindBoxShopCfg>::const_iterator it = shop_cfg_vec.begin(); it != shop_cfg_vec.end(); it++)
	{
		if (it->section_start <= open_day && it->section_end >= open_day && it->seq == seq)
		{
			return &(*it);
		}
	}
	return NULL;
}

const LuckyBlindBoxRechargeCfg * RandActivityLuckyBlindBoxConfig::GetRechargeCfg(int open_day, int seq) const
{
	for (std::vector<LuckyBlindBoxRechargeCfg>::const_iterator it = recharge_cfg_vec.begin(); it != recharge_cfg_vec.end(); it++)
	{
		if (it->section_start <= open_day && it->section_end >= open_day && it->seq == seq)
		{
			return &(*it);
		}
	}
	return NULL;
}

const LuckyBlindBoxRewardCfg * RandActivityLuckyBlindBoxConfig::RandRewardCfg(int reward_group, int check_floor, BitMap<MAX_LUCKY_BLIND_BOX_REWARD_GROUP_NUM>* bit_map, bool *floor_back) const
{
	if (check_floor > 0)
	{
		//先检查是否触发保底
		std::map<int, std::map<int, LuckyBlindBoxRewardCfg*> >::const_iterator it = reward_floor_map.find(reward_group);
		if (it != reward_floor_map.end())
		{
			std::map<int, LuckyBlindBoxRewardCfg*>::const_iterator floor_map = it->second.find(check_floor);
			if (floor_map != it->second.end())
			{
				if (!bit_map->IsBitSet(floor_map->second->seq))//没保底过才能触发
				{
					if(NULL != floor_back) *floor_back = true;
					return floor_map->second;//保底直接返回
				}
			}
		}
		
	}
	std::map<int, LuckyBlindBoxRewardListCfg>::const_iterator it = reward_cfg_map.find(reward_group);
	if (it != reward_cfg_map.end())
	{
		int rate_value = rand() % it->second.weight;
		for (std::vector<LuckyBlindBoxRewardCfg>::const_iterator itor = it->second.group_vec.begin(); itor != it->second.group_vec.end(); itor++)
		{
			if (rate_value <= itor->rate)
			{
				return &(*itor);
			}
			rate_value -= itor->rate;
		}
	}
	return NULL;
}

int RandActivityLuckyBlindBoxConfig::InitBaseCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element) return -544;

	while (NULL != data_element)
	{
		LuckyBlindBoxBaseCfg node;

		if (!GetSubNodeValue(data_element, "type", node.type) || node.type < 0) return -1;
		if (!GetSubNodeValue(data_element, "section_start", node.section_start)) return -2;
		if (!GetSubNodeValue(data_element, "section_end", node.section_end)) return -3;
		if (!GetSubNodeValue(data_element, "seq", node.seq))return -4;
		if (!GetSubNodeValue(data_element, "reward_group", node.reward_group))return -4;
		if (node.section_end == 0)
		{
			node.section_end = MAX_INT;
		}

		base_cfg_vec.push_back(node);
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityLuckyBlindBoxConfig::InitShopCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element) return -544;

	while (NULL != data_element)
	{
		LuckyBlindBoxShopCfg node;

		if (!GetSubNodeValue(data_element, "type", node.type) || node.type < 0) return -1;
		if (!GetSubNodeValue(data_element, "section_start", node.section_start)) return -2;
		if (!GetSubNodeValue(data_element, "section_end", node.section_end)) return -3;
		if (node.section_end == 0)
		{
			node.section_end = MAX_INT;
		}
		if (!GetSubNodeValue(data_element, "seq", node.seq))return -4;
		if (!GetSubNodeValue(data_element, "limit_times", node.limit_times))return -7;
		if (!GetSubNodeValue(data_element, "limit_type", node.limit_type))return -8;
		if (!GetSubNodeValue(data_element, "price", node.price))return -9;
		if (!GetSubNodeValue(data_element, "price_type", node.price_type) || node.price_type < MONEY_TYPE_GOLD || node.price_type >= MONEY_TYPE_MAX)return -10;
		if (!GetSubNodeValue(data_element, "cfg_or", node.cfg_or) || node.cfg_or < 0 || node.cfg_or > 1)return -11;

		TiXmlElement * reward_list_element = data_element->FirstChildElement("reward_item_list");
		if (NULL != reward_list_element)
		{
			TiXmlElement * reward_element = reward_list_element->FirstChildElement("reward_item");
			while (NULL != reward_element)
			{
				ItemConfigData reward;
				if (!reward.ReadConfig(reward_element)) return -5;
				node.reward_vec.push_back(reward);
				reward_element = reward_element->NextSiblingElement();
			}
		}

		shop_cfg_vec.push_back(node);
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityLuckyBlindBoxConfig::InitRechargeCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element) return -544;

	while (NULL != data_element)
	{
		LuckyBlindBoxRechargeCfg node;

		if (!GetSubNodeValue(data_element, "type", node.type) || node.type < 0) return -1;
		if (!GetSubNodeValue(data_element, "section_start", node.section_start)) return -2;
		if (!GetSubNodeValue(data_element, "section_end", node.section_end)) return -3;
		if (node.section_end == 0)node.section_end = MAX_INT;
		if (!GetSubNodeValue(data_element, "seq", node.seq))return -4;
		if (!GetSubNodeValue(data_element, "get_type", node.get_type))return -7;
		if (!GetSubNodeValue(data_element, "get_times", node.get_times))return -8;
		if (!GetSubNodeValue(data_element, "add_recharge", node.add_recharge))return -9;

		TiXmlElement * reward_list_element = data_element->FirstChildElement("reward_item_list");
		if (NULL != reward_list_element)
		{
			TiXmlElement * reward_element = reward_list_element->FirstChildElement("reward_item");
			while (NULL != reward_element)
			{
				ItemConfigData reward;
				if (!reward.ReadConfig(reward_element)) return -5;
				node.reward_vec.push_back(reward);
				reward_element = reward_element->NextSiblingElement();
			}
		}

		recharge_cfg_vec.push_back(node);
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityLuckyBlindBoxConfig::InitRewardCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element) return -544;

	while (NULL != data_element)
	{
		int reward_group = 0;
		if (!GetSubNodeValue(data_element, "reward_group", reward_group) || reward_group <= 0) return -1;
		LuckyBlindBoxRewardListCfg &node_list = reward_cfg_map[reward_group];
		node_list.reward_group = reward_group;
	
		LuckyBlindBoxRewardCfg node;
		if (!GetSubNodeValue(data_element, "seq", node.seq) || node.seq >= MAX_LUCKY_BLIND_BOX_REWARD_GROUP_NUM) return -2;
		if (!GetSubNodeValue(data_element, "rate", node.rate)) return -3;
		if (!GetSubNodeValue(data_element, "guarantee_times", node.guarantee_times)) return -4;
		if (!GetSubNodeValue(data_element, "hearsay", node.hearsay)) return -4;
		node_list.weight += node.rate;
		TiXmlElement * reward_element = data_element->FirstChildElement("reward_item");
		if (NULL != reward_element)
		{
			if (!node.reward.ReadConfig(reward_element)) return -6;
		}

		node_list.group_vec.push_back(node);
		data_element = data_element->NextSiblingElement();
	}

	for (std::map<int, LuckyBlindBoxRewardListCfg>::iterator it = reward_cfg_map.begin(); it != reward_cfg_map.end(); it++)
	{
		for (std::vector<LuckyBlindBoxRewardCfg>::iterator reward_it = it->second.group_vec.begin(); reward_it != it->second.group_vec.end(); reward_it++)
		{
			if (reward_it->guarantee_times <= 0)continue;
			std::map<int, LuckyBlindBoxRewardCfg*> &map_node = reward_floor_map[it->first];
			if (map_node.find(reward_it->guarantee_times) != map_node.end())
			{
				return -99;
			}

			map_node.insert(std::pair<int, LuckyBlindBoxRewardCfg*>(reward_it->guarantee_times, &(*reward_it)));
		}
	}

	return 0;
}

int RandActivityLuckyBlindBoxConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element) return -544;

	if (!GetSubNodeValue(data_element, "cfg_ver", other_cfg.cfg_ver) || other_cfg.cfg_ver < 0) return -1;
	TiXmlElement * reward_element = data_element->FirstChildElement("consume_item");
	if (NULL != reward_element)
	{
		if (!other_cfg.cost_item.ReadConfig(reward_element)) return -5;
	}

	return 0;
}