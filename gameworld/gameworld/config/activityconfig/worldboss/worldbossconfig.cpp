#include "worldbossconfig.hpp"
#include "npc/npcpool.h"

WorldBossConfig::WorldBossConfig()
{
}

WorldBossConfig::~WorldBossConfig()
{
	std::vector<WorldBossMonsterCfg>().swap(m_monster_cfg);
	std::vector<WorldBossRankRewardCfg>().swap(m_reward_cfg);
	std::vector<WorldBoss2MonsterCfg>().swap(m_monster_cfg2);
	std::vector<WorldBoss2RankRewardCfg>().swap(m_reward_cfg2);
}

bool WorldBossConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;
	 
	LOAD_CONFIG("other", InitOtherCfg);
	LOAD_CONFIG("reward_group", InitRewardGroupCfg);
	LOAD_CONFIG("coordinate_group", InitPosCfg);

	LOAD_CONFIG("monster", InitMonsterCfg);
	LOAD_CONFIG("reward_act_1", InitRewardCfg);
	LOAD_CONFIG("dragon", InitMonsterCfg2);
	LOAD_CONFIG("reward_act_10", InitRewardCfg2);
	
	return true;
}

const WorldBossMonsterCfg * WorldBossConfig::GetWorldBossMonsterCfg(int open_day)
{
	for (int i = 0; i < (int)m_monster_cfg.size(); ++i)
	{
		if (m_monster_cfg[i].min_open_day <= open_day && open_day <= m_monster_cfg[i].max_open_day)
		{
			return &m_monster_cfg[i];
		}
	}

	return NULL;
}

Posi WorldBossConfig::GetRandPosByPosGroupID(int rand_pos_id)
{
	std::map<int, WorldBossRandPosCfg>::iterator iter = m_rand_pos_cfg.find(rand_pos_id);
	if (iter != m_rand_pos_cfg.end())
	{
		int r = rand() % WorldBossRandPosCfg::MAX_RATE_COUNT;

		for (int i = 0; i < (int)iter->second.group_pos.size(); ++i)
		{
			if (r < iter->second.group_pos[i].rate)
			{
				return iter->second.group_pos[i].pos;
			}

			r -= iter->second.group_pos[i].rate;
		}
	}

	return Posi(0, 0);
}

WorldBossRankRewardGroupCfg WorldBossConfig::GetWorldBossRankRewardCfg(int role_level, int reward_type)
{
	std::vector<int> reward_group_id;
	for (int i = 0; i < (int)m_reward_cfg.size(); ++i)
	{
		if (m_reward_cfg[i].min_level <= role_level && role_level <= m_reward_cfg[i].max_level)
		{			
			std::map<int, std::vector<int> >::iterator iter = m_reward_cfg[i].reward_list.find(reward_type);
			if (iter != m_reward_cfg[i].reward_list.end())
			{
				reward_group_id = iter->second;
			}

			break;
		}
	}

	WorldBossRankRewardGroupCfg ret;

	for (int i = 0; i < (int)reward_group_id.size(); ++i)
	{
		const WorldBossRankRewardGroupCfg * temp = this->GetGroupReward(reward_group_id[i]);
		{
			if (temp != NULL)
			{
				ret = ret + *temp;
			}
		}
	}

	return ret;
}

const WorldBoss2MonsterCfg * WorldBossConfig::GetWorldBoss2MonsterCfg(int open_day)
{
	for (int i = 0; i < (int)m_monster_cfg2.size(); ++i)
	{
		if (m_monster_cfg2[i].min_open_day <= open_day && open_day <= m_monster_cfg2[i].max_open_day)
		{
			return &m_monster_cfg2[i];
		}
	}

	return NULL;
}

WorldBossRankRewardGroupCfg WorldBossConfig::GetWorldBoss2RankRewardCfg(int role_level, int reward_type)
{
	std::vector<int> reward_group_id;
	for (int i = 0; i < (int)m_reward_cfg2.size(); ++i)
	{
		if (m_reward_cfg2[i].min_level <= role_level && role_level <= m_reward_cfg2[i].max_level)
		{
			std::map<int, std::vector<int> >::iterator iter = m_reward_cfg2[i].reward_list.find(reward_type);
			if (iter != m_reward_cfg2[i].reward_list.end())
			{
				reward_group_id = iter->second;
			}

			break;
		}
	}

	WorldBossRankRewardGroupCfg ret;

	for (int i = 0; i < (int)reward_group_id.size(); ++i)
	{
		const WorldBossRankRewardGroupCfg * temp = this->GetGroupReward(reward_group_id[i]);
		{
			if (temp != NULL)
			{
				ret = ret + *temp;
			}
		}
	}

	return ret;
}

const WorldBossRankRewardGroupCfg * WorldBossConfig::GetGroupReward(int reward_group_id)
{
	std::map<int, WorldBossRankRewardGroupCfg>::iterator iter = m_reward_group_cfg.find(reward_group_id);
	if (iter != m_reward_group_cfg.end())
	{
		return &iter->second;
	}

	return NULL;
}

const WorldBossOtherCfg & WorldBossConfig::GetOtherCfg()
{
	return m_other_cfg;
}

int WorldBossConfig::InitMonsterCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	int last_max_open_day = 0;
	while (NULL != root_element)
	{
		WorldBossMonsterCfg cfg;

		if (!GetSubNodeValue(root_element, "scene_id", cfg.scene_id))
		{
			return -1;
		}

		if (!GetSubNodeValue(root_element, "coordinate_group_id", cfg.coordinate_group_id))
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "min_open_day", cfg.min_open_day) || cfg.min_open_day <= last_max_open_day)
		{
			return -3;
		}

		if (!GetSubNodeValue(root_element, "max_open_day", cfg.max_open_day) || cfg.max_open_day < cfg.min_open_day)
		{
			return -4;
		}
		last_max_open_day = cfg.max_open_day;

		if (!GetSubNodeValue(root_element, "max_hp", cfg.boss_max_hp))
		{
			return -5;
		}

		if (!GetSubNodeValue(root_element, "npc_id", cfg.ncp_monster_id))
		{
			return -6;
		}

		if (NULL == NpcPool::Instance()->GetServerNPCInfo(cfg.ncp_monster_id))
		{
			return -7;
		}

		m_monster_cfg.push_back(cfg);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int WorldBossConfig::InitRewardCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	int last_max_level = 0;
	while (NULL != root_element)
	{
		WorldBossRankRewardCfg cfg;

		if (!GetSubNodeValue(root_element, "min_level", cfg.min_level) || cfg.min_level <= 0 || cfg.min_level < last_max_level)
		{
			return -1;
		}

		if (!GetSubNodeValue(root_element, "max_level", cfg.max_level) || cfg.max_level < cfg.min_level)
		{
			return -2;
		}
		last_max_level = cfg.max_level;


		for (int i = 0; i < WORLD_BOSS_REWARD_TYPE_MAX; ++i)
		{
			char reward_str[128] = { 0 };
			SNPRINTF(reward_str, sizeof(reward_str) - 1, "reward_%d", i);
			TiXmlElement * reward_element = root_element->FirstChildElement(reward_str);
			if (NULL != reward_element)
			{
				TiXmlElement *node_element = reward_element->FirstChildElement("node");
				std::vector<int> & reward_ids = cfg.reward_list[i];
				while (node_element)
				{
					int reward_id = 0;
					if (!GetNodeValue(node_element, reward_id))
					{
						return -399;
					}

					reward_ids.push_back(reward_id);
					node_element = node_element->NextSiblingElement();
				}
			}			
		}

		m_reward_cfg.push_back(cfg);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int WorldBossConfig::InitRewardGroupCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	while (NULL != root_element)
	{
		int reward_group = 0;
		if (!GetSubNodeValue(root_element, "reward_group", reward_group))
		{
			return -1;
		}

		WorldBossRankRewardGroupCfg & cfg = m_reward_group_cfg[reward_group];
		if (!GetSubNodeValue(root_element, "bind_coin", cfg.bind_coin) || cfg.bind_coin < 0)
		{
			return -2;
		}

		TiXmlElement * reward_list_element = root_element->FirstChildElement("reward_list");
		if (NULL != reward_list_element)
		{
			TiXmlElement * reward_element = reward_list_element->FirstChildElement("reward");
			while (reward_element != NULL)
			{
				ItemConfigData reward;
				if (!reward.ReadConfig(reward_element))
				{
					return -104;
				}
				cfg.rewards.push_back(reward);
				reward_element = reward_element->NextSiblingElement();
			}
		}

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int WorldBossConfig::InitMonsterCfg2(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	int last_max_open_day = 0;
	while (NULL != root_element)
	{
		WorldBoss2MonsterCfg cfg;

		if (!GetSubNodeValue(root_element, "scene_id", cfg.scene_id))
		{
			return -1;
		}

		if (!GetSubNodeValue(root_element, "coordinate_group_id", cfg.coordinate_group_id))
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "min_open_day", cfg.min_open_day) || cfg.min_open_day <= last_max_open_day)
		{
			return -3;
		}

		if (!GetSubNodeValue(root_element, "max_open_day", cfg.max_open_day) || cfg.max_open_day < cfg.min_open_day)
		{
			return -4;
		}
		last_max_open_day = cfg.max_open_day;

		if (!GetSubNodeValue(root_element, "max_hp", cfg.boss_max_hp))
		{
			return -5;
		}

		if (!GetSubNodeValue(root_element, "npc_id", cfg.ncp_monster_id))
		{
			return -6;
		}

		m_monster_cfg2.push_back(cfg);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int WorldBossConfig::InitRewardCfg2(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	int last_max_level = 0;
	while (NULL != root_element)
	{
		WorldBoss2RankRewardCfg cfg;

		if (!GetSubNodeValue(root_element, "min_level", cfg.min_level) || cfg.min_level <= 0 || cfg.min_level < last_max_level)
		{
			return -1;
		}

		if (!GetSubNodeValue(root_element, "max_level", cfg.max_level) || cfg.max_level < cfg.min_level)
		{
			return -2;
		}
		last_max_level = cfg.max_level;


		for (int i = 0; i < WORLD_BOSS_REWARD_TYPE_MAX; ++i)
		{
			char reward_str[128] = { 0 };
			SNPRINTF(reward_str, sizeof(reward_str) - 1, "reward_%d", i);
			TiXmlElement * reward_element = root_element->FirstChildElement(reward_str);
			if (NULL != reward_element)
			{
				TiXmlElement *node_element = reward_element->FirstChildElement("node");
				std::vector<int> & reward_ids = cfg.reward_list[i];
				while (node_element)
				{
					int reward_id = 0;
					if (!GetNodeValue(node_element, reward_id))
					{
						return -399;
					}

					reward_ids.push_back(reward_id);
					node_element = node_element->NextSiblingElement();
				}
			}
		}

		m_reward_cfg2.push_back(cfg);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int WorldBossConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	if (!GetSubNodeValue(root_element, "minin_level", m_other_cfg.need_role_level))
	{
		return -1;
	}

	if (!GetSubNodeValue(root_element, "max_time", m_other_cfg.limit_play_times) || m_other_cfg.limit_play_times <= 0)
	{
		return -2;
	}

	if (!GetSubNodeValue(root_element, "min_dwnum", m_other_cfg.need_role_num))
	{
		return -3;
	}

	if (!GetSubNodeValue(root_element, "act_1_damage_rank", m_other_cfg.world_boss_hurt_rank_num))
	{
		return -4;
	}

	if (!GetSubNodeValue(root_element, "act_1_cure_rank", m_other_cfg.world_boss_heal_rank_num))
	{
		return -5;
	}

	if (!GetSubNodeValue(root_element, "act_1_lucky", m_other_cfg.world_boss_luck_rank_num))
	{
		return -6;
	}

	if (!GetSubNodeValue(root_element, "act_10_damage_rank", m_other_cfg.world_boss2_hurt_rank_num))
	{
		return -4;
	}

	if (!GetSubNodeValue(root_element, "act_10_cure_rank", m_other_cfg.world_boss2_heal_rank_num))
	{
		return -5;
	}

	if (!GetSubNodeValue(root_element, "act_10_lucky", m_other_cfg.world_boss2_luck_rank_num))
	{
		return -6;
	}


	return 0;
}

int WorldBossConfig::InitPosCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	while (NULL != root_element)
	{
		int group_id = 0;
		if (!GetSubNodeValue(root_element, "group_id", group_id))
		{
			return -1;
		}

		WorldBossRandPosCfg & cfg = m_rand_pos_cfg[group_id];
		WorldBossRandPosCfg::ItemCfg item_cfg;

		if (!GetSubNodeValue(root_element, "rate", item_cfg.rate) || item_cfg.rate <= 0)
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "x", item_cfg.pos.x) || item_cfg.pos.x < 0)
		{
			return -3;
		}

		if (!GetSubNodeValue(root_element, "y", item_cfg.pos.y) || item_cfg.pos.y < 0)
		{
			return -4;
		}

		cfg.group_pos.push_back(item_cfg);
		root_element = root_element->NextSiblingElement();
	}

	if (m_rand_pos_cfg.size() == 0)
	{
		return -10085;
	}

	for (std::map<int, WorldBossRandPosCfg >::iterator iter = m_rand_pos_cfg.begin(); iter != m_rand_pos_cfg.end(); ++iter)
	{
		int count = 0;
		for (int i = 0; i < (int)iter->second.group_pos.size(); ++i)
		{
			count += iter->second.group_pos[i].rate;
		}

		if (count != WorldBossRandPosCfg::MAX_RATE_COUNT)
		{
			return -10086;
		}
	}

	return 0;
}
