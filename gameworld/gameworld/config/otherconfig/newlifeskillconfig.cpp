#include "newlifeskillconfig.hpp"

#include "servercommon/rolenewlifeskilldef.hpp"
#include "servercommon/struct/battle/battle_def.hpp"

#include "item/itempool.h"
#include "skill/skillpool.hpp"
#include "servercommon/maildef.hpp"

NewLifeSkillConfig::NewLifeSkillConfig()
{
}

NewLifeSkillConfig::~NewLifeSkillConfig()
{
	std::vector<int>().swap(m_system_food_list);
}

bool NewLifeSkillConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG("other", InitOtherCfg);
	LOAD_CONFIG("level_up", InitLevelCfg);
	LOAD_CONFIG("recipe", InitRecipeCfg);
	LOAD_CONFIG("props", InitPropsCfg);
	LOAD_CONFIG("battle_cook", InitSystemFoodCfg);
	LOAD_CONFIG("recovery", InitRecoveryCfg);

	return true;
}

const NewLifeSkillLevelCfg * NewLifeSkillConfig::GetNewLifeSkillLevelCfg(int skill_level)
{
	std::map<int, NewLifeSkillLevelCfg>::iterator it = m_new_life_skill_level_cfg.find(skill_level);
	if(it == m_new_life_skill_level_cfg.end()) return NULL;

	return &it->second;
}

const NewLifeSkillRecipeCfg * NewLifeSkillConfig::GetRecipeCfg(int sn)
{
	std::map<int, NewLifeSkillRecipeCfg>::const_iterator it = m_recipe_cfg.find(sn);
	if (it == m_recipe_cfg.end())
	{
		return NULL;
	}
	return &it->second;
}

const NewLifeSkillPropsCfg * NewLifeSkillConfig::GetPropsCfg(int seq)
{
	std::map<int, NewLifeSkillPropsCfg>::const_iterator it = m_props_cfg.find(seq);
	if (it == m_props_cfg.end())
	{
		return NULL;
	}
	return &it->second;
}

int NewLifeSkillConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * DataElement = RootElement->FirstChildElement("data");
	if (NULL == DataElement)
	{
		return -999;
	}
	if (!GetSubNodeValue(DataElement, "occupation_double_times", m_other_cfg.occupation_double_times) ||  m_other_cfg.occupation_double_times <= 0) return -1;
	if (!GetSubNodeValue(DataElement, "food_time", m_other_cfg.food_time) || m_other_cfg.food_time <= 0) return -2;
	if (!GetSubNodeValue(DataElement, "shift_card", m_other_cfg.shift_card_time) || m_other_cfg.shift_card_time <= 0) return -3;

	return 0;
}

int NewLifeSkillConfig::InitLevelCfg(TiXmlElement * RootElement)
{
	TiXmlElement * DataElement = RootElement->FirstChildElement("data");
	if (NULL == DataElement)
	{
		return -999;
	}

	while (NULL != DataElement)
	{
		int skill_level = 0;
		if (!GetSubNodeValue(DataElement, "skill_level", skill_level) || skill_level <= 0 || m_new_life_skill_level_cfg.end() != m_new_life_skill_level_cfg.find(skill_level)) return -1;

		NewLifeSkillLevelCfg node_cfg;
		if (!GetSubNodeValue(DataElement, "up_expend", node_cfg.up_expend) || node_cfg.up_expend < 0) return -2;
		if (!GetSubNodeValue(DataElement, "double_rate", node_cfg.double_rate) || node_cfg.double_rate <= 0) return -3;
		if (!GetSubNodeValue(DataElement, "pro_double_rate", node_cfg.prof_double_rate) || node_cfg.prof_double_rate <= 0) return -4;
		if (!GetSubNodeValue(DataElement, "up_expend_medicine", node_cfg.up_expend_medicine) || node_cfg.up_expend_medicine < 0) return -5;
		if (!GetSubNodeValue(DataElement, "up_expend_cook", node_cfg.up_expend_cook) || node_cfg.up_expend_cook < 0) return -6;

		m_new_life_skill_level_cfg[skill_level] = node_cfg;
		DataElement = DataElement->NextSiblingElement();
	}

	return 0;
}

int NewLifeSkillConfig::InitRecipeCfg(TiXmlElement * RootElement)
{
	TiXmlElement * DataElement = RootElement->FirstChildElement("data");
	if (NULL == DataElement)
	{
		return -999;
	}

	while(NULL != DataElement)
	{
		int sn = 0;
		if (!GetSubNodeValue(DataElement, "sn", sn) || sn <= 0 || m_recipe_cfg.end() != m_recipe_cfg.find(sn)) return -1;

		NewLifeSkillRecipeCfg node_cfg;
		if (!GetSubNodeValue(DataElement, "work_type", node_cfg.new_life_skill_type_limit) || !this->IsNewLifeProf(node_cfg.new_life_skill_type_limit)) return -2;
		if (!GetSubNodeValue(DataElement, "work_level", node_cfg.new_life_skill_level_limit) || node_cfg.new_life_skill_level_limit <= 0) return -3;
	
		TiXmlElement * role_prof_element = DataElement->FirstChildElement("role_type");
		if (NULL == role_prof_element)
		{
			return -4;
		}
		TiXmlElement * node_data_element = role_prof_element->FirstChildElement("node");
		if (NULL == node_data_element)
		{
			return -5;
		}
		while (NULL != node_data_element)
		{
			int role_prof_type = 0;
			if (!GetNodeValue(node_data_element, role_prof_type) || role_prof_type <= 0 || role_prof_type >= PROFESSION_TYPE_NUM)
			{
				return -6;
			}
			if (node_cfg.role_prof_set.end() != node_cfg.role_prof_set.find(role_prof_type))
			{
				return -7;
			}

			node_cfg.role_prof_set.insert(role_prof_type);
			node_data_element = node_data_element->NextSiblingElement();
		}

		if (!GetSubNodeValue(DataElement, "materials_num", node_cfg.need_item_count) || node_cfg.need_item_count <= 0 || node_cfg.need_item_count > NewLifeSkillRecipeCfg::MAX_NEED_ITEM_COUNT) return -8;
		char materials_item_buf[64] = {0};
		char materials_num_buf[64] = {0};
		for (int i = 0; i < node_cfg.need_item_count && i < ARRAY_LENGTH(node_cfg.need_item_list); i++)
		{
			SNPRINTF(materials_item_buf, sizeof(materials_item_buf), "materials_%d", i + 1);
			SNPRINTF(materials_num_buf, sizeof(materials_num_buf), "num%d", i + 1);
			if (!GetSubNodeValue(DataElement, materials_item_buf, node_cfg.need_item_list[i].item_id) || NULL == ITEMPOOL->GetItem(node_cfg.need_item_list[i].item_id))
			{
				return -(30 + i);
			}
			if (!GetSubNodeValue(DataElement, materials_num_buf, node_cfg.need_item_list[i].num) || node_cfg.need_item_list[i].num <= 0)
			{
				return -(40 + i);
			}
		}

		if (!GetSubNodeValue(DataElement, "expend_active", node_cfg.expend_active) || node_cfg.expend_active <= 0) return -9;
		if (!GetSubNodeValue(DataElement, "compose_item", node_cfg.compose_item_id) || NULL == ITEMPOOL->GetItem(node_cfg.compose_item_id)) return -10;

		m_recipe_cfg[sn] = node_cfg;
		DataElement = DataElement->NextSiblingElement();
	}

	return 0;
}

int NewLifeSkillConfig::InitPropsCfg(TiXmlElement * RootElement)
{
	TiXmlElement * DataElement = RootElement->FirstChildElement("data");
	if (NULL == DataElement)
	{
		return -999;
	}

	std::set<int> change_card_list;
	while (NULL != DataElement)
	{
		int seq = 0;
		if (!GetSubNodeValue(DataElement, "seq", seq) || seq <= 0 || m_props_cfg.end() != m_props_cfg.find(seq) || seq >= MAX_INT16) return -1;

		NewLifeSkillPropsCfg node_cfg;
		if (!GetSubNodeValue(DataElement, "type", node_cfg.type)) return -2;
		int shift_card_res = 0;
		if (!GetSubNodeValue(DataElement, "shift_card_res", shift_card_res)) return -3;
		if (!GetSubNodeValue(DataElement, "expend_point", node_cfg.expend_point)) return -30;
		if (!GetSubNodeValue(DataElement, "increase_point", node_cfg.increase_point)) return -31;
		if (!GetSubNodeValue(DataElement, "skill_id", node_cfg.skill_id) || node_cfg.skill_id < 0) return -32;
		if (!GetSubNodeValue(DataElement, "skill_point", node_cfg.use_skill_need_point) || node_cfg.use_skill_need_point < 0) return -33;

		switch (node_cfg.type)
		{
			case NEW_LIFE_SKILL_PROPS_TYPE_CHANGE_CARD:
			{
				if (0 == shift_card_res) return -4;
				if (node_cfg.expend_point <= 0) return -50;
				if (node_cfg.increase_point <= 0) return -51;
				if (node_cfg.skill_id > 0)
				{
					if (NULL == SkillPool::Instance().GetPassiveSkillCfg(node_cfg.skill_id))
					{
						if (NULL == SkillPool::Instance().GetSkillCfg(node_cfg.skill_id) || node_cfg.use_skill_need_point <= 0)
						{
							return -52;
						}
					}
				}
				change_card_list.insert(seq);
			}
			break;
			case NEW_LIFE_SKILL_PROPS_TYPE_FOOD_0:
			case NEW_LIFE_SKILL_PROPS_TYPE_FOOD_1:
			case NEW_LIFE_SKILL_PROPS_TYPE_FOOD_2:
			case NEW_LIFE_SKILL_PROPS_TYPE_FOOD_3:
			case NEW_LIFE_SKILL_PROPS_TYPE_FOOD_4:
			case NEW_LIFE_SKILL_PROPS_TYPE_FOOD_5:
			{
				if (0 != shift_card_res) return -5;
				if (0 != node_cfg.skill_id) return -40;
			}
			break;
			default:
			return -6;
		}

		if (!GetSubNodeValue(DataElement, "attribute_num", node_cfg.attr_count) || node_cfg.attr_count <= 0 || node_cfg.attr_count > NewLifeSkillPropsCfg::MAX_ATTR_NUM) return -7;

		char buf[64] = { 0 };
		char buf1[64] = { 0 };
		for (int i = 0; i < node_cfg.attr_count && i < ARRAY_LENGTH(node_cfg.attr_list); i++)
		{
			SNPRINTF(buf, sizeof(buf), "attr_type_%d", i + 1);
			SNPRINTF(buf1, sizeof(buf1), "attribute_value_%d", i + 1);

			if (!GetSubNodeValue(DataElement, buf, node_cfg.attr_list[i].attr_type) || node_cfg.attr_list[i].attr_type < 0 || node_cfg.attr_list[i].attr_type >= BATTLE_ATTR_MAX) return -(10 + i);
			if (!GetSubNodeValue(DataElement, buf1, node_cfg.attr_list[i].attr_value)) return -(20 + i);
		}

		m_props_cfg[seq] = node_cfg;
		DataElement = DataElement->NextSiblingElement();
	}

	if ((int)change_card_list.size() > MAX_NEW_LIFE_SKILL_CHANGE_SURFACE_NUM)
	{
		return -200;
	}

	return 0;
}

int NewLifeSkillConfig::InitSystemFoodCfg(TiXmlElement* RootElement)
{
	TiXmlElement * DataElement = RootElement->FirstChildElement("data");
	if (NULL == DataElement)
	{
		return -999;
	}

	m_system_food_list.reserve(6);
	while (DataElement)
	{
		int food_seq = 0;
		if (!GetSubNodeValue(DataElement, "seq", food_seq) || NULL == this->GetPropsCfg(food_seq))
		{
			return -1;
		}

		m_system_food_list.push_back(food_seq);

		DataElement = DataElement->NextSiblingElement();
	}

	return 0;
}

int NewLifeSkillConfig::InitRecoveryCfg(TiXmlElement * RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement)
	{
		return -999;
	}

	while (NULL != dataElement)
	{
		int change_card_seq = 0;
		if (!GetSubNodeValue(dataElement, "change_seq", change_card_seq))
		{
			return -1;
		}
		const NewLifeSkillPropsCfg * props_cfg = this->GetPropsCfg(change_card_seq);
		if (NULL == props_cfg || NEW_LIFE_SKILL_PROPS_TYPE_CHANGE_CARD != props_cfg->type)
		{
			return -2;
		}
		if (m_recovery_map.end() != m_recovery_map.find(change_card_seq))
		{
			return -3;
		}

		NewLifeSkillRecoveryCfg node_cfg;
		if (!GetSubNodeValue(dataElement, "price_type", node_cfg.mail_virtual_type) || node_cfg.mail_virtual_type < MAIL_VIRTUAL_ITEM_BEGIN || node_cfg.mail_virtual_type >= MAIL_VIRTUAL_ITEM_MAX)
		{
			return -4;
		}
		if (!GetSubNodeValue(dataElement, "price", node_cfg.price) || node_cfg.price <= 0)
		{
			return -5;
		}

		m_recovery_map[change_card_seq] = node_cfg;
		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

bool NewLifeSkillConfig::IsNewLifeProf(int new_life_skill_type)
{
	bool ret = false;
	UNSTD_STATIC_CHECK(NEW_LIFE_SKILL_TYPE_MAX == 6);
	switch (new_life_skill_type)
	{
		case NEW_LIFE_SKILL_TYPE_PHARMACIST:
		case NEW_LIFE_SKILL_TYPE_COOK:
		{
			ret = true;
		}
		break;
		default:
		break;
	}

	return ret;
}

const NewLifeSkillRecoveryCfg * NewLifeSkillConfig::GetRecoveryCfg(int change_card_seq) const
{
	std::map<int, NewLifeSkillRecoveryCfg>::const_iterator it = m_recovery_map.find(change_card_seq);
	if (it == m_recovery_map.end())
	{
		return NULL;
	}

	return &it->second;
}

