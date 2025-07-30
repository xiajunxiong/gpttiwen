#include "randactivitycijiuyingxinconfig.hpp"
#include "servercommon/activitydef.hpp"

RandActivityCiJiuYingXinConfig::RandActivityCiJiuYingXinConfig()
{
}

RandActivityCiJiuYingXinConfig::~RandActivityCiJiuYingXinConfig()
{
}

bool RandActivityCiJiuYingXinConfig::Init(TiXmlElement *RootElement, std::string *err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("question_bank", InitCiJiuYingXinCfg);
	LOAD_RA_CONFIG("gift_configure", InitCiJiuYingXinRewardCfg);

	return true;
}

int RandActivityCiJiuYingXinConfig::RandSubject() const
{
	if (0 >= weight_count)return 0;
	
	int rand_num = rand() % weight_count;
	
	for (std::map<int, RACiJiuYingXinSubjectCfg>::const_iterator it = m_subject_map.begin(); it != m_subject_map.end(); it++)
	{
		if (it->second.weight >= rand_num)
		{
			return it->second.id;
		}
		else
		{
			rand_num -= it->second.weight;
		}
	}
	return 0;
}

const RACiJiuYingXinSubjectCfg * RandActivityCiJiuYingXinConfig::GetCiJiuYingXinSubjectCfg(int id) const
{
	std::map<int, RACiJiuYingXinSubjectCfg>::const_iterator it =  m_subject_map.find(id);
	if (it != m_subject_map.end())
	{
		return &it->second;
	}
	return NULL;
}

const RACiJiuYingXinRewardListCfg * RandActivityCiJiuYingXinConfig::GetCiJiuYingXinRewardListCfg(int id) const
{
	std::map<int, RACiJiuYingXinRewardListCfg>::const_iterator it = m_reward_map.find(id);
	if (it != m_reward_map.end())
	{
		return &it->second;
	}
	return NULL;
}

int RandActivityCiJiuYingXinConfig::InitCiJiuYingXinCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}
	weight_count = 0;
	while (NULL != data_element)
	{
		int question_id = 0;
		if (!GetSubNodeValue(data_element, "question_id", question_id) || question_id < 0) return -1;

		RACiJiuYingXinSubjectCfg &node_cfg = m_subject_map[question_id];
		node_cfg.id = question_id;

		if (!GetSubNodeValue(data_element, "answer_id", node_cfg.answer_id) || node_cfg.answer_id < 0) return -2;
		if (!GetSubNodeValue(data_element, "rate", node_cfg.weight) || node_cfg.weight < 0) return -2;
		weight_count += node_cfg.weight;
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityCiJiuYingXinConfig::InitCiJiuYingXinRewardCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}
	
	while (NULL != data_element)
	{
		int question_id = 0;
		if (!GetSubNodeValue(data_element, "question_id", question_id) || question_id < 0) return -1;

		RACiJiuYingXinRewardListCfg &node_cfg = m_reward_map[question_id];

		TiXmlElement * item_list_element = data_element->FirstChildElement("reward_item_list");
		if (NULL != item_list_element)
		{
			TiXmlElement * item_element = item_list_element->FirstChildElement("reward_item");
			while (NULL != item_element)
			{
				if (!node_cfg.item_list[node_cfg.item_count].ReadConfig(item_element)) return -2;
				node_cfg.item_count += 1;
				item_element = item_element->NextSiblingElement();
			}
		}
		
		data_element = data_element->NextSiblingElement();
	}

	if (m_reward_map.size() != m_subject_map.size())
	{
		return -9999;
	}

	return 0;
}
