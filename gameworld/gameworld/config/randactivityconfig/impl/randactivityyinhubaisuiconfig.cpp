#include "randactivityyinhubaisuiconfig.hpp"

#include "item/itembase.h"

RandActivityYinHuBaiSuiConfig::RandActivityYinHuBaiSuiConfig()
{
}

RandActivityYinHuBaiSuiConfig::~RandActivityYinHuBaiSuiConfig()
{
	for (std::map<short, std::vector<RAYinHuBaiSuiGiftCfg> >::iterator it = m_gift_cfg.begin(); it!= m_gift_cfg.end(); it++)
	{
		std::vector<RAYinHuBaiSuiGiftCfg>().swap(it->second);
	}
}

bool RandActivityYinHuBaiSuiConfig::Init(TiXmlElement * RootElement, std::string * err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("gift_configure", InitGiftCfg);
	LOAD_RA_CONFIG("buy_configure", InitBuyCfg);

	return true;
}

const RAYinHuBaiSuiGiftCfg * RandActivityYinHuBaiSuiConfig::GetGiftCfg(RandActivityManager * ramgr, int seq) const
{
	if (NULL == ramgr) return NULL;
	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_YIN_HU_BAI_SUI);

	std::map<short, std::vector<RAYinHuBaiSuiGiftCfg> >::const_iterator gift_it = m_gift_cfg.upper_bound(act_real_open_day);
	if (gift_it == m_gift_cfg.begin())
		return NULL;
	gift_it--;

	if (seq < 0 || seq >= (int)gift_it->second.size())
		return NULL;

	return &gift_it->second[seq];
}

const RAYinHuBaiSuiBuyCfg * RandActivityYinHuBaiSuiConfig::GetBuyCfg(RandActivityManager * ramgr) const
{
	if (NULL == ramgr) return NULL;
	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_YIN_HU_BAI_SUI);

	std::map<short, RAYinHuBaiSuiBuyCfg>::const_iterator it = m_buy_cfg.upper_bound(act_real_open_day);
	if (it == m_buy_cfg.begin())
		return NULL;
	it--;

	return &it->second;
}

int RandActivityYinHuBaiSuiConfig::GetGiftMax(RandActivityManager * ramgr) const
{
	if (NULL == ramgr) return NULL;
	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_YIN_HU_BAI_SUI);

	std::map<short, std::vector<RAYinHuBaiSuiGiftCfg> >::const_iterator gift_it = m_gift_cfg.upper_bound(act_real_open_day);
	if (gift_it == m_gift_cfg.begin())
		return NULL;
	gift_it--;

	return gift_it->second.size();
}

int RandActivityYinHuBaiSuiConfig::InitGiftCfg(TiXmlElement * RootElement)
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

		RAYinHuBaiSuiGiftCfg cfg;
		if (!GetSubNodeValue(data_element, "activity_days", cfg.activity_days) || cfg.activity_days <= 0)
			return -2;

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

			cfg.item.push_back(item);
			item_element = item_element->NextSiblingElement();
		}

		m_gift_cfg[section_start].push_back(cfg);
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityYinHuBaiSuiConfig::InitBuyCfg(TiXmlElement * RootElement)
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

		RAYinHuBaiSuiBuyCfg cfg;
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
