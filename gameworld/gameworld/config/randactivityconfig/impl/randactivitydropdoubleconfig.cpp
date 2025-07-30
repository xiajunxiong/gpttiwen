#include "randactivitydropdoubleconfig.hpp"

#include "config/randactivityconfig/randactivityconfigmanager.hpp"

RandActivityDropDoubleConfig::RandActivityDropDoubleConfig()
{

}

RandActivityDropDoubleConfig::~RandActivityDropDoubleConfig()
{
	std::vector<DropDoubleSectionCfg>().swap(m_section_cfg);
}

bool RandActivityDropDoubleConfig::Init(TiXmlElement *RootElement, std::string *err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("burst_rate_double", InitDropDoubleSectionCfg);

	return true;
}

const DropDoubleCfg * RandActivityDropDoubleConfig::GetDropPeomoteCfg(RandActivityManager* ramgr, int fb_id) const
{
	if (NULL == ramgr) return NULL;

	int open_day = LOGIC_CONFIG->GetRandActivityConfigManager()->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_DROP_DOUBLE);

	for (std::vector<DropDoubleSectionCfg>::const_iterator it = m_section_cfg.begin(); it != m_section_cfg.end(); ++it)
	{
		if (it->section_start <= open_day && it->section_end >= open_day)
		{
			for (std::vector<DropDoubleCfg>::const_iterator vit = it->node.begin(); vit != it->node.end(); ++vit)
			{
				if (vit->fb_id == fb_id)
				{
					return &(*vit);
				}
			}
		}
	}
	return NULL;
}

int RandActivityDropDoubleConfig::InitDropDoubleSectionCfg(TiXmlElement *RootElement)
{
	if (NULL == RootElement)
	{
		return -10000;
	}
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -20000;
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

				m_section_cfg.push_back(DropDoubleSectionCfg());
			}
		}
		if (m_section_cfg.size() == 0)
		{
			if (section_start != 1)
			{
				return -11111;
			}
			m_section_cfg.push_back(DropDoubleSectionCfg());
		}

		DropDoubleSectionCfg& node_cfg = m_section_cfg[m_section_cfg.size() - 1];
		node_cfg.section_start = section_start;
		node_cfg.section_end = section_end;

		DropDoubleCfg item_cfg;

		if (!GetSubNodeValue(data_element, "seq1", item_cfg.seq) || item_cfg.seq < 0) return -1;
		if (!GetSubNodeValue(data_element, "fb_id", item_cfg.fb_id)) return -2;
		if (!GetSubNodeValue(data_element, "drop_promote", item_cfg.drop_promote) || item_cfg.drop_promote <= 0) return -3;

		node_cfg.node.push_back(item_cfg);
		data_element = data_element->NextSiblingElement();
	}
	return 0;
}
