#include "randactivityluckysignconfig.hpp"

RandActivityLuckySignConfig::RandActivityLuckySignConfig()
{
}

RandActivityLuckySignConfig::~RandActivityLuckySignConfig()
{
	std::vector<RALuckySignSectionCfg>().swap(m_lucky_sign_section_cfg);
}

bool RandActivityLuckySignConfig::Init(TiXmlElement * RootElement, std::string * err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("huaqian_group", InitRewardGroupCfg);
	LOAD_RA_CONFIG("sign_group_id", InitSignGroupCfg);
	LOAD_RA_CONFIG("lucky_sign", InitCfg);		//花签基础配置需要放在奖励组与签文组读取之后
	
	return true;
}

const RALuckySignCfg * RandActivityLuckySignConfig::GetRandCfg(RandActivityManager * ramgr) const
{
	if(NULL == ramgr) return NULL;

	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_LUCKY_SIGN);
	for (int i = 0; i < (int)m_lucky_sign_section_cfg.size(); i++)
	{
		if (m_lucky_sign_section_cfg[i].section_start <= act_real_open_day && m_lucky_sign_section_cfg[i].section_end >= act_real_open_day && !m_lucky_sign_section_cfg[i].node_list.empty())
		{
			int rate_num = RandomNum((int)m_lucky_sign_section_cfg[i].node_list.size());
			return &m_lucky_sign_section_cfg[i].node_list[rate_num];
		}
	}
	return NULL;
}

const RALuckySignSignCfg * RandActivityLuckySignConfig::GetSignCfg(int sign_group_id) const
{
	std::map<int, RALuckySignSignGroupCfg>::const_iterator it = m_sign_group_map.find(sign_group_id);
	if(m_sign_group_map.end() == it || it->second.node_list.empty()) return NULL;

	int rate_num = RandomNum(it->second.rate_count);
	for (int i = 0; i < (int)it->second.node_list.size(); i++)
	{
		if (rate_num < it->second.node_list[i].rate)
		{
			return &it->second.node_list[i];
		}
		rate_num -= it->second.node_list[i].rate;
	}

	return NULL;
}

const RALuckySignRewardCfg * RandActivityLuckySignConfig::GetRewardCfg(int reward_group_id) const
{
	std::map<int, RALuckySignRewardGroupCfg>::const_iterator it = m_reward_group_map.find(reward_group_id);
	if (m_reward_group_map.end() == it || it->second.node_list.empty()) return NULL;

	int rate_num = RandomNum(it->second.rate_count);
	for (int i = 0; i < (int)it->second.node_list.size(); i++)
	{
		if (rate_num < it->second.node_list[i].rate)
		{
			return &it->second.node_list[i];
		}
		rate_num -= it->second.node_list[i].rate;
	}

	return NULL;
}

int RandActivityLuckySignConfig::InitRewardGroupCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}
	int next_type_seq = 0;
	while (NULL != data_element)
	{
		int reward_group_id = 0;
		if (!GetSubNodeValue(data_element, "reward_group_id", reward_group_id) || reward_group_id <= 0) return -1;

		RALuckySignRewardGroupCfg & reward_group_cfg = m_reward_group_map[reward_group_id];
		RALuckySignRewardCfg node_cfg;

		if (!GetSubNodeValue(data_element, "type", node_cfg.type_seq) || node_cfg.type_seq != next_type_seq) return -2;
		next_type_seq += 1;

		if (!GetSubNodeValue(data_element, "rate", node_cfg.rate) || node_cfg.rate <= 0) return -3;

		TiXmlElement * reward_list_element = data_element->FirstChildElement("reward_item_list");
		if (NULL != reward_list_element)
		{
			TiXmlElement * reward_element = reward_list_element->FirstChildElement("reward_item");
			while (NULL != reward_element)
			{
				ItemConfigData reward;
				if (!reward.ReadConfig(reward_element))
				{
					return -101;
				}
				node_cfg.reward_list.push_back(reward);
				reward_element = reward_element->NextSiblingElement();
			}
		}
		if (node_cfg.reward_list.empty())
		{
			return -777;
		}
		
		reward_group_cfg.node_list.push_back(node_cfg);
		reward_group_cfg.rate_count += node_cfg.rate;
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityLuckySignConfig::InitSignGroupCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}
	std::set<int> sign_id_set;
	while (NULL != data_element)
	{
		int sign_group_id = 0;
		if (!GetSubNodeValue(data_element, "sign_group_id", sign_group_id) || sign_group_id <= 0) return -1;
		
		RALuckySignSignGroupCfg & sign_group_cfg = m_sign_group_map[sign_group_id];
		RALuckySignSignCfg node_cfg;
		if (!GetSubNodeValue(data_element, "sign_id", node_cfg.sign_id) || sign_id_set.end() != sign_id_set.find(node_cfg.sign_id)) return -2;
		if (!GetSubNodeValue(data_element, "rate", node_cfg.rate) || node_cfg.rate <= 0) return -3;

		sign_group_cfg.node_list.push_back(node_cfg);
		sign_group_cfg.rate_count += node_cfg.rate;
		sign_id_set.insert(node_cfg.sign_id);
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityLuckySignConfig::InitCfg(TiXmlElement * RootElement)
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

		if (m_lucky_sign_section_cfg.size() > 0)
		{
			int pre_index = m_lucky_sign_section_cfg.size() - 1;
			if (section_start != m_lucky_sign_section_cfg[pre_index].section_start ||
				section_end != m_lucky_sign_section_cfg[pre_index].section_end)
			{
				if (section_start < m_lucky_sign_section_cfg[pre_index].section_end)
				{
					return -777;
				}

				m_lucky_sign_section_cfg.push_back(RALuckySignSectionCfg());
			}
		}
		if (m_lucky_sign_section_cfg.size() == 0)
		{
			if (section_start != 1)
			{
				return -11111;
			}
			m_lucky_sign_section_cfg.push_back(RALuckySignSectionCfg());
		}

		RALuckySignSectionCfg & node_cfg = m_lucky_sign_section_cfg[m_lucky_sign_section_cfg.size() - 1];
		node_cfg.section_start = section_start;
		node_cfg.section_end = section_end;

		RALuckySignCfg cfg;
		if (!GetSubNodeValue(data_element, "sign_group_id", cfg.sign_group_id) || m_sign_group_map.find(cfg.sign_group_id) == m_sign_group_map.end()) return -1;
		if (!GetSubNodeValue(data_element, "reward_group_id", cfg.reward_group_id) || m_reward_group_map.find(cfg.reward_group_id) == m_reward_group_map.end()) return -2;

		node_cfg.node_list.push_back(cfg);
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}


