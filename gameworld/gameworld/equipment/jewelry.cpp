#include <assert.h>
#include "jewelry.h"
#include "obj/character/role.h"
#include "servercommon/errornum.h"
#include "globalconfig/globalconfig.h"
#include "engineadapter.h"
#include "item/itempool.h"
#include "item/knapsack.h"
#include "servercommon/struct/battle/battle_def.hpp"
#include "config/logicconfigmanager.hpp"
#include "other/workshop/jewelryconfig.hpp"
#include "config/otherconfig/sundryconfig.hpp"
#include "gameworld/config/otherconfig/scoresystemconfig.hpp"
#include "gameworld/config/otherconfig/entryconfig.hpp"
#include "other/workshop/suitlistconfig.hpp"
#include "other/equipstar/equipstarconfig.hpp"

Jewelry::Jewelry(short jwewlry_type) : ItemBase(I_TYPE_JEWELRY), m_jewelry_type(jwewlry_type), m_equipment_level(0), m_ornament_type(0), m_is_unlimit_level(false), m_attr_add_type(0), m_suit_random_group(0)
{
}

int Jewelry::Use(Role *role, const ItemGridData &item_data, short &num, short param1, short param2, short param3, bool & out_need_set_dirty, int param_type /*= ITEM_PARAM_TYPE_INVAILD*/, void * param /*= NULL*/) const
{
	role->NoticeNum(errornum::EN_KNAPSACK_USE_NOT_DIRECT);
	return -1;
}

void Jewelry::RandomNetValueOnPutItem(Role * role, NetValueItemParam * param_data, int put_reason) const
{
	if (NULL == param_data) return;

	if (param_data->jewelry_param.attr_list[0].attr_value <= 0 && param_data->jewelry_param.attr_list[1].attr_value <= 0)
	{
		this->RandomNetValueParam(&param_data->jewelry_param);
		if (PUT_REASON_WORKSHOP_COMPOUND == put_reason && NULL != role) role->GetName(param_data->jewelry_param.creator_name);
		// 检测是否珍品
		param_data->jewelry_param.is_treasure = (char)this->CheckEquipmentIsTreasure(param_data->jewelry_param);
	}

	if (this->GetColor() >= ItemBase::I_COLOR_STAR && this->GetColor() <= ItemBase::I_COLOR_GOD)
	{
		param_data->jewelry_param.strengthen_level += 1;
	}

	//额外词条 
	int entry_num = 0;// LOGIC_CONFIG->GetEntryConfig()->GetEntryNumConfig(this->GetEntryNum());
					  //词条解锁数
	if (this->IsNeedReCalSmeltEntryNum())
	{
		const GodEquipAttrNumConfig* attr_cfg = LOGIC_CONFIG->GetEntryConfig()->GetGodEquipmentNumCfg(param_data->jewelry_param.strengthen_level, this->GetEquipColor());
		if (attr_cfg != NULL)entry_num = entry_num >= attr_cfg->max_num ? entry_num : attr_cfg->max_num;
	}
	else
	{
		const MaxAttrNumCfg *attr_max_info = LOGIC_CONFIG->GetEntryConfig()->GetEntryAttrMaxNumConfig(this->GetJewelryLevel(), this->GetEquipColor());
		if (attr_max_info != NULL)
		{
			entry_num = attr_max_info->max_num;
		}
	}

	for (int voc_idx = 0; voc_idx < entry_num && voc_idx < JewelryParam::EXA_ATTR_NUM_MAX; voc_idx++)
	{
		param_data->jewelry_param.attr_extra_inc[voc_idx].un_lock = 0;
	}
	if (entry_num > 0)//是否有词条
	{
		RandGroupCfg entry_type_list[JewelryParam::EXA_ATTR_NUM_MAX];
		int real_entry_num = LOGIC_CONFIG->GetEntryConfig()->GetEntryRandConfig(this->GetEntryGroup(), entry_num, entry_type_list);
		if (real_entry_num > 0)
		{
			for (int attr_type_index = 0; attr_type_index < JewelryParam::EXA_ATTR_NUM_MAX; attr_type_index++)
			{
				if (entry_type_list[attr_type_index].att_type < 0)continue;
				{
					const AttValueCfg *attr_info = LOGIC_CONFIG->GetEntryConfig()->GetEntryAttrValueConfig4(this->GetJewelryLevel(), entry_type_list[attr_type_index].quality, entry_type_list[attr_type_index].att_type);
					if (attr_info != NULL)
					{
						//if (param_data->jewelry_param.inc_num >= JewelryParam::EXA_ATTR_NUM_MAX) break;
						int attr_value = rand() % ((attr_info->max - attr_info->min) + 1) + attr_info->min;
						if (attr_value <= 0)continue;
						JewelryParam & attr_param = param_data->jewelry_param;
						AttrVocInfo & attr_inc = attr_param.attr_extra_inc[attr_type_index];
						if (attr_inc.un_lock != 1)continue;
						attr_inc.attr_type = entry_type_list[attr_type_index].att_type;
						attr_inc.attr_value = attr_value;
						//param_data->jewelry_param.inc_num++;
					}
				}
			}
			param_data->jewelry_param.inc_num = entry_num;
			/*//检查是否保底
			//保底，颜色以装备本身品质颜色
			int voc_top_color = 0;
			for (int color_idx = 0; color_idx < 6; color_idx++)
			{
			if (entry_type_list[color_idx].quality > voc_top_color)voc_top_color = entry_type_list[color_idx].quality;
			}
			if (voc_top_color < (this->GetEquipColor() > 5 ? 5 : this->GetEquipColor()) && param_data->jewelry_param.inc_num > 0)//如果没有一个词条的颜色大于等于装备颜色，则从已随机词条抽取一个，边做保底词条
			{
			int rand_voc_idx = rand() % param_data->jewelry_param.inc_num;
			if (rand_voc_idx >= 0 && rand_voc_idx < JewelryParam::EXA_ATTR_NUM_MAX)
			{
			AttrVocInfo &top_voc = param_data->jewelry_param.attr_extra_inc[rand_voc_idx];
			const AttValueCfg *attr_info = LOGIC_CONFIG->GetEntryConfig()->GetEntryAttrValueConfig2(this->GetJewelryLevel(), this->GetEquipColor() > 5 ? 5 : this->GetEquipColor(), top_voc.attr_type);
			if (attr_info != NULL)
			{
			int attr_value = rand() % ((attr_info->max - attr_info->min) + 1) + attr_info->min;
			if (attr_value > 0)
			{
			top_voc.attr_value = attr_value;
			}
			}
			}
			}*/
		}
	}
}

bool Jewelry::RandomNetValueParam(JewelryParam * out_param) const
{
	bool has_rand = false;//检查
	out_param->attr_num = 0;
	if (star_group > 0)
	{
		const EquipStarGroupCfg * star_level_cfg = LOGIC_CONFIG->GetEquipStarConfig()->RandStarLevelCfg(star_group);
		if (NULL != star_level_cfg)
		{
			const EquipStarCfg * star_cfg = LOGIC_CONFIG->GetEquipStarConfig()->GetEquipStarAttrCfg(this->GetItemId(), star_level_cfg->item_star);
			if (NULL != star_cfg)
			{
				for (int attr_idx = 0; attr_idx < EquipStarCfg::ATTR_NUM_MAX; ++attr_idx)
				{
					if (out_param->attr_num >= JewelryParam::ATTR_NUM_MAX) break;

					const EquipStarAttr & attr_as = star_cfg->attr_voc[attr_idx];
					if (attr_as.attr_max == 0 && attr_as.attr_min == 0) continue;

					AttrInfo & p_ai = out_param->attr_list[out_param->attr_num++];
					p_ai.attr_type = attr_as.attr_type;
					int rand_adv = attr_as.attr_max - attr_as.attr_min + 1;
					if (rand_adv == 0)
					{
						p_ai.attr_value = attr_as.attr_min;
					}
					else
					{
						p_ai.attr_value = rand() % rand_adv + attr_as.attr_min;
					}
				}
				out_param->star_level = star_cfg->item_star;
				has_rand = true;
			}
		}
	}
	if (!has_rand)
	{
		// 基本属性
		out_param->attr_list[0].attr_type = m_attr_add_type;
		out_param->attr_list[0].attr_value = rand() % (m_default_attr_range.max_val - m_default_attr_range.min_val + 1) + m_default_attr_range.min_val;


		out_param->attr_list[1].attr_type = m_attr_add_type2;
		out_param->attr_list[1].attr_value = rand() % (m_default_attr_range2.max_val - m_default_attr_range2.min_val + 1) + m_default_attr_range2.min_val;

		if (out_param->attr_list[0].attr_value != 0)out_param->attr_num += 1;
		if (out_param->attr_list[1].attr_value != 0)out_param->attr_num += 1;
	}

	//随机套装id
	const SuitListGroup * suit_cfg = LOGIC_CONFIG->GetSuitListConfig()->GetSuitListGroup(m_suit_random_group);
	if (suit_cfg != NULL)
	{
		int suit_rad = rand() % suit_cfg->rate_weight;
		for (std::vector<SuitGroup>::const_iterator suit_it = suit_cfg->m_suit_group.begin(); suit_it != suit_cfg->m_suit_group.end(); suit_it++)
		{
			if (suit_it->rate > suit_rad)
			{
				out_param->new_suit_effect_id = suit_it->id;
				break;
			}
		}
	}
	out_param->suit_effect_change_flag = 1;
	return true;
}

bool Jewelry::RandomNetValueParamReforge(JewelryParam * out_param, int ly_id_1, int ly_id_2, int anima_1, int anima_2) const
{
	// 	基本属性
	// 		out_param->attr_pair.attr_type = m_attr_add_type;
	// 		out_param->attr_pair.attr_value = rand() % (m_default_attr_range.max_val - m_default_attr_range.min_val + 1) + m_default_attr_range.min_val;
	// 	
	// 		// 基本属性
	// 		out_param->attr_pair2.attr_type = m_attr_add_type2;
	// 		out_param->attr_pair2.attr_value = rand() % (m_default_attr_range2.max_val - m_default_attr_range2.min_val + 1) + m_default_attr_range2.min_val;
	// 	
	// 		// 词牌ID
	// 		out_param->lyrics_id = LOGIC_CONFIG->GetJewelryConfig()->SearchLegalLyricID(m_limit_level, m_color, ly_id_1, ly_id_2);
	// 		if (0 == out_param->lyrics_id) return false;
	// 	
	// 		// 传说属性
	// 		if (out_param->attr_pair.attr_value >= m_default_attr_range.max_val && m_color >= ItemBase::I_COLOR_ORANGE)
	// 		{
	// 			const SuitBasicCfg * cfg_sbc = LOGIC_CONFIG->GetJewelryConfig()->GetBasicCfgByLyricID(out_param->lyrics_id);
	// 			if (NULL == cfg_sbc) return false;
	// 	
	// 			const BattleAttrPair * cfg_bap = LOGIC_CONFIG->GetJewelryConfig()->GetLegendaryAttr(cfg_sbc->lyric_type);
	// 			if (NULL != cfg_bap)
	// 			{
	// 				out_param->lege_attr.attr_type = cfg_bap->attr_type;
	// 				out_param->lege_attr.attr_value = cfg_bap->attr_value;
	// 			}
	// 		}
	// 	
	// 		int min_anima = GetMin(anima_1, anima_2);
	// 		out_param->anima_val = (INT_MAX == anima_1 || INT_MAX == anima_2) ? GetRand(min_anima, INT_MAX) : min_anima;	// 灵气值
	return true;
}

int Jewelry::CheckEquipmentIsTreasure(const JewelryParam & p) const
{
	//int m_equip_level = this->GetJewelryLevel();
	const EquipmentScoreCfg * score_cfg = LOGIC_CONFIG->GetScoreSystemConfig()->GetEquipmentScoreCfg(this->GetItemId());
	if (score_cfg == NULL)return ITEM_SCORE_LEVEL_DEF;
	int score = 0;
	for (int attr_idx = 0; attr_idx < p.attr_num && attr_idx < EquipmentParam::ATTR_NUM_MAX; ++attr_idx)
	{
		const AttrInfo & info = p.attr_list[attr_idx];
		if (info.attr_type < 0 || info.attr_value <= 0) continue;
		score += info.attr_value;
	}

	if (score_cfg->score_s <= score)
	{
		return ITEM_SCORE_LEVEL_S;
	}

	if (score_cfg->score_a <= score)
	{
		return ITEM_SCORE_LEVEL_A;
	}

	if (score_cfg->score_b <= score)
	{
		return ITEM_SCORE_LEVEL_B;
	}

	return ITEM_SCORE_LEVEL_DEF;
}

bool Jewelry::CheckJewelryAttrValid(int type, Attribute value) const
{
	if (type == m_attr_add_type)
	{
		if (value >= m_default_attr_range.min_val && value <= m_default_attr_range.max_val)
		{
			return true;
		}
	}
	else if (type == m_attr_add_type2)
	{
		if (value >= m_default_attr_range2.min_val && value <= m_default_attr_range2.max_val)
		{
			return true;
		}
	}

	return false;
}

bool Jewelry::CheckJewelryAttrMax(JewelryParam * param) const
{
	if (NULL == param || param->attr_num <= 0) return false;

	for (int i = 0; i < param->attr_num && i < JewelryParam::ATTR_NUM_MAX; i++)
	{
		if (param->attr_list[i].attr_type == m_attr_add_type)
		{
			if (param->attr_list[i].attr_value != m_default_attr_range.max_val)
			{
				return false;
			}
		}
		else if (param->attr_list[i].attr_type == m_attr_add_type2)
		{
			if (param->attr_list[i].attr_value != m_default_attr_range2.max_val)
			{
				return false;
			}
		}
	}

	return true;
}

bool Jewelry::CanStrongUltimate(const JewelryParam & p)
{
	bool can_ultimate = false;
	for (int p_idx = 0; p_idx < EquipmentParam::ATTR_NUM_MAX && p_idx < p.attr_num; p_idx++)
	{
		const AttrInfo & p_ai = p.attr_list[p_idx];
		const short & ultimate_info = p.ultimate_attr_list[p_idx];
		for (int attr_idx = 0; attr_idx < EQUIP_JEWELRY_ATTR_NUM; attr_idx++)
		{
			if (p_ai.attr_type == m_attr_type[attr_idx])
			{
				if (ultimate_info < m_max_value[attr_idx])
				{
					can_ultimate = true;
				}
			}
		}
	}
	return can_ultimate;
}

bool Jewelry::GetNormalUltimateMaxValue(int ultimate_type, int * min_value, int * max_value)
{
	if (ultimate_type == m_attr_add_type)
	{
		*min_value = m_default_attr_range.min_val;
		*max_value = m_default_attr_range.max_val;
		return true;
	}
	else if (ultimate_type == m_attr_add_type2)
	{
		*min_value = m_default_attr_range2.min_val;
		*max_value = m_default_attr_range2.max_val;
		return true;
	}
	return false;
}

bool Jewelry::GetUltimateMaxValue(int ultimate_type, int * max_value, int * min_value)
{
	for (int attr_idx = 0; attr_idx < EQUIP_JEWELRY_ATTR_NUM; attr_idx++)
	{
		if (ultimate_type == m_attr_type[attr_idx])
		{
			*max_value = m_max_value[attr_idx];
			if (NULL != min_value) *min_value = m_min_value[attr_idx];
			return true;
		}
	}

	return false;
}

bool Jewelry::GetNormalMinAndMaxUltimateValue(int * min_value, int * max_value)
{
	if (m_min_jg_common <= 0 || m_max_jg_common <= 0)
	{
		return false;
	}
	*min_value = m_min_jg_common;
	*max_value = m_max_jg_common;
	return true;
}

bool Jewelry::GetStrongMinAndMaxUltimateValue(int * min_value, int * max_value)
{
	if (m_min_jg_added <= 0 || m_max_jg_added <= 0)
	{
		return false;
	}
	*min_value = m_min_jg_added;
	*max_value = m_max_jg_added;
	return true;
}
