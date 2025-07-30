#include "activityreliclootingconfig.hpp"
#include "npc//npcpool.h"
#include "servercommon/activitydef.hpp"

ActivityRelicLootingConfig::ActivityRelicLootingConfig()
{
}

ActivityRelicLootingConfig::~ActivityRelicLootingConfig()
{
	std::vector<SkillDropCfg>().swap(m_skill_drop_vec);
	std::vector<DropPositionCfg>().swap(m_drop_posi_vec);
}

bool ActivityRelicLootingConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;
	
	LOAD_CONFIG("other", InitOtherCfg);
	LOAD_CONFIG("skill", InitSkillCfg);
	LOAD_CONFIG("res_release", InitResReleaseCfg);
	LOAD_CONFIG("box_position", InitBoxPositionCfg);
	LOAD_CONFIG("box_reward", InitBoxRewardCfg);
	LOAD_CONFIG("rank_reward", InitRankRewardCfg);

	return true;
}

const RelicLootingSkillCfg * ActivityRelicLootingConfig::GetSkillDetail(int skill_id)
{
	std::map<int, RelicLootingSkillCfg>::iterator it = m_skill_info_map.find(skill_id);
	if (it != m_skill_info_map.end())
	{
		return &it->second;
	}
	return NULL;
}

const SkillDropCfg * ActivityRelicLootingConfig::RandSkill()
{
	if (drop_weight <= 0)return NULL;
	int rand_weight = rand() % drop_weight;
	for (std::vector<SkillDropCfg>::iterator it = m_skill_drop_vec.begin(); it != m_skill_drop_vec.end(); it++)
	{
		if (rand_weight < it->res_chance)
		{
			return &(*it);
		}
		rand_weight -= it->res_chance;
	}
	return NULL;
}

const RankRewardCfg * ActivityRelicLootingConfig::GetRankRewardCfg(int rank_pos)
{
	for (std::map<int, RankRewardCfg>::iterator it = m_rank_reward_map.begin(); it != m_rank_reward_map.end(); it++)
	{
		if (rank_pos >= it->second.rank_range_min && rank_pos <= it->second.rank_range_max)
		{
			return &it->second;
		}
	}
	return NULL;
}

const RewardBoxCfg * ActivityRelicLootingConfig::GetRewardCfg(int sn)
{
	if (sn < 0 || sn >= MAX_RELIC_LOOTING_REWARD_NUM)return NULL;
	std::map<int, RewardBoxCfg>::iterator it = m_reward_box_map.find(sn);
	if (it != m_reward_box_map.end())
	{
		return &it->second;
	}
	return NULL;
}

int ActivityRelicLootingConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -__LINE__;
	}

	if (!GetSubNodeValue(root_element, "scene_id", m_other_cfg.scene_id))
	{
		return -__LINE__;
	}

	if (!GetSubNodeValue(root_element, "duration", m_other_cfg.duration))
	{
		return -__LINE__;
	}

	if (!GetSubNodeValue(root_element, "pick_distance", m_other_cfg.pick_distance))
	{
		return -__LINE__;
	}

	if (!GetSubNodeValue(root_element, "first_props", m_other_cfg.first_props))
	{
		return -__LINE__;
	}

	if (!GetSubNodeValue(root_element, "props_time", m_other_cfg.props_time))
	{
		return -__LINE__;
	}

	if (!GetSubNodeValue(root_element, "props_num", m_other_cfg.props_num))
	{
		return -__LINE__;
	}

	if (!GetSubNodeValue(root_element, "once_team_players", m_other_cfg.once_team_players) || m_other_cfg.once_team_players > RELIC_LOOT_MAX_ONE_SIDE_PLAYER_NUM || m_other_cfg.once_team_players <= 0)
	{
		return -__LINE__;
	}

	if (!GetSubNodeValue(root_element, "win_box_num", m_other_cfg.win_box_num))
	{
		return -__LINE__;
	}

	for (int i = 0; i < ARRAY_LENGTH(m_other_cfg.defender_pos_list); ++i)
	{
		std::string s = STRING_SPRINTF("defender_pos_x_%d", i + 1);
		if (!GetSubNodeValue(root_element, s, m_other_cfg.defender_pos_list[i].x))
		{
			return -__LINE__;
		}
		s = STRING_SPRINTF("defender_pos_y_%d", i + 1);
		if (!GetSubNodeValue(root_element, s, m_other_cfg.defender_pos_list[i].y))
		{
			return -__LINE__;
		}
	}

	for (int i = 0; i < ARRAY_LENGTH(m_other_cfg.defender_pos_list); ++i)
	{
		std::string s = STRING_SPRINTF("attacker_pos_x_%d", i + 1);
		if (!GetSubNodeValue(root_element, s, m_other_cfg.attacker_pos_list[i].x))
		{
			return -__LINE__;
		}
		s = STRING_SPRINTF("attacker_pos_y_%d", i + 1);
		if (!GetSubNodeValue(root_element, s, m_other_cfg.attacker_pos_list[i].y))
		{
			return -__LINE__;
		}
	}

	if (!GetSubNodeValue(root_element, "box_score", m_other_cfg.box_score))
	{
		return -__LINE__;
	}

	if (!GetSubNodeValue(root_element, "capture_score", m_other_cfg.capture_score))
	{
		return -__LINE__;
	}

	if (!GetSubNodeValue(root_element, "first_score", m_other_cfg.first_score))
	{
		return -__LINE__;
	}

	if (!GetSubNodeValue(root_element, "second_score", m_other_cfg.second_score))
	{
		return -__LINE__;
	}

	if (!GetSubNodeValue(root_element, "third_score", m_other_cfg.third_score))
	{
		return -__LINE__;
	}

	if (!GetSubNodeValue(root_element, "time_score_3", m_other_cfg.time_score_3))
	{
		return -__LINE__;
	}

	if (!GetSubNodeValue(root_element, "time_score_5", m_other_cfg.time_score_5))
	{
		return -__LINE__;
	}

	if (!GetSubNodeValue(root_element, "winner_score", m_other_cfg.winner_score))
	{
		return -__LINE__;
	}

	if (!GetSubNodeValue(root_element, "loser_score", m_other_cfg.loser_score))
	{
		return -__LINE__;
	}

	return 0;
}

int ActivityRelicLootingConfig::InitSkillCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}
	int pre_sn = 1;
	while (NULL != root_element)
	{
		int sn = 0;
		if (!GetSubNodeValue(root_element, "sn", sn) || sn < RelicLootingSkillCfg::SKILL_ID_1 || sn > RelicLootingSkillCfg::SKILL_ID_14 || sn != pre_sn)
		{
			return -1;
		}
		RelicLootingSkillCfg &cfg = m_skill_info_map[sn];
		cfg.sn = sn;
		pre_sn += 1;

		if (!GetSubNodeValue(root_element, "skill_type", cfg.skill_type))
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "param1", cfg.param1))
		{
			return -3;
		}

		TiXmlElement * param2_element = root_element->FirstChildElement("param2");
		if (param2_element != NULL && param2_element->FirstChild())
		{
			if (!GetSubNodeValue(root_element, "param2", cfg.param2))
			{
				return -4;
			}
		}

		TiXmlElement * param3_element = root_element->FirstChildElement("param3");
		if (param3_element != NULL && param3_element->FirstChild())
		{
			if (!GetSubNodeValue(root_element, "param3", cfg.param3))
			{
				return -5;
			}
		}
		
		TiXmlElement * param4_element = root_element->FirstChildElement("param4");
		if (param4_element != NULL && param4_element->FirstChild())
		{
			if (!GetSubNodeValue(root_element, "param4", cfg.param4))
			{
				return -6;
			}
		}

		TiXmlElement * param5_element = root_element->FirstChildElement("param5");
		if (param5_element != NULL && param5_element->FirstChild())
		{
			if (!GetSubNodeValue(root_element, "param5", cfg.param5))
			{
				return -7;
			}
		}

		switch (cfg.sn)
		{
		case RelicLootingSkillCfg::SKILL_ID_1:
			{
				if (cfg.param1 <= 0)return __LINE__;
				if (cfg.param2 <= 0)return __LINE__;
				if (cfg.param3 <= 0)return __LINE__;
			}
			break;
		case RelicLootingSkillCfg::SKILL_ID_2:
			{
				if (cfg.param1 <= 0)return __LINE__;
				if (cfg.param2 <= 0)return __LINE__;
				if (cfg.param3 <= 0)return __LINE__;
				if (cfg.param4 <= 0)return __LINE__;
			}
			break;
		case RelicLootingSkillCfg::SKILL_ID_3:
			{
				if (cfg.param1 <= 0)return __LINE__;
				if (cfg.param2 <= 0)return __LINE__;
			}
			break;
		case RelicLootingSkillCfg::SKILL_ID_4:
			{
				if (cfg.param1 <= 0)return __LINE__;
				if (cfg.param2 <= 0)return __LINE__;
			}
			break;
		case RelicLootingSkillCfg::SKILL_ID_5:
			{
				if (cfg.param1 <= 0)return __LINE__;
				if (cfg.param2 <= 0)return __LINE__;
			}
			break;
		case RelicLootingSkillCfg::SKILL_ID_6:
			{
				if (cfg.param1 <= 0)return __LINE__;
				if (cfg.param2 <= 0)return __LINE__;
				if (cfg.param3 <= 0)return __LINE__;
			}
			break;
		case RelicLootingSkillCfg::SKILL_ID_7:
			{
				if (cfg.param1 <= 0)return __LINE__;
				if (cfg.param2 <= 0)return __LINE__;
				if (cfg.param3 <= 0)return __LINE__;
				if (cfg.param4 <= 0)return __LINE__;
				if (cfg.param5 <= 0)return __LINE__;
			}
			break;
		case RelicLootingSkillCfg::SKILL_ID_8:
			{
				if (cfg.param1 <= 0)return __LINE__;
				if (cfg.param2 <= 0)return __LINE__;
			}
			break;
		case RelicLootingSkillCfg::SKILL_ID_9:
			{
				if (cfg.param1 <= 0)return __LINE__;
			}
			break;
		case RelicLootingSkillCfg::SKILL_ID_10:
			{
				if (cfg.param1 <= 0)return __LINE__;
			}
			break;
		case RelicLootingSkillCfg::SKILL_ID_11:
			{
				if (cfg.param1 <= 0)return __LINE__;
				if (cfg.param2 <= 0)return __LINE__;
			}
			break;
		case RelicLootingSkillCfg::SKILL_ID_12:
			{
				if (cfg.param1 <= 0)return __LINE__;
			}
			break;
		case RelicLootingSkillCfg::SKILL_ID_13:
			{
				if (cfg.param1 <= 0)return __LINE__;
				if (cfg.param2 <= 0)return __LINE__;
			}
			break;
		case RelicLootingSkillCfg::SKILL_ID_14:
			{
				if (cfg.param1 <= 0)return __LINE__;
			}
			break;
		default:
			break;
		}
		
		if (!GetSubNodeValue(root_element, "is_item", cfg.skill_limit))
		{
			return -8;
		}

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int ActivityRelicLootingConfig::InitResReleaseCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	drop_weight = 0;
	while (NULL != root_element)
	{
		SkillDropCfg cfg;
		if (!GetSubNodeValue(root_element, "sn", cfg.sn))
		{
			return -1;
		}

		if (!GetSubNodeValue(root_element, "res_chance", cfg.res_chance))
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "npc_id", cfg.npc_id) || NULL == NPCPOOL->GetServerNPCInfo(cfg.npc_id))
		{
			return -3;
		}

		drop_weight += cfg.res_chance;
		m_skill_drop_vec.push_back(cfg);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int ActivityRelicLootingConfig::InitBoxPositionCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	while (NULL != root_element)
	{
		DropPositionCfg cfg;
		if (!GetSubNodeValue(root_element, "box_sn", cfg.box_sn))
		{
			return -1;
		}

		if (!GetSubNodeValue(root_element, "box_x", cfg.box_x))
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "box_y", cfg.box_y))
		{
			return -3;
		}

		if (!GetSubNodeValue(root_element, "boss_dir", cfg.boss_dir))
		{
			return -4;
		}

		if (!GetSubNodeValue(root_element, "npc_id", cfg.npc_id) || NULL == NPCPOOL->GetServerNPCInfo(cfg.npc_id))
		{
			return -5;
		}

		m_drop_posi_vec.push_back(cfg);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int ActivityRelicLootingConfig::InitBoxRewardCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}
	int pre_sn = 0;
	while (NULL != root_element)
	{
		int sn = 0;
		if (!GetSubNodeValue(root_element, "sn", sn) || sn != pre_sn + 1)
		{
			return -1;
		}
		pre_sn += 1;
		RewardBoxCfg & cfg = m_reward_box_map[sn];
		cfg.sn = sn;
		if (!GetSubNodeValue(root_element, "param_type", cfg.param_type) || cfg.param_type < ActivityRelicLootingRoleData::RELIC_LOOTING_PARAM_TYPE_0 ||
			cfg.param_type >= ActivityRelicLootingRoleData::RELIC_LOOTING_PARAM_TYPE_MAX)
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "param", cfg.param) || cfg.param <= 0)
		{
			return -3;
		}

		TiXmlElement * reward_list_element = root_element->FirstChildElement("item_list_list");

		if (reward_list_element != NULL)
		{
			TiXmlElement * reward_element = reward_list_element->FirstChildElement("item_list");

			while (reward_element)
			{
				ItemConfigData reward;

				if (!reward.ReadConfig(reward_element))
				{
					return -4;
				}

				cfg.m_reward_vec.push_back(reward);
				reward_element = reward_element->NextSiblingElement();
			}
		}
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int ActivityRelicLootingConfig::InitRankRewardCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	int pre_sn = 0;
	while (NULL != root_element)
	{
		int sn = 0;
		if (!GetSubNodeValue(root_element, "sn", sn) || sn != pre_sn + 1)
		{
			return -1;
		}
		pre_sn += 1;
		RankRewardCfg & cfg = m_rank_reward_map[sn];

		if (!GetSubNodeValue(root_element, "rank_range_min", cfg.rank_range_min))
		{
			return -3;
		}

		if (!GetSubNodeValue(root_element, "rank_range_max", cfg.rank_range_max))
		{
			return -4;
		}

		TiXmlElement * reward_list_element = root_element->FirstChildElement("item_list_list");

		if (reward_list_element != NULL)
		{
			TiXmlElement * reward_element = reward_list_element->FirstChildElement("item_list");

			while (reward_element)
			{
				ItemConfigData reward;

				if (!reward.ReadConfig(reward_element))
				{
					return -104;
				}

				cfg.m_rank_reward_vec.push_back(reward);
				reward_element = reward_element->NextSiblingElement();
			}
		}
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}