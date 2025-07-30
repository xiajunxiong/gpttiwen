#include "petgodfbconfig.hpp"
#include "checkresourcecenter.hpp"
#include "npc/npcpool.h"

PetGodFbConfig::PetGodFbConfig()
{
}

PetGodFbConfig::~PetGodFbConfig()
{
	std::vector<long long>().swap(m_exp_reward_list);
}

bool PetGodFbConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;
	LOAD_CONFIG("other", InitOtherCfg);
	LOAD_CONFIG("reward_group", InitRewardGroupCfg);
	LOAD_CONFIG("crystal_config", InitFBCfg);
	LOAD_CONFIG("buy_power", InitBuyTimesCfg);
	LOAD_CONFIG("exp", InitEXPRewardCfg);

	return true;
}

const PetGodFbConfig::FBCfg * PetGodFbConfig::GetFBCfg(int fb_seq) const
{
	std::map<int, FBCfg>::const_iterator fb_it = m_fb_cfg_map.find(fb_seq);
	if (m_fb_cfg_map.end() == fb_it)
	{
		return NULL;
	}

	return &fb_it->second;
}

bool PetGodFbConfig::PetGodFbConfig::GetFBRewardData(int fb_seq, int role_level, std::vector<ItemConfigData> &out_item_list) const
{
	std::map<int, FBCfg>::const_iterator fb_it = m_fb_cfg_map.find(fb_seq);
	if (m_fb_cfg_map.end() == fb_it)
	{
		return false;
	}

	int group_num = fb_it->second.reward_group_list.size();
	for (int i = 0; i < group_num; i++)
	{
		const int & group_id = fb_it->second.reward_group_list[i];

		const RewardItemInfo* item_info = this->GetGroupReward(group_id, role_level);
		if (NULL == item_info)
		{
			continue;
		}

		out_item_list.push_back(item_info->reward_item);
	}

	return true;
}

const PetGodFbConfig::BuyTimeCfg * PetGodFbConfig::GetBuyTimeCfg(int buy_times) const
{
	BuyTimeCfgMap::const_iterator cfg_it = m_buy_times_map.find(buy_times);
	if (m_buy_times_map.end() == cfg_it)
	{
		return NULL;
	}
	return &cfg_it->second;
}
long long PetGodFbConfig::PetGodFbConfig::GetRewardExp(int role_level) const
{
	if (0 >= role_level
		|| (int)m_exp_reward_list.size() <= role_level)
	{
		return 0;
	}

	return m_exp_reward_list[role_level];
}

int PetGodFbConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -10000;
	}

	if (!GetSubNodeValue(root_element, "enter_num", m_other_cfg.day_base_times)
		|| 0 > m_other_cfg.day_base_times)
	{
		return -1;
	}

	return 0;
}

int PetGodFbConfig::InitRewardGroupCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -10000;
	}

	while (NULL != root_element)
	{
		RewardItemInfo cfg_info;

		if (!GetSubNodeValue(root_element, "reward_id", cfg_info.group_id)
			|| 0 > cfg_info.group_id)
		{
			return -1;
		}
		if (!GetSubNodeValue(root_element, "rate", cfg_info.power)
			|| 0 >= cfg_info.power)
		{
			return -2;
		}
		if (!GetSubNodeValue(root_element, "levels_min", cfg_info.role_min_level)
			|| 0 > cfg_info.role_min_level)
		{
			return -3;
		}
		if (!GetSubNodeValue(root_element, "level_max", cfg_info.role_max_level)
			|| 0 > cfg_info.role_max_level)
		{
			return -4;
		}

		TiXmlElement * reward_element = root_element->FirstChildElement("reward_item");
		if (!cfg_info.reward_item.ReadConfig(reward_element))
		{
			return -101;
		}

		RewardPool& pool_info = m_reward_group_id_map[cfg_info.group_id][cfg_info.role_min_level];
		pool_info.total_power += cfg_info.power;
		pool_info.power_pool[pool_info.total_power] = cfg_info;

		root_element = root_element->NextSiblingElement();
	}
	for (RewardGroupIDMap::iterator group_it = m_reward_group_id_map.begin();
		m_reward_group_id_map.end() != group_it;
		group_it++)
	{
		for (RewardPoolLevelMap::iterator level_it = group_it->second.begin();
			group_it->second.end()!= level_it;
			level_it++)
		{
			if (PET_GOD_REWARD_GROUP_BASE_RATE != level_it->second.total_power)
			{
				return -100000 - group_it->first * 1000 - level_it->first;
			}
		}
	}

	return 0;
}

int PetGodFbConfig::InitBuyTimesCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -10000;
	}

	while (NULL != root_element)
	{
		int buy_times = 0;
		if (!GetSubNodeValue(root_element, "buy_times", buy_times)
			|| 0 >= buy_times
			|| buy_times != (int)m_buy_times_map.size() + 1)
		{
			return -1;
		}

		BuyTimeCfg & cfg_info = m_buy_times_map[buy_times];
		cfg_info.buy_times = buy_times;

		if (!GetSubNodeValue(root_element, "price", cfg_info.cost_gold)
			|| 0 > cfg_info.cost_gold)
		{
			return -2;
		}

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int PetGodFbConfig::InitEXPRewardCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -10000;
	}
	m_exp_reward_list.clear();
	m_exp_reward_list.push_back(0);

	while (NULL != root_element)
	{
		int level = 0;
		if (!GetSubNodeValue(root_element, "level", level)
			|| level != (int)m_exp_reward_list.size())
		{
			return -1;
		}

		long long exp = 0;
		if (!GetSubNodeValue(root_element, "exp", exp)
			|| 0 > exp)
		{
			return -2;
		}
		m_exp_reward_list.push_back(exp);

		root_element = root_element->NextSiblingElement();
	}
	return 0;
}

int PetGodFbConfig::InitFBCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -10000;
	}

	while (NULL != root_element)
	{
		int fb_seq = 0;
		if (!GetSubNodeValue(root_element, "seq", fb_seq)
			|| fb_seq - 1 != (int)m_fb_cfg_map.size())
		{
			return -1;
		}

		FBCfg & fb_info = m_fb_cfg_map[fb_seq];
		fb_info.fb_seq = fb_seq;

		if (!GetSubNodeValue(root_element, "scene_id", fb_info.scene_id))
		{
			return -2;
		}
		CheckResourceCenter::Instance().GetSceneCheck()->Add(fb_info.scene_id, __FUNCTION__);

		if (!GetSubNodeValue(root_element, "min_level", fb_info.role_level_limit))
		{
			return -3;
		}

		if (!GetSubNodeValue(root_element, "power_expend", fb_info.cost_power)
			|| 0 >= fb_info.cost_power)
		{
			return -4;
		}

		TiXmlElement * reward_group_list = root_element->FirstChildElement("reward_group_1");
		if (NULL == reward_group_list)
		{
			return -500;
		}
		TiXmlElement * node_element = reward_group_list->FirstChildElement("node");
		while (node_element != NULL)
		{
			int reward_group_id = 0;
			if (!GetNodeValue(node_element, reward_group_id)
				|| m_reward_group_id_map.end() == m_reward_group_id_map.find(reward_group_id))
			{
				return -501;
			}

			fb_info.reward_group_list.push_back(reward_group_id);
			node_element = node_element->NextSiblingElement();
		}
		if (PET_GOD_FB_DRAW_NUM != (int)fb_info.reward_group_list.size())
		{
			return -502;
		}
		
		if (!GetSubNodeValue(root_element, "reward", fb_info.draw_num)
			|| 0 >= fb_info.draw_num
			|| PET_GOD_FB_DRAW_NUM < fb_info.draw_num)
		{
			return -503;
		}

		if (fb_info.reward_group_list.empty())
		{
			return -5;
		}

		if (!GetSubNodeValue(root_element, "born_x", fb_info.role_pos.x))
		{
			return -6;
		}

		if (!GetSubNodeValue(root_element, "born_y", fb_info.role_pos.y))
		{
			return -7;
		}

		if (!GetSubNodeValue(root_element, "x", fb_info.boss_pos.x))
		{
			return -8;
		}

		if (!GetSubNodeValue(root_element, "y", fb_info.boss_pos.y))
		{
			return -9;
		}

		if (!GetSubNodeValue(root_element, "dir_y", fb_info.boss_dir))
		{
			return -10;
		}

		if (!GetSubNodeValue(root_element, "recover_npc_id", fb_info.recover_npc_id))
		{
			return -11;
		}
		const ServerNPCInfoCfg* npc_cfg = NPCPOOL->GetServerNPCInfo(fb_info.recover_npc_id);
		if (NULL == npc_cfg)
		{
			return -12;
		}
		if (!GetSubNodeValue(root_element, "recover_npc_x", fb_info.recover_npc_pos.x))
		{
			return -13;
		}
		if (!GetSubNodeValue(root_element, "recover_npc_y", fb_info.recover_npc_pos.y))
		{
			return -14;
		}

		if (!GetSubNodeValue(root_element, "recover_npc_dir", fb_info.recover_npc_dir))
		{
			return -15;
		}

		if (!GetSubNodeValue(root_element, "fb_type", fb_info.fb_type))
		{
			return -16;
		}

		if (!GetSubNodeValue(root_element, "level", fb_info.fb_level))
		{
			return -17;
		}

		if (!GetSubNodeValue(root_element, "monster_att_group", fb_info.boss_npc_id))
		{
			return -18;
		}
		const ServerNPCInfoCfg* boss_npc_cfg = NPCPOOL->GetServerNPCInfo(fb_info.boss_npc_id);
		if (NULL == boss_npc_cfg
			|| SERVER_NPC_TYPE_MONSTER != boss_npc_cfg->server_npc_type)
		{
			return -19;
		}

		if (!GetSubNodeValue(root_element, "saodang_zhanli", fb_info.auto_limit_cap))
		{
			return -20;
		}

		root_element = root_element->NextSiblingElement();
	}
	return 0;
}

const PetGodFbConfig::RewardItemInfo * PetGodFbConfig::GetGroupReward(int group_id, int role_level) const
{
	RewardGroupIDMap::const_iterator level_map_it = m_reward_group_id_map.find(group_id);
	if (level_map_it == m_reward_group_id_map.end())
	{
		return NULL;
	}

	if (level_map_it->second.empty())
	{
		return NULL;
	}

	RewardPoolLevelMap::const_iterator reward_pool_it = level_map_it->second.upper_bound(role_level);
	if (level_map_it->second.end() == reward_pool_it)
	{
		reward_pool_it--;
	}

	//会存在没有奖励的情况  基数一定  有效权重变动
	int rand_num = RandomNum(PET_GOD_REWARD_GROUP_BASE_RATE);

	RewardPowerMap::const_iterator reward_cit = reward_pool_it->second.power_pool.upper_bound(rand_num);
	if (reward_cit == reward_pool_it->second.power_pool.end())
	{
		return NULL;
	}

	return &reward_cit->second;
}
