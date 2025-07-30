#include "mirageterritoryconfig.hpp"
#include "servercommon/mirageterritorydef.h"

MirageTerritoryConfig::MirageTerritoryConfig()
{
}

MirageTerritoryConfig::~MirageTerritoryConfig()
{
	std::vector<MirageTerritoryChalleCfg>().swap(m_challenge_vec);
	std::vector<MirageTerritoryRewardCfg>().swap(m_reward_cfg);
	std::vector<MirageTerritoryAreaCfg>().swap(m_area_cfg);
	std::vector<PartnerArmChipCfg>().swap(m_partner_reward_vec);
}

bool MirageTerritoryConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG_2("challenge", InitChallengeCfg);
	LOAD_CONFIG_2("other", InitOtherCfg);
	LOAD_CONFIG_2("reward", InitRewardCfg);
	LOAD_CONFIG_2("area", InitAreaCfg);
	LOAD_CONFIG_2("partner_arm_chip", InitPartnerRewardCfg);

	return true;
}

const MirageTerritoryAreaCfg * MirageTerritoryConfig::GetAreaCfg(int seq)
{
	for (std::vector<MirageTerritoryAreaCfg>::iterator it = m_area_cfg.begin(); it != m_area_cfg.end(); it++)
	{
		if (it->seq == seq)
		{
			return &(*it);
		}
	}
	return NULL;
}

const MirageTerritoryChalleCfg * MirageTerritoryConfig::GetChallengeCfg(int area, int num)
{
	for (std::vector<MirageTerritoryChalleCfg>::iterator it = m_challenge_vec.begin(); it != m_challenge_vec.end(); it++)
	{
		if (it->num == num && it->area == area)
		{
			return &(*it);
		}
	}

	return NULL;
}

const MirageTerritoryRewardCfg * MirageTerritoryConfig::GetRewardCfg(int seq)
{
	for (std::vector<MirageTerritoryRewardCfg>::iterator it = m_reward_cfg.begin(); it != m_reward_cfg.end(); it++)
	{
		if (it->seq == seq)
		{
			return &(*it);
		}
	}
	return NULL;
}

const PartnerArmChipCfg * MirageTerritoryConfig::GetParnterRewardCfg(int partner_id)
{
	for (std::vector<PartnerArmChipCfg>::iterator it = m_partner_reward_vec.begin(); it != m_partner_reward_vec.end(); it++)
	{
		if (it->partner_id == partner_id)
		{
			return &(*it);
		}
	}
	return NULL;
}

int MirageTerritoryConfig::InitChallengeCfg(TiXmlElement * RootElement, std::string& errormsg)
{
	TiXmlElement* dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -10000;

	while (dataElement)
	{
		MirageTerritoryChalleCfg node;

		if (!GetSubNodeValue(dataElement, "seq", node.seq) || node.seq < 0)
		{
			errormsg = STRING_SPRINTF("error is [node.seq <= 0]");
			return -1;
		}

		if (!GetSubNodeValue(dataElement, "area", node.area) || node.area < 0)
		{
			errormsg = STRING_SPRINTF("error is [node.area < 0]");
			return -2;
		}

		if (!GetSubNodeValue(dataElement, "num", node.num) || node.num <= 0)
		{
			errormsg = STRING_SPRINTF("error is [node.num <= 0]");
			return -2;
		}

		if (!GetSubNodeValue(dataElement, "reward_id", node.reward_id) || node.reward_id < 0)
		{
			errormsg = STRING_SPRINTF("error is [node.reward_id < 0]");
			return -3;
		}

		if (!GetSubNodeValue(dataElement, "monster_group", node.monster_group) || node.monster_group < 0)
		{
			errormsg = STRING_SPRINTF("error is [node.monster_group < 0]");
			return -4;
		}

		m_challenge_vec.push_back(node);
		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int MirageTerritoryConfig::InitOtherCfg(TiXmlElement * RootElement, std::string& errormsg)
{
	TiXmlElement* dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -10000;

	if (!GetSubNodeValue(dataElement, "open_level", m_other_cfg.open_level) || m_other_cfg.open_level <= 0 || m_other_cfg.open_level > MAX_ROLE_LEVEL)
	{
		errormsg = STRING_SPRINTF("error is [open_level > %d] or [open_level <= 0]", MAX_ROLE_LEVEL);
		return -1;
	}

	if (!GetSubNodeValue(dataElement, "partner_num", m_other_cfg.partner_num) || m_other_cfg.partner_num < 0 || m_other_cfg.partner_num > MAX_MIRAGE_TERRITORY_PARTNER_NUM)
	{
		errormsg = STRING_SPRINTF("error is [partner_num < 0] OR [partner_num > %d]", MAX_MIRAGE_TERRITORY_PARTNER_NUM);
		return -2;
	}

	if (!GetSubNodeValue(dataElement, "dispatch_time", m_other_cfg.dispatch_time) || m_other_cfg.dispatch_time < 0)
	{
		errormsg = STRING_SPRINTF("error is [dispatch_time < 0]");
		return -3;
	}

	if (!GetSubNodeValue(dataElement, "dispatch_num", m_other_cfg.dispatch_num) || m_other_cfg.dispatch_num < 0 || m_other_cfg.dispatch_num > MAX_MIRAGE_TERRITORY_PARTNER_NUM)
	{
		errormsg = STRING_SPRINTF("error is [dispatch_num < 0] OR [partner_num > %d]", MAX_MIRAGE_TERRITORY_PARTNER_NUM);
		return -4;
	}

	return 0;
}

int MirageTerritoryConfig::InitRewardCfg(TiXmlElement * RootElement, std::string& errormsg)
{
	TiXmlElement* dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -10000;
	while (dataElement)
	{
		int seq = -1;
		if (!GetSubNodeValue(dataElement, "seq", seq) || seq < 0)
		{
			errormsg = STRING_SPRINTF("error is [seq < 0]");
			return -1;
		}
		MirageTerritoryRewardCfg node_group;
		node_group.seq = seq;

		{
			TiXmlElement * reward_list_element = dataElement->FirstChildElement("reward_list");

			if (NULL != reward_list_element)
			{
				TiXmlElement * reward_element = reward_list_element->FirstChildElement("reward");
				while (reward_element != NULL)
				{
					ItemConfigData node_item;
					if (!node_item.ReadConfig(reward_element))
					{
						errormsg = STRING_SPRINTF("error is reward ReadConfig fail");
						return -3;
					}
					node_group.reward_vec.push_back(node_item);
					reward_element = reward_element->NextSiblingElement();
				}
			}
		}

		{
			TiXmlElement * reward_list_element = dataElement->FirstChildElement("dispatch_reward_list");

			if (NULL != reward_list_element)
			{
				TiXmlElement * reward_element = reward_list_element->FirstChildElement("dispatch_reward");
				while (reward_element != NULL)
				{
					ItemConfigData node_item;
					if (!node_item.ReadConfig(reward_element))
					{
						errormsg = STRING_SPRINTF("error is reward ReadConfig fail");
						return -3;
					}
					node_group.pass_reward_vec.push_back(node_item);
					reward_element = reward_element->NextSiblingElement();
				}
			}
		}

		if (!GetSubNodeValue(dataElement, "arm_chip_num", node_group.arm_chip_num) || node_group.arm_chip_num < 0)
		{
			errormsg = STRING_SPRINTF("error is [arm_chip_num < 0]");
			return -2;
		}

		m_reward_cfg.push_back(node_group);
		dataElement = dataElement->NextSiblingElement();
	}
	return 0;
}

int MirageTerritoryConfig::InitAreaCfg(TiXmlElement * RootElement, std::string& errormsg)
{
	TiXmlElement* dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -10000;
	while (dataElement)
	{
		MirageTerritoryAreaCfg node_group;
		if (!GetSubNodeValue(dataElement, "seq", node_group.seq) || node_group.seq < 0 || node_group.seq >= MirageTerritoryAreaCfg::MIRAGE_TERRITORY_AREA_TYPE_MAX)
		{
			errormsg = STRING_SPRINTF("error is [seq < 0]");
			return -1;
		}

		if (!GetSubNodeValue(dataElement, "job1", node_group.job1) || node_group.job1 < 0)
		{
			errormsg = STRING_SPRINTF("error is [job1 < 0]");
			return -2;
		}

		if (!GetSubNodeValue(dataElement, "job2", node_group.job2) || node_group.job2 < 0)
		{
			errormsg = STRING_SPRINTF("error is [job2 < 0]");
			return -3;
		}
		
		m_area_cfg.push_back(node_group);
		dataElement = dataElement->NextSiblingElement();
	}
	return 0;
}

int MirageTerritoryConfig::InitPartnerRewardCfg(TiXmlElement * RootElement, std::string & errormsg)
{
	TiXmlElement* dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -10000;
	while (dataElement)
	{
		PartnerArmChipCfg node;
		if (!GetSubNodeValue(dataElement, "partner_id", node.partner_id) || node.partner_id < 0)
		{
			errormsg = STRING_SPRINTF("error is [seq < 0]");
			return -1;
		}

		if (!GetSubNodeValue(dataElement, "arms_id", node.arms_id) || node.arms_id < 0)
		{
			errormsg = STRING_SPRINTF("error is [job1 < 0]");
			return -2;
		}

		m_partner_reward_vec.push_back(node);
		dataElement = dataElement->NextSiblingElement();
	}
	return 0;
}
