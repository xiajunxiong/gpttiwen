
#include "global/jieyi/jieyizuconfig.h"

JieYiConfig::JieYiConfig()
{
}

JieYiConfig::~JieYiConfig()
{
	std::vector<JieYiSkillCfg>().swap(m_skill_cfg);
}

bool JieYiConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;
	LOAD_CONFIG("other", InitOtherCfg);
	LOAD_CONFIG("friendship_skill", InitSkillCfg);
	return true;
}

const JieYiOtherCfg & JieYiConfig::GetOtherCfg() const
{
	return m_other_cfg;
}

std::vector<JieYiSkillCfg> JieYiConfig::GetJieYiSkill(int intimacy, int day)
{
	std::map<int, JieYiSkillCfg> m;
	for (int i = 0; i < (int)m_skill_cfg.size(); ++i)
	{
		const JieYiSkillCfg & curr = m_skill_cfg[i];
		if (day >= curr.need_day && intimacy >= curr.need_intimacy)
		{
			JieYiSkillCfg * tmp = MapValuesPtr(m, curr.skill_type);
			if (tmp && tmp->skill_level >= curr.skill_level)
			{
				continue;
			}
		
			MapAddOrUpdate(m, curr.skill_type, curr);
		}
	}

	return MapValuesAsVector(m);
}

int JieYiConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -__LINE__;
	}

	if (!GetSubNodeValue(root_element, "intimacy_request", m_other_cfg.intimacy_request))
	{
		return -__LINE__;
	}

	if (!GetSubNodeValue(root_element, "title_revise_consume", m_other_cfg.jie_yi_name_consume_item_id))
	{
		return -__LINE__;
	}

	if (!GetSubNodeValue(root_element, "consume_num1", m_other_cfg.jie_yi_name_consume_num))
	{
		return -__LINE__;
	}

	if (!GetSubNodeValue(root_element, "name_revise_consume", m_other_cfg.jie_yi_member_name_consume_item_id))
	{
		return -__LINE__;
	}

	if (!GetSubNodeValue(root_element, "consume_num2", m_other_cfg.jie_yi_member_name_consume_num))
	{
		return -__LINE__;
	}

	if (!GetSubNodeValue(root_element, "cooling_period_time", m_other_cfg.jie_yi_zu_leave_wait_time))
	{
		return -__LINE__;
	}

	if (!GetSubNodeValue(root_element, "tan_he_not_online_time", m_other_cfg.jie_yi_tan_he_offline_time))
	{
		return -__LINE__;
	}

	if (!GetSubNodeValue(root_element, "tan_he_vote_time", m_other_cfg.jie_yi_tan_he_vote_time))
	{
		return -__LINE__;
	}

	return 0;
}

int JieYiConfig::InitSkillCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		JieYiSkillCfg cfg;
		if (!GetSubNodeValue(root_element, "friendship_intimacy", cfg.need_intimacy))
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(root_element, "friendship_time", cfg.need_day))
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(root_element, "sp_id", cfg.skill_id))
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(root_element, "skill_sort", cfg.skill_type))
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(root_element, "skill_level", cfg.skill_level))
		{
			return -__LINE__;
		}

		m_skill_cfg.push_back(cfg);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}
