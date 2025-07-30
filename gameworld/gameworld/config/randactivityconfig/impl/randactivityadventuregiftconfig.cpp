#include "randactivityadventuregiftconfig.hpp"

RandActivityAdventureGiftConfig::RandActivityAdventureGiftConfig()
{
}

RandActivityAdventureGiftConfig::~RandActivityAdventureGiftConfig()
{
	for (std::map<int, std::vector<RAAdventureGiftItemCfg> >::iterator it = m_gift_cfg.begin(); it != m_gift_cfg.end(); it++)
	{
		std::vector<RAAdventureGiftItemCfg>().swap(it->second);
	}
}


bool RandActivityAdventureGiftConfig::Init(TiXmlElement *RootElement, std::string * err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("other", InitOtherCfg);
	LOAD_RA_CONFIG("adventure_gift", InitAdventureGiftCfg);

	return true;
}

const RAAdventureGiftItemCfg * RandActivityAdventureGiftConfig::CheckAdventureGift(int type, int old_value, int curr_value) const
{
	std::map<int, std::vector<RAAdventureGiftItemCfg> >::const_iterator iter = m_gift_cfg.find(type);
	if (iter == m_gift_cfg.end())
	{
		return NULL;
	}

	const std::vector<RAAdventureGiftItemCfg> & gift_list = iter->second;

	for (int i = (int)gift_list.size() - 1; i >= 0; --i)
	{
		if (old_value < gift_list[i].param2 && gift_list[i].param2 <= curr_value)
		{
			return &gift_list[i];
		}
	}

	return NULL;
}

const RAAdventureGiftItemCfg * RandActivityAdventureGiftConfig::GetAdventureGiftCfg(int seq) const
{
	for (std::map<int, std::vector<RAAdventureGiftItemCfg> >::const_iterator iter = m_gift_cfg.begin(); iter != m_gift_cfg.end(); ++iter)
	{
		const std::vector<RAAdventureGiftItemCfg> & gift_list = iter->second;

		for (int i = (int)gift_list.size() - 1; i >= 0; --i)
		{
			if (gift_list[i].seq == seq)
			{
				return &gift_list[i];
			}
		}
	}

	return NULL;
}

int RandActivityAdventureGiftConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -__LINE__;
	}

	if (!GetSubNodeValue(root_element, "time", m_other_cfg.during_time) || m_other_cfg.during_time <= 0)
	{
		return -__LINE__;
	}

	return 0;
}

int RandActivityAdventureGiftConfig::InitAdventureGiftCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -888;
	}

	int next_seq = 0;
	while (NULL != root_element)
	{
		RAAdventureGiftItemCfg cfg;

		cfg.during_time = m_other_cfg.during_time;

		if (cfg.during_time == 0)
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(root_element, "type", cfg.seq) || cfg.seq != next_seq)
		{
			return -__LINE__;
		}
		next_seq += 1;

		if (!GetSubNodeValue(root_element, "parameter1", cfg.param1))
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(root_element, "parameter2", cfg.param2))
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(root_element, "type_money", cfg.consume_type))
		{
			return -__LINE__;
		}


		if (!GetSubNodeValue(root_element, "price", cfg.consume_value) || cfg.consume_value <= 0)
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(root_element, "hearsay", cfg.is_broadcast))
		{
			return -__LINE__;
		}

		TiXmlElement * reward_element = root_element->FirstChildElement("item");
		if (!cfg.reward.ReadConfig(reward_element))
		{
			return -__LINE__;
		}

		m_gift_cfg[cfg.param1].push_back(cfg);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}
