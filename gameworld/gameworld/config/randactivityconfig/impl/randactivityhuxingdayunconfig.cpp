#include "randactivityhuxingdayunconfig.hpp"

#include "item/itembase.h"

RandActivityHuXingDaYunConfig::RandActivityHuXingDaYunConfig()
{
}

RandActivityHuXingDaYunConfig::~RandActivityHuXingDaYunConfig()
{
	for (std::map<short, std::vector<RAHuXingDaYunGiftCfg> >::iterator it = m_gift_cfg.begin(); it != m_gift_cfg.end(); it++)
	{
		std::vector<RAHuXingDaYunGiftCfg>().swap(it->second);
	}
}

bool RandActivityHuXingDaYunConfig::Init(TiXmlElement * RootElement, std::string * err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("gift_configure", InitGiftCfg);
	LOAD_RA_CONFIG("gift_group", InitGroupCfg);
	LOAD_RA_CONFIG("other", InitOtherCfg);

	return true;
}

const RAHuXingDaYunGiftCfg * RandActivityHuXingDaYunConfig::GetGiftCfg(RandActivityManager * ramgr, int seq) const
{
	if (NULL == ramgr) return NULL;
	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_HU_XING_DA_YUN);

	std::map<short, std::vector<RAHuXingDaYunGiftCfg> >::const_iterator day_it = m_gift_cfg.upper_bound(act_real_open_day);
	if (day_it == m_gift_cfg.begin())
		return NULL;
	day_it--;

	if (seq < 0 || seq >= (int)day_it->second.size())
		return NULL;

	return &day_it->second[seq];
}

const std::vector<ItemConfigData>* RandActivityHuXingDaYunConfig::GetGroupCfg(int group_id) const
{
	std::map<short, RAHuXingDaYunGroupMap>::const_iterator group_it = m_group_cfg.find(group_id);
	if (group_it == m_group_cfg.end())
		return NULL;

	int rate = RandomNum(group_it->second.max_rate);
	std::map<int, std::vector<ItemConfigData> >::const_iterator it = group_it->second.map_cfg.upper_bound(rate);
	if (it == group_it->second.map_cfg.end())
		return NULL;

	return &it->second;
}

int RandActivityHuXingDaYunConfig::InitGiftCfg(TiXmlElement * RootElement)
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

		RAHuXingDaYunGiftCfg cfg;
		TiXmlElement * list_element = data_element->FirstChildElement("first_reward_item_list");
		if (NULL == list_element)
			return -1000;

		TiXmlElement * item_element = list_element->FirstChildElement("first_reward_item");
		if (NULL == item_element)
			return -2000;

		while (NULL != item_element)
		{
			ItemConfigData item;
			if (!item.ReadConfig(item_element))
				return -2001;

			cfg.first_reward.push_back(item);
			item_element = item_element->NextSiblingElement();
		}

		if (!GetSubNodeValue(data_element, "repeat_reward_group", cfg.repeat_reward_group) || cfg.repeat_reward_group <= 0)
			return -2;
		if (!GetSubNodeValue(data_element, "be_invited_reward_group", cfg.be_invited_reward_group) || cfg.be_invited_reward_group <= 0)
			return -3;

		m_gift_cfg[section_start].push_back(cfg);
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityHuXingDaYunConfig::InitGroupCfg(TiXmlElement * RootElement)
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

		RAHuXingDaYunGroupMap * map_cfg = &m_group_cfg[reward_group];

		int rate = 0;
		if (!GetSubNodeValue(data_element, "rate", rate) || rate <= 0)
			return -2;

		map_cfg->max_rate += rate;
		std::vector<ItemConfigData> * cfg = &map_cfg->map_cfg[map_cfg->max_rate];

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

			cfg->push_back(item);
			item_element = item_element->NextSiblingElement();
		}

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityHuXingDaYunConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -10000;
	}

	if (!GetSubNodeValue(data_element, "be_invite_times", m_other_cfg.be_invite_times) || m_other_cfg.be_invite_times < 0)
		return -1;

	if (!GetSubNodeValue(data_element, "consume_times", m_other_cfg.consume_time) || m_other_cfg.consume_time <= 0)
		return -2;

	return 0;
}
