#include "couragechallengeconfig.h"
#include "checkresourcecenter.hpp"
#include "gameworld/monster/monster_group_pool.hpp"
#include "servercommon/struct/battle/battle_def.hpp"

CourageChallengeConfig::CourageChallengeConfig()
{
	memset(m_pet_helper_num_clear_level_hash, 0, sizeof(m_pet_helper_num_clear_level_hash));
}

CourageChallengeConfig::~CourageChallengeConfig()
{
	for (std::map<int, std::vector<CourageChallengeLevelCfg> >::iterator it = m_level_cfg_by_layer_level.begin(); it != m_level_cfg_by_layer_level.end(); it++)
	{
		std::vector<CourageChallengeLevelCfg>().swap(it->second);
	}
}

bool CourageChallengeConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG("challenge", InitLayerLevelCfg);
	LOAD_CONFIG("other", InitOtherCfg);
	LOAD_CONFIG("score_reward", InitScoreRewardCfg);
	LOAD_CONFIG("star_reward", InitMaxScoreRewardCfg);
	LOAD_CONFIG("help_fight", InitPetHelperCfg);
	LOAD_CONFIG("new_challenge", InitNewLevelCfg);
	LOAD_CONFIG("achievement_reward", InitNewRewardCfg);

	return true;
}

const CourageChallengeOtherCfg& CourageChallengeConfig::GetOtherCfg()
{
	return m_other_cfg;
}

const CourageChallengeLevelCfg * CourageChallengeConfig::GetLevelCfg(int layer_level, int level_seq)
{
	CourageScoreKey level_key(layer_level, level_seq);
	std::map<CourageScoreKey, CourageChallengeLevelCfg>::iterator it = m_level_cfg.find(level_key);
	if (it != m_level_cfg.end()) return &it->second;

	return NULL;
}

const CourageChallengeLevelCfg * CourageChallengeConfig::GetLevelCfgByMonsterGroupId(int monster_group_id)
{
	std::map<int, CourageChallengeLevelCfg>::iterator it = m_level_cfg_by_monster_group_id.find(monster_group_id);
	if(it != m_level_cfg_by_monster_group_id.end()) return &it->second;

	return NULL;
}

const std::vector<CourageChallengeLevelCfg>* CourageChallengeConfig::GetLevelCfgByLayerLevel(int layer_level)
{
	std::map<int, std::vector<CourageChallengeLevelCfg> >::iterator it = m_level_cfg_by_layer_level.find(layer_level);
	if(it == m_level_cfg_by_layer_level.end()) return NULL;

	return &it->second;
}

const CourageChallengeScoreRewardCfg * CourageChallengeConfig::GetScoreRewardCfg(int layer_level, int level_seq)
{
	CourageScoreKey level_key(layer_level, level_seq);
	std::map<CourageScoreKey, CourageChallengeScoreRewardCfg>::iterator it = m_score_reward_cfg.find(level_key);
	if (it != m_score_reward_cfg.end()) return &it->second;

	return NULL;
}

const CourageChallengeMaxScoreRewardCfg * CourageChallengeConfig::GetMaxScoreRewardCfg(int layer_level)
{
	std::map<int, CourageChallengeMaxScoreRewardCfg>::iterator it = m_max_score_reward_cfg.find(layer_level);
	if(it != m_max_score_reward_cfg.end()) return &it->second;

	return NULL;
}

int CourageChallengeConfig::GetPetHelperNum(int clear_level_num)
{
	int ret = 0;
	for (int i = 1; i < ARRAY_LENGTH(m_pet_helper_num_clear_level_hash); ++i)
	{
		if (clear_level_num >= m_pet_helper_num_clear_level_hash[i])
		{
			ret = i;
		}
		else
		{
			break;
		}
	}

	return ret;
}

const NewCourageChallengeLevelCfg * CourageChallengeConfig::GetNewLevelCfg(int layer_level, int level_seq) const
{
	CourageScoreKey level_key(layer_level, level_seq);
	std::map<CourageScoreKey, NewCourageChallengeLevelCfg>::const_iterator it = m_new_level_cfg.find(level_key);
	if (it != m_new_level_cfg.end())
	{
		return &it->second;
	}
	return NULL;
}

const NewCourageChallengeLevelCfg * CourageChallengeConfig::GetNewLevelCfgByMonster(int monster_group_id) const
{
	std::map<int, NewCourageChallengeLevelCfg>::const_iterator it = m_new_level_cfg_by_monster.find(monster_group_id);
	if (it != m_new_level_cfg_by_monster.end())
	{
		return &it->second;
	}
	return NULL;
}

const NewCourageChallengeRewardCfg * CourageChallengeConfig::GetNewRewardCfg(int reward_seq) const
{
	if (0 > reward_seq || reward_seq >= (int)m_new_reward_cfg.size())
	{
		return NULL;
	}
	return &m_new_reward_cfg[reward_seq];
}

int CourageChallengeConfig::InitLayerLevelCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -999;
	}
	min_layer_level = 0;
	int last_seq = 0;
	while (NULL != data_element)
	{
		int seq = 0;
		if (!GetSubNodeValue(data_element, "seq", seq) || seq != last_seq + 1)
		{
			return -111;
		}
		last_seq = seq;

		int layer_level = 0, level_seq = 0;
		if (!GetSubNodeValue(data_element, "layer_level", layer_level) || layer_level <= 0 || layer_level > MAX_ROLE_LEVEL)
		{
			return -1;
		}
		if (!GetSubNodeValue(data_element, "level_seq", level_seq) || level_seq < 0 || level_seq >= MAX_COURAGE_CHALLENGE_LEVEL_SEQ)
		{
			return -2;
		}

		CourageScoreKey level_key(layer_level, level_seq);
		if(m_level_cfg.find(level_key) != m_level_cfg.end()) return -888;

		CourageChallengeLevelCfg cfg;
		cfg.seq = seq;
		cfg.layer_level = layer_level;
		cfg.level_seq = level_seq;

		if (!GetSubNodeValue(data_element, "level", cfg.level) || cfg.level <= 0 || cfg.level > MAX_ROLE_LEVEL)
		{
			return -3;
		}

		char condition[64] = {0};
		for (int i = 1; i <= COURAGE_CHALLENGE_CONDITION_MAX_NUM; i++)
		{
			SNPRINTF(condition, sizeof(condition), "condition_%d", i);
			if (!GetSubNodeValue(data_element, condition, cfg.condition[i - 1]) || cfg.condition[i - 1] < 0)
			{
				return -10 - i;
			}
		}

		if (!GetSubNodeValue(data_element, "monster_group", cfg.monster_group_id) || (NULL == MonsterGroupPool::GetInstance().GetMonsterGroupCfg(cfg.monster_group_id)))
		{
			return -4;
		}
		if (m_level_cfg_by_monster_group_id.find(cfg.monster_group_id) != m_level_cfg_by_monster_group_id.end()) return -777;

		if (!GetSubNodeValue(data_element, "coin", cfg.coin))
		{
			return -5;
		}

		TiXmlElement * clear_reward_list = data_element->FirstChildElement("clear_reward_list");
		if (NULL == clear_reward_list)
		{
			return -103;
		}
		TiXmlElement * clear_reward = clear_reward_list->FirstChildElement("clear_reward");
		while (clear_reward != NULL)
		{
			ItemConfigData reward;
			if (!reward.ReadConfig(clear_reward))
			{
				return -104;
			}
			cfg.pass_reward.push_back(reward);
			clear_reward = clear_reward->NextSiblingElement();
		}

		TiXmlElement * extra_reward_list = data_element->FirstChildElement("extra_reward_list");
		if (NULL == extra_reward_list)
		{
			return -105;
		}
		TiXmlElement * extra_reward = extra_reward_list->FirstChildElement("extra_reward");
		while (extra_reward != NULL)
		{
			ItemConfigData reward;
			if (!reward.ReadConfig(extra_reward))
			{
				return -106;
			}
			cfg.extra_reward.push_back(reward);
			extra_reward = extra_reward->NextSiblingElement();
		}

		if (min_layer_level == 0)
		{
			min_layer_level = cfg.layer_level;
		}
		else
		{
			min_layer_level = min_layer_level <= cfg.layer_level ? min_layer_level : cfg.layer_level;
		}

		if (!GetSubNodeValue(data_element, "attacker_buff_id", cfg.attacker_buff_id))
		{
			return -44;
		}

		if (!GetSubNodeValue(data_element, "attacker_buff_level", cfg.attacker_buff_level))
		{
			return -55;
		}

		if (!GetSubNodeValue(data_element, "num", cfg.guanqia_num) || cfg.guanqia_num <= 0)
		{
			return -66;
		}

		m_level_cfg[level_key] = cfg;
		m_level_cfg_by_monster_group_id[cfg.monster_group_id] = cfg;
		if (m_level_cfg.size() > MAX_COURAGE_CHALLENGE_LEVEL_NUM)
		{
			return -7777;
		}

		m_level_cfg_by_layer_level[layer_level].push_back(cfg);
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int CourageChallengeConfig::InitOtherCfg(TiXmlElement *RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -999;
	}

	if (!GetSubNodeValue(data_element, "open_level", m_other_cfg.open_level) || m_other_cfg.open_level < 0 || m_other_cfg.open_level > MAX_ROLE_LEVEL)
	{
		return -1;
	}

	if (!GetSubNodeValue(data_element, "refresh_time", m_other_cfg.refresh_time))
	{
		return -2;
	}

	if (!GetSubNodeValue(data_element, "scene_id", m_other_cfg.scene_id))
	{
		return -3;
	}

	if (!GetSubNodeValue(data_element, "extra_reward_level", m_other_cfg.extra_reward_level))
	{
		return -4;
	}

	if (!GetSubNodeValue(data_element, "new_scene_id", m_other_cfg.new_scene_id))
	{
		return -5;
	}

	if (!GetSubNodeValue(data_element, "reward_prize", m_other_cfg.new_token_reward_prize))
	{
		return -6;
	}

	CheckResourceCenter::Instance().GetSceneCheck()->Add(m_other_cfg.scene_id, __FUNCTION__);
	CheckResourceCenter::Instance().GetSceneCheck()->Add(m_other_cfg.new_scene_id, __FUNCTION__);

	return 0;
}

int CourageChallengeConfig::InitScoreRewardCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -999;
	}

	while (NULL != data_element)
	{
		int layer_level = 0, seq = 0;

		if (!GetSubNodeValue(data_element, "layer_level", layer_level) || layer_level <= 0 || layer_level > MAX_ROLE_LEVEL)
		{
			return -1;
		}
		if (!GetSubNodeValue(data_element, "seq", seq) || seq < 0 || seq >= COURAGE_CHALLENGE_SCORE_LAYER_LEVEL_MAX_NUM - 1)	
		{
			return -2;
		}
		CourageScoreKey score_key(layer_level, seq);
		if(m_score_reward_cfg.find(score_key) != m_score_reward_cfg.end()) return -888;

		CourageChallengeScoreRewardCfg node;
		if (!GetSubNodeValue(data_element, "star", node.star) || node.star < 0)
		{
			return -3;
		}

		TiXmlElement * reward_list_element = data_element->FirstChildElement("reward_list");
		if (NULL == reward_list_element)
		{
			return -100;
		}
		TiXmlElement * reward_element = reward_list_element->FirstChildElement("reward");
		while (reward_element != NULL)
		{
			ItemConfigData reward;
			if (!reward.ReadConfig(reward_element))
			{
				return -100 - node.star;
			}
			node.score_reward.push_back(reward);
			reward_element = reward_element->NextSiblingElement();
		}

		m_score_reward_cfg[score_key] = node;
		data_element = data_element->NextSiblingElement();
	}
	
	return 0;
}

int CourageChallengeConfig::InitMaxScoreRewardCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -999;
	}

	while (NULL != data_element)
	{
		int layer_level = 0;
		if (!GetSubNodeValue(data_element, "layer_level", layer_level) || layer_level <= 0 || layer_level > MAX_ROLE_LEVEL)
		{
			return -1;
		}
		CourageChallengeMaxScoreRewardCfg cfg;
		if (!GetSubNodeValue(data_element, "max_star", cfg.max_star) || cfg.max_star <= 0)
		{
			return -2;
		}
		if (!GetSubNodeValue(data_element, "reset", cfg.version))
		{
			return -3;
		}

		TiXmlElement * reward_list_element = data_element->FirstChildElement("reward_list");
		if (NULL == reward_list_element)
		{
			return -100;
		}
		TiXmlElement * reward_element = reward_list_element->FirstChildElement("reward");
		while (reward_element != NULL)
		{
			ItemConfigData reward;
			if (!reward.ReadConfig(reward_element))
			{
				return -100 - layer_level;
			}
			cfg.max_score_reward.push_back(reward);
			reward_element = reward_element->NextSiblingElement();
		}

		m_max_score_reward_cfg[layer_level] = cfg;
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int CourageChallengeConfig::InitPetHelperCfg(TiXmlElement *RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -999;
	}

	int last_pet_num = 0;
	while (NULL != data_element)
	{
		int num = 0;
		if (!GetSubNodeValue(data_element, "num", num) || num <= 0)
		{
			return -1;
		}

		int pet_num = 0;
		if (!GetSubNodeValue(data_element, "pet_num", pet_num) || pet_num != last_pet_num + 1 || pet_num >= ARRAY_LENGTH(m_pet_helper_num_clear_level_hash))
		{
			return -2;
		}
		last_pet_num = pet_num;

		if (num <= m_pet_helper_num_clear_level_hash[pet_num - 1])
		{
			return -3;
		}

		m_pet_helper_num_clear_level_hash[pet_num] = num;

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int CourageChallengeConfig::InitNewLevelCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -999;
	}

	int last_seq = 0;
	while (NULL != data_element)
	{
		int seq = 0;
		if (!GetSubNodeValue(data_element, "seq", seq) || seq != last_seq + 1 || seq >= MAX_NEW_COURAGE_CHALLENGE_LIST_NUM)
		{
			return -1;
		}
		last_seq = seq;

		int layer_level = 0, level_seq = 0;
		if (!GetSubNodeValue(data_element, "layer_level", layer_level) || layer_level <= 0)
		{
			return -2;
		}
		if (!GetSubNodeValue(data_element, "level_seq", level_seq) || level_seq < 0)
		{
			return -3;
		}

		CourageScoreKey level_key(layer_level, level_seq);
		if (m_new_level_cfg.find(level_key) != m_new_level_cfg.end()) return -111;

		NewCourageChallengeLevelCfg cfg;
		cfg.seq = seq;
		cfg.layer_level = layer_level;
		cfg.level_seq = level_seq;

		if (!GetSubNodeValue(data_element, "level", cfg.level) || cfg.level <= 0)
		{
			return -4;
		}

		char condition[64] = { 0 };
		for (int i = 1; i <= COURAGE_CHALLENGE_CONDITION_MAX_NUM; i++)
		{
			SNPRINTF(condition, sizeof(condition), "condition_%d", i);
			if (!GetSubNodeValue(data_element, condition, cfg.condition[i - 1]) || cfg.condition[i - 1] < 0)
			{
				return -10 - i;
			}
		}

		if (!GetSubNodeValue(data_element, "monster_group", cfg.monster_group_id) || (NULL == MonsterGroupPool::GetInstance().GetMonsterGroupCfg(cfg.monster_group_id)))
		{
			return -5;
		}
		if (m_new_level_cfg_by_monster.find(cfg.monster_group_id) != m_new_level_cfg_by_monster.end()) return -222;

		TiXmlElement * clear_reward_list = data_element->FirstChildElement("clear_reward_list");
		if (NULL == clear_reward_list)
		{
			return -101;
		}
		TiXmlElement * clear_reward = clear_reward_list->FirstChildElement("clear_reward");
		while (clear_reward != NULL)
		{
			ItemConfigData reward;
			if (!reward.ReadConfig(clear_reward))
			{
				return -102;
			}
			cfg.pass_reward.push_back(reward);
			clear_reward = clear_reward->NextSiblingElement();
		}

		TiXmlElement * extra_reward_list = data_element->FirstChildElement("extra_reward_list");
		if (NULL != extra_reward_list)
		{
			TiXmlElement * extra_reward = extra_reward_list->FirstChildElement("extra_reward");
			while (extra_reward != NULL)
			{
				ItemConfigData reward;
				if (reward.ReadConfig(extra_reward))
				{
					cfg.extra_reward.push_back(reward);
				}
				extra_reward = extra_reward->NextSiblingElement();
			}
		}

		m_new_level_cfg[level_key] = cfg;
		m_new_level_cfg_by_monster[cfg.monster_group_id] = cfg;
		if ((int)m_level_cfg.size() > MAX_NEW_COURAGE_CHALLENGE_LIST_NUM)
		{
			return -999;
		}

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int CourageChallengeConfig::InitNewRewardCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -999;
	}

	while (NULL != data_element)
	{
		NewCourageChallengeRewardCfg node;
		if (!GetSubNodeValue(data_element, "reward_seq", node.seq) || 0 > node.seq || node.seq != (int)m_new_reward_cfg.size())
		{
			return -1;
		}

		if (!GetSubNodeValue(data_element, "star_num", node.star_num) || 0 > node.star_num)
		{
			return -2;
		}
		
		TiXmlElement * common_reward_list = data_element->FirstChildElement("reward_list");
		if (NULL == common_reward_list)
		{
			return -101;
		}
		TiXmlElement * common_reward = common_reward_list->FirstChildElement("reward");
		while (common_reward != NULL)
		{
			ItemConfigData reward;
			if (!reward.ReadConfig(common_reward))
			{
				return -102;
			}
			node.reward.push_back(reward);
			common_reward = common_reward->NextSiblingElement();
		}

		TiXmlElement * token_reward_list = data_element->FirstChildElement("higher_reward_list");
		if (NULL == token_reward_list)
		{
			return -103;
		}
		TiXmlElement * token_reward = token_reward_list->FirstChildElement("higher_reward");
		while (token_reward != NULL)
		{
			ItemConfigData reward;
			if (!reward.ReadConfig(token_reward))
			{
				return -104;
			}
			node.token_reward.push_back(reward);
			token_reward = token_reward->NextSiblingElement();
		}

		m_new_reward_cfg.push_back(node);
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}
