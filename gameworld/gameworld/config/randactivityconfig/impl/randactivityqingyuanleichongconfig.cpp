#include "randactivityqingyuanleichongconfig.hpp"

#include "global/randactivity/randactivitymanager.hpp"
#include "world.h"

RandActivityQingYuanLeiChongConfig::RandActivityQingYuanLeiChongConfig()
{
}

RandActivityQingYuanLeiChongConfig::~RandActivityQingYuanLeiChongConfig()
{
	for (std::map<int, std::vector<RAQingYuanLeiChongCfg> >::iterator it = m_cfg_map.begin(); it != m_cfg_map.end(); it++)
	{
		std::vector<RAQingYuanLeiChongCfg>().swap(it->second);
	}
}

bool RandActivityQingYuanLeiChongConfig::Init(TiXmlElement * RootElement, std::string * err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("gift_configure", InitCfg);
	LOAD_RA_CONFIG("other", InitOtherCfg);

	return true;
}

const RAQingYuanLeiChongCfg * RandActivityQingYuanLeiChongConfig::GetCfg(RandActivityManager * ramgr, int seq, unsigned int join_times) const
{
	if (NULL == ramgr) return NULL;
	int act_real_open_day = this->GetOpenDayByTimestamp(ramgr, join_times);

	std::map<int, std::vector<RAQingYuanLeiChongCfg> >::const_iterator it = m_cfg_map.upper_bound(act_real_open_day);
	if (it == m_cfg_map.begin())
		return NULL;
	it--;

	if (seq < 0 || seq >= (int)it->second.size())
		return NULL;

	return &it->second[seq];
}

int RandActivityQingYuanLeiChongConfig::GetGiftSize(RandActivityManager * ramgr, unsigned int join_times) const
{
	if (NULL == ramgr) return 0;
	int act_real_open_day = this->GetOpenDayByTimestamp(ramgr, join_times);

	std::map<int, std::vector<RAQingYuanLeiChongCfg> >::const_iterator it = m_cfg_map.upper_bound(act_real_open_day);
	if (it == m_cfg_map.begin())
		return 0;
	it--;

	return it->second.size();
}

int RandActivityQingYuanLeiChongConfig::InitCfg(TiXmlElement * RootElement)
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

		RAQingYuanLeiChongCfg cfg;
		if (!GetSubNodeValue(root_element, "seq", cfg.seq) || cfg.seq < 0)
			return -2;
		if (!GetSubNodeValue(root_element, "acc_price", cfg.acc_price) || cfg.acc_price <= 0)
			return -3;

		TiXmlElement * list_element = root_element->FirstChildElement("reward_item_list");
		if (NULL == list_element)
			return -1000;

		TiXmlElement * item_element = list_element->FirstChildElement("reward_item");
		while (NULL != item_element)
		{
			ItemConfigData item;
			if (!item.ReadConfig(item_element))
				return -4;

			cfg.reward_item.push_back(item);
			item_element = item_element->NextSiblingElement();
		}

		m_cfg_map[section_start].push_back(cfg);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityQingYuanLeiChongConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -10000;
	}

	if (!GetSubNodeValue(root_element, "continue_times", m_other_cfg.continue_times) || m_other_cfg.continue_times <= 0)
		return -1;
	if (!GetSubNodeValue(root_element, "reset_times", m_other_cfg.reset_times) || m_other_cfg.reset_times <= 0)
		return -2;

	return 0;
}
