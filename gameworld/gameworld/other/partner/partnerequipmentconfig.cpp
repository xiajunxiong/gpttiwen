#include "partnerequipmentconfig.hpp"
#include "servercommon/struct/moneyotherdef.h"

PartnerEquipmentConfig::~PartnerEquipmentConfig()
{
	for (std::map<int, std::vector<PartnerEqRandGroups> >::iterator it = m_attr_type_rand_map.begin(); it != m_attr_type_rand_map.end(); it++)
	{
		std::vector<PartnerEqRandGroups>().swap(it->second);
	}

	for (std::map<int, std::map<int, std::vector<PartnerEqLimitCfg> > >::iterator it = m_pe_limit_map.begin(); it != m_pe_limit_map.end(); it++){
		for (std::map<int, std::vector<PartnerEqLimitCfg> >::iterator node_it = it->second.begin(); node_it != it->second.end(); node_it++){
			std::vector<PartnerEqLimitCfg>().swap(node_it->second);
		}
	}
}

bool PartnerEquipmentConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG("att_value", InitEquipmentAttrCfg);
	LOAD_CONFIG("rand_group", InitAttrRandGroupCfg);
	LOAD_CONFIG("suit", InitSuitAttributeCfg);
	LOAD_CONFIG("partner_equipment", InitPartnerEquipmentCfg);
	LOAD_CONFIG("hole", InitEquipMentHoleCfg);
	LOAD_CONFIG("partner_equipment_resolve", InitEquipResolveCfg);
	LOAD_CONFIG("partner_hole", InitEquipMentNewHoleCfg);

	return true;
}

bool PartnerEquipmentConfig::RandAttrTypeAndValue(int equip_id, BattleAttrPair out_attr[MAX_PARTNER_NATAL_CARD_VOC_NUM])
{
	const ItemBase * item_base = ITEMPOOL->GetItem(equip_id);
	if (NULL == item_base) return false;

	std::map<ItemID, PartnerEqBasicInfor>::iterator find_it = m_equipment_info_map.find(equip_id);
	if (m_equipment_info_map.end() == find_it) return false;

	for (int i = 0; i < MAX_PARTNER_NATAL_CARD_VOC_NUM; i++)
	{
		if (find_it->second.rand_group[i] <= 0)continue;
		std::map<int, std::vector<PartnerEqRandGroups> >::iterator attr_type_it = m_attr_type_rand_map.find(find_it->second.rand_group[i]);
		if (m_attr_type_rand_map.end() == attr_type_it) continue;

		int rand_prob = rand() % RAND_UNIVERSAL_RATIO_BASE_NUM;
		for (std::vector<PartnerEqRandGroups>::iterator type_it = attr_type_it->second.begin(); type_it != attr_type_it->second.end(); ++type_it)
		{
			if (rand_prob < type_it->prob_rate)
			{
				out_attr[i].attr_type = type_it->attr_type;
				break;
			}

			rand_prob -= type_it->prob_rate;
		}

		int equip_level = item_base->GetLimitLevel();
		for (std::map<ParamSection, std::map<int, std::map<int, ParamSection> > >::iterator loop_it = m_attr_value_rand_map.begin(); loop_it != m_attr_value_rand_map.end(); ++loop_it)
		{
			if (equip_level < loop_it->first.section_min || equip_level > loop_it->first.section_max) continue;
			int equip_color = item_base->GetColor();
			std::map<int, std::map<int, ParamSection> >::iterator color_find_it = loop_it->second.find(equip_color);
			if (loop_it->second.end() == color_find_it)return false;

			std::map<int, ParamSection>::iterator type_find_it = color_find_it->second.find(out_attr[i].attr_type);
			if (color_find_it->second.end() == type_find_it) return false;

			out_attr[i].attr_value = rand() % (type_find_it->second.section_max - type_find_it->second.section_min + 1) + type_find_it->second.section_min;
			break;
		}
	}

	return true;
}

bool PartnerEquipmentConfig::CheckAttrTypeAndValue(int equip_id, int attr_idx, int attr_type, int attr_value)
{
	const ItemBase * item_base = ITEMPOOL->GetItem(equip_id);
	if (NULL == item_base) return false;

	std::map<ItemID, PartnerEqBasicInfor>::iterator find_it = m_equipment_info_map.find(equip_id);
	if (m_equipment_info_map.end() == find_it) return false;

	if (attr_idx < 0 || attr_idx >= MAX_PARTNER_NATAL_CARD_VOC_NUM)return false;
	if (find_it->second.rand_group[attr_idx] <= 0)return false;
	
	std::map<int, std::vector<PartnerEqRandGroups> >::iterator attr_type_it = m_attr_type_rand_map.find(find_it->second.rand_group[attr_idx]);
	if (m_attr_type_rand_map.end() == attr_type_it)return false;

	//int rand_prob = rand() % RAND_UNIVERSAL_RATIO_BASE_NUM;
	bool attr_type_true = false;
	for (std::vector<PartnerEqRandGroups>::iterator type_it = attr_type_it->second.begin(); type_it != attr_type_it->second.end(); ++type_it)
	{
		if (attr_type == type_it->attr_type)
		{
			attr_type_true = true;
		}
	}
	if (!attr_type_true)return false;
	int equip_level = item_base->GetLimitLevel();
	bool attr_value_true = false;
	for (std::map<ParamSection, std::map<int, std::map<int, ParamSection> > >::iterator loop_it = m_attr_value_rand_map.begin(); loop_it != m_attr_value_rand_map.end(); ++loop_it)
	{
		if (equip_level < loop_it->first.section_min || equip_level > loop_it->first.section_max) continue;
		int equip_color = item_base->GetColor();
		std::map<int, std::map<int, ParamSection> >::iterator color_find_it = loop_it->second.find(equip_color);
		if (loop_it->second.end() == color_find_it)return false;

		std::map<int, ParamSection>::iterator type_find_it = color_find_it->second.find(attr_type);
		if (color_find_it->second.end() == type_find_it) return false;

		if (type_find_it->second.section_min > attr_value || type_find_it->second.section_max < attr_value)return false;
		break;
	}

	return true;
}

const PartnerEqSuitInform * PartnerEquipmentConfig::GetEquipmentSuitID(int equip_id)
{
	std::map<ItemID, PartnerEqBasicInfor>::iterator index_it = m_equipment_info_map.find(equip_id);
	if (m_equipment_info_map.end() == index_it) return 0;

	std::map<int, PartnerEqSuitInform>::iterator id_it = m_index_to_suit_map.find(index_it->second.suit_index);
	if (m_index_to_suit_map.end() != id_it) return & id_it->second;

	return 0;
}

const PartnerEqSuitEffect * PartnerEquipmentConfig::GetPartnerEquipmentAttrAdditionCfg(int suit_id, int level, int color)
{
	std::map<int, std::map<int, std::map<int, PartnerEqSuitEffect> > >::iterator id_it = m_suit_attr_map.find(suit_id);
	if (m_suit_attr_map.end() == id_it) return NULL;

	std::map<int, std::map<int, PartnerEqSuitEffect> >::iterator lv_it = id_it->second.find(level);
	if (id_it->second.end() == lv_it) return NULL;

	std::map<int, PartnerEqSuitEffect>::iterator color_it = lv_it->second.find(color);
	if (lv_it->second.end() == color_it) return NULL;

	return &color_it->second;
}

const PartnerEqLimitCfg * PartnerEquipmentConfig::GetPEUnlockLimitByUnlockLevel(int partner_id, int slot_idx, int unlock_level)
{
	std::map<int, std::map<int, std::vector<PartnerEqLimitCfg> > >::iterator it = m_pe_limit_map.find(partner_id);
	if (it != m_pe_limit_map.end())
	{
		std::map<int, std::vector<PartnerEqLimitCfg> >::iterator it_slot = it->second.find(slot_idx);
		if (it_slot != it->second.end())
		{
			for (std::vector<PartnerEqLimitCfg>::iterator vec_it = it_slot->second.begin(); vec_it != it_slot->second.end();vec_it++)
			{
				if (vec_it->hole_level == unlock_level)
				{
					return &(*vec_it);
				}
			}
		}
	}
	return NULL;
}

const PartnerEqLimitCfg * PartnerEquipmentConfig::GetPEUnlockLimitBegin(int partner_id, int slot_idx)
{
	std::map<int, std::map<int, std::vector<PartnerEqLimitCfg> > >::iterator it = m_pe_limit_map.find(partner_id);
	if (it != m_pe_limit_map.end())
	{
		std::map<int, std::vector<PartnerEqLimitCfg> >::iterator it_slot = it->second.find(slot_idx);
		if (it_slot != it->second.end())
		{
			std::vector<PartnerEqLimitCfg>::iterator vec_it = it_slot->second.begin();
			if(vec_it != it_slot->second.end())
			{
				return &(*vec_it);
			}
		}
	}
	return NULL;
}

const std::vector<PartnerEqLimitCfg>* PartnerEquipmentConfig::GetPEUlockLimitVec(int partner_id, int slot_idx)
{
	std::map<int, std::map<int, std::vector<PartnerEqLimitCfg> > >::iterator it = m_pe_limit_map.find(partner_id);
	if (it != m_pe_limit_map.end())
	{
		std::map<int, std::vector<PartnerEqLimitCfg> >::iterator it_slot = it->second.find(slot_idx);
		if (it_slot != it->second.end())
		{
			return &it_slot->second;
		}
	}
	return NULL;
}

const NewPartnerEqLimitCfg * PartnerEquipmentConfig::GetNewPEUnlockLimitBegin(int partner_id, int slot_idx)
{
	if (partner_id >= 0 && partner_id < PARTNER_NUM_MAX)
	{
		if (slot_idx >= 0 && slot_idx < PE_TYPE_MAX)
		{
			return &m_new_pe_limit[partner_id][slot_idx];
		}
	}
	return NULL;
}

const int PartnerEquipmentConfig::GetPEResolveValue(int PE_level, int PE_color)
{
	if (PE_level <= 0 || PE_level > MAX_ROLE_LEVEL)return -1;
	if (PE_color <= ItemBase::I_COLOR_INVALID || PE_color >= ItemBase::I_COLOR_MAX)return -1;
	return m_resolve_2[PE_level][PE_color];
}

int PartnerEquipmentConfig::InitPartnerEquipmentCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		ItemID item_id = 0; PartnerEqBasicInfor tmp_pebi;
		if (!GetSubNodeValue(root_element, "item_id", item_id) || NULL == ITEMPOOL->GetItem(item_id))
		{
			return -1;
		}
		
		if (!GetSubNodeValue(root_element, "att_group", tmp_pebi.rand_group[0]))
		{
			return -2;
		}

		if (tmp_pebi.rand_group[0] > 0)
		{
			if (m_attr_type_rand_map.end() == m_attr_type_rand_map.find(tmp_pebi.rand_group[0]))
			{
				return -22;
			}
		}

		if (!GetSubNodeValue(root_element, "index", tmp_pebi.suit_index) || (0 != tmp_pebi.suit_index && m_index_to_suit_map.end() == m_index_to_suit_map.find(tmp_pebi.suit_index)))
		{
			return -3;
		}

		if (!GetSubNodeValue(root_element, "att_group_2", tmp_pebi.rand_group[1]))
		{
			return -4;
		}

		if (tmp_pebi.rand_group[1] > 0)
		{
			if (m_attr_type_rand_map.end() == m_attr_type_rand_map.find(tmp_pebi.rand_group[1]))
			{
				return -44;
			}
		}

		if (!GetSubNodeValue(root_element, "att_group_3", tmp_pebi.rand_group[2]))
		{
			return -5;
		}

		if (tmp_pebi.rand_group[2] > 0)
		{
			if (m_attr_type_rand_map.end() == m_attr_type_rand_map.find(tmp_pebi.rand_group[2]))
			{
				return -55;
			}
		}

		if (!GetSubNodeValue(root_element, "att_group_4", tmp_pebi.rand_group[3]))
		{
			return -6;
		}

		if (tmp_pebi.rand_group[3] > 0)
		{
			if (m_attr_type_rand_map.end() == m_attr_type_rand_map.find(tmp_pebi.rand_group[3]))
			{
				return -55;
			}
		}

		m_equipment_info_map[item_id] = tmp_pebi;
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int PartnerEquipmentConfig::InitAttrRandGroupCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		int group_id = 0; PartnerEqRandGroups tmp_perg;
		if (!GetSubNodeValue(root_element, "att_group", group_id) || group_id < 0)
		{
			return -1;
		}

		if (!GetSubNodeValue(root_element, "att_type", tmp_perg.attr_type) || tmp_perg.attr_type < 0 || tmp_perg.attr_type >= BATTLE_ATTR_MAX)
		{
			return -2;
		}

		switch (tmp_perg.attr_type)
		{
		case BATTLE_ATTR_CUR_HP:
		case BATTLE_ATTR_CUR_MP:
			return -3;
		}

		if (!GetSubNodeValue(root_element, "rate", tmp_perg.prob_rate) || tmp_perg.prob_rate <= 0 || tmp_perg.prob_rate > RAND_UNIVERSAL_RATIO_BASE_NUM)
		{
			return -4;
		}

		std::map<int, std::vector<PartnerEqRandGroups> >::iterator find_it = m_attr_type_rand_map.find(group_id);
		if (m_attr_type_rand_map.end() != find_it)
		{
			find_it->second.push_back(tmp_perg);
		}
		else
		{
			std::vector<PartnerEqRandGroups> tmp_vec_perg; tmp_vec_perg.push_back(tmp_perg);
			m_attr_type_rand_map[group_id] = tmp_vec_perg;
		}

		root_element = root_element->NextSiblingElement();
	}

	for (std::map<int, std::vector<PartnerEqRandGroups> >::iterator loop_it = m_attr_type_rand_map.begin(); loop_it != m_attr_type_rand_map.end(); ++loop_it)
	{
		int rate_total = 0;
		for (std::vector<PartnerEqRandGroups>::iterator rand_it = loop_it->second.begin(); rand_it != loop_it->second.end(); ++rand_it)
		{
			rate_total += rand_it->prob_rate;
		}

		if (RAND_UNIVERSAL_RATIO_BASE_NUM != rate_total) return -500;
	}

	return 0;
}

int PartnerEquipmentConfig::InitEquipmentAttrCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		int color = 0; int attr_type = 0; ParamSection level_sec, attr_sec;
		if (!GetSubNodeValue(root_element, "min_level", level_sec.section_min) || level_sec.section_min <= 0)
		{
			return -1;
		}

		if (!GetSubNodeValue(root_element, "max_level", level_sec.section_max) || level_sec.section_max < level_sec.section_min)
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "color", color) || color <= ItemBase::I_COLOR_INVALID || color >= ItemBase::I_COLOR_MAX)
		{
			return -3;
		}

		if (!GetSubNodeValue(root_element, "att_type", attr_type) || attr_type < 0 || attr_type >= BATTLE_ATTR_MAX)
		{
			return -4;
		}

		switch (attr_type)
		{
		case BATTLE_ATTR_CUR_HP:
		case BATTLE_ATTR_CUR_MP:
			return -5;
		}

		if (!GetSubNodeValue(root_element, "min", attr_sec.section_min) || attr_sec.section_min < 0)
		{
			return -6;
		}

		if (!GetSubNodeValue(root_element, "max", attr_sec.section_max) || attr_sec.section_max < attr_sec.section_min)
		{
			return -7;
		}

		m_attr_value_rand_map[level_sec][color][attr_type] = attr_sec;
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int PartnerEquipmentConfig::InitSuitAttributeCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		int index = 0; PartnerEqSuitEffect tmp_con; PartnerEqSuitInform tmp_pei;
		if (!GetSubNodeValue(root_element, "index", index) || m_index_to_suit_map.end() != m_index_to_suit_map.find(index))
		{
			return -3;
		}

		if (!GetSubNodeValue(root_element, "suit_id", tmp_pei.suit_id))
		{
			return -1;
		}

		if (!GetSubNodeValue(root_element, "suit_level", tmp_pei.suit_level))
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "suit_color", tmp_pei.suit_color))
		{
			return -3;
		}

		const char * base_attr[PARTNER_ATTR_MAX] = { "maxhp", "maxmp", "gongji", "fangyu", "minjie", "huifu", "jingshen" };
		for (int i = 0; i < PARTNER_ATTR_MAX; ++i)
		{
			if (!GetSubNodeValue(root_element, base_attr[i], tmp_con.basic_attr[i]))
			{
				return -(100 + i);
			}
		}

		const char * resi_attr[PARTNER_RESI_MAX] = { "def_zhongdu", "def_hunshui", "def_shihua", "def_hunluan", "def_jiuzui" };
		for (int i = 0; i < PARTNER_RESI_MAX; ++i)
		{
			if (!GetSubNodeValue(root_element, resi_attr[i], tmp_con.resis_attr[i]))
			{
				return -(200 + i);
			}
		}

		const char * amen_attr[PARTNER_AMEN_MAX] = { "bisha", "mingzhong", "shanduo", "fanji", "mogong", "mokang", "bisha_dmg" ,"jianren" };
		for (int i = 0; i < PARTNER_AMEN_MAX; ++i)
		{
			if (!GetSubNodeValue(root_element, amen_attr[i], tmp_con.amend_attr[i]))
			{
				return -(300 + i);
			}
		}

		m_suit_attr_map[tmp_pei.suit_id][tmp_pei.suit_level][tmp_pei.suit_color] = tmp_con;
		m_index_to_suit_map[index] = tmp_pei;
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int PartnerEquipmentConfig::InitEquipMentHoleCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;
	int pre_level = 0;
	int pre_hole_postion = 0;
	while (NULL != root_element)
	{
		int partner_id = 0;
		if (!GetSubNodeValue(root_element, "partner_id", partner_id) || partner_id <= 0)
		{
			return -1;
		}
		int hole_postion = 0;
		if (!GetSubNodeValue(root_element, "hole_postion", hole_postion) || hole_postion < PE_TYPE_SKILY + 1 || hole_postion >= PE_TYPE_MAX + 1)
		{
			return -2;
		}

		if (pre_hole_postion != hole_postion)
		{
			pre_level = 0;
		}
		pre_hole_postion = hole_postion;

		PartnerEqLimitCfg node_cfg;
		if (!GetSubNodeValue(root_element, "hole_level", node_cfg.hole_level) || node_cfg.hole_level < 0)
		{
			return -3;
		}

		if (pre_level != 0 && pre_level + PARTNER_HOLE_PER_LEVEL != node_cfg.hole_level)
		{
			return -4;
		}

		pre_level = node_cfg.hole_level;

		if (!GetSubNodeValue(root_element, "hole_need", node_cfg.hole_need) || NULL == ITEMPOOL->GetItem(node_cfg.hole_need))
		{
			return -5;
		}

		if (!GetSubNodeValue(root_element, "hole_num", node_cfg.hole_num) || node_cfg.hole_num <= 0)
		{
			return -6;
		}

		std::map<int, std::map<int, std::vector<PartnerEqLimitCfg> > >::iterator it = m_pe_limit_map.find(partner_id);
		if (it != m_pe_limit_map.end())
		{
			std::map<int, std::vector<PartnerEqLimitCfg> >::iterator it2 = it->second.find(hole_postion);
			if (it2 != it->second.end())
			{
				it2->second.push_back(node_cfg);
			}
			else
			{
				it->second[hole_postion].push_back(node_cfg);
			}
		}
		else
		{
			m_pe_limit_map[partner_id][hole_postion].push_back(node_cfg);
		}

		
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int PartnerEquipmentConfig::InitEquipResolveCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		int level = 0;
		if (!GetSubNodeValue(root_element, "level", level) || level <= 0 || level > MAX_ROLE_LEVEL) return -1;

		int color = 0;
		if (!GetSubNodeValue(root_element, "color", color) || color <= ItemBase::I_COLOR_INVALID || color > ItemBase::I_COLOR_MAX) return -2;

		int & cfg = m_resolve_2[level][color];

		if (!GetSubNodeValue(root_element, "qiyun", cfg) || cfg < 0) return -3;

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int PartnerEquipmentConfig::InitEquipMentNewHoleCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;
	while (NULL != root_element)
	{
		int partner_id = 0;
		if (!GetSubNodeValue(root_element, "partner_id", partner_id) || partner_id <= 0 || partner_id >= PARTNER_NUM_MAX)
		{
			return -1;
		}
		int hole_postion = 0;
		if (!GetSubNodeValue(root_element, "hole_postion", hole_postion) || hole_postion < PE_TYPE_SKILY + 1 || hole_postion >= PE_TYPE_MAX + 1)
		{
			return -2;
		}

		NewPartnerEqLimitCfg &node_cfg = m_new_pe_limit[partner_id][hole_postion - 1];
		if (!GetSubNodeValue(root_element, "hole_level", node_cfg.hole_level) || node_cfg.hole_level < 0)
		{
			return -3;
		}

		if (!GetSubNodeValue(root_element, "hole_need", node_cfg.hole_need))
		{
			return -5;
		}

		if (!GetSubNodeValue(root_element, "hole_num", node_cfg.hole_num) || node_cfg.hole_num < 0)
		{
			return -6;
		}

		if (!GetSubNodeValue(root_element, "price_type", node_cfg.price_type) || node_cfg.price_type < 0 || node_cfg.price_type >= MONEY_TYPE_MAX)
		{
			return -7;
		}

		if (!GetSubNodeValue(root_element, "price_num", node_cfg.price_num) || node_cfg.price_num < 0)
		{
			return -8;
		}

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}