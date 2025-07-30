#include "item/itempool.h"
#include "item/itembase.h"
#include "equipment/equipment.h"
#include "equipment/jewelry.h"
#include "zhulingconfig.hpp"
#include <set>
#include <functional>

ZhuLingConfig::ZhuLingConfig()
{
}

ZhuLingConfig ::~ZhuLingConfig()
{
	std::vector<ZhuLingCombinationCfg>().swap(m_combination_cfg);
}

bool ZhuLingConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;
	LOAD_CONFIG("spirit_stone", InitZhuLingCfg);
	LOAD_CONFIG("spirit_stone_resolve", InitDecomposeCfg);
	LOAD_CONFIG("resolve_item_group", InitDecomposeRandRewardCfg);
	LOAD_CONFIG("resonance_attr", InitZhuLingCombinationAttrCfg);
	LOAD_CONFIG("resonance", InitZhuLingCombinationCfg);
	LOAD_CONFIG("limit_level", InitZhuLingSlotCfg);
	LOAD_CONFIG("wear_restrictions", InitZhuLingPutOnCfg);

	return true;
}

int ZhuLingConfig::GetZhuLingUpLevelBaseItemId(int type)
{
	const ZhuLingCfg * cfg = this->GetZhuLingCfgByTypeAndLevel(type, 0);
	if (cfg)
	{
		return cfg->item_id;
	}

	return 0;
}

const ZhuLingCfg * ZhuLingConfig::GetZhuLingCfgByTypeAndLevel(int type, int level)
{
	ZhuLingCfgTypeAndLevelKey key;
	key.type = type;
	key.zhu_ling_level = level;
	std::map<ZhuLingCfgTypeAndLevelKey, ZhuLingCfg * >::iterator iter = m_zhu_ling_type_and_level_map_cfg.find(key);
	if (iter != m_zhu_ling_type_and_level_map_cfg.end())
	{
		return iter->second;
	}

	return NULL;
}

const ZhuLingCfg * ZhuLingConfig::GetZhuLingCfg(int item_id)
{
	std::map<int, ZhuLingCfg>  ::iterator iter = m_zhu_ling_cfg.find(item_id);
	if (iter == m_zhu_ling_cfg.end())
	{
		return NULL;
	}

	return &iter->second;
}

const ZhuLingDecomposeCfg * ZhuLingConfig::GetZhuLingDecomposeCfg(ZhuLingDecomposeCfgKey key)
{
	return MapValuesConstPtr(m_decompose_cfg, key);
}

const ZhuLingDecomposeRandRewardCfg::ItemCfg * ZhuLingConfig::GetDecomposeRandRewardCfg(int group_id)
{
	const ZhuLingDecomposeRandRewardCfg * cfg = MapValuesConstPtr(m_decompose_rand_reward_cfg, group_id);
	if (cfg == NULL)
	{
		return NULL;
	}

	int r = RandomNum(cfg->rate_count);

	for (int i = 0; i < (int)cfg->group_item_cfg.size(); ++i)
	{
		if (r < cfg->group_item_cfg[i].rate)
		{
			return &cfg->group_item_cfg[i];
		}

		r -= cfg->rate_count;
	}

	return NULL;
}

int ZhuLingConfig::GetSlotNeedLevel(int slot_index)
{
	int level = MapValue(m_zhu_ling_slot_cfg, slot_index);
	return level > 0 ? level : INT_MAX;
}

const ZhuLingOtherCfg & ZhuLingConfig::GetOtherCfg() const
{
	return m_other_cfg;
}

bool ZhuLingConfig::CanPutOn(int equip_item_id, int zhu_ling_id)
{
	if (zhu_ling_id == 0)
	{
		return true;
	}


	const ItemBase * item_info = ITEMPOOL->GetItem(equip_item_id);
	if (item_info == NULL)
	{
		return false;
	}

	const ZhuLingCfg * cfg = this->GetZhuLingCfg(zhu_ling_id);
	if (cfg == NULL)
	{
		return false;
	}

	int equip_type = 0;
	switch (item_info->GetItemType())
	{
		case I_TYPE_EQUIPMENT:
			{
				const Equipment * equip = (const Equipment *)item_info;
				equip_type = equip->GetEquipIndex();

			}
			break;
		case I_TYPE_JEWELRY:
			{
				const Jewelry * equip = (const Jewelry *)item_info;
				 equip_type = equip->GetJewelryType() + 200;

			}
			break;
		default:
			return false;
	}

	std::map<int, std::set<int> >::iterator iter = m_zhu_ling_put_on_cfg.find(equip_type);
	if (iter == m_zhu_ling_put_on_cfg.end())
	{
		return false;
	}

	std::set<int>::iterator iter2 = iter->second.find(cfg->spirit_stone_type);
	if (iter2 == iter->second.end())
	{
		return false;
	}

	return true;
}

const ZhuLingCombinationAttrCfg * ZhuLingConfig::GetZhuLingCombinationAttrCfg(const ZhuLingCombinationAttrCfgKey & key)
{
	return MapValuesConstPtr(m_combination_attr_cfg, key);
}

const ZhuLingCombinationCfg * ZhuLingConfig::GetZhuLingCombinationCfg(const std::map<int, int> & m, std::map<int, std::vector<int> > & zhu_ling_level_info, int ARG_OUT & combination_zhu_ling_level)
{
	for (std::map<int, std::vector<int> >::iterator iter = zhu_ling_level_info.begin(); iter != zhu_ling_level_info.end(); ++iter)
	{
		std::vector<int> & vec = iter->second;
		std::sort(vec.begin(), vec.end(), std::greater<int>());
	}

	const ZhuLingCombinationCfg * ret = NULL;
	for (std::vector<ZhuLingCombinationCfg>::reverse_iterator iter = m_combination_cfg.rbegin(); iter != m_combination_cfg.rend(); ++iter)
	{
		combination_zhu_ling_level = INT_MAX;
		const ZhuLingCombinationCfg & curr = *iter;

		bool b = true;
		for (std::map<int, int>::const_iterator iter2 = curr.zhu_ling_color_need_num.begin(); iter2 != curr.zhu_ling_color_need_num.end(); ++iter2)
		{
			int color = iter2->first;
			int num = iter2->second;
			const std::map<int, int> ::const_iterator tmp_iter = m.find(color);
			if (tmp_iter == m.end() || tmp_iter->second < num)
			{
				b = false;
				break;
			}

			std::map<int, std::vector<int> >::const_iterator level_iter = zhu_ling_level_info.find(color);
			if (level_iter != zhu_ling_level_info.end())
			{
				int index = num - 1;

				const std::vector<int> & vec = level_iter->second;
				if (index < 0 || index >= (int)vec.size())
				{
					combination_zhu_ling_level = 0;
				}
				else
				{
					combination_zhu_ling_level = std::min(combination_zhu_ling_level, vec[index]);
				}
			}
		}

		if (b)
		{
			ret = &curr;
			break;
		}
		else
		{
			combination_zhu_ling_level = 0;
		}
	}

	return ret;
}

int ZhuLingConfig::InitZhuLingCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}

	std::map<int, int> next_level_map;
	while (NULL != data_element)
	{
		ZhuLingCfg cfg;

		if (!GetSubNodeValue(data_element, "type", cfg.type))
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(data_element, "spirit_stone_id", cfg.item_id))
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(data_element, "spirit_stone_type", cfg.spirit_stone_type))
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(data_element, "spirit_stone_level", cfg.zhu_ling_level) || next_level_map[cfg.type] != cfg.zhu_ling_level)
		{
			return -__LINE__;
		}
		next_level_map[cfg.type] += 1;

		if (!GetSubNodeValue(data_element, "need_coin", cfg.need_coin))
		{
			return -__LINE__;
		}


		if (!GetSubNodeValue(data_element, "need_num", cfg.need_item_num))
		{
			return -__LINE__;
		}

		for (int i = 0; i < 4; ++i)
		{
			std::string type=STRING_SPRINTF("type_%d", i + 1);
			std::string value = STRING_SPRINTF("value_%d", i + 1);

			ZhuLingCfg::AttrItem attr;
			if (!GetSubNodeValue(data_element, type, attr.attr_type))
			{
				return -__LINE__;
			}

			if (!GetSubNodeValue(data_element, value, attr.attr_value))
			{
				return -__LINE__;
			}

			if (attr.attr_value == 0)
			{
				continue;
			}

			cfg.attr_list.push_back(attr);
		}

		m_zhu_ling_cfg[cfg.item_id] = cfg;
		data_element = data_element->NextSiblingElement();
	}
	
	for (std::map<int, ZhuLingCfg>::iterator iter = m_zhu_ling_cfg.begin(); iter != m_zhu_ling_cfg.end(); ++iter)
	{
		ZhuLingCfg & curr = iter->second;
		ZhuLingCfgTypeAndLevelKey key;
		key.type = curr.type;
		key.zhu_ling_level = curr.zhu_ling_level;

		if (m_zhu_ling_type_and_level_map_cfg.find(key) != m_zhu_ling_type_and_level_map_cfg.end())
		{
			return -__LINE__;
		}

		m_zhu_ling_type_and_level_map_cfg[key] = &curr;
	}

	for (std::map<int, int>::iterator iter = next_level_map.begin(); iter != next_level_map.end(); ++iter)
	{
		int type = iter->first;
		int next_level = iter->second;

		for (int level = 0; level < next_level; ++level)
		{
			ZhuLingCfgTypeAndLevelKey key;
			key.type = type;
			key.zhu_ling_level = level;

			std::map<ZhuLingCfgTypeAndLevelKey, ZhuLingCfg * > ::iterator cfg_iter = m_zhu_ling_type_and_level_map_cfg.find(key);
			if (cfg_iter == m_zhu_ling_type_and_level_map_cfg.end())
			{
				return -__LINE__;
			}

			ZhuLingCfg * cfg = cfg_iter->second;
			if (cfg == NULL)
			{
				return -__LINE__;
			}

			if (level == 0)
			{
				cfg->need_base_item_num = 1;
				continue;
			}

			ZhuLingCfgTypeAndLevelKey key2;
			key2.type = type;
			key2.zhu_ling_level = level - 1;
			std::map<ZhuLingCfgTypeAndLevelKey, ZhuLingCfg * > ::iterator cfg_iter2 = m_zhu_ling_type_and_level_map_cfg.find(key2);
			if (cfg_iter2 == m_zhu_ling_type_and_level_map_cfg.end())
			{
				return -__LINE__;
			}

			ZhuLingCfg * last_level_cfg = cfg_iter2->second;
			if (last_level_cfg == NULL)
			{
				return -__LINE__;
			}

			cfg->need_base_item_num = last_level_cfg->need_base_item_num * last_level_cfg->need_item_num;

			if (cfg->need_base_item_num <= 0)
			{
				return -__LINE__;
			}
		}
	}


	return 0;
}

int ZhuLingConfig::InitDecomposeCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}

	while (NULL != data_element)
	{		
		ZhuLingDecomposeCfg cfg;
		if (!GetSubNodeValue(data_element, "spirit_stone_type", cfg.spirit_stone_type))
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(data_element, "color", cfg.color))
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(data_element, "level", cfg.zhu_ling_level))
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(data_element, "resolve_num", cfg.get_fen_chen_num))
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(data_element, "special_resolve_rate", cfg.get_item_reward_rate))
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(data_element, "resolve_item_group", cfg.get_item_reward_group_id))
		{
			return -__LINE__;
		}

		ZhuLingDecomposeCfgKey key;
		key.spirit_stone_type = cfg.spirit_stone_type;
		key.color = cfg.color;
		key.zhu_ling_level = cfg.zhu_ling_level;

		m_decompose_cfg[key] = cfg;
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int ZhuLingConfig::InitDecomposeRandRewardCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}

	while (NULL != data_element)
	{		
		int group_id = 0;
		if (!GetSubNodeValue(data_element, "resolve_item_group", group_id))
		{
			return -__LINE__;
		}

		ZhuLingDecomposeRandRewardCfg & cfg=m_decompose_rand_reward_cfg[group_id];
		ZhuLingDecomposeRandRewardCfg::ItemCfg item_cfg;
		if (!GetSubNodeValue(data_element, "rate", item_cfg.rate) || item_cfg.rate <= 0)
		{
			return -__LINE__;
		}

		TiXmlElement * item_element = data_element->FirstChildElement("resolve_item");
		if (NULL != item_element)
		{
			if (!item_cfg.reward.ReadConfig(item_element)) 
			{
				return -__LINE__;
			}
		}

		cfg.rate_count += item_cfg.rate;
		cfg.group_item_cfg.push_back(item_cfg);
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int ZhuLingConfig::InitZhuLingCombinationAttrCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}

	while (NULL != data_element)
	{
		ZhuLingCombinationAttrCfg cfg;

		for (int i = 0; i < 4; ++i)
		{
			ZhuLingCombinationAttrCfg::AttrItemCfg attr_cfg;

			std::string type = STRING_SPRINTF("type_%d", i + 1);
			std::string value = STRING_SPRINTF("value_%d", i + 1);

			if (!GetSubNodeValue(data_element, type, attr_cfg.type))
			{
				return -__LINE__;
			}

			if (!GetSubNodeValue(data_element, value, attr_cfg.value))
			{
				return -__LINE__;
			}

			if (attr_cfg.value == 0)
			{
				continue;
			}

			cfg.attr_cfg.push_back(attr_cfg);
		}

		for (int i = 0; i < 5; ++i)
		{
			ZhuLingCombinationAttrCfg::AttrItemCfg attr_cfg;

			std::string type = STRING_SPRINTF("extra_type_%d", i + 1);
			std::string value = STRING_SPRINTF("extra_value_%d", i + 1);

			if (!GetSubNodeValue(data_element, value, attr_cfg.value))
			{
				return -__LINE__;
			}

			if (attr_cfg.value == 0)
			{
				continue;
			}

			{
				TiXmlElement * node_list_ele = data_element->FirstChildElement(type.c_str());
				if (NULL == node_list_ele)
				{
					return -__LINE__;
				}

				TiXmlElement * node_ele = node_list_ele->FirstChildElement("node");
				while (NULL != node_ele)
				{
					int t = 0;
					if (!GetNodeValue(node_ele, t))
					{
						return -__LINE__;
					}
					attr_cfg.type = t;

					cfg.attr_percentage_cfg.push_back(attr_cfg);
					node_ele = node_ele->NextSiblingElement();
				}
			}			
		}


		if (!GetSubNodeValue(data_element, "resonance_group_id", cfg.group_id))
		{
			return -__LINE__;
		}


		if (!GetSubNodeValue(data_element, "resonance_level", cfg.combination_level))
		{
			return -__LINE__;
		}

		std::set<int> base_prof_set;
		{
			TiXmlElement * node_list_ele = data_element->FirstChildElement("job_type");
			if (NULL == node_list_ele)
			{
				return -__LINE__;
			}

			TiXmlElement * node_ele = node_list_ele->FirstChildElement("node");
			while (NULL != node_ele)
			{
				int t = 0;
				if (!GetNodeValue(node_ele, t))
				{
					return -__LINE__;
				}
				base_prof_set.insert(t);
				node_ele = node_ele->NextSiblingElement();
			}
		}

		std::set<int> equip_type_set;
		{
			TiXmlElement * node_list_ele = data_element->FirstChildElement("equip_type");
			if (NULL == node_list_ele)
			{
				return -__LINE__;
			}

			TiXmlElement * node_ele = node_list_ele->FirstChildElement("node");
			while (NULL != node_ele)
			{
				int t = 0;
				if (!GetNodeValue(node_ele, t))
				{
					return -__LINE__;
				}
				equip_type_set.insert(t);
				node_ele = node_ele->NextSiblingElement();
			}
		}

		for (std::set<int>::iterator iter1 = base_prof_set.begin(); iter1 != base_prof_set.end(); ++iter1)
		{
			int prof = *iter1;
			for (std::set<int>::iterator iter2 = equip_type_set.begin(); iter2 != equip_type_set.end(); ++iter2)
			{
				int equip_type = *iter2;

				cfg.base_prof = prof;
				cfg.equip_type = equip_type;

				ZhuLingCombinationAttrCfgKey key;
				key.group_id = cfg.group_id;
				key.combination_level = cfg.combination_level;
				key.base_prof = cfg.base_prof;
				key.equip_type = cfg.equip_type;

				m_combination_attr_cfg[key] = cfg;
			}
		}

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int ZhuLingConfig::InitZhuLingCombinationCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}

	while (NULL != data_element)
	{
		ZhuLingCombinationCfg cfg;

		if (!GetSubNodeValue(data_element, "resonance_group_id", cfg.attr_group_id))
		{
			return -__LINE__;
		}

		for (int i = 0; i < EQUIP_ZHU_LING_SLOT_NUM; ++i)
		{
			std::string color_str = STRING_SPRINTF("color%d", i + 1);
			int color = 0;
			if (!GetSubNodeValue(data_element, color_str, color))
			{
				return -__LINE__;
			}

			if (color <= 0)
			{
				continue;
			}

			cfg.zhu_ling_color_need_num[color] += 1;
		}

		m_combination_cfg.push_back(cfg);

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int ZhuLingConfig::InitZhuLingSlotCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}

	while (NULL != data_element)
	{
		int slot_index = 0;
		if (!GetSubNodeValue(data_element, "solt_id", slot_index))
		{
			return -__LINE__;
		}

		int role_level = 0;
		if (!GetSubNodeValue(data_element, "limit_level", role_level))
		{
			return -__LINE__;
		}
		m_zhu_ling_slot_cfg[slot_index] = role_level;
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int ZhuLingConfig::InitZhuLingPutOnCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}

	while (NULL != data_element)
	{
		int equip_type = 0;
		if (!GetSubNodeValue(data_element, "equip_type", equip_type))
		{
			return -__LINE__;
		}

		std::set<int> & s = m_zhu_ling_put_on_cfg[equip_type];
		{
			TiXmlElement * node_list_ele = data_element->FirstChildElement("spirit_stone_type");
			if (NULL == node_list_ele)
			{
				return -__LINE__;
			}

			TiXmlElement * node_ele = node_list_ele->FirstChildElement("node");
			while (NULL != node_ele)
			{
				int t = 0;
				if (!GetNodeValue(node_ele, t))
				{
					return -__LINE__;
				}
				s.insert(t);
				node_ele = node_ele->NextSiblingElement();
			}
		}

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

ZhuLingCfg::ZhuLingCfg()
{
	type = 0;
	item_id = 0;
	zhu_ling_level = 0;
	need_coin = 0;
	need_item_num = 0;
	spirit_stone_type = 0;
	need_base_item_num = 0;
}

ZhuLingCfg::AttrItem::AttrItem()
{
	attr_type = 0;
	attr_value = 0;
}
