#include "schoolexerciseconfig.hpp"
#include "checkresourcecenter.hpp"

SchoolExerciseConfig::SchoolExerciseConfig() : m_challenge_cfg_num(0)
{

}

SchoolExerciseConfig::~SchoolExerciseConfig()
{

}

bool SchoolExerciseConfig::Init(const std::string &configname, std::string *err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG("challenge", InitChallengeCfg);

	return true;
}

const SchoolExerciseChallengeCfg* SchoolExerciseConfig::GetChallengeCfg(int seq)
{
	if (seq < 0 || seq >= m_challenge_cfg_num) return NULL;

	return &m_challenge_cfg_list[seq];
}

int SchoolExerciseConfig::InitChallengeCfg(TiXmlElement * RootElement)
{
	TiXmlElement* dataElement = RootElement->FirstChildElement("data");

	int last_seq = -1;
	while (dataElement)
	{
		int seq = 0;
		if (!GetSubNodeValue(dataElement, "seq", seq) || seq != last_seq + 1 || seq < 0 || seq >= ARRAY_LENGTH(m_challenge_cfg_list))
		{
			return -1;
		}
		last_seq = seq;

		SchoolExerciseChallengeCfg& cfg = m_challenge_cfg_list[seq];
		cfg.seq = seq;

		if (!GetSubNodeValue(dataElement, "level", cfg.unlock_level) || cfg.unlock_level < 0 || cfg.unlock_level > MAX_ROLE_LEVEL)
		{
			return -2;
		}

		if (!GetSubNodeValue(dataElement, "monster_group", cfg.monster_group_id) || cfg.monster_group_id <= 0)
		{
			return -3;
		}
		CheckResourceCenter::Instance().GetMonsterGroupCheck()->Add(cfg.monster_group_id, __FUNCTION__);

		if (GetSubNodeValue(dataElement, "npc_demo_seq", cfg.npc_demo_seq))
		{
			if (cfg.npc_demo_seq < 0) return -33;
		}

		TiXmlElement* clear_reward_list_element = dataElement->FirstChildElement("clear_reward_list");
		if (NULL == clear_reward_list_element)
		{
			return -4;
		}

		TiXmlElement* clear_reward_element = clear_reward_list_element->FirstChildElement("clear_reward");
		cfg.item_reward_num = 0;
		while (clear_reward_element)
		{
			if (!cfg.item_reward_list[cfg.item_reward_num].ReadConfig(clear_reward_element))
			{
				printf("SchoolExerciseConfig::InitChallengeCfg Error Reading Item Reward, seq[%d]\n", seq);
				return -5;
			}
			cfg.item_reward_num += 1;
			clear_reward_element = clear_reward_element->NextSiblingElement();
		}

		if (!GetSubNodeValue(dataElement, "coin", cfg.coin_reward) || cfg.coin_reward <= 0)
		{
			return -6;
		}

		if (!GetSubNodeValue(dataElement, "group", cfg.group) || cfg.group < 0)
		{
			return -7;
		}

		if (!GetSubNodeValue(dataElement, "layer", cfg.layer) || cfg.layer < 0)
		{
			return -8;
		}

		m_challenge_cfg_num = seq + 1;
		dataElement = dataElement->NextSiblingElement();
	}

	for (int i = m_challenge_cfg_num - 1; i > 0; --i)
	{
		SchoolExerciseChallengeCfg& cur_cfg = m_challenge_cfg_list[i];
		if (0 == cur_cfg.layer) continue;

		for (int k = i - 1; k >= 0; --k)
		{
			SchoolExerciseChallengeCfg& pre_cfg = m_challenge_cfg_list[k];

			if (cur_cfg.group != pre_cfg.group) continue;

			if (cur_cfg.layer == pre_cfg.layer + 1)
			{
				cur_cfg.pre_seq = pre_cfg.seq;
				break;
			}
		}
	}

	return 0;
}

