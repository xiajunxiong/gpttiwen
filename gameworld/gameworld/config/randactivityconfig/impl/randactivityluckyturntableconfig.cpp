#include "randactivityluckyturntableconfig.hpp"

#include "config/randactivityconfig/randactivityconfigmanager.hpp"

RandActivityLuckyTurnTableConfig::RandActivityLuckyTurnTableConfig()
{

}

RandActivityLuckyTurnTableConfig::~RandActivityLuckyTurnTableConfig()
{
	std::vector<LuckyTurntableRewardSectionCfg>().swap(m_reward_section_cfg);
	std::vector<LuckyTurntableRewardCfg>().swap(m_reward_cfg);
}

bool RandActivityLuckyTurnTableConfig::Init(TiXmlElement *RootElement, std::string *err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("turntable_parameter", InitTurntableParamCfg);
	LOAD_RA_CONFIG("turntable_rewards", InitTurntableRewardCfg);

	return true;
}

const LuckyTurntableParamCfg* RandActivityLuckyTurnTableConfig::GetParamCfg(int type) const
{
	if (type < 0 || type >= RA_LUCKY_TURNTABLE_TYPE_MAX) return NULL;

	return &m_param_cfg[type];
}

const LuckyTurntableRewardCfg* RandActivityLuckyTurnTableConfig::GetRandRewardCfg(RandActivityManager* ramgr) const
{
	if (NULL == ramgr) return NULL;

	int act_real_open_day = LOGIC_CONFIG->GetRandActivityConfigManager()->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_LUCKY_TURNTABLE);
	for (int i = 0; i < (int)m_reward_section_cfg.size(); i++)
	{
		if (m_reward_section_cfg[i].section_start <= act_real_open_day && m_reward_section_cfg[i].section_end >= act_real_open_day)
		{
			int rand_num = rand() % m_reward_section_cfg[i].rate_count;
			for (int j = 0; j < MAX_RA_LUCKY_TURNTABLE_REWARD_NUM; j++)
			{
				if (rand_num <= m_reward_section_cfg[i].node[j].rate)
				{
					return &m_reward_section_cfg[i].node[j];
				}
				rand_num -= m_reward_section_cfg[i].node[j].rate;
			}
		}
	}

	return NULL;
}

const LuckyTurntableRewardCfg* RandActivityLuckyTurnTableConfig::GetRewardCfgBySeq(int seq) const
{
	if (seq < 0 || seq >= (int)m_reward_cfg.size()) return NULL;

	return &m_reward_cfg[seq];
}

int RandActivityLuckyTurnTableConfig::InitTurntableParamCfg(TiXmlElement *RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}
	while (NULL != data_element)
	{
		int type = 0;
		if (!GetSubNodeValue(data_element, "type", type) || type < RA_LUCKY_TURNTABLE_TYPE_SCHOOL_TASK || type >= RA_LUCKY_TURNTABLE_TYPE_MAX) return -1;
		LuckyTurntableParamCfg & node_cfg = m_param_cfg[type];
		if (!GetSubNodeValue(data_element, "parameter", node_cfg.param) || node_cfg.param <= 0) return -2;
		if (!GetSubNodeValue(data_element, "frequency", node_cfg.frequency) || node_cfg.frequency < 0) return -3;

		data_element = data_element->NextSiblingElement();
	}
	return 0;
}

int RandActivityLuckyTurnTableConfig::InitTurntableRewardCfg(TiXmlElement *RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}

	int seq = 0;
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

		if (m_reward_section_cfg.size() > 0)
		{
			int pre_index = m_reward_section_cfg.size() - 1;
			if (section_start != m_reward_section_cfg[pre_index].section_start ||
				section_end != m_reward_section_cfg[pre_index].section_end)
			{
				if (section_start < m_reward_section_cfg[pre_index].section_end)
				{
					return -888;
				}

				m_reward_section_cfg.push_back(LuckyTurntableRewardSectionCfg());
				seq = 0;
			}
		}
		if (m_reward_section_cfg.size() == 0)
		{
			if (section_start != 1)
			{
				return -11111;
			}
			m_reward_section_cfg.push_back(LuckyTurntableRewardSectionCfg());
			seq = 0;
		}

		LuckyTurntableRewardSectionCfg & node_cfg = m_reward_section_cfg[m_reward_section_cfg.size() - 1];
		node_cfg.section_start = section_start;
		node_cfg.section_end = section_end;

		LuckyTurntableRewardCfg item_cfg;

		TiXmlElement * item_element = data_element->FirstChildElement("reward_item");
		if (NULL != item_element)
		{
			if (!item_cfg.reward_item.ReadConfig(item_element)) return -100;
		}
		if (!GetSubNodeValue(data_element, "weight", item_cfg.rate) || item_cfg.rate <= 0) return -2;
		if (!GetSubNodeValue(data_element, "hearsay", item_cfg.hearsay) || item_cfg.hearsay < 0) return -3;
		if (!GetSubNodeValue(data_element, "seq", item_cfg.seq) || item_cfg.seq < 0) return -4;

		if (seq >= MAX_RA_LUCKY_TURNTABLE_REWARD_NUM) return -5;

		node_cfg.rate_count += item_cfg.rate;
		node_cfg.node[seq++] = item_cfg;
		m_reward_cfg.push_back(item_cfg);
		data_element = data_element->NextSiblingElement();
	}
	return 0;
}
