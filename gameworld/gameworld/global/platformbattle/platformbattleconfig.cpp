#include "config/sharedconfig/sharedconfig.h"
#include "engineadapter.h"

#include "platformbattleconfig.hpp"

PlatformBattleConfig::PlatformBattleConfig()
{
}

PlatformBattleConfig::~PlatformBattleConfig()
{
}

bool PlatformBattleConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG("leitai", InitPlatformCfg);
	LOAD_CONFIG("reward", InitRewardCfg);
	LOAD_CONFIG("pos", InitSceneCfg);
	LOAD_CONFIG("robot_level", InitRobotLevelCfg);
	LOAD_CONFIG("leitai_achieve", InitAchievementCfg);
	LOAD_CONFIG("other", InitOtherCfg);

	return true;
}

const PlatformBattleOtherCfg & PlatformBattleConfig::GetOtherCfg()
{
	return m_other_cfg;
}

int PlatformBattleConfig::GetMaxAllowableNum(int platform_number)
{
	std::map<int, PlatformBattlePlatformCfg>::iterator iter = m_platform_cfg.find(platform_number);
	if (iter == m_platform_cfg.end())
	{
		return 0;
	}

	return iter->second.type == 1 ? 5 : 3;
}

int PlatformBattleConfig::GetRandRobotRoleLevel(int platform_number, int robot_level_index)
{
	std::map<int, PlatformBattlePlatformCfg>::iterator iter = m_platform_cfg.find(platform_number);
	if (iter == m_platform_cfg.end())
	{
		return 1;
	}

	int min_level = 1;
	int max_level = MAX_ROLE_LEVEL;

	const PlatformBattlePlatformCfg & cfg = iter->second;
	if (0 <= robot_level_index && robot_level_index <= (int)cfg.robot_level_cfg.size())
	{
		min_level = cfg.robot_level_cfg[robot_level_index].min_robot_level;
		max_level = cfg.robot_level_cfg[robot_level_index].max_robot_level;
	}

	return RandomNum(min_level, max_level + 1);
}

const PlatformBattleRewardCfg::RewardItemCfg * PlatformBattleConfig::GetRankReward(int platform_number, int rank)
{
	std::map<int, PlatformBattlePlatformCfg>::iterator iter = m_platform_cfg.find(platform_number);
	if (iter == m_platform_cfg.end())
	{
		return NULL;
	}

	std::map<int, PlatformBattleRewardCfg>::iterator reward_iter = m_reward_cfg.find(iter->second.reward_id);
	if (reward_iter == m_reward_cfg.end())
	{
		return NULL;
	}

	for (int i = 0; i < (int)reward_iter->second.rank_reward.size(); ++i)
	{
		PlatformBattleRewardCfg::RewardItemCfg & cfg = reward_iter->second.rank_reward[i];
		if (cfg.min_rank <= rank && rank < cfg.max_rank)
		{
			return &cfg;
		}
	}

	return NULL;
}

const PlatformBattleSceneCfg * PlatformBattleConfig::GetSceneCfg(int seq)
{
	return MapValuesConstPtr(m_scene_cfg, seq);
}

const PlatformBattlePlatformCfg * PlatformBattleConfig::GetPlatformBattlePlatformCfg(int platform_number)
{
	std::map<int, PlatformBattlePlatformCfg>::iterator iter = m_platform_cfg.find(platform_number);
	if (iter == m_platform_cfg.end())
	{
		return NULL;
	}

	return &iter->second;
}

const PlatformBattleAchievementCfg * PlatformBattleConfig::GetPlatformBattleAchievementCfg(int type)
{
	std::map<int, PlatformBattleAchievementCfg> ::const_iterator iter = m_achievement_cfg.find(type);
	if (iter != m_achievement_cfg.end())
	{
		return &iter->second;
	}

	return NULL;
}

const PlatformBattleAchievementItemItemCfg * PlatformBattleConfig::GetPlatformBattleAchievementItemItemCfg(int type, int seq)
{
	const PlatformBattleAchievementCfg * cfg = this->GetPlatformBattleAchievementCfg(type);
	if (cfg == NULL)
	{
		return NULL;
	}

	return cfg->GetPlatformBattleAchievementItemItemCfg(seq);
}

int PlatformBattleConfig::InitRewardCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	while (NULL != root_element)
	{
		int reward_id = 0;
		if (!GetSubNodeValue(root_element, "reward_id", reward_id))
		{
			return -1;
		}

		PlatformBattleRewardCfg & reward_cfg = m_reward_cfg[reward_id];

		int last_max_rank = 1;
		if (reward_cfg.rank_reward.size() != 0)
		{
			last_max_rank = reward_cfg.rank_reward.back().max_rank;
		}


		PlatformBattleRewardCfg::RewardItemCfg cfg;

		if (!GetSubNodeValue(root_element, "min_rank", cfg.min_rank) || cfg.min_rank <= 0 || cfg.min_rank != last_max_rank)
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "max_rank", cfg.max_rank) || cfg.max_rank < cfg.min_rank)
		{
			return -3;
		}

		if (!GetSubNodeValue(root_element, "bind_coin", cfg.coin) || cfg.coin < 0)
		{
			return -4;
		}

		if (!GetSubNodeValue(root_element, "points", cfg.credits) || cfg.credits < 0)
		{
			return -5;
		}

		TiXmlElement * reward_elements = root_element->FirstChildElement("reward_list");
		if (NULL != reward_elements)
		{
			TiXmlElement * reward_item_element = reward_elements->FirstChildElement("reward");
			while (reward_item_element)
			{
				ItemConfigData reward;
				if (!reward.ReadConfig(reward_item_element))
				{
					return -200;
				}

				cfg.reward_item.push_back(reward);
				reward_item_element = reward_item_element->NextSiblingElement();
			}
		}

		reward_cfg.rank_reward.push_back(cfg);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int PlatformBattleConfig::InitPlatformCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	int next_platform_number = 1;
	while (NULL != root_element)
	{
		int platform_number = 0;
		if (!GetSubNodeValue(root_element, "seq", platform_number) || platform_number != next_platform_number)
		{
			return -1;
		}
		next_platform_number += 1;

		PlatformBattlePlatformCfg & cfg = m_platform_cfg[platform_number];
		if (!GetSubNodeValue(root_element, "leitai_type", cfg.type))
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "reward_id", cfg.reward_id))
		{
			return -3;
		}

		if (!GetSubNodeValue(root_element, "position", cfg.position))
		{
			return -4;
		}

		if (!GetSubNodeValue(root_element, "leitai_level", cfg.platform_level))
		{
			return -5;
		}

		root_element = root_element->NextSiblingElement();
	}

	if ((int)m_platform_cfg.size() != PLATFORM_BATTLE_PLATFORM_NUM)
	{
		return -900;
	}

	return 0;
}

int PlatformBattleConfig::InitSceneCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	while (NULL != root_element)
	{
		PlatformBattleSceneCfg cfg;
		if (!GetSubNodeValue(root_element, "seq", cfg.seq))
		{
			return -1;
		}

		if (!GetSubNodeValue(root_element, "scene_id", cfg.scene_id))
		{
			return -111;
		}

		if (!GetSubNodeValue(root_element, "enter_x", cfg.enter_pos.x))
		{
			return -3;
		}

		if (!GetSubNodeValue(root_element, "enter_y", cfg.enter_pos.y))
		{
			return -4;
		}

		m_scene_cfg[cfg.seq] = cfg;
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int PlatformBattleConfig::InitRobotLevelCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -__LINE__;
	}

	int next_platform_number = 1;
	while (NULL != root_element)
	{
		int platform_number = 0;
		if (!GetSubNodeValue(root_element, "seq", platform_number) || platform_number != next_platform_number)
		{
			return -1;
		}
		next_platform_number += 1;

		PlatformBattlePlatformCfg & cfg = m_platform_cfg[platform_number];

		for (int i = 0; i < PLATFORM_BATTLE_RANK_ITEM_MAX; ++i)
		{
			PlatformBattleRobotLevelCfg level_cfg;
			std::string min_level = STRING_SPRINTF("min_level_%d", i);
			std::string max_level = STRING_SPRINTF("max_level_%d", i);

			if (!GetSubNodeValue(root_element, min_level, level_cfg.min_robot_level) || level_cfg.min_robot_level <= 0)
			{
				return -__LINE__;
			}

			if (!GetSubNodeValue(root_element, max_level, level_cfg.max_robot_level) || level_cfg.max_robot_level < level_cfg.min_robot_level)
			{
				return -__LINE__;
			}

			cfg.robot_level_cfg.push_back(level_cfg);
		}

		root_element = root_element->NextSiblingElement();
	}

	if ((int)m_platform_cfg.size() < PLATFORM_BATTLE_PLATFORM_NUM)
	{
		return -__LINE__;
	}

	return 0;
}

int PlatformBattleConfig::InitAchievementCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -__LINE__;
	}

	while (NULL != root_element)
	{
		int type = 0;
		if (!GetSubNodeValue(root_element, "type", type))
		{
			return -__LINE__;
		}
		
		PlatformBattleAchievementCfg & cfg = m_achievement_cfg[type];

		TiXmlElement * platform_list_element = root_element->FirstChildElement("platform_list");
		if (NULL != platform_list_element)
		{
			TiXmlElement * node_element = platform_list_element->FirstChildElement("node");
			while (node_element)
			{
				int temp = 0;
				if (!GetNodeValue(node_element, temp))
				{
					return -20001;
				}

				cfg.platform_list.insert(temp);

				if (temp == -1)
				{
					cfg.is_all_platform = true;
					cfg.platform_list.clear();
					break;
				}

				node_element = node_element->NextSiblingElement();
			}
		}

		PlatformBattleAchievementItemItemCfg item_cfg;
		if (!GetSubNodeValue(root_element, "seq", item_cfg.seq))
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(root_element, "param", item_cfg.param))
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(root_element, "price_type", item_cfg.jifen_type))
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(root_element, "price_num", item_cfg.jifen) || item_cfg.jifen < 0)
		{
			return -__LINE__;
		}

		TiXmlElement * item_list_element = root_element->FirstChildElement("item_list_list");
		if (NULL != item_list_element)
		{
			TiXmlElement * item_element = item_list_element->FirstChildElement("item_list");
			while (NULL != item_element)
			{
				ItemConfigData temp_item;
				if (!temp_item.ReadConfig(item_element))
				{
					return -__LINE__;
				}

				item_cfg.rewards.push_back(temp_item);
				item_element = item_element->NextSiblingElement();
			}
		}

		if (item_cfg.seq < 0 || item_cfg.seq >= 16)  //领取标志用的是 short，不能大于16条目
		{
			return -__LINE__;
		}

		cfg.item_cfg[item_cfg.seq] = item_cfg;
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int PlatformBattleConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	if (!GetSubNodeValue(root_element, "robot_level", m_other_cfg.robot_show_level) || m_other_cfg.robot_show_level <= 0)
	{
		return -__LINE__;
	}

	return 0;
}

bool PlatformBattleAchievementCfg::IsInPlatformList(int platform_number) const
{
	if (platform_number <= 0 || platform_number > PLATFORM_BATTLE_PLATFORM_NUM)
	{
		return false;
	}

	if (is_all_platform)
	{
		return true;
	}

	std::set<int>::iterator iter = platform_list.find(-1);
	if (iter != platform_list.end())
	{
		return true;
	}

	iter = platform_list.find(platform_number);
	if (iter != platform_list.end())
	{
		return true;
	}

	return false;
}

const PlatformBattleAchievementItemItemCfg * PlatformBattleAchievementCfg::GetPlatformBattleAchievementItemItemCfg(int seq) const
{
	std::map<int, PlatformBattleAchievementItemItemCfg> ::const_iterator iter = item_cfg.find(seq);
	if (iter != item_cfg.end())
	{
		return &iter->second;
	}

	return NULL;
}
