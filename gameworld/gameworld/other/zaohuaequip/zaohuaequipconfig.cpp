#include "zaohuaequipconfig.hpp"
#include "item/itempool.h"
#include "servercommon/commondata.hpp"
#include <functional>

ZaoHuaEquipConfig::ZaoHuaEquipConfig()
{
}

ZaoHuaEquipConfig::~ZaoHuaEquipConfig()
{
	std::vector<YuanHunZhuCombinationCfg>().swap(m_yuan_hun_zhu_combination_cfg);
	for (std::map<ZaoHuaEquipHunLiAttrCfgKey, std::vector<ZaoHuaEquipHunLiAttrCfg > >::iterator it = m_hun_li_attr_cfg.begin(); it != m_hun_li_attr_cfg.end(); it++)
	{
		std::vector<ZaoHuaEquipHunLiAttrCfg >().swap(it->second);
	}
	for (std::map<int, std::vector<ZaoHuaEquipRefineResonanceCfg> >::iterator it = m_refine_resonance_cfg.begin(); it != m_refine_resonance_cfg.end(); it++)
	{
		std::vector<ZaoHuaEquipRefineResonanceCfg>().swap(it->second);
	}
	std::vector<ZaoHuaChangeJobCfg>().swap(m_change_job_vec);
}

bool ZaoHuaEquipConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG("equip_suit", InitEquipSuitCfg);
	LOAD_CONFIG("equip_make", InitEquipComposeCfg);
	LOAD_CONFIG("other", InitOhterCfg);
	LOAD_CONFIG("vigorous", InitVigorousCfg);
	LOAD_CONFIG("refine", InitZaoHuaEquipRefineCfg);
	LOAD_CONFIG("refine_resonante", InitZaoHuaEquipRefineResonanceCfg);

	LOAD_CONFIG("blood_sacrifice", InitZaoHuaEquipHunLiAttrCfg);
	LOAD_CONFIG("intelligence", InitZaoHuaEquipLingXingValueCfg);
	LOAD_CONFIG("yuan_soul_pearl", InitYuanHunZhuCfg);
	LOAD_CONFIG("yuan_soul_quality", InitYuanHunZhuAttrGroupCfg);
	LOAD_CONFIG("yuan_soul_resonate", InitYuanHunZhuCombinationCfg);
	LOAD_CONFIG("decompose", InitYuanHunZhuDecomposeCfg);
	LOAD_CONFIG("yuan_soul_compose", InitYuanHunZhuComposeCfg);
	LOAD_CONFIG("resonate_add", InitYuanHunZhuCombinationAttrCfg);	
	LOAD_CONFIG("cast", InitZaoHuaLongZhuCfg);
	LOAD_CONFIG("affix_group",InitZaoHuaLongZhuCiTiaoCfg);
	LOAD_CONFIG("affix_quality", InitZaoHuaLongZhuDrawCfg);
	LOAD_CONFIG("enchant", InitZaoHuaRuMoCfg);
	LOAD_CONFIG("change_job", InitChangeJobCfg);
	
	return true;
}

const ZaoHuaSuitCfg * ZaoHuaEquipConfig::GetSuitCfg(int suit_id, int color, int level, int suit_num)
{
	long long key = this->GetKeyByLevelAndSuitAndColor(suit_id, color, level, suit_num);
	std::map<long long, ZaoHuaSuitCfg>::iterator it = m_suit_map.find(key);
	if (it != m_suit_map.end())
	{
		return &it->second;
	}
	return NULL;
}

const ZaoHuaComposeCfg * ZaoHuaEquipConfig::GetComposeCfg(int compose_seq)
{
	std::map<int, ZaoHuaComposeCfg>::iterator it = m_compose_map.find(compose_seq);
	if (it != m_compose_map.end())
	{
		return &it->second;
	}
	return NULL;
}

const ZaoHuaVigorousCfg * ZaoHuaEquipConfig::GetZaoHuaVigorousCfg(int seq)
{
	std::map<int, ZaoHuaVigorousCfg>::iterator it = m_vigorous_map.find(seq);
	if (it != m_vigorous_map.end())
	{
		return &it->second;
	}
	return NULL;
}

const ZaoHuaEquipOtherCfg & ZaoHuaEquipConfig::GetOtherCfg()
{
	return m_other_cfg;
}

const ZaoHuaEquipHunLiAttrCfg * ZaoHuaEquipConfig::GetZaoHuaEquipHunLiAttrCfg(int base_prof, int zao_hua_equip_index, int hun_li_value)
{
	ZaoHuaEquipHunLiAttrCfgKey key;
	key.base_prof = base_prof;
	key.zao_hua_equip_index = zao_hua_equip_index;

	const std::vector<ZaoHuaEquipHunLiAttrCfg > * cfg_ptr = MapValuesConstPtr(m_hun_li_attr_cfg, key);
	if (!cfg_ptr)
	{
		return NULL;
	}

	const std::vector<ZaoHuaEquipHunLiAttrCfg > & cfg = *cfg_ptr;

	for (std::vector<ZaoHuaEquipHunLiAttrCfg >::const_reverse_iterator iter = cfg.rbegin(); iter != cfg.rend(); ++iter)
	{
		const ZaoHuaEquipHunLiAttrCfg & curr = *iter;
		if (hun_li_value >= curr.hun_li_value)
		{
			return &curr;
		}
	}

	return NULL;
}

int ZaoHuaEquipConfig::GetRandLingXingValue()
{
	int r = RandomNum(m_ling_xing_cfg.rate_count);

	for (int i = 0; i < (int)m_ling_xing_cfg.item_list.size(); ++i)
	{
		const ZaoHuaEquipLingXingValueItemCfg & curr = m_ling_xing_cfg.item_list[i];
		if (r < curr.rate)
		{
			return curr.ling_xing_value;
		}

		r -= curr.rate;
	}


	return 0;
}


const YuanHunZhuCfg * ZaoHuaEquipConfig::YuanHunZhuGetCfg(int item_id)
{
	return MapValuesConstPtr(m_yuan_hun_zhu_cfg,item_id);
}

const YuanHunZhuAttrGroupCfg * ZaoHuaEquipConfig::YuanHunZhuGetAttrGroupCfg(int group_id)
{
	return MapValuesConstPtr(m_yuan_hun_zhu_attr_group_cfg, group_id);
}

const YuanHunZhuAttrGroupItemCfg * ZaoHuaEquipConfig::YuanHunZhuGetAttrGroupItemCfg(int group_id, int seq)
{
	const YuanHunZhuAttrGroupCfg * cfg = this->YuanHunZhuGetAttrGroupCfg(group_id);
	if (!cfg)
	{
		return NULL;
	}

	return  MapValuesConstPtr(cfg->item_list, seq);
}

const YuanHunZhuAttrGroupItemCfg * ZaoHuaEquipConfig::YuanHunZhuGetRandAttrGroupItemCfg(int group_id)
{
	const YuanHunZhuAttrGroupCfg * cfg = this->YuanHunZhuGetAttrGroupCfg(group_id);
	if (!cfg)
	{
		return NULL;
	}

	int r = RandomNum(cfg->rate_count);

	for (std::map<int, YuanHunZhuAttrGroupItemCfg>::const_iterator iter = cfg->item_list.begin(); iter != cfg->item_list.end(); ++iter)
	{
		const YuanHunZhuAttrGroupItemCfg & item_cfg = iter->second;
		if (r < item_cfg.rate)
		{
			return &item_cfg;
		}

		r -= item_cfg.rate;
	}

	return NULL;
}

const YuanHunZhuCombinationAttrCfg * ZaoHuaEquipConfig::YuanHunZhuGetCombinationAttrCfg(int base_prof, int combination_id, int combination_level)
{
	YuanHunZhuCombinationAttrCfgKey key;
	key.base_prof = base_prof;
	key.combination_id = combination_id;
	key.combination_level = combination_level;
	return MapValuesConstPtr(m_yuan_hun_zhu_combination_attr_cfg, key);
}

const YuanHunZhuCombinationAttrCfg * ZaoHuaEquipConfig::YuanHunZhuGetCombinationAttrCfg2(int base_prof, std::map<int, std::vector<int> > & yuan_hun_zhu_level_info)
{
	std::map<int, int> color_to_num;
	for (std::map<int, std::vector<int> >::iterator iter = yuan_hun_zhu_level_info.begin(); iter != yuan_hun_zhu_level_info.end(); ++iter)
	{
		std::vector<int> & vec = iter->second;
		std::sort(vec.begin(), vec.end(), std::greater<int>());
		int color = iter->first;
		int num = (int)vec.size();

		color_to_num[color] = num;
	}

	int combination_yuan_hun_zhu_level = 0;
	const YuanHunZhuCombinationCfg * cfg = NULL;
	for (std::vector<YuanHunZhuCombinationCfg>::reverse_iterator iter = m_yuan_hun_zhu_combination_cfg.rbegin(); iter != m_yuan_hun_zhu_combination_cfg.rend(); ++iter)
	{
		const YuanHunZhuCombinationCfg & curr = *iter;

		bool has_combination = true;
		int min_yuan_hun_zhu_level = INT_MAX;
		for (std::map<int, int>::const_iterator iter2 = curr.yuan_hun_zhu_color_need_num.begin(); iter2 != curr.yuan_hun_zhu_color_need_num.end(); ++iter2)
		{
			int color = iter2->first;
			int num = iter2->second;
			const std::map<int, int> ::const_iterator tmp_iter = color_to_num.find(color);
			if (tmp_iter == color_to_num.end() || tmp_iter->second < num)
			{
				has_combination = false;
				break;
			}

			std::map<int, std::vector<int> >::const_iterator level_iter = yuan_hun_zhu_level_info.find(color);
			if (level_iter != yuan_hun_zhu_level_info.end())
			{
				int index = num - 1;

				const std::vector<int> & vec = level_iter->second;
				if (index < 0 || index >= (int)vec.size())
				{
					has_combination = false;
					break;
				}
				else
				{
					min_yuan_hun_zhu_level = std::min(min_yuan_hun_zhu_level, vec[index]);
					break;
				}
			}
		}

		if (has_combination)
		{
			cfg = &curr;
			combination_yuan_hun_zhu_level = min_yuan_hun_zhu_level;
			break;
		}
	}

	if (!cfg)
	{
		return NULL;
	}

	return this->YuanHunZhuGetCombinationAttrCfg(base_prof, cfg->combination_group_id, combination_yuan_hun_zhu_level);
}

const YuanHunZhuDecomposeCfg * ZaoHuaEquipConfig::YuanHunZhuGetDecomposeCfg(int item_id)
{
	return MapValuesConstPtr(m_yuan_hun_zhu_decompose_cfg, item_id);
}

const YuanHunZhuComposeCfg * ZaoHuaEquipConfig::YuanHunZhuGetComposeCfg(int item_id)
{
	return MapValuesConstPtr(m_yuan_hun_zhu_compose_cfg, item_id);
}

const ZaoHuaEquipRefineCfg * ZaoHuaEquipConfig::GetZaoHuaEquipRefineCfg(int base_prof, int zao_hua_equip_index, int refine_level)
{
	ZaoHuaEquipRefineCfgKey key;
	key.base_prof = base_prof;
	key.zao_hua_equip_index = zao_hua_equip_index;
	key.refine_level = refine_level;

	return MapValuesConstPtr(m_refine_cfg, key);;
}

const ZaoHuaEquipRefineResonanceCfg * ZaoHuaEquipConfig::GetZaoHuaEquipRefineResonanceCfg(int base_prof, int refine_level)
{
	const std::vector<ZaoHuaEquipRefineResonanceCfg> * cfg_ptr = MapValuesConstPtr(m_refine_resonance_cfg, base_prof);
	if (!cfg_ptr)
	{
		return NULL;
	}

	const std::vector<ZaoHuaEquipRefineResonanceCfg> & cfg = *cfg_ptr;
	for (std::vector<ZaoHuaEquipRefineResonanceCfg>::const_reverse_iterator iter = cfg.rbegin(); iter != cfg.rend(); ++iter)
	{		
		if (refine_level >= iter->need_refine_all_level)
		{
			return &(*iter);
		}
	}

	return NULL;
}

int ZaoHuaEquipConfig::GetZaoHuaLongZhuMaxLevel()
{
	std::map<int, ZaoHuaLongZhuDrawCfg>::reverse_iterator iter = m_long_zhu_draw_cfg.rbegin();
	if (iter != m_long_zhu_draw_cfg.rend())
	{
		return iter->first;
	}

	return 0;
}

const ZaoHuaLongZhuCfg * ZaoHuaEquipConfig::GetZaoHuaLongZhuCfg(int item_id)
{
	return MapValuesConstPtr(m_long_zhu_cfg, item_id);
}

const ZaoHuaLongZhuCiTiaoCfg * ZaoHuaEquipConfig::GetZaoHuaLongZhuCiTiaoCfg(int ci_tiao_id)
{
	return MapValuesConstPtr(m_long_zhu_ci_tiao_cfg, ci_tiao_id);
}

const AttrItemCfg * ZaoHuaEquipConfig::GetZaoHuaLongZhuCiTiaoAttrItemCfg(int ci_tiao_id, int ci_tiao_level)
{
	const ZaoHuaLongZhuCiTiaoCfg * cfg = this->GetZaoHuaLongZhuCiTiaoCfg(ci_tiao_id);
	if (!cfg)
	{
		return NULL;
	}

	return MapValuesConstPtr(cfg->level_to_attr_cfg, ci_tiao_level);
}

const ZaoHuaLongZhuDrawCfg * ZaoHuaEquipConfig::GetZaoHuaLongZhuDrawCfg(int long_zhu_level)
{
	return MapValuesConstPtr(m_long_zhu_draw_cfg, long_zhu_level);
}

int ZaoHuaEquipConfig::GetZaoHuaLongRandZhuCiTiao(int item_id, int long_zhu_level)
{
	const ZaoHuaLongZhuCfg * cfg = this->GetZaoHuaLongZhuCfg(item_id);
	if (!cfg)
	{
		return 0;
	}

	std::vector<int> tmp_list;		//等级过滤后的列表
	for (int i = 0; i < (int)cfg->rand_ci_tiao_list.size(); ++i)
	{
		int curr_ci_tiao_id = cfg->rand_ci_tiao_list[i];
		const ZaoHuaLongZhuCiTiaoCfg * ci_tiao_cfg = this->GetZaoHuaLongZhuCiTiaoCfg(curr_ci_tiao_id);
		if (!ci_tiao_cfg)
		{
			continue;
		}

		if (long_zhu_level < ci_tiao_cfg->need_long_zhu_level)
		{
			continue;
		}

		tmp_list.push_back(curr_ci_tiao_id);
	}

	int s = (int)tmp_list.size();
	if (s == 0)
	{
		return 0;
	}

	int r = RandomNum(s);

	return tmp_list[r];
}

const ZaoHuaRuMoCfg * ZaoHuaEquipConfig::GetZaoHuaRuMoCfg(int item_id)
{
	return MapValuesConstPtr(m_ru_mo_cfg, item_id);
}

const ZaoHuaChangeJobCfg * ZaoHuaEquipConfig::GetChangeJobCfg(int level, int prof, int color, int equip_type)
{
	if (level >= 0 && level < ItemNamespace::EQUIPMENT_MAX_LEVEL &&
		prof >= 0 && prof < PROFESSION_TYPE_NUM &&
		color >= 0 && color < ItemBase::I_COLOR_MAX &&
		equip_type >= 0 && equip_type < ZaoHuaItem::Z_TYPE_MAX)
	{
		return m_change_job_list[level][prof][color][equip_type];
	}
	return NULL;
}

int ZaoHuaEquipConfig::InitEquipSuitCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		int zaohua_equip_suit = 0;
		if (!GetSubNodeValue(root_element, "zaohua_equip_suit", zaohua_equip_suit) || zaohua_equip_suit <= 0 || zaohua_equip_suit >= PER_ZAO_HUA_KEY_SUIT_TYPE || zaohua_equip_suit >= ZAO_HUA_SUIT_APPEARANCE_MAX)
		{
			return -1;
		}
		int level = 0;
		if (!GetSubNodeValue(root_element, "level", level) || level < 0 || level > MAX_ROLE_LEVEL)
		{
			return -2;
		}
		int color = 0;
		if (!GetSubNodeValue(root_element, "color", color) || color < ItemBase::I_COLOR_INVALID || color > ItemBase::I_COLOR_MAX)
		{
			return -3;
		}
		int suit_num = 0;
		if (!GetSubNodeValue(root_element, "suit_num", suit_num) || suit_num < 0 || suit_num > ZaoHuaItem::Z_TYPE_MAX)
		{
			return -4;
		}
		long long key = this->GetKeyByLevelAndSuitAndColor(zaohua_equip_suit, color, level, suit_num);

		ZaoHuaSuitCfg & node_cfg = m_suit_map[key];

		node_cfg.zaohua_equip_suit = zaohua_equip_suit;
		node_cfg.color = color;
		node_cfg.level = level;
		node_cfg.suit_num = suit_num;

		memset(node_cfg.attr_type_list, 0, sizeof(node_cfg.attr_type_list));
		for (int i = 0; i < ZaoHuaSuitCfg::MAX_SUIT_VOC_NUM; i++)
		{
			std::string reader_mate = STRING_SPRINTF("attr_type_%d", i + 1);
			if (!GetSubNodeValue(root_element, reader_mate, node_cfg.attr_type_list[i].attr_type) || node_cfg.attr_type_list[i].attr_type < 0 || node_cfg.attr_type_list[i].attr_type >= BATTLE_ATTR_MAX)
			{
				return -5;
			}
			reader_mate.clear();
			reader_mate = STRING_SPRINTF("attr_add_%d", i + 1);
			if (!GetSubNodeValue(root_element, reader_mate, node_cfg.attr_type_list[i].attr_value))
			{
				return -6;
			}
		}

		if (!GetSubNodeValue(root_element, "sp_id", node_cfg.sp_id) || node_cfg.sp_id < 0)
		{
			return -7;
		}

		if (!GetSubNodeValue(root_element, "score", node_cfg.score))
		{
			return -8;
		}

		for (int i = 0; i < ZaoHuaSuitCfg::ZAO_HUA_POTENTIAL_TYPE_MAX; i++)
		{
			std::string reader_mate = STRING_SPRINTF("potential_add_%d", i + 1);
			if (!GetSubNodeValue(root_element, reader_mate, node_cfg.potential_add[i]) || node_cfg.potential_add[i] < 0)
			{
				return -9;
			}
		}

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int ZaoHuaEquipConfig::InitEquipComposeCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		int seq = 0;
		if (!GetSubNodeValue(root_element, "seq", seq) || seq <= 0)
		{
			return -1;
		}
		ZaoHuaComposeCfg & node = m_compose_map[seq];
		
		if (!GetSubNodeValue(root_element, "target", node.target) || NULL == ITEMPOOL->GetItem(node.target))
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "stuff_id_1", node.stuff_id_1))
		{
			return -3;
		}
		
		if (!GetSubNodeValue(root_element, "stuff_num_1", node.stuff_num_1))
		{
			return -4;
		}

		if (!GetSubNodeValue(root_element, "stuff_id_2", node.stuff_id_2))
		{
			return -5;
		}

		if (!GetSubNodeValue(root_element, "stuff_num_2", node.stuff_num_2))
		{
			return -6;
		}

		if (!GetSubNodeValue(root_element, "price", node.price))
		{
			return -7;
		}

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int ZaoHuaEquipConfig::InitOhterCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	if (!GetSubNodeValue(root_element, "max_soul_power", m_other_cfg.hun_li_max_value)) ret_line;
	if (!GetSubNodeValue(root_element, "soul_power_add", m_other_cfg.hun_li_add_value)) ret_line;
	if (!GetSubNodeValue(root_element, "intelligence_added", m_other_cfg.ling_xing_extra_hun_li_add_value)) ret_line;
	if (!GetSubNodeValue(root_element, "blood_sacrifice_consume", m_other_cfg.hun_li_uplv_comsume_item_id)) ret_line;
	if (!GetSubNodeValue(root_element, "consume_num", m_other_cfg.hun_li_uplv_comsume_item_num)) ret_line;
	if (!GetSubNodeValue(root_element, "item_id", m_other_cfg.long_zhu_draw_comsume_item_id)) ret_line;
	if (!GetSubNodeValue(root_element, "enchant_consume", m_other_cfg.ru_mo_comsume_item_id)) ret_line;
	if (!GetSubNodeValue(root_element, "enchant_consume_num", m_other_cfg.ru_mo_comsume_item_num)) ret_line;

	return 0;
}

int ZaoHuaEquipConfig::InitVigorousCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		int seq = 0;
		if (!GetSubNodeValue(root_element, "seq", seq) || seq <= 0 || seq >= ZAO_HUA_SUIT_APPEARANCE_MAX)
		{
			return -1;
		}
		ZaoHuaVigorousCfg & node = m_vigorous_map[seq];
		node.seq = seq;
		if (!GetSubNodeValue(root_element, "zaohua_equip_suit", node.zaohua_equip_suit))
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "level", node.level))
		{
			return -3;
		}

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int ZaoHuaEquipConfig::InitZaoHuaEquipRefineCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		ZaoHuaEquipRefineCfg cfg;
		if (!GetSubNodeValue(root_element, "limit_prof", cfg.base_prof)) ret_line;
		if (!GetSubNodeValue(root_element, "hole_type", cfg.zao_hua_equip_index)) ret_line;
		if (!GetSubNodeValue(root_element, "refine_level", cfg.refine_level)) ret_line;
		if (!GetSubNodeValue(root_element, "need_equiplevel", cfg.refine_need_equip_level)) ret_line;
		
		for (int i = 0; i < 2; ++i)
		{
			std::string str1 = STRING_SPRINTF("refine_consum_%d", i + 1);
			std::string str2 = STRING_SPRINTF("refine_consum_num_%d", i + 1);

			int item_id = 0;
			int num = 0;
			if (!GetSubNodeValue(root_element, str1, item_id)) ret_line;
			if (!GetSubNodeValue(root_element, str2, num)) ret_line;

			if (item_id == 0 || num <= 0)
			{
			}

			cfg.comsume_cfg[item_id] += num;
		}

		for (int i = 0; i < 2; ++i)
		{
			std::string str1 = STRING_SPRINTF("attr_type_%d", i + 1);
			std::string str2 = STRING_SPRINTF("attr_add_%d", i + 1);

			AttrItemCfg tmp;
			if (!GetSubNodeValue(root_element, str1, tmp.attr_type)) ret_line;
			if (!GetSubNodeValue(root_element, str2, tmp.attr_value)) ret_line;

			cfg.attr_cfg.push_back(tmp);
		}
		

		ZaoHuaEquipRefineCfgKey key;
		key.base_prof = cfg.base_prof;
		key.zao_hua_equip_index = cfg.zao_hua_equip_index;
		key.refine_level = cfg.refine_level;
		m_refine_cfg[key] = cfg;

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int ZaoHuaEquipConfig::InitZaoHuaEquipRefineResonanceCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		ZaoHuaEquipRefineResonanceCfg cfg;
		if (!GetSubNodeValue(root_element, "limit_prof", cfg.base_prof)) ret_line;
		if (!GetSubNodeValue(root_element, "refine_level_all", cfg.need_refine_all_level)) ret_line;

		for (int i = 0; i < 3; ++i)
		{
			std::string str1 = STRING_SPRINTF("attr_type_%d", i + 1);
			std::string str2 = STRING_SPRINTF("attr_add_%d", i + 1);

			AttrItemCfg tmp;
			if (!GetSubNodeValue(root_element, str1, tmp.attr_type)) ret_line;
			if (!GetSubNodeValue(root_element, str2, tmp.attr_value)) ret_line;

			cfg.attr_cfg.push_back(tmp);
		}

		std::vector<ZaoHuaEquipRefineResonanceCfg> & vec = m_refine_resonance_cfg[cfg.base_prof];
		if (!vec.empty())
		{
			if (vec.back().need_refine_all_level > cfg.need_refine_all_level)
			{
				ret_line;
			}
		}

		vec.push_back(cfg);

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int ZaoHuaEquipConfig::InitZaoHuaEquipLingXingValueCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	m_ling_xing_cfg.rate_count = 0;
	while (NULL != root_element)
	{
		ZaoHuaEquipLingXingValueItemCfg cfg;
		if (!GetSubNodeValue(root_element, "intelligence", cfg.ling_xing_value)) ret_line;
		if (!GetSubNodeValue(root_element, "rate", cfg.rate) || cfg.rate <= 0) ret_line;

		m_ling_xing_cfg.rate_count += cfg.rate;
		m_ling_xing_cfg.item_list.push_back(cfg);

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int ZaoHuaEquipConfig::InitZaoHuaEquipHunLiAttrCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		ZaoHuaEquipHunLiAttrCfg cfg;
		if (!GetSubNodeValue(root_element, "limit_prof", cfg.base_prof)) ret_line;
		if (!GetSubNodeValue(root_element, "hole_type", cfg.zao_hua_equip_index)) ret_line;
		if (!GetSubNodeValue(root_element, "soul_power", cfg.hun_li_value)) ret_line;

		for (int i = 0; i < 6; ++i) 
		{
			std::string str1 = STRING_SPRINTF("attr_type_%d", i + 1);
			std::string str2 = STRING_SPRINTF("attr_add_%d", i + 1);

			AttrItemCfg tmp;
			if (!GetSubNodeValue(root_element, str1, tmp.attr_type)) ret_line;
			if (!GetSubNodeValue(root_element, str2, tmp.attr_value)) ret_line;

			cfg.attr_list.push_back(tmp);
		}

		ZaoHuaEquipHunLiAttrCfgKey key;
		key.base_prof = cfg.base_prof;
		key.zao_hua_equip_index = cfg.zao_hua_equip_index;

		std::vector<ZaoHuaEquipHunLiAttrCfg > & vec = m_hun_li_attr_cfg[key];
		if (!vec.empty())
		{
			if (cfg.hun_li_value < vec.back().hun_li_value)
			{
				ret_line;
			}
		}

		vec.push_back(cfg);

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int ZaoHuaEquipConfig::InitYuanHunZhuCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}

	std::map<int, int> type_to_level_map;
	while (NULL != data_element)
	{
		YuanHunZhuCfg cfg;

		if (!GetSubNodeValue(data_element, "type", cfg.type))
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(data_element, "yuan_soul_id", cfg.item_id))
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(data_element, "next_level_id", cfg.next_item_id))
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(data_element, "yuan_soul_level", cfg.yuan_hun_zhu_level) || type_to_level_map[cfg.type] + 1 != cfg.yuan_hun_zhu_level)
		{
			return -__LINE__;
		}
		type_to_level_map[cfg.type] = cfg.yuan_hun_zhu_level;

		if (!GetSubNodeValue(data_element, "need_num", cfg.up_level_need_money_value))
		{
			return -__LINE__;
		}


		for (int i = 0; i < ARRAY_LENGTH(cfg.slot_attr_rand_group_id); ++i)
		{
			std::string s = STRING_SPRINTF("hole_%d", i + 1);
		
			if (!GetSubNodeValue(data_element, s, cfg.slot_attr_rand_group_id[i]))
			{
				return -__LINE__;
			}
		}

		m_yuan_hun_zhu_cfg[cfg.item_id] = cfg;
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int ZaoHuaEquipConfig::InitYuanHunZhuAttrGroupCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}

	while (NULL != data_element)
	{	
		int group_id = 0;
		if (!GetSubNodeValue(data_element, "group_id", group_id))
		{
			return -__LINE__;
		}

		YuanHunZhuAttrGroupCfg & cfg = m_yuan_hun_zhu_attr_group_cfg[group_id];
		cfg.group_id = group_id;

		YuanHunZhuAttrGroupItemCfg item_cfg;

		if (!GetSubNodeValue(data_element, "seq", item_cfg.seq))
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(data_element, "rate", item_cfg.rate) || item_cfg.rate <= 0)
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(data_element, "type", item_cfg.attr.attr_type))
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(data_element, "value", item_cfg.attr.attr_value))
		{
			return -__LINE__;
		}

		if (MapValuesConstPtr(cfg.item_list, item_cfg.seq) != NULL)
		{
			return -__LINE__;
		}

		cfg.rate_count += item_cfg.rate;
		cfg.item_list[item_cfg.seq] = item_cfg;
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int ZaoHuaEquipConfig::InitYuanHunZhuCombinationAttrCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}

	while (NULL != data_element)
	{
		YuanHunZhuCombinationAttrCfg cfg;

		if (!GetSubNodeValue(data_element, "limit_prof", cfg.base_prof))
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(data_element, "yuan_hun_zhu_combination_id", cfg.combination_id))
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(data_element, "combination_level", cfg.combination_level))
		{
			return -__LINE__;
		}

		for (int i = 0; i < 4; ++i)
		{
			AttrItemCfg attr_cfg;

			std::string type = STRING_SPRINTF("type_%d", i + 1);
			std::string value = STRING_SPRINTF("value_%d", i + 1);

			if (!GetSubNodeValue(data_element, type, attr_cfg.attr_type))
			{
				return -__LINE__;
			}

			if (!GetSubNodeValue(data_element, value, attr_cfg.attr_value))
			{
				return -__LINE__;
			}

			if (attr_cfg.attr_value == 0)
			{
				continue;
			}

			cfg.attr_cfg.push_back(attr_cfg);
		}

		if (!GetSubNodeValue(data_element, "sp_id", cfg.sp_id))
		{
			return -__LINE__;
		}

		YuanHunZhuCombinationAttrCfgKey key;
		key.base_prof = cfg.base_prof;
		key.combination_id = cfg.combination_id;
		key.combination_level = cfg.combination_level;
		m_yuan_hun_zhu_combination_attr_cfg[key] = cfg;

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int ZaoHuaEquipConfig::InitYuanHunZhuCombinationCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}

	while (NULL != data_element)
	{
		YuanHunZhuCombinationCfg cfg;

		if (!GetSubNodeValue(data_element, "resonance_group_id", cfg.combination_group_id))return -__LINE__;

		int num = 0;
		if (!GetSubNodeValue(data_element, "yuan_soul_num", num))return -__LINE__;
		int color = 0;
		if (!GetSubNodeValue(data_element, "yuan_soul_color", color))return -__LINE__;

		cfg.yuan_hun_zhu_color_need_num[color] = num;
	
		m_yuan_hun_zhu_combination_cfg.push_back(cfg);
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int ZaoHuaEquipConfig::InitYuanHunZhuDecomposeCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}

	while (NULL != data_element)
	{
		YuanHunZhuDecomposeCfg cfg;

		if (!GetSubNodeValue(data_element, "id", cfg.item_id))return -__LINE__;

		ItemConfigData t;
		t.is_bind = true;
		if (!GetSubNodeValue(data_element, "return_item", t.item_id))return -__LINE__;
		if (!GetSubNodeValue(data_element, "return_num", t.num))return -__LINE__;
		if (!GetSubNodeValue(data_element, "return_money", cfg.decompose_get_money_type))return -__LINE__;
		if (!GetSubNodeValue(data_element, "return_money_num", cfg.decompose_get_money_value))return -__LINE__;

		if (t.item_id != 0)
		{
			cfg.decompose_get_item.push_back(t);
		}

		m_yuan_hun_zhu_decompose_cfg[cfg.item_id] = cfg;
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int ZaoHuaEquipConfig::InitYuanHunZhuComposeCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}

	while (NULL != data_element)
	{
		YuanHunZhuComposeCfg cfg;

		if (!GetSubNodeValue(data_element, "id", cfg.item_id))return -__LINE__;
		if (!GetSubNodeValue(data_element, "use_item", cfg.yuan_hun_zhu_compose_need_item_id))return -__LINE__;
		if (!GetSubNodeValue(data_element, "use_num", cfg.yuan_hun_zhu_compose_need_item_num))return -__LINE__;
		m_yuan_hun_zhu_compose_cfg[cfg.item_id] = cfg;
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int ZaoHuaEquipConfig::InitZaoHuaLongZhuDrawCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}

	while (NULL != data_element)
	{
		ZaoHuaLongZhuDrawCfg cfg;
		cfg.rate_count = 0;

		if (!GetSubNodeValue(data_element, "level", cfg.level))return -__LINE__;
		if (!GetSubNodeValue(data_element, "mini_get_times", cfg.up_level_bao_di_ci_shu))return -__LINE__;
		if (!GetSubNodeValue(data_element, "expends_num", cfg.comsume_item_num_draw_1))return -__LINE__;
		if (!GetSubNodeValue(data_element, "ten_expends_num", cfg.comsume_item_num_draw_10))return -__LINE__;
		if (!GetSubNodeValue(data_element, "level", cfg.level))return -__LINE__;
		
		for (int i = 0; i < 4; ++i)
		{
			ZaoHuaLongZhuDrawUpLevelCfg item_cfg;
			item_cfg.level = i;

			std::string s1 = STRING_SPRINTF("rate_%d", i);
			if (!GetSubNodeValue(data_element, s1, item_cfg.rate))return -__LINE__;
			
			cfg.rate_count += item_cfg.rate;
			cfg.item_list.push_back(item_cfg);
		}

		m_long_zhu_draw_cfg[cfg.level] = cfg;
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int ZaoHuaEquipConfig::InitZaoHuaLongZhuCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}

	while (NULL != data_element)
	{
		ZaoHuaLongZhuCfg cfg;

		if (!GetSubNodeValue(data_element, "equipment_id", cfg.item_id))return -__LINE__;
		if (ReadCfgListValue(data_element, "affix_group", cfg.rand_ci_tiao_list) != 0) return -__LINE__;
		if (!GetSubNodeValue(data_element, "affix_limit", cfg.limit_max_level))return -__LINE__;
		if (!GetSubNodeValue(data_element, "affix_num", cfg.ci_tiao_num))return -__LINE__;
		
		m_long_zhu_cfg[cfg.item_id] = cfg;
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int ZaoHuaEquipConfig::InitZaoHuaLongZhuCiTiaoCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}

	while (NULL != data_element)
	{
		ZaoHuaLongZhuCiTiaoCfg cfg;

		if (!GetSubNodeValue(data_element, "affix_id", cfg.ci_tiao_id))return -__LINE__;
		if (!GetSubNodeValue(data_element, "lowest_level", cfg.need_long_zhu_level))return -__LINE__;

		int attr_type = 0;
		if (!GetSubNodeValue(data_element, "affix_attribute", attr_type))return -__LINE__;

		for (int i = 0; i < 35; ++i)
		{
			int level = i + 1;
			AttrItemCfg item_cfg;
			item_cfg.attr_type = attr_type;

			std::string s1 = STRING_SPRINTF("level_%d", level);
			if (!GetSubNodeValue(data_element, s1, item_cfg.attr_value))return -__LINE__;

			cfg.level_to_attr_cfg[level] = item_cfg;
		}

		m_long_zhu_ci_tiao_cfg[cfg.ci_tiao_id] = cfg;
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int ZaoHuaEquipConfig::InitZaoHuaRuMoCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}

	while (NULL != data_element)
	{
		ZaoHuaRuMoCfg cfg;

		if (!GetSubNodeValue(data_element, "enchant_before", cfg.item_id))return -__LINE__;
		if (!GetSubNodeValue(data_element, "enchant_after", cfg.to_item_id))return -__LINE__;


		m_ru_mo_cfg[cfg.item_id] = cfg;
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int ZaoHuaEquipConfig::InitChangeJobCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}
	memset(m_change_job_list, NULL, sizeof(m_change_job_list));
	while (NULL != data_element)
	{
		ZaoHuaChangeJobCfg cfg;

		if (!GetSubNodeValue(data_element, "equip_level", cfg.equip_level))return -__LINE__;
		if (!GetSubNodeValue(data_element, "limit_prof", cfg.limit_prof))return -__LINE__;
		if (!GetSubNodeValue(data_element, "color", cfg.color))return -__LINE__;
		if (!GetSubNodeValue(data_element, "equip_type", cfg.equip_type))return -__LINE__;
		if (!GetSubNodeValue(data_element, "id", cfg.id))return -__LINE__;

		m_change_job_vec.push_back(cfg);
		data_element = data_element->NextSiblingElement();
	}

	for (std::vector<ZaoHuaChangeJobCfg>::iterator it = m_change_job_vec.begin(); it != m_change_job_vec.end(); it++)
	{
		if (it->equip_level >= 0 && it->equip_level < ItemNamespace::EQUIPMENT_MAX_LEVEL &&
			it->limit_prof >= 0 && it->limit_prof < PROFESSION_TYPE_NUM &&
			it->color >= 0 && it->color < ItemBase::I_COLOR_MAX &&
			it->equip_type >= 0 && it->equip_type < ZaoHuaItem::Z_TYPE_MAX)
		{
			m_change_job_list[it->equip_level][it->limit_prof][it->color][it->equip_type] = &(*it);
		}
		else
		{
			return -999;
		}
	}

	return 0;
}

long long ZaoHuaEquipConfig::GetKeyByLevelAndSuitAndColor(int suit_id, int color, int level, int suit_num)
{
	long long ret = suit_id * PER_ZAO_HUA_KEY_SUIT_TYPE + color * PER_ZAO_HUA_KEY_COLOR + level * PER_ZAO_HUA_KEY_LEVEL + suit_num * PER_ZAO_HUA_KEY_SUIT_NUM;
	return ret;
}

void ZaoHuaEquipConfig::GetLevelAnsSuitAndColorAndSuitNumByKey(long long key, int * suit_id, int * color, int * level, int * suit_num)
{
	int or_suit_num = key / PER_ZAO_HUA_KEY_SUIT_NUM;
	int or_level = (key % PER_ZAO_HUA_KEY_SUIT_NUM) / PER_ZAO_HUA_KEY_LEVEL;
	int or_color = ((key % PER_ZAO_HUA_KEY_SUIT_NUM) % PER_ZAO_HUA_KEY_LEVEL) / PER_ZAO_HUA_KEY_COLOR;
	int or_suit_id = (((key % PER_ZAO_HUA_KEY_SUIT_NUM) % PER_ZAO_HUA_KEY_LEVEL) % PER_ZAO_HUA_KEY_COLOR) / PER_ZAO_HUA_KEY_SUIT_TYPE;
	if (NULL != suit_id)*suit_id = or_suit_id;
	if (NULL != color)*color = or_color;
	if (NULL != level)*level = or_level;
	if (NULL != suit_num)*suit_num = or_suit_num;
}

int ZaoHuaLongZhuDrawCfg::GetRandUpLevel(int bao_di_ci_shu) const
{
	int r = RandomNum(rate_count);

	int level = 0;
	for (int i = 0; i < (int)item_list.size(); ++i)
	{
		const ZaoHuaLongZhuDrawUpLevelCfg & curr = item_list[i];
		if (r < curr.rate)
		{
			level = curr.level;
			break;
		}

		r -= curr.rate;
	}

	if (level <= 0 && bao_di_ci_shu >= up_level_bao_di_ci_shu)
	{
		level = 1;
	}

	return level;
}
