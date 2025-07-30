#include "peishiconfig.hpp"
#include "checkresourcecenter.hpp"
#include "item/itempool.h"
#include "servercommon/flyupsystemdef.h"

PeiShiConfig::PeiShiConfig()
{
	for (int i = 0; i < ACCESSORIES_SUIT_TYPE_MAX; i++)
	{
		for (int j = 0; j < ItemBase::I_COLOR_MAX; j++)
		{
			point_entry_cfg[i][j] = NULL;
		}
	}
	for (int i = 0; i < MAX_PEISHI_SEQ; i++)
	{
		for (int j = 0; j < BATTLE_ATTR_MAX; j++)
		{
			point_entry_reward_cfg[i][j] = NULL;
		}
	}
	for (int i = 0; i < ACCESSORIES_SUIT_TYPE_MAX; i++)
	{
		for (int j = 0; j < ItemBase::I_COLOR_MAX; j++)
		{
			point_peishi_skill_cfg[i][j] = NULL;
		}
	}
	for (int i = 0; i < MAX_UINT16; i++)
	{
		for (int j = 0; j < MAX_PEISHI_STRENGTHEN_LEVEL; j++)
		{
			strengthen_level_item_list[i][j] = NULL;
		}
	}

	for (int i = 0; i < ItemBase::I_COLOR_MAX; i++)
	{
		for (int j = 0; j < MAX_ACCESSORIES_VOC_NUM; j++)
		{
			change_list[i][j] = NULL;
		}
	}
}

PeiShiConfig::~PeiShiConfig()
{
	std::vector<PeishiEntryCfg>().swap(m_entry_cfg);
	for (std::map<int, std::vector<EntryRewardCfg> >::iterator it = m_entry_reward_cfg.begin(); it!= m_entry_reward_cfg.end(); it++)
	{
		std::vector<EntryRewardCfg>().swap(it->second);
	}
	std::vector<PeishiSkillCfg>().swap(m_peishi_skill_cfg);
	std::vector<ChuanChengCfg>().swap(m_chuancheng_cfg);
	std::vector<ChuanChengCostCfg>().swap(m_chuancheng_cost_cfg);
	std::vector<PeiShiStrengthenCfg>().swap(m_strengthen_cfg);
	std::vector<PeiShiChangeCfg>().swap(m_change_cfg);
}

bool PeiShiConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG("other", InitOtherCfg);
	LOAD_CONFIG("entry", InitEntryCfg);
	LOAD_CONFIG("entry_reward", InitEntryRewardCfg);
	LOAD_CONFIG("peishi_skill", InitPeishiSkillCfg);
	LOAD_CONFIG("chuancheng", InitChuanChengCfg);
	LOAD_CONFIG("chuancheng_cost", InitChuanChengCostCfg);
	LOAD_CONFIG_2("strengthen", InitStrengthenCfg);
	LOAD_CONFIG_2("display", InitChangeCfg);
	LOAD_CONFIG_2("xianshi", InitSpecialEquipCfg);
	return true;
}

const PeishiEntryCfg * PeiShiConfig::GetEntryCfg(int suit_type, int color)
{
	if (suit_type < 0 || suit_type >= ACCESSORIES_SUIT_TYPE_MAX)return NULL;
	if (color < 0 || color >= ItemBase::I_COLOR_MAX)return NULL;
	
	return point_entry_cfg[suit_type][color];
}

const EntryRewardCfg * PeiShiConfig::RandEntryRewardCfg(int seq)
{
	std::map<int, std::vector<EntryRewardCfg> >::iterator it = m_entry_reward_cfg.find(seq);
	if (it != m_entry_reward_cfg.end())
	{
		if (it->second.size() > 0)
		{
			int rand_idx = rand() % (int)it->second.size();
			return &it->second[rand_idx];
		}
	}
	return NULL;
}

const EntryRewardCfg * PeiShiConfig::GetEntryRewardCfg(int seq, int attr_type)
{
	if (seq < 0 || seq >= MAX_PEISHI_SEQ)return NULL;
	if (attr_type < 0 || attr_type >= BATTLE_ATTR_MAX)return NULL;

	return point_entry_reward_cfg[seq][attr_type];
}

const PeishiSkillCfg * PeiShiConfig::GetPeiShiSkillCfg(int suit_type, int color)
{
	if (suit_type < 0 || suit_type >= ACCESSORIES_SUIT_TYPE_MAX)return NULL;
	if (color < 0 || color >= ItemBase::I_COLOR_MAX)return NULL;

	return point_peishi_skill_cfg[suit_type][color];
}

const ChuanChengCfg * PeiShiConfig::GetChuanChengCfg(int num)
{
	for (std::vector<ChuanChengCfg>::reverse_iterator it = m_chuancheng_cfg.rbegin(); it != m_chuancheng_cfg.rend(); it++)
	{
		if (it->entry_num <= num)
		{
			return &(*it);
		}
	}

	return NULL;
}

const ChuanChengCostCfg * PeiShiConfig::GetChuanChengCostCfg(int level, int color)
{
	for (std::vector<ChuanChengCostCfg>::iterator it = m_chuancheng_cost_cfg.begin(); it != m_chuancheng_cost_cfg.end(); it++)
	{
		if (level == it->Material_level && color == it->Material_quality)
		{
			return &(*it);
		}
	}
	return NULL;
}

const PeiShiStrengthenCfg * PeiShiConfig::GetPeiShiStrengthenCfg(int item_id, int level)
{
	// 这里返回的是level + 1的配置
	if (item_id >= 0 && item_id < MAX_UINT16)
	{
		if (level >= 0 && level < MAX_PEISHI_STRENGTHEN_LEVEL)
		{
			return strengthen_level_item_list[item_id][level];
		}
	}
	return NULL;
}

const PeiShiChangeCfg * PeiShiConfig::GetChangeCfg(int quality, int entry_num)
{
	if (quality > ItemBase::I_COLOR_INVALID && quality < ItemBase::I_COLOR_MAX)
	{
		if (entry_num >= 0 && entry_num < MAX_PEISHI_STRENGTHEN_LEVEL)
		{
			return change_list[quality][entry_num];
		}
	}
	return NULL;
}

const PeiShiSpecialEquipCfg * PeiShiConfig::GetSpecialEquip(int type, int level)
{
	std::map<int, std::map<int, PeiShiSpecialEquipCfg> >::iterator it = special_equip_map.find(type);
	if (it != special_equip_map.end())
	{
		std::map<int, PeiShiSpecialEquipCfg>::iterator it2 = it->second.find(level);
		if (it->second.end() != it2)
		{
			return &it2->second;
		}
	}
	return NULL;
}

int PeiShiConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;

	if (!GetSubNodeValue(root_element, "open_level", m_other_cfg.open_level) || m_other_cfg.open_level < 0) return -1;

	if (!GetSubNodeValue(root_element, "entry_exp", m_other_cfg.entry_exp) || m_other_cfg.entry_exp < 0) return -2;

	if (!GetSubNodeValue(root_element, "tianmingdan_id", m_other_cfg.tianmingdan_id) || m_other_cfg.tianmingdan_id < 0) return -3;

	if (!GetSubNodeValue(root_element, "tianmingdan_exp", m_other_cfg.tianmingdan_exp) || m_other_cfg.tianmingdan_exp < 0) return -4;

	if (!GetSubNodeValue(root_element, "feisheng_open", m_other_cfg.feisheng_open) || m_other_cfg.feisheng_open < 0 || m_other_cfg.feisheng_open > MAX_ROLE_LEVEL) return -5;

	if (!GetSubNodeValue(root_element, "need_feisheng", m_other_cfg.need_feisheng) || m_other_cfg.need_feisheng <= FlyUpSystemParam::FLY_UP_STAG_NO || m_other_cfg.need_feisheng >= FlyUpSystemParam::FLY_UP_STAG_MAX) return -6;

	if (!GetSubNodeValue(root_element, "displace_card", m_other_cfg.displace_card) || NULL == ITEMPOOL->GetItem(m_other_cfg.displace_card)) return -7;

	if (!GetSubNodeValue(root_element, "displace_card_feisheng", m_other_cfg.displace_card_feisheng) || NULL == ITEMPOOL->GetItem(m_other_cfg.displace_card_feisheng)) return -8;

	return 0;
}

int PeiShiConfig::InitEntryCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;

	while (NULL != root_element)
	{
		PeishiEntryCfg node_cfg;

		if (!GetSubNodeValue(root_element, "suit_type", node_cfg.suit_type)) return -1;

		if (!GetSubNodeValue(root_element, "peishi_color", node_cfg.peishi_color)) return -2;

		if (!GetSubNodeValue(root_element, "seq", node_cfg.seq)) return -3;

		m_entry_cfg.push_back(node_cfg);
		root_element = root_element->NextSiblingElement();
	}

	for (std::vector<PeishiEntryCfg>::iterator it = m_entry_cfg.begin(); it != m_entry_cfg.end(); it++)
	{
		if (it->suit_type >= 0 || it->suit_type < ACCESSORIES_SUIT_TYPE_MAX ||
			it->peishi_color >= 0 || it->peishi_color < ItemBase::I_COLOR_MAX)
		{
			point_entry_cfg[it->suit_type][it->peishi_color] = &(*it);
		}
	}

	return 0;
}

int PeiShiConfig::InitEntryRewardCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;

	while (NULL != root_element)
	{
		int seq = 0;
		if (!GetSubNodeValue(root_element, "seq", seq) || seq < 0 || seq >= MAX_PEISHI_SEQ) return -1;
		std::vector<EntryRewardCfg> &node = m_entry_reward_cfg[seq];
		EntryRewardCfg node_cfg;
		node_cfg.seq = seq;
		if (!GetSubNodeValue(root_element, "att_type", node_cfg.att_type) || node_cfg.att_type < 0 || node_cfg.att_type >= BATTLE_ATTR_MAX) return -2;
		
		if (!GetSubNodeValue(root_element, "att_min", node_cfg.att_min) || node_cfg.att_min < 0) return -3;
		if (!GetSubNodeValue(root_element, "att_max", node_cfg.att_max) || node_cfg.att_max < 0) return -4;
		node.push_back(node_cfg);
		root_element = root_element->NextSiblingElement();
	}

	for (std::map<int, std::vector<EntryRewardCfg> >::iterator it = m_entry_reward_cfg.begin(); it != m_entry_reward_cfg.end(); it++)
	{
		for (std::vector<EntryRewardCfg>::iterator it_2 = it->second.begin(); it_2 != it->second.end(); it_2++)
		{
			if (it_2->seq >= 0 && it_2->seq < MAX_PEISHI_SEQ &&
				it_2->att_type >= BATTLE_ATTR_MIN && it_2->att_type < BATTLE_ATTR_MAX)
			{
				point_entry_reward_cfg[it_2->seq][it_2->att_type] = &(*it_2);
			}
		}
	}

	return 0;
}

int PeiShiConfig::InitPeishiSkillCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;

	while (NULL != root_element)
	{
		PeishiSkillCfg node;
		if (!GetSubNodeValue(root_element, "suit_type", node.suit_type)) return -1;
		if (!GetSubNodeValue(root_element, "min_color", node.min_color)) return -2;
		if (!GetSubNodeValue(root_element, "skill_1", node.skill_1)) return -3;
		if (!GetSubNodeValue(root_element, "skill1_profession", node.skill1_profession)) return -4;
		if (!GetSubNodeValue(root_element, "skill_2", node.skill_2)) return -5;
		if (!GetSubNodeValue(root_element, "skill2_profession", node.skill2_profession)) return -6;
		if (!GetSubNodeValue(root_element, "skill_level", node.skill_level)) return -7;

		m_peishi_skill_cfg.push_back(node);
		root_element = root_element->NextSiblingElement();
	}

	for (std::vector<PeishiSkillCfg>::iterator it = m_peishi_skill_cfg.begin(); it != m_peishi_skill_cfg.end(); it++)
	{
		if (it->suit_type >= 0 || it->suit_type < ACCESSORIES_SUIT_TYPE_MAX ||
			it->min_color >= 0 || it->min_color < ItemBase::I_COLOR_MAX)
		{
			point_peishi_skill_cfg[it->suit_type][it->min_color] = &(*it);
		}
	}

	return 0;
}

int PeiShiConfig::InitChuanChengCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;
	int pre_entry_num = 0;
	while (NULL != root_element)
	{
		ChuanChengCfg node_cfg;
		if (!GetSubNodeValue(root_element, "entry_num", node_cfg.entry_num)) return -1;
		if (!GetSubNodeValue(root_element, "chuancheng_entry_num", node_cfg.chuancheng_entry_num)) return -2;
		if (node_cfg.entry_num < pre_entry_num)return -3;//必须前一个词条比后一个大
		m_chuancheng_cfg.push_back(node_cfg);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int PeiShiConfig::InitChuanChengCostCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;
	while (NULL != root_element)
	{
		ChuanChengCostCfg node_cfg;
		if (!GetSubNodeValue(root_element, "Material_level", node_cfg.Material_level)) return -1;
		if (!GetSubNodeValue(root_element, "Material_quality", node_cfg.Material_quality)) return -2;
		if (!GetSubNodeValue(root_element, "coin", node_cfg.coin)) return -3;
		m_chuancheng_cost_cfg.push_back(node_cfg);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int PeiShiConfig::InitStrengthenCfg(TiXmlElement * RootElement, std::string & errormsg)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;
	while (NULL != root_element)
	{
		PeiShiStrengthenCfg node_cfg;
		if (!GetSubNodeValue(root_element, "peishi_id", node_cfg.peishi_id) || NULL == ITEMPOOL->GetItem(node_cfg.peishi_id)) return -1;
		if (!GetSubNodeValue(root_element, "peishi_level", node_cfg.peishi_level) || node_cfg.peishi_level - 1 < 0 || node_cfg.peishi_level - 1 >= MAX_PEISHI_STRENGTHEN_LEVEL) return -2;
		if (!GetSubNodeValue(root_element, "role_level", node_cfg.role_level) || node_cfg.role_level < 0 || node_cfg.role_level > MAX_ROLE_LEVEL) return -3;
		if (!GetSubNodeValue(root_element, "att_type_1", node_cfg.att_type_1)) return -4;
		if (!GetSubNodeValue(root_element, "attr_value_1", node_cfg.attr_value_1)) return -5;
		if (!GetSubNodeValue(root_element, "att_type_2", node_cfg.att_type_2)) return -6;
		if (!GetSubNodeValue(root_element, "attr_value_2", node_cfg.attr_value_2)) return -7;

		TiXmlElement * list_element = root_element->FirstChildElement("ltem_list_list");
		if (NULL != list_element)
		{
			TiXmlElement * item_element = list_element->FirstChildElement("ltem_list");
			while (NULL != item_element)
			{
				ItemConfigData item;
				if (!item.ReadConfig(item_element))
					return -4;

				node_cfg.cost_item_list.push_back(item);
				item_element = item_element->NextSiblingElement();
			}
		}

		m_strengthen_cfg.push_back(node_cfg);
		root_element = root_element->NextSiblingElement();
	}

	for (std::vector<PeiShiStrengthenCfg>::iterator it = m_strengthen_cfg.begin(); it != m_strengthen_cfg.end(); it++)
	{
		if (it->peishi_id >= 0 && it->peishi_id < MAX_UINT16)
		{
			if (it->peishi_level - 1 >= 0 && it->peishi_level - 1 <= MAX_ROLE_LEVEL)
			{
				strengthen_level_item_list[it->peishi_id][it->peishi_level - 1] = &(*it);
			}
		}
	}

	return 0;
}

int PeiShiConfig::InitChangeCfg(TiXmlElement * RootElement, std::string & errormsg)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;
	while (NULL != root_element)
	{
		PeiShiChangeCfg node_cfg;
		if (!GetSubNodeValue(root_element, "displace_quality", node_cfg.displace_quality) || node_cfg.displace_quality <= ItemBase::I_COLOR_INVALID || node_cfg.displace_quality >= ItemBase::I_COLOR_MAX) return -1;
		if (!GetSubNodeValue(root_element, "entry", node_cfg.entry) || node_cfg.entry - 1 < 0 || node_cfg.entry - 1 >= MAX_ACCESSORIES_VOC_NUM) return -2;
		if (!GetSubNodeValue(root_element, "cost_num", node_cfg.cost_num) || node_cfg.cost_num < 0) return -3;

		m_change_cfg.push_back(node_cfg);
		root_element = root_element->NextSiblingElement();
	}

	for (std::vector<PeiShiChangeCfg>::iterator it = m_change_cfg.begin(); it != m_change_cfg.end(); it++)
	{
		if (it->displace_quality > ItemBase::I_COLOR_INVALID && it->displace_quality < ItemBase::I_COLOR_MAX)
		{
			if (it->entry - 1 >= 0 && it->entry - 1 <= MAX_ACCESSORIES_VOC_NUM)
			{
				change_list[it->displace_quality][it->entry - 1] = &(*it);
			}
		}
	}

	return 0;
}

int PeiShiConfig::InitSpecialEquipCfg(TiXmlElement * RootElement, std::string & errormsg)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;
	while (NULL != root_element)
	{
		int type = 0;
		if (!GetSubNodeValue(root_element, "type", type) || type < 0) return -__LINE__;
		int level = 0;
		if (!GetSubNodeValue(root_element, "level", level) || level < 0) return -__LINE__;
		PeiShiSpecialEquipCfg &node_cfg = special_equip_map[type][level];
		node_cfg.type = type;
		node_cfg.level = level;
		if (!GetSubNodeValue(root_element, "spec_value_2", node_cfg.spec_value_2)) return -__LINE__;
		
		if (!GetSubNodeValue(root_element, "att_type_1", node_cfg.attr_type_list[0])) return -__LINE__;
		if (!GetSubNodeValue(root_element, "attr_value_1", node_cfg.attr_value_list[0])) return -__LINE__;

		if (!GetSubNodeValue(root_element, "att_type_2", node_cfg.attr_type_list[1])) return -__LINE__;
		if (!GetSubNodeValue(root_element, "attr_value_2", node_cfg.attr_value_list[1])) return -__LINE__;

		if (!GetSubNodeValue(root_element, "att_type_3", node_cfg.attr_type_list[2])) return -__LINE__;
		if (!GetSubNodeValue(root_element, "attr_value_3", node_cfg.attr_value_list[2])) return -__LINE__;

		if (!GetSubNodeValue(root_element, "att_type_4", node_cfg.attr_type_list[3])) return -__LINE__;
		if (!GetSubNodeValue(root_element, "attr_value_4", node_cfg.attr_value_list[3])) return -__LINE__;

		if (!GetSubNodeValue(root_element, "att_type_5", node_cfg.attr_type_list[4])) return -__LINE__;
		if (!GetSubNodeValue(root_element, "attr_value_5", node_cfg.attr_value_list[4])) return -__LINE__;

		if (!GetSubNodeValue(root_element, "att_type_6", node_cfg.attr_type_list[5])) return -__LINE__;
		if (!GetSubNodeValue(root_element, "attr_value_6", node_cfg.attr_value_list[5])) return -__LINE__;
		
		TiXmlElement * list_element = root_element->FirstChildElement("item_list_list");
		if (NULL != list_element)
		{
			TiXmlElement * item_element = list_element->FirstChildElement("item_list");
			while (NULL != item_element)
			{
				ItemConfigData item;
				if (!item.ReadConfig(item_element))
					return -4;

				node_cfg.cost_vec.push_back(item);
				item_element = item_element->NextSiblingElement();
			}
		}

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}
