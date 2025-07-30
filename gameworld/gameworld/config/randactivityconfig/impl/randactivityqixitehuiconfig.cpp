#include "randactivityqixitehuiconfig.hpp"
#include "servercommon/roleactivity/raqixitehuiparam.hpp"

RandActivityQiXiTeHuiConfig::RandActivityQiXiTeHuiConfig()
{
}

RandActivityQiXiTeHuiConfig::~RandActivityQiXiTeHuiConfig()
{
	std::vector<RAQiXiTeHuiBaseSectionCfg>().swap(m_base_section_cfg);
	std::vector<RAQiXiTeHuiPhaseRewardSectionCfg>().swap(m_phase_reward_cfg);
}

bool RandActivityQiXiTeHuiConfig::Init(TiXmlElement * RootElement, std::string * err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("reward_group", InitRewardGroupCfg);
	LOAD_RA_CONFIG("basis_configuration", InitBaseCfg);
	LOAD_RA_CONFIG("phase_reward", InitPhaseCfg);
	LOAD_RA_CONFIG("other", InitOtherCfg);

	return true;
}

int RandActivityQiXiTeHuiConfig::GetConfigVer() const
{
	return m_other_cfg.cfg_ver;
}

const RAQiXiTeHuiBaseCfg * RandActivityQiXiTeHuiConfig::GetBaseCfg(RandActivityManager * ramgr, int seq) const
{
	if(NULL == ramgr || seq < 0) return NULL;

	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_QI_XI_TE_HUI);
	for (int i = 0; i < (int)m_base_section_cfg.size(); ++i)
	{
		if (m_base_section_cfg[i].section_start <= act_real_open_day && m_base_section_cfg[i].section_end >= act_real_open_day)
		{
			if (seq < (int)m_base_section_cfg[i].base_list.size())
			{
				return &m_base_section_cfg[i].base_list[seq];
			}
		}
	}

	return NULL;
}

const RAQiXiTeHuiRewardCfg * RandActivityQiXiTeHuiConfig::GetRewardCfg(int reward_group_id) const
{
	std::map<int, RAQiXiTeHuiRewardRroupCfg>::const_iterator it = m_reward_group_cfg.find(reward_group_id);
	if (it == m_reward_group_cfg.end() || it->second.rate_count <= 0 || it->second.reward_list.empty())
	{
		return NULL;
	}
	int rand_rate = RandomNum(it->second.rate_count);
	for (int i = 0; i < (int)it->second.reward_list.size(); ++i)
	{
		if (rand_rate < it->second.reward_list[i].rate)
		{
			return &it->second.reward_list[i];
		}
		rand_rate -= it->second.reward_list[i].rate;
	}

	return NULL;
}

const RAQiXiTeHuiPhaseRewardCfg * RandActivityQiXiTeHuiConfig::GetPhaseRewardCfg(RandActivityManager* ramgr, int seq) const
{
	if (NULL == ramgr || seq < 0) return NULL;

	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_QI_XI_TE_HUI);
	for (int i = 0; i < (int)m_phase_reward_cfg.size(); ++i)
	{
		if (m_phase_reward_cfg[i].section_start <= act_real_open_day && m_phase_reward_cfg[i].section_end >= act_real_open_day)
		{
			if (seq < (int)m_phase_reward_cfg[i].reward_list.size())
			{
				return &m_phase_reward_cfg[i].reward_list[seq];
			}
		}
	}

	return NULL;
}

const RAQiXiTeHuiPhaseRewardSectionCfg * RandActivityQiXiTeHuiConfig::GetPhaseSectionCfg(RandActivityManager * ramgr) const
{
	if (NULL == ramgr) return NULL;

	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_QI_XI_TE_HUI);
	for (int i = 0; i < (int)m_phase_reward_cfg.size(); ++i)
	{
		if (m_phase_reward_cfg[i].section_start <= act_real_open_day && m_phase_reward_cfg[i].section_end >= act_real_open_day)
		{
			return &m_phase_reward_cfg[i];
		}
	}

	return NULL;
}

int RandActivityQiXiTeHuiConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element) return -544;

	if (!GetSubNodeValue(data_element, "cfg_ver", m_other_cfg.cfg_ver) || m_other_cfg.cfg_ver < 0) 
	{
		return -1;
	}

	return 0;
}

int RandActivityQiXiTeHuiConfig::InitBaseCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element) return -544;

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

		if (m_base_section_cfg.size() > 0)
		{
			int pre_index = m_base_section_cfg.size() - 1;
			if (section_start != m_base_section_cfg[pre_index].section_start ||
				section_end != m_base_section_cfg[pre_index].section_end)
			{
				if (section_start < m_base_section_cfg[pre_index].section_end)
				{
					return -100;
				}
				m_base_section_cfg.push_back(RAQiXiTeHuiBaseSectionCfg());
			}
		}

		if (m_base_section_cfg.size() == 0)
		{
			if (section_start != 1)
			{
				return -200;
			}
			m_base_section_cfg.push_back(RAQiXiTeHuiBaseSectionCfg());
		}

		RAQiXiTeHuiBaseSectionCfg & node_cfg = m_base_section_cfg[m_base_section_cfg.size() - 1];
		node_cfg.section_start = section_start;
		node_cfg.section_end = section_end;

		int seq = 0;
		RAQiXiTeHuiBaseCfg base_cfg;

		if (!GetSubNodeValue(data_element, "seq", seq) || seq < 0 || seq != (int)node_cfg.base_list.size() || seq >= RA_QI_XI_TE_HUI_MAX_BASE_NUM) return -1;
		if (!GetSubNodeValue(data_element, "activity_days", base_cfg.activity_day) || base_cfg.activity_day < 0) return -2;
		if (!GetSubNodeValue(data_element, "reward_group", base_cfg.reward_group) || 
			m_reward_group_cfg.end() == m_reward_group_cfg.find(base_cfg.reward_group)) return -3;

		if (!GetSubNodeValue(data_element, "buy_limit", base_cfg.buy_limit) || base_cfg.buy_limit < ACTVITY_BUY_LIMIT_TYPE_BEGIN ||
			base_cfg.buy_limit >= ACTVITY_BUY_LIMIT_TYPE_MAX) return -4;
		
		if (!GetSubNodeValue(data_element, "limit_times", base_cfg.limit_times) || base_cfg.limit_times < 0) return -5;
		if (!GetSubNodeValue(data_element, "price", base_cfg.price) || base_cfg.price < 10) return -6;

		node_cfg.base_list.push_back(base_cfg);
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityQiXiTeHuiConfig::InitRewardGroupCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element) return -544;

	while (NULL != data_element)
	{
		int reward_group_id = 0;
		if (!GetSubNodeValue(data_element, "reward_group", reward_group_id) || reward_group_id <= 0) return -1;

		RAQiXiTeHuiRewardCfg node_cfg;
		if (!GetSubNodeValue(data_element, "rate", node_cfg.rate) || node_cfg.rate <= 0) return -2;

		TiXmlElement* reward_list_element = data_element->FirstChildElement("reward_item_list");
		if (NULL == reward_list_element)
		{
			return -100;
		}
		TiXmlElement * reward_element = reward_list_element->FirstChildElement("reward_item");
		if (NULL == reward_element)
		{
			return -101;
		}
		while (NULL != reward_element)
		{
			ItemConfigData item;
			if (!item.ReadConfig(reward_element)) return -102;

			node_cfg.item_list.push_back(item);
			reward_element = reward_element->NextSiblingElement();
		}

		m_reward_group_cfg[reward_group_id].reward_list.push_back(node_cfg);
		m_reward_group_cfg[reward_group_id].rate_count += node_cfg.rate;
		data_element = data_element->NextSiblingElement();
	}
	return 0;
}

int RandActivityQiXiTeHuiConfig::InitPhaseCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element) return -544;

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

		if (m_phase_reward_cfg.size() > 0)
		{
			int pre_index = m_phase_reward_cfg.size() - 1;
			if (section_start != m_phase_reward_cfg[pre_index].section_start ||
				section_end != m_phase_reward_cfg[pre_index].section_end)
			{
				if (section_start < m_phase_reward_cfg[pre_index].section_end)
				{
					return -100;
				}
				m_phase_reward_cfg.push_back(RAQiXiTeHuiPhaseRewardSectionCfg());
			}
		}

		if (m_phase_reward_cfg.size() == 0)
		{
			if (section_start != 1)
			{
				return -200;
			}
			m_phase_reward_cfg.push_back(RAQiXiTeHuiPhaseRewardSectionCfg());
		}

		RAQiXiTeHuiPhaseRewardSectionCfg & node_cfg = m_phase_reward_cfg[m_phase_reward_cfg.size() - 1];
		node_cfg.section_start = section_start;
		node_cfg.section_end = section_end;

		int seq = 0;
		RAQiXiTeHuiPhaseRewardCfg base_cfg;
	
		if (!GetSubNodeValue(data_element, "seq", seq) || seq < 0 || seq != (int)node_cfg.reward_list.size() || seq >= RA_QI_XI_TE_HUI_MAX_PHASE_NUM) return -1;
		if (!GetSubNodeValue(data_element, "buy_times", base_cfg.buy_times) || base_cfg.buy_times <= 0) return -2;

		TiXmlElement * reward_element = data_element->FirstChildElement("reward_item");
		if (NULL == reward_element)
		{
			return -101;
		}
		
		if (!base_cfg.reward.ReadConfig(reward_element)) 
		{
			return -102;
		}

		node_cfg.reward_list.push_back(base_cfg);
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}
