#include "config/randactivityconfig/randactivityconfigmanager.hpp"

#include "randactivityyuanqiyanhuoconfig.hpp"

RandActivityYuanQiYanHuoConfig::RandActivityYuanQiYanHuoConfig()
{
}

RandActivityYuanQiYanHuoConfig::~RandActivityYuanQiYanHuoConfig()
{
	std::vector<RandActivityYuanQiYanHuoSectionCfg>().swap(m_section_cfg);
	std::vector<RandActivityYuanQiYanHuoYanHuoRewardSectionCfg>().swap(m_yan_huo_section_cfg);
}

bool RandActivityYuanQiYanHuoConfig::Init(TiXmlElement * RootElement, std::string * err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("reward", InitRewardCfg);
	LOAD_RA_CONFIG("other", InitOtherfg);
	LOAD_RA_CONFIG("reward_yanhuo", InitYanHuoRewardCfg);

	return true;
}

const RandActivityYuanQiYanHuoItemCfg * RandActivityYuanQiYanHuoConfig::GetRandActivityYuanQiYuanHuoItemCfg(RandActivityManager * ramgr, int seq) const
{
	int act_real_open_day = LOGIC_CONFIG->GetRandActivityConfigManager()->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_YUAN_QI_YAN_HUO);
	for (std::vector<RandActivityYuanQiYanHuoSectionCfg>::const_iterator it = m_section_cfg.begin(); it != m_section_cfg.end(); it++)
	{
		if (it->section_start <= act_real_open_day && it->section_end >= act_real_open_day)
		{
			return MapValuesConstPtr(it->item_list, seq);
		}
	}

	return NULL;
}

const RandActivityYuanQiYanHuoOtherCfg & RandActivityYuanQiYanHuoConfig::GetOtherCfg() const
{
	return m_other_cfg;
}

const RandActivityYuanQiYanHuoSectionCfg * RandActivityYuanQiYanHuoConfig::GetRandActivityYuanQiYuanHuoSectionCfg(RandActivityManager * ramgr, time_t time) const
{
	int act_real_open_day = LOGIC_CONFIG->GetRandActivityConfigManager()->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_YUAN_QI_YAN_HUO);
	if (time > 0)
	{
		act_real_open_day = this->GetOpenDayByTimestamp(ramgr, time);
	}

	for (std::vector<RandActivityYuanQiYanHuoSectionCfg>::const_iterator it = m_section_cfg.begin(); it != m_section_cfg.end(); it++)
	{
		if (it->section_start <= act_real_open_day && it->section_end >= act_real_open_day)
		{
			return &(*it);
		}
	}

	return NULL;
}

const RandActivityYuanQiYanHuoYanHuoRewardItemCfg * RandActivityYuanQiYanHuoConfig::GetRandActivityYuanQiYanHuoYanHuoRewardItemCfg(RandActivityManager * ramgr) const
{
	int act_real_open_day = LOGIC_CONFIG->GetRandActivityConfigManager()->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_YUAN_QI_YAN_HUO);
	for (std::vector<RandActivityYuanQiYanHuoYanHuoRewardSectionCfg>::const_iterator it = m_yan_huo_section_cfg.begin(); it != m_yan_huo_section_cfg.end(); it++)
	{
		if (it->section_start <= act_real_open_day && it->section_end >= act_real_open_day)
		{
			int r = RandomNum(it->rate_count);
			for (int i = 0; i < (int)it->item_list.size(); ++i)
			{
				const RandActivityYuanQiYanHuoYanHuoRewardItemCfg & curr = it->item_list[i];
				if (r<curr.rate)
				{
					return &curr;
				}

				r -= curr.rate;
			}
		}
	}

	return NULL;
}

int RandActivityYuanQiYanHuoConfig::GetConfigVer() const
{
	return m_other_cfg.cfg_ver;
}

int RandActivityYuanQiYanHuoConfig::InitRewardCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}

	int next_seq = 0;
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

		if (m_section_cfg.size() > 0)
		{
			int pre_index = m_section_cfg.size() - 1;
			if (section_start != m_section_cfg[pre_index].section_start ||
				section_end != m_section_cfg[pre_index].section_end)
			{
				if (section_start < m_section_cfg[pre_index].section_end)
				{
					return -888;
				}

				next_seq = 0;
				m_section_cfg.push_back(RandActivityYuanQiYanHuoSectionCfg());
			}
		}
		if (m_section_cfg.size() == 0)
		{
			if (section_start != 1)
			{
				return -11111;
			}
			m_section_cfg.push_back(RandActivityYuanQiYanHuoSectionCfg());
		}

		RandActivityYuanQiYanHuoSectionCfg & node_cfg = m_section_cfg[m_section_cfg.size() - 1];
		node_cfg.section_start = section_start;
		node_cfg.section_end = section_end;

		RandActivityYuanQiYanHuoItemCfg item_cfg;
		if (!GetSubNodeValue(data_element, "seq", item_cfg.seq)) return -__LINE__;
		if (ReadItemConfigDataList(data_element, "reward_item", item_cfg.rewards) != 0)return -__LINE__;

		next_seq += 1;

		node_cfg.item_list[item_cfg.seq] = item_cfg;
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityYuanQiYanHuoConfig::InitOtherfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -544;
	}

	if (!GetSubNodeValue(data_element, "price", m_other_cfg.need_chong_zhi_gold)) return -__LINE__;
	if (ReadItemConfigData2(data_element, "reward_item", m_other_cfg.buy_rewards) != 0)return -__LINE__;
	if (ReadItemConfigData2(data_element, "reward1_item", m_other_cfg.free_rewards) != 0)return -__LINE__;
	if (!GetSubNodeValue(data_element, "item_id", m_other_cfg.yuan_huo_item_id)) return -__LINE__;
	if (!GetSubNodeValue(data_element, "time_start", m_other_cfg.yuan_huo_item_open_begin_time)) return -__LINE__;
	if (!GetSubNodeValue(data_element, "time_end", m_other_cfg.yuan_huo_item_open_end_time)) return -__LINE__;
	if (!GetSubNodeValue(data_element, "cfg_ver", m_other_cfg.cfg_ver)) return -__LINE__;

	return 0;
}

int RandActivityYuanQiYanHuoConfig::InitYanHuoRewardCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}

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

		if (m_yan_huo_section_cfg.size() > 0)
		{
			int pre_index = m_yan_huo_section_cfg.size() - 1;
			if (section_start != m_yan_huo_section_cfg[pre_index].section_start ||
				section_end != m_yan_huo_section_cfg[pre_index].section_end)
			{
				if (section_start < m_yan_huo_section_cfg[pre_index].section_end)
				{
					return -888;
				}


				m_yan_huo_section_cfg.push_back(RandActivityYuanQiYanHuoYanHuoRewardSectionCfg());
			}
		}

		if (m_yan_huo_section_cfg.size() == 0)
		{
			if (section_start != 1)
			{
				return -11111;
			}
			m_yan_huo_section_cfg.push_back(RandActivityYuanQiYanHuoYanHuoRewardSectionCfg());
		}

		RandActivityYuanQiYanHuoYanHuoRewardSectionCfg & node_cfg = m_yan_huo_section_cfg[m_yan_huo_section_cfg.size() - 1];
		node_cfg.section_start = section_start;
		node_cfg.section_end = section_end;

		RandActivityYuanQiYanHuoYanHuoRewardItemCfg item_cfg;
		if (!GetSubNodeValue(data_element, "rate", item_cfg.rate) || item_cfg.rate <= 0) return -__LINE__;
		if (ReadItemConfigDataList(data_element, "reward_item", item_cfg.rewards) != 0)return -__LINE__;
		
		node_cfg.rate_count += item_cfg.rate;
		node_cfg.item_list.push_back(item_cfg);
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

