#include "randactivityyinhuwenshiconfig.hpp"

#include "item/itembase.h"

RandActivityYinHuWenShiConfig::RandActivityYinHuWenShiConfig()
{
}

RandActivityYinHuWenShiConfig::~RandActivityYinHuWenShiConfig()
{
	for (std::map<short, std::vector<short> >::iterator it = m_gift_cfg.begin(); it!= m_gift_cfg.end(); it++)
	{
		std::vector<short>().swap(it->second);
	}
}

bool RandActivityYinHuWenShiConfig::Init(TiXmlElement * RootElement, std::string * err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("gift_configure", InitGiftCfg);
	LOAD_RA_CONFIG("gift_group", InitGroupCfg);
	LOAD_RA_CONFIG("buy_configure", InitBuyCfg);

	return true;
}

const RAYinHuWenShiGroupCfg * RandActivityYinHuWenShiConfig::GetGiftCfg(RandActivityManager * ramgr, int seq) const
{
	if (NULL == ramgr) return NULL;
	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_YINHU_WENSHI);

	std::map<short, std::vector<short> >::const_iterator gift_it = m_gift_cfg.upper_bound(act_real_open_day);
	if (gift_it == m_gift_cfg.begin())
		return NULL;
	gift_it--;

	if (seq < 0 || seq >= (int)gift_it->second.size())
		return NULL;

	int group = gift_it->second[seq];
	std::map<short, RAYinHuWenShiGroupMap>::const_iterator group_it = m_group_cfg.find(group);
	if (group_it == m_group_cfg.end())
		return NULL;

	int rate = RandomNum(group_it->second.max_rate);
	std::map<int, RAYinHuWenShiGroupCfg>::const_iterator it = group_it->second.map_cfg.upper_bound(rate);
	if (it == group_it->second.map_cfg.end())
		return NULL;

	return &it->second;
}

const RAYinHuWenShiBuyCfg * RandActivityYinHuWenShiConfig::GetBuyCfg(RandActivityManager * ramgr) const
{
	if (NULL == ramgr) return NULL;
	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_YINHU_WENSHI);

	std::map<short, RAYinHuWenShiBuyCfg>::const_iterator it = m_buy_cfg.upper_bound(act_real_open_day);
	if (it == m_buy_cfg.begin())
		return NULL;
	it--;

	return &it->second;
}

int RandActivityYinHuWenShiConfig::GetGiftMax(RandActivityManager * ramgr) const
{
	if (NULL == ramgr) return NULL;
	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_YINHU_WENSHI);

	std::map<short, std::vector<short> >::const_iterator gift_it = m_gift_cfg.upper_bound(act_real_open_day);
	if (gift_it == m_gift_cfg.begin())
		return NULL;
	gift_it--;

	return gift_it->second.size();
}

int RandActivityYinHuWenShiConfig::InitGiftCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -10000;
	}

	while (NULL != data_element)
	{
		int section_start = 0;
		if (!GetSubNodeValue(data_element, "section_start", section_start) || section_start <= 0)
			return -1;

		int reward_group = 0;
		if (!GetSubNodeValue(data_element, "reward_group", reward_group) || reward_group <= 0)
			return -2;

		m_gift_cfg[section_start].push_back(reward_group);
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityYinHuWenShiConfig::InitGroupCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -10000;
	}

	while (NULL != data_element)
	{
		int reward_group = 0;
		if (!GetSubNodeValue(data_element, "reward_group", reward_group) || reward_group <= 0)
			return -1;

		RAYinHuWenShiGroupMap * map_cfg = &m_group_cfg[reward_group];
		int rate = 0;
		if (!GetSubNodeValue(data_element, "rate", rate) || rate <= 0)
			return -2;

		RAYinHuWenShiGroupCfg cfg;
		if (!GetSubNodeValue(data_element, "type", cfg.type) || cfg.type < 0)
			return -3;

		TiXmlElement * list_element = data_element->FirstChildElement("reward_item_list");
		if (NULL == list_element)
			return -1000;

		TiXmlElement * item_element = list_element->FirstChildElement("reward_item");
		if (NULL == item_element)
			return -2000;

		while (NULL != item_element)
		{
			ItemConfigData item;
			if (!item.ReadConfig(item_element))
				return -2001;
		
			cfg.reward.push_back(item);
			item_element = item_element->NextSiblingElement();
		}

		int hearsay = 0;
		if (!GetSubNodeValue(data_element, "hearsay", hearsay))
			return -4;

		cfg.is_hearsay = hearsay != 0;

		map_cfg->max_rate += rate;
		map_cfg->map_cfg[map_cfg->max_rate] = cfg;

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityYinHuWenShiConfig::InitBuyCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -10000;
	}

	while (NULL != data_element)
	{
		int section_start = 0;
		if (!GetSubNodeValue(data_element, "section_start", section_start) || section_start <= 0)
			return -1;

		RAYinHuWenShiBuyCfg cfg;
		TiXmlElement * item_element = data_element->FirstChildElement("reward_item");
		if (NULL == item_element)
			return -1000;

		if (!cfg.reward_item.ReadConfig(item_element))
			return -2;

		if (!GetSubNodeValue(data_element, "price", cfg.price) || cfg.price <= 0)
			return -3;

		m_buy_cfg[section_start] = cfg;
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}
