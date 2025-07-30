#include "randactivityyuanqifanpaiconfig.hpp"

RandActivityYuanQiFanPaiConfig::RandActivityYuanQiFanPaiConfig()
{
}

RandActivityYuanQiFanPaiConfig::~RandActivityYuanQiFanPaiConfig()
{
	std::vector<YQFPBaseSectionCfg>().swap(m_base_section_cfg);
	std::vector<YQFPTimesSectionCfg>().swap(m_times_section_cfg);
	std::vector<YQFPGatherCfg>().swap(m_gather_cfg);
}

bool RandActivityYuanQiFanPaiConfig::Init(TiXmlElement * RootElement, std::string * err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("reward", InitBaseCfg);
	LOAD_RA_CONFIG("reward_frequency", InitTimesCfg);
	LOAD_RA_CONFIG("gather", InitGtherCfg);
	LOAD_RA_CONFIG("other", InitOtherCfg);

	return true;
}

const YQFPBaseCfg * RandActivityYuanQiFanPaiConfig::GetRandRewardCfg(RandActivityManager * ramgr) const
{
	if (NULL == ramgr) return NULL;

	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_YUNA_QI_FAN_PAI);

	for (int i = 0; i < (int)m_base_section_cfg.size(); ++i)
	{
		if (m_base_section_cfg[i].section_start <= act_real_open_day && m_base_section_cfg[i].section_end >= act_real_open_day)
		{
			int rate = RandomNum(m_base_section_cfg[i].total_rate);

			for (int j = 0; j < (int)m_base_section_cfg[i].reward_cfg.size(); j++)
			{
				if (m_base_section_cfg[i].reward_cfg[j].rate >= rate)
				{
					return &m_base_section_cfg[i].reward_cfg[j];
				}
			}
		}
	}

	return NULL;
}

const YQFPTimesRewardCfg * RandActivityYuanQiFanPaiConfig::GetTimesRewardCfg(RandActivityManager * ramgr, int seq) const
{
	if (NULL == ramgr || seq < 0) return NULL;

	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_YUNA_QI_FAN_PAI);

	for (int i = 0; i < (int)m_times_section_cfg.size(); ++i)
	{
		if (m_times_section_cfg[i].section_start <= act_real_open_day && m_times_section_cfg[i].section_end >= act_real_open_day)
		{
			if (seq < (int)m_times_section_cfg[i].reward_cfg.size())
			{
				return &m_times_section_cfg[i].reward_cfg[seq];
			}
		}
	}

	return NULL;
}

const YQFPGatherCfg * RandActivityYuanQiFanPaiConfig::GetGatherCfg(int seq) const
{
	if (seq < 0) return NULL;

	if (seq < (int)m_gather_cfg.size())
	{
		return &m_gather_cfg[seq];
	}

	return NULL;
}

int RandActivityYuanQiFanPaiConfig::InitBaseCfg(TiXmlElement * RootElement)
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
				m_base_section_cfg.push_back(YQFPBaseSectionCfg());
			}
		}

		if (m_base_section_cfg.size() == 0)
		{
			if (section_start != 1)
			{
				return -200;
			}
			m_base_section_cfg.push_back(YQFPBaseSectionCfg());
		}

		YQFPBaseSectionCfg & node_cfg = m_base_section_cfg[m_base_section_cfg.size() - 1];
		node_cfg.section_start = section_start;
		node_cfg.section_end = section_end;

		YQFPBaseCfg base_cfg;
		int rate;

		if (!GetSubNodeValue(data_element, "rate", rate) || rate < 0) return -1;
		node_cfg.total_rate += rate;
		base_cfg.rate = node_cfg.total_rate;

		if (!GetSubNodeValue(data_element, "hearsay", base_cfg.is_hearsay) || (base_cfg.is_hearsay != 0 && base_cfg.is_hearsay != 1)) return -2;

		TiXmlElement * reward_element = data_element->FirstChildElement("reward_item");
		if (NULL == reward_element) return -199;

		if (!base_cfg.reward.ReadConfig(reward_element)) return -3;

		node_cfg.reward_cfg.push_back(base_cfg);

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityYuanQiFanPaiConfig::InitTimesCfg(TiXmlElement * RootElement)
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

		if (m_times_section_cfg.size() > 0)
		{
			int pre_index = m_times_section_cfg.size() - 1;
			if (section_start != m_times_section_cfg[pre_index].section_start ||
				section_end != m_times_section_cfg[pre_index].section_end)
			{
				if (section_start < m_times_section_cfg[pre_index].section_end)
				{
					return -100;
				}
				m_times_section_cfg.push_back(YQFPTimesSectionCfg());
			}
		}

		if (m_times_section_cfg.size() == 0)
		{
			if (section_start != 1)
			{
				return -200;
			}
			m_times_section_cfg.push_back(YQFPTimesSectionCfg());
		}

		YQFPTimesSectionCfg & node_cfg = m_times_section_cfg[m_times_section_cfg.size() - 1];
		node_cfg.section_start = section_start;
		node_cfg.section_end = section_end;

		YQFPTimesRewardCfg times_cfg;

		if (!GetSubNodeValue(data_element, "task", times_cfg.task_limit) || times_cfg.task_limit < 0) return -1;

		TiXmlElement* reward_element = data_element->FirstChildElement("reward_item");
		if (NULL == reward_element) return -199;

		if (!times_cfg.reward.ReadConfig(reward_element)) return -2;

		node_cfg.reward_cfg.push_back(times_cfg);

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityYuanQiFanPaiConfig::InitGtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element) return -544;

	while (NULL != data_element)
	{
		YQFPGatherCfg gather_cfg;
		ItemConfigData consunme_item;

		std::vector<TiXmlElement *> element_list;
		element_list.resize(4);		// 四列
		element_list[0] = data_element->FirstChildElement("item1");
		element_list[1] = data_element->FirstChildElement("item2");
		element_list[2] = data_element->FirstChildElement("item3");
		element_list[3] = data_element->FirstChildElement("item4");

		for (std::vector<TiXmlElement *>::const_iterator it = element_list.begin(); it != element_list.end(); ++it)
		{
			if (NULL == *it) return -999;
			
			if (!consunme_item.ReadConfigNoCheck(*it)) 
				continue;

			gather_cfg.consunme_item.push_back(consunme_item);
		}
		if (gather_cfg.consunme_item.empty()) return -2;

		TiXmlElement * reward_element = data_element->FirstChildElement("reward_item");
		if (NULL == reward_element) return -888;

		if (!gather_cfg.reward.ReadConfig(reward_element)) return -3;

		m_gather_cfg.push_back(gather_cfg);

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityYuanQiFanPaiConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element) return -544;

	TiXmlElement* consunme_element = data_element->FirstChildElement("reward2_item");
	if (NULL == consunme_element) return -1;

	if (!m_other_cfg.consunme_item.ReadConfig(consunme_element)) return -2;
	if (ReadItemConfigData(data_element, "reward_item", m_other_cfg.day_fetch_reward) != 0)return -3;
	

	return 0;
}
