#include "giftranditemconfig.hpp"
#include "checkresourcecenter.hpp"
#include "item/itempool.h"
#include "item/itembase.h"
#include <algorithm>

GiftRandItemconfig::GiftRandItemconfig()
{
}

GiftRandItemconfig::~GiftRandItemconfig()
{
}

bool GiftRandItemconfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG("randitem", InitBoxCfg);
	LOAD_CONFIG("redbag", InitRedBagCfg);

	return true;
}
static int static_rand_num = 0;

bool BoxRand(BoxItemCfg a)
{
	if (static_rand_num < a.rate) return true;
	static_rand_num -= a.rate;
	return false;
}

bool GiftRandItemconfig::RandItem(int box_type, BoxItemCfg * item_cfg)
{
	if (NULL == item_cfg) return false;
	std::map<int, BoxCfg>::const_iterator box_it = m_box_map.find(box_type);
	if (box_it == m_box_map.end()) return false;

	static_rand_num = RandomNum(box_it->second.total_rate);
	std::vector<BoxItemCfg>::const_iterator begin = box_it->second.item_list.begin();
	std::vector<BoxItemCfg>::const_iterator end = box_it->second.item_list.end();
	std::vector<BoxItemCfg>::const_iterator item_it = std::find_if(begin, end, BoxRand);
	if (item_it != end)
	{
		*item_cfg = *item_it;
		return true;
	}

	return false;
}

const BoxItemCfg * GiftRandItemconfig::GetBoxItemCfg(int box_type, int index)
{
	std::map<int, BoxCfg>::const_iterator box_it = m_box_map.find(box_type);
	if (box_it == m_box_map.end()) return NULL;
	int max_size = box_it->second.item_list.size();
	if (index < 0 || index >= max_size) return NULL;
	return &box_it->second.item_list[index];
}

const GiftRandItemconfig::RedBagCfg::ItemInfo * GiftRandItemconfig::GetRandRedBagReward(int box_type) const
{
	RedBagCfgMap::const_iterator red_bag_cit = m_red_box_type_map.find(box_type);
	if (m_red_box_type_map.end() == red_bag_cit)
	{
		return NULL;
	}

	int rand_num = RandomNum(red_bag_cit->second.total_power);
	std::map<int, RedBagCfg::ItemInfo>::const_iterator item_cit 
		= red_bag_cit->second.item_map.upper_bound(rand_num);
	if (red_bag_cit->second.item_map.begin() == item_cit)
	{
		return NULL;
	}
	item_cit--;

	return &item_cit->second;
}

int GiftRandItemconfig::InitBoxCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	while (NULL != root_element)
	{
		int box_type = 0;
		if (!GetSubNodeValue(root_element, "box_type", box_type))return -1;

		BoxCfg& node_cfg = m_box_map[box_type];
		BoxItemCfg item_cfg;
		if (!GetSubNodeValue(root_element, "rate", item_cfg.rate) || item_cfg.rate < 0)return -2;
		if (!GetSubNodeValue(root_element, "item_type", item_cfg.item_type) || item_cfg.item_type < ITEM_TYPE_BEGIN || item_cfg.item_type >= ITEM_TYPE_END)return -3;
		if (!GetSubNodeValue(root_element, "is_broadcast", item_cfg.is_broadcast))return -4;

		TiXmlElement *item_element = root_element->FirstChildElement("reward_item");
		item_cfg.item_data.ReadConfigNoCheck(item_element);
	
		switch (item_cfg.item_type)
		{
		case ITEM_TYPE_KNAPSACK:
			{
				CheckResourceCenter::Instance().GetItemValidCheck()->Add(item_cfg.item_data.item_id, item_cfg.item_data.item_id, item_cfg.item_data.num, __FUNCTION__);
			}
			break;
		}

		node_cfg.item_list.push_back(item_cfg);
		node_cfg.total_rate += item_cfg.rate;

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int GiftRandItemconfig::InitRedBagCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	while (NULL != root_element)
	{
		int box_type = 0;
		if (!GetSubNodeValue(root_element, "box_type", box_type))
		{
			return -1;
		}

		RedBagCfg & cfg_info = m_red_box_type_map[box_type];
		cfg_info.box_type = box_type;

		int power = 0;
		if (!GetSubNodeValue(root_element, "rate", power)
			|| 0 >= power)
		{
			return -2;
		}

		RedBagCfg::ItemInfo &item_cfg = cfg_info.item_map[cfg_info.total_power];

		cfg_info.total_power += power;

		if (!GetSubNodeValue(root_element, "price_type", item_cfg.money_type)
			|| MONEY_TYPE_GOLD > item_cfg.num
			|| MONEY_TYPE_MAX <= item_cfg.num)
		{
			return -3;
		}
		if (!GetSubNodeValue(root_element, "num", item_cfg.num)
			|| 0 >= item_cfg.num)
		{
			return -4;
		}

		root_element = root_element->NextSiblingElement();
	}
	return 0;
}
