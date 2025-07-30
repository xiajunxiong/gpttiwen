#include "config/randactivityconfig/randactivityconfigmanager.hpp"

#include "randactivitylingpaitianci2config.hpp"

RandActivityLingPaiTianCi2Config::RandActivityLingPaiTianCi2Config()
{
}

RandActivityLingPaiTianCi2Config::~RandActivityLingPaiTianCi2Config()
{
	std::vector<RALingPaiTianCi2SectionCfg>().swap(m_section_cfg);
}

bool RandActivityLingPaiTianCi2Config::Init(TiXmlElement * RootElement, std::string * err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("gift_configure", InitCfg);

	return true;
}


const RALingPaiTianCi2Cfg * RandActivityLingPaiTianCi2Config::GetRALingPaiTianCi2CfgCfg(RandActivityManager * ramgr, int seq) const
{
	if (seq < 0 || seq >= RA_LING_PAI_TIAN_CI_2_MAX_ITEM) return NULL;

	int act_real_open_day = LOGIC_CONFIG->GetRandActivityConfigManager()->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_LING_PAI_TIAN_CI_2);
	for (std::vector<RALingPaiTianCi2SectionCfg>::const_iterator it = m_section_cfg.begin(); it != m_section_cfg.end(); it++)
	{
		if (it->section_start <= act_real_open_day && it->section_end >= act_real_open_day)
		{
			if (seq >= (int)it->item.size())
			{
				return NULL;
			}

			return &it->item[seq];
		}
	}

	return NULL;
}

int RandActivityLingPaiTianCi2Config::InitCfg(TiXmlElement * RootElement)
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
				m_section_cfg.push_back(RALingPaiTianCi2SectionCfg());
			}
		}
		if (m_section_cfg.size() == 0)
		{
			if (section_start != 1)
			{
				return -11111;
			}
			m_section_cfg.push_back(RALingPaiTianCi2SectionCfg());
		}

		RALingPaiTianCi2SectionCfg & node_cfg = m_section_cfg[m_section_cfg.size() - 1];
		node_cfg.section_start = section_start;
		node_cfg.section_end = section_end;

		RALingPaiTianCi2Cfg item_cfg;
		if (!GetSubNodeValue(data_element, "seq", item_cfg.seq) || item_cfg.seq != next_seq || item_cfg.seq >= RA_LING_PAI_TIAN_CI_2_MAX_ITEM) return -__LINE__;
		if (!GetSubNodeValue(data_element, "acc_price", item_cfg.need_chong_zhi_gold) || item_cfg.need_chong_zhi_gold <= 0) return -__LINE__;
		if (!GetSubNodeValue(data_element, "is_borcast", item_cfg.is_borcast)) return -__LINE__;

		TiXmlElement * gift_item_element = data_element->FirstChildElement("gift_item");
		ItemConfigData temp;
		if (temp.ReadConfig(gift_item_element))
		{
			item_cfg.borcast_item_id = temp.item_id;
		}

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
