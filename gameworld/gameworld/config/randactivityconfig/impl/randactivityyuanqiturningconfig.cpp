#include "randactivityyuanqiturningconfig.hpp"
#include "global/randactivity/randactivitymanager.hpp"
#include <set>

RandActivityYuanQiTurningConfig::RandActivityYuanQiTurningConfig()
{
}

RandActivityYuanQiTurningConfig::~RandActivityYuanQiTurningConfig()
{
	std::vector<RAYuanQiTurningInfoBankList>().swap(m_info_bank);
}

bool RandActivityYuanQiTurningConfig::Init(TiXmlElement * RootElement, std::string * err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("vitality_draw", InitBankCfg);
	LOAD_RA_CONFIG("sign_group", InitBankDetailCfg);
	LOAD_RA_CONFIG("vitality_raward_group", InitRewardCfg);

	return true;
}

const RAYuanQiTurningInfoBank * RandActivityYuanQiTurningConfig::RankInfoBank(int open_day) const
{
	for (std::vector<RAYuanQiTurningInfoBankList>::const_iterator it = m_info_bank.begin(); it != m_info_bank.end(); it++)
	{
		if (it->section_start <= open_day && it->section_end >= open_day)
		{
			if ((short)it->bank_vec.size() > 0)
			{
				int rand_idx = rand() % it->bank_vec.size();

				return &it->bank_vec[rand_idx];
			}
		}
	}
	return NULL;
}

const RAYuanQiTurningInfoDetail * RandActivityYuanQiTurningConfig::RankInfoDetail(int bank_group) const
{
	std::map<int, RAYuanQiTurningInfoDetailList>::const_iterator it = m_info_detail.find(bank_group);
	if (it != m_info_detail.end())
	{
		if (it->second.weight > 0)
		{
			int rand_weight = rand() % it->second.weight;
			for (std::vector<RAYuanQiTurningInfoDetail>::const_iterator itor = it->second.info.begin(); itor != it->second.info.end(); itor++)
			{
				if (itor->rate > rand_weight)
				{
					return &(*itor);
				}
				rand_weight -= itor->rate;
			}
		}
	}
	return NULL;
}

const RAYuanQiTurningRewardGroup * RandActivityYuanQiTurningConfig::GetRewardCfg(int reward_group) const
{
	std::map<int, RAYuanQiTurningRewardGroupList>::const_iterator it = m_reward_group.find(reward_group);
	if (it != m_reward_group.end())
	{
		if (it->second.weight > 0)
		{
			int rand_weight = rand() % it->second.weight;
			for (std::vector<RAYuanQiTurningRewardGroup>::const_iterator itor = it->second.reward_vec.begin(); itor != it->second.reward_vec.end(); itor++)
			{
				if (itor->rate > rand_weight)
				{
					return &(*itor);
				}
				rand_weight -= itor->rate;
			}
		}
	}
	return NULL;
}

int RandActivityYuanQiTurningConfig::InitBankCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}

	while (data_element != NULL)
	{
		RAYuanQiTurningInfoBank node;
		if (!GetSubNodeValue(data_element, "type", node.type) || node.type < 0)
		{
			return -1;
		}

		if (!GetSubNodeValue(data_element, "section_start", node.section_start) || node.section_start < 0)
		{
			return -2;
		}

		if (!GetSubNodeValue(data_element, "section_end", node.section_end) || node.section_end < 0)
		{
			return -3;
		}

		if (node.section_end <= 0)
		{
			node.section_end = MAX_INT;
		}

		if (!GetSubNodeValue(data_element, "seq", node.seq) || node.seq < 0)
		{
			return -4;
		}

		if (!GetSubNodeValue(data_element, "sign_group", node.sign_group) || node.sign_group < 0)
		{
			return -5;
		}

		if (!GetSubNodeValue(data_element, "reward_group", node.reward_group) || node.reward_group < 0)
		{
			return -6;
		}
		bool has_find = false;
		for (std::vector<RAYuanQiTurningInfoBankList>::iterator it = m_info_bank.begin(); it != m_info_bank.end(); it++)
		{
			if (it->section_start == node.section_start)
			{
				//已经有这个了
				it->bank_vec.push_back(node);
				has_find = true;
				break;
			}
		}

		if (!has_find)
		{
			RAYuanQiTurningInfoBankList node_list;
			node_list.section_start = node.section_start;
			node_list.section_end = node.section_end;
			node_list.bank_vec.push_back(node);
			m_info_bank.push_back(node_list);
		}
		
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityYuanQiTurningConfig::InitBankDetailCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}

	while (data_element != NULL)
	{
		int sign_group_id = 0;
		if (!GetSubNodeValue(data_element, "sign_group_id", sign_group_id) || sign_group_id <= 0)
		{
			return -1;
		}
		std::map<int, RAYuanQiTurningInfoDetailList>::iterator it = m_info_detail.find(sign_group_id);
		if (it == m_info_detail.end())
		{
			RAYuanQiTurningInfoDetailList &node_list = m_info_detail[sign_group_id];
			node_list.weight = 0;
		}
		RAYuanQiTurningInfoDetailList &node_list = m_info_detail[sign_group_id];

		RAYuanQiTurningInfoDetail node;
		node.sign_group_id = sign_group_id;
		if (!GetSubNodeValue(data_element, "sign_id", node.sign_id) || node.sign_id < 0)
		{
			return -2;
		}

		if (!GetSubNodeValue(data_element, "rate", node.rate) || node.rate < 0)
		{
			return -3;
		}
		node_list.weight += node.rate;
		node_list.info.push_back(node);
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityYuanQiTurningConfig::InitRewardCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}

	while (data_element != NULL)
	{
		int reward_group_id = 0;
		if (!GetSubNodeValue(data_element, "reward_group_id", reward_group_id) || reward_group_id <= 0)
		{
			return -1;
		}
		std::map<int, RAYuanQiTurningRewardGroupList>::iterator it = m_reward_group.find(reward_group_id);
		if (it == m_reward_group.end())
		{
			RAYuanQiTurningRewardGroupList &node_list = m_reward_group[reward_group_id];
			node_list.weight = 0;
		}
		RAYuanQiTurningRewardGroupList &node_list = m_reward_group[reward_group_id];

		RAYuanQiTurningRewardGroup node;
		node.reward_group_id = reward_group_id;
		if (!GetSubNodeValue(data_element, "type", node.type) || node.type < 0)
		{
			return -2;
		}

		if (!GetSubNodeValue(data_element, "rate", node.rate) || node.rate < 0)
		{
			return -3;
		}

		TiXmlElement * reward_item_list_element = data_element->FirstChildElement("reward_item_list");
		if (reward_item_list_element != NULL)
		{
			TiXmlElement * reward_item_element = reward_item_list_element->FirstChildElement("reward_item");
			while (NULL != reward_item_element)
			{
				ItemConfigData node_reward;
				if (!node_reward.ReadConfig(reward_item_element))
				{
					return -5;
				}
				node.reward_vec.push_back(node_reward);
				reward_item_element = reward_item_element->NextSiblingElement();
			}
		}

		node_list.weight += node.rate;
		node_list.reward_vec.push_back(node);
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}