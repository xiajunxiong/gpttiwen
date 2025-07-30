#include "rashanhaijubaoconfig.hpp"
#include "servercommon/maildef.hpp"

RAShanHaiJuBaoConfig::RAShanHaiJuBaoConfig()
{
}

RAShanHaiJuBaoConfig::~RAShanHaiJuBaoConfig()
{
}

bool RAShanHaiJuBaoConfig::Init(TiXmlElement * RootElement, std::string * err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("other", InitOtherCfg);
	LOAD_RA_CONFIG("basis_configuration", InitDrawCfg);
	LOAD_RA_CONFIG("phase_reward", InitExtraRewardCfg);

	return true;
}

const RAShanHaiJuBao::DrawItemCfg * RAShanHaiJuBaoConfig::GetRandDrawCfg(RandActivityManager * ramgr, int phase_seq) const
{
	//活动开启时的开服天数
	const int act_begin_game_open_day 
		= this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_SHAN_HAI_JU_BAO);
	RAShanHaiJuBao::SectionCfgMap::const_iterator section_cit 
		= m_section_map.upper_bound(act_begin_game_open_day);
	if (m_section_map.begin() == section_cit)
	{
		return NULL;
	}
	section_cit--;

	if (0 >= phase_seq
		|| SHAN_HAI_JU_BAO_EXTRA_REWARD_MAX < phase_seq)
	{
		return NULL;
	}

	RAShanHaiJuBao::PhaseDrawPoolMap::const_iterator draw_pool_cit 
		= section_cit->second.phase_draw_pool_map.find(phase_seq);
	if (section_cit->second.phase_draw_pool_map.end() == draw_pool_cit)
	{
		return NULL;
	}

	//当前活动开启天数
	const int act_open_day = this->GetActOpenDay(ramgr, RAND_ACTIVITY_TYPE_SHAN_HAI_JU_BAO);
	if (act_open_day < draw_pool_cit->second.phase_act_day_begin)
	{
		return NULL;
	}
	
	int rand_num = RandomNum(draw_pool_cit->second.power_count);
	RAShanHaiJuBao::DrawPoolPowerMap::const_iterator item_cit
		= draw_pool_cit->second.draw_pool.upper_bound(rand_num);
	if (draw_pool_cit->second.draw_pool.begin() == item_cit)
	{
		return NULL;
	}
	item_cit--;

	return &item_cit->second;
}

const RAShanHaiJuBao::ExtraRewardCfg * RAShanHaiJuBaoConfig::GetExtraRewardCfg(int phase_seq, int extra_reward_index) const
{
	if (0 >= phase_seq
		|| SHAN_HAI_JU_BAO_EXTRA_PHASE_MAX < phase_seq)
	{
		return NULL;
	}

	if (0 > extra_reward_index
		|| m_extra_phase_cfg_[phase_seq].m_extra_reward_count <= extra_reward_index
		|| SHAN_HAI_JU_BAO_EXTRA_REWARD_MAX <= extra_reward_index)
	{
		return NULL;
	}

	return &m_extra_phase_cfg_[phase_seq].m_extra_reward_cfg_[extra_reward_index];
}

int RAShanHaiJuBaoConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -1000;
	}

	if (!GetSubNodeValue(data_element, "consume_item", m_other_cfg.draw_cost_item_id)
		|| NULL == ITEMPOOL->GetItem(m_other_cfg.draw_cost_item_id))
	{
		return -1;
	}

	if (!GetSubNodeValue(data_element, "consume_num", m_other_cfg.cost_num)
		|| 0 >= m_other_cfg.cost_num)
	{
		return -2;
	}

	return 0;
}

int RAShanHaiJuBaoConfig::InitDrawCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -7000;
	}

	while (NULL != data_element)
	{
		RAShanHaiJuBao::DrawItemCfg temp_cfg;

		if (!GetSubNodeValue(data_element, "section_start", temp_cfg.section_day_begin)
			|| 0 >= temp_cfg.section_day_begin)
		{
			return -1;
		}

		if (!GetSubNodeValue(data_element, "phase", temp_cfg.phase_seq)
			|| 0 >= temp_cfg.phase_seq
			|| SHAN_HAI_JU_BAO_EXTRA_REWARD_MAX < temp_cfg.phase_seq)
		{
			return -2;
		}

		if (!GetSubNodeValue(data_element, "activity_time", temp_cfg.phase_act_day_begin)
			|| 0 >= temp_cfg.phase_act_day_begin)
		{
			return -3;
		}

		if (!GetSubNodeValue(data_element, "seq", temp_cfg.draw_index)
			|| 0 > temp_cfg.draw_index)
		{
			return -4;
		}

		{
			TiXmlElement * item_list_element = data_element->FirstChildElement("reward_item");
			if (NULL == item_list_element)
			{
				return -101;
			}

			if (!temp_cfg.draw_item.ReadConfig(item_list_element))
			{
				return -102;
			}
		}

		if (!GetSubNodeValue(data_element, "rate", temp_cfg.draw_power)
			|| 0 >= temp_cfg.draw_power)
		{
			return -5;
		}

		if (!GetSubNodeValue(data_element, "is_borcast", temp_cfg.is_brocast)
			|| 0 > temp_cfg.is_brocast)
		{
			return -6;
		}

		if (!GetSubNodeValue(data_element, "type", temp_cfg.show_type)
			|| 0 > temp_cfg.show_type)
		{
			return -7;
		}

		RAShanHaiJuBao::DrawPool &draw_pool_cfg = m_section_map[temp_cfg.section_day_begin].phase_draw_pool_map[temp_cfg.phase_seq];
		
		if (temp_cfg.draw_index != (int)draw_pool_cfg.draw_pool.size())
		{
			return -11;
		}

		draw_pool_cfg.draw_pool[draw_pool_cfg.power_count] = temp_cfg;
		draw_pool_cfg.power_count += temp_cfg.draw_power;

		if (0 == temp_cfg.draw_index)
		{
			draw_pool_cfg.phase_seq = temp_cfg.phase_seq;
			draw_pool_cfg.phase_act_day_begin = temp_cfg.phase_act_day_begin;
		}
		else
		{
			if (draw_pool_cfg.phase_seq != temp_cfg.phase_seq)
			{
				return -12;
			}

			if (draw_pool_cfg.phase_act_day_begin != temp_cfg.phase_act_day_begin)
			{
				return -13;
			}
		}


		data_element = data_element->NextSiblingElement();
	}
	return 0;
}

int RAShanHaiJuBaoConfig::InitExtraRewardCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -7000;
	}

	while (NULL != data_element)
	{
		int phase = 0;
		if (!GetSubNodeValue(data_element, "phase", phase)
			|| 0 >= phase
			|| SHAN_HAI_JU_BAO_EXTRA_PHASE_MAX < phase)
		{
			return -1;
		}

		RAShanHaiJuBao::ExtraPhaseCfg &phase_cfg = m_extra_phase_cfg_[phase];

		int extra_reward_index = 0;
		if (!GetSubNodeValue(data_element, "reward_index", extra_reward_index)
			|| phase_cfg.m_extra_reward_count != extra_reward_index)
		{
			return -2;
		}

		RAShanHaiJuBao::ExtraRewardCfg & cfg_info = phase_cfg.m_extra_reward_cfg_[extra_reward_index];
		cfg_info.extra_reward_index = extra_reward_index;
		phase_cfg.m_extra_reward_count++;

		if (!GetSubNodeValue(data_element, "lucky_draw_num", cfg_info.need_draw_num)
			|| 0 > cfg_info.need_draw_num)
		{
			return -3;
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

				cfg_info.reward_item_vec.push_back(temp_item);
				item_element = item_element->NextSiblingElement();
			}

			int reward_count = (int)cfg_info.reward_item_vec.size();
			if (0 >= reward_count
				|| MAX_ATTACHMENT_ITEM_NUM <= reward_count)
			{
				return -103;
			}
		}

		data_element = data_element->NextSiblingElement();
	}
	return 0;
}
