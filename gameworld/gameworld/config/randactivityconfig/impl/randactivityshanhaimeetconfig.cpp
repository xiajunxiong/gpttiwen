#include "randactivityshanhaimeetconfig.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"


RandActivityShanHaiMeetConfig::RandActivityShanHaiMeetConfig()
{
}

RandActivityShanHaiMeetConfig::~RandActivityShanHaiMeetConfig()
{
	std::vector<RAShanHaiMeetSectionCfg>().swap(m_section_cfg);
	std::vector<RAShanHaiMeetPokerCfg>().swap(m_poker_cfg);
}

bool RandActivityShanHaiMeetConfig::Init(TiXmlElement * RootElement, std::string * err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("basis_configuration", InitBaseCfg);
	LOAD_RA_CONFIG("draw_configuration", InitPokerCfg);
	LOAD_RA_CONFIG("reward_group", InitRewardCfg);
	LOAD_RA_CONFIG("other", InitOtherCfg);

	return true;
}

const RAShanHaiMeetCfg * RandActivityShanHaiMeetConfig::GetBaseCfg(RandActivityManager * ramgr, int seq) const
{
	int act_real_open_day = LOGIC_CONFIG->GetRandActivityConfigManager()->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_SHAN_HAI_MEET);
	for (std::vector<RAShanHaiMeetSectionCfg>::const_iterator it = m_section_cfg.begin(); it != m_section_cfg.end(); ++it)
	{
		if (it->section_start <= act_real_open_day && act_real_open_day <= it->section_end)
		{
			if (0 > seq || seq >= (int)it->item.size()) return NULL;

			return &it->item[seq];
		}
	}
	return NULL;
}

const int RandActivityShanHaiMeetConfig::GetBaseCfgSize(RandActivityManager * ramgr) const
{
	int act_real_open_day = LOGIC_CONFIG->GetRandActivityConfigManager()->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_SHAN_HAI_MEET);
	for (std::vector<RAShanHaiMeetSectionCfg>::const_iterator it = m_section_cfg.begin(); it != m_section_cfg.end(); ++it)
	{
		if (it->section_start <= act_real_open_day && act_real_open_day <= it->section_end)
		{
			return (int)it->item.size();
		}
	}
	return 0;
}

const RAShanHaiMeetPokerCfg * RandActivityShanHaiMeetConfig::GetPokerCfg(RandActivityManager * ramgr) const
{
	int act_real_open_day = LOGIC_CONFIG->GetRandActivityConfigManager()->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_SHAN_HAI_MEET);
	for (std::vector<RAShanHaiMeetPokerCfg>::const_iterator it = m_poker_cfg.begin(); it != m_poker_cfg.end(); ++it)
	{
		if (it->section_start <= act_real_open_day && act_real_open_day <= it->section_end)
		{
			return &(*it);
		}
	}
	return NULL;
}

const RAShanHaiMeetRewardCfg * RandActivityShanHaiMeetConfig::GetRewardCfg(int reward_group) const
{
	std::map<int, RAShanHaiMeetRewardListCfg>::const_iterator it = m_reward_cfg.find(reward_group);
	if (it == m_reward_cfg.end())
	{
		return NULL;
	}

	int rand_rate = rand() % it->second.all_rate;

	for (std::vector<RAShanHaiMeetRewardCfg>::const_iterator iter = it->second.item.begin(); iter != it->second.item.end(); ++iter)
	{
		rand_rate -= iter->rate;

		if (0 > rand_rate)
		{
			return &(*iter);
		}
	}

	return NULL;
}

int RandActivityShanHaiMeetConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -999;
	}

	while (NULL != data_element)
	{
		if (!GetSubNodeValue(data_element, "buy_money", m_other_cfg.buy_money) || 0 >= m_other_cfg.buy_money) return -1;
		if (!GetSubNodeValue(data_element, "cfg_ver", m_other_cfg.cfg_ver) || 0 >= m_other_cfg.cfg_ver) return -2;
		if (!GetSubNodeValue(data_element, "consume_item", m_other_cfg.poker_cost_id) || NULL == ITEMPOOL->GetItem(m_other_cfg.poker_cost_id)) return -3;
		if (!GetSubNodeValue(data_element, "consume_num", m_other_cfg.poker_cost_num) || 0 >= m_other_cfg.poker_cost_num) return -4;

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityShanHaiMeetConfig::InitBaseCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -__LINE__;
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

		if (m_section_cfg.size() > 0)
		{
			int pre_index = m_section_cfg.size() - 1;
			if (section_start != m_section_cfg[pre_index].section_start ||
				section_end != m_section_cfg[pre_index].section_end)
			{
				if (section_start < m_section_cfg[pre_index].section_end)
				{
					return -__LINE__;
				}

				m_section_cfg.push_back(RAShanHaiMeetSectionCfg());
			}
		}
		if (m_section_cfg.size() == 0)
		{
			if (section_start != 1)
			{
				return -__LINE__;
			}
			m_section_cfg.push_back(RAShanHaiMeetSectionCfg());
		}

		RAShanHaiMeetSectionCfg & cfg = m_section_cfg[m_section_cfg.size() - 1];
		cfg.section_start = section_start;
		cfg.section_end = section_end;

		RAShanHaiMeetCfg node;
		if (!GetSubNodeValue(data_element, "seq", node.seq) || node.seq != (int)cfg.item.size()) return -1;
		if (!GetSubNodeValue(data_element, "task_id", node.task_id) || node.task_id <= 0) return -3;
		if (!GetSubNodeValue(data_element, "task_type", node.task_type) || node.task_type < RA_SHAN_HAI_MEET_ACT_TYPE_0 || node.task_type >= RA_SHAN_HAI_MEET_ACT_TYPE_END) return -4;
		if (!GetSubNodeValue(data_element, "param_1", node.param_1) || node.param_1 <= 0) return -5;
		if (!GetSubNodeValue(data_element, "param_2", node.param_2) || node.param_2 < 0) return -6;

		TiXmlElement * reward_list_element = data_element->FirstChildElement("reward_item_list");
		if (reward_list_element != NULL)
		{
			TiXmlElement * reward_element = reward_list_element->FirstChildElement("reward_item");
			while (reward_element)
			{
				ItemConfigData reward;
				if (!reward.ReadConfig(reward_element))
				{
					return -88;
				}
				node.finish_item.push_back(reward);
				reward_element = reward_element->NextSiblingElement();
			}
		}

		TiXmlElement * unfinish_reward_list_element = data_element->FirstChildElement("unfinish_reward_item_list");
		if (unfinish_reward_list_element != NULL)
		{
			TiXmlElement * unfinish_reward_element = unfinish_reward_list_element->FirstChildElement("unfinish_reward_item");
			while (unfinish_reward_element)
			{
				ItemConfigData reward;
				if (!reward.ReadConfig(unfinish_reward_element))
				{
					return -99;
				}
				node.unfinish_item.push_back(reward);
				unfinish_reward_element = unfinish_reward_element->NextSiblingElement();
			}
		}

		cfg.item.push_back(node);
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityShanHaiMeetConfig::InitPokerCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -999;
	}

	while (NULL != data_element)
	{
		RAShanHaiMeetPokerCfg node;
		if (!GetSubNodeValue(data_element, "type", node.seq) || 0 > node.seq || node.seq != (int)m_poker_cfg.size()) return -1;
		if (!GetSubNodeValue(data_element, "section_start", node.section_start) || 0 >= node.section_start) return -2;
		if (!GetSubNodeValue(data_element, "section_end", node.section_end) || 0 > node.section_end) return -3;
		if (0 == node.section_end) node.section_end = INT_MAX;
		if (!GetSubNodeValue(data_element, "reward_group", node.reward_group) || 0 >= node.reward_group) return -4;

		m_poker_cfg.push_back(node);
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityShanHaiMeetConfig::InitRewardCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -999;
	}

	while (NULL != data_element)
	{
		int reward_group = 0;
		if (!GetSubNodeValue(data_element, "reward_group", reward_group) || 0 >= reward_group) return -1;

		RAShanHaiMeetRewardListCfg& cfg = m_reward_cfg[reward_group];
		RAShanHaiMeetRewardCfg node;

		if (!GetSubNodeValue(data_element, "type", node.seq) || 0 > node.seq || node.seq != (int)cfg.item.size()) return -2;
		if (!GetSubNodeValue(data_element, "rate", node.rate) || 0 > node.rate) return -3;
		cfg.all_rate += node.rate;

		TiXmlElement * item_element = data_element->FirstChildElement("reward_item");
		if (NULL != item_element)
		{
			if (!node.item.ReadConfig(item_element)) return -100;
		}

		cfg.item.push_back(node);
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}
