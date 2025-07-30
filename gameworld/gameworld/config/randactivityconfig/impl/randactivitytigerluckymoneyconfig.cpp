#include "randactivitytigerluckymoneyconfig.hpp"
#include "servercommon/roleactivity/ratigerluckymoneyparam.hpp"


RandActivityTigerLuckyMoneyConfig::RandActivityTigerLuckyMoneyConfig()
{

}


RandActivityTigerLuckyMoneyConfig::~RandActivityTigerLuckyMoneyConfig()
{
	std::vector<RATLMBasisSectionCfg>().swap(m_basis_section_cfg);
	std::vector<RATLMRewardSectionCfg>().swap(m_reward_section_cfg);
}

bool RandActivityTigerLuckyMoneyConfig::Init(TiXmlElement *RootElement, std::string *err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("basic_configure", InitBasisCfg);
	LOAD_RA_CONFIG("gift_configure", InitRewardCfg);

	return true;
}

const std::vector<RATLMBasisCfg> * RandActivityTigerLuckyMoneyConfig::GetBasisCfg(RandActivityManager * ramgr, int role_level) const
{
	if (NULL == ramgr || role_level <= 0 || role_level > MAX_ROLE_LEVEL) return NULL;

	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_TIGER_LUCKY_MONEY);

	for (int i = 0; i < (int)m_basis_section_cfg.size(); ++i)
	{
		if (m_basis_section_cfg[i].section_start <= act_real_open_day && m_basis_section_cfg[i].section_end >= act_real_open_day)
		{
			for (int j = m_basis_section_cfg[i].basis_level_cfg.size() - 1; j >= 0; --j)
			{
				if (role_level >= m_basis_section_cfg[i].basis_level_cfg[j].level)
				{
					return &m_basis_section_cfg[i].basis_level_cfg[j].basis_cfg;
				}
			}
		}
	}

	return NULL;
}

const RATLMRewardCfg * RandActivityTigerLuckyMoneyConfig::GetRewardCfg(RandActivityManager * ramgr) const
{
	if (NULL == ramgr) return NULL;

	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_TIGER_LUCKY_MONEY);

	for (int i = 0; i < (int)m_reward_section_cfg.size(); ++i)
	{
		if (m_reward_section_cfg[i].section_start <= act_real_open_day && m_reward_section_cfg[i].section_end >= act_real_open_day)
		{
			return &m_reward_section_cfg[i].reward_cfg;
		}
	}

	return NULL;
}

const int RandActivityTigerLuckyMoneyConfig::GetStartLevel() const
{
	if (m_basis_section_cfg.empty()) return 0;
	if (m_basis_section_cfg[0].basis_level_cfg.empty()) return 0;

	return m_basis_section_cfg[0].basis_level_cfg[0].level;
}

int RandActivityTigerLuckyMoneyConfig::InitBasisCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -544;
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

		if (m_basis_section_cfg.size() > 0)
		{
			int pre_index = m_basis_section_cfg.size() - 1;
			if (section_start != m_basis_section_cfg[pre_index].section_start ||
				section_end != m_basis_section_cfg[pre_index].section_end)
			{
				if (section_start < m_basis_section_cfg[pre_index].section_end)
				{
					return -100;
				}
				m_basis_section_cfg.push_back(RATLMBasisSectionCfg());
			}
		}

		if (m_basis_section_cfg.size() == 0)
		{
			if (section_start != 1)
			{
				return -200;
			}
			m_basis_section_cfg.push_back(RATLMBasisSectionCfg());
		}

		RATLMBasisSectionCfg & node_cfg = m_basis_section_cfg[m_basis_section_cfg.size() - 1];
		node_cfg.section_start = section_start;
		node_cfg.section_end = section_end;

		int level_limit;
		if (!GetSubNodeValue(data_element, "level", level_limit) || level_limit <= 0 || level_limit > MAX_ROLE_LEVEL) return -__LINE__;

		if (node_cfg.basis_level_cfg.size() > 0)
		{
			int pre_index = node_cfg.basis_level_cfg.size() - 1;
			if (level_limit != node_cfg.basis_level_cfg[pre_index].level)
			{
				if (level_limit < node_cfg.basis_level_cfg[pre_index].level)
				{
					return -__LINE__;
				}
				node_cfg.basis_level_cfg.push_back(RATLMBasisLevelCfg());
			}
		}

		if (node_cfg.basis_level_cfg.size() == 0)
		{
			node_cfg.basis_level_cfg.push_back(RATLMBasisLevelCfg());
		}

		RATLMBasisLevelCfg & node_node_cfg = node_cfg.basis_level_cfg[node_cfg.basis_level_cfg.size() - 1];
		node_node_cfg.level = level_limit;

		RATLMBasisCfg basis_cfg;

		if (!GetSubNodeValue(data_element, "seq", basis_cfg.seq) || basis_cfg.seq < 0) return -__LINE__;
		if (!GetSubNodeValue(data_element, "task_type", basis_cfg.task_type)|| basis_cfg.task_type < 0 || basis_cfg.task_type >= RATLM::RATLM_TASK_TYPE_MAX) return -__LINE__;
		if (!GetSubNodeValue(data_element, "parameter_1", basis_cfg.task_parameter) || basis_cfg.task_parameter < 0) return -__LINE__;

		TiXmlElement * reward_list_element = data_element->FirstChildElement("reward_item_list");
		if (NULL != reward_list_element)
		{
			TiXmlElement * reward_element = reward_list_element->FirstChildElement("reward_item");
			if (NULL == reward_element) return -__LINE__;

			while (NULL != reward_element)
			{
				ItemConfigData item;
				if (!item.ReadConfigNoCheck(reward_element))
				{
					return -__LINE__;
				}

				basis_cfg.reward_item.push_back(item);
				reward_element = reward_element->NextSiblingElement();
			}
		}

		node_node_cfg.basis_cfg.push_back(basis_cfg);
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityTigerLuckyMoneyConfig::InitRewardCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -544;
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

		if (m_reward_section_cfg.size() > 0)
		{
			int pre_index = m_reward_section_cfg.size() - 1;
			if (section_start != m_reward_section_cfg[pre_index].section_start ||
				section_end != m_reward_section_cfg[pre_index].section_end)
			{
				if (section_start < m_reward_section_cfg[pre_index].section_end)
				{
					return -100;
				}
				m_reward_section_cfg.push_back(RATLMRewardSectionCfg());
			}
		}

		if (m_reward_section_cfg.size() == 0)
		{
			if (section_start != 1)
			{
				return -200;
			}
			m_reward_section_cfg.push_back(RATLMRewardSectionCfg());
		}

		RATLMRewardSectionCfg & node_cfg = m_reward_section_cfg[m_reward_section_cfg.size() - 1];
		node_cfg.section_start = section_start;
		node_cfg.section_end = section_end;

		RATLMRewardCfg reward_cfg;

		if (!GetSubNodeValue(data_element, "reward_type", reward_cfg.reward_type) || reward_cfg.reward_type < 0 || reward_cfg.reward_type >= RATLM::LUCKY_MONEY_TYPE_MAX) return -__LINE__;
		if (!GetSubNodeValue(data_element, "item_id", reward_cfg.item_id) || reward_cfg.item_id < 0) return -__LINE__;
		if (reward_cfg.item_id != 0 && NULL == ITEMPOOL->GetItem(reward_cfg.item_id))
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(data_element, "start_num", reward_cfg.start_num) || reward_cfg.start_num < 0) return -__LINE__;
		if (!GetSubNodeValue(data_element, "add_num", reward_cfg.add_num) || reward_cfg.add_num < 0) return -__LINE__;


		node_cfg.reward_cfg = reward_cfg;

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}
