#include "randactivityskydropluxurygiftconfig.hpp"

#include "servercommon/activitydef.hpp"
#include "npc/npcpool.h"

RandActivitySkyDropLuxuryGiftConfig::RandActivitySkyDropLuxuryGiftConfig()
{

}

RandActivitySkyDropLuxuryGiftConfig::~RandActivitySkyDropLuxuryGiftConfig()
{

}

bool RandActivitySkyDropLuxuryGiftConfig::Init(TiXmlElement *RootElement, std::string *err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("act_open_skygift", InitCfg);

	return true;
}

const std::vector<SkyDropGiftCfg>* RandActivitySkyDropLuxuryGiftConfig::GetSkyDropGiftCfg(RandActivityManager* ramgr) const
{
	if (NULL == ramgr) return NULL;

	int act_real_open_day = this->GetActOpenDay(ramgr, RAND_ACTIVITY_TYPE_SKY_DROP_LUXURY_GIFT);

	for (int i = 0; i < MAX_RANDACTIVITY_MAX_SECTION && i < m_sky_drop_count; i++)
	{
		if (act_real_open_day >= m_sky_drop_gift_sections[i].section_start &&
			act_real_open_day < m_sky_drop_gift_sections[i].section_end)
		{
			return &m_sky_drop_gift_sections[i].section_cfg_vec;
		}
	}
	return NULL;
}

int RandActivitySkyDropLuxuryGiftConfig::InitCfg(TiXmlElement * RootElement)
{
	if (NULL == RootElement)
	{
		return -10000;
	}

	TiXmlElement *data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -20000;
	}
	m_sky_drop_count = 0;
	int pre_start = 0;
	int pre_end = 0;
	while (NULL != data_element)
	{
		bool has_section_cfg = true;

		int section_start = 0;
		if (!GetSubNodeValue(data_element, "section_start", section_start))
		{
			has_section_cfg = false;
		}

		int section_end = 0;
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

		if (pre_start != section_start && pre_end != section_end)
		{
			m_sky_drop_count += 1;
			pre_start = section_start;
			pre_end = section_end;
			if (m_sky_drop_count > MAX_RANDACTIVITY_MAX_SECTION)
			{
				return -1;
			}
		}

		SkyDropGiftSectionCfg &cfg = m_sky_drop_gift_sections[m_sky_drop_count - 1];
		cfg.section_start = section_start;
		cfg.section_end = section_end;

		SkyDropGiftCfg node_cfg;

		if (!GetSubNodeValue(data_element, "scene_id", node_cfg.scene_id)) return -2;
		if (!GetSubNodeValue(data_element, "next_time", node_cfg.next_time)) return -3;
		if (!GetSubNodeValue(data_element, "max_box_num", node_cfg.max_box_num)) return -4;
		if (!GetSubNodeValue(data_element, "treasure_chest_id", node_cfg.treasure_chest_id) || NULL == NPCPOOL->GetServerNPCInfo(node_cfg.treasure_chest_id)) return -5;
		if (!GetSubNodeValue(data_element, "mount", node_cfg.mount)) return -6;

		cfg.section_cfg_vec.push_back(node_cfg);

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

