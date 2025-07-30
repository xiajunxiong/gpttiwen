#include "randactivitycanglongcifuconfig.hpp"

#include "servercommon/activitydef.hpp"

RandActivityCangLongCiFuConfig::RandActivityCangLongCiFuConfig()
{
}

RandActivityCangLongCiFuConfig::~RandActivityCangLongCiFuConfig()
{
	std::vector<CLCFBasicSectionCfg>().swap(m_base_section_cfg);
	std::vector<CLCFRewardGroupCfg>().swap(m_reward_group_cfg);
}

bool RandActivityCangLongCiFuConfig::Init(TiXmlElement * RootElement, std::string * err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("basis_configuration", InitBasicCfg);
	LOAD_RA_CONFIG("reward_configuration", InitRewardCfg);
	LOAD_RA_CONFIG("other", InitOtherCfg);

	return true;
}

const std::pair<ItemConfigData, int>* RandActivityCangLongCiFuConfig::GetBasicBySeqCfg(RandActivityManager * ramgr, int seq, int& fragments_num) const
{
	if (NULL == ramgr) return NULL;

	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_CANG_LONG_CI_FU);

	for (int i = 0; i < (int)m_base_section_cfg.size(); ++i)
	{
		if (m_base_section_cfg[i].section_start <= act_real_open_day && m_base_section_cfg[i].section_end >= act_real_open_day)
		{
			if (seq < 0 || seq >= (int)m_base_section_cfg[i].consume_item_list.size()) return NULL;

			fragments_num = (int)m_base_section_cfg[i].consume_item_list.size();
			return &m_base_section_cfg[i].consume_item_list[seq];
		}
	}

	return NULL;
}

const std::vector<ItemConfigData> * RandActivityCangLongCiFuConfig::GetRewardCfg(int reward_group) const
{
	for (int i = 0; i < (int)m_reward_group_cfg.size(); ++i)
	{
		if (m_reward_group_cfg[i].reward_group == reward_group)
		{
			int rand_rate = RandomNum(m_reward_group_cfg[i].total_rate);

			for (size_t j = 0; j < m_reward_group_cfg[i].reward_list.size(); j++)
			{
				if (rand_rate < m_reward_group_cfg[i].reward_list[j].first)
				{
					return &m_reward_group_cfg[i].reward_list[j].second;
				}
			}
		}
	}

	return NULL;
}

int RandActivityCangLongCiFuConfig::InitBasicCfg(TiXmlElement * RootElement)
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
				m_base_section_cfg.push_back(CLCFBasicSectionCfg());
			}
		}

		if (m_base_section_cfg.size() == 0)
		{
			if (section_start != 1)
			{
				return -200;
			}
			m_base_section_cfg.push_back(CLCFBasicSectionCfg());
		}

		CLCFBasicSectionCfg & node_cfg = m_base_section_cfg[m_base_section_cfg.size() - 1];
		node_cfg.section_start = section_start;
		node_cfg.section_end = section_end;

		std::pair<ItemConfigData, int> consume_cfg;

		TiXmlElement* reward_element = data_element->FirstChildElement("light_item");
		if (NULL == reward_element) return -100;

		if (!consume_cfg.first.ReadConfig(reward_element)) return -1;
		if (!GetSubNodeValue(data_element, "light_group", consume_cfg.second) || consume_cfg.second < 0) return -2;

		node_cfg.consume_item_list.push_back(consume_cfg);

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityCangLongCiFuConfig::InitRewardCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element) return -544;

	while (data_element)
	{

		int reward_group = -1;

		if (!GetSubNodeValue(data_element, "group", reward_group) || reward_group < 0) return -1;

		if (m_reward_group_cfg.size() > 0)
		{
			if (m_reward_group_cfg[m_reward_group_cfg.size() - 1].reward_group != reward_group)
			{
				for (size_t i = 0; i < m_reward_group_cfg.size(); i++)
				{
					if (m_reward_group_cfg[i].reward_group == reward_group) return -2;
				}

				m_reward_group_cfg.push_back(CLCFRewardGroupCfg());
			}
		}
		else if (m_reward_group_cfg.size() == 0)
		{
			m_reward_group_cfg.push_back(CLCFRewardGroupCfg());
		}

		CLCFRewardGroupCfg & node_cfg = m_reward_group_cfg[m_reward_group_cfg.size() - 1];
		node_cfg.reward_group = reward_group;

		int rate = 0;
		std::pair<int, std::vector<ItemConfigData> > temp_pair;

		if (!GetSubNodeValue(data_element, "rate", rate) || rate < 0) return -3;
		node_cfg.total_rate += rate;
		temp_pair.first = node_cfg.total_rate;

		TiXmlElement* reward_list_element = data_element->FirstChildElement("reward_item_list");
		if (NULL == reward_list_element) return -100;

		TiXmlElement* reward_element = reward_list_element->FirstChildElement("reward_item");
		if (NULL == reward_element) return -200;

		while (reward_element)
		{
			ItemConfigData item;
			if (!item.ReadConfig(reward_element)) return -4;

			temp_pair.second.push_back(item);

			reward_element = reward_element->NextSiblingElement();
		}	
		if (temp_pair.second.empty()) return -5;

		node_cfg.reward_list.push_back(temp_pair);

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityCangLongCiFuConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element) return -544;

	TiXmlElement* reward_element = data_element->FirstChildElement("all_light_item");
	if (NULL == reward_element) return -1;

	if (!m_other_cfg.last_reward.ReadConfig(reward_element)) return -2;

	return 0;
}