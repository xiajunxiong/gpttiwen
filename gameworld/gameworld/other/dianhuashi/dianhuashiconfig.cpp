#include <set>
#include "item/itempool.h"
#include "servercommon/serverconfig/battleconfig/battle_special_effect_pool.hpp"
#include "dianhuashiconfig.hpp"

DianHuaShiConfig::DianHuaShiConfig()
{
}

DianHuaShiConfig::~DianHuaShiConfig()
{
	std::vector<DianHuaSpecialEffectDetailCfg>().swap(m_skill_vec);
	std::vector<DianHuaEffectsProbabilityCfg>().swap(m_effect_pro_vec);
	std::vector<DianHuaSkillLevelCfg>().swap(m_all_skill_level_vec);
}

bool DianHuaShiConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG("other", InitOtherCfg);
	LOAD_CONFIG("effects", InitEffectDetailCfg);
	LOAD_CONFIG("effects_probability", InitEffectProCfg);
	LOAD_CONFIG("level_probability", InitEffectLevelProCfg);
	LOAD_CONFIG("skill_level", InitEffectSkillLevelCfg);

	return true;
}

const DianHuaSpecialEffectDetailCfg * DianHuaShiConfig::GetEffectDetailCfg(int effect_id, int level)
{
	for (std::vector<DianHuaSpecialEffectDetailCfg>::iterator it = m_skill_vec.begin(); it != m_skill_vec.end(); it++)
	{
		if (it->effect_level == level && it->type == effect_id)
		{
			return &(*it);
		}
	}
	return NULL;
}

const DianHuaEffectsProbabilityCfg * DianHuaShiConfig::RandEffectProCfg()
{
	if (effect_pro_weight <= 0)return NULL;
	int rand_num = rand() % effect_pro_weight;
	for (std::vector<DianHuaEffectsProbabilityCfg>::iterator it = m_effect_pro_vec.begin(); it != m_effect_pro_vec.end(); it++)
	{
		if (it->dianhua_probability >= rand_num)
		{
			return &(*it);
		}
		rand_num -= it->dianhua_probability;
	}
	return NULL;
}

const DianHuaLevelProbabilityCfg * DianHuaShiConfig::RandEffectLevelProCfg(int effect_level)
{
	for (std::map<int, DianHuaLevelProbabilityListCfg>::iterator it = m_level_pro_vec.find(effect_level); it != m_level_pro_vec.end(); it++)
	{
		if (it->second.effect_level != effect_level)continue;
		if (it->second.effect_pro_level_weight <= 0)return NULL;
		int rand_num = rand() % it->second.effect_pro_level_weight;
		for (std::vector<DianHuaLevelProbabilityCfg>::iterator level_it = it->second.level_vec.begin(); level_it != it->second.level_vec.end(); level_it++)
		{
			if (level_it->effect_probability >0 && level_it->effect_probability >= rand_num)
			{
				return &(*level_it);
			}
			rand_num -= level_it->effect_probability;
		}
	}
	return NULL;
}

const DianHuaSkillLevelCfg * DianHuaShiConfig::GetSkillLevelCfg(int all_skill_level)
{
	for (std::vector<DianHuaSkillLevelCfg>::reverse_iterator it = m_all_skill_level_vec.rbegin(); it != m_all_skill_level_vec.rend(); it++)
	{
		if (it->all_effect_level <= all_skill_level)
		{
			return &(*it);
		}
	}
	return NULL;
}

int DianHuaShiConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -1000;

	if (!GetSubNodeValue(dataElement, "open_condition", m_other_cfg.open_condition))
	{
		return -__LINE__;
	}

	if (!GetSubNodeValue(dataElement, "open_level", m_other_cfg.open_level))
	{
		return -__LINE__;
	}

	if (!GetSubNodeValue(dataElement, "fumo_id", m_other_cfg.fumo_id) || NULL == ITEMPOOL->GetItem(m_other_cfg.fumo_id))
	{
		return -__LINE__;
	}

	if (!GetSubNodeValue(dataElement, "xifu_id", m_other_cfg.xifu_id) || NULL == ITEMPOOL->GetItem(m_other_cfg.xifu_id))
	{
		return -__LINE__;
	}

	if (!GetSubNodeValue(dataElement, "dianhua_id", m_other_cfg.dianhua_id) || NULL == ITEMPOOL->GetItem(m_other_cfg.dianhua_id))
	{
		return -__LINE__;
	}

	const ItemBase * ib = ITEMPOOL->GetItem(m_other_cfg.dianhua_id);
	if (NULL == ib || ib->GetItemType() != I_TYPE_DIAN_HUA_SHI)
	{
		return -__LINE__;
	}

	return 0;
}

int DianHuaShiConfig::InitEffectDetailCfg(TiXmlElement * RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -1000;

	while (dataElement)
	{
		DianHuaSpecialEffectDetailCfg cfg;
		if (!GetSubNodeValue(dataElement, "type", cfg.type))
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(dataElement, "effect_level", cfg.effect_level))
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(dataElement, "bonus_type", cfg.bonus_type))
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(dataElement, "bonus_attributes", cfg.bonus_attributes))
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(dataElement, "equipmen_num", cfg.equipmen_num))
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(dataElement, "set_skills", cfg.set_skills))
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(dataElement, "xifu_consume", cfg.xifu_consume))
		{
			return -__LINE__;
		}

		TiXmlElement * resolve_get_item_list_element = dataElement->FirstChildElement("resolve_get_item_list");

		if (NULL != resolve_get_item_list_element)
		{
			TiXmlElement * resolve_get_item_element = resolve_get_item_list_element->FirstChildElement("resolve_get_item");
			while (resolve_get_item_element != NULL)
			{
				ItemConfigData node_item;
				if (!node_item.ReadConfig(resolve_get_item_element))
				{
					return -999;
				}
				cfg.resolve_vec.push_back(node_item);
				resolve_get_item_element = resolve_get_item_element->NextSiblingElement();
			}
		}

		m_skill_vec.push_back(cfg);

		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int DianHuaShiConfig::InitEffectProCfg(TiXmlElement * RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -1000;
	effect_pro_weight = 0;
	while (dataElement)
	{
		DianHuaEffectsProbabilityCfg cfg;
		if (!GetSubNodeValue(dataElement, "seq", cfg.seq))
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(dataElement, "dianhua_probability", cfg.dianhua_probability))
		{
			return -__LINE__;
		}
		effect_pro_weight += cfg.dianhua_probability;
		m_effect_pro_vec.push_back(cfg);

		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int DianHuaShiConfig::InitEffectLevelProCfg(TiXmlElement * RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -1000;
	while (dataElement)
	{
		int effect_level = 0;
		
		if (!GetSubNodeValue(dataElement, "effect_level", effect_level))
		{
			return -__LINE__;
		}
		DianHuaLevelProbabilityListCfg & node = m_level_pro_vec[effect_level];
		node.effect_level = effect_level;
		DianHuaLevelProbabilityCfg cfg;
		cfg.effect_level = effect_level;
		if (!GetSubNodeValue(dataElement, "effect_probability", cfg.effect_probability))
		{
			return -__LINE__;
		}
		if (!GetSubNodeValue(dataElement, "new_effect_level", cfg.new_effect_level))
		{
			return -__LINE__;
		}
		node.effect_pro_level_weight += cfg.effect_probability;
		node.level_vec.push_back(cfg);

		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int DianHuaShiConfig::InitEffectSkillLevelCfg(TiXmlElement * RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -1000;
	int pre_level = 0;
	while (dataElement)
	{
		DianHuaSkillLevelCfg cfg;
		if (!GetSubNodeValue(dataElement, "all_effect_level", cfg.all_effect_level))
		{
			return -__LINE__;
		}
		if (pre_level == 0)pre_level = cfg.all_effect_level;
		if (pre_level > cfg.all_effect_level)
		{
			return -__LINE__;//前一个一定要小于等于后一个
		}
		if (!GetSubNodeValue(dataElement, "skill_level", cfg.skill_level))
		{
			return -__LINE__;
		}

		m_all_skill_level_vec.push_back(cfg);

		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}
