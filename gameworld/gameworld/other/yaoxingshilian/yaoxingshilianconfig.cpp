#include "yaoxingshilianconfig.hpp"

YaoXingShiLianConfig::YaoXingShiLianConfig()
{
}

YaoXingShiLianConfig::~YaoXingShiLianConfig()
{
	std::vector<YaoXingShiLianRewardCfg>().swap(m_reward_vec);
	std::vector<YaoXingShiLianReward2Cfg>().swap(m_reward2_vec);
}

bool YaoXingShiLianConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG("other", InitOtherCfg);
	LOAD_CONFIG("customs_pass", InitLevelCfg);
	LOAD_CONFIG("achievement1", InitAchievementCfg);
	LOAD_CONFIG("achievement2", InitAchievement2Cfg);

	return true;
}

const YaoXingShiLianLevelCfg * YaoXingShiLianConfig::GetLevelCfg(int seq)
{
	std::map<int, YaoXingShiLianLevelCfg>::iterator it = m_level_cfg.find(seq);
	if (m_level_cfg.end() != it)
	{
		return &it->second;
	}
	return NULL;
}

const YaoXingShiLianRewardCfg * YaoXingShiLianConfig::GetRewardCfg(int seq)
{
	if (seq - 1 >= 0 && seq - 1 < MAX_YAOXINGSHILIAN_REWARD_LIST)
	{
		return reward_list[seq - 1];
	}
	return NULL;
}

const YaoXingShiLianReward2Cfg * YaoXingShiLianConfig::GetReward2Cfg(int seq)
{
	if (seq - 1 >= 0 && seq - 1 < MAX_YAOXINGSHILIAN_REWARD2_LIST)
	{
		return reward_list2[seq - 1];
	}
	return NULL;
}

int YaoXingShiLianConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement* dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -10000;

	if (!GetSubNodeValue(dataElement, "max_rounds", m_other_cfg.max_rounds) || m_other_cfg.max_rounds <= 0)
	{
		return -1;
	}

	if (!GetSubNodeValue(dataElement, "rmb", m_other_cfg.rmb) || m_other_cfg.rmb <= 0)
	{
		return -1;
	}

	return 0;
}

int YaoXingShiLianConfig::InitLevelCfg(TiXmlElement * RootElement)
{
	TiXmlElement* dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -10000;

	while (dataElement)
	{
		int seq = 0;
		if (!GetSubNodeValue(dataElement, "seq", seq) || seq <= 0)
		{
			return -1;
		}

		YaoXingShiLianLevelCfg &list_node = m_level_cfg[seq];
		list_node.seq = seq;
		
		if (!GetSubNodeValue(dataElement, "num", list_node.num) || list_node.num <= 0)
		{
			return -2;
		}

		if (!GetSubNodeValue(dataElement, "score", list_node.score) || list_node.score <= 0)
		{
			return -3;
		}

		if (!GetSubNodeValue(dataElement, "level_limit", list_node.level_limit) || list_node.level_limit <= 0 || list_node.level_limit > MAX_ROLE_LEVEL)
		{
			return -101;
		}

		if (!GetSubNodeValue(dataElement, "monster_one", list_node.monster_one) || list_node.monster_one <= 0)
		{
			return -4;
		}

		if (!GetSubNodeValue(dataElement, "monster_two", list_node.monster_two) || list_node.monster_two <= 0)
		{
			return -5;
		}

		if (!GetSubNodeValue(dataElement, "coin", list_node.coin) || list_node.coin < 0)
		{
			return -6;
		}

		TiXmlElement* item_list_Element = dataElement->FirstChildElement("clear_reward_list");
		if (NULL != item_list_Element)
		{
			TiXmlElement* item_Element = item_list_Element->FirstChildElement("clear_reward");
			while (NULL != item_Element)
			{
				ItemConfigData reward_node;
				if (!reward_node.ReadConfig(item_Element))
				{
					return -7;
				}
				list_node.reward_vec.push_back(reward_node);
				item_Element = item_Element->NextSiblingElement();
			}
		}

		dataElement = dataElement->NextSiblingElement();
	}
	return 0;
}

int YaoXingShiLianConfig::InitAchievementCfg(TiXmlElement * RootElement)
{
	for (int i = 0; i < MAX_YAOXINGSHILIAN_REWARD_LIST; i++)
	{
		reward_list[i] = NULL;
	}

	TiXmlElement* dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -10000;
	while (dataElement)
	{
		YaoXingShiLianRewardCfg node;

		if (!GetSubNodeValue(dataElement, "seq", node.seq) || node.seq - 1 < 0 || node.seq - 1 >= MAX_YAOXINGSHILIAN_REWARD_LIST)
		{
			return -1;
		}

		if (!GetSubNodeValue(dataElement, "num", node.num) || node.num <= 0 || node.num > (int)m_level_cfg.size())
		{
			return -3;
		}

		if (!GetSubNodeValue(dataElement, "achievement", node.achievement) || node.achievement < YAO_XING_ACH_TYPE_NO || node.achievement >= YAO_XING_ACH_TYPE_MAX)
		{
			return -4;
		}

		if (!GetSubNodeValue(dataElement, "param1", node.param1))
		{
			return -5;
		}

		if (!GetSubNodeValue(dataElement, "param2", node.param2))
		{
			return -6;
		}

		TiXmlElement* extra_list_Element = dataElement->FirstChildElement("extra_reward_list");
		if (NULL != extra_list_Element)
		{
			TiXmlElement* item_Element = extra_list_Element->FirstChildElement("extra_reward");
			while (NULL != item_Element)
			{
				ItemConfigData reward_node;
				if (!reward_node.ReadConfig(item_Element))
				{
					return -7;
				}
				node.reward_vec.push_back(reward_node);
				item_Element = item_Element->NextSiblingElement();
			}
		}
		m_reward_vec.push_back(node);
		dataElement = dataElement->NextSiblingElement();
	}

	for (std::vector<YaoXingShiLianRewardCfg>::iterator it = m_reward_vec.begin(); it != m_reward_vec.end(); it++)
	{
		if (it->seq - 1 >= 0 && it->seq - 1 < MAX_YAOXINGSHILIAN_REWARD_LIST)
		{
			reward_list[it->seq - 1] = &(*it);
		}
	}

	return 0;
}

int YaoXingShiLianConfig::InitAchievement2Cfg(TiXmlElement * RootElement)
{
	for (int i = 0; i < MAX_YAOXINGSHILIAN_REWARD2_LIST; i++)
	{
		reward_list2[i] = NULL;
	}

	TiXmlElement* dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -10000;
	while (dataElement)
	{
		YaoXingShiLianReward2Cfg node;

		if (!GetSubNodeValue(dataElement, "seq", node.seq) || node.seq - 1 < 0 || node.seq - 1 >= MAX_YAOXINGSHILIAN_REWARD2_LIST)
		{
			return -1;
		}

		if (!GetSubNodeValue(dataElement, "num", node.num) || node.num <= 0 || node.num > (int)m_level_cfg.size())
		{
			return -3;
		}

		if (!GetSubNodeValue(dataElement, "achievement_2", node.achievement_2) || node.achievement_2 < YAO_XING_ACH_TYPE_NO || node.achievement_2 >= YAO_XING_ACH_TYPE_MAX)
		{
			return -4;
		}

		if (!GetSubNodeValue(dataElement, "achievement", node.achievement))
		{
			return -5;
		}

		if (!GetSubNodeValue(dataElement, "param1", node.param1))
		{
			return -6;
		}

		if (!GetSubNodeValue(dataElement, "param2", node.param2))
		{
			return -7;
		}

		TiXmlElement* extra_list_Element = dataElement->FirstChildElement("extra_reward_list");
		if (NULL != extra_list_Element)
		{
			TiXmlElement* item_Element = extra_list_Element->FirstChildElement("extra_reward");
			while (NULL != item_Element)
			{
				ItemConfigData reward_node;
				if (!reward_node.ReadConfig(item_Element))
				{
					return -8;
				}
				node.reward_vec.push_back(reward_node);
				item_Element = item_Element->NextSiblingElement();
			}
		}
		m_reward2_vec.push_back(node);
		dataElement = dataElement->NextSiblingElement();
	}

	for (std::vector<YaoXingShiLianReward2Cfg>::iterator it = m_reward2_vec.begin(); it != m_reward2_vec.end(); it++)
	{
		if (it->seq - 1 >= 0 && it->seq - 1 < MAX_YAOXINGSHILIAN_REWARD_LIST)
		{
			reward_list2[it->seq - 1] = &(*it);
		}
	}

	return 0;
}
