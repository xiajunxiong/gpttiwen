#include <numeric>
#include "monsterwaveconfig.h"
#include "servercommon/struct/battle/battle_def.hpp"

MonsterWaveConfig::MonsterWaveConfig()
{
}

bool MonsterWaveConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG("cave_content", InitLevelSceneCfg);
	LOAD_CONFIG("reward_random", InitGroupReward);
	LOAD_CONFIG("other", InitOtherCfg);
	return true;
}

const MonsterWaveLevelCfg * MonsterWaveConfig::GetLevelCfg(int role_level)
{
	for (std::map<int, MonsterWaveLevelCfg>::reverse_iterator iter = m_level_cfg.rbegin(); iter != m_level_cfg.rend(); ++iter)
	{
		if (role_level >= iter->first)
		{
			 return  &iter->second;
		}
	}

	return NULL;
}

bool MonsterWaveConfig::CanPlay(int role_level, int & fb_level)
{
	for (std::map<int, MonsterWaveLevelCfg>::reverse_iterator iter = m_level_cfg.rbegin(); iter != m_level_cfg.rend(); ++iter)
	{
		if (role_level >= iter->first)
		{
			fb_level = iter->first;
			return true;
		}
	}


	return false;
}

void MonsterWaveConfig::GetRandReward(int reward_id, std::vector<ItemConfigData>& out_list)
{
	std::map<int, MonsterWaveRewardCfg > ::iterator iter = m_rewards_cfg.find(reward_id);
	if (iter == m_rewards_cfg.end())
	{
		return;
	}

	int r = rand() % MonsterWaveRewardCfg::MAX_RATE_COUNT;

	for (int i = 0; i < (int)iter->second.group_reward.size(); ++i)
	{
		if (r < iter->second.group_reward[i].rate)
		{
			out_list.push_back(iter->second.group_reward[i].reward);
			break;
		}

		r -= iter->second.group_reward[i].rate;
	}
}

int MonsterWaveConfig::InitLevelSceneCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	int next_seq = 1;
	while (NULL != root_element)
	{
		int seq = 0;
		if (!GetSubNodeValue(root_element, "seq", seq) || seq != next_seq)
		{
			return -1;
		}
		next_seq += 1;

		int need_role_level = 0;
		if (!GetSubNodeValue(root_element, "level", need_role_level) || need_role_level > MAX_ROLE_LEVEL)
		{
			return -2;
		}
		MonsterWaveLevelCfg & cfg = m_level_cfg[need_role_level];
		cfg.need_role_level = need_role_level;

		if (!GetSubNodeValue(root_element, "monster_group", cfg.monster_group_id))
		{
			return -3;
		}

		int wave_num = 0;
		if (!GetSubNodeValue(root_element, "num", wave_num) || wave_num <= 0)
		{
			return -4;
		}
		
		MonsterWaveLevelCfg::ItemCfg item_cfg;
		if (!GetSubNodeValue(root_element, "min_level", item_cfg.min_level) || item_cfg.min_level <= 0 || item_cfg.min_level > MAX_ROLE_LEVEL)
		{
			return -5;
		}
		if (!GetSubNodeValue(root_element, "max_level", item_cfg.max_Level) || item_cfg.max_Level < item_cfg.min_level || item_cfg.max_Level > MAX_ROLE_LEVEL)
		{
			return -6;
		}

		TiXmlElement * clear_reward = root_element->FirstChildElement("clear_reward");
		if (NULL != clear_reward)
		{
			TiXmlElement *node_element = clear_reward->FirstChildElement("node");
			while (node_element)
			{
				int reward_id = 0;
				if (!GetNodeValue(node_element, reward_id))
				{
					return -7;
				}

				item_cfg.wave_reward.push_back(reward_id);
				node_element = node_element->NextSiblingElement();
			}
		}

		if (item_cfg.wave_reward.size() == 0)
		{
			return -300;
		}

		cfg.all_wave_reward[wave_num].push_back(item_cfg);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int MonsterWaveConfig::InitGroupReward(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	while (NULL != root_element)
	{
		int group_id = 0;
		if (!GetSubNodeValue(root_element, "random", group_id))
		{
			return -1;
		}

		MonsterWaveRewardCfg & cfg = m_rewards_cfg[group_id];
		MonsterWaveRewardCfg::ItemCfg item_cfg;

		if (!GetSubNodeValue(root_element, "rate", item_cfg.rate) || item_cfg.rate <= 0)
		{
			return -2;
		}

		TiXmlElement * clear_reward = root_element->FirstChildElement("reward");
		if (!item_cfg.reward.ReadConfig(clear_reward))
		{
			return -104;
		}

		cfg.group_reward.push_back(item_cfg);
		root_element = root_element->NextSiblingElement();
	}

	if (m_rewards_cfg.size() == 0)
	{
		return -10085;
	}

	for (std::map<int, MonsterWaveRewardCfg >::iterator iter = m_rewards_cfg.begin(); iter != m_rewards_cfg.end(); ++iter)
	{
		int count = 0;
		for (int i = 0; i < (int)iter->second.group_reward.size(); ++i)
		{
			count += iter->second.group_reward[i].rate;
		}

		if (count > MonsterWaveRewardCfg::MAX_RATE_COUNT)
		{
			return -10086;
		}
	}

	return 0;
}

int MonsterWaveConfig::InitOtherCfg(TiXmlElement *RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	if (!GetSubNodeValue(root_element, "max_round", m_other_cfg.max_round_limit) || m_other_cfg.max_round_limit < 10 || m_other_cfg.max_round_limit > MAX_ROUND_NUM)
	{
		return -100;
	}

	return 0;
}

