#include "randactivityciniuyingxinhuconfig.hpp"
#include "item/itembase.h"

RandActivityCiNiuYingXinHuConfig::RandActivityCiNiuYingXinHuConfig()
{
}

RandActivityCiNiuYingXinHuConfig::~RandActivityCiNiuYingXinHuConfig()
{
	for (std::map<short, std::vector<RACiNiuYingXinHuBasisCfg> >::iterator it = m_basis_cfg.begin(); it != m_basis_cfg.end(); it++)
	{
		std::vector<RACiNiuYingXinHuBasisCfg>().swap(it->second);
	}

	for (std::map<short, std::map<short, std::vector<RACiNiuYingXinHuGiftCfg> > >::iterator it = m_gift_cfg.begin(); it != m_gift_cfg.end(); it++){
		for (std::map<short, std::vector<RACiNiuYingXinHuGiftCfg> >::iterator nit = it->second.begin(); nit != it->second.end(); nit++){
			std::vector<RACiNiuYingXinHuGiftCfg>().swap(nit->second);
		}
	}
}

bool RandActivityCiNiuYingXinHuConfig::Init(TiXmlElement * RootElement, std::string * err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("basis_configure", InitBasisCfg);
	LOAD_RA_CONFIG("gift_configure", InitGiftCfg);
	LOAD_RA_CONFIG("gift_group", InitGroupCfg);
	LOAD_RA_CONFIG("other", InitOtherCfg);

	return true;
}

const RACiNiuYingXinHuBasisCfg * RandActivityCiNiuYingXinHuConfig::GetBasisCfg(RandActivityManager * ramgr, int seq) const
{
	if (NULL == ramgr) return NULL;
	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_CINIU_YINGXINHU);

	std::map<short, std::vector<RACiNiuYingXinHuBasisCfg> >::const_iterator it = m_basis_cfg.upper_bound(act_real_open_day);
	if (it == m_basis_cfg.begin())
		return NULL;
	it--;

	if (seq < 0 || seq >= (int)it->second.size())
		return NULL;

	return &it->second[seq];
}

const RACiNiuYingXinHuGiftCfg * RandActivityCiNiuYingXinHuConfig::GetGiftCfg(RandActivityManager * ramgr, int module_type, int seq) const
{
	if (NULL == ramgr) return NULL;
	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_CINIU_YINGXINHU);

	std::map<short, std::map<short, std::vector<RACiNiuYingXinHuGiftCfg> > >::const_iterator day_it = m_gift_cfg.upper_bound(act_real_open_day);
	if (day_it == m_gift_cfg.begin())
		return NULL;
	day_it--;

	std::map<short, std::vector<RACiNiuYingXinHuGiftCfg> >::const_iterator type_it = day_it->second.find(module_type);
	if (type_it == day_it->second.end())
		return NULL;

	if (seq < 0 || seq >= (int)type_it->second.size())
		return NULL;

	return &type_it->second[seq];
}

int RandActivityCiNiuYingXinHuConfig::GetGiftMaxCount(RandActivityManager * ramgr, int module_type) const
{
	if (NULL == ramgr) return 0;
	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_CINIU_YINGXINHU);

	std::map<short, std::map<short, std::vector<RACiNiuYingXinHuGiftCfg> > >::const_iterator day_it = m_gift_cfg.upper_bound(act_real_open_day);
	if (day_it == m_gift_cfg.begin())
		return 0;
	day_it--;

	std::map<short, std::vector<RACiNiuYingXinHuGiftCfg> >::const_iterator type_it = day_it->second.find(module_type);
	if (type_it == day_it->second.end())
		return 0;

	return (int)type_it->second[(int)type_it->second.size() - 1].param;
}

int RandActivityCiNiuYingXinHuConfig::GetGiftMax(RandActivityManager * ramgr, int module_type) const
{
	if (NULL == ramgr) return 0;
	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_CINIU_YINGXINHU);

	std::map<short, std::map<short, std::vector<RACiNiuYingXinHuGiftCfg> > >::const_iterator day_it = m_gift_cfg.upper_bound(act_real_open_day);
	if (day_it == m_gift_cfg.begin())
		return 0;
	day_it--;

	std::map<short, std::vector<RACiNiuYingXinHuGiftCfg> >::const_iterator type_it = day_it->second.find(module_type);
	if (type_it == day_it->second.end())
		return 0;

	return (int)type_it->second.size();
}

const RACiNiuYingXinHuGroup * RandActivityCiNiuYingXinHuConfig::GetGroupCfg(int group) const
{
	std::map<short, RACiNiuYingXinHuGroupMap>::const_iterator it = m_group_cfg.find(group);
	if (it == m_group_cfg.end())
		return NULL;

	int rate = RandomNum(it->second.max_rate);
	std::map<int, RACiNiuYingXinHuGroup>::const_iterator item_it = it->second.item_map.upper_bound(rate);
	if (item_it == it->second.item_map.end())
		return NULL;

	return &item_it->second;
}

int RandActivityCiNiuYingXinHuConfig::InitBasisCfg(TiXmlElement * RootElement)
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

		RACiNiuYingXinHuBasisCfg cfg;
		if (!GetSubNodeValue(data_element, "activity_days", cfg.activity_days) || cfg.activity_days <= 0)
			return -2;

		if (!GetSubNodeValue(data_element, "awaken_reward_group", cfg.awaken_reward_group) || cfg.awaken_reward_group <= 0)
			return -3;

		m_basis_cfg[section_start].push_back(cfg);
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityCiNiuYingXinHuConfig::InitGiftCfg(TiXmlElement * RootElement)
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

		int module_type = 0;
		if (!GetSubNodeValue(data_element, "module_type", module_type) || module_type < 0)
			return -2;

		RACiNiuYingXinHuGiftCfg cfg;
		if (!GetSubNodeValue(data_element, "parameter", cfg.param) || cfg.param < 0)
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
				return -4;

			cfg.reward.push_back(item);
			item_element = item_element->NextSiblingElement();
		}

		m_gift_cfg[section_start][module_type].push_back(cfg);
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityCiNiuYingXinHuConfig::InitGroupCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -10000;
	}

	while (NULL != data_element)
	{
		int group = 0;
		if (!GetSubNodeValue(data_element, "reward_group", group) || group <= 0)
			return -1;

		RACiNiuYingXinHuGroupMap &map_cfg = m_group_cfg[group];

		RACiNiuYingXinHuGroup cfg;
		int rate = 0;
		if (!GetSubNodeValue(data_element, "rate", rate) || rate <= 0)
			return -2;
		map_cfg.max_rate += rate;

		TiXmlElement * item_element = data_element->FirstChildElement("reward_item");
		if (NULL == item_element)
			return -1000;

		if (!cfg.item.ReadConfig(item_element))
			return -3;

		int hearsay = 0;
		if (!GetSubNodeValue(data_element, "hearsay", hearsay))
			return -4;

		cfg.is_hearsay = 0 != hearsay;
		map_cfg.item_map[map_cfg.max_rate] = cfg;
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityCiNiuYingXinHuConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -10000;
	}

	if (!GetSubNodeValue(data_element, "submit_item", m_other_cfg.item_id) || NULL == ITEMPOOL->GetItem(m_other_cfg.item_id))
		return -1;

	if (!GetSubNodeValue(data_element, "item_num", m_other_cfg.item_num) || m_other_cfg.item_num <= 0)
		return -2;

	if (!GetSubNodeValue(data_element, "add_tiger_air", m_other_cfg.add_tiger_air) || m_other_cfg.add_tiger_air <= 0)
		return -3;

	return 0;
}
