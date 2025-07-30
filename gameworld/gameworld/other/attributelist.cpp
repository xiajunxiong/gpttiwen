#include "attributelist.hpp"

#include "servercommon/struct/battle/battle_special_effect_def.hpp"
#include "servercommon/serverconfig/battleconfig/battle_special_effect_pool.hpp"

void GetSpecialEffectAttr(std::vector<int> sp_list, AttributeList & attr, const AttributeList & base_attr_list, int role_level, int level_by_type, AttributeList * base_point_list)
{
	for (int i = 0; i < (int)sp_list.size(); ++i)
	{
		GetSpecialEffectAttr(sp_list[i], attr, base_attr_list, role_level, level_by_type, base_point_list);
	}
}

void GetSpecialEffectAttr(int sp_id, AttributeList & attr, const AttributeList & base_attr_list, int role_level, int level_by_type, AttributeList * base_point_list)
{
	const SpecialEffectCfg * effect_cfg = BattleSpecialEffectPool::GetInstance().GetSpecialEffectCfg(sp_id);
	if (NULL == effect_cfg)
	{
		return;
	}
	AttributeList sp_attr;
	for (int k = 0; k < effect_cfg->effect_num && k < EFFECT_DETAIL_NUM; ++k)
	{
		const SpecialEffectKey & key = effect_cfg->effect_detail_list[k];
		if (key.effect_type != SPECIAL_EFFECT_TYPE_ADD_ATTR) continue;

		const EffectDetailCfg * detail_cfg = BattleSpecialEffectPool::GetInstance().GetEffectDetailCfg(key);
		if (NULL == detail_cfg) continue;

		if (SPE_ATTR_ADD_ATTR == detail_cfg->effect_sub_type && detail_cfg->param_num >= 2
			&& detail_cfg->param_list[0] >= BATTLE_ATTR_MIN && detail_cfg->param_list[0] < BATTLE_ATTR_MAX)
		{
			sp_attr.m_attrs[detail_cfg->param_list[0]] += detail_cfg->param_list[1];
		}

		if (SPE_ATTR_ADD_ATTR_BY_LEVEL == detail_cfg->effect_sub_type && detail_cfg->param_num >= 2
			&& detail_cfg->param_list[0] >= BATTLE_ATTR_MIN && detail_cfg->param_list[0] < BATTLE_ATTR_MAX)
		{
			BattleAttr add_attr = static_cast<BattleAttr>(ceil(role_level * (double)detail_cfg->param_list[1] / BATTLE_PERCENT_NUM));
			if (detail_cfg->param_num >= 3)
			{
				add_attr += detail_cfg->param_list[2];
			}
			sp_attr.m_attrs[detail_cfg->param_list[0]] += add_attr;
		}

		if (SPE_ATTR_ADD_ATTR_BY_PET_LEVEL == detail_cfg->effect_sub_type && detail_cfg->param_num >= 3
			&& detail_cfg->param_list[0] >= BATTLE_ATTR_MIN && detail_cfg->param_list[0] < BATTLE_ATTR_MAX)
		{
			BattleAttr add_attr = static_cast<BattleAttr>(ceil(level_by_type * (double)detail_cfg->param_list[1] / BATTLE_PERCENT_NUM) + detail_cfg->param_list[2]);
			sp_attr.m_attrs[detail_cfg->param_list[0]] += add_attr;
		}

		if (SPE_ATTR_ADD_ATTR_BY_PARTNER_LEVEL == detail_cfg->effect_sub_type && detail_cfg->param_num >= 2
			&& detail_cfg->param_list[0] >= BATTLE_ATTR_MIN && detail_cfg->param_list[0] < BATTLE_ATTR_MAX)
		{
			BattleAttr add_attr = static_cast<BattleAttr>(ceil(level_by_type * (double)detail_cfg->param_list[1] / BATTLE_PERCENT_NUM));
			if (detail_cfg->param_num >= 3)
			{
				add_attr += detail_cfg->param_list[2];
			}
			sp_attr.m_attrs[detail_cfg->param_list[0]] += add_attr;
		}

		if (SPE_ATTR_ADD_ATTR_BY_HUANSHOU_LEVEL == detail_cfg->effect_sub_type && detail_cfg->param_num >= 3
			&& detail_cfg->param_list[0] >= BATTLE_ATTR_MIN && detail_cfg->param_list[0] < BATTLE_ATTR_MAX)
		{
			BattleAttr add_attr = static_cast<BattleAttr>(ceil(level_by_type * (double)detail_cfg->param_list[1] / BATTLE_PERCENT_NUM) + detail_cfg->param_list[2]);
			sp_attr.m_attrs[detail_cfg->param_list[0]] += add_attr;
		}

		if (SPE_ATTR_ADD_ATTR_BY_ANOTHER_ATTR == detail_cfg->effect_sub_type && detail_cfg->param_num >= 3
			&& detail_cfg->param_list[0] >= BATTLE_ATTR_MIN && detail_cfg->param_list[0] < BATTLE_ATTR_MAX
			&& detail_cfg->param_list[1] >= BATTLE_ATTR_MIN && detail_cfg->param_list[1] < BATTLE_ATTR_MAX
			&& detail_cfg->param_list[2] > 0)
		{
			int attr_type0 = detail_cfg->param_list[0];
			int attr_type1 = detail_cfg->param_list[1];
			sp_attr.m_attrs[attr_type0] += static_cast<int>(1.0 * base_attr_list.m_attrs[attr_type1] / (1.0 * detail_cfg->param_list[2] / BATTLE_PERCENT_NUM));
		}
	}

	attr.Add(sp_attr.m_attrs);
	if (NULL != base_point_list)
	{
		base_point_list->Add(sp_attr.m_attrs);
	}
}

