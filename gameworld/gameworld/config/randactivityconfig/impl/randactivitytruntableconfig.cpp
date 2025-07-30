#include "randactivitytruntableconfig.hpp"

#include "servercommon/activitydef.hpp"

RandActivityTrunTableConfig::RandActivityTrunTableConfig()
{

}

RandActivityTrunTableConfig::~RandActivityTrunTableConfig()
{

}

bool RandActivityTrunTableConfig::Init(TiXmlElement *RootElement, std::string *err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("open_turntable", InitCfg);

	return true;
}

const RATurnTableSectionCfg* RandActivityTrunTableConfig::GetTurnTableItemCfg(RandActivityManager* ramgr) const
{
	if (NULL == ramgr) return NULL;

	int act_real_open_day = this->GetActOpenDay(ramgr, RAND_ACTIVITY_TYPE_TRUN_TABLE);
	for (std::vector<RATurnTableSectionCfg>::const_iterator it = m_turn_table_sections.begin(); it != m_turn_table_sections.end(); ++it)
	{
		if (act_real_open_day >= it->section_start && act_real_open_day <= it->section_end)
		{
			return &(*it);
		}
	}
	return NULL;
}

const RATurnTableCfg* RandActivityTrunTableConfig::GetTurnTableReward(const RATurnTableSectionCfg *turntablereward_cfg)const
{
	int rand_num = 0;
	for (int i = 0; i < RA_TURN_TABLE_MAX_NUM; i++)
	{
		rand_num += turntablereward_cfg->node_cfg[i].weight;
	}
	rand_num = rand() % rand_num;
	int weight = 0;
	for (int i = 0; i < RA_TURN_TABLE_MAX_NUM; i++)
	{
		weight += turntablereward_cfg->node_cfg[i].weight;
		if (rand_num <= weight)
		{
			return &turntablereward_cfg->node_cfg[i];
		}
	}
	return NULL;
}

int RandActivityTrunTableConfig::InitCfg(TiXmlElement * RootElement)
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

		if (m_turn_table_sections.size() > 0)
		{
			int pre_index = m_turn_table_sections.size() - 1;
			if (section_start != m_turn_table_sections[pre_index].section_start ||
				section_end != m_turn_table_sections[pre_index].section_end)
			{
				if (section_start < m_turn_table_sections[pre_index].section_end)
				{
					return -888;
				}

				m_turn_table_sections.push_back(RATurnTableSectionCfg());
			}
		}


		if (m_turn_table_sections.size() == 0)
		{
			if (section_start != 1)
			{
				return -10101;
			}

			m_turn_table_sections.push_back(RATurnTableSectionCfg());
		}

		RATurnTableSectionCfg &section_cfg = m_turn_table_sections[m_turn_table_sections.size() - 1];

		section_cfg.section_start = section_start;
		section_cfg.section_end = section_end;

		int seq = 0;
		if (!GetSubNodeValue(data_element, "seq", seq)) return -1;
		RATurnTableCfg &node_cfg = section_cfg.node_cfg[seq];
		node_cfg.seq = seq;

		TiXmlElement *item_element = data_element->FirstChildElement("reward_item");
		if (!node_cfg.item.ReadConfig(item_element)) return -100;

		if (!GetSubNodeValue(data_element, "weight", node_cfg.weight)) return -2;

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}
