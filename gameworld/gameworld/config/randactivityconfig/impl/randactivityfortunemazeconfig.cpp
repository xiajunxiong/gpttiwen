#include "randactivityfortunemazeconfig.hpp"

RandActivityFortuneMazeConfig::RandActivityFortuneMazeConfig()
{
}

RandActivityFortuneMazeConfig::~RandActivityFortuneMazeConfig()
{
	std::vector<OtherSectionCfg>().swap(m_other_section_vec);
	std::vector<FortuneMazeTaskListCfg>().swap(m_fortune_maze_map);
	std::vector<FortuneMazeRewardGroupSectionCfg>().swap(m_reward_section_cfg);
}

bool RandActivityFortuneMazeConfig::Init(TiXmlElement * RootElement, std::string * err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("other", InitOtherCfg);
	LOAD_RA_CONFIG("task", InitFortuneMazeTaskListCfg);
	LOAD_RA_CONFIG("reward", InitRewardCfg);

	return true;
}

const OtherCfg * RandActivityFortuneMazeConfig::GetRandSceneId(RandActivityManager * ramgr) const
{
	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_FORTUNE_MAZE);
	for (std::vector<OtherSectionCfg>::const_iterator section_it = m_other_section_vec.begin(); m_other_section_vec.end() != section_it; ++section_it)
	{
		if (section_it->section_end >= act_real_open_day && act_real_open_day >= section_it->section_start)
		{
			if ((int)section_it->m_other_vec.size() <= 0)return NULL;

			int rand_idx = rand() % section_it->m_other_vec.size();

			return &section_it->m_other_vec[rand_idx];

			break;
		}
	}
	return NULL;
}

const OtherCfg * RandActivityFortuneMazeConfig::GetCfgBySceneId(int reward_group_id, RandActivityManager * ramgr) const
{
	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_FORTUNE_MAZE);
	for (std::vector<OtherSectionCfg>::const_iterator section_it = m_other_section_vec.begin(); m_other_section_vec.end() != section_it; ++section_it)
	{
		if (section_it->section_end >= act_real_open_day && act_real_open_day >= section_it->section_start)
		{
			for (std::vector<OtherCfg>::const_iterator it = section_it->m_other_vec.begin(); it != section_it->m_other_vec.end(); it++)
			{
				if (it->reward_group_id == reward_group_id)
				{
					return &(*it);
				}
			}
			break;
		}
	}
	return NULL;
}

const FortuneMazeTaskListCfg * RandActivityFortuneMazeConfig::GetFortuneMazeTaskListConfig(RandActivityManager * ramgr)const
{
	if (NULL == ramgr) return NULL;
	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_FORTUNE_MAZE);
	for (std::vector<FortuneMazeTaskListCfg>::const_iterator it = m_fortune_maze_map.begin(); it != m_fortune_maze_map.end(); it++)
	{
		if (it->section_start <= act_real_open_day && it->section_end >= act_real_open_day)
		{
			return &(*it);
		}
	}
	return NULL;
}

const FortuneMazeRewardGroupCfg * RandActivityFortuneMazeConfig::GetFortuneMazeRewardGroupConfig(int group_id, RandActivityManager * ramgr)const
{
	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_FORTUNE_MAZE);
	for (std::vector<FortuneMazeRewardGroupSectionCfg>::const_iterator section_it = m_reward_section_cfg.begin(); m_reward_section_cfg.end() != section_it; ++section_it)
	{
		if (section_it->section_end >= act_real_open_day && act_real_open_day >= section_it->section_start)
		{
			std::map<int, FortuneMazeRewardGroupCfg>::const_iterator it = section_it->m_reward_map.find(group_id);
			if (it != section_it->m_reward_map.end())
			{
				return &it->second;
			}
			break;
		}
	}
	
	return NULL;
}

int RandActivityFortuneMazeConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -999;
	}

	int last_section = 0;
	while (NULL != data_element)
	{
		int section_start = 0, section_end = 0;
		if (!GetSubNodeValue(data_element, "section_start", section_start) || section_start <= 0) return -999;
		if (!GetSubNodeValue(data_element, "section_end", section_end) || section_end < 0) return -998;
		if (0 == section_end)
		{
			section_end = MAX_INT;
		}

		if (section_end < section_start) return -997;

		if (section_start != last_section)
		{
			m_other_section_vec.push_back(OtherSectionCfg(section_start, section_end));
			last_section = section_start;
		}

		OtherCfg node_cfg;
		if (!GetSubNodeValue(data_element, "scene_id", node_cfg.scene_id) || node_cfg.scene_id <= 0) return -1;
		if (!GetSubNodeValue(data_element, "weight", node_cfg.weight)) return -2;
		if (!GetSubNodeValue(data_element, "npc_id", node_cfg.npc_id)) return -3;
		if (!GetSubNodeValue(data_element, "reward_group_id", node_cfg.reward_group_id)) return -4;
		if (!GetSubNodeValue(data_element, "x", node_cfg.box_pos.x)) return -5;
		if (!GetSubNodeValue(data_element, "y", node_cfg.box_pos.y)) return -6;

		m_other_section_vec.rbegin()->m_other_weight += node_cfg.weight;
		m_other_section_vec.rbegin()->m_other_vec.push_back(node_cfg);
		data_element = data_element->NextSiblingElement();
	}
	return 0;
}

int RandActivityFortuneMazeConfig::InitFortuneMazeTaskListCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -999;
	}

	int seq = 0;
	while (NULL != data_element)
	{
		bool has_section_cfg = true;
		int section_start = 0, section_end = 0;
		if (!GetSubNodeValue(data_element, "section_start", section_start))
		{
			has_section_cfg = false;
		}
		if (!GetSubNodeValue(data_element, "section_end", section_end))
		{
			has_section_cfg = false;
		}

		if (!has_section_cfg) // 没配置，默认给一个无限区间
		{
			section_start = 1;
			section_end = INT_MAX;
		}

		if (0 == section_end) // 0 代表无穷
		{
			section_end = INT_MAX;
		}
		
		if (m_fortune_maze_map.size() > 0)
		{
			int pre_index = m_fortune_maze_map.size() - 1;
			if (section_start != m_fortune_maze_map[pre_index].section_start ||
				section_end != m_fortune_maze_map[pre_index].section_end)
			{
				if (section_start < m_fortune_maze_map[pre_index].section_end)
				{
					return -1;
				}
				
				m_fortune_maze_map.push_back(FortuneMazeTaskListCfg());
			}
		}

		if (m_fortune_maze_map.size() == 0)
		{
			if (section_start != 1)
			{
				return -2;
			}
			m_fortune_maze_map.push_back(FortuneMazeTaskListCfg());
			seq = 0;
		}

		FortuneMazeTaskListCfg & node_cfg = m_fortune_maze_map[m_fortune_maze_map.size() - 1];
		node_cfg.section_start = section_start;
		node_cfg.section_end = section_end;

		FortuneMazeTaskCfg task_cfg;
		if (!GetSubNodeValue(data_element, "seq1", task_cfg.seq1) || task_cfg.seq1 < 0 || task_cfg.seq1 >= MAX_FORTUNE_MAZE_TASK_NUM) return -3;
		if (!GetSubNodeValue(data_element, "type", task_cfg.type) || task_cfg.type < FORTUNE_MAZE_TASK_TYPE_1 || task_cfg.type >= FORTUNE_MAZE_TASK_TYPE_MAX) return -4;
		if (!GetSubNodeValue(data_element, "parameter", task_cfg.parameter) || task_cfg.parameter <= 0) return -5;
		if (!GetSubNodeValue(data_element, "open_number", task_cfg.open_number) || task_cfg.open_number <= 0) return -6;
		if (!GetSubNodeValue(data_element, "seq", task_cfg.seq)) return -7;

		node_cfg.task_list.push_back(task_cfg);
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityFortuneMazeConfig::InitRewardCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}
	int last_section = 0;
	while (NULL != data_element)
	{
		int section_start = 0, section_end = 0;
		if (!GetSubNodeValue(data_element, "section_start", section_start) || section_start <= 0) return -999;
		if (!GetSubNodeValue(data_element, "section_end", section_end) || section_end < 0) return -998;
		if (0 == section_end)
		{
			section_end = MAX_INT;
		}

		if (section_end < section_start) return -997;

		if (section_start != last_section)
		{
			m_reward_section_cfg.push_back(FortuneMazeRewardGroupSectionCfg(section_start, section_end));
			last_section = section_start;
		}

		int reward_group_id = 0;
		if (!GetSubNodeValue(data_element, "reward_group_id", reward_group_id))
		{
			return -1;
		}
		FortuneMazeRewardGroupCfg &node_cfg = m_reward_section_cfg.rbegin()->m_reward_map[reward_group_id];
		node_cfg.reward_group_id = reward_group_id;
		if (!GetSubNodeValue(data_element, "hearsay", node_cfg.hearsay))
		{
			return -2;
		}

		TiXmlElement * reward_list_element = data_element->FirstChildElement("reward_item_list");
		if (NULL == reward_list_element)
		{
			return -100;
		}
		TiXmlElement * reward_element = reward_list_element->FirstChildElement("reward_item");
		while (reward_element != NULL)
		{
			ItemConfigData reward;
			if (!reward.ReadConfig(reward_element))
			{
				return -101;
			}
			node_cfg.reward_vec.push_back(reward);
			reward_element = reward_element->NextSiblingElement();
		}
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}
