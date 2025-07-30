#include "randactivitysealpromoteconfig.hpp"

#include "config/randactivityconfig/randactivityconfigmanager.hpp"

RandActivitySealPromoteConfig::RandActivitySealPromoteConfig()
{

}

RandActivitySealPromoteConfig::~RandActivitySealPromoteConfig()
{
	std::vector<RASealPromoteSectionCfg>().swap(m_seal_promote_section_cfg);
}

bool RandActivitySealPromoteConfig::Init(TiXmlElement *RootElement, std::string *err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("probability_promote", InitSealPromoteSectionCfg);

	return true;
}

const RASealPromoteCfg * RandActivitySealPromoteConfig::GetSealPromoteCfg(RandActivityManager* ramgr, int pet_id) const
{
	int open_day = LOGIC_CONFIG->GetRandActivityConfigManager()->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_SEAL_PROMOTE);

	for (std::vector<RASealPromoteSectionCfg>::const_iterator it = m_seal_promote_section_cfg.begin(); it != m_seal_promote_section_cfg.end(); ++it)
	{
		if (it->section_start <= open_day && it->section_end >= open_day)
		{
			for (std::vector<RASealPromoteCfg>::const_iterator vit = it->node.begin(); vit != it->node.end(); ++vit)
			{
				if (vit->pet_id == pet_id)
				{
					return &(*vit);
				}
			}
		}
	}
	return NULL;
}

int RandActivitySealPromoteConfig::InitSealPromoteSectionCfg(TiXmlElement *RootElement)
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

		if (m_seal_promote_section_cfg.size() > 0)
		{
			int pre_index = m_seal_promote_section_cfg.size() - 1;
			if (section_start != m_seal_promote_section_cfg[pre_index].section_start ||
				section_end != m_seal_promote_section_cfg[pre_index].section_end)
			{
				if (section_start < m_seal_promote_section_cfg[pre_index].section_end)
				{
					return -888;
				}

				m_seal_promote_section_cfg.push_back(RASealPromoteSectionCfg());
			}
		}
		if (m_seal_promote_section_cfg.size() == 0)
		{
			if (section_start != 1)
			{
				return -11111;
			}
			m_seal_promote_section_cfg.push_back(RASealPromoteSectionCfg());
		}

		RASealPromoteSectionCfg & node_cfg = m_seal_promote_section_cfg[m_seal_promote_section_cfg.size() - 1];
		node_cfg.section_start = section_start;
		node_cfg.section_end = section_end;

		RASealPromoteCfg item_cfg;
		if (!GetSubNodeValue(data_element, "seq", item_cfg.seq) || item_cfg.seq < 0) return -1;
		if (!GetSubNodeValue(data_element, "pets_id", item_cfg.pet_id)) return -2;
		if (!GetSubNodeValue(data_element, "rate_common", item_cfg.rate_common)) return -3;
		if (!GetSubNodeValue(data_element, "rate_silver", item_cfg.rate_silver)) return -4;
		if (!GetSubNodeValue(data_element, "rate_gold", item_cfg.rate_gold)) return -5;

		node_cfg.node.push_back(item_cfg);
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}
