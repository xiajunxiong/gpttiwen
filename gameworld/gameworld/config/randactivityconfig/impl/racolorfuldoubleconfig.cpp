#include "racolorfuldoubleconfig.hpp"

RAColorfulDoubleConfig::RAColorfulDoubleConfig()
{
}

RAColorfulDoubleConfig::~RAColorfulDoubleConfig()
{
}

bool RAColorfulDoubleConfig::Init(TiXmlElement * RootElement, std::string * err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("other", InitOtherCfg);
	LOAD_RA_CONFIG("reward", InitFreeRewardCfg);
	LOAD_RA_CONFIG("reward_buy", InitBoxCfg);
	LOAD_RA_CONFIG("task", InitTaskCfg);

	return true;
}

const RAColorfulDouble::FreeRewardCfg * RAColorfulDoubleConfig::GetFreeRewardCfg(RandActivityManager * ramgr) const
{
	const int act_begin_game_open_day
		= this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_COLORFUL_DOUBLE);
	RAColorfulDouble::FreeSectionMap::const_iterator section_cfg_cit 
		= m_free_section_day_map.upper_bound(act_begin_game_open_day);
	if (m_free_section_day_map.begin() == section_cfg_cit)
	{
		return NULL;
	}
	section_cfg_cit--;

	return &section_cfg_cit->second;
}

const RAColorfulDouble::BosRewardCfg * RAColorfulDoubleConfig::GetBoxRewardCfg(RandActivityManager * ramgr, unsigned int act_begin_time, int box_seq) const
{
	RAColorfulDouble::BoxSeqMap::const_iterator box_cit = m_box_seq_map.find(box_seq);
	if (m_box_seq_map.end() == box_cit)
	{
		return NULL;
	}

	const int act_begin_game_open_day
		= this->GetOpenDayByTimestamp(ramgr, act_begin_time);

	RAColorfulDouble::BoxSectionDayMap::const_iterator section_cit
		= box_cit->second.upper_bound(act_begin_game_open_day);
	if (box_cit->second.begin() == section_cit)
	{
		return NULL;
	}
	section_cit--;

	return &section_cit->second;
}

const RAColorfulDouble::TaskVec * RAColorfulDoubleConfig::GetTaskVecByType(RandActivityManager * ramgr, int act_open_day, int task_type) const
{
	RAColorfulDouble::TaskDayMap::const_iterator day_cit = m_task_day_map.find(act_open_day);
	if (m_task_day_map.end() == day_cit)
	{
		return NULL;
	}

	RAColorfulDouble::TaskTypeMap::const_iterator task_vec_cit = day_cit->second.find(task_type);
	if (day_cit->second.end() == task_vec_cit)
	{
		return NULL;
	}

	return &task_vec_cit->second;
}

int RAColorfulDoubleConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -1000;
	}

	if (!GetSubNodeValue(data_element, "buy_type", m_other_cfg.act_type)
		|| 0 >= m_other_cfg.act_type
		|| RAColorfulDouble::OtherCfg::ACT_TYPE_MAX <= m_other_cfg.act_type)
	{
		return -1;
	}

	if (!GetSubNodeValue(data_element, "price_type", m_other_cfg.act_cost_type)
		|| 0 > m_other_cfg.act_cost_type
		|| RAColorfulDouble::OtherCfg::ACT_COST_TYPE_MAX <= m_other_cfg.act_cost_type)
	{
		return -2;
	}

	if (!GetSubNodeValue(data_element, "price", m_other_cfg.act_cost_num)
		|| 0 >= m_other_cfg.act_cost_num)
	{
		return -3;
	}

	if (!GetSubNodeValue(data_element, "cfg_ver", m_other_cfg.cfg_ver)
		|| 0 > m_other_cfg.cfg_ver)
	{
		return -4;
	}

	if (!GetSubNodeValue(data_element, "must_type", m_other_cfg.box_act_reward_type)
		|| 0 > m_other_cfg.box_act_reward_type
		|| RAColorfulDouble::OtherCfg::ACT_REWARD_TYPE_MAX <= m_other_cfg.box_act_reward_type)
	{
		return -5;
	}
	if (!GetSubNodeValue(data_element, "currency_must", m_other_cfg.box_act_reward_num)
		|| 0 > m_other_cfg.box_act_reward_num)
	{
		return -6;
	}

	return 0;
}

int RAColorfulDoubleConfig::InitFreeRewardCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -1000;
	}

	while (NULL != data_element)
	{
		int section_begin_day = 0;
		if (!GetSubNodeValue(data_element, "section_start", section_begin_day)
			|| 0 >= section_begin_day)
		{
			return -1;
		}

		RAColorfulDouble::FreeRewardCfg &cfg_info = m_free_section_day_map[section_begin_day];
		cfg_info.section_day_begin = section_begin_day;
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

				cfg_info.free_reward_vec.push_back(temp_item);
				item_element = item_element->NextSiblingElement();
			}
		}

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RAColorfulDoubleConfig::InitBoxCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -1000;
	}

	while (NULL != data_element)
	{
		int box_seq = 0;
		if (!GetSubNodeValue(data_element, "seq", box_seq)
			|| 0 >= box_seq
			|| RAColorfulDouble::BOX_COUNT < box_seq)
		{
			return -1;
		}

		RAColorfulDouble::BoxSectionDayMap & section_day_map = m_box_seq_map[box_seq];
	
		int section_begin_day = 0;
		if (!GetSubNodeValue(data_element, "section_start", section_begin_day)
			|| 0 >= section_begin_day)
		{
			return -2;
		}
		RAColorfulDouble::BosRewardCfg& cfg_info = section_day_map[section_begin_day];
		cfg_info.box_seq = box_seq;
		cfg_info.section_day_begin = section_begin_day;

		int section_begin_day_index = 0;
		if (!GetSubNodeValue(data_element, "seq1", section_begin_day_index)
			|| (int)section_day_map.size() != section_begin_day_index + 1)
		{
			return -3;
		}
		cfg_info.need_act_open_day = box_seq;

		if (!GetSubNodeValue(data_element, "task", cfg_info.need_finish_task_num)
			|| 0 > cfg_info.need_finish_task_num)
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

				cfg_info.box_reward_vec.push_back(temp_item);
				item_element = item_element->NextSiblingElement();
			}
		}

		data_element = data_element->NextSiblingElement();
	}
	return 0;
}

int RAColorfulDoubleConfig::InitTaskCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -1000;
	}

	while (NULL != data_element)
	{
		int act_day = 0;
		if (!GetSubNodeValue(data_element, "section", act_day)
			|| 0 >= act_day)
		{
			return -1;
		}
		RAColorfulDouble::TaskTypeMap & day_task_type_map = m_task_day_map[act_day];

		RAColorfulDouble::TaskCfg cfg_info;
		cfg_info.act_open_day = act_day;

		if (!GetSubNodeValue(data_element, "seq1", cfg_info.task_index)
			|| 0 > cfg_info.task_index
			|| RAColorfulDouble::DAY_TASK_COUNT <= cfg_info.task_index)
		{
			return -2;
		}

		if (!GetSubNodeValue(data_element, "type", cfg_info.task_type)
			|| 0 >= cfg_info.task_type
			|| RAColorfulDouble::TSAK_TYPE_MAX <= cfg_info.task_type)
		{
			return -3;
		}
		if (!GetSubNodeValue(data_element, "parameter", cfg_info.task_param)
			|| 0 > cfg_info.task_param)
		{
			return -4;
		}

		day_task_type_map[cfg_info.task_type].push_back(cfg_info);
		data_element = data_element->NextSiblingElement();
	}
	return 0;
}
