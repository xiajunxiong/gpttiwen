#include "endlesstowerconfig.hpp"

EndlessTowerConfig::EndlessTowerConfig()
{
}

EndlessTowerConfig::~EndlessTowerConfig()
{
	std::vector<EndlessTowerChallengeCfg>().swap(m_challenge_cfg);
	std::vector<EndlessTowerRankRewardCfg>().swap(m_rank_reward_vec);
}

bool EndlessTowerConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG_2("challenge", InitChallengeCfg);
	LOAD_CONFIG_2("other", InitOtherCfg);
	LOAD_CONFIG_2("reward", InitRewardCfg);
	LOAD_CONFIG_2("god_reward", InitGodRewardCfg);
	LOAD_CONFIG_2("rank_reward", InitRankRewardCfg);

	return true;
}

const EndlessTowerChallengeCfg * EndlessTowerConfig::GetEndlessTowerChallengeCfg(int seq)
{
	if (seq - 1 < 0 || seq - 1 >= ENDLESS_TOWER_MAX_SEQ)return NULL;
	
	return m_challeng_list[seq - 1];
}

const EndlessTowerRewardCfg * EndlessTowerConfig::GetEndLessTowerRewardCfg(int reward_seq)
{
	std::map<int, EndlessTowerRewardGroupCfg>::iterator it = m_reward_cfg.find(reward_seq);
	if (it != m_reward_cfg.end())
	{
		if ((int)it->second.reward_group.size() > 0)
		{
			int r_idx = rand() % (int)it->second.reward_group.size();
			return &it->second.reward_group[r_idx];
		}
	}
	return NULL;
}

const GodRewardCfg * EndlessTowerConfig::GetGodRewardCfg(int god_seq)
{
	std::map<int, GodRewardListCfg>::iterator it = m_god_reward_cfg.find(god_seq);
	if (it != m_god_reward_cfg.end())
	{
		if ((int)it->second.reward_cfg.size() > 0)
		{
			int r_idx = rand() % (int)it->second.reward_cfg.size();
			return &it->second.reward_cfg[r_idx];
		}
	}
	return NULL;
}

const EndlessTowerRankRewardCfg * EndlessTowerConfig::GetRankRewardCfg(int rank_id)
{
	for (std::vector<EndlessTowerRankRewardCfg>::iterator it = m_rank_reward_vec.begin(); it != m_rank_reward_vec.end(); it++)
	{
		if (it->min_rank <= rank_id && it->max_rank >= rank_id)
		{
			return &(*it);
		}
	}
	return NULL;
}

int EndlessTowerConfig::InitChallengeCfg(TiXmlElement * RootElement, std::string& errormsg)
{
	TiXmlElement* dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -10000;
	memset(m_challeng_list, NULL, sizeof(m_challeng_list));
	while (dataElement)
	{
		EndlessTowerChallengeCfg node;

		if (!GetSubNodeValue(dataElement, "seq", node.seq) || node.seq <= 0 || node.seq > ENDLESS_TOWER_MAX_SEQ)
		{
			errormsg = STRING_SPRINTF("error is [node.seq > %d] or [node.seq <= 0]", ENDLESS_TOWER_MAX_SEQ);
			return -1;
		}

		if (!GetSubNodeValue(dataElement, "num", node.num) || node.num < 0)
		{
			errormsg = STRING_SPRINTF("error is [node.num < 0]");
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

		m_challenge_cfg.push_back(node);
		dataElement = dataElement->NextSiblingElement();
	}

	for (std::vector<EndlessTowerChallengeCfg>::iterator it = m_challenge_cfg.begin(); it != m_challenge_cfg.end(); it++)
	{
		if (it->seq - 1 >= 0 && it->seq - 1 < ENDLESS_TOWER_MAX_SEQ)
		{
			m_challeng_list[it->seq - 1] = &(*it);
		}
	}

	return 0;
}

int EndlessTowerConfig::InitOtherCfg(TiXmlElement * RootElement, std::string& errormsg)
{
	TiXmlElement* dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -10000;

	while (dataElement)
	{
		if (!GetSubNodeValue(dataElement, "open_level", m_other_cfg.open_level) || m_other_cfg.open_level <= 0 || m_other_cfg.open_level > MAX_ROLE_LEVEL)
		{
			errormsg = STRING_SPRINTF("error is [open_level > %d] or [open_level <= 0]", MAX_ROLE_LEVEL);
			return -1;
		}

		if (!GetSubNodeValue(dataElement, "scene_id", m_other_cfg.scene_id) || m_other_cfg.scene_id < 0)
		{
			errormsg = STRING_SPRINTF("error is [scene_id < 0]");
			return -2;
		}
		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int EndlessTowerConfig::InitRewardCfg(TiXmlElement * RootElement, std::string& errormsg)
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
		EndlessTowerRewardGroupCfg & node_group = m_reward_cfg[seq];
		node_group.seq = seq;

		EndlessTowerRewardCfg node;

		if (!GetSubNodeValue(dataElement, "god_reward_id", node.god_reward_id) || node.god_reward_id < 0)
		{
			errormsg = STRING_SPRINTF("error is [god_reward_id < 0]");
			return -2;
		}

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
				node.reward_vec.push_back(node_item);
				reward_element = reward_element->NextSiblingElement();
			}
		}

		node_group.reward_group.push_back(node);
		dataElement = dataElement->NextSiblingElement();
	}
	return 0;
}

int EndlessTowerConfig::InitGodRewardCfg(TiXmlElement * RootElement, std::string& errormsg)
{
	TiXmlElement* dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -10000;
	while (dataElement)
	{
		int god_reward_id = -1;
		if (!GetSubNodeValue(dataElement, "god_reward_id", god_reward_id) || god_reward_id < 0)
		{
			errormsg = STRING_SPRINTF("error is [god_reward_id < 0]");
			return -1;
		}
		GodRewardListCfg & node_group = m_god_reward_cfg[god_reward_id];

		GodRewardCfg node;
		node.god_reward_id = god_reward_id;

		if (!GetSubNodeValue(dataElement, "rate", node.rate) || node.rate < 0)
		{
			errormsg = STRING_SPRINTF("error is [rate < 0]");
			return -3;
		}

		TiXmlElement * reward_list_element = dataElement->FirstChildElement("god_reward_list");

		if (NULL != reward_list_element)
		{
			TiXmlElement * reward_element = reward_list_element->FirstChildElement("god_reward");
			if (reward_element != NULL)
			{
				ItemConfigData god_reward_node;
				if (!god_reward_node.ReadConfig(reward_element))
				{
					errormsg = STRING_SPRINTF("error is reward ReadConfig fail");
					return -4;
				}
				node.god_reward.push_back(god_reward_node);
			}
		}
		
		node_group.weight += node.rate;
		node_group.reward_cfg.push_back(node);
		dataElement = dataElement->NextSiblingElement();
	}
	return 0;
}

int EndlessTowerConfig::InitRankRewardCfg(TiXmlElement * RootElement, std::string & errormsg)
{
	TiXmlElement* dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -10000;
	while (dataElement)
	{
		EndlessTowerRankRewardCfg node;

		if (!GetSubNodeValue(dataElement, "rank_reward_id", node.rank_reward_id) || node.rank_reward_id < 0)
		{
			errormsg = STRING_SPRINTF("error is [rank_reward_id < 0]");
			return -1;
		}

		if (!GetSubNodeValue(dataElement, "min_rank", node.min_rank) || node.min_rank <= 0)
		{
			errormsg = STRING_SPRINTF("error is [min_rank < 0]");
			return -2;
		}

		if (!GetSubNodeValue(dataElement, "max_rank", node.max_rank) || node.max_rank > 100)
		{
			errormsg = STRING_SPRINTF("error is [max_rank > 100]");
			return -3;
		}

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
					return -4;
				}
				node.reward_vec.push_back(node_item);
				reward_element = reward_element->NextSiblingElement();
			}
		}

		m_rank_reward_vec.push_back(node);
		dataElement = dataElement->NextSiblingElement();
	}
	return 0;
}
