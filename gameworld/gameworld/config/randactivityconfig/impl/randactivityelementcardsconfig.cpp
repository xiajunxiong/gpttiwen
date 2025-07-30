#include "randactivityelementcardsconfig.hpp"

#include "config/randactivityconfig/randactivityconfigmanager.hpp"

RandActivityElementCardsConfig::RandActivityElementCardsConfig()
{

}

RandActivityElementCardsConfig::~RandActivityElementCardsConfig()
{
	std::vector<RAElementCardsSectionCfg>().swap(m_section_cfg);
}

bool RandActivityElementCardsConfig::Init(TiXmlElement *RootElement, std::string *err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("other", InitElementCardsOtherCfg);
	LOAD_RA_CONFIG("reward", InitElementCardsSectionCfg);

	return true;
}

const RAElementCardsCfg * RandActivityElementCardsConfig::GetElementCardsCfg(RandActivityManager* ramgr) const
{
	if (NULL == ramgr) return NULL;

	int open_day = LOGIC_CONFIG->GetRandActivityConfigManager()->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_ELEMENT_CARDS);

	for (std::vector<RAElementCardsSectionCfg>::const_iterator it = m_section_cfg.begin(); it != m_section_cfg.end(); ++it)
	{
		if (it->section_start <= open_day && it->section_end >= open_day)
		{
			int rand_weight = rand() % it->all_weight;
			if (0 > rand_weight) return NULL;

			for (std::vector<RAElementCardsCfg>::const_iterator vit = it->node.begin(); vit != it->node.end(); ++vit)
			{
				if (vit->weight > rand_weight)
				{
					return &(*vit);
				}
				rand_weight -= vit->weight;
			}
		}
	}
	return NULL;
}

int RandActivityElementCardsConfig::InitElementCardsOtherCfg(TiXmlElement *RootElement)
{
	TiXmlElement *data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -999;
	}

	while (NULL != data_element)
	{
		RAElementCardsOtherCfg & cfg = m_other_cfg;

		if (!GetSubNodeValue(data_element, "frequency", cfg.login_add_count) || cfg.login_add_count < 0) return -1;

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityElementCardsConfig::InitElementCardsSectionCfg(TiXmlElement *RootElement)
{
	if (NULL == RootElement) return -10000;

	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element) return -20000;

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

				m_section_cfg.push_back(RAElementCardsSectionCfg());
			}
		}
		if (m_section_cfg.size() == 0)
		{
			if (section_start != 1)
			{
				return -11111;
			}
			m_section_cfg.push_back(RAElementCardsSectionCfg());
		}

		RAElementCardsSectionCfg & node_cfg = m_section_cfg[m_section_cfg.size() - 1];
		node_cfg.section_start = section_start;
		node_cfg.section_end = section_end;

		RAElementCardsCfg item_cfg;

		if (!GetSubNodeValue(data_element, "seq", item_cfg.seq) || item_cfg.seq < 0) return -1;
		if (!GetSubNodeValue(data_element, "weight", item_cfg.weight) || item_cfg.weight < 0) return -2;
		node_cfg.all_weight += item_cfg.weight;

		TiXmlElement * reward_list_element = data_element->FirstChildElement("reward_item_list");
		if (reward_list_element != NULL)
		{
			TiXmlElement * reward_element = reward_list_element->FirstChildElement("reward_item");
			while (reward_element)
			{
				ItemConfigData reward;
				if (!reward.ReadConfig(reward_element))
				{
					return -__LINE__;
				}
				item_cfg.item.push_back(reward);
				reward_element = reward_element->NextSiblingElement();
			}
		}

		node_cfg.node.push_back(item_cfg);
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}
