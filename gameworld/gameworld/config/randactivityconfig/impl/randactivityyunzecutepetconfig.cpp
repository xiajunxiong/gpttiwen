#include "randactivityyunzecutepetconfig.hpp"

RandActivityYunZeCutePetConfig::RandActivityYunZeCutePetConfig()
{

}


RandActivityYunZeCutePetConfig::~RandActivityYunZeCutePetConfig()
{
	std::vector<RAYZCutePetTaskSectionCfg>().swap(m_task_cfg);
	std::vector<RAYZCutePetRewardSectionCfg>().swap(m_reward_section_cfg);
}

bool RandActivityYunZeCutePetConfig::Init(TiXmlElement *RootElement, std::string *err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("basics", InitBasicsCfg);
	LOAD_RA_CONFIG("task", InitTaskCfg);
	LOAD_RA_CONFIG("reward", InitRewardCfg);

	return true;
}

const RAYZCutePetBasicsCfg * RandActivityYunZeCutePetConfig::GetMonsterByIdCfg(int monster_id)const
{
	if (monster_id < 0) return NULL;

	std::map<int, RAYZCutePetBasicsCfg>::const_iterator it = m_basics_cfg.find(monster_id);
	if (it == m_basics_cfg.end()) return NULL;

	return &it->second;
}

int RandActivityYunZeCutePetConfig::GetTaskCfg(RandActivityManager* ramgr, ARG_OUT std::vector<RAYZCutePetTaskCfg> & task_out)const
{
	if (NULL == ramgr) return -1;

	int act_open_day = this->GetActOpenDay(ramgr, RAND_ACTIVITY_TYPE_YUN_ZE_CUTE_PET);

	bool is_success_get = false;
	for (int i = 0; i < (int)m_task_cfg.size(); ++i)
	{
		if (act_open_day == m_task_cfg[i].section_day)
		{
			task_out = m_task_cfg[i].task;
			is_success_get = true;
		}
	}

	if (!is_success_get) return -2;

	return 0;
}

const RAYZCutePetRewardCfg * RandActivityYunZeCutePetConfig::GetRewardCfg(RandActivityManager* ramgr,int reward_group) const
{
	if (NULL == ramgr || reward_group < 0) return NULL;

	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_YUN_ZE_CUTE_PET);

	for (int i = 0; i < (int)m_reward_section_cfg.size(); ++i)
	{
		if (act_real_open_day >= m_reward_section_cfg[i].section_start && act_real_open_day <= m_reward_section_cfg[i].section_end)
		{
			for (int j = 0; j < (int)m_reward_section_cfg[i].reward_list.size(); ++j)
			{
				if (m_reward_section_cfg[i].reward_list[j].reward_group == reward_group)
				{
					return &m_reward_section_cfg[i].reward_list[j];
				}
			}
		}
	}

	return NULL;
}

int RandActivityYunZeCutePetConfig::InitBasicsCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -544;
	}

	while (NULL != data_element)
	{
		RAYZCutePetBasicsCfg basics_cfg;

		int monster_id = 0;
		if (!GetSubNodeValue(data_element, "monster", monster_id) || monster_id < 0) return -1;
		if (!GetSubNodeValue(data_element, "combat", basics_cfg.combat) || basics_cfg.combat < 0) return -2;
		if (!GetSubNodeValue(data_element, "reward", basics_cfg.reward_group) || basics_cfg.reward_group < 0) return -3;

		m_basics_cfg[monster_id] = basics_cfg;

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityYunZeCutePetConfig::InitTaskCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -544;
	}

	while (NULL != data_element)
	{
		int section_day = 0;
		if (!GetSubNodeValue(data_element, "section", section_day) || section_day < 0)
		{
			return -1;
		}

		if (m_task_cfg.size() == 0)
		{
			if (section_day != 1)
			{
				return -2;
			}
			m_task_cfg.push_back(RAYZCutePetTaskSectionCfg());
		}
		else
		{
			if (section_day != m_task_cfg[m_task_cfg.size() - 1].section_day)
			{
				if (section_day < m_task_cfg[m_task_cfg.size() - 1].section_day)
				{
					return -3;
				}
				m_task_cfg.push_back(RAYZCutePetTaskSectionCfg());
			}
		}

		RAYZCutePetTaskSectionCfg & node_cfg = m_task_cfg[m_task_cfg.size() - 1];
		node_cfg.section_day = section_day;

		RAYZCutePetTaskCfg task_cfg;

		if (!GetSubNodeValue(data_element, "seq1", task_cfg.seq) || task_cfg.seq < 0) return -7;
		if (!GetSubNodeValue(data_element, "type", task_cfg.task_id) || task_cfg.task_id < 0) return -4;
		if (!GetSubNodeValue(data_element, "parameter", task_cfg.task_state) || task_cfg.task_state < 0) return -5;
		if (!GetSubNodeValue(data_element, "combat", task_cfg.task_combat) || task_cfg.task_combat < 0) return -6;

		m_task_cfg[m_task_cfg.size() - 1].task.push_back(task_cfg);

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityYunZeCutePetConfig::InitRewardCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -544;
	}

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

		if (m_reward_section_cfg.size() > 0)
		{
			int pre_index = m_reward_section_cfg.size() - 1;
			if (section_start != m_reward_section_cfg[pre_index].section_start ||
				section_end != m_reward_section_cfg[pre_index].section_end)
			{
				if (section_start < m_reward_section_cfg[pre_index].section_end)
				{
					return -100;
				}
				m_reward_section_cfg.push_back(RAYZCutePetRewardSectionCfg());
			}
		}

		if (m_reward_section_cfg.size() == 0)
		{
			if (section_start != 1)
			{
				return -200;
			}
			m_reward_section_cfg.push_back(RAYZCutePetRewardSectionCfg());
		}

		RAYZCutePetRewardSectionCfg & node_cfg = m_reward_section_cfg[m_reward_section_cfg.size() - 1];
		node_cfg.section_start = section_start;
		node_cfg.section_end = section_end;

		RAYZCutePetRewardCfg reward_cfg;

		if (!GetSubNodeValue(data_element, "item_id", reward_cfg.reward_group)) return -1;	

		std::map<int, RAYZCutePetBasicsCfg>::const_iterator it = m_basics_cfg.begin();
		for (; it != m_basics_cfg.end(); ++it)
		{
			if (it->second.reward_group == reward_cfg.reward_group) break;		
		}
		if (it == m_basics_cfg.end()) return -2;
		
		TiXmlElement * reward_list_element = data_element->FirstChildElement("reward_item_list");
		if (NULL == reward_list_element) return -300;

		TiXmlElement * reward_element = reward_list_element->FirstChildElement("reward_item");
		if (NULL == reward_element) return -400;

		while (NULL != reward_element)
		{
			ItemConfigData item;
			if (!item.ReadConfig(reward_element))
			{
				return -3;
			}

			reward_cfg.reward.push_back(item);

			reward_element = reward_element->NextSiblingElement();
		}

		m_reward_section_cfg[m_reward_section_cfg.size() - 1].reward_list.push_back(reward_cfg);

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}
