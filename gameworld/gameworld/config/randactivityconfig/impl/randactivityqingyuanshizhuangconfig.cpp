#include "randactivityqingyuanshizhuangconfig.hpp"

RandActivityQingYuanShiZhuangConfig::RandActivityQingYuanShiZhuangConfig()
{
}

RandActivityQingYuanShiZhuangConfig::~RandActivityQingYuanShiZhuangConfig()
{
	for (std::map<int, std::vector<RAQingYuanShiZhuangGiftCfg> >::iterator it = m_cfg_map.begin(); it!= m_cfg_map.end(); it++)
	{
		std::vector<RAQingYuanShiZhuangGiftCfg>().swap(it->second);
	}
}

bool RandActivityQingYuanShiZhuangConfig::Init(TiXmlElement * RootElement, std::string * err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("gift_configure", InitGiftCfg);
	LOAD_RA_CONFIG("other", InitOtherCfg);

	return true;
}

const RAQingYuanShiZhuangGiftCfg * RandActivityQingYuanShiZhuangConfig::GetGiftCfg(RandActivityManager * ramgr, int seq) const
{
	if (NULL == ramgr) return NULL;
	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_QING_YUAN_SHI_ZHUANG);

	std::map<int, std::vector<RAQingYuanShiZhuangGiftCfg> >::const_iterator it = m_cfg_map.upper_bound(act_real_open_day);
	if (it == m_cfg_map.begin())
		return NULL;
	it--;

	if (seq < 0 || seq >= (int)it->second.size())
		return NULL;

	return &it->second[seq];
}

int RandActivityQingYuanShiZhuangConfig::GetGiftSize(RandActivityManager * ramgr) const
{
	if (NULL == ramgr) return 0;
	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_QING_YUAN_SHI_ZHUANG);

	std::map<int, std::vector<RAQingYuanShiZhuangGiftCfg> >::const_iterator it = m_cfg_map.upper_bound(act_real_open_day);
	if (it == m_cfg_map.begin())
		return 0;
	it--;

	return it->second.size();
}

int RandActivityQingYuanShiZhuangConfig::InitGiftCfg(TiXmlElement * RootElement)
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

		RAQingYuanShiZhuangGiftCfg cfg;
		if (!GetSubNodeValue(root_element, "seq", cfg.seq) || cfg.seq < 0)
			return -2;
		if (!GetSubNodeValue(root_element, "price_type", cfg.price_type) || cfg.price_type < 0)
			return -3;
		if (!GetSubNodeValue(root_element, "price", cfg.price) || cfg.price <= 0)
			return -4;

		TiXmlElement * item_element = root_element->FirstChildElement("reward_item");
		if (NULL == item_element)
			return -1000;

		if (!cfg.reward_item.ReadConfig(item_element))
			return -5;

		m_cfg_map[section_start].push_back(cfg);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityQingYuanShiZhuangConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -10000;
	}

	

	return 0;
}
