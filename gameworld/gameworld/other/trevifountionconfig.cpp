#include "trevifountionconfig.hpp"

TreviFountionconfig::TreviFountionconfig()
{
}

TreviFountionconfig::~TreviFountionconfig()
{
}

bool TreviFountionconfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG("wish_reward", InitWishRewardCfg);
	LOAD_CONFIG("buff_list", InitBuffCfg);
	LOAD_CONFIG("other", InitOtherCfg);
	LOAD_CONFIG("reward_find", InitFindRewardCfg);

	return true;
}

const WishBuffCfg* TreviFountionconfig::RandBuff(int buff_group)
{
	std::map<int, int>::const_iterator rate_it = m_total_rate.find(buff_group);
	if (rate_it == m_total_rate.end()) return NULL;

	std::map<int, std::map<int, WishBuffCfg> >::const_iterator buff_it = m_buff_group_map.find(buff_group);
	if (buff_it == m_buff_group_map.end()) return NULL;

	int rand_num = RandomNum(rate_it->second);
	for (std::map<int, WishBuffCfg>::const_iterator it = buff_it->second.begin(); it != buff_it->second.end(); ++it)
	{
		if (rand_num < it->second.buff_rate)
		{
			return &it->second;
		}
		rand_num -= it->second.buff_rate;
	}
	return NULL;
}

const WishBuffCfg * TreviFountionconfig::GetBuffCfg(int buff_id)
{
	std::map<int, WishBuffCfg>::const_iterator it = m_buff_map.find(buff_id);
	if (it == m_buff_map.end()) return NULL;
	return &it->second;
}

const WishRewardCfg * TreviFountionconfig::GetRewardCfg(int level)
{
	std::map<int, WishRewardCfg>::const_iterator it = m_reward_map.find(level);
	if (it == m_reward_map.end()) return NULL;
	return &it->second;
}

bool TreviFountionconfig::GetFindReward(int level, int type, int * coin, int * exp)
{
	for (std::map<int, std::map<int, TreviFountionFindCfg> >::const_iterator level_it = m_find_reward_map.begin(), level_end = m_find_reward_map.end();
		 level_end != level_it; ++level_it)
	{
		int min_level = level_it->first / 1000;
		int max_level = level_it->first % 1000;

		if (level >= min_level && level <= max_level)
		{
			std::map<int, TreviFountionFindCfg>::const_iterator it = level_it->second.find(type);
			if (it == level_it->second.end())
			{
				return false;
			}
			else
			{
				if (NULL != coin) *coin = it->second.coin;
				if (NULL != exp) *exp = it->second.exp;
				
				return true;
			}
		}
	}
	return false;
}

int TreviFountionconfig::InitWishRewardCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}
	int last_level = 0;
	while (NULL != root_element)
	{
		int level = 0;
		if (!GetSubNodeValue(root_element, "role_level", level) || level != last_level + 1)return -1;
		last_level = level;

		WishRewardCfg& node_cfg = m_reward_map[level];
		node_cfg.level = level;
		if (!GetSubNodeValue(root_element, "exp", node_cfg.exp) || node_cfg.exp < 0)return -2;
		if (!GetSubNodeValue(root_element, "coin", node_cfg.coin) || node_cfg.coin < 0)return -3;
		if (!GetSubNodeValue(root_element, "buff_group", node_cfg.buff_group) || node_cfg.buff_group < 0)return -4;
		TiXmlElement *reward_element = root_element->FirstChildElement("reward");
		if (reward_element != NULL)
		{
			node_cfg.reward.ReadConfig(reward_element);
		}

		root_element = root_element->NextSiblingElement();
	}
	return 0;
}

int TreviFountionconfig::InitBuffCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	while (NULL != root_element)
	{
		int buff_id = 0; 
		if (!GetSubNodeValue(root_element, "buff_id", buff_id) || buff_id < 0)return -1;
		WishBuffCfg& node_cfg = m_buff_map[buff_id];
		node_cfg.buff_id = buff_id;

		if (!GetSubNodeValue(root_element, "buff_type", node_cfg.buff_type) || node_cfg.buff_type < 0)return -2;
		if (!GetSubNodeValue(root_element, "time", node_cfg.buff_time) || node_cfg.buff_time < 0)return -3;
		if (!GetSubNodeValue(root_element, "buff_rate", node_cfg.buff_rate) || node_cfg.buff_rate < 0)return -3;

		int buff_group = 0;
		if (!GetSubNodeValue(root_element, "buff_group", buff_group) || buff_group <= 0)return -3;


		char str[10] = { 0 };
		for (int i = 1; i <= 4; ++i)
		{
			sprintf(str, "param_%d", i);
			GetSubNodeValue(root_element, str, node_cfg.param_list[i - 1]);
		}

		m_total_rate[buff_group] += node_cfg.buff_rate;
		m_buff_group_map[buff_group][buff_id] = node_cfg;

		root_element = root_element->NextSiblingElement();
	}
	return 0;
}

int TreviFountionconfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	while (NULL != root_element)
	{
		if (!GetSubNodeValue(root_element, "wish_num", m_other_cfg.wish_num))return -1;

		root_element = root_element->NextSiblingElement();
	}
	return 0;
}

int TreviFountionconfig::InitFindRewardCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	while (NULL != root_element)
	{
		TreviFountionFindCfg node_cfg;
		int find_type;
		if (!GetSubNodeValue(root_element, "find_type", find_type) || find_type < 0 || find_type > 1)return -1;
		int min_level;
		int max_level;
		if (!GetSubNodeValue(root_element, "min_level", min_level)) return -2;
		if (!GetSubNodeValue(root_element, "max_level", max_level)) return -3;

		if (!GetSubNodeValue(root_element, "exp", node_cfg.exp)) return -4;
		if (!GetSubNodeValue(root_element, "coin", node_cfg.coin)) return -5;
		m_find_reward_map[min_level * 1000 + max_level][find_type] = node_cfg;
		root_element = root_element->NextSiblingElement();
	}
	return 0;
}
