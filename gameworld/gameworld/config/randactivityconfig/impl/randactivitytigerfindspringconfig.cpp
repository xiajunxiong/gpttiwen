#include "randactivitytigerfindspringconfig.hpp"
#include "global/randactivity/randactivitymanager.hpp"


RandActivityTigerFindSpringConfig::RandActivityTigerFindSpringConfig()
{

}


RandActivityTigerFindSpringConfig::~RandActivityTigerFindSpringConfig()
{
	std::vector<RATFSRewardSectionCfg>().swap(m_reward_section_cfg);
}

bool RandActivityTigerFindSpringConfig::Init(TiXmlElement *RootElement, std::string *err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("gift_configure", InitRewardCfg);
	LOAD_RA_CONFIG("other", InitOtherCfg);

	return true;
}

const RATFSRewardCfg * RandActivityTigerFindSpringConfig::GetRewardCfg(RandActivityManager * ramgr, bool is_yesterday) const
{
	if (NULL == ramgr) return NULL;

	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_TIGER_FIND_SPRING);
	int act_open_day = this->GetActOpenDay(ramgr, RAND_ACTIVITY_TYPE_TIGER_FIND_SPRING);

	for (int i = 0; i < (int)m_reward_section_cfg.size(); ++i)
	{
		if (m_reward_section_cfg[i].section_start <= act_real_open_day && m_reward_section_cfg[i].section_end >= act_real_open_day)
		{
			for (int j = 0; j < (int)m_reward_section_cfg[i].reward_cfg.size(); ++j)
			{
				if (is_yesterday)
				{
					if ((act_open_day - 1) != 0 && m_reward_section_cfg[i].reward_cfg[j].activity_day <= (act_open_day - 1))
					{
						return &m_reward_section_cfg[i].reward_cfg[j];
					}
				}

				if (m_reward_section_cfg[i].reward_cfg[j].activity_day == act_open_day)
				{
					return &m_reward_section_cfg[i].reward_cfg[j];
				}
			}
		}
	}

	return NULL;
}

int RandActivityTigerFindSpringConfig::InitRewardCfg(TiXmlElement * RootElement)
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
				m_reward_section_cfg.push_back(RATFSRewardSectionCfg());
			}
		}

		if (m_reward_section_cfg.size() == 0)
		{
			if (section_start != 1)
			{
				return -200;
			}
			m_reward_section_cfg.push_back(RATFSRewardSectionCfg());
		}

		RATFSRewardSectionCfg & node_cfg = m_reward_section_cfg[m_reward_section_cfg.size() - 1];
		node_cfg.section_start = section_start;
		node_cfg.section_end = section_end;

		RATFSRewardCfg reward_cfg;

		if (!GetSubNodeValue(data_element, "activity_day", reward_cfg.activity_day) || reward_cfg.activity_day <= 0) return -__LINE__;
		if (!GetSubNodeValue(data_element, "seq", reward_cfg.seq) || reward_cfg.seq < 0) return -__LINE__;
		if (!GetSubNodeValue(data_element, "scene_id", reward_cfg.scene_id) || reward_cfg.scene_id < 0) return -__LINE__;
		if (!GetSubNodeValue(data_element, "x", reward_cfg.reward_pos.x) || reward_cfg.reward_pos.x < 0) return -__LINE__;
		if (!GetSubNodeValue(data_element, "y", reward_cfg.reward_pos.y) || reward_cfg.reward_pos.y < 0) return -__LINE__;

		TiXmlElement * reward_list_element = data_element->FirstChildElement("reward_item_list");
		if (NULL == reward_list_element) return -__LINE__;

		TiXmlElement * reward_element = reward_list_element->FirstChildElement("reward_item");
		if (NULL == reward_element) return -__LINE__;

		while (NULL != reward_element)
		{
			ItemConfigData item;
			if (!item.ReadConfig(reward_element))
			{
				return -__LINE__;
			}

			reward_cfg.reward_list.push_back(item);
			reward_element = reward_element->NextSiblingElement();
		}

		node_cfg.reward_cfg.push_back(reward_cfg);

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityTigerFindSpringConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -544;
	}

	if (!GetSubNodeValue(data_element, "explore_radius", m_other_cfg.explore_radius) || m_other_cfg.explore_radius < 0) return -__LINE__;

	return 0;
}
