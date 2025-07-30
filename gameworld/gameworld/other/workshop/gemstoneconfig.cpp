#include "gemstoneconfig.hpp"
#include "item/itempool.h"
#include "equipment/equipment.h"
#include "servercommon/workshopdef.h"

GemstoneConfig::GemstoneConfig()
{
}

GemstoneConfig::~GemstoneConfig()
{
}

bool GemstoneConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG("other", InitGemstoneOtherCfg);
	LOAD_CONFIG("jewel_list", InitGemstoneAttrsAdd);	// 必须放在基础价值表的后面，会检测宝石类型是否合法
	LOAD_CONFIG("jewel_set", InitGemstoneCanMount);
	LOAD_CONFIG("limit_level", InitGemstoneSlotOpen);

	return true;
}

bool GemstoneConfig::CheckGemstoneCanMount(int equip_type, int gem_type)
{
	if (equip_type < GEMSTONE_TYPE_HELMET || equip_type >= GEMSTONE_TYPE_MAX)
	{
		return false;
	}

	std::set<int> f_it = m_can_mount_cfg[equip_type];
	if (f_it.end() != f_it.find(gem_type)) return true;
	
	return false;
}

bool GemstoneConfig::CheckGemMountSlotOpen(int slot_id, int equip_lv, bool isgodequip )
{
	std::map<int, SlotOpenConfig>::iterator f_it = m_slot_open_cfg.find(slot_id);
	
	if (isgodequip)
	{
		if (m_slot_open_cfg.end() != f_it && equip_lv >= f_it->second.godequiplimit_lv) return true;
	}
	else
	{
		if (m_slot_open_cfg.end() != f_it && equip_lv >= f_it->second.equiplimit_lv) return true;
	}

	return false;
}

int GemstoneConfig::GetGemstoneItemID(int gem_type, int gem_level)
{
	return m_gemstone_map[gem_type][gem_level];
}

int GemstoneConfig::GetGemstoneByType(int equip_type, int gem_type)
{
	if (equip_type == WORKSHOP_EQUIP_POSI_EQUIP)
	{
		gem_type -= Equipment::E_TYPE_MIN;
		return gem_type;
	}
	else if(equip_type == WORKSHOP_STONE_TYPE_JEWELRY)
	{
		gem_type += GEMSTONE_TYPE_RING;
		return gem_type;
	}
	return -1;
}

const GemstoneBasicInfoCfg * GemstoneConfig::GetGemstoneBasicInfoCfg(int gem_item_id)
{
	std::map<int, GemstoneBasicInfoCfg>::iterator f_it = m_gem_attrs_cfg.find(gem_item_id);
	if (m_gem_attrs_cfg.end() != f_it) return & f_it->second;

	return NULL;
}

int GemstoneConfig::InitGemstoneOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	if (!GetSubNodeValue(root_element, "compose_consume", m_gemstone_other_cfg.compose_price_once) || m_gemstone_other_cfg.compose_price_once < 0)
	{
		return -1;
	}

	return 0;
}

int GemstoneConfig::InitGemstoneAttrsAdd(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		int t_gem_id = 0; GemstoneBasicInfoCfg t_gbic;
		if (!GetSubNodeValue(root_element, "jewel_id", t_gem_id) || NULL == ITEMPOOL->GetItem(t_gem_id) || m_gem_attrs_cfg.end() != m_gem_attrs_cfg.find(t_gem_id))
		{
			return -1;
		}

		if (!GetSubNodeValue(root_element, "jewel_type", t_gbic.gemstone_type))
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "level", t_gbic.level))
		{
			return -3;
		}

		if (0 != m_gemstone_map[t_gbic.gemstone_type][t_gbic.level])
		{
			return -4;
		}

		if (!GetSubNodeValue(root_element, "level_limit", t_gbic.level_limit))
		{
			return -5;
		}

		if (!GetSubNodeValue(root_element, "godequip_level", t_gbic.godequip_level))
		{
			return -6;
		}

		for (int t_cur = 0; t_cur < GEM_ATTR_COUNT_MAX; ++t_cur)
		{
			char * attr_reader = new char[10]; BattleAttrPair & t_bap = t_gbic.attr_add[t_cur];
			sprintf(attr_reader, "type_%d", t_cur + 1);
			if (!GetSubNodeValue(root_element, attr_reader, t_bap.attr_type) || t_bap.attr_type < 0 || t_bap.attr_type >= BATTLE_ATTR_MAX)
			{
				return -(100 + t_cur);
			}

			sprintf(attr_reader, "value_%d", t_cur + 1);
			if (!GetSubNodeValue(root_element, attr_reader, t_bap.attr_value))
			{
				return -(200 + t_cur);
			}
		}


		m_gem_attrs_cfg.insert(std::pair<int, GemstoneBasicInfoCfg>(t_gem_id, t_gbic));
		m_gemstone_map[t_gbic.gemstone_type][t_gbic.level] = t_gem_id;
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int GemstoneConfig::InitGemstoneCanMount(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		int t_equip_type = 0;
		if (!GetSubNodeValue(root_element, "equip_type", t_equip_type) || t_equip_type < 0 || t_equip_type >= GEMSTONE_TYPE_MAX)
		{
			return -1;
		}

		/*int slot_num = 0;
		if (!GetSubNodeValue(root_element, "slot", slot_num) || slot_num < 0 || slot_num >= NEW_EQUIP_STONE_MOUNT_MAX)
		{
			return -2;
		}*/

		std::set<int> &t_s_gem_type = m_can_mount_cfg[t_equip_type];

		TiXmlElement * gem_type_list = root_element->FirstChildElement("jewel_type");
		if (NULL == gem_type_list) return -2000;
		
		TiXmlElement * gem_type_node = gem_type_list->FirstChildElement("node");
		while (NULL != gem_type_node)		// 可以为空
		{
			int gemstone_type = 0;
			if (!GetNodeValue(gem_type_node, gemstone_type))
			{
				printf("\n\nError Read -> EquipType: %d | StoneType: %d\n", t_equip_type, gemstone_type);
				return -3;
			}

			t_s_gem_type.insert(gemstone_type);
			gem_type_node = gem_type_node->NextSiblingElement();
		}

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int GemstoneConfig::InitGemstoneSlotOpen(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		int t_slot_id = 0, t_equip_lv = 0, t_god_limit_lv = 0;
		if (!GetSubNodeValue(root_element, "solt_id", t_slot_id) || t_slot_id < 0 || t_slot_id >= NEW_EQUIP_STONE_MOUNT_MAX
			|| m_slot_open_cfg.end() != m_slot_open_cfg.find(t_slot_id))
		{
			return -1;
		}

		if (!GetSubNodeValue(root_element, "limit_level", t_equip_lv) || t_equip_lv < 0/* || TODO: 不能大于最大等级*/)
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "godlimit_level",t_god_limit_lv))
		{
			return -3;
		}

		SlotOpenConfig t_slot_open;
		t_slot_open.equiplimit_lv = t_equip_lv;
		t_slot_open.godequiplimit_lv = t_god_limit_lv;

		m_slot_open_cfg.insert(std::pair<int, SlotOpenConfig>(t_slot_id, t_slot_open));
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}