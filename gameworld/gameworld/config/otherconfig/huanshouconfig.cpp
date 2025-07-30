#include "huanshouconfig.hpp"

#include "item/itempool.h"
#include "skill/skillpool.hpp"

#include "servercommon/struct/battle/battle_def.hpp"

HuanShouConfig::HuanShouConfig()
{
}

HuanShouConfig::~HuanShouConfig()
{
	for (int i = 0; i < ARRAY_ITEM_COUNT(m_huan_shou_egg_list); ++i)
	{
		std::vector<HuanShouEggCfg>().swap(m_huan_shou_egg_list[i]);
	}
}

bool HuanShouConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG_2("other", InitOtherCfg);
	LOAD_CONFIG_2("att_group", InitAttrGroupCfg);
	LOAD_CONFIG_2("potential_point", InitPointUnlcokCfg);
	LOAD_CONFIG_2("huanshou_potential", InitPointGroupCfg);	//潜能组配置需在幻兽基础配置读取之前

	LOAD_CONFIG_2("huanshou", InitBaseCfg);
	LOAD_CONFIG_2("egg_item", InitEggItemCfg);	//幻兽蛋道具配置需在幻兽蛋配置读取前
	LOAD_CONFIG_2("huanshou_egg", InitEggCfg);
	LOAD_CONFIG_2("huanshou_hole", InitSkillHoleCfg);



	return true;
}

const HuanShouBaseCfg * HuanShouConfig::GetBaseCfg(int huanshou_id) const
{
	if (huanshou_id < 0 || huanshou_id >= m_base_count || huanshou_id >= ARRAY_ITEM_COUNT(m_base_cfg_list))
	{
		return NULL;
	}

	return &m_base_cfg_list[huanshou_id];
}

const std::vector<ItemConfigData>* HuanShouConfig::GetDecomposeCfg(int item_id) const
{
	std::map<int, std::vector<ItemConfigData> >::const_iterator it = m_decompose_map.find(item_id);
	if (it == m_decompose_map.end() || it->second.empty())
	{
		return NULL;
	}

	return &it->second;
}

const HuanShouAttrGroupCfg * HuanShouConfig::GetAttrGroupCfg(int attr_group_id) const
{
	std::map<int, HuanShouAttrGroupCfg>::const_iterator it = m_attr_group_map.find(attr_group_id);
	if (it == m_attr_group_map.end())
	{
		return NULL;
	}

	return &it->second;
}

const HuanShouAttrGroupLevelCfg * HuanShouConfig::GetAttrGroupLevelCfg(int attr_group_id, int str_level) const
{
	std::map<int, HuanShouAttrGroupCfg>::const_iterator it = m_attr_group_map.find(attr_group_id);
	if (it == m_attr_group_map.end() || str_level <= 0 || str_level > it->second.max_str_level || str_level > ARRAY_ITEM_COUNT(it->second.level_list))
	{
		return NULL;
	}

	return &it->second.level_list[str_level - 1];
}

const HuanShouAttrGroupLevelCfg * HuanShouConfig::GetAttrGroupLevelCfgByLevel(int attr_group_id, int str_level) const
{
	std::map<int, HuanShouAttrGroupCfg>::const_iterator it = m_attr_group_map.find(attr_group_id);
	if (it == m_attr_group_map.end() || str_level <= 0 || it->second.max_str_level <= 0)
	{
		return NULL;
	}

	int index = GetMin(str_level, it->second.max_str_level) - 1;

	return &it->second.level_list[index];
}

const HuanShouEggOpenCfg * HuanShouConfig::GetHuanShouEggOpenCfg(int item_id) const
{
	for (int i = 0; i < m_huan_shou_egg_count && i < ARRAY_ITEM_COUNT(m_huan_shou_egg_item_list); ++i)
	{
		if (item_id == m_huan_shou_egg_item_list[i].item_id)
		{
			return &m_huan_shou_egg_item_list[i];
		}
	}

	return NULL;
}

const HuanShouEggCfg * HuanShouConfig::GetRandHuanShouEggCfgByEgg(int egg_id) const
{
	if (egg_id < 0 || egg_id >= m_huan_shou_egg_count || egg_id >= ARRAY_ITEM_COUNT(m_huan_shou_egg_list))
	{
		return NULL;
	}

	int rate_num = RandomNum(MAX_HUAN_SHOU_EGG_RATE_NUM);
	for (int i = 0; i < (int)m_huan_shou_egg_list[egg_id].size(); ++i)
	{
		if (rate_num < m_huan_shou_egg_list[egg_id][i].rate)
		{
			return &m_huan_shou_egg_list[egg_id][i];
		}

		rate_num -= m_huan_shou_egg_list[egg_id][i].rate;
	}

	return NULL;
}

const HuanShouEggCfg * HuanShouConfig::GetRandRareHuanShouEggCfgByEgg(int egg_id) const
{	
	if (egg_id < 0 || egg_id >= m_huan_shou_egg_count || egg_id >= ARRAY_ITEM_COUNT(m_huan_shou_egg_list))
	{
		return NULL;
	}

	int rate_count = 0;
	std::vector<const HuanShouEggCfg *> rare_list;
	for (int i = 0; i < (int)m_huan_shou_egg_list[egg_id].size(); ++i)
	{
		if (!m_huan_shou_egg_list[egg_id][i].is_rare) continue;

		rate_count += m_huan_shou_egg_list[egg_id][i].rate;
		rare_list.push_back(&m_huan_shou_egg_list[egg_id][i]);
	}

	if (rare_list.empty())
	{
		return NULL;
	}

	int rate_num = RandomNum(rate_count);
	for (int i = 0; i < (int)rare_list.size(); ++i)
	{
		if (rate_num < rare_list[i]->rate)
		{
			return rare_list[i];
		}

		rate_num -= rare_list[i]->rate;
	}

	return NULL;
}

bool HuanShouConfig::IsCanAddNewSkill(int has_hole_count) const
{
	if (m_skill_hole_map.empty())
	{
		return false;
	}

	int rate_count = 0;
	for (std::map<int, int>::const_reverse_iterator it = m_skill_hole_map.rbegin(); it != m_skill_hole_map.rend(); ++it)
	{
		if (has_hole_count >= it->first)
		{
			rate_count = it->second;
			break;
		}

	}
	if (rate_count <= 0)
	{
		return false;
	}
	
	if (RandomNum(BATTLE_PERCENT_NUM) < rate_count)
	{
		return true;
	}

	return false;
}

const HuanShouPointUnlockCfg * HuanShouConfig::GetPointUnlockCfg(int point_index) const
{
	if (point_index < 0 || point_index >= m_point_count || point_index >= ARRAY_ITEM_COUNT(m_point_unlock_list))
	{
		return NULL;
	}

	return &m_point_unlock_list[point_index];
}

const HuanShouPotentialPointLevelCfg * HuanShouConfig::GetPointLevelCfg(int point_group_id, int point_index, int level) const
{
	if (point_group_id < 0 || point_group_id >= m_ppg_count || point_group_id >= ARRAY_ITEM_COUNT(m_ppg_list))
	{
		return NULL;
	}

	const HuanShouPotentialPointGroupCfg & group_cfg = m_ppg_list[point_group_id];
	if (point_index < 0 || point_index >= m_point_count || point_index >= ARRAY_ITEM_COUNT(group_cfg.point_list))
	{
		return NULL;
	}
	if (level <= 0 || level > group_cfg.max_level_list[point_index] || level > ARRAY_ITEM_COUNT(group_cfg.point_list[point_index]))
	{
		return NULL;
	}

	return &group_cfg.point_list[point_index][level - 1];
}

int HuanShouConfig::InitOtherCfg(TiXmlElement * RootElement, std::string & errormsg)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -999;
	}

	if (!GetSubNodeValue(data_element, "potential_item", m_other_cfg.potential_item_id) || NULL == ITEMPOOL->GetItem(m_other_cfg.potential_item_id))
	{
		errormsg = STRING_SPRINTF("potential_item[%d] error, not find item", m_other_cfg.potential_item_id);
		return -1;
	}

	if (!GetSubNodeValue(data_element, "exchange_price", m_other_cfg.exchange_price) || m_other_cfg.exchange_price <= 0)
	{
		errormsg = STRING_SPRINTF("exchange_price[%d] error, <= 0", m_other_cfg.exchange_price);
		return -2;
	}

	if (!GetSubNodeValue(data_element, "wait_time", m_other_cfg.wait_time) || m_other_cfg.wait_time <= 0)
	{
		errormsg = STRING_SPRINTF("wait_time[%d] error, <= 0", m_other_cfg.wait_time);
		return -3;
	}

	if (!GetSubNodeValue(data_element, "liandan_max", m_other_cfg.liandan_max) || m_other_cfg.liandan_max <= 0 || m_other_cfg.liandan_max > MAX_INT16)
	{
		errormsg = STRING_SPRINTF("liandan_max[%d] error, <= 0 or > %d", m_other_cfg.liandan_max, MAX_INT16);
		return -4;
	}

	if (!GetSubNodeValue(data_element, "lock_item", m_other_cfg.lock_item_id) || NULL == ITEMPOOL->GetItem(m_other_cfg.lock_item_id))
	{
		errormsg = STRING_SPRINTF("lock_item[%d] error, not find item", m_other_cfg.wait_time);
		return -40;
	}

	UNSTD_STATIC_CHECK(ARRAY_ITEM_COUNT(m_other_cfg.need_item_num_list) == ARRAY_ITEM_COUNT(m_other_cfg.need_skill_num_list));
	for (int i = 0; i < ARRAY_ITEM_COUNT(m_other_cfg.need_item_num_list); ++i)
	{
		std::string need_skill_str = STRING_SPRINTF("need_skill_num%d", i + 1);
		std::string need_item_str = STRING_SPRINTF("need_item_num%d", i + 1);

		if (!GetSubNodeValue(data_element, need_skill_str, m_other_cfg.need_skill_num_list[i]) || m_other_cfg.need_skill_num_list[i] <= 0)
		{
			errormsg = STRING_SPRINTF("%s[%d] error, <= 0", need_skill_str.c_str(), m_other_cfg.need_skill_num_list[i]);
			return -(50 + i);
		}
		if (!GetSubNodeValue(data_element, need_item_str, m_other_cfg.need_item_num_list[i]) || m_other_cfg.need_item_num_list[i] <= 0)
		{
			errormsg = STRING_SPRINTF("%s[%d] error, <= 0", need_item_str.c_str(), m_other_cfg.need_item_num_list[i]);
			return -(60 + i);
		}
	}

	return 0;
}

int HuanShouConfig::InitBaseCfg(TiXmlElement * RootElement, std::string & errormsg)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -999;
	}

	m_base_count = 0;
	while (NULL != data_element)
	{
		int huanshou_id = 0;
		if (!GetSubNodeValue(data_element, "huanshou_id", huanshou_id) || huanshou_id != m_base_count || huanshou_id >= ARRAY_ITEM_COUNT(m_base_cfg_list))
		{
			errormsg = STRING_SPRINTF("huanshou_id[%d] error, != %d or >= %d", huanshou_id, m_base_count, ARRAY_ITEM_COUNT(m_base_cfg_list));
			return -1;
		}

		HuanShouBaseCfg node_cfg;
		if (!GetSubNodeValue(data_element, "huanshou_quality", node_cfg.quality))
		{
			errormsg = STRING_SPRINTF("huanshou_id[%d] error, huanshou_quality[%d] not find data", huanshou_id, node_cfg.quality);
			return -2;
		}

		if (!GetSubNodeValue(data_element, "item_id", node_cfg.convert_item_id) || NULL == ITEMPOOL->GetItem(node_cfg.convert_item_id))
		{
			errormsg = STRING_SPRINTF("huanshou_id[%d] error, item_id[%d] not find item", huanshou_id, node_cfg.convert_item_id);
			return -3;
		}

		if (!GetSubNodeValue(data_element, "att_group", node_cfg.attr_group_id) || NULL == this->GetAttrGroupCfg(node_cfg.attr_group_id))
		{
			errormsg = STRING_SPRINTF("huanshou_id[%d] error, att_group[%d] not find attr_group", huanshou_id, node_cfg.attr_group_id);
			return -3;
		}

		if (!GetSubNodeValue(data_element, "skill_lattice", node_cfg.skill_lattice) || node_cfg.skill_lattice <= 0 || node_cfg.skill_lattice > MAX_HUAN_SHOU_SKILL_GRID_CUR_NUM)
		{
			errormsg = STRING_SPRINTF("huanshou_id[%d] error, skill_lattice[%d] <= 0 or > %d", huanshou_id, node_cfg.skill_lattice, MAX_HUAN_SHOU_SKILL_GRID_CUR_NUM);
			return -5;
		}

		if (!GetSubNodeValue(data_element, "potential_group", node_cfg.potential_group_id) || node_cfg.potential_group_id < 0 || 
			node_cfg.potential_group_id >= m_ppg_count)
		{
			errormsg = STRING_SPRINTF("huanshou_id[%d] error, potential_group[%d] < 0 or >= %d", huanshou_id, node_cfg.potential_group_id, m_ppg_count);
			return -6;
		}

		TiXmlElement * item_list_element = data_element->FirstChildElement("fenjie_item_list");
		if (NULL == item_list_element)
		{
			errormsg = STRING_SPRINTF("huanshou_id[%d] error, [fenjie_item_list] not find data", huanshou_id);
			return -10;
		}

		TiXmlElement * item_element = item_list_element->FirstChildElement("fenjie_item");
		if (NULL == item_element)
		{
			errormsg = STRING_SPRINTF("huanshou_id[%d] error, [fenjie_item] not find data", huanshou_id);
			return -11;
		}

		while (NULL != item_element)
		{
			ItemConfigData item;
			if (!item.ReadConfig(item_element))
			{
				return -100;
			}

			node_cfg.fenjie_item_list.push_back(item);
			item_element = item_element->NextSiblingElement();
		}

		if (m_decompose_map.end() != m_decompose_map.find(node_cfg.convert_item_id))
		{
			errormsg = STRING_SPRINTF("huanshou_id[%d] error, convert_item_id[%d] repeat", huanshou_id, node_cfg.convert_item_id);
			return -110;
		}

		m_base_cfg_list[m_base_count++] = node_cfg;
		m_decompose_map[node_cfg.convert_item_id] = node_cfg.fenjie_item_list;
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int HuanShouConfig::InitAttrGroupCfg(TiXmlElement * RootElement, std::string & errormsg)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -999;
	}

	while (NULL != data_element)
	{
		int attr_group_id = 0;
		if (!GetSubNodeValue(data_element, "group", attr_group_id) || attr_group_id < 0)
		{
			errormsg = STRING_SPRINTF("group[%d] error, < 0?", attr_group_id);
			return -1;
		}

		HuanShouAttrGroupCfg & node_cfg = m_attr_group_map[attr_group_id];
		int level = 0;
		if (!GetSubNodeValue(data_element, "level", level) || level != node_cfg.max_str_level + 1 || level > ARRAY_ITEM_COUNT(node_cfg.level_list))
		{
			errormsg = STRING_SPRINTF("group[%d] error, level[%d] != %d or > %d?", attr_group_id, level, node_cfg.max_str_level + 1, ARRAY_ITEM_COUNT(node_cfg.level_list));
			return -2;
		}
		HuanShouAttrGroupLevelCfg & level_cfg = node_cfg.level_list[node_cfg.max_str_level];
		node_cfg.max_str_level = level;

		if (!GetSubNodeValue(data_element, "att_num", level_cfg.attr_count) || level_cfg.attr_count < 0 || level_cfg.attr_count > ARRAY_ITEM_COUNT(level_cfg.attr_list))
		{
			errormsg = STRING_SPRINTF("group[%d] error, att_num[%d] < 0 or > %d?", attr_group_id, level_cfg.attr_count, ARRAY_ITEM_COUNT(level_cfg.attr_list));
			return -3;
		}

		for (int i = 0; i < level_cfg.attr_count && i < ARRAY_ITEM_COUNT(level_cfg.attr_list); ++i)
		{
			std::string type_str = STRING_SPRINTF("attr_type_%d", i + 1);
			std::string value_str = STRING_SPRINTF("attribute_value_%d", i + 1);

			if (!GetSubNodeValue(data_element, type_str, level_cfg.attr_list[i].attr_type) || level_cfg.attr_list[i].attr_type < BATTLE_ATTR_MIN || level_cfg.attr_list[i].attr_type >= BATTLE_ATTR_MAX) 
			{
				errormsg = STRING_SPRINTF("group[%d] error, %s[%d] < %d or >= %d?", attr_group_id, type_str.c_str(), level_cfg.attr_list[i].attr_type, 
					BATTLE_ATTR_MIN, BATTLE_ATTR_MAX);
				return -(100 + i);
			}
			if (!GetSubNodeValue(data_element, value_str, level_cfg.attr_list[i].attr_value) || level_cfg.attr_list[i].attr_value < 0) 
			{
				errormsg = STRING_SPRINTF("group[%d] error, %s[%d] < 0 ?", attr_group_id, value_str.c_str(), level_cfg.attr_list[i].attr_value);
				return -(150 + i);
			}
		}

		GetSubNodeValue(data_element, "skill_id", level_cfg.skill_id);
	
		if (level_cfg.skill_id > 0)
		{
			if (NULL == SkillPool::Instance().GetSkillCfg(level_cfg.skill_id) && NULL == SkillPool::Instance().GetPassiveSkillCfg(level_cfg.skill_id))
			{
				errormsg = STRING_SPRINTF("group[%d] error, skill_id[%d] not find skill?", attr_group_id, level_cfg.skill_id);
				return -4;
			}
			if (!GetSubNodeValue(data_element, "skill_level", level_cfg.skill_level) || level_cfg.skill_level <= 0 || level_cfg.skill_level > MAX_BATTLE_SKILL_LEVEL)
			{
				errormsg = STRING_SPRINTF("group[%d] error, skill_level[%d] <= 0 or > %d?", attr_group_id, level_cfg.skill_level, MAX_BATTLE_SKILL_LEVEL);
				return -5;
			}

			if (!GetSubNodeValue(data_element, "cd_start", level_cfg.cd_start) || level_cfg.cd_start < 0)
			{
				errormsg = STRING_SPRINTF("group[%d] error, cd_start[%d] < 0?", attr_group_id, level_cfg.cd_start);
				return -6;
			}
			if (!GetSubNodeValue(data_element, "cd", level_cfg.cd_perform) || level_cfg.cd_perform < 0)
			{
				errormsg = STRING_SPRINTF("group[%d] error, cd[%d] < 0?", attr_group_id, level_cfg.cd_perform);
				return -7;
			}
		}

		TiXmlElement * item_list_element = data_element->FirstChildElement("cost_item_list");
		if (NULL != item_list_element)
		{
			TiXmlElement * item_element = item_list_element->FirstChildElement("cost_item");
			if (NULL == item_element)
			{
				errormsg = STRING_SPRINTF("group[%d] error, [cost_item] not find data", attr_group_id);
				return -11;
			}

			std::map<int, int> consume_map;
			while (NULL != item_element)
			{
				ItemConfigData item;
				if (!item.ReadConfig(item_element))
				{
					return -100;
				}

				consume_map[item.item_id] += item.num;
				item_element = item_element->NextSiblingElement();
			}
			//读配置的时候就做堆叠处理
			if (consume_map.size() > 1)
			{	
				for (std::map<int, int>::const_iterator it = consume_map.begin(); it != consume_map.end(); ++it)
				{
					level_cfg.consume_list.push_back(ItemConfigData(it->first, true, it->second));
				}
			}
		}

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int HuanShouConfig::InitEggItemCfg(TiXmlElement * RootElement, std::string & errormsg)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -999;
	}

	m_huan_shou_egg_count = 0;
	std::set<int> item_list;
	while (NULL != data_element)
	{
		int egg_id = 0;
		if (!GetSubNodeValue(data_element, "egg_id", egg_id) || egg_id != m_huan_shou_egg_count || egg_id >= ARRAY_ITEM_COUNT(m_huan_shou_egg_item_list))
		{
			errormsg = STRING_SPRINTF("egg_id[%d] error, != %d or >= %d?", egg_id, m_huan_shou_egg_count, ARRAY_ITEM_COUNT(m_huan_shou_egg_item_list));
			return -1;
		}
		
		HuanShouEggOpenCfg node_cfg;
		node_cfg.egg_id = egg_id;
		if (!GetSubNodeValue(data_element, "item_id", node_cfg.item_id) || NULL == ITEMPOOL->GetItem(node_cfg.item_id))
		{
			errormsg = STRING_SPRINTF("egg_id[%d] error, item_id[%d] not find item?", egg_id, node_cfg.item_id);
			return -2;
		}

		if (item_list.end() != item_list.find(node_cfg.item_id))
		{
			errormsg = STRING_SPRINTF("egg_id[%d] error, item_id[%d] repeat?", egg_id, node_cfg.item_id);
			return -3;
		}

		if (!GetSubNodeValue(data_element, "rare_guarantee", node_cfg.rare_guarantee) || node_cfg.rare_guarantee < 0)
		{
			errormsg = STRING_SPRINTF("egg_id[%d] error, rare_guarantee[%d] < 0?", egg_id, node_cfg.rare_guarantee);
			return -4;
		}
		
		item_list.insert(node_cfg.item_id);
		m_huan_shou_egg_item_list[m_huan_shou_egg_count++] = node_cfg;
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int HuanShouConfig::InitEggCfg(TiXmlElement * RootElement, std::string & errormsg)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -999;
	}

	while (NULL != data_element)
	{
		int egg_id = 0;
		if (!GetSubNodeValue(data_element, "egg_id", egg_id) || egg_id < 0 || egg_id >= m_huan_shou_egg_count)
		{
			errormsg = STRING_SPRINTF("egg_id[%d] error, < 0 or >= %d?", egg_id, m_huan_shou_egg_count);
			return -1;
		}

		HuanShouEggCfg node_cfg;
		if (!GetSubNodeValue(data_element, "huanshou_id", node_cfg.huanshou_id) || NULL == this->GetBaseCfg(node_cfg.huanshou_id))
		{
			errormsg = STRING_SPRINTF("egg_id[%d] error, huanshou_id[%d] not find huanshou?", egg_id, node_cfg.huanshou_id);
			return -2;
		}

		if (!GetSubNodeValue(data_element, "rate", node_cfg.rate) || node_cfg.rate < 0)
		{
			errormsg = STRING_SPRINTF("egg_id[%d] huanshou_id[%d] error, rate[%d] < 0?", egg_id, node_cfg.huanshou_id, node_cfg.rate);
			return -3;
		}

		int is_broadcast = 0;
		GetSubNodeValue(data_element, "is_broadcast", is_broadcast);
		node_cfg.is_broadcast = (0 != is_broadcast);

		int is_rare = 0;
		GetSubNodeValue(data_element, "is_rare", is_rare);
		node_cfg.is_rare = (0 != is_rare);

		m_huan_shou_egg_list[egg_id].push_back(node_cfg);
		data_element = data_element->NextSiblingElement();
	}

	for (int i = 0; i < m_huan_shou_egg_count && i < ARRAY_ITEM_COUNT(m_huan_shou_egg_list); ++i)
	{
		if (m_huan_shou_egg_list[i].empty())
		{
			errormsg = STRING_SPRINTF("egg_id[%d] error, has empty??", i);
			return -100;
		}

		int rate_count = 0;
		for (int k = 0; k < (int)m_huan_shou_egg_list[i].size(); ++k)
		{
			rate_count += m_huan_shou_egg_list[i][k].rate;
		}

		if (rate_count != MAX_HUAN_SHOU_EGG_RATE_NUM)
		{
			errormsg = STRING_SPRINTF("egg_id[%d] error, rate_count[%d] not equal %d?", i, rate_count, MAX_HUAN_SHOU_EGG_RATE_NUM);
			return -101;
		}
	}

	return 0;
}

int HuanShouConfig::InitSkillHoleCfg(TiXmlElement * RootElement, std::string & errormsg)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -999;
	}

	while (NULL != data_element)
	{
		int hole_count = 0;
		if (!GetSubNodeValue(data_element, "hole", hole_count) || hole_count < 0 || hole_count > MAX_HUAN_SHOU_SKILL_GRID_CUR_NUM)
		{
			errormsg = STRING_SPRINTF("hole[%d] error, < 0 or > %d", hole_count, MAX_HUAN_SHOU_SKILL_GRID_CUR_NUM);
			return -1;
		}

		int unlock_new_hole_percent = 0;
		if (!GetSubNodeValue(data_element, "rate", unlock_new_hole_percent) || unlock_new_hole_percent < 0 || unlock_new_hole_percent > BATTLE_PERCENT_NUM)
		{
			errormsg = STRING_SPRINTF("hole[%d] error, rate[%d] < 0 or > %d", hole_count, unlock_new_hole_percent, BATTLE_PERCENT_NUM);
			return -2;
		}

		m_skill_hole_map[hole_count] = unlock_new_hole_percent;
		data_element = data_element->NextSiblingElement();
	}
	return 0;
}

int HuanShouConfig::InitPointUnlcokCfg(TiXmlElement * RootElement, std::string & errormsg)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -999;
	}

	m_point_count = 0;
	while (NULL != data_element)
	{
		int point_index = 0;
		if (!GetSubNodeValue(data_element, "potential_point", point_index) || point_index != m_point_count || point_index >= ARRAY_ITEM_COUNT(m_point_unlock_list))
		{
			errormsg = STRING_SPRINTF("potential_point[%d] error, != %d or >= %d", point_index, m_point_count, MAX_HUAN_SHOU_SKILL_GRID_CUR_NUM);
			return -1;
		}

		HuanShouPointUnlockCfg node_cfg;
		if (!GetSubNodeValue(data_element, "condition", node_cfg.need_active_level) || node_cfg.need_active_level < 0 || node_cfg.need_active_level > NAX_HUAN_SHOU_STR_LEVEL_NUM)
		{
			errormsg = STRING_SPRINTF("potential_point[%d] error, condition[%d] < 0 or > %d", point_index, node_cfg.need_active_level, NAX_HUAN_SHOU_STR_LEVEL_NUM);
			return -2;
		}

		TiXmlElement * adjoin_element = data_element->FirstChildElement("adjoin");
		if (NULL == adjoin_element)
		{
			errormsg = STRING_SPRINTF("potential_point[%d] error, [adjoin] not find", point_index);
			return -3;
		}

		TiXmlElement * node_element = adjoin_element->FirstChildElement("node");
		if (NULL == node_element)
		{
			errormsg = STRING_SPRINTF("potential_point[%d] error, [node] not find", point_index);
			return -4;
		}
		while (NULL != node_element)
		{
			int adjoin_point_index = 0;
			if (!GetNodeValue(node_element, adjoin_point_index) || adjoin_point_index == point_index || node_cfg.adjoin_list.end() != node_cfg.adjoin_list.find(adjoin_point_index))
			{
				errormsg = STRING_SPRINTF("potential_point[%d] error, adjoin[%d] equal potential_point[%d] or repeat?", point_index, adjoin_point_index, point_index);
				return -10;
			}

			if (adjoin_point_index < 0 || adjoin_point_index >= MAX_HUAN_SHOU_CUR_POTENTIAL_POINT_NUM)
			{
				return -11;
			}

			node_cfg.adjoin_list.insert(adjoin_point_index);
			node_element = node_element->NextSiblingElement();
		}

		m_point_unlock_list[m_point_count++] = node_cfg;
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int HuanShouConfig::InitPointGroupCfg(TiXmlElement * RootElement, std::string & errormsg)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -999;
	}

	m_ppg_count = 0;
	int last_point_index = 0;
	bool is_has = false;
	while (NULL != data_element)
	{
		int ppg_id = 0;
		if (!GetSubNodeValue(data_element, "group", ppg_id) || ppg_id < 0 || ppg_id >= ARRAY_ITEM_COUNT(m_ppg_list))
		{
			errormsg = STRING_SPRINTF("group[%d] error, < 0 or >= %d", ppg_id, ARRAY_ITEM_COUNT(m_ppg_list));
			return -1;
		}

		int point_index = 0;
		if (!GetSubNodeValue(data_element, "potential_point", point_index) || point_index < 0 || point_index >= m_point_count|| point_index >= MAX_HUAN_SHOU_POTENTIAL_POINT)
		{
			errormsg = STRING_SPRINTF("group[%d] error, potential_point[%d] < 0 or >= %d or >= %d", ppg_id, point_index, m_point_count, MAX_HUAN_SHOU_POTENTIAL_POINT);
			return -2;
		}

		int point_level = 0;
		if (!GetSubNodeValue(data_element, "qiannen_level", point_level) || point_level <= 0 || point_level > MAX_HUAN_SHOU_POINT_LEVEL_NUM)
		{
			errormsg = STRING_SPRINTF("group[%d] potential_point[%d] error, qiannen_level[%d] <= 0 or > %d", ppg_id, point_index, point_level, MAX_HUAN_SHOU_POINT_LEVEL_NUM);
			return -3;
		}

		HuanShouPotentialPointGroupCfg & group_cfg = m_ppg_list[ppg_id];
		if (m_ppg_count == ppg_id)
		{
			if (last_point_index == point_index)
			{
				if (point_level != group_cfg.max_level_list[point_index] + 1)
				{
					errormsg = STRING_SPRINTF("group[%d] potential_point[%d] error, qiannen_level[%d] != %d", ppg_id, point_index, point_level, group_cfg.max_level_list[point_index] + 1);
					return -4;
				}
			}
			else if (last_point_index + 1 == point_index)
			{
				if (1 != point_level)
				{
					errormsg = STRING_SPRINTF("group[%d] potential_point[%d] error, qiannen_level[%d] != 1", ppg_id, point_index, point_level);
					return -5;
				}
			}
			else
			{
				errormsg = STRING_SPRINTF("group[%d] potential_point[%d] qiannen_level[%d] error, potential_point[%d] must in [%d, %d]", ppg_id, point_index, point_level,
					last_point_index, last_point_index + 1);
				return -6;
			}
		}
		else if (m_ppg_count + 1 == ppg_id)
		{
			if (last_point_index + 1 != m_point_count && 0 != point_index)
			{
				errormsg = STRING_SPRINTF("group[%d] potential_point[%d] qiannen_level[%d] error, potential_point != %d or != 0", ppg_id, point_index, point_level, m_point_count - 1);
				return -7;
			}
			else if (1 != point_level)
			{
				errormsg = STRING_SPRINTF("group[%d] potential_point[%d] error, qiannen_level[%d] != 1", ppg_id, point_index, point_level);
				return -8;
			}

			m_ppg_count = ppg_id;
		}
		else
		{
			errormsg = STRING_SPRINTF("group[%d] potential_point[%d] error, group must in[%d, %d]", ppg_id, point_index, point_level, m_ppg_count, m_ppg_count + 1);
			return -9;
		}
		last_point_index = point_index;
		group_cfg.max_level_list[point_index] = point_level;

		HuanShouPotentialPointLevelCfg & level_cfg = group_cfg.point_list[point_index][point_level - 1];
		level_cfg.level = point_level;

		TiXmlElement * item_list_element = data_element->FirstChildElement("cost_item_list");
		if (NULL == item_list_element)
		{
			errormsg = STRING_SPRINTF("group[%d] potential_point[%d] qiannen_level[%d] error, [cost_item_list] not find data", ppg_id, point_index, point_level);
			return -100;
		}

		TiXmlElement * item_element = item_list_element->FirstChildElement("cost_item");
		if (NULL == item_element)
		{
			errormsg = STRING_SPRINTF("group[%d] potential_point[%d] qiannen_level[%d] error, [cost_item] not find data", ppg_id, point_index, point_level);
			return -101;
		}

		while (NULL != item_element)
		{
			ItemConfigData item;
			if (!item.ReadConfig(item_element))
			{
				return -110;
			}

			level_cfg.consume_list.push_back(item);
			item_element = item_element->NextSiblingElement();
		}

		GetSubNodeValue(data_element, "tianfu_type", level_cfg.tianfu_type);
		GetSubNodeValue(data_element, "param1", level_cfg.param1);
		GetSubNodeValue(data_element, "param2", level_cfg.param2);
		switch (level_cfg.tianfu_type)
		{
			case HUAN_SHOU_POINT_TIAN_FU_TYPE_ATTR:
			{
				if (level_cfg.param1 < BATTLE_ATTR_MIN || level_cfg.param1 >= BATTLE_ATTR_MAX || level_cfg.param2 <= 0)
				{
					errormsg = STRING_SPRINTF("group[%d] potential_point[%d] qiannen_level[%d] error, param1[%d] < 0 or >=%d or param2[%d] <= 0", ppg_id, point_index, point_level,
						level_cfg.param1, BATTLE_ATTR_MAX, level_cfg.param2);
					return -10;
				}
			}
			break;
			case HUAN_SHOU_POINT_TIAN_FU_TYPE_PASSIVE_SKILL:
			{
				if (NULL == SkillPool::Instance().GetPassiveSkillCfg(level_cfg.param1) || level_cfg.param2 <= 0)
				{
					errormsg = STRING_SPRINTF("group[%d] potential_point[%d] qiannen_level[%d] error, param1[%d] not find skill or param2[%d] <= 0", ppg_id, point_index, point_level,
						level_cfg.param1, level_cfg.param2);
					return -11;
				}
			}
			break;
			default:
			{
				errormsg = STRING_SPRINTF("group[%d] potential_point[%d] qiannen_level[%d] error, tianfu_type[%d] < 0 or >= %d", ppg_id, point_index, point_level,
					level_cfg.tianfu_type, HUAN_SHOU_POINT_TIAN_FU_TYPE_MAX);
				return -120;
			}
			break;
		}

		is_has = true;
		data_element = data_element->NextSiblingElement();
	}
	if (is_has)
	{
		m_ppg_count += 1;
	}


	return 0;
}
