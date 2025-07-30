#include "config/randactivityconfig/randactivityconfigmanager.hpp"

#include "randactivityduanyangfuliconfig.hpp"

RandActivityDuanYangFuLiConfig::RandActivityDuanYangFuLiConfig()
{
}

RandActivityDuanYangFuLiConfig::~RandActivityDuanYangFuLiConfig()
{
	std::vector<RandActivityDuanYangFuLiSectionCfg>().swap(m_section_cfg);
}

bool RandActivityDuanYangFuLiConfig::Init(TiXmlElement * RootElement, std::string * err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("other", InitOtherfg);
	LOAD_RA_CONFIG("basis_configuration", InitRandActivityDuanYangFiLiCfg);

	return true;
}

const RandActivityDuanYangFiLiOtherCfg & RandActivityDuanYangFuLiConfig::GetOtherCfg() const
{
	return m_other_cfg;
}

const RandActivityDuanYangFiLiCfg * RandActivityDuanYangFuLiConfig::GetRandActivityDuanYangFiLiCfg(RandActivityManager * ramgr, int reward_seq) const
{
	const RandActivityDuanYangFuLiSectionCfg * section_cfg = this->GetRandActivityDuanYangFiLiSectionCfg(ramgr);

	if (!section_cfg)
	{
		return NULL;
	}

	return MapValuesConstPtr(section_cfg->item_list, reward_seq);
}

const RandActivityDuanYangFuLiSectionCfg * RandActivityDuanYangFuLiConfig::GetRandActivityDuanYangFiLiSectionCfg(RandActivityManager * ramgr, time_t time) const
{
	int act_real_open_day = LOGIC_CONFIG->GetRandActivityConfigManager()->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_DUAN_YANG_FU_LI);
	if (time > 0)
	{
		act_real_open_day = this->GetOpenDayByTimestamp(ramgr, (unsigned int)time);
	}

	for (std::vector<RandActivityDuanYangFuLiSectionCfg>::const_iterator it = m_section_cfg.begin(); it != m_section_cfg.end(); it++)
	{
		if (it->section_start <= act_real_open_day && it->section_end >= act_real_open_day)
		{
			return &(*it);
		}
	}

	return NULL;
}

int RandActivityDuanYangFuLiConfig::GetConfigVer() const
{
	return m_other_cfg.cfg_ver;
}

int RandActivityDuanYangFuLiConfig::InitOtherfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}

	if (!GetSubNodeValue(data_element, "discount_price", m_other_cfg.all_buy_need_chong_zhi_gold)) return -__LINE__;
	if (!GetSubNodeValue(data_element, "cfg_ver", m_other_cfg.cfg_ver)) return -__LINE__;

	return 0;
}

int RandActivityDuanYangFuLiConfig::InitRandActivityDuanYangFiLiCfg(TiXmlElement * RootElement)
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

				m_section_cfg.push_back(RandActivityDuanYangFuLiSectionCfg());
			}
		}
		if (m_section_cfg.size() == 0)
		{
			if (section_start != 1)
			{
				return -11111;
			}
			m_section_cfg.push_back(RandActivityDuanYangFuLiSectionCfg());
		}

		RandActivityDuanYangFuLiSectionCfg & node_cfg = m_section_cfg[m_section_cfg.size() - 1];
		node_cfg.section_start = section_start;
		node_cfg.section_end = section_end;

		RandActivityDuanYangFiLiCfg item_cfg;
		if (!GetSubNodeValue(data_element, "seq", item_cfg.seq)) return -__LINE__;
		if (!GetSubNodeValue(data_element, "activity_times", item_cfg.day)) return -__LINE__;
		if (!GetSubNodeValue(data_element, "receive_days", item_cfg.can_fetch_times)) return -__LINE__;
		if (!GetSubNodeValue(data_element, "price", item_cfg.need_chong_zhi_gold)) return -__LINE__;
		if (ReadItemConfigDataList(data_element, "reward_item", item_cfg.rewards) != 0)return -__LINE__;

		node_cfg.item_list[item_cfg.seq] = item_cfg;
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}
