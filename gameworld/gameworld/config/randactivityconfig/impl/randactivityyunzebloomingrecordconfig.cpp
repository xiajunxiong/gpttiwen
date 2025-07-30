#include "randactivityyunzebloomingrecordconfig.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"

RandActivityYunZeBloomingRecordConfig::RandActivityYunZeBloomingRecordConfig()
{

}

RandActivityYunZeBloomingRecordConfig::~RandActivityYunZeBloomingRecordConfig()
{
	std::vector<RAYZBloomingBasicsCfg>().swap(m_basics_cfg);
	std::vector<RAYZBloomingRecordRewardSectionCfg>().swap(m_reward_cfg);
}

bool RandActivityYunZeBloomingRecordConfig::Init(TiXmlElement *RootElement, std::string *err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("basics", InitBasicsCfg);
	LOAD_RA_CONFIG("reward", InitRewardCfg);
	LOAD_RA_CONFIG("other", InitOtherCfg);

	return true;
}

int RandActivityYunZeBloomingRecordConfig::InitBasicsCfg(TiXmlElement *RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -544;
	}

	while (NULL != data_element)
	{
		RAYZBloomingBasicsCfg cfg;
		if (!GetSubNodeValue(data_element, "integral", cfg.integral) || cfg.integral < 0) return -__LINE__;
		if (!GetSubNodeValue(data_element, "item_id", cfg.reward_group) || cfg.reward_group <= 0) return -__LINE__;

		m_basics_cfg.push_back(cfg);

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityYunZeBloomingRecordConfig::InitRewardCfg(TiXmlElement *RootElement)
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

				m_reward_cfg.push_back(RAYZBloomingRecordRewardSectionCfg());
			}
		}
		if (m_reward_cfg.size() == 0)
		{
			if (section_start != 1)
			{
				return -11111;
			}
			m_reward_cfg.push_back(RAYZBloomingRecordRewardSectionCfg());
		}

		RAYZBloomingRecordRewardSectionCfg & node_cfg = m_reward_cfg[m_reward_cfg.size() - 1];
		node_cfg.section_start = section_start;
		node_cfg.section_end = section_end;

		RAYZBloomingRecordRewardCfg reward_cfg;
		if (!GetSubNodeValue(data_element, "item_id", reward_cfg.reward_group) || reward_cfg.reward_group <= 0) return -__LINE__;

		TiXmlElement * reward_list_element = data_element->FirstChildElement("reward_item_list");
		if (NULL == reward_list_element) return -__LINE__;

		TiXmlElement * reward_element = reward_list_element->FirstChildElement("reward_item");
		if (NULL == reward_element) return -__LINE__;
		while (NULL != reward_element)
		{
			ItemConfigData item;
			if (!item.ReadConfig(reward_element)) return -__LINE__;

			reward_cfg.reward_list.push_back(item);
			reward_element = reward_element->NextSiblingElement();
		}

		node_cfg.reward_group_list.push_back(reward_cfg);

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityYunZeBloomingRecordConfig::InitOtherCfg(TiXmlElement *RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -544;
	}

	if (!GetSubNodeValue(root_element, "time", m_other_cfg.time) || m_other_cfg.time <= 0) return -__LINE__;
	if (!GetSubNodeValue(root_element, "frequency", m_other_cfg.frequency) || m_other_cfg.frequency <= 0) return -__LINE__;
	if (!GetSubNodeValue(root_element, "frequency_total", m_other_cfg.frequency_total) || m_other_cfg.frequency_total<= 0) return -__LINE__;

	return 0;
}

const RAYZBloomingRecordRewardCfg * RandActivityYunZeBloomingRecordConfig::GetRewardByIntegralCfg(RandActivityManager * ramgr, int reward_group)const
{
	if (NULL == ramgr || reward_group <= 0) return NULL;

	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_YUN_ZE_BLOOMING_RECORD);

	for (int i = 0; i < (int)m_reward_cfg.size(); ++i)
	{
		if (m_reward_cfg[i].section_start <= act_real_open_day && m_reward_cfg[i].section_end >= act_real_open_day)
		{
			for (int j = 0; j < (int)m_reward_cfg[i].reward_group_list.size(); ++j)
			{
				if (m_reward_cfg[i].reward_group_list[j].reward_group == reward_group)
				{
					return &m_reward_cfg[i].reward_group_list[j];
				}
			}
		}
	}

	return NULL;
}

bool RandActivityYunZeBloomingRecordConfig::IsCanReceiveReward(int cur_integral,int reward_group)const
{
	if (cur_integral < 0 || reward_group <=0) return false;

	for (int i = 0; i < (int)m_basics_cfg.size(); ++i)
	{
		if (m_basics_cfg[i].reward_group == reward_group)
		{
			if (m_basics_cfg[i].integral <= cur_integral)
			{
				return true;
			}
		}
	}

	return false;
}
