#include "holyequipmanager.hpp"

#include "other/shengqi/holyequipmentconfig.hpp"
#include "gamelog.h"
#include "servercommon/uniqueidgenerator.hpp"

#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"

#include "item/itemgriddata.h"

#include "item/shengqi/holyitem.h"
#include "item/shengqi/runestoneitem.h"
#include "config/logicconfigmanager.hpp"

#include "other/funopen/funopendef.hpp"
#include "other/funopen/funopen.hpp"
#include "other/flyupsystem/flyupsystem.hpp"
#include "servercommon/errornum.h"
#include "other/collection/collection.hpp"
#include "servercommon/string/gameworldstr.h"
#include "other/route/mailroute.hpp"
#include "other/event/eventhandler.hpp"
#include "servercommon/serverconfig/battleconfig/battle_hallow_gift_pool.hpp"
#include "other/pet/pet.hpp"
#include "other/roleattribute/roleattributeplan.hpp"
#include "servercommon/servercommon.h"

ItemGridData * HolyEquipManager::GetHolyBagNode(int bag_idx)
{
	if (bag_idx < 0 || bag_idx >= ItemNamespace::MAX_HOLY_EQUIP_BAG_NUM)return NULL;
	return m_holy_grid_list[bag_idx];
}

ItemGridData * HolyEquipManager::GetFuShiBagNode(int bag_idx)
{
	if (bag_idx < 0 || bag_idx >= ItemNamespace::MAX_RUNE_STONE_BAG_NUM)return NULL;
	return m_fushi_grid_list[bag_idx];
}

HolyEquipManager::HolyEquipManager():m_role_module_mgr(NULL), m_holy_bag_is_change(false), m_fushi_bag_is_change(false)
{
	for (int i = 0; i < ARRAY_LENGTH(m_attrs_add); i++)
	{
		m_attrs_add[i].Reset();
	}

	m_holy_grid_num = 0;
	for (int i = 0; i < ItemNamespace::MAX_HOLY_EQUIP_BAG_NUM; ++i)
	{
		m_holy_old_state[i] = false;
		m_holy_dirty_mark[i] = false;
		m_holy_grid_list[i] = NULL;
	}

	m_fushi_grid_num = 0;
	for (int i = 0; i < ItemNamespace::MAX_RUNE_STONE_BAG_NUM; ++i)
	{
		m_fushi_old_state[i] = false;
		m_fushi_dirty_mark[i] = false;
		m_fushi_grid_list[i] = NULL;
	}
}

HolyEquipManager::~HolyEquipManager()
{
	for (int i = 0; i < ItemNamespace::MAX_HOLY_EQUIP_BAG_NUM; ++i)
	{
		if (NULL != m_holy_grid_list[i])
		{
			this->ClearHolyGrid(i);
		}
	}

	for (int i = 0; i < ItemNamespace::MAX_RUNE_STONE_BAG_NUM; ++i)
	{
		if (NULL != m_fushi_grid_list[i])
		{
			this->ClearFuShiGrid(i);
		}
	}
}

void HolyEquipManager::Init(RoleModuleManager * mgr, const HolyEquipCommonParam & param, 
	int shengqi_param_list_num, const ItemListParam::ItemListData * shengqi_data_list[],
	int fushi_param_list_num, const ItemListParam::ItemListData * fushi_data_list[])
{
	m_role_module_mgr = mgr;
	m_param = param;

	// 圣器
	for (int i = 0; i < shengqi_param_list_num; i++)
	{
		if (NULL == shengqi_data_list[i]) continue;
		if (shengqi_data_list[i]->index >= ItemNamespace::HOLY_EQUIP_BAG_INDEX_BEGIN && shengqi_data_list[i]->index < ItemNamespace::HOLY_EQUIP_BAG_INDEX_END)
		{
			int equip_index = shengqi_data_list[i]->index - ItemNamespace::HOLY_EQUIP_BAG_INDEX_BEGIN; // index经过转换则为背包下表
			if (equip_index < 0 || equip_index >= ItemNamespace::MAX_HOLY_EQUIP_BAG_NUM) continue;

			if (NULL == m_holy_grid_list[equip_index]) m_holy_grid_list[equip_index] = new ItemGridData();
			m_holy_grid_list[equip_index]->Set(shengqi_data_list[i]->item_wrapper);
			m_holy_old_state[equip_index] = true;
			if (equip_index >= m_holy_grid_num)m_holy_grid_num = equip_index + 1;
		}
	}

	// 符石
	for (int i = 0; i < fushi_param_list_num; i++)
	{
		if (NULL == fushi_data_list[i]) continue;
		if (fushi_data_list[i]->index >= ItemNamespace::RUNE_STONE_BAG_INDEX_BEGIN && fushi_data_list[i]->index < ItemNamespace::RUNE_STONE_BAG_INDEX_END)
		{
			int fushi_index = fushi_data_list[i]->index - ItemNamespace::RUNE_STONE_BAG_INDEX_BEGIN; // index经过转换则为背包下表
			if(fushi_index < 0 || fushi_index >= ItemNamespace::MAX_RUNE_STONE_BAG_NUM) continue;

			if (NULL == m_fushi_grid_list[fushi_index]) m_fushi_grid_list[fushi_index] = new ItemGridData();
			m_fushi_grid_list[fushi_index]->Set(fushi_data_list[i]->item_wrapper);
			m_fushi_old_state[fushi_index] = true;
			if (fushi_index >= m_fushi_grid_num)m_fushi_grid_num = fushi_index + 1;
		}
	}
}

void HolyEquipManager::GetInitParam(HolyEquipCommonParam * param)
{
	*param = m_param;
}

void HolyEquipManager::GetChangeHolyEquipBagList(int * count, ItemListParam::ItemListData * save_list)
{
	int tmp_count = 0;

	for (int index = 0; index < ARRAY_LENGTH(m_holy_grid_list); ++index)
	{
		if (m_holy_dirty_mark[index])
		{
			if (NULL == m_holy_grid_list[index] || m_holy_grid_list[index]->Invalid())
			{
				if (!m_holy_old_state[index]) continue;

				save_list[tmp_count].change_state_item = structcommon::CS_DELETE;

				save_list[tmp_count].index = index + ItemNamespace::HOLY_EQUIP_BAG_INDEX_BEGIN;
				save_list[tmp_count].item_wrapper.Reset();
				++tmp_count;

				gamelog::g_sheng_qi_system.printf(LL_INFO, "%s CS_DELETE role[%d,%s] item from index[%d]", __FUNCTION__,
					m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), index);
			}
			else
			{
				if (m_holy_old_state[index])
				{
					save_list[tmp_count].change_state_item = structcommon::CS_UPDATE;

					gamelog::g_sheng_qi_system.printf(LL_INFO, "%s CS_UPDATE role[%d,%s] item_id[%d] num[%d] from index[%d]", __FUNCTION__,
						m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_holy_grid_list[index]->item_id, 1, index);
				}
				else
				{
					save_list[tmp_count].change_state_item = structcommon::CS_INSERT;

					gamelog::g_sheng_qi_system.printf(LL_INFO, "%s CS_INSERT role[%d,%s] item_id[%d] num[%d] index[%d]", __FUNCTION__,
						m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_holy_grid_list[index]->item_id, 1, index);
				}

				save_list[tmp_count].index = index + ItemNamespace::HOLY_EQUIP_BAG_INDEX_BEGIN;
				m_holy_grid_list[index]->WrapTo(&save_list[tmp_count].item_wrapper);
				++tmp_count;

				gamelog::g_sheng_qi_system.printf(LL_INFO, "%s role[%d, %s] index[%d] item_id[%d] param[%s]", __FUNCTION__, m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(),
					index, m_holy_grid_list[index]->item_id, m_holy_grid_list[index]->param->GetParamLog(I_TYPE_HOLY_EQUIP));
			}
		}
	}

	*count = tmp_count;
}

void HolyEquipManager::GetChangeFuShiBagList(int * count, ItemListParam::ItemListData * save_list)
{
	int tmp_count = 0;

	for (int i = 0; i < ARRAY_LENGTH(m_fushi_grid_list); ++i)
	{
		if (m_fushi_dirty_mark[i])
		{
			if (NULL == m_fushi_grid_list[i] || m_fushi_grid_list[i]->Invalid())
			{
				if (!m_fushi_old_state[i]) continue;

				save_list[tmp_count].change_state_item = structcommon::CS_DELETE;

				save_list[tmp_count].index = i + ItemNamespace::RUNE_STONE_BAG_INDEX_BEGIN;
				save_list[tmp_count].item_wrapper.Reset();
				++tmp_count;

				gamelog::g_sheng_qi_system.printf(LL_INFO, "%s CS_DELETE role[%d,%s] item from index[%d]", __FUNCTION__,
					m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), i);
			}
			else
			{
				if (m_fushi_old_state[i])
				{
					save_list[tmp_count].change_state_item = structcommon::CS_UPDATE;

					gamelog::g_sheng_qi_system.printf(LL_INFO, "%s CS_UPDATE role[%d,%s] item_id[%d] num[%d] from index[%d]", __FUNCTION__,
						m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_fushi_grid_list[i]->item_id, 1, i);
				}
				else
				{
					save_list[tmp_count].change_state_item = structcommon::CS_INSERT;

					gamelog::g_sheng_qi_system.printf(LL_INFO, "%s CS_INSERT role[%d,%s] item_id[%d] num[%d] index[%d]", __FUNCTION__,
						m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_fushi_grid_list[i]->item_id, 1, i);
				}

				save_list[tmp_count].index = i + ItemNamespace::RUNE_STONE_BAG_INDEX_BEGIN;
				m_fushi_grid_list[i]->WrapTo(&save_list[tmp_count].item_wrapper);
				++tmp_count;
			}
		}
	}

	*count = tmp_count;
}

void HolyEquipManager::GetHolyEquipList(int * count, QueryItemListParam::ItemListData * save_list)
{
	int tmp_count = 0;

	for (int i = 0; i < HOLY_EQUIP_STATUS_MAX; ++i)
	{
		int index = m_role_module_mgr->GetCommonData().holy_equip_status_index[i];
		if (index < 0 || index >= ItemNamespace::MAX_HOLY_EQUIP_BAG_NUM) continue;
		if (NULL == m_holy_grid_list[index] || m_holy_grid_list[index]->Invalid() || NULL == m_holy_grid_list[index]->param) continue;

		save_list[tmp_count].index = index + ItemNamespace::HOLY_EQUIP_BAG_INDEX_BEGIN;
		m_holy_grid_list[index]->WrapTo(&save_list[tmp_count].item_wrapper);

		save_list[tmp_count].change_state_item = structcommon::CS_NONE;

// 		gamelog::g_sheng_qi_system.printf(LL_INFO, "%s role[%d, %s] index[%d] item_id[%d] param[%s]", __FUNCTION__, m_role_module_mgr->GetUid(),m_role_module_mgr->GetRoleName(),
// 			index, m_holy_grid_list[index]->item_id, m_holy_grid_list[index]->param->GetParamLog(I_TYPE_HOLY_EQUIP));
		if (++tmp_count >= *count) break;
	}

	*count = tmp_count;
}

void HolyEquipManager::ClearHolyDirtyMark()
{
	if (!m_holy_bag_is_change) return;

	for (int i = 0; i < ItemNamespace::MAX_HOLY_EQUIP_BAG_NUM; ++i)
	{
		if (m_holy_dirty_mark[i])
		{
			if (NULL == m_holy_grid_list[i] || m_holy_grid_list[i]->Invalid())
			{
				m_holy_old_state[i] = false;
			}
			else
			{
				m_holy_old_state[i] = true;
			}
		}
	}

	memset(m_holy_dirty_mark, 0, sizeof(m_holy_dirty_mark));

	m_holy_bag_is_change = false;
}

void HolyEquipManager::ClearFuShiDirtyMark()
{
	if (!m_fushi_bag_is_change) return;

	for (int i = 0; i < ItemNamespace::MAX_RUNE_STONE_BAG_NUM; ++i)
	{
		if (m_fushi_dirty_mark[i])
		{
			if (NULL == m_fushi_grid_list[i] || m_fushi_grid_list[i]->Invalid())
			{
				m_fushi_old_state[i] = false;
			}
			else
			{
				m_fushi_old_state[i] = true;
			}
		}
	}

	memset(m_fushi_dirty_mark, 0, sizeof(m_fushi_dirty_mark));

	m_fushi_bag_is_change = false;
}

bool HolyEquipManager::GetHolyEquipGridData(short index, ItemDataWrapper & carrier_data)
{
	if (index < 0 || index >= ItemNamespace::MAX_HOLY_EQUIP_BAG_NUM)
	{
		return false;
	}

	if (NULL == m_holy_grid_list[index] || m_holy_grid_list[index]->Invalid()) return false;

	m_holy_grid_list[index]->WrapTo(&carrier_data);
	return true;
}

void HolyEquipManager::ReCalcPetAttr(unsigned long long pet_unique_id, AttributeList & base_add)
{
	this->RefreshPetAttr();

	AttributeList single_attrs;
	for (int i = 0; i < ARRAY_LENGTH(m_attrs_add) && i < HOLY_EQUIP_STATUS_MAX; i++)
	{
		int index = m_role_module_mgr->GetCommonData().holy_equip_status_index[i];
		if (index == -1) continue;

		if (index < 0 || index >= ItemNamespace::MAX_HOLY_EQUIP_BAG_NUM) continue;
		ItemGridData* item_data = m_holy_grid_list[index];
		if (NULL == item_data || NULL == item_data->param) continue;

		if ((unsigned long long)item_data->param->holy_equip_param.pet_unique_id == pet_unique_id)
		{
			single_attrs = m_attrs_add[i];
			break;
		}
	}

	base_add.Add(single_attrs.m_attrs);
}

void HolyEquipManager::RefreshPetAttr()
{
	for (int i = 0; i < ARRAY_LENGTH(m_attrs_add) && i < HOLY_EQUIP_STATUS_MAX; i++)
	{
		m_attrs_add[i].Reset();

		int index = m_role_module_mgr->GetCommonData().holy_equip_status_index[i];
		if (index == -1) continue;

		if (index < 0 || index >= ItemNamespace::MAX_HOLY_EQUIP_BAG_NUM) continue;

		ItemGridData* item_data = m_holy_grid_list[index];
		if (NULL == item_data || NULL == item_data->param) continue;

		for (short j = 0; j < item_data->param->holy_equip_param.valid_extra_attr_num; j++)
		{
			m_attrs_add[i].m_attrs[item_data->param->holy_equip_param.m_extra_attr_list[j].attr_type] = item_data->param->holy_equip_param.m_extra_attr_list[j].attr_value;
		}
	}
}

bool HolyEquipManager::HasBlessPet(int index)
{
	ItemGridData* holy_data = this->GetHolyBagNode(index);
	if (NULL == holy_data || NULL == holy_data->param) return false;

	if (holy_data->param->holy_equip_param.pet_id == 0 || holy_data->param->holy_equip_param.pet_unique_id == 0)
	{
		return false;
	}

	return true;
}

void HolyEquipManager::ReCalHolyBaseAttr(int index, bool is_send)
{
	ItemGridData* holy_grid = this->GetHolyBagNode(index);
	if (NULL == holy_grid || NULL == holy_grid->param) return;

	HolyEquipParam& holy_param = holy_grid->param->holy_equip_param;

	AttrInfo t_attr_list[MAX_HOLY_EQUIP_BASE_VOC_NUM];
	ArrayFillZero(t_attr_list);

	// 初始化加上基础属性
	for (int i = 0; i < ARRAY_LENGTH(holy_param.base_attr_list); i++)
	{
		t_attr_list[i].attr_type = holy_param.base_attr_list[i].attr_type;
		t_attr_list[i].attr_value = holy_param.base_attr_list[i].attr_value;
	}

	// 主动技能附加属性
	for (int i = 0; i < ARRAY_LENGTH(holy_param.active_skills); i++)
	{
		if(holy_param.active_skills[i] <= 0) continue;

		const AttrInfo* attr = LOGIC_CONFIG->GetHolyEquipMentConfig()->GetAttrBySkill(holy_param.active_skills[i]);
		if(NULL ==attr) continue;

		for (int j = 0; j < ARRAY_LENGTH(t_attr_list); j++)
		{
			if (t_attr_list[j].attr_type == attr->attr_type)
			{
				t_attr_list[j].attr_value += attr->attr_value;
				break;
			}
		}
	}

	// 符石附加属性
	for (int hole = 0; hole < ARRAY_LENGTH(holy_param.rune_stone_list); hole++)
	{
		if (holy_param.rune_stone_hole[hole] != HolyEquipParam::SKILL_HOLE_STATUS_HAVE_VALUE) continue;

		for (int j = 0; j < holy_param.rune_stone_list[hole].attr_num; j++)
		{
			t_attr_list[holy_param.rune_stone_list[hole].voc_list[j].attr_type].attr_value += holy_param.rune_stone_list[hole].voc_list[j].attr_value;
		}
	}

	bool has_plan_attr = false;		// 当is_send与has_plan_attr都为true时, 发送单条信息
	// 天赋技能附加属性(某些是以百分比形式加的属性,放到靠后位置)
	for (int i = 0; i < ARRAY_LENGTH(holy_param.talent_skills); i++)
	{
		if (holy_param.talent_skills[i] <= 0) continue;

		const HallowGiftCfg* gift_cfg = BattleHallowGiftPool::Instance().GetGiftCfg(holy_param.talent_skills[i]);
		if (NULL == gift_cfg) continue;

		AttrInfo t_talent_attr_list[MAX_HOLY_EQUIP_BASE_VOC_NUM];
		ArrayFillZero(t_talent_attr_list);

		for (size_t j = 0; j < gift_cfg->gift_effects.size(); j++)
		{
			const HallowGiftEffectCfg* gift_effect_cfg = BattleHallowGiftPool::Instance().GetGiftEffectCfg(gift_cfg->gift_effects[j]);
			if (NULL == gift_effect_cfg) continue;

			const HallowGiftEffectKey& key = gift_cfg->gift_effects[j];
			if (key.effect_class != HALLOW_GIFT_EFFECT_TYPE_ADD_ATTR) continue;

			
			if (gift_effect_cfg->effect_type == HG_ADD_ATTR_PERCENT)
			{
				UNSTD_STATIC_CHECK(HolyItem::HOLY_ATTR_TYPE_MAX == 7);
				switch (gift_effect_cfg->param1)
				{
				case HolyItem::HOLY_ATTR_TYPE_ACTIVENESS:
				case HolyItem::HOLY_ATTR_TYPE_SPEED:
					{
						t_talent_attr_list[gift_effect_cfg->param1].attr_value = (t_attr_list[gift_effect_cfg->param1].attr_value * gift_effect_cfg->param2 / 100);
					}
					break;
				case HolyItem::HOLY_ATTR_TYPE_ADD_ATTACK:
				case HolyItem::HOLY_ATTR_TYPE_RECOVER:
				case HolyItem::HOLY_ATTR_TYPE_HELP:
				case HolyItem::HOLY_ATTR_TYPE_FALL:
				case HolyItem::HOLY_ATTR_TYPE_COPE_FALL:
					{
						t_talent_attr_list[gift_effect_cfg->param1].attr_value = gift_effect_cfg->param2 * 10;
					}
					break;
				}
			}
			else if (gift_effect_cfg->effect_type == HG_ADD_ATTR_BASE_ON_MASTER_ADD_POINT)
			{
				AttributePlan plan;
				m_role_module_mgr->GetRoleAttributePlan()->GetAttributePlan(plan);

				double holy_attr_value = t_attr_list[gift_effect_cfg->param3].attr_value;
				double role_attr_value = plan.add_point_list[gift_effect_cfg->param1];
				double improve_points = gift_effect_cfg->param2;
				double improve_value = gift_effect_cfg->param4;

				double add_num = 0.0;

				UNSTD_STATIC_CHECK(HolyItem::HOLY_ATTR_TYPE_MAX == 7);
				switch (gift_effect_cfg->param3)
				{
				case HolyItem::HOLY_ATTR_TYPE_ACTIVENESS:
				case HolyItem::HOLY_ATTR_TYPE_SPEED:
					{
						add_num = holy_attr_value * ((role_attr_value / improve_points) * (improve_value / 10000.0));
					}
					break;
				case HolyItem::HOLY_ATTR_TYPE_ADD_ATTACK:
				case HolyItem::HOLY_ATTR_TYPE_RECOVER:
				case HolyItem::HOLY_ATTR_TYPE_HELP:
				case HolyItem::HOLY_ATTR_TYPE_FALL:
				case HolyItem::HOLY_ATTR_TYPE_COPE_FALL:
					{
						add_num = ((role_attr_value / improve_points) * (improve_value / 10000.0)) * 1000.0;
					}
					break;
				}

				t_talent_attr_list[gift_effect_cfg->param3].attr_value = (int)add_num;
				has_plan_attr = true;
			}
			else if (gift_effect_cfg->effect_type == HG_ADD_ATTR_VALUE)
			{
				t_talent_attr_list[gift_effect_cfg->param1].attr_value = gift_effect_cfg->param2;
			}
		}

		for (int j = 0; j < ARRAY_LENGTH(t_attr_list); j++)
		{
			t_attr_list[j].attr_value += t_talent_attr_list[j].attr_value;		
		}
	}

	memcpy(holy_param.total_attr_list, t_attr_list, sizeof(holy_param.total_attr_list));

	if (is_send && has_plan_attr)
	{
		this->SetHolyBagDirty(index, true, HOLY_EQUIP_SEND_REASON_MOVE);
	}
}

void HolyEquipManager::ReCalAllHolyBaseAttr()
{
	for (int i = 0; i < ItemNamespace::MAX_HOLY_EQUIP_BAG_NUM; i++)
	{
		this->ReCalHolyBaseAttr(i, true);
	}
}

const AttributeList & HolyEquipManager::ReCalcAttr(AttributeList & base_add, bool is_recalc)
{
	if (is_recalc)
	{
		m_role_attrs_add.Reset();

		for (int i = 0; i < MAX_HOLY_EQUIP_FIGHT_STANDBY_NUM; i++)
		{
			int index = m_role_module_mgr->GetCommonData().holy_equip_status_index[i];

			ItemGridData* holy_grid = this->GetHolyBagNode(index);
			if(NULL == holy_grid || NULL == holy_grid->param) continue;

			for (int j = 0; j < ARRAY_LENGTH(holy_grid->param->holy_equip_param.rune_stone_hole); j++)
			{
				if(holy_grid->param->holy_equip_param.rune_stone_hole[j] != HolyEquipParam::SKILL_HOLE_STATUS_HAVE_VALUE) continue;

				m_role_attrs_add.m_attrs[holy_grid->param->holy_equip_param.rune_stone_list[j].role_attr.attr_type] += holy_grid->param->holy_equip_param.rune_stone_list[j].role_attr.attr_value;
			}
		}
	}

	base_add.Add(m_role_attrs_add.m_attrs);
	return m_role_attrs_add;
}

void HolyEquipManager::ChangeProfession(int old_prof, int new_prof)
{
	for (int index = 0; index < ItemNamespace::MAX_HOLY_EQUIP_BAG_NUM; index++)
	{
		ItemGridData* holy_grid = this->GetHolyBagNode(index);
		if(NULL == holy_grid || NULL == holy_grid->param) continue;

		for (int j = 0; j < ARRAY_LENGTH(holy_grid->param->holy_equip_param.rune_stone_hole); j++)
		{
			if (holy_grid->param->holy_equip_param.rune_stone_hole[j] != HolyEquipParam::SKILL_HOLE_STATUS_HAVE_VALUE) continue;

			const ItemBase * fushi_base = ITEMPOOL->GetItem(holy_grid->param->holy_equip_param.rune_stone_list[j].item_id);
			if (fushi_base == NULL) continue;
			const RuneStoneItem * op_fushi_item = (const RuneStoneItem *)fushi_base;

			const AttrInfo * attr_cfg = LOGIC_CONFIG->GetHolyEquipMentConfig()->GetRoleAttr(m_role_module_mgr->GetRole(), op_fushi_item->GetItemId(), op_fushi_item->GetFuShiType());
			if(NULL == attr_cfg) continue;

			holy_grid->param->holy_equip_param.rune_stone_list[j].role_attr = *attr_cfg;
		}
	}

	for (size_t index = 0; index < (unsigned int)ItemNamespace::MAX_RUNE_STONE_BAG_NUM; index++)
	{
		ItemGridData* fushi_grid = this->GetFuShiBagNode(index);
		if (NULL == fushi_grid || NULL == fushi_grid->param) continue;

		const ItemBase * fushi_base = ITEMPOOL->GetItem(fushi_grid->item_id);
		if (fushi_base == NULL) continue;
		const RuneStoneItem * op_fushi_item = (const RuneStoneItem *)fushi_base;

		const AttrInfo * attr_cfg = LOGIC_CONFIG->GetHolyEquipMentConfig()->GetRoleAttr(m_role_module_mgr->GetRole(), op_fushi_item->GetItemId(), op_fushi_item->GetFuShiType());
		if (NULL == attr_cfg) continue;

		fushi_grid->param->rune_stone_param.role_attr = *attr_cfg;
	}

	this->SendAllInfo();
}

bool IsValidAttrType(int type)
{
	return type >= 0 && type < MAX_HOLY_EQUIP_BASE_VOC_NUM;
}

void HolyEquipManager::ClientReq(Protocol::CSHolyEquipOpReq * msg)
{
	if (NULL == msg) return;

	if (!FunOpen::IsFunOpen(m_role_module_mgr->GetRole(), FUN_TYPE_SHENG_QI, true)) return;

	// 飞升阶段判定
	if (m_role_module_mgr->GetFlyUpSystem()->GetFlyUpFlag() < LOGIC_CONFIG->GetHolyEquipMentConfig()->GetOtherCfg().feisheng_type)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_NEED_ROLE_FEI_SHENG);
		return;
	}

	switch (msg->op_type)
	{
	case Protocol::CSHolyEquipOpReq::OP_TYPE_INFO_REQ:
		{
		}
		break;
	case Protocol::CSHolyEquipOpReq::OP_TYPE_PUT_ON:
		{
			this->SetHolyEquipStatus(msg->param1, msg->param2);
		}
		break;
	case Protocol::CSHolyEquipOpReq::OP_TYPE_UNLOCK_HOLE:
		{
			this->UnlockSkillHole(msg->param1, msg->param2);
		}
		break;
	case Protocol::CSHolyEquipOpReq::OP_TYPE_STUDY_NEW_SKILL:
		{
			this->StudySkill(msg->param1, msg->param2);
		}
		break;
	case Protocol::CSHolyEquipOpReq::OP_TYPE_SAVE_NEW_SKILL:
		{
			this->SaveSkill(msg->param1, msg->param2);
		}
		break;
	case Protocol::CSHolyEquipOpReq::OP_TYPE_INLAY_FU_SHI:
		{
			this->PutOnFuShi(msg->param1, msg->param2, msg->param3);
		}
		break;
	case Protocol::CSHolyEquipOpReq::OP_TYPE_TAKE_OFF_FU_SHI:
		{
			this->TakeOffFuShi(msg->param1, msg->param2);
		}
		break;
	case Protocol::CSHolyEquipOpReq::OP_TYPE_FU_SHI_FUSION:
		{
			this->FusionFuShi(msg->param1, msg->param2, msg->param3, msg->param4, msg->param5, msg->param6);
		}
		break;
	case Protocol::CSHolyEquipOpReq::OP_TYPE_FU_SHI_RECAST:
		{
			this->RecastFuShi(msg->param1, msg->param2);
		}
		break;
	case Protocol::CSHolyEquipOpReq::OP_TYPE_UNLOCK_FUSHI_HOLE:
		{
			this->UnlockFuShiHole(msg->param1, msg->param2);
		}
		break;
	case Protocol::CSHolyEquipOpReq::OP_TYPE_FU_SHI_RECAST_ON_HOLY_EQUIP:
		{
			this->RecastFuShiOnHolyEquip(msg->param1, msg->param2, msg->param3);
		}
		break;
	case Protocol::CSHolyEquipOpReq::OP_TYPE_HOLY_BLESS_PET:
		{
			this->BlessPet(msg->param1, msg->param2);
		}
		break;
	case Protocol::CSHolyEquipOpReq::OP_TYPE_HOLY_FOLLOW:
		{
			this->HolyFollow(msg->param1);
		}
		break;
	case Protocol::CSHolyEquipOpReq::OP_TYPE_HOLY_SKILL_CONVERT:
		{
			this->HallowsSkillConvert(msg->param1, msg->param2);
		}
		break;
	default:
		{
			gamelog::g_sheng_qi_system.printf(LL_INFO, "%s role[%d, %s] op_type[%d] param[%d, %d, %d, %d, %d]  Invalid Operation", __FUNCTION__,
				m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), msg->op_type, msg->param1, msg->param2, msg->param3, msg->param4, msg->param5);
		}
		break;
	}
}

void HolyEquipManager::OnRoleLogin()
{
	for (int index = 0; index < ARRAY_LENGTH(m_holy_grid_list); index++)
	{
		ItemGridData * grid_data = this->GetHolyBagNode(index);
		if(NULL == grid_data || NULL == grid_data->param || grid_data->Invalid()) continue;

		// 顺便把圣器收录问题的修复也放这里,触发一下收录检查
		EventHandler::Instance().OnGetHolyEquip(m_role_module_mgr->GetRole(), grid_data->item_id);

		if (grid_data->param->holy_equip_param.has_net_value != 0 && grid_data->param->holy_equip_param.level <= 0)
		{
			const ItemBase * new_item = ITEMPOOL->GetItem(grid_data->item_id);
			if (NULL == new_item) continue;
			const HolyItem * ib = (const HolyItem *)new_item;
			if (new_item->GetItemType() != I_TYPE_HOLY_EQUIP) continue;

			static NetValueItemParam p_nvip; p_nvip.Reset();
			ib->RandomNetValueOnPutItem(m_role_module_mgr->GetRole(), &p_nvip, 0);
			grid_data->param->holy_equip_param = p_nvip.holy_equip_param;
		}

		this->ReCalHolyBaseAttr(index);
		this->SetHolyBagDirty(index, true);
	}

	{	// 检查状态数组是否正确
		std::vector<int> temp_vec;
		for (int i = 0; i < ARRAY_LENGTH(m_role_module_mgr->GetCommonData().holy_equip_status_index); i++)
		{
			if (m_role_module_mgr->GetCommonData().holy_equip_status_index[i] != -1)
			{
				ItemGridData * grid_data = this->GetHolyBagNode(m_role_module_mgr->GetCommonData().holy_equip_status_index[i]);
				if (NULL == grid_data || NULL == grid_data->param || grid_data->param->holy_equip_param.holy_status == HolyEquipParam::HOLY_EQUIP_STATUS_REST ||
					std::find(temp_vec.begin(), temp_vec.end(), m_role_module_mgr->GetCommonData().holy_equip_status_index[i]) != temp_vec.end())
				{
					m_role_module_mgr->GetCommonData().holy_equip_status_index[i] = -1;
				}
				else
				{
					temp_vec.push_back(m_role_module_mgr->GetCommonData().holy_equip_status_index[i]);
				}
			}
		}
	}

	this->RefreshPetAttr();
	this->SendAllInfo();
	this->SendHolyFollowInfo();
}

const ItemGridData * HolyEquipManager::GetFightHolyEquip(int * out_index)
{
	return this->MutableFightHolyEquip(out_index);
}

int HolyEquipManager::GetEmptyStandbytStatus()
{
	for (int i = 0; i < HOLY_EQUIP_STATUS_MAX; i++)
	{
		if (m_role_module_mgr->GetCommonData().holy_equip_status_index[i] == -1)
		{
			return i;
		}
	}
	return -1;
}

int HolyEquipManager::FindStatusByIndex(int index)
{
	if (index < 0 || index >= ItemNamespace::MAX_HOLY_EQUIP_BAG_NUM) return -111;
	if (NULL == m_holy_grid_list[index] || m_holy_grid_list[index]->Invalid()) return -222;
	ItemGridData* item_data = m_holy_grid_list[index];
	if (NULL == item_data || NULL == item_data->param) return -333;

	return item_data->param->holy_equip_param.holy_status;
}

const ItemGridData * HolyEquipManager::GetHolyEquipByIndex(int index)
{
	return this->GetHolyBagNode(index);
}

void HolyEquipManager::HolyItemLevelUp(Protocol::CSHolyEquipLevelUpReq * req)
{
	if (req->holy_equip < 0 || req->holy_equip >= ItemNamespace::MAX_HOLY_EQUIP_BAG_NUM) return;
	if (NULL == m_holy_grid_list[req->holy_equip] || m_holy_grid_list[req->holy_equip]->Invalid()) return;
	ItemGridData* item_data = m_holy_grid_list[req->holy_equip];
	if (NULL == item_data || NULL == item_data->param) return;

	if (item_data->param->holy_equip_param.level >= (unsigned int)HolyItem::MAX_HOLY_ITEM_LEVEL)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_CURR_LEVEL_LIMIT);
		return;
	}
	const ItemBase * item_base = ITEMPOOL->GetItem(item_data->item_id);
	if (item_base == NULL) return;
	const HolyItem * holy_item = (const HolyItem *)item_base;

	std::pair<ItemID, int> consume_list[MAX_HOLY_LEVEL_UP_ITEM_TYPE];
	int jinghua_add_exp = 0;
	for (int i = 0; i < req->material_type_num && i < MAX_HOLY_LEVEL_UP_ITEM_TYPE; i++)
	{
		int exp_cfg = LOGIC_CONFIG->GetHolyEquipMentConfig()->GetItemExpCfg(req->material_type[i]);
		if (exp_cfg < 0)
		{
			m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
			return;
		}

		if (req->material_num[i] == 0) return;

		consume_list[i] = std::make_pair(req->material_type[i], req->material_num[i]);
		if (m_role_module_mgr->GetKnapsack()->Count(consume_list[i].first) < consume_list[i].second)
		{
			m_role_module_mgr->NoticeNum(errornum::EN_ITEM_AMOUNT_NOT_ENOUGH);
			return;
		}
		jinghua_add_exp += (exp_cfg*req->material_num[i]);
	}

	int holy_fenjie_exp = 0;
	std::vector<int> holy_index_list;
	for (int i = 0; i < req->holy_index_num && i < MAX_HOLY_LEVEL_UP_INDEX_NUM; i++)
	{
		if (req->holy_index[i] < 0 || req->holy_index[i] >= ItemNamespace::MAX_HOLY_EQUIP_BAG_NUM) return;
		if (NULL == m_holy_grid_list[req->holy_index[i]] || m_holy_grid_list[req->holy_index[i]]->Invalid()) return;
		if (NULL == m_holy_grid_list[req->holy_index[i]]->param) return;

		if (m_holy_grid_list[req->holy_index[i]]->param->holy_equip_param.holy_status != HolyEquipParam::HOLY_EQUIP_STATUS_REST)
		{
			m_role_module_mgr->NoticeNum(errornum::EN_HOLY_EQUIP_CAN_NOT_ADD_FINGHT_STANDBY);
			return;
		}

		// 去重,以及以免有要升级的圣器被放进去了
		if (std::find(holy_index_list.begin(), holy_index_list.end(), req->holy_index[i]) != holy_index_list.end() || req->holy_equip == req->holy_index[i])
		{
			continue;
		}
		holy_index_list.push_back(req->holy_index[i]);

		int base_exp = LOGIC_CONFIG->GetHolyEquipMentConfig()->GetHolyEquipExp(m_holy_grid_list[req->holy_index[i]]->item_id);
		if (base_exp < 0) return;

		holy_fenjie_exp += (base_exp + m_holy_grid_list[req->holy_index[i]]->param->holy_equip_param.exp);
	}

	std::string log_str;
	int old_level = item_data->param->holy_equip_param.level;

	// 获取最高等级所需的经验
	const HolyEquipLevelUpAttrCfg* level_cfg = LOGIC_CONFIG->GetHolyEquipMentConfig()->GetHolyLevelCfg(holy_item->GetColor(), HolyItem::MAX_HOLY_ITEM_LEVEL);
	if (NULL == level_cfg) return;

	if (jinghua_add_exp > 0)
	{
		std::map<int, int> consume_map;
		for (int i = 0; i < req->material_type_num && i < MAX_HOLY_LEVEL_UP_ITEM_TYPE; i++)
		{
			std::map<int, int>::iterator it = consume_map.find(consume_list[i].first);
			if (it == consume_map.end())
			{
				consume_map.insert(consume_list[i]);
			}
			else
			{
				it->second += consume_list[i].second;
			}		
		}

		if ((jinghua_add_exp + m_holy_grid_list[req->holy_equip]->param->holy_equip_param.exp) > (unsigned int)level_cfg->upgrade_exp)
		{
			int surplus_exp = jinghua_add_exp + m_holy_grid_list[req->holy_equip]->param->holy_equip_param.exp - level_cfg->upgrade_exp;
			jinghua_add_exp = level_cfg->upgrade_exp - m_holy_grid_list[req->holy_equip]->param->holy_equip_param.exp;

			
			for (std::map<int, int>::iterator it = consume_map.begin(); it != consume_map.end(); it++)
			{
				int exp_cfg = LOGIC_CONFIG->GetHolyEquipMentConfig()->GetItemExpCfg(it->first);
				if (exp_cfg < 0)
				{
					return;
				}

				while (surplus_exp >= exp_cfg)
				{
					if (it->second <= 0) break;

					it->second -= 1;
					surplus_exp -= exp_cfg;
				}
			}

			jinghua_add_exp += surplus_exp;
		}

		if (!m_role_module_mgr->GetKnapsack()->ConsumeItemByMap(consume_map, __FUNCTION__))
		{
			return;
		}

		for (int i = 0; i < req->material_type_num && i < MAX_HOLY_LEVEL_UP_ITEM_TYPE; i++)
		{
			if (consume_list[i].second != 0)
			{
				log_str += STRING_SPRINTF("[%d, %d]", consume_list[i].first, consume_list[i].second);
			}
		}

		if (!this->HolyAddExp(*item_data, jinghua_add_exp))
		{
			gamelog::g_sheng_qi_system.printf(LL_INFO, "%s role[%d, %s] HolyAddExp Fialed!!! index[%d] add_exp[%d] consume_item[%s] param_log[%s]", __FUNCTION__,
				m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), req->holy_equip, jinghua_add_exp, log_str.c_str(), item_data->param->GetParamLog(I_TYPE_HOLY_EQUIP));
			return;
		}
	}

	std::string consume_id_list = "[";
	do
	{
		if (holy_fenjie_exp > 0)
		{
			int count = 0;
			for (int i = 0; i < (int)holy_index_list.size() && i < MAX_HOLY_LEVEL_UP_INDEX_NUM; i++)
			{
				if (item_data->param->holy_equip_param.level >= (unsigned int)HolyItem::MAX_HOLY_ITEM_LEVEL) break;

				int base_exp = LOGIC_CONFIG->GetHolyEquipMentConfig()->GetHolyEquipExp(m_holy_grid_list[holy_index_list[i]]->item_id);
				if (base_exp < 0) continue;
				int holy_add_exp = base_exp + (m_holy_grid_list[holy_index_list[i]]->param->holy_equip_param.exp / HolyEquipOtherCfg::HOLY_EQUIP_EXP_BACK_PROPORTION * LOGIC_CONFIG->GetHolyEquipMentConfig()->GetOtherCfg().exp_back);

				log_str += STRING_SPRINTF("{holy_index[%d] net_value[%s] } ", holy_index_list[i], m_holy_grid_list[holy_index_list[i]]->param->GetParamLog(I_TYPE_HOLY_EQUIP));

				if(!this->IsCanTakeOffAll(holy_index_list[i])) continue;

				int consume_real_id = 0;	//log
				ItemGridData* consume_bag_node = this->GetHolyBagNode(holy_index_list[i]);
				if (NULL != consume_bag_node)
				{
					consume_real_id = consume_bag_node->item_id;;
				}

				if (!this->ConsumeHolyGrid(holy_index_list[i], HOLY_EQUIP_SEND_REASON_HOLY_LVUP, __FUNCTION__, false))
				{
					m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
					gamelog::g_sheng_qi_system.printf(LL_INFO, "%s role[%d, %s] consume holy_equip Failed!!!! index[%d] param[%s]", __FUNCTION__,
						m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), holy_index_list[i], m_holy_grid_list[holy_index_list[i]]->param->GetParamLog(I_TYPE_HOLY_EQUIP));
					continue;
				}

				if (0 != consume_real_id)	//log
				{
					consume_id_list += STRING_SPRINTF("%d ", consume_real_id);
				}

				if (!this->HolyAddExp(*item_data, holy_add_exp))
				{
					break;
				}

				count++;
			}

			{
				Protocol::SCSCHolyEquipLevelUpConsumeInfo msg;
				msg.bag_type = SCSCHolyEquipLevelUpConsumeInfo::BAG_TYPE_HOLY;
				msg.count = count;

				memset(msg.index, 0, sizeof(msg.index));
				for (int i = 0; i < count && i < MAX_HOLY_LEVEL_UP_INDEX_NUM; i++)
				{
					msg.index[i] = holy_index_list[i];
				}

				m_role_module_mgr->NetSend(&msg, sizeof(msg));
			}
		}
	} while (0);

	if (item_data->param->holy_equip_param.level >= (unsigned int)HolyItem::MAX_HOLY_ITEM_LEVEL)
	{
		item_data->param->holy_equip_param.exp = level_cfg->upgrade_exp;
	}

	if ((unsigned int)old_level != item_data->param->holy_equip_param.level && this->HasBlessPet(req->holy_equip))
	{
		int temp_pet_index = -1;
		const PetParam* temp_pet_data = m_role_module_mgr->GetPet()->GetPetParam(item_data->param->holy_equip_param.pet_id, item_data->param->holy_equip_param.pet_unique_id, &temp_pet_index);
		if (NULL != temp_pet_data)
		{
			m_role_module_mgr->GetPet()->ReCalcAttr(temp_pet_index);
			m_role_module_mgr->GetPet()->SendSingleInfo(temp_pet_index, Protocol::SCSendPetSingleInfo::PET_NOTIE_TYPE_HOLY_BLESS);
			m_role_module_mgr->GetPet()->SendPetAttrListReason(temp_pet_index);
		}
	}

	this->SetHolyBagDirty(req->holy_equip, true, HOLY_EQUIP_SEND_REASON_MOVE);

	gamelog::g_sheng_qi_system.printf(LL_INFO, "%s role[%d, %s] item_id[%d] holy_index[%d] holy_net_value[%s] consume_item[%s]", __FUNCTION__, m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(),
		m_holy_grid_list[req->holy_equip]->item_id, req->holy_equip, m_holy_grid_list[req->holy_equip]->param->GetParamLog(I_TYPE_HOLY_EQUIP), log_str.c_str());

	consume_id_list += STRING_SPRINTF("]");
	ROLE_LOG_QUICK10(LOG_TYPE_SHENG_QI_LEVEL_UP, m_role_module_mgr->GetRole(), LOG_REASON_LEVEL_UP, item_base->GetItemId(),
		consume_id_list.c_str(), item_data->param->GetParamLog(I_TYPE_HOLY_EQUIP), old_level, item_data->param->holy_equip_param.level, 0, 0, 0);
}

bool HolyEquipManager::HolyAddExp(ItemGridData& item_data, int add_exp)
{
	if (NULL == item_data.param) return false;
	if (item_data.param->holy_equip_param.level >= (unsigned int)HolyItem::MAX_HOLY_ITEM_LEVEL) return false;

	const ItemBase * item_base = ITEMPOOL->GetItem(item_data.item_id);
	if (item_base == NULL) return false;
	const HolyItem * holy_item = (const HolyItem *)item_base;

	const HolyEquipLevelUpAttrCfg* level_cfg = LOGIC_CONFIG->GetHolyEquipMentConfig()->GetHolyLevelCfg(holy_item->GetColor(), item_data.param->holy_equip_param.level + 1);
	if (NULL == level_cfg) return false;

	unsigned int temp_exp = item_data.param->holy_equip_param.exp + add_exp;

	int level_need_exp = level_cfg->upgrade_exp;

	while(temp_exp >= (unsigned int)level_cfg->upgrade_exp)
	{
		if (!m_role_module_mgr->GetMoney()->UseCoinBind(level_cfg->coin_cost, __FUNCTION__))
		{
			return false;
		}

		item_data.param->holy_equip_param.level++;

		if (item_data.param->holy_equip_param.level >= (unsigned int)HolyItem::MAX_HOLY_ITEM_LEVEL)
		{
			break;
		}

		level_cfg = LOGIC_CONFIG->GetHolyEquipMentConfig()->GetHolyLevelCfg(holy_item->GetColor(), item_data.param->holy_equip_param.level + 1);
		if (NULL == level_cfg)
		{
			item_data.param->holy_equip_param.exp = level_need_exp;
			return false;
		}
		level_need_exp = level_cfg->upgrade_exp;
	}

	item_data.param->holy_equip_param.exp = temp_exp;
	level_cfg = LOGIC_CONFIG->GetHolyEquipMentConfig()->GetHolyLevelCfg(holy_item->GetColor(), item_data.param->holy_equip_param.level);
	if (NULL == level_cfg) return false;

	for (int i = 0; i < item_data.param->holy_equip_param.valid_extra_attr_num && i < MAX_HOLY_EQUIP_EXTRA_VOC_NUM; i++)
	{
		item_data.param->holy_equip_param.m_extra_attr_list[i].attr_value = level_cfg->attr_list[item_data.param->holy_equip_param.m_extra_attr_list[i].attr_type].attr_value;
	}

	item_data.param->holy_equip_param.active_max = level_cfg->active_max;

	return true;
}

void HolyEquipManager::CalculateScore(int index)
{
	if (index < 0 || index >= ItemNamespace::MAX_HOLY_EQUIP_BAG_NUM) return;
	if (NULL == m_holy_grid_list[index] || m_holy_grid_list[index]->Invalid()) return;

	ItemGridData* item_data = m_holy_grid_list[index];
	if (NULL == item_data || NULL == item_data->param) return;

	int score = 0;
	// 总基础属性评分
	for (int i = 0; i < MAX_HOLY_EQUIP_BASE_VOC_NUM; i++)
	{
		score += (item_data->param->holy_equip_param.total_attr_list[i].attr_value * LOGIC_CONFIG->GetScoreSystemConfig()->GetHolyEquipScoreCfg(item_data->param->holy_equip_param.total_attr_list[i].attr_type));
	}

	// 符石带来的属性评分
	for (int i = 0; i < MAX_RUNE_STONE_NUM; i++)
	{
		if (item_data->param->holy_equip_param.rune_stone_hole[i] == HolyEquipParam::SKILL_HOLE_STATUS_LOCK) continue;

		for (short j = 0; j < item_data->param->holy_equip_param.rune_stone_list[i].attr_num; j++)
		{
			score += (item_data->param->holy_equip_param.rune_stone_list[i].voc_list[j].attr_value * LOGIC_CONFIG->GetScoreSystemConfig()->GetHolyEquipScoreCfg(item_data->param->holy_equip_param.rune_stone_list[i].voc_list[j].attr_type));
		}
	}

	// 主动技能评分
	for (int i = 0; i < MAX_ACTIVE_SKILLS_NUM; i++)
	{
		if (item_data->param->holy_equip_param.active_skills[i] <= 0) continue;

		score += LOGIC_CONFIG->GetHolyEquipMentConfig()->GetSkillScoreCfg(item_data->param->holy_equip_param.active_skills[i]);
	}

	// 天赋评分
	for (int i = 0; i < MAX_TALENT_SKILLS_NUM; i++)
	{
		if (item_data->param->holy_equip_param.talent_skills[i] <= 0) continue;

		score += LOGIC_CONFIG->GetHolyEquipMentConfig()->GetSkillScoreCfg(item_data->param->holy_equip_param.talent_skills[i]);
	}

	item_data->param->holy_equip_param.score = score;
}


void HolyEquipManager::SetHolyEquipStatus(int index, int status)
{
	if (index < 0 || index >= ItemNamespace::MAX_HOLY_EQUIP_BAG_NUM) return;

	if (NULL == m_holy_grid_list[index] || m_holy_grid_list[index]->Invalid()) return;

	ItemGridData* item_data = m_holy_grid_list[index];
	if (NULL == item_data) return;

	if (NULL == item_data->param) return;

	int ret = 0;
	switch (status)
	{
	case HolyEquipParam::HOLY_EQUIP_STATUS_REST:
		{
			ret = this->SetRestStatus(index);
		}
		break;
	case HolyEquipParam::HOLY_EQUIP_STATUS_STANDBY:
		{
			ret = this->SetStandbyStatus(index);
		}
		break;
	case HolyEquipParam::HOLY_EQUIP_STATUS_FIGHTING:
		{
			ret = this->SetFightingStatus(index);
		}
		break;
	default:
		{
			m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		}	
		return;
	}

	m_role_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_HOLY_EQUIP, __FUNCTION__);	

	std::string log_str;
	for (int i = 0; i < ARRAY_LENGTH(m_role_module_mgr->GetCommonData().holy_equip_status_index); i++)
	{
		log_str += STRING_SPRINTF("%d ", m_role_module_mgr->GetCommonData().holy_equip_status_index[i]);
	}

	if (ret < 0)
	{
		gamelog::g_sheng_qi_system.printf(LL_INFO, "%s role[%d, %s] index[%d] op_type[%d] operation_failed ret[%d] cur_status_array[%s]", 
			__FUNCTION__, m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), index, status, ret, log_str.c_str());
		return;
	}
	gamelog::g_sheng_qi_system.printf(LL_INFO, "%s role[%d, %s] index[%d] status_change[%d] cur_status_array[%s]", __FUNCTION__,
		m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), index, status, log_str.c_str());

	{
		std::string holy_state_log;
		for (int i = 0; i < ARRAY_LENGTH(m_role_module_mgr->GetCommonData().holy_equip_status_index); i++)
		{
			if (m_role_module_mgr->GetCommonData().holy_equip_status_index[i] == -1) continue;

			const ItemGridData* grid_data = this->GetHolyEquipByIndex(m_role_module_mgr->GetCommonData().holy_equip_status_index[i]);
			if (NULL == grid_data) continue;

			holy_state_log += STRING_SPRINTF("{state:%d item_id:%d fushi:", grid_data->param->holy_equip_param.holy_status, grid_data->item_id);

			for (int j = 0; j < ARRAY_LENGTH(grid_data->param->holy_equip_param.rune_stone_list); j++)
			{
				if(grid_data->param->holy_equip_param.rune_stone_hole[j] != HolyEquipParam::SKILL_HOLE_STATUS_HAVE_VALUE) continue;

				holy_state_log += STRING_SPRINTF("[%d:%d]", j, grid_data->param->holy_equip_param.rune_stone_list[j].item_id);
			}

			holy_state_log += STRING_SPRINTF("}");
		}

		ROLE_LOG_QUICK6(LOG_TYPE_SHENG_QI_STATE_CHANGE, m_role_module_mgr->GetRole(), LOG_REASON_STATE_CHANGE, 0, holy_state_log.c_str(), item_data->param->GetParamLog(I_TYPE_HOLY_EQUIP));
	}
}

int HolyEquipManager::SetFightingStatus(int index)
{
	int holy_status = this->FindStatusByIndex(index);
	if (holy_status < 0) return holy_status;
	if (holy_status == HolyEquipParam::HOLY_EQUIP_STATUS_FIGHTING) return -1;

	int out_index = -1;
	const ItemGridData* fight_holy = this->GetFightHolyEquip(&out_index);
	if (NULL == fight_holy)		// 没有出战圣器, 这种情况 out_index 不可用!
	{
		// 需要判断有没有位置新增出战圣器
		if (holy_status == HolyEquipParam::HOLY_EQUIP_STATUS_REST)
		{
			int free_status = this->GetEmptyStandbytStatus();
			if (free_status < 0) 
			{
				m_role_module_mgr->NoticeNum(errornum::EN_HOLY_EQUIP_STANDBY_MAX);
				return -2;
			}

			m_role_module_mgr->GetCommonData().holy_equip_status_index[free_status] = index;
		}
		//没有出战圣器并且原本是待机状态的圣器只需要改变状态就可以了
	}
	else
	{
		if (out_index < 0) return -3;

		if (holy_status == HolyEquipParam::HOLY_EQUIP_STATUS_REST)
		{
			int state_index = this->GetEmptyStandbytStatus();

			// 有空位时把原出战圣器设为待机,没有就设置为休息
			if (state_index == -1)
			{
				this->StatusChange(m_role_module_mgr->GetCommonData().holy_equip_status_index[out_index], HolyEquipParam::HOLY_EQUIP_STATUS_REST);
				m_role_module_mgr->GetCommonData().holy_equip_status_index[out_index] = index;
			}
			else
			{
				this->StatusChange(m_role_module_mgr->GetCommonData().holy_equip_status_index[out_index], HolyEquipParam::HOLY_EQUIP_STATUS_STANDBY);
				m_role_module_mgr->GetCommonData().holy_equip_status_index[state_index] = index;
			}		
		}
		else if(holy_status == HolyEquipParam::HOLY_EQUIP_STATUS_STANDBY)
		{
			this->StatusChange(m_role_module_mgr->GetCommonData().holy_equip_status_index[out_index], HolyEquipParam::HOLY_EQUIP_STATUS_STANDBY);
		}
	}

	this->StatusChange(index, HolyEquipParam::HOLY_EQUIP_STATUS_FIGHTING);

	return 0;
}

int HolyEquipManager::SetStandbyStatus(int index)
{
	int holy_status = this->FindStatusByIndex(index);
	if (holy_status < 0) return holy_status;
	if (holy_status == HolyEquipParam::HOLY_EQUIP_STATUS_STANDBY) return -1;

	if (holy_status == HolyEquipParam::HOLY_EQUIP_STATUS_REST)
	{
		int free_status = this->GetEmptyStandbytStatus();
		if (free_status < 0)
		{
			m_role_module_mgr->NoticeNum(errornum::EN_HOLY_EQUIP_STANDBY_MAX);
			return -2;
		}

		m_role_module_mgr->GetCommonData().holy_equip_status_index[free_status] = index;
	}

	this->StatusChange(index, HolyEquipParam::HOLY_EQUIP_STATUS_STANDBY);

	return 0;
}

int HolyEquipManager::SetRestStatus(int index)
{
	int holy_status = this->FindStatusByIndex(index);
	if (holy_status < 0) return holy_status;
	if (holy_status == HolyEquipParam::HOLY_EQUIP_STATUS_REST) return -1;

	for (int i = 0; i < ARRAY_LENGTH(m_role_module_mgr->GetCommonData().holy_equip_status_index); i++)
	{
		if (m_role_module_mgr->GetCommonData().holy_equip_status_index[i] == index)
		{
			m_role_module_mgr->GetCommonData().holy_equip_status_index[i] = -1;
			break;
		}
	}

	this->StatusChange(index, HolyEquipParam::HOLY_EQUIP_STATUS_REST);
	
	// 圣器跟随状态取消
	if (index == m_param.follow_index)
	{
		this->HolyFollow();
	}

	return 0;
}

void HolyEquipManager::StatusChange(int index, int status_type)
{
	if (status_type != -1)
	{
		m_holy_grid_list[index]->param->holy_equip_param.holy_status = status_type;
	}

	if (this->HasBlessPet(index) && this->FindStatusByIndex(index) == HolyEquipParam::HOLY_EQUIP_STATUS_REST)
	{
		this->BlessPet(index, -1);
	}

	this->SetHolyBagDirty(index, false, HOLY_EQUIP_SEND_REASON_STATUS);
}

void HolyEquipManager::UnlockSkillHole(int index, int hole)
{
	if (index < 0 || index >= ItemNamespace::MAX_HOLY_EQUIP_BAG_NUM) return;
	if (hole  < 0 || hole  >= MAX_ACTIVE_SKILLS_NUM) return;

	if (NULL == m_holy_grid_list[index] || m_holy_grid_list[index]->Invalid()) return;

	ItemGridData* item_data = m_holy_grid_list[index];
	if (NULL == item_data || NULL == item_data->param) return;

	if (item_data->param->holy_equip_param.active_skills[hole] != HolyEquipParam::SKILL_HOLE_STATUS_LOCK) return;

	const HolyEquipInitiativeSkillHoleCfg * hole_cfg = LOGIC_CONFIG->GetHolyEquipMentConfig()->GetSkillHoleCfg(hole);
	if (NULL == hole_cfg) return;

	const ItemBase * item_base = ITEMPOOL->GetItem(item_data->item_id);
	if (item_base == NULL) return;
	const HolyItem * holy_item = (const HolyItem *)item_base;

	if (holy_item->GetColor() < hole_cfg->shengqi_quality)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_HOLY_EQUIP_QUALITY_TOO_CHEAP);
		return;
	}

	for (int i = 0; i <= hole; i++)
	{
		if (item_data->param->holy_equip_param.active_skills[i] == HolyEquipParam::SKILL_HOLE_STATUS_LOCK)
		{
			if (i != hole)
			{
				m_role_module_mgr->NoticeNum(errornum::EN_HOLY_EQUIP_FORE_SKILL_HOLE_UNLOCK);
				return;
			}
		}
	}

	// 开孔时直接解锁一个技能
	int skill_id = LOGIC_CONFIG->GetHolyEquipMentConfig()->RandHolyItemSkill(item_data->param->holy_equip_param, item_data->item_id, holy_item->GetShengQiType(), holy_item->GetColor(), hole);
	if (skill_id <= 0)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		gamelog::g_sheng_qi_system.printf(LL_INFO, "%s role[%d, %s] RandHolyItemSkill Failed!!! hole[%d] error_num[%d]", __FUNCTION__,
			m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), hole, skill_id);
		return;
	}

	if (m_role_module_mgr->GetKnapsack()->Count(hole_cfg->unlock_cost) < hole_cfg->num)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_ITEM_NOT_ELIGIBLE);
		return;
	}

	if (!m_role_module_mgr->GetKnapsack()->ConsumeItem(hole_cfg->unlock_cost, hole_cfg->num, __FUNCTION__))
	{
		return;
	}
	
	item_data->param->holy_equip_param.active_skills[hole] = skill_id;

	this->ReCalHolyBaseAttr(index);
	this->SetHolyBagDirty((short)index, true, HOLY_EQUIP_SEND_REASON_MOVE);

	gamelog::g_sheng_qi_system.printf(LL_INFO, "%s role[%d, %s] index[%d] open_skill_hole[%d] item_id[%d] holy_param[%s]", 
		__FUNCTION__, m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), index, hole, item_data->item_id, item_data->param->GetParamLog(I_TYPE_HOLY_EQUIP));
}

void HolyEquipManager::StudySkill(int index, int consume_item_id)
{
	if (index < 0 || index >= ItemNamespace::MAX_HOLY_EQUIP_BAG_NUM) return;

	if (NULL == m_holy_grid_list[index] || m_holy_grid_list[index]->Invalid()) return;

	ItemGridData* item_data = m_holy_grid_list[index];
	if (NULL == item_data || NULL == item_data->param) return;

	if (item_data->param->holy_equip_param.unsaved_skill_id[0] != 0)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_HOLY_EQUIP_HAS_NO_SAVE_SKILL);
		return;
	}

	const ItemBase * item_base = ITEMPOOL->GetItem(item_data->item_id);
	if (item_base == NULL) return;	
	const HolyItem * holy_item = (const HolyItem *)item_base;

	int skill_id = LOGIC_CONFIG->GetHolyEquipMentConfig()->GetSkillIdByTypeAndItem(holy_item->GetShengQiType(), consume_item_id);
	if (skill_id <= 0)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		gamelog::g_sheng_qi_system.printf(LL_INFO, "%s role[%d, %s] GetSkillIdByTypeAndItem Failed!!! holy_index[%d] consume_item_id[%d] skill_id[%d]", __FUNCTION__,
			m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), index, consume_item_id, skill_id);
		return;
	}

	int open_skill_hole = 0;
	{
		int out_skill_id = 0, out_skill_lv = 0;
		for (int i = 0; i < ARRAY_LENGTH(item_data->param->holy_equip_param.active_skills); i++)
		{
			if (item_data->param->holy_equip_param.active_skills[i] == HolyEquipParam::SKILL_HOLE_STATUS_LOCK) continue;

			open_skill_hole++;

			if (LOGIC_CONFIG->GetHolyEquipMentConfig()->GetSkillLevel(item_data->param->holy_equip_param.active_skills[i], &out_skill_id, &out_skill_lv))	// 复用out_skill_id, out_skill_lv
			{
				if (out_skill_id == skill_id)
				{
					m_role_module_mgr->NoticeNum(errornum::EN_HOLY_EQUIP_NOT_REPEATABLE_STUDY);
					return;
				}
			}
		}
	}
#ifdef _DEBUG
	assert(open_skill_hole);	// 开孔数量必不可能是0
#endif // _DEBUG

	int hole = RandomNum(open_skill_hole);

 	//消耗
	if (m_role_module_mgr->GetKnapsack()->Count(consume_item_id) < 1)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_ITEM_NOT_ELIGIBLE);
		return;
	}

	if (!m_role_module_mgr->GetKnapsack()->ConsumeItem(consume_item_id, 1, __FUNCTION__))
	{
		return;
	}

	item_data->param->holy_equip_param.unsaved_skill_id[0] = skill_id;
	item_data->param->holy_equip_param.unsaved_skill_id[1] = hole;
	this->SetHolyBagDirty((short)index, false, HOLY_EQUIP_SEND_REASON_SKILL_STUDY);

	gamelog::g_sheng_qi_system.printf(LL_INFO, "%s role[%d, %s] index[%d] study_skill[%d] hole[%d] consume_item_id[%d, %d] holy_param[%s]", __FUNCTION__,
		m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), index, skill_id, hole, consume_item_id, 1, item_data->param->GetParamLog(I_TYPE_HOLY_EQUIP));
}

void HolyEquipManager::SaveSkill(int index, int is_save)
{
	if (index < 0 || index >= ItemNamespace::MAX_HOLY_EQUIP_BAG_NUM) return;

	if (NULL == m_holy_grid_list[index] || m_holy_grid_list[index]->Invalid()) return;

	ItemGridData* item_data = m_holy_grid_list[index];
	if (NULL == item_data || NULL == item_data->param) return;

	if (is_save == 1)
	{
		if (item_data->param->holy_equip_param.unsaved_skill_id[0] <= 0)
		{
			m_role_module_mgr->NoticeNum(errornum::EN_HOLY_EQUIP_NO_SKILLS_TO_SAVE);
			return;
		}

		if (item_data->param->holy_equip_param.active_skills[item_data->param->holy_equip_param.unsaved_skill_id[1]] == HolyEquipParam::SKILL_HOLE_STATUS_LOCK)
		{
			m_role_module_mgr->NoticeNum(errornum::EN_HOLY_EQUIP_SKILL_HOLE_LOCK);
			return;
		}

		if (item_data->param->holy_equip_param.unsaved_skill_id[1] < 0 || item_data->param->holy_equip_param.unsaved_skill_id[1] >= MAX_ACTIVE_SKILLS_NUM)
		{
			return;
		}

		item_data->param->holy_equip_param.active_skills[item_data->param->holy_equip_param.unsaved_skill_id[1]] = item_data->param->holy_equip_param.unsaved_skill_id[0];
	}

	item_data->param->holy_equip_param.unsaved_skill_id[0] = 0;
	item_data->param->holy_equip_param.unsaved_skill_id[1] = 0;

	this->ReCalHolyBaseAttr(index);
	this->SetHolyBagDirty((short)index, true, HOLY_EQUIP_SEND_REASON_SKILL_STUDY);

	gamelog::g_sheng_qi_system.printf(LL_INFO, "%s role[%d, %s] index[%d] is_save[%d] hole_param[%s]", __FUNCTION__,
		m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), index, is_save, item_data->param->GetParamLog(I_TYPE_HOLY_EQUIP));
}

void HolyEquipManager::GmStudySkill(int skill_id, int hole)
{
	if (hole < 0 || hole >= MAX_ACTIVE_SKILLS_NUM) return;

	int out_index_in_bag = -1;
	ItemGridData* fight_holy = this->MutableFightHolyEquip(NULL, &out_index_in_bag);
	if (NULL == fight_holy)
	{
		return;
	}

	fight_holy->param->holy_equip_param.active_skills[hole] = skill_id;
	this->ReCalHolyBaseAttr((short)out_index_in_bag);
	this->SetHolyBagDirty((short)out_index_in_bag, false, HOLY_EQUIP_SEND_REASON_MOVE);

	gamelog::g_sheng_qi_system.printf(LL_INFO, "%s role[%d, %s] index_in_bag[%d] study_skill[%d] hole[%d]", __FUNCTION__,
		m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), out_index_in_bag, skill_id, hole);
}

void HolyEquipManager::GmSetGift(int gift_id, int hole)
{
	if (hole < 0 || hole >= MAX_TALENT_SKILLS_NUM) return;

	int out_index_in_bag = -1;
	ItemGridData* fight_holy = this->MutableFightHolyEquip(NULL, &out_index_in_bag);
	if (NULL == fight_holy)
	{
		return;
	}

	fight_holy->param->holy_equip_param.talent_skills[hole] = gift_id;
	this->ReCalHolyBaseAttr((short)out_index_in_bag);
	this->SetHolyBagDirty((short)out_index_in_bag, false, HOLY_EQUIP_SEND_REASON_MOVE);

	gamelog::g_sheng_qi_system.printf(LL_INFO, "%s role[%d, %s] index_in_bag[%d] gift_id[%d] hole[%d]", __FUNCTION__,
		m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), out_index_in_bag, gift_id, hole);
}

void HolyEquipManager::UnlockFuShiHole(int index, int fushi_hole)
{
	if (index < 0 || index >= ItemNamespace::MAX_HOLY_EQUIP_BAG_NUM) return;
	if (fushi_hole < 0 || fushi_hole >= MAX_RUNE_STONE_NUM) return;

	if (NULL == m_holy_grid_list[index] || m_holy_grid_list[index]->Invalid()) return;

	ItemGridData* item_data = m_holy_grid_list[index];
	if (NULL == item_data || NULL == item_data->param) return;

	if (item_data->param->holy_equip_param.rune_stone_hole[fushi_hole] == HolyEquipParam::SKILL_HOLE_STATUS_UNLOCK) return;

	const RuneStoneHoleCfg* hole_cfg = LOGIC_CONFIG->GetHolyEquipMentConfig()->GetFuShiHoleCfg(fushi_hole);
	if (NULL == hole_cfg) return;

	if (item_data->param->holy_equip_param.level < (unsigned int)hole_cfg->shengqi_level)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_HOLY_EQUIP_LEVEL_NOT_ENOUGH);
		return;
	}

	if (m_role_module_mgr->GetKnapsack()->Count(hole_cfg->unlock_cost) < hole_cfg->consume_num)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_ITEM_NOT_ELIGIBLE);
		return;
	}

	for (int i = 0; i < fushi_hole; i++)
	{
		if (item_data->param->holy_equip_param.rune_stone_hole[i] == HolyEquipParam::SKILL_HOLE_STATUS_LOCK)
		{
			m_role_module_mgr->NoticeNum(errornum::EN_HOLY_EQUIP_PLEASE_UNLOCK_LOWER_LEVEL_HOLE);
			return;
		}
	}

	if (!m_role_module_mgr->GetKnapsack()->ConsumeItem(hole_cfg->unlock_cost, hole_cfg->consume_num, __FUNCTION__))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	item_data->param->holy_equip_param.rune_stone_hole[fushi_hole] = HolyEquipParam::SKILL_HOLE_STATUS_UNLOCK;
	this->SetHolyBagDirty((short)index, false, HOLY_EQUIP_SEND_REASON_MOVE);

	gamelog::g_sheng_qi_system.printf(LL_INFO, "%s role[%d, %s] index[%d] open_fushi_hole[%d] consume_item[%d, %d] holy_level[%d] holy_param[%s]", __FUNCTION__,
		m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(),
		index, fushi_hole, hole_cfg->unlock_cost, hole_cfg->consume_num, item_data->param->holy_equip_param.level, item_data->param->GetParamLog(I_TYPE_HOLY_EQUIP));
}

void HolyEquipManager::PutOnFuShi(int sheng_qi_index, int sheng_qi_hole, int fu_shi_index)
{
	if (sheng_qi_index < 0 || sheng_qi_index >= ItemNamespace::MAX_HOLY_EQUIP_BAG_NUM) return;
	if (fu_shi_index   < 0 || fu_shi_index   >= ItemNamespace::MAX_RUNE_STONE_BAG_NUM) return;
	if (sheng_qi_hole  < 0 || sheng_qi_hole  >= MAX_RUNE_STONE_NUM) return;

	if (NULL == m_holy_grid_list[sheng_qi_index] || m_holy_grid_list[sheng_qi_index]->Invalid()) return;
	if (NULL == m_fushi_grid_list[fu_shi_index]  || m_fushi_grid_list[fu_shi_index]->Invalid()) return;
	
	ItemGridData* holy_item_data = m_holy_grid_list[sheng_qi_index];
	if (NULL == holy_item_data || NULL == holy_item_data->param) return;

	ItemGridData* fushi_item_data = m_fushi_grid_list[fu_shi_index];
	if (NULL == fushi_item_data || NULL == fushi_item_data->param) return;

	if (holy_item_data->param->holy_equip_param.rune_stone_hole[sheng_qi_hole] == HolyEquipParam::SKILL_HOLE_STATUS_LOCK) return;

	const ItemBase * fushi_base = ITEMPOOL->GetItem(fushi_item_data->item_id);
	if (fushi_base == NULL) return;
	const RuneStoneItem * op_fushi_item = (const RuneStoneItem *)fushi_base;

	for (int i = 0; i < MAX_RUNE_STONE_NUM; i++)
	{
		if(i == sheng_qi_hole || holy_item_data->param->holy_equip_param.rune_stone_hole[i] != HolyEquipParam::SKILL_HOLE_STATUS_HAVE_VALUE) continue;

		const ItemBase * item_base = ITEMPOOL->GetItem(holy_item_data->param->holy_equip_param.rune_stone_list[i].item_id);
		if (item_base == NULL) continue;
		const RuneStoneItem * fushi_item = (const RuneStoneItem *)item_base;

		if (fushi_item->GetFuShiType() == op_fushi_item->GetFuShiType())
		{
			m_role_module_mgr->NoticeNum(errornum::EN_HOLY_EQUIP_HAVE_SAME_TYPE_FUSHI);
			return;
		}
	}

	// 如果有符石先卸下来
	if (holy_item_data->param->holy_equip_param.rune_stone_hole[sheng_qi_hole] == HolyEquipParam::SKILL_HOLE_STATUS_HAVE_VALUE)
	{
		this->TakeOffFuShi(sheng_qi_index, sheng_qi_hole);
	}

	holy_item_data->param->holy_equip_param.rune_stone_hole[sheng_qi_hole] = HolyEquipParam::SKILL_HOLE_STATUS_HAVE_VALUE;
	holy_item_data->param->holy_equip_param.rune_stone_list[sheng_qi_hole] = fushi_item_data->param->rune_stone_param;
	this->ConsumeFuShiGrid(fu_shi_index, HOLY_EQUIP_SEND_REASON_FUSHI_INLAY, __FUNCTION__);

	this->ReCalHolyBaseAttr(sheng_qi_index);
	m_role_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_HOLY_EQUIP, __FUNCTION__);
	this->SetHolyBagDirty(sheng_qi_index, true, HOLY_EQUIP_SEND_REASON_MOVE, true);

	gamelog::g_sheng_qi_system.printf(LL_INFO, "%s role[%d, %s] holy_index[%d] holy_hole[%d] fushi_index[%d] ", __FUNCTION__,
		m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), sheng_qi_index, sheng_qi_hole, fu_shi_index);

	if (holy_item_data->param->holy_equip_param.holy_status == HolyEquipParam::HOLY_EQUIP_STATUS_FIGHTING || 
		holy_item_data->param->holy_equip_param.holy_status == HolyEquipParam::HOLY_EQUIP_STATUS_STANDBY)
	{
		std::string holy_state_log = STRING_SPRINTF("{state:%d item_id:%d fushi:", holy_item_data->param->holy_equip_param.holy_status, holy_item_data->item_id);

		for (int j = 0; j < ARRAY_LENGTH(holy_item_data->param->holy_equip_param.rune_stone_list); j++)
		{
			if (holy_item_data->param->holy_equip_param.rune_stone_hole[j] != HolyEquipParam::SKILL_HOLE_STATUS_HAVE_VALUE) continue;

			holy_state_log += STRING_SPRINTF("[%d:%d]", j, holy_item_data->param->holy_equip_param.rune_stone_list[j].item_id);
		}
		holy_state_log += STRING_SPRINTF("}");

		ROLE_LOG_QUICK6(LOG_TYPE_SHENG_QI_STATE_CHANGE, m_role_module_mgr->GetRole(), LOG_REASON_PUT_ON_FU_SHI, 0, holy_state_log.c_str(), holy_item_data->param->GetParamLog(I_TYPE_HOLY_EQUIP));
	}
}

void HolyEquipManager::TakeOffFuShi(int sheng_qi_index, int sheng_qi_hole)
{
	if (sheng_qi_index < 0 || sheng_qi_index >= ItemNamespace::MAX_HOLY_EQUIP_BAG_NUM) return;
	if (sheng_qi_hole < 0 || sheng_qi_hole >= MAX_RUNE_STONE_NUM) return;

	if (NULL == m_holy_grid_list[sheng_qi_index] || m_holy_grid_list[sheng_qi_index]->Invalid()) return;

	ItemGridData* item_data = m_holy_grid_list[sheng_qi_index];
	if (NULL == item_data || NULL == item_data->param) return;

	if (item_data->param->holy_equip_param.rune_stone_hole[sheng_qi_hole] != HolyEquipParam::SKILL_HOLE_STATUS_HAVE_VALUE) return;

	if (this->GetFuShiEmptyBag() == -1)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_FU_SHI_BAG_NO_SPACE);
		return;
	}

	ItemGridData* holy_item_data = m_holy_grid_list[sheng_qi_index];
	if (NULL == holy_item_data || NULL == holy_item_data->param) return;

	RuneStoneParam fushi_param = holy_item_data->param->holy_equip_param.rune_stone_list[sheng_qi_hole];

	ItemDataWrapper compound_equip; compound_equip.Reset();
	compound_equip.item_id = fushi_param.item_id;
	compound_equip.num = 1;
	compound_equip.is_bind = 0;
	compound_equip.has_param = 1;
	UniqueIDGenerator::Instance()->GenUniqueID(&compound_equip.item_unique_id);
	memcpy(compound_equip.param_data, &fushi_param, sizeof(fushi_param));

	if (!this->AddNewFuShiItem(&compound_equip, PUT_REASON_TAKE_OFF_FU_SHI))
	{
		gamelog::g_sheng_qi_system.printf(LL_INFO, "%s role[%d, %s] AddFuShiItem Failed!!!! item_id[%d]", __FUNCTION__, m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), fushi_param.item_id);
		return;
	}

	item_data->param->holy_equip_param.rune_stone_hole[sheng_qi_hole] = HolyEquipParam::SKILL_HOLE_STATUS_UNLOCK;
	item_data->param->holy_equip_param.rune_stone_list[sheng_qi_hole].Reset();

	this->ReCalHolyBaseAttr(sheng_qi_index);
	m_role_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_HOLY_EQUIP, __FUNCTION__);
	this->SetHolyBagDirty(sheng_qi_index, true, HOLY_EQUIP_SEND_REASON_MOVE, true);

	gamelog::g_sheng_qi_system.printf(LL_INFO, "%s role[%d, %s] holy_index[%d] holy_hole[%d] ", __FUNCTION__, m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), sheng_qi_index, sheng_qi_hole);	

	if (holy_item_data->param->holy_equip_param.holy_status == HolyEquipParam::HOLY_EQUIP_STATUS_FIGHTING ||
		holy_item_data->param->holy_equip_param.holy_status == HolyEquipParam::HOLY_EQUIP_STATUS_STANDBY)
	{
		std::string holy_state_log = STRING_SPRINTF("{state:%d item_id:%d fushi:", holy_item_data->param->holy_equip_param.holy_status, holy_item_data->item_id);

		for (int j = 0; j < ARRAY_LENGTH(holy_item_data->param->holy_equip_param.rune_stone_list); j++)
		{
			if (holy_item_data->param->holy_equip_param.rune_stone_hole[j] != HolyEquipParam::SKILL_HOLE_STATUS_HAVE_VALUE) continue;

			holy_state_log += STRING_SPRINTF("[%d:%d]", j, holy_item_data->param->holy_equip_param.rune_stone_list[j].item_id);
		}
		holy_state_log += STRING_SPRINTF("}");

		ROLE_LOG_QUICK6(LOG_TYPE_SHENG_QI_STATE_CHANGE, m_role_module_mgr->GetRole(), LOG_REASON_TAKE_OFF_FU_SHI, 0, holy_state_log.c_str(), holy_item_data->param->GetParamLog(I_TYPE_HOLY_EQUIP));
	}
}

void HolyEquipManager::TakeOffAllFuShi(int index)
{
	if (index < 0 || index >= ItemNamespace::MAX_HOLY_EQUIP_BAG_NUM) return;
	if (NULL == m_holy_grid_list[index] || m_holy_grid_list[index]->Invalid()) return;

	ItemGridData* item_data = m_holy_grid_list[index];
	if (NULL == item_data || NULL == item_data->param) return;

	{	// 外部需要判断,理论上这里不应该再出现背包不足了
		int count = 0;
		for (int i = 0; i < MAX_RUNE_STONE_NUM; i++)
		{
			if (item_data->param->holy_equip_param.rune_stone_hole[i] == HolyEquipParam::SKILL_HOLE_STATUS_HAVE_VALUE)
			{
				count++;
			}
		}
		// 符石背包不足, 不可以卸下
		if (!this->HasFuShiBagNum(count))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_FU_SHI_BAG_NO_SPACE);
			return;
		}
	}

	for (int i = 0; i < MAX_RUNE_STONE_NUM; i++)
	{
		if (item_data->param->holy_equip_param.rune_stone_hole[i] != HolyEquipParam::SKILL_HOLE_STATUS_HAVE_VALUE) continue;

		if (this->GetFuShiEmptyBag() != -1)
		{
			this->TakeOffFuShi(index, i);
		}
	}
}

bool HolyEquipManager::IsCanTakeOffAll(int index)
{
	ItemGridData* item_data = this->GetHolyBagNode(index);
	if (NULL == item_data)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return false;
	}
	
	int count = 0;
	for (int i = 0; i < MAX_RUNE_STONE_NUM; i++)
	{
		if (item_data->param->holy_equip_param.rune_stone_hole[i] == HolyEquipParam::SKILL_HOLE_STATUS_HAVE_VALUE)
		{
			count++;
		}
	}

	// 符石背包不足, 不可以卸下
	if (!this->HasFuShiBagNum(count))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_FU_SHI_BAG_NO_SPACE);
		return false;
	}

	return true;
}

void HolyEquipManager::FusionFuShi(int fushi_level, int fushi_index1, int fushi_index2, int fushi_index3, int fushi_index4, int fushi_index5)
{
	if (fushi_level <= 1 || fushi_level > RuneStoneItem::MAX_RUNE_STONE_LEVEL) return;

	const RuneStoneFusingCfg* fusing_cfg = LOGIC_CONFIG->GetHolyEquipMentConfig()->GetFuShiFusingCfg(fushi_level - 1);
	if (NULL == fusing_cfg) return;

	int consume_count = fusing_cfg->need_item_num;
	if (consume_count > 5) return;

	int index_array[5] = { fushi_index1, fushi_index2, fushi_index3, fushi_index4, fushi_index5 };

	{	// 检查传进来的参数有没有重复的index
		std::set<int> temp_index_list;

		for (int i = 0; i < consume_count; i++)
		{
			if (temp_index_list.find(index_array[i]) != temp_index_list.end())
			{
				m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
				return;
			}
			temp_index_list.insert(index_array[i]);
		}
	}

	for (int i = 0; i < consume_count; i++)
	{
		if (index_array[i] < 0 || index_array[i] >= ItemNamespace::MAX_RUNE_STONE_BAG_NUM) return;
		if (NULL == m_fushi_grid_list[index_array[i]] || m_fushi_grid_list[index_array[i]]->Invalid()) return;
	}

	ItemGridData* item_data[5] = { NULL };
	for (int i = 0; i < consume_count; i++)
	{
		item_data[i] = m_fushi_grid_list[index_array[i]];
	}

	for (int i = 0; i < consume_count; i++)
	{
		if (NULL == item_data[i] || NULL == item_data[i]->param) return;
	}

	bool is_same_type = true;		// 类型相同合成的也是同类型的符石
	int type = 0;
	for (int i = 0; i < consume_count; i++)
	{
		const ItemBase * fushi_base = ITEMPOOL->GetItem(item_data[i]->item_id);
		if (fushi_base == NULL) return;
		const RuneStoneItem * fushi_item = (const RuneStoneItem *)fushi_base;

		if (fushi_item->GetFuShiLevel() >= RuneStoneItem::MAX_RUNE_STONE_LEVEL) return;

		if ((fushi_level - 1) != fushi_item->GetFuShiLevel())
		{
			m_role_module_mgr->NoticeNum(errornum::EN_HOLY_EQUIP_FUSHI_LEVEL_NOT_SAME);
			return;
		}

		if (i == 0)
		{
			type = fushi_item->GetFuShiType();
		}
		else
		{
			if (type != fushi_item->GetFuShiType())
			{
				is_same_type = false;
			}
		}
	}

	if (!m_role_module_mgr->GetMoney()->CoinBindMoreThan(fusing_cfg->cost_coin))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_COIN_NOT_ENOUGH);
		return;
	}

	if (!is_same_type)		//种类不同,随机类型
	{
		type = RandomNum(RuneStoneItem::RUNE_STONE_TYPE_MAX - 1) + 1;
	}

	const ItemID* item_id = LOGIC_CONFIG->GetHolyEquipMentConfig()->GetFuShiIdByTypeLevelCfg(type, fushi_level);
	if (NULL == item_id) return;

	{
		if (!m_role_module_mgr->GetMoney()->UseCoinBind(fusing_cfg->cost_coin, __FUNCTION__))
		{
			return;
		}

		for (int i = 0; i < consume_count; i++)	// 清掉道具数据
		{
			if (!this->ConsumeFuShiGrid(index_array[i], HOLY_EQUIP_SEND_REASON_FUSHI_FUSING, __FUNCTION__, false))		// 这个改变不做通知,有下面的协议发通知
			{
				return;
			}
		}

		Protocol::SCSCHolyEquipLevelUpConsumeInfo msg;
		msg.bag_type = SCSCHolyEquipLevelUpConsumeInfo::BAG_TYPE_FUSHI;
		msg.count = consume_count;

		memset(msg.index, 0, sizeof(msg.index));
		for (int i = 0; i < consume_count; i++)
		{
			msg.index[i] = index_array[i];
		}

		m_role_module_mgr->NetSend(&msg, sizeof(msg));
	}

	int out_idx = 0;
	const ItemGridData * igd = this->AddFuShiItem(*item_id, HOLY_EQUIP_SEND_REASON_FUSHI_FUSING, &out_idx);

	if (NULL == igd)
	{
		gamelog::g_sheng_qi_system.printf(LL_WARNING, "%s role[%d, %s] item_id[%d] out_idx[%d] AddFuShiItem Failed!!!!!!", __FUNCTION__,
			m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), *item_id, out_idx);
		return;
	}
	this->SetFuShiBagDirty(out_idx, HOLY_EQUIP_SEND_REASON_FUSHI_FUSING);
}

void HolyEquipManager::RecastFuShi(int fushi_index, int consume_index)
{
	if (fushi_index   < 0 || fushi_index >= ItemNamespace::MAX_RUNE_STONE_BAG_NUM) return;
	if (consume_index < 0 || consume_index >= ItemNamespace::MAX_RUNE_STONE_BAG_NUM) return;

	if (NULL == m_fushi_grid_list[fushi_index] || m_fushi_grid_list[fushi_index]->Invalid()) return;
	if (NULL == m_fushi_grid_list[consume_index] || m_fushi_grid_list[consume_index]->Invalid()) return;

	ItemGridData* fushi_item_data = m_fushi_grid_list[fushi_index];
	if (NULL == fushi_item_data || NULL == fushi_item_data->param) return;

	ItemGridData* consume_item_data = m_fushi_grid_list[consume_index];
	if (NULL == consume_item_data || NULL == consume_item_data->param) return;

	const ItemBase * fushi_base = ITEMPOOL->GetItem(fushi_item_data->item_id);
	if (fushi_base == NULL) return;
	const RuneStoneItem * fushi_item = (const RuneStoneItem *)fushi_base;

	const ItemBase * consume_base = ITEMPOOL->GetItem(consume_item_data->item_id);
	if (consume_base == NULL) return;
	const RuneStoneItem * consume_item = (const RuneStoneItem *)consume_base;

	if(fushi_item->GetFuShiLevel() <= 1) return;
	if (fushi_item->GetFuShiLevel() - 1 != consume_item->GetFuShiLevel())
	{
		m_role_module_mgr->NoticeNum(errornum::EN_HOLY_EQUIP_FUSHI_LEVEL_ERROR);
		return;
	}

	const RuneStoneRecastCfg* recast_cfg = LOGIC_CONFIG->GetHolyEquipMentConfig()->GetFuShiRecastCfg(fushi_item->GetFuShiLevel());
	if (recast_cfg == NULL) return;
	
	{
		if (!m_role_module_mgr->GetMoney()->CoinBindMoreThan(recast_cfg->cost_coin))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_COIN_NOT_ENOUGH);
			return;
		}

		if (!m_role_module_mgr->GetMoney()->UseCoinBind(recast_cfg->cost_coin, __FUNCTION__))
		{
			return;
		}

		this->ConsumeFuShiGrid(consume_index, HOLY_EQUIP_SEND_REASON_FUSHI_REACST, __FUNCTION__);
	}

	// 生成新净值	
	const ItemBase * new_item = ITEMPOOL->GetItem(fushi_item->GetItemId());
	if (NULL == new_item)return;

	const RuneStoneItem * ib = (const RuneStoneItem *)new_item;
	static NetValueItemParam p_nvip; p_nvip.Reset();
	int put_reason = 0;
	ib->RandomNetValueOnPutItem(m_role_module_mgr->GetRole(), &p_nvip, put_reason);
	fushi_item_data->param->rune_stone_param = p_nvip.rune_stone_param; //--
	this->SetFuShiBagDirty(fushi_index, HOLY_EQUIP_SEND_REASON_FUSHI_REACST);

	gamelog::g_sheng_qi_system.printf(LL_INFO, "%s user[%d,%s] index[%d] equip_param[%s]", __FUNCTION__,
		m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), fushi_index, fushi_item_data->param->GetParamLog(I_TYPE_RUNE_STONE));
}

void HolyEquipManager::RecastFuShiOnHolyEquip(int index, int hole, int consume_fushi_index)
{
	if (index < 0 || index >= ItemNamespace::MAX_HOLY_EQUIP_BAG_NUM) return;
	if (hole  < 0 || hole >= MAX_RUNE_STONE_NUM) return;
	if (consume_fushi_index < 0 || consume_fushi_index >= ItemNamespace::MAX_RUNE_STONE_BAG_NUM) return;

	if (NULL == m_holy_grid_list[index] || m_holy_grid_list[index]->Invalid()) return;
	if (NULL == m_fushi_grid_list[consume_fushi_index] || m_fushi_grid_list[consume_fushi_index]->Invalid()) return;

	ItemGridData* item_data = m_holy_grid_list[index];
	if (NULL == item_data || NULL == item_data->param) return;

	ItemGridData* consume_fushi_data = m_fushi_grid_list[consume_fushi_index];
	if (NULL == consume_fushi_data || NULL == consume_fushi_data->param) return;

	if (item_data->param->holy_equip_param.rune_stone_hole[hole] != HolyEquipParam::SKILL_HOLE_STATUS_HAVE_VALUE) return;

	const ItemBase * fushi_base = ITEMPOOL->GetItem(item_data->param->holy_equip_param.rune_stone_list[hole].item_id);
	if (fushi_base == NULL) return;
	const RuneStoneItem * fushi_item = (const RuneStoneItem *)fushi_base;

	if (fushi_item->GetFuShiLevel() <= 1) return;	// 一级符石不可重铸

	const ItemBase * consume_base = ITEMPOOL->GetItem(consume_fushi_data->item_id);
	if (consume_base == NULL) return;
	const RuneStoneItem * consume_item = (const RuneStoneItem *)consume_base;

	if (fushi_item->GetFuShiLevel() - 1 != consume_item->GetFuShiLevel())
	{
		m_role_module_mgr->NoticeNum(errornum::EN_HOLY_EQUIP_FUSHI_LEVEL_ERROR);
		return;
	}

	const RuneStoneRecastCfg* recast_cfg = LOGIC_CONFIG->GetHolyEquipMentConfig()->GetFuShiRecastCfg(fushi_item->GetFuShiLevel());
	if (recast_cfg == NULL) return;

	{
		if (!m_role_module_mgr->GetMoney()->CoinBindMoreThan(recast_cfg->cost_coin))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_COIN_NOT_ENOUGH);
			return;
		}

		if (!m_role_module_mgr->GetMoney()->UseCoinBind(recast_cfg->cost_coin, __FUNCTION__))
		{
			return;
		}

		this->ConsumeFuShiGrid(consume_fushi_index, HOLY_EQUIP_SEND_REASON_FUSHI_REACST, __FUNCTION__);
	}

	// 生成新净值	
	const ItemBase * new_item = ITEMPOOL->GetItem(fushi_item->GetItemId());
	if (NULL == new_item)return;

	const RuneStoneItem * ib = (const RuneStoneItem *)new_item;
	static NetValueItemParam p_nvip; p_nvip.Reset();
	int put_reason = 0;
	ib->RandomNetValueOnPutItem(m_role_module_mgr->GetRole(), &p_nvip, put_reason);

	item_data->param->holy_equip_param.rune_stone_list[hole] = p_nvip.rune_stone_param; //--

	this->ReCalHolyBaseAttr(index);
	m_role_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_HOLY_EQUIP, __FUNCTION__);

	this->SetHolyBagDirty(index, true, HOLY_EQUIP_SEND_REASON_FUSHI_REACST);

	gamelog::g_sheng_qi_system.printf(LL_INFO, "%s user[%d,%s] index[%d] consume_fushi[%d] equip_param[%s] ", __FUNCTION__,
		m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), index, consume_fushi_index, item_data->param->GetParamLog(I_TYPE_HOLY_EQUIP));
}

void HolyEquipManager::BlessPet(int holy_index, int pet_index)
{
	if (holy_index < 0 || holy_index >= ItemNamespace::MAX_HOLY_EQUIP_BAG_NUM) return;

	if (pet_index != -1)
	{
		int holy_state = this->FindStatusByIndex(holy_index);
		if (holy_state != HolyEquipParam::HOLY_EQUIP_STATUS_STANDBY && holy_state != HolyEquipParam::HOLY_EQUIP_STATUS_FIGHTING)
		{
			m_role_module_mgr->NoticeNum(errornum::EN_HOLY_EQUIP_CAN_NOT_BLESS_PET);
			return;
		}

		const ItemGridData* temp_pet_grid = m_role_module_mgr->GetPet()->GetPetGridData(pet_index);
		if (NULL == temp_pet_grid) return;

		for (int i = 0; i < ARRAY_LENGTH(m_role_module_mgr->GetCommonData().holy_equip_status_index); i++)
		{
			const ItemGridData* temp_holy_data = this->GetHolyBagNode(m_role_module_mgr->GetCommonData().holy_equip_status_index[i]);
			if (NULL == temp_holy_data) continue;

			if (temp_holy_data->param->holy_equip_param.pet_id == temp_pet_grid->item_id && temp_holy_data->param->holy_equip_param.pet_unique_id == temp_pet_grid->item_unique_id)
			{
				m_role_module_mgr->NoticeNum(errornum::EN_HOLY_EQUIP_BLESS_PET_ONE_ON_ONE);
				return;
			}
		}
	}
	
	const ItemGridData* holy_data = this->GetHolyBagNode(holy_index);
	if (NULL == holy_data) return;

	// -1表示取下圣佑宠物
	if (pet_index == -1)
	{
		// 先获取宠物信息,然后无论是否获取到宠物信息都清掉圣器的祝福先,然后重算宠物信息
		int temp_pet_index = -1;
		const PetParam* temp_pet_data = m_role_module_mgr->GetPet()->GetPetParam(holy_data->param->holy_equip_param.pet_id, holy_data->param->holy_equip_param.pet_unique_id, &temp_pet_index);
		
		holy_data->param->holy_equip_param.pet_id = 0;
		holy_data->param->holy_equip_param.pet_unique_id = 0;

		if (NULL != temp_pet_data)
		{
			m_role_module_mgr->GetPet()->ReCalcAttr(temp_pet_index);
			m_role_module_mgr->GetPet()->SendSingleInfo(temp_pet_index, Protocol::SCSendPetSingleInfo::PET_NOTIE_TYPE_HOLY_BLESS);
			m_role_module_mgr->GetPet()->SendPetAttrListReason(temp_pet_index);
		}	
	}
	else
	{
		const ItemGridData* pet_grid = m_role_module_mgr->GetPet()->GetPetGridData(pet_index);
		if (NULL == pet_grid) return;

		{	//如果之前有圣佑的宠物,先取下来
			int old_pet_index = -1;
			const PetParam* old_pet_data = m_role_module_mgr->GetPet()->GetPetParam(holy_data->param->holy_equip_param.pet_id, holy_data->param->holy_equip_param.pet_unique_id, &old_pet_index);
			if (NULL != old_pet_data)
			{
				holy_data->param->holy_equip_param.pet_id = 0;
				holy_data->param->holy_equip_param.pet_unique_id = 0;

				m_role_module_mgr->GetPet()->ReCalcAttr(old_pet_index);
				m_role_module_mgr->GetPet()->SendSingleInfo(old_pet_index, Protocol::SCSendPetSingleInfo::PET_NOTIE_TYPE_HOLY_BLESS);
				m_role_module_mgr->GetPet()->SendPetAttrListReason(old_pet_index);
			}
		}

		holy_data->param->holy_equip_param.pet_id = (unsigned short)pet_grid->param->pet_param.pet_id;
		holy_data->param->holy_equip_param.pet_unique_id = pet_grid->item_unique_id;

		m_role_module_mgr->GetPet()->ReCalcAttr(pet_index);
		m_role_module_mgr->GetPet()->SendSingleInfo(pet_index, Protocol::SCSendPetSingleInfo::PET_NOTIE_TYPE_HOLY_BLESS);
		m_role_module_mgr->GetPet()->SendPetAttrListReason(pet_index);
	}

	gamelog::g_sheng_qi_system.printf(LL_INFO, "%s role[%d, %s] holy_index[%d] pet_index[%d] pet_id[%d] pet_unique_id[%lld]", __FUNCTION__,
		m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), holy_index, pet_index, holy_data->param->holy_equip_param.pet_id, holy_data->param->holy_equip_param.pet_unique_id);
	
	this->SetHolyBagDirty(holy_index, false, HOLY_EQUIP_SEND_REASON_BLESS_PET);
}

void HolyEquipManager::DeleteBlessPet(long long pet_unique_id)
{
	for (size_t i = 0; i < HOLY_EQUIP_STATUS_MAX; i++)
	{
		int index = m_role_module_mgr->GetCommonData().holy_equip_status_index[i];
		if (index == -1) continue;

		ItemGridData* item_data = this->GetHolyBagNode(index);
		if (NULL == item_data || NULL == item_data->param) continue;

		if (item_data->param->holy_equip_param.pet_unique_id == pet_unique_id)
		{
			this->BlessPet(index, -1);
		}
	}
}

void HolyEquipManager::HolyFollow(int index)
{
	m_param.follow_index = index;
	this->SendHolyFollowInfo();
}

void HolyEquipManager::HallowsSkillConvert(int skill_1, int skill_2)
{
	const ItemBase* item_1 = ITEMPOOL->GetItem((ItemID)skill_1);
	if (NULL == item_1) return;
	const ItemBase* item_2 = ITEMPOOL->GetItem((ItemID)skill_2);
	if (NULL == item_2) return;

	std::map<int, int> consume_map;
	if (skill_1 == skill_2)
	{
		consume_map.insert(std::make_pair(skill_1, 2));
	}
	else
	{
		consume_map.insert(std::make_pair(skill_1, 1));
		consume_map.insert(std::make_pair(skill_2, 1));
	}

	if (item_1->GetItemSubType() != ItemNamespace::ITEM_SUB_TYPE_HALLOWS_SKILL || (item_1->GetColor() != ItemBase::I_COLOR_ORANGE && item_1->GetColor() != ItemBase::I_COLOR_RED))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_USE_ITEM_ERROR);
		return;
	}

	if (item_1->GetItemSubType() != item_2->GetItemSubType() || item_1->GetColor() != item_2->GetColor())
	{
		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_USE_ITEM_ERROR);
		return;
	}

	const ItemConfigData& item_cfg = item_1->GetColor() == ItemBase::I_COLOR_ORANGE ? LOGIC_CONFIG->GetHolyEquipMentConfig()->GetOtherCfg().baoxiang1 : LOGIC_CONFIG->GetHolyEquipMentConfig()->GetOtherCfg().baoxiang2;

	if (!m_role_module_mgr->GetKnapsack()->CheckForPut2(item_cfg)) return;

	if (m_role_module_mgr->GetKnapsack()->HasItemByMap(consume_map))
	{
		if (m_role_module_mgr->GetKnapsack()->ConsumeItemByMap(consume_map, __FUNCTION__))
		{
			m_role_module_mgr->GetKnapsack()->Put(item_cfg, PUT_REASON_HALLOWS_SKILL_CONVERT);
		}
	}
	else
	{
		m_role_module_mgr->NoticeNum(errornum::EN_ITEM_NOT_ELIGIBLE);
		return;
	}

	gamelog::g_sheng_qi_system.printf(LL_INFO, "%s role[%d, %s] consume_item1[%d, %d] consume_item2[%d, %d] reward[%d, %d]", __FUNCTION__,
		m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), skill_1, 1, skill_2, 1, item_cfg.item_id, item_cfg.num);
}

void HolyEquipManager::ClearAll()
{
	for (int i = 0; i < ItemNamespace::MAX_HOLY_EQUIP_BAG_NUM; i++)
	{
		this->ClearHolyGrid(i);
		m_holy_dirty_mark[i] = true;
	}
	m_holy_bag_is_change = true;

	for (int i = 0; i < ItemNamespace::MAX_RUNE_STONE_BAG_NUM; i++)
	{
		this->ClearFuShiGrid(i);
		m_fushi_dirty_mark[i] = true;
	}
	m_fushi_bag_is_change = true;

	memset(m_role_module_mgr->GetCommonData().holy_equip_status_index, -1, sizeof(m_role_module_mgr->GetCommonData().holy_equip_status_index));

	this->SendAllInfo();

	gamelog::g_sheng_qi_system.printf(LL_INFO, "%s role[%d, %s] Clear All!!!", __FUNCTION__, m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName());
}

void HolyEquipManager::ClearHolyGrid(int index)
{
	if (index < 0 || index >= ItemNamespace::MAX_HOLY_EQUIP_BAG_NUM)return;
	if (NULL == m_holy_grid_list[index]) return;

	m_holy_grid_list[index]->Clear();
	delete m_holy_grid_list[index];
	m_holy_grid_list[index] = NULL;
}

void HolyEquipManager::ClearFuShiGrid(int index)
{
	if (index < 0 || index >= ItemNamespace::MAX_RUNE_STONE_BAG_NUM)return;
	if (NULL == m_fushi_grid_list[index]) return;

	m_fushi_grid_list[index]->Clear();
	delete m_fushi_grid_list[index];
	m_fushi_grid_list[index] = NULL;
}

bool HolyEquipManager::ConsumeHolyGrid(int consume_idx, int reason_type, const char * reason, bool is_notice)
{
	ItemGridData* bag_node = this->GetHolyBagNode(consume_idx);
	if (NULL == bag_node)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return false;
	}

	const ItemBase * ib = ITEMPOOL->GetItem(bag_node->item_id);
	if (NULL == ib)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return false;
	}

	// 圣器跟随状态取消
	if (consume_idx == m_param.follow_index)
	{
		this->HolyFollow();
	}

	this->TakeOffAllFuShi(consume_idx);

	const char *item_param_string = NULL;
	item_param_string = bag_node->param->GetParamLog(ib->GetItemType());
	this->ClearHolyGrid(consume_idx);
	this->SetHolyBagDirty(consume_idx, false, reason_type, is_notice);
	
	ROLE_LOG_QUICK6(LOG_TYPE_ITEM_CONSUME, m_role_module_mgr->GetRole(), bag_node->item_id, bag_node->num, reason, item_param_string);
	gamelog::g_sheng_qi_system.printf(LL_INFO, "%s user[%d,%s] consume_idx[%d] reason_type[%d] equip_param[%s]", __FUNCTION__,
		m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), consume_idx, reason_type, item_param_string);

	return true;
}

bool HolyEquipManager::ConsumeFuShiGrid(int consume_idx, int reason_type, const char * reason, bool is_notice)
{
	ItemGridData* bag_node = this->GetFuShiBagNode(consume_idx);
	if (NULL == bag_node)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return false;
	}

	const ItemBase * ib = ITEMPOOL->GetItem(bag_node->item_id);
	if (NULL == ib)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return false;
	}

	const char *item_param_string = NULL;
	item_param_string = bag_node->param->GetParamLog(ib->GetItemType());
	this->ClearFuShiGrid(consume_idx);
	this->SetFuShiBagDirty(consume_idx, reason_type, is_notice);

	ROLE_LOG_QUICK6(LOG_TYPE_ITEM_CONSUME, m_role_module_mgr->GetRole(), bag_node->item_id, bag_node->num, reason, item_param_string);
	gamelog::g_sheng_qi_system.printf(LL_INFO, "%s user[%d,%s] consume_idx[%d] reason_type[%d] equip_param[%s]", __FUNCTION__,
		m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), consume_idx, reason_type, item_param_string);

	return true;
}

void HolyEquipManager::SetHolyBagDirty(short index, bool is_recalculate, short reason, bool is_notice)
{
	if (index < 0 || index >= ItemNamespace::MAX_HOLY_EQUIP_BAG_NUM) return;

	if (is_recalculate)
	{
		this->CalculateScore(index);
	}

	if (this->HasBlessPet(index))
	{
		ItemGridData* holy_grid = this->GetHolyBagNode(index);
		if (NULL != holy_grid)
		{
			int pet_index = -1;
			const PetParam* pet_data = m_role_module_mgr->GetPet()->GetPetParam(holy_grid->param->holy_equip_param.pet_id, holy_grid->param->holy_equip_param.pet_unique_id, &pet_index);
			if(NULL != pet_data)
			{
				m_role_module_mgr->GetPet()->ReCalcAttr(pet_index);
			}
		}
	}

	m_holy_dirty_mark[index] = true;
	m_holy_bag_is_change = true;
	if(is_notice) this->SendSingleHolyBagInfo(index, reason);
}

void HolyEquipManager::SetFuShiBagDirty(short index, short reason, bool is_notice)
{
	if (index < 0 || index >= ItemNamespace::MAX_RUNE_STONE_BAG_NUM) return;

	m_fushi_dirty_mark[index] = true;
	m_fushi_bag_is_change = true;
	if(is_notice) this->SendSingleFuShiBagInfo(index, reason);
}

void HolyEquipManager::GmClearBag(int op_type)
{
	switch (op_type)
	{
	case 0:
		{
			this->ClearAll();
			this->HolyFollow();
		}
		break;
	case 1:
		{
			for (int i = 0; i < ItemNamespace::MAX_HOLY_EQUIP_BAG_NUM; i++)
			{
				this->ClearHolyGrid(i);
				m_holy_dirty_mark[i] = true;
			}
			m_holy_bag_is_change = true;

			memset(m_role_module_mgr->GetCommonData().holy_equip_status_index, -1, sizeof(m_role_module_mgr->GetCommonData().holy_equip_status_index));
			this->SendHolyBagListInfo();
			this->HolyFollow();
		}
		break;
	case 2:
		{
			for (int i = 0; i < ItemNamespace::MAX_RUNE_STONE_BAG_NUM; i++)
			{
				this->ClearFuShiGrid(i);
				m_fushi_dirty_mark[i] = true;
			}
			m_fushi_bag_is_change = true;

			this->SendFuShiBagListInfo();
		}
		break;
	default:
		break;
	}

	m_role_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_HOLY_EQUIP, __FUNCTION__);
}

void HolyEquipManager::CheckHolyEquipCollection()
{
	for (int index = 0; index < ItemNamespace::MAX_HOLY_EQUIP_BAG_NUM; index++)
	{
		if (NULL == m_holy_grid_list[index] || m_holy_grid_list[index]->Invalid()) continue;

		ItemGridData* item_data = m_holy_grid_list[index];
		if (NULL == item_data || NULL == item_data->param) continue;

		m_role_module_mgr->GetCollection()->OnHolyEquipCollection(item_data->item_id);
	}
}

void HolyEquipManager::GetBattleHallows(ARG_OUT short* hallow_num, ARG_OUT BattleHallowInfo* hallows, ARG_OUT short* fight_hallow_idx, int max_hallow_num)
{
	for (int i = 0; i < HOLY_EQUIP_STATUS_MAX
		&& *hallow_num < max_hallow_num; ++i)
	{
		int index = m_role_module_mgr->GetCommonData().holy_equip_status_index[i];
		const ItemGridData* hallow = this->GetHolyEquipByIndex(index);
		if (NULL == hallow || NULL == hallow->param) continue;

		const HolyEquipParam& param = hallow->param->holy_equip_param;
		if (HolyEquipParam::HOLY_EQUIP_STATUS_REST == param.holy_status) continue;

		const HolyEquipBaseCfg* hallow_cfg = LOGIC_CONFIG->GetHolyEquipMentConfig()->GetBaseCfgById(hallow->item_id);
		if (NULL == hallow_cfg) continue;

		hallows[*hallow_num].hallow_id = hallow_cfg->shengqi_id;
		hallows[*hallow_num].hallow_lv = param.level;
		param.GetBattleHallowAttr(hallows[*hallow_num]);

		int skill_num = 0;
		for (int k = 0; k < ARRAY_ITEM_COUNT(param.active_skills); ++k)
		{
			int skill_id = 0;
			int skill_lv = 0;
			bool ret = LOGIC_CONFIG->GetHolyEquipMentConfig()->
				GetSkillLevel(param.active_skills[k], &skill_id, &skill_lv);
			if (ret)
			{
				hallows[*hallow_num].skill_id_list[skill_num] = skill_id;
				hallows[*hallow_num].skill_lv_list[skill_num] = skill_lv;
				skill_num += 1;
			}
		}
		hallows[*hallow_num].skill_num = skill_num;

		if (HolyEquipParam::HOLY_EQUIP_STATUS_FIGHTING == param.holy_status)
		{
			*fight_hallow_idx = *hallow_num;
		}

		int gift_num = 0;
		for (int k = 0; k < ARRAY_ITEM_COUNT(param.talent_skills); ++k)
		{
			hallows[*hallow_num].gift_id_list[gift_num++] = param.talent_skills[k];
		}
		hallows[*hallow_num].gift_num = gift_num;

		*hallow_num += 1;
	}
}

void HolyEquipManager::SendAllInfo()
{
	this->SendHolyBagListInfo();
	this->SendFuShiBagListInfo();
}

void HolyEquipManager::SendFirstGainFlagInfo()
{
	Protocol::SCHolyEquipFirstGainInfo msg;
	msg.first_gain = m_param.first_gain;

	m_role_module_mgr->NetSend(&msg, sizeof(msg));
}

void HolyEquipManager::SendSingleHolyBagInfo(int index, short reason_type)
{
	static Protocol::SCHolyBagItemChange single_change;
	single_change.reason = reason_type;
	single_change.put_reason = 0;
	single_change.index = static_cast<short>(index);
	single_change.bag_type = Protocol::SCHolyBagItemChange::BAG_TYPE_HOLY;
	single_change.reserve_sh = 0;
	if (NULL == m_holy_grid_list[index] || m_holy_grid_list[index]->Invalid())
	{
		single_change.item_id = 0;
		single_change.num = 0;
		single_change.is_bind = 0;
		single_change.has_param = 0;
		single_change.invalid_time = 0;
		single_change.item_unique_id = 0;
		single_change.is_market_buy_flag = 0;
	}
	else
	{
		single_change.item_id = m_holy_grid_list[index]->item_id;
		single_change.num = m_holy_grid_list[index]->num;
		single_change.is_bind = m_holy_grid_list[index]->is_bind ? 1 : 0;
		single_change.has_param = (NULL != m_holy_grid_list[index]->param);
		single_change.invalid_time = m_holy_grid_list[index]->invalid_time;
		single_change.item_unique_id = m_holy_grid_list[index]->item_unique_id;
		single_change.is_market_buy_flag = m_holy_grid_list[index]->is_market_buy_flag;

		const ItemBase* item_base = ITEMPOOL->GetItem(single_change.item_id);
		if (NULL == item_base)
		{
			return;
		}
		single_change.param_length = m_holy_grid_list[index]->param->GetDataByItemType(item_base->GetItemType(), single_change.param_data);
	}

	EngineAdapter::Instance().NetSendSerializeMsg(m_role_module_mgr->GetRole()->GetNetId(), &single_change);
}

void HolyEquipManager::SendSingleFuShiBagInfo(int index, short reason_type)
{
	static Protocol::SCHolyBagItemChange single_change;
	single_change.reason = reason_type;
	single_change.put_reason = 0;
	single_change.index = static_cast<short>(index);
	single_change.bag_type = Protocol::SCHolyBagItemChange::BAG_TYPE_FUSHI;
	single_change.reserve_sh = 0;
	if (NULL == m_fushi_grid_list[index] || m_fushi_grid_list[index]->Invalid())
	{
		single_change.item_id = 0;
		single_change.num = 0;
		single_change.is_bind = 0;
		single_change.has_param = 0;
		single_change.invalid_time = 0;
		single_change.item_unique_id = 0;
		single_change.is_market_buy_flag = 0;
	}
	else
	{
		single_change.item_id = m_fushi_grid_list[index]->item_id;
		single_change.num = m_fushi_grid_list[index]->num;
		single_change.is_bind = m_fushi_grid_list[index]->is_bind ? 1 : 0;
		single_change.has_param = (NULL != m_fushi_grid_list[index]->param);
		single_change.invalid_time = m_fushi_grid_list[index]->invalid_time;
		single_change.item_unique_id = m_fushi_grid_list[index]->item_unique_id;
		single_change.is_market_buy_flag = m_fushi_grid_list[index]->is_market_buy_flag;

		const ItemBase* item_base = ITEMPOOL->GetItem(single_change.item_id);
		if (NULL == item_base)
		{
			return;
		}
		single_change.param_length = m_fushi_grid_list[index]->param->GetDataByItemType(item_base->GetItemType(), single_change.param_data);
	}

	EngineAdapter::Instance().NetSendSerializeMsg(m_role_module_mgr->GetRole()->GetNetId(), &single_change);
}

void HolyEquipManager::SendHolyBagListInfo()
{
	static Protocol::SCHolyBagListInfo info;
	info.item_count = 0;

	for (int i = 0; i < ItemNamespace::MAX_HOLY_EQUIP_BAG_NUM; ++i)
	{
		if (NULL == m_holy_grid_list[i] || m_holy_grid_list[i]->Invalid()) continue;

		const ItemGridData * item_grid_data = m_holy_grid_list[i];
		const ItemBase * item_base = ITEMPOOL->GetItem(item_grid_data->item_id);
		if (NULL == item_base) continue;

		InfoType2 & ptc_con = info.item_list[info.item_count++];
		ptc_con.item_id = item_grid_data->item_id;
		ptc_con.is_bind = (char)item_grid_data->is_bind;
		ptc_con.index = i;
		ptc_con.is_market_buy_flag = item_grid_data->is_market_buy_flag;
		ptc_con.reserve_ch = 0;
		ptc_con.num = item_grid_data->num;
		ptc_con.has_param = false;
		ptc_con.invalid_time = item_grid_data->invalid_time;
		ptc_con.item_unique_id = item_grid_data->item_unique_id;

		if (NULL != item_grid_data->param) // 只发送有参数的
		{
			ptc_con.has_param = true;
			ptc_con.param_length = item_grid_data->param->GetDataByItemType(item_base->GetItemType(), ptc_con.param_data);
		}
	}

	EngineAdapter::Instance().NetSendSerializeMsg(m_role_module_mgr->GetRole()->GetNetId(), &info);
}

void HolyEquipManager::SendFuShiBagListInfo()
{	
	static Protocol::SCFuShiBagListInfo info;
	info.item_count = 0;

	for (int i = 0; i < ItemNamespace::MAX_RUNE_STONE_BAG_NUM; ++i)
	{
		if (NULL == m_fushi_grid_list[i] || m_fushi_grid_list[i]->Invalid()) continue;

		const ItemGridData * item_grid_data = m_fushi_grid_list[i];
		const ItemBase * item_base = ITEMPOOL->GetItem(item_grid_data->item_id);
		if (NULL == item_base) continue;

		InfoType2 & ptc_con = info.item_list[info.item_count++];
		ptc_con.item_id = item_grid_data->item_id;
		ptc_con.is_bind = (char)item_grid_data->is_bind;
		ptc_con.index = i;
		ptc_con.is_market_buy_flag = item_grid_data->is_market_buy_flag;
		ptc_con.reserve_ch = 0;
		ptc_con.num = item_grid_data->num;
		ptc_con.has_param = false;
		ptc_con.invalid_time = item_grid_data->invalid_time;
		ptc_con.item_unique_id = item_grid_data->item_unique_id;

		if (NULL != item_grid_data->param) // 只发送有参数的
		{
			ptc_con.has_param = true;
			ptc_con.param_length = item_grid_data->param->GetDataByItemType(item_base->GetItemType(), ptc_con.param_data);
		}
	}

	EngineAdapter::Instance().NetSendSerializeMsg(m_role_module_mgr->GetRole()->GetNetId(), &info);
}

void HolyEquipManager::SendHolyFollowInfo()
{
	Protocol::SCHolyEquipFollowInfo msg;
	msg.follow_index = m_param.follow_index;
	m_role_module_mgr->NetSend(&msg, sizeof(msg));
}

ItemGridData* HolyEquipManager::MutableFightHolyEquip(int * out_index /* = NULL */, int* out_index_in_bag /* = NULL */)
{
	for (int i = 0; i < HOLY_EQUIP_STATUS_MAX; i++)
	{
		int index = m_role_module_mgr->GetCommonData().holy_equip_status_index[i];

		if (index == -1) continue;
		if (index < 0 || index >= ItemNamespace::MAX_HOLY_EQUIP_BAG_NUM) continue;

		if (NULL != m_holy_grid_list[index] && !m_holy_grid_list[index]->Invalid())
		{
			if (NULL != m_holy_grid_list[index]->param)
			{
				if (m_holy_grid_list[index]->param->holy_equip_param.holy_status == HolyEquipParam::HOLY_EQUIP_STATUS_FIGHTING)
				{
					if (out_index != NULL) *out_index = i;
					if (NULL != out_index_in_bag) *out_index_in_bag = index;
					return m_holy_grid_list[index];
				}
			}
		}
	}
	return NULL;
}

int HolyEquipManager::GetHolyItemEmptyBag()
{
	for (int i = 0; i < ItemNamespace::MAX_HOLY_EQUIP_BAG_NUM; i++)
	{
		if (NULL == m_holy_grid_list[i] || m_holy_grid_list[i]->Invalid()) return i;
	}
	return -1;
}

bool HolyEquipManager::HasHolyItemBagNum(int grid_num)
{
	if (grid_num < 0) return false;

	int count = 0;
	for (int i = 0; i < ItemNamespace::MAX_HOLY_EQUIP_BAG_NUM; i++)
	{
		if (NULL != m_holy_grid_list[i] && !m_holy_grid_list[i]->Invalid())continue;
		count++;
	}

	return count >= grid_num;
}

bool HolyEquipManager::AddNewHolyItem(const ItemDataWrapper * new_item, int outside_reason, int * out_idx, int reason_type)
{
	if (NULL == new_item) return false;

	const ItemBase * new_ib = ITEMPOOL->GetItem(new_item->item_id);
	if (NULL == new_ib) return false;

	if (new_ib->GetItemType() != I_TYPE_HOLY_EQUIP)return false;

	int empty_bag_idx = this->GetHolyItemEmptyBag();
	if (empty_bag_idx < 0)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_HOLY_EQUIP_BAG_NO_SPACE);
		return false;
	}
	
	if (NULL == m_holy_grid_list[empty_bag_idx])
	{
		m_holy_grid_list[empty_bag_idx] = new ItemGridData(); 
	}
	
	m_holy_grid_list[empty_bag_idx]->Set(*new_item);
	if (out_idx != NULL)*out_idx = empty_bag_idx;
	this->ReCalHolyBaseAttr(empty_bag_idx);
	this->SetHolyBagDirty(empty_bag_idx, true, reason_type);

	gamelog::g_sheng_qi_system.printf(LL_INFO, "%s role[%d, %s] empty_bag_idx[%d] equip_param[%s]", __FUNCTION__,
		m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), empty_bag_idx, m_holy_grid_list[empty_bag_idx]->param->GetParamLog(I_TYPE_HOLY_EQUIP));
	ROLE_LOG_QUICK6(LOG_TYPE_HOLY_EQUIP, m_role_module_mgr->GetRole(), new_item->item_id, outside_reason, __FUNCTION__, m_holy_grid_list[empty_bag_idx]->param->GetParamLog(new_ib->GetItemType()));

	EventHandler::Instance().OnGetHolyEquip(m_role_module_mgr->GetRole(), new_ib->GetItemId());

	return true;
}

bool HolyEquipManager::AddNewHolyItem(const ItemGridData * new_item, int outside_reason, int * out_idx)
{
	if (NULL == new_item) return false;

	const ItemBase * new_ib = ITEMPOOL->GetItem(new_item->item_id);
	if (NULL == new_ib)return false;

	if (new_ib->GetItemType() != I_TYPE_HOLY_EQUIP)return false;

	int empty_bag_idx = this->GetHolyItemEmptyBag();
	if (empty_bag_idx < 0)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_HOLY_EQUIP_BAG_NO_SPACE);
		return false;
	}

	if (NULL == m_holy_grid_list[empty_bag_idx])
	{
		m_holy_grid_list[empty_bag_idx] = new ItemGridData();
	}

	ItemDataWrapper item_wrapper(new_item->item_id, 1, (short)new_item->is_bind, 1,
		new_item->param->data, new_item->invalid_time, 0, new_item->item_unique_id, new_item->is_market_buy_flag);

	m_holy_grid_list[empty_bag_idx]->Set(item_wrapper);
	if (out_idx != NULL)*out_idx = empty_bag_idx;
	this->ReCalHolyBaseAttr(empty_bag_idx);
	this->SetHolyBagDirty(empty_bag_idx, true);
	
	gamelog::g_sheng_qi_system.printf(LL_INFO, "%s role[%d, %s] empty_bag_idx[%d] equip_param[%s]", __FUNCTION__,
		m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), empty_bag_idx, new_item->param->GetParamLog(I_TYPE_HOLY_EQUIP));
	ROLE_LOG_QUICK6(LOG_TYPE_HOLY_EQUIP, m_role_module_mgr->GetRole(), new_item->item_id, outside_reason, __FUNCTION__, m_holy_grid_list[empty_bag_idx]->param->GetParamLog(new_ib->GetItemType()));

	EventHandler::Instance().OnGetHolyEquip(m_role_module_mgr->GetRole(), new_ib->GetItemId());

	return true;
}

const ItemGridData * HolyEquipManager::AddHolyItem(ItemID new_id, int outside_reason, int * out_idx, int reason_type)
{
	int entry_idx = this->GetHolyItemEmptyBag();
	if (entry_idx < 0)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_HOLY_EQUIP_BAG_NO_SPACE);
		return NULL;
	}

	const ItemBase * new_item = ITEMPOOL->GetItem(new_id);
	if (NULL == new_item)return NULL;

	if (new_item->GetItemType() != I_TYPE_HOLY_EQUIP)return NULL;

	const HolyItem * ib = (const HolyItem *)new_item;
	
	ItemDataWrapper compound_item; compound_item.Reset();
	compound_item.item_id = new_id;
	compound_item.num = 1;
	compound_item.is_bind = 0;
	UniqueIDGenerator::Instance()->GenUniqueID(&compound_item.item_unique_id);
	static NetValueItemParam p_nvip; p_nvip.Reset();
	int put_reason = 0;
	{
		ib->RandomNetValueOnPutItem(m_role_module_mgr->GetRole(), &p_nvip, put_reason);
		p_nvip.GetDataByItemType(ib->GetItemType(), compound_item.param_data);
		compound_item.has_param = 1;
		ROLE_LOG_QUICK6(LOG_TYPE_HOLY_EQUIP, m_role_module_mgr->GetRole(), new_id, outside_reason, __FUNCTION__, p_nvip.GetParamLog(new_item->GetItemType()));
	}

	if (m_holy_grid_list[entry_idx] == NULL)
	{
		m_holy_grid_list[entry_idx] = new ItemGridData();
	}

	m_holy_grid_list[entry_idx]->Set(compound_item);
	if (out_idx != NULL)*out_idx = entry_idx;

	this->ReCalHolyBaseAttr(entry_idx);
	this->SetHolyBagDirty(entry_idx, true, reason_type);

	gamelog::g_sheng_qi_system.printf(LL_INFO, "%s user[%d, %s] entry_idx[%d] item_id[%d] equip_param[%s]", __FUNCTION__,
		m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), entry_idx, new_id, m_holy_grid_list[entry_idx]->param->GetParamLog(I_TYPE_HOLY_EQUIP));

	EventHandler::Instance().OnGetHolyEquip(m_role_module_mgr->GetRole(), new_id);

	if (!m_param.first_gain)
	{
		m_param.first_gain = 1;
		this->SendFirstGainFlagInfo();
	}

	return m_holy_grid_list[entry_idx];
}

ItemGridData * HolyEquipManager::GetHolyItemGridDataKnapsack(int index)
{
	if (index < 0 || index >= ARRAY_LENGTH(m_holy_grid_list))
	{
		return NULL;
	}

	if (NULL == m_holy_grid_list[index] || m_holy_grid_list[index]->Invalid())
	{
		return NULL;
	}

	return m_holy_grid_list[index];
}

int HolyEquipManager::GetFuShiEmptyBag()
{
	for (int i = 0; i < ItemNamespace::MAX_RUNE_STONE_BAG_NUM; i++)
	{
		if (NULL == m_fushi_grid_list[i] || m_fushi_grid_list[i]->Invalid()) return i;
	}
	return -1;
}

bool HolyEquipManager::HasFuShiBagNum(int grid_num)
{
	if (grid_num < 0) return false;

	int count = 0;
	for (int i = 0; i < ItemNamespace::MAX_RUNE_STONE_BAG_NUM; i++)
	{
		if (NULL != m_fushi_grid_list[i] && !m_fushi_grid_list[i]->Invalid())continue;
		count++;
	}

	return count >= grid_num;
}

bool HolyEquipManager::AddNewFuShiItem(const ItemDataWrapper * new_item, int outside_reason, int * out_idx)
{
	if (NULL == new_item) return false;

	const ItemBase * new_ib = ITEMPOOL->GetItem(new_item->item_id);
	if (NULL == new_ib) return false;

	if (new_ib->GetItemType() != I_TYPE_RUNE_STONE)return false;

	int empty_bag_idx = this->GetFuShiEmptyBag();
	if (empty_bag_idx < 0)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_FU_SHI_BAG_NO_SPACE);
		return false;
	}

	if (NULL == m_fushi_grid_list[empty_bag_idx])
	{
		m_fushi_grid_list[empty_bag_idx] = new ItemGridData();
	}

	m_fushi_grid_list[empty_bag_idx]->Set(*new_item);
	if (out_idx != NULL)*out_idx = empty_bag_idx;
	this->SetFuShiBagDirty(empty_bag_idx);

	gamelog::g_sheng_qi_system.printf(LL_INFO, "%s role[%d, %s] empty_bag_idx[%d] equip_param[%s]", __FUNCTION__,
		m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), empty_bag_idx, m_fushi_grid_list[empty_bag_idx]->param->GetParamLog(I_TYPE_RUNE_STONE));
	ROLE_LOG_QUICK6(LOG_TYPE_FU_SHI, m_role_module_mgr->GetRole(), new_item->item_id, outside_reason, __FUNCTION__, m_fushi_grid_list[empty_bag_idx]->param->GetParamLog(new_ib->GetItemType()));

	return true;
}

bool HolyEquipManager::AddNewFuShiItem(const ItemGridData * new_item, int outside_reason, int * out_idx)
{
	if (NULL == new_item) return false;

	const ItemBase * new_ib = ITEMPOOL->GetItem(new_item->item_id);
	if (NULL == new_ib) return false;

	if (new_ib->GetItemType() != I_TYPE_RUNE_STONE)return false;

	int empty_bag_idx = this->GetFuShiEmptyBag();
	if (empty_bag_idx < 0)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_FU_SHI_BAG_NO_SPACE);
		return false;
	}

	if (NULL == m_fushi_grid_list[empty_bag_idx])
	{
		m_fushi_grid_list[empty_bag_idx] = new ItemGridData();
	}

	ItemDataWrapper item_wrapper(new_item->item_id, 1, (short)new_item->is_bind, 1,
		new_item->param->data, new_item->invalid_time, 0, new_item->item_unique_id, new_item->is_market_buy_flag);

	m_fushi_grid_list[empty_bag_idx]->Set(item_wrapper);
	if (out_idx != NULL)*out_idx = empty_bag_idx;
	this->SetFuShiBagDirty(empty_bag_idx);

	gamelog::g_sheng_qi_system.printf(LL_INFO, "%s role[%d, %s] empty_bag_idx[%d] equip_param[%s]", __FUNCTION__,
		m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), empty_bag_idx, new_item->param->GetParamLog(I_TYPE_RUNE_STONE));
	ROLE_LOG_QUICK6(LOG_TYPE_FU_SHI, m_role_module_mgr->GetRole(), new_item->item_id, outside_reason, __FUNCTION__, m_fushi_grid_list[empty_bag_idx]->param->GetParamLog(new_ib->GetItemType()));

	return true;
}

const ItemGridData * HolyEquipManager::AddFuShiItem(ItemID new_id,int outside_reason, int * out_idx)
{
	int entry_idx = this->GetFuShiEmptyBag();
	if (entry_idx < 0)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_FU_SHI_BAG_NO_SPACE);
		return NULL;
	}

	const ItemBase * new_item = ITEMPOOL->GetItem(new_id);
	if (NULL == new_item)return NULL;

	if (new_item->GetItemType() != I_TYPE_RUNE_STONE)return NULL;

	const RuneStoneItem * ib = (const RuneStoneItem *)new_item;

	ItemDataWrapper compound_equip; compound_equip.Reset();
	compound_equip.item_id = new_id;
	compound_equip.num = 1;
	compound_equip.is_bind = 0;
	UniqueIDGenerator::Instance()->GenUniqueID(&compound_equip.item_unique_id);
	static NetValueItemParam p_nvip; p_nvip.Reset();
	int put_reason = 0;
	{
		ib->RandomNetValueOnPutItem(m_role_module_mgr->GetRole(), &p_nvip, put_reason);
		p_nvip.GetDataByItemType(ib->GetItemType(), compound_equip.param_data);
		compound_equip.has_param = 1;
		ROLE_LOG_QUICK6(LOG_TYPE_FU_SHI, m_role_module_mgr->GetRole(), compound_equip.item_id, outside_reason, __FUNCTION__, p_nvip.GetParamLog(new_item->GetItemType()));
	}

	if (m_fushi_grid_list[entry_idx] == NULL)
	{
		m_fushi_grid_list[entry_idx] = new ItemGridData();
	}

	m_fushi_grid_list[entry_idx]->Set(compound_equip);
	if (out_idx != NULL)*out_idx = entry_idx;

	gamelog::g_sheng_qi_system.printf(LL_INFO, "%s user[%d,%s] entry_idx[%d] item_id[%d] equip_param[%s]", __FUNCTION__,
		m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), entry_idx, new_id, m_fushi_grid_list[entry_idx]->param->GetParamLog(I_TYPE_RUNE_STONE));

	this->CalculateScore(entry_idx);
	this->ReCalHolyBaseAttr(entry_idx);
	this->SetFuShiBagDirty(entry_idx, HOLY_EQUIP_SEND_REASON_IN);

	return m_fushi_grid_list[entry_idx];
}

ItemGridData * HolyEquipManager::GetFuShiGridDataKnapsack(int index)
{
	if (index < 0 || index >= ARRAY_LENGTH(m_fushi_grid_list))
	{
		return NULL;
	}

	if (NULL == m_fushi_grid_list[index] || m_fushi_grid_list[index]->Invalid())
	{
		return NULL;
	}

	return m_fushi_grid_list[index];
}
