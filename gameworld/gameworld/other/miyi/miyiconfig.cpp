#include <set>

#include "miyiconfig.hpp"
#include "monster/monster_group_pool.hpp"
#include "npc/npcpool.h"

MiYiConfig::MiYiConfig() : max_reward_proportion(0)
{
}

MiYiConfig::~MiYiConfig()
{
}

bool MiYiConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG("dexter_reward", InitLevelCfg);
	LOAD_CONFIG("reward", InitRewardCfg);
	LOAD_CONFIG("scene", InitLevelCfg2);
	LOAD_CONFIG("pass_reward", InitRoleLevelRewardCfg);
	LOAD_CONFIG("other", InitOtherCfg);
	LOAD_CONFIG("reward_proportion", InitRewardProportionCfg);

	return true;
}

const MiYiLevelCfg * MiYiConfig::GetMiYiLevelCfgBySceneId(int scene_id)
{
	std::map<int, MiYiLevelCfg>::iterator iter = m_level_cfg.find(scene_id);
	
	if (iter != m_level_cfg.end())
	{
		return &iter->second;
	}

	return NULL;
}

const MiYiLevelCfg * MiYiConfig::GetMiYiLevelCfgBySeq(int seq)
{
	std::map<int, MiYiLevelCfg>::iterator iter = m_level_cfg_by_seq.find(seq);

	if (iter != m_level_cfg_by_seq.end())
	{
		return &iter->second;
	}

	return NULL;
}

const MiYiSimpleCfg * MiYiConfig::GetMiYiLevelSimpleCfg(int monster_group_id)
{
	std::map<int, MiYiSimpleCfg>::const_iterator it = m_level_cfg_by_monster.find(monster_group_id);
	if (it == m_level_cfg_by_monster.end())
	{
		return NULL;
	}
	return &it->second;
}

std::vector<ItemConfigData> MiYiConfig::GetRandRewardItem(std::vector<int> reward_id)
{
	std::vector<ItemConfigData> rewards;
	for (int i = 0; i < (int)reward_id.size(); ++i)
	{
		this->GetRandReward(reward_id[i], rewards);
	}

	return rewards;
}

std::vector<ItemConfigData> MiYiConfig::GetRewardItem(int level_reward_group_id, int role_level, int pass_count)
{
	return this->GetRandRewardItem(this->GetRoleLevelRewardId(level_reward_group_id, role_level, pass_count));
}

int MiYiConfig::GetRewardProportionType(int pass_count)
{
	for (int i = 0; i < MI_YI_MAX_REWARD_PROPORTION_NUM; i++)
	{
		if (pass_count < reward_proportion[i])
		{
			return i;
		}
	}
	return -1;
}

const std::map<int, MiYiLevelCfg> & MiYiConfig::GetAllLevelCfg() const
{
	return m_level_cfg;
}

int MiYiConfig::InitLevelCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	std::set<int> check_monster_repeat;
	std::set<int> check_monster_group_repeat;
	std::map<int, int> check_scene_to_seq_repeat;
	while (NULL != root_element)
	{
		int scene_id = 0;
		if (!GetSubNodeValue(root_element, "scene_id", scene_id))
		{
			return -490;
		}

		MiYiLevelCfg & cfg = m_level_cfg[scene_id];
		
		MiYiMonsterCfg monster_cfg;

		cfg.scene_id = scene_id;

		int seq = 0;
		if (!GetSubNodeValue(root_element, "seq", seq))
		{
			return -1;
		}
		cfg.seq = seq;

		std::map<int, int>::iterator iter = check_scene_to_seq_repeat.find(scene_id);
		if (iter != check_scene_to_seq_repeat.end() && iter->second != cfg.seq)
		{
			return -111;
		}
		else
		{
			check_scene_to_seq_repeat[scene_id] = cfg.seq;
		}

		if (cfg.seq < 0 || cfg.seq >= MI_YI_MAX_LEVEL)
		{
			return -112;
		}

		MiYiLevelCfg & seq_cfg = m_level_cfg_by_seq[seq];
		if (cfg.seq == -1)
		{
			cfg.seq = seq;
		}

		if (cfg.seq != seq)
		{
			return -9965;
		}

		if (!GetSubNodeValue(root_element, "npc_id", monster_cfg.npc_monster_id))
		{
			return -3;
		}

		if (check_monster_repeat.find(monster_cfg.npc_monster_id) != check_monster_repeat.end())
		{
			return -31;
		}
		check_monster_repeat.insert(monster_cfg.npc_monster_id);

		if (!GetSubNodeValue(root_element, "level", monster_cfg.monster_level))
		{
			return -33;
		}

		if (!GetSubNodeValue(root_element, "x", monster_cfg.npc_monster_pos.x))
		{
			return -4;
		}

		if (!GetSubNodeValue(root_element, "y", monster_cfg.npc_monster_pos.y))
		{
			return -5;
		}

		if (!GetSubNodeValue(root_element, "exp", monster_cfg.add_exp))
		{
			return -6;
		}

		if (!GetSubNodeValue(root_element, "coin", monster_cfg.add_coin))
		{
			return -7;
		}

		if (!GetSubNodeValue(root_element, "level_group_id", monster_cfg.role_level_reward_group_id))
		{
			return -8;
		}

		if (!GetSubNodeValue(root_element, "dir_y", monster_cfg.dir_y))
		{
			return -9;
		}
		const ServerNPCInfoCfg* npc_cfg = NPCPOOL->GetServerNPCInfo(monster_cfg.npc_monster_id);
		if (NULL == npc_cfg || SERVER_NPC_TYPE_MONSTER != npc_cfg->server_npc_type)
		{
			return -10;
		}

		int monster_group_id = npc_cfg->server_npc_param.monster_param.monster_group_id;
		if (NULL == MonsterGroupPool::GetInstance().GetMonsterGroupCfg(monster_group_id))
		{
			return -11;
		}
		if (check_monster_group_repeat.end() != check_monster_group_repeat.find(monster_group_id))
		{
			return -12;
		}

		TiXmlElement * first_reward_element = root_element->FirstChildElement("first_reward_id");
		if (NULL != first_reward_element)
		{
			TiXmlElement *node_element = first_reward_element->FirstChildElement("node");
			while (node_element)
			{
				int reward_id = 0;
				if (!GetNodeValue(node_element, reward_id))
				{
					return -299;
				}

				monster_cfg.first_pass_reward_id_vec.push_back(reward_id);
				node_element = node_element->NextSiblingElement();
			}
		}

		if (monster_cfg.first_pass_reward_id_vec.empty())
		{
			return -300;
		}

		TiXmlElement * weekly_reward_element = root_element->FirstChildElement("weekly_reward_id");
		if (NULL != weekly_reward_element)
		{
			TiXmlElement *node_element = weekly_reward_element->FirstChildElement("node");
			while (node_element)
			{
				int reward_id = 0;
				if (!GetNodeValue(node_element, reward_id))
				{
					return -399;
				}

				monster_cfg.weekly_reward_id_vec.push_back(reward_id);
				node_element = node_element->NextSiblingElement();
			}
		}

		TiXmlElement * item_list_element = root_element->FirstChildElement("weekly_reward_list");
		if (NULL != item_list_element)
		{
			TiXmlElement * item_element = item_list_element->FirstChildElement("weekly_reward");
			while (NULL != item_element)
			{
				ItemConfigData item;
				if (!item.ReadConfig(item_element))
				{
					return -__LINE__;
				}

				monster_cfg.week_first_kill_reward.push_back(item);
				item_element = item_element->NextSiblingElement();
			}
		}

		if (monster_cfg.weekly_reward_id_vec.empty())
		{
			return -400;
		}

		monster_cfg.monster_seq = (int)cfg.monsters.size();		//第几个怪物
		cfg.monsters.push_back(monster_cfg);
		m_level_cfg_by_monster[monster_group_id] = MiYiSimpleCfg(cfg.seq, monster_cfg.monster_seq);
		if ((int)cfg.monsters.size() > MI_YI_MAX_MONSTER_NUM)
		{
			return -500;
		}
		seq_cfg = cfg;
		root_element = root_element->NextSiblingElement();
	}

	if ((int)m_level_cfg.size() >= MI_YI_MAX_LEVEL)
	{
		return -111000;
	}

	if ((int)m_level_cfg_by_seq.size() >= MI_YI_MAX_LEVEL)
	{
		return -111000;
	}

	return 0;
}

int MiYiConfig::InitLevelCfg2(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	while (NULL != root_element)
	{
		int scene_id = 0;
		if (!GetSubNodeValue(root_element, "scene_id", scene_id))
		{
			return -490;
		}

		MiYiLevelCfg & cfg = m_level_cfg[scene_id];
		if (!GetSubNodeValue(root_element, "x", cfg.enter_pos.x) || cfg.enter_pos.x < 0)
		{
			return -491;
		}

		if (!GetSubNodeValue(root_element, "y", cfg.enter_pos.y) || cfg.enter_pos.y < 0)
		{
			return -492;
		}

		if (!GetSubNodeValue(root_element, "level", cfg.need_role_level) || cfg.need_role_level <= 0)
		{
			return -493;
		}

		if (!GetSubNodeValue(root_element, "pre_task", cfg.pre_task) || cfg.pre_task < 0)
		{
			return -494;
		}

		if (!GetSubNodeValue(root_element, "pass_score", cfg.sao_dang_capability))
		{
			return -495;
		}

		if (!GetSubNodeValue(root_element, "pass_level", cfg.sao_dang_need_role_level))
		{
			return -496;
		}

		if (!GetSubNodeValue(root_element, "can_use_pass", cfg.can_sao_dang))
		{
			return -497;
		}
	
		root_element = root_element->NextSiblingElement();
	}

	for (std::map<int, MiYiLevelCfg>::iterator iter = m_level_cfg.begin(); iter != m_level_cfg.end(); ++iter)
	{
		if (iter->second.enter_pos.x == -1 || iter->second.enter_pos.y == -1)
		{
			return -10086;
		}
	}


	return 0;
}

int MiYiConfig::InitRewardCfg(TiXmlElement * RootElement)
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

		MiYiRewardCfg & cfg = m_rewards_cfg[group_id];
		MiYiRewardCfg::ItemCfg item_cfg;

		if (!GetSubNodeValue(root_element, "rate", item_cfg.rate) || item_cfg.rate <= 0)
		{
			return -2;
		}

		TiXmlElement * pass_reward_element = root_element->FirstChildElement("reward_list");
		if (NULL != pass_reward_element)
		{
			TiXmlElement *node_element = pass_reward_element->FirstChildElement("reward");
			while (NULL != node_element)
			{
				ItemConfigData item;
				if (!item.ReadConfig(node_element))
				{
					return -104;
				}

				item_cfg.reward.push_back(item);
				node_element = node_element->NextSiblingElement();
			}
		}
		cfg.group_reward.push_back(item_cfg);
		root_element = root_element->NextSiblingElement();
	}

	if (m_rewards_cfg.size() == 0)
	{
		return 10085;
	}

	for (std::map<int, MiYiRewardCfg >::iterator iter = m_rewards_cfg.begin(); iter != m_rewards_cfg.end(); ++iter)
	{
		int count = 0;
		for (int i = 0; i < (int)iter->second.group_reward.size(); ++i)
		{
			count += iter->second.group_reward[i].rate;
		}

		if (count > MiYiRewardCfg::MAX_RATE_COUNT)
		{
			return -10086;
		}
	}

	return 0;
}

int MiYiConfig::InitRoleLevelRewardCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}


	while (NULL != root_element)
	{
		int level_group_id = 0;
		if (!GetSubNodeValue(root_element, "level_group_id", level_group_id))
		{
			return -1;
		}

		MiYiRoleLevelRewardCfg & cfg = m_role_level_reward_cfg[level_group_id];
		MiYiRoleLevelRewardCfg::ItemCfg item_cfg;

		if (!GetSubNodeValue(root_element, "min_level", item_cfg.min_role_level))
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "max_level", item_cfg.max_role_level) || item_cfg.max_role_level < item_cfg.min_role_level)
		{
			return -3;
		}

		if (cfg.group_reward.size() != 0)
		{
			if (item_cfg.min_role_level <= cfg.group_reward.back().max_role_level)
			{
				return -4;
			}
		}

		TiXmlElement * pass_reward_element = root_element->FirstChildElement("reward_group_id");
		if (NULL != pass_reward_element)
		{
			TiXmlElement *node_element = pass_reward_element->FirstChildElement("node");
			while (node_element)
			{
				int reward_id = 0;
				if (!GetNodeValue(node_element, reward_id))
				{
					return -399;
				}

				item_cfg.reward_group_id.push_back(reward_id);
				node_element = node_element->NextSiblingElement();
			}
		}

		TiXmlElement * pass_reward_1_element = root_element->FirstChildElement("reward_group_id_1");
		if (NULL != pass_reward_1_element)
		{
			TiXmlElement *node_element = pass_reward_1_element->FirstChildElement("node");
			while (node_element)
			{
				int reward_id = 0;
				if (!GetNodeValue(node_element, reward_id))
				{
					return -499;
				}

				item_cfg.reward_group_id_1.push_back(reward_id);
				node_element = node_element->NextSiblingElement();
			}
		}

		TiXmlElement * pass_reward_2_element = root_element->FirstChildElement("reward_group_id_2");
		if (NULL != pass_reward_2_element)
		{
			TiXmlElement *node_element = pass_reward_2_element->FirstChildElement("node");
			while (node_element)
			{
				int reward_id = 0;
				if (!GetNodeValue(node_element, reward_id))
				{
					return -599;
				}

				item_cfg.reward_group_id_2.push_back(reward_id);
				node_element = node_element->NextSiblingElement();
			}
		}

		cfg.group_reward.push_back(item_cfg);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int MiYiConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}
	if (!GetSubNodeValue(root_element, "monster_num", m_other_cfg.monster_num) || m_other_cfg.monster_num < 0)
	{
		return -1;
	}

	if (!GetSubNodeValue(root_element, "time", m_other_cfg.weekly_kill_times) || m_other_cfg.weekly_kill_times < 0)
	{
		return -2;
	}
	
	TiXmlElement * list_element = root_element->FirstChildElement("reward_list");
	if (list_element != NULL)
	{

		TiXmlElement * item_element = list_element->FirstChildElement("reward");
		if (item_element != NULL)
		{
			if (!m_other_cfg.reward.ReadConfig(item_element)) return -100;
		}
		
	}

	if (!GetSubNodeValue(root_element, "kill_time", m_other_cfg.get_xia_yi_zhi_times))
	{
		return -33;
	}
	

	return 0;
}

int MiYiConfig::InitRewardProportionCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}
	int index = 0;
	while (NULL != root_element)
	{
		int kill_time = 0;
		if (!GetSubNodeValue(root_element, "time", kill_time))
		{
			return -1;
		}
		if(index >= MI_YI_MAX_REWARD_PROPORTION_NUM) return -2;

		if (index > 0)
		{
			if(reward_proportion[index - 1] > kill_time) return -3;
		}
		 
		reward_proportion[index++] = kill_time;
		max_reward_proportion = max_reward_proportion > kill_time ? max_reward_proportion : kill_time;
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

void MiYiConfig::GetRandReward(int reward_id, std::vector<ItemConfigData>& out_list)
{
	std::map<int, MiYiRewardCfg > ::iterator iter = m_rewards_cfg.find(reward_id);
	if (iter == m_rewards_cfg.end())
	{
		return;
	}

	int r = rand() % MiYiRewardCfg::MAX_RATE_COUNT;

	for (int i = 0; i < (int)iter->second.group_reward.size(); ++i)
	{
		if (r < iter->second.group_reward[i].rate)
		{
			for (int j = 0; j < (int)iter->second.group_reward[i].reward.size(); j++)
			{
				out_list.push_back(iter->second.group_reward[i].reward[j]);
			}
			break;
		}

		r -= iter->second.group_reward[i].rate;
	}
}

std::vector<int> MiYiConfig::GetRoleLevelRewardId(int level_reward_group_id, int role_level, int pass_count)
{
	std::vector<int> ret;
	const MiYiRoleLevelRewardCfg & cfg = m_role_level_reward_cfg[level_reward_group_id];

	for (int i = 0; i < (int)cfg.group_reward.size(); ++i)
	{
		const MiYiRoleLevelRewardCfg::ItemCfg & item_cfg = cfg.group_reward[i];
		if (item_cfg.min_role_level <= role_level && role_level <= item_cfg.max_role_level)
		{
			int type = this->GetRewardProportionType(pass_count);
			if(REWARD_PROPORTION_TYPE_ALL == type)
			{
				ret = item_cfg.reward_group_id;
			}
			else if (REWARD_PROPORTION_TYPE_FIRST == type)
			{
				ret = item_cfg.reward_group_id_1;
			}
			else if (REWARD_PROPORTION_TYPE_SECOND == type)
			{
				ret = item_cfg.reward_group_id_2;
			}
			else
			{
				continue;
			}
			break;
		}
	}

	return ret;
}
