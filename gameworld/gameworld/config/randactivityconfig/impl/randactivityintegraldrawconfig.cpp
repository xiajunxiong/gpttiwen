#include "randactivityintegraldrawconfig.hpp"

RandActivityIntegralDrawConfig::RandActivityIntegralDrawConfig()
{
}

RandActivityIntegralDrawConfig::~RandActivityIntegralDrawConfig()
{
	for (std::map<int, std::vector<RAIntegralDrawRewardCfg> >::iterator it = m_reward_cfg.begin(); it != m_reward_cfg.end(); it++)
	{
		std::vector<RAIntegralDrawRewardCfg>().swap(it->second);
	}
}

bool RandActivityIntegralDrawConfig::Init(TiXmlElement * RootElement, std::string * err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("basis_configuration", InitRewardCfg);
	LOAD_RA_CONFIG("other", InitOtherCfg);

	return true;
}

const RAIntegralDrawRewardCfg * RandActivityIntegralDrawConfig::GetRewardCfg(RandActivityManager * ramgr, int seq) const
{
	if (NULL == ramgr) return NULL;
	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_HORCRUX_TIANCHENG);

	std::map<int, std::vector<RAIntegralDrawRewardCfg> >::const_iterator reward_it = m_reward_cfg.upper_bound(act_real_open_day);
	if (reward_it == m_reward_cfg.begin())
		return NULL;
	reward_it--;

	if (seq < 0 || seq >= (int)reward_it->second.size())
		return NULL;

	return &reward_it->second[seq];
}

int RandActivityIntegralDrawConfig::InitRewardCfg(TiXmlElement * RootElement)
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

		RAIntegralDrawRewardCfg cfg;
		if (!GetSubNodeValue(root_element, "seq", cfg.seq) || cfg.seq < 0 || cfg.seq >= MAX_RA_INTEGRAL_DRAW_LINE_NUM)
			return -2;

		if (!GetSubNodeValue(root_element, "color_num", cfg.color_num) || cfg.color_num <= 0)
			return -3;

		int is_bor = 0;
		if (!GetSubNodeValue(root_element, "is_borcast", is_bor) || is_bor < 0)
			return -4;

		cfg.is_borcast = 0 != is_bor;

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

		m_reward_cfg[section_start].push_back(cfg);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityIntegralDrawConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -10000;
	}

	if (!GetSubNodeValue(root_element, "day_times", m_other_cfg.day_times) || m_other_cfg.day_times <= 0)
		return -1;

	return 0;
}
