#include "godequipupconfig.hpp"
#include "item/itempool.h"
#include "gameworld/gamelog.h"

GodEquipUpConfig::GodEquipUpConfig()
{
}

GodEquipUpConfig::~GodEquipUpConfig()
{
}

GodEquipUpConfig *g_god_equip_config = NULL;
GodEquipUpConfig * GodEquipUpConfig::Instance()
{
	if (NULL == g_god_equip_config)
	{
		g_god_equip_config = new GodEquipUpConfig();
	}
	return g_god_equip_config;
}

bool GodEquipUpConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG("godequip_strengthen", InitGodEquipUpCfg);
	LOAD_CONFIG("godjewelry_strengthen", InitGodJewelryUpCfg);
	LOAD_CONFIG("turn_back_item", InitGodItemTurnBackCfg);
	LOAD_CONFIG("turn_back_item2", InitGodItemTurnBack2Cfg);
	LOAD_CONFIG("godequip_addition", InitGodEquipResonanceCfg);
	LOAD_CONFIG("godequip_id", InitGodEquipBroadCastCfg);
	LOAD_CONFIG("other", InitOtherCfg);

	return true;
}

bool GodEquipUpConfig::Reload(const std::string & configname, std::string * err)
{
	GodEquipUpConfig *temp = new GodEquipUpConfig();

	if (!temp->Init(configname, err))
	{
		delete temp; temp = NULL;
		return false;
	}

	if (NULL != g_god_equip_config)
	{
		delete g_god_equip_config;
	}

	g_god_equip_config = temp;

	return true;
}

const StrengthenCfg * GodEquipUpConfig::GetStrengthenCfg(ItemID item_id, int level)
{
	if (level < 0 || level >= MAX_GOD_EQUIP_MAX_LEVEL + 1)
	{
		return NULL;
	}
	return item_point[item_id][level];
}

const StrengthenCfg * GodEquipUpConfig::GetStrengthenByJob(int job, int level, int color, int equip_type)
{
	if ((job / PROFESSION_BASE) < 0 || (job / PROFESSION_BASE) >= PROFESSION_TYPE_NUM)return NULL;
	if (level < 0 || level >= MAX_GOD_EQUIP_MAX_LEVEL + 1)return NULL;
	if (color < ItemBase::I_COLOR_STAR || color > ItemBase::I_COLOR_GOD)return NULL;
	if (equip_type < ULTIMATE_EQUIP_TYPE_E_HELMET || equip_type >= ULTIMATE_EQUIP_TYPE_MAX)return NULL;

	return job_point[(job / PROFESSION_BASE)][level][color][equip_type];
}

const StrengthenCfg * GodEquipUpConfig::GetStrengBackCfg(ItemID item_id, int level)
{
	if (level < 0 || level >= MAX_GOD_EQUIP_MAX_LEVEL + 1)
	{
		return NULL;
	}
	return turn_back_item_point[item_id][level];
}

const GodEquipTurnBackListCfg * GodEquipUpConfig::GetTurnBackUserId(int uid)
{
	std::map<int, GodEquipTurnBackListCfg>::iterator it = turn_back_user.find(uid);
	if (it != turn_back_user.end())
	{
		return &it->second;
	}
	return NULL;
}

const GodEquipTurnBackListCfg * GodEquipUpConfig::GetTurnBackUserId2(int uid)
{
	std::map<int, GodEquipTurnBackListCfg>::iterator it = turn_back_user2.find(uid);
	if (it != turn_back_user2.end())
	{
		return &it->second;
	}
	return NULL;
}

const GodEquipResonanceCfg * GodEquipUpConfig::GetResonanceCfg(int prof, int level)
{
	if (prof > 0 && prof < PROFESSION_TYPE_NUM)
	{
		if (level > 0 && level <= MAX_GOD_EQUIP_MAX_LEVEL)
		{
			return resonance_list[prof][level];
		}
	}
	return NULL;
}

const ItemID GodEquipUpConfig::RandGodEquipId() const
{
	std::vector<ItemID> temp_list = m_broadcast_list;
	RandomShuffle(&temp_list[0], temp_list.size());
	return temp_list[0];
}

int GodEquipUpConfig::InitGodEquipUpCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;
	m_strengthen_vec.clear();
	for (int i = 0; i < MAX_UINT16; i++)
	{
		for (int j = 0; j < MAX_GOD_EQUIP_MAX_LEVEL + 1; j++)
		{
			item_point[i][j] = NULL;
		}
	}
	for (int i = 0; i < MAX_UINT16; i++)
	{
		for (int j = 0; j < MAX_GOD_EQUIP_MAX_LEVEL + 1; j++)
		{
			turn_back_item_point[i][j] = NULL;
		}
	}
	memset(job_point, NULL, sizeof(job_point));

	while (NULL != root_element)
	{
		ItemID id = 0;
		if (!GetSubNodeValue(root_element, "id", id) || id <= 0 || NULL == ITEMPOOL->GetItem(id))
		{
			return -1;
		}
		int level = 0;
		if (!GetSubNodeValue(root_element, "level", level) || level < 0 || level > MAX_GOD_EQUIP_MAX_LEVEL)
		{
			return -2;
		}

		StrengthenCfg node_cfg;
		node_cfg.id = id;
		node_cfg.level = level;
		if (!GetSubNodeValue(root_element, "need_rolelevel", node_cfg.need_rolelevel) || node_cfg.need_rolelevel < 0 || node_cfg.need_rolelevel > MAX_ROLE_LEVEL)
		{
			return -3;
		}

		if (!GetSubNodeValue(root_element, "equip_type", node_cfg.equip_type) || node_cfg.equip_type < ULTIMATE_EQUIP_TYPE_E_HELMET || node_cfg.equip_type >= ULTIMATE_EQUIP_TYPE_MAX)
		{
			return -4;
		}

		if (!GetSubNodeValue(root_element, "color", node_cfg.color) || node_cfg.color < ItemBase::I_COLOR_STAR || node_cfg.color > ItemBase::I_COLOR_GOD)
		{
			return -5;
		}

		if (!GetSubNodeValue(root_element, "limit_prof", node_cfg.limit_prof) || node_cfg.limit_prof < 0 || (node_cfg.limit_prof / PROFESSION_BASE) >= PROFESSION_TYPE_NUM)
		{
			return -6;
		}
		memset(node_cfg.attr_add_list, 0, sizeof(node_cfg.attr_add_list));
		memset(node_cfg.attr_type_list, 0, sizeof(node_cfg.attr_type_list));
		for (int i = 0; i < MAX_GOD_STRENGTHEN_ATTR_LIST; i++)
		{
			std::string reader_mate = STRING_SPRINTF("attr_type_%d", i + 1);
			if (!GetSubNodeValue(root_element, reader_mate, node_cfg.attr_type_list[i]) || node_cfg.attr_type_list[i] < 0)
			{
				return -7;
			}
			reader_mate.clear();
			reader_mate = STRING_SPRINTF("attr_add_%d", i + 1);
			if (!GetSubNodeValue(root_element, reader_mate, node_cfg.attr_add_list[i]))
			{
				return -8;
			}
		}

		if (!GetSubNodeValue(root_element, "suit_id", node_cfg.suit_id) || node_cfg.suit_id < 0)
		{
			return -9;
		}

		if (!GetSubNodeValue(root_element, "strengthen_id", node_cfg.strengthen_id))
		{
			return -10;
		}

		if (node_cfg.strengthen_id != 0)
		{
			if (NULL == ITEMPOOL->GetItem(node_cfg.strengthen_id))
			{
				return -11;
			}
		}

		TiXmlElement * item_list_element = root_element->FirstChildElement("strengthen_consume_item_list");
		if (NULL != item_list_element)
		{
			ItemConfigData node_reward;
			TiXmlElement * item_element = item_list_element->FirstChildElement("strengthen_consume_item");
			while (NULL != item_element)
			{
				if (!node_reward.ReadConfig(item_element))
				{
					return -12;
				}
				node_cfg.consume_item_list.push_back(node_reward);
				item_element = item_element->NextSiblingElement();
			}
		}

		TiXmlElement * item_list_element2 = root_element->FirstChildElement("break_consume_item_list");
		if (NULL != item_list_element2)
		{
			ItemConfigData node_reward;
			TiXmlElement * item_element = item_list_element2->FirstChildElement("break_consume_item");
			while (NULL != item_element)
			{
				if (!node_reward.ReadConfig(item_element))
				{
					return -12;
				}
				node_cfg.consume_item_list_2.push_back(node_reward);
				item_element = item_element->NextSiblingElement();
			}
		}

		m_strengthen_vec.push_back(node_cfg);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int GodEquipUpConfig::InitGodJewelryUpCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		ItemID id = 0;
		if (!GetSubNodeValue(root_element, "id", id) || id <= 0 || NULL == ITEMPOOL->GetItem(id))
		{
			return -1;
		}

		int level = 0;
		if (!GetSubNodeValue(root_element, "level", level) || level < 0)
		{
			return -2;
		}

		StrengthenCfg node_cfg;
		node_cfg.id = id;
		node_cfg.level = level;

		if (!GetSubNodeValue(root_element, "need_rolelevel", node_cfg.need_rolelevel) || node_cfg.need_rolelevel < 0 || node_cfg.need_rolelevel > MAX_ROLE_LEVEL)
		{
			return -3;
		}

		if (!GetSubNodeValue(root_element, "equip_type", node_cfg.equip_type) || node_cfg.equip_type < ULTIMATE_EQUIP_TYPE_E_HELMET || node_cfg.equip_type >= ULTIMATE_EQUIP_TYPE_MAX)
		{
			return -4;
		}

		if (!GetSubNodeValue(root_element, "color", node_cfg.color) || node_cfg.color < ItemBase::I_COLOR_STAR || node_cfg.color > ItemBase::I_COLOR_GOD)
		{
			return -5;
		}

		if (!GetSubNodeValue(root_element, "limit_prof", node_cfg.limit_prof) || node_cfg.limit_prof < 0 || (node_cfg.limit_prof / PROFESSION_BASE) >= PROFESSION_TYPE_NUM)
		{
			return -6;
		}

		memset(node_cfg.attr_add_list, 0, sizeof(node_cfg.attr_add_list));
		memset(node_cfg.attr_type_list, 0, sizeof(node_cfg.attr_type_list));

		for (int i = 0; i < MAX_GOD_STRENGTHEN_ATTR_LIST; i++)
		{
			std::string reader_mate = STRING_SPRINTF("attr_type_%d", i + 1);
			if (!GetSubNodeValue(root_element, reader_mate, node_cfg.attr_type_list[i]) || node_cfg.attr_type_list[i] < 0)
			{
				return -7;
			}
			reader_mate.clear();
			reader_mate = STRING_SPRINTF("attr_add_%d", i + 1);
			if (!GetSubNodeValue(root_element, reader_mate, node_cfg.attr_add_list[i]) || node_cfg.attr_add_list[i] < 0)
			{
				return -8;
			}
		}

		if (!GetSubNodeValue(root_element, "suit_id", node_cfg.suit_id) || node_cfg.suit_id < 0)
		{
			return -9;
		}

		if (!GetSubNodeValue(root_element, "strengthen_id", node_cfg.strengthen_id))
		{
			return -10;
		}

		if (node_cfg.strengthen_id != 0)
		{
			if (NULL == ITEMPOOL->GetItem(node_cfg.strengthen_id))
			{
				return -11;
			}
		}

		TiXmlElement * item_list_element = root_element->FirstChildElement("strengthen_consume_item_list");
		if (NULL != item_list_element)
		{
			ItemConfigData node_reward;
			TiXmlElement * item_element = item_list_element->FirstChildElement("strengthen_consume_item");
			while (NULL != item_element)
			{
				if (!node_reward.ReadConfig(item_element))
				{
					return -12;
				}
				node_cfg.consume_item_list.push_back(node_reward);
				item_element = item_element->NextSiblingElement();
			}
		}

		TiXmlElement * item_list_element2 = root_element->FirstChildElement("break_consume_item_list");
		if (NULL != item_list_element2)
		{
			ItemConfigData node_reward;
			TiXmlElement * item_element = item_list_element2->FirstChildElement("break_consume_item");
			while (NULL != item_element)
			{
				if (!node_reward.ReadConfig(item_element))
				{
					return -12;
				}
				node_cfg.consume_item_list_2.push_back(node_reward);
				item_element = item_element->NextSiblingElement();
			}
		}

		gamelog::g_log_workshop.printf(LL_INFO, "HOTFIX BreakThroughGarde InitGodJewelryUpCfg");

		m_strengthen_vec.push_back(node_cfg);
		root_element = root_element->NextSiblingElement();
	}

	for (std::vector<StrengthenCfg>::iterator it = m_strengthen_vec.begin(); it != m_strengthen_vec.end(); it++)
	{
		item_point[it->id][it->level] = &(*it);
		job_point[it->limit_prof / PROFESSION_BASE][it->level][it->color][it->equip_type] = &(*it);
		if (it->strengthen_id > 0 && it->strengthen_id < MAX_UINT16)
		{
			turn_back_item_point[it->strengthen_id][it->level] = &(*it);
		}
	}

	return 0;
}

int GodEquipUpConfig::InitGodItemTurnBackCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		GodEquipTurnBackNode node;
		if (!GetSubNodeValue(root_element, "uid", node.uid))
		{
			return -1;
		}
		
		if (!GetSubNodeValue(root_element, "item_id", node.back_item))
		{
			return -1;
		}

		TiXmlElement * item_list_element = root_element->FirstChildElement("return_item_list");
		if (NULL != item_list_element)
		{
			ItemConfigData node_reward;
			TiXmlElement * item_element = item_list_element->FirstChildElement("return_item");
			while (NULL != item_element)
			{
				if (!node_reward.ReadConfig(item_element))
				{
					return -12;
				}
				node.return_item_list.push_back(node_reward);
				item_element = item_element->NextSiblingElement();
			}
		}

		GodEquipTurnBackListCfg & node_list = turn_back_user[node.uid];

		node_list.uid = node.uid;
		node_list.back_item_vec.push_back(node);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int GodEquipUpConfig::InitGodItemTurnBack2Cfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		GodEquipTurnBackNode node;
		if (!GetSubNodeValue(root_element, "uid", node.uid))
		{
			return -1;
		}

		if (!GetSubNodeValue(root_element, "item_id", node.back_item))
		{
			return -1;
		}

		TiXmlElement * item_list_element = root_element->FirstChildElement("return_item_list");
		if (NULL != item_list_element)
		{
			ItemConfigData node_reward;
			TiXmlElement * item_element = item_list_element->FirstChildElement("return_item");
			while (NULL != item_element)
			{
				if (!node_reward.ReadConfig(item_element))
				{
					return -12;
				}
				node.return_item_list.push_back(node_reward);
				item_element = item_element->NextSiblingElement();
			}
		}

		GodEquipTurnBackListCfg & node_list = turn_back_user2[node.uid];

		node_list.uid = node.uid;
		node_list.back_item_vec.push_back(node);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int GodEquipUpConfig::InitGodEquipResonanceCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;
	for (int i = 0; i < PROFESSION_TYPE_NUM; i++)
	{
		for (int j = 0; j < MAX_GOD_EQUIP_MAX_LEVEL + 1; j++)
		{
			resonance_list[i][j] = NULL;
		}
	}
	while (NULL != root_element)
	{
		GodEquipResonanceCfg node;
		if (!GetSubNodeValue(root_element, "limit_prof", node.limit_prof))
		{
			return -1;
		}

		if (!GetSubNodeValue(root_element, "level", node.level))
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "addition_level", node.addition_level))
		{
			return -3;
		}

		if (!GetSubNodeValue(root_element, "attr_type_1", node.attr_type_1))
		{
			return -4;
		}

		if (!GetSubNodeValue(root_element, "attr_add_1", node.attr_add_1))
		{
			return -5;
		}

		if (!GetSubNodeValue(root_element, "attr_type_2", node.attr_type_2))
		{
			return -6;
		}

		if (!GetSubNodeValue(root_element, "attr_add_2", node.attr_add_2))
		{
			return -7;
		}
		m_resonance_vec.push_back(node);
		root_element = root_element->NextSiblingElement();
	}

	for (std::vector<GodEquipResonanceCfg>::iterator it = m_resonance_vec.begin(); it != m_resonance_vec.end(); it++)
	{
		if (it->limit_prof / PROFESSION_BASE > 0 && it->limit_prof / PROFESSION_BASE < PROFESSION_TYPE_NUM)
		{
			if (it->level > 0 && it->level <= MAX_GOD_EQUIP_MAX_LEVEL)
			{
				resonance_list[it->limit_prof / PROFESSION_BASE][it->level] = &(*it);
			}
		}
	}

	return 0;
}

int GodEquipUpConfig::InitGodEquipBroadCastCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element) return -544;

	while (data_element)
	{
		ItemID godequip_id;
		if (!GetSubNodeValue(data_element, "id", godequip_id) || NULL == ITEMPOOL->GetItem(godequip_id)) return -1;

		m_broadcast_list.push_back(godequip_id);

		data_element = data_element->NextSiblingElement();
	}

	if (m_broadcast_list.empty()) return -2;

	return 0;
}

int GodEquipUpConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element) return -544;

	if (!GetSubNodeValue(data_element, "interval", m_other_cfg.broadcast_interval) || m_other_cfg.broadcast_interval < 0) return -1;
	if (!GetSubNodeValue(data_element, "time", m_other_cfg.start_broadcast_time) || m_other_cfg.start_broadcast_time < 0) return -2;
	if (!GetSubNodeValue(data_element, "world_rank", m_other_cfg.world_rank) || m_other_cfg.world_rank < 0) return -3;

	return 0;
}
