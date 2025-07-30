#include "roleattrpelletconfig.hpp"

#include "item/itempool.h"
#include "servercommon/attributedef.hpp"
#include "servercommon/struct/battle/battle_def.hpp"
#include "servercommon/roleattrpelletdef.hpp"

RoleAttrPelletConfig::RoleAttrPelletConfig()
{
}

RoleAttrPelletConfig::~RoleAttrPelletConfig()
{
}

bool RoleAttrPelletConfig::Init(const std::string & configname, std::string * err)
{

	PRE_LOAD_CONFIG;

	LOAD_CONFIG("other", InitOtherCfg);
	LOAD_CONFIG("attribute_list", InitAttrListCfg);
	LOAD_CONFIG("role_item_random", InitRoleItemCfg);	//需要放在属性列表读取之后

	return true;
}

const RoleAttrPelletAttrListCfg * RoleAttrPelletConfig::GetPelletListCfg(int attr_pellet_id, int type_param)
{
	long long attr_pellet_key = this->GetAttrPelletKey(attr_pellet_id, type_param);
	std::map<long long, RoleAttrPelletAttrListCfg>::iterator it = m_attr_list_map.find(attr_pellet_key);
	if(it == m_attr_list_map.end()) return NULL;

	return &it->second;
}

const RoleAttrPelletAttrListCfg * RoleAttrPelletConfig::GetAttrPelletCfg(int role_attr_pellet_type, int type_param, int attr_type)
{
	std::map<int, RoleAttrPelletAttrListCfg>::iterator it = m_attr_type_map.find(this->GetAttrPelleTypeKey(role_attr_pellet_type, type_param, attr_type));
	if(it == m_attr_type_map.end()) return NULL;

	return &it->second;
}

const std::set<int>* RoleAttrPelletConfig::GetRolePelletItemCfg(int gift_item_id)
{
	std::map<int, std::set<int> >::iterator it = m_role_item_cfg.find(gift_item_id);
	if(it == m_role_item_cfg.end()) return NULL;

	return &it->second;
}

int RoleAttrPelletConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;

	if (!GetSubNodeValue(root_element, "use_limit", m_other_cfg.use_limit) || m_other_cfg.use_limit <= 0) return -1;

	return 0;
}

int RoleAttrPelletConfig::InitAttrListCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;

	while (NULL != root_element)
	{
		int attr_pellet_id = 0, role_attr_pellet_type = 0, type_param = 0;
		if (!GetSubNodeValue(root_element, "attribute_pellet_id", attr_pellet_id) || NULL == ITEMPOOL->GetItem(attr_pellet_id)) return -1;
		if (!GetSubNodeValue(root_element, "attribute_pellet_type", role_attr_pellet_type)) return -2;
		if (!GetSubNodeValue(root_element, "type_param", type_param)) return -3;

		switch (role_attr_pellet_type)
		{
			case ROLE_ATTR_PELLET_TYPE_ROLE:
			case ROLE_ATTR_PELLET_TYPE_PARTNER:
			{
				if(type_param <= 0 || type_param >= PROFESSION_TYPE_NUM) return -4;
			}
			break;
			case ROLE_ATTR_PELLET_TYPE_PET:
			{
				type_param--;
				if(type_param < ADD_POINT_TYPE_MIN || type_param >= ADD_POINT_TYPE_MAX) return -5;
			}
			break;
			default:
			return -6;
		}
		long long attr_pellet_key = this->GetAttrPelletKey(attr_pellet_id, type_param);
		if (m_attr_list_map.end() != m_attr_list_map.find(attr_pellet_key))
		{	
			return -7;
		}

		RoleAttrPelletAttrListCfg node_cfg;
		node_cfg.attr_pellet_id = attr_pellet_id;
		node_cfg.role_attr_pellet_type = role_attr_pellet_type;
		node_cfg.type_param = type_param;
		if (!GetSubNodeValue(root_element, "attr_type", node_cfg.attr_type) || node_cfg.attr_type < BATTLE_ATTR_MIN || node_cfg.attr_type >= BATTLE_ATTR_MAX) return -8;
		if (!GetSubNodeValue(root_element, "attr_value", node_cfg.attr_value) || node_cfg.attr_value < 0) return -9;

		int attr_pellet_type_key = this->GetAttrPelleTypeKey(role_attr_pellet_type, type_param, node_cfg.attr_type);
		if (m_attr_type_map.end() != m_attr_type_map.find(attr_pellet_type_key))
		{
			return -10;
		}
		int attr_type_type = GetPelletTypeByAttrType(node_cfg.attr_type);
		if (attr_type_type < 0 || attr_type_type >= ATTR_PELLET_TYPE_MAX)
		{
			return -11;
		}

		m_attr_list_set.insert(attr_pellet_id);
		m_attr_list_map[attr_pellet_key] = node_cfg;
		m_attr_type_map[attr_pellet_type_key] = node_cfg;
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int RoleAttrPelletConfig::InitRoleItemCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;

	while (NULL != root_element)
	{
		int gift_item_id = 0, attr_pellet_id = 0;
		if (!GetSubNodeValue(root_element, "gift_item_id", gift_item_id) || NULL == ITEMPOOL->GetItem(gift_item_id)) return -1;
		if (!GetSubNodeValue(root_element, "item_id", attr_pellet_id) || NULL == ITEMPOOL->GetItem(attr_pellet_id)) return -2;
		if (m_attr_list_set.find(attr_pellet_id) == m_attr_list_set.end())
		{
			return -3;
		}

		m_role_item_cfg[gift_item_id].insert(attr_pellet_id);
		root_element = root_element->NextSiblingElement();
	}
	for (std::map<int, std::set<int> >::iterator it = m_role_item_cfg.begin(); it != m_role_item_cfg.end(); ++it)
	{
		for (std::set<int>::iterator pellet_it = it->second.begin(); pellet_it != it->second.end(); ++pellet_it)
		{
			if (m_role_item_cfg.end() != m_role_item_cfg.find(*pellet_it))
			{
				return -33;
			}
		}
	}

	return 0;
}

long long RoleAttrPelletConfig::GetAttrPelletKey(int attr_pellet_id, int type_param)
{
	AttrPelletKey attr_key;
	attr_key.param[0] = attr_pellet_id;
	attr_key.param[1] = type_param;

	return attr_key.unique_key;
}

void RoleAttrPelletConfig::GetAttrPelletParam(long long unique_key, int * attr_pellet_id, int * type_param)
{
	if(NULL == attr_pellet_id || NULL == type_param) return;
	AttrPelletKey attr_key;
	attr_key.unique_key = unique_key;
	*attr_pellet_id = attr_key.param[0];
	*type_param = attr_key.param[1];
}

int RoleAttrPelletConfig::GetAttrPelleTypeKey(int role_attr_pellet_type, int type_param, int attr_type)
{
	return MAX_ROLE_ATTR_PELLET_TYPE_BASE_NUM * role_attr_pellet_type + type_param * MAX_ROLE_TYPE_PARAM_BASE_NUM + attr_type;
}
