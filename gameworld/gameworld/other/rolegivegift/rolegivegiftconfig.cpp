#include "rolegivegiftconfig.hpp"

RoleGiveGiftConfig::RoleGiveGiftConfig()
{
}

RoleGiveGiftConfig::~RoleGiveGiftConfig()
{
}

bool RoleGiveGiftConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG("give_item", InitRoleGiveGiftCfg);
	LOAD_CONFIG("give_times", InitRoleGiveTimesCfg);

	return true;

}

const RoleGiveGiftCfg * RoleGiveGiftConfig::GetRoleGiveGiftCfg(int item_id)
{
	return MapValuesConstPtr(m_give_gift_cfg, item_id);
}

const RoleGiveTimesCfg * RoleGiveGiftConfig::GetRoleGiveTimesCfg(int times_group_id)
{
	return MapValuesConstPtr(m_give_times_cfg, times_group_id);
}

int RoleGiveGiftConfig::InitRoleGiveGiftCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	int next_seq = 0;
	while (NULL != root_element)
	{
		RoleGiveGiftCfg cfg;
		if (!GetSubNodeValue(root_element, "times_group_id", cfg.times_group_id) || cfg.times_group_id <= 0)
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(root_element, "item_id", cfg.item_id))
		{
			return -__LINE__;
		}

		m_give_gift_cfg[cfg.item_id] = cfg;
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int RoleGiveGiftConfig::InitRoleGiveTimesCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	int next_seq = 0;
	while (NULL != root_element)
	{
		RoleGiveTimesCfg cfg;
		if (!GetSubNodeValue(root_element, "times_group_id", cfg.times_group_id) || cfg.times_group_id <= 0)
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(root_element, "give_times", cfg.give_times))
		{
			return -__LINE__;
		}

		int is_day_return = 0;
		if (!GetSubNodeValue(root_element, "is_return", is_day_return) || is_day_return < 0)
		{
			return -__LINE__;
		}
		cfg.is_day_return = (0 != is_day_return) ? true : false;

		m_give_times_cfg[cfg.times_group_id] = cfg;
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}
