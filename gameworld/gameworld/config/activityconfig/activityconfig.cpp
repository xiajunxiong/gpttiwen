#include "activityconfig.hpp"
#include "servercommon/activitydef.hpp"

ActivityConfig::ActivityConfig()
{
}

ActivityConfig::~ActivityConfig()
{
	std::vector<ActivitySignUpCfg>().swap(m_sign_up_cfg);
	for (std::map<int, std::vector<ActivityJoinRewardCfg> >::iterator it = m_join_reward_map.begin(); it != m_join_reward_map.end(); it++)
	{
		std::vector<ActivityJoinRewardCfg>().swap(it->second);
	}
}

bool ActivityConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG("sign_up", InitActivitySignUpCfg);
	LOAD_CONFIG("join_reward", InitActivityJoinRewardCfg);

	return true;
}

const ActivitySignUpCfg * ActivityConfig::GetSignUpCfg(int seq)
{
	if (seq < 0 || seq >= (int)m_sign_up_cfg.size()) return NULL;

	return &m_sign_up_cfg[seq];
}

const ActivitySignUpCfg * ActivityConfig::GetSignUpCfgById(int activity_id)
{
	std::map<int, ActivitySignUpCfg>::iterator it = m_sign_up_by_id_map.find(activity_id);
	if (it != m_sign_up_by_id_map.end()) return &it->second;

	return NULL;
}

const ActivityJoinRewardCfg * ActivityConfig::GetJoinRewardCfg(int _activity_type , int _level)
{
	std::map<int, std::vector<ActivityJoinRewardCfg> >::iterator it = m_join_reward_map.find(_activity_type);
	if (it == m_join_reward_map.end())
		return NULL;

	for (unsigned int i = 0; i < it->second.size(); ++i)
	{
		if(_level < it->second[i].min_level)
			break;

		if (_level >= it->second[i].min_level && _level <= it->second[i].max_level)
			return &it->second[i];
	}

	return NULL;
}

int ActivityConfig::InitActivitySignUpCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}
	while (NULL != data_element)
	{
		ActivitySignUpCfg node_cfg;
		if (!GetSubNodeValue(data_element, "seq", node_cfg.seq) || node_cfg.seq < 0 || node_cfg.activity_id >= MAX_SIGN_UP_NUM) return -1;
		if (!GetSubNodeValue(data_element, "activity_id", node_cfg.activity_id) || node_cfg.activity_id <= ACTIVITY_TYPE_INVALID || node_cfg.activity_id >= ACTIVITY_TYPE_MAX) return -2;
		if (!GetSubNodeValue(data_element, "open", node_cfg.open) || node_cfg.open < 0 || node_cfg.open > 1) return -3;

		TiXmlElement * list_element = data_element->FirstChildElement("reward_item_list");
		if (NULL != list_element)
		{
			TiXmlElement * item_element = list_element->FirstChildElement("reward_item");
			while (NULL != item_element)
			{
				ItemConfigData item;
				if (!item.ReadConfig(item_element)) return -100;

				node_cfg.item_list.push_back(item);
				item_element = item_element->NextSiblingElement();
			}
		}

		m_sign_up_cfg.push_back(node_cfg);
		m_sign_up_by_id_map[node_cfg.activity_id] = node_cfg;
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int ActivityConfig::InitActivityJoinRewardCfg(TiXmlElement *RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -101010;
	}

	int last_seq = -1 , last_min = -1, last_max = -1;
	while (NULL != data_element)
	{
		int activity_type = 0 , seq = 0;
		ActivityJoinRewardCfg temp_cfg;

		if (!GetSubNodeValue(data_element, "seq", seq) || seq < 0)
			return -10001;

		if (last_seq != seq)
		{
			last_min = -1;
			last_max = -1;
		}

		if (!GetSubNodeValue(data_element, "activity_id", activity_type) || activity_type <= ACTIVITY_TYPE_INVALID || activity_type >= ACTIVITY_TYPE_MAX)
			return -1;

		if (!GetSubNodeValue(data_element, "min_level", temp_cfg.min_level) || temp_cfg.min_level <= 0)
			return -2;

		if (!GetSubNodeValue(data_element, "max_level", temp_cfg.max_level) || temp_cfg.max_level <= 0)
			return -3;

		if (temp_cfg.min_level > temp_cfg.max_level)
		{
			return -301;
		}

		if ((-1 != last_min || -1 != last_max) && temp_cfg.min_level <= last_max)
		{
			return -302;
		}

		if (!GetSubNodeValue(data_element, "exp", temp_cfg.exp) || temp_cfg.exp < 0)
			return -4;

		if (!GetSubNodeValue(data_element, "coin", temp_cfg.coin) || temp_cfg.coin < 0)
			return -5;


		TiXmlElement * list_element = data_element->FirstChildElement("reward_item_list");
		if (NULL != list_element)
		{
			TiXmlElement * item_element = list_element->FirstChildElement("reward_item");
			while (NULL != item_element)
			{
				ItemConfigData item;
				if (!item.ReadConfig(item_element)) return -100;

				temp_cfg.reward_vec.push_back(item);
				item_element = item_element->NextSiblingElement();
			}
		}

		last_min = temp_cfg.min_level;
		last_max = temp_cfg.max_level;

		std::map<int, std::vector<ActivityJoinRewardCfg> >::iterator it = m_join_reward_map.find(activity_type);
		if (m_join_reward_map.end() == it)
		{
			std::vector<ActivityJoinRewardCfg> temp_vec;
			temp_vec.push_back(temp_cfg);
			m_join_reward_map.insert(std::make_pair(activity_type, temp_vec));
		}
		else
		{
			it->second.push_back(temp_cfg);
		}

		last_seq = seq;
		last_min = temp_cfg.min_level;
		last_max = temp_cfg.max_level;
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}
