#include "workshopconfig.hpp"
#include "item/itempool.h"
#include "gamelog.h"

WorkshopConfig::WorkshopConfig() : god_equip_version_id(-1)
{
}

WorkshopConfig::~WorkshopConfig()
{
	std::vector<WorlshopGodEquipReturn>().swap(m_god_equip_return_vec);
}

bool WorkshopConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG("compose_list", InitCompoundList);
	LOAD_CONFIG("equip_strength", InitCraftAttrAdd);
	LOAD_CONFIG("exchange_item_group", InitConvertGroup);
	LOAD_CONFIG("exchange", InitItemExchange);
	LOAD_CONFIG("jg_consume", InitUltimateConsume);
	LOAD_CONFIG("other", InitOhterCfg);
	LOAD_CONFIG("godequip_return", InitGodEquipReturn);

	return true;
}

const WorkshopItemCompound * WorkshopConfig::GetItemCompoundCfg(int comp_seq)
{
	std::map<int, WorkshopItemCompound>::iterator f_it = m_compound_items_map.find(comp_seq);
	if (m_compound_items_map.end() != f_it) return & f_it->second;

	return NULL;
}

const WorkshopItemCompound * WorkshopConfig::GetItemCompoundCfgByItemId(ItemID _item_id)
{
	std::map<int, WorkshopItemCompound>::iterator it = m_target_to_wiccfg.find(_item_id);
	if (m_target_to_wiccfg.end() != it) return &it->second;
	return NULL;
}

const WorkshopCraftAttrAdd * WorkshopConfig::GetCraftAttrAddCfg(int equip_id)
{
	std::map<int, WorkshopCraftAttrAdd>::iterator f_it = m_craft_attr_add_map.find(equip_id);
	if (m_craft_attr_add_map.end() != f_it) return & f_it->second;

	return NULL;
}

const WorkshopItemExchange * WorkshopConfig::GetItemExchangeCfg(int exch_seq)
{
	std::map<int, WorkshopItemExchange>::iterator find_it = m_exchange_props_map.find(exch_seq);
	if (m_exchange_props_map.end() != find_it) return & find_it->second;

	return NULL;
}

const ItemPairCon * WorkshopConfig::GetFastComposeGemCon(int gemstone_id)
{
	std::map<int, ItemPairCon>::iterator f_it = m_gemstone_compose_search_map.find(gemstone_id);
	if (m_gemstone_compose_search_map.end() != f_it) return & f_it->second;

	return NULL;
}

bool WorkshopConfig::IsItemBelongsToGroup(int group_id, int item_id)
{
	std::map<int, std::set<WorkshopExRate> >::iterator group_it = m_convert_group_map.find(group_id);
	if (m_convert_group_map.end() != group_it && group_it->second.end() != group_it->second.find(WorkshopExRate(item_id))) return true;

	return false;
}

const ItemID WorkshopConfig::RandAExchangeItem(int group_id)
{
	std::map<int, std::set<WorkshopExRate> >::iterator group_it = m_convert_group_map.find(group_id);
	if (m_convert_group_map.end() == group_it) return 0;

	int weight = rand() % RAND_UNIVERSAL_RATIO_BASE_NUM;
	for (std::set<WorkshopExRate>::iterator loop = group_it->second.begin(); loop != group_it->second.end(); ++loop)
	{
		if (weight < loop->ex_rate) return loop->item_id;
		weight -= loop->ex_rate;
	}
	
	return 0;
}

const WorkshopUltimateCfg * WorkshopConfig::GetUltimateCfg(int equip_type, int color, int limit_level)
{
	if (equip_type >= ULTIMATE_EQUIP_TYPE_E_HELMET && equip_type < ULTIMATE_EQUIP_TYPE_MAX)
	{
		if (color > ItemBase::I_COLOR_INVALID && color < ItemBase::I_COLOR_MAX)
		{
			if (limit_level >= 0 && limit_level <= MAX_ROLE_LEVEL)
			{
				return &m_ultimate_consume[equip_type][color][limit_level];
			}
		}
	}
	return NULL;
}

const WorkShopUltimateOtherCfg * WorkshopConfig::GetRandUlitmateCfg()
{
	if (m_ultimate_weight.weight_count <= 0)return NULL;
	
	int rand_value = rand() % m_ultimate_weight.weight_count;

	for (int i = 0; i < 5; i++)
	{
		if (m_ultimate_weight.value_ultimate[i].rate > 0 && m_ultimate_weight.value_ultimate[i].rate >= rand_value)
		{
			return &m_ultimate_weight.value_ultimate[i];
		}
		rand_value -= m_ultimate_weight.value_ultimate[i].rate;
	}
	return NULL;
}

const WorlshopGodEquipReturn * WorkshopConfig::GetGodEquipReturnCfg(int color, int level)
{
	for (std::vector<WorlshopGodEquipReturn>::iterator it = m_god_equip_return_vec.begin(); it != m_god_equip_return_vec.end(); it++)
	{
		if (it->color == color && it->level == level)
		{
			return &(*it);
		}
	}
	return NULL;
}

int WorkshopConfig::InitCompoundList(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		int t_comp_seq = 0; WorkshopItemCompound t_wic;
		if (!GetSubNodeValue(root_element, "seq", t_comp_seq) || t_comp_seq <= 0 || m_compound_items_map.end() != m_compound_items_map.find(t_comp_seq))
		{
			return -1;
		}

		if (!GetSubNodeValue(root_element, "target", t_wic.composition) || NULL == ITEMPOOL->GetItem(t_wic.composition))
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "limit_level", t_wic.level_limit) || t_wic.level_limit < 0 || t_wic.level_limit > MAX_ROLE_LEVEL)
		{
			return -3;
		}

		if (!GetSubNodeValue(root_element, "stuff_id_spe", t_wic.fortifier_id) || (0 != t_wic.fortifier_id && NULL == ITEMPOOL->GetItem(t_wic.fortifier_id)))
		{
			return -4;
		}
		
		if (!GetSubNodeValue(root_element, "price", t_wic.need_coin) || t_wic.need_coin < 0)
		{
			return -5;
		}

		if (!GetSubNodeValue(root_element, "stuff_num_spe", t_wic.craft_need) || t_wic.craft_need < 0)
		{
			return -6;
		}

		if (!GetSubNodeValue(root_element, "rate", t_wic.succ_prob) || t_wic.succ_prob <= 0 || t_wic.succ_prob > 100)
		{
			return -7;
		}

		if (!GetSubNodeValue(root_element, "resolve_return_lingyu", t_wic.resolve_return_lingyu))
		{
			return -71;
		}

		ItemPairCon & q_ipc = t_wic.fail_return_item[0];
		if (!GetSubNodeValue(root_element, "fail_return_item", q_ipc.item_id) || (0 != q_ipc.item_id && NULL == ITEMPOOL->GetItem(q_ipc.item_id)))
		{
			return -8;
		}

		if (!GetSubNodeValue(root_element, "fail_return_num", q_ipc.item_num) || (NULL != ITEMPOOL->GetItem(q_ipc.item_id) && q_ipc.item_num <= 0))
		{
			return -9;
		}

		ItemPairCon & q_ipc2 = t_wic.fail_return_item[1];
		if (!GetSubNodeValue(root_element, "fail_return_item2", q_ipc2.item_id) || (0 != q_ipc2.item_id && NULL == ITEMPOOL->GetItem(q_ipc2.item_id)))
		{
			return -10;
		}

		if (!GetSubNodeValue(root_element, "fail_return_num2", q_ipc2.item_num) || (NULL != ITEMPOOL->GetItem(q_ipc2.item_id) && q_ipc2.item_num <= 0))
		{
			return -11;
		}

		for (int t_cur = 0; t_cur < COMPOUND_MATE_KIND_MAX; ++t_cur)
		{
			MaterialCon & t_mc = t_wic.need_mate[t_cur];
			
			std::string reader_mate = STRING_SPRINTF("stuff_id_%d", t_cur + 1);
			if (!GetSubNodeValue(root_element, reader_mate, t_mc.mate_id) || (0 != t_mc.mate_id && NULL == ITEMPOOL->GetItem(t_mc.mate_id)))
			{
				return -(20 + t_cur);
			}

			reader_mate = STRING_SPRINTF("stuff_num_%d", t_cur + 1);
			if (!GetSubNodeValue(root_element, reader_mate, t_mc.need_num) || (NULL != ITEMPOOL->GetItem(t_mc.mate_id) && t_mc.need_num <= 0))
			{
				return -(30 + t_cur);
			}

			reader_mate = STRING_SPRINTF("stuff_return_%d", t_cur + 1);
			if (!GetSubNodeValue(root_element, reader_mate, t_mc.can_return) || (WORKSHOP_RETURN_NONE != t_mc.can_return && WORKSHOP_RETURN_TRUE != t_mc.can_return))
			{
				return -(40 + t_cur);
			}

			reader_mate = STRING_SPRINTF("resolve_return_%d", t_cur + 1);
			int can_decompose = 0;
			if (!GetSubNodeValue(root_element, reader_mate, can_decompose))
			{
				return -(50 + t_cur);
			}

			t_mc.can_decompose = can_decompose == 1 ? true : false;

			reader_mate = STRING_SPRINTF("min_value_%d", t_cur + 1);
			if (!GetSubNodeValue(root_element, reader_mate, t_mc.min_value))
			{
				return -(60 + t_cur);
			}
			reader_mate = STRING_SPRINTF("max_value_%d", t_cur + 1);
			if (!GetSubNodeValue(root_element, reader_mate, t_mc.max_value))
			{
				return -(70 + t_cur);
			}

			if (t_mc.max_value < t_mc.min_value)
			{
				return -99;
			}
		}
		
		if (!GetSubNodeValue(root_element, "active_seq", t_wic.active_sign) || t_wic.active_sign >= SPECIAL_RECIPE_TOTAL_NUM || t_wic.active_sign < 0)
		{
			return -11;
		}

		m_compound_items_map.insert(std::pair<int, WorkshopItemCompound>(t_comp_seq, t_wic));
		m_target_to_wiccfg[t_wic.composition] = t_wic;

		int group = 0;
		if (!GetSubNodeValue(root_element, "group", group))
		{
			return -12;
		}
		if (4 == group)
		{
			ItemPairCon item_con; item_con.item_id = t_wic.need_mate[0].mate_id; item_con.item_num = t_wic.need_mate[0].need_num;
			m_gemstone_compose_search_map.insert(std::pair<int, ItemPairCon>(t_wic.composition, item_con));
		}

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int WorkshopConfig::InitCraftAttrAdd(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		int t_equip_id = 0; WorkshopCraftAttrAdd t_wcaa;
		if (!GetSubNodeValue(root_element, "equip_id", t_equip_id) || m_craft_attr_add_map.end() != m_craft_attr_add_map.find(t_equip_id))
		{
			return -1;
		}
		
		if (!GetSubNodeValue(root_element, "effect_rate_up", t_wcaa.effect_prob_inc) || t_wcaa.effect_prob_inc < 0)
		{
			return -2;
		}

		const char * cfg_reader[Equipment::EQUIP_ATTR_IDX_MAX] = { "min_maxhp", "min_maxmp", "min_gongji", "min_fangyu", "min_minjie",
			 "min_jingshen", "min_huifu", "min_bisha", "min_bisha_dmg", "min_jianren", "min_fanji", "min_mingzhong", "min_shanduo", 
			 "min_def_zhongdu", "min_def_shihua", "min_def_hunshui", "min_def_jiuzui", "min_def_hunluan", "min_mogong", "min_mokang" };
		for (int t_cur = 0; t_cur < Equipment::EQUIP_ATTR_IDX_MAX; ++t_cur)
		{
			if (!GetSubNodeValue(root_element, cfg_reader[t_cur], t_wcaa.min_val_instead[t_cur]))
			{
				return -(3 + t_cur);
			}
		}

		m_craft_attr_add_map.insert(std::pair<int, WorkshopCraftAttrAdd>(t_equip_id, t_wcaa));
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int WorkshopConfig::InitItemExchange(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		int exchange_id = 0; WorkshopItemExchange tmp_wie;
		if (!GetSubNodeValue(root_element, "seq", exchange_id) || m_exchange_props_map.end() != m_exchange_props_map.find(exchange_id))
		{
			return -1;
		}

		if (!GetSubNodeValue(root_element, "exchange_type", tmp_wie.exchange_type) || tmp_wie.exchange_type < 0 || tmp_wie.exchange_type >= WORKSHOP_EXCHANGE_TYPE_MAX)
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "param_1", tmp_wie.param1))
		{
			return -3;
		}

		if (!GetSubNodeValue(root_element, "param_2", tmp_wie.param2))
		{
			return -4;
		}

		if (!GetSubNodeValue(root_element, "num", tmp_wie.need_num) || tmp_wie.need_num <= 0)
		{
			return -5;
		}

		if (!GetSubNodeValue(root_element, "price", tmp_wie.need_coin) || tmp_wie.need_coin < 0)
		{
			return -30;
		}
		
		if (!GetSubNodeValue(root_element, "fail_return_item", tmp_wie.fail_return_item) || tmp_wie.fail_return_item < 0)
		{
			return -31;
		}

		if (!GetSubNodeValue(root_element, "fail_return_num", tmp_wie.fail_return_num) || tmp_wie.fail_return_num < 0)
		{
			return -32;
		}

		UNSTD_STATIC_CHECK(5 == WORKSHOP_EXCHANGE_TYPE_MAX);
		switch (tmp_wie.exchange_type)
		{
		case WORKSHOP_EXCHANGE_PET_SKILL_BOOK:
			if (m_convert_group_map.end() == m_convert_group_map.find(tmp_wie.param1)) return -6;
			if (m_convert_group_map.end() == m_convert_group_map.find(tmp_wie.param2)) return -7;
			if (tmp_wie.need_num > ItemNamespace::MAX_BATCH_CONSUME_ITEM_NUM * 2) return -8;
			break;
		case WORKSHOP_EXCHANGE_PET_SEALS_CARD:
			if (NULL == ITEMPOOL->GetItem(tmp_wie.param1)) return -9;
			if (NULL == ITEMPOOL->GetItem(tmp_wie.param2)) return -10;
			if (NULL == ITEMPOOL->GetItem(tmp_wie.fail_return_item)) return -16;
			if (0 >= tmp_wie.fail_return_num) return -17;
			break;
		case WORKSHOP_DISCOMPOSE_FABAO:
			if (NULL == ITEMPOOL->GetItem(tmp_wie.param1)) return -10;
			if (NULL == ITEMPOOL->GetItem(tmp_wie.param2)) return -11;
			break;
		case WORKSHOP_SYNTHESIS_KEY:
			if (NULL == ITEMPOOL->GetItem(tmp_wie.param1)) return -12;
			if (NULL == ITEMPOOL->GetItem(tmp_wie.param2)) return -13;
			break;
		case WORKSHOP_SYNTHESIS_SUPER_MARROW:
			if (NULL == ITEMPOOL->GetItem(tmp_wie.param1)) return -14;
			if (NULL == ITEMPOOL->GetItem(tmp_wie.param2)) return -15;
			if (NULL == ITEMPOOL->GetItem(tmp_wie.fail_return_item)) return -18;
			if (0 >= tmp_wie.fail_return_num) return -19;
			break;
		}

		if (!GetSubNodeValue(root_element, "exchange_num", tmp_wie.give_num) || tmp_wie.give_num <= 0)
		{
			return -11;
		}

		if (!GetSubNodeValue(root_element, "rate", tmp_wie.succ_rate) || tmp_wie.succ_rate <= 0 || tmp_wie.succ_rate > RAND_UNIVERSAL_RATIO_BASE_NUM)
		{
			return -12;
		}

		m_exchange_props_map[exchange_id] = tmp_wie;
		root_element = root_element->NextSiblingElement();
	}
	return 0;
}

int WorkshopConfig::InitConvertGroup(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		int group_id = 0; WorkshopExRate tmp_con;
		if (!GetSubNodeValue(root_element, "item_group", group_id))
		{
			return -1;
		}

		if (!GetSubNodeValue(root_element, "item_id", tmp_con.item_id) || NULL == ITEMPOOL->GetItem(tmp_con.item_id))
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "rate", tmp_con.ex_rate) || tmp_con.ex_rate <= 0 || tmp_con.ex_rate > RAND_UNIVERSAL_RATIO_BASE_NUM)
		{
			return -3;
		}

		std::map<int, std::set<WorkshopExRate> >::iterator f_it = m_convert_group_map.find(group_id);
		if (m_convert_group_map.end() != f_it)		// 如果找到
		{
			if (f_it->second.end() != f_it->second.find(tmp_con)) return -4;
			f_it->second.insert(tmp_con);
		}
		else
		{
			std::set<WorkshopExRate> tmp_rate_set; tmp_rate_set.insert(tmp_con);
			m_convert_group_map[group_id] = tmp_rate_set;
		}

		root_element = root_element->NextSiblingElement();
	}
	
	// 检查总概率
	for (std::map<int, std::set<WorkshopExRate> >::iterator loop = m_convert_group_map.begin(); loop != m_convert_group_map.end(); ++loop)
	{
		int prob_total = 0;
		for (std::set<WorkshopExRate>::iterator item_it = loop->second.begin(); item_it != loop->second.end(); ++item_it)
		{
			prob_total += item_it->ex_rate;
		}

		if (RAND_UNIVERSAL_RATIO_BASE_NUM != prob_total) return -500;
	}
	
	return 0;
}

int WorkshopConfig::InitUltimateConsume(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		int equip_type = 0;
		if (!GetSubNodeValue(root_element, "equip_type", equip_type) || equip_type < ULTIMATE_EQUIP_TYPE_E_HELMET || equip_type >= ULTIMATE_EQUIP_TYPE_MAX)
		{
			return -1;
		}
		int color = 0;
		if (!GetSubNodeValue(root_element, "color", color) || color <= ItemBase::I_COLOR_INVALID || color >= ItemBase::I_COLOR_MAX)
		{
			return -2;
		}
		int limit_level = 0;
		if (!GetSubNodeValue(root_element, "limit_level", limit_level) || limit_level < 0 || limit_level > MAX_ROLE_LEVEL)
		{
			return -3;
		}

		WorkshopUltimateCfg &node_cfg = m_ultimate_consume[equip_type][color][limit_level];

		node_cfg.equip_type = equip_type;
		node_cfg.color = color;
		node_cfg.limit_level = limit_level;

		TiXmlElement * common_consume_item_list_element = root_element->FirstChildElement("common_consume_item_list");
		
		if (NULL != common_consume_item_list_element)
		{
			TiXmlElement * common_consume_item_element = common_consume_item_list_element->FirstChildElement("common_consume_item");
			while (common_consume_item_element != NULL)
			{
				ItemConfigData node_item;
				if (!node_item.ReadConfig(common_consume_item_element))
				{
					return -999;
				}
				node_cfg.common_consume_item.push_back(node_item);
				common_consume_item_element = common_consume_item_element->NextSiblingElement();
			}
		}

		TiXmlElement * added_consume_item_list_element = root_element->FirstChildElement("added_consume_item_list");

		if (NULL != added_consume_item_list_element)
		{
			TiXmlElement * added_consume_item_element = added_consume_item_list_element->FirstChildElement("added_consume_item");
			while (added_consume_item_element != NULL)
			{
				ItemConfigData node_item;
				if (!node_item.ReadConfig(added_consume_item_element))
				{
					return -999;
				}
				node_cfg.added_consume_item.push_back(node_item);
				added_consume_item_element = added_consume_item_element->NextSiblingElement();
			}
		}

		root_element = root_element->NextSiblingElement();
	}

	// 检查总概率
	for (std::map<int, std::set<WorkshopExRate> >::iterator loop = m_convert_group_map.begin(); loop != m_convert_group_map.end(); ++loop)
	{
		int prob_total = 0;
		for (std::set<WorkshopExRate>::iterator item_it = loop->second.begin(); item_it != loop->second.end(); ++item_it)
		{
			prob_total += item_it->ex_rate;
		}

		if (RAND_UNIVERSAL_RATIO_BASE_NUM != prob_total) return -500;
	}

	return 0;
}

int WorkshopConfig::InitOhterCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	m_ultimate_weight.weight_count = 0;

	for (int i = 0; i < 5; i++)
	{
		std::string read_string = STRING_SPRINTF("jg_extent_min%d", i + 1);
		if (!GetSubNodeValue(root_element, read_string, m_ultimate_weight.value_ultimate[i].jg_extent_min))
		{
			return -(10 * i + 1);
		}
		read_string = STRING_SPRINTF("jg_extent_max%d", i + 1);
		if (!GetSubNodeValue(root_element, read_string, m_ultimate_weight.value_ultimate[i].jg_extent_max))
		{
			return -(10 * i + 2);
		}
		read_string = STRING_SPRINTF("rate_%d", i + 1);
		if (!GetSubNodeValue(root_element, read_string, m_ultimate_weight.value_ultimate[i].rate))
		{
			return -(10 * i + 3);
		}
		m_ultimate_weight.weight_count += m_ultimate_weight.value_ultimate[i].rate;
	}
	return 0;
}

int WorkshopConfig::InitGodEquipReturn(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;
	while (NULL != root_element)
	{
		WorlshopGodEquipReturn node;

		if (!GetSubNodeValue(root_element, "color", node.color))
		{
			return -1;
		}

		if (!GetSubNodeValue(root_element, "level", node.level))
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "reset", node.reset))
		{
			return -3;
		}

		if (god_equip_version_id == -1)god_equip_version_id = node.reset;
		if (god_equip_version_id != node.reset)
		{
			return -999;//所有版本号必须一致
		}

		TiXmlElement * item_list_element = root_element->FirstChildElement("godequip_return_item_list");
		if (NULL != item_list_element)
		{
			TiXmlElement * item_element = item_list_element->FirstChildElement("godequip_return_item");
			while (NULL != item_element)
			{
				ItemConfigData reward_cfg;
				if (!reward_cfg.ReadConfig(item_element))
				{
					return -4;
				}
				node.turn_back_reward_vec.push_back(reward_cfg);
				item_element = item_element->NextSiblingElement();
			}
		}
		m_god_equip_return_vec.push_back(node);
		root_element = root_element->NextSiblingElement();
	}
	return 0;
}
