#include "randactivityshanhaimibaoconfig.hpp"

#include "item/itempool.h"

RandActivityShanHaiMiBaoConfig::RandActivityShanHaiMiBaoConfig()
{
}

RandActivityShanHaiMiBaoConfig::~RandActivityShanHaiMiBaoConfig()
{
	std::vector<RAShanHaiMiBaoSectionCfg>().swap(m_base_section_cfg);
}

bool RandActivityShanHaiMiBaoConfig::Init(TiXmlElement * RootElement, std::string * err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("basis_configuration", InitBaseCfg);
	LOAD_RA_CONFIG("raward_group", InitRewardGroupCfg);
	LOAD_RA_CONFIG("reward_configuration", InitFinishCondRewardCfg);	//需要放在奖励组读取之后
	
	return true;
}

const RAShanHaiMiBaoCfg * RandActivityShanHaiMiBaoConfig::GetBaseCfg(RandActivityManager * ramgr, int row, int column) const
{
	if(NULL == ramgr || row < 0 || row >= MAX_RA_LOTUS_MI_BAO_ROW_NUM || column < 0 || column >= MAX_RA_LOTUS_MI_BAO_COLUMN_NUM) return NULL;

	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_LOTUS_MI_BAO);
	for (int i = 0; i < (int)m_base_section_cfg.size(); i++)
	{
		if (m_base_section_cfg[i].section_start <= act_real_open_day && m_base_section_cfg[i].section_end >= act_real_open_day)
		{
			return &m_base_section_cfg[i].node_list[row][column];
		}
	}

	return NULL;
}

const RAShanHaiMiBaoRewardCfg * RandActivityShanHaiMiBaoConfig::GetRandRewardCfg(int reward_group_id) const
{
	std::map<int, RAShanHaiMiBaoRewardGroupCfg>::const_iterator it = m_reward_group_map.find(reward_group_id);
	if(it == m_reward_group_map.end() || it->second.node_list.empty()) return NULL;

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

const RAShanMiBaoFinishCondRewardCfg * RandActivityShanHaiMiBaoConfig::GetCondRewardCfg(int cond_id, int param) const
{
	std::map<int, std::map<int, RAShanMiBaoFinishCondRewardCfg> >::const_iterator cond_it = m_finish_cond_reward_map.find(cond_id);
	if(cond_it == m_finish_cond_reward_map.end()) return NULL;

	std::map<int, RAShanMiBaoFinishCondRewardCfg>::const_iterator reward_it = cond_it->second.find(param);
	if(reward_it == cond_it->second.end()) return NULL;

	return &reward_it->second;
}

int RandActivityShanHaiMiBaoConfig::InitBaseCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}

	int next_row = 1, next_column = 0;
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
					return -777;
				}
				next_row = 1;
				next_column = 0;
				m_base_section_cfg.push_back(RAShanHaiMiBaoSectionCfg());
			}
		}
		if (m_base_section_cfg.size() == 0)
		{
			if (section_start != 1)
			{
				return -11111;
			}
			m_base_section_cfg.push_back(RAShanHaiMiBaoSectionCfg());
		}

		RAShanHaiMiBaoSectionCfg & node_cfg = m_base_section_cfg[m_base_section_cfg.size() - 1];
		node_cfg.section_start = section_start;
		node_cfg.section_end = section_end;

		int row = 0, column = 0;
		if (!GetSubNodeValue(data_element, "line_num", row) ||  row != next_row) return -1;
		if (!GetSubNodeValue(data_element, "seq", column) || column != next_column) return -2;
		next_column++;
		if (next_column > MAX_RA_LOTUS_MI_BAO_COLUMN_NUM)
		{
			next_row++;
			next_column = 0;
			data_element = data_element->NextSiblingElement();
			continue;
		}
		if (next_row > MAX_RA_LOTUS_MI_BAO_ROW_NUM)
		{	
			data_element = data_element->NextSiblingElement();
			continue;
		}
		RAShanHaiMiBaoCfg cfg;
		if (!GetSubNodeValue(data_element, "activation_item_id", cfg.active_item_id) || NULL == ITEMPOOL->GetItem(cfg.active_item_id)) 
		{
			return -3;
		}
		if (!GetSubNodeValue(data_element, "activation_item_num", cfg.active_item_num) || cfg.active_item_num <= 0) return -4;

		node_cfg.node_list[row - 1][column] = cfg;
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityShanHaiMiBaoConfig::InitRewardGroupCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}
	while (NULL != data_element)
	{
		int reward_group_id = 0;
		if (!GetSubNodeValue(data_element, "reward_group", reward_group_id) || reward_group_id <= 0) return -1;

		RAShanHaiMiBaoRewardCfg node_cfg;
		TiXmlElement * reward_list_element = data_element->FirstChildElement("reward_item_list");
		if (NULL == reward_list_element)
		{
			return -100;
		}
		TiXmlElement * reward_element = reward_list_element->FirstChildElement("reward_item");
		while (reward_element != NULL)
		{
			ItemConfigData reward;
			if (!reward.ReadConfig(reward_element))
			{
				return -101;
			}
			node_cfg.reward_list.push_back(reward);
			reward_element = reward_element->NextSiblingElement();
		}
		if (node_cfg.reward_list.empty())
		{
			return -102;
		}
		if (!GetSubNodeValue(data_element, "rate", node_cfg.rate) || node_cfg.rate <= 0) return -2;

		m_reward_group_map[reward_group_id].rate_count += node_cfg.rate;
		m_reward_group_map[reward_group_id].node_list.push_back(node_cfg);
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityShanHaiMiBaoConfig::InitFinishCondRewardCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}
	while (NULL != data_element)
	{
		int condition_id = 0, param = 0;
		if (!GetSubNodeValue(data_element, "lucky_condition", condition_id) || condition_id <= RA_LOTUS_MI_BAO_CONDITION_TYPE_BEGIN || condition_id >= RA_LOTUS_MI_BAO_CONDITION_TYPE_MAX) return -1;
		if (!GetSubNodeValue(data_element, "param", param)) return -2;

		RAShanMiBaoFinishCondRewardCfg node_cfg;

		if (!GetSubNodeValue(data_element, "reward_group", node_cfg.reward_group_id)) return -3;

		switch (condition_id)
		{
			case RA_LOTUS_MI_BAO_CONDITION_TYPE_ROW:
			case RA_LOTUS_MI_BAO_CONDITION_TYPE_COLUMN:
			{
				if(param <= 0 || param > MAX_RA_LOTUS_MI_BAO_ROW_NUM || param > MAX_RA_LOTUS_MI_BAO_COLUMN_NUM) return -4;
				if(m_reward_group_map.find(node_cfg.reward_group_id) == m_reward_group_map.end()) return -5;
			}
			break;
			case RA_LOTUS_MI_BAO_CONDITION_TYPE_ALL:
			{
				if(param != MAX_RA_LOTUS_MI_BAO_GRID_NUM) return -6;
				TiXmlElement * reward_element = data_element->FirstChildElement("fixe_reward");	//二者奖励必须要有一个
				if (NULL != reward_element)
				{
					if (node_cfg.fixe_reward.ReadConfig(reward_element))
					{
						node_cfg.is_has_fixe_reward = true;
					}
				}
				if(!node_cfg.is_has_fixe_reward)
				{
					if (m_reward_group_map.find(node_cfg.reward_group_id) == m_reward_group_map.end()) return -8;
				}
			}
			break;
		default:
			return -9;
		}

		m_finish_cond_reward_map[condition_id][param] = node_cfg;
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}
