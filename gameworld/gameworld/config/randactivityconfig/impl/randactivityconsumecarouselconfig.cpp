#include "randactivityconsumecarouselconfig.hpp"

#include "servercommon/activitydef.hpp"

RandActivityConsumeCarouselConfig::RandActivityConsumeCarouselConfig()
{

}

RandActivityConsumeCarouselConfig::~RandActivityConsumeCarouselConfig()
{

}

bool RandActivityConsumeCarouselConfig::Init(TiXmlElement *RootElement, std::string *err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("consumer_carousel", InitCfg);

	return true;
}

const RAConsumerCarouselCfg * RandActivityConsumeCarouselConfig::GetConsumerCarouselReward(RandActivityManager* ramgr) const
{
	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_CONSUME_CAROUSEL);
	for (std::vector<RAConsumerCarouselSectionCfg>::const_iterator it = m_consume_carousel_sections.begin(); it != m_consume_carousel_sections.end(); ++it)
	{
		if (act_real_open_day >= it->section_start && act_real_open_day <= it->section_end)
		{
			int rand_num = rand() % it->rate_count;
			for (int i = 0; i < CONSUME_CAROUSEL_MAX_NUM; i++)
			{
				if (rand_num <= it->node_cfg[i].score)
					return &it->node_cfg[i];
				rand_num -= it->node_cfg[i].score;
			}
		}
	}

	return NULL;
}

const RAConsumerCarouselCfg * RandActivityConsumeCarouselConfig::GetConsumerCarouselbyseq(RandActivityManager* ramgr, int seq) const
{
	if (seq < 0 || seq >= CONSUME_CAROUSEL_MAX_NUM) return NULL;

	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_CONSUME_CAROUSEL);
	for (std::vector<RAConsumerCarouselSectionCfg>::const_iterator it = m_consume_carousel_sections.begin(); it != m_consume_carousel_sections.end(); ++it)
	{
		if (act_real_open_day >= it->section_start && act_real_open_day <= it->section_end)
		{
			return &it->node_cfg[seq];
		}
	}
	return NULL;
}

int RandActivityConsumeCarouselConfig::InitCfg(TiXmlElement * RootElement)
{
	TiXmlElement *data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -999;
	}
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
		if (m_consume_carousel_sections.size() > 0)
		{
			int index = m_consume_carousel_sections.size() - 1;
			if (section_start != m_consume_carousel_sections[index].section_start || section_end != m_consume_carousel_sections[index].section_end)
			{
				if (section_start < m_consume_carousel_sections[index].section_end)
				{
					return -888;
				}
				m_consume_carousel_sections.push_back(RAConsumerCarouselSectionCfg());
			}

		}
		else
			m_consume_carousel_sections.push_back(RAConsumerCarouselSectionCfg());

		RAConsumerCarouselSectionCfg &section_cfg = m_consume_carousel_sections[m_consume_carousel_sections.size() - 1];
		section_cfg.section_start = section_start;
		section_cfg.section_end = section_end;

		int seq = 0;
		if (!GetSubNodeValue(data_element, "seq", seq) || seq < 0 || seq >= CONSUME_CAROUSEL_MAX_NUM)return -1;
		RAConsumerCarouselCfg &node_cfg = section_cfg.node_cfg[seq];
		node_cfg.seq = seq;

		if (!GetSubNodeValue(data_element, "score", node_cfg.score) || node_cfg.score < 0)return -2;
		section_cfg.rate_count += node_cfg.score;

		TiXmlElement * list_node = data_element->FirstChildElement("reward_item_list");
		if (NULL != list_node)
		{
			TiXmlElement * item_node = list_node->FirstChildElement("reward_item");
			if (NULL != item_node)
			{
				if (!node_cfg.item.ReadConfig(item_node)) return -100;

				item_node = item_node->NextSiblingElement();
			}
		}
		if (!GetSubNodeValue(data_element, "show", node_cfg.show) || node_cfg.show < 0 || node_cfg.show > 1)return -3;

		data_element = data_element->NextSiblingElement();
	}
	return 0;
}

