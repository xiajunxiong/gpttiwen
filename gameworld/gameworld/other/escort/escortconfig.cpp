#include "escortconfig.hpp"
#include <algorithm>
#include "task/taskpool.h"

EscortConfig::EscortConfig()
{
}

EscortConfig::~EscortConfig()
{
	for (std::map<int, std::vector<EscortNpcCfg> >::iterator it = m_npc_group_cfg.begin(); it != m_npc_group_cfg.end(); it++)
	{
		std::vector<EscortNpcCfg>().swap(it->second);
	}
}

bool EscortConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG("other", InitOtherCfg);
	LOAD_CONFIG("monster_group", InitMonsterGroupCfg);
	LOAD_CONFIG("NPC_group", InitNpcGroupCfg);
	LOAD_CONFIG("yunbiao_config", InitEscortTaskCfg);

	return true;
}

const EscortTaskCfg * EscortConfig::GetTaskCfg(int seq)
{
	std::map<int, EscortTaskCfg>::const_iterator it = m_escort_task_cfg.find(seq);
	if (it == m_escort_task_cfg.end()) return NULL;
	return &it->second;
}

int EscortConfig::GetTaskSeq(int level, int task_type, int task_id)
{
	for (std::map<int, EscortTaskCfg>::const_iterator it = m_escort_task_cfg.begin(), end = m_escort_task_cfg.end(); it != end; ++it)
	{
		if (it->second.min_level <= level && level <= it->second.max_level && task_type == it->second.progress_desc && task_id == it->second.task_id)
		{
			return it->first;
		}
	}
	return 0;
}

static int temp_num = 0;
bool _RandNpc(EscortNpcCfg cfg)
{
	return temp_num < cfg.rate;
}

int EscortConfig::RandNpc(int seq)
{
	std::map<int, std::vector<EscortNpcCfg> >::const_iterator group_it = m_npc_group_cfg.find(seq);
	if (group_it == m_npc_group_cfg.end()) return 0;

	temp_num = RandomNum(m_npc_group_total_weight[seq]);
	std::vector<EscortNpcCfg>::const_iterator it = std::find_if(group_it->second.begin(), group_it->second.end(), _RandNpc);
	if (it != group_it->second.end()) return it->npc_id;

	return 0;
}

int EscortConfig::RandMonsterGroup(int seq)
{
	std::map<int, EscortTaskCfg>::const_iterator escort_it = m_escort_task_cfg.find(seq);
	if (escort_it == m_escort_task_cfg.end()) return 0;
	std::map<int, EscortMonsterGroupCfg>::const_iterator monster_it = m_monster_group_cfg.find(escort_it->second.random_monster);
	if (monster_it == m_monster_group_cfg.end()) return 0;

	int randnum = RandomNum(monster_it->second.total_weight);
	for (std::map<int, int>::const_iterator it = monster_it->second.monster_group.begin(), end = monster_it->second.monster_group.end(); it != end; ++it)
	{
		if (randnum < it->second)
		{
			return it->first;
		}
		randnum -= it->second;
	}

	return 0;
}

const EscortNpcCfg * EscortConfig::GetEscortNpcCfg(int npc_group, int npc_id)
{
	std::map<int, std::vector<EscortNpcCfg> >::iterator it = m_npc_group_cfg.find(npc_group);
	if (it == m_npc_group_cfg.end())
	{
		return NULL;
	}

	for (std::vector<EscortNpcCfg>::iterator itor = it->second.begin(); itor != it->second.end(); itor++)
	{
		if (itor->npc_id == npc_id)
		{
			return &(*itor);
		}
	}

	return NULL;
}

int EscortConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;
	while (NULL != root_element)
	{
		if (!GetSubNodeValue(root_element, "daily_time", m_other_cfg.daily_num)) return -1;
		if (!GetSubNodeValue(root_element, "normal_open_level", m_other_cfg.normal_open_level)) return -2;
		if (!GetSubNodeValue(root_element, "senior_open_level", m_other_cfg.senior_open_level)) return -3;
		if (!GetSubNodeValue(root_element, "accept_seq", m_other_cfg.accept_npc)) return -4;
		if (!GetSubNodeValue(root_element, "slow_down", m_other_cfg.slow_down)) return -5;
		if (!GetSubNodeValue(root_element, "offline_protect_time", m_other_cfg.offline_protect_time)) return -6;
		if (!GetSubNodeValue(root_element, "time_limit", m_other_cfg.time_limit)) return -7;

		root_element = root_element->NextSiblingElement();
	}
	return 0;
}

int EscortConfig::InitMonsterGroupCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;
	while (NULL != root_element)
	{
		int seq = 0;
		if (!GetSubNodeValue(root_element, "seq", seq)) return -1;
		int monster_group_id = 0;
		if (!GetSubNodeValue(root_element, "monster_group_id", monster_group_id)) return -2;
		int rate = 0;
		if (!GetSubNodeValue(root_element, "rate", rate)) return -3;

		m_monster_group_cfg[seq].monster_group[monster_group_id] = rate;
		m_monster_group_cfg[seq].total_weight += rate;
		root_element = root_element->NextSiblingElement();
	}
	return 0;
}

int EscortConfig::InitNpcGroupCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;
	while (NULL != root_element)
	{
		int seq = 0;
		if (!GetSubNodeValue(root_element, "seq", seq)) return -1;
		EscortNpcCfg npc_cfg;
		if (!GetSubNodeValue(root_element, "npc_seq", npc_cfg.npc_id)) return -2;
		int rate = 0;
		if (!GetSubNodeValue(root_element, "rate", rate)) return -3;

		if (!GetSubNodeValue(root_element, "village_id", npc_cfg.village_id)) return -4;

		if (!GetSubNodeValue(root_element, "reward_prestige", npc_cfg.reward_prestige)) return -5;

		m_npc_group_total_weight[seq] += rate;
		npc_cfg.rate = m_npc_group_total_weight[seq];

		m_npc_group_cfg[seq].push_back(npc_cfg);
		root_element = root_element->NextSiblingElement();
	}
	return 0;
}

int EscortConfig::InitEscortTaskCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;
	while (NULL != root_element)
	{
		EscortTaskCfg node_cfg;
		if (!GetSubNodeValue(root_element, "seq", node_cfg.seq)) return -1;
		if (!GetSubNodeValue(root_element, "min_level", node_cfg.min_level)) return -2;
		if (!GetSubNodeValue(root_element, "max_level", node_cfg.max_level)) return -3;
		if (!GetSubNodeValue(root_element, "deposit", node_cfg.deposit)) return -4;
		if (!GetSubNodeValue(root_element, "reward_coin", node_cfg.reward_coin)) return -5;
		if (!GetSubNodeValue(root_element, "random_monster", node_cfg.random_monster)) return -7;
		if (!GetSubNodeValue(root_element, "NPC_group", node_cfg.NPC_group)) return -8;
		if (!GetSubNodeValue(root_element, "progress_desc", node_cfg.progress_desc)) return -9;
		if (!GetSubNodeValue(root_element, "task_id", node_cfg.task_id)) return -10;

		switch (node_cfg.progress_desc)
		{
		case ESCORT_TYPE_NORMAL:
		case ESCORT_TYPE_SENIOR:
			{
				if (0 != node_cfg.task_id) return -11;
			}
			break;
		case ESCORT_TYPE_CHALLENGE_TASK:
			{
				const TaskInfomaConfig * task_cfg = TASKPOOL->GetTaskCfgByID(node_cfg.task_id);
				if (NULL == task_cfg) return -12;
			}
			break;
		case ESCORT_TYPE_OTHER_TASK:
			{
				const TaskInfomaConfig * task_cfg = TASKPOOL->GetTaskCfgByID(node_cfg.task_id);
				if (NULL == task_cfg) return -13;
			}
			break;
		default:
			return -14;
		}

		node_cfg.reward_count = 0;
		TiXmlElement *reward_list_element = root_element->FirstChildElement("reward_list_list");
		if (NULL != reward_list_element)
		{
			TiXmlElement *reward_element = reward_list_element->FirstChildElement("reward_list");
			while (NULL != reward_element)
			{
				if (!node_cfg.reward_list[node_cfg.reward_count].ReadConfig(reward_element))
				{
					return -99;
				}

				if (++node_cfg.reward_count >= ESCORT_REWARD_MAX_NUM)return -98;
				reward_element = reward_element->NextSiblingElement();
			}
		}

		m_escort_task_cfg[node_cfg.seq] = node_cfg;

		root_element = root_element->NextSiblingElement();
	}
	return 0;
}
