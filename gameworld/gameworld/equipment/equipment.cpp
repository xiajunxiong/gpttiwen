#include <assert.h>
#include "equipment.h"
#include "obj/character/role.h"
#include "servercommon/errornum.h"
#include "globalconfig/globalconfig.h"
#include "engineadapter.h"
#include "item/itempool.h"
#include "item/knapsack.h"
#include "servercommon/struct/battle/battle_def.hpp"
#include "servercommon/serverconfig/battleconfig/battle_special_effect_pool.hpp"
#include "config/logicconfigmanager.hpp"
#include "config/otherconfig/sundryconfig.hpp"
#include "config/otherconfig/entryconfig.hpp"
#include "config/otherconfig/scoresystemconfig.hpp"
#include "gamelog.h"
#include "other/workshop/suitlistconfig.hpp"
#include "other/equipstar/equipstarconfig.hpp"

//////////////////////////////////////////////////////////////////////////
static const int EQUIP_ATTR_TO_BATTLE_ATTR[Equipment::EQUIP_ATTR_IDX_MAX] = {
	BATTLE_ATTR_MAX_HP,
	BATTLE_ATTR_MAX_MP,
	BATTLE_ATTR_ATTACK,
	BATTLE_ATTR_DEFENSE,
	BATTLE_ATTR_AGILITY,
	BATTLE_ATTR_MENTAL,
	BATTLE_ATTR_RECOVERY,
	BATTLE_ATTR_CRITICAL,
	BATTLE_ATTR_CRITICAL_INC_VALUE,
	BATTLE_ATTR_CRITICAL_DEC_VALUE,
	BATTLE_ATTR_COUNTER_ATTACK,
	BATTLE_ATTR_HIT,
	BATTLE_ATTR_DODGE,
	BATTLE_ATTR_ANTI_POISON,
	BATTLE_ATTR_ANTI_PETRIFY,
	BATTLE_ATTR_ANTI_FREEZE,
	BATTLE_ATTR_ANTI_DRUNK,
	BATTLE_ATTR_ANTI_CHAOS,
	BATTLE_ATTR_MAGIC_ATTACK,
	BATTLE_ATTR_MAGIC_DEFENSE
};
UNSTD_STATIC_CHECK(Equipment::EQUIP_ATTR_IDX_MAX == 20);

Equipment::Equipment(short equip_type) : ItemBase(I_TYPE_EQUIPMENT), m_equipment_type(equip_type), m_equipment_class(0),
m_entry_num(0), m_entry_group(0), m_weapon_type(0), m_is_unlimit_level(false), m_special_equip(0), m_min_jg_common(0),
m_max_jg_common(0), m_min_jg_added(0), m_max_jg_added(0), m_effect_random_group(0), m_equip_level(0), m_limit_prof_grade(0),
m_suit_random_group(0), star_group(0)
{
	memset(m_attr_list, 0, sizeof(m_attr_list));
	memset(m_attr_type, 0, sizeof(m_attr_type));
	memset(m_min_value, 0, sizeof(m_min_value));
	memset(m_max_value, 0, sizeof(m_max_value));
}

int Equipment::Use(Role *role, const ItemGridData &item_data, short &num, short param1, short param2, short param3, bool & out_need_set_dirty, int param_type /*= ITEM_PARAM_TYPE_INVAILD*/, void * param /*= NULL*/) const
{
	/*
	if (NULL == role || NULL == item_data.param) return -1;

	num = 1;  // 装备的消耗数量肯定为1

	ItemDataWrapper item_wrapper(m_item_id, 1, 1);
	item_wrapper.invalid_time = item_data.invalid_time;
	item_data.param->GetData(item_wrapper.param_data);
	item_wrapper.has_param = 1;

	const ItemBase *item = ITEMPOOL->GetItem(item_wrapper.item_id);
	if (NULL == item || I_TYPE_EQUIPMENT != item->GetItemType()) return -1;

	//TODO return role->GetEquipmentManager()->PutOn(item_wrapper, equip_index);
	*/

	role->NoticeNum(errornum::EN_KNAPSACK_USE_NOT_DIRECT);

	return -1;
}

int Equipment::GetSellPrice(NetValueItemParam *param) const
{
	int sell_price = this->ItemBase::GetSellPrice(param);

	/*
	if (NULL != param)
	{
	EquipmentParam *equip_param = &param->equip_param;

	}
	*/
	return sell_price;

}

bool Equipment::CanDiscard(NetValueItemParam *param, int *reason) const
{
	if (NULL == param) return false;

	return this->ItemBase::CanDiscard(param, reason);
}

bool Equipment::IsMatchIndex(short equip_type, short equip_index)
{
	if (equip_type < E_TYPE_MIN || equip_type >= E_TYPE_MAX || equip_index < E_INDEX_MIN || equip_index >= E_INDEX_MAX) return false;

	if ((equip_type - E_TYPE_MIN) != (equip_index - E_INDEX_MIN)) return false;

	return true;
}

bool Equipment::IsValidType(short equip_type)
{
	if (equip_type < Equipment::E_TYPE_MIN || equip_type >= Equipment::E_TYPE_MAX) return false;

	return true;
}

void Equipment::RandomNetValueOnPutItem(Role* role, NetValueItemParam* param_data, int put_reason) const
{
	if (NULL == param_data) return;

	EquipmentParam & data = param_data->equip_param;
	if (0 == data.has_random_net_value)
	{
		data.has_random_net_value = 1;

		//当从非工坊途径获取到装备时，对装备的随机属性赋值
		if (0 == data.attr_num) this->RandomEquipmentAttr(param_data);

		if (this->GetColor() >= ItemBase::I_COLOR_STAR && this->GetColor() <= ItemBase::I_COLOR_GOD)
		{
			data.strengthen_level += 1;
		}

		//熔炼属性
		int entry_num = 0;// LOGIC_CONFIG->GetEntryConfig()->GetEntryNumConfig(this->GetEntryNum());
						  //词条解锁数
		if (this->IsNeedReCalSmeltEntryNum())
		{
			const GodEquipAttrNumConfig* attr_cfg = LOGIC_CONFIG->GetEntryConfig()->GetGodEquipmentNumCfg(param_data->jewelry_param.strengthen_level, this->GetEquipColor());
			if (attr_cfg != NULL)entry_num = entry_num >= attr_cfg->max_num ? entry_num : attr_cfg->max_num;
		}
		else
		{
			const MaxAttrNumCfg *attr_max_info = LOGIC_CONFIG->GetEntryConfig()->GetEntryAttrMaxNumConfig(this->GetEquipLevel(), this->GetEquipColor());
			if (attr_max_info != NULL)
			{
				entry_num = attr_max_info->max_num;
			}
		}

		for (int voc_idx = 0; voc_idx < entry_num && voc_idx < EquipmentParam::EXA_ATTR_NUM_MAX; voc_idx++)
		{
			data.attr_extra_inc[voc_idx].un_lock = 0;
		}
		if (entry_num > 0)
		{
			RandGroupCfg entry_type_list[EquipmentParam::EXA_ATTR_NUM_MAX];
			int real_entry_num = LOGIC_CONFIG->GetEntryConfig()->GetEntryRandConfig(this->GetEntryGroup(), entry_num, entry_type_list);
			if (real_entry_num > 0)
			{
				for (int attr_type_index = 0; attr_type_index < EquipmentParam::EXA_ATTR_NUM_MAX; attr_type_index++)
				{
					if (entry_type_list[attr_type_index].att_type < 0)continue;
					{
						const AttValueCfg *attr_info = LOGIC_CONFIG->GetEntryConfig()->GetEntryAttrValueConfig3(this->GetEquipLevel(), entry_type_list[attr_type_index].quality, entry_type_list[attr_type_index].att_type);
						if (attr_info != NULL)
						{
							//if (param_data->equip_param.inc_num >= EquipmentParam::EXA_ATTR_NUM_MAX) break;
							int attr_value = rand() % ((attr_info->max - attr_info->min) + 1) + attr_info->min;
							if (attr_value <= 0)
							{
								if (NULL != role)
								{
									gamelog::g_log_debug.printf(LL_WARNING, "Equipment::RandomNetValueOnPutItem role[%d,%s] attr_value[type:%d] <= 0!!", role->GetUID(), role->GetName(), attr_info->att_type);
								}
								else
								{
									gamelog::g_log_debug.printf(LL_WARNING, "Equipment::RandomNetValueOnPutItem attr_value[type:%d] <= 0!!", attr_info->att_type);
								}
								continue;
							}
							EquipmentParam & attr_param = param_data->equip_param;
							AttrVocInfo & attr_inc = attr_param.attr_extra_inc[attr_type_index];
							if (attr_inc.un_lock != 1)continue;
							attr_inc.attr_type = entry_type_list[attr_type_index].att_type;
							attr_inc.attr_value = attr_value;
							//param_data->equip_param.inc_num++;
						}
					}
				}
				data.inc_num = entry_num;
				/*//检查是否保底
				//保底，颜色以装备本身品质颜色
				int voc_top_color = 0;
				for (int color_idx = 0; color_idx < 6; color_idx++)
				{
				if (entry_type_list[color_idx].quality > voc_top_color)voc_top_color = entry_type_list[color_idx].quality;
				}

				if (voc_top_color < (this->GetEquipColor() > 5 ? 5 : this->GetEquipColor()) && param_data->equip_param.inc_num > 0)//如果没有一个词条的颜色大于等于装备颜色，则从已随机词条抽取一个，边做保底词条
				{
				int rand_voc_idx = rand() % param_data->equip_param.inc_num;
				if (rand_voc_idx >= 0 && rand_voc_idx < EquipmentParam::EXA_ATTR_NUM_MAX)
				{
				AttrVocInfo &top_voc = param_data->equip_param.attr_extra_inc[rand_voc_idx];
				const AttValueCfg *attr_info = LOGIC_CONFIG->GetEntryConfig()->GetEntryAttrValueConfig(this->GetEquipLevel(), this->GetEquipColor() > 5 ? 5 : this->GetEquipColor(), top_voc.attr_type);
				if (attr_info != NULL)
				{
				int attr_value = rand() % ((attr_info->max - attr_info->min) + 1) + attr_info->min;
				if (attr_value > 0)
				{
				//EquipmentParam & attr_param = param_data->equip_param;
				top_voc.attr_value = attr_value;
				}
				}
				}
				}*/
			}

			if (entry_num != real_entry_num)
			{
				if (NULL != role)
				{
					gamelog::g_log_debug.printf(LL_WARNING, "Equipment::RandomNetValueOnPutItem role[%d,%s] lost_entry!\n", role->GetUID(), role->GetName());
				}
				else
				{
					gamelog::g_log_debug.printf(LL_WARNING, "Equipment::RandomNetValueOnPutItem lost_entry!\n");
				}
			}
		}
		//检查是否有重复词条
		for (int same_index = 0; same_index < EquipmentParam::EXA_ATTR_NUM_MAX; same_index++)
		{
			AttrVocInfo &voc_info = param_data->equip_param.attr_extra_inc[same_index];
			if (voc_info.attr_type >= 0 && voc_info.attr_value > 0)
			{
				for (int same_index2 = 0; same_index2 < EquipmentParam::EXA_ATTR_NUM_MAX; same_index2++)
				{
					AttrVocInfo &voc_info2 = param_data->equip_param.attr_extra_inc[same_index2];
					if (same_index2 != same_index &&
						voc_info2.attr_type > 0 &&
						voc_info2.attr_type == voc_info.attr_type)
					{
						if (NULL != role)
						{
							gamelog::g_log_debug.printf(LL_WARNING, "Equipment::RandomNetValueOnPutItem role[%d,%s] has_same!!!!", role->GetUID(), role->GetName());
						}
						else
						{
							gamelog::g_log_debug.printf(LL_WARNING, "Equipment::RandomNetValueOnPutItem has_same!!!!");
						}
					}
				}
			}
		}
	}

	const SuitListGroup * suit_cfg = LOGIC_CONFIG->GetSuitListConfig()->GetSuitListGroup(m_suit_random_group);
	if (suit_cfg != NULL)
	{
		int suit_rad = rand() % suit_cfg->rate_weight;
		for (std::vector<SuitGroup>::const_iterator suit_it = suit_cfg->m_suit_group.begin(); suit_it != suit_cfg->m_suit_group.end(); suit_it++)
		{
			if (suit_it->rate > suit_rad)
			{
				data.new_suit_effect_id = suit_it->id;
				break;
			}
		}
	}
	data.suit_effect_change_flag = 1;
	// 检测是否珍品
	data.is_treasure = (char)this->CheckEquipmentIsTreasure(data);
}

bool Equipment::GetEquipAttrSegmentByIdx(int equip_attr_idx, AttrSegment * out_segment) const
{
	out_segment->Reset();
	if (equip_attr_idx < 0 || equip_attr_idx >= EQUIP_ATTR_IDX_MAX) return false;

	const AttrSegment & t_attr = m_attr_list[equip_attr_idx];
	out_segment->min_val = t_attr.min_val;
	out_segment->max_val = t_attr.max_val;

	return true;
}

void Equipment::RandomAttrByInsteadMin(NetValueItemParam * out_param, const int min_instead_list[EQUIP_ATTR_IDX_MAX]) const
{
	EquipmentParam & p = out_param->equip_param;
	p.attr_num = 0;
	for (int attr_idx = 0; attr_idx < EQUIP_ATTR_IDX_MAX; ++attr_idx)
	{
		if (p.attr_num >= EquipmentParam::ATTR_NUM_MAX) break;

		const AttrSegment & attr_as = m_attr_list[attr_idx];
		//if (attr_as.max_val <= 0) continue;//负数也要

		int min_val_r = (min_instead_list[attr_idx] > 0 && min_instead_list[attr_idx] <= attr_as.max_val) ? min_instead_list[attr_idx] : attr_as.min_val;
		if (min_val_r == 0)continue;//空值跳过
		AttrInfo & p_ai = p.attr_list[p.attr_num++];
		p_ai.attr_type = EQUIP_ATTR_TO_BATTLE_ATTR[attr_idx];
		p_ai.attr_value = rand() % (attr_as.max_val - min_val_r + 1) + min_val_r;
	}
}

void Equipment::RandomAttrByInsteadMax(NetValueItemParam * out_param, const int max_instead_list[EQUIP_ATTR_IDX_MAX]) const
{
	EquipmentParam & p = out_param->equip_param;
	p.attr_num = 0;
	for (int attr_idx = 0; attr_idx < EQUIP_ATTR_IDX_MAX; ++attr_idx)
	{
		if (p.attr_num >= EquipmentParam::ATTR_NUM_MAX) break;

		const AttrSegment & attr_as = m_attr_list[attr_idx];
		if (attr_as.max_val <= 0) continue;

		int max_val_r = (max_instead_list[attr_idx] > 0 && max_instead_list[attr_idx] >= attr_as.min_val) ? max_instead_list[attr_idx] : attr_as.max_val;

		AttrInfo & p_ai = p.attr_list[p.attr_num++];
		p_ai.attr_type = EQUIP_ATTR_TO_BATTLE_ATTR[attr_idx];
		p_ai.attr_value = rand() % (max_val_r - attr_as.min_val + 1) + attr_as.min_val;
	}
}

void Equipment::RandomEquipmentAttr(NetValueItemParam * out_param) const
{
	EquipmentParam & p = out_param->equip_param;
	p.attr_num = 0;
	memset(p.attr_list, 0, sizeof(p.attr_list));
	bool has_rand = false;//检查
	if (star_group > 0)
	{
		const EquipStarGroupCfg * star_level_cfg = LOGIC_CONFIG->GetEquipStarConfig()->RandStarLevelCfg(this->GetItemId());
		if (NULL != star_level_cfg)
		{
			const EquipStarCfg * star_cfg = LOGIC_CONFIG->GetEquipStarConfig()->GetEquipStarAttrCfg(this->GetItemId(), star_level_cfg->item_star);
			if (NULL != star_cfg)
			{
				for (int attr_idx = 0; attr_idx < EquipStarCfg::ATTR_NUM_MAX; ++attr_idx)
				{
					if (p.attr_num >= EquipmentParam::ATTR_NUM_MAX) break;

					const EquipStarAttr & attr_as = star_cfg->attr_voc[attr_idx];
					if (attr_as.attr_max == 0 && attr_as.attr_min == 0) continue;

					AttrInfo & p_ai = p.attr_list[p.attr_num++];
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
				p.star_level = star_cfg->item_star;
				has_rand = true;
			}
		}
	}

	if (!has_rand)
	{
		p.attr_num = 0;
		for (int attr_idx = 0; attr_idx < EQUIP_ATTR_IDX_MAX; ++attr_idx)
		{
			if (p.attr_num >= EquipmentParam::ATTR_NUM_MAX) break;

			const AttrSegment & attr_as = m_attr_list[attr_idx];
			if (attr_as.max_val == 0 && attr_as.min_val == 0) continue;

			AttrInfo & p_ai = p.attr_list[p.attr_num++];
			p_ai.attr_type = EQUIP_ATTR_TO_BATTLE_ATTR[attr_idx];
			int rand_adv = attr_as.max_val - attr_as.min_val + 1;
			if (rand_adv == 0)
			{
				p_ai.attr_value = attr_as.min_val;
			}
			else
			{
				p_ai.attr_value = rand() % rand_adv + attr_as.min_val;
			}
		}
	}
}

void Equipment::MaxValEquipmentAttr(NetValueItemParam * out_param) const
{
	EquipmentParam & p = out_param->equip_param;
	p.attr_num = 0;
	for (int attr_idx = 0; attr_idx < EQUIP_ATTR_IDX_MAX; ++attr_idx)
	{
		if (p.attr_num >= EquipmentParam::ATTR_NUM_MAX) break;

		const AttrSegment & attr_as = m_attr_list[attr_idx];
		if (attr_as.max_val <= 0) continue;

		AttrInfo & p_ai = p.attr_list[p.attr_num++];
		p_ai.attr_type = EQUIP_ATTR_TO_BATTLE_ATTR[attr_idx];
		p_ai.attr_value = attr_as.max_val;
	}
}

void Equipment::RandomSpecialEffect(NetValueItemParam * out_param) const
{
	EquipmentParam & p = out_param->equip_param;
	p.sp_id = BattleSpecialEffectPool::GetInstance().RandomSpecialEffect(m_effect_random_group);
}

int Equipment::CheckEquipmentIsTreasure(const EquipmentParam & p) const
{
	if (this->GetColor() == I_COLOR_ORANGE)return ITEM_SCORE_LEVEL_DEF;
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

bool Equipment::CheckEquipmentAttrValid(int type, Attribute value) const
{
	for (int attr_idx = 0; attr_idx < EQUIP_ATTR_IDX_MAX; ++attr_idx)
	{
		if (type == EQUIP_ATTR_TO_BATTLE_ATTR[attr_idx])
		{
			if (value < m_attr_list[attr_idx].min_val || value > m_attr_list[attr_idx].max_val)
			{
				return false;
			}
			else
			{
				return true;
			}
		}
	}

	return false;
}

bool Equipment::CheckEquipmentAttrMax(NetValueItemParam * param) const
{
	if (NULL == param || param->equip_param.attr_num <= 0) return false;

	EquipmentParam & p = param->equip_param;
	for (int i = 0; i < p.attr_num && p.attr_num <= EquipmentParam::ATTR_NUM_MAX; i++)
	{
		for (int attr_idx = 0; attr_idx < EQUIP_ATTR_IDX_MAX; ++attr_idx)
		{
			if (p.attr_list[i].attr_type == EQUIP_ATTR_TO_BATTLE_ATTR[attr_idx])
			{
				if (p.attr_list[i].attr_value != m_attr_list[attr_idx].max_val)
				{
					return false;
				}
			}
		}
	}

	return true;
}

bool Equipment::CanStrongUltimate(const EquipmentParam & p)
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

bool Equipment::GetNormalUltimateMaxValue(int ultimate_type, int *min_value, int * max_value)
{
	for (int attr_idx = 0; attr_idx < EQUIP_ATTR_IDX_MAX; ++attr_idx)
	{
		if (ultimate_type == EQUIP_ATTR_TO_BATTLE_ATTR[attr_idx])
		{
			*max_value = m_attr_list[attr_idx].max_val;
			*min_value = m_attr_list[attr_idx].min_val;
			return true;
		}
	}
	return false;
}

bool Equipment::GetUltimateMaxValue(int ultimate_type, int * max_value, int * min_value)
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

bool Equipment::GetNormalMinAndMaxUltimateValue(int * min_value, int * max_value)
{
	if (m_min_jg_common <= 0 || m_max_jg_common <= 0)
	{
		return false;
	}
	*min_value = m_min_jg_common;
	*max_value = m_max_jg_common;
	return true;
}

bool Equipment::GetStrongMinAndMaxUltimateValue(int * min_value, int * max_value)
{
	if (m_min_jg_added <= 0 || m_max_jg_added <= 0)
	{
		return false;
	}
	*min_value = m_min_jg_added;
	*max_value = m_max_jg_added;
	return true;
}

short Equipment::GetBattleWeaponType(short equipment_class)
{
	short battle_weapon_type = WEAPON_TYPE_NONE;

	switch (equipment_class)
	{
	case E_CLASS_FISTS_GLOVE:
		{
			battle_weapon_type = WEAPON_TYPE_GLOVES;
		}
		break;

	case E_CLASS_LARGE_SWORD:
		{
			battle_weapon_type = WEAPON_TYPE_SWORD;
		}
		break;

	case E_CLASS_BATTLE_AXES:
		{
			battle_weapon_type = WEAPON_TYPE_AXE;
		}
		break;

	case E_CLASS_SHARP_SPEAR:
		{
			battle_weapon_type = WEAPON_TYPE_SPEAR;
		}
		break;

	case E_CLASS_WAR_LONGBOW:
		{
			battle_weapon_type = WEAPON_TYPE_BOW;
		}
		break;

	case E_CLASS_MAGIC_STAFF:
		{
			battle_weapon_type = WEAPON_TYPE_WAND;
		}
		break;

	case E_CLASS_CLERIC_WAND:
		{
			battle_weapon_type = WEAPON_TYPE_LANTERN;
		}
		break;

	case E_CLASS_WIZARD_WAND:
		{
			battle_weapon_type = WEAPON_TYPE_FEATHER_FAN;
		}
		break;
	}

	UNSTD_STATIC_CHECK(9 == WEAPON_TYPE_MAX)

		return battle_weapon_type;
}

int Equipment::GetEquipmentAttrType(int battle_attr_type)
{
	int equipment_attr_type = -1;

	UNSTD_STATIC_CHECK(20 == EQUIP_ATTR_IDX_MAX)
		switch (battle_attr_type)
		{
		case BATTLE_ATTR_MAX_HP:
			equipment_attr_type = EQUIP_ATTR_IDX_MAXHP;
			break;
		case BATTLE_ATTR_MAX_MP:
			equipment_attr_type = EQUIP_ATTR_IDX_MAXMP;
			break;
		case BATTLE_ATTR_ATTACK:
			equipment_attr_type = EQUIP_ATTR_IDX_ATTACK;
			break;
		case BATTLE_ATTR_DEFENSE:
			equipment_attr_type = EQUIP_ATTR_IDX_DEFENSE;
			break;
		case BATTLE_ATTR_AGILITY:
			equipment_attr_type = EQUIP_ATTR_IDX_AGILITY;
			break;
		case BATTLE_ATTR_MENTAL:
			equipment_attr_type = EQUIP_ATTR_IDX_MENTAL;
			break;
		case BATTLE_ATTR_RECOVERY:
			equipment_attr_type = EQUIP_ATTR_IDX_RECOVERY;
			break;
		case BATTLE_ATTR_CRITICAL:
			equipment_attr_type = EQUIP_ATTR_IDX_CRITICAL;
			break;
		case BATTLE_ATTR_CRITICAL_INC_VALUE:
			equipment_attr_type = EQUIP_ATTR_IDX_CRITICAL_INC_VALUE;
			break;
		case BATTLE_ATTR_CRITICAL_DEC_VALUE:
			equipment_attr_type = EQUIP_ATTR_IDX_CRITICAL_DEC_VALUE;
			break;
		case BATTLE_ATTR_COUNTER_ATTACK:
			equipment_attr_type = EQUIP_ATTR_IDX_COUNTER_ATTACK;
			break;
		case BATTLE_ATTR_HIT:
			equipment_attr_type = EQUIP_ATTR_IDX_HIT;
			break;
		case BATTLE_ATTR_DODGE:
			equipment_attr_type = EQUIP_ATTR_IDX_DODGE;
			break;
		case BATTLE_ATTR_ANTI_POISON:
			equipment_attr_type = EQUIP_ATTR_IDX_ANTI_POISON;
			break;
		case BATTLE_ATTR_ANTI_PETRIFY:
			equipment_attr_type = EQUIP_ATTR_IDX_ANTI_PETRIFY;
			break;
		case BATTLE_ATTR_ANTI_FREEZE:
			equipment_attr_type = EQUIP_ATTR_IDX_ANTI_SLEEP;
			break;
		case BATTLE_ATTR_ANTI_DRUNK:
			equipment_attr_type = EQUIP_ATTR_IDX_ANTI_DRUNK;
			break;
		case BATTLE_ATTR_ANTI_CHAOS:
			equipment_attr_type = EQUIP_ATTR_IDX_ANTI_CHAOS;
			break;
		case BATTLE_ATTR_MAGIC_ATTACK:
			equipment_attr_type = EQUIP_ATTR_IDX_MAGIC_ATTACK;
			break;
		case BATTLE_ATTR_MAGIC_DEFENSE:
			equipment_attr_type = EQUIP_ATTR_IDX_MAGIC_DEFENSE;
			break;
		}

	return equipment_attr_type;
}

bool Equipment::IsSingleHandWeapon() const
{
	return true; // 默认所有都是单手武器
}

bool Equipment::IsWeapon() const
{
	return E_TYPE_WEAPON == m_equipment_type;
}

bool Equipment::IsShield() const
{
	return E_TYPE_SHIELD == m_equipment_type;
}