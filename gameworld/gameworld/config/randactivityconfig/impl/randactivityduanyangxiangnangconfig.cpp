#include "randactivityduanyangxiangnangconfig.hpp"

#include "servercommon/activitydef.hpp"

RandActivityDuanYangXiangNangConfig::RandActivityDuanYangXiangNangConfig()
{
}

RandActivityDuanYangXiangNangConfig::~RandActivityDuanYangXiangNangConfig()
{
	std::vector<DYXNBasicSectionCfg>().swap(m_task_section_cfg);
	std::vector<DYXNRewardSectionCfg>().swap(m_reward_section_cfg);
	std::vector<std::pair<ItemID, int> >().swap(m_make_cfg);
}

bool RandActivityDuanYangXiangNangConfig::Init(TiXmlElement * RootElement, std::string * err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("basis_configuration", InitBasicCfg);
	LOAD_RA_CONFIG("reward_configuration", InitRewardCfg);
	LOAD_RA_CONFIG("make_configuration", InitMakeCfg);

	return true;
}

const DYXNBasicCfg * RandActivityDuanYangXiangNangConfig::GetTaskCfg(RandActivityManager * ramgr, int seq) const
{
	if (NULL == ramgr) return NULL;

	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_DUAN_YANG_XIANG_NANG);

	for (int i = 0; i < (int)m_task_section_cfg.size(); ++i)
	{
		if (m_task_section_cfg[i].section_start <= act_real_open_day && m_task_section_cfg[i].section_end >= act_real_open_day)
		{
			if (seq < 0 || seq >= (int)m_task_section_cfg[i].task_list.size()) return NULL;

			return &m_task_section_cfg[i].task_list[seq];
		}
	}

	return NULL;
}

int RandActivityDuanYangXiangNangConfig::GetTaskNumCfg(RandActivityManager * ramgr) const
{
	if (NULL == ramgr) return 0;

	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_DUAN_YANG_XIANG_NANG);

	for (int i = 0; i < (int)m_task_section_cfg.size(); ++i)
	{
		if (m_task_section_cfg[i].section_start <= act_real_open_day && m_task_section_cfg[i].section_end >= act_real_open_day)
		{
			return (int)m_task_section_cfg[i].task_list.size();
		}
	}

	return 0;
}

const DYXNRewardCfg * RandActivityDuanYangXiangNangConfig::RandRewardCfg(RandActivityManager * ramgr) const
{
	if (NULL == ramgr) return NULL;

	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_DUAN_YANG_XIANG_NANG);

	for (size_t i = 0; i < m_reward_section_cfg.size(); i++)
	{
		if (m_reward_section_cfg[i].section_start <= act_real_open_day && m_reward_section_cfg[i].section_end >= act_real_open_day)
		{
			int rand_rate = RandomNum(m_reward_section_cfg[i].total_rate);

			for (size_t j = 0; j < m_reward_section_cfg[i].reward_cfg.size(); j++)
			{
				if (m_reward_section_cfg[i].reward_cfg[j].rate > rand_rate)
				{
					return &m_reward_section_cfg[i].reward_cfg[j];
				}
			}
		}
	}

	return NULL;
}


int RandActivityDuanYangXiangNangConfig::InitBasicCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element) return -544;

	while (data_element)
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

		if (m_task_section_cfg.size() > 0)
		{
			int pre_index = m_task_section_cfg.size() - 1;
			if (section_start != m_task_section_cfg[pre_index].section_start ||
				section_end != m_task_section_cfg[pre_index].section_end)
			{
				if (section_start < m_task_section_cfg[pre_index].section_end)
				{
					return -100;
				}
				m_task_section_cfg.push_back(DYXNBasicSectionCfg());
			}
		}

		if (m_task_section_cfg.size() == 0)
		{
			if (section_start != 1)
			{
				return -200;
			}
			m_task_section_cfg.push_back(DYXNBasicSectionCfg());
		}

		DYXNBasicSectionCfg & node_cfg = m_task_section_cfg[m_task_section_cfg.size() - 1];
		node_cfg.section_start = section_start;
		node_cfg.section_end = section_end;

		DYXNBasicCfg basic_cfg;

		if (!GetSubNodeValue(data_element, "task_type", basic_cfg.task_type) || basic_cfg.task_type < DYXNBasicCfg::TASK_TYPE_JING_JI_CHANG || basic_cfg.task_type >= DYXNBasicCfg::TASK_TYPE_MAX) return -2;
		if (!GetSubNodeValue(data_element, "parameter", basic_cfg.param) || basic_cfg.param <= 0) return -3;

		TiXmlElement* reward_list_element = data_element->FirstChildElement("reward_item_list");
		if (NULL == reward_list_element) return -4;
		TiXmlElement* reward_element = reward_list_element->FirstChildElement("reward_item");
		if (NULL == reward_element) return -5;

		while (reward_element)
		{
			ItemConfigData item;
			if (!item.ReadConfig(reward_element)) return -6;

			basic_cfg.reward_list.push_back(item);

			reward_element = reward_element->NextSiblingElement();
		}

		node_cfg.task_list.push_back(basic_cfg);

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityDuanYangXiangNangConfig::InitRewardCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element) return -544;

	while (data_element)
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
				m_reward_section_cfg.push_back(DYXNRewardSectionCfg());
			}
		}

		if (m_reward_section_cfg.size() == 0)
		{
			if (section_start != 1)
			{
				return -200;
			}
			m_reward_section_cfg.push_back(DYXNRewardSectionCfg());
		}

		DYXNRewardSectionCfg & node_cfg = m_reward_section_cfg[m_reward_section_cfg.size() - 1];
		node_cfg.section_start = section_start;
		node_cfg.section_end = section_end;

		DYXNRewardCfg reward_cfg;

		int rate = 0;
		if (!GetSubNodeValue(data_element, "rate", rate) || rate < 0) return -1;
		node_cfg.total_rate += rate;
		reward_cfg.rate = node_cfg.total_rate;

		TiXmlElement* reward_list_element = data_element->FirstChildElement("reward_item_list");
		if (NULL == reward_list_element) return -4;
		TiXmlElement* reward_element = reward_list_element->FirstChildElement("reward_item");
		if (NULL == reward_element) return -5;

		while (reward_element)
		{
			ItemConfigData item;
			if (!item.ReadConfig(reward_element)) return -6;

			reward_cfg.reward_list.push_back(item);

			reward_element = reward_element->NextSiblingElement();
		}

		node_cfg.reward_cfg.push_back(reward_cfg);

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityDuanYangXiangNangConfig::InitMakeCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element) return -544;

	while (data_element)
	{
		ItemID item_id = 0;
		int num = -1;

		if (!GetSubNodeValue(data_element, "submit_item_id", item_id) || NULL == ITEMPOOL->GetItem(item_id)) return -1;
		if (!GetSubNodeValue(data_element, "submit_num", num) || num < 0) return -2;

		m_make_cfg.push_back(std::make_pair(item_id, num));

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}
