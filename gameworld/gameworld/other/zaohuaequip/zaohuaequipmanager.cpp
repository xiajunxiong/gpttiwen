#include "zaohuaequipmanager.hpp"
#include "engineadapter.h"
#include "config/logicconfigmanager.hpp"
#include "gamelog.h"
#include "servercommon/logdef.h"
#include "item/zaohua/zaohuaitem.hpp"
#include "servercommon/errornum.h"
#include "other/workshop/suitlistconfig.hpp"
#include "servercommon/serverconfig/battleconfig/battle_special_effect_pool.hpp"
#include "servercommon/struct/battle/battle_special_effect_def.hpp"
#include "global/marketmanager/marketmanager.hpp"
#include "servercommon/noticenum.h"
#include "servercommon/uniqueidgenerator.hpp"
#include "protocol/msgzaohuaequipment.h"
#include "other/funopen/funopen.hpp"

ZaoHuaEquipManager::ZaoHuaEquipManager() : m_role_module_mgr(NULL), m_bag_is_change(false)
{
	m_attrs_add.Reset();
	m_capability = 0;
	m_next_order_tamp = 0;
	m_wear_level = 0;
	memset(m_potential_add, 0, sizeof(m_potential_add));
	m_wear_grid_num = 0;
	for (int i = 0; i < ItemNamespace::MAX_ZAO_HUA_EQUIPMENT_GRID_NUM; ++i)
	{
		m_wear_old_state[i] = false;
		m_wear_dirty_mark[i] = false;
		m_wear_grid_list[i] = NULL;
	}

	m_grid_num = 0;
	for (int i = 0; i < ItemNamespace::MAX_ZAO_HUA_BAG_NUM; ++i)
	{
		m_old_state[i] = false;
		m_dirty_mark[i] = false;
		m_grid_list[i] = NULL;
	}
}

ZaoHuaEquipManager::~ZaoHuaEquipManager()
{
	for (int i = 0; i < ItemNamespace::MAX_ZAO_HUA_EQUIPMENT_GRID_NUM; ++i)
	{
		if (NULL != m_wear_grid_list[i])
		{
			this->ClearWearGrid(i);
		}
	}

	for (int i = 0; i < ItemNamespace::MAX_ZAO_HUA_BAG_NUM; ++i)
	{
		if (NULL != m_grid_list[i])
		{
			this->ClearGrid(i);
		}
	}
}

void ZaoHuaEquipManager::Init(RoleModuleManager * role_module_manager, const ZaoHuaRefineParam & refine_param, const ZaoHuaLongZhuParam & long_zhu_param, int param_list_num, const ItemListParam::ItemListData * param_data_list[])
{
	m_role_module_mgr = role_module_manager;
	m_refine_param = refine_param;
	m_long_zhu_param = long_zhu_param;

	for (int i = 0; i < param_list_num && i < ItemNamespace::MAX_ZAO_HUA_EQUIPMENT_GRID_NUM + ItemNamespace::MAX_ZAO_HUA_BAG_NUM; i++)
	{
		if (NULL == param_data_list[i]) continue;
		if (param_data_list[i]->index >= ItemNamespace::ZAO_HUA_EQUIPMENT_INDEX_BEGIN && param_data_list[i]->index < ItemNamespace::ZAO_HUA_EQUIPMENT_INDEX_END)
		{
			int equip_index = param_data_list[i]->index - ItemNamespace::ZAO_HUA_EQUIPMENT_INDEX_BEGIN; // index经过转换则为装备部位
			if (equip_index < 0 || equip_index >= ZaoHuaItem::Z_TYPE_MAX) continue;

			if (NULL == m_wear_grid_list[equip_index])m_wear_grid_list[equip_index] = new ItemGridData();
			m_wear_grid_list[equip_index]->Set(param_data_list[i]->item_wrapper);
			m_wear_old_state[equip_index] = true;
			gamelog::g_zao_hua_equip.printf(LL_INFO, "%s user[%d,%s] equip_index[%d] equip_param[%s]", __FUNCTION__,
				m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), equip_index, m_wear_grid_list[equip_index]->param->GetParamLog(I_TYPE_ZAO_HUA_EQUIP));
		}
		else if (param_data_list[i]->index >= ItemNamespace::ZAO_HUA_BAG_INDEX_BEGIN && param_data_list[i]->index < ItemNamespace::ZAO_HUA_BAG_INDEX_END)
		{
			int equip_index = param_data_list[i]->index - ItemNamespace::ZAO_HUA_BAG_INDEX_BEGIN; // index经过转换则为背包下表
			if (equip_index < 0 || equip_index >= ItemNamespace::MAX_ZAO_HUA_BAG_NUM) continue;

			if (NULL == m_grid_list[equip_index])m_grid_list[equip_index] = new ItemGridData();
			m_grid_list[equip_index]->Set(param_data_list[i]->item_wrapper);
			m_old_state[equip_index] = true;
			gamelog::g_zao_hua_equip.printf(LL_INFO, "%s user[%d,%s] equip_index[%d] equip_param[%s]", __FUNCTION__,
				m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), equip_index, m_grid_list[equip_index]->param->GetParamLog(I_TYPE_ZAO_HUA_EQUIP));
		}
	}
}

void ZaoHuaEquipManager::GetInitParam(ZaoHuaRefineParam * refine_param, ZaoHuaLongZhuParam * long_zhu_param)
{
	*refine_param = m_refine_param;
	*long_zhu_param = m_long_zhu_param;
}

void ZaoHuaEquipManager::OnLogin()
{
	this->SendBagList();
	this->SendWearList();
	this->SendWearApp();
	this->SendCommonInfo();
}

void ZaoHuaEquipManager::ChangeProfession(int old_prof, int new_prof)
{
	bool need_send = false;
	for (int equip_index = 0;equip_index < ZaoHuaItem::Z_TYPE_MAX; equip_index++)//身上变更
	{
		if (NULL == m_wear_grid_list[equip_index] || m_wear_grid_list[equip_index]->Invalid())continue;

		ItemGridData* equip = m_wear_grid_list[equip_index];
		const ItemBase * i_b = ITEMPOOL->GetItem(equip->item_id);
		if (NULL == i_b || i_b->GetItemType() != I_TYPE_ZAO_HUA_EQUIP)continue;
		const ZaoHuaItem * z_b = (const ZaoHuaItem *)i_b;

		const ZaoHuaChangeJobCfg * change_cfg = LOGIC_CONFIG->GetZaoHuaEquipConfig()->GetChangeJobCfg(z_b->GetEquipLevel(), new_prof / PROFESSION_BASE, z_b->GetColor(), z_b->GetEquipType());
		if (NULL == change_cfg)continue;

		equip->item_id = change_cfg->id;
		this->SetZaoHuaEquipGridDirty(equip_index, false);
		need_send = true;
	}
	if (need_send)
	{
		m_role_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_ZAO_HUA, __FUNCTION__);
		this->SendWearList();
	}
	need_send = false;
	for (int bag_index = 0;bag_index < ItemNamespace::MAX_ZAO_HUA_BAG_NUM; bag_index++)//背包变更
	{
		if (NULL == m_grid_list[bag_index] || m_grid_list[bag_index]->Invalid())continue;
		ItemGridData* bag = m_grid_list[bag_index];
		const ItemBase * i_b = ITEMPOOL->GetItem(bag->item_id);
		if (NULL == i_b || i_b->GetItemType() != I_TYPE_ZAO_HUA_EQUIP)continue;
		const ZaoHuaItem * z_b = (const ZaoHuaItem *)i_b;

		const ZaoHuaChangeJobCfg * change_cfg = LOGIC_CONFIG->GetZaoHuaEquipConfig()->GetChangeJobCfg(z_b->GetEquipLevel(), new_prof / PROFESSION_BASE, z_b->GetColor(), z_b->GetEquipType());
		if (NULL == change_cfg)continue;

		bag->item_id = change_cfg->id;
		this->SetZaoHuaBagDirty(bag_index, ZAO_HUA_SEND_REASON_NORMAL, PUT_REASON_NO_NOTICE, false);
		need_send = true;
	}
	if (need_send)this->SendBagList();
}

bool ZaoHuaEquipManager::GetZaoHuaEquipGridData(short equip_index, ItemGridData * equip_data)
{
	if (NULL == equip_data || equip_index < 0 || equip_index >= ZaoHuaItem::Z_TYPE_MAX) return false;

	if (NULL == m_wear_grid_list[equip_index] || m_wear_grid_list[equip_index]->Invalid()) return false;

	equip_data->Set(*m_wear_grid_list[equip_index]);

	return true;
}

bool ZaoHuaEquipManager::GetZaoHuaEquipGridData(short equip_index, ItemDataWrapper * equip_data)
{
	if (NULL == equip_data || equip_index < 0 || equip_index >= ZaoHuaItem::Z_TYPE_MAX) return false;

	if (NULL == m_wear_grid_list[equip_index] || m_wear_grid_list[equip_index]->Invalid()) return false;

	m_wear_grid_list[equip_index]->WrapTo(equip_data);

	return true;
}

void ZaoHuaEquipManager::GetZaoHuaEquipWearList(int * count, ItemListParam::ItemListData * save_list)
{
	int tmp_count = 0;

	for (int i = 0; i < ZaoHuaItem::Z_TYPE_MAX; ++i)
	{
		if (NULL == m_wear_grid_list[i] || m_wear_grid_list[i]->Invalid() || NULL == m_wear_grid_list[i]->param) continue;

		save_list[tmp_count].index = i + ItemNamespace::ZAO_HUA_EQUIPMENT_INDEX_BEGIN; // 装备存储时的index从这个开始
		m_wear_grid_list[i]->WrapTo(&save_list[tmp_count].item_wrapper);

		save_list[tmp_count].change_state_item = structcommon::CS_NONE;

		if (++tmp_count >= *count) break;
	}

	*count = tmp_count;
}

void ZaoHuaEquipManager::GetZaoHuaEquipBagList(int * count, ItemListParam::ItemListData * save_list)
{
	int tmp_count = 0;

	for (int i = 0; i < ItemNamespace::MAX_ZAO_HUA_BAG_NUM; ++i)
	{
		if (NULL == m_wear_grid_list[i] || m_wear_grid_list[i]->Invalid() || NULL == m_wear_grid_list[i]->param) continue;

		save_list[tmp_count].index = i + ItemNamespace::ZAO_HUA_BAG_INDEX_BEGIN; // 装备存储时的index从这个开始
		m_wear_grid_list[i]->WrapTo(&save_list[tmp_count].item_wrapper);

		save_list[tmp_count].change_state_item = structcommon::CS_NONE;

		if (++tmp_count >= *count) break;
	}

	*count = tmp_count;
}

void ZaoHuaEquipManager::PutOnEquip(int index)
{
	const ItemGridData* node = this->GetBagNode(index);
	if (NULL == node || node->Invalid())return;

	const ItemBase *ib = ITEMPOOL->GetItem(node->item_id);
	if (ib == NULL || ib->GetItemType() != I_TYPE_ZAO_HUA_EQUIP)return;

	const ZaoHuaItem * z_ib = (const ZaoHuaItem*)ib;
	int equip_type = z_ib->GetEquipType();

	int limit_prof = z_ib->GetLimitProf();
	if (limit_prof != m_role_module_mgr->GetRole()->GetBaseProfession())
	{
		m_role_module_mgr->NoticeNum(errornum::EN_ITEM_PROF_LIMIT);
		return;
	}

	if (equip_type < ZaoHuaItem::Z_TYPE_1 || equip_type >= ZaoHuaItem::Z_TYPE_MAX)return;


	if (m_wear_grid_list[equip_type] != NULL)//有装备先脱下
	{
		bool ret = this->TakeOffEquip(equip_type);
		if (!ret)return;
	}

	if (m_wear_grid_list[equip_type] == NULL)
	{
		m_wear_grid_list[equip_type] = new ItemGridData();
	}

	gamelog::g_zao_hua_equip.printf(LL_INFO, "%s user[%d,%s] equip_type[%d],item_id[%d] equip_param[%s]", __FUNCTION__,
		m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), equip_type, node->item_id, node->param->GetParamLog(I_TYPE_ZAO_HUA_EQUIP));
	ItemDataWrapper item_wrapper(node->item_id, 1, (short)node->is_bind, 1,
		node->param->data, node->invalid_time, 0, node->item_unique_id, node->is_market_buy_flag);
	m_wear_grid_list[equip_type]->Set(item_wrapper);
	this->ClearGrid(index);
	this->SetZaoHuaBagDirty(index);
	this->SetZaoHuaEquipGridDirty(equip_type);
	m_role_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_ZAO_HUA, __FUNCTION__);
	m_role_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_ATTRIBUTE_PLAN, __FUNCTION__);
}

bool ZaoHuaEquipManager::TakeOffEquip(int index)
{
	if (index < ZaoHuaItem::Z_TYPE_1 || index >= ZaoHuaItem::Z_TYPE_MAX)return false;

	ItemGridData* wear_node = m_wear_grid_list[index];

	if (wear_node == NULL || wear_node->Invalid())
	{
		m_role_module_mgr->NoticeNum(errornum::EN_ZAO_HUA_NO_WEAR_EQUIP);
		return false;
	}

	bool ret = this->AddNewZaoHuaItem(wear_node);

	if (!ret)return false;
	gamelog::g_zao_hua_equip.printf(LL_INFO, "%s user[%d,%s] equip_type[%d] item_id[%d] equip_param[%s]", __FUNCTION__,
		m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), index,wear_node->item_id, wear_node->param->GetParamLog(I_TYPE_ZAO_HUA_EQUIP));
	this->ClearWearGrid(index);
	this->SetZaoHuaEquipGridDirty(index);
	m_role_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_ZAO_HUA, __FUNCTION__);
	m_role_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_ATTRIBUTE_PLAN, __FUNCTION__);
	return true;
}

int ZaoHuaEquipManager::GetEmptyBag()
{
	for (int i = 0; i < ItemNamespace::MAX_ZAO_HUA_BAG_NUM; i++)
	{
		if (NULL == m_grid_list[i] || m_grid_list[i]->Invalid())return i;
	}
	return -1;
}

bool ZaoHuaEquipManager::HasBagNum(int grid_num)
{
	int bag_num = 0;
	for (int i = 0; i < ItemNamespace::MAX_ZAO_HUA_BAG_NUM; i++)
	{
		if (NULL != m_grid_list[i] && !m_grid_list[i]->Invalid())continue;
		bag_num += 1;
	}
	return bag_num >= grid_num;
}

bool ZaoHuaEquipManager::AddNewZaoHuaItem(const ItemDataWrapper * new_item, int *out_idx/* = NULL*/)
{
	const ItemBase * new_ib = ITEMPOOL->GetItem(new_item->item_id);
	if (NULL == new_ib)return false;

	if (new_ib->GetItemType() != I_TYPE_ZAO_HUA_EQUIP)return false;

	int empty_bag_idx = this->GetEmptyBag();
	if (empty_bag_idx < 0)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_PET_BAG_EXTEND_LIMIT);
		return false;
	}

	if (new_item == NULL)return false;
	if (NULL == m_grid_list[empty_bag_idx])
	{
		m_grid_list[empty_bag_idx] = new ItemGridData();
	}

	m_grid_list[empty_bag_idx]->Set(*new_item);
	if (out_idx != NULL)*out_idx = empty_bag_idx;
	this->SetZaoHuaBagDirty(empty_bag_idx, ZAO_HUA_SEND_REASON_IN, PUT_REASON_ZAO_HUA_COMPOSE);
	this->CheckAddItem(new_ib);
	gamelog::g_zao_hua_equip.printf(LL_INFO, "%s user[%d,%s] entry_idx[%d] item_id[%d] equip_param[%s]", __FUNCTION__,
		m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), empty_bag_idx, m_grid_list[empty_bag_idx]->item_id, m_grid_list[empty_bag_idx]->param->GetParamLog(I_TYPE_ZAO_HUA_EQUIP));
	return false;
}

bool ZaoHuaEquipManager::AddNewZaoHuaItem(const ItemGridData * new_item, int * out_idx)
{
	const ItemBase * new_ib = ITEMPOOL->GetItem(new_item->item_id);
	if (NULL == new_ib)return false;

	if (new_ib->GetItemType() != I_TYPE_ZAO_HUA_EQUIP)return false;

	int empty_bag_idx = this->GetEmptyBag();
	if (empty_bag_idx < 0)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_ZAO_HUA_KNAPSACK_NO_SPACE);
		return false;
	}

	if (new_item == NULL)return false;
	if (NULL == m_grid_list[empty_bag_idx])
	{
		m_grid_list[empty_bag_idx] = new ItemGridData();
	}

	ItemDataWrapper item_wrapper(new_item->item_id, 1, (short)new_item->is_bind, 1,
		new_item->param->data, new_item->invalid_time, 0, new_item->item_unique_id, new_item->is_market_buy_flag);

	m_grid_list[empty_bag_idx]->Set(item_wrapper);

	gamelog::g_zao_hua_equip.printf(LL_INFO, "%s user[%d,%s] empty_bag_idx[%d] item_id[%d] equip_param[%s]", __FUNCTION__,
		m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), empty_bag_idx,new_item->item_id, new_item->param->GetParamLog(I_TYPE_ZAO_HUA_EQUIP));

	if (out_idx != NULL)*out_idx = empty_bag_idx;
	this->SetZaoHuaBagDirty(empty_bag_idx, ZAO_HUA_SEND_REASON_IN, PUT_REASON_ZAO_HUA_COMPOSE);
	this->CheckAddItem(new_ib);
	return true;
}

const ItemGridData * ZaoHuaEquipManager::AddZaoHuaItem(ItemID new_id, bool is_from_compose, int *out_idx/* = NULL*/)
{
	int entry_idx = this->GetEmptyBag();
	if (entry_idx < 0)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_ZAO_HUA_KNAPSACK_NO_SPACE);
		return NULL;
	}

	const ItemBase * new_item = ITEMPOOL->GetItem(new_id);
	if (NULL == new_item)return NULL;

	if (new_item->GetItemType() != I_TYPE_ZAO_HUA_EQUIP)return NULL;

	const ZaoHuaItem * ib = (const ZaoHuaItem *)new_item;

	ItemDataWrapper compound_equip; compound_equip.Reset();
	compound_equip.item_id = new_id;
	compound_equip.num = 1;
	compound_equip.is_bind = 0;
	UniqueIDGenerator::Instance()->GenUniqueID(&compound_equip.item_unique_id);
	NetValueItemParam p_nvip; p_nvip.Reset();
	int put_reason = 0;
	{
		ib->RandomNetValueOnPutItem(m_role_module_mgr->GetRole(), &p_nvip, put_reason);
		p_nvip.GetDataByItemType(ib->GetItemType(), compound_equip.param_data);
		compound_equip.has_param = 1;
		ROLE_LOG_QUICK6(LOG_TYPE_ZAO_HUA_ADD_ITEM, m_role_module_mgr->GetRole(), compound_equip.item_id, 0, p_nvip.GetParamLog(new_item->GetItemType()), __FUNCTION__);
	}

	if (m_grid_list[entry_idx] == NULL)
	{
		m_grid_list[entry_idx] = new ItemGridData();
	}

	m_grid_list[entry_idx]->Set(compound_equip);
	gamelog::g_zao_hua_equip.printf(LL_INFO, "%s user[%d,%s] entry_idx[%d] item_id[%d] equip_param[%s]", __FUNCTION__,
		m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), entry_idx, m_grid_list[entry_idx]->item_id, m_grid_list[entry_idx]->param->GetParamLog(I_TYPE_ZAO_HUA_EQUIP));

	this->SetZaoHuaBagDirty(entry_idx, ZAO_HUA_SEND_REASON_IN, is_from_compose ? PUT_REASON_ZAO_HUA_COMPOSE : PUT_REASON_NO_NOTICE, true);
	this->CheckAddItem(new_item);
	return m_grid_list[entry_idx];
}

ItemGridData * ZaoHuaEquipManager::GetItemGridData(int index)
{
	if (index < 0 || index >= ARRAY_LENGTH(m_wear_grid_list))
	{
		return NULL;
	}

	if (m_wear_grid_list[index] == NULL)
	{
		return NULL;
	}

	if (m_wear_grid_list[index]->Invalid())
	{
		return NULL;
	}

	return m_wear_grid_list[index];
}

ItemGridData * ZaoHuaEquipManager::GetItemGridDataKnapsack(int index)
{
	if (index < 0 || index >= ARRAY_LENGTH(m_grid_list))
	{
		return NULL;
	}

	if (m_grid_list[index] == NULL)
	{
		return NULL;
	}

	if (m_grid_list[index]->Invalid())
	{
		return NULL;
	}

	return m_grid_list[index];
}

void ZaoHuaEquipManager::ClearAll()
{
	for (int i = 0; i < ItemNamespace::MAX_ZAO_HUA_BAG_NUM; i++)
	{
		const ItemGridData* bag_node = this->GetBagNode(i);
		if (NULL != bag_node) this->ConsumeItem(i, __FUNCTION__);
	}
}

bool ZaoHuaEquipManager::ConsumeItem(int consume_idx, const char * reason)
{
	const ItemGridData* bag_node = this->GetBagNode(consume_idx);
	if (NULL == bag_node) return false;
	const ItemBase * ib = ITEMPOOL->GetItem(bag_node->item_id);
	if (NULL == ib) return false;

	const char *item_param_string = NULL;
	item_param_string = bag_node->param->GetParamLog(ib->GetItemType());
	this->ClearGrid(consume_idx);
	this->SetZaoHuaBagDirty(consume_idx, ZAO_HUA_SEND_REASON_OUT, PUT_REASON_NO_NOTICE, true);
	ROLE_LOG_QUICK6(LOG_TYPE_ITEM_CONSUME, m_role_module_mgr->GetRole(), bag_node->item_id, bag_node->num, reason, item_param_string);
	gamelog::g_zao_hua_equip.printf(LL_INFO, "%s user[%d,%s] consume_idx[%d] consume_item_id[%d] equip_param[%s]", __FUNCTION__,
		m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), consume_idx, ib->GetItemId(), item_param_string);
	return true;
}

bool ZaoHuaEquipManager::InOrder()
{
	UInt32 now_time = static_cast<UInt32>(EngineAdapter::Instance().Time());
	if (m_next_order_tamp > 0 && m_next_order_tamp > now_time)return false;
	ZaoHuaSortStruct sort_stru[ItemNamespace::MAX_ZAO_HUA_BAG_NUM];
	memset(sort_stru, 0, sizeof(sort_stru));

	int count = 0;

	for (int i = 0;i < ItemNamespace::MAX_ZAO_HUA_BAG_NUM; i++)
	{
		if (NULL != m_grid_list[i] && !m_grid_list[i]->Invalid())
		{
			const ItemBase *itembase = ITEMPOOL->GetItem(m_grid_list[i]->item_id);
			if (NULL == itembase || itembase->GetItemType() != I_TYPE_ZAO_HUA_EQUIP) continue;
			const ZaoHuaItem * z_b = (const ZaoHuaItem *)itembase;
			long long prior_id = 0;
			prior_id = ZaoHuaSortStruct::GetSupplyPriorId(z_b->GetEquipLevel(), itembase->GetItemType(), itembase->GetItemSubType(), itembase->GetColor(), z_b->GetEquipType(), itembase->GetItemId(), 0);
			sort_stru[count].Set(prior_id, m_grid_list[i]->is_bind, i);
			gamelog::g_zao_hua_equip.printf(LL_INFO, "%s [old_list] user[%d,%s] i[%d] item_id[%d]", __FUNCTION__,
				m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), i, m_grid_list[i]->item_id);
			++count;
		}
		else
		{
			long long prior_id = 0;
			prior_id = ZaoHuaSortStruct::GetSupplyPriorId(0, 0, 0, 0, 0, 0, 0);
			sort_stru[count].Set(prior_id, 0, i);

			++count;
		}
	}

	if (count > ItemNamespace::MAX_ZAO_HUA_BAG_NUM) return false;
	qsort(sort_stru, count, sizeof(ZaoHuaSortStruct), ZaoHuaSortCompare);

	ItemDataWrapper tmp_wrapper_list[ItemNamespace::MAX_ZAO_HUA_BAG_NUM];
	for (int i = 0; i < ItemNamespace::MAX_ZAO_HUA_BAG_NUM; i++)
	{
		tmp_wrapper_list[i].Reset();
	}

	for (int i = 0; i < count; i++) // 将排序后的物品存到临时空间里,并且检查真实背包数量
	{
		int src_index = sort_stru[i].src_index;
		if (NULL == m_grid_list[src_index])continue;
		m_grid_list[src_index]->WrapTo(&tmp_wrapper_list[i]);

		// 把原位置清空
		this->ClearGrid(src_index);
		this->SetZaoHuaBagDirty(src_index, ZAO_HUA_SEND_REASON_CLEARUP, false);
	}
	int real_count = 0;
	for (int i = 0; i < count; i++) // 从临时空间拷贝回来
	{
		if (tmp_wrapper_list[i].Invalid())continue;
		if (NULL == m_grid_list[i])
		{
			m_grid_list[i] = new ItemGridData();
		}
		m_grid_list[real_count]->Set(tmp_wrapper_list[i]);
		this->SetZaoHuaBagDirty(real_count, ZAO_HUA_SEND_REASON_CLEARUP, false);
		gamelog::g_zao_hua_equip.printf(LL_INFO, "%s [new_list] user[%d,%s] real_count[%d] item_id[%d]", __FUNCTION__,
			m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), real_count, m_grid_list[i]->item_id);
		real_count++;
	}
	m_grid_num = real_count;

	for (int i = real_count; i < count; i++) // 将后面的全部设为空
	{
		if (NULL == m_grid_list[i] || m_grid_list[i]->Invalid()) continue;

		this->ClearGrid(i);
		this->SetZaoHuaBagDirty(i, ZAO_HUA_SEND_REASON_CLEARUP, false);
	}
	this->SendBagList();
	m_next_order_tamp = now_time + 10;
	return true;
}

const AttributeList & ZaoHuaEquipManager::ReCalcAttr(AttributeList & base_add, bool is_recalc)
{
	if (is_recalc)
	{		
		int base_prof = m_role_module_mgr->GetRole()->GetBaseProfession();

		m_sp_id_list.clear();
		m_attrs_add.Reset();
		m_capability = 0;
		m_wear_level = 0;
		memset(m_potential_add, 0, sizeof(m_potential_add));
		// 装备加成
		Attribute m_exa_attrs[BATTLE_ATTR_MAX];
		memset(m_exa_attrs, 0, sizeof(m_exa_attrs));

		SuitEffectReCalc3 new_suit_list; new_suit_list.Reset();

		for (int equip_idx = 0; equip_idx < ZaoHuaItem::Z_TYPE_MAX; ++equip_idx)
		{
			if (NULL == m_wear_grid_list[equip_idx] || m_wear_grid_list[equip_idx]->Invalid())
			{
				continue;
			}

			int equip_item_id = m_wear_grid_list[equip_idx]->item_id;
			const ZaoHuaEquipParam & curr_param = m_wear_grid_list[equip_idx]->param->zao_hua_param;
			const ZaoHuaItem* equip = (const ZaoHuaItem*)(ITEMPOOL->GetItem(m_wear_grid_list[equip_idx]->item_id));
			if (NULL == equip || I_TYPE_ZAO_HUA_EQUIP != equip->GetItemType()) continue;

			if (m_wear_grid_list[equip_idx]->param == NULL)continue;
			//ZaoHuaEquipParam & p_eqed_equ = m_wear_grid_list[equip_idx]->param->zao_hua_param;
			m_wear_level += 90 + (equip->GetEquipLevel() - 1) * 10;
			{//基础属性
				AttrInfo attr_list[ZaoHuaItem::MAX_ZAO_HUA_BASE_VOC_NUM];
				equip->GetAttrList(attr_list);
				for (int attr_idx = 0; attr_idx < ZaoHuaItem::MAX_ZAO_HUA_BASE_VOC_NUM; attr_idx++)
				{
					if (attr_list[attr_idx].attr_type < 0 || attr_list[attr_idx].attr_type >= BATTLE_ATTR_MAX) continue;
					if (attr_list[attr_idx].attr_value > 0)m_attrs_add.m_attrs[attr_list[attr_idx].attr_type] += attr_list[attr_idx].attr_value;
				}
			}

			//魂力属性
			const ZaoHuaEquipHunLiAttrCfg * hun_li_attr_cfg = LOGIC_CONFIG->GetZaoHuaEquipConfig()->GetZaoHuaEquipHunLiAttrCfg(base_prof, equip_idx, curr_param.hun_li_value);
			if (hun_li_attr_cfg)
			{
				m_attrs_add.AddAttrCfg(hun_li_attr_cfg->attr_list);
			}

			// 装备元魂珠属性
			std::map<int, std::vector<int>  > zhu_ling_color_level;
			for (int zhu_ling_grid = 0; zhu_ling_grid < EQUIP_ZHU_LING_SLOT_NUM; ++zhu_ling_grid)
			{
				const YuanHunZhuParam & curr = curr_param.yuan_hun_zhu[zhu_ling_grid];
				if (curr.yuan_hun_zhu_item_id == 0)
				{
					continue;
				}

				m_attrs_add.AddAttrInfo(curr.attrs);

				const YuanHunZhuCfg * zhu_ling_cfg = LOGIC_CONFIG->GetZaoHuaEquipConfig()->YuanHunZhuGetCfg(curr.yuan_hun_zhu_item_id);
				const ItemBase * item_info = ITEMPOOL->GetItem(curr.yuan_hun_zhu_item_id);
				if (zhu_ling_cfg && item_info)
				{
					int color = item_info->GetColor();
					zhu_ling_color_level[color].push_back(zhu_ling_cfg->yuan_hun_zhu_level);
				}
			}

			//元魂珠组合属性			
			const YuanHunZhuCombinationAttrCfg * zhu_ling_combination_attr_cfg = LOGIC_CONFIG->GetZaoHuaEquipConfig()->YuanHunZhuGetCombinationAttrCfg2(base_prof, zhu_ling_color_level);;
			if (zhu_ling_combination_attr_cfg)
			{
				m_attrs_add.AddAttrCfg(zhu_ling_combination_attr_cfg->attr_cfg);
				m_sp_id_list.insert(zhu_ling_combination_attr_cfg->sp_id);
			}
			
			//精炼属性
			const ZaoHuaRefineItemParam * refine_param = this->GetZaoHuaRefineItemParam(equip_idx);
			if (refine_param)
			{
				const ZaoHuaEquipRefineCfg * refine_cfg = LOGIC_CONFIG->GetZaoHuaEquipConfig()->GetZaoHuaEquipRefineCfg(base_prof, equip_idx, refine_param->refine_level);
				if (refine_cfg)
				{
					m_attrs_add.AddAttrCfg(refine_cfg->attr_cfg);
				}
			}
			
			//珑铸效果
			const ZaoHuaLongZhuCfg * long_zhu_cfg = LOGIC_CONFIG->GetZaoHuaEquipConfig()->GetZaoHuaLongZhuCfg(equip_item_id);
			if (long_zhu_cfg)
			{		
				for (int long_zhu_slot_index = 0; long_zhu_slot_index < ZAO_HUA_LONG_ZHU_SLOT_NUM && long_zhu_slot_index < long_zhu_cfg->ci_tiao_num; ++long_zhu_slot_index)
				{
					ZaoHuaLongZhuSlotParam * long_zhu_slot_param = this->GetZaoHuaLongZhuSlotParam(equip_idx, long_zhu_slot_index);
					if (!long_zhu_slot_param)
					{
						continue;
					}

					const AttrItemCfg * long_zhu_attr_cfg = LOGIC_CONFIG->GetZaoHuaEquipConfig()->GetZaoHuaLongZhuCiTiaoAttrItemCfg(long_zhu_slot_param->wear_ci_tiao.ci_tiao_id, long_zhu_slot_param->wear_ci_tiao.ci_tiao_level);
					if (!long_zhu_attr_cfg)
					{
						continue;						
					}

					m_attrs_add.AddAttrCfg(*long_zhu_attr_cfg);
				}
			}


			//套装效果
			if (equip->GetSuitType() > 0)
			{
				ReCalcSuit new_suit;
				new_suit.color = equip->GetColor();
				new_suit.level = equip->GetEquipLevel();
				new_suit_list.AddSuit(equip->GetSuitType(), &new_suit);
			}
		}

		//精炼共鸣属性
		int refine_resonance_level = this->GetRefineResonanceLevel();
		const ZaoHuaEquipRefineResonanceCfg * refine_resonance_cfg = LOGIC_CONFIG->GetZaoHuaEquipConfig()->GetZaoHuaEquipRefineResonanceCfg(base_prof, refine_resonance_level);
		if (refine_resonance_cfg)
		{
			m_attrs_add.AddAttrCfg(refine_resonance_cfg->attr_cfg);
		}

		//套装效果赋予
		for (int suit_idx = 0; suit_idx < new_suit_list.suit_type_num && suit_idx < ZaoHuaItem::Z_TYPE_MAX; suit_idx++)//先检查是否有同类组合，件数堆叠
		{
			ReCalcSuitList & suit_node = new_suit_list.suit_type_list[suit_idx];
			for (int suit_num = 0; suit_num < suit_node.num && suit_num < ZaoHuaItem::Z_TYPE_MAX; suit_num++)
			{
				const ZaoHuaSuitCfg * suit_cfg = LOGIC_CONFIG->GetZaoHuaEquipConfig()->GetSuitCfg(suit_node.suit_id, suit_node.suit_list[suit_num].color, suit_node.suit_list[suit_num].level, suit_num + 1);
				if (suit_cfg == NULL)continue;

				//满足套装增加属性
				for (int effect_value_idx = 0; effect_value_idx < ZaoHuaSuitCfg::MAX_SUIT_VOC_NUM; effect_value_idx++)
				{
					const AttrInfo &suit_value = suit_cfg->attr_type_list[effect_value_idx];
					if (suit_value.attr_type < 0 || suit_value.attr_type >= BATTLE_ATTR_MAX) continue;
					m_attrs_add.m_attrs[suit_value.attr_type] += suit_value.attr_value;
				}

				//特效
				if (suit_cfg->sp_id)
				{
					const SpecialEffectCfg* spe_cfg = BattleSpecialEffectPool::GetInstance().GetSpecialEffectCfg(suit_cfg->sp_id);
					if (NULL != spe_cfg)
					{
						for (int i = 0; i < spe_cfg->effect_num; ++i)
						{
							const SpecialEffectKey& key = spe_cfg->effect_detail_list[i];
							if (key.effect_type != SPECIAL_EFFECT_TYPE_ADD_ATTR) continue;

							const EffectDetailCfg* detail_cfg = BattleSpecialEffectPool::GetInstance().GetEffectDetailCfg(key);
							if (NULL == detail_cfg) continue;

							if (SPE_ATTR_ADD_ATTR == detail_cfg->effect_sub_type && detail_cfg->param_num >= 2
								&& detail_cfg->param_list[0] >= BATTLE_ATTR_MIN && detail_cfg->param_list[0] < BATTLE_ATTR_MAX)
							{
								m_attrs_add.m_attrs[detail_cfg->param_list[0]] += detail_cfg->param_list[1];
							}

							if (SPE_ATTR_ADD_ATTR_BY_LEVEL == detail_cfg->effect_sub_type && detail_cfg->param_num >= 2
								&& detail_cfg->param_list[0] >= BATTLE_ATTR_MIN && detail_cfg->param_list[0] < BATTLE_ATTR_MAX)
							{
								BattleAttr add_attr = static_cast<BattleAttr>(1.0f * m_role_module_mgr->GetRole()->GetLevel() * detail_cfg->param_list[1] / BATTLE_PERCENT_NUM + 0.5f);
								m_attrs_add.m_attrs[detail_cfg->param_list[0]] += add_attr;
							}
						}
					}
				}

				if (suit_cfg->potential_add > 0)
				{
					//需要触发人物潜能属性重算
					for (int i = 0; i < ZaoHuaSuitCfg::ZAO_HUA_POTENTIAL_TYPE_MAX; i++)
					{
						m_potential_add[i] += suit_cfg->potential_add[i];
					}
				}
			}
		}
	}
	base_add.Add(m_attrs_add.m_attrs);
	return m_attrs_add;
}

void ZaoHuaEquipManager::SetZaoHuaEquipGridDirty(short equip_index, bool to_notice_client)
{
	if (equip_index < 0 || equip_index >= ZaoHuaItem::Z_TYPE_MAX) return;

	m_wear_dirty_mark[equip_index] = true;
	m_wear_is_change = true;
	this->SendWearChange(equip_index);
}

void ZaoHuaEquipManager::SetZaoHuaBagDirty(short equip_index,short reason_type, short put_reason, bool to_notice_client)
{
	if (equip_index < 0 || equip_index >= ItemNamespace::MAX_ZAO_HUA_BAG_NUM) return;

	m_dirty_mark[equip_index] = true;
	m_bag_is_change = true;
	this->SendSingleBagInfo(equip_index, reason_type, put_reason, to_notice_client);
}

void ZaoHuaEquipManager::GetChangeZaoHuaEquipWearList(int * count, ItemListParam::ItemListData * save_list)
{
	int tmp_count = 0;

	for (int i = 0; i < ItemNamespace::MAX_ZAO_HUA_EQUIPMENT_GRID_NUM && tmp_count < ItemNamespace::MAX_ZAO_HUA_EQUIPMENT_GRID_NUM; ++i)
	{
		if (m_wear_dirty_mark[i])
		{
			if (NULL == m_wear_grid_list[i] || m_wear_grid_list[i]->Invalid())
			{
				if (!m_wear_old_state[i]) continue;

				save_list[tmp_count].change_state_item = structcommon::CS_DELETE;

				save_list[tmp_count].index = i + ItemNamespace::ZAO_HUA_EQUIPMENT_INDEX_BEGIN;
				save_list[tmp_count].item_wrapper.Reset();
				++tmp_count;

				gamelog::g_zao_hua_equip.printf(LL_INFO, "%s CS_DELETE role[%d,%s] item from index[%d]", __FUNCTION__,
					m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), i);
			}
			else
			{
				if (m_wear_old_state[i])
				{
					save_list[tmp_count].change_state_item = structcommon::CS_UPDATE;

					gamelog::g_zao_hua_equip.printf(LL_INFO, "%s CS_UPDATE role[%d,%s] item_id[%d] num[%d] from index[%d]", __FUNCTION__,
						m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_wear_grid_list[i]->item_id, 1, i);
				}
				else
				{
					save_list[tmp_count].change_state_item = structcommon::CS_INSERT;

					gamelog::g_zao_hua_equip.printf(LL_INFO, "%s CS_INSERT role[%d,%s] item_id[%d] num[%d] index[%d]", __FUNCTION__,
						m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_wear_grid_list[i]->item_id, 1, i);
				}

				save_list[tmp_count].index = i + ItemNamespace::ZAO_HUA_EQUIPMENT_INDEX_BEGIN;
				m_wear_grid_list[i]->WrapTo(&save_list[tmp_count].item_wrapper);
				++tmp_count;
			}
		}
	}

	*count = tmp_count;
}

void ZaoHuaEquipManager::GetChangeZaoHuaEquipBagList(int *count, ItemListParam::ItemListData *save_list)
{
	int tmp_count = 0;

	for (int i = 0; i < ItemNamespace::MAX_ZAO_HUA_BAG_NUM && tmp_count < ItemNamespace::MAX_ZAO_HUA_BAG_NUM; ++i)
	{
		if (m_dirty_mark[i])
		{
			if (NULL == m_grid_list[i] || m_grid_list[i]->Invalid())
			{
				if (!m_old_state[i]) continue;

				save_list[tmp_count].change_state_item = structcommon::CS_DELETE;

				save_list[tmp_count].index = i + ItemNamespace::ZAO_HUA_BAG_INDEX_BEGIN;
				save_list[tmp_count].item_wrapper.Reset();
				++tmp_count;

				gamelog::g_zao_hua_equip.printf(LL_INFO, "%s CS_DELETE role[%d,%s] item from index[%d]", __FUNCTION__,
					m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), i);
			}
			else
			{
				if (m_old_state[i])
				{
					save_list[tmp_count].change_state_item = structcommon::CS_UPDATE;

					gamelog::g_zao_hua_equip.printf(LL_INFO, "%s CS_UPDATE role[%d,%s] item_id[%d] num[%d] from index[%d]", __FUNCTION__,
						m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_grid_list[i]->item_id, 1, i);
				}
				else
				{
					save_list[tmp_count].change_state_item = structcommon::CS_INSERT;

					gamelog::g_zao_hua_equip.printf(LL_INFO, "%s CS_INSERT role[%d,%s] item_id[%d] num[%d] index[%d]", __FUNCTION__,
						m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_grid_list[i]->item_id, 1, i);
				}

				save_list[tmp_count].index = i + ItemNamespace::ZAO_HUA_BAG_INDEX_BEGIN;
				m_grid_list[i]->WrapTo(&save_list[tmp_count].item_wrapper);
				++tmp_count;
			}
		}
	}

	*count = tmp_count;
}

void ZaoHuaEquipManager::OperatorMsg(Protocol::CSZaoHuaEquipmentOperaReq * msg)
{
	if (!FunOpen::IsFunOpen(m_role_module_mgr->GetRole(), FUN_TYPE_ZAO_HUA, true))return;
	if (NULL == msg)return;
	switch (msg->msg_type)
	{
	case Protocol::CSZaoHuaEquipmentOperaReq::OPERA_TYPE_INFO_REQ:
		this->SendWearList();
		this->SendBagList();
		this->SendWearApp();
		break;
	case Protocol::CSZaoHuaEquipmentOperaReq::OPERA_TYPE_PUTON:
		this->PutOnEquip(msg->param1);
		break;
	case Protocol::CSZaoHuaEquipmentOperaReq::OPERA_TYPE_TAKEOFF:
		this->TakeOffEquip(msg->param1);
		break;

	case Protocol::CSZaoHuaEquipmentOperaReq::OPERA_TYPE_ORDER:
		this->InOrder();
		break;
	case Protocol::CSZaoHuaEquipmentOperaReq::OPERA_TYPE_COMPOSE:
		this->ComposeItem(msg->param1, msg->param2 == 1, msg->param3);
		break;
	case Protocol::CSZaoHuaEquipmentOperaReq::OPERA_TYPE_DISCOMPOSE:
		this->DisCompose(msg->param1);
		break;
	case Protocol::CSZaoHuaEquipmentOperaReq::OPERA_TYPE_ACTIVE_APP:
		this->ActiveSuitApp(msg->param1);
		break;
	case Protocol::CSZaoHuaEquipmentOperaReq::OPERA_TYPE_APP_WEAR:
		this->WearSuitApp(msg->param1);
		break;
	case Protocol::CSZaoHuaEquipmentOperaReq::OPERA_TYPE_APP_WEAROFF:
		this->TaskOffSuitApp();
		break;
	default:
		break;
	}
}

void ZaoHuaEquipManager::ComposeItem(int compose_seq, bool is_put_on, int index)
{
	const ZaoHuaComposeCfg * compose_cfg = LOGIC_CONFIG->GetZaoHuaEquipConfig()->GetComposeCfg(compose_seq);
	if (NULL == compose_cfg)return;

	if (compose_cfg->stuff_id_1 > 0)//造化装备
	{
		ItemGridData equip_data;
		if (is_put_on)
		{
			if (!this->GetZaoHuaEquipGridData((short)index, &equip_data))
			{
				m_role_module_mgr->NoticeNum(errornum::EN_ZAO_HUA_NO_WEAR_EQUIP);
				return;
			}

			if (equip_data.item_id != compose_cfg->stuff_id_1)
			{
				m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
				return;
			}
		}
		else
		{
			const ItemGridData* bag_data = this->GetBagNode(index);
			if (NULL == bag_data || bag_data->Invalid())
			{
				m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
				return;
			}

			if (bag_data->item_id != compose_cfg->stuff_id_1)
			{
				m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
				return;
			}
		}
	}

	if (compose_cfg->stuff_id_2 > 0)//普通消耗
	{
		if (!m_role_module_mgr->GetKnapsack()->HasItem(compose_cfg->stuff_id_2, compose_cfg->stuff_num_2))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_ITEM_NOT_ELIGIBLE);
			return;
		}
	}

	if (compose_cfg->price > 0)
	{
		if (!m_role_module_mgr->GetMoney()->CoinBindMoreThan(compose_cfg->price))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_COIN_NOT_ENOUGH);
			return;
		}
	}

	//是否能给予目标道具
	const ItemBase * new_ib = ITEMPOOL->GetItem(compose_cfg->target);
	if (NULL == new_ib)return;

	//检查完毕-消耗并且给予
	ItemDataWrapper equip_data; equip_data.Reset();//拷贝旧道具信息

	if (compose_cfg->stuff_id_1 > 0)//造化装备
	{
		if (is_put_on)
		{
			if (!this->GetZaoHuaEquipGridData((short)index, &equip_data))
			{
				m_role_module_mgr->NoticeNum(errornum::EN_ZAO_HUA_NO_WEAR_EQUIP);
				return;
			}
			this->ClearWearGrid(index);
			this->SetZaoHuaEquipGridDirty(index, false);

		}
		else
		{
			const ItemGridData* bag_data = this->GetBagNode(index);

			if (NULL == bag_data || bag_data->Invalid())
			{
				m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
				return;
			}
			
			bag_data->WrapTo(&equip_data);
			this->ClearGrid(index);
			this->SetZaoHuaBagDirty(index, ZAO_HUA_SEND_REASON_OUT, PUT_REASON_NO_NOTICE, true);
		}
	}

	if (compose_cfg->stuff_id_2 > 0)//普通消耗
	{
		if (!m_role_module_mgr->GetKnapsack()->ConsumeItem(compose_cfg->stuff_id_2, compose_cfg->stuff_num_2, __FUNCTION__))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_ITEM_NOT_ELIGIBLE);
			return;
		}
	}

	if (compose_cfg->price > 0)
	{
		if (!m_role_module_mgr->GetMoney()->UseCoinBind(compose_cfg->price, __FUNCTION__))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_COIN_NOT_ENOUGH);
			return;
		}
	}

	equip_data.item_id = compose_cfg->target;
	equip_data.num = 1;
	equip_data.is_bind = new_ib->IsBind() ? 1 : 0;
	equip_data.invalid_time = new_ib->CalInvalidTime();
	equip_data.gold_price = 0;
	
	/*ItemDataWrapper real_info;
	real_info.Reset();
	equip_data.WrapTo(&real_info);*/
	NetValueItemParam param;
	param.Reset();
	param.SetData(equip_data.param_data);
	if (is_put_on)
	{
		if (m_wear_grid_list[index] == NULL)
		{
			m_wear_grid_list[index] = new ItemGridData();
		}
		else
		{
			//旧部位还有道具，记录一下日志
			gamelog::g_zao_hua_equip.printf(LL_INFO, "%s user[%d,%s] err[is_put_on] old_item_data[%d] old_param[%s]",
				__FUNCTION__, m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), equip_data.item_id, param.GetParamLog(new_ib->GetItemType()));
		}

		m_wear_grid_list[index]->Set(equip_data);
		
		this->SetZaoHuaEquipGridDirty(index);
		//新装备日志
		gamelog::g_zao_hua_equip.printf(LL_INFO, "%s user[%d,%s] new_item_data[%d] new_param[%s]",
			__FUNCTION__, m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), m_wear_grid_list[index]->item_id, m_wear_grid_list[index]->param->GetParamLog(new_ib->GetItemType()));

		m_role_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_ZAO_HUA, __FUNCTION__);
	}
	else
	{
		bool ret = true;
		if (equip_data.has_param == 0)
		{
			const ItemGridData *  new_node = this->AddZaoHuaItem(equip_data.item_id, true, NULL);
			if (NULL == new_node)
			{
				ret = false;
			}
			else
			{
				new_node->WrapTo(&equip_data);
			}
		}
		else
		{
			ret = this->AddNewZaoHuaItem(&equip_data, NULL);
		}

		if (!ret)
		{
			//放入失败记录日志

			gamelog::g_zao_hua_equip.printf(LL_INFO, "%s user[%d,%s] err[AddNewZaoHuaItem] item_data[%d] param[%s]",
				__FUNCTION__, m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), equip_data.item_id, param.GetParamLog(new_ib->GetItemType()));
			return;
		}
		//新装备日志
		gamelog::g_zao_hua_equip.printf(LL_INFO, "%s user[%d,%s] item_data[%d] param[%s]",
			__FUNCTION__, m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), equip_data.item_id, param.GetParamLog(new_ib->GetItemType()));
	}

	this->SendComposeResult(true, equip_data);
}

void ZaoHuaEquipManager::DisCompose(int index)
{
	const ItemGridData* bag_idx = this->GetBagNode(index);
	if (NULL == bag_idx || bag_idx->Invalid())return;

	const ItemBase * i_b = ITEMPOOL->GetItem(bag_idx->item_id);
	if (NULL == i_b || i_b->GetItemType() != I_TYPE_ZAO_HUA_EQUIP)return;

	const ZaoHuaItem * z_b = (const ZaoHuaItem*)i_b;

	const ZaoHuaEquipParam & param = bag_idx->param->zao_hua_param;

	std::vector<ItemConfigData> return_item_list;

	int ret_item = 0;
	int ret_num = 0;
	z_b->GetReturn(&ret_item, &ret_num);

	if (NULL == ITEMPOOL->GetItem(ret_item) || ret_num <= 0)return;

	ItemConfigData temp(ret_item, false, ret_num);
	return_item_list.push_back(temp);

	const ZaoHuaEquipOtherCfg & other_cfg = LOGIC_CONFIG->GetZaoHuaEquipConfig()->GetOtherCfg();
	int max_hun_li = std::min(other_cfg.hun_li_max_value, param.hun_li_value);

	if (max_hun_li > 0)
	{
		int one_up_level_add_value = other_cfg.hun_li_add_value + (param.ling_li_value - 1) * other_cfg.ling_xing_extra_hun_li_add_value;
		int times = (max_hun_li + one_up_level_add_value - 1) / one_up_level_add_value;
		ItemConfigData temp(other_cfg.hun_li_uplv_comsume_item_id, false, other_cfg.hun_li_uplv_comsume_item_num * times);
		return_item_list.push_back(temp);
	}

	if (!m_role_module_mgr->GetKnapsack()->CheckForPutList2(return_item_list))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_ITEM_COLUMN_NO_SPACE);
		return;
	}

	if (!this->ConsumeItem(index, __FUNCTION__))
	{
		return;
	}

	if (!m_role_module_mgr->GetKnapsack()->PutVec(return_item_list, PUT_REASON_ZAO_HUA_DISCOMPOSE))
	{
		for (int i = 0; i < (int)return_item_list.size(); ++i)
		{
			const ItemConfigData & curr = return_item_list[i];
			gamelog::g_zao_hua_equip.printf(LL_INFO, "%s user[%d,%s] put_err[item_id:%d,num:%d]", __FUNCTION__,
				m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), curr.item_id, curr.num);
		}		

		return;
	}

	m_role_module_mgr->NoticeNum(noticenum::NT_ZAO_HUA_DISCOMPOSE_SUCC);
}

void ZaoHuaEquipManager::DisComposeList(Protocol::CSZaoHuaEquipmentDiscomposeReq * msg)
{
	if (!FunOpen::IsFunOpen(m_role_module_mgr->GetRole(), FUN_TYPE_ZAO_HUA, true))return;
	if (NULL == msg)return;
	std::vector<ItemConfigData> real_return_vec;
	for (int i = 0; i < 512 && i < ItemNamespace::MAX_ZAO_HUA_BAG_NUM; i++)
	{
		if (!msg->discompose_list.IsBitSet(i))continue;
		const ItemGridData* node = this->GetBagNode(i);
		if (NULL == node || node->Invalid())continue;

		const ItemBase * i_b = ITEMPOOL->GetItem(node->item_id);
		if (NULL == i_b || i_b->GetItemType() != I_TYPE_ZAO_HUA_EQUIP)return;
		
		const ZaoHuaItem * z_b = (const ZaoHuaItem*)i_b;

		const ZaoHuaEquipParam & param = node->param->zao_hua_param;

		int ret_item = 0;
		int ret_num = 0;
		z_b->GetReturn(&ret_item, &ret_num);

		if (NULL == ITEMPOOL->GetItem(ret_item) || ret_num <= 0)return;

		real_return_vec.push_back(ItemConfigData(ret_item, false, ret_num));

		const ZaoHuaEquipOtherCfg & other_cfg = LOGIC_CONFIG->GetZaoHuaEquipConfig()->GetOtherCfg();
		int max_hun_li = std::min(other_cfg.hun_li_max_value, param.hun_li_value);

		if (max_hun_li > 0)
		{
			int one_up_level_add_value = other_cfg.hun_li_add_value + (param.ling_li_value - 1) * other_cfg.ling_xing_extra_hun_li_add_value;
			int times = (max_hun_li + one_up_level_add_value - 1) / one_up_level_add_value;
			ItemConfigData temp(other_cfg.hun_li_uplv_comsume_item_id, false, other_cfg.hun_li_uplv_comsume_item_num * times);
			real_return_vec.push_back(temp);
		}
	}

	if (real_return_vec.size() <= 0 || !m_role_module_mgr->GetKnapsack()->CheckForPutList((short)real_return_vec.size(), &real_return_vec[0]))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_ITEM_COLUMN_NO_SPACE);
		return;
	}

	for (int i = 0; i < 512; i++)
	{
		if (!msg->discompose_list.IsBitSet(i))continue;
		this->ConsumeItem(i, __FUNCTION__);
	}

	if (real_return_vec.size() > 0)
	{
		m_role_module_mgr->GetKnapsack()->PutVec(real_return_vec, PUT_REASON_ZAO_HUA_DISCOMPOSE);
		m_role_module_mgr->GetKnapsack()->NotifyGetItemListVec(real_return_vec, GET_REASON_ZAO_HUA_DISCOMPOSE);
	}

	m_role_module_mgr->NoticeNum(noticenum::NT_ZAO_HUA_DISCOMPOSE_SUCC);
}

void ZaoHuaEquipManager::ActiveSuitApp(int app_id)
{
	const ZaoHuaVigorousCfg * vigorous_cfg = LOGIC_CONFIG->GetZaoHuaEquipConfig()->GetZaoHuaVigorousCfg(app_id);
	if (NULL == vigorous_cfg)return;
	int m_suit_id = 0;
	int m_low_level = 0;
	for (int equip_idx = 0; equip_idx < ZaoHuaItem::Z_TYPE_MAX; ++equip_idx)
	{
		if (NULL == m_wear_grid_list[equip_idx] || m_wear_grid_list[equip_idx]->Invalid())
		{
			m_role_module_mgr->NoticeNum(errornum::EN_ZAO_HUA_NO_WEAR_EQUIP);
			return;
		}

		const ZaoHuaItem* equip = (const ZaoHuaItem*)(ITEMPOOL->GetItem(m_wear_grid_list[equip_idx]->item_id));
		if (NULL == equip || I_TYPE_ZAO_HUA_EQUIP != equip->GetItemType()) return;

		if(m_suit_id == 0) m_suit_id = equip->GetSuitType();
		else if(m_suit_id != equip->GetSuitType())
		{
			m_role_module_mgr->NoticeNum(errornum::EN_ZAO_HUA_WEAR_ERR_SUIT);
			return;
		}

		if (m_low_level == 0 || m_low_level > equip->GetEquipLevel())
		{
			m_low_level = equip->GetEquipLevel();
		}
	}

	if (vigorous_cfg->zaohua_equip_suit != m_suit_id)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_ZAO_HUA_WEAR_ERR_SUIT);
		return;
	}

	if (vigorous_cfg->level > m_low_level)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_ZAO_HUA_SUIT_LEVEL_LIMIT);
		return;
	}

	if (m_role_module_mgr->GetCommonData().zao_hua_app_flag.IsBitSet(app_id))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_ZAO_HUA_APP_HAS_ACTIVE);//外观已激活
		return;
	}

	m_role_module_mgr->GetCommonData().zao_hua_app_flag.SetBit(app_id);
	this->SendWearApp();
}

void ZaoHuaEquipManager::WearSuitApp(int app_id)
{
	if (!m_role_module_mgr->GetCommonData().zao_hua_app_flag.IsBitSet(app_id))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_ZAO_HUA_APP_NOT_ACTIVE);//未激活外观无法穿戴
		return;
	}

	m_role_module_mgr->GetCommonData().zao_hua_wear_app_id = app_id;
	m_role_module_mgr->GetRole()->SendChangeAppearanceNotice();
	this->SendWearApp();
}

void ZaoHuaEquipManager::TaskOffSuitApp()
{
	m_role_module_mgr->GetCommonData().zao_hua_wear_app_id = 0;
	m_role_module_mgr->GetRole()->SendChangeAppearanceNotice();
	this->SendWearApp();
}

void ZaoHuaEquipManager::ClearDirtyMark()
{
	if (!m_bag_is_change) return;

	for (int i = 0; i < ItemNamespace::MAX_ZAO_HUA_BAG_NUM; ++i)
	{
		if (m_dirty_mark[i])
		{
			if (NULL == m_grid_list[i] || m_grid_list[i]->Invalid())
			{
				m_old_state[i] = false;
			}
			else
			{
				m_old_state[i] = true;
			}
		}
	}

	memset(m_dirty_mark, 0, sizeof(m_dirty_mark));

	m_bag_is_change = false;
}

void ZaoHuaEquipManager::ClearWearDirtyMark()
{
	if (!m_wear_is_change) return;

	for (int i = 0; i < ItemNamespace::MAX_ZAO_HUA_EQUIPMENT_GRID_NUM; ++i)
	{
		if (m_wear_dirty_mark[i])
		{
			if (NULL == m_wear_grid_list[i] || m_wear_grid_list[i]->Invalid())
			{
				m_wear_old_state[i] = false;
			}
			else
			{
				m_wear_old_state[i] = true;
			}
		}
	}

	memset(m_wear_dirty_mark, 0, sizeof(m_wear_dirty_mark));

	m_wear_is_change = false;
}

void ZaoHuaEquipManager::GetUserSpecialEffectList(int max_other_sp_num, short * out_other_sp_num, int * out_other_sp_list)
{
}

void ZaoHuaEquipManager::ClearWearGrid(int index)
{
	if (index < 0 || index >= ZaoHuaItem::Z_TYPE_MAX)return;
	if (NULL == m_wear_grid_list[index]) return;

	m_wear_grid_list[index]->Clear();
	delete m_wear_grid_list[index];
	m_wear_grid_list[index] = NULL;
}

void ZaoHuaEquipManager::ClearGrid(int index)
{
	if (index < 0 || index >= ItemNamespace::MAX_ZAO_HUA_BAG_NUM)return;
	if (NULL == m_grid_list[index]) return;

	m_grid_list[index]->Clear();
	delete m_grid_list[index];
	m_grid_list[index] = NULL;
}

void ZaoHuaEquipManager::SendSingleBagInfo(int index, short reason_type, short put_reason, bool is_notice)
{
	if (is_notice)
	{
		Protocol::SCZaoHuaBagItemChange kic;
		kic.reason = reason_type;
		kic.put_reason = put_reason;
		kic.index = static_cast<short>(index);
		if (NULL == m_grid_list[index] || m_grid_list[index]->Invalid())
		{
			kic.item_id = 0;
			kic.num = 0;
			kic.is_bind = 0;
			kic.has_param = 0;
			kic.invalid_time = 0;
			kic.item_unique_id = 0;
			kic.is_market_buy_flag = 0;
			kic.reserve_ch = 0;
		}
		else
		{
			kic.item_id = m_grid_list[index]->item_id;
			kic.num = m_grid_list[index]->num;
			kic.is_bind = m_grid_list[index]->is_bind ? 1 : 0;
			kic.has_param = (NULL != m_grid_list[index]->param);
			kic.invalid_time = m_grid_list[index]->invalid_time;
			kic.item_unique_id = m_grid_list[index]->item_unique_id;
			kic.is_market_buy_flag = m_grid_list[index]->is_market_buy_flag;
			kic.reserve_ch = 0;

			const ItemBase* item_base = ITEMPOOL->GetItem(kic.item_id);
			if (NULL == item_base)
			{
				return;
			}
			kic.param_length = m_grid_list[index]->param->GetDataByItemType(item_base->GetItemType(), kic.param_data);
		}

		EngineAdapter::Instance().NetSendSerializeMsg(m_role_module_mgr->GetRole()->GetNetId(), &kic);
	}
}

void ZaoHuaEquipManager::SendComposeResult(bool is_succ, const ItemDataWrapper & get_item)
{
	const ItemBase * item_base = ITEMPOOL->GetItem(get_item.item_id);
	if (NULL == item_base) return;

	Protocol::SCZaoHuaEquipmentComposeResults wcr;
	wcr.com_result = is_succ ? 1 : 0;
	wcr.has_param = (char)get_item.has_param;
	wcr.item_id = get_item.item_id;
	wcr.number = get_item.num;

	wcr.param_len = NetValueItemParam::GetDataLengthByItemType(item_base->GetItemType());
	memcpy(wcr.param_data, get_item.param_data, wcr.param_len);

	EngineAdapter::Instance().NetSendSerializeMsg(m_role_module_mgr->GetRole()->GetNetId(), &wcr);
}

void ZaoHuaEquipManager::SendBagList(int reason)
{
	Protocol::SCZaoHuaBagListInfo kia;
	kia.item_count = 0;
	kia.reason = reason;

	for (int i = 0; i < ItemNamespace::MAX_ZAO_HUA_BAG_NUM; ++i)
	{
		if (NULL == m_grid_list[i] || m_grid_list[i]->Invalid()) continue;

		const ItemGridData * item_grid_data = m_grid_list[i];
		const ItemBase * item_base = ITEMPOOL->GetItem(item_grid_data->item_id);
		if (NULL == item_base) continue;

		InfoType2 & ptc_con = kia.item_list[kia.item_count++];
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

	EngineAdapter::Instance().NetSendSerializeMsg(m_role_module_mgr->GetRole()->GetNetId(), &kia);
}

void ZaoHuaEquipManager::SendWearList()
{
	Protocol::SCZaoHuaEquipmentInfoAck ack;

	ack.count = 0;

	for (int i = ZaoHuaItem::Z_TYPE_MIN; i < ZaoHuaItem::Z_TYPE_MAX && ack.count < ItemNamespace::MAX_ZAO_HUA_EQUIPMENT_GRID_NUM; ++i)
	{
		if (NULL == m_wear_grid_list[i] || m_wear_grid_list[i]->Invalid()) continue;

		ack.equipment_list[ack.count].index = i;
		ack.equipment_list[ack.count].item_id = m_wear_grid_list[i]->item_id;
		ack.equipment_list[ack.count].num = m_wear_grid_list[i]->num;
		//ack.equipment_list[ack.count].is_bind = m_equipment_list[i].is_bind ? 1 : 0;
		ack.equipment_list[ack.count].item_unique_id = m_wear_grid_list[i]->item_unique_id;

		if (NULL != m_wear_grid_list[i]->param)
		{
			ack.equipment_list[ack.count].param = m_wear_grid_list[i]->param->zao_hua_param;
		}

		ack.count += 1;
	}

	int send_len = sizeof(ack) - (ItemNamespace::MAX_ZAO_HUA_EQUIPMENT_GRID_NUM - ack.count) * sizeof(ack.equipment_list[0]);
	EngineAdapter::Instance().NetSend(m_role_module_mgr->GetRole()->GetNetId(), (const char*)&ack, send_len);
}

void ZaoHuaEquipManager::SendWearApp()
{
	Protocol::SCZaoHuaAppearance info;

	info.wear_app_id = m_role_module_mgr->GetCommonData().zao_hua_wear_app_id;
	info.flag = m_role_module_mgr->GetCommonData().zao_hua_app_flag;

	m_role_module_mgr->NetSend((const char *)&info, sizeof(info));
}

void ZaoHuaEquipManager::SendCommonInfo()
{
	Protocol::SCZaoHuaEquipmentCommonDataInfo info;

	info.show_mo_zhuang = m_role_module_mgr->GetCommonData().zao_hua_show_mo_zhuang;

	m_role_module_mgr->NetSend((const char *)&info, sizeof(info));
}

ItemGridData * ZaoHuaEquipManager::GetItemGridData(int item_src, int index, int column)
{
	ItemGridData * grid_data = NULL;
	if (YUAN_HUN_ZHU_ITEM_SRC_KNAPSACK == item_src)
	{
		grid_data = m_role_module_mgr->GetKnapsack()->GetItemGridData2(column, index);
	}

	if (YUAN_HUN_ZHU_ITEM_SRC_WEAR_ZAO_HUA_EQUIP == item_src)
	{
		grid_data = this->GetItemGridData(index);
	}

	if (YUAN_HUN_ZHU_ITEM_SRC_ZAO_HUA_EQUIP_KNAPSACK == item_src)
	{
		grid_data = this->GetItemGridDataKnapsack(index);
	}

	return grid_data;
}

ZaoHuaEquipParam * ZaoHuaEquipManager::GetZaoHuaEquipParam(int item_src, int index, int column, int * ARG_OUT item_id_ptr, long long * ARG_OUT item__uuid)
{
	const ItemGridData * grid_data = this->GetItemGridData(item_src, index, column);
	if (!grid_data)
	{
		return NULL;
	}

	const ZaoHuaItem * item_info = this->GetZaoHuaItem(grid_data->item_id);
	if (!item_info)
	{
		return NULL;
	}

	if (!grid_data->param)
	{
		return NULL;
	}

	if (item_id_ptr)
	{
		*item_id_ptr = grid_data->item_id;
	}

	if (item__uuid)
	{
		*item__uuid = grid_data->item_unique_id;
	}

	return &(grid_data->param->zao_hua_param);
}

ZaoHuaEquipParam * ZaoHuaEquipManager::GetZaoHuaEquipParamByEquipSlotIndex(int equip_slot_index, int ARG_OUT * item_id_ptr)
{
	return this->GetZaoHuaEquipParam(YUAN_HUN_ZHU_ITEM_SRC_WEAR_ZAO_HUA_EQUIP, equip_slot_index,0, item_id_ptr);
}

int ZaoHuaEquipManager::SetItemDirty(int item_src, int index, int column)
{
	if (item_src == YUAN_HUN_ZHU_ITEM_SRC_KNAPSACK)
	{
		m_role_module_mgr->GetKnapsack()->SetItemDirty(column, index, true, Knapsack::ITEM_CHANGE_NOTICE_REASON_DATA_CHANGE);
	}

	if (item_src == YUAN_HUN_ZHU_ITEM_SRC_WEAR_ZAO_HUA_EQUIP)
	{
		this->SetZaoHuaEquipGridDirty(index);
	}

	if (item_src == YUAN_HUN_ZHU_ITEM_SRC_ZAO_HUA_EQUIP_KNAPSACK)
	{
		this->SetZaoHuaBagDirty(index);
	}

	return 0;
}

const ZaoHuaItem * ZaoHuaEquipManager::GetZaoHuaItem(int item_id)
{
	const ItemBase * item_info = ITEMPOOL->GetItem(item_id);
	if (!item_info)
	{
		return NULL;
	}

	if (item_info->GetItemType() != I_TYPE_ZAO_HUA_EQUIP)
	{
		return NULL;
	}

	return (const ZaoHuaItem *)item_info;
}

int ZaoHuaEquipManager::Refine(int equip_slot_index)
{
	if (equip_slot_index < 0 || equip_slot_index >= ARRAY_LENGTH(m_refine_param.refine_info_list))
	{
		ret_line;
	}

	ZaoHuaRefineItemParam & refine_info = m_refine_param.refine_info_list[equip_slot_index];

	int item_id = 0;
	ZaoHuaEquipParam * zao_hua_param = this->GetZaoHuaEquipParamByEquipSlotIndex(equip_slot_index, &item_id);
	if (!zao_hua_param)
	{
		ret_line;
	}


	const ZaoHuaItem * item_info = this->GetZaoHuaItem(item_id);
	if (!item_info)
	{
		ret_line;
	}

	int base_prof = m_role_module_mgr->GetRole()->GetBaseProfession();
	const ZaoHuaEquipRefineCfg * cfg = LOGIC_CONFIG->GetZaoHuaEquipConfig()->GetZaoHuaEquipRefineCfg(base_prof, equip_slot_index, refine_info.refine_level + 1);
	if (!cfg)
	{
		ret_line;
	}

	if (cfg->refine_need_equip_level > item_info->GetEquipLevel())
	{
		ret_line;
	}

	if (!m_role_module_mgr->GetKnapsack()->ConsumeItemByMap(cfg->comsume_cfg, __FUNCTION__))
	{
		ret_line;
	}

	refine_info.refine_level += 1;
	this->SendRefineInfo();

	m_role_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_ZAO_HUA, __FUNCTION__);


	gamelog::g_log_zao_hua.printf(LL_INFO, "%s line:%d | role[%d %s] equip_slot_index:%d refine_level:%d", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
		equip_slot_index, refine_info.refine_level);

	return 0;
}

int ZaoHuaEquipManager::GetRefineResonanceLevel()
{	
	int min_level = INT_MAX;
	for (int equip_idx = 0; equip_idx < ZaoHuaItem::Z_TYPE_MAX; ++equip_idx)
	{
		ZaoHuaEquipParam * zao_hua_param = this->GetZaoHuaEquipParamByEquipSlotIndex(equip_idx);
		if (!zao_hua_param)
		{
			return 0;
		}

		ZaoHuaRefineItemParam * refine_param = this->GetZaoHuaRefineItemParam(equip_idx);
		if (!refine_param)
		{
			return 0;
		}

		min_level = std::min(min_level, (int)refine_param->refine_level);
	}

	return min_level;
}

ZaoHuaRefineItemParam * ZaoHuaEquipManager::GetZaoHuaRefineItemParam(int equip_slot_index)
{
	if (equip_slot_index < 0 || equip_slot_index >= ARRAY_LENGTH(m_refine_param.refine_info_list))
	{
		return NULL;
	}

	ZaoHuaRefineItemParam & refine_info = m_refine_param.refine_info_list[equip_slot_index];

	return &refine_info;
}

void ZaoHuaEquipManager::SendRefineInfo()
{
	Protocol::SCZaoHuaReFineInfo protocol;

	for (int i = 0; i < ARRAY_LENGTH(protocol.refine_info) && i < ARRAY_LENGTH(m_refine_param.refine_info_list); ++i)
	{
		protocol.refine_info[i] = m_refine_param.refine_info_list[i].refine_level;
	}
	
	m_role_module_mgr->NetSend(&protocol, sizeof(protocol));
}

void ZaoHuaEquipManager::HunLiUplv(int item_src, int index, int column)
{
	int item_id = 0;
	long long item_uuid = 0;
	ZaoHuaEquipParam * param = this->GetZaoHuaEquipParam(item_src, index, column, &item_id, &item_uuid);
	if (!param)
	{
		return;
	}

	if (param->ling_li_value <= 0)
	{
		return;
	}

	const ZaoHuaEquipOtherCfg & other_cfg = LOGIC_CONFIG->GetZaoHuaEquipConfig()->GetOtherCfg();

	if (param->hun_li_value >= other_cfg.hun_li_max_value)
	{
		return;
	}

	if (!m_role_module_mgr->GetKnapsack()->ConsumeItem(other_cfg.hun_li_uplv_comsume_item_id, other_cfg.hun_li_uplv_comsume_item_num, __FUNCTION__))
	{
		return;
	}

	param->hun_li_value += other_cfg.hun_li_add_value + (param->ling_li_value - 1) * other_cfg.ling_xing_extra_hun_li_add_value;

	this->SetItemDirty(item_src, index, column);

	gamelog::g_log_zao_hua.printf(LL_INFO, "%s line:%d | role[%d %s] item_id:%d item_uuid:%lld param->hun_li_value:%d param->ling_li_value:%d", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
		item_id, item_uuid, param->hun_li_value, param->ling_li_value);

	m_role_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_ZAO_HUA, __FUNCTION__);
}

int ZaoHuaEquipManager::YuanHunZhuUpLevel(int item_src, int index, int column, int slot_index)
{
	ItemGridData * grid_data = this->GetItemGridData(item_src, index, column);
	if (grid_data == NULL)
	{
		return -__LINE__;
	}

	YuanHunZhuParamRet  yuan_hun_zhu_info = this->YuanHunZhuGetParam(grid_data, slot_index);
	if (!yuan_hun_zhu_info.is_ret_succ)
	{
		return -__LINE__;
	}

	const YuanHunZhuCfg * cfg = LOGIC_CONFIG->GetZaoHuaEquipConfig()->YuanHunZhuGetCfg(yuan_hun_zhu_info.item_id);
	if (cfg == NULL)
	{
		return -__LINE__;
	}

	const YuanHunZhuCfg * next_cfg = LOGIC_CONFIG->GetZaoHuaEquipConfig()->YuanHunZhuGetCfg(cfg->next_item_id);
	if (next_cfg == NULL)
	{
		return -__LINE__;
	}

	if (!m_role_module_mgr->GetMoney()->MoneyTypeMoreThan(MONEY_TYPE_CAN_HUN, cfg->up_level_need_money_value))
	{
		return -__LINE__;
	}

	const YuanHunZhuAttrGroupItemCfg * next_level_attr[YUAN_HUN_ZHU_MAX_ATTR_NUM] = { NULL,NULL,NULL };

	for (int i = 0; i < YUAN_HUN_ZHU_MAX_ATTR_NUM; ++i)
	{
		int attr_group_id = next_cfg->slot_attr_rand_group_id[i];
		int attr_seq = yuan_hun_zhu_info.param->attr_seq[i];
		if (attr_group_id == 0)
		{
			continue;
		}

		const YuanHunZhuAttrGroupItemCfg * attr_cfg = LOGIC_CONFIG->GetZaoHuaEquipConfig()->YuanHunZhuGetAttrGroupItemCfg(attr_group_id, attr_seq);
		if (!attr_cfg)
		{
			return -__LINE__;
		}

		next_level_attr[i] = attr_cfg;
	}

	//消耗，下面不能返回了
	if (!m_role_module_mgr->GetMoney()->UseMoney(MONEY_TYPE_CAN_HUN, cfg->up_level_need_money_value, __FUNCTION__))
	{
		return -__LINE__;
	}

	int old_yuan_hun_zhu_id = yuan_hun_zhu_info.param->yuan_hun_zhu_item_id;
	yuan_hun_zhu_info.param->yuan_hun_zhu_item_id = cfg->next_item_id;

	if (item_src == YUAN_HUN_ZHU_ITEM_SRC_KNAPSACK && yuan_hun_zhu_info.is_yuan_hun_zhu)
	{
		grid_data->item_id = cfg->next_item_id;
	}

	std::string attr_log_str = "{yuan_hun_zhu_attr: ";
	for (int i = 0; i < YUAN_HUN_ZHU_MAX_ATTR_NUM; ++i)
	{
		const YuanHunZhuAttrGroupItemCfg * curr = next_level_attr[i];
		if (curr)
		{
			int attr_seq = yuan_hun_zhu_info.param->attr_seq[i];
			AttrInfo old_attr = yuan_hun_zhu_info.param->attrs[i];
			yuan_hun_zhu_info.param->attrs[i] = curr->attr;

			attr_log_str += STRING_SPRINTF("slot:%d attr_seq:%d attr_type:%d->%d ,attr_value:%d->%d", i, attr_seq,
				old_attr.attr_type, yuan_hun_zhu_info.param->attrs[i].attr_type, old_attr.attr_value, yuan_hun_zhu_info.param->attrs[i].attr_value);
		}		
	}

	attr_log_str += "}";
	
	this->SetItemDirty(item_src, index, column);
	gamelog::g_log_yuan_hun_zhu.printf(LL_INFO, "%s line:%d | role[%d %s] item_src:%d  index:%d column:%d slot_index:%d item_id[%d to %d] level[%d to %d] %s uuid:%lld ", __FUNCTION__, __LINE__,
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), item_src, index, column, slot_index, old_yuan_hun_zhu_id, next_cfg->item_id, cfg->yuan_hun_zhu_level,
		next_cfg->yuan_hun_zhu_level, attr_log_str.c_str(), grid_data->item_unique_id);

	if (item_src == YUAN_HUN_ZHU_ITEM_SRC_WEAR_ZAO_HUA_EQUIP)
	{
		m_role_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_ZAO_HUA, __FUNCTION__);
	}

	return 0;
}

int ZaoHuaEquipManager::YuanHunZhuDecompose(int index, int column)
{
	const ItemGridData * decompose_item_grid = m_role_module_mgr->GetKnapsack()->GetItemGridData2(column, index);
	if (!decompose_item_grid)
	{
		ret_line;
	}

	const YuanHunZhuDecomposeCfg * cfg = LOGIC_CONFIG->GetZaoHuaEquipConfig()->YuanHunZhuGetDecomposeCfg(decompose_item_grid->item_id);
	if (!cfg)
	{
		ret_line;
	}

	if (!m_role_module_mgr->GetKnapsack()->PutVec(cfg->decompose_get_item, PUT_REASON_YUAN_HUN_ZHU_DECOMPOSE))
	{
		ret_line;
	}

	m_role_module_mgr->GetMoney()->OnAddMoney(cfg->decompose_get_money_type, cfg->decompose_get_money_value, __FUNCTION__);
	m_role_module_mgr->GetKnapsack()->ConsumeItemByIndex(column, index, 1, __FUNCTION__);

	return 0;
}

int ZaoHuaEquipManager::YuanHunZhuCompose(int item_id)
{
	const YuanHunZhuComposeCfg * cfg = LOGIC_CONFIG->GetZaoHuaEquipConfig()->YuanHunZhuGetComposeCfg(item_id);
	if (!cfg)
	{
		ret_line;
	}

	if (!m_role_module_mgr->GetKnapsack()->HasItem(cfg->yuan_hun_zhu_compose_need_item_id,cfg->yuan_hun_zhu_compose_need_item_num))
	{
		ret_line;
	}

	if (!m_role_module_mgr->GetKnapsack()->Put2(cfg->item_id, 1, PUT_REASON_YUAN_HUN_ZHU_COMPOSE))
	{
		ret_line;
	}

	m_role_module_mgr->GetKnapsack()->ConsumeItem(cfg->yuan_hun_zhu_compose_need_item_id, cfg->yuan_hun_zhu_compose_need_item_num, __FUNCTION__);

	return 0;
}

int ZaoHuaEquipManager::YuanHunZhuPutOn(int item_src, int index, int column, int slot_index, int other_item_src, int other_index, int other_column, int other_slot_index)
{
	const ItemGridData * target_grid_data = this->GetItemGridData(item_src, index, column);
	if (target_grid_data == NULL)
	{
		return -__LINE__;
	}

	YuanHunZhuParamRet target_yuan_hun_zhu_param = this->YuanHunZhuGetParam(target_grid_data, slot_index, true);
	if (!target_yuan_hun_zhu_param.is_ret_succ || target_yuan_hun_zhu_param.is_yuan_hun_zhu)
	{
		return -__LINE__;
	}

	const ItemGridData * other_grid_data = this->GetItemGridData(other_item_src, other_index, other_column);
	if (other_grid_data == NULL)
	{
		return -__LINE__;
	}

	YuanHunZhuParamRet other_yaun_hun_zhu_param = this->YuanHunZhuGetParam(other_grid_data, other_slot_index);
	if (!other_yaun_hun_zhu_param.is_ret_succ)
	{
		return -__LINE__;
	}

	if (other_yaun_hun_zhu_param.is_yuan_hun_zhu)  //如果穿上的那个物品是一个铸灵
	{
		NetValueItemParam tp;
		tp.yuan_hun_zhu_param = *target_yuan_hun_zhu_param.param;

		ItemDataWrapper take_off_yuan_hun_zhu;
		take_off_yuan_hun_zhu.item_id = target_yuan_hun_zhu_param.param->yuan_hun_zhu_item_id;
		take_off_yuan_hun_zhu.num = 1;
		take_off_yuan_hun_zhu.is_bind = true;
		take_off_yuan_hun_zhu.has_param = 1;
		tp.GetData(take_off_yuan_hun_zhu.param_data);

		if (take_off_yuan_hun_zhu.item_id != 0 && !m_role_module_mgr->GetKnapsack()->CheckForPutItemDataWrapper(take_off_yuan_hun_zhu))
		{
			return -__LINE__;
		}

		if (take_off_yuan_hun_zhu.item_id != 0)
		{
			m_role_module_mgr->GetKnapsack()->Put(take_off_yuan_hun_zhu, PUT_REASON_YUAN_HUN_ZHU_PUT_ON);
		}

		gamelog::g_log_yuan_hun_zhu.printf(LL_INFO, "%s line:%d | role[%d %s] item_src:%d  index:%d column:%d slot_index:%d item_id[%d to %d]  uuid:%lld", __FUNCTION__, __LINE__,
			m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), item_src, index, column, slot_index, take_off_yuan_hun_zhu.item_id, other_yaun_hun_zhu_param.item_id, target_grid_data->item_unique_id);

		ROLE_LOG_QUICK10(LOG_TYPE_YUAN_HUN_ZHU, m_role_module_mgr->GetRole(), take_off_yuan_hun_zhu.item_id, other_yaun_hun_zhu_param.item_id, "chuan shang", NULL, target_yuan_hun_zhu_param.eq_item_id, target_grid_data->item_unique_id, 0, 0, 0);

		*target_yuan_hun_zhu_param.param = *other_yaun_hun_zhu_param.param;
		//能获取出来至少有一个
		m_role_module_mgr->GetKnapsack()->ConsumeItemByIndex(other_column, other_index, 1, __FUNCTION__);
		other_yaun_hun_zhu_param.param = NULL;		//other_yaun_hun_zhu_param 后面不能继续用了

		this->SetItemDirty(item_src, index, column);
	}
	else   //如果穿上的那个物品是其他装备上一个铸灵
	{
		if (target_yuan_hun_zhu_param.param == other_yaun_hun_zhu_param.param)
		{
			return -__LINE__;
		}

		gamelog::g_log_yuan_hun_zhu.printf(LL_INFO, "%s line:%d | role[%d %s] item_src:%d  index:%d column:%d slot_index:%d item_id[%d to %d]  uuid:%lld", __FUNCTION__, __LINE__,
			m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), item_src, index, column, slot_index, target_yuan_hun_zhu_param.item_id, other_yaun_hun_zhu_param.item_id, target_grid_data->item_unique_id);

		gamelog::g_log_yuan_hun_zhu.printf(LL_INFO, "%s line:%d | role[%d %s] item_src:%d  index:%d column:%d slot_index:%d item_id[%d to %d]  uuid:%lld", __FUNCTION__, __LINE__,
			m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), other_item_src, other_index, other_column, other_slot_index, other_yaun_hun_zhu_param.item_id, target_yuan_hun_zhu_param.item_id, other_grid_data->item_unique_id);

		ROLE_LOG_QUICK10(LOG_TYPE_YUAN_HUN_ZHU, m_role_module_mgr->GetRole(), target_yuan_hun_zhu_param.item_id, other_yaun_hun_zhu_param.item_id, "jiao_huan", NULL, target_yuan_hun_zhu_param.eq_item_id, target_grid_data->item_unique_id, 0, 0, 0);
		ROLE_LOG_QUICK10(LOG_TYPE_YUAN_HUN_ZHU, m_role_module_mgr->GetRole(), other_yaun_hun_zhu_param.item_id, target_grid_data->item_id, "jiao_huan", NULL, other_yaun_hun_zhu_param.eq_item_id, other_grid_data->item_unique_id, 0, 0, 0);

		std::swap(*target_yuan_hun_zhu_param.param, *other_yaun_hun_zhu_param.param);

		this->SetItemDirty(item_src, index, column);
		this->SetItemDirty(other_item_src, other_index, other_column);
	}

	if (item_src == YUAN_HUN_ZHU_ITEM_SRC_WEAR_ZAO_HUA_EQUIP)
	{
		m_role_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_ZAO_HUA, __FUNCTION__);
	}

	return 0;
}

int ZaoHuaEquipManager::YuanHunZhuTakeOff(int item_src, int index, int column, int slot_index)
{
	const ItemGridData * grid_data = this->GetItemGridData(item_src, index, column);
	if (grid_data == NULL)
	{
		return -__LINE__;
	}

	YuanHunZhuParamRet yuan_hun_zhu_param = this->YuanHunZhuGetParam(grid_data, slot_index, true);
	if (!yuan_hun_zhu_param.is_ret_succ)
	{
		return -__LINE__;
	}

	if (yuan_hun_zhu_param.item_id == 0)
	{
		return -__LINE__;
	}

	NetValueItemParam tp;
	tp.yuan_hun_zhu_param = *yuan_hun_zhu_param.param;

	ItemDataWrapper take_off_yuan_hun_zhu;
	take_off_yuan_hun_zhu.item_id = yuan_hun_zhu_param.item_id;
	take_off_yuan_hun_zhu.num = 1;
	take_off_yuan_hun_zhu.is_bind = true;
	take_off_yuan_hun_zhu.has_param = 1;
	tp.GetData(take_off_yuan_hun_zhu.param_data);
	
	if (!m_role_module_mgr->GetKnapsack()->Put(take_off_yuan_hun_zhu, PUT_REASON_YUAN_HUN_ZHU_TAKE_OFF))
	{
		m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return -__LINE__;
	}

	yuan_hun_zhu_param.param->Reset();
	this->SetItemDirty(item_src, index, column);

	gamelog::g_log_yuan_hun_zhu.printf(LL_INFO, "%s line:%d | role[%d %s] item_src:%d  index:%d column:%d slot_index:%d item_id[%d to %d]  uuid:%lld", __FUNCTION__, __LINE__,
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), item_src, index, column, slot_index, yuan_hun_zhu_param.item_id, 0, grid_data->item_unique_id);

	ROLE_LOG_QUICK10(LOG_TYPE_YUAN_HUN_ZHU, m_role_module_mgr->GetRole(), take_off_yuan_hun_zhu.item_id, 0, "tuo xia", NULL, yuan_hun_zhu_param.eq_item_id, grid_data->item_unique_id, 0, 0, 0);

	if (item_src == YUAN_HUN_ZHU_ITEM_SRC_WEAR_ZAO_HUA_EQUIP)
	{
		m_role_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_ZAO_HUA, __FUNCTION__);
	}

	return 0;
}


YuanHunZhuParamRet ZaoHuaEquipManager::YuanHunZhuGetParam(const ItemGridData * grid_data, int slot_index, bool is_need_equipment)
{
	YuanHunZhuParamRet ret;
	ret.item_id = 0;
	ret.eq_item_id = 0;
	ret.is_yuan_hun_zhu = false;
	ret.is_ret_succ = false;

	if (grid_data == NULL)
	{
		return ret;
	}

	const ItemBase * item_info = ITEMPOOL->GetItem(grid_data->item_id);
	if (item_info == NULL)
	{
		return ret;
	}

	switch (item_info->GetItemType())
	{
	case I_TYPE_ZAO_HUA_EQUIP:
		{
			ZaoHuaEquipParam & eq_paramp = grid_data->param->zao_hua_param;
			if (0 <= slot_index && slot_index < ARRAY_LENGTH(eq_paramp.yuan_hun_zhu))
			{
				ret.is_ret_succ = true;
				ret.eq_item_id = grid_data->item_id;
				ret.item_id = eq_paramp.yuan_hun_zhu[slot_index].yuan_hun_zhu_item_id;
				ret.param = &eq_paramp.yuan_hun_zhu[slot_index];
			}
		}
		break;
	case I_TYPE_YUAN_HUN_ZHU:
		{
			if (!is_need_equipment)
			{
				ret.is_ret_succ = true;
				ret.item_id = grid_data->item_id;
				ret.is_yuan_hun_zhu = true;
				ret.param = &grid_data->param->yuan_hun_zhu_param;
			}
		}
		break;
	}

	return ret;
}

int ZaoHuaEquipManager::LongZhuDraw(int equip_index, int slot_index, int draw_times)
{
	ZaoHuaLongZhuSlotParam * long_zhu_slot_param = this->GetZaoHuaLongZhuSlotParam(equip_index, slot_index);
	if (!long_zhu_slot_param)
	{
		ret_line;
	}

	const ItemGridData * grid_data = this->GetItemGridData(equip_index);
	if (!grid_data)
	{
		ret_line;
	}


	int item_id = grid_data->item_id;	
	int long_zhu_level = long_zhu_slot_param->long_zhu_level;
	int bao_di_ci_shu = long_zhu_slot_param->bao_di_ci_shu;

	const ZaoHuaLongZhuCfg * cfg = LOGIC_CONFIG->GetZaoHuaEquipConfig()->GetZaoHuaLongZhuCfg(item_id);
	if (!cfg)
	{
		ret_line;
	}

	int long_zhu_max_level = std::min(LOGIC_CONFIG->GetZaoHuaEquipConfig()->GetZaoHuaLongZhuMaxLevel(), cfg->limit_max_level);

	const ZaoHuaLongZhuDrawCfg * consume_cfg = LOGIC_CONFIG->GetZaoHuaEquipConfig()->GetZaoHuaLongZhuDrawCfg(long_zhu_level);		//消耗用未升级的等级
	if (!consume_cfg)
	{
		ret_line;
	}

	const ZaoHuaEquipOtherCfg & other_cfg = LOGIC_CONFIG->GetZaoHuaEquipConfig()->GetOtherCfg();		//消耗用未升级的等级

	ZaoHuaLongZhuCiTiaoInfo old_wear_ci_tiao = long_zhu_slot_param->wear_ci_tiao;

	switch (draw_times)
	{
		case 1:
			{
				if (!m_role_module_mgr->GetKnapsack()->HasItem(other_cfg.long_zhu_draw_comsume_item_id,consume_cfg->comsume_item_num_draw_1))
				{
					ret_line;
				}

				ZaoHuaLongZhuCiTiaoInfo * free_ci_tiao = long_zhu_slot_param->LongZhuGetFreeCiTiao();
				if (!free_ci_tiao)
				{
					ret_line;
				}

				
				bao_di_ci_shu += 1;
				int rand_ci_tiao_id = LOGIC_CONFIG->GetZaoHuaEquipConfig()->GetZaoHuaLongRandZhuCiTiao(item_id, long_zhu_level);
				if (rand_ci_tiao_id == 0)
				{
					ret_line;
				}

				const ZaoHuaLongZhuDrawCfg * draw_cfg = LOGIC_CONFIG->GetZaoHuaEquipConfig()->GetZaoHuaLongZhuDrawCfg(long_zhu_level);
				if (!draw_cfg)
				{
					ret_line;
				}

				if (long_zhu_level < long_zhu_max_level)
				{
					int up_level_value = draw_cfg->GetRandUpLevel(bao_di_ci_shu);
					if (up_level_value > 0)
					{
						long_zhu_level += up_level_value;
						long_zhu_level = std::min(long_zhu_level, long_zhu_max_level);
						bao_di_ci_shu = 0;
					}
				}

				free_ci_tiao->ci_tiao_id = rand_ci_tiao_id;
				free_ci_tiao->ci_tiao_level = long_zhu_level;

				gamelog::g_log_zao_hua.printf(LL_INFO, "%s line:%d | role[%d %s] equip_index:%d slot_index:%d item_id:%d long_zhu_level:%d bao_di_ci_shu:%d get_ci_tiao_id:%d get_long_zhu_level:%d", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
					equip_index, slot_index, item_id, long_zhu_level, bao_di_ci_shu, rand_ci_tiao_id, long_zhu_level);

				m_role_module_mgr->GetKnapsack()->ConsumeItem(other_cfg.long_zhu_draw_comsume_item_id, consume_cfg->comsume_item_num_draw_1, __FUNCTION__);
			}
			break;
		case 10:
			{
				if (!m_role_module_mgr->GetKnapsack()->HasItem(other_cfg.long_zhu_draw_comsume_item_id, consume_cfg->comsume_item_num_draw_10))
				{
					ret_line;
				}

				std::vector<ZaoHuaLongZhuCiTiaoInfo> rand_ci_tiao_list;
				for (int i = 0; i < draw_times; ++i)
				{
					bao_di_ci_shu += 1;
					int rand_ci_tiao_id = LOGIC_CONFIG->GetZaoHuaEquipConfig()->GetZaoHuaLongRandZhuCiTiao(item_id, long_zhu_level);
					if (rand_ci_tiao_id == 0)
					{
						ret_line;
					}

					const ZaoHuaLongZhuDrawCfg * draw_cfg = LOGIC_CONFIG->GetZaoHuaEquipConfig()->GetZaoHuaLongZhuDrawCfg(long_zhu_level);
					if (!draw_cfg)
					{
						ret_line;
					}

					if (long_zhu_level < long_zhu_max_level)
					{
						int up_level_value = draw_cfg->GetRandUpLevel(bao_di_ci_shu);
						if (up_level_value > 0)
						{
							long_zhu_level += up_level_value;
							long_zhu_level = std::min(long_zhu_level, long_zhu_max_level);
							bao_di_ci_shu = 0;
						}
					}

					ZaoHuaLongZhuCiTiaoInfo tmp;
					tmp.ci_tiao_id = rand_ci_tiao_id;
					tmp.ci_tiao_level = long_zhu_level;
					rand_ci_tiao_list.push_back(tmp);
				}

				long_zhu_slot_param->ClearAllCiTiao();   //10连抽自动清空
				for (int i = 0, k = 0; i < (int)rand_ci_tiao_list.size() && k < ARRAY_LENGTH(long_zhu_slot_param->ci_tiao_list); ++i)
				{
					const ZaoHuaLongZhuCiTiaoInfo & curr = rand_ci_tiao_list[i];
					if (long_zhu_slot_param->wear_ci_tiao.ci_tiao_id == 0)
					{
						long_zhu_slot_param->wear_ci_tiao = curr;
					}
					else
					{
						long_zhu_slot_param->ci_tiao_list[k++] = curr;
					}

					gamelog::g_log_zao_hua.printf(LL_INFO, "%s line:%d | role[%d %s] draw_10 equip_index:%d slot_index:%d item_id:%d long_zhu_level:%d bao_di_ci_shu:%d get_ci_tiao_id:%d get_long_zhu_level:%d", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
						equip_index, slot_index, item_id, long_zhu_level, bao_di_ci_shu, curr.ci_tiao_id, curr.ci_tiao_level);
				}

				m_role_module_mgr->GetKnapsack()->ConsumeItem(other_cfg.long_zhu_draw_comsume_item_id, consume_cfg->comsume_item_num_draw_10, __FUNCTION__);
			}
			break;
		default:
			ret_line;
	}

	long_zhu_slot_param->bao_di_ci_shu = bao_di_ci_shu;
	long_zhu_slot_param->long_zhu_level = long_zhu_level;

	if (old_wear_ci_tiao.ci_tiao_id != long_zhu_slot_param->wear_ci_tiao.ci_tiao_id || old_wear_ci_tiao.ci_tiao_level != long_zhu_slot_param->wear_ci_tiao.ci_tiao_level)
	{
		m_role_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_ZAO_HUA, __FUNCTION__);
	}

	this->LongZhuSendSlotInfo(equip_index, slot_index);

	return 0;
}

int ZaoHuaEquipManager::LongZhuPutOn(int equip_index, int slot_index, int ci_tiao_index)
{
	ZaoHuaLongZhuSlotParam * long_zhu_slot_param = this->GetZaoHuaLongZhuSlotParam(equip_index, slot_index);
	if (!long_zhu_slot_param)
	{
		ret_line;
	}

	ZaoHuaLongZhuCiTiaoInfo ci_tiao = long_zhu_slot_param->ClearCiTiao(ci_tiao_index);
	ZaoHuaLongZhuCiTiaoInfo old = long_zhu_slot_param->wear_ci_tiao;
	long_zhu_slot_param->wear_ci_tiao = ci_tiao;
	this->LongZhuSendSlotInfo(equip_index, slot_index);

	gamelog::g_log_zao_hua.printf(LL_INFO, "%s line:%d | role[%d %s]  equip_index:%d slot_index:%d ci_tiao_index:%d ci_tiao_id:%d to %d,ci_tiao_lv:%d to %d", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
		equip_index, slot_index, ci_tiao_index, old.ci_tiao_id, old.ci_tiao_level, ci_tiao.ci_tiao_level, ci_tiao.ci_tiao_id);

	m_role_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_ZAO_HUA, __FUNCTION__);

	return 0;
}

int ZaoHuaEquipManager::LongZhuClear(int equip_index, int slot_index, int ci_tiao_index, bool is_clear_all)
{
	ZaoHuaLongZhuSlotParam * long_zhu_slot_param = this->GetZaoHuaLongZhuSlotParam(equip_index, slot_index);
	if (!long_zhu_slot_param)
	{
		ret_line;
	}

	if (is_clear_all)
	{
		long_zhu_slot_param->ClearAllCiTiao();

		gamelog::g_log_zao_hua.printf(LL_INFO, "%s line:%d | role[%d %s]  equip_index:%d slot_index:%d clear all", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
			equip_index, slot_index);
	}
	else
	{
		ZaoHuaLongZhuCiTiaoInfo ci_tiao = long_zhu_slot_param->ClearCiTiao(ci_tiao_index);

		gamelog::g_log_zao_hua.printf(LL_INFO, "%s line:%d | role[%d %s]  equip_index:%d slot_index:%d ci_tiao_index:%d ci_tiao_id:%d,ci_tiao_lv: %d", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
			equip_index, slot_index, ci_tiao_index, ci_tiao.ci_tiao_level, ci_tiao.ci_tiao_id);
	}

	this->LongZhuSendSlotInfo(equip_index, slot_index);

	return 0;
}

int ZaoHuaEquipManager::LongZhuUnlockSlot(int equip_index, int slot_index)
{
	ZaoHuaLongZhuPartParam * long_zhu_part_param = this->GetZaoHuaLongZhuPartParam(equip_index);
	if (!long_zhu_part_param)
	{
		ret_line;
	}

	SetBit(long_zhu_part_param->slot_unlock_flag, slot_index);

	this->LongZhuSendInfo();

	return 0;
}

ZaoHuaLongZhuSlotParam * ZaoHuaEquipManager::GetZaoHuaLongZhuSlotParam(int equip_index, int slot_index)
{
	ZaoHuaLongZhuPartParam * long_zhu_part_param = this->GetZaoHuaLongZhuPartParam(equip_index);
	if (!long_zhu_part_param)
	{
		return NULL;
	}

	if (slot_index < 0 || slot_index >= ARRAY_LENGTH(long_zhu_part_param->long_zhu_slot_list))
	{
		return NULL;
	}

	return &(long_zhu_part_param->long_zhu_slot_list[slot_index]);
}

ZaoHuaLongZhuPartParam * ZaoHuaEquipManager::GetZaoHuaLongZhuPartParam(int equip_index)
{
	if (equip_index < 0 || equip_index >= ARRAY_LENGTH(m_long_zhu_param.long_zhu_info))
	{
		return NULL;
	}

	return &(m_long_zhu_param.long_zhu_info[equip_index]);
}

void ZaoHuaEquipManager::LongZhuSendInfo()
{
	Protocol::SCZaoHuaLongZhuInfo protocol;

	for (int i = 0; i < ARRAY_LENGTH(protocol.long_zhu_info) && i < ARRAY_LENGTH(m_long_zhu_param.long_zhu_info); ++i)
	{
		protocol.long_zhu_info[i] = m_long_zhu_param.long_zhu_info[i];
	}

	m_role_module_mgr->NetSend(&protocol, sizeof(protocol));
}

void ZaoHuaEquipManager::LongZhuSendSlotInfo(int equip_index, int slot_index)
{
	ZaoHuaLongZhuSlotParam * long_zhu_slot_param = this->GetZaoHuaLongZhuSlotParam(equip_index, slot_index);
	if (!long_zhu_slot_param)
	{
		return;
	}

	Protocol::SCZaoHuaLongZhuSlotInfo protocol;
	protocol.equip_index = equip_index;
	protocol.slot_index = slot_index;
	protocol.long_zhu_slot_info = *long_zhu_slot_param;

	m_role_module_mgr->NetSend(&protocol, sizeof(protocol));
}

int ZaoHuaEquipManager::RuMo(int item_src, int index)
{
	ItemGridData * grid_data = this->GetItemGridData(item_src, index, 0);
	if (!grid_data)
	{
		ret_line;
	}

	const ItemBase * item = ITEMPOOL->GetItem(grid_data->item_id);
	if (!item || item->GetItemType() != I_TYPE_ZAO_HUA_EQUIP)
	{
		ret_line;
	}

	const ZaoHuaRuMoCfg * cfg = LOGIC_CONFIG->GetZaoHuaEquipConfig()->GetZaoHuaRuMoCfg(grid_data->item_id);
	if (!cfg)
	{
		ret_line;
	}

	const ZaoHuaEquipOtherCfg & other_cfg = LOGIC_CONFIG->GetZaoHuaEquipConfig()->GetOtherCfg();

	if (!m_role_module_mgr->GetKnapsack()->ConsumeKnapsackItem(other_cfg.ru_mo_comsume_item_id, other_cfg.ru_mo_comsume_item_num, __FUNCTION__))
	{
		ret_line;
	}

	const ItemBase * new_item = ITEMPOOL->GetItem(cfg->to_item_id);
	if (!new_item || new_item->GetItemType() != I_TYPE_ZAO_HUA_EQUIP)
	{
		ret_line;
	}

	grid_data->item_id = cfg->to_item_id;

	gamelog::g_log_zao_hua.printf(LL_INFO, "%s %d | role[%d,%s] item_uuid:%lld item_id:%d to %d", __FUNCTION__, __LINE__,
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), grid_data->item_unique_id, cfg->item_id, cfg->to_item_id);

	this->SetItemDirty(item_src, index, 0);

	if (item_src == YUAN_HUN_ZHU_ITEM_SRC_WEAR_ZAO_HUA_EQUIP)
	{
		m_role_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_ZAO_HUA, __FUNCTION__);
	}
	
	this->CheckAddItem(new_item);
	return 0;
}

void ZaoHuaEquipManager::CheckAddItem(const ItemBase * item_base)
{
	if (NULL == item_base)return;
	if (item_base->GetColor() >= ItemBase::I_COLOR_NIELLO)
	{
		m_role_module_mgr->GetCommonData().zao_hua_show_mo_zhuang = 1;
		this->SendCommonInfo();
	}
}

void ZaoHuaEquipManager::GetEquipSpecialEffect(int max_other_sp_num, short * out_other_sp_num, int * out_other_sp_list)
{
	for (std::set<int>::iterator iter = m_sp_id_list.begin(); iter != m_sp_id_list.end() && *out_other_sp_num < max_other_sp_num; ++iter)
	{				
		out_other_sp_list[*out_other_sp_num] = *iter;
		*out_other_sp_num += 1;	
	}
}

void ZaoHuaEquipManager::GetPotentialAdd(int potential_add[ZaoHuaSuitCfg::ZAO_HUA_POTENTIAL_TYPE_MAX])
{
	for (int i = 0; i < ZaoHuaSuitCfg::ZAO_HUA_POTENTIAL_TYPE_MAX; i++)
	{
		potential_add[i] = m_potential_add[i];
	}
}

void ZaoHuaEquipManager::SendWearChange(int index)
{
	if (index < ZaoHuaItem::Z_TYPE_MIN || index >= ZaoHuaItem::Z_TYPE_MAX) return;

	Protocol::SCZaoHuaEquipmentGridChange msg;

	if (NULL == m_wear_grid_list[index] || m_wear_grid_list[index]->Invalid())
	{
		msg.equip_info.index = index;
		msg.equip_info.item_id = 0;
		msg.equip_info.num = 0;
		msg.equip_info.is_bind = 0;
		msg.equip_info.item_unique_id = 0;
	}
	else
	{
		msg.equip_info.index = index;
		msg.equip_info.item_id = m_wear_grid_list[index]->item_id;
		msg.equip_info.num = m_wear_grid_list[index]->num;
		msg.equip_info.is_bind = m_wear_grid_list[index]->is_bind ? 1 : 0;
		msg.equip_info.item_unique_id = m_wear_grid_list[index]->item_unique_id;
		if (NULL != m_wear_grid_list[index]->param)
		{
			msg.equip_info.param = m_wear_grid_list[index]->param->zao_hua_param;
		}
		else
		{
			memset(&msg.equip_info.param, 0, sizeof(msg.equip_info.param));
		}
	}

	EngineAdapter::Instance().NetSend(m_role_module_mgr->GetRole()->GetNetId(), (const char*)&msg, sizeof(msg));
}

const ItemGridData * ZaoHuaEquipManager::GetBagNode(int bag_idx) const
{
	if (bag_idx < 0 || bag_idx >= ItemNamespace::MAX_ZAO_HUA_BAG_NUM)return NULL;
	return m_grid_list[bag_idx];
}

int ZaoHuaSortCompare(const void * arg1, const void * arg2)
{
	ZaoHuaSortStruct *s1 = (ZaoHuaSortStruct*)arg1;
	ZaoHuaSortStruct *s2 = (ZaoHuaSortStruct*)arg2;
	if (s1 == NULL || s2 == NULL)
	{
		if (s1 == NULL && s2 == NULL)return -1;
		else if (s1 == NULL && s2 != NULL)return 1;
		else return -1;
	}

	if (s1->prior_id != s2->prior_id) // 从大到小
	{
		return s2->prior_id > s1->prior_id ? 1 : -1;
	}
	else if (s1->is_bind != s2->is_bind)
	{
		return (char)s2->is_bind - (char)s1->is_bind;
	}
	else
	{
		return s1->src_index - s2->src_index;
	}
}
