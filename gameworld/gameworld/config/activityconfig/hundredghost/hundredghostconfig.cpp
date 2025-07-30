#include "hundredghostconfig.hpp"
#include "npc/npcpool.h"

HundredGhostConfig::HundredGhostConfig() : m_monstar_boss_num(0), m_monstar_big_boss_num(0)
{
}

HundredGhostConfig::~HundredGhostConfig()
{
	std::vector<HundredGhostMonsterCfg>().swap(m_monster_cfg);
	for (std::map<int, std::vector<HundredGhostRewardCfg> >::iterator it = m_reward_cfg.begin(); it != m_reward_cfg.end(); it++)
	{
		std::vector<HundredGhostRewardCfg>().swap(it->second);
	}
	std::vector<HundredGhostPosiCfg>().swap(m_posi_cfg);
}

bool HundredGhostConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG("other", InitOtherCfg);
	LOAD_CONFIG("monster_coordinate", InitMonsterCfg);
	LOAD_CONFIG("reward_group", InitRewardCfg);
	LOAD_CONFIG("coordinate", InitPosiCfg);

	return true;
}

const HundredGhostMonsterCfg * HundredGhostConfig::GetMonsterCfg(int npc_id) const
{
	for (std::vector<HundredGhostMonsterCfg>::const_iterator it = m_monster_cfg.begin(); it != m_monster_cfg.end(); ++it)
	{
		if (npc_id == it->npc_id)
		{
			return &(*it);
		}
	}

	return NULL;
}

const void HundredGhostConfig::GetMonsterListCfg(int num, std::vector<HundredGhostMonsterCfg>& monster_list) const
{
	if (0 >= num) return;

	//活动开始时 普通鬼王全部生成
	std::vector<HundredGhostMonsterCfg> temp_vec;
	for (std::vector<HundredGhostMonsterCfg>::const_iterator it = m_monster_cfg.begin(); it != m_monster_cfg.end(); ++it)
	{
		if (HUNDRED_GHOST_MONSTER_TYPE_1 == it->monster_type)
		{
			temp_vec.push_back(*it);
		}
	}

	//随机普通鬼怪
	std::vector<HundredGhostMonsterCfg> common_vec;
	for (std::vector<HundredGhostMonsterCfg>::const_iterator it = m_monster_cfg.begin(); it != m_monster_cfg.end(); ++it)
	{
		if (HUNDRED_GHOST_MONSTER_TYPE_0 == it->monster_type)
		{
			common_vec.push_back(*it);
		}
	}

	std::random_shuffle(common_vec.begin(), common_vec.end());
	temp_vec.insert(temp_vec.end(), common_vec.begin(), common_vec.end());

	int count = 0;
	for (std::vector<HundredGhostMonsterCfg>::const_iterator it = temp_vec.begin(); it != temp_vec.end() && count < num; ++it)
	{
		monster_list.push_back((*it));
		count++;
	}
}

const void HundredGhostConfig::GetOneMonsterCfg(HundredGhostMonsterCfg & monster, std::vector<HundredGhostNpcInfo> info_list) const
{
	//随机普通鬼怪
	std::vector<HundredGhostMonsterCfg> temp_vec;
	for (std::vector<HundredGhostMonsterCfg>::const_iterator it = m_monster_cfg.begin(); it != m_monster_cfg.end(); ++it)
	{
		if (HUNDRED_GHOST_MONSTER_TYPE_0 == it->monster_type)
		{
			temp_vec.push_back(*it);
		}
	}

	std::random_shuffle(temp_vec.begin(), temp_vec.end());

	for (std::vector<HundredGhostMonsterCfg>::const_iterator it = temp_vec.begin(); it != temp_vec.end(); ++it)
	{
		bool is_find = false;
		for (std::vector<HundredGhostNpcInfo>::const_iterator iter = info_list.begin(); iter != info_list.end(); ++iter)
		{
			if (it->npc_id == iter->npc_id)
			{
				is_find = true;
				break;
			}
		}

		if (!is_find)
		{
			monster = *it;
			return;
		}
	}
}

const HundredGhostMonsterCfg * HundredGhostConfig::GetMonsterBossCfg(int death_num) const
{
	if (0 >= death_num)
	{
		return NULL;
	}

	for (std::vector<HundredGhostMonsterCfg>::const_iterator it = m_monster_cfg.begin(); it != m_monster_cfg.end(); ++it)
	{
		if (HUNDRED_GHOST_MONSTER_TYPE_2 == it->monster_type && it->param_1 == death_num)
		{
			return &(*it);
		}
	}


	return NULL;
}

const HundredGhostMonsterCfg * HundredGhostConfig::GetMonsterBigBossCfg(int death_num, std::vector<int> npc_id_list) const
{
	if (m_other_cfg.max_num - m_monstar_big_boss_num > death_num)
	{
		return NULL;
	}

	static std::vector<HundredGhostMonsterCfg> temp_vec;
	temp_vec.clear();

	for (std::vector<HundredGhostMonsterCfg>::const_iterator it = m_monster_cfg.begin(); it != m_monster_cfg.end(); ++it)
	{
		if (HUNDRED_GHOST_MONSTER_TYPE_3 == it->monster_type)
		{
			temp_vec.push_back(*it);
		}
	}

	if (0 >= temp_vec.size())
	{
		return NULL;
	}

	std::random_shuffle(temp_vec.begin(), temp_vec.end());

	for (std::vector<HundredGhostMonsterCfg>::const_iterator it = temp_vec.begin(); it != temp_vec.end(); ++it)
	{
		bool is_find = false;
		for (std::vector<int>::const_iterator iter = npc_id_list.begin(); iter != npc_id_list.end(); ++iter)
		{
			if (it->npc_id == *iter)
			{
				is_find = true;
				break;
			}
		}

		if (!is_find)
		{
			return &(*it);
		}
	}

	return NULL;
}

const void HundredGhostConfig::GetRewradCfg(int npc_id, int level, int* coin, std::vector<ItemConfigData>& out_reward)
{
	const HundredGhostMonsterCfg* cfg = this->GetMonsterCfg(npc_id);
	if (NULL == cfg)
	{
		return;
	}

	for (int i = 0; i < (int)cfg->reward_id_vec.size(); ++i)
	{
		this->GetRandReward(cfg->reward_id_vec[i], level, coin, out_reward);
	}
}

const void HundredGhostConfig::GetRandReward(int group_id, int level, int* coin, std::vector<ItemConfigData>& out_reward)
{
	std::map<int, std::vector<HundredGhostRewardCfg> >::const_iterator iter = m_reward_cfg.find(group_id);
	if (m_reward_cfg.end() == iter)
	{
		return;
	}

	int rand_weight = rand() % 10000;
	for (std::vector<HundredGhostRewardCfg>::const_iterator it = iter->second.begin(); it != iter->second.end(); ++it)
	{
		if (it->min_level <= level && level <= it->max_level)
		{
			rand_weight -= it->rate;
			if (0 > rand_weight)
			{
				*coin += it->coin;
				out_reward.push_back(it->item);
				break;
			}
		}
	}
}

const bool HundredGhostConfig::IsItemNotice(int item_id)
{
	for (std::vector<int>::const_iterator it = m_other_cfg.notice_id_vec.begin(); it != m_other_cfg.notice_id_vec.end(); ++it)
	{
		if (item_id == *it)
		{
			return true;
		}
	}
	return false;
}

const void HundredGhostConfig::GetMonsterPosiListCfg(int num, std::vector<Posi>& posi_list) const
{
	if (0 >= num) return;

	std::vector<HundredGhostPosiCfg> temp_vec;
	temp_vec.assign(m_posi_cfg.begin(), m_posi_cfg.end());
	std::random_shuffle(temp_vec.begin(), temp_vec.end());

	int count = 0;
	for (std::vector<HundredGhostPosiCfg>::const_iterator it = temp_vec.begin(); it != temp_vec.end() && count < num; ++it)
	{
		posi_list.push_back(it->pos);
		count++;
	}
}

const void HundredGhostConfig::GetOneMonsterPosiCfg(Posi & posi, std::vector<HundredGhostNpcInfo> info_list) const
{
	std::vector<HundredGhostPosiCfg> temp_vec;
	temp_vec.assign(m_posi_cfg.begin(), m_posi_cfg.end());
	std::random_shuffle(temp_vec.begin(), temp_vec.end());

	for (std::vector<HundredGhostPosiCfg>::const_iterator it = temp_vec.begin(); it != temp_vec.end(); ++it)
	{
		bool is_find = false;
		for (std::vector<HundredGhostNpcInfo>::const_iterator iter = info_list.begin(); iter != info_list.end(); ++iter)
		{
			if (it->pos == iter->npc_pos)
			{
				is_find = true;
				break;
			}
		}

		if (!is_find)
		{
			posi = it->pos;
			return;
		}
	}
}

int HundredGhostConfig::InitMonsterCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	m_monstar_boss_num = 0;
	m_monstar_big_boss_num = 0;
	while (NULL != root_element)
	{
		HundredGhostMonsterCfg cfg;

		if (!GetSubNodeValue(root_element, "seq", cfg.seq) || cfg.seq != (int)m_monster_cfg.size())
		{
			return -1;
		}

		if (!GetSubNodeValue(root_element, "scene_id", cfg.scene_id) || 0 > cfg.scene_id)
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "npc_id", cfg.npc_id) || 0 >= cfg.npc_id)
		{
			return -3;
		}

		if (!GetSubNodeValue(root_element, "monster_group", cfg.monster_group_id) || 0 >= cfg.monster_group_id)
		{
			return -4;
		}

		if (!GetSubNodeValue(root_element, "monster_type", cfg.monster_type) || HUNDRED_GHOST_MONSTER_TYPE_0 > cfg.monster_type || cfg.monster_type > HUNDRED_GHOST_MONSTER_TYPE_3)
		{
			return -5;
		}

		if (!GetSubNodeValue(root_element, "param_1", cfg.param_1) || 0 > cfg.param_1)
		{
			return -6;
		}
		if (HUNDRED_GHOST_MONSTER_TYPE_2 == cfg.monster_type && 0 >= cfg.param_1)
		{
			return -99;
		}

		if (HUNDRED_GHOST_MONSTER_TYPE_2 == cfg.monster_type)
		{
			m_monstar_boss_num++;
		}
		else if (HUNDRED_GHOST_MONSTER_TYPE_3 == cfg.monster_type)
		{
			m_monstar_big_boss_num++;
		}

		TiXmlElement * reward_list = root_element->FirstChildElement("flop_reward");
		if (NULL != reward_list)
		{
			TiXmlElement *node_element = reward_list->FirstChildElement("node");
			while (node_element)
			{
				int reward_id = 0;
				if (!GetNodeValue(node_element, reward_id))
				{
					return -100;
				}

				cfg.reward_id_vec.push_back(reward_id);
				node_element = node_element->NextSiblingElement();
			}
		}

		m_monster_cfg.push_back(cfg);
		root_element = root_element->NextSiblingElement();
	}

	if ((int)m_monster_cfg.size() < m_other_cfg.scene_num)
	{
		return -888;
	}
	if (m_monstar_big_boss_num >= m_other_cfg.scene_num)
	{
		return -777;
	}

	return 0;
}

int HundredGhostConfig::InitRewardCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	while (NULL != root_element)
	{
		int group_id = 0;
		if (!GetSubNodeValue(root_element, "group_id", group_id) || 0 > group_id)
		{
			return -1;
		}

		std::vector<HundredGhostRewardCfg>& node = m_reward_cfg[group_id];
		HundredGhostRewardCfg cfg;

		if (!GetSubNodeValue(root_element, "min_level", cfg.min_level) || 0 >= cfg.min_level)
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "max_level", cfg.max_level) || cfg.min_level > cfg.max_level)
		{
			return -3;
		}

		if (!GetSubNodeValue(root_element, "coin", cfg.coin) || 0 > cfg.coin)
		{
			return -4;
		}

		if (!GetSubNodeValue(root_element, "rate", cfg.rate) || 0 > cfg.rate || cfg.rate > 10000)
		{
			return -5;
		}

		TiXmlElement * item_element = root_element->FirstChildElement("reward");
		if (NULL != item_element)
		{
			if (!cfg.item.ReadConfig(item_element)) return -100;
		}

		node.push_back(cfg);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int HundredGhostConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	while (NULL != root_element)
	{
		HundredGhostOtherCfg& cfg = m_other_cfg;

		if (!GetSubNodeValue(root_element, "max_monster_num", cfg.max_num) || 0 > cfg.max_num)
		{
			return -1;
		}

		if (!GetSubNodeValue(root_element, "scene_monster_num", cfg.scene_num) || cfg.scene_num > cfg.max_num)
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "requirement_players_num", cfg.team_num) || 0 >= cfg.team_num)
		{
			return -3;
		}

		if (!GetSubNodeValue(root_element, "requirement_level", cfg.fight_min_level) || 0 >= cfg.fight_min_level)
		{
			return -4;
		}

		TiXmlElement * notice_list = root_element->FirstChildElement("notice");
		if (NULL != notice_list)
		{
			TiXmlElement *node_element = notice_list->FirstChildElement("node");
			while (node_element)
			{
				int notice_id = 0;
				if (!GetNodeValue(node_element, notice_id))
				{
					return -100;
				}

				cfg.notice_id_vec.push_back(notice_id);
				node_element = node_element->NextSiblingElement();
			}
		}

		if (!GetSubNodeValue(root_element, "max_rounds", cfg.max_rounds) || 0 >= cfg.max_rounds)
		{
			return -5;
		}

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int HundredGhostConfig::InitPosiCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	while (NULL != root_element)
	{
		HundredGhostPosiCfg cfg;

		if (!GetSubNodeValue(root_element, "seq", cfg.seq) || 0 > cfg.seq || cfg.seq != (int)m_posi_cfg.size())
		{
			return -1;
		}

		if (!GetSubNodeValue(root_element, "x", cfg.pos.x) || 0 > cfg.pos.x)
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "y", cfg.pos.y) || 0 > cfg.pos.y)
		{
			return -3;
		}

		m_posi_cfg.push_back(cfg);
		root_element = root_element->NextSiblingElement();
	}

	if ((int)m_posi_cfg.size() < m_other_cfg.scene_num + m_monstar_boss_num)
	{
		return -99;
	}

	return 0;
}
