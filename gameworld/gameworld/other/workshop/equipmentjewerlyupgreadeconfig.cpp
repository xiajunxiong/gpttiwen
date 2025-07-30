#include "equipmentjewerlyupgreadeconfig.hpp"
#include "servercommon/servercommon.h"
#include "checkresourcecenter.hpp"
#include "servercommon/string/gameworldstr.h"
#include "servercommon/workshopdef.h"

EquipmentJewerlyUpgreadeConfig::EquipmentJewerlyUpgreadeConfig()
{
}

EquipmentJewerlyUpgreadeConfig::~EquipmentJewerlyUpgreadeConfig()
{
	for (std::map<int, std::vector<PositionSuitCfg> >::iterator it = m_suit_map.begin(); it != m_suit_map.end(); it++)
	{
		std::vector<PositionSuitCfg>().swap(it->second);
	}
}

bool EquipmentJewerlyUpgreadeConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG("equip_hole_upgrade", InitPositionCfg);
	LOAD_CONFIG("godjewelry_strengthen", InitSuitCfg);
	LOAD_CONFIG("equip_break", InitStrengthenCfg);

	return true;
}

const PositionCfg * EquipmentJewerlyUpgreadeConfig::GetPositionCfg(int profession, int hole_idx, int hole_level)
{
	std::map<int, PositionListCfg>::iterator it = m_position_map.find(profession);
	if (it == m_position_map.end())return NULL;

	if (hole_idx < 0 || hole_idx >= EQUIP_JEWELRY_TYPE_MAX)return NULL;
	
	if (hole_level <= 0 || hole_level >= EQUIPMENT_JEWERLY_HOLE_LEVEL_MAX)return NULL;

	return &(it->second.position_cfg[hole_idx][hole_level]);
}

const std::vector<PositionSuitCfg>* EquipmentJewerlyUpgreadeConfig::GetSuitListCfg(int profession)
{
	std::map<int, std::vector<PositionSuitCfg> >::iterator it = m_suit_map.find(profession);
	if (it != m_suit_map.end())
	{
		return &it->second;
	}
	return NULL;
}

const EquipJewelryStrengthenCfg * EquipmentJewerlyUpgreadeConfig::GetStrengthenCfg(int profession, int hole_idx, int hole_grade)
{
	std::map<int, PositionStrengthenListCfg>::iterator it = m_strengthen_map.find(profession);
	if (it == m_strengthen_map.end())return NULL;

	if (hole_idx < 0 || hole_idx >= EQUIP_JEWELRY_TYPE_MAX)return NULL;

	if (hole_grade <= 0 || hole_grade >= EQUIPMENT_JEWERLY_HOLE_STRENGTHEN_LEVEL_MAX)return NULL;

	if (it->second.position_cfg[hole_idx][hole_grade].grade != hole_grade || it->second.position_cfg[hole_idx][hole_grade].hole_type != hole_idx)return NULL;

	return &(it->second.position_cfg[hole_idx][hole_grade]);
}

int EquipmentJewerlyUpgreadeConfig::InitPositionCfg(TiXmlElement * RootElement)
{
	TiXmlElement* dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement)
	{
		return -1000;
	}

	while (dataElement)
	{
		short limit_prof = 0;
		if (!GetSubNodeValue(dataElement, "limit_prof", limit_prof) || limit_prof <= 0 || limit_prof >= PROF_TYPE_MAX)
		{
			return -1;
		}

		PositionListCfg &node_cfg = m_position_map[limit_prof];

		int hole_type = -1;
		if (!GetSubNodeValue(dataElement, "hole_type", hole_type) || hole_type < 0 || hole_type >= EQUIP_JEWELRY_TYPE_MAX)
		{
			return -2;
		}
		int hole_level = 0;
		if (!GetSubNodeValue(dataElement, "hole_level", hole_level) || hole_level < 0 || hole_level >= EQUIPMENT_JEWERLY_HOLE_LEVEL_MAX)
		{
			return -3;
		}
		int grade = 0;
		if (!GetSubNodeValue(dataElement, "grade", grade) || grade < 0)
		{
			return -99;
		}

		PositionCfg &node = node_cfg.position_cfg[hole_type][hole_level];
		node.is_legal = true;
		node.hole_type = hole_type;
		node.hole_level = hole_level;
		node.grade = grade;
		if (!GetSubNodeValue(dataElement, "need_equip_level", node.need_equip_level) || node.need_equip_level < 0 || node.need_equip_level > MAX_ROLE_LEVEL)
		{
			return -4;
		}

		if (!GetSubNodeValue(dataElement, "need_godequip_level", node.need_godequip_level) || node.need_godequip_level < 0 || node.need_godequip_level > MAX_GOD_EQUIP_MAX_LEVEL)
		{
			return -41;
		}

		for (int i = 0; i < PositionCfg::ATTR_NUM; i++)
		{
			std::string read_path = STRING_SPRINTF("attr_type_%d", i + 1);
			if (!GetSubNodeValue(dataElement, read_path, node.attr_type[i]) || node.attr_type[i] < 0)
			{
				return -5;
			}

			read_path = STRING_SPRINTF("attr_add_%d", i + 1);
			if (!GetSubNodeValue(dataElement, read_path, node.attr_add[i]) || node.attr_add[i] < 0)
			{
				return -6;
			}
		}

		TiXmlElement *item_list_Element = dataElement->FirstChildElement("strengthen_consume_item_list");
		if (NULL != item_list_Element)
		{
			TiXmlElement * item_element = item_list_Element->FirstChildElement("strengthen_consume_item");
			while (item_element)
			{
				ItemConfigData item_node;
				if (!item_node.ReadConfig(item_element))
				{
					return -7;
				}
				node.consume_item_list.push_back(item_node);
				item_element = item_element->NextSiblingElement();
			}
		}

		dataElement = dataElement->NextSiblingElement();
	}
	return 0;
}

int EquipmentJewerlyUpgreadeConfig::InitSuitCfg(TiXmlElement * RootElement)
{
	TiXmlElement* dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement)
	{
		return -1000;
	}

	while (dataElement)
	{
		short limit_prof = 0;
		if (!GetSubNodeValue(dataElement, "limit_prof", limit_prof) || limit_prof <= 0 || limit_prof >= PROF_TYPE_MAX)
		{
			return -1;
		}

		std::vector<PositionSuitCfg> &node_cfg = m_suit_map[limit_prof];

		int hole_level_all = -1;
		if (!GetSubNodeValue(dataElement, "hole_level_all", hole_level_all) || hole_level_all < 0)
		{
			return -2;
		}

		PositionSuitCfg node;

		node.hole_level_all = hole_level_all;

		for (int i = 0; i < PositionSuitCfg::MAX_ATTR_NUM; i++)
		{
			std::string read_path = STRING_SPRINTF("attr_type_%d", i + 1);
			if (!GetSubNodeValue(dataElement, read_path, node.attr_type[i]) || node.attr_type[i] < 0)
			{
				return -3;
			}

			read_path = STRING_SPRINTF("attr_add_%d", i + 1);
			if (!GetSubNodeValue(dataElement, read_path, node.attr_add[i]) || node.attr_add[i] < 0)
			{
				return -4;
			}
		}

		if (!GetSubNodeValue(dataElement, "sp_id", node.sp_id) || node.sp_id < 0)
		{
			return -5;
		}

		node_cfg.push_back(node);

		dataElement = dataElement->NextSiblingElement();
	}
	return 0;
}

int EquipmentJewerlyUpgreadeConfig::InitStrengthenCfg(TiXmlElement * RootElement)
{
	TiXmlElement* dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement)
	{
		return -1000;
	}

	while (dataElement)
	{
		short limit_prof = 0;
		if (!GetSubNodeValue(dataElement, "limit_prof", limit_prof) || limit_prof <= 0 || limit_prof >= PROF_TYPE_MAX)
		{
			return -1;
		}

		PositionStrengthenListCfg &node_cfg = m_strengthen_map[limit_prof];

		int hole_type = -1;
		if (!GetSubNodeValue(dataElement, "hole_type", hole_type) || hole_type < 0 || hole_type >= EQUIP_JEWELRY_TYPE_MAX)
		{
			return -2;
		}

		int grade = 0;
		if (!GetSubNodeValue(dataElement, "grade", grade) || grade < 0 || grade >= EQUIPMENT_JEWERLY_HOLE_STRENGTHEN_LEVEL_MAX)
		{
			return -99;
		}

		EquipJewelryStrengthenCfg &node = node_cfg.position_cfg[hole_type][grade];
		node.hole_type = hole_type;
		node.grade = grade;

		for (int i = 0; i < EquipJewelryStrengthenCfg::ATTR_NUM; i++)
		{
			std::string read_path = STRING_SPRINTF("attr_type_%d", i + 1);
			if (!GetSubNodeValue(dataElement, read_path, node.attr_type[i]) || node.attr_type[i] < 0)
			{
				return -5;
			}

			read_path = STRING_SPRINTF("attr_add_%d", i + 1);
			if (!GetSubNodeValue(dataElement, read_path, node.attr_add[i]) || node.attr_add[i] < 0)
			{
				return -6;
			}
		}

		TiXmlElement *item_list_Element = dataElement->FirstChildElement("strengthen_consume_item_list");
		if (NULL != item_list_Element)
		{
			TiXmlElement * item_element = item_list_Element->FirstChildElement("strengthen_consume_item");
			while (item_element)
			{
				ItemConfigData item_node;
				if (!item_node.ReadConfig(item_element))
				{
					return -7;
				}
				node.consume_item_list.push_back(item_node);
				item_element = item_element->NextSiblingElement();
			}
		}

		dataElement = dataElement->NextSiblingElement();
	}
	return 0;
}
