#include "config/randactivityconfig/randactivityconfigmanager.hpp"

#include "randactivityduanyangzixuanconfig.hpp"

RandActivityDuanYangZiXuanConfig::RandActivityDuanYangZiXuanConfig()
{
}

RandActivityDuanYangZiXuanConfig::~RandActivityDuanYangZiXuanConfig()
{
	std::vector<RandActivityDuanYangZiXuanSectionCfg>().swap(m_section_cfg);
}

bool RandActivityDuanYangZiXuanConfig::Init(TiXmlElement * RootElement, std::string * err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("reward_group", InitRewardGroupCfg);
	LOAD_RA_CONFIG("other", InitOtherfg);
	LOAD_RA_CONFIG("basis_configuration", InitRandActivityDuanYangZiXuanCfg);

	return true;
}

const RandActivityDuanYangZiXuanOtherCfg & RandActivityDuanYangZiXuanConfig::GetOtherCfg() const
{
	return m_other_cfg;
}

const RandActivityDuanYangZiXuanRewardGroupCfg * RandActivityDuanYangZiXuanConfig::GetRandActivityDuanYangZiXuanRewardGroupCfg(RandActivityManager * ramgr, int group_id, int reward_seq) const
{
	RandActivityDuanYangZiXuanRewardGroupCfgKey key;
	key.group_id = group_id;
	key.reward_seq = reward_seq;
	return MapValuesConstPtr(m_reward_group_cfg, key);
}

const RandActivityDuanYangZiXuanCfg * RandActivityDuanYangZiXuanConfig::GetRandActivityDuanYangZiXuanCfg(RandActivityManager * ramgr, int day) const
{
	const RandActivityDuanYangZiXuanSectionCfg * section_cfg = GetRandActivityDuanYangZiXuanSectionCfg(ramgr);

	if (!section_cfg)
	{
		return NULL;
	}

	return MapValuesConstPtr(section_cfg->item_list, day);
}

const RandActivityDuanYangZiXuanSectionCfg * RandActivityDuanYangZiXuanConfig::GetRandActivityDuanYangZiXuanSectionCfg(RandActivityManager * ramgr, time_t time) const
{
	int act_real_open_day = LOGIC_CONFIG->GetRandActivityConfigManager()->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_DUAN_YANG_ZI_XUAN);
	if (time > 0)
	{
		act_real_open_day = this->GetOpenDayByTimestamp(ramgr, (unsigned int)time);
	}

	for (std::vector<RandActivityDuanYangZiXuanSectionCfg>::const_iterator it = m_section_cfg.begin(); it != m_section_cfg.end(); it++)
	{
		if (it->section_start <= act_real_open_day && it->section_end >= act_real_open_day)
		{
			return &(*it);
		}
	}

	return NULL;
}

int RandActivityDuanYangZiXuanConfig::GetConfigVer() const
{
	return 0;
}

int RandActivityDuanYangZiXuanConfig::InitOtherfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}

	if (!GetSubNodeValue(data_element, "price", m_other_cfg.need_chong_zhi_gold)) return -__LINE__;
	if (!GetSubNodeValue(data_element, "cfg_ver", m_other_cfg.cfg_ver)) return -__LINE__;

	return 0;
}

int RandActivityDuanYangZiXuanConfig::InitRewardGroupCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}

	while (NULL != data_element)
	{
		RandActivityDuanYangZiXuanRewardGroupCfg cfg;
		if (!GetSubNodeValue(data_element, "reward_group", cfg.group_id)) return -__LINE__;
		if (!GetSubNodeValue(data_element, "seq", cfg.reward_seq)) return -__LINE__;
		if (ReadItemConfigDataList(data_element, "reward_item", cfg.rewards) != 0) return -__LINE__;

		RandActivityDuanYangZiXuanRewardGroupCfgKey key;
		key.group_id = cfg.group_id;
		key.reward_seq = cfg.reward_seq;

		m_reward_group_cfg[key] = cfg;
		data_element = data_element->NextSiblingElement();
	}

	return 0;

}

int RandActivityDuanYangZiXuanConfig::InitRandActivityDuanYangZiXuanCfg(TiXmlElement * RootElement)
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

				m_section_cfg.push_back(RandActivityDuanYangZiXuanSectionCfg());
			}
		}
		if (m_section_cfg.size() == 0)
		{
			if (section_start != 1)
			{
				return -11111;
			}
			m_section_cfg.push_back(RandActivityDuanYangZiXuanSectionCfg());
		}

		RandActivityDuanYangZiXuanSectionCfg & node_cfg = m_section_cfg[m_section_cfg.size() - 1];
		node_cfg.section_start = section_start;
		node_cfg.section_end = section_end;

		RandActivityDuanYangZiXuanCfg item_cfg;
		if (!GetSubNodeValue(data_element, "activity_day", item_cfg.day)) return -__LINE__;
		if (!GetSubNodeValue(data_element, "reward_group", item_cfg.reward_group_id)) return -__LINE__;

		node_cfg.item_list[item_cfg.day] = item_cfg;
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}
