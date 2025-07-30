#include "petsoulequipmentconfig.hpp"
#include "item/itempool.h"
#include "item/itembase.h"

PetSoulEquipMentConfig::PetSoulEquipMentConfig()
{
}

PetSoulEquipMentConfig::~PetSoulEquipMentConfig()
{
	std::vector<AttributeUplevelCfg>().swap(m_soul_uplevel_cfg);
	std::vector<PetEquipSoulStepsCfg>().swap(m_soul_steps_cfg);
	std::vector<PetSoulEquipItemIdExpCfg>().swap(m_item_exp_cfg);
}

bool PetSoulEquipMentConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG("other", InitOtherCfg);
	LOAD_CONFIG("pet_weapon_cfg", InitBaseItemGroup);
	LOAD_CONFIG("attribute_num_group", InitAttributeNumGroup);
	LOAD_CONFIG("att_group", InitAttributeTypeValueGroup);
	LOAD_CONFIG("intensify_att", InitAttributeTypeIntensifyCfg);
	LOAD_CONFIG("pet_weapon_intensify", InitAttributeUplevelCfg);
	LOAD_CONFIG("pet_weapon_steps", InitEquipSoulStepsCfg);
	LOAD_CONFIG("steps_att", InitEquipSoulStepsValueCfg);
	LOAD_CONFIG("skill_buff", InitEquipSoulSpidCfg);
	LOAD_CONFIG("pet_weapon_refine", InitEquipSoulRefineCfg);
	LOAD_CONFIG("item_id_exp", InitItemIdExpCfg);
	LOAD_CONFIG("pet_weapon_exchange", InitPetEquipSoulExchangeCfg);
	LOAD_CONFIG("sp_priority", InitPetEquipSoulPriority);

	return true;
}

const PetSoulEquipBaseCfg * PetSoulEquipMentConfig::GetPetSoulEquipBaseCfg(int seq)
{
	std::map<int, PetSoulEquipBaseCfg>::iterator it = m_base_cfg.find(seq);
	if (it != m_base_cfg.end())
	{
		return &it->second;
	}
	return NULL;
}

const PetSoulSpPriority * PetSoulEquipMentConfig::GetPetSoulSpPriority(int sp_id)
{
	return MapValuesConstPtr(m_pet_soul_sp_priority_map, sp_id);
}


const AttributeNumGroup * PetSoulEquipMentConfig::GetPetSoulEquipAttrNumCfg(int group_id)
{
	std::map<int, AttributeNumGroup>::iterator it = m_attr_num_cfg.find(group_id);
	if (it != m_attr_num_cfg.end())
	{
		return &it->second;
	}
	return NULL;
}

const AttributeTypeValueDetailGroup * PetSoulEquipMentConfig::RandAttrTypeValueCfg(int group_id, int attr_idx)
{
	std::map<int, AttributeTypeValueGroup>::iterator it = m_attr_type_cfg.find(group_id);
	if (it != m_attr_type_cfg.end())
	{
		std::map<int, AttributeTypeValueSeqGroup>::iterator second_it = it->second.attribute_list.find(attr_idx);
		if (second_it != it->second.attribute_list.end())
		{
			int rand_attr = 0;
			if (second_it->second.weight > 0)
			{
				rand_attr = rand() % second_it->second.weight;
			}
			else
			{
				rand_attr = 1;
			}
			for (std::vector<AttributeTypeValueDetailGroup>::iterator trd_it = second_it->second.m_attribute_vec.begin(); trd_it != second_it->second.m_attribute_vec.end(); trd_it++)
			{
				if (trd_it->rate_10000 >= rand_attr)
				{
					return &(*trd_it);
				}
				rand_attr -= trd_it->rate_10000;
			}
		}
	}
	return NULL;
}

const AttributeIntensifyValueCfg * PetSoulEquipMentConfig::GetPetSoulEquipIntensifyValueCfg(int quality, int attr_type)
{
	std::map<int, AttributeIntensifyValueTypeCfg>::iterator it = m_intensify_cfg.find(quality);
	if (it != m_intensify_cfg.end())
	{
		for (std::vector<AttributeIntensifyValueCfg>::iterator sec_it = it->second.int_vec.begin(); sec_it != it->second.int_vec.end(); sec_it++)
		{
			if (sec_it->att_type == attr_type)
			{
				return &(*sec_it);
			}
		}
	}
	return NULL;
}

const AttributeUplevelCfg * PetSoulEquipMentConfig::GetPetSoulEquipUpLevelCfg(int quality, int level)
{
	for (std::vector<AttributeUplevelCfg>::iterator it = m_soul_uplevel_cfg.begin(); it != m_soul_uplevel_cfg.end(); it++)
	{
		if (it->quality == quality && it->levels == level)
		{
			return &(*it);
		}
	}
	return NULL;
}

const PetEquipSoulStepsCfg * PetSoulEquipMentConfig::GetPetSoulEquipStepsCfg(int quality, int steps)
{
	for (std::vector<PetEquipSoulStepsCfg>::iterator it = m_soul_steps_cfg.begin(); it != m_soul_steps_cfg.end(); it++)
	{
		if (it->quality == quality && it->steps == steps)
		{
			return &(*it);
		}
	}
	return NULL;
}

const PetSoulEquipStepsAttrCfg * PetSoulEquipMentConfig::GetPetSoulEquipStepsAddValueCfg(int quality, int attr_type)
{
	std::map<int, PetSoulEquipStepsTypeCfg>::iterator it = m_pet_soul_equip_steps_cfg.find(quality);
	if (it != m_pet_soul_equip_steps_cfg.end())
	{
		for (std::vector<PetSoulEquipStepsAttrCfg>::iterator sec_it = it->second.int_vec.begin(); sec_it != it->second.int_vec.end(); sec_it++)
		{
			if (sec_it->att_type == attr_type)
			{
				return &(*sec_it);
			}
		}
	}
	return NULL;
}

const PetSoulEquipSpidCfg * PetSoulEquipMentConfig::GetPetSoulEquipSpidCfg(int seq)
{
	std::map<int, PetSoulEquipSpidGroupCfg>::iterator it = m_pet_soul_equip_spid_cfg.find(seq);
	if (it != m_pet_soul_equip_spid_cfg.end())
	{
		if (it->second.weight > 0)
		{
			int rand_num = rand() % it->second.weight;
			for (std::vector<PetSoulEquipSpidCfg>::iterator sec_it = it->second.m_spid_cfg.begin(); sec_it != it->second.m_spid_cfg.end(); sec_it++)
			{
				if (sec_it->rate >= rand_num)
				{
					return &(*sec_it);
				}
				rand_num -= sec_it->rate;
			}
		}
	}
	return NULL;
}

const PetEquipSoulRefineCfg * PetSoulEquipMentConfig::GetPetSoulEquipRefineCfg(int seq, int refine)
{
	std::map<int, PetEquipSoulRefineGroupCfg>::iterator it = m_refine_cfg.find(seq);
	if (it != m_refine_cfg.end())
	{
		for (std::vector<PetEquipSoulRefineCfg>::iterator sec_it = it->second.m_refine_vec.begin(); sec_it != it->second.m_refine_vec.end(); sec_it++)
		{
			if (sec_it->refine == refine)
			{
				return &(*sec_it);
			}
		}
	}
	return NULL;
}

const PetSoulEquipItemIdExpCfg * PetSoulEquipMentConfig::GetPetSoulEquipItemIdExpCfg(int item_id)
{
	for (std::vector<PetSoulEquipItemIdExpCfg>::iterator it = m_item_exp_cfg.begin(); it != m_item_exp_cfg.end(); it++)
	{
		if (it->item_id == item_id)
		{
			return &(*it);
		}
	}
	return NULL;
}

const PetEquipSoulExchangeCfg * PetSoulEquipMentConfig::GetEquipSoulExchangeCfg(int seq)
{
	std::map<int, PetEquipSoulExchangeCfg>::iterator it = m_exchange_vec.find(seq);
	if (it != m_exchange_vec.end())
	{
		return &it->second;
	}
	return NULL;
}

void PetSoulEquipMentConfig::GetBackItemByExtraExp(int extra_exp, std::vector<ItemConfigData> * back_vec)
{
	if (NULL == back_vec)return;
	std::vector<PetSoulEquipItemIdExpCfg>::iterator begin_it = m_item_exp_cfg.begin();
	if (begin_it == m_item_exp_cfg.end())return;
	int max_round = 1000;//限制最大循环次数
	for (int round = 0; extra_exp >= begin_it->item_exp && round <= max_round; round++)
	{
		int max_exp = 0;
		int max_item_id = 0;
		for (std::vector<PetSoulEquipItemIdExpCfg>::iterator it = m_item_exp_cfg.begin(); it != m_item_exp_cfg.end(); it++)
		{
			if (it->item_exp >= extra_exp)break;
			if (max_exp < it->item_exp)
			{
				max_exp = it->item_exp;
				max_item_id = it->item_id;
			}
		}

		if (max_exp > 0)
		{
			int back_num = extra_exp / max_exp;
			extra_exp -= max_exp * back_num;
			ItemConfigData back_item_node;
			back_item_node.item_id = max_item_id;
			back_item_node.num = back_num;
			back_vec->push_back(back_item_node);
		}
	}
}

int PetSoulEquipMentConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	if (!GetSubNodeValue(root_element, "exp_attenuation_100", m_other_cfg.exp_attenuation_100) || m_other_cfg.exp_attenuation_100 < 0)
	{
		return -1;
	}

	if (!GetSubNodeValue(root_element, "need_coin_100_exp", m_other_cfg.cost_coin) || m_other_cfg.cost_coin < 0)
	{
		return -2;
	}
	
	return 0;
}

int PetSoulEquipMentConfig::InitBaseItemGroup(TiXmlElement * RootElement)
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
		PetSoulEquipBaseCfg &node_cfg = m_base_cfg[seq];

		if (!GetSubNodeValue(root_element, "type", node_cfg.type))
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "quality", node_cfg.quality))
		{
			return -3;
		}

		if (!GetSubNodeValue(root_element, "level_max", node_cfg.level_max))
		{
			return -4;
		}

		if (!GetSubNodeValue(root_element, "intensify_group", node_cfg.intensify_group))
		{
			return -5;
		}

		if (!GetSubNodeValue(root_element, "steps_group", node_cfg.steps_group))
		{
			return -6;
		}

		if (!GetSubNodeValue(root_element, "refine_group", node_cfg.refine_group))
		{
			return -7;
		}

		if (!GetSubNodeValue(root_element, "attribute_num_group", node_cfg.attribute_num))
		{
			return -8;
		}

		if (!GetSubNodeValue(root_element, "attribute_group", node_cfg.attribute_group))
		{
			return -9;
		}

		if (!GetSubNodeValue(root_element, "pet_weapon_skill", node_cfg.pet_weapon_skill))
		{
			return -10;
		}

		if (!GetSubNodeValue(root_element, "pet_weapon_item", node_cfg.pet_weapon_item))
		{
			return -11;
		}

		if (!GetSubNodeValue(root_element, "levels_1_pet_weapon_exp", node_cfg.levels_1_pet_weapon_exp))
		{
			return -12;
		}

		if (!GetSubNodeValue(root_element, "pet_weapon_type", node_cfg.pet_weapon_type))
		{
			return -13;
		}

		if (!GetSubNodeValue(root_element, "pet_weapon_pet_id", node_cfg.pet_weapon_pet_id))
		{
			return -14;
		}

		if (!GetSubNodeValue(root_element, "change_item", node_cfg.change_item))
		{
			return -15;
		}

		if (!GetSubNodeValue(root_element, "change_item_num", node_cfg.change_item_num))
		{
			return -16;
		}
		
		root_element = root_element->NextSiblingElement();
	}
	return 0;
}

int PetSoulEquipMentConfig::InitAttributeNumGroup(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		int attribute_num_group = 0;
		if (!GetSubNodeValue(root_element, "attribute_num_group", attribute_num_group) || attribute_num_group < 0)
		{
			return -1;
		}
		AttributeNumGroup &node_cfg = m_attr_num_cfg[attribute_num_group];
		node_cfg.attribute_num_group = attribute_num_group;

		if (!GetSubNodeValue(root_element, "num_min", node_cfg.num_min))
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "num_max", node_cfg.num_max))
		{
			return -3;
		}

		root_element = root_element->NextSiblingElement();
	}
	return 0;
}

int PetSoulEquipMentConfig::InitAttributeTypeValueGroup(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		int attribute_group = 0;
		if (!GetSubNodeValue(root_element, "attribute_group", attribute_group) || attribute_group < 0)
		{
			return -1;
		}
		AttributeTypeValueGroup &node_cfg = m_attr_type_cfg[attribute_group];
		node_cfg.attribute_group = attribute_group;
		int attribute_seq = 0;
		if (!GetSubNodeValue(root_element, "attribute_seq", attribute_seq) || attribute_seq < 0)
		{
			return -1;
		}

		AttributeTypeValueSeqGroup &node_list = node_cfg.attribute_list[attribute_seq];
		node_list.attribute_seq = attribute_seq;

		AttributeTypeValueDetailGroup node;
		if (!GetSubNodeValue(root_element, "att_type", node.att_type))
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "att_min", node.att_min))
		{
			return -3;
		}

		if (!GetSubNodeValue(root_element, "att_max", node.att_max))
		{
			return -4;
		}

		if (!GetSubNodeValue(root_element, "rate_10000", node.rate_10000))
		{
			return -5;
		}

		node_list.weight += node.rate_10000;
		node_list.m_attribute_vec.push_back(node);
		root_element = root_element->NextSiblingElement();
	}
	return 0;
}

int PetSoulEquipMentConfig::InitAttributeTypeIntensifyCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		int quality = 0;
		if (!GetSubNodeValue(root_element, "quality", quality) || quality < 0)
		{
			return -1;
		}
		AttributeIntensifyValueTypeCfg & node_cfg = m_intensify_cfg[quality];
		node_cfg.quality = quality;
		AttributeIntensifyValueCfg node;
		if (!GetSubNodeValue(root_element, "att_type", node.att_type))
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "intensify_att_min", node.intensify_att_min))
		{
			return -3;
		}

		if (!GetSubNodeValue(root_element, "intensify_att_max", node.intensify_att_max))
		{
			return -4;
		}
		node_cfg.int_vec.push_back(node);
		root_element = root_element->NextSiblingElement();
	}
	return 0;
}

int PetSoulEquipMentConfig::InitAttributeUplevelCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		AttributeUplevelCfg node_cfg;

		if (!GetSubNodeValue(root_element, "quality", node_cfg.quality))
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "levels", node_cfg.levels))
		{
			return -3;
		}

		if (!GetSubNodeValue(root_element, "need_steps", node_cfg.need_steps))
		{
			return -4;
		}

		if (!GetSubNodeValue(root_element, "exp", node_cfg.exp))
		{
			return -5;
		}
		m_soul_uplevel_cfg.push_back(node_cfg);
		root_element = root_element->NextSiblingElement();
	}
	return 0;
}

int PetSoulEquipMentConfig::InitEquipSoulStepsCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		PetEquipSoulStepsCfg node_cfg;

		if (!GetSubNodeValue(root_element, "quality", node_cfg.quality))
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "steps", node_cfg.steps))
		{
			return -3;
		}

		if (!GetSubNodeValue(root_element, "min_level", node_cfg.min_level))
		{
			return -4;
		}

		if (!GetSubNodeValue(root_element, "max_level", node_cfg.max_level))
		{
			return -5;
		}

		TiXmlElement * item_list_element = root_element->FirstChildElement("item_list_list");
		if (NULL != item_list_element)
		{
			TiXmlElement * item_element = item_list_element->FirstChildElement("item_list");
			while (item_element != NULL)
			{
				ItemConfigData item_node;
				if (!item_node.ReadConfig(item_element))
				{
					return -6;
				}
				node_cfg.m_cost_vec.push_back(item_node);
				item_element = item_element->NextSiblingElement();
			}
		}

		m_soul_steps_cfg.push_back(node_cfg);
		root_element = root_element->NextSiblingElement();
	}
	return 0;
}

int PetSoulEquipMentConfig::InitEquipSoulStepsValueCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		int quality = 0;
		if (!GetSubNodeValue(root_element, "quality", quality) || quality < 0)
		{
			return -1;
		}
		PetSoulEquipStepsTypeCfg &node_list = m_pet_soul_equip_steps_cfg[quality];
		node_list.quality = quality;
		PetSoulEquipStepsAttrCfg node;
		if (!GetSubNodeValue(root_element, "att_type", node.att_type))
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "intensify_att_min", node.intensify_att_min))
		{
			return -3;
		}

		if (!GetSubNodeValue(root_element, "intensify_att_max", node.intensify_att_max))
		{
			return -4;
		}

		node_list.int_vec.push_back(node);
		root_element = root_element->NextSiblingElement();
	}
	return 0;
}

int PetSoulEquipMentConfig::InitEquipSoulSpidCfg(TiXmlElement * RootElement)
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
		PetSoulEquipSpidGroupCfg &node_list = m_pet_soul_equip_spid_cfg[seq];

		PetSoulEquipSpidCfg node;
		if (!GetSubNodeValue(root_element, "buff_id", node.sp_id))
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "rate_10000", node.rate))
		{
			return -3;
		}
		node_list.weight += node.rate;
		node_list.m_spid_cfg.push_back(node);
		root_element = root_element->NextSiblingElement();
	}
	return 0;
}

int PetSoulEquipMentConfig::InitEquipSoulRefineCfg(TiXmlElement * RootElement)
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
		PetEquipSoulRefineGroupCfg &node_list = m_refine_cfg[seq];
		node_list.seq = seq;
		PetEquipSoulRefineCfg node;
		if (!GetSubNodeValue(root_element, "refine", node.refine))
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "skill_buff", node.new_spid))
		{
			return -3;
		}

		if (!GetSubNodeValue(root_element, "attr_type_1", node.attr_type_1))
		{
			return -4;
		}

		if (!GetSubNodeValue(root_element, "attr_value_1", node.attr_value_1))
		{
			return -5;
		}

		if (!GetSubNodeValue(root_element, "attr_type_2", node.attr_type_2))
		{
			return -6;
		}

		if (!GetSubNodeValue(root_element, "attr_value_2", node.attr_value_2))
		{
			return -7;
		}

		node_list.m_refine_vec.push_back(node);
		root_element = root_element->NextSiblingElement();
	}
	return 0;
}

int PetSoulEquipMentConfig::InitItemIdExpCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;
	int pre_item_exp = 0;
	while (NULL != root_element)
	{
		PetSoulEquipItemIdExpCfg node;
		if (!GetSubNodeValue(root_element, "item_id", node.item_id))
		{
			return -1;
		}

		if (!GetSubNodeValue(root_element, "item_exp", node.item_exp))
		{
			return -2;
		}
		if (pre_item_exp == 0)pre_item_exp = node.item_exp;
		
		if (pre_item_exp > node.item_exp)//必须大小顺序排下去
		{
			return -99;
		}

		if (!GetSubNodeValue(root_element, "need_coin_100_exp", node.cost_coin))
		{
			return -3;
		}
		pre_item_exp = node.item_exp;
		m_item_exp_cfg.push_back(node);
		root_element = root_element->NextSiblingElement();
	}
	return 0;
}

int PetSoulEquipMentConfig::InitPetEquipSoulExchangeCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		int pet_weapon_seq = 0;
		if (!GetSubNodeValue(root_element, "pet_weapon_seq", pet_weapon_seq) || pet_weapon_seq < 0)
		{
			return -1;
		}

		PetEquipSoulExchangeCfg &node = m_exchange_vec[pet_weapon_seq];
		node.pet_weapon_seq = pet_weapon_seq;
		if (!GetSubNodeValue(root_element, "pet_weapon_item_id", node.pet_weapon_item_id))
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "expend_item_id", node.expend_item_id) || NULL == ITEMPOOL->GetItem(node.expend_item_id))
		{
			return -3;
		}

		if (!GetSubNodeValue(root_element, "expend_num", node.expend_num) || node.expend_num <= 0)
		{
			return -4;
		}

		root_element = root_element->NextSiblingElement();
	}
	return 0;
}

int PetSoulEquipMentConfig::InitPetEquipSoulPriority(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		PetSoulSpPriority cfg;
		if (!GetSubNodeValue(root_element, "spid", cfg.spid))
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "sp_type", cfg.sp_type))
		{
			return -3;
		}

		if (!GetSubNodeValue(root_element, "priority", cfg.priority))
		{
			return -4;
		}

		m_pet_soul_sp_priority_map[cfg.spid] = cfg;

		root_element = root_element->NextSiblingElement();
	}
	return 0;
}
