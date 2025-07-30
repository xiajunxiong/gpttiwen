#include "netherworldpurgatoryconfig.hpp"
#include "servercommon/struct/itemlistparam.h"

NetherWorldPurgatoryConfig::NetherWorldPurgatoryConfig()
{
}

NetherWorldPurgatoryConfig::~NetherWorldPurgatoryConfig()
{
	std::vector<NetherWorldPurgatoryMonsterPassRewardCfg>().swap(pass_reward_vec);
}

bool NetherWorldPurgatoryConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG("pass_reward", InitMonsterPassRewardCfg);
	LOAD_CONFIG("silver_reward", InitSilverRewardCfg);
	LOAD_CONFIG("gold_reward", InitGoldRewardCfg);
	LOAD_CONFIG("other", InitOtherCfg);

	return true;
}

const NetherWorldPurgatoryMonsterCfg * NetherWorldPurgatoryConfig::GetMonsterConditionCfg(int capther_id, int challenge_id, int monster_group_id)
{
	if (challenge_id - 1 < 0 || challenge_id - 1 >= MAX_NETHER_WORLD_GATORY_INDEX)return NULL;
	if (capther_id - 1 < 0 || capther_id - 1 >= MAX_NETHER_WORLD_GATORY_CHAPTER_NUM)return NULL;
	NetherWorldPurgatoryMonsterPassRewardCfg *node = pass_reward[capther_id - 1][challenge_id - 1];
	if (node != NULL)
	{
		if (node->monster_group.monster_group_id == monster_group_id)
		{
			return &node->monster_group;
		}
	}
	return NULL;
}

const NetherWorldPurgatoryMonsterPassRewardCfg * NetherWorldPurgatoryConfig::PassRewardCfg(int chapter_id, int challenge_id)
{
	if (chapter_id - 1 < 0 || chapter_id - 1 > MAX_NETHER_WORLD_GATORY_CHAPTER_NUM)return NULL;
	if (challenge_id - 1 < 0 || challenge_id - 1 >= MAX_NETHER_WORLD_GATORY_INDEX)return NULL;

	return pass_reward[chapter_id - 1][challenge_id - 1];
}

const NetherWorldPurgatoryRewardListCfg * NetherWorldPurgatoryConfig::GetRewardCfg(int chapter_id)
{
	if (chapter_id - 1 < 0 || chapter_id - 1 > MAX_NETHER_WORLD_GATORY_CHAPTER_NUM)return NULL;

	return &reward_vec[chapter_id - 1];
}

int NetherWorldPurgatoryConfig::InitMonsterPassRewardCfg(TiXmlElement * RootElement)
{
	TiXmlElement* dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -10000;
	memset(pass_reward, NULL, sizeof(pass_reward));
	while (dataElement)
	{
		NetherWorldPurgatoryMonsterPassRewardCfg node;

		if (!GetSubNodeValue(dataElement, "chapters", node.chapters) || node.chapters <= 0 || node.chapters > MAX_NETHER_WORLD_GATORY_CHAPTER_NUM)
		{
			return -1;
		}

		if (!GetSubNodeValue(dataElement, "challenge_id", node.challenge_id) || node.challenge_id <= 0 || node.challenge_id > MAX_NETHER_WORLD_GATORY_INDEX)
		{
			return -2;
		}

		if (!GetSubNodeValue(dataElement, "unlock_chapter", node.unlock_chapter) || node.unlock_chapter < 0 || node.unlock_chapter > MAX_NETHER_WORLD_GATORY_CHAPTER_NUM)
		{
			return -3;
		}

		if (!GetSubNodeValue(dataElement, "unlock_challenge", node.unlock_challenge) || node.unlock_challenge < 0 || node.unlock_challenge > MAX_NETHER_WORLD_GATORY_INDEX)
		{
			return -4;
		}

		{
			NetherWorldPurgatoryMonsterCfg & info_node = node.monster_group;
			if (!GetSubNodeValue(dataElement, "monster_group_id_1", info_node.monster_group_id))
			{
				return -5;
			}

			char path[30];
			for (int j = 0; j < MAX_NETHER_WORLD_GATORY_TASK_NUM - 1; j++)
			{
				memset(path, 0, sizeof(path));
				sprintf(path, "group1_condition_id_%d", j + 1);
				if (!GetSubNodeValue(dataElement, path, info_node.condition_id[j]))
				{
					return -6;
				}
			}

			{
				TiXmlElement* item_list_Element = dataElement->FirstChildElement("reward_1_list");
				if (NULL != item_list_Element)
				{
					TiXmlElement* item_Element = item_list_Element->FirstChildElement("reward_1");
					while (NULL != item_Element)
					{
						ItemConfigData reward_node;
						if (!reward_node.ReadConfig(item_Element))
						{
							return -7;
						}
						info_node.condition1_reward.push_back(reward_node);
						item_Element = item_Element->NextSiblingElement();
					}
				}
			}

			{
				TiXmlElement* item_list_Element = dataElement->FirstChildElement("reward_2_list");
				if (NULL != item_list_Element)
				{
					TiXmlElement* item_Element = item_list_Element->FirstChildElement("reward_2");
					while (NULL != item_Element)
					{
						ItemConfigData reward_node;
						if (!reward_node.ReadConfig(item_Element))
						{
							return -8;
						}
						info_node.condition2_reward.push_back(reward_node);
						item_Element = item_Element->NextSiblingElement();
					}
				}
			}

			{
				TiXmlElement* item_list_Element = dataElement->FirstChildElement("reward_3_list");
				if (NULL != item_list_Element)
				{
					TiXmlElement* item_Element = item_list_Element->FirstChildElement("reward_3");
					while (NULL != item_Element)
					{
						ItemConfigData reward_node;
						if (!reward_node.ReadConfig(item_Element))
						{
							return -9;
						}
						info_node.condition3_reward.push_back(reward_node);
						item_Element = item_Element->NextSiblingElement();
					}
				}
			}
		}

		{
			TiXmlElement* item_list_Element = dataElement->FirstChildElement("final_reward_list");
			if (NULL != item_list_Element)
			{
				TiXmlElement* item_Element = item_list_Element->FirstChildElement("final_reward");
				while (NULL != item_Element)
				{
					ItemConfigData reward_node;
					if (!reward_node.ReadConfig(item_Element))
					{
						return -10;
					}
					node.final_reward.push_back(reward_node);
					item_Element = item_Element->NextSiblingElement();
				}
			}
		}

		pass_reward_vec.push_back(node);
		dataElement = dataElement->NextSiblingElement();
	}

	for (std::vector<NetherWorldPurgatoryMonsterPassRewardCfg>::iterator it = pass_reward_vec.begin(); it != pass_reward_vec.end(); it++)
	{
		if (it->challenge_id - 1 >= 0 && it->challenge_id - 1 < MAX_NETHER_WORLD_GATORY_INDEX)
		{
			pass_reward[it->chapters - 1][it->challenge_id - 1] = &(*it);
		}
	}

	return 0;
}

int NetherWorldPurgatoryConfig::InitSilverRewardCfg(TiXmlElement * RootElement)
{
	TiXmlElement* dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -10000;

	while (dataElement)
	{
		int lattice = 0;
		if (!GetSubNodeValue(dataElement, "lattice", lattice) || lattice <= 0 || lattice > MAX_NETHER_WORLD_GATORY_INDEX)
		{
			return -1;
		}
		int chapters = 0;
		if (!GetSubNodeValue(dataElement, "chapters", chapters) || chapters <= 0 || chapters > MAX_NETHER_WORLD_GATORY_CHAPTER_NUM)
		{
			return -2;
		}
		NetherWorldPurgatoryRewardCfg & node = reward_vec[chapters - 1].silver_vec[lattice - 1];

		node.lattice = lattice;
		node.chapters = chapters;

		TiXmlElement* item_Element = dataElement->FirstChildElement("silver_reward");
		if (NULL != item_Element)
		{
			ItemConfigData reward_node;
			if (!reward_node.ReadConfig(item_Element))
			{
				return -3;
			}
			node.reward.push_back(reward_node);
		}

		dataElement = dataElement->NextSiblingElement();
	}
	return 0;
}

int NetherWorldPurgatoryConfig::InitGoldRewardCfg(TiXmlElement * RootElement)
{
	TiXmlElement* dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -10000;

	while (dataElement)
	{
		int lattice = 0;
		if (!GetSubNodeValue(dataElement, "lattice", lattice) || lattice <= 0 || lattice > MAX_NETHER_WORLD_GATORY_INDEX)
		{
			return -1;
		}

		int chapters = 0;
		if (!GetSubNodeValue(dataElement, "chapters", chapters) || chapters <= 0 || chapters > MAX_NETHER_WORLD_GATORY_CHAPTER_NUM)
		{
			return -2;
		}

		NetherWorldPurgatoryRewardCfg & node = reward_vec[chapters - 1].gold_vec[lattice - 1];

		node.chapters = chapters;
		node.lattice = lattice;

		TiXmlElement* item_Element = dataElement->FirstChildElement("gold_reward");
		if (NULL != item_Element)
		{
			ItemConfigData reward_node;
			if (!reward_node.ReadConfig(item_Element))
			{
				return -3;
			}
			node.reward.push_back(reward_node);
		}

		dataElement = dataElement->NextSiblingElement();
	}
	return 0;
}

int NetherWorldPurgatoryConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement* dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -10000;

	{
		if (!GetSubNodeValue(dataElement, "times", other_cfg.times) || other_cfg.times < 0)
		{
			return -1;
		}

		if (!GetSubNodeValue(dataElement, "dibuff_id", other_cfg.debuff_id) || other_cfg.debuff_id <= 0)
		{
			return -2;
		}

		if (!GetSubNodeValue(dataElement, "dibuff_level", other_cfg.debuff_level) || other_cfg.debuff_level <= 0)
		{
			return -2;
		}
	}

	return 0;
}
