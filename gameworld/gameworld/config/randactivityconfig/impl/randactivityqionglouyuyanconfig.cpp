#include "randactivityqionglouyuyanconfig.hpp"
#include "item/itempool.h"

RandActivityQiongLouYuYanConfig::RandActivityQiongLouYuYanConfig()
{
}

RandActivityQiongLouYuYanConfig::~RandActivityQiongLouYuYanConfig()
{
	std::vector<RAQiongLouYuYanConfig>().swap(qiong_lou_yu_yan_vec);
}

bool RandActivityQiongLouYuYanConfig::Init(TiXmlElement * RootElement, std::string * err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("other", InitOtherCfg);		
	LOAD_RA_CONFIG("qionglou_banquet", InitBanquetCfg);
	LOAD_RA_CONFIG("banquet_group", InitRewardGroupCfg);

	return true;
}

const RAQiongLouYuYanConfig * RandActivityQiongLouYuYanConfig::GetConfig(int open_day, int num) const
{
	for (std::vector<RAQiongLouYuYanConfig>::const_iterator it = qiong_lou_yu_yan_vec.begin(); it != qiong_lou_yu_yan_vec.end(); it++)
	{
		if (it->section_start <= open_day && it->section_end >= open_day)
		{
			if (it->prep_down <= num && it->prep_up >= num)
			{
				return &(*it);
			}
		}
	}
	return NULL;
}

const RAQiongLouYuYanConfig * RandActivityQiongLouYuYanConfig::GetConfigBySeq(int open_day, int seq) const
{
	for (std::vector<RAQiongLouYuYanConfig>::const_iterator it = qiong_lou_yu_yan_vec.begin(); it != qiong_lou_yu_yan_vec.end(); it++)
	{
		if (it->section_start <= open_day && it->section_end >= open_day)
		{
			if (it->seq == seq)
			{
				return &(*it);
			}
		}
	}
	return NULL;
}

const RAQiongLouYuYanConfig * RandActivityQiongLouYuYanConfig::GetConfigByBanquetType(int open_day, int banquet_type) const
{
	for (std::vector<RAQiongLouYuYanConfig>::const_iterator it = qiong_lou_yu_yan_vec.begin(); it != qiong_lou_yu_yan_vec.end(); it++)
	{
		if (it->section_start <= open_day && it->section_end >= open_day)
		{
			if (it->banquet_type == banquet_type)
			{
				return &(*it);
			}
		}
	}
	return NULL;
}

const RAQiongLouYuYanRewardConfig * RandActivityQiongLouYuYanConfig::RandRewardByGroup(int reward_group) const
{
	std::map<int, RAQiongLouYuYanRewardGroupConfig>::const_iterator it = qiong_lou_yu_yan_reward_map.find(reward_group);
	if (it != qiong_lou_yu_yan_reward_map.end())
	{
		if (it->second.weight > 0)
		{
			int rand_num = rand() % it->second.weight;
			for (std::vector<RAQiongLouYuYanRewardConfig>::const_iterator reward_it = it->second.reward_vec.begin(); reward_it != it->second.reward_vec.end(); reward_it++)
			{
				if (reward_it->rate >= rand_num)
				{
					return &(*reward_it);
				}
				rand_num -= reward_it->rate;
			}
		}
	}
	return NULL;
}

int RandActivityQiongLouYuYanConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}
	if (!GetSubNodeValue(data_element, "item_id_1", m_other_cfg.item_id_list[0]) || NULL == ITEMPOOL->GetItem(m_other_cfg.item_id_list[0])) return -1;
	if (!GetSubNodeValue(data_element, "item_id_2", m_other_cfg.item_id_list[1]) || NULL == ITEMPOOL->GetItem(m_other_cfg.item_id_list[1])) return -2;
	if (!GetSubNodeValue(data_element, "item_id_3", m_other_cfg.item_id_list[2]) || NULL == ITEMPOOL->GetItem(m_other_cfg.item_id_list[2])) return -3;

	if (!GetSubNodeValue(data_element, "prep_num", m_other_cfg.prep_num) || m_other_cfg.prep_num <= 0) return -4;
	if (!GetSubNodeValue(data_element, "reward_group_2", m_other_cfg.reward_group_2) || m_other_cfg.reward_group_2 <= 0) return -5;
	if (!GetSubNodeValue(data_element, "banquet_num", m_other_cfg.banquet_num) || m_other_cfg.banquet_num <= 0) return -6;
	if (!GetSubNodeValue(data_element, "banquet_times", m_other_cfg.banquet_times) || m_other_cfg.banquet_times <= 0) return -7;
	if (!GetSubNodeValue(data_element, "phase_times", m_other_cfg.phase_times) || m_other_cfg.phase_times <= 0) return -8;
	if (!GetSubNodeValue(data_element, "start_time", m_other_cfg.start_time) || m_other_cfg.start_time > 2400) return -9;
	if (!GetSubNodeValue(data_element, "prepare_times", m_other_cfg.prepare_times) || m_other_cfg.prepare_times < 0) return -10;
	if (!GetSubNodeValue(data_element, "refresh", m_other_cfg.refresh) || m_other_cfg.refresh < 0) return -11;

	return 0;
}

int RandActivityQiongLouYuYanConfig::InitBanquetCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}

	while (NULL != data_element)
	{
		int section_start = 0, section_end = 0;
		if (!GetSubNodeValue(data_element, "section_start", section_start))
		{
			return -1;
		}
		if (!GetSubNodeValue(data_element, "section_end", section_end))
		{
			return -2;
		}

		if (0 == section_end) // 0 代表无穷
		{
			section_end = INT_MAX;
		}

		RAQiongLouYuYanConfig node;

		node.section_start = section_start;
		node.section_end = section_end;

		if (!GetSubNodeValue(data_element, "type", node.type))
		{
			return -3;
		}

		if (!GetSubNodeValue(data_element, "seq", node.seq))
		{
			return -6;
		}

		if (!GetSubNodeValue(data_element, "prep_down", node.prep_down))
		{
			return -7;
		}

		if (!GetSubNodeValue(data_element, "prep_up", node.prep_up))
		{
			return -8;
		}

		if (0 == node.prep_up) // 0 代表无穷
		{
			node.prep_up = INT_MAX;
		}

		if (!GetSubNodeValue(data_element, "banquet_type", node.banquet_type))
		{
			return -9;
		}

		if (!GetSubNodeValue(data_element, "reward_group", node.reward_group))
		{
			return -10;
		}

		if (!GetSubNodeValue(data_element, "sp_reward_group", node.sp_reward_group))
		{
			return -11;
		}
		qiong_lou_yu_yan_vec.push_back(node);
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityQiongLouYuYanConfig::InitRewardGroupCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}
	while (NULL != data_element)
	{
		int reward_group = 0;
		if (!GetSubNodeValue(data_element, "reward_group", reward_group))
		{
			return -1;
		}
		RAQiongLouYuYanRewardGroupConfig & node = qiong_lou_yu_yan_reward_map[reward_group];
		RAQiongLouYuYanRewardConfig node_cfg;
		node_cfg.reward_group = reward_group;
		if (!GetSubNodeValue(data_element, "rate", node_cfg.rate))
		{
			return -2;
		}

		TiXmlElement * reward_item_list_element = data_element->FirstChildElement("reward_item_list");
		if (reward_item_list_element != NULL)
		{
			TiXmlElement * reward_item_element = reward_item_list_element->FirstChildElement("reward_item");
			while (reward_item_element != NULL)
			{
				ItemConfigData reward_node;
				if (!reward_node.ReadConfig(reward_item_element))
				{
					return -3;
				}
				node_cfg.m_reward_vec.push_back(reward_node);
				reward_item_element = reward_item_element->NextSiblingElement();
			}
		}
		node.reward_vec.push_back(node_cfg);
		data_element = data_element->NextSiblingElement();
	}

	for (std::map<int, RAQiongLouYuYanRewardGroupConfig>::iterator it = qiong_lou_yu_yan_reward_map.begin(); it != qiong_lou_yu_yan_reward_map.end(); it++)
	{
		it->second.weight = 0;
		for (std::vector<RAQiongLouYuYanRewardConfig>::iterator it_or = it->second.reward_vec.begin(); it_or != it->second.reward_vec.end(); it_or++)
		{
			it->second.weight += it_or->rate;
		}
	}

	return 0;
}
