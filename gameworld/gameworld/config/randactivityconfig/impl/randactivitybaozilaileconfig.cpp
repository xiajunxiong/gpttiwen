#include "randactivitybaozilaileconfig.hpp"

RandActivityBaoZiLaiLeConfig::RandActivityBaoZiLaiLeConfig()
{
}

RandActivityBaoZiLaiLeConfig::~RandActivityBaoZiLaiLeConfig()
{
	std::vector<BZLLBaseSectionCfg>().swap(m_base_section_cfg);
}

bool RandActivityBaoZiLaiLeConfig::Init(TiXmlElement * RootElement, std::string * err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("reward", InitBaseCfg);
	LOAD_RA_CONFIG("other", InitOtherCfg);

	return true;
}

const std::vector<ItemConfigData>* RandActivityBaoZiLaiLeConfig::GetRewardCfg(RandActivityManager * ramgr, int seq, bool is_win) const
{
	if (NULL == ramgr) return NULL;

	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_BAO_ZI_LAI_LE);

	for (int i = 0; i < (int)m_base_section_cfg.size(); ++i)
	{
		if (m_base_section_cfg[i].section_start <= act_real_open_day && m_base_section_cfg[i].section_end >= act_real_open_day)
		{
			std::map<int, BZLLBaseCfg>::const_iterator it = m_base_section_cfg[i].reward_cfg.find(seq);
			if (it == m_base_section_cfg[i].reward_cfg.end()) return NULL;

			if (is_win)
			{
				return &it->second.win_reward_list;
			}
			else
			{
				return &it->second.fail_reward_list;
			}
		}
	}

	return NULL;
}

int RandActivityBaoZiLaiLeConfig::InitBaseCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element) return -544;

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

		if (m_base_section_cfg.size() > 0)
		{
			int pre_index = m_base_section_cfg.size() - 1;
			if (section_start != m_base_section_cfg[pre_index].section_start ||
				section_end != m_base_section_cfg[pre_index].section_end)
			{
				if (section_start < m_base_section_cfg[pre_index].section_end)
				{
					return -100;
				}
				m_base_section_cfg.push_back(BZLLBaseSectionCfg());
			}
		}

		if (m_base_section_cfg.size() == 0)
		{
			if (section_start != 1)
			{
				return -200;
			}
			m_base_section_cfg.push_back(BZLLBaseSectionCfg());
		}

		BZLLBaseSectionCfg & node_cfg = m_base_section_cfg[m_base_section_cfg.size() - 1];
		node_cfg.section_start = section_start;
		node_cfg.section_end = section_end;

		int seq = 0;
		BZLLBaseCfg base_cfg;

		if (!GetSubNodeValue(data_element, "seq", seq) || seq < 0) return -1;
		if (!GetSubNodeValue(data_element, "reward_number", base_cfg.number) || base_cfg.number < 0) return -2;
		if (!GetSubNodeValue(data_element, "time", base_cfg.time) || base_cfg.time < 0) return -3;

		TiXmlElement * win_list_element = data_element->FirstChildElement("reward_item_list");
		if (NULL == win_list_element) return -11;
		TiXmlElement * win_element = win_list_element->FirstChildElement("reward_item");
		if (NULL == win_element) return -12;
		while (NULL != win_element)
		{
			ItemConfigData item;
			if (!item.ReadConfig(win_element)) return -4;

			base_cfg.win_reward_list.push_back(item);

			win_element = win_element->NextSiblingElement();
		}

		TiXmlElement * fail_list_element = data_element->FirstChildElement("reward2_item_list");
		if (NULL == fail_list_element) return -13;
		TiXmlElement * fail_element = fail_list_element->FirstChildElement("reward2_item");
		if (NULL == fail_element) return -14;
		while (NULL != fail_element)
		{
			ItemConfigData item;
			if (!item.ReadConfig(fail_element)) return -5;

			base_cfg.fail_reward_list.push_back(item);

			fail_element = fail_element->NextSiblingElement();
		}

		node_cfg.reward_cfg[seq] = base_cfg;

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityBaoZiLaiLeConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element) return -544;

	if (!GetSubNodeValue(data_element, "frequency", m_other_cfg.frequency) || m_other_cfg.frequency < 0) return -1;

	return 0;
}
