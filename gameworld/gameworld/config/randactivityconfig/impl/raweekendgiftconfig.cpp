#include "raweekendgiftconfig.hpp"

RAWeekendGiftConfig::RAWeekendGiftConfig()
{
}

RAWeekendGiftConfig::~RAWeekendGiftConfig()
{
	std::vector<RAWeekendGift::GiftCfg>().swap(m_gift_cfg_vec);
}

bool RAWeekendGiftConfig::Init(TiXmlElement * RootElement, std::string * err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("other", InitOtherCfg);
	LOAD_RA_CONFIG("reward_group", InitGiftCfg);
	LOAD_RA_CONFIG("gift_weekend", InitPhaseCfg);

	return true;
}

const int RAWeekendGiftConfig::GetRandGiftIndex(RandActivityManager * ramgr, int role_level, int chong_zhi_value) const
{
	//开服天数阶段
	const int now_game_open_day = this->GetCurRealOpenDay(ramgr);
	RAWeekendGift::SectionDayMap::const_iterator section_day_cit
		= m_section_day_cfg.upper_bound(now_game_open_day);
	if (m_section_day_cfg.begin() == section_day_cit)
	{
		return -1;
	}
	section_day_cit--;

	//角色等级阶段
	RAWeekendGift::SectionLevelMap::const_iterator level_cit
		= section_day_cit->second.upper_bound(role_level);
	if (section_day_cit->second.begin() == level_cit)
	{
		return -1;
	}
	level_cit--;

	//符合充值金额的奖励组
	const RAWeekendGift::PhaseCfgVec & phase_cfg_vec = level_cit->second;
	int phase_cfg_vec_size = phase_cfg_vec.size();
	int group_id = 0;
	for (int i = phase_cfg_vec_size; i > 0; i--)
	{//取满足充值条件的第一个奖励组
		if (phase_cfg_vec[i - 1].chong_zhi_min > chong_zhi_value
			|| phase_cfg_vec[i - 1].chong_zhi_max < chong_zhi_value)
		{
			continue;
		}
		group_id = phase_cfg_vec[i - 1].gift_group_id;
		break;
	}

	RAWeekendGift::GroupIDMap::const_iterator group_cit = m_group_id_map.find(group_id);
	if (m_group_id_map.end() == group_cit)
	{
		return -1;
	}

	//随机出礼包index
	const RAWeekendGift::GroupCfg & group_cfg = group_cit->second;
	int rand_num = RandomNum(group_cfg.total_power);
	RAWeekendGift::GroupCfg::RandPool::const_iterator gift_index_cit
		= group_cfg.gift_index_rand_pool.upper_bound(rand_num);
	if (group_cfg.gift_index_rand_pool.begin() == gift_index_cit)
	{
		return -1;
	}
	gift_index_cit--;

	return gift_index_cit->second;
}

const RAWeekendGift::GiftCfg * RAWeekendGiftConfig::GetGiftCfg(int gift_type_index) const
{
	if (0 > gift_type_index
		|| (int)m_gift_cfg_vec.size() <= gift_type_index)
	{
		return NULL;
	}

	return &m_gift_cfg_vec[gift_type_index];
}

int RAWeekendGiftConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	if (!GetSubNodeValue(root_element, "duration_time", m_other_cfg.duration_time) || m_other_cfg.duration_time <= 0)
	{
		return -1;
	}
	if (!GetSubNodeValue(root_element, "cfg_ver_1", m_other_cfg.cfg_ver) || m_other_cfg.cfg_ver < 0)
	{
		return -2;
	}
	if (!GetSubNodeValue(root_element, "is_weekend_open", m_other_cfg.is_weekend_gift_open)
		|| 0 > m_other_cfg.is_weekend_gift_open
		|| 1 < m_other_cfg.is_weekend_gift_open)
	{
		return -3;
	}

	return 0;
}

int RAWeekendGiftConfig::InitGiftCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -777;
	}

	while (NULL != data_element)
	{
		RAWeekendGift::GiftCfg cfg_info;
		if (!GetSubNodeValue(data_element, "type", cfg_info.gift_type_index)
			|| (int)m_gift_cfg_vec.size() != cfg_info.gift_type_index)
		{
			return -1;
		}

		if (!GetSubNodeValue(data_element, "limit_num", cfg_info.limit_num)
			|| 0 > cfg_info.limit_num)
		{
			return -3;
		}
		if (!GetSubNodeValue(data_element, "buy_money", cfg_info.buy_money)
			|| 0 > cfg_info.buy_money)
		{
			return -4;
		}

		{
			TiXmlElement * item_list_element = data_element->FirstChildElement("reward_item_list");
			if (NULL != item_list_element)
			{
				TiXmlElement * item_element = item_list_element->FirstChildElement("reward_item");
				while (NULL != item_element)
				{
					ItemConfigData item;
					if (!item.ReadConfig(item_element))
					{
						return -104;
					}

					cfg_info.item_vec.push_back(item);
					item_element = item_element->NextSiblingElement();
				}
			}
		}

		m_gift_cfg_vec.push_back(cfg_info);

		int group_id = 0;
		if (!GetSubNodeValue(data_element, "group_id", group_id))
		{
			return -101;
		}
		int power = 0;
		if (!GetSubNodeValue(data_element, "rate", power)
			|| 0 >= power)
		{
			return -102;
		}

		RAWeekendGift::GroupCfg &group_cfg = m_group_id_map[group_id];
		group_cfg.group_id = group_id;
		group_cfg.gift_index_rand_pool[group_cfg.total_power] = cfg_info.gift_type_index;
		group_cfg.total_power += power;

		data_element = data_element->NextSiblingElement();
	}
	return 0;
}

int RAWeekendGiftConfig::InitPhaseCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -777;
	}

	while (NULL != data_element)
	{
		RAWeekendGift::PhaseCfg cfg_info;
		if (!GetSubNodeValue(data_element, "section_start", cfg_info.day_section_begin)
			|| 0 > cfg_info.day_section_begin)
		{
			return -1;
		}
		if (!GetSubNodeValue(data_element, "grade_num", cfg_info.role_level_section_begin)
			|| 0 > cfg_info.role_level_section_begin)
		{
			return -2;
		}
		if (!GetSubNodeValue(data_element, "recharge_down", cfg_info.chong_zhi_min)
			|| 0 > cfg_info.chong_zhi_min)
		{
			return -3;
		}
		if (!GetSubNodeValue(data_element, "recharge_up", cfg_info.chong_zhi_max)
			|| cfg_info.chong_zhi_min > cfg_info.chong_zhi_max)
		{
			return -4;
		}
		if (!GetSubNodeValue(data_element, "group_id", cfg_info.gift_group_id)
			|| NULL == GetGiftCfg(cfg_info.gift_group_id))
		{
			return -5;
		}

		m_section_day_cfg[cfg_info.day_section_begin][cfg_info.role_level_section_begin].push_back(cfg_info);

		data_element = data_element->NextSiblingElement();
	}
	return 0;
}
