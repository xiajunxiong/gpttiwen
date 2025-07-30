#include "randactivityyunzeersanshiconfig.hpp"
#include "servercommon/roleactivity/rayunzeersanshiparam.hpp"

RandActivityYunZeErSanShiConfig::RandActivityYunZeErSanShiConfig()
{
}

RandActivityYunZeErSanShiConfig::~RandActivityYunZeErSanShiConfig()
{
	std::vector<RAYZErSanShiBasicsCfg>().swap(m_basics_cfg);
	std::vector<RAYZErSanShiRewardSectionCfg>().swap(m_reward_cfg);
	std::vector<RAYZErSanShiTurnTableRewardSectionCfg>().swap(m_turntable_reward_cfg);
}

bool RandActivityYunZeErSanShiConfig::Init(TiXmlElement * RootElement, std::string * err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("basics", InitBasicsCfg);
	LOAD_RA_CONFIG("reward", InitRewardCfg);
	LOAD_RA_CONFIG("reward_turntable", InitTurntableRewardCfg);
	LOAD_RA_CONFIG("other", InitOtherCfg);

	return true;
}

int RandActivityYunZeErSanShiConfig::InitBasicsCfg(TiXmlElement *RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}

	while (NULL != data_element)
	{
		RAYZErSanShiBasicsCfg t_basics_cfg;

		if (!GetSubNodeValue(data_element, "task_id", t_basics_cfg.task_id) || t_basics_cfg.task_id <= 0)
		{
			return -1;
		}

		if (!GetSubNodeValue(data_element, "item_id", t_basics_cfg.reward_group_id) || t_basics_cfg.reward_group_id <= 0)
		{
			return -2;
		}

		m_basics_cfg.push_back(t_basics_cfg);
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityYunZeErSanShiConfig::InitRewardCfg(TiXmlElement *RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
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

		if (m_reward_cfg.size() > 0)
		{
			int pre_index = m_reward_cfg.size() - 1;
			if (section_start != m_reward_cfg[pre_index].section_start ||
				section_end != m_reward_cfg[pre_index].section_end)
			{
				if (section_start < m_reward_cfg[pre_index].section_end)
				{
					return -777;
				}

				m_reward_cfg.push_back(RAYZErSanShiRewardSectionCfg());
			}
		}
		if (m_reward_cfg.size() == 0)
		{
			if (section_start != 1)
			{
				return -11111;
			}
			m_reward_cfg.push_back(RAYZErSanShiRewardSectionCfg());
		}

		RAYZErSanShiRewardSectionCfg & node_cfg = m_reward_cfg[m_reward_cfg.size() - 1];
		node_cfg.section_start = section_start;
		node_cfg.section_end = section_end;

		RAYZErSanShiRewardCfg t_reward_list_cfg;
		if (!GetSubNodeValue(data_element, "item_id", t_reward_list_cfg.reward_group) || t_reward_list_cfg.reward_group<=0)
		{
			return -1;
		}

		TiXmlElement * reward_list_element = data_element->FirstChildElement("reward_item_list");
		if (NULL == reward_list_element) return -__LINE__;

		TiXmlElement * reward_element = reward_list_element->FirstChildElement("reward_item");
		if (NULL == reward_element) return -__LINE__;

		ItemConfigData t_item;
		while (NULL != reward_element)
		{
			if (!t_item.ReadConfig(reward_element))
			{
				return -__LINE__;
			}

			t_reward_list_cfg.reward_list.push_back(t_item);
			reward_element = reward_element->NextSiblingElement();
		}

		node_cfg.reward.push_back(t_reward_list_cfg);

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityYunZeErSanShiConfig::InitTurntableRewardCfg(TiXmlElement *RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
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

		if (m_turntable_reward_cfg.size() > 0)
		{
			int pre_index = m_turntable_reward_cfg.size() - 1;
			if (section_start != m_turntable_reward_cfg[pre_index].section_start ||
				section_end != m_turntable_reward_cfg[pre_index].section_end)
			{
				if (section_start < m_turntable_reward_cfg[pre_index].section_end)
				{
					return -777;
				}

				m_turntable_reward_cfg.push_back(RAYZErSanShiTurnTableRewardSectionCfg());
			}
		}
		if (m_turntable_reward_cfg.size() == 0)
		{
			if (section_start != 1)
			{
				return -11111;
			}
			m_turntable_reward_cfg.push_back(RAYZErSanShiTurnTableRewardSectionCfg());
		}

		RAYZErSanShiTurnTableRewardSectionCfg & node_cfg = m_turntable_reward_cfg[m_turntable_reward_cfg.size() - 1];
		node_cfg.section_start = section_start;
		node_cfg.section_end = section_end;


		RAYZErSanShiTurnTableRewardCfg reward_cfg;
		if (!GetSubNodeValue(data_element, "seq", reward_cfg.seq) || reward_cfg.seq < 0) return -1;

		int rate = 0;
		if (!GetSubNodeValue(data_element, "weight", rate) || rate <= 0) return -2;	
		m_turntable_reward_cfg[m_turntable_reward_cfg.size() - 1].m_turntable_total_rate += rate;
		reward_cfg.rate = m_turntable_reward_cfg[m_turntable_reward_cfg.size() - 1].m_turntable_total_rate;

		if (!GetSubNodeValue(data_element, "hearsay", reward_cfg.is_hearsay) || (reward_cfg.is_hearsay != 0 && reward_cfg.is_hearsay != 1)) return -3;

		TiXmlElement * reward_element = data_element->FirstChildElement("reward_item");
		if (NULL == reward_element) return -__LINE__;

		ItemConfigData t_item;
		if (!t_item.ReadConfig(reward_element))
		{
			return -4;
		}

		reward_cfg.turntable_reward = t_item;
		node_cfg.turntable_reward.push_back(reward_cfg);

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityYunZeErSanShiConfig::InitOtherCfg(TiXmlElement *RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}

	if (!GetSubNodeValue(data_element, "frequency", m_other_cfg.frequency) || m_other_cfg.frequency <= 0 || m_other_cfg.frequency>YUNZE_ER_SAN_SHI_MAX_TASK_NUM) return -1;
	if (!GetSubNodeValue(data_element, "frequency_total", m_other_cfg.frequency_total) || m_other_cfg.frequency_total <= 0) return -2;
	if (!GetSubNodeValue(data_element, "exp", m_other_cfg.exp) || m_other_cfg.exp < 0) return -3;

	return 0;
}

const RAYZErSanShiTurnTableRewardCfg * RandActivityYunZeErSanShiConfig::GetRandTurntableRewardCfg(RandActivityManager* ramgr) const
{
	if (NULL == ramgr) return NULL;

	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_YUN_ZE_ER_SAN_SHI);
	for (int i = 0; i < static_cast<int>(m_turntable_reward_cfg.size()); ++i)
	{
		if (m_turntable_reward_cfg[i].section_start <= act_real_open_day && m_turntable_reward_cfg[i].section_end >= act_real_open_day)
		{
			int rand_reward = RandomNum(m_turntable_reward_cfg[i].m_turntable_total_rate);

			for (int j = 0; j < (int)m_turntable_reward_cfg[i].turntable_reward.size(); ++j)
			{
				if (m_turntable_reward_cfg[i].turntable_reward[j].rate > rand_reward)
				{
					return &m_turntable_reward_cfg[i].turntable_reward[j];
				}
			}
		}
	}

	return NULL;
}

const RAYZErSanShiRewardCfg * RandActivityYunZeErSanShiConfig::GetRewardCfg(RandActivityManager* ramgr, int task_id) const
{
	if (NULL == ramgr || task_id <= 0) return NULL;

	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_YUN_ZE_ER_SAN_SHI);
	for (int i = 0; i < (int)m_reward_cfg.size(); ++i)
	{
		if (m_reward_cfg[i].section_start <= act_real_open_day && m_reward_cfg[i].section_end >= act_real_open_day)
		{
			for (int j = 0; (int)j < (int)m_basics_cfg.size(); ++j)
			{
				if (m_basics_cfg[j].task_id == task_id)
				{
					for (int k = 0; (int)k < (int)m_reward_cfg[i].reward.size(); ++k)
					{
						if (m_basics_cfg[j].reward_group_id == m_reward_cfg[i].reward[k].reward_group)
						{
							return &m_reward_cfg[i].reward[k];
						}

					}
				}
			}
		}
	}

	return NULL;
}

int RandActivityYunZeErSanShiConfig::GetRandTaskCfg(int frequency)const
{
	if(frequency >(int)m_basics_cfg.size())
	{
		return -1;
	}

	std::vector<int> rand_task;
	for (int i = 0; i < (int)m_basics_cfg.size(); ++i)
	{
		rand_task.push_back(m_basics_cfg[i].task_id);
	}
	if (rand_task.size() == 0) return -2;

	int rand_num = RandomNum(rand_task.size());

	return rand_task[rand_num];
}

