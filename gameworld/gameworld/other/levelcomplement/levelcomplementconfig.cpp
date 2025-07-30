#include "levelcomplementconfig.hpp"

#include "servercommon/struct/moneyotherdef.h"

RoleLevelComplementConfig::RoleLevelComplementConfig() : m_max_level_difference(0)
{
}

RoleLevelComplementConfig::~RoleLevelComplementConfig()
{
}

bool RoleLevelComplementConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG("exp_cfg", InitLevelCfg);
	LOAD_CONFIG("exp_cfg_1", InitNewLevelCfg);
	LOAD_CONFIG("other", InitOtherCfg);

	return true;
}

const RoleLevelComplementCfg * RoleLevelComplementConfig::GetLevelCfg(int day_difference)
{
	std::map<int, RoleLevelComplementCfg>::iterator it = m_role_level_map.find(day_difference);
	if (it != m_role_level_map.end())
	{
		return &it->second;
	}
	//找不到检查day_diff
	if (day_difference > 0)
	{
		//如果天数大于0则是时间过长，直接返回最大配置
		std::map<int, RoleLevelComplementCfg>::reverse_iterator rb_it = m_role_level_map.rbegin();
		return &rb_it->second;
	}
	return NULL;
}

const RoleNewLevelComplementCfg * RoleLevelComplementConfig::GetNewLevelCfg(int level_difference) const
{
	if (level_difference <= 0 || m_max_level_difference <= 0 || m_max_level_difference >= ARRAY_LENGTH(m_new_level_complement_list) ||
		level_difference >= ARRAY_LENGTH(m_new_level_complement_list)) return NULL;

	if (level_difference > m_max_level_difference)
	{
		return &m_new_level_complement_list[m_max_level_difference];
	}

	return &m_new_level_complement_list[level_difference];
}

int RoleLevelComplementConfig::InitLevelCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	while (NULL != root_element)
	{
		int day_difference = 0;
		if (!GetSubNodeValue(root_element, "day_difference", day_difference) || day_difference <= 0)
		{
			return -1;
		}
		RoleLevelComplementCfg &node_cfg = m_role_level_map[day_difference];
		node_cfg.day_difference = day_difference;

		if (!GetSubNodeValue(root_element, "complement_day", node_cfg.complement_day) || node_cfg.complement_day <= 0)
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "add_exp_num", node_cfg.add_exp_num) || node_cfg.add_exp_num <= 0)
		{
			return -3;
		}

		if (!GetSubNodeValue(root_element, "first_day_exp", node_cfg.first_day_exp) || node_cfg.first_day_exp <= 0)
		{
			return -4;
		}

		if (!GetSubNodeValue(root_element, "second_day_exp", node_cfg.second_day_exp) || node_cfg.second_day_exp <= 0)
		{
			return -41;
		}

		if (!GetSubNodeValue(root_element, "exp", node_cfg.exp) || node_cfg.exp <= 0)
		{
			return -5;
		}

		if (!GetSubNodeValue(root_element, "exp_limit", node_cfg.exp_limit) || node_cfg.exp_limit <= 0)
		{
			return -6;
		}
		
		if (!GetSubNodeValue(root_element, "multiple", node_cfg.multiple) || node_cfg.multiple <= 0)
		{
			return -7;
		}

		if (!GetSubNodeValue(root_element, "buy_exp", node_cfg.buy_exp) || node_cfg.buy_exp <= 0)
		{
			return -8;
		}

		if (!GetSubNodeValue(root_element, "price_type", node_cfg.price_type) || node_cfg.price_type < 0)
		{
			return -9;
		}

		if (!GetSubNodeValue(root_element, "price", node_cfg.price) || node_cfg.price <= 0)
		{
			return -10;
		}

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int RoleLevelComplementConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	if (!GetSubNodeValue(root_element, "rank_difference", m_other_cfg.rank_level_difference) || m_other_cfg.rank_level_difference <= 0 || 
		m_other_cfg.rank_level_difference >= MAX_ROLE_LEVEL)
	{
		return -1;
	}

	if (!GetSubNodeValue(root_element, "ranking", m_other_cfg.ranking) || m_other_cfg.ranking <= 0 || m_other_cfg.ranking > MAX_LEVEL_COMPLEMENT_RANK_NUM)
	{
		return -2;
	}

	if (!GetSubNodeValue(root_element, "buy_num", m_other_cfg.day_buy_num) || m_other_cfg.day_buy_num <= 0)
	{
		return -3;
	}

	return 0;
}

int RoleLevelComplementConfig::InitNewLevelCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -999;
	}

	m_max_level_difference = 0;
	while (NULL != data_element)
	{
		int level_difference = 0;
		if (!GetSubNodeValue(data_element, "level_difference", level_difference) || level_difference != (1 + m_max_level_difference) || 
			level_difference > MAX_ROLE_LEVEL)
		{
			return -1;
		}
		m_max_level_difference = level_difference;
		
		RoleNewLevelComplementCfg node_cfg;
		
		if (!GetSubNodeValue(data_element, "exp", node_cfg.day_add_exp_coefficient) || node_cfg.day_add_exp_coefficient <= 0)
		{
			return -2;
		}

		if (!GetSubNodeValue(data_element, "exp_limit", node_cfg.exp_limit_coefficient) || node_cfg.exp_limit_coefficient < node_cfg.day_add_exp_coefficient)
		{
			return -3;
		}
		int max_multiple_coefficient = 0;
		for (int i = 0; i < ARRAY_LENGTH(node_cfg.multiple_coefficient_list); ++i)
		{
			std::string buf;
			if (0 == i)
			{
				buf = "multiple";
			}
			else
			{
				buf = STRING_SPRINTF("multiple_%d", i);
			}
			if (!GetSubNodeValue(data_element, buf, node_cfg.multiple_coefficient_list[i]) || node_cfg.multiple_coefficient_list[i] <= 0)
			{
				return -(30 + i);
			}
			if (max_multiple_coefficient > node_cfg.multiple_coefficient_list[i])
			{
				return -(40 + i);
			}
			max_multiple_coefficient = node_cfg.multiple_coefficient_list[i];
		}

		if (!GetSubNodeValue(data_element, "buy_exp", node_cfg.buy_exp_coefficient) || node_cfg.buy_exp_coefficient <= 0)
		{
			return -4;
		}

		if (!GetSubNodeValue(data_element, "price_type", node_cfg.money_type) || node_cfg.money_type <= MONEY_TYPE_GOLD || 
			node_cfg.money_type >= MONEY_TYPE_MAX)
		{
			return -5;
		}

		if (!GetSubNodeValue(data_element, "price", node_cfg.price) || node_cfg.price <= 0)
		{
			return -6;
		}

		m_new_level_complement_list[level_difference] = node_cfg;
		data_element = data_element->NextSiblingElement();
	}
	return 0;
}
