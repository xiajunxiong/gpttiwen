#include "petequipmentconfig.hpp"
#include "equipment/jewelry.h"
#include "item/itempool.h"
#include "item/itembase.h"

PetEquipMentConfig::PetEquipMentConfig()
{
}

PetEquipMentConfig::~PetEquipMentConfig()
{
}

bool PetEquipMentConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG("number_id", InitNumberGroup);
	LOAD_CONFIG("skill_group", InitSkillGroup);
	LOAD_CONFIG("synthesis", InitCompoundGroup);
	LOAD_CONFIG("refining", InitRefiningCfg);
	LOAD_CONFIG("item_group", InitCompoundItemRandGroup);

	//this->InitJewelryMap();			// 从 ITEMPOOL 拉取灵饰相关信息
	return true;
}

const PetEquipMentInfo * PetEquipMentConfig::GetPetEquipMentInfo(int seq)
{
	std::map<int, PetEquipMentInfo>::iterator it = m_pet_equipment_map.find(seq);
	if (it != m_pet_equipment_map.end())
	{
		return &it->second;
	}
	return NULL;
}

const PetEquipMentNumber * PetEquipMentConfig::GetPetEquipMentRankSkillNumber(int seq)
{
	std::map<int, PetEquipMentNumberGroup>::iterator it = m_pet_equipment_number_map.find(seq);
	if (it != m_pet_equipment_number_map.end())
	{
		if (it->second.weight_count <= 0)return NULL;
		int rand_num = rand() % it->second.weight_count;
		for (std::vector<PetEquipMentNumber>::iterator itor = it->second.rand_vec.begin(); itor != it->second.rand_vec.end(); itor++)
		{
			if (itor->weight > rand_num)
			{
				return &(*itor);
			}
			else
			{
				rand_num -= itor->weight;
			}
		}
	}
	return NULL;
}

const PetEquipMentSkill * PetEquipMentConfig::GetPetEquipMentRandSkill(int seq)
{
	std::map<int, PetEquipMentSkillGroup>::iterator it = m_pet_equipment_skill_map.find(seq);
	if (it != m_pet_equipment_skill_map.end())
	{
		if (it->second.weight_count <= 0)return NULL;
		int rand_num = rand() % it->second.weight_count;
		for (std::vector<PetEquipMentSkill>::iterator itor = it->second.rand_vec.begin(); itor != it->second.rand_vec.end(); itor++)
		{
			if (itor->weight > rand_num)
			{
				return &(*itor);
			}
			else
			{
				rand_num -= itor->weight;
			}
		}
	}
	return NULL;
}

const PetEquipMentSkillGroup * PetEquipMentConfig::GetSkillRandMap(int seq)
{
	std::map<int, PetEquipMentSkillGroup>::iterator it = m_pet_equipment_skill_map.find(seq);
	if (it != m_pet_equipment_skill_map.end())
	{
		return &it->second;
	}
	return NULL;
}

const PetEquipMentCompoundItemRand * PetEquipMentConfig::GetPetEquipMentRandCompoundItem(int seq)
{
	std::map<int, PetEquipMentCompoundItemRandGroup>::iterator it = m_pet_equipment_compound_item_map.find(seq);
	if (it != m_pet_equipment_compound_item_map.end())
	{
		if (it->second.weight_count <= 0)return NULL;
		int rand_num = rand() % it->second.weight_count;
		for (std::vector<PetEquipMentCompoundItemRand>::iterator itor = it->second.rand_vec.begin(); itor != it->second.rand_vec.end(); itor++)
		{
			if (itor->weight > rand_num)
			{
				return &(*itor);
			}
			else
			{
				rand_num -= itor->weight;
			}
		}
	}
	return NULL;
}

const PetEquipMentCompoundItemGroup * PetEquipMentConfig::GetPetEquipMentCompound(int seq)
{
	std::map<int, PetEquipMentCompoundItemGroup>::iterator it = m_pet_equipment_compound_map.find(seq);
	if (it != m_pet_equipment_compound_map.end())
	{
		return &it->second;
	}
	return NULL;
}

const PetEquipMentRefiningGroup * PetEquipMentConfig::GetPetEquipMentRefiningCfg(int seq)
{
	std::map<int, PetEquipMentRefiningGroup>::iterator it = m_pet_equipment_refining_map.find(seq);
	if (it != m_pet_equipment_refining_map.end())
	{
		return &it->second;
	}
	return NULL;
}

int PetEquipMentConfig::InitNumberGroup(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		int seq = 0;
		if (!GetSubNodeValue(root_element, "seq", seq) || seq < 0)
		{
			return -1;
		}
		PetEquipMentNumberGroup &node_cfg = m_pet_equipment_number_map[seq];
		PetEquipMentNumber node;
		if (!GetSubNodeValue(root_element, "number", node.number))
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "weight", node.weight))
		{
			return -3;
		}

		node_cfg.weight_count += node.weight;
		node_cfg.rand_vec.push_back(node);
		root_element = root_element->NextSiblingElement();
	}
	return 0;
}

int PetEquipMentConfig::InitSkillGroup(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		int seq = 0;
		if (!GetSubNodeValue(root_element, "seq", seq) || seq < 0)
		{
			return -1;
		}
		PetEquipMentSkillGroup &node_cfg = m_pet_equipment_skill_map[seq];
		PetEquipMentSkill node;
		if (!GetSubNodeValue(root_element, "gift_id", node.gift_id))
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "weight", node.weight))
		{
			return -3;
		}

		node_cfg.weight_count += node.weight;
		node_cfg.rand_vec.push_back(node);
		root_element = root_element->NextSiblingElement();
	}
	return 0;
}

int PetEquipMentConfig::InitCompoundItemRandGroup(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		int seq = 0;
		if (!GetSubNodeValue(root_element, "seq", seq) || seq < 0)
		{
			return -1;
		}
		PetEquipMentCompoundItemRandGroup &node_cfg = m_pet_equipment_compound_item_map[seq];
		PetEquipMentCompoundItemRand node;
		if (!GetSubNodeValue(root_element, "target", node.target))
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "weight", node.weight))
		{
			return -3;
		}

		node_cfg.weight_count += node.weight;
		node_cfg.rand_vec.push_back(node);
		root_element = root_element->NextSiblingElement();
	}
	return 0;
}

int PetEquipMentConfig::InitCompoundGroup(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		int seq = 0;
		if (!GetSubNodeValue(root_element, "seq", seq) || seq < 0)
		{
			return -1;
		}
		PetEquipMentCompoundItemGroup & node_cfg = m_pet_equipment_compound_map[seq];

		if (!GetSubNodeValue(root_element, "stuff_id_1", node_cfg.stuff_id_list[0]))
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "stuff_num_1", node_cfg.stuff_num_list[0]))
		{
			return -3;
		}

		if (!GetSubNodeValue(root_element, "stuff_id_2", node_cfg.stuff_id_list[1]))
		{
			return -4;
		}

		if (!GetSubNodeValue(root_element, "stuff_num_2", node_cfg.stuff_num_list[1]))
		{
			return -5;
		}

		if (!GetSubNodeValue(root_element, "target", node_cfg.target))
		{
			return -6;
		}

		//检查
		for (int i = 0; i < PetEquipMentCompoundItemGroup::MAX_PET_EQUIPMENT_COMPOUND_NUM; i++)
		{
			if (node_cfg.stuff_id_list[i] != 0)
			{
				if (node_cfg.stuff_num_list[i] != 1)return -99;//消耗的是单件装备数量必须是1
			}
		}

		root_element = root_element->NextSiblingElement();
	}
	return 0;
}

int PetEquipMentConfig::InitRefiningCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		int seq = 0;
		if (!GetSubNodeValue(root_element, "seq", seq) || seq < 0)
		{
			return -1;
		}
		PetEquipMentRefiningGroup & node_cfg = m_pet_equipment_refining_map[seq];

		if (!GetSubNodeValue(root_element, "id", node_cfg.id))
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "item_id", node_cfg.item_id))
		{
			return -3;
		}

		if (!GetSubNodeValue(root_element, "number", node_cfg.number))
		{
			return -4;
		}

		root_element = root_element->NextSiblingElement();
	}
	return 0;
}