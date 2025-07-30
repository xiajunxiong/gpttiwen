#include "randactivitycumulativerechargeconfig.hpp"

RandActivityCumulativeRechargeConfig::RandActivityCumulativeRechargeConfig()
{

}

RandActivityCumulativeRechargeConfig::~RandActivityCumulativeRechargeConfig()
{

}

bool RandActivityCumulativeRechargeConfig::Init(TiXmlElement *RootElement, std::string *err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("other", InitOtherCfg);
	LOAD_RA_CONFIG("gift_configure", InitCumulativeRechargeCfg);

	return true;
}

const CumulativeRechargeCfg * RandActivityCumulativeRechargeConfig::GetRewardCfg(int open_day)const
{
	for (std::map<int, CumulativeRechargeCfg>::const_iterator it = m_cfg_map.begin(); it != m_cfg_map.end(); it++)
	{
		if (open_day >= it->second.section_start && open_day <= it->second.section_end)
		{
			return &it->second;
		}
	}
	return NULL;
}

int RandActivityCumulativeRechargeConfig::GetConfigVer() const
{
	return m_other_cfg.cfg_ver;
}

int RandActivityCumulativeRechargeConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;

	if (!GetSubNodeValue(root_element, "cfg_ver", m_other_cfg.cfg_ver) || m_other_cfg.cfg_ver <= 0) return -1;

	TiXmlElement * reward_list_element = root_element->FirstChildElement("free_reward_item_list");
	if (NULL == reward_list_element) return -544;

	TiXmlElement * reward_element = reward_list_element->FirstChildElement("free_reward_item");
	if (NULL == reward_element) return -545;

	while (NULL != reward_element)
	{
		ItemConfigData item;
		if (!item.ReadConfig(reward_element))
		{
			return -546;
		}

		m_other_cfg.free_reward.push_back(item);
		reward_element = reward_element->NextSiblingElement();
	}



	return 0;
}

int RandActivityCumulativeRechargeConfig::InitCumulativeRechargeCfg(TiXmlElement *RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;

	while (NULL != root_element)
	{
		int type = 0;
		if (!GetSubNodeValue(root_element, "type", type) || type < 0) return -1;

		if (m_cfg_map.find(type) != m_cfg_map.end())
		{
			return -33;
		}

		CumulativeRechargeCfg &node_cfg = m_cfg_map[type];
		node_cfg.type = type;
		if (!GetSubNodeValue(root_element, "section_start", node_cfg.section_start)) return -2;
		if (!GetSubNodeValue(root_element, "section_end", node_cfg.section_end)) return -3;
		if (node_cfg.section_end <= 0)
		{
			node_cfg.section_end = MAX_INT;
		}
		if (!GetSubNodeValue(root_element, "acc_price", node_cfg.acc_price) || node_cfg.acc_price < 0) return -4;
		if (!GetSubNodeValue(root_element, "is_borcast", node_cfg.is_broadcast)) return -5;
		if (!GetSubNodeValue(root_element, "buy_money", node_cfg.buy_money) || node_cfg.buy_money < 0) return -6;

		//累计充值时直购金额为0,使用直购金额时,直购金额需要大于0
		if (node_cfg.acc_price == 0)	
		{
			if(node_cfg.buy_money <= 0) return -7;
		}
		else
		{
			if (node_cfg.buy_money != 0) return -8;
		}

		TiXmlElement *item_list_element = root_element->FirstChildElement("reward_item_list");
		if (item_list_element == NULL) return -9;
		TiXmlElement *item_element = item_list_element->FirstChildElement("reward_item");
		while (item_element != NULL)
		{
			ItemConfigData reward_cfg;
			if (!reward_cfg.ReadConfig(item_element))
			{
				return -10;
			}
			node_cfg.m_reward_vec.push_back(reward_cfg);
			item_element = item_element->NextSiblingElement();
		}
		root_element = root_element->NextSiblingElement();
	}
	return 0;
}