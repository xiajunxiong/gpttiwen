#include "rashanhaimetconfig.hpp"
#include "servercommon/maildef.hpp"

RAShanHaiMetConfig::RAShanHaiMetConfig()
{
}

RAShanHaiMetConfig::~RAShanHaiMetConfig()
{
}

bool RAShanHaiMetConfig::Init(TiXmlElement * RootElement, std::string * err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("other", InitOtherCfg);
	LOAD_RA_CONFIG("task_list", InitTaskGroupCfg);
	LOAD_RA_CONFIG("basis_configuration", InitDailyCfg);

	return true;
}

const RAShanHaiMet::DailyCfg * RAShanHaiMetConfig::GetDailyCfg(RandActivityManager * ramgr, int act_day_index) const
{
	//活动开启时的开服天数
	const int act_begin_game_open_day
		= this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_SHAN_HAI_MET);
	RAShanHaiMet::SectionMap::const_iterator section_cit
		= m_section_map.upper_bound(act_begin_game_open_day);
	if (m_section_map.begin() == section_cit)
	{
		return NULL;
	}
	section_cit--;

	RAShanHaiMet::SectionCfgDayMap::const_iterator daily_cfg_cit 
		= section_cit->second.find(act_day_index);
	if (section_cit->second.end() == daily_cfg_cit)
	{
		return NULL;
	}

	return &daily_cfg_cit->second;
}

int RAShanHaiMetConfig::GetConfigVer() const
{
	return m_other_cfg.cfg_ver;
}

const RAShanHaiMet::TaskGroupCfg * RAShanHaiMetConfig::GetTaskGroupCfg(int task_group_id) const
{
	RAShanHaiMet::TaskGroupIDMap::const_iterator task_group_cit 
		= m_task_group_id_cfg.find(task_group_id);
	if (m_task_group_id_cfg.end() == task_group_cit)
	{
		return NULL;
	}

	return &task_group_cit->second;
}

int RAShanHaiMetConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -1000;
	}

	if (!GetSubNodeValue(data_element, "buy_money", m_other_cfg.high_reward_buy_money)
		|| 0 >= m_other_cfg.high_reward_buy_money)
	{
		return -1;
	}

	if (!GetSubNodeValue(data_element, "cfg_ver", m_other_cfg.cfg_ver)
		|| 0 >= m_other_cfg.cfg_ver)
	{
		return -2;
	}

	return 0;
}

int RAShanHaiMetConfig::InitTaskGroupCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -7000;
	}

	while (NULL != data_element)
	{
		int task_group_id = 0;
		if (!GetSubNodeValue(data_element, "task_group", task_group_id)
			|| 0 >= task_group_id)
		{
			return -1;
		}

		int task_seq = 0;
		if (!GetSubNodeValue(data_element, "task_id", task_seq)
			|| 0 >= task_seq
			|| (int)m_task_group_id_cfg[task_group_id].size() + 1 != task_seq)
		{
			return -2;
		}

		RAShanHaiMet::TaskCfg & cfg_info = m_task_group_id_cfg[task_group_id][task_seq];
		cfg_info.task_group_id = task_group_id;
		cfg_info.task_seq = task_seq;

		if (!GetSubNodeValue(data_element, "task_type", cfg_info.task_type)
			|| 0 > cfg_info.task_type
			|| RAShanHaiMet::TSAK_TYPE_MAX <= cfg_info.task_type)
		{
			return -3;
		}

		if (!GetSubNodeValue(data_element, "param_1", cfg_info.task_param_1))
		{
			return -4;
		}

		if (!GetSubNodeValue(data_element, "param_2", cfg_info.task_param_2))
		{
			return -5;
		}

		data_element = data_element->NextSiblingElement();
	}
	return 0;
}

int RAShanHaiMetConfig::InitDailyCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -7000;
	}

	while (NULL != data_element)
	{
		int section_day_begin = 0;
		if (!GetSubNodeValue(data_element, "section_start", section_day_begin)
			|| 0 >= section_day_begin)
		{
			return -1;
		}

		int act_day_index = 0;
		if (!GetSubNodeValue(data_element, "seq", act_day_index)
			|| 0 > act_day_index
			|| (int)m_section_map[section_day_begin].size() != act_day_index)
		{
			return -2;
		}

		int act_days = 0;
		if (!GetSubNodeValue(data_element, "activity_days", act_days)
			|| act_day_index + 1 != act_days)
		{
			return -3;
		}

		RAShanHaiMet::DailyCfg & cfg_info = m_section_map[section_day_begin][act_day_index];
		cfg_info.section_day_begin = section_day_begin;
		cfg_info.act_day_index = act_day_index;

		if (!GetSubNodeValue(data_element, "task_group", cfg_info.task_group_id)
			|| NULL == this->GetTaskGroupCfg(cfg_info.task_group_id))
		{
			return -4;
		}

		{
			TiXmlElement * item_list_element = data_element->FirstChildElement("reward_item_list");
			if (NULL == item_list_element)
			{
				return -101;
			}
			TiXmlElement * item_element = item_list_element->FirstChildElement("reward_item");
			while (NULL != item_element)
			{
				ItemConfigData temp_item;
				if (!temp_item.ReadConfig(item_element))
				{
					return -102;
				}

				cfg_info.normal_reward_vec.push_back(temp_item);
				item_element = item_element->NextSiblingElement();
			}

			int reward_count = (int)cfg_info.normal_reward_vec.size();
			if (0 >= reward_count
				|| MAX_ATTACHMENT_ITEM_NUM <= reward_count)
			{
				return -103;
			}
		}

		{
			TiXmlElement * item_list_element = data_element->FirstChildElement("unfinish_reward_item_list");
			if (NULL == item_list_element)
			{
				return -201;
			}
			TiXmlElement * item_element = item_list_element->FirstChildElement("unfinish_reward_item");
			while (NULL != item_element)
			{
				ItemConfigData temp_item;
				if (!temp_item.ReadConfig(item_element))
				{
					return -202;
				}

				cfg_info.unfinish_reward_vec.push_back(temp_item);
				item_element = item_element->NextSiblingElement();
			}

			int reward_count = (int)cfg_info.unfinish_reward_vec.size();
			if (0 >= reward_count
				|| MAX_ATTACHMENT_ITEM_NUM <= reward_count)
			{
				return -203;
			}
		}

		{
			TiXmlElement * item_list_element = data_element->FirstChildElement("double_reward_item_list");
			if (NULL == item_list_element)
			{
				return -301;
			}
			TiXmlElement * item_element = item_list_element->FirstChildElement("double_reward_item");
			while (NULL != item_element)
			{
				ItemConfigData temp_item;
				if (!temp_item.ReadConfig(item_element))
				{
					return -302;
				}

				cfg_info.extra_reward_vec.push_back(temp_item);
				item_element = item_element->NextSiblingElement();
			}

			int reward_count = (int)cfg_info.extra_reward_vec.size();
			if (0 >= reward_count
				|| MAX_ATTACHMENT_ITEM_NUM <= reward_count)
			{
				return -303;
			}
		}

		data_element = data_element->NextSiblingElement();
	}
	return 0;
}
