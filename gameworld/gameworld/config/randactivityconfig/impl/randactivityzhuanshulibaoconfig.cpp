#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "randactivityzhuanshulibaoconfig.hpp"

RandActivityZhuanShuLiBaoConfig::RandActivityZhuanShuLiBaoConfig()
{

}

RandActivityZhuanShuLiBaoConfig::~RandActivityZhuanShuLiBaoConfig()
{
	for (std::map<int, std::vector<RAZhuanShuLiBaoItemCfg> >::iterator it = m_item_cfg.begin(); it != m_item_cfg.end(); it++)
	{
		std::vector<RAZhuanShuLiBaoItemCfg>().swap(it->second);
	}
}

bool RandActivityZhuanShuLiBaoConfig::Init(TiXmlElement * RootElement, std::string * err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("gift_configure", InitBuyCfg);
	LOAD_RA_CONFIG("other", InitOtherCfg);
	LOAD_RA_CONFIG("gift_day", InitDayGiftCfg);
	LOAD_RA_CONFIG("gift_day_phase", InitDayPhaseCfg);

	return true;
}

std::vector<const RAZhuanShuLiBaoItemCfg *> RandActivityZhuanShuLiBaoConfig::GetRAZhuanShuLiBaoItemCfg(int min_role_level, int max_role_level, int chong_zhi_value) const
{
	std::vector<const RAZhuanShuLiBaoItemCfg *> ret;

	for (std::map<int, std::vector<RAZhuanShuLiBaoItemCfg> >::const_iterator iter = m_item_cfg.begin(); iter != m_item_cfg.end(); ++iter)
	{
		if (iter->first <= min_role_level || iter->first > max_role_level)
		{
			continue;
		}

		bool is_get = false;
		for (int i = 0; i < (int)iter->second.size(); ++i)
		{
			const RAZhuanShuLiBaoItemCfg & curr = iter->second[i];
			if (curr.chongzhi_min <= chong_zhi_value && chong_zhi_value <= curr.chongzhi_max)
			{
				is_get = true;
				ret.push_back(&curr);
				break;
			}
		}

		if (!is_get && !iter->second.empty())
		{
			ret.push_back(&iter->second.back()); //wiii
		}
	}

	return ret;
}

const RAZhuanShuLiBaoItemCfg * RandActivityZhuanShuLiBaoConfig::GetRAZhuanShuLiBaoItemCfg(int type) const
{
	for (std::map<int, std::vector<RAZhuanShuLiBaoItemCfg> >::const_iterator iter = m_item_cfg.begin(); iter != m_item_cfg.end(); ++iter)
	{
		for (int i = 0; i < (int)iter->second.size(); ++i)
		{
			const RAZhuanShuLiBaoItemCfg & curr = iter->second[i];
			if (curr.type == type)
			{
				return &curr;
			}
		}
	}

	return NULL;
}

const RAZhuanShuLiBaoOtherCfg & RandActivityZhuanShuLiBaoConfig::GetOtherCfg() const
{
	return m_other_cfg;
}

bool RandActivityZhuanShuLiBaoConfig::IsExceedMaxDay(int phase, int login_day) const
{
	std::map<int, std::map<int, RAZhuanShuLiBaoDayCfg> >::const_iterator it = m_day_gift_cfg.find(phase);
	if (it == m_day_gift_cfg.end() || it->second.empty())
	{
		return false;
	}

	return login_day > (int)it->second.size();
}

const RAZhuanShuLiBaoDayCfg * RandActivityZhuanShuLiBaoConfig::GetDayGiftCfgByLogin(int phase, int login_day) const
{
	std::map<int, std::map<int, RAZhuanShuLiBaoDayCfg> >::const_iterator it = m_day_gift_cfg.find(phase);
	if (it == m_day_gift_cfg.end() || it->second.empty())
	{
		return NULL;
	}
	for (std::map<int, RAZhuanShuLiBaoDayCfg>::const_reverse_iterator type_it = it->second.rbegin(); type_it != it->second.rend(); ++type_it)
	{
		if (login_day > type_it->first)
		{
			return &type_it->second;
		}
	}

	return NULL;
}

const RAZhuanShuLiBaoDayCfg * RandActivityZhuanShuLiBaoConfig::GetDayGiftCfg(int phase, int type)const
{
	std::map<int, std::map<int, RAZhuanShuLiBaoDayCfg> >::const_iterator it = m_day_gift_cfg.find(phase);
	if (it == m_day_gift_cfg.end() || it->second.empty())
	{
		return NULL;
	}
	std::map<int, RAZhuanShuLiBaoDayCfg>::const_iterator type_it = it->second.find(type);
	if(type_it == it->second.end()) return NULL;

	return &type_it->second;
}

int RandActivityZhuanShuLiBaoConfig::GetDayPhaseByLevel(int role_level)const
{
	for (std::map<int, int>::const_iterator it = m_day_gift_level_map.begin(); it != m_day_gift_level_map.end(); ++it)
	{
		if (role_level >= it->first / 1000 && role_level <= it->first % 1000)
		{
			return it ->second;
		}
	}
	return 0;
}

int RandActivityZhuanShuLiBaoConfig::GetConfigVer() const
{
	return m_other_cfg.cfg_ver;
}

int RandActivityZhuanShuLiBaoConfig::InitBuyCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}

	int next_type = 0;
	while (NULL != data_element)
	{
		RAZhuanShuLiBaoItemCfg item_cfg;
		if (!GetSubNodeValue(data_element, "grade_num", item_cfg.role_level)) return -__LINE__;
		if (!GetSubNodeValue(data_element, "type", item_cfg.type) || item_cfg.type != next_type) return -__LINE__;
		if (!GetSubNodeValue(data_element, "limit_num", item_cfg.limit_buy_times) || item_cfg.limit_buy_times < 0) return -__LINE__;
		if (!GetSubNodeValue(data_element, "buy_money", item_cfg.need_chong_zhi_gold) || item_cfg.need_chong_zhi_gold < 0) return -__LINE__;
		if (!GetSubNodeValue(data_element, "gold_num", item_cfg.add_gold)) return -__LINE__;
		if (!GetSubNodeValue(data_element, "recharge_down", item_cfg.chongzhi_min)) return -__LINE__;
		if (!GetSubNodeValue(data_element, "recharge_up", item_cfg.chongzhi_max)) return -__LINE__;

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

				item_cfg.rewards.push_back(item);
				item_element = item_element->NextSiblingElement();
			}
		}

		next_type += 1;
		m_item_cfg[item_cfg.role_level].push_back(item_cfg);
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityZhuanShuLiBaoConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	if (!GetSubNodeValue(root_element, "duration_time", m_other_cfg.duration_time) || m_other_cfg.duration_time <= 0)
	{
		return -__LINE__;
	}
	if (!GetSubNodeValue(root_element, "cfg_ver", m_other_cfg.cfg_ver) || m_other_cfg.cfg_ver < 0)
	{
		return -__LINE__;
	}
	if (!GetSubNodeValue(root_element, "is_weekend_open", m_other_cfg.is_weekend_gift_open)
		|| 0 > m_other_cfg.is_weekend_gift_open
		|| 1 < m_other_cfg.is_weekend_gift_open)
	{
		return -3;
	}

	return 0;
}

int RandActivityZhuanShuLiBaoConfig::InitDayGiftCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}
	while (NULL != data_element)
	{
		RAZhuanShuLiBaoDayCfg node_cfg;
		if (!GetSubNodeValue(data_element, "phase", node_cfg.phase)) return -1;
		if (!GetSubNodeValue(data_element, "type", node_cfg.type)) return -2;
		if (m_day_phase_set.end() != m_day_phase_set.find(node_cfg.phase))
		{
			return-3;
		}
		std::map<int, std::map<int, RAZhuanShuLiBaoDayCfg> >::iterator it = m_day_gift_cfg.find(node_cfg.phase);
		if (it != m_day_gift_cfg.end() && it->second.end() != it->second.find(node_cfg.type))
		{
			return -4;
		}
		if (!GetSubNodeValue(data_element, "gold_num", node_cfg.gold_num) || node_cfg.gold_num < 0) return -5;
		if (!GetSubNodeValue(data_element, "limit_num", node_cfg.limit_num) || node_cfg.limit_num <= 0) return -6;
		if (!GetSubNodeValue(data_element, "buy_money", node_cfg.buy_money) || node_cfg.buy_money <= 0) return -7;

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

				node_cfg.item_list.push_back(item);
				item_element = item_element->NextSiblingElement();
			}
		}
		m_day_gift_cfg[node_cfg.phase][node_cfg.type] = node_cfg;
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityZhuanShuLiBaoConfig::InitDayPhaseCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}
	while (NULL != data_element)
	{
		int min_level = 0, max_level = 0;
		if (!GetSubNodeValue(data_element, "grade_open", min_level) || min_level <= 0 || min_level > MAX_ROLE_LEVEL) return -1;
		if (!GetSubNodeValue(data_element, "grade_end", max_level) || max_level < 0) return -2;

		if (0 == max_level)
		{
			max_level = MAX_ROLE_LEVEL;
		}
		int phase = 0;
		if (!GetSubNodeValue(data_element, "phase", phase) || phase <= 0 || m_day_phase_set.find(phase) != m_day_phase_set.end()) return -3;

		int level_key = min_level * 1000 + max_level;
		m_day_gift_level_map[level_key] = phase;
		m_day_phase_set.insert(phase);
		data_element = data_element->NextSiblingElement();
	}
	
	return 0;
}
