#include "randactivityxinfulingchongconfig.hpp"

#include "servercommon/struct/moneyotherdef.h"

RandActivityXinFuLingChongConfig::RandActivityXinFuLingChongConfig()
{
}

RandActivityXinFuLingChongConfig::~RandActivityXinFuLingChongConfig()
{
}

bool RandActivityXinFuLingChongConfig::Init(TiXmlElement * RootElement, std::string * err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG_2("reward_bug", InitRewardCfg);

	return true;
}

int RandActivityXinFuLingChongConfig::GetConfigVer() const
{
	return m_reward_cfg.cfg_ver;
}

int RandActivityXinFuLingChongConfig::InitRewardCfg(TiXmlElement * RootElement, std::string & errormsg)
{
	TiXmlElement *data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -999;
	}

	if (!GetSubNodeValue(data_element, "price", m_reward_cfg.price) || m_reward_cfg.price <= 0)
	{
		errormsg = STRING_SPRINTF("price[%d] <= 0?", m_reward_cfg.price);
		return -1;
	}

	if (!GetSubNodeValue(data_element, "price_type", m_reward_cfg.price_type) )
	{
		errormsg = STRING_SPRINTF("price_type[%d] not find data?", m_reward_cfg.price_type);
		return -2;
	}

	int cfg_or = 0;
	if (!GetSubNodeValue(data_element, "cfg_or", cfg_or))
	{
		errormsg = STRING_SPRINTF("cfg_or[%d] not find data?", cfg_or);
		return -3;
	}

	m_reward_cfg.is_cmd_buy = (0 != cfg_or);
	if (!m_reward_cfg.is_cmd_buy)
	{
		if (m_reward_cfg.price_type < 0 || m_reward_cfg.price_type >= MONEY_TYPE_MAX)
		{
			errormsg = STRING_SPRINTF("cfg_or[%d], price_type[%d] < 0 or >= %d?", cfg_or, MONEY_TYPE_MAX);
			return -4;
		}
	}
	
	if (!GetSubNodeValue(data_element, "cfg_ver", m_reward_cfg.cfg_ver) || m_reward_cfg.cfg_ver < 0)
	{
		errormsg = STRING_SPRINTF("cfg_ver[%d] < 0?", m_reward_cfg.cfg_ver);
		return -5;
	}

	TiXmlElement *reward_list_element = data_element->FirstChildElement("reward_item_list");
	if (NULL == reward_list_element)
	{
		errormsg = STRING_SPRINTF("[reward_item_list] not find?");
		return -6;
	}
	TiXmlElement *reward_element = reward_list_element->FirstChildElement("reward_item");
	if (NULL == reward_element)
	{
		errormsg = STRING_SPRINTF("[reward_element] not find?");
		return -7;
	}
	while (NULL != reward_element)
	{
		ItemConfigData item;
		if (!item.ReadConfig(reward_element))
		{
			return -8;
		}
		m_reward_cfg.reward_list.push_back(item);
		
		reward_element = reward_element->NextSiblingElement();
	}

	return 0;
}
