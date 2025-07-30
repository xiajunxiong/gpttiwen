#include "randactivitycharismatictanabataconfig.hpp"
#include "global/randactivity/randactivitymanager.hpp"

RandActivityCharismaticTanabataConfig::RandActivityCharismaticTanabataConfig()
{

}


RandActivityCharismaticTanabataConfig::~RandActivityCharismaticTanabataConfig()
{
	std::vector<CharismaticTanabataRewardConfig>().swap(reward_vec);
}

bool RandActivityCharismaticTanabataConfig::Init(TiXmlElement *RootElement, std::string *err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("basis_configuration", InitBaseCfg);
	LOAD_RA_CONFIG("reward_configuration", InitRewardCfg);

	return true;
}

const CharismaticTanabataBaseConfig * RandActivityCharismaticTanabataConfig::GetBaseCfg(int open_day, int reward_type, int seq) const
{
	if (reward_type < 0 || reward_type >= CharismaticTanabataBaseListConfig::BASE_TYPE_MAX)return NULL;
	for (std::map<int, CharismaticTanabataBaseListConfig>::const_iterator it = base_map.begin(); it != base_map.end(); it++)
	{
		int begin_day = 0;
		int end_day = 0;
		this->GetDayDataByKey(it->first, &begin_day, &end_day);
		if (open_day >= begin_day && (end_day == 0 || end_day >= open_day))
		{
			std::map<int, CharismaticTanabataBaseConfig>::const_iterator base_it = it->second.reward_type_map[reward_type].find(seq);
			if (base_it != it->second.reward_type_map[reward_type].end())
			{
				return &base_it->second;
			}
		}
	}
	return NULL;
}

const CharismaticTanabataRewardConfig * RandActivityCharismaticTanabataConfig::GetRewardCfg(int open_day, int rank_idx) const
{
	for (std::vector<CharismaticTanabataRewardConfig>::const_iterator it = reward_vec.begin(); it != reward_vec.end(); it++)
	{
		if (open_day >= it->start_section && (it->end_section == 0 || it->end_section >= open_day))
		{
			if (it->ranking == rank_idx)
			{
				return &(*it);
			}
		}
	}
	return NULL;
}

int RandActivityCharismaticTanabataConfig::InitBaseCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -999;
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

		int key = this->GetKeyByTimeBeginEnd(section_start, section_end);

		CharismaticTanabataBaseListConfig &node = base_map[key];
		node.start_section = section_start;
		node.end_section = section_end;

		int reward_type = -1;
		if (!GetSubNodeValue(data_element, "reward_type", reward_type) || reward_type < CharismaticTanabataBaseListConfig::BASE_TYPE_LOVE || reward_type >= CharismaticTanabataBaseListConfig::BASE_TYPE_MAX) return -__LINE__;
		
		std::map<int, CharismaticTanabataBaseConfig> & map_node = node.reward_type_map[reward_type];
		int seq = -1;
		if (!GetSubNodeValue(data_element, "seq", seq) || seq < 0) return -__LINE__;
		CharismaticTanabataBaseConfig & node_cfg = map_node[seq];

		if (!GetSubNodeValue(data_element, "add_intimate", node_cfg.add_intimate) || node_cfg.add_intimate < 0) return -__LINE__;

		TiXmlElement * reward_list_element = data_element->FirstChildElement("reward_item_list");
		if (NULL != reward_list_element)
		{
			TiXmlElement * reward_element = reward_list_element->FirstChildElement("reward_item");
			while (NULL != reward_element)
			{
				ItemConfigData reward_node;
				if (!reward_node.ReadConfig(reward_element))
				{
					return -__LINE__;
				}
				node_cfg.reward_vec.push_back(reward_node);
				reward_element = reward_element->NextSiblingElement();
			}
		}

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityCharismaticTanabataConfig::InitRewardCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -999;
	}

	while (NULL != data_element)
	{
		CharismaticTanabataRewardConfig node;

		if (!GetSubNodeValue(data_element, "section_start", node.start_section))
		{
			return -1;
		}

		if (!GetSubNodeValue(data_element, "section_end", node.end_section))
		{
			return -2;
		}

		if (!GetSubNodeValue(data_element, "seq", node.seq) || node.seq < 0) return -__LINE__;

		if (!GetSubNodeValue(data_element, "ranking", node.ranking) || node.ranking < 0) return -__LINE__;

		TiXmlElement * reward_list_element = data_element->FirstChildElement("reward_item_list");
		if (NULL != reward_list_element)
		{
			TiXmlElement * reward_element = reward_list_element->FirstChildElement("reward_item");
			while (NULL != reward_element)
			{
				ItemConfigData reward_node;
				if (!reward_node.ReadConfig(reward_element))
				{
					return -__LINE__;
				}
				node.reward_vec.push_back(reward_node);
				reward_element = reward_element->NextSiblingElement();
			}
		}
		reward_vec.push_back(node);
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

void RandActivityCharismaticTanabataConfig::GetDayDataByKey(int key, int * begin_day, int * end_day) const
{
	*begin_day = key / 1000;
	*end_day = key % 1000;
}