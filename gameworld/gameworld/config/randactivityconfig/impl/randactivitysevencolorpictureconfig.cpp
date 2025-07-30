#include "randactivitysevencolorpictureconfig.hpp"

#include "gamecommon.h"
#include "config/logicconfigmanager.hpp"
#include "servercommon/serverconfig/localconfig.hpp"

RandActivitySevenColorPictureConfig::RandActivitySevenColorPictureConfig()
{

}

RandActivitySevenColorPictureConfig::~RandActivitySevenColorPictureConfig()
{
	std::vector<RASevenColorPictureOtherCfg>().swap(m_other_cfg);
	std::vector<RASevenColorPictureAwardSectionCfg>().swap(m_seven_color_section_cfg);
}

bool RandActivitySevenColorPictureConfig::Init(TiXmlElement *RootElement, std::string *err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("other", InitSevenColorPictureOtherCfg);
	LOAD_RA_CONFIG("seven_color_picture_pigment", InitSevenColorPicturePigmentCfg);
	LOAD_RA_CONFIG("seven_color_picture_award", InitSevenColorPictureAwardCfg);

	return true;
}

const RASevenColorPictureOtherCfg & RandActivitySevenColorPictureConfig::GetOtherCfg(RandActivityManager * ramgr) const
{
	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_SEVEN_COLOR_PICTURE);

	for (std::vector<RASevenColorPictureOtherCfg>::const_iterator it = m_other_cfg.begin(); m_other_cfg.end() != it; ++it)
	{
		if (it->section_end >= act_real_open_day && act_real_open_day >= it->section_start)
		{
			return *it;
		}
	}
	return *m_other_cfg.rbegin();
}

const RASevenColorPicturePigmentCfg* RandActivitySevenColorPictureConfig::GetSevenColorPicturePigmentCfg(int seq) const
{
	if (0 < seq || seq >= SEVEN_COLOR_PIGMENT_TYPE_MAX)
	{
		return NULL;
	}

	return &m_seven_color_picture_pigment_cfg[seq]; 
}

const RASevenColorPictureAwardCfg* RandActivitySevenColorPictureConfig::GetSevenColorPictureAwardCfg(int level, int score, RandActivityManager * ramgr) const
{
	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_SEVEN_COLOR_PICTURE);
	for (std::vector<RASevenColorPictureAwardSectionCfg>::const_iterator section_it = m_seven_color_section_cfg.begin(); m_seven_color_section_cfg.end() != section_it; ++section_it)
	{
		if (section_it->section_end >= act_real_open_day && act_real_open_day >= section_it->section_start)
		{
			for (std::map<int, RASevenColorPictureAwardLevelCfg>::const_iterator it = section_it->m_seven_color_picture_award_level_cfg.begin(); it != section_it->m_seven_color_picture_award_level_cfg.end(); it++)
			{
				if (it->first / 1000 <= level && it->first % 1000 >= level)
				{
					for (int i = 0; i < it->second.count && i < SEVEN_COLOR_PICTURE_AWARD__LEVEL_MAX_NUM; i++)
					{
						if (score >= it->second.node_list[i].summation_score)
							return &it->second.node_list[i];
					}
				}
			}

			break;
		}
	}
	
	return NULL;
}

int RandActivitySevenColorPictureConfig::InitSevenColorPictureOtherCfg(TiXmlElement *RootElement)
{
	TiXmlElement *data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -999;
	}
	while (NULL != data_element)
	{
		RASevenColorPictureOtherCfg node;

		TiXmlElement * item_node = data_element->FirstChildElement("seven_color_collection_reward");
		if (NULL != item_node)
		{
			if (!node.seven_color_collection_reward.ReadConfig(item_node)) return -100;

			item_node = item_node->NextSiblingElement();
		}
		if (!GetSubNodeValue(data_element, "section_start", node.section_start) || node.section_start < 0) return -111;
		if (!GetSubNodeValue(data_element, "section_end", node.section_end) || node.section_end < 0) return -222;
		if (0 == node.section_end)
		{
			node.section_end = MAX_INT;
		}
		if (node.section_end < node.section_start) return -997;

		if (!GetSubNodeValue(data_element, "time", node.end_day_time) || node.end_day_time < 0) return -1;
		if (!GetSubNodeValue(data_element, "coin_num", node.coin_num) || node.coin_num <= 0) return -2;
		if (!GetSubNodeValue(data_element, "gold_num", node.gold_num) || node.gold_num <= 0) return -3;
		if (!GetSubNodeValue(data_element, "is_complement", node.is_complement)) return -4;

		m_other_cfg.push_back(node);

		data_element = data_element->NextSiblingElement();
	}
	if (0 == m_other_cfg.size())
	{
		return -998;
	}
	return 0;
}

int RandActivitySevenColorPictureConfig::InitSevenColorPicturePigmentCfg(TiXmlElement *RootElement)
{
	TiXmlElement *data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -999;
	}
	while (NULL != data_element)
	{
		int seq = 0;
		if (!GetSubNodeValue(data_element, "seq", seq) || seq < 0 || seq >= SEVEN_COLOR_PIGMENT_TYPE_MAX) return -1;

		RASevenColorPicturePigmentCfg node_cfg;
		node_cfg.seq = seq;

		if (!GetSubNodeValue(data_element, "quality", node_cfg.quality) || node_cfg.quality < SEVEN_COLOR_PIGMENT_TOP_TYPE || node_cfg.quality >= SEVEN_COLOR_PIGMENT_TYPE_MAX) return -2;

		if (!GetSubNodeValue(data_element, "is_special", node_cfg.is_special) || node_cfg.is_special <= 0) return -3;

		m_seven_color_picture_pigment_cfg[node_cfg.seq] = node_cfg;

		data_element = data_element->NextSiblingElement();
	}
	return 0;
}

int RandActivitySevenColorPictureConfig::InitSevenColorPictureAwardCfg(TiXmlElement *RootElement)
{
	TiXmlElement *data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -999;
	}
	int last_section = 0;
	while (NULL != data_element)
	{
		int section_start = 0, section_end = 0;
		if (!GetSubNodeValue(data_element, "section_start", section_start) || section_start <= 0) return -999;
		if (!GetSubNodeValue(data_element, "section_end", section_end) || section_end < 0) return -998;
		if (0 == section_end)
		{
			section_end = MAX_INT;
		}

		if (section_end < section_start) return -997;

		if (section_start != last_section)
		{
			m_seven_color_section_cfg.push_back(RASevenColorPictureAwardSectionCfg(section_start, section_end));
			last_section = section_start;
		}

		int seq = 0;
		if (!GetSubNodeValue(data_element, "seq", seq) || seq < 0) return -1;

		RASevenColorPictureAwardCfg node_cfg;
		node_cfg.seq = seq;

		if (!GetSubNodeValue(data_element, "grade_start", node_cfg.grade_start) || node_cfg.grade_start <= 0 || node_cfg.grade_start > MAX_ROLE_LEVEL)return -2;
		if (!GetSubNodeValue(data_element, "grade_end", node_cfg.grade_end) || node_cfg.grade_end < 0) return -3;
		if (!GetSubNodeValue(data_element, "seq1", node_cfg.seq1) || node_cfg.seq1 < 0) return -4;
		if (!GetSubNodeValue(data_element, "summation_score", node_cfg.summation_score) || node_cfg.summation_score < 0) return -5;
		if (!GetSubNodeValue(data_element, "hearsay", node_cfg.hearsay) || node_cfg.hearsay < 0) return -6;
		
		LOAD_ITEM_NODE("reward_item_list", "reward_item", node_cfg.reward, node_cfg.reward_num);

		if (node_cfg.grade_end == 0)
		{
			node_cfg.grade_end = MAX_ROLE_LEVEL;
		}
		int level_key = 1000 * node_cfg.grade_start + node_cfg.grade_end;
		UNSTD_STATIC_CHECK(MAX_ROLE_LEVEL <= 1000);

		m_seven_color_section_cfg.rbegin()->m_seven_color_picture_award_level_cfg[level_key].node_list[node_cfg.seq1] = node_cfg;
		m_seven_color_section_cfg.rbegin()->m_seven_color_picture_award_level_cfg[level_key].count++;
		m_seven_color_section_cfg.rbegin()->m_seven_color_picture_award_cfg.push_back(node_cfg);
		data_element = data_element->NextSiblingElement();
	}
	return 0;
}

