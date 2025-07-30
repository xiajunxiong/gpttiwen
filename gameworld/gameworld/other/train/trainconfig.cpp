#include "trainconfig.hpp"
#include "checkresourcecenter.hpp"

TrainConfig::TrainConfig() : m_pet_train_max_level(0)
{
}
TrainConfig::~TrainConfig()
{
}

bool TrainConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;	
	
	LOAD_CONFIG("character_train", InitTrainCfg);
	LOAD_CONFIG("unlock_train_level", InitTrainLevelCfg);
	LOAD_CONFIG("job_relation", InitProfReflectMap);
	LOAD_CONFIG("pet_train", InitPetTrainCfg);
	LOAD_CONFIG("glass_tripod", InitItemConvertCfg);
	LOAD_CONFIG("train_type", InitTrainAttrCfg);
	LOAD_CONFIG("other", InitOtherCfg);
	LOAD_CONFIG("train_list", InitSkillTypeCfg);
	LOAD_CONFIG("pet_tattr_rain", InitPetTrain2Cfg);
	LOAD_CONFIG("partner_tattr_rain", InitPartnerTrainCfg);

	return true;
}

const TrainCfg * TrainConfig::GetTrainCfg(int type, int level)
{
	std::map<int, std::map<int, TrainCfg> >::const_iterator type_it = m_train_map.find(type);
	if (type_it == m_train_map.end()) return NULL;

	std::map<int, TrainCfg>::const_iterator level_it = type_it->second.find(level);
	if (level_it == type_it->second.end()) return NULL;

	return &level_it->second;
}

const TrainAttrCfg * TrainConfig::GetTrainAttrCfg(int prof, int type, int level)
{
	std::map<int, std::map<int, std::map<int, TrainAttrCfg> > > ::const_iterator prof_it = m_train_attr_map.find(prof);
	if (prof_it == m_train_attr_map.end()) return NULL;

	std::map<int, std::map<int, TrainAttrCfg> >::const_iterator type_it = prof_it->second.find(type);
	if (type_it == prof_it->second.end()) return NULL;

	std::map<int, TrainAttrCfg>::const_iterator level_it = type_it->second.find(level);
	if (level_it == type_it->second.end()) return NULL;

	return &level_it->second;
}

int TrainConfig::GetMaxTrainLevel(int train_type, int role_level)
{
	std::map<int, int> & m = m_train_level_map[train_type];

	int ret = 0;
	for (std::map<int, int >::const_iterator it = m.begin(); it != m.end(); ++it)
	{
		if (role_level >= it->second)
		{
			ret = it->first;
		}
		else
		{
			break;
		}
	}

	return ret;
}

int TrainConfig::GetMainAttrType(int prof)
{
	if (prof >= PROFESSION_BASE) prof /= PROFESSION_BASE;
	std::map<int, int>::iterator f_it = m_prof_attr_map.find(prof);
	if (m_prof_attr_map.end() != f_it) return f_it->second;

	return (-1);
}

const PetTrainCfg* TrainConfig::GetPetTrainCfg(int skill_level)
{
	if (skill_level >= 0 && skill_level <= m_pet_train_max_level)
	{
		return &m_pet_train_cfg_list[skill_level];
	}

	return NULL;
}

int TrainConfig::GetPetTrainMaxLevel()
{
	return m_pet_train_max_level;
}

int TrainConfig::GetPetTrainMaxLevelByRoleLevel(int role_level)
{
	if(role_level <= 0 || role_level > MAX_ROLE_LEVEL) return 0;
	
	int max_learn_skill_level = 0;
	for (int i = 0; i < MAX_PET_TRAIN_SKILL_LEVEL; i++)
	{
		if(m_pet_train_cfg_list[i].need_role_level <= 0 || m_pet_train_cfg_list[i].need_role_level > MAX_ROLE_LEVEL)
		{
			break;
		}
		if (m_pet_train_cfg_list[i].need_role_level <= role_level)
		{
			max_learn_skill_level = m_pet_train_cfg_list[i].skill_level;
		}
	}

	return max_learn_skill_level;
}

const TrainItemConvertCfg * TrainConfig::GetTrainItemConvertCfg(int seq)
{
	std::map<int, TrainItemConvertCfg>::iterator iter = m_item_convert_cfg.find(seq);
	if (iter != m_item_convert_cfg.end())
	{
		return &iter->second;
	}

	return NULL;
}

const TrainOtherCfg & TrainConfig::GetOtherCfg()
{
	return m_other_cfg;
}

std::set<int> TrainConfig::GetTrainTypeBySkillType(int train_skill_type)
{
	std::set<int> ret;
	for (std::map<int, int>::iterator iter = m_skill_type_cfg.begin(); iter != m_skill_type_cfg.end(); ++iter)
	{
		if (iter->second == train_skill_type)
		{
			ret.insert(iter->first);
		}
	}

	return ret;
}

int TrainConfig::GetPetTrainCfgMaxLevel2(int type)
{
	std::map<int, std::map<int, PetTrainCfg2> >::const_iterator type_it = m_pet_train_2_map.find(type);
	if (type_it == m_pet_train_2_map.end()) return 0;

	std::map<int, PetTrainCfg2>::const_reverse_iterator level_it = type_it->second.rbegin();
	if (level_it == type_it->second.rend()) return 0;

	return level_it->first;
}

const PetTrainCfg2 * TrainConfig::GetPetTrainCfg2(int type, int level)
{
	std::map<int, std::map<int, PetTrainCfg2> >::const_iterator type_it = m_pet_train_2_map.find(type);
	if (type_it == m_pet_train_2_map.end()) return NULL;

	std::map<int, PetTrainCfg2>::const_iterator level_it = type_it->second.find(level);
	if (level_it == type_it->second.end()) return NULL;

	return &level_it->second;
}

int TrainConfig::GetPetTrain2MaxLevelByRoleLevel(int type, int role_level)
{
	std::map<int, std::map<int, PetTrainCfg2> >::const_iterator type_it = m_pet_train_2_map.find(type);
	if (type_it == m_pet_train_2_map.end()) return 0;

	for (std::map<int, PetTrainCfg2>::const_reverse_iterator level_it = type_it->second.rbegin(); level_it != type_it->second.rend(); ++level_it)
	{
		if (role_level >= level_it->second.need_role_level)
		{
			return level_it->first;
		}
	}

	return 0;
}

int TrainConfig::GetPartnerTrainCfgMaxLevel(int type)
{
	std::map<int, std::map<int, PartnerTrainCfg> >::const_iterator type_it = m_partner_train_map.find(type);
	if (type_it == m_partner_train_map.end()) return 0;

	std::map<int, PartnerTrainCfg>::const_reverse_iterator level_it = type_it->second.rbegin();
	if (level_it == type_it->second.rend()) return 0;

	return level_it->first;
}

const PartnerTrainCfg * TrainConfig::GetPartnerTrainCfg(int type, int level)
{
	std::map<int, std::map<int, PartnerTrainCfg> >::const_iterator type_it = m_partner_train_map.find(type);
	if (type_it == m_partner_train_map.end()) return NULL;

	std::map<int, PartnerTrainCfg>::const_iterator level_it = type_it->second.find(level);
	if (level_it == type_it->second.end()) return NULL;

	return &level_it->second;
}

std::set<int> TrainConfig::GetAllPetTrainCfg2Type()
{
	std::set<int> ret;
	for (std::map<int, std::map<int, PetTrainCfg2> >::const_iterator type_it = m_pet_train_2_map.begin(); type_it != m_pet_train_2_map.end(); ++type_it)
	{
		ret.insert(type_it->first);
	}

	return ret;
}

std::set<int> TrainConfig::GetAllPartnerTrainCfgType()
{
	std::set<int> ret;
	for (std::map<int, std::map<int, PartnerTrainCfg> >::const_iterator type_it = m_partner_train_map.begin(); type_it != m_partner_train_map.end(); ++type_it)
	{
		ret.insert(type_it->first);
	}

	return ret;
}

bool TrainConfig::IsPetTrainCfg2Type(int type)
{
	std::map<int, std::map<int, PetTrainCfg2> >::const_iterator type_it = m_pet_train_2_map.find(type);
	if (type_it == m_pet_train_2_map.end()) return false;

	return true;
}

bool TrainConfig::IsPartnerTrainCfgType(int type)
{
	std::map<int, std::map<int, PartnerTrainCfg> >::const_iterator type_it = m_partner_train_map.find(type);
	if (type_it == m_partner_train_map.end()) return false;

	return true;
}

int TrainConfig::InitTrainCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;
	while (NULL != root_element)
	{
		int train_type = 0;
		if (!GetSubNodeValue(root_element, "train_type", train_type) || train_type < TRAIN_BEGIN || train_type >= TRAIN_ENG)return -1;
		int train_level = 0;
		if (!GetSubNodeValue(root_element, "train_level", train_level) || train_level < 0 || train_level >= MAX_UINT16)return -2;

		TrainCfg & node_cfg = m_train_map[train_type][train_level];
		node_cfg.type = train_type;
		node_cfg.level = train_level;

		if (!GetSubNodeValue(root_element, "consume_type", node_cfg.consume_type))return -21;
		if (!GetSubNodeValue(root_element, "attribute_value", node_cfg.attribute_value) || node_cfg.attribute_value < 0)return -3;
		if (!GetSubNodeValue(root_element, "need_exp", node_cfg.max_exp) || node_cfg.max_exp < 0 || node_cfg.max_exp >= MAX_INT)return -4;
		if (!GetSubNodeValue(root_element, "bind_coin_consume", node_cfg.bind_coin_consume) || node_cfg.bind_coin_consume < 0)return -5;
		if (!GetSubNodeValue(root_element, "get_exp", node_cfg.get_exp) || node_cfg.get_exp < 0)return -6;
		if (!GetSubNodeValue(root_element, "train_ball_exp", node_cfg.train_ball_exp) || node_cfg.train_ball_exp < 0)return -7;
		if (!GetSubNodeValue(root_element, "train_ball_consume", node_cfg.train_ball_consume) || node_cfg.train_ball_consume < 0)return -8;
		if (!GetSubNodeValue(root_element, "item_id", node_cfg.item_id) || node_cfg.item_id < 0)return -9;
		if (!GetSubNodeValue(root_element, "level_limit", node_cfg.level_limit) || node_cfg.level_limit < 0)return -10;
		
		if (node_cfg.consume_type == 1)
		{
			CheckResourceCenter::Instance().GetItemValidCheck()->Add(node_cfg.item_id, node_cfg.item_id, 1, __FUNCTION__);
		}
		
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int TrainConfig::InitTrainAttrCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;
	while (NULL != root_element)
	{
		int train_type = 0;
		if (!GetSubNodeValue(root_element, "train_type", train_type) || train_type < TRAIN_BEGIN || train_type >= TRAIN_ENG)return -1;
		int train_level = 0;
		if (!GetSubNodeValue(root_element, "train_level", train_level) || train_level < 0 || train_level >= MAX_UINT16)return -2;
		int prof = 0;
		if (!GetSubNodeValue(root_element, "job_id", prof)) return -20;

		TrainAttrCfg & node_cfg = m_train_attr_map[prof][train_type][train_level];
		node_cfg.type = train_type;
		node_cfg.level = train_level;
		node_cfg.prof = prof;

		if (!GetSubNodeValue(root_element, "attribute_type", node_cfg.attribute_type))return -22;
		if (!GetSubNodeValue(root_element, "attribute_value", node_cfg.attribute_value) || node_cfg.attribute_value < 0)return -3;

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int TrainConfig::InitTrainLevelCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;

	while (NULL != root_element)
	{
		int train_type = 0;
		if (!GetSubNodeValue(root_element, "train_type", train_type)) return -__LINE__;

		std::map<int, int> & m = m_train_level_map[train_type];
		int last_level = 0;
		if (!m.empty())
		{
			last_level = m.rbegin()->first;
		}

		int level = 0;
		if (!GetSubNodeValue(root_element, "train_level", level)) return -__LINE__;
		if (level <= last_level)
		{
			return -__LINE__;
		}
		if (!GetSubNodeValue(root_element, "character_level", m[level])) return -__LINE__;
		

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int TrainConfig::InitProfReflectMap(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -10000;

	while (NULL != root_element)
	{
		int prof = 0, main_attr_type = 0;
		if (!GetSubNodeValue(root_element, "job_id", prof) || prof >= PROFESSION_TYPE_NUM || prof < 0)
		{
			return -1;
		}

		if (!GetSubNodeValue(root_element, "addition", main_attr_type) || main_attr_type < 0 || main_attr_type >= BATTLE_ATTR_MAX)
		{
			return -2;
		}

		m_prof_attr_map.insert(std::pair<int, int>(prof, main_attr_type));
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int TrainConfig::InitPetTrainCfg(TiXmlElement* RootElement)
{
	TiXmlElement* root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -10000;

	int next_skill_level = 0;
	while (NULL != root_element)
	{
		PetTrainCfg cfg;
		if (!GetSubNodeValue(root_element, "skill_level", cfg.skill_level) || cfg.skill_level != next_skill_level)
		{
			return -1;
		}
		next_skill_level += 1;
		if (!GetSubNodeValue(root_element, "skill_learn_level", cfg.need_role_level) )
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "need_exp", cfg.need_exp) || cfg.need_exp < 0)
		{
			return -3;
		}

		if (!GetSubNodeValue(root_element, "bind_coin_consume", cfg.need_coin) || cfg.need_coin <= 0)
		{
			return -4;
		}

		if (!GetSubNodeValue(root_element, "get_exp", cfg.add_exp) || cfg.add_exp <= 0)
		{
			return -5;
		}

		if (cfg.skill_level < 0 || cfg.skill_level >= ARRAY_LENGTH(m_pet_train_cfg_list))
		{
			return -6;
		}

		m_pet_train_cfg_list[cfg.skill_level] = cfg;
		m_pet_train_max_level = GetMax(cfg.skill_level, m_pet_train_max_level);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int TrainConfig::InitItemConvertCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -10000;

	while (NULL != root_element)
	{
		TrainItemConvertCfg cfg;
		if (!GetSubNodeValue(root_element, "seq", cfg.seq))
		{
			return -__LINE__;
		}
 
		if (!GetSubNodeValue(root_element, "target", cfg.target_item_id))
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(root_element, "compose_num", cfg.target_item_num))
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(root_element, "fee_num", cfg.comsume_size) || cfg.comsume_size > cfg.MAX_COMSUME_SIZE)
		{
			return -__LINE__;
		}

		for (int i = 0; i < cfg.comsume_size && i < cfg.MAX_COMSUME_SIZE; ++i)
		{
			std::string item_id = STRING_SPRINTF("collection_itemfee_%d", i + 1);
			std::string item_num = STRING_SPRINTF("collection_itemnum_%d", i + 1);

			if (!GetSubNodeValue(root_element, item_id, cfg.comsume_item_id[i]) || cfg.comsume_item_id[i] <= 0)
			{
				return -__LINE__;
			}

			if (!GetSubNodeValue(root_element, item_num, cfg.comsume_num[i]) || cfg.comsume_num[i] <= 0)
			{
				return -__LINE__;
			}
		}

		m_item_convert_cfg[cfg.seq] = cfg;
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int TrainConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -10000;

	if (!GetSubNodeValue(root_element, "item_id", m_other_cfg.item_ret))
	{
		return -__LINE__;
	}

	return 0;
}

int TrainConfig::InitSkillTypeCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -__LINE__;

	while (NULL != root_element)
	{
		int train_type = 0;
		if (!GetSubNodeValue(root_element, "train_type", train_type))
		{
			return -__LINE__;
		}

		int skill_type = 0;
		if (!GetSubNodeValue(root_element, "skill_type", skill_type))
		{
			return -__LINE__;
		}

		m_skill_type_cfg[train_type] = skill_type;
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int TrainConfig::InitPetTrain2Cfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;
	while (NULL != root_element)
	{
		int train_type = 0;
		if (!GetSubNodeValue(root_element, "train_type", train_type))return -1;
		int train_level = 0;
		if (!GetSubNodeValue(root_element, "train_level", train_level) || train_level < 0 || train_level >= MAX_UINT16)return -2;

		PetTrainCfg2 & node_cfg = m_pet_train_2_map[train_type][train_level];
		node_cfg.type = train_type;
		node_cfg.level = train_level;

		if (!GetSubNodeValue(root_element, "consume_type", node_cfg.consume_type))return -21;
		if (!GetSubNodeValue(root_element, "attribute_value", node_cfg.attribute_value) || node_cfg.attribute_value < 0)return -3;
		if (!GetSubNodeValue(root_element, "need_exp", node_cfg.max_exp) || node_cfg.max_exp < 0 || node_cfg.max_exp >= MAX_INT)return -4;
		if (!GetSubNodeValue(root_element, "bind_coin_consume", node_cfg.bind_coin_consume) || node_cfg.bind_coin_consume < 0)return -5;
		if (!GetSubNodeValue(root_element, "get_exp", node_cfg.get_exp) || node_cfg.get_exp < 0)return -6;
		if (!GetSubNodeValue(root_element, "train_ball_exp", node_cfg.train_ball_exp) || node_cfg.train_ball_exp < 0)return -7;
		if (!GetSubNodeValue(root_element, "train_ball_consume", node_cfg.train_ball_consume) || node_cfg.train_ball_consume < 0)return -8;
		if (!GetSubNodeValue(root_element, "item_id", node_cfg.item_id) || node_cfg.item_id < 0)return -9;
		if (!GetSubNodeValue(root_element, "need_role_level", node_cfg.need_role_level))return -10;

		TiXmlElement * attr_type_list = root_element->FirstChildElement("attr_type");
		if (NULL == attr_type_list) return -100;

		TiXmlElement * attr_type_node = attr_type_list->FirstChildElement("node");
		while (NULL != attr_type_node)
		{
			int attr_type = 0;
			if (!GetNodeValue(attr_type_node, attr_type))
			{
				return -2000;
			}

			node_cfg.attr_type_list.insert(attr_type);
			attr_type_node = attr_type_node->NextSiblingElement();
		}

		if (node_cfg.consume_type == 1)
		{
			CheckResourceCenter::Instance().GetItemValidCheck()->Add(node_cfg.item_id, node_cfg.item_id, 1, __FUNCTION__);
		}

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int TrainConfig::InitPartnerTrainCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;
	while (NULL != root_element)
	{
		int train_type = 0;
		if (!GetSubNodeValue(root_element, "train_type", train_type))return -1;
		int train_level = 0;
		if (!GetSubNodeValue(root_element, "train_level", train_level) || train_level < 0 || train_level >= MAX_UINT16)return -2;

		PartnerTrainCfg & node_cfg = m_partner_train_map[train_type][train_level];
		node_cfg.type = train_type;
		node_cfg.level = train_level;

		if (!GetSubNodeValue(root_element, "consume_type", node_cfg.consume_type))return -21;
		if (!GetSubNodeValue(root_element, "attribute_value", node_cfg.attribute_value) || node_cfg.attribute_value < 0)return -3;
		if (!GetSubNodeValue(root_element, "need_exp", node_cfg.max_exp) || node_cfg.max_exp < 0 || node_cfg.max_exp >= MAX_INT)return -4;
		if (!GetSubNodeValue(root_element, "bind_coin_consume", node_cfg.bind_coin_consume) || node_cfg.bind_coin_consume < 0)return -5;
		if (!GetSubNodeValue(root_element, "get_exp", node_cfg.get_exp) || node_cfg.get_exp < 0)return -6;
		if (!GetSubNodeValue(root_element, "train_ball_exp", node_cfg.train_ball_exp) || node_cfg.train_ball_exp < 0)return -7;
		if (!GetSubNodeValue(root_element, "train_ball_consume", node_cfg.train_ball_consume) || node_cfg.train_ball_consume < 0)return -8;
		if (!GetSubNodeValue(root_element, "item_id", node_cfg.item_id) || node_cfg.item_id < 0)return -9;
		if (!GetSubNodeValue(root_element, "need_role_level", node_cfg.need_role_level))return -10;

		TiXmlElement * attr_type_list = root_element->FirstChildElement("attr_type");
		if (NULL == attr_type_list) return -100;

		TiXmlElement * attr_type_node = attr_type_list->FirstChildElement("node");
		while (NULL != attr_type_node)
		{
			int attr_type = 0;
			if (!GetNodeValue(attr_type_node, attr_type))
			{
				return -2000;
			}

			node_cfg.attr_type_list.insert(attr_type);
			attr_type_node = attr_type_node->NextSiblingElement();
		}

		if (node_cfg.consume_type == 1)
		{
			CheckResourceCenter::Instance().GetItemValidCheck()->Add(node_cfg.item_id, node_cfg.item_id, 1, __FUNCTION__);
		}

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}
