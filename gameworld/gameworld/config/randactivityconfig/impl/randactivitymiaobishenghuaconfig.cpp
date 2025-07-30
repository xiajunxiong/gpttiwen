#include "randactivitymiaobishenghuaconfig.hpp"
#include "gameworld/item/itempool.h"

RandActivityMiaoBiShengHuaConfig::RandActivityMiaoBiShengHuaConfig()
{
}

RandActivityMiaoBiShengHuaConfig::~RandActivityMiaoBiShengHuaConfig()
{
	std::vector<MBSHBaseSectionCfg>().swap(m_base_section_cfg);
}

bool RandActivityMiaoBiShengHuaConfig::Init(TiXmlElement * RootElement, std::string * err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("reward", InitBaseCfg);
	LOAD_RA_CONFIG("other", InitOtherCfg);

	return true;
}

const std::vector<ItemConfigData>* RandActivityMiaoBiShengHuaConfig::GetRewardCfg(RandActivityManager * ramgr) const
{
	if (NULL == ramgr) return NULL;

	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_MIAO_BI_SHENG_HUA);

	for (int i = 0; i < (int)m_base_section_cfg.size(); ++i)
	{
		if (m_base_section_cfg[i].section_start <= act_real_open_day && m_base_section_cfg[i].section_end >= act_real_open_day)
		{
			return &m_base_section_cfg[i].reward_list;
		}
	}

	return NULL;
}

int RandActivityMiaoBiShengHuaConfig::InitBaseCfg(TiXmlElement * RootElement)
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
				m_base_section_cfg.push_back(MBSHBaseSectionCfg());
			}
		}

		if (m_base_section_cfg.size() == 0)
		{
			if (section_start != 1)
			{
				return -200;
			}
			m_base_section_cfg.push_back(MBSHBaseSectionCfg());
		}

		MBSHBaseSectionCfg & node_cfg = m_base_section_cfg[m_base_section_cfg.size() - 1];
		node_cfg.section_start = section_start;
		node_cfg.section_end = section_end;

		TiXmlElement* reward_list_element = data_element->FirstChildElement("reward_item_list");
		if (NULL == reward_list_element) return -111;
		TiXmlElement* reward_element = reward_list_element->FirstChildElement("reward_item");
		if (NULL == reward_element) return -112;

		while (NULL != reward_element)
		{
			ItemConfigData item;
			if (!item.ReadConfig(reward_element)) return -1;

			node_cfg.reward_list.push_back(item);

			reward_element = reward_element->NextSiblingElement();
		}


		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityMiaoBiShengHuaConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element) return -544;

	if (!GetSubNodeValue(data_element, "frequency", m_other_cfg.consunme_num) || m_other_cfg.consunme_num < 0) return -1;
	if (!GetSubNodeValue(data_element, "item_id", m_other_cfg.consunme_item) || NULL == ITEMPOOL->GetItem(m_other_cfg.consunme_item)) return -2;

	return 0;
}
