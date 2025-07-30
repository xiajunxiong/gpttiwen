#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"

#include "randactivityoneyuangouconfig.hpp"


RandActivityOneYuanGouConfig::RandActivityOneYuanGouConfig()
{
}

RandActivityOneYuanGouConfig::~RandActivityOneYuanGouConfig()
{
	std::vector<RAOneYuanGouSectionCfg>().swap(m_section_cfg);
}

bool RandActivityOneYuanGouConfig::Init(TiXmlElement * RootElement, std::string * err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("gift_configure", InitBuyCfg);
	LOAD_RA_CONFIG("other", InitOtherCfg);

	return true;
}

const RAOneYuanGouItemCfg * RandActivityOneYuanGouConfig::GetRAOneYuanGouItemCfg(RandActivityManager * ramgr, int role_level) const
{
	int act_real_open_day = LOGIC_CONFIG->GetRandActivityConfigManager()->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_ONE_YUAN_GOU);
	for (std::vector<RAOneYuanGouSectionCfg>::const_iterator it = m_section_cfg.begin(); it != m_section_cfg.end(); it++)
	{
		if (it->section_start <= act_real_open_day && it->section_end >= act_real_open_day)
		{
			for (int i = 0; i < (int)it->item.size(); ++i)
			{
				if (it->item[i].level_down <= role_level && role_level <= it->item[i].level_up)
				{
					return &it->item[i];
				}
			}

			return NULL;
		}
	}

	return NULL;
}

const RAOneYuanGouOtherCfg & RandActivityOneYuanGouConfig::GetOtherCfg() const
{
	return m_other_cfg;
}

int RandActivityOneYuanGouConfig::GetConfigVer() const
{
	return m_other_cfg.cfg_ver;
}

int RandActivityOneYuanGouConfig::InitBuyCfg(TiXmlElement * RootElement)
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
				m_section_cfg.push_back(RAOneYuanGouSectionCfg());
			}
		}
		if (m_section_cfg.size() == 0)
		{
			if (section_start != 1)
			{
				return -11111;
			}
			m_section_cfg.push_back(RAOneYuanGouSectionCfg());
		}

		RAOneYuanGouSectionCfg & node_cfg = m_section_cfg[m_section_cfg.size() - 1];
		node_cfg.section_start = section_start;
		node_cfg.section_end = section_end;

		RAOneYuanGouItemCfg item_cfg;
		if (!GetSubNodeValue(data_element, "seq", item_cfg.seq) || item_cfg.seq != next_seq) return -__LINE__;
		if (!GetSubNodeValue(data_element, "level_down", item_cfg.level_down)) return -__LINE__;
		if (!GetSubNodeValue(data_element, "level_up", item_cfg.level_up)) return -__LINE__;

		TiXmlElement * item_list_element = data_element->FirstChildElement("reward_item_list");
		if (NULL != item_list_element)
		{
			TiXmlElement * item_element = item_list_element->FirstChildElement("reward_item");
			while (NULL != item_element)
			{
				ItemConfigData item;
				if (!item.ReadConfig(item_element))
				{
					return -__LINE__;
				}

				item_cfg.rewards.push_back(item);
				item_element = item_element->NextSiblingElement();
			}
		}

		next_seq += 1;

		node_cfg.item.push_back(item_cfg);
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityOneYuanGouConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}

	if (!GetSubNodeValue(data_element, "cfg_ver", m_other_cfg.cfg_ver))return -__LINE__;
	if (!GetSubNodeValue(data_element, "buy_money", m_other_cfg.need_chong_zhi_gold))return -__LINE__;

	return 0;
}
