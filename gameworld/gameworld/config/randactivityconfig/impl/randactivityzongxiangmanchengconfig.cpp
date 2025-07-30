#include "randactivityzongxiangmanchengconfig.hpp"

#include <algorithm>
#include "servercommon/roleactivity/razongxiangparam.hpp"

RandActivityZongXiangManChengConfig::RandActivityZongXiangManChengConfig()
{
}

RandActivityZongXiangManChengConfig::~RandActivityZongXiangManChengConfig()
{
	std::vector<ZongXiangManChengBaseConfig>().swap(m_base_vec);
	std::vector<ZongXiangRewardConfig>().swap(m_reward_vec);
}

bool RandActivityZongXiangManChengConfig::Init(TiXmlElement * RootElement, std::string * err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("basis_configuration", InitBaseCfg);
	LOAD_RA_CONFIG("reward_group", InitRewardCfg);

	return true;
}

const ZongXiangManChengBaseConfig * RandActivityZongXiangManChengConfig::GetBaseCfg(int open_day, int seq) const
{
	for (std::vector<ZongXiangManChengBaseConfig>::const_iterator it = m_base_vec.begin(); it != m_base_vec.end(); it++)
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

const ZongXiangRewardConfig * RandActivityZongXiangManChengConfig::GetRewardCfg(int reward_id) const
{
	std::vector<const ZongXiangRewardConfig*> rand_vec;
	for (std::vector<ZongXiangRewardConfig>::const_iterator it = m_reward_vec.begin(); it != m_reward_vec.end(); it++)
	{
		if (it->reward_group == reward_id)
		{
			rand_vec.push_back(&(*it));
		}
	}
	if ( (int)rand_vec.size() > 0)
	{
		int rand_idx = rand() % (int)rand_vec.size();
		return rand_vec[rand_idx];
	}
	return NULL;
}

int RandActivityZongXiangManChengConfig::InitBaseCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element) return -544;

	while (NULL != data_element)
	{
		ZongXiangManChengBaseConfig node;

		if (!GetSubNodeValue(data_element, "type", node.type) || node.type < 0) return -1;
		if (!GetSubNodeValue(data_element, "section_start", node.section_start)) return -2;
		if (!GetSubNodeValue(data_element, "section_end", node.section_end)) return -3;
		if (node.section_end == 0)
		{
			node.section_end = MAX_INT;
		}
		if (!GetSubNodeValue(data_element, "seq", node.seq) || node.seq >= MAX_ZONG_XIANG_MAN_CHENG_FLAG) return -4;
		if (!GetSubNodeValue(data_element, "light_type", node.light_type) || node.light_type <= ZongXiangManChengBaseConfig::ZONG_XIANG_TYPE_NO ||
			node.light_type > ZongXiangManChengBaseConfig::ZONG_XIANG_TYPE_2) return -5;
		if (!GetSubNodeValue(data_element, "light_condition", node.light_condition) || node.light_condition < ZongXiangManChengBaseConfig::ZONG_XIANG_CONDITION_0 ||
			node.light_condition > ZongXiangManChengBaseConfig::ZONG_XIANG_CONDITION_1) return -6;
		if (!GetSubNodeValue(data_element, "parameter", node.parameter) || node.parameter < 0) return -7;
		if (!GetSubNodeValue(data_element, "reward_group", node.reward_group) || node.reward_group < 0) return -8;

		m_base_vec.push_back(node);
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityZongXiangManChengConfig::InitRewardCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element) return -544;

	while (NULL != data_element)
	{
		ZongXiangRewardConfig node;
		
		if (!GetSubNodeValue(data_element, "reward_group", node.reward_group))
		{
			return -1;
		}
		if (!GetSubNodeValue(data_element, "rate", node.rate))
		{
			return -2;
		}

		TiXmlElement* reward_list_element = data_element->FirstChildElement("reward_item_list");
		if (NULL != reward_list_element)
		{
			TiXmlElement * reward_element = reward_list_element->FirstChildElement("reward_item");
			while (NULL != reward_element)
			{
				ItemConfigData item;
				if (!item.ReadConfig(reward_element)) return -2;

				node.m_reward_vec.push_back(item);
				reward_element = reward_element->NextSiblingElement();
			}
		}
		m_reward_vec.push_back(node);
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}