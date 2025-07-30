#include "randactivitylantangchumuconfig.hpp"
#include "servercommon/activitydef.hpp"

RandActivityLanTangChuMuConfig::RandActivityLanTangChuMuConfig()
{
}

RandActivityLanTangChuMuConfig::~RandActivityLanTangChuMuConfig()
{
	std::vector<LTCMBasicSectionCfg>().swap(m_task_section_cfg);
	std::vector<std::pair<ItemID, int> >().swap(m_consume_cfg);
	std::vector<LTCMRewardSectionCfg>().swap(m_reward_section_cfg);
}

bool RandActivityLanTangChuMuConfig::Init(TiXmlElement * RootElement, std::string * err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG_2("basis_configuration", InitTaskCfg);
	LOAD_RA_CONFIG_2("herbs_configuration", InitHerbsCfg);
	LOAD_RA_CONFIG_2("reward_configuration", InitRewardCfg);

	return true;
}

const LTCMBasicCfg * RandActivityLanTangChuMuConfig::GetTaskCfg(RandActivityManager * ramgr, int seq) const
{
	if (NULL == ramgr) return NULL;

	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_LAN_TANG_CHU_MU);

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

int RandActivityLanTangChuMuConfig::GetTaskNumCfg(RandActivityManager * ramgr) const
{
	if (NULL == ramgr) return 0;

	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_LAN_TANG_CHU_MU);

	for (int i = 0; i < (int)m_task_section_cfg.size(); ++i)
	{
		if (m_task_section_cfg[i].section_start <= act_real_open_day && m_task_section_cfg[i].section_end >= act_real_open_day)
		{
			return (int)m_task_section_cfg[i].task_list.size();
		}
	}

	return 0;
}

const std::vector<ItemConfigData>* RandActivityLanTangChuMuConfig::RandRewrdCfg(RandActivityManager * ramgr) const
{
	if (NULL == ramgr) return NULL;

	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_LAN_TANG_CHU_MU);

	for (size_t i = 0; i < m_reward_section_cfg.size(); i++)
	{
		if (m_reward_section_cfg[i].section_start <= act_real_open_day && m_reward_section_cfg[i].section_end >= act_real_open_day)
		{
			int rand_rate = RandomNum(m_reward_section_cfg[i].total_rate);

			for (size_t j = 0; j < m_reward_section_cfg[i].reward_cfg.size(); j++)
			{
				if (m_reward_section_cfg[i].reward_cfg[j].rate > rand_rate)
				{
					return &m_reward_section_cfg[i].reward_cfg[j].reward_list;
				}
			}
		}
	}

	return NULL;
}

int RandActivityLanTangChuMuConfig::InitTaskCfg(TiXmlElement * RootElement, std::string & errormsg)
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
				m_task_section_cfg.push_back(LTCMBasicSectionCfg());
			}
		}

		if (m_task_section_cfg.size() == 0)
		{
			if (section_start != 1)
			{
				return -200;
			}
			m_task_section_cfg.push_back(LTCMBasicSectionCfg());
		}

		LTCMBasicSectionCfg & node_cfg = m_task_section_cfg[m_task_section_cfg.size() - 1];
		node_cfg.section_start = section_start;
		node_cfg.section_end = section_end;

		LTCMBasicCfg basic_cfg;

		if (!GetSubNodeValue(data_element, "task_type", basic_cfg.task_type) || basic_cfg.task_type < LTCMBasicCfg::TASK_TYPE_JING_JI_CHANG || basic_cfg.task_type >= LTCMBasicCfg::TASK_TYPE_MAX) return -2;
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

int RandActivityLanTangChuMuConfig::InitHerbsCfg(TiXmlElement * RootElement, std::string & errormsg)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element) return -544;

	while (data_element)
	{
		std::pair<ItemID, int> consume_item;

		if (!GetSubNodeValue(data_element, "submit_item_id", consume_item.first) || NULL == ITEMPOOL->GetItem(consume_item.first))
		{
			errormsg = STRING_SPRINTF("can not find item_id[%d]", consume_item.first);
			return -1;
		}

		if (!GetSubNodeValue(data_element, "submit_num", consume_item.second) || consume_item.second < 0) return -2;

		m_consume_cfg.push_back(consume_item);

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityLanTangChuMuConfig::InitRewardCfg(TiXmlElement * RootElement, std::string & errormsg)
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
				m_reward_section_cfg.push_back(LTCMRewardSectionCfg());
			}
		}

		if (m_reward_section_cfg.size() == 0)
		{
			if (section_start != 1)
			{
				return -200;
			}
			m_reward_section_cfg.push_back(LTCMRewardSectionCfg());
		}

		LTCMRewardSectionCfg & node_cfg = m_reward_section_cfg[m_reward_section_cfg.size() - 1];
		node_cfg.section_start = section_start;
		node_cfg.section_end = section_end;

		LTCMRewardCfg reward_cfg;

		int rate = 0;
		if (!GetSubNodeValue(data_element, "rate", rate) || rate < 0) return -1;
		node_cfg.total_rate += rate;
		reward_cfg.rate = node_cfg.total_rate;

		TiXmlElement* reward_list_element = data_element->FirstChildElement("bath_reward_item_list");
		if (NULL == reward_list_element) return -4;
		TiXmlElement* reward_element = reward_list_element->FirstChildElement("bath_reward_item");
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
