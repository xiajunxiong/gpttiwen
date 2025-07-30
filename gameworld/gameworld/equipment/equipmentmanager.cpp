#include <memory.h>

#include "servercommon/struct/itemlistparam.h"
#include "servercommon/errornum.h"
#include "servercommon/noticenum.h"
#include "servercommon/uniqueidgenerator.hpp"

#include "engineadapter.h"
#include "equipmentmanager.h"
#include "gamelog.h"
#include "obj/character/role.h"
#include "config/logicconfigmanager.hpp"

#include "item/knapsack.h"
#include "item/itempool.h"
#include "item/itembase.h"

#include "other/rolemodulemanager.hpp"
#include "other/event/eventhandler.hpp"
#include "other/capability/capability.hpp"
#include "other/huashen/huashenmanager.hpp"
#include "other/workshop/jewelryconfig.hpp"
#include "other/workshop/gemstoneconfig.hpp"
#include "other/roleattribute/roleexpconfig.hpp"

#include "protocol/msgequipment.h"
#include "protocol/msgjewelry.h"
#include "protocol/msghuanjiezhanchang.h"
#include "servercommon/serverconfig/battleconfig/battle_special_effect_pool.hpp"
#include "item/elementcrystal/elementcrystal.hpp"
#include "config/otherconfig/sundryconfig.hpp"
#include "config/activityconfig/huanjiezhanchang/activityhuanjiezhanchangconfig.hpp"
#include "servercommon/struct/battle/battle_special_effect_def.hpp"

#include "config/otherconfig/scoresystemconfig.hpp"
#include "other/workshop/godequipupconfig.hpp"
#include "other/workshop/workshop.hpp"
#include "other/workshop/suitlistconfig.hpp"
#include "other/workshop/equipmentjewerlyupgreadeconfig.hpp"
#include "other/workshop/workshopconfig.hpp"
#include "servercommon/string/gameworldstr.h"
#include "other/rolezhuling/zhulingconfig.hpp"

#include "protocol/msgzhuling.h"
#include "other/route/mailroute.hpp"

#include "other/tianming/tianming.hpp"
#include "other/dianhuashi/dianhuashiconfig.hpp"
#include "skill/skillpool.hpp"
#include "config/otherconfig/entryconfig.hpp"
#include "protocol/msgworkshop.h"
#include "other/roleactivity/impl/roleactivityfortunemaze.hpp"
#include "other/roleactivity/roleactivitymanager.hpp"
#include "other/roleactivity/impl/roleactivitygodtoken.hpp"
#include "global/marketmanager/marketmanager.hpp"
#include "other/equipstar/equipstarconfig.hpp"
#include "chat/chatmanager.h"
#include "equipment/accessoriesmentmanager.h"

/////////////////////////////////////////////////////////////////////////////////////////////////

EquipmentManager::EquipmentManager() : m_module_mgr(NULL), need_confirm(false), m_equipment_is_change(false), m_jewelry_is_change(false)
{
	dianhuashi_id = 0;
	dianhuashi_num = 0;
	all_dianhuashi_level = 0;

	low_god_equip_level = 0;//神装最低强化等级
	has_wear_all_equip = true;

	memset(m_dirty_mark, 0, sizeof(m_dirty_mark));
	memset(m_old_state, 0, sizeof(m_old_state));

	memset(m_j_dirty_mark, 0, sizeof(m_j_dirty_mark));
	memset(m_j_old_state, 0, sizeof(m_j_old_state));

	memset(m_huashen_dirty_mark, 0, sizeof(m_huashen_dirty_mark));
	memset(m_huashen_old_state, 0, sizeof(m_huashen_old_state));

	memset(m_huashen_j_dirty_mark, 0, sizeof(m_huashen_j_dirty_mark));
	memset(m_huashen_j_old_state, 0, sizeof(m_huashen_j_old_state));
}

EquipmentManager::~EquipmentManager()
{
	for (int i = 0; i < Equipment::E_INDEX_MAX; ++i)
	{
		if (NULL != m_equipment_list[i].param)
		{
			delete m_equipment_list[i].param;
			m_equipment_list[i].param = NULL;
		}
	}
}

void EquipmentManager::Init(RoleModuleManager *module_mgr, const EquipmentOtherParam * other_param,
	int param_list_num, const ItemListParam::ItemListData * param_data_list[],
	int jewelry_list_num, const ItemListParam::ItemListData * jewelry_data_list[])
{
	m_module_mgr = module_mgr;
	m_other_param = *other_param;

	if (0 == m_other_param.element_crystal_id)
	{
		ItemID element_crystal_id = LOGIC_CONFIG->GetSundryConfig()->GetConstantCfg().init_crystal;

		const ItemBase* item = ITEMPOOL->GetItem(element_crystal_id);
		if (NULL != item)
		{
			const ElementCrystal* crystal = (const ElementCrystal*)item;
			m_other_param.element_crystal_id = element_crystal_id;
			m_other_param.element_crystal_used_times = crystal->GetCrystalLimitTimes();
		}
		else
		{
			m_other_param.element_crystal_id = 0;
			m_other_param.element_crystal_used_times = 0;
		}
	}
	if (m_other_param.element_crystal_id != 0 && m_other_param.element_crystal_id != (ItemID)-1 && m_other_param.element_crystal_unique_id == 0)
	{
		long long item_unique_id = 0;
		UniqueIDGenerator::Instance()->GenUniqueID(&item_unique_id);
		if (0 != item_unique_id)
		{
			m_other_param.element_crystal_unique_id = item_unique_id;
			gamelog::g_log_item_track.printf(LL_INFO, "TRACK_ITEM_FLAG %s assign unique_id[%lld] to role[%d,%s] item[%d] num[1]]", __FUNCTION__,
				item_unique_id, m_module_mgr->GetRole()->GetUID(), m_module_mgr->GetRole()->GetName(), m_other_param.element_crystal_id);
		}
	}
	if (m_other_param.huashen_element_crystal_unique_id != 0)
	{
		m_module_mgr->GetHuaShenManager()->PutOn(0, m_other_param.huashen_element_crystal_unique_id, HUASHEN_GRID_INDEX_TYPE_CRYSTAL);
	}

	//检查是否需要修改突破等级
	for (int upgreade_idx = 0; upgreade_idx < EQUIP_JEWELRY_TYPE_MAX; upgreade_idx++)
	{
		{//强化加成
			int &hole_level = m_other_param.equipment_jewelry_upgreade[upgreade_idx];
			int &grade_level = m_other_param.equipment_jewelry_strengthen[upgreade_idx];
			const PositionCfg * cfg = LOGIC_CONFIG->GetEquipmentJewerlyUpgreadeConfig()->GetPositionCfg(m_module_mgr->GetRole()->GetBaseProfession(), upgreade_idx, hole_level);
			if (NULL != cfg)
			{
				if (cfg->grade > grade_level + 1)//超级别，默认是老玩家号，直接修改突破等级
				{
					grade_level = cfg->grade;
				}
			}
		}
	}

	for (int i = 0; i < param_list_num && i < EQUIPMENT_GRID_MAX_COUNT; i++)
	{
		if (NULL == param_data_list[i]) break;
		if (param_data_list[i]->index >= ItemNamespace::EQUIPMENT_INDEX_BEGIN && param_data_list[i]->index < ItemNamespace::EQUIPMENT_INDEX_END)
		{
			int equip_index = param_data_list[i]->index - ItemNamespace::EQUIPMENT_INDEX_BEGIN; // index经过转换则为装备部位
			if (equip_index < 0 || equip_index >= Equipment::E_INDEX_MAX) continue;

			if (!m_equipment_list[equip_index].Invalid())
			{
				gamelog::g_log_serious_error.printf(LL_CRITICAL, "[EquipmentManager::Init Duplicate][[%s %d] equip_index:%d old_item_id:%d new_item_id:%d]", m_module_mgr->GetRole()->GetName(), UidToInt(m_module_mgr->GetRole()->GetUserId()),
					equip_index, (int)m_equipment_list[equip_index].item_id, (int)param_data_list[i]->item_wrapper.item_id);
				continue;
			}

			UInt16 item_id = param_data_list[i]->item_wrapper.item_id;
			const Equipment *equip = (const Equipment *)ITEMPOOL->GetItem(item_id);
			if (NULL == equip || equip->GetItemType() != I_TYPE_EQUIPMENT)
			{
				gamelog::g_log_serious_error.printf(LL_CRITICAL, "[EquipmentManager::Init IsNotEquipment][[%s %d] equip_index:%d item_id:%d]", m_module_mgr->GetRole()->GetName(), UidToInt(m_module_mgr->GetRole()->GetUserId()),
					equip_index, (int)item_id);
				continue;
			}

			NetValueItemParam* param = equip->CreateParamFromParamData(param_data_list[i]->item_wrapper.param_data);
			if (NULL == param) // struct_data有错
			{
				gamelog::g_log_serious_error.printf(LL_CRITICAL, "[EquipmentManager::Init StructDataError][[%s %d] equip_index:%d item_id:%d]", m_module_mgr->GetRole()->GetName(), UidToInt(m_module_mgr->GetRole()->GetUserId()),
					equip_index, (int)item_id);
				continue;
			}

			{
				//检查套装效果是否符合对应装备id
				EquipmentParam * equip_param = (EquipmentParam *)param;
				const StrengthenCfg *change_cfg = GODEQUIP_CONFIG->GetStrengthenCfg(item_id, equip_param->strengthen_level);
				if (NULL != change_cfg)
				{
					if (change_cfg->suit_id != equip_param->new_suit_effect_id)
					{
						equip_param->suit_effect_id = 0;//清空旧配置
						gamelog::g_log_item_track.printf(LL_CRITICAL, "[EquipmentManager::Init StructDataError][[%s %d] equip_index:%d item_id:%d] old_suit_id[%d] new_suit_id[%d]", m_module_mgr->GetRole()->GetName(), m_module_mgr->GetRoleUID(),
							equip_index, (int)item_id, equip_param->new_suit_effect_id, change_cfg->suit_id);
						equip_param->new_suit_effect_id = change_cfg->suit_id;//修复不对的套装id
					}
				}

				if (equip->GetSuitRandomGroup() != equip_param->new_suit_effect_id)//修复不对的套装id
				{
					equip_param->new_suit_effect_id = equip->GetSuitRandomGroup();
				}

				if (equip_param->inc_num > 0)
				{
					bool check_same = false;
					for (int attr_idx = 0; attr_idx < EquipmentParam::EXA_ATTR_NUM_MAX; attr_idx++)
					{
						AttrVocInfo &node_attr = equip_param->attr_extra_inc[attr_idx];
						if (node_attr.un_lock == 1)
						{
							if (node_attr.attr_value > 0 && !LOGIC_CONFIG->GetEntryConfig()->EntryGroupHasAttrTypr(equip->GetEntryGroup(), node_attr.attr_type))//无符合的配置
							{
								check_same = true;
							}
							for (int check_idx = attr_idx + 1; check_idx < EquipmentParam::EXA_ATTR_NUM_MAX; check_idx++)
							{
								if (node_attr.attr_value <= 0 || equip_param->attr_extra_inc[check_idx].attr_value <= 0)continue;
								if (node_attr.attr_type == equip_param->attr_extra_inc[check_idx].attr_type)
								{
									check_same = true;
								}
							}
						}
					}

					if (check_same)
					{
						//词条出现重复相同，先按照配置检查是否应该存在此词条，如果没有直接重新随机一条，如果有，按照范围重新随机一次
						RandGroupCfg entry_type_list[EquipmentParam::EXA_ATTR_NUM_MAX];
						int real_entry_num = LOGIC_CONFIG->GetEntryConfig()->GetEntryRandConfig(equip->GetEntryGroup(), equip_param->inc_num, entry_type_list, NULL);//获得新的词条列表
						if (real_entry_num > 0)
						{
							for (int new_voc_idx = 0; new_voc_idx < equip_param->inc_num && new_voc_idx < real_entry_num && new_voc_idx < EquipmentParam::EXA_ATTR_NUM_MAX; new_voc_idx++)
							{
								AttrVocInfo &node_attr = equip_param->attr_extra_inc[new_voc_idx];
								RandGroupCfg &node_new_attr = entry_type_list[new_voc_idx];
								const AttValueCfg * old_cfg = LOGIC_CONFIG->GetEntryConfig()->GetEntryAttrValueConfigBack3(equip->GetEquipLevel(), node_attr.attr_type, node_attr.attr_value);
								if (NULL != old_cfg && node_attr.attr_value > 0 && LOGIC_CONFIG->GetEntryConfig()->EntryGroupHasAttrTypr(equip->GetEntryGroup(), node_attr.attr_type))//有符合的配置
								{
									//计算数值占用范围百分比
									double attr_persnt = ((double)(node_attr.attr_value - old_cfg->min) / (old_cfg->max - old_cfg->min));
									const AttValueCfg * new_cfg = LOGIC_CONFIG->GetEntryConfig()->GetEntryAttrValueConfig3(equip->GetEquipLevel(), old_cfg->quality, node_new_attr.att_type);
									if (new_cfg != NULL)
									{
										int new_value = (int)((new_cfg->max - new_cfg->min) * attr_persnt) + new_cfg->min;
										gamelog::g_log_workshop.printf(LL_INFO, "%s equip_index[%d] user[%s,%d] voc_idx[%d] voc[attr_type:%d] [old_value:%d] [new_attr_value:%d]", __FUNCTION__,
											equip_index, m_module_mgr->GetRoleName(), m_module_mgr->GetUid(), i, node_attr.attr_type, node_attr.attr_value, new_value);
										node_attr.attr_value = new_value;
										node_attr.attr_type = node_new_attr.att_type;
									}
								}
								else
								{
									const AttValueCfg *attr_info = LOGIC_CONFIG->GetEntryConfig()->GetEntryAttrValueConfig3(equip->GetEquipLevel(), node_new_attr.quality, node_new_attr.att_type);
									if (attr_info != NULL)
									{
										int attr_value = 0;
										if (((attr_info->max - attr_info->min) + 1) > 0)attr_value = rand() % ((attr_info->max - attr_info->min) + 1) + attr_info->min;
										else attr_value = attr_info->min;
										if (attr_value > 0)
										{
											if (node_attr.un_lock == 1)
											{
												gamelog::g_log_workshop.printf(LL_INFO, "%s equip_index[%d] user[%s,%d] voc_idx[%d] old_attr[type:%d value:%d] new_attr[type:%d value:%d]", __FUNCTION__,
													equip_index, m_module_mgr->GetRoleName(), m_module_mgr->GetUid(), new_voc_idx, node_attr.attr_type, node_attr.attr_value, node_new_attr.att_type, attr_value);
												node_attr.attr_type = node_new_attr.att_type;
												node_attr.attr_value = attr_value;
											}
										}
									}
								}
							}
						}
						this->SetEquipGridDirty(equip_index, false, ROLE_TYPE_BENZUN);
					}
				}
			}
			m_equipment_list[equip_index].item_id = item_id;
			m_equipment_list[equip_index].num = param_data_list[i]->item_wrapper.num;
			m_equipment_list[equip_index].invalid_time = param_data_list[i]->item_wrapper.invalid_time;
			m_equipment_list[equip_index].param = param;
			m_equipment_list[equip_index].param->equip_param.CheckModifyVersion();
			m_equipment_list[equip_index].is_market_buy_flag = param_data_list[i]->item_wrapper.is_market_buy_flag;
			m_equipment_list[equip_index].item_unique_id = param_data_list[i]->item_wrapper.item_unique_id;
			if (m_equipment_list[equip_index].item_unique_id == 0)
			{
				long long item_unique_id = 0;
				UniqueIDGenerator::Instance()->GenUniqueID(&item_unique_id);
				if (0 != item_unique_id)
				{
					m_equipment_list[equip_index].item_unique_id = item_unique_id;
					this->SetEquipGridDirty(equip_index, false, ROLE_TYPE_BENZUN);
#ifdef TRACK_ITEM_FLAG
					gamelog::g_log_item_track.printf(LL_INFO, "TRACK_ITEM_FLAG %s assign unique_id[%lld] to role[%d,%s] item[%d,%s] num[%d] from index[%d]", __FUNCTION__,
						item_unique_id, m_module_mgr->GetRole()->GetUID(), m_module_mgr->GetRole()->GetName(),
						m_equipment_list[equip_index].item_id, equip->GetItemName(), m_equipment_list[equip_index].num, equip_index);
#endif
				}
			}

			if (m_equipment_list[equip_index].invalid_time > 0 && !equip->HasInvalidTime())
			{
				m_equipment_list[equip_index].invalid_time = 0;
			}
			m_old_state[equip_index] = true;
		}

		if (param_data_list[i]->index >= ItemNamespace::HUASHEN_EQUIPEMNET_INDEX_BEGIN && param_data_list[i]->index < ItemNamespace::HUASHEN_EQUIPEMNET_INDEX_END)
		{
			int equip_index = param_data_list[i]->index - ItemNamespace::HUASHEN_EQUIPEMNET_INDEX_BEGIN; // index经过转换则为装备部位
			if (equip_index < 0 || equip_index >= Equipment::E_INDEX_MAX) continue;

			if (!m_huashen_equipment_list[equip_index].Invalid())
			{
				gamelog::g_log_serious_error.printf(LL_CRITICAL, "[EquipmentManager::Init Duplicate huashen][[%s %d] equip_index:%d old_item_id:%d new_item_id:%d]", m_module_mgr->GetRole()->GetName(), UidToInt(m_module_mgr->GetRole()->GetUserId()),
					equip_index, (int)m_huashen_equipment_list[equip_index].item_id, (int)param_data_list[i]->item_wrapper.item_id);
				continue;
			}

			UInt16 item_id = param_data_list[i]->item_wrapper.item_id;
			const Equipment *equip = (const Equipment *)ITEMPOOL->GetItem(item_id);
			if (NULL == equip || equip->GetItemType() != I_TYPE_EQUIPMENT)
			{
				gamelog::g_log_serious_error.printf(LL_CRITICAL, "[EquipmentManager::Init IsNotEquipment huashen][[%s %d] equip_index:%d item_id:%d]", m_module_mgr->GetRole()->GetName(), UidToInt(m_module_mgr->GetRole()->GetUserId()),
					equip_index, (int)item_id);
				continue;
			}

			NetValueItemParam* param = equip->CreateParamFromParamData(param_data_list[i]->item_wrapper.param_data);
			if (NULL == param) // struct_data有错
			{
				gamelog::g_log_serious_error.printf(LL_CRITICAL, "[EquipmentManager::Init StructDataError huashen][[%s %d] equip_index:%d item_id:%d]", m_module_mgr->GetRole()->GetName(), UidToInt(m_module_mgr->GetRole()->GetUserId()),
					equip_index, (int)item_id);
				continue;
			}

			m_huashen_equipment_list[equip_index].item_id = item_id;
			m_huashen_equipment_list[equip_index].num = param_data_list[i]->item_wrapper.num;
			m_huashen_equipment_list[equip_index].invalid_time = param_data_list[i]->item_wrapper.invalid_time;
			m_huashen_equipment_list[equip_index].param = param;
			m_huashen_equipment_list[equip_index].param->equip_param.CheckModifyVersion();
			m_huashen_equipment_list[equip_index].item_unique_id = param_data_list[i]->item_wrapper.item_unique_id;
			m_module_mgr->GetHuaShenManager()->PutOn(equip_index, m_huashen_equipment_list[equip_index].item_unique_id, HUASHEN_GRID_INDEX_TYPE_EQUIPMENT);


			if (m_huashen_equipment_list[equip_index].invalid_time > 0 && !equip->HasInvalidTime())
			{
				m_huashen_equipment_list[equip_index].invalid_time = 0;
			}

			m_huashen_old_state[equip_index] = true;
		}
	}

	for (int j = 0; j < jewelry_list_num; ++j)
	{
		if (NULL == jewelry_data_list[j]) break;

		if (jewelry_data_list[j]->index >= ItemNamespace::JEWELRY_INDEX_BEGIN && jewelry_data_list[j]->index < ItemNamespace::JEWELRY_INDEX_END)
		{
			int jewelry_type = jewelry_data_list[j]->index - ItemNamespace::JEWELRY_INDEX_BEGIN; // index经过转换则为装备部位
			if (jewelry_type < 0 || jewelry_type >= Jewelry::J_TYPE_MAX) continue;

			if (!m_jewelry_list[jewelry_type].Invalid())
			{
				gamelog::g_log_serious_error.printf(LL_CRITICAL, "[EquipmentManager::Init Duplicate][[%s %d] jewelry_type:%d old_item_id:%d new_item_id:%d]", m_module_mgr->GetRole()->GetName(), UidToInt(m_module_mgr->GetRole()->GetUserId()),
					jewelry_type, (int)m_jewelry_list[jewelry_type].item_id, (int)jewelry_data_list[j]->item_wrapper.item_id);
				continue;
			}

			UInt16 item_id = jewelry_data_list[j]->item_wrapper.item_id;
			const Jewelry * jewelry = (const Jewelry *)ITEMPOOL->GetItem(item_id);
			if (NULL == jewelry || jewelry->GetItemType() != I_TYPE_JEWELRY)
			{
				gamelog::g_log_serious_error.printf(LL_CRITICAL, "[EquipmentManager::Init IsNotJewelry][[%s %d] jewelry_type:%d item_id:%d]", m_module_mgr->GetRole()->GetName(), UidToInt(m_module_mgr->GetRole()->GetUserId()),
					jewelry_type, (int)item_id);
				continue;
			}

			NetValueItemParam * param = jewelry->CreateParamFromParamData(jewelry_data_list[j]->item_wrapper.param_data);
			if (NULL == param) // struct_data有错
			{
				gamelog::g_log_serious_error.printf(LL_CRITICAL, "[EquipmentManager::Init StructDataError][[%s %d] jewelry_type:%d item_id:%d]", m_module_mgr->GetRole()->GetName(), UidToInt(m_module_mgr->GetRole()->GetUserId()),
					jewelry_type, (int)item_id);
				continue;
			}

			{
				//检查套装效果是否符合对应装备id
				JewelryParam * jewelry_param = (JewelryParam *)param;
				const StrengthenCfg *change_cfg = GODEQUIP_CONFIG->GetStrengthenCfg(item_id, jewelry_param->strengthen_level);
				if (NULL != change_cfg)
				{
					if (change_cfg->suit_id != jewelry_param->new_suit_effect_id)
					{
						jewelry_param->suit_effect_id = 0;//清空旧配置
						gamelog::g_log_item_track.printf(LL_CRITICAL, "[EquipmentManager::Init StructDataError][[%s %d] jewelry_type:%d item_id:%d] old_suit_id[%d] new_suit_id[%d]", m_module_mgr->GetRole()->GetName(), m_module_mgr->GetRoleUID(),
							jewelry_type, (int)item_id, jewelry_param->new_suit_effect_id, change_cfg->suit_id);
						jewelry_param->new_suit_effect_id = change_cfg->suit_id;//修复不对的套装id
					}
				}

				if (jewelry->GetSuitRandomGroup() != jewelry_param->new_suit_effect_id)//修复不对的套装id
				{
					jewelry_param->new_suit_effect_id = jewelry->GetSuitRandomGroup();
				}

				if (jewelry_param->inc_num > 0)
				{
					bool check_same = false;
					for (int attr_idx = 0; attr_idx < EquipmentParam::EXA_ATTR_NUM_MAX; attr_idx++)
					{
						AttrVocInfo &node_attr = jewelry_param->attr_extra_inc[attr_idx];
						if (jewelry_param->attr_extra_inc[attr_idx].un_lock == 1)
						{
							if (node_attr.attr_value > 0 && !LOGIC_CONFIG->GetEntryConfig()->EntryGroupHasAttrTypr(jewelry->GetEntryGroup(), node_attr.attr_type))//无符合的配置
							{
								check_same = true;
							}

							for (int check_idx = attr_idx + 1; check_idx < EquipmentParam::EXA_ATTR_NUM_MAX; check_idx++)
							{
								if (node_attr.attr_value <= 0 || jewelry_param->attr_extra_inc[check_idx].attr_value <= 0)continue;
								if (node_attr.attr_type == jewelry_param->attr_extra_inc[check_idx].attr_type)
								{
									check_same = true;
								}
							}
						}
					}

					if (check_same)
					{
						//词条出现重复相同，先按照配置检查是否应该存在此词条，如果没有直接重新随机一条，如果有，按照范围重新随机一次
						RandGroupCfg entry_type_list[EquipmentParam::EXA_ATTR_NUM_MAX];
						int real_entry_num = LOGIC_CONFIG->GetEntryConfig()->GetEntryRandConfig(jewelry->GetEntryGroup(), jewelry_param->inc_num, entry_type_list, NULL);//获得新的词条列表
						if (real_entry_num > 0)
						{
							for (int new_voc_idx = 0; new_voc_idx < jewelry_param->inc_num && new_voc_idx < real_entry_num && new_voc_idx < EquipmentParam::EXA_ATTR_NUM_MAX; new_voc_idx++)
							{
								AttrVocInfo &node_attr = jewelry_param->attr_extra_inc[new_voc_idx];
								RandGroupCfg &node_new_attr = entry_type_list[new_voc_idx];
								const AttValueCfg * old_cfg = LOGIC_CONFIG->GetEntryConfig()->GetEntryAttrValueConfigBack4(jewelry->GetJewelryLevel(), node_attr.attr_type, node_attr.attr_value);
								if (old_cfg != NULL && node_attr.attr_value > 0 && LOGIC_CONFIG->GetEntryConfig()->EntryGroupHasAttrTypr(jewelry->GetEntryGroup(), node_attr.attr_type))//有符合的配置
								{
									//计算数值占用范围百分比
									double attr_persnt = ((double)(node_attr.attr_value - old_cfg->min) / (old_cfg->max - old_cfg->min));
									const AttValueCfg * new_cfg = LOGIC_CONFIG->GetEntryConfig()->GetEntryAttrValueConfig3(jewelry->GetJewelryLevel(), old_cfg->quality, node_new_attr.att_type);
									if (new_cfg != NULL)
									{
										int new_value = (int)((new_cfg->max - new_cfg->min) * attr_persnt) + new_cfg->min;
										gamelog::g_log_workshop.printf(LL_INFO, "%s jewelry_index[%d] user[%s,%d] voc_idx[%d] voc[attr_type:%d] [old_value:%d] [new_attr_value:%d]", __FUNCTION__,
											jewelry_type, m_module_mgr->GetRoleName(), m_module_mgr->GetUid(), new_voc_idx, node_attr.attr_type, node_attr.attr_value, new_value);
										node_attr.attr_value = new_value;
										node_attr.attr_type = node_new_attr.att_type;
									}
								}
								else
								{
									const AttValueCfg *attr_info = LOGIC_CONFIG->GetEntryConfig()->GetEntryAttrValueConfig3(jewelry->GetJewelryLevel(), node_new_attr.quality, node_new_attr.att_type);
									if (attr_info != NULL)
									{
										int attr_value = 0;
										if (((attr_info->max - attr_info->min) + 1) > 0)attr_value = rand() % ((attr_info->max - attr_info->min) + 1) + attr_info->min;
										else attr_value = attr_info->min;
										if (attr_value > 0)
										{
											if (node_attr.un_lock == 1)
											{
												gamelog::g_log_workshop.printf(LL_INFO, "%s jewelry_index[%d] user[%s,%d] voc_idx[%d] old_attr[type:%d value:%d] new_attr[type:%d value:%d]", __FUNCTION__,
													jewelry_type, m_module_mgr->GetRoleName(), m_module_mgr->GetUid(), new_voc_idx, node_attr.attr_type, node_attr.attr_value, node_new_attr.att_type, attr_value);
												node_attr.attr_type = node_new_attr.att_type;
												node_attr.attr_value = attr_value;
											}
										}
									}
								}
							}
						}
						this->SetJewelryGridDirty(jewelry_type, false, ROLE_TYPE_BENZUN);
					}
				}
			}

			ItemGridData & t_igd = m_jewelry_list[jewelry_type];
			t_igd.item_id = item_id;
			t_igd.num = jewelry_data_list[j]->item_wrapper.num;
			t_igd.invalid_time = jewelry_data_list[j]->item_wrapper.invalid_time;
			t_igd.param = param;
			t_igd.param->jewelry_param.CheckModifyVersion();
			t_igd.is_market_buy_flag = jewelry_data_list[j]->item_wrapper.is_market_buy_flag;
			t_igd.item_unique_id = jewelry_data_list[j]->item_wrapper.item_unique_id;
			if (t_igd.item_unique_id == 0)
			{
				long long item_unique_id = 0;
				UniqueIDGenerator::Instance()->GenUniqueID(&item_unique_id);
				if (0 != item_unique_id)
				{
					t_igd.item_unique_id = item_unique_id;
					this->SetJewelryGridDirty(jewelry_type, false, ROLE_TYPE_BENZUN);
#ifdef TRACK_ITEM_FLAG
					gamelog::g_log_item_track.printf(LL_INFO, "TRACK_ITEM_FLAG %s assign unique_id[%lld] to role[%d,%s] item[%d,%s] num[%d] from index[%d]", __FUNCTION__,
						item_unique_id, m_module_mgr->GetRole()->GetUID(), m_module_mgr->GetRole()->GetName(),
						t_igd.item_id, jewelry->GetItemName(), t_igd.num, jewelry_type);
#endif
				}
			}

			if (t_igd.invalid_time > 0 && !jewelry->HasInvalidTime()) t_igd.invalid_time = 0;
			m_j_old_state[jewelry_type] = true;
		}

		if (jewelry_data_list[j]->index >= ItemNamespace::HUASHEN_JEWELRY_INDEX_BEGIN && jewelry_data_list[j]->index < ItemNamespace::HUASHEN_JEWELRY_INDEX_END)
		{
			int jewelry_type = jewelry_data_list[j]->index - ItemNamespace::HUASHEN_JEWELRY_INDEX_BEGIN; // index经过转换则为装备部位
			if (jewelry_type < 0 || jewelry_type >= Jewelry::J_TYPE_MAX) continue;

			if (!m_huashen_jewelry_list[jewelry_type].Invalid())
			{
				gamelog::g_log_serious_error.printf(LL_CRITICAL, "[EquipmentManager::Init Duplicate][[%s %d] jewelry_type:%d old_item_id:%d new_item_id:%d]", m_module_mgr->GetRole()->GetName(), UidToInt(m_module_mgr->GetRole()->GetUserId()),
					jewelry_type, (int)m_huashen_jewelry_list[jewelry_type].item_id, (int)jewelry_data_list[j]->item_wrapper.item_id);
				continue;
			}

			UInt16 item_id = jewelry_data_list[j]->item_wrapper.item_id;
			const Jewelry * jewelry = (const Jewelry *)ITEMPOOL->GetItem(item_id);
			if (NULL == jewelry || jewelry->GetItemType() != I_TYPE_JEWELRY)
			{
				gamelog::g_log_serious_error.printf(LL_CRITICAL, "[EquipmentManager::Init IsNotJewelry][[%s %d] jewelry_type:%d item_id:%d]", m_module_mgr->GetRole()->GetName(), UidToInt(m_module_mgr->GetRole()->GetUserId()),
					jewelry_type, (int)item_id);
				continue;
			}

			NetValueItemParam * param = jewelry->CreateParamFromParamData(jewelry_data_list[j]->item_wrapper.param_data);
			if (NULL == param) // struct_data有错
			{
				gamelog::g_log_serious_error.printf(LL_CRITICAL, "[EquipmentManager::Init StructDataError][[%s %d] jewelry_type:%d item_id:%d]", m_module_mgr->GetRole()->GetName(), UidToInt(m_module_mgr->GetRole()->GetUserId()),
					jewelry_type, (int)item_id);
				continue;
			}

			ItemGridData & t_igd = m_huashen_jewelry_list[jewelry_type];
			t_igd.item_id = item_id;
			t_igd.num = jewelry_data_list[j]->item_wrapper.num;
			t_igd.invalid_time = jewelry_data_list[j]->item_wrapper.invalid_time;
			t_igd.param = param;
			t_igd.param->jewelry_param.CheckModifyVersion();
			t_igd.item_unique_id = jewelry_data_list[j]->item_wrapper.item_unique_id;
			m_module_mgr->GetHuaShenManager()->PutOn(jewelry_type, t_igd.item_unique_id, HUASHEN_GRID_INDEX_TYPE_JEWELRY);

			if (t_igd.invalid_time > 0 && !jewelry->HasInvalidTime()) t_igd.invalid_time = 0;
			m_huashen_j_old_state[jewelry_type] = true;
		}
	}
}

void EquipmentManager::GetInitParam(EquipmentOtherParam* other_param)
{
	if (NULL != other_param)
	{
		*other_param = m_other_param;
	}
}

void EquipmentManager::OnLogin()
{
	this->CheckNewSuitChange();//修复升星出现的套装漏洞
	this->SendResonaceInfo();
}

void EquipmentManager::Update(time_t now_second)
{

}

const AttributeList & EquipmentManager::ReCalcAttr(AttributeList & base_add, bool is_recalc, int role_type, AttributeList * equip_special_attr)
{
	AttributeList* attr_add = NULL;
	ItemGridData * equipment_list = NULL;
	ItemGridData * jewelry_list = NULL;
	ItemID* element_crystal_id = NULL;
	short* element_crystal_used_times = NULL;
	if (role_type == ROLE_TYPE_BENZUN)
	{
		attr_add = &m_attrs_add;
		equipment_list = m_equipment_list;
		jewelry_list = m_jewelry_list;
		element_crystal_id = &m_other_param.element_crystal_id;
		element_crystal_used_times = &m_other_param.element_crystal_used_times;
	}
	else
	{
		attr_add = &m_huashen_attr_add;
		equipment_list = m_huashen_equipment_list;
		jewelry_list = m_huashen_jewelry_list;
		element_crystal_id = &m_other_param.huashen_element_crystal_id;
		element_crystal_used_times = &m_other_param.huashen_element_crystal_used_times;
	}
	
	if (is_recalc)
	{
		attr_add->Reset();
		if (ROLE_TYPE_HUASHEN == role_type)
		{
			m_huashen_speical_logic_attrs.Reset();
		}

		// 装备加成
		Attribute m_exa_attrs[BATTLE_ATTR_MAX];
		memset(m_exa_attrs, 0, sizeof(m_exa_attrs));
		int suit_count = 0;
		SuitEffectReCalc equip_suit_list[MAX_SUIT_TYPE];
		bool all_wear = true;
		bool wear_idx[EQUIP_JEWELRY_TYPE_MAX];
		memset(wear_idx, false, sizeof(wear_idx));

		dianhuashi_id = 0;
		dianhuashi_num = 0;
		all_dianhuashi_level = 0;
		
		low_god_equip_level = 0;//神装最低强化等级
		has_wear_all_equip = true;

		for (int equip_idx = 0; equip_idx < Equipment::E_INDEX_MAX; ++equip_idx)
		{
			if (equipment_list[equip_idx].Invalid())
			{
				has_wear_all_equip = false;
				continue;
			}

			const Equipment* equip = (const Equipment*)(ITEMPOOL->GetItem(equipment_list[equip_idx].item_id));
			if (NULL == equip || I_TYPE_EQUIPMENT != equip->GetItemType()) continue;

			int limit_prof = equip->GetLimitProf();
			if (-1 != limit_prof && limit_prof != m_module_mgr->GetRole()->GetBaseProfession()) continue;

			EquipmentParam & p_eqed_equ = equipment_list[equip_idx].param->equip_param;
			for (int t_a_n = 0; t_a_n < p_eqed_equ.attr_num && t_a_n < EquipmentParam::ATTR_NUM_MAX; ++t_a_n)
			{
				AttrInfo & p_eai = p_eqed_equ.attr_list[t_a_n];
				if (p_eai.attr_type < 0 || p_eai.attr_type >= BATTLE_ATTR_MAX) continue;

				attr_add->m_attrs[p_eai.attr_type] += p_eai.attr_value;
				if (p_eqed_equ.ultimate_attr_list[t_a_n] > 0)
					attr_add->m_attrs[p_eai.attr_type] += p_eqed_equ.ultimate_attr_list[t_a_n];

				// 天命装备 基础属性加成
				int attr_total_per = m_module_mgr->GetTianMing()->GetAttrPer(TIAN_MING_ADD_ATTR_SYS_TYPE_EQUIPMENT);
				int tian_ming_add_val = int((p_eai.attr_value + p_eqed_equ.ultimate_attr_list[t_a_n]) * 1.0f * attr_total_per / 10000);
				attr_add->m_attrs[p_eai.attr_type] += tian_ming_add_val;
			}
			if (equip->GetColor() >= ItemBase::I_COLOR_GOD_COLOR_BEGAIN && equip->GetColor() <= ItemBase::I_COLOR_GOD_COLOR_END)
			{
				if (p_eqed_equ.strengthen_level > low_god_equip_level)
				{
					low_god_equip_level = p_eqed_equ.strengthen_level;
				}
			}
			else
			{
				has_wear_all_equip = false;
			}
			// 额外属性提升
			AttributePlan tmp_ap;
			for (int attr_inc_idx = 0; attr_inc_idx < p_eqed_equ.inc_num && attr_inc_idx < EquipmentParam::EXA_ATTR_NUM_MAX; ++attr_inc_idx)
			{
				const AttrVocInfo & inc_con = p_eqed_equ.attr_extra_inc[attr_inc_idx];
				if (inc_con.attr_type < 0 || inc_con.attr_type >= BATTLE_ATTR_MAX) continue;

				if (inc_con.attr_type < BATTLE_ATTR_ELEMENT_END)
				{
					m_exa_attrs[inc_con.attr_type] += inc_con.attr_value;
				}
				else
				{
					tmp_ap.add_point_list[inc_con.attr_type - BATTLE_ADD_POINT_TYPE_BEGIN] = inc_con.attr_value;
				}
			}
			AttributeList extra_attrs;
			LOGIC_CONFIG->GetRoleExpConfig()->ConvertPointToAttribute(tmp_ap, extra_attrs);
			attr_add->Add(extra_attrs.m_attrs);
			m_huashen_speical_logic_attrs.Add(extra_attrs.m_attrs);

			//套装效果
			if (p_eqed_equ.new_suit_effect_id > 0)
			{
				bool has_suit_add = false;
				for (int suit_idx = 0; suit_idx < suit_count && suit_idx < MAX_SUIT_TYPE; suit_idx++)//先检查是否有同类组合，件数堆叠
				{
					SuitEffectReCalc &node_cale = equip_suit_list[suit_idx];
					if (node_cale.sp_id == p_eqed_equ.new_suit_effect_id)
					{
						//检查多件套中最高品质
						bool has_insert = false;
						for (int suit_num_begin = 0; suit_num_begin < node_cale.num; suit_num_begin++)
						{
							if ((int)node_cale.color_top.size() <= suit_num_begin)continue;
							if (node_cale.color_top[suit_num_begin] < equip->GetColor())
							{
								std::vector<int>::iterator it = node_cale.color_top.begin();
								node_cale.color_top.insert(it + suit_num_begin, equip->GetColor());
								has_insert = true;
								break;
							}
						}
						if (!has_insert)
						{
							node_cale.color_top.push_back(equip->GetColor());
						}

						node_cale.num += 1;
						has_suit_add = true;
						break;
					}
				}
				if (!has_suit_add)
				{
					equip_suit_list[suit_count].sp_id = p_eqed_equ.new_suit_effect_id;
					equip_suit_list[suit_count].color_top.push_back(equip->GetColor());
					equip_suit_list[suit_count++].num = 1;
				}
			}

			// 宝石属性
			for (int gem_grid = 0; gem_grid < ARRAY_LENGTH(p_eqed_equ.new_stone); ++gem_grid)
			{
				if (0 == p_eqed_equ.new_stone[gem_grid]) continue;

				const GemstoneBasicInfoCfg * gbic = LOGIC_CONFIG->GetGemstoneConfig()->GetGemstoneBasicInfoCfg(p_eqed_equ.new_stone[gem_grid]);
				if (NULL == gbic) continue;

				for (int gem_attr_num = 0; gem_attr_num < GEM_ATTR_COUNT_MAX; ++gem_attr_num)
				{
					const BattleAttrPair & gem_bap = gbic->attr_add[gem_attr_num];
					if (0 == gem_bap.attr_value) continue;

					attr_add->m_attrs[gem_bap.attr_type] += gem_bap.attr_value;

					// 天命 宝石 基础属性加成
					int attr_total_per = m_module_mgr->GetTianMing()->GetAttrPer(TIAN_MING_ADD_ATTR_SYS_TYPE_GEM);
					int tian_ming_add_val = int(gem_bap.attr_value * 1.0f * attr_total_per / 10000);
					attr_add->m_attrs[gem_bap.attr_type] += tian_ming_add_val;
				}
			}


			// 装备铸灵属性
			std::map<int, int> zhu_ling_color_num;
			std::map<int, int> zhu_ling_base_attr;  //用来计算下面组合属性加成
			std::map<int, std::vector<int>  > zhu_ling_color_level;
			for (int zhu_ling_grid = 0; zhu_ling_grid < EQUIP_ZHU_LING_SLOT_NUM; ++zhu_ling_grid)
			{
				const ZhuLinParam & curr = p_eqed_equ.zhu_ling_info[zhu_ling_grid];
				if (curr.zhu_ling_item_id == 0)
				{
					continue;
				}

				const ZhuLingCfg * zhu_ling_cfg = LOGIC_CONFIG->GetZhuLingConfig()->GetZhuLingCfg(curr.zhu_ling_item_id);
				if (NULL == zhu_ling_cfg)
				{
					continue;
				}

				for (int i = 0; i < (int)zhu_ling_cfg->attr_list.size(); ++i)
				{
					const ZhuLingCfg::AttrItem & zhu_ling_attr = zhu_ling_cfg->attr_list[i];
					if (0 <= zhu_ling_attr.attr_type && zhu_ling_attr.attr_type < BATTLE_ATTR_MAX)
					{
						attr_add->m_attrs[zhu_ling_attr.attr_type] += zhu_ling_attr.attr_value;
						zhu_ling_base_attr[zhu_ling_attr.attr_type] += zhu_ling_attr.attr_value;
					}
				}

				const ItemBase * item_info = ITEMPOOL->GetItem(curr.zhu_ling_item_id);
				if (item_info)
				{
					int color = item_info->GetColor();
					zhu_ling_color_num[color] += 1;
					zhu_ling_color_level[color].push_back(zhu_ling_cfg->zhu_ling_level);
				}
			}

			//铸灵组合属性
			int zhu_ling_combination_level = 0;
			const ZhuLingCombinationCfg * zhu_ling_combination_cfg = LOGIC_CONFIG->GetZhuLingConfig()->GetZhuLingCombinationCfg(zhu_ling_color_num, zhu_ling_color_level, zhu_ling_combination_level);
			if (zhu_ling_combination_cfg)
			{
				ZhuLingCombinationAttrCfgKey key;
				key.base_prof = m_module_mgr->GetRole()->GetBaseProfession();
				key.combination_level = zhu_ling_combination_level;
				key.equip_type = equip_idx;
				key.group_id = zhu_ling_combination_cfg->attr_group_id;
				const ZhuLingCombinationAttrCfg * zhu_ling_combination_attr_cfg = LOGIC_CONFIG->GetZhuLingConfig()->GetZhuLingCombinationAttrCfg(key);
				if (zhu_ling_combination_attr_cfg)
				{
					for (int i = 0; i < (int)zhu_ling_combination_attr_cfg->attr_cfg.size(); ++i)
					{
						const ZhuLingCombinationAttrCfg::AttrItemCfg & zhu_ling_attr = zhu_ling_combination_attr_cfg->attr_cfg[i];
						if (0 <= zhu_ling_attr.type && zhu_ling_attr.type < BATTLE_ATTR_MAX)
						{
							attr_add->m_attrs[zhu_ling_attr.type] += zhu_ling_attr.value;
						}
					}

					for (int i = 0; i < (int)zhu_ling_combination_attr_cfg->attr_percentage_cfg.size(); ++i)
					{
						const ZhuLingCombinationAttrCfg::AttrItemCfg & zhu_ling_attr = zhu_ling_combination_attr_cfg->attr_percentage_cfg[i];

						if (zhu_ling_base_attr.find(zhu_ling_attr.type) == zhu_ling_base_attr.end())
						{
							continue;
						}

						if (0 <= zhu_ling_attr.type && zhu_ling_attr.type < BATTLE_ATTR_MAX)
						{
							Attribute add_value = (Attribute)(zhu_ling_attr.value / 10000.0 * zhu_ling_base_attr[zhu_ling_attr.type]);
							attr_add->m_attrs[zhu_ling_attr.type] += add_value;
						}
					}
				}
			}

			// 仙饰加成
			int special_level = m_module_mgr->GetAccessoriesmentManager()->GetAccessoriesSpecialLevel();
			if (special_level > 0)
			{
				const PeiShiSpecialEquipCfg * special_cfg = LOGIC_CONFIG->GetPeiShiConfig()->GetSpecialEquip(PeiShiSpecialEquipCfg::SPECIAL_TYPE_2, special_level);
				if (NULL != special_cfg)
				{
					if (special_cfg->spec_value_2 > 0)
					{
						for (std::map<int, int>::iterator it = zhu_ling_base_attr.begin(); it != zhu_ling_base_attr.end(); it++)
						{
							if (0 <= it->first && it->first < BATTLE_ATTR_MAX)
							{
								int special_value = int(it->second * 1.0f * special_cfg->spec_value_2 / 1000);
								attr_add->m_attrs[it->first] += special_value;
							}
						}
					}
				}
			}

			LOGIC_CONFIG->GetRoleExpConfig()->ConvertPointToAttribute2(*attr_add);

			if (p_eqed_equ.sp_id > 0) // 有特效,则需要看是否加属性的特效
			{
				const SpecialEffectCfg* spe_cfg = BattleSpecialEffectPool::GetInstance().GetSpecialEffectCfg(p_eqed_equ.sp_id);
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
							attr_add->m_attrs[detail_cfg->param_list[0]] += detail_cfg->param_list[1];
						}

						if (SPE_ATTR_ADD_ATTR_BY_LEVEL == detail_cfg->effect_sub_type && detail_cfg->param_num >= 2
							&& detail_cfg->param_list[0] >= BATTLE_ATTR_MIN && detail_cfg->param_list[0] < BATTLE_ATTR_MAX)
						{
							BattleAttr add_attr = static_cast<BattleAttr>(1.0f * m_module_mgr->GetRole()->GetLevel() * detail_cfg->param_list[1] / BATTLE_PERCENT_NUM + 0.5f);
							attr_add->m_attrs[detail_cfg->param_list[0]] += add_attr;
						}
					}
				}
			}

			if (p_eqed_equ.effect_id > 0 || p_eqed_equ.effect_level > 0)
			{
				{
					const DianHuaSpecialEffectDetailCfg * dianhuashi_cfg = LOGIC_CONFIG->GetDianHuaShiConfig()->GetEffectDetailCfg(p_eqed_equ.effect_id, p_eqed_equ.effect_level);
					if (NULL != dianhuashi_cfg)
					{
						AttributePlan tmp_ap;
						if (dianhuashi_cfg->bonus_type < BATTLE_ATTR_ELEMENT_END)
						{
							m_exa_attrs[dianhuashi_cfg->bonus_type] += dianhuashi_cfg->bonus_attributes;
						}
						else
						{
							tmp_ap.add_point_list[dianhuashi_cfg->bonus_type - BATTLE_ADD_POINT_TYPE_BEGIN] += dianhuashi_cfg->bonus_attributes;
						}
						AttributeList extra_attrs;
						LOGIC_CONFIG->GetRoleExpConfig()->ConvertPointToAttribute(tmp_ap, extra_attrs);
						attr_add->Add(extra_attrs.m_attrs);
					}
				}

				{
					if (dianhuashi_id == 0)
					{
						dianhuashi_id = p_eqed_equ.effect_id;
					}
					else
					{
						if (dianhuashi_id != p_eqed_equ.effect_id)dianhuashi_num = 0;//有点化石不一样
						dianhuashi_id = p_eqed_equ.effect_id;
					}

					dianhuashi_num += 1;
					all_dianhuashi_level += p_eqed_equ.effect_level;
				}
			}

			wear_idx[equip_idx] = true;
		}

		//点化石套装
		if (dianhuashi_id > 0 && dianhuashi_num >= Equipment::E_INDEX_MAX && all_dianhuashi_level > 0)
		{
			const DianHuaSkillLevelCfg * level_cfg = LOGIC_CONFIG->GetDianHuaShiConfig()->GetSkillLevelCfg(all_dianhuashi_level);
			if (NULL != level_cfg)
			{
				const DianHuaSpecialEffectDetailCfg * dianhuashi_cfg = LOGIC_CONFIG->GetDianHuaShiConfig()->GetEffectDetailCfg(dianhuashi_id, level_cfg->skill_level);
				if (NULL != dianhuashi_cfg)
				{
					const GamePassiveSkillCfg * passive_skill = SkillPool::Instance().GetPassiveSkillCfg(dianhuashi_cfg->set_skills);
					if (NULL != passive_skill)
					{
						// 专属技能是有技能等级的，注意！
						int skill_level = level_cfg->skill_level;
						int spid = passive_skill->sp_id;
						if (passive_skill->is_level_spid && skill_level >= 1 && skill_level < ARRAY_LENGTH(passive_skill->level_spid_list))
						{
							spid = passive_skill->level_spid_list[skill_level];
						}
						const SpecialEffectCfg* spe_cfg = BattleSpecialEffectPool::GetInstance().GetSpecialEffectCfg(spid);
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
									attr_add->m_attrs[detail_cfg->param_list[0]] += detail_cfg->param_list[1];
								}

								if (SPE_ATTR_ADD_ATTR_BY_LEVEL == detail_cfg->effect_sub_type && detail_cfg->param_num >= 2
									&& detail_cfg->param_list[0] >= BATTLE_ATTR_MIN && detail_cfg->param_list[0] < BATTLE_ATTR_MAX)
								{
									BattleAttr add_attr = static_cast<BattleAttr>(1.0f * m_module_mgr->GetRole()->GetLevel() * detail_cfg->param_list[1] / BATTLE_PERCENT_NUM + 0.5f);
									attr_add->m_attrs[detail_cfg->param_list[0]] += add_attr;
								}
							}
						}
					}
				}
			}
		}

		//套装效果赋予
		for (int suit_idx = 0; suit_idx < suit_count && suit_idx < MAX_SUIT_TYPE; suit_idx++)//先检查是否有同类组合，件数堆叠
		{
			SuitEffectReCalc & suit_node = equip_suit_list[suit_idx];
			for (int suit_num = 0; suit_num < suit_node.num && suit_num < MAX_SUIT_TYPE; suit_num++)
			{
				if ((int)suit_node.color_top.size() <= suit_num)continue;
				const SuitEffect * suit_cfg = LOGIC_CONFIG->GetSuitListConfig()->GetSuitEffectList(suit_node.sp_id, suit_node.color_top[suit_num], suit_num + 1);
				if (suit_cfg == NULL)continue;

				//满足套装增加属性
				for (int effect_value_idx = 0; effect_value_idx < MAX_SUIT_EFFECT_VALUE_NUM; effect_value_idx++)
				{
					const SuitValue &suit_value = suit_cfg->m_value[effect_value_idx];
					if (suit_value.attr_type < 0 || suit_value.attr_type >= BATTLE_ATTR_MAX) continue;
					attr_add->m_attrs[suit_value.attr_type] += suit_value.attr_add;
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
								attr_add->m_attrs[detail_cfg->param_list[0]] += detail_cfg->param_list[1];
							}

							if (SPE_ATTR_ADD_ATTR_BY_LEVEL == detail_cfg->effect_sub_type && detail_cfg->param_num >= 2
								&& detail_cfg->param_list[0] >= BATTLE_ATTR_MIN && detail_cfg->param_list[0] < BATTLE_ATTR_MAX)
							{
								BattleAttr add_attr = static_cast<BattleAttr>(1.0f * m_module_mgr->GetRole()->GetLevel() * detail_cfg->param_list[1] / BATTLE_PERCENT_NUM + 0.5f);
								attr_add->m_attrs[detail_cfg->param_list[0]] += add_attr;
							}
						}
					}
				}
			}
		}

		for (int attr_inc_idx2 = 0; attr_inc_idx2 < BATTLE_ATTR_MAX; attr_inc_idx2++)
		{
			/* 不再需要除以10， 因为属性已改为按千分比计算 -- by chenyulin
			switch (attr_inc_idx2)//特殊，词条用的是千分比，所以最终值除以10
			{
			case BATTLE_ATTR_PHYSICAL_DEC_DAMAGE:
			case BATTLE_ATTR_MAGIC_DEC_DAMAGE:
			case BATTLE_ATTR_PHYSICAL_INC_DAMAGE:
			case BATTLE_ATTR_MAGIC_INC_DAMAGE:

			case BATTLE_ATTR_CRITICAL:
			case BATTLE_ATTR_HIT:
			case BATTLE_ATTR_DODGE:
			case BATTLE_ATTR_COUNTER_ATTACK:

			case BATTLE_ATTR_ANTI_POISON:
			case BATTLE_ATTR_ANTI_FREEZE:
			case BATTLE_ATTR_ANTI_PETRIFY:
			case BATTLE_ATTR_ANTI_CHAOS:
			case BATTLE_ATTR_ANTI_DRUNK:
			m_exa_attrs[attr_inc_idx2] = m_exa_attrs[attr_inc_idx2] / 10;
			break;
			}
			*/
			attr_add->m_attrs[attr_inc_idx2] += m_exa_attrs[attr_inc_idx2];
			m_huashen_speical_logic_attrs.m_attrs[attr_inc_idx2] += m_exa_attrs[attr_inc_idx2];
		}

		memset(m_exa_attrs, 0, sizeof(m_exa_attrs));

		suit_count = 0;
		for (int i = 0; i < ARRAY_ITEM_COUNT(equip_suit_list); ++i)
		{
			equip_suit_list->Reset();
		}
		// 灵饰加成
		bool need_calc_suit = true;// SuitBasicCfg calc_sbc;
		for (int jewelry_type = 0; jewelry_type < Jewelry::J_TYPE_MAX; ++jewelry_type)
		{
			if (jewelry_list[jewelry_type].Invalid() || NULL == jewelry_list[jewelry_type].param)		// 没有装备
			{
				if (need_calc_suit) need_calc_suit = false;
				has_wear_all_equip = false;
				continue;
			}

			const Jewelry* jewelry = (const Jewelry*)(ITEMPOOL->GetItem(jewelry_list[jewelry_type].item_id));
			if (NULL == jewelry || I_TYPE_JEWELRY != jewelry->GetItemType()) continue;

			int limit_prof = jewelry->GetLimitProf();
			if (-1 != limit_prof && limit_prof != m_module_mgr->GetRole()->GetBaseProfession()) continue;

			JewelryParam & p_eqed_jew = jewelry_list[jewelry_type].param->jewelry_param;

			if (jewelry->GetColor() >= ItemBase::I_COLOR_GOD_COLOR_BEGAIN && jewelry->GetColor() <= ItemBase::I_COLOR_GOD_COLOR_END)
			{
				if (p_eqed_jew.strengthen_level > low_god_equip_level)
				{
					low_god_equip_level = p_eqed_jew.strengthen_level;
				}
			}
			else
			{
				has_wear_all_equip = false;
			}

			// 基础属性
			for (int t_a_n = 0; t_a_n < p_eqed_jew.attr_num && t_a_n < JewelryParam::ATTR_NUM_MAX; ++t_a_n)
			{
				AttrInfo & p_eai = p_eqed_jew.attr_list[t_a_n];
				if (p_eai.attr_type < 0 || p_eai.attr_type >= BATTLE_ATTR_MAX) continue;

				attr_add->m_attrs[p_eai.attr_type] += p_eai.attr_value;
				if (p_eqed_jew.ultimate_attr_list[t_a_n] > 0)attr_add->m_attrs[p_eai.attr_type] += p_eqed_jew.ultimate_attr_list[t_a_n];

				// 天命装备 基础属性加成 [灵饰居然和装备不再一个数组里..]
				int attr_total_per = m_module_mgr->GetTianMing()->GetAttrPer(TIAN_MING_ADD_ATTR_SYS_TYPE_EQUIPMENT);
				int tian_ming_add_val = int((p_eai.attr_value + p_eqed_jew.ultimate_attr_list[t_a_n]) * 1.0f * attr_total_per / 10000);
				attr_add->m_attrs[p_eai.attr_type] += tian_ming_add_val;
			}

			// 灵饰铸灵属性
			std::map<int, int> zhu_ling_color_num;
			std::map<int, int> zhu_ling_base_attr;  //用来计算下面组合属性加成
			std::map<int, std::vector<int>  > zhu_ling_color_level;
			for (int zhu_ling_grid = 0; zhu_ling_grid < EQUIP_ZHU_LING_SLOT_NUM; ++zhu_ling_grid)
			{
				const ZhuLinParam & curr = p_eqed_jew.zhu_ling_info[zhu_ling_grid];
				if (curr.zhu_ling_item_id == 0)
				{
					continue;
				}

				const ZhuLingCfg * zhu_ling_cfg = LOGIC_CONFIG->GetZhuLingConfig()->GetZhuLingCfg(curr.zhu_ling_item_id);
				if (NULL == zhu_ling_cfg)
				{
					continue;
				}

				for (int i = 0; i < (int)zhu_ling_cfg->attr_list.size(); ++i)
				{
					const ZhuLingCfg::AttrItem & zhu_ling_attr = zhu_ling_cfg->attr_list[i];
					if (0 <= zhu_ling_attr.attr_type && zhu_ling_attr.attr_type < BATTLE_ATTR_MAX)
					{
						attr_add->m_attrs[zhu_ling_attr.attr_type] += zhu_ling_attr.attr_value;
						zhu_ling_base_attr[zhu_ling_attr.attr_type] += zhu_ling_attr.attr_value;
					}
				}

				const ItemBase * item_info = ITEMPOOL->GetItem(curr.zhu_ling_item_id);
				if (item_info)
				{
					int color = item_info->GetColor();
					zhu_ling_color_num[color] += 1;
					zhu_ling_color_level[color].push_back(zhu_ling_cfg->zhu_ling_level);
				}
			}

			//铸灵组合属性
			int zhu_ling_combination_level = 100;
			const ZhuLingCombinationCfg * zhu_ling_combination_cfg = LOGIC_CONFIG->GetZhuLingConfig()->GetZhuLingCombinationCfg(zhu_ling_color_num, zhu_ling_color_level, zhu_ling_combination_level);
			if (zhu_ling_combination_cfg)
			{
				ZhuLingCombinationAttrCfgKey key;
				key.base_prof = m_module_mgr->GetRole()->GetBaseProfession();
				key.combination_level = zhu_ling_combination_level;
				key.equip_type = jewelry_type + 200;
				key.group_id = zhu_ling_combination_cfg->attr_group_id;
				const ZhuLingCombinationAttrCfg * zhu_ling_combination_attr_cfg = LOGIC_CONFIG->GetZhuLingConfig()->GetZhuLingCombinationAttrCfg(key);
				if (zhu_ling_combination_attr_cfg)
				{
					for (int i = 0; i < (int)zhu_ling_combination_attr_cfg->attr_cfg.size(); ++i)
					{
						const ZhuLingCombinationAttrCfg::AttrItemCfg & zhu_ling_attr = zhu_ling_combination_attr_cfg->attr_cfg[i];
						if (0 <= zhu_ling_attr.type && zhu_ling_attr.type < BATTLE_ATTR_MAX)
						{
							attr_add->m_attrs[zhu_ling_attr.type] += zhu_ling_attr.value;
						}
					}

					for (int i = 0; i < (int)zhu_ling_combination_attr_cfg->attr_percentage_cfg.size(); ++i)
					{
						const ZhuLingCombinationAttrCfg::AttrItemCfg & zhu_ling_attr = zhu_ling_combination_attr_cfg->attr_percentage_cfg[i];

						if (zhu_ling_base_attr.find(zhu_ling_attr.type) == zhu_ling_base_attr.end())
						{
							continue;
						}

						if (0 <= zhu_ling_attr.type && zhu_ling_attr.type < BATTLE_ATTR_MAX)
						{
							Attribute add_value = (Attribute)(zhu_ling_attr.value / 10000.0 * zhu_ling_base_attr[zhu_ling_attr.type]);
							attr_add->m_attrs[zhu_ling_attr.type] += add_value;
						}
					}
				}
			}

			// 仙饰加成
			int special_level = m_module_mgr->GetAccessoriesmentManager()->GetAccessoriesSpecialLevel();
			if (special_level > 0)
			{
				const PeiShiSpecialEquipCfg * special_cfg = LOGIC_CONFIG->GetPeiShiConfig()->GetSpecialEquip(PeiShiSpecialEquipCfg::SPECIAL_TYPE_2, special_level);
				if (NULL != special_cfg)
				{
					if (special_cfg->spec_value_2 > 0)
					{
						for (std::map<int, int>::iterator it = zhu_ling_base_attr.begin(); it != zhu_ling_base_attr.end(); it++)
						{
							if (0 <= it->first && it->first < BATTLE_ATTR_MAX)
							{
								int special_value = int(it->second * 1.0f * special_cfg->spec_value_2 / 1000);
								attr_add->m_attrs[it->first] += special_value;
							}
						}
					}
				}
			}

			LOGIC_CONFIG->GetRoleExpConfig()->ConvertPointToAttribute2(*attr_add);

			//套装效果
			if (p_eqed_jew.new_suit_effect_id > 0)
			{
				bool has_suit_add = false;
				for (int suit_idx = 0; suit_idx < suit_count && suit_idx < MAX_SUIT_TYPE; suit_idx++)//先检查是否有同类组合，件数堆叠
				{
					SuitEffectReCalc & node_cale = equip_suit_list[suit_idx];
					if (node_cale.sp_id == p_eqed_jew.new_suit_effect_id)
					{
						//检查多件套中最高品质
						bool has_insert = false;
						for (int suit_num_begin = 0; suit_num_begin < node_cale.num; suit_num_begin++)
						{
							if ((int)node_cale.color_top.size() <= suit_num_begin)continue;
							if (node_cale.color_top[suit_num_begin] < jewelry->GetColor())
							{
								std::vector<int>::iterator it = node_cale.color_top.begin();
								node_cale.color_top.insert(it + suit_num_begin, jewelry->GetColor());
								has_insert = true;
								break;
							}
						}
						if (!has_insert)
						{
							node_cale.color_top.push_back(jewelry->GetColor());
						}

						node_cale.num += 1;
						has_suit_add = true;
						break;
					}
				}
				if (!has_suit_add)
				{
					equip_suit_list[suit_count].sp_id = p_eqed_jew.new_suit_effect_id;
					equip_suit_list[suit_count].color_top.push_back(jewelry->GetColor());
					equip_suit_list[suit_count++].num = 1;
				}
			}

			// 额外属性提升
			AttributePlan tmp_ap;

			for (int attr_inc_idx = 0; attr_inc_idx < p_eqed_jew.inc_num && attr_inc_idx < JewelryParam::EXA_ATTR_NUM_MAX; ++attr_inc_idx)
			{
				const AttrVocInfo & inc_con = p_eqed_jew.attr_extra_inc[attr_inc_idx];
				if (inc_con.attr_type < 0 || inc_con.attr_type >= BATTLE_ATTR_MAX) continue;

				if (inc_con.attr_type < BATTLE_ATTR_ELEMENT_END)
				{
					m_exa_attrs[inc_con.attr_type] += inc_con.attr_value;
				}
				else
				{
					tmp_ap.add_point_list[inc_con.attr_type - BATTLE_ADD_POINT_TYPE_BEGIN] = inc_con.attr_value;
				}
			}

			for (int gem_grid = 0; gem_grid < ARRAY_LENGTH(p_eqed_jew.new_stone); ++gem_grid)
			{
				if (0 == p_eqed_jew.new_stone[gem_grid]) continue;

				const GemstoneBasicInfoCfg * gbic = LOGIC_CONFIG->GetGemstoneConfig()->GetGemstoneBasicInfoCfg(p_eqed_jew.new_stone[gem_grid]);
				if (NULL == gbic) continue;

				for (int gem_attr_num = 0; gem_attr_num < GEM_ATTR_COUNT_MAX; ++gem_attr_num)
				{
					const BattleAttrPair & gem_bap = gbic->attr_add[gem_attr_num];
					if (0 == gem_bap.attr_value) continue;

					attr_add->m_attrs[gem_bap.attr_type] += gem_bap.attr_value;

					// 天命 宝石 基础属性加成[灵饰居然和装备不再一个数组里..]
					int attr_total_per = m_module_mgr->GetTianMing()->GetAttrPer(TIAN_MING_ADD_ATTR_SYS_TYPE_GEM);
					int tian_ming_add_val = int(gem_bap.attr_value * 1.0f * attr_total_per / 10000);
					attr_add->m_attrs[gem_bap.attr_type] += tian_ming_add_val;
				}
			}
			AttributeList extra_attrs;
			LOGIC_CONFIG->GetRoleExpConfig()->ConvertPointToAttribute(tmp_ap, extra_attrs);
			attr_add->Add(extra_attrs.m_attrs);
			m_huashen_speical_logic_attrs.Add(extra_attrs.m_attrs);

			wear_idx[jewelry_type + EQUIP_JEWELRY_TYPE_GEM_RING] = true;
			/*switch (p_eqed_jew.attr_pair.attr_type)
			{
			case Jewelry::S_ATTR_ALL_RESISTANCE:
			for (int type = BATTLE_ATTR_ANTI_VALUE_BEGIN; type < BATTLE_ATTR_ANTI_VALUE_END && type < BATTLE_ATTR_MAX; ++type)
			{
			m_attrs_add.m_attrs[type] += p_eqed_jew.attr_pair.attr_value;
			}
			break;

			default:
			if (p_eqed_jew.attr_pair.attr_type >= 0 && p_eqed_jew.attr_pair.attr_type < BATTLE_ATTR_MAX)
			{
			m_attrs_add.m_attrs[p_eqed_jew.attr_pair.attr_type] += p_eqed_jew.attr_pair.attr_value;
			}
			break;
			}

			if (need_calc_suit)		// 检测灵饰词牌是否成套
			{
			const SuitBasicCfg * p_sbc = LOGIC_CONFIG->GetJewelryConfig()->GetBasicCfgByLyricID(p_eqed_jew.lyrics_id);
			if (NULL == p_sbc || (0 != calc_sbc.lyric_type && p_sbc->lyric_type != calc_sbc.lyric_type))
			{
			need_calc_suit = false;
			continue;
			}

			// 比对，取较低阶的
			if (0 == calc_sbc.lyric_type || *p_sbc < calc_sbc) calc_sbc = *p_sbc;
			}*/
		}

		if (has_wear_all_equip && low_god_equip_level > 0)
		{
			for (int resonace_level_idx = 0; resonace_level_idx <= low_god_equip_level; resonace_level_idx++)
			{
				const GodEquipResonanceCfg * resonace_cfg = GODEQUIP_CONFIG->GetResonanceCfg(m_module_mgr->GetRole()->GetAppearanceProf(), resonace_level_idx);
				if (resonace_cfg != NULL)
				{
					if (resonace_cfg->addition_level > m_other_param.resonace_open_level)continue;//解锁共鸣等级不足跳过

					if (resonace_cfg->attr_type_1 >= BATTLE_ATTR_MIN && resonace_cfg->attr_type_1 <= BATTLE_ATTR_ELEMENT_END)
					{
						attr_add->m_attrs[resonace_cfg->attr_type_1] += resonace_cfg->attr_add_1;
					}

					if (resonace_cfg->attr_type_2 >= BATTLE_ATTR_MIN && resonace_cfg->attr_type_2 <= BATTLE_ATTR_ELEMENT_END)
					{
						attr_add->m_attrs[resonace_cfg->attr_type_2] += resonace_cfg->attr_add_2;
					}
				}
			}
		}

		for (int suit_idx = 0; suit_idx < suit_count && suit_idx < MAX_SUIT_TYPE; suit_idx++)//先检查是否有同类组合，件数堆叠
		{
			SuitEffectReCalc & suit_node = equip_suit_list[suit_idx];
			for (int suit_num = 0; suit_num < suit_node.num && suit_num < MAX_SUIT_TYPE; suit_num++)
			{
				if ((int)suit_node.color_top.size() <= suit_num)continue;
				const SuitEffect * suit_cfg = LOGIC_CONFIG->GetSuitListConfig()->GetSuitEffectList(suit_node.sp_id, suit_node.color_top[suit_num], suit_num + 1);
				if (suit_cfg == NULL)continue;

				//满足套装增加属性
				for (int effect_value_idx = 0; effect_value_idx < MAX_SUIT_EFFECT_VALUE_NUM; effect_value_idx++)
				{
					const SuitValue &suit_value = suit_cfg->m_value[effect_value_idx];
					if (suit_value.attr_type < 0 || suit_value.attr_type >= BATTLE_ATTR_MAX) continue;
					attr_add->m_attrs[suit_value.attr_type] += suit_value.attr_add;
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
								attr_add->m_attrs[detail_cfg->param_list[0]] += detail_cfg->param_list[1];
							}

							if (SPE_ATTR_ADD_ATTR_BY_LEVEL == detail_cfg->effect_sub_type && detail_cfg->param_num >= 2
								&& detail_cfg->param_list[0] >= BATTLE_ATTR_MIN && detail_cfg->param_list[0] < BATTLE_ATTR_MAX)
							{
								BattleAttr add_attr = static_cast<BattleAttr>(1.0f * m_module_mgr->GetRole()->GetLevel() * detail_cfg->param_list[1] / BATTLE_PERCENT_NUM + 0.5f);
								attr_add->m_attrs[detail_cfg->param_list[0]] += add_attr;
							}
						}
					}
				}
			}
		}

		for (int attr_inc_idx2 = 0; attr_inc_idx2 < BATTLE_ATTR_MAX; attr_inc_idx2++)
		{
			/* 不再需要除以10， 因为属性已改为按千分比计算 -- by chenyulin
			switch (attr_inc_idx2)//特殊，词条用的是千分比，所以最终值除以10
			{
			case BATTLE_ATTR_PHYSICAL_DEC_DAMAGE:
			case BATTLE_ATTR_MAGIC_DEC_DAMAGE:
			case BATTLE_ATTR_PHYSICAL_INC_DAMAGE:
			case BATTLE_ATTR_MAGIC_INC_DAMAGE:

			case BATTLE_ATTR_CRITICAL:
			case BATTLE_ATTR_HIT:
			case BATTLE_ATTR_DODGE:
			case BATTLE_ATTR_COUNTER_ATTACK:

			case BATTLE_ATTR_ANTI_POISON:
			case BATTLE_ATTR_ANTI_FREEZE:
			case BATTLE_ATTR_ANTI_PETRIFY:
			case BATTLE_ATTR_ANTI_CHAOS:
			case BATTLE_ATTR_ANTI_DRUNK:
			m_exa_attrs[attr_inc_idx2] = m_exa_attrs[attr_inc_idx2] / 10;
			break;
			}
			*/
			attr_add->m_attrs[attr_inc_idx2] += m_exa_attrs[attr_inc_idx2];
			m_huashen_speical_logic_attrs.m_attrs[attr_inc_idx2] += m_exa_attrs[attr_inc_idx2];
		}

		// 灵饰加成 - 词牌名套装
		/*if (need_calc_suit)
		{
			const SuitInfoTail * p_sit = LOGIC_CONFIG->GetJewelryConfig()->GetSuitInfo(calc_sbc.suit_level, calc_sbc.suit_color, calc_sbc.lyric_type);
			if (NULL != p_sit)
			{
				for (int suit_attr_num = 0; suit_attr_num < SUIT_ATTR_COUNT_MAX; ++suit_attr_num)
				{
					const BattleAttrPair & suit_attr_add = p_sit->attr_add[suit_attr_num];
					if (0 == suit_attr_add.attr_value) continue;

					switch (suit_attr_add.attr_type)
					{
					case Jewelry::S_ATTR_ALL_RESISTANCE:
						{
							m_attrs_add.m_attrs[BATTLE_ATTR_ANTI_POISON] += suit_attr_add.attr_value;
							m_attrs_add.m_attrs[BATTLE_ATTR_ANTI_FREEZE] += suit_attr_add.attr_value;
							m_attrs_add.m_attrs[BATTLE_ATTR_ANTI_PETRIFY] += suit_attr_add.attr_value;
							m_attrs_add.m_attrs[BATTLE_ATTR_ANTI_CHAOS] += suit_attr_add.attr_value;
							m_attrs_add.m_attrs[BATTLE_ATTR_ANTI_DRUNK] += suit_attr_add.attr_value;
						}
						break;

					default:
						if (suit_attr_add.attr_type >= 0 && suit_attr_add.attr_type < BATTLE_ATTR_MAX)
						{
							m_attrs_add.m_attrs[suit_attr_add.attr_type] += suit_attr_add.attr_value;
						}
						break;
					}
				}
			}
		}*/

		for (int upgreade_idx = 0; upgreade_idx < EQUIP_JEWELRY_TYPE_MAX; upgreade_idx++)
		{
			if (wear_idx[upgreade_idx])
			{
				{//强化加成
					int &hole_level = m_other_param.equipment_jewelry_upgreade[upgreade_idx];
					const PositionCfg * cfg = LOGIC_CONFIG->GetEquipmentJewerlyUpgreadeConfig()->GetPositionCfg(m_module_mgr->GetRole()->GetBaseProfession(), upgreade_idx, hole_level);
					if (NULL != cfg)
					{
						if (cfg->is_legal)
						{
							for (int attr_idx = 0; attr_idx < PositionCfg::ATTR_NUM; attr_idx++)
							{
								attr_add->m_attrs[cfg->attr_type[attr_idx]] += cfg->attr_add[attr_idx];

								// 天命 装备强化 基础属性加成
								int attr_total_per = m_module_mgr->GetTianMing()->GetAttrPer(TIAN_MING_ADD_ATTR_SYS_TYPE_EQUIP_STRENGTHEN);
								int tian_ming_add_val = int(cfg->attr_add[attr_idx] * 1.0f * attr_total_per / 10000);
								attr_add->m_attrs[cfg->attr_type[attr_idx]] += tian_ming_add_val;
							}
						}
					}
				}

				{//突破加成
					int &grade_level = m_other_param.equipment_jewelry_strengthen[upgreade_idx];
					const EquipJewelryStrengthenCfg * break_cfg = LOGIC_CONFIG->GetEquipmentJewerlyUpgreadeConfig()->GetStrengthenCfg(m_module_mgr->GetRole()->GetBaseProfession(), upgreade_idx, grade_level);
					if (NULL != break_cfg)
					{
						for (int attr_idx = 0; attr_idx < PositionCfg::ATTR_NUM; attr_idx++)
						{
							attr_add->m_attrs[break_cfg->attr_type[attr_idx]] += break_cfg->attr_add[attr_idx];

							// 天命 装备强化 基础属性加成
							int attr_total_per = m_module_mgr->GetTianMing()->GetAttrPer(TIAN_MING_ADD_ATTR_SYS_TYPE_EQUIP_STRENGTHEN);
							int tian_ming_add_val = int(break_cfg->attr_add[attr_idx] * 1.0f * attr_total_per / 10000);
							attr_add->m_attrs[break_cfg->attr_type[attr_idx]] += tian_ming_add_val;
						}
					}
				}
			}
			else
			{
				all_wear = false;
			}
		}

		if (all_wear)//套装
		{
			int low_level = -1;
			for (int upgreade_idx = 0; upgreade_idx < EQUIP_JEWELRY_TYPE_MAX; upgreade_idx++)
			{
				if (low_level < 0 || low_level > m_other_param.equipment_jewelry_upgreade[upgreade_idx])
				{
					low_level = m_other_param.equipment_jewelry_upgreade[upgreade_idx];;
				}
			}
			const std::vector<PositionSuitCfg> * cfg = LOGIC_CONFIG->GetEquipmentJewerlyUpgreadeConfig()->GetSuitListCfg(m_module_mgr->GetRole()->GetBaseProfession());
			if (NULL != cfg)
			{
				for (std::vector<PositionSuitCfg>::const_iterator it = cfg->begin(); it != cfg->end(); it++)
				{
					if (it->hole_level_all <= low_level)
					{
						for (int attr_idx = 0; attr_idx < PositionSuitCfg::MAX_ATTR_NUM; attr_idx++)
						{
							attr_add->m_attrs[it->attr_type[attr_idx]] += it->attr_add[attr_idx];
						}

						if (it->sp_id > 0)
						{
							const SpecialEffectCfg* spe_cfg = BattleSpecialEffectPool::GetInstance().GetSpecialEffectCfg(it->sp_id);
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
										attr_add->m_attrs[detail_cfg->param_list[0]] += detail_cfg->param_list[1];
									}

									if (SPE_ATTR_ADD_ATTR_BY_LEVEL == detail_cfg->effect_sub_type && detail_cfg->param_num >= 2
										&& detail_cfg->param_list[0] >= BATTLE_ATTR_MIN && detail_cfg->param_list[0] < BATTLE_ATTR_MAX)
									{
										BattleAttr add_attr = static_cast<BattleAttr>(1.0f * m_module_mgr->GetRole()->GetLevel() * detail_cfg->param_list[1] / BATTLE_PERCENT_NUM + 0.5f);
										attr_add->m_attrs[detail_cfg->param_list[0]] += add_attr;
									}
								}
							}
						}
					}
				}
			}
		}

		if (element_crystal_used_times > 0)
		{
			const ItemBase* element_item = ITEMPOOL->GetItem(*element_crystal_id);
			if (NULL != element_item)
			{
				const ElementCrystal* element_crystal = (const ElementCrystal*)element_item;
				attr_add->m_attrs[BATTLE_ATTR_ELEMENT_FIRE] = element_crystal->GetElementFire();
				attr_add->m_attrs[BATTLE_ATTR_ELEMENT_WATER] = element_crystal->GetElementWater();
				attr_add->m_attrs[BATTLE_ATTR_ELEMENT_WIND] = element_crystal->GetElementWind();
				attr_add->m_attrs[BATTLE_ATTR_ELEMENT_EARTH] = element_crystal->GetElementEarth();

			}
			else
			{
				attr_add->m_attrs[BATTLE_ATTR_ELEMENT_FIRE] = 0;
				attr_add->m_attrs[BATTLE_ATTR_ELEMENT_WATER] = 0;
				attr_add->m_attrs[BATTLE_ATTR_ELEMENT_WIND] = 0;
				attr_add->m_attrs[BATTLE_ATTR_ELEMENT_EARTH] = 0;
			}
		}
		else
		{
			attr_add->m_attrs[BATTLE_ATTR_ELEMENT_FIRE] = 0;
			attr_add->m_attrs[BATTLE_ATTR_ELEMENT_WATER] = 0;
			attr_add->m_attrs[BATTLE_ATTR_ELEMENT_WIND] = 0;
			attr_add->m_attrs[BATTLE_ATTR_ELEMENT_EARTH] = 0;
		}
		//化身限制装备属性上限(熔炼不算)
		if (ROLE_TYPE_HUASHEN == role_type)
		{
			const ActivityHuanJieZhanChangEquipAttrLimitCfg * limit_cfg = LOGIC_CONFIG->GetActivityHuanJieZhanChangConfig()->GetEquipAttrLimitCfg(HUAN_JIE_HUA_SHEN_ATTR_LIMIT_TYPE_EQUIPMENT, m_module_mgr->GetRole()->GetAppearanceProf(), 0);
			if (NULL != limit_cfg)
			{
				AttributeList snap_attrs = *attr_add;
				snap_attrs.Deduct(m_huashen_speical_logic_attrs.m_attrs);

				for (int i = 0; i < HJZC_EQUIP_ATTR_LIMIT_MAX_NUM; i++)
				{
					const ActivityHuanJieZhanChangEquipAttrLimitCfg::AttrConCfg & con_cfg = limit_cfg->attr_limit_list[i];
					if (snap_attrs.m_attrs[con_cfg.attr_type] > con_cfg.attr_value)
					{
						snap_attrs.m_attrs[con_cfg.attr_type] = con_cfg.attr_value;
					}
				}
				snap_attrs.Add(m_huashen_speical_logic_attrs.m_attrs);
				attr_add->Set(snap_attrs.m_attrs);
			}
		}
	}
	if (ROLE_TYPE_BENZUN == role_type)
	{
		int old_equip_capability = m_other_param.equip_jewelry_cap;
		m_other_param.equip_jewelry_cap = m_module_mgr->GetCapability()->GetCalcCapByAttrList(*attr_add);
		this->SendDianHuaShiCap();
		EventHandler::Instance().OnRoleEquipCapabilityChange(m_module_mgr->GetRole(), old_equip_capability, m_other_param.equip_jewelry_cap);
	}
	else if (ROLE_TYPE_HUASHEN == role_type && NULL != equip_special_attr)
	{
		equip_special_attr->Set(m_huashen_speical_logic_attrs.m_attrs);	//获取熔炼属性到外部限制
	}

	base_add.Add(attr_add->m_attrs);
	return *attr_add;
}

void EquipmentManager::ReCalcAttrPer(AttributeList & base_add, const AttributeList & cpy_attr_list)
{
	/*m_attrs_add_per.Reset();
	此处如果取消注释,本尊+化身属性来源需要添加
	// 灵饰
	for (int j_type = 0; j_type < Jewelry::J_TYPE_MAX; ++j_type)
	{
		if (m_jewelry_list[j_type].Invalid()) continue;

		JewelryParam & p_eqed_jew = m_jewelry_list[j_type].param->jewelry_param;
		if (p_eqed_jew.anima_val <= 0 || LGP_TYPE_INVALID == p_eqed_jew.lege_attr.attr_type) continue;

		switch (p_eqed_jew.lege_attr.attr_type)
		{
		case LGP_TYPE_BASIC_ATK:
			base_add.m_attrs[BATTLE_ATTR_ATTACK] += (cpy_attr_list.m_attrs[BATTLE_ATTR_ATTACK] * p_eqed_jew.lege_attr.attr_value / 100);
			break;
		case LGP_TYPE_BASIC_SPI:
			base_add.m_attrs[BATTLE_ATTR_MENTAL] += (cpy_attr_list.m_attrs[BATTLE_ATTR_MENTAL] * p_eqed_jew.lege_attr.attr_value / 100);
			break;
		case LGP_TYPE_BASIC_REC:
			base_add.m_attrs[BATTLE_ATTR_RECOVERY] += (cpy_attr_list.m_attrs[BATTLE_ATTR_RECOVERY] * p_eqed_jew.lege_attr.attr_value / 100);
			break;
		case LGP_TYPE_BASIC_MHP_AND_MMP:
			base_add.m_attrs[BATTLE_ATTR_MAX_HP] += (cpy_attr_list.m_attrs[BATTLE_ATTR_MAX_HP] * p_eqed_jew.lege_attr.attr_value / 100);
			base_add.m_attrs[BATTLE_ATTR_MAX_MP] += (cpy_attr_list.m_attrs[BATTLE_ATTR_MAX_MP] * p_eqed_jew.lege_attr.attr_value / 100);
			break;
		default:
			break;
		}
	}

	base_add.Add(m_attrs_add_per.m_attrs);*/
}

int EquipmentManager::GetEquipDianHuaShiCap()
{
	if (dianhuashi_id > 0 && dianhuashi_num >= Equipment::E_INDEX_MAX && all_dianhuashi_level > 0)
	{
		const DianHuaSkillLevelCfg * level_cfg = LOGIC_CONFIG->GetDianHuaShiConfig()->GetSkillLevelCfg(all_dianhuashi_level);
		if (NULL != level_cfg)
		{
			const DianHuaSpecialEffectDetailCfg * dianhuashi_cfg = LOGIC_CONFIG->GetDianHuaShiConfig()->GetEffectDetailCfg(dianhuashi_id, level_cfg->skill_level);
			if (NULL != dianhuashi_cfg)
			{
				const GamePassiveSkillCfg * passive_skill = SkillPool::Instance().GetPassiveSkillCfg(dianhuashi_cfg->set_skills);
				if (NULL != passive_skill)
				{
					// 专属技能是有技能等级的，注意！
					int skill_level = level_cfg->skill_level;

					const SkillScoreCfg * skill_score_cfg = LOGIC_CONFIG->GetScoreSystemConfig()->GetSkillScpreCfg(dianhuashi_cfg->set_skills, skill_level);
					if (skill_score_cfg != NULL)
					{
						return skill_score_cfg->skill_score;
					}
				}
			}
		}
	}
	return 0;
}

int EquipmentManager::GetEquipGemNumByGrade(int grade)
{
	int equip_gem_num = 0;
	for (int i = Equipment::E_INDEX_MIN; i < Equipment::E_INDEX_MAX; ++i)
	{
		if (m_equipment_list[i].Invalid() || NULL == m_equipment_list[i].param) continue;

		const Equipment* equip = (const Equipment*)(ITEMPOOL->GetItem(m_equipment_list[i].item_id));
		if (NULL == equip || I_TYPE_EQUIPMENT != equip->GetItemType()) continue;

		EquipmentParam & p_eqed_equ = m_equipment_list[i].param->equip_param;
		for (int gem_grid = 0; gem_grid < ARRAY_LENGTH(p_eqed_equ.new_stone); ++gem_grid)
		{
			if (0 == p_eqed_equ.new_stone[gem_grid]) continue;

			const GemstoneBasicInfoCfg * gbic = LOGIC_CONFIG->GetGemstoneConfig()->GetGemstoneBasicInfoCfg(p_eqed_equ.new_stone[gem_grid]);
			if (NULL == gbic) continue;

			if (gbic->level >= grade)
			{
				equip_gem_num++;
			}
		}
	}
	for (int j_type = 0; j_type < Jewelry::J_TYPE_MAX; ++j_type)
	{
		if (m_jewelry_list[j_type].Invalid() || NULL == m_jewelry_list[j_type].param) continue;

		const Jewelry* jewelry = (const Jewelry*)(ITEMPOOL->GetItem(m_jewelry_list[j_type].item_id));
		if (NULL == jewelry || I_TYPE_JEWELRY != jewelry->GetItemType()) continue;

		JewelryParam & p_eqed_jew = m_jewelry_list[j_type].param->jewelry_param;
		for (int gem_grid = 0; gem_grid < ARRAY_LENGTH(p_eqed_jew.new_stone); ++gem_grid)
		{
			if (0 == p_eqed_jew.new_stone[gem_grid]) continue;

			const GemstoneBasicInfoCfg * gbic = LOGIC_CONFIG->GetGemstoneConfig()->GetGemstoneBasicInfoCfg(p_eqed_jew.new_stone[gem_grid]);
			if (NULL == gbic) continue;

			if (gbic->level >= grade)
			{
				equip_gem_num++;
			}
		}
	}
	return equip_gem_num;
}

int EquipmentManager::GetWearEquipNumByColor(int color)
{
	int wear_equip_num = 0;
	for (int i = Equipment::E_INDEX_MIN; i < Equipment::E_INDEX_MAX; ++i)
	{
		if (m_equipment_list[i].Invalid() || NULL == m_equipment_list[i].param) continue;

		const Equipment* equip = (const Equipment*)(ITEMPOOL->GetItem(m_equipment_list[i].item_id));
		if (NULL == equip || I_TYPE_EQUIPMENT != equip->GetItemType()) continue;

		if (0 == color || equip->GetColor())
		{
			wear_equip_num++;
		}
	}

	for (int j_type = 0; j_type < Jewelry::J_TYPE_MAX; ++j_type)
	{
		if (m_jewelry_list[j_type].Invalid() || NULL == m_jewelry_list[j_type].param) continue;

		const Jewelry* jewelry = (const Jewelry*)(ITEMPOOL->GetItem(m_jewelry_list[j_type].item_id));
		if (NULL == jewelry || I_TYPE_JEWELRY != jewelry->GetItemType()) continue;

		if (0 == color || jewelry->GetColor())
		{
			wear_equip_num++;
		}
	}

	return wear_equip_num;
}

void EquipmentManager::GetCurWeapon(ItemID* out_weapon_id, short* out_weapon_type, ItemID* out_sub_weapon_id /* = NULL */, short role_type /* = ROLE_TYPE_BENZUN */)
{
	ItemGridData * equipment_list = NULL;
	if (role_type == ROLE_TYPE_BENZUN)
	{
		equipment_list = m_equipment_list;
	}
	else
	{
		equipment_list = m_huashen_equipment_list;
	}
	if (equipment_list[Equipment::E_INDEX_WEAPON].Invalid())
	{
		if (NULL != out_weapon_id) *out_weapon_id = 0;
		if (NULL != out_weapon_type) *out_weapon_type = WEAPON_TYPE_NONE;
	}
	else
	{
		const ItemBase* item = ITEMPOOL->GetItem(equipment_list[Equipment::E_INDEX_WEAPON].item_id);
		if (NULL == item || I_TYPE_EQUIPMENT != item->GetItemType() ||
			(-1 != item->GetLimitProf() && item->GetLimitProf() != m_module_mgr->GetRole()->GetBaseProfession()))
		{
			if (NULL != out_weapon_id) *out_weapon_id = 0;
			if (NULL != out_weapon_type) *out_weapon_type = WEAPON_TYPE_NONE;
		}
		else
		{
			const Equipment* weapon = (const Equipment*)item;
			if (NULL != out_weapon_id) *out_weapon_id = weapon->GetItemId();
			if (NULL != out_weapon_type) *out_weapon_type = weapon->GetWeaponType();
		}
	}

	if (equipment_list[Equipment::E_INDEX_SHIELD].Invalid())
	{
		if (NULL != out_sub_weapon_id) *out_sub_weapon_id = 0;
	}
	else
	{
		const ItemBase* item = ITEMPOOL->GetItem(equipment_list[Equipment::E_INDEX_SHIELD].item_id);
		if (NULL == item || I_TYPE_EQUIPMENT != item->GetItemType() ||
			(-1 != item->GetLimitProf() && item->GetLimitProf() != m_module_mgr->GetRole()->GetBaseProfession()))
		{
			if (NULL != out_sub_weapon_id) *out_sub_weapon_id = 0;
		}
		else
		{
			if (NULL != out_sub_weapon_id) *out_sub_weapon_id = item->GetItemId();
		}
	}
}

ItemID EquipmentManager::GetCurWeaponId()
{
	if (m_equipment_list[Equipment::E_INDEX_WEAPON].Invalid())
	{
		return 0;
	}
	else
	{
		return m_equipment_list[Equipment::E_INDEX_WEAPON].item_id;
	}
}

void EquipmentManager::GetWeaponAttr(short max_attr_num, short* out_attr_num, AttrInfo* out_attr_list)
{
	if (NULL == out_attr_num || NULL == out_attr_list) return;

	*out_attr_num = 0;

	if (m_equipment_list[Equipment::E_INDEX_WEAPON].Invalid()) return;

	NetValueItemParam* param = m_equipment_list[Equipment::E_INDEX_WEAPON].param;
	if (NULL == param) return;

	for (int i = 0; i < max_attr_num && i < param->equip_param.attr_num; ++i)
	{
		out_attr_list[(*out_attr_num)++] = param->equip_param.attr_list[i];
	}
}

void EquipmentManager::GetEquipSpecialEffect(int max_weapon_sp_num, short* out_weapon_sp_num, int* out_weapon_sp_list,
	int max_other_sp_num, short* out_other_sp_num, int* out_other_sp_list)
{
	if (NULL == out_weapon_sp_num || NULL == out_weapon_sp_list || NULL == out_other_sp_num || NULL == out_other_sp_list) return;
	int suit_count = 0;
	SuitEffectReCalc equip_suit_list[MAX_SUIT_TYPE];
	for (int i = Equipment::E_INDEX_MIN; i < Equipment::E_INDEX_MAX; ++i)
	{
		if (m_equipment_list[i].Invalid()) continue;
		const Equipment* equip = (const Equipment*)(ITEMPOOL->GetItem(m_equipment_list[i].item_id));
		if (equip == NULL)continue;
		if (NULL == m_equipment_list[i].param) continue;

		//套装效果
		if (m_equipment_list[i].param->equip_param.new_suit_effect_id > 0)
		{
			bool has_suit_add = false;
			for (int suit_idx = 0; suit_idx < suit_count && suit_idx < MAX_SUIT_TYPE; suit_idx++)//先检查是否有同类组合，件数堆叠
			{
				SuitEffectReCalc & node_cale = equip_suit_list[suit_idx];
				if (node_cale.sp_id == m_equipment_list[i].param->equip_param.new_suit_effect_id)
				{
					//检查多件套中最高品质
					bool has_insert = false;
					for (int suit_num_begin = 0; suit_num_begin < node_cale.num; suit_num_begin++)
					{
						if ((int)node_cale.color_top.size() <= suit_num_begin)continue;
						if (node_cale.color_top[suit_num_begin] < equip->GetColor())
						{
							std::vector<int>::iterator it = node_cale.color_top.begin();
							node_cale.color_top.insert(it + suit_num_begin, equip->GetColor());
							has_insert = true;
							break;
						}
					}
					if (!has_insert)
					{
						node_cale.color_top.push_back(equip->GetColor());
					}

					equip_suit_list[suit_idx].num += 1;
					has_suit_add = true;
					break;
				}
			}
			if (!has_suit_add)
			{
				equip_suit_list[suit_count].sp_id = m_equipment_list[i].param->equip_param.new_suit_effect_id;
				equip_suit_list[suit_count].color_top.push_back(equip->GetColor());
				equip_suit_list[suit_count++].num = 1;
			}
		}

		if (m_equipment_list[i].param->equip_param.sp_id <= 0) continue;

		if (i == Equipment::E_INDEX_WEAPON)
		{
			if (*out_weapon_sp_num < max_weapon_sp_num)
			{
				out_weapon_sp_list[(*out_weapon_sp_num)++] = m_equipment_list[i].param->equip_param.sp_id;
			}
		}
		else
		{
			if (*out_other_sp_num < max_other_sp_num)
			{
				out_other_sp_list[(*out_other_sp_num)++] = m_equipment_list[i].param->equip_param.sp_id;
			}
		}
	}

	//套装效果赋予
	for (int suit_idx = 0; suit_idx < suit_count && suit_idx < MAX_SUIT_TYPE; suit_idx++)//先检查是否有同类组合，件数堆叠
	{
		SuitEffectReCalc & suit_node = equip_suit_list[suit_idx];
		for (int suit_num = 0; suit_num < suit_node.num && suit_num < MAX_SUIT_TYPE; suit_num++)
		{
			if ((int)suit_node.color_top.size() <= suit_num)continue;
			const SuitEffect * suit_cfg = LOGIC_CONFIG->GetSuitListConfig()->GetSuitEffectList(suit_node.sp_id, suit_node.color_top[suit_num], suit_num + 1);
			if (suit_cfg == NULL)continue;

			if (suit_cfg->sp_id > 0)
			{
				if (*out_other_sp_num < max_other_sp_num)
				{
					out_other_sp_list[(*out_other_sp_num)++] = suit_cfg->sp_id;
				}
			}
		}
	}
	suit_count = 0;
	for (int i = 0; i < ARRAY_ITEM_COUNT(equip_suit_list); ++i)
	{
		equip_suit_list->Reset();
	}
	for (int i = Jewelry::J_TYPE_GEM_RING; i < Jewelry::J_TYPE_MAX; ++i)
	{
		if (m_jewelry_list[i].Invalid()) continue;
		const Jewelry* jewelry = (const Jewelry*)(ITEMPOOL->GetItem(m_jewelry_list[i].item_id));
		if (jewelry == NULL)continue;
		if (NULL == m_jewelry_list[i].param) continue;

		//套装效果
		if (m_jewelry_list[i].param->jewelry_param.new_suit_effect_id > 0)
		{
			bool has_suit_add = false;
			for (int suit_idx = 0; suit_idx < suit_count && suit_idx < MAX_SUIT_TYPE; suit_idx++)//先检查是否有同类组合，件数堆叠
			{
				SuitEffectReCalc & node_cale = equip_suit_list[suit_idx];
				if (node_cale.sp_id == m_jewelry_list[i].param->jewelry_param.new_suit_effect_id)
				{
					//检查多件套中最高品质
					bool has_insert = false;
					for (int suit_num_begin = 0; suit_num_begin < node_cale.num; suit_num_begin++)
					{
						if ((int)node_cale.color_top.size() <= suit_num_begin)continue;
						if (node_cale.color_top[suit_num_begin] < jewelry->GetColor())
						{
							std::vector<int>::iterator it = node_cale.color_top.begin();
							node_cale.color_top.insert(it + suit_num_begin, jewelry->GetColor());
							has_insert = true;
							break;
						}
					}
					if (!has_insert)
					{
						node_cale.color_top.push_back(jewelry->GetColor());
					}

					equip_suit_list[suit_idx].num += 1;
					has_suit_add = true;
					break;
				}
			}
			if (!has_suit_add)
			{
				equip_suit_list[suit_count].sp_id = m_jewelry_list[i].param->jewelry_param.new_suit_effect_id;
				equip_suit_list[suit_count].color_top.push_back(jewelry->GetColor());
				equip_suit_list[suit_count++].num = 1;
			}
		}
	}

	//套装效果赋予
	for (int suit_idx = 0; suit_idx < suit_count && suit_idx < MAX_SUIT_TYPE; suit_idx++)//先检查是否有同类组合，件数堆叠
	{
		SuitEffectReCalc & suit_node = equip_suit_list[suit_idx];
		for (int suit_num = 0; suit_num < suit_node.num && suit_num < MAX_SUIT_TYPE; suit_num++)
		{
			if ((int)suit_node.color_top.size() <= suit_num)continue;
			const SuitEffect * suit_cfg = LOGIC_CONFIG->GetSuitListConfig()->GetSuitEffectList(suit_node.sp_id, suit_node.color_top[suit_num], suit_num + 1);
			if (suit_cfg == NULL)continue;

			if (suit_cfg->sp_id > 0)
			{
				if (*out_other_sp_num < max_other_sp_num)
				{
					out_other_sp_list[(*out_other_sp_num)++] = suit_cfg->sp_id;
				}
			}
		}
	}

	//点化石套装
	if (dianhuashi_id > 0 && dianhuashi_num >= Equipment::E_INDEX_MAX && all_dianhuashi_level > 0)
	{
		const DianHuaSkillLevelCfg * level_cfg = LOGIC_CONFIG->GetDianHuaShiConfig()->GetSkillLevelCfg(all_dianhuashi_level);
		if (NULL != level_cfg)
		{
			const DianHuaSpecialEffectDetailCfg * dianhuashi_cfg = LOGIC_CONFIG->GetDianHuaShiConfig()->GetEffectDetailCfg(dianhuashi_id, level_cfg->skill_level);
			if (NULL != dianhuashi_cfg)
			{
				const GamePassiveSkillCfg * passive_skill = SkillPool::Instance().GetPassiveSkillCfg(dianhuashi_cfg->set_skills);
				if (NULL != passive_skill)
				{
					// 专属技能是有技能等级的，注意！
					int skill_level = level_cfg->skill_level;
					int spid = passive_skill->sp_id;
					if (passive_skill->is_level_spid && skill_level >= 1 && skill_level < ARRAY_LENGTH(passive_skill->level_spid_list))
					{
						spid = passive_skill->level_spid_list[skill_level];
					}
					const SpecialEffectCfg* spe_cfg = BattleSpecialEffectPool::GetInstance().GetSpecialEffectCfg(spid);
					if (NULL != spe_cfg)
					{
						bool has_battle_effect = false;
						for (int i = 0; i < spe_cfg->effect_num; ++i)
						{
							const SpecialEffectKey& key = spe_cfg->effect_detail_list[i];
							if (key.effect_type == SPECIAL_EFFECT_TYPE_ADD_ATTR) continue;

							const EffectDetailCfg* detail_cfg = BattleSpecialEffectPool::GetInstance().GetEffectDetailCfg(key);
							if (NULL == detail_cfg) continue;
							has_battle_effect = true;
						}
						if (has_battle_effect)
						{
							if (*out_other_sp_num < max_other_sp_num)
							{
								out_other_sp_list[(*out_other_sp_num)++] = spid;
							}
						}
					}
				}
			}
		}
	}
}

int EquipmentManager::PutOn(short column, short index, short equip_index, short role_type)
{
	if (role_type < 0 || role_type >= ROLE_TYPE_MAX) return -1;
	static ItemGridData grid_data; grid_data.Reset();
	ItemGridData *equipment_list = NULL;
	int t_role_lv = m_module_mgr->GetRole()->GetLevel();
	if (role_type == ROLE_TYPE_BENZUN)
	{
		if (!m_module_mgr->GetKnapsack()->GetItemGridData(column, index, &grid_data))
		{
			return -2;
		}
		equipment_list = m_equipment_list;
		//t_role_lv = m_module_mgr->GetRole()->GetLevel();
	}
	else
	{
		if (!m_module_mgr->GetHuaShenManager()->GetItemGridData(index, &grid_data))
		{
			return -3;
		}
		equipment_list = m_huashen_equipment_list;
		//t_role_lv = m_module_mgr->GetHuaShenManager()->GetHuaShenLevel();
	}

	const Equipment * equip = (const Equipment *)ITEMPOOL->GetItem(grid_data.item_id);
	if (NULL == equip || I_TYPE_EQUIPMENT != equip->GetItemType()) return -4;

	if (NULL == grid_data.param) return -5;

	short equip_type = equip->GetEquipType();
	if (!Equipment::IsValidType(equip_type)) return -6;

	if (equip_index < 0 || equip_index >= Equipment::E_INDEX_MAX) return -7;

	if (!Equipment::IsMatchIndex(equip_type, equip_index)) return -8;

	//加上职业限制
	if (-1 != equip->GetLimitProf() && m_module_mgr->GetRole()->GetBaseProfession() != equip->GetLimitProf())
	{
		return -9;
	}
	if (!equip->IsUnLimitLevel() && t_role_lv < equip->GetLimitLevel())
	{
		m_module_mgr->NoticeNum(errornum::EN_EQUIPMENT_PUT_LEVEL_LIMIT);
		return -10;
	}

	int take_off_column = -1;
	int take_off_index = -1;
	bool need_send_zhu_ling_notice = false;
	if (equipment_list[equip_index].param && grid_data.param)
	{
		need_send_zhu_ling_notice = equipment_list[equip_index].param->equip_param.HasZhuLing() && grid_data.param->equip_param.HasZhuLing();//没什么用，客户端需要
	}

	bool has_notic = false;
	//检查所穿戴的装备是否有宝石
	if (!equipment_list[equip_index].Invalid()) // 该位置有装备 脱之
	{
		int ret = this->TakeOff(equip_index, true, role_type, &take_off_column, &take_off_index);
		if (0 != ret)
		{
			return -11;
		}
	}

	// 当装备双手武器且穿戴了盾牌时，脱下盾牌
	if (equip->IsWeapon() && !equip->IsSingleHandWeapon() && !equipment_list[Equipment::E_INDEX_SHIELD].Invalid())
	{
		int ret = this->TakeOff(Equipment::E_INDEX_SHIELD, true, role_type);
		if (0 != ret)
		{
			return -12;
		}
	}
	else if (equip->IsShield()) // 当装备盾牌，且穿戴双手武器时，脱下双手武器
	{
		if (!equipment_list[Equipment::E_INDEX_WEAPON].Invalid())
		{
			const Equipment * weapon = (const Equipment *)ITEMPOOL->GetItem(equipment_list[Equipment::E_INDEX_WEAPON].item_id);
			if (NULL != weapon && I_TYPE_EQUIPMENT == weapon->GetItemType() && !weapon->IsSingleHandWeapon())
			{
				int ret = this->TakeOff(Equipment::E_INDEX_WEAPON, true, role_type);
				if (0 != ret)
				{
					return -13;
				}
			}
		}
	}

	NetValueItemParam * param = equip->CreateParamFromStruct(*grid_data.param);
	if (NULL == param) return -14;

	ItemGridData & e_idx = equipment_list[equip_index];
	e_idx.item_id = grid_data.item_id;
	e_idx.param = param;
	e_idx.num = 1;
	e_idx.invalid_time = grid_data.invalid_time;
	e_idx.is_bind = true;
	e_idx.is_market_buy_flag = grid_data.is_market_buy_flag;

	const ItemGridData * temp = NULL;
	if (ROLE_TYPE_BENZUN == role_type)
	{
		temp = m_module_mgr->GetKnapsack()->GetItemGridData2(take_off_column, take_off_index);
	}
	else
	{
		temp = m_module_mgr->GetHuaShenManager()->GetItemGridData2(take_off_index);
	}
	Protocol::SCZhuLingPutOnSuccNotice zhu_ling_notice;
	zhu_ling_notice.p1 = 0;
	if (NULL != temp)
	{
		const Equipment * temp_info = (const Equipment *)ITEMPOOL->GetItem(temp->item_id);
		if (temp_info != NULL && I_TYPE_EQUIPMENT == temp_info->GetItemType())
		{
			//获得脱下来那个装备，如果正要穿上的装备没有铸灵，就要将 脱下的装备上的铸灵 放到 身上的装备去
			if (!e_idx.param->equip_param.HasZhuLing() && temp->param->equip_param.HasZhuLing())
			{
				need_send_zhu_ling_notice = true;
				zhu_ling_notice.p1 = 1;
				for (int i = 0; i < EQUIP_ZHU_LING_SLOT_NUM; ++i)
				{
					e_idx.param->equip_param.zhu_ling_info[i] = temp->param->equip_param.zhu_ling_info[i];
					temp->param->equip_param.zhu_ling_info[i].Reset();
				}
			}
			
			int t_cur = 0;
			for (; t_cur < NEW_EQUIP_STONE_MOUNT_MAX; ++t_cur)//检测开启的孔数		//此时要以穿戴的这件装备能接受的孔位,防止一些BUG
			{
				bool is_godequip = equip->GetColor() >= ItemBase::I_COLOR_RED ? true : false;
				int equip_level = is_godequip ? e_idx.param->equip_param.strengthen_level : equip->GetLimitLevel();
				if (!LOGIC_CONFIG->GetGemstoneConfig()->CheckGemMountSlotOpen(t_cur, equip_level, is_godequip))break;
			}

			//检查准备要穿戴的装备宝石数量，有空位则转移一颗宝石
			int vacancy_idx = e_idx.param->equip_param.HasVacancyStone();//有空位的宝石位置
			const ItemGridData & stone_data = *temp;
			int round = 0;//避免死循环
			while (vacancy_idx < t_cur && stone_data.param->equip_param.HasStone() && round < 50)
			{
				round += 1;
				ItemID top_stone_id = 0;
				int top_level = 0;
				int top_stone_idx = t_cur;
				for (int stone_idx = 0; stone_idx < t_cur && stone_idx < ARRAY_LENGTH(stone_data.param->equip_param.new_stone); stone_idx++)
				{
					if (stone_data.param->equip_param.new_stone[stone_idx] <= 0) continue;
					const GemstoneBasicInfoCfg * gbic = LOGIC_CONFIG->GetGemstoneConfig()->GetGemstoneBasicInfoCfg(stone_data.param->equip_param.new_stone[stone_idx]);
					if (gbic != NULL)
					{
						if (gbic->level > top_level)
						{
							top_stone_id = stone_data.param->equip_param.new_stone[stone_idx];
							top_level = gbic->level;
							top_stone_idx = stone_idx;
						}
					}
				}
				if (top_stone_idx >= 0 && top_stone_idx < t_cur && top_stone_idx < ARRAY_LENGTH(e_idx.param->equip_param.new_stone))
				{
					//替换宝石
					e_idx.param->equip_param.new_stone[vacancy_idx] = top_stone_id;
					//消除宝石
					stone_data.param->equip_param.new_stone[top_stone_idx] = 0;
					if (role_type == ROLE_TYPE_BENZUN && !has_notic)m_module_mgr->NoticeNum(noticenum::NT_EQUIP_STONE_TRANF_SUCC);
					has_notic = true;
				}
				vacancy_idx = e_idx.param->equip_param.HasVacancyStone();
			}

			if (ROLE_TYPE_BENZUN == role_type)
			{
				m_module_mgr->GetKnapsack()->SetItemDirty(take_off_column, take_off_index, true, Knapsack::ITEM_CHANGE_NOTICE_REASON_DATA_CHANGE);
			}
			else
			{
				m_module_mgr->GetHuaShenManager()->SetItemDirty(take_off_index, true, Knapsack::ITEM_CHANGE_NOTICE_REASON_DATA_CHANGE);
			}
		}
	}

	if (need_send_zhu_ling_notice && role_type == ROLE_TYPE_BENZUN)
	{
		m_module_mgr->NetSend(&zhu_ling_notice, sizeof(zhu_ling_notice));
	}
	/*const EquipmentScoreCfg * score_cfg = LOGIC_CONFIG->GetScoreSystemConfig()->GetEquipmentScoreCfg(e_idx.item_id);
	if (score_cfg)
	{
		EquipmentParam & data = param->equip_param;
		if (data.is_treasure == ITEM_SCORE_LEVEL_S)
		{
			e_idx.is_bind = false;
		}
	}*/

	e_idx.item_unique_id = grid_data.item_unique_id;

	if (role_type == ROLE_TYPE_BENZUN)
	{
		if (!m_module_mgr->GetKnapsack()->ConsumeItemByIndex(column, index, 1, __FUNCTION__))
		{
			return -15;
		}
		m_other_param.m_equipment_lock_list[equip_index].Reset();
		this->SendEquipMentLockInfo();
		m_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_EQUIPMENT, __FUNCTION__);
	}
	else
	{
		if (!m_module_mgr->GetHuaShenManager()->Consume(index, true))
		{
			return -16;
		}
		m_module_mgr->GetHuaShenManager()->PutOn(equip_index, e_idx.item_unique_id, HUASHEN_GRID_INDEX_TYPE_EQUIPMENT);
		m_module_mgr->GetHuaShenManager()->ReCalcAttr(AttributeList::RECALC_REASON_EQUIPMENT, __FUNCTION__);
	}
	////此时宝石经过了转移,需要同步数据
	//if (has_notic || grid_data.is_bind != e_idx.is_bind)
	//{
	//	this->SynItemDataPut(I_TYPE_EQUIPMENT, equip_index, &e_idx, role_type);
	//}

	this->SetEquipGridDirty(equip_index, true, role_type);
	EventHandler::Instance().OnRolePutOnEquip(m_module_mgr->GetRole(), equip_index);
	return 0;
}

int EquipmentManager::TakeOff(short equip_index, bool is_from_puton, short role_type, int * take_off_column, int * take_off_index)
{
	if (equip_index < 0 || equip_index >= Equipment::E_INDEX_MAX || role_type < 0 || role_type >= ROLE_TYPE_MAX) return -1;

	ItemGridData * equipment_list = NULL;
	if (role_type == ROLE_TYPE_BENZUN)
	{
		equipment_list = m_equipment_list;
	}
	else
	{
		equipment_list = m_huashen_equipment_list;
	}

	if (equipment_list[equip_index].Invalid()) return -2;

	EquipmentParam *equip_param = (EquipmentParam *)equipment_list[equip_index].param;
	if (NULL == equip_param) return -3;

	const Equipment *equip = (const Equipment*)ITEMPOOL->GetItem(equipment_list[equip_index].item_id);
	if (NULL == equip || equip->GetItemType() != I_TYPE_EQUIPMENT) return -4;

	if (role_type == ROLE_TYPE_BENZUN)
	{
		if (!m_module_mgr->GetKnapsack()->CheckForPut(equip->GetItemId(), 1))
		{
			m_module_mgr->GetRole()->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
			return -5;
		}

		ItemDataWrapper item_wrapper(equipment_list[equip_index].item_id, 1, (short)equipment_list[equip_index].is_bind, 1,
			equipment_list[equip_index].param->data, equipment_list[equip_index].invalid_time, 0, equipment_list[equip_index].item_unique_id, equipment_list[equip_index].is_market_buy_flag);

		if (!m_module_mgr->GetKnapsack()->Put(item_wrapper, PUT_REASON_NO_NOTICE, take_off_column, take_off_index))
		{
			return -6;
		}
	}
	else
	{
		ItemDataWrapper item_data;
		equipment_list[equip_index].WrapTo(&item_data);
		if (!m_module_mgr->GetHuaShenManager()->Put(item_data, true, take_off_index))
		{
			return -7;
		}
	}

	equipment_list[equip_index].Clear();		// 指针已无用 删除之
	if (ROLE_TYPE_BENZUN == role_type)
	{
		m_other_param.m_equipment_lock_list[equip_index].Reset();
	}

	this->SetEquipGridDirty(equip_index, !is_from_puton, role_type);
	if (!is_from_puton)
	{
		if (role_type == ROLE_TYPE_BENZUN)
		{
			m_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_EQUIPMENT, __FUNCTION__);
		}
		else
		{
			m_module_mgr->GetHuaShenManager()->ReCalcAttr(AttributeList::RECALC_REASON_EQUIPMENT, __FUNCTION__);
		}
	}

	EventHandler::Instance().OnRoleTakeOffEquip(m_module_mgr->GetRole(), equip_index, is_from_puton, role_type);
	return 0;
}

void EquipmentManager::EquipGemMounting(int stone_type, int equip_grid, int stone_grid)
{
	ItemGridData * tmp_equip = NULL;
	ItemGridData tmp_stone;
	tmp_stone.Reset();

	if (WORKSHOP_STONE_TYPE_EQUIP == stone_type)
	{
		if (equip_grid >= Equipment::E_INDEX_MIN && equip_grid < Equipment::E_INDEX_MAX)
		{
			tmp_equip = &m_equipment_list[equip_grid];
		}
	}
	else
	{
		if (equip_grid >= Jewelry::J_TYPE_GEM_RING && equip_grid < Jewelry::J_TYPE_MAX)
		{
			tmp_equip = &m_jewelry_list[equip_grid];
		}
	}

	if (NULL == tmp_equip || tmp_equip->Invalid() || NULL == tmp_equip->param) return;
	m_module_mgr->GetKnapsack()->GetItemGridData(ItemNamespace::ITEM_COLUMN_TYPE_ITEM, stone_grid, &tmp_stone);
	if (tmp_stone.Invalid()) return;

	const ItemBase * equip_ib = ITEMPOOL->GetItem(tmp_equip->item_id);
	if (NULL == equip_ib)
	{
		m_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	int t_cur = 0;

	ItemID *stone_id = NULL;
	if (WORKSHOP_STONE_TYPE_EQUIP == stone_type)
	{
		if (I_TYPE_EQUIPMENT != equip_ib->GetItemType())
		{
			m_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
			return;
		}
		EquipmentParam & p_ep = tmp_equip->param->equip_param;
		stone_id = p_ep.new_stone;
	}
	else if (WORKSHOP_STONE_TYPE_JEWELRY == stone_type)
	{
		if (I_TYPE_JEWELRY != equip_ib->GetItemType())
		{
			m_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
			return;
		}
		JewelryParam & p_ep = tmp_equip->param->jewelry_param;
		stone_id = p_ep.new_stone;
	}


	const Equipment * temp_info = (const Equipment *)equip_ib;
	for (; t_cur < NEW_EQUIP_STONE_MOUNT_MAX; ++t_cur)
	{
		const GemstoneBasicInfoCfg * p_gbic = LOGIC_CONFIG->GetGemstoneConfig()->GetGemstoneBasicInfoCfg(stone_id[t_cur]);

		bool is_godequip = temp_info->GetColor() >= ItemBase::I_COLOR_RED ? true : false;
		int equip_level = temp_info->GetLimitLevel();
		if (is_godequip)
		{
			if (WORKSHOP_STONE_TYPE_EQUIP == stone_type)
			{
				equip_level = tmp_equip->param->equip_param.strengthen_level;
			}
			else
			{
				equip_level = tmp_equip->param->jewelry_param.strengthen_level;
			}
		}
		if (NULL == p_gbic && LOGIC_CONFIG->GetGemstoneConfig()->CheckGemMountSlotOpen(t_cur, equip_level, is_godequip)) break;
	}

	if (t_cur >= NEW_EQUIP_STONE_MOUNT_MAX)
	{
		m_module_mgr->NoticeNum(errornum::EN_WORKSHOP_GEM_MOUNT_ALREADY_MAX);
		return;
	}
	int gamestone_type = -1;
	int equip_type = WORKSHOP_STONE_TYPE_EQUIP;
	if (WORKSHOP_STONE_TYPE_EQUIP == stone_type)
	{
		Equipment * t_e = (Equipment *)equip_ib;
		equip_type = t_e->GetEquipType();
		gamestone_type = LOGIC_CONFIG->GetGemstoneConfig()->GetGemstoneByType(WORKSHOP_STONE_TYPE_EQUIP, equip_type);
	}
	else if (WORKSHOP_STONE_TYPE_JEWELRY == stone_type)
	{
		Jewelry * t_e = (Jewelry *)equip_ib;
		equip_type = t_e->GetJewelryType();
		gamestone_type = LOGIC_CONFIG->GetGemstoneConfig()->GetGemstoneByType(WORKSHOP_STONE_TYPE_JEWELRY, equip_type);
	}

	if (gamestone_type < 0)return;

	const GemstoneBasicInfoCfg * mate_gem = LOGIC_CONFIG->GetGemstoneConfig()->GetGemstoneBasicInfoCfg(tmp_stone.item_id);
	if (NULL == equip_ib || NULL == mate_gem || !LOGIC_CONFIG->GetGemstoneConfig()->CheckGemstoneCanMount(gamestone_type, mate_gem->gemstone_type))
	{
		m_module_mgr->NoticeNum(errornum::EN_WORKSHOP_GEM_PARAM_OCCUR_ERROR);
		return;
	}

	bool is_godequip = temp_info->GetColor() >= ItemBase::I_COLOR_RED ? true : false;
	int equip_level = temp_info->GetLimitLevel();
	if (is_godequip)
	{
		if (WORKSHOP_STONE_TYPE_EQUIP == stone_type)
		{
			equip_level = tmp_equip->param->equip_param.strengthen_level;
		}
		else
		{
			equip_level = tmp_equip->param->jewelry_param.strengthen_level;
		}
	}

	if ((is_godequip ? mate_gem->godequip_level : mate_gem->level_limit) > equip_level)
	{
		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_workshop_strone_puton_level_notice);
		if (length > 0)
		{
			Protocol::SCSystemMsg sm;
			int sendlen = 0;
			if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT)))
			{
				EngineAdapter::Instance().NetSend(m_module_mgr->GetRole()->GetNetId(), (const char*)&sm, sendlen);
			}
		}
		return;
	}

	// 消耗宝石
	if (!m_module_mgr->GetKnapsack()->ConsumeItemByIndex(ItemNamespace::ITEM_COLUMN_TYPE_ITEM, stone_grid, 1, __FUNCTION__))
	{
		m_module_mgr->NoticeNum(errornum::EN_STUFF_NOT_ENOUGH);
		return;
	}

	// 消耗成功，生效
	if (false == tmp_equip->is_bind) tmp_equip->is_bind = true;
	stone_id[t_cur] = tmp_stone.item_id;

	ROLE_LOG_QUICK6(LOG_TYPE_GEM_INLAY, m_module_mgr->GetRole(), tmp_equip->item_id, tmp_stone.item_id, "GemMount", NULL);

	// 重算属性、发送信息
	m_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_EQUIPMENT, __FUNCTION__);
	if (WORKSHOP_STONE_TYPE_EQUIP == stone_type)
	{
		this->SetEquipGridDirty(equip_grid);
	}
	else
	{
		this->SetJewelryGridDirty(equip_grid);
	}
	gamelog::g_log_workshop.printf(LL_INFO, "%s user[%d,%s] weapon_id{[%d] param: [%s]}", __FUNCTION__,
		m_module_mgr->GetUid(), m_module_mgr->GetRole()->GetName(),
		tmp_equip->item_id, tmp_equip->param->GetParamLog(equip_ib->GetItemType()));
	EventHandler::Instance().OnGemMounting(m_module_mgr->GetRole(), tmp_stone.item_id);

	if (stone_type)
	{
		if (equip_type < Equipment::E_TYPE_MIN || equip_type >= Equipment::E_TYPE_MAX) return;
		int equip_index = Equipment::E_TYPE_MAX - equip_type - 1;
		this->SynItemDataPut(I_TYPE_EQUIPMENT, equip_index, tmp_equip);
	}
	else
	{
		if (equip_type < Jewelry::J_TYPE_GEM_RING || equip_type >= Jewelry::J_TYPE_MAX) return;
		int equip_index = Jewelry::J_TYPE_MAX - equip_type - 1;
		this->SynItemDataPut(I_TYPE_JEWELRY, equip_index, tmp_equip);
	}
}

void EquipmentManager::EquipGemDismount(int stone_type, int equip_grid, int stone_slot)
{
	if (stone_slot < 0 || stone_slot >= NEW_EQUIP_STONE_MOUNT_MAX) return;

	ItemGridData * tmp_equip = NULL;

	if (WORKSHOP_STONE_TYPE_EQUIP == stone_type)
	{
		if (equip_grid >= Equipment::E_INDEX_MIN && equip_grid < Equipment::E_INDEX_MAX)
		{
			tmp_equip = &m_equipment_list[equip_grid];
		}
	}
	else
	{
		if (equip_grid >= Jewelry::J_TYPE_GEM_RING && equip_grid < Jewelry::J_TYPE_MAX)
		{
			tmp_equip = &m_jewelry_list[equip_grid];
		}
	}

	if (NULL == tmp_equip || tmp_equip->Invalid() || NULL == tmp_equip->param) return;

	const ItemBase * equip_ib = ITEMPOOL->GetItem(tmp_equip->item_id);
	if (NULL == equip_ib)
	{
		m_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	ItemID *stone_id = NULL;
	if (WORKSHOP_STONE_TYPE_EQUIP == stone_type)
	{
		if (I_TYPE_EQUIPMENT != equip_ib->GetItemType())
		{
			m_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
			return;
		}
		EquipmentParam & p_ep = tmp_equip->param->equip_param;
		stone_id = p_ep.new_stone;
	}
	else if (WORKSHOP_STONE_TYPE_JEWELRY == stone_type)
	{
		if (I_TYPE_JEWELRY != equip_ib->GetItemType())
		{
			m_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
			return;
		}
		JewelryParam & p_ep = tmp_equip->param->jewelry_param;
		stone_id = p_ep.new_stone;
	}

	if (NULL == stone_id)
	{
		return;
	}

	ItemID & target_gem = stone_id[stone_slot];
	if (NULL == LOGIC_CONFIG->GetGemstoneConfig()->GetGemstoneBasicInfoCfg(target_gem))
	{
		target_gem = 0;
		m_module_mgr->NoticeNum(errornum::EN_WORKSHOP_GEM_PARAM_OCCUR_ERROR);
		return;
	}

	if (!m_module_mgr->GetKnapsack()->Put(ItemConfigData(target_gem, true, 1), PUT_REASON_NO_NOTICE)) return;

	gamelog::g_log_workshop.printf(LL_INFO, "Workshop::EquipGemDismount user[%d,%s] weapon_id{[%d] param: [%s]}",
		m_module_mgr->GetUid(), m_module_mgr->GetRole()->GetName(),
		tmp_equip->item_id, tmp_equip->param->GetParamLog(equip_ib->GetItemType()));

	EventHandler::Instance().OnGemDismount(m_module_mgr->GetRole(), target_gem);

	ROLE_LOG_QUICK6(LOG_TYPE_GEM_INLAY, m_module_mgr->GetRole(), tmp_equip->item_id, target_gem, "GemDismount", NULL);

	// 宝石已放入背包，注意不能中途返回
	target_gem = 0;

	m_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_EQUIPMENT, __FUNCTION__);
	if (WORKSHOP_STONE_TYPE_EQUIP == stone_type)
	{
		this->SetEquipGridDirty(equip_grid);
	}
	else
	{
		this->SetJewelryGridDirty(equip_grid);
	}
}

void EquipmentManager::EquipGemUpgrades(int stone_type, int equip_grid, int stone_slot)
{
	if (stone_slot < 0 || stone_slot >= NEW_EQUIP_STONE_MOUNT_MAX) return;

	ItemGridData * tmp_equip = NULL;
	if (WORKSHOP_STONE_TYPE_EQUIP == stone_type)
	{
		if (equip_grid >= Equipment::E_INDEX_MIN && equip_grid < Equipment::E_INDEX_MAX)
		{
			tmp_equip = &m_equipment_list[equip_grid];
		}
	}
	else
	{
		if (equip_grid >= Jewelry::J_TYPE_GEM_RING && equip_grid < Jewelry::J_TYPE_MAX)
		{
			tmp_equip = &m_jewelry_list[equip_grid];
		}
	}

	if (NULL == tmp_equip || tmp_equip->Invalid() || NULL == tmp_equip->param) return;

	const ItemBase * equip_ib = ITEMPOOL->GetItem(tmp_equip->item_id);
	if (NULL == equip_ib)
	{
		m_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	ItemID *stone_id = NULL;
	if (WORKSHOP_STONE_TYPE_EQUIP == stone_type)
	{
		if (I_TYPE_EQUIPMENT != equip_ib->GetItemType())
		{
			m_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
			return;
		}
		EquipmentParam & p_ep = tmp_equip->param->equip_param;
		stone_id = p_ep.new_stone;
	}
	else if (WORKSHOP_STONE_TYPE_JEWELRY == stone_type)
	{
		if (I_TYPE_JEWELRY != equip_ib->GetItemType())
		{
			m_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
			return;
		}
		JewelryParam & p_ep = tmp_equip->param->jewelry_param;
		stone_id = p_ep.new_stone;
	}

	const ItemID & gem_item_id = stone_id[stone_slot];

	if (tmp_equip->Invalid() || 0 == gem_item_id) return;

	const GemstoneBasicInfoCfg * cur_gem_cfg = LOGIC_CONFIG->GetGemstoneConfig()->GetGemstoneBasicInfoCfg(gem_item_id);
	if (NULL == cur_gem_cfg)
	{
		m_module_mgr->NoticeNum(errornum::EN_WORKSHOP_GEM_PARAM_OCCUR_ERROR);
		return;
	}

	GemUpgradedata & gud_con = m_gem_upgrade_param.ug_data;
	gud_con.target_gem = LOGIC_CONFIG->GetGemstoneConfig()->GetGemstoneItemID(cur_gem_cfg->gemstone_type, cur_gem_cfg->level + 1);
	if (0 == gud_con.target_gem)
	{
		m_module_mgr->NoticeNum(errornum::EN_WORKSHOP_GEM_ALREADY_MAX_LEVEL);
		return;
	}

	{
		const GemstoneBasicInfoCfg * cur_gem_cfg_up = LOGIC_CONFIG->GetGemstoneConfig()->GetGemstoneBasicInfoCfg(gud_con.target_gem);
		if (NULL != cur_gem_cfg_up)
		{
			const Equipment * temp_info = (const Equipment *)equip_ib;

			bool is_godequip = temp_info->GetColor() >= ItemBase::I_COLOR_RED ? true : false;
			int equip_level = temp_info->GetLimitLevel();
			if (is_godequip)
			{
				if (WORKSHOP_STONE_TYPE_EQUIP == stone_type)
				{
					equip_level = tmp_equip->param->equip_param.strengthen_level;
				}
				else
				{
					equip_level = tmp_equip->param->jewelry_param.strengthen_level;
				}
			}

			if ((is_godequip ? cur_gem_cfg_up->godequip_level : cur_gem_cfg_up->level_limit) > equip_level)
			{
				int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_workshop_strone_level_notice);
				if (length > 0)
				{
					Protocol::SCSystemMsg sm;
					int sendlen = 0;
					if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT)))
					{
						EngineAdapter::Instance().NetSend(m_module_mgr->GetRole()->GetNetId(), (const char*)&sm, sendlen);
					}
				}
				return;
			}
		}
	}

	// 设定初状态
	gud_con.fin_gem_id = gud_con.target_gem;
	gud_con.fin_need_numb = 1;
	m_gem_upgrade_param.compose_times = 0;

	std::map<ItemID, int> tmp_owned_list; tmp_owned_list[gem_item_id] = 1;	// 装备上镶嵌着的也算进去
	this->CalcGemOneKeyUpgradeNeed(tmp_owned_list);
	tmp_owned_list[gem_item_id] -= 1;		// 再减掉装备上镶嵌的

											// 如果小于等于 0，缩短列表
	for (std::map<ItemID, int>::iterator loop = tmp_owned_list.begin(); loop != tmp_owned_list.end();)
	{
		if (loop->second <= 0)
		{
			tmp_owned_list.erase(loop++);
		}
		else
		{
			++loop;
		}
	}

	if ((int)tmp_owned_list.size() > ItemNamespace::MAX_BATCH_CONSUME_ITEM_NUM)
	{
		m_module_mgr->NoticeNum(errornum::EN_WORKSHOP_WITHOUT_LEGAL_RESULTS);
		return;
	}

	gud_con.list_num = 0;
	for (std::map<ItemID, int>::iterator loop = tmp_owned_list.begin(); loop != tmp_owned_list.end(); ++loop)
	{
		gud_con.own_list[gud_con.list_num++] = GemUpgradedata::ItemPair(loop->first, loop->second);
	}

	//const GemstoneOtherInfoCfg & gem_other_cfg = LOGIC_CONFIG->GetGemstoneConfig()->GetGemstoneOtherInfoCfg();
	gud_con.consume_coins = 0;
	if (gud_con.fin_need_numb > 0)
	{
		if (!MarketManager::Instance().GetItemPrice(gud_con.fin_gem_id, gud_con.fin_need_numb, &gud_con.money_type, &gud_con.consume_coins))
		{
			m_module_mgr->NoticeNum(errornum::EN_WORKSHOP_WITHOUT_LEGAL_RESULTS);
			return;
		}
	}

	// 记录数据
	m_gem_upgrade_param.SetParam(stone_type, 0, equip_grid, stone_slot);
	this->SendGemOneKeyUpgradeNeed();
	need_confirm = true;
}

void EquipmentManager::ConfirmGemUpgrade()
{
	if (!need_confirm)return;
	if (m_gem_upgrade_param.stone_type < WORKSHOP_STONE_TYPE_EQUIP || m_gem_upgrade_param.stone_type > WORKSHOP_STONE_TYPE_JEWELRY)
	{
		m_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	if (m_gem_upgrade_param.stone_slot < 0 || m_gem_upgrade_param.stone_slot >= NEW_EQUIP_STONE_MOUNT_MAX)
	{
		m_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	ItemGridData * tmp_equip = NULL;
	if (WORKSHOP_STONE_TYPE_EQUIP == m_gem_upgrade_param.stone_type)
	{
		if (m_gem_upgrade_param.equip_grid >= Equipment::E_INDEX_MIN && m_gem_upgrade_param.equip_grid < Equipment::E_INDEX_MAX)
		{
			tmp_equip = &m_equipment_list[m_gem_upgrade_param.equip_grid];
		}
	}
	else
	{
		if (m_gem_upgrade_param.equip_grid >= Jewelry::J_TYPE_GEM_RING && m_gem_upgrade_param.equip_grid < Jewelry::J_TYPE_MAX)
		{
			tmp_equip = &m_jewelry_list[m_gem_upgrade_param.equip_grid];
		}
	}
	if (NULL == tmp_equip || tmp_equip->Invalid() || NULL == tmp_equip->param) return;

	const ItemBase * equip_ib = ITEMPOOL->GetItem(tmp_equip->item_id);
	if (NULL == equip_ib)
	{
		m_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	ItemID *stone_id = NULL;
	if (WORKSHOP_STONE_TYPE_EQUIP == m_gem_upgrade_param.stone_type)
	{
		if (I_TYPE_EQUIPMENT != equip_ib->GetItemType())
		{
			m_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
			return;
		}
		EquipmentParam & p_ep = tmp_equip->param->equip_param;
		stone_id = p_ep.new_stone;
	}
	else if (WORKSHOP_STONE_TYPE_JEWELRY == m_gem_upgrade_param.stone_type)
	{
		if (I_TYPE_JEWELRY != equip_ib->GetItemType())
		{
			m_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
			return;
		}
		JewelryParam & p_ep = tmp_equip->param->jewelry_param;
		stone_id = p_ep.new_stone;
	}

	ItemID & gem_item_id = stone_id[m_gem_upgrade_param.stone_slot];
	if (tmp_equip->Invalid() || 0 == gem_item_id) return;

	GemUpgradedata & gud_con = m_gem_upgrade_param.ug_data;
	if (gud_con.IsInvalid() || gud_con.target_gem == gem_item_id) return;

	if (gud_con.consume_coins > 0)
	{
		switch (gud_con.money_type)
		{
		case MONEY_TYPE_GOLD:
			if (!m_module_mgr->GetMoney()->GoldMoreThan(gud_con.consume_coins))
			{
				m_module_mgr->NoticeNum(errornum::EN_GOLD_NOT_ENOUGH);
				return;
			}
			break;
		case MONEY_TYPE_IMMORTAL_COIN:
			if (!m_module_mgr->GetMoney()->ImmortalCoinMoreThan(gud_con.consume_coins))
			{
				m_module_mgr->NoticeNum(errornum::EN_IMMORTAL_COIN_NOT_ENOUGH);
				return;
			}
			break;
		case MONEY_TYPE_COIN_BIND:
			if (!m_module_mgr->GetMoney()->CoinBindMoreThan(gud_con.consume_coins))
			{
				m_module_mgr->NoticeNum(errornum::EN_COIN_NOT_ENOUGH);
				return;
			}
			break;
		case MONEY_TYPE_SILVER_COIN:
			if (!m_module_mgr->GetMoney()->OtherCoinMoreThan(gud_con.consume_coins, MONEY_TYPE_SILVER_COIN))
			{
				m_module_mgr->NoticeNum(errornum::EN_SILVER_COIN_NOT_ENOUGH);
				return;
			}
			break;
		default:
			if (!m_module_mgr->GetMoney()->OtherCoinMoreThan(gud_con.consume_coins, gud_con.money_type))
			{
				m_module_mgr->NoticeNum(errornum::EN_OTHER_MONEY_NOT_ENOUGH);
				return;
			}
			break;
		}
	}

	// 获取材料列表
	int stuff_count = 0; ItemExtern::ItemConsumeConfig stuff_list[ItemNamespace::MAX_BATCH_CONSUME_ITEM_NUM];
	for (int t_cur = 0; t_cur < gud_con.list_num && stuff_count < ItemNamespace::MAX_BATCH_CONSUME_ITEM_NUM; ++t_cur)
	{
		const GemUpgradedata::ItemPair & mate_item = gud_con.own_list[t_cur];
		stuff_list[stuff_count++] = ItemExtern::ItemConsumeConfig(mate_item.item_id, false, mate_item.item_num);
	}

	// 消耗
	if (stuff_count > 0)
	{
		static ItemExtern::ItemConsumeList consume_list; consume_list.Reset();
		if (!ItemExtern::GetConsumeListByOrder(m_module_mgr->GetRole(), false, stuff_count, stuff_list, &consume_list, true))
		{
			m_module_mgr->NoticeNum(errornum::EN_STUFF_NOT_ENOUGH);		// 正常来说不会走到这里
			return;
		}

		if (consume_list.count > 0 && !m_module_mgr->GetKnapsack()->ConsumeItemByIndexList(consume_list.count, consume_list.column_list, consume_list.index_list, consume_list.num_list, "WorkshopCompoundItem")) return;
	}

	if (gud_con.consume_coins > 0)
	{
		switch (gud_con.money_type)
		{
		case MONEY_TYPE_GOLD:
			if (!m_module_mgr->GetMoney()->UseGold(gud_con.consume_coins, "WorkshopGemOneKeyUpgrade"))
			{
				return;
			}
			break;
		case MONEY_TYPE_IMMORTAL_COIN:
			if (!m_module_mgr->GetMoney()->UseImmortalCoin(gud_con.consume_coins, "WorkshopGemOneKeyUpgrade"))
			{
				return;
			}
			break;
		case MONEY_TYPE_COIN_BIND:
			if (!m_module_mgr->GetMoney()->UseCoinBind(gud_con.consume_coins, "WorkshopGemOneKeyUpgrade"))
			{
				return;
			}
			break;
		case MONEY_TYPE_SILVER_COIN:
			if (!m_module_mgr->GetMoney()->UseOtherCoin(gud_con.consume_coins, MONEY_TYPE_SILVER_COIN, "WorkshopGemOneKeyUpgrade"))
			{
				return;
			}
			EventHandler::Instance().OnMarketItemPurchasedByOtherSystem(m_module_mgr->GetRole(), gud_con.fin_gem_id, gud_con.fin_need_numb,
				gud_con.consume_coins, gud_con.money_type);
			break;

		default:
			if (!m_module_mgr->GetMoney()->UseOtherCoin(gud_con.consume_coins, gud_con.money_type, "WorkshopGemOneKeyUpgrade"))
			{
				return;
			}
			break;
		}
	}

	// 道具已消耗，注意不能中途返回
	gem_item_id = gud_con.target_gem;
	if (WORKSHOP_EQUIP_POSI_EQUIP == m_gem_upgrade_param.equip_posi)
	{
		m_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_EQUIPMENT, __FUNCTION__);
		if (WORKSHOP_STONE_TYPE_EQUIP == m_gem_upgrade_param.stone_type)
		{
			this->SetEquipGridDirty(m_gem_upgrade_param.equip_grid);
		}
		else
		{
			this->SetJewelryGridDirty(m_gem_upgrade_param.equip_grid);
		}
	}

	if (gud_con.fin_need_numb > 0)
	{
		RoleActivityFortuneMaze * fortune_maze = static_cast<RoleActivityFortuneMaze *>
			(m_module_mgr->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_FORTUNE_MAZE));
		if (NULL != fortune_maze)
		{
			fortune_maze->BuyStone(gud_con.fin_need_numb);
		}
		EventHandler::Instance().OnBuyItem(m_module_mgr->GetRole(), gud_con.fin_gem_id, gud_con.fin_need_numb);
	}
	gud_con.SetInvalid();

	gamelog::g_log_workshop.printf(LL_INFO, "Workshop::EquipGemDismount user[%d,%s] weapon_id{[%d] param: [%s]}",
		m_module_mgr->GetUid(), m_module_mgr->GetRole()->GetName(),
		tmp_equip->item_id, tmp_equip->param->GetParamLog(equip_ib->GetItemType()));

	EventHandler::Instance().OnCompoundGem(m_module_mgr->GetRole(), gem_item_id);

	ROLE_LOG_QUICK6(LOG_TYPE_GEM_INLAY, m_module_mgr->GetRole(), tmp_equip->item_id, gem_item_id, "GemUpgrade", NULL);

	if (gud_con.consume_coins > 0)
	{
		RoleActivityGodToken * god_token = static_cast<RoleActivityGodToken *>
			(m_module_mgr->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_GOD_TOKEN));
		if (NULL != god_token && god_token->IsRoleraGodTokenOpen())
		{
			god_token->OnRAGodTokenAddCount(RA_GOD_TOKEN_DAY_TYPE_SHOP_BUY_GEM);
			god_token->OnRAGodTokenWeekAddCount(RA_GOD_TOKEN_TYPE_WEEK_SHOP_BUY_GEM);
		}
	}
	need_confirm = false;
}

void EquipmentManager::CalcGemOneKeyUpgradeNeed(std::map<ItemID, int>& owned_list)
{
	GemUpgradedata & gud_con = m_gem_upgrade_param.ug_data;
	if (gud_con.fin_need_numb <= 0) return;

	const ItemPairCon * mate_con = LOGIC_CONFIG->GetWorkshopConfig()->GetFastComposeGemCon(gud_con.fin_gem_id);
	if (NULL == mate_con) return;

	int owned_num = m_module_mgr->GetKnapsack()->Count(mate_con->item_id, false) + owned_list[mate_con->item_id];
	int need_num = gud_con.fin_need_numb * mate_con->item_num;
	int consume_num = GetMin(owned_num, need_num);
	owned_list[mate_con->item_id] = consume_num;

	m_gem_upgrade_param.compose_times += gud_con.fin_need_numb;

	// 额外所需的
	gud_con.fin_need_numb = need_num - consume_num;
	if (gud_con.fin_need_numb > 0)
	{
		gud_con.fin_gem_id = mate_con->item_id;
		this->CalcGemOneKeyUpgradeNeed(owned_list);
	}
}

void EquipmentManager::SendGemOneKeyUpgradeNeed()
{
	static Protocol::SCGemOneKeyUpgradesNeeds gokun;

	gokun.mate_data = m_gem_upgrade_param.ug_data;

	m_module_mgr->NetSend((const char *)&gokun, sizeof(gokun));
}

void EquipmentManager::LockEquipMentVoc(short equip_type, short equip_index, short voc_idx, short lock)
{
	switch (equip_type)
	{
	case 1://装备
		{
			if (equip_index < 0 || equip_index >= Equipment::E_INDEX_MAX)return;
			EquipVocLock &lock_info = m_other_param.m_equipment_lock_list[equip_index];
			if (voc_idx < 0 || voc_idx >= EquipVocLock::EXA_ATTR_NUM_MAX)return;
			lock_info.is_lock[voc_idx] = lock > 0 ? 1 : 0;
		}
		break;
	case 2://灵饰
		{
			if (equip_index < 0 || equip_index >= Jewelry::J_TYPE_MAX)return;
			EquipVocLock &lock_info = m_other_param.m_jewelry_lock_list[equip_index];
			if (voc_idx < 0 || voc_idx >= EquipVocLock::EXA_ATTR_NUM_MAX)return;
			lock_info.is_lock[voc_idx] = lock > 0 ? 1 : 0;
		}
		break;
	default:
		break;
	}
	this->SendEquipMentLockInfo();
}

// 用于物品使用时调用，所以没有消耗，消耗应在外部调用处加上
bool EquipmentManager::PutOnElementCrystal(const ItemGridData &grid_data)
{
	const ItemBase* item_base = ITEMPOOL->GetItem(grid_data.item_id);
	if (NULL == item_base || I_TYPE_ELEMENT_CRYSTAL != item_base->GetItemType())
	{
		return false;
	}

	if (0 != m_other_param.element_crystal_id && (ItemID)-1 != m_other_param.element_crystal_id)
	{
		if (!this->TakeOffElementCrystal(true))
		{
			return false;
		}
	}

	m_other_param.element_crystal_unique_id = grid_data.item_unique_id;
	m_other_param.element_crystal_id = grid_data.item_id;
	m_other_param.element_crystal_used_times = grid_data.param->element_crystal_param.used_times;
	m_other_param.element_crystal_is_market_buy_flag = grid_data.is_market_buy_flag;

	this->SendElementCrystalChangeNotice();

	m_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_EQUIPMENT, __FUNCTION__);

	return true;
}

bool EquipmentManager::TakeOffElementCrystal(bool is_from_puton /*= false*/)
{
	if (0 == m_other_param.element_crystal_id || (ItemID)-1 == m_other_param.element_crystal_id)
	{
		return false;
	}

	const ItemBase *item_base = ITEMPOOL->GetItem(m_other_param.element_crystal_id);
	if (NULL == item_base) return false;

	if (!m_module_mgr->GetKnapsack()->CheckForPut(m_other_param.element_crystal_id, 1))
	{
		m_module_mgr->GetRole()->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return false;
	}

	static NetValueItemParam tmp_item_param;
	tmp_item_param.Reset();
	tmp_item_param.element_crystal_param.used_times = m_other_param.element_crystal_used_times;
	tmp_item_param.element_crystal_param.has_set_net_value = 1;

	static ItemDataWrapper wrapper;
	wrapper.Reset();
	wrapper.item_id = m_other_param.element_crystal_id;
	wrapper.num = 1;
	wrapper.is_bind = true;
	wrapper.is_market_buy_flag = m_other_param.element_crystal_is_market_buy_flag;
	wrapper.item_unique_id = m_other_param.element_crystal_unique_id;
	if (wrapper.item_unique_id == 0)
	{
		long long item_unique_id = 0;
		UniqueIDGenerator::Instance()->GenUniqueID(&item_unique_id);
		if (0 != item_unique_id)
		{
			wrapper.item_unique_id = item_unique_id;
			gamelog::g_log_item_track.printf(LL_INFO, "TRACK_ITEM_FLAG %s assign unique_id[%lld] to role[%d,%s] item[%d,%s] num[%d]]", __FUNCTION__,
				item_unique_id, m_module_mgr->GetRole()->GetUID(), m_module_mgr->GetRole()->GetName(), wrapper.item_id, item_base->GetItemName(), wrapper.num);
		}
	}
	tmp_item_param.GetData(wrapper.param_data);

	/*if (!m_module_mgr->GetKnapsack()->Put(wrapper, PUT_REASON_NO_NOTICE))
	{
		m_module_mgr->GetRole()->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return false;
	}*/
	if (m_other_param.element_crystal_used_times == 0)
	{
		m_module_mgr->GetKnapsack()->Put(wrapper, PUT_REASON_PARTNER_ELEMENT_TAKEOFF_BAD);
	}
	else
	{
		m_module_mgr->GetKnapsack()->Put(wrapper, PUT_REASON_PARTNER_ELEMENT_TAKEOFF_DEFAULT);
	}

	m_other_param.element_crystal_id = (ItemID)-1;
	m_other_param.element_crystal_used_times = 0;
	m_other_param.element_crystal_unique_id = 0;
	m_other_param.element_crystal_is_market_buy_flag = 0;
	if (!is_from_puton)
	{
		this->SendElementCrystalChangeNotice();

		m_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_EQUIPMENT, __FUNCTION__);
	}

	return true;
}

int EquipmentManager::ClearMixedElementCrystal()
{
	if (0 == m_other_param.element_crystal_id || (ItemID)-1 == m_other_param.element_crystal_id)
	{
		return 0;
	}

	if (!IsMixedElementCrystalId(m_other_param.element_crystal_id))
	{
		return 0;
	}

	m_other_param.element_crystal_id = (ItemID)-1;
	m_other_param.element_crystal_used_times = 0;
	m_other_param.element_crystal_unique_id = 0;
	m_other_param.element_crystal_is_market_buy_flag = 0;

	this->SendElementCrystalChangeNotice();
	m_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_EQUIPMENT, __FUNCTION__);

	return 1;
}

void EquipmentManager::SendEquipmentInfoToRole()
{
	static Protocol::SCEquipmentInfoAck ack;

	ack.element_crystal_id = m_other_param.element_crystal_id;
	ack.used_times = m_other_param.element_crystal_used_times;
	if (ack.element_crystal_id == (ItemID)-1)
	{
		ack.element_crystal_id = 0;
	}

	ack.reserve_sh = 0;
	ack.count = 0;

	for (int i = Equipment::E_INDEX_MIN; i < Equipment::E_INDEX_MAX && ack.count < ItemNamespace::MAX_EQUIPMENT_GRID_NUM; ++i)
	{
		if (m_equipment_list[i].Invalid()) continue;

		ack.equipment_list[ack.count].index = i;
		ack.equipment_list[ack.count].item_id = m_equipment_list[i].item_id;
		ack.equipment_list[ack.count].num = m_equipment_list[i].num;
		//ack.equipment_list[ack.count].is_bind = m_equipment_list[i].is_bind ? 1 : 0;
		ack.equipment_list[ack.count].item_unique_id = m_equipment_list[i].item_unique_id;

		if (NULL != m_equipment_list[i].param)
		{
			ack.equipment_list[ack.count].param = m_equipment_list[i].param->equip_param;
		}

		/*const EquipmentScoreCfg * score_cfg = LOGIC_CONFIG->GetScoreSystemConfig()->GetEquipmentScoreCfg(m_equipment_list[i].item_id);
		if (NULL != score_cfg && NULL != m_equipment_list[i].param)
		{
			EquipmentParam & data = m_equipment_list[i].param->equip_param;
			if (data.is_treasure == ITEM_SCORE_LEVEL_S)
			{
				m_equipment_list[i].is_bind = false;
				ack.equipment_list[ack.count].is_bind = 0;
			} else
			{
				m_equipment_list[i].is_bind = true;
				ack.equipment_list[ack.count].is_bind = 1;
			}
		} else
		{
			m_equipment_list[i].is_bind = true;
			ack.equipment_list[ack.count].is_bind = 1;
		}*/

		ack.count += 1;
	}

	int send_len = sizeof(ack) - (ItemNamespace::MAX_EQUIPMENT_GRID_NUM - ack.count) * sizeof(ack.equipment_list[0]);
	EngineAdapter::Instance().NetSend(m_module_mgr->GetRole()->GetNetId(), (const char*)&ack, send_len);
}

void EquipmentManager::SendEquipmentInfoToRoleByIndex(short equip_index)
{
	if (equip_index < Equipment::E_INDEX_MIN || equip_index >= Equipment::E_INDEX_MAX) return;

	static Protocol::SCEquipmentGridChange msg;
	msg.equip_info.index = equip_index;
	msg.equip_info.item_id = m_equipment_list[equip_index].item_id;
	msg.equip_info.num = m_equipment_list[equip_index].num;
	msg.equip_info.is_bind = m_equipment_list[equip_index].is_bind ? 1 : 0;
	msg.equip_info.item_unique_id = m_equipment_list[equip_index].item_unique_id;
	if (NULL != m_equipment_list[equip_index].param)
	{
		msg.equip_info.param = m_equipment_list[equip_index].param->equip_param;
	}
	else
	{
		memset(&msg.equip_info.param, 0, sizeof(msg.equip_info.param));
	}

	EngineAdapter::Instance().NetSend(m_module_mgr->GetRole()->GetNetId(), (const char*)&msg, sizeof(msg));
}

void EquipmentManager::SendElementCrystalChangeNotice()
{
	static Protocol::SCElementCrystalChangeNotice msg;
	msg.element_crystal_id = m_other_param.element_crystal_id;
	msg.used_times = m_other_param.element_crystal_used_times;
	if (msg.element_crystal_id == (ItemID)-1)
	{
		msg.element_crystal_id = 0;
	}

	EngineAdapter::Instance().NetSend(m_module_mgr->GetRole()->GetNetId(), (const char*)&msg, sizeof(msg));
}

void EquipmentManager::SetEquipGridDirty(short equip_index, bool to_notice_client /* = true */, short role_type /*= ROLE_TYPE_BENZUN*/)
{
	if (equip_index < 0 || equip_index >= Equipment::E_INDEX_MAX) return;

	if (role_type == ROLE_TYPE_BENZUN)
	{
		m_dirty_mark[equip_index] = true;
	}
	else
	{
		m_huashen_dirty_mark[equip_index] = true;
	}
	m_equipment_is_change = true;

	if (to_notice_client)
	{
		if (ROLE_TYPE_BENZUN == role_type)
		{
			this->SendEquipmentInfoToRoleByIndex(equip_index);
		}
		else
		{
			this->SendHuaShenEquipmentInfoByIndex(equip_index);
		}
	}
}

bool EquipmentManager::GetEquipmentGridData(short equip_index, ItemGridData *equip_data, short role_type)
{
	if (NULL == equip_data || equip_index < 0 || equip_index >= Equipment::E_INDEX_MAX || role_type < 0 || role_type >= ROLE_TYPE_MAX) return false;

	ItemGridData * equipment_list = NULL;
	if (role_type == ROLE_TYPE_BENZUN)
	{
		equipment_list = m_equipment_list;
	}
	else
	{
		equipment_list = m_huashen_equipment_list;
	}

	if (equipment_list[equip_index].Invalid()) return false;

	equip_data->Set(equipment_list[equip_index]);

	return true;
}

ItemGridData * EquipmentManager::GetEquipmentGridData2(int equip_index, short role_type /* = ROLE_TYPE_BENZUN */)
{
	if (equip_index < 0 || equip_index >= Equipment::E_INDEX_MAX)
	{
		return NULL;
	}

	ItemGridData * equip_list = NULL;
	if (role_type == ROLE_TYPE_BENZUN)
	{
		equip_list = m_equipment_list;
	}
	else if (role_type == ROLE_TYPE_HUASHEN)
	{
		equip_list = m_huashen_equipment_list;
	}
	else
	{
		return false;
	}

	if (equip_list[equip_index].Invalid())
	{
		return false;
	}

	return &equip_list[equip_index];
}

bool EquipmentManager::GetEquipmentGridData(short equip_index, ItemDataWrapper & equip_data, short role_type)
{
	if (equip_index < 0 || equip_index >= Equipment::E_INDEX_MAX
		|| role_type < 0 || role_type >= ROLE_TYPE_MAX)
	{
		return false;
	}

	ItemGridData * equipment_list = NULL;
	if (role_type == ROLE_TYPE_BENZUN)
	{
		equipment_list = m_equipment_list;
	}
	else
	{
		equipment_list = m_huashen_equipment_list;
	}

	if (equipment_list[equip_index].Invalid()) return false;

	equipment_list[equip_index].WrapTo(&equip_data);

	return true;
}

bool EquipmentManager::GetEquipmentGridVocLock(short equip_index, short voc_idx)
{
	if (equip_index < 0 || equip_index >= Equipment::E_INDEX_MAX || voc_idx < 0 || voc_idx >= EquipmentParam::EXA_ATTR_NUM_MAX) return false;

	if (m_equipment_list[equip_index].Invalid()) return false;

	if (m_other_param.m_equipment_lock_list[equip_index].is_lock[voc_idx] > 0)return true;

	return false;
}

void EquipmentManager::GetEquipmentList(int *count, QueryItemListParam::ItemListData *save_list)
{
	int tmp_count = 0;

	for (int i = 0; i < Equipment::E_INDEX_MAX; ++i)
	{
		if (m_equipment_list[i].Invalid() || NULL == m_equipment_list[i].param) continue;

		save_list[tmp_count].index = i + ItemNamespace::EQUIPMENT_INDEX_BEGIN; // 装备存储时的index从这个开始
		m_equipment_list[i].WrapTo(&save_list[tmp_count].item_wrapper);

		save_list[tmp_count].change_state_item = structcommon::CS_NONE;

		if (++tmp_count >= *count) break;
	}

	*count = tmp_count;
}

void EquipmentManager::GetChangeEquipmentList(int *count, ItemListParam::ItemListData *save_list)
{
	if (!m_equipment_is_change)
	{
		*count = 0;
		return;
	}

	int tmp_count = 0;

	for (int i = 0; i < Equipment::E_INDEX_MAX && tmp_count < *count; ++i)
	{
		if (m_dirty_mark[i])
		{
			save_list[tmp_count].index = i + ItemNamespace::EQUIPMENT_INDEX_BEGIN;

			if (!m_equipment_list[i].Invalid() && NULL != m_equipment_list[i].param)
			{
				m_equipment_list[i].WrapTo(&save_list[tmp_count].item_wrapper);

				save_list[tmp_count].change_state_item = (m_old_state[i] ? structcommon::CS_UPDATE : structcommon::CS_INSERT);
			}
			else
			{
				if (!m_old_state[i]) continue;

				save_list[tmp_count].change_state_item = structcommon::CS_DELETE;
			}

			if (++tmp_count >= *count) break;
		}
	}

	for (int i = 0; i < Equipment::E_INDEX_MAX && tmp_count < *count; ++i)
	{
		if (m_huashen_dirty_mark[i])
		{
			save_list[tmp_count].index = i + ItemNamespace::HUASHEN_EQUIPEMNET_INDEX_BEGIN;

			if (!m_huashen_equipment_list[i].Invalid() && NULL != m_huashen_equipment_list[i].param)
			{
				m_huashen_equipment_list[i].WrapTo(&save_list[tmp_count].item_wrapper);

				save_list[tmp_count].change_state_item = (m_huashen_old_state[i] ? structcommon::CS_UPDATE : structcommon::CS_INSERT);
			}
			else
			{
				if (!m_huashen_old_state[i]) continue;

				save_list[tmp_count].change_state_item = structcommon::CS_DELETE;
			}

			if (++tmp_count >= *count) break;
		}
	}

	*count = tmp_count;
}

void EquipmentManager::ClearDirtyMark()
{
	if (!m_equipment_is_change) return;

	for (int i = 0; i < Equipment::E_INDEX_MAX; ++i)
	{
		if (m_dirty_mark[i]) m_old_state[i] = !m_equipment_list[i].Invalid();
		if (m_huashen_dirty_mark[i]) m_huashen_old_state[i] = !m_huashen_equipment_list[i].Invalid();

	}

	memset(m_dirty_mark, 0, sizeof(m_dirty_mark));
	memset(m_huashen_dirty_mark, 0, sizeof(m_huashen_dirty_mark));

	m_equipment_is_change = false;
}

bool EquipmentManager::HuaShenPutOnElementCrystal(int index)
{
	static ItemGridData grid_data; grid_data.Reset();
	if (!m_module_mgr->GetHuaShenManager()->GetItemGridData(index, &grid_data))
	{
		return false;
	}
	if (0 != m_other_param.huashen_element_crystal_id && (ItemID)-1 != m_other_param.huashen_element_crystal_id)
	{
		if (!this->HuaShenTakeOffElementCrystal(true))
		{
			return false;
		}
	}

	long long unique_id = grid_data.item_unique_id;
	int crystal_id = grid_data.item_id;
	int crystal_used_times = grid_data.param->element_crystal_param.used_times;
	bool is_bind = grid_data.is_bind;
	if (!m_module_mgr->GetHuaShenManager()->Consume(index, true))
	{
		return false;
	}

	m_other_param.huashen_element_crystal_unique_id = unique_id;
	m_other_param.huashen_element_crystal_id = crystal_id;
	m_other_param.huashen_element_crystal_used_times = crystal_used_times;
	m_other_param.huashen_element_crystal_is_bind = true;
	if (!is_bind)
	{
		m_module_mgr->GetKnapsack()->SynItemBindInfo(ITEM_COLUMN_TYPE_ITEM, unique_id, true);
	}
	m_module_mgr->GetHuaShenManager()->PutOn(0, grid_data.item_unique_id, HUASHEN_GRID_INDEX_TYPE_CRYSTAL);
	this->SendHuaShenElementCrystalChangeNotice();
	m_module_mgr->GetHuaShenManager()->ReCalcAttr(AttributeList::RECALC_REASON_EQUIPMENT, __FUNCTION__);
	return true;
}

bool EquipmentManager::HuaShenTakeOffElementCrystal(bool is_from_puton)
{
	if (0 == m_other_param.huashen_element_crystal_id || (ItemID)-1 == m_other_param.huashen_element_crystal_id)
	{
		return false;
	}
	const ItemBase *itembase = ITEMPOOL->GetItem(m_other_param.huashen_element_crystal_id);
	if (NULL == itembase) return false;


	static NetValueItemParam tmp_item_param;
	tmp_item_param.Reset();
	tmp_item_param.element_crystal_param.used_times = m_other_param.huashen_element_crystal_used_times;
	tmp_item_param.element_crystal_param.has_set_net_value = 1;

	static ItemDataWrapper wrapper;
	wrapper.Reset();
	wrapper.item_id = m_other_param.huashen_element_crystal_id;
	wrapper.num = 1;
	wrapper.is_bind = m_other_param.huashen_element_crystal_is_bind;
	wrapper.item_unique_id = m_other_param.huashen_element_crystal_unique_id;
	wrapper.has_param = 1;

	tmp_item_param.GetData(wrapper.param_data);

	//先删除映射,顺序不可调换
	m_module_mgr->GetHuaShenManager()->TakeOff(wrapper.item_unique_id);
	if (!m_module_mgr->GetHuaShenManager()->SynBenZunItemData(&wrapper, HUASHEN_KNAPSACK_OP_TYPE_ADD, true))
	{
		return false;
	}

	m_other_param.huashen_element_crystal_id = (ItemID)-1;
	m_other_param.huashen_element_crystal_used_times = 0;
	m_other_param.huashen_element_crystal_unique_id = 0;
	m_other_param.huashen_element_crystal_is_bind = 0;

	if (!is_from_puton)
	{
		this->SendHuaShenElementCrystalChangeNotice();
		m_module_mgr->GetHuaShenManager()->ReCalcAttr(AttributeList::RECALC_REASON_EQUIPMENT, __FUNCTION__);
	}

	return true;
}

bool EquipmentManager::HuaShenCrystalUpdate(ItemDataWrapper * item_data)
{
	if (NULL == item_data || 0 == item_data->has_param) return false;

	static NetValueItemParam item_value;
	item_value.SetData(item_data->param_data);
	m_other_param.huashen_element_crystal_used_times = item_value.element_crystal_param.used_times;
	m_other_param.huashen_element_crystal_is_bind = (char)item_data->is_bind;
	return true;
}

bool EquipmentManager::HuaShenEquipmentUpdate(int equip_index, ItemDataWrapper * item_data)
{
	if (NULL == item_data || equip_index < 0 || equip_index >= Equipment::E_INDEX_MAX || item_data->has_param == 0) return false;

	//防止出现BUG导致服务器挂了
	ItemGridData & i_d = m_huashen_equipment_list[equip_index];
	if (NULL == i_d.param)
	{
		i_d.param = ItemBase::CreateParam();
	}
	i_d.param->SetData(item_data->param_data);
	i_d.is_bind = (0 != item_data->is_bind);
	if (m_huashen_equipment_list[equip_index].item_id != item_data->item_id)
	{
		m_huashen_equipment_list[equip_index].item_id = item_data->item_id;
	}
	this->SetEquipGridDirty(equip_index, true, ROLE_TYPE_HUASHEN);
	return true;
}

bool EquipmentManager::HuaShenEquipmentReset(int equip_index, bool is_notice)
{
	if (equip_index < 0 || equip_index >= Equipment::E_INDEX_MAX) return false;

	m_huashen_equipment_list[equip_index].Clear();
	this->SetEquipGridDirty(equip_index, is_notice, ROLE_TYPE_HUASHEN);
	return true;
}

bool EquipmentManager::HuaShenCrystalReset(bool is_notice)
{
	m_other_param.huashen_element_crystal_id = (ItemID)-1;
	m_other_param.huashen_element_crystal_used_times = 0;
	m_other_param.huashen_element_crystal_unique_id = 0;
	if (is_notice)
	{
		this->SendHuaShenElementCrystalChangeNotice();
	}
	return true;
}

void EquipmentManager::GetHuaShenWeaponAttr(short max_attr_num, short * out_attr_num, AttrInfo * out_attr_list)
{
	if (NULL == out_attr_num || NULL == out_attr_list) return;

	*out_attr_num = 0;

	if (m_huashen_equipment_list[Equipment::E_INDEX_WEAPON].Invalid()) return;

	NetValueItemParam* param = m_huashen_equipment_list[Equipment::E_INDEX_WEAPON].param;
	if (NULL == param) return;

	for (int i = 0; i < max_attr_num && i < param->equip_param.attr_num; ++i)
	{
		out_attr_list[(*out_attr_num)++] = param->equip_param.attr_list[i];
	}
}

void EquipmentManager::GetHuaShenEquipSpecialEffect(int max_weapon_sp_num, short * out_weapon_sp_num, int * out_weapon_sp_list, int max_other_sp_num, short * out_other_sp_num, int * out_other_sp_list)
{
	if (NULL == out_weapon_sp_num || NULL == out_weapon_sp_list || NULL == out_other_sp_num || NULL == out_other_sp_list) return;

	for (int i = Equipment::E_INDEX_MIN; i < Equipment::E_INDEX_MAX; ++i)
	{
		if (m_huashen_equipment_list[i].Invalid()) continue;

		if (NULL == m_huashen_equipment_list[i].param) continue;

		if (m_huashen_equipment_list[i].param->equip_param.sp_id <= 0) continue;

		if (i == Equipment::E_INDEX_WEAPON)
		{
			if (*out_weapon_sp_num < max_weapon_sp_num)
			{
				out_weapon_sp_list[(*out_weapon_sp_num)++] = m_huashen_equipment_list[i].param->equip_param.sp_id;
			}
		}
		else
		{
			if (*out_other_sp_num < max_other_sp_num)
			{
				out_other_sp_list[(*out_other_sp_num)++] = m_huashen_equipment_list[i].param->equip_param.sp_id;
			}
		}
	}
}

void EquipmentManager::SendHuaShenEquipmentInfo()
{
	static Protocol::SCHuaShenEquipmentInfoAck ack;

	ack.element_crystal_id = m_other_param.huashen_element_crystal_id;
	ack.used_times = m_other_param.huashen_element_crystal_used_times;
	if (ack.element_crystal_id == (ItemID)-1)
	{
		ack.element_crystal_id = 0;
	}

	ack.reserve_short = 0;
	ack.huashen_count = 0;

	for (int i = Equipment::E_INDEX_MIN; i < Equipment::E_INDEX_MAX && ack.huashen_count < ItemNamespace::MAX_EQUIPMENT_GRID_NUM; ++i)
	{
		if (m_huashen_equipment_list[i].Invalid()) continue;

		ack.huashen_equipment_list[ack.huashen_count].index = i;
		ack.huashen_equipment_list[ack.huashen_count].item_id = m_huashen_equipment_list[i].item_id;
		ack.huashen_equipment_list[ack.huashen_count].num = m_huashen_equipment_list[i].num;
		ack.huashen_equipment_list[ack.huashen_count].is_bind = m_huashen_equipment_list[i].is_bind ? 1 : 0;
		ack.huashen_equipment_list[ack.huashen_count].item_unique_id = m_huashen_equipment_list[i].item_unique_id;
		ack.huashen_equipment_list[ack.huashen_count].param = m_huashen_equipment_list[i].param->equip_param;
		ack.huashen_count += 1;
	}

	int send_len = sizeof(ack) - (ItemNamespace::MAX_EQUIPMENT_GRID_NUM - ack.huashen_count) * sizeof(ack.huashen_equipment_list[0]);
	EngineAdapter::Instance().NetSend(m_module_mgr->GetRole()->GetNetId(), (const char*)&ack, send_len);
}

void EquipmentManager::SendHuaShenEquipmentInfoByIndex(short equip_index)
{
	if (equip_index < Equipment::E_INDEX_MIN || equip_index >= Equipment::E_INDEX_MAX) return;

	static Protocol::SCHuaShenEquipmentGridChange info;
	info.equip_info.index = equip_index;
	info.equip_info.item_id = m_huashen_equipment_list[equip_index].item_id;
	info.equip_info.num = m_huashen_equipment_list[equip_index].num;
	info.equip_info.is_bind = m_huashen_equipment_list[equip_index].is_bind;
	info.equip_info.item_unique_id = m_huashen_equipment_list[equip_index].item_unique_id;
	if (NULL != m_huashen_equipment_list[equip_index].param)
	{
		info.equip_info.param = m_huashen_equipment_list[equip_index].param->equip_param;
	}
	else
	{
		memset(&info.equip_info.param, 0, sizeof(info.equip_info.param));
	}
	EngineAdapter::Instance().NetSend(m_module_mgr->GetRole()->GetNetId(), (const char*)&info, sizeof(info));
}

void EquipmentManager::SendHuaShenElementCrystalChangeNotice()
{
	static Protocol::SCHuaShenElementCrystalChangeNotice msg;
	msg.element_crystal_id = m_other_param.huashen_element_crystal_id;
	msg.used_times = m_other_param.huashen_element_crystal_used_times;
	if (msg.element_crystal_id == (ItemID)-1)
	{
		msg.element_crystal_id = 0;
	}

	EngineAdapter::Instance().NetSend(m_module_mgr->GetRole()->GetNetId(), (const char*)&msg, sizeof(msg));
}

bool EquipmentManager::GetJewelryGridData(short jewelry_type, ItemGridData * carrier_data, int role_type)
{
	if (NULL == carrier_data || jewelry_type < 0 || jewelry_type >= Jewelry::J_TYPE_MAX || m_jewelry_list[jewelry_type].Invalid()) return false;

	ItemGridData * jewelry_list = NULL;
	if (role_type == ROLE_TYPE_BENZUN)
	{
		jewelry_list = m_jewelry_list;
	}
	else if (role_type == ROLE_TYPE_HUASHEN)
	{
		jewelry_list = m_huashen_jewelry_list;
	}
	else
	{
		return false;
	}

	carrier_data->Set(jewelry_list[jewelry_type]);
	return true;
}

ItemGridData * EquipmentManager::GetJewelryGridData2(int jewelry_type, int role_type /* = ROLE_TYPE_BENZUN */)
{
	if (jewelry_type < 0 || jewelry_type >= Jewelry::J_TYPE_MAX)
	{
		return NULL;
	}

	ItemGridData * jewelry_list = NULL;
	if (role_type == ROLE_TYPE_BENZUN)
	{
		jewelry_list = m_jewelry_list;
	}
	else if (role_type == ROLE_TYPE_HUASHEN)
	{
		jewelry_list = m_huashen_jewelry_list;
	}
	else
	{
		return false;
	}

	if (jewelry_list[jewelry_type].Invalid())
	{
		return false;
	}

	return &jewelry_list[jewelry_type];
}

bool EquipmentManager::GetJewelryGridData(short jewelry_type, ItemDataWrapper & carrier_data, int role_type)
{
	if (jewelry_type < 0 || jewelry_type >= Jewelry::J_TYPE_MAX
		|| role_type < 0 || role_type >= ROLE_TYPE_MAX)
	{
		return false;
	}

	ItemGridData * jewelry_list = NULL;
	if (role_type == ROLE_TYPE_BENZUN)
	{
		jewelry_list = m_jewelry_list;
	}
	else
	{
		jewelry_list = m_huashen_jewelry_list;
	}

	if (jewelry_list[jewelry_type].Invalid()) return false;

	jewelry_list[jewelry_type].WrapTo(&carrier_data);

	return true;
}

bool EquipmentManager::GetJewelryGridVocLock(short jewelry_type, short voc_idx)
{
	if (jewelry_type < 0 || jewelry_type >= Jewelry::J_TYPE_MAX || voc_idx < 0 || voc_idx >= JewelryParam::EXA_ATTR_NUM_MAX) return false;

	if (m_jewelry_list[jewelry_type].Invalid()) return false;

	if (m_other_param.m_jewelry_lock_list[jewelry_type].is_lock[voc_idx] > 0)return true;

	return false;
}

void EquipmentManager::GetJewelryList(int * count, QueryItemListParam::ItemListData * save_list)
{
	int tmp_count = 0;

	for (int i = 0; i < Jewelry::J_TYPE_MAX; ++i)
	{
		if (m_jewelry_list[i].Invalid() || NULL == m_jewelry_list[i].param) continue;

		save_list[tmp_count].index = i + ItemNamespace::JEWELRY_INDEX_BEGIN;
		m_jewelry_list[i].WrapTo(&save_list[tmp_count].item_wrapper);

		save_list[tmp_count].change_state_item = structcommon::CS_NONE;

		if (++tmp_count >= *count) break;
	}

	*count = tmp_count;
}

void EquipmentManager::GetChangeJewelryList(int * count, ItemListParam::ItemListData * save_list)
{
	if (!m_jewelry_is_change)
	{
		*count = 0;
		return;
	}

	int tmp_count = 0;

	for (int i = 0; i < Jewelry::J_TYPE_MAX && tmp_count < *count; ++i)
	{
		if (!m_j_dirty_mark[i]) continue;

		save_list[tmp_count].index = i + ItemNamespace::JEWELRY_INDEX_BEGIN;

		if (!m_jewelry_list[i].Invalid() && NULL != m_jewelry_list[i].param)
		{
			m_jewelry_list[i].WrapTo(&save_list[tmp_count].item_wrapper);
			save_list[tmp_count].change_state_item = (m_j_old_state[i] ? structcommon::CS_UPDATE : structcommon::CS_INSERT);
		}
		else
		{
			if (!m_j_old_state[i]) continue;
			save_list[tmp_count].change_state_item = structcommon::CS_DELETE;
		}

		if (++tmp_count >= *count) break;
	}

	for (int i = 0; i < Jewelry::J_TYPE_MAX && tmp_count < *count; ++i)
	{
		if (!m_huashen_j_dirty_mark[i]) continue;

		save_list[tmp_count].index = i + ItemNamespace::HUASHEN_JEWELRY_INDEX_BEGIN;

		if (!m_huashen_jewelry_list[i].Invalid() && NULL != m_huashen_jewelry_list[i].param)
		{
			m_huashen_jewelry_list[i].WrapTo(&save_list[tmp_count].item_wrapper);
			save_list[tmp_count].change_state_item = (m_huashen_j_old_state[i] ? structcommon::CS_UPDATE : structcommon::CS_INSERT);
		}
		else
		{
			if (!m_huashen_j_old_state[i]) continue;
			save_list[tmp_count].change_state_item = structcommon::CS_DELETE;
		}

		if (++tmp_count >= *count) break;
	}


	*count = tmp_count;
}

void EquipmentManager::SendAllJewelryInfo()
{
	static Protocol::SCJewelryAllInform jai;

	jai.jewelry_count = 0;
	for (int j_type = 0; j_type < Jewelry::J_TYPE_MAX; ++j_type)
	{
		if (m_jewelry_list[j_type].Invalid()) continue;

		const ItemGridData & p_igd = m_jewelry_list[j_type];
		JewelryPro & p_jp = jai.jewelry_list[jai.jewelry_count++];
		p_jp.jewelry_type = j_type;
		p_jp.jewelry_item = p_igd.item_id;
		p_jp.item_unique_id = p_igd.item_unique_id;
		p_jp.j_param = p_igd.param->jewelry_param;

		/*const EquipmentScoreCfg * score_cfg = LOGIC_CONFIG->GetScoreSystemConfig()->GetEquipmentScoreCfg(p_igd.item_id);
		if (score_cfg)
		{
			JewelryParam & data = p_igd.param->jewelry_param;
			if (data.is_treasure == ITEM_SCORE_LEVEL_S)
			{
				p_igd.is_bind = false;
				p_jp.is_bind = 0;
			} else
			{
				p_igd.is_bind = true;
				p_jp.is_bind = 1;
			}
		} else
		{
			p_igd.is_bind = true;
			p_jp.is_bind = 0;
		}*/
	}

	int send_len = sizeof(jai) - (Jewelry::J_TYPE_MAX - jai.jewelry_count) * sizeof(JewelryPro);
	EngineAdapter::Instance().NetSend(m_module_mgr->GetRole()->GetNetId(), (const char*)&jai, send_len);
}

void EquipmentManager::SendSingleJewelryInfo(short jewelry_type, bool is_wear)
{
	if (jewelry_type < 0 || jewelry_type >= Jewelry::J_TYPE_MAX) return;

	static Protocol::SCJewelryInfoAlter jia;

	const ItemGridData & p_igd = m_jewelry_list[jewelry_type];

	if (!is_wear) const_cast<ItemGridData &>(p_igd).Clear();

	if (NULL != p_igd.param)
	{
		jia.jewelry_info.j_param = p_igd.param->jewelry_param;
		/*const EquipmentScoreCfg * score_cfg = LOGIC_CONFIG->GetScoreSystemConfig()->GetEquipmentScoreCfg(p_igd.item_id);
		if (NULL != score_cfg)
		{
			JewelryParam & data = p_igd.param->jewelry_param;
			if (data.is_treasure == ITEM_SCORE_LEVEL_S)
			{
				p_igd.is_bind = false;
			}
			else
			{
				p_igd.is_bind = true;
			}
		}
		else
		{
			p_igd.is_bind = true;
		}*/
	}
	else
	{
		memset(&jia.jewelry_info.j_param, 0, sizeof(jia.jewelry_info.j_param));
	}

	jia.jewelry_info.jewelry_type = jewelry_type;
	jia.jewelry_info.jewelry_item = p_igd.item_id;
	jia.jewelry_info.item_unique_id = p_igd.item_unique_id;
	jia.jewelry_info.is_bind = p_igd.is_bind ? 1 : 0;


	EngineAdapter::Instance().NetSend(m_module_mgr->GetRole()->GetNetId(), (const char*)&jia, sizeof(jia));
}

bool EquipmentManager::WearJewelry(short grid_index)
{
	static ItemGridData grid_data; grid_data.Reset();
	if (!m_module_mgr->GetKnapsack()->GetItemGridData(ItemNamespace::ITEM_COLUMN_TYPE_ITEM, grid_index, &grid_data)) return false;

	const Jewelry * jewelry = (const Jewelry *)ITEMPOOL->GetItem(grid_data.item_id);
	if (NULL == jewelry || I_TYPE_JEWELRY != jewelry->GetItemType() || NULL == grid_data.param) return false;

	const short j_type = jewelry->GetJewelryType();
	if (j_type >= Jewelry::J_TYPE_MAX || j_type < 0) return false;

	if (!jewelry->IsUnLimitLevel() && m_module_mgr->GetRole()->GetLevel() < jewelry->GetLimitLevel())
	{
		m_module_mgr->NoticeNum(errornum::EN_EQUIPMENT_PUT_LEVEL_LIMIT);
		return false;
	}
	int take_off_column = -1;
	int take_off_index = -1;
	bool need_zhu_ling_notice = false; //没什么用，客户端需要
	if (NULL != m_jewelry_list[j_type].param && NULL != grid_data.param)
	{
		need_zhu_ling_notice = m_jewelry_list[j_type].param->jewelry_param.HasZhuLing() && grid_data.param->jewelry_param.HasZhuLing();//没什么用，客户端需要
	}
	if (!m_jewelry_list[j_type].Invalid())
	{
		bool ret = this->TakeOffJewelry(j_type, false, &take_off_column, &take_off_index);
		if (!ret)
		{
			return ret;
		}
	}

	NetValueItemParam * param = jewelry->CreateParamFromStruct(*grid_data.param);
	if (NULL == param) return false;

	ItemGridData & p_igd = m_jewelry_list[j_type];
	if (p_igd.param)
	{
		need_zhu_ling_notice = p_igd.param->jewelry_param.HasZhuLing() && param->jewelry_param.HasZhuLing();//没什么用，客户端需要
	}

	p_igd.item_id = grid_data.item_id;
	p_igd.param = param;
	p_igd.num = 1;
	p_igd.is_bind = true;//穿戴变绑定 [由品质确定]
	p_igd.is_market_buy_flag = grid_data.is_market_buy_flag;

	bool has_notic = false;
	Protocol::SCZhuLingPutOnSuccNotice zhu_ling_notice;
	zhu_ling_notice.p1 = 0;
	const ItemGridData * temp = m_module_mgr->GetKnapsack()->GetItemGridData2(take_off_column, take_off_index);
	if (NULL != temp)
	{
		const Equipment * temp_info = (const Equipment *)ITEMPOOL->GetItem(temp->item_id);
		if (temp_info != NULL && I_TYPE_JEWELRY == temp_info->GetItemType())
		{
			//获得脱下来那个装备，如果正要穿上的装备没有铸灵，正要替换铸灵
			if (!p_igd.param->jewelry_param.HasZhuLing() && temp->param->jewelry_param.HasZhuLing())
			{
				need_zhu_ling_notice = true;
				zhu_ling_notice.p1 = 1;
				for (int i = 0; i < EQUIP_ZHU_LING_SLOT_NUM; ++i)
				{
					p_igd.param->jewelry_param.zhu_ling_info[i] = temp->param->jewelry_param.zhu_ling_info[i];
					temp->param->jewelry_param.zhu_ling_info[i].Reset();
				}
			}

			//检查准备要穿戴的装备宝石数量，有空位则转移一颗宝石
			int vacancy_idx = p_igd.param->jewelry_param.HasVacancyStone();//有空位的宝石位置 
			const ItemGridData & stone_data = *temp;
			int round = 0;//避免死循环

			int t_cur = 0;
			for (; t_cur < NEW_EQUIP_STONE_MOUNT_MAX; ++t_cur)//检测开启的孔数	//此时要以穿戴的这件装备能接受的孔位,防止一些BUG
			{
				bool is_godequip = jewelry->GetColor() >= ItemBase::I_COLOR_RED ? true : false;
				int equip_level = true == is_godequip ? p_igd.param->jewelry_param.strengthen_level : jewelry->GetLimitLevel();
				if (!LOGIC_CONFIG->GetGemstoneConfig()->CheckGemMountSlotOpen(t_cur, equip_level, is_godequip))break;
			}

			while (vacancy_idx < t_cur && stone_data.param->jewelry_param.HasStone() && round < 50)
			{
				round += 1;
				ItemID top_stone_id = 0;
				int top_level = 0;
				int top_stone_idx = t_cur;
				for (int stone_idx = 0; stone_idx < t_cur && stone_idx < ARRAY_LENGTH(stone_data.param->jewelry_param.new_stone); stone_idx++)
				{
					if (stone_data.param->jewelry_param.new_stone[stone_idx] <= 0) continue;
					const GemstoneBasicInfoCfg * gbic = LOGIC_CONFIG->GetGemstoneConfig()->GetGemstoneBasicInfoCfg(stone_data.param->jewelry_param.new_stone[stone_idx]);
					if (gbic != NULL)
					{
						if (gbic->level > top_level)
						{
							top_stone_id = stone_data.param->jewelry_param.new_stone[stone_idx];
							top_level = gbic->level;
							top_stone_idx = stone_idx;
						}
					}
				}
				if (top_stone_idx >= 0 && top_stone_idx < t_cur && top_stone_idx < ARRAY_LENGTH(p_igd.param->jewelry_param.new_stone))
				{
					//替换宝石
					p_igd.param->jewelry_param.new_stone[vacancy_idx] = top_stone_id;
					//消除宝石
					stone_data.param->jewelry_param.new_stone[top_stone_idx] = 0;
					if (!has_notic)m_module_mgr->NoticeNum(noticenum::NT_EQUIP_STONE_TRANF_SUCC);
					has_notic = true;
				}
				vacancy_idx = p_igd.param->jewelry_param.HasVacancyStone();
			}
			m_module_mgr->GetKnapsack()->SetItemDirty(take_off_column, take_off_index, true, Knapsack::ITEM_CHANGE_NOTICE_REASON_DATA_CHANGE);
		}
	}

	if (need_zhu_ling_notice)
	{
		m_module_mgr->NetSend(&zhu_ling_notice, sizeof(zhu_ling_notice));
	}
	/*const EquipmentScoreCfg * score_cfg = LOGIC_CONFIG->GetScoreSystemConfig()->GetEquipmentScoreCfg(p_igd.item_id);
	if (score_cfg)
	{
		JewelryParam & data = param->jewelry_param;
		if (data.is_treasure == ITEM_SCORE_LEVEL_S)
		{
			p_igd.is_bind = false;
		}
	}*/

	p_igd.invalid_time = grid_data.invalid_time;
	p_igd.item_unique_id = grid_data.item_unique_id;

	if (!m_module_mgr->GetKnapsack()->ConsumeItemByIndex(ItemNamespace::ITEM_COLUMN_TYPE_ITEM, grid_index, 1, __FUNCTION__))
	{
		return false;
	}
	m_other_param.m_jewelry_lock_list[j_type].Reset();
	this->SendEquipMentLockInfo();
	m_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_EQUIPMENT, __FUNCTION__);
	//此时宝石经过了转移,需要同步数据
	/*if (has_notic || grid_data.is_bind != p_igd.is_bind)
	{
		this->SynItemDataPut(I_TYPE_JEWELRY, j_type, &p_igd);
	}*/

	this->SetJewelryGridDirty(j_type);
	EventHandler::Instance().OnPutOnJewelry(m_module_mgr->GetRole(), j_type);

	return true;
}

bool EquipmentManager::TakeOffJewelry(short jewelry_type, bool need_recalc, int * take_off_column, int * take_off_index)
{
	if (jewelry_type < 0 || jewelry_type >= Jewelry::J_TYPE_MAX || m_jewelry_list[jewelry_type].Invalid()) return false;

	ItemGridData & p_igd = m_jewelry_list[jewelry_type];
	const Jewelry * jewelry = (const Jewelry *)ITEMPOOL->GetItem(p_igd.item_id);
	if (NULL == jewelry || I_TYPE_JEWELRY != jewelry->GetItemType()) return false;

	Knapsack * knapsack = m_module_mgr->GetKnapsack();
	if (!knapsack->CheckForPut(jewelry->GetItemId(), 1))
	{
		m_module_mgr->GetRole()->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return false;
	}

	ItemDataWrapper item_wrapper(p_igd.item_id, 1, p_igd.is_bind ? 1 : 0, 1, p_igd.param->data, p_igd.invalid_time, 0, p_igd.item_unique_id, p_igd.is_market_buy_flag);

	/*const EquipmentScoreCfg * score_cfg = LOGIC_CONFIG->GetScoreSystemConfig()->GetEquipmentScoreCfg(p_igd.item_id);
	if (score_cfg)
	{
		JewelryParam & data = p_igd.param->jewelry_param;
		if (data.is_treasure == ITEM_SCORE_LEVEL_S)
		{
			p_igd.is_bind = false;
	}
		else
		{
			p_igd.is_bind = true;
		}
	}
	else
	{
		p_igd.is_bind = true;
	}*/
	item_wrapper.is_bind = p_igd.is_bind ? 1 : 0;

	if (!knapsack->Put(item_wrapper, PUT_REASON_NO_NOTICE, take_off_column, take_off_index)) return false;

	//p_igd.Clear();
	m_other_param.m_jewelry_lock_list[jewelry_type].Reset();

	this->SetJewelryGridDirty(jewelry_type, true, ROLE_TYPE_BENZUN, false);
	if (need_recalc) m_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_EQUIPMENT, __FUNCTION__);
	EventHandler::Instance().OnTakeOffJewelry(m_module_mgr->GetRole(), jewelry_type);
	return true;
}

ItemGridData * EquipmentManager::GetJewalryListType(int type, short role_type)
{
	if (role_type == ROLE_TYPE_BENZUN)
	{
		if (type < Jewelry::J_TYPE_GEM_RING || type >= Jewelry::J_TYPE_MAX)return NULL;
		if (m_jewelry_list[type].Invalid())return NULL;
		return &m_jewelry_list[type];
	}
	else
	{
		if (type < Jewelry::J_TYPE_GEM_RING || type >= Jewelry::J_TYPE_MAX)return NULL;
		if (m_huashen_jewelry_list[type].Invalid())return NULL;
		return &m_huashen_jewelry_list[type];
	}
	return NULL;
}

ItemGridData * EquipmentManager::GetEquipListType(int type, short role_type)
{
	if (role_type == ROLE_TYPE_BENZUN)
	{
		if (type < Equipment::E_INDEX_MIN || type >= Equipment::E_INDEX_MAX)return NULL;
		if (m_equipment_list[type].Invalid())return NULL;
		return &m_equipment_list[type];
	}
	else
	{
		if (type <= Equipment::E_INDEX_MIN || type >= Equipment::E_INDEX_MAX)return NULL;
		if (m_huashen_equipment_list[type].Invalid())return NULL;
		return &m_huashen_equipment_list[type];
	}
	return NULL;
}

void EquipmentManager::EquipUpStar(int seq, int equip_type, int equip_grid, int cost_grid, int cost_grid_2)
{
	const EquipStarUpCfg * up_star_cfg = LOGIC_CONFIG->GetEquipStarConfig()->GetEquipStarUpCfg(seq);
	if (NULL == up_star_cfg)
	{
		m_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	if (NULL == ITEMPOOL->GetItem(up_star_cfg->compose_id))
	{
		return;
	}

	ItemGridData * tmp_equip = NULL;

	if (WORKSHOP_STONE_TYPE_EQUIP == equip_type)
	{
		if (equip_grid >= Equipment::E_INDEX_MIN && equip_grid < Equipment::E_INDEX_MAX)
		{
			tmp_equip = &m_equipment_list[equip_grid];
		}
	}
	else
	{
		if (equip_grid >= Jewelry::J_TYPE_GEM_RING && equip_grid < Jewelry::J_TYPE_MAX)
		{
			tmp_equip = &m_jewelry_list[equip_grid];
		}
	}
	if (NULL == tmp_equip || tmp_equip->Invalid() || NULL == tmp_equip->param)return;

	const ItemBase * equip_ib = ITEMPOOL->GetItem(tmp_equip->item_id);
	if (NULL == equip_ib)
	{
		m_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	if (equip_ib->GetItemType() == I_TYPE_EQUIPMENT)
	{
		if (tmp_equip->param->equip_param.star_level + 1 != up_star_cfg->item_star)
		{
			m_module_mgr->NoticeNum(errornum::EN_STAR_UP_ERR_NOT_SAME_STAR_LEVEL);
			return;
		}

		if (equip_ib->GetColor() != up_star_cfg->color_limit)
		{
			m_module_mgr->NoticeNum(errornum::EN_STAR_UP_ERR_NOT_SAME_STAR_LEVEL);
			return;
		}

		if (equip_ib->GetLimitLevel() != up_star_cfg->level_limit)
		{
			m_module_mgr->NoticeNum(errornum::EN_STAR_UP_ERR_NOT_SAME_STAR_LEVEL);
			return;
		}
	}
	else if (equip_ib->GetItemType() == I_TYPE_JEWELRY)
	{
		if (tmp_equip->param->jewelry_param.star_level + 1 != up_star_cfg->item_star)
		{
			m_module_mgr->NoticeNum(errornum::EN_STAR_UP_ERR_NOT_SAME_STAR_LEVEL);
			return;
		}

		if (equip_ib->GetColor() != up_star_cfg->color_limit)
		{
			m_module_mgr->NoticeNum(errornum::EN_STAR_UP_ERR_NOT_SAME_STAR_LEVEL);
			return;
		}

		if (equip_ib->GetLimitLevel() != up_star_cfg->level_limit)
		{
			m_module_mgr->NoticeNum(errornum::EN_STAR_UP_ERR_NOT_SAME_STAR_LEVEL);
			return;
		}
	}
	else
	{
		m_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	//检查消耗
	ItemGridData cost_tmp1; cost_tmp1.Reset();
	ItemGridData cost_tmp2; cost_tmp2.Reset();
	if (!m_module_mgr->GetKnapsack()->GetItemGridData(ItemNamespace::ITEM_COLUMN_TYPE_ITEM, cost_grid, &cost_tmp1) ||
		!m_module_mgr->GetKnapsack()->GetItemGridData(ItemNamespace::ITEM_COLUMN_TYPE_ITEM, cost_grid_2, &cost_tmp2))
	{
		m_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	const ItemBase * cost_tmp1_ib = ITEMPOOL->GetItem(cost_tmp1.item_id);
	if (NULL == cost_tmp1_ib)
	{
		m_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	const ItemBase * cost_tmp2_ib = ITEMPOOL->GetItem(cost_tmp2.item_id);
	if (NULL == cost_tmp2_ib)
	{
		m_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	{//检查消耗道具1
		if (cost_tmp1_ib->GetItemType() == I_TYPE_JEWELRY)
		{
			const Jewelry * cost_tmp1_base = (const Jewelry *)cost_tmp1_ib;
			if (cost_tmp1.param->jewelry_param.star_level + 1 != up_star_cfg->item_star)
			{
				m_module_mgr->NoticeNum(errornum::EN_STAR_UP_ERR_NOT_SAME_STAR_LEVEL);
				return;//同星级
			}

			if (cost_tmp1_base->GetLimitLevel() != up_star_cfg->level_limit)
			{
				m_module_mgr->NoticeNum(errornum::EN_STAR_UP_ERR_NOT_SAME_LEVEL);
				return;//同等级
			}

			if (cost_tmp1_base->GetColor() != up_star_cfg->color_limit)
			{
				m_module_mgr->NoticeNum(errornum::EN_STAR_UP_ERR_NOT_SAME_QUALITY);
				return;//同品质
			}

			if (cost_tmp1.param->jewelry_param.HasStone() || cost_tmp1.param->jewelry_param.HasZhuLing())
			{
				m_module_mgr->NoticeNum(errornum::EN_EQUIP_JEWELRY_UP_STAR_LEVEL_ERR);
				return;
			}
		}
		else if (cost_tmp1_ib->GetItemType() == I_TYPE_EQUIPMENT)
		{
			const Equipment * cost_tmp1_base = (const Equipment *)cost_tmp1_ib;
			if (cost_tmp1.param->equip_param.star_level + 1 != up_star_cfg->item_star)
			{
				m_module_mgr->NoticeNum(errornum::EN_STAR_UP_ERR_NOT_SAME_STAR_LEVEL);
				return;//同星级
			}

			if (cost_tmp1_ib->GetLimitLevel() != up_star_cfg->level_limit)
			{
				m_module_mgr->NoticeNum(errornum::EN_STAR_UP_ERR_NOT_SAME_LEVEL);
				return;//同等级
			}

			if (cost_tmp1_base->GetColor() != up_star_cfg->color_limit)
			{
				m_module_mgr->NoticeNum(errornum::EN_STAR_UP_ERR_NOT_SAME_QUALITY);
				return;//同品质
			}

			if (cost_tmp1.param->equip_param.HasStone() || cost_tmp1.param->equip_param.HasZhuLing())
			{
				m_module_mgr->NoticeNum(errornum::EN_EQUIP_JEWELRY_UP_STAR_LEVEL_ERR);
				return;
			}
		}
		else
		{
			m_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
			return;
		}
	}

	{//检查消耗道具2
		if (cost_tmp2_ib->GetItemType() == I_TYPE_JEWELRY)
		{
			const Jewelry * cost_tmp2_base = (const Jewelry *)cost_tmp2_ib;
			if (cost_tmp2.param->jewelry_param.star_level + 1 != up_star_cfg->item_star)
			{
				m_module_mgr->NoticeNum(errornum::EN_STAR_UP_ERR_NOT_SAME_STAR_LEVEL);
				return;//同星级
			}

			if (cost_tmp2_base->GetLimitLevel() != up_star_cfg->level_limit)
			{
				m_module_mgr->NoticeNum(errornum::EN_STAR_UP_ERR_NOT_SAME_LEVEL);
				return;//同等级
			}

			if (cost_tmp2_base->GetColor() != up_star_cfg->color_limit)
			{
				m_module_mgr->NoticeNum(errornum::EN_STAR_UP_ERR_NOT_SAME_QUALITY);
				return;//同品质
			}

			if (cost_tmp2.param->jewelry_param.HasStone() || cost_tmp2.param->jewelry_param.HasZhuLing())
			{
				m_module_mgr->NoticeNum(errornum::EN_EQUIP_JEWELRY_UP_STAR_LEVEL_ERR);
				return;
			}
		}
		else if (cost_tmp2_ib->GetItemType() == I_TYPE_EQUIPMENT)
		{
			const Jewelry * cost_tmp2_base = (const Jewelry *)cost_tmp2_ib;
			if (cost_tmp2.param->equip_param.star_level + 1 != up_star_cfg->item_star)
			{
				m_module_mgr->NoticeNum(errornum::EN_STAR_UP_ERR_NOT_SAME_STAR_LEVEL);
				return;//同星级
			}

			if (cost_tmp2_base->GetLimitLevel() != up_star_cfg->level_limit)
			{
				m_module_mgr->NoticeNum(errornum::EN_STAR_UP_ERR_NOT_SAME_LEVEL);
				return;//同等级
			}

			if (cost_tmp2_base->GetColor() != up_star_cfg->color_limit)
			{
				m_module_mgr->NoticeNum(errornum::EN_STAR_UP_ERR_NOT_SAME_QUALITY);
				return;//同品质
			}

			if (cost_tmp2.param->equip_param.HasStone() || cost_tmp2.param->equip_param.HasZhuLing())
			{
				m_module_mgr->NoticeNum(errornum::EN_EQUIP_JEWELRY_UP_STAR_LEVEL_ERR);
				return;
			}
		}
		else
		{
			m_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
			return;
		}
	}

	if (WORKSHOP_STONE_TYPE_JEWELRY == equip_type)
	{
		if (equip_ib->GetItemType() != I_TYPE_JEWELRY)return;
		JewelryParam &p = tmp_equip->param->jewelry_param;
		const EquipStarCfg * next_star_attr_cfg = LOGIC_CONFIG->GetEquipStarConfig()->GetEquipStarAttrCfg(up_star_cfg->compose_id, up_star_cfg->item_star);
		if (NULL == next_star_attr_cfg)return;
		const ItemBase * compose_i_b = ITEMPOOL->GetItem(up_star_cfg->compose_id);
		if (compose_i_b->GetItemType() != equip_ib->GetItemType())return;
		const Jewelry * CheckSuit_ib = (const Jewelry *)compose_i_b;
		const Jewelry * new_ib = (const Jewelry *)equip_ib;
		if (CheckSuit_ib->GetJewelryType() != new_ib->GetJewelryType())return;

		p.star_level = next_star_attr_cfg->item_star;
		p.attr_num = 0;
		memset(p.attr_list, 0, sizeof(p.attr_list));
		for (int attr_idx = 0; attr_idx < JewelryParam::ATTR_NUM_MAX; ++attr_idx)
		{
			if (p.attr_num >= JewelryParam::ATTR_NUM_MAX) break;

			const EquipStarAttr & attr_as = next_star_attr_cfg->attr_voc[attr_idx];
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

		if (NULL != CheckSuit_ib)
		{
			const SuitListGroup * suit_cfg = LOGIC_CONFIG->GetSuitListConfig()->GetSuitListGroup(CheckSuit_ib->GetSuitRandomGroup());
			if (suit_cfg != NULL)//重新确认套装
			{
				int suit_rad = rand() % suit_cfg->rate_weight;
				for (std::vector<SuitGroup>::const_iterator suit_it = suit_cfg->m_suit_group.begin(); suit_it != suit_cfg->m_suit_group.end(); suit_it++)
				{
					if (suit_it->rate > suit_rad)
					{
						p.new_suit_effect_id = suit_it->id;
						break;
					}
				}
			}
		}
	}
	else
	{
		if (equip_ib->GetItemType() != I_TYPE_EQUIPMENT)return;

		const ItemBase * compose_i_b = ITEMPOOL->GetItem(up_star_cfg->compose_id);
		if (compose_i_b->GetItemType() != equip_ib->GetItemType())return;
		const Equipment * CheckSuit_ib = (const Equipment *)compose_i_b;
		const Equipment * new_ib = (const Equipment *)equip_ib;
		if (CheckSuit_ib->GetEquipType() != new_ib->GetEquipType())return;

		EquipmentParam &p = tmp_equip->param->equip_param;
		const EquipStarCfg * next_star_attr_cfg = LOGIC_CONFIG->GetEquipStarConfig()->GetEquipStarAttrCfg(up_star_cfg->compose_id, up_star_cfg->item_star);
		if (NULL == next_star_attr_cfg)return;
		p.star_level = next_star_attr_cfg->item_star;
		p.attr_num = 0;
		memset(p.attr_list, 0, sizeof(p.attr_list));
		for (int attr_idx = 0; attr_idx < EquipmentParam::ATTR_NUM_MAX; ++attr_idx)
		{
			if (p.attr_num >= EquipmentParam::ATTR_NUM_MAX) break;

			const EquipStarAttr & attr_as = next_star_attr_cfg->attr_voc[attr_idx];
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
		if (NULL != CheckSuit_ib)
		{
			const SuitListGroup * suit_cfg = LOGIC_CONFIG->GetSuitListConfig()->GetSuitListGroup(CheckSuit_ib->GetSuitRandomGroup());
			if (suit_cfg != NULL)//重新确认套装
			{
				int suit_rad = rand() % suit_cfg->rate_weight;
				for (std::vector<SuitGroup>::const_iterator suit_it = suit_cfg->m_suit_group.begin(); suit_it != suit_cfg->m_suit_group.end(); suit_it++)
				{
					if (suit_it->rate > suit_rad)
					{
						p.new_suit_effect_id = suit_it->id;
						break;
					}
				}
			}
		}
	}

	bool ret1 = m_module_mgr->GetKnapsack()->ConsumeItemByIndex(ItemNamespace::ITEM_COLUMN_TYPE_ITEM, cost_grid, 1, __FUNCTION__);
	bool ret2 = m_module_mgr->GetKnapsack()->ConsumeItemByIndex(ItemNamespace::ITEM_COLUMN_TYPE_ITEM, cost_grid_2, 1, __FUNCTION__);
	if (!ret1 || !ret2)
	{
		//消耗失败记录
		gamelog::g_log_workshop.printf(LL_INFO, "%s Equipment user[%d,%s] ConsumeItemByIndex false ret1[%d] cost_grid_1[item_id(%d),cost_grid(%d)] ret2[%d] cost_grid_2[item_id(%d),cost_grid(%d)]",
			__FUNCTION__, m_module_mgr->GetUid(), m_module_mgr->GetRole()->GetName(), ret1 ? 1 : 0, cost_tmp1.item_id, cost_grid,
			ret2 ? 1 : 0, cost_tmp2.item_id, cost_grid_2);
	}

	tmp_equip->item_id = up_star_cfg->compose_id;

	m_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_EQUIPMENT, __FUNCTION__);
	if (WORKSHOP_STONE_TYPE_EQUIP == equip_type)
	{
		this->SetEquipGridDirty(equip_grid, true);
	}
	else
	{
		this->SetJewelryGridDirty(equip_grid, true);
	}

	if (equip_type)
	{
		if (equip_type < Equipment::E_TYPE_MIN || equip_type >= Equipment::E_TYPE_MAX) return;
		int equip_index = Equipment::E_TYPE_MAX - equip_type - 1;
		this->SynItemDataPut(I_TYPE_EQUIPMENT, equip_index, tmp_equip);
	}
	else
	{
		if (equip_type < Jewelry::J_TYPE_GEM_RING || equip_type >= Jewelry::J_TYPE_MAX) return;
		int equip_index = Jewelry::J_TYPE_MAX - equip_type - 1;
		this->SynItemDataPut(I_TYPE_JEWELRY, equip_index, tmp_equip);
	}
}

void EquipmentManager::UpEquipGrade(int equip_type, int equip_grid, int role_type)
{
	ItemGridData * tmp_equip = NULL;

	if (WORKSHOP_STONE_TYPE_EQUIP == equip_type)
	{
		if (equip_grid >= Equipment::E_INDEX_MIN && equip_grid < Equipment::E_INDEX_MAX)
		{
			tmp_equip = &m_equipment_list[equip_grid];
		}
	}
	else
	{
		if (equip_grid >= Jewelry::J_TYPE_GEM_RING && equip_grid < Jewelry::J_TYPE_MAX)
		{
			tmp_equip = &m_jewelry_list[equip_grid];
		}
	}

	if (NULL == tmp_equip || tmp_equip->Invalid() || NULL == tmp_equip->param)
	{
		m_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	const ItemBase * equip_ib = ITEMPOOL->GetItem(tmp_equip->item_id);
	if (NULL == equip_ib)
	{
		m_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	if (equip_ib->GetColor() < ItemBase::I_COLOR_STAR || equip_ib->GetColor() > ItemBase::I_COLOR_GOD)
	{
		return;
	}

	int real_equip_grid = equip_grid;
	if (WORKSHOP_STONE_TYPE_JEWELRY == equip_type)
	{
		real_equip_grid = Equipment::E_INDEX_MAX + equip_grid;
	}

	int stuff_count = 0; ItemExtern::ItemConsumeConfig stuff_list[ItemNamespace::MAX_BATCH_CONSUME_ITEM_NUM];

	AttrInfo *attr_info = NULL;
	short attr_num = 0;
	const StrengthenCfg * consume_cfg = NULL;
	if (WORKSHOP_STONE_TYPE_EQUIP == equip_type)
	{
		if (I_TYPE_EQUIPMENT != equip_ib->GetItemType())
		{
			m_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
			return;
		}
		Equipment * t_e = (Equipment *)equip_ib;
		if (1 == t_e->GetSpecialEquip())
		{
			m_module_mgr->NoticeNum(errornum::EN_WORKSHOP_CAN_USE_THIS_EQUIP);
			return;
		}
		EquipmentParam & p_ep = tmp_equip->param->equip_param;
		attr_info = p_ep.attr_list;
		attr_num = p_ep.attr_num;
		//检查进阶消耗
		consume_cfg = GODEQUIP_CONFIG->GetStrengthenCfg(equip_ib->GetItemId(), p_ep.strengthen_level + 1);
		if (consume_cfg == NULL)
		{
			m_module_mgr->NoticeNum(errornum::EN_GOD_UP_GREADE_NULL);
			return;
		}
		if (consume_cfg->need_rolelevel > m_module_mgr->GetRole()->GetLevel())
		{
			m_module_mgr->NoticeNum(errornum::EN_ITEM_LEVEL_LIMIT);
			return;
		}
		for (; stuff_count < (int)consume_cfg->consume_item_list.size() && stuff_count < ItemNamespace::MAX_BATCH_CONSUME_ITEM_NUM; ++stuff_count)
		{
			ItemExtern::ItemConsumeConfig & stuff_con = stuff_list[stuff_count];

			stuff_con.item_id = consume_cfg->consume_item_list[stuff_count].item_id;
			stuff_con.num = consume_cfg->consume_item_list[stuff_count].num;
			stuff_con.buyable = false;
		}
	}
	else if (WORKSHOP_STONE_TYPE_JEWELRY == equip_type)
	{
		if (I_TYPE_JEWELRY != equip_ib->GetItemType())
		{
			m_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
			return;
		}
		JewelryParam & p_ep = tmp_equip->param->jewelry_param;
		Jewelry * t_j = (Jewelry *)equip_ib;
		if (1 == t_j->GetSpecialEquip())
		{
			m_module_mgr->NoticeNum(errornum::EN_WORKSHOP_CAN_USE_THIS_EQUIP);
			return;
		}
		attr_info = p_ep.attr_list;
		attr_num = p_ep.attr_num;
		//检查进阶消耗
		consume_cfg = GODEQUIP_CONFIG->GetStrengthenCfg(equip_ib->GetItemId(), p_ep.strengthen_level + 1);
		if (consume_cfg == NULL)
		{
			m_module_mgr->NoticeNum(errornum::EN_GOD_UP_GREADE_NULL);
			return;
		}
		if (consume_cfg->need_rolelevel > m_module_mgr->GetRole()->GetLevel())
		{
			m_module_mgr->NoticeNum(errornum::EN_ITEM_LEVEL_LIMIT);
			return;
		}
		for (; stuff_count < (int)consume_cfg->consume_item_list.size() && stuff_count < ItemNamespace::MAX_BATCH_CONSUME_ITEM_NUM; ++stuff_count)
		{
			ItemExtern::ItemConsumeConfig & stuff_con = stuff_list[stuff_count];

			stuff_con.item_id = consume_cfg->consume_item_list[stuff_count].item_id;
			stuff_con.num = consume_cfg->consume_item_list[stuff_count].num;
			stuff_con.buyable = false;
		}
	}
	if (attr_info == NULL)return;
	if (consume_cfg == NULL)return;
	// 消耗，消耗完成后不允许返回
	if (stuff_count > 0)
	{
		static ItemExtern::ItemConsumeList consume_list; consume_list.Reset();
		if (!ItemExtern::GetConsumeListByOrder(m_module_mgr->GetRole(), false, stuff_count, stuff_list, &consume_list, false))
		{
			m_module_mgr->GetRole()->NoticeNum(errornum::EN_STUFF_NOT_ENOUGH);
			return;
		}

		if (consume_list.count > 0 && !m_module_mgr->GetKnapsack()->ConsumeItemByIndexList(consume_list.count, consume_list.column_list, consume_list.index_list, consume_list.num_list, __FUNCTION__)) return;
	}
	gamelog::g_log_workshop.printf(LL_INFO, "Workshop::UpEquipGrade user[%d,%s] weapon_id[%d] param: [%s]", m_module_mgr->GetUid(),
		m_module_mgr->GetRole()->GetName(), tmp_equip->item_id, tmp_equip->param->GetParamLog(equip_ib->GetItemType()));
	//覆盖进阶效果---直接覆盖属性效果
	if (WORKSHOP_STONE_TYPE_EQUIP == equip_type)
	{
		EquipmentParam & p_ep = tmp_equip->param->equip_param;
		p_ep.strengthen_level += 1;
		p_ep.attr_num = 0;
		for (int i = 0; i < EquipmentParam::ATTR_NUM_MAX; i++)
		{
			p_ep.attr_list[i].attr_type = consume_cfg->attr_type_list[i];
			p_ep.attr_list[i].attr_value = consume_cfg->attr_add_list[i];
			if (consume_cfg->attr_add_list[i] <= 0)continue;//值为0则词条为假不增加词条数量
			p_ep.attr_num++;
		}
		//重算熔炼词条数量(调高词条,新增为空词条,调低则保留之前的)
		Equipment *new_equip = (Equipment*)equip_ib;
		if (new_equip->IsNeedReCalSmeltEntryNum())
		{
			const GodEquipAttrNumConfig* max_num_cfg = LOGIC_CONFIG->GetEntryConfig()->GetGodEquipmentNumCfg(p_ep.strengthen_level, new_equip->GetEquipColor());
			if (max_num_cfg != NULL && max_num_cfg->max_num <= EquipmentParam::EXA_ATTR_NUM_MAX)p_ep.inc_num = p_ep.inc_num >= max_num_cfg->max_num ? p_ep.inc_num : max_num_cfg->max_num;
		}
		for (int voc_idx = 0; voc_idx < p_ep.inc_num && voc_idx < EquipmentParam::EXA_ATTR_NUM_MAX; voc_idx++)
		{
			//p_ep.attr_extra_inc[voc_idx].un_lock = 1;
		}
	}
	else if (WORKSHOP_STONE_TYPE_JEWELRY == equip_type)
	{
		JewelryParam & p_ep = tmp_equip->param->jewelry_param;
		p_ep.strengthen_level += 1;
		p_ep.attr_num = 0;
		for (int i = 0; i < JewelryParam::ATTR_NUM_MAX; i++)
		{
			p_ep.attr_list[i].attr_type = consume_cfg->attr_type_list[i];
			p_ep.attr_list[i].attr_value = consume_cfg->attr_add_list[i];
			if (consume_cfg->attr_add_list[i] <= 0)continue;//值为0则词条为假不增加词条数量
			p_ep.attr_num++;
		}
		//重算熔炼词条数量(调高词条,新增为空词条,调低则保留之前的)
		Jewelry *new_equip = (Jewelry*)equip_ib;
		if (new_equip->IsNeedReCalSmeltEntryNum())
		{
			const GodEquipAttrNumConfig* max_num_cfg = LOGIC_CONFIG->GetEntryConfig()->GetGodEquipmentNumCfg(p_ep.strengthen_level, new_equip->GetEquipColor());
			if (max_num_cfg != NULL && max_num_cfg->max_num <= EquipmentParam::EXA_ATTR_NUM_MAX)p_ep.inc_num = p_ep.inc_num >= max_num_cfg->max_num ? p_ep.inc_num : max_num_cfg->max_num;
		}
		for (int voc_idx = 0; voc_idx < p_ep.inc_num && voc_idx < EquipmentParam::EXA_ATTR_NUM_MAX; voc_idx++)
		{
			//p_ep.attr_extra_inc[voc_idx].un_lock = 1;
		}
	}

	// 重算属性、发送信息
	m_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_EQUIPMENT, __FUNCTION__);
	if (WORKSHOP_STONE_TYPE_EQUIP == equip_type)
	{
		this->SetEquipGridDirty(equip_grid, true, role_type);
	}
	else
	{
		this->SetJewelryGridDirty(equip_grid, true, role_type);
	}

	if (equip_type)
	{
		if (equip_type < Equipment::E_TYPE_MIN || equip_type >= Equipment::E_TYPE_MAX) return;
		int equip_index = Equipment::E_TYPE_MAX - equip_type - 1;
		this->SynItemDataPut(I_TYPE_EQUIPMENT, equip_index, tmp_equip, role_type);
	}
	else
	{
		if (equip_type < Jewelry::J_TYPE_GEM_RING || equip_type >= Jewelry::J_TYPE_MAX) return;
		int equip_index = Jewelry::J_TYPE_MAX - equip_type - 1;
		this->SynItemDataPut(I_TYPE_JEWELRY, equip_index, tmp_equip, role_type);
	}

	return;
}

void EquipmentManager::BreakThroughGarde(int equip_type, int equip_grid, int role_type)
{
	ItemGridData * tmp_equip = NULL;

	if (WORKSHOP_STONE_TYPE_EQUIP == equip_type)
	{
		if (equip_grid >= Equipment::E_INDEX_MIN && equip_grid < Equipment::E_INDEX_MAX)
		{
			tmp_equip = &m_equipment_list[equip_grid];
		}
	}
	else
	{
		if (equip_grid >= 0 && equip_grid < Jewelry::J_TYPE_MAX)
		{
			tmp_equip = &m_jewelry_list[equip_grid];
		}
	}
	if (NULL == tmp_equip || tmp_equip->Invalid() || NULL == tmp_equip->param)return;

	const ItemBase * equip_ib = ITEMPOOL->GetItem(tmp_equip->item_id);
	if (NULL == equip_ib)
	{
		m_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	if (equip_ib->GetColor() < ItemBase::I_COLOR_STAR || equip_ib->GetColor() > ItemBase::I_COLOR_GOD)
	{
		return;
	}

	int real_equip_grid = equip_grid;
	if (WORKSHOP_STONE_TYPE_JEWELRY == equip_type)
	{
		real_equip_grid = Equipment::E_INDEX_MAX + equip_grid;
	}

	int stuff_count = 0; ItemExtern::ItemConsumeConfig stuff_list[ItemNamespace::MAX_BATCH_CONSUME_ITEM_NUM];

	AttrInfo *attr_info = NULL;
	short attr_num = 0;
	const StrengthenCfg * consume_cfg = NULL;
	const StrengthenCfg * new_item_cfg = NULL;
	if (WORKSHOP_STONE_TYPE_EQUIP == equip_type)
	{
		if (I_TYPE_EQUIPMENT != equip_ib->GetItemType())
		{
			m_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
			return;
		}
		Equipment * t_e = (Equipment *)equip_ib;
		if (1 == t_e->GetSpecialEquip())
		{
			m_module_mgr->NoticeNum(errornum::EN_WORKSHOP_CAN_USE_THIS_EQUIP);
			return;
		}
		EquipmentParam & p_ep = tmp_equip->param->equip_param;
		attr_info = p_ep.attr_list;
		attr_num = p_ep.attr_num;
		//检查进阶消耗
		consume_cfg = GODEQUIP_CONFIG->GetStrengthenCfg(equip_ib->GetItemId(), p_ep.strengthen_level);
		if (consume_cfg == NULL)
		{
			m_module_mgr->NoticeNum(errornum::EN_GOD_STRONGTHEN_NULL);
			return;
		}
		new_item_cfg = GODEQUIP_CONFIG->GetStrengthenCfg(consume_cfg->strengthen_id, p_ep.strengthen_level);
		if (new_item_cfg == NULL)
		{
			m_module_mgr->NoticeNum(errornum::EN_GOD_STRONGTHEN_NULL);
			return;
		}
		if (consume_cfg->need_rolelevel > m_module_mgr->GetRole()->GetLevel())
		{
			m_module_mgr->NoticeNum(errornum::EN_ITEM_LEVEL_LIMIT);
			return;
		}
		for (; stuff_count < (int)consume_cfg->consume_item_list_2.size() && stuff_count < ItemNamespace::MAX_BATCH_CONSUME_ITEM_NUM; ++stuff_count)
		{
			ItemExtern::ItemConsumeConfig & stuff_con = stuff_list[stuff_count];

			stuff_con.item_id = consume_cfg->consume_item_list_2[stuff_count].item_id;
			stuff_con.num = consume_cfg->consume_item_list_2[stuff_count].num;
			stuff_con.buyable = false;
		}
	}
	else if (WORKSHOP_STONE_TYPE_JEWELRY == equip_type)
	{
		if (I_TYPE_JEWELRY != equip_ib->GetItemType())
		{
			m_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
			return;
		}
		JewelryParam & p_ep = tmp_equip->param->jewelry_param;
		Jewelry * t_j = (Jewelry *)equip_ib;
		if (1 == t_j->GetSpecialEquip())
		{
			m_module_mgr->NoticeNum(errornum::EN_WORKSHOP_CAN_USE_THIS_EQUIP);
			return;
		}
		attr_info = p_ep.attr_list;
		attr_num = p_ep.attr_num;
		//检查进阶消耗
		consume_cfg = GODEQUIP_CONFIG->GetStrengthenCfg(equip_ib->GetItemId(), p_ep.strengthen_level);
		if (consume_cfg == NULL)
		{
			m_module_mgr->NoticeNum(errornum::EN_GOD_STRONGTHEN_NULL);
			return;
		}
		new_item_cfg = GODEQUIP_CONFIG->GetStrengthenCfg(consume_cfg->strengthen_id, p_ep.strengthen_level);
		if (new_item_cfg == NULL)
		{
			m_module_mgr->NoticeNum(errornum::EN_GOD_STRONGTHEN_NULL);
			return;
		}
		if (consume_cfg->need_rolelevel > m_module_mgr->GetRole()->GetLevel())
		{
			m_module_mgr->NoticeNum(errornum::EN_ITEM_LEVEL_LIMIT);
			return;
		}
		for (; stuff_count < (int)consume_cfg->consume_item_list_2.size() && stuff_count < ItemNamespace::MAX_BATCH_CONSUME_ITEM_NUM; ++stuff_count)
		{
			ItemExtern::ItemConsumeConfig & stuff_con = stuff_list[stuff_count];

			stuff_con.item_id = consume_cfg->consume_item_list_2[stuff_count].item_id;
			stuff_con.num = consume_cfg->consume_item_list_2[stuff_count].num;
			stuff_con.buyable = false;
		}
	}
	if (consume_cfg == NULL)return;
	if (NULL == new_item_cfg)return;
	if (attr_info == NULL)return;
	const ItemBase * new_item = ITEMPOOL->GetItem(consume_cfg->strengthen_id);
	if (new_item == NULL)return;
	if (new_item->GetItemType() != equip_ib->GetItemType())
	{
		return;
	}
	// 消耗，消耗完成后不允许返回
	if (stuff_count > 0)
	{
		static ItemExtern::ItemConsumeList consume_list; consume_list.Reset();
		if (!ItemExtern::GetConsumeListByOrder(m_module_mgr->GetRole(), false, stuff_count, stuff_list, &consume_list, false))
		{
			m_module_mgr->GetRole()->NoticeNum(errornum::EN_STUFF_NOT_ENOUGH);
			return;
		}

		if (consume_list.count > 0 && !m_module_mgr->GetKnapsack()->ConsumeItemByIndexList(consume_list.count, consume_list.column_list, consume_list.index_list, consume_list.num_list, __FUNCTION__)) return;
	}

	//覆盖进阶效果---直接覆盖属性效果
	int strength_level = 0;
	if (WORKSHOP_STONE_TYPE_EQUIP == equip_type)
	{
		tmp_equip->item_id = consume_cfg->strengthen_id;
		EquipmentParam & p_ep = tmp_equip->param->equip_param;
		strength_level = p_ep.strengthen_level;
		p_ep.attr_num = 0;
		for (int i = 0; i < EquipmentParam::ATTR_NUM_MAX; i++)
		{
			p_ep.attr_list[i].attr_type = new_item_cfg->attr_type_list[i];
			p_ep.attr_list[i].attr_value = new_item_cfg->attr_add_list[i];
			if (new_item_cfg->attr_add_list[i] <= 0)continue;//值为0则词条为假不增加词条数量
			p_ep.attr_num++;
		}
		p_ep.new_suit_effect_id = new_item_cfg->suit_id;
		//重算熔炼词条数量(调高词条,新增为空词条,调低则保留之前的)
		Equipment *new_equip = (Equipment*)new_item;
		if (new_equip->IsNeedReCalSmeltEntryNum())
		{
			const GodEquipAttrNumConfig* max_num_cfg = LOGIC_CONFIG->GetEntryConfig()->GetGodEquipmentNumCfg(p_ep.strengthen_level, new_equip->GetEquipColor());
			if (max_num_cfg != NULL && max_num_cfg->max_num <= EquipmentParam::EXA_ATTR_NUM_MAX)p_ep.inc_num = p_ep.inc_num >= max_num_cfg->max_num ? p_ep.inc_num : max_num_cfg->max_num;
		}
		for (int voc_idx = 0; voc_idx < p_ep.inc_num && voc_idx < EquipmentParam::EXA_ATTR_NUM_MAX; voc_idx++)
		{
			//p_ep.attr_extra_inc[voc_idx].un_lock = 1;
		}
	}
	else if (WORKSHOP_STONE_TYPE_JEWELRY == equip_type)
	{
		tmp_equip->item_id = consume_cfg->strengthen_id;
		JewelryParam & p_ep = tmp_equip->param->jewelry_param;
		strength_level = p_ep.strengthen_level;
		p_ep.attr_num = 0;
		for (int i = 0; i < JewelryParam::ATTR_NUM_MAX; i++)
		{
			p_ep.attr_list[i].attr_type = new_item_cfg->attr_type_list[i];
			p_ep.attr_list[i].attr_value = new_item_cfg->attr_add_list[i];
			if (new_item_cfg->attr_add_list[i] <= 0)continue;//值为0则词条为假不增加词条数量
			p_ep.attr_num++;
		}
		p_ep.new_suit_effect_id = new_item_cfg->suit_id;
		//重算熔炼词条数量(调高词条,新增为空词条,调低则保留之前的)
		Jewelry *new_equip = (Jewelry*)new_item;
		if (new_equip->IsNeedReCalSmeltEntryNum())
		{
			const GodEquipAttrNumConfig* max_num_cfg = LOGIC_CONFIG->GetEntryConfig()->GetGodEquipmentNumCfg(p_ep.strengthen_level, new_equip->GetEquipColor());
			if (max_num_cfg != NULL)p_ep.inc_num = p_ep.inc_num >= max_num_cfg->max_num ? p_ep.inc_num : max_num_cfg->max_num;
		}
		for (int voc_idx = 0; voc_idx < p_ep.inc_num && voc_idx < EquipmentParam::EXA_ATTR_NUM_MAX; voc_idx++)
		{
			//p_ep.attr_extra_inc[voc_idx].un_lock = 1;
		}
	}

	int sign_id1 = 0;
	int sign_id2 = 0;
	ItemDataWrapper new_data;
	tmp_equip->param->GetData(new_data.param_data);
	new_data.item_id = tmp_equip->item_id;
	new_data.num = tmp_equip->num;
	new_data.is_bind = tmp_equip->is_bind;
	new_data.item_unique_id = tmp_equip->item_unique_id;
	ChatManager::Instance().AddChatItemInfo(&sign_id1, &sign_id2, new_data);

	int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_god_equip_break_through_broadcast,
		m_module_mgr->GetRole()->GetName(), tmp_equip->item_id, sign_id1, sign_id2, strength_level);
	if (length > 0)
	{
		Protocol::SCSystemMsg sm;
		int sendlen = 0;
		if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_SYSTEM_CHAT)))
		{
			World::GetInstWorld()->SendToAllGateway((const char*)&sm, sendlen);
		}
	}

	gamelog::g_log_workshop.printf(LL_INFO, "Workshop::BreakThroughGarde user[%d,%s] weapon_id[%d] param: [%s]", m_module_mgr->GetUid(),
		m_module_mgr->GetRole()->GetName(), tmp_equip->item_id, tmp_equip->param->GetParamLog(new_item->GetItemType()));
	// 重算属性、发送信息
	m_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_EQUIPMENT, __FUNCTION__);
	if (WORKSHOP_STONE_TYPE_EQUIP == equip_type)
	{
		this->SetEquipGridDirty(equip_grid, true, role_type);
	}
	else
	{
		this->SetJewelryGridDirty(equip_grid, true, role_type);
	}

	if (equip_type)
	{
		if (equip_type < Equipment::E_TYPE_MIN || equip_type >= Equipment::E_TYPE_MAX) return;
		int equip_index = Equipment::E_TYPE_MAX - equip_type - 1;
		m_module_mgr->GetEquipmentManager()->SynItemDataPut(I_TYPE_EQUIPMENT, equip_index, tmp_equip, role_type);
	}
	else
	{
		if (equip_type < Jewelry::J_TYPE_GEM_RING || equip_type >= Jewelry::J_TYPE_MAX) return;
		int equip_index = Jewelry::J_TYPE_MAX - equip_type - 1;
		m_module_mgr->GetEquipmentManager()->SynItemDataPut(I_TYPE_JEWELRY, equip_index, tmp_equip, role_type);
	}
}

void EquipmentManager::DianHuaEquip(int equip_grid, int dianhuashi_coloumn, int dianhuashi_grid)
{
	const ItemGridData * tmp_equip = NULL;
	if (equip_grid < Equipment::E_INDEX_MIN || equip_grid >= Equipment::E_INDEX_MAX)return;
	
	tmp_equip = &m_equipment_list[equip_grid];

	if (tmp_equip == NULL || tmp_equip->Invalid() || NULL == tmp_equip->param)return;

	int *effect_id = NULL;
	int *effect_level = NULL;
	int *refresh_effect_id = NULL;
	int *refresh_effect_level = NULL;
	EquipmentParam & equip_param = tmp_equip->param->equip_param;
	effect_id = &equip_param.effect_id;
	effect_level = &equip_param.effect_level;
	refresh_effect_id = &equip_param.refresh_effect_id;
	refresh_effect_level = &equip_param.refresh_effect_level;

	ItemGridData dianhuashi_item;
	dianhuashi_item.Reset();
	if (!m_module_mgr->GetKnapsack()->GetItemGridData(dianhuashi_coloumn, dianhuashi_grid, &dianhuashi_item))
	{
		return;
	}

	const ItemBase * equip_ib = ITEMPOOL->GetItem(tmp_equip->item_id);
	const ItemBase * dianhuashi_ib = ITEMPOOL->GetItem(dianhuashi_item.item_id);
	if (NULL == equip_ib || NULL == dianhuashi_ib)return;

	if (dianhuashi_ib->GetItemType() != I_TYPE_DIAN_HUA_SHI || equip_ib->GetItemType() != I_TYPE_EQUIPMENT)
	{
		return;
	}

	DianHuaShiParam & dian_hua_shi_param = dianhuashi_item.param->dian_hua_shi_param;

	*refresh_effect_id = dian_hua_shi_param.effect_id;
	*refresh_effect_level = dian_hua_shi_param.effect_level;

	//消耗点化石
	m_module_mgr->GetKnapsack()->ConsumeItemByIndex(dianhuashi_coloumn, dianhuashi_grid, 1, __FUNCTION__);
	m_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_EQUIPMENT, __FUNCTION__);
	this->SetEquipGridDirty(equip_grid, true);
}

void EquipmentManager::SaveDianHuaEquip(int equip_grid)
{
	ItemGridData * tmp_equip = NULL;
	if (equip_grid < Equipment::E_INDEX_MIN || equip_grid >= Equipment::E_INDEX_MAX)return;

	tmp_equip = &m_equipment_list[equip_grid];

	if (tmp_equip == NULL || tmp_equip->Invalid() || NULL == tmp_equip->param)return;

	const ItemBase * ib = ITEMPOOL->GetItem(tmp_equip->item_id);
	if (NULL == ib || ib->GetItemType() != I_TYPE_EQUIPMENT)
	{
		m_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	if (tmp_equip->param->equip_param.refresh_effect_id <= 0 || tmp_equip->param->equip_param.refresh_effect_level <= 0)
	{
		gamelog::g_log_workshop.printf(LL_INFO, "Workshop::SaveDianHuaEquip Err user[%d,%s] err_effect_id[%d] err_effect_level[%d]", m_module_mgr->GetRoleUID(), m_module_mgr->GetRoleName(),
			tmp_equip->param->equip_param.refresh_effect_id, tmp_equip->param->equip_param.refresh_effect_level);
		return;
	}

	tmp_equip->param->equip_param.effect_id = tmp_equip->param->equip_param.refresh_effect_id;
	tmp_equip->param->equip_param.effect_level = tmp_equip->param->equip_param.refresh_effect_level;

	tmp_equip->param->equip_param.refresh_effect_id = 0;
	tmp_equip->param->equip_param.refresh_effect_level = 0;

	m_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_EQUIPMENT, __FUNCTION__);
	this->SetEquipGridDirty(equip_grid, true);
}

void EquipmentManager::XiFuEquip(int equip_grid)
{
	ItemGridData * tmp_equip = NULL;
	if (equip_grid < Equipment::E_INDEX_MIN || equip_grid >= Equipment::E_INDEX_MAX)return;

	tmp_equip = &m_equipment_list[equip_grid];

	if (tmp_equip == NULL || tmp_equip->Invalid() || NULL == tmp_equip->param)return;

	const ItemBase * ib = ITEMPOOL->GetItem(tmp_equip->item_id);
	if (NULL == ib || ib->GetItemType() != I_TYPE_EQUIPMENT || NULL == tmp_equip->param)
	{
		m_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	if (tmp_equip->param->equip_param.effect_id <= 0 || tmp_equip->param->equip_param.effect_level <= 0)
	{
		gamelog::g_log_workshop.printf(LL_INFO, "Workshop::XiFuEquip Err user[%d,%s] err_effect_id[%d] err_effect_level[%d]", m_module_mgr->GetRoleUID(), m_module_mgr->GetRoleName()
			, tmp_equip->param->equip_param.effect_id, tmp_equip->param->equip_param.effect_level);
		return;
	}

	//消耗吸附石
	const DianHuaShiOtherCfg & other_cfg = LOGIC_CONFIG->GetDianHuaShiConfig()->GetOtherCfg();
	const DianHuaSpecialEffectDetailCfg * effect_cfg = LOGIC_CONFIG->GetDianHuaShiConfig()->GetEffectDetailCfg(tmp_equip->param->equip_param.effect_id, tmp_equip->param->equip_param.effect_level);
	if (effect_cfg == NULL || effect_cfg->xifu_consume <= 0)return;

	if (!m_module_mgr->GetKnapsack()->ConsumeItem(other_cfg.xifu_id, effect_cfg->xifu_consume, __FUNCTION__))
	{
		m_module_mgr->NoticeNum(errornum::EN_ITEM_AMOUNT_NOT_ENOUGH);
		return;
	}

	{
		const ItemBase * ib = ITEMPOOL->GetItem(other_cfg.dianhua_id);
		if (NULL == ib || ib->GetItemType() != I_TYPE_DIAN_HUA_SHI)
		{
			m_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
			return;
		}
	}

	ItemGridData new_item;
	new_item.Reset();
	new_item.item_id = other_cfg.dianhua_id;
	new_item.num = 1;

	NetValueItemParam new_param;
	new_param.dian_hua_shi_param.has_net_value = 1;
	new_param.dian_hua_shi_param.effect_id = tmp_equip->param->equip_param.effect_id;
	new_param.dian_hua_shi_param.effect_level = tmp_equip->param->equip_param.effect_level;
	new_item.param = &new_param;
	if (new_item.param == NULL)return;
	tmp_equip->param->equip_param.effect_id = 0;
	tmp_equip->param->equip_param.effect_level = 0;
	long long item_unique_id = 0;
	UniqueIDGenerator::Instance()->GenUniqueID(&item_unique_id);
	ItemDataWrapper item_wrapper(new_item.item_id, 1, (short)new_item.is_bind, 1,
		new_item.param->data, new_item.invalid_time, 0, item_unique_id, new_item.is_market_buy_flag);

	if (!m_module_mgr->GetKnapsack()->Put(item_wrapper, PUT_REASON_DIAN_HUA_SHI))
	{
		m_module_mgr->NoticeNum(errornum::EN_KNAPSACK_ITEM_COLUMN_NO_SPACE);
		return;
	}

	m_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_EQUIPMENT, __FUNCTION__);
	this->SetEquipGridDirty(equip_grid, true);
}

void EquipmentManager::FuMoEquip(int equip_grid, int * new_spid, int * new_level)
{
	ItemGridData * tmp_equip = NULL;
	if (equip_grid < Equipment::E_INDEX_MIN || equip_grid >= Equipment::E_INDEX_MAX)return;

	tmp_equip = &m_equipment_list[equip_grid];

	if (tmp_equip == NULL || tmp_equip->Invalid() || NULL == tmp_equip->param)return;

	const ItemBase * ib = ITEMPOOL->GetItem(tmp_equip->item_id);
	if (NULL == ib || ib->GetItemType() != I_TYPE_EQUIPMENT)
	{
		m_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	//消耗附魔石
	const DianHuaShiOtherCfg & other_cfg = LOGIC_CONFIG->GetDianHuaShiConfig()->GetOtherCfg();
	if (!m_module_mgr->GetKnapsack()->ConsumeItem(other_cfg.fumo_id, 1, __FUNCTION__))
	{
		m_module_mgr->NoticeNum(errornum::EN_ITEM_AMOUNT_NOT_ENOUGH);
		return;
	}
	EquipmentParam & equip_param = tmp_equip->param->equip_param;
	const DianHuaEffectsProbabilityCfg * effect_id_cfg = LOGIC_CONFIG->GetDianHuaShiConfig()->RandEffectProCfg();
	if (NULL == effect_id_cfg)return;
	const DianHuaLevelProbabilityCfg * effect_level_cfg = LOGIC_CONFIG->GetDianHuaShiConfig()->RandEffectLevelProCfg(equip_param.effect_level);
	if (NULL == effect_level_cfg)return;

	equip_param.refresh_effect_id = effect_id_cfg->seq;
	equip_param.refresh_effect_level = effect_level_cfg->new_effect_level;
	if (new_spid != NULL)*new_spid = equip_param.refresh_effect_id;
	if (new_level != NULL)*new_level = equip_param.refresh_effect_level;

	m_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_EQUIPMENT, __FUNCTION__);
	this->SetEquipGridDirty(equip_grid, true);
}

void EquipmentManager::SetJewelryGridDirty(short jewelry_type, bool notify, short role_type, bool is_wear)
{
	if (jewelry_type < 0 || jewelry_type >= Jewelry::J_TYPE_MAX || role_type < 0 || role_type >= ROLE_TYPE_MAX) return;

	if (role_type == ROLE_TYPE_BENZUN)
	{
		m_j_dirty_mark[jewelry_type] = true;
	}
	else
	{
		m_huashen_j_dirty_mark[jewelry_type] = true;
	}
	m_jewelry_is_change = true;

	if (notify)
	{
		if (ROLE_TYPE_BENZUN == role_type)
		{
			this->SendSingleJewelryInfo(jewelry_type, is_wear);
		}
		else
		{
			this->SendHuaShenSingleJewelryInfo(jewelry_type);
		}
	}
}

void EquipmentManager::ClearJewelryDirty()
{
	if (!m_jewelry_is_change) return;

	for (int i = 0; i < Jewelry::J_TYPE_MAX; ++i)
	{
		if (m_j_dirty_mark[i]) m_j_old_state[i] = !m_jewelry_list[i].Invalid();
		if (m_huashen_j_dirty_mark[i]) m_huashen_j_old_state[i] = !m_huashen_jewelry_list[i].Invalid();
	}

	memset(m_j_dirty_mark, 0, sizeof(m_j_dirty_mark));
	memset(m_huashen_j_dirty_mark, 0, sizeof(m_huashen_j_dirty_mark));

	m_jewelry_is_change = false;
}

bool EquipmentManager::HuaShenWearJewelry(short grid_index)
{
	static ItemGridData grid_data; grid_data.Reset();
	if (!m_module_mgr->GetHuaShenManager()->GetItemGridData(grid_index, &grid_data))
	{
		return false;
	}

	const Jewelry * jewelry = (const Jewelry *)ITEMPOOL->GetItem(grid_data.item_id);
	if (NULL == jewelry || I_TYPE_JEWELRY != jewelry->GetItemType() || NULL == grid_data.param) return false;

	const short j_type = jewelry->GetJewelryType();
	if (j_type >= Jewelry::J_TYPE_MAX || j_type < 0) return false;

	//化身穿戴等级限制使用本尊
	if (m_module_mgr->GetRole()->GetLevel() < jewelry->GetLimitLevel())
	{
		m_module_mgr->NoticeNum(errornum::EN_EQUIPMENT_PUT_LEVEL_LIMIT);
		return false;
	}

	int take_off_index = -1;
	bool need_zhu_ling_notice = false; //没什么用，客户端需要
	if (NULL != m_huashen_jewelry_list[j_type].param)
	{
		need_zhu_ling_notice = m_huashen_jewelry_list[j_type].param->jewelry_param.HasZhuLing() && grid_data.param->jewelry_param.HasZhuLing();//没什么用，客户端需要
	}

	if (!m_huashen_jewelry_list[j_type].Invalid())
	{
		bool ret = this->HuaShenTakeOffJewelry(j_type, false, &take_off_index);
		if (!ret)
		{
			return ret;
		}
	}

	NetValueItemParam * param = jewelry->CreateParamFromStruct(*grid_data.param);
	if (NULL == param) return false;

	ItemGridData & p_igd = m_huashen_jewelry_list[j_type];
	p_igd.item_id = grid_data.item_id;
	p_igd.param = param;
	p_igd.num = 1;
	p_igd.is_bind = true;
	p_igd.invalid_time = grid_data.invalid_time;
	p_igd.item_unique_id = grid_data.item_unique_id;
	p_igd.is_market_buy_flag = grid_data.is_market_buy_flag;

	bool has_notic = false;
	Protocol::SCZhuLingPutOnSuccNotice zhu_ling_notice;
	zhu_ling_notice.p1 = 0;
	const ItemGridData * temp = m_module_mgr->GetHuaShenManager()->GetItemGridData2(take_off_index);
	if (NULL != temp)
	{
		const Equipment * temp_info = (const Equipment *)ITEMPOOL->GetItem(temp->item_id);
		if (temp_info != NULL && I_TYPE_JEWELRY == temp_info->GetItemType())
		{
			//获得脱下来那个装备，如果正要穿上的装备没有铸灵，正要替换铸灵
			if (!p_igd.param->jewelry_param.HasZhuLing() && temp->param->jewelry_param.HasZhuLing())
			{
				need_zhu_ling_notice = true;
				zhu_ling_notice.p1 = 1;
				for (int i = 0; i < EQUIP_ZHU_LING_SLOT_NUM; ++i)
				{
					p_igd.param->jewelry_param.zhu_ling_info[i] = temp->param->jewelry_param.zhu_ling_info[i];
					temp->param->jewelry_param.zhu_ling_info[i].Reset();
				}
			}

			//检查准备要穿戴的装备宝石数量，有空位则转移一颗宝石
			int vacancy_idx = p_igd.param->jewelry_param.HasVacancyStone();//有空位的宝石位置
			const ItemGridData & stone_data = *temp;
			int round = 0;//避免死循环

			int t_cur = 0;
			for (; t_cur < NEW_EQUIP_STONE_MOUNT_MAX; ++t_cur)//检测开启的孔数	//此时要以穿戴的这件装备能接受的孔位,防止一些BUG
			{
				bool is_godequip = jewelry->GetColor() >= ItemBase::I_COLOR_RED ? true : false;
				int equip_level = true == is_godequip ? p_igd.param->jewelry_param.strengthen_level : jewelry->GetLimitLevel();
				if (!LOGIC_CONFIG->GetGemstoneConfig()->CheckGemMountSlotOpen(t_cur, equip_level, is_godequip))break;
			}

			while (vacancy_idx < t_cur && stone_data.param->jewelry_param.HasStone() && round < 50)
			{
				round += 1;
				ItemID top_stone_id = 0;
				int top_level = 0;
				int top_stone_idx = t_cur;
				for (int stone_idx = 0; stone_idx < t_cur && stone_idx < ARRAY_LENGTH(stone_data.param->jewelry_param.new_stone); stone_idx++)
				{
					if (stone_data.param->jewelry_param.new_stone[stone_idx] <= 0) continue;
					const GemstoneBasicInfoCfg * gbic = LOGIC_CONFIG->GetGemstoneConfig()->GetGemstoneBasicInfoCfg(stone_data.param->jewelry_param.new_stone[stone_idx]);
					if (gbic != NULL)
					{
						if (gbic->level > top_level)
						{
							top_stone_id = stone_data.param->jewelry_param.new_stone[stone_idx];
							top_level = gbic->level;
							top_stone_idx = stone_idx;
						}
					}
				}
				if (top_stone_idx >= 0 && top_stone_idx < t_cur && top_stone_idx < ARRAY_LENGTH(p_igd.param->jewelry_param.new_stone))
				{
					//替换宝石
					p_igd.param->jewelry_param.new_stone[vacancy_idx] = top_stone_id;
					//消除宝石
					stone_data.param->jewelry_param.new_stone[top_stone_idx] = 0;
					//if (!has_notic)m_module_mgr->NoticeNum(noticenum::NT_EQUIP_STONE_TRANF_SUCC);
					has_notic = true;
				}
				vacancy_idx = p_igd.param->jewelry_param.HasVacancyStone();
			}

			m_module_mgr->GetHuaShenManager()->SetItemDirty(take_off_index, true, Knapsack::ITEM_CHANGE_NOTICE_REASON_DATA_CHANGE);
		}
	}

	if (need_zhu_ling_notice)
	{
		//	m_module_mgr->NetSend(&zhu_ling_notice, sizeof(zhu_ling_notice));
	}

	/*const EquipmentScoreCfg * score_cfg = LOGIC_CONFIG->GetScoreSystemConfig()->GetEquipmentScoreCfg(p_igd.item_id);
	if (score_cfg)
	{
		JewelryParam & data = param->jewelry_param;
		if (data.is_treasure == ITEM_SCORE_LEVEL_S)
		{
			p_igd.is_bind = false;
		}
	}*/

	if (!m_module_mgr->GetHuaShenManager()->Consume(grid_index, true))
	{
		return false;
	}

	m_module_mgr->GetHuaShenManager()->PutOn(j_type, p_igd.item_unique_id, HUASHEN_GRID_INDEX_TYPE_JEWELRY);
	this->SetJewelryGridDirty(j_type, true, ROLE_TYPE_HUASHEN);
	m_module_mgr->GetHuaShenManager()->ReCalcAttr(AttributeList::RECALC_REASON_EQUIPMENT, __FUNCTION__);

	return true;
}

bool EquipmentManager::HuaShenTakeOffJewelry(short jewelry_type, bool need_recalc, int * take_off_index)
{
	if (jewelry_type < 0 || jewelry_type >= Jewelry::J_TYPE_MAX || m_huashen_jewelry_list[jewelry_type].Invalid()) return false;

	ItemGridData & p_igd = m_huashen_jewelry_list[jewelry_type];
	const Jewelry * jewelry = (const Jewelry *)ITEMPOOL->GetItem(p_igd.item_id);
	if (NULL == jewelry || I_TYPE_JEWELRY != jewelry->GetItemType()) return false;

	ItemDataWrapper item_wrapper;
	p_igd.WrapTo(&item_wrapper);
	if (!m_module_mgr->GetHuaShenManager()->Put(item_wrapper, true, take_off_index))
	{
		return false;
	}

	p_igd.Clear();
	this->SetJewelryGridDirty(jewelry_type, need_recalc, ROLE_TYPE_HUASHEN);
	if (need_recalc) m_module_mgr->GetHuaShenManager()->ReCalcAttr(AttributeList::RECALC_REASON_EQUIPMENT, __FUNCTION__);
	return true;
}

bool EquipmentManager::HuaShenJewelryUpdate(short jewelry_type, ItemDataWrapper * item_data)
{
	if (NULL == item_data || jewelry_type < 0 || jewelry_type >= Jewelry::J_TYPE_MAX || item_data->has_param == 0) return false;

	//防止出现BUG导致服务器挂了
	if (NULL == m_huashen_jewelry_list[jewelry_type].param)
	{
		m_huashen_jewelry_list[jewelry_type].param = ItemBase::CreateParam();
	}

	m_huashen_jewelry_list[jewelry_type].param->SetData(item_data->param_data);
	m_huashen_jewelry_list[jewelry_type].is_bind = (0 != item_data->is_bind);
	if (m_huashen_jewelry_list[jewelry_type].item_id != item_data->item_id)
	{
		m_huashen_jewelry_list[jewelry_type].item_id = item_data->item_id;
	}
	this->SetJewelryGridDirty(jewelry_type, true, ROLE_TYPE_HUASHEN);
	return true;
}

bool EquipmentManager::HuaShenJewelryReset(short jewelry_type, bool is_notice)
{
	if (jewelry_type < 0 || jewelry_type >= Jewelry::J_TYPE_MAX) return false;

	m_huashen_jewelry_list[jewelry_type].Clear();
	this->SetJewelryGridDirty(jewelry_type, is_notice, ROLE_TYPE_HUASHEN);
	return true;
}

void EquipmentManager::SendHuaShenAllJewelryInfo()
{
	static Protocol::SCHuaShenJewelryAllInform info;
	info.jewelry_count = 0;
	for (int j_type = 0; j_type < Jewelry::J_TYPE_MAX; j_type++)
	{
		if (m_huashen_jewelry_list[j_type].Invalid()) continue;

		const ItemGridData & p_igd = m_huashen_jewelry_list[j_type];
		JewelryPro & p_jp = info.jewelry_list[info.jewelry_count++];
		p_jp.jewelry_type = j_type;
		p_jp.jewelry_item = p_igd.item_id;
		p_jp.item_unique_id = p_igd.item_unique_id;
		p_jp.j_param = p_igd.param->jewelry_param;
	}

	int send_len = sizeof(info) - (Jewelry::J_TYPE_MAX - info.jewelry_count) * sizeof(info.jewelry_list[0]);
	EngineAdapter::Instance().NetSend(m_module_mgr->GetRole()->GetNetId(), (const char*)&info, send_len);
}

void EquipmentManager::SendHuaShenSingleJewelryInfo(short jewelry_type)
{
	if (jewelry_type < 0 || jewelry_type >= Jewelry::J_TYPE_MAX) return;

	static Protocol::SCHuaShenJewelryInfoChange info;

	const ItemGridData & p_igd = m_huashen_jewelry_list[jewelry_type];
	info.jewelry_info.jewelry_type = jewelry_type;
	info.jewelry_info.jewelry_item = p_igd.item_id;
	info.jewelry_info.item_unique_id = p_igd.item_unique_id;
	if (NULL != p_igd.param)
	{
		info.jewelry_info.j_param = p_igd.param->jewelry_param;
		/*const EquipmentScoreCfg * score_cfg = LOGIC_CONFIG->GetScoreSystemConfig()->GetEquipmentScoreCfg(p_igd.item_id);
		if (NULL != score_cfg)
		{
			JewelryParam & data = p_igd.param->jewelry_param;
			if (data.is_treasure == ITEM_SCORE_LEVEL_S)
			{
				const_cast<ItemGridData &>(p_igd).is_bind = false;
			}
		}*/
	}
	else
	{
		memset(&info.jewelry_info.j_param, 0, sizeof(info.jewelry_info.j_param));
	}


	EngineAdapter::Instance().NetSend(m_module_mgr->GetRole()->GetNetId(), (const char*)&info, sizeof(info));
}

int EquipmentManager::GetReachingColor(int level, int color)
{
	int ret = 0;

	for (int i = Equipment::E_INDEX_MIN; i < Equipment::E_INDEX_MAX; ++i)
	{
		const Equipment *equip = (const Equipment *)ITEMPOOL->GetItem(m_equipment_list[i].item_id);
		if (NULL == equip || equip->GetItemType() != I_TYPE_EQUIPMENT) continue;

		if (equip->GetColor() >= color && equip->GetEquipLevel() >= level)
		{
			ret++;
		}
	}

	return ret;
}

void EquipmentManager::OnChangeProfession()
{
	//检查身上装备
	for (int equip_index = 0; equip_index < Equipment::E_INDEX_MAX; equip_index++)
	{
		ItemGridData & i_d = m_equipment_list[equip_index];
		if (i_d.Invalid())continue;
		if (m_module_mgr->GetWorkshop()->ChnageGodEquip(&i_d))
		{
			this->SetEquipGridDirty(equip_index);
		}
	}
	//检查身上灵饰
	for (int jewelry_index = 0; jewelry_index < Jewelry::J_TYPE_MAX; jewelry_index++)
	{
		ItemGridData & i_d = m_jewelry_list[jewelry_index];
		if (i_d.Invalid())continue;
		if (m_module_mgr->GetWorkshop()->ChnageGodEquip(&i_d))
		{
			this->SetJewelryGridDirty(jewelry_index);
		}
	}

	m_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_EQUIPMENT, __FUNCTION__);
	this->SendEquipmentInfoToRole();
}

void EquipmentManager::OnRoleStartFight(int battle_mode)
{
	//幻界战场战斗模式
	if (battle_mode == BATTLE_MODE_HUAN_JIE_ZHAN_CHANG_TIAN_TI)
	{
		if (m_other_param.huashen_element_crystal_used_times > 0)
		{
			--m_other_param.huashen_element_crystal_used_times;
			//本尊与化身使用同一个元素水晶
			bool is_same = false;
			if (m_other_param.huashen_element_crystal_unique_id == m_other_param.element_crystal_unique_id)
			{
				m_other_param.element_crystal_used_times = m_other_param.huashen_element_crystal_used_times;
				is_same = true;
			}
			else
			{
				ItemGridData item_data; item_data.Reset();
				if (this->GetElementCrystal(&item_data, ROLE_TYPE_HUASHEN))
				{
					m_module_mgr->GetKnapsack()->SynItemData(ITEM_COLUMN_TYPE_ITEM, &item_data);
				}
			}
			this->SendHuaShenElementCrystalChangeNotice();
			if (is_same)
			{
				this->SendElementCrystalChangeNotice();
			}
		}
	}
	else
	{
		if (m_other_param.element_crystal_used_times > 0)
		{
			--m_other_param.element_crystal_used_times;
			this->SendElementCrystalChangeNotice();
		}
	}
}

void EquipmentManager::OnRoleFinishFight()
{
	//主角水晶次数用完自动脱下
	if (m_other_param.element_crystal_used_times <= 0 && m_other_param.element_crystal_id != (ItemID)-1)
	{
		this->TakeOffElementCrystal();
		m_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_EQUIPMENT, __FUNCTION__);
		m_module_mgr->NoticeNum(errornum::EN_YOUR_ELEMENT_CRYTAL_COUNT_DONE);
	}
	if (m_other_param.huashen_element_crystal_id != (ItemID)-1 && m_other_param.huashen_element_crystal_used_times <= 0 && m_other_param.huashen_element_crystal_id != 0)
	{
		//化身屏蔽废弃水晶
		m_other_param.huashen_element_crystal_id = (ItemID)-1;
		m_other_param.huashen_element_crystal_used_times = 0;
		m_other_param.huashen_element_crystal_unique_id = 0;
		m_other_param.huashen_element_crystal_is_bind = 0;
		m_module_mgr->GetHuaShenManager()->ReCalcAttr(AttributeList::RECALC_REASON_EQUIPMENT, __FUNCTION__);
		m_module_mgr->NoticeNum(errornum::EN_YOUR_ELEMENT_CRYTAL_COUNT_DONE);
	}
}

bool EquipmentManager::GetElementCrystal(ItemGridData * item_data, int role_type)
{
	if (NULL == item_data || role_type < 0 || role_type >= ROLE_TYPE_MAX) return false;

	static NetValueItemParam tmp_item_param;
	tmp_item_param.Reset();
	static ItemDataWrapper wrapper;
	wrapper.Reset();

	if (role_type == ROLE_TYPE_BENZUN)
	{
		tmp_item_param.element_crystal_param.used_times = m_other_param.element_crystal_used_times;
		tmp_item_param.element_crystal_param.has_set_net_value = 1;
		wrapper.item_id = m_other_param.element_crystal_id;
		wrapper.num = 1;
		wrapper.is_bind = true;
		wrapper.item_unique_id = m_other_param.element_crystal_unique_id;
	}
	else
	{
		tmp_item_param.element_crystal_param.used_times = m_other_param.huashen_element_crystal_used_times;
		tmp_item_param.element_crystal_param.has_set_net_value = 1;
		wrapper.item_id = m_other_param.huashen_element_crystal_id;
		wrapper.num = 1;
		wrapper.is_bind = m_other_param.huashen_element_crystal_is_bind;
		wrapper.item_unique_id = m_other_param.huashen_element_crystal_unique_id;
	}
	wrapper.has_param = 1;
	tmp_item_param.GetData(wrapper.param_data);
	item_data->Set(wrapper);

	return true;
}

void EquipmentManager::ClearHuaShenEquipAndJewelry()
{
	for (int i = 0; i < Equipment::E_INDEX_MAX; ++i)
	{
		m_huashen_equipment_list[i].Clear();		// 指针已无用 删除之
		this->SetEquipGridDirty(i, false, ROLE_TYPE_HUASHEN);
	}
	for (int i = 0; i < Jewelry::J_TYPE_MAX; ++i)
	{
		m_huashen_jewelry_list[i].Clear();		// 指针已无用 删除之
		this->SetJewelryGridDirty(i, false, ROLE_TYPE_HUASHEN);
	}
}

void EquipmentManager::ClearHuaShenEquip()
{
	for (int i = 0; i < Equipment::E_INDEX_MAX; ++i)
	{
		m_huashen_equipment_list[i].Clear();		// 指针已无用 删除之
		this->SetEquipGridDirty(i, false, ROLE_TYPE_HUASHEN);
	}
	for (int i = 0; i < Jewelry::J_TYPE_MAX; ++i)
	{
		m_huashen_jewelry_list[i].Clear();		// 指针已无用 删除之
		this->SetJewelryGridDirty(i, false, ROLE_TYPE_HUASHEN);
	}

	m_other_param.huashen_element_crystal_id = 0;
	m_other_param.huashen_element_crystal_unique_id = 0;
	m_other_param.huashen_element_crystal_used_times = 0;
	m_other_param.huashen_element_crystal_is_bind = 0;
}

void EquipmentManager::SynHuaShenEquipAndJewelry(bool is_check)
{
	if (is_check)
	{
		//检测此时化身与本尊穿戴装备是否一致
		for (int i = 0; i < Equipment::E_INDEX_MAX; i++)
		{
			if (m_equipment_list[i].Invalid())
			{
				if (!m_huashen_equipment_list[i].Invalid())
				{
					this->TakeOff(i, true, ROLE_TYPE_HUASHEN);
				}
				continue;
			}
			int key_index = HUASHEN_GRID_INDEX_TYPE_EQUIPMENT * HUASHEN_GRID_INDEX_TYPE_BASE + i;
			if (!m_module_mgr->GetHuaShenManager()->IsCheckItemIdentical(m_equipment_list[i].item_unique_id, key_index))
			{
				m_module_mgr->GetHuaShenManager()->ConsumeItem(m_equipment_list[i].item_unique_id, false);
				if (!m_huashen_equipment_list[i].Invalid())
				{
					this->TakeOff(i, true, ROLE_TYPE_HUASHEN);
				}
				ItemDataWrapper temp_item_wrapper;
				m_equipment_list[i].WrapTo(&temp_item_wrapper);
				m_huashen_equipment_list[i].Set(temp_item_wrapper);
				m_module_mgr->GetHuaShenManager()->PutOn(i, m_huashen_equipment_list[i].item_unique_id, HUASHEN_GRID_INDEX_TYPE_EQUIPMENT);
			}
		}

		for (int i = 0; i < Jewelry::J_TYPE_MAX; i++)
		{
			if (m_jewelry_list[i].Invalid())
			{
				if (!m_huashen_jewelry_list[i].Invalid())
				{
					this->HuaShenTakeOffJewelry(i, false);
				}
				continue;
			}
			int key_index = HUASHEN_GRID_INDEX_TYPE_JEWELRY * HUASHEN_GRID_INDEX_TYPE_BASE + i;
			if (!m_module_mgr->GetHuaShenManager()->IsCheckItemIdentical(m_jewelry_list[i].item_unique_id, key_index))
			{
				m_module_mgr->GetHuaShenManager()->ConsumeItem(m_jewelry_list[i].item_unique_id, false);
				if (!m_huashen_jewelry_list[i].Invalid())
				{
					this->HuaShenTakeOffJewelry(i, false);
				}
				ItemDataWrapper temp_item_wrapper;
				m_jewelry_list[i].WrapTo(&temp_item_wrapper);
				m_huashen_jewelry_list[i].Set(temp_item_wrapper);
				m_module_mgr->GetHuaShenManager()->PutOn(i, m_huashen_jewelry_list[i].item_unique_id, HUASHEN_GRID_INDEX_TYPE_JEWELRY);
			}
		}
		bool is_huashen_wear_crystal = false;
		if (0 != m_other_param.huashen_element_crystal_id && (ItemID)-1 != m_other_param.huashen_element_crystal_id)
		{
			is_huashen_wear_crystal = true;
		}

		if (m_other_param.element_crystal_id != 0 && m_other_param.element_crystal_id != (ItemID)-1)
		{
			int key_index = HUASHEN_GRID_INDEX_TYPE_CRYSTAL * HUASHEN_GRID_INDEX_TYPE_BASE;
			if (!m_module_mgr->GetHuaShenManager()->IsCheckItemIdentical(m_other_param.element_crystal_unique_id, key_index))
			{
				m_module_mgr->GetHuaShenManager()->ConsumeItem(m_other_param.element_crystal_unique_id, false);
				if (is_huashen_wear_crystal)
				{
					this->HuaShenTakeOffElementCrystal(true);
				}
				m_other_param.huashen_element_crystal_id = m_other_param.element_crystal_id;
				m_other_param.huashen_element_crystal_unique_id = m_other_param.element_crystal_unique_id;
				m_other_param.huashen_element_crystal_used_times = m_other_param.element_crystal_used_times;
				m_other_param.huashen_element_crystal_is_bind = 1;			//此时是从本尊的穿戴同步过来的，所以默认绑定
				m_module_mgr->GetHuaShenManager()->PutOn(0, m_other_param.huashen_element_crystal_unique_id, HUASHEN_GRID_INDEX_TYPE_CRYSTAL);
			}
		}
		else
		{
			if (is_huashen_wear_crystal)
			{
				this->HuaShenTakeOffElementCrystal(true);
			}

		}
	}
	else
	{
		for (int i = 0; i < Equipment::E_INDEX_MAX; i++)
		{
			if (m_equipment_list[i].Invalid()) continue;

			ItemDataWrapper temp_item_wrapper;
			m_equipment_list[i].WrapTo(&temp_item_wrapper);
			m_huashen_equipment_list[i].Set(temp_item_wrapper);
			m_module_mgr->GetHuaShenManager()->PutOn(i, m_huashen_equipment_list[i].item_unique_id, HUASHEN_GRID_INDEX_TYPE_EQUIPMENT);
		}
		for (int i = 0; i < Jewelry::J_TYPE_MAX; i++)
		{
			if (m_jewelry_list[i].Invalid()) continue;

			ItemDataWrapper temp_item_wrapper;
			m_jewelry_list[i].WrapTo(&temp_item_wrapper);
			m_huashen_jewelry_list[i].Set(temp_item_wrapper);
			m_module_mgr->GetHuaShenManager()->PutOn(i, m_huashen_jewelry_list[i].item_unique_id, HUASHEN_GRID_INDEX_TYPE_JEWELRY);
		}
		if (m_other_param.element_crystal_id != 0 && m_other_param.element_crystal_id != (ItemID)-1)
		{
			m_other_param.huashen_element_crystal_id = m_other_param.element_crystal_id;
			m_other_param.huashen_element_crystal_unique_id = m_other_param.element_crystal_unique_id;
			m_other_param.huashen_element_crystal_used_times = m_other_param.element_crystal_used_times;
			m_other_param.huashen_element_crystal_is_bind = 1;			//此时是从本尊的穿戴同步过来的，所以默认绑定
			m_module_mgr->GetHuaShenManager()->PutOn(0, m_other_param.huashen_element_crystal_unique_id, HUASHEN_GRID_INDEX_TYPE_CRYSTAL);
		}
	}
}

void EquipmentManager::HuaShenKeySyn(std::set<long long> * item_set)
{
	for (int i = 0; i < Equipment::E_INDEX_MAX; i++)
	{
		if (m_equipment_list[i].Invalid()) continue;

		m_module_mgr->GetHuaShenManager()->SynBenZunItemData(&m_equipment_list[i], HUASHEN_KNAPSACK_OP_TYPE_KEY_SYN);
		if (NULL != item_set)
		{
			item_set->insert(m_equipment_list[i].item_unique_id);
		}
	}
	for (int i = 0; i < Jewelry::J_TYPE_MAX; i++)
	{
		if (m_jewelry_list[i].Invalid()) continue;

		m_module_mgr->GetHuaShenManager()->SynBenZunItemData(&m_jewelry_list[i], HUASHEN_KNAPSACK_OP_TYPE_KEY_SYN);
		if (NULL != item_set)
		{
			item_set->insert(m_jewelry_list[i].item_unique_id);
		}
	}

	if (m_other_param.element_crystal_id != 0 && m_other_param.element_crystal_id != (ItemID)-1)
	{
		ItemGridData item_data; item_data.Reset();
		this->GetElementCrystal(&item_data, ROLE_TYPE_BENZUN);
		m_module_mgr->GetHuaShenManager()->SynBenZunItemData(&item_data, HUASHEN_KNAPSACK_OP_TYPE_KEY_SYN);
		if (NULL != item_set)
		{
			item_set->insert(m_other_param.element_crystal_unique_id);
		}
	}
}

void EquipmentManager::CheckItemIsHuaShenPut(int item_type, int index, ItemGridData * grid)
{
	if (NULL == grid || NULL == grid->param) return;
	switch (item_type)
	{
	case I_TYPE_EQUIPMENT:
		{
			if (index < Equipment::E_INDEX_MIN || index >= Equipment::E_INDEX_MAX || m_huashen_equipment_list[index].Invalid() ||
				m_huashen_equipment_list[index].item_unique_id != grid->item_unique_id) return;

			m_huashen_equipment_list[index].param->SetData(grid->param->data);
			m_huashen_equipment_list[index].is_bind = grid->is_bind;
			this->SetEquipGridDirty(index, true, ROLE_TYPE_HUASHEN);
		}
		break;
	case I_TYPE_JEWELRY:
		{
			if (index < 0 || index >= Jewelry::J_TYPE_MAX || m_huashen_jewelry_list[index].Invalid() ||
				m_huashen_jewelry_list[index].item_unique_id != grid->item_unique_id) return;

			m_huashen_jewelry_list[index].param->SetData(grid->param->data);
			m_huashen_jewelry_list[index].is_bind = grid->is_bind;
			this->SetJewelryGridDirty(index, true, ROLE_TYPE_HUASHEN);
		}
		break;
	default:
		return;
	}
}

void EquipmentManager::SynItemDataPut(int item_type, int index, const ItemGridData * grid, int role_type)
{
	//当前本尊与化身都使用的同一套装备,此时不需要再去同步(以前穿戴不同,并且工坊可以看到化身的穿戴)
	return;			
}

void EquipmentManager::OpenResonaceLevel()
{
	const GodEquipResonanceCfg * resonance_cfg = GODEQUIP_CONFIG->GetResonanceCfg(m_module_mgr->GetRole()->GetAppearanceProf(), low_god_equip_level);
	if (has_wear_all_equip && NULL != resonance_cfg)
	{
		if (resonance_cfg->addition_level <= m_other_param.resonace_open_level)return;
		gamelog::g_log_role_data.printf(LL_INFO, "EquipmentManager::OpenResonaceLevel user[%s,%d] prof[%d] low_god_equip_level[%d] old_level[%d] new_level[%d]", m_module_mgr->GetRoleName(), m_module_mgr->GetRoleUID(),
			(int)m_module_mgr->GetRole()->GetAppearanceProf(), low_god_equip_level, m_other_param.resonace_open_level, m_other_param.resonace_open_level + 1);
		m_other_param.resonace_open_level += 1;
		m_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_EQUIPMENT, __FUNCTION__);
		this->SendResonaceInfo();
	}
}

void EquipmentManager::SendDianHuaShiCap()
{
	Protocol::SCRoleAttrList info;
	info.attr_count = 0;
	info.notify_reason = Protocol::SCRoleAttrList::NOTIFY_REASON_DIAN_HUA_SHI_CAP;
	info.capability = m_module_mgr->GetCapability()->GetRoleCapability();

	int length = sizeof(info) - sizeof(info.attr_list);
	m_module_mgr->NetSend(&info, length);
}

void EquipmentManager::SendEquipMentLockInfo()
{
	Protocol::SCEquipmentLockInfo info;
	for (int i = 0; i < Equipment::E_INDEX_MAX; i++)
	{
		memcpy(info.m_equipment_list[i].is_lock, m_other_param.m_equipment_lock_list[i].is_lock, sizeof(info.m_equipment_list[i].is_lock));
	}

	for (int y = 0; y < Jewelry::J_TYPE_MAX; y++)
	{
		memcpy(info.m_jewelry_list[y].is_lock, m_other_param.m_jewelry_lock_list[y].is_lock, sizeof(info.m_jewelry_list[y].is_lock));
	}

	m_module_mgr->NetSend((const char*)&info, sizeof(info));
}

void EquipmentManager::UpGreade(int idx)
{
	if (idx < 0 || idx >= EQUIP_JEWELRY_TYPE_MAX)return;

	int &hole_level = m_other_param.equipment_jewelry_upgreade[idx];
	int &grade_level = m_other_param.equipment_jewelry_strengthen[idx];
	const PositionCfg * cfg = LOGIC_CONFIG->GetEquipmentJewerlyUpgreadeConfig()->GetPositionCfg(m_module_mgr->GetRole()->GetBaseProfession(), idx, hole_level + 1);
	if (cfg == NULL)return;

	switch (idx)
	{
		case EQUIP_JEWELRY_TYPE_HELMET:
		case EQUIP_JEWELRY_TYPE_ARMOUR:
		case EQUIP_JEWELRY_TYPE_WEAPON:
		case EQUIP_JEWELRY_TYPE_SHIELD:
		case EQUIP_JEWELRY_TYPE_SHOE:
		{
			const ItemGridData* node = &m_equipment_list[idx];
			if (NULL == node || node->Invalid() || NULL == node->param)return;
			const ItemBase * i_b = ITEMPOOL->GetItem(node->item_id);
			if (NULL == i_b || i_b->GetItemType() != I_TYPE_EQUIPMENT)return;
			if (i_b->GetColor() >= ItemBase::I_COLOR_GOD_COLOR_BEGAIN && i_b->GetColor() <= ItemBase::I_COLOR_GOD_COLOR_END)
			{
				//神装
				if (cfg->need_godequip_level > 0 && cfg->need_godequip_level > node->param->equip_param.strengthen_level)
				{
					m_module_mgr->NoticeNum(errornum::EN_GOD_EQUIP_NO_LEVEL);
					return;
				}
			}
			else
			{
				if (cfg->need_equip_level > 0 && cfg->need_equip_level > i_b->GetLimitLevel())
				{
					m_module_mgr->NoticeNum(errornum::EN_EQUIP_JEWELRY_UP_EQUIP_LEVEL_LIMIT);
					return;
				}
			}
		}
		break;
		case EQUIP_JEWELRY_TYPE_GEM_RING:
		case EQUIP_JEWELRY_TYPE_BRACELET:
		case EQUIP_JEWELRY_TYPE_PENDANTS:
		{
			const ItemGridData* node = &m_jewelry_list[idx - EQUIP_JEWELRY_TYPE_GEM_RING];
			if (NULL == node || node->Invalid() || NULL == node->param)return;
			const ItemBase * i_b = ITEMPOOL->GetItem(node->item_id);
			if (NULL == i_b || i_b->GetItemType() != I_TYPE_JEWELRY)return;
			if (i_b->GetColor() >= ItemBase::I_COLOR_GOD_COLOR_BEGAIN && i_b->GetColor() <= ItemBase::I_COLOR_GOD_COLOR_END)
			{
				//神装
				if (cfg->need_godequip_level > 0 && cfg->need_godequip_level > node->param->jewelry_param.strengthen_level)
				{
					m_module_mgr->NoticeNum(errornum::EN_GOD_EQUIP_NO_LEVEL);
					return;
				}
			}
			else
			{
				if (cfg->need_equip_level > 0 && cfg->need_equip_level > i_b->GetLimitLevel())
				{
					m_module_mgr->NoticeNum(errornum::EN_EQUIP_JEWELRY_UP_EQUIP_LEVEL_LIMIT);
					return;
				}
			}
		}
		break;
	default:
		break;
	}

	//检查是否突破
	if (cfg->grade > grade_level)//是的需要突破
	{
		const EquipJewelryStrengthenCfg * next_grade_cfg = LOGIC_CONFIG->GetEquipmentJewerlyUpgreadeConfig()->GetStrengthenCfg(m_module_mgr->GetRole()->GetBaseProfession(), idx, grade_level + 1);
		if (NULL != next_grade_cfg)
		{
			if ((short)next_grade_cfg->consume_item_list.size() > 0)
			{
				int stuff_count = 0; ItemExtern::ItemConsumeConfig stuff_list[ItemNamespace::MAX_BATCH_CONSUME_ITEM_NUM];

				for (int t_cur = 0; t_cur < ItemNamespace::MAX_BATCH_CONSUME_ITEM_NUM && t_cur < (short)next_grade_cfg->consume_item_list.size(); t_cur++)
				{
					const ItemConfigData p_mc = next_grade_cfg->consume_item_list[t_cur];
					if (p_mc.num <= 0) continue;

					const ItemBase * mate_ib = ITEMPOOL->GetItem(p_mc.item_id);
					if (NULL == mate_ib) continue;

					stuff_list[stuff_count] = ItemExtern::ItemConsumeConfig(p_mc.item_id, p_mc.is_bind ? true : false, p_mc.num);
					++stuff_count;
				}

				static ItemExtern::ItemConsumeList consume_list; consume_list.Reset();
				if (!ItemExtern::GetConsumeListByOrder(m_module_mgr->GetRole(), false, (short)next_grade_cfg->consume_item_list.size(), stuff_list, &consume_list, true))
				{
					m_module_mgr->NoticeNum(errornum::EN_STUFF_NOT_ENOUGH);
					return;
				}

				if (consume_list.count > 0 && !m_module_mgr->GetKnapsack()->ConsumeItemByIndexList(consume_list.count, consume_list.column_list, consume_list.index_list, consume_list.num_list, "UpGreadeBreak")) return;

				grade_level = next_grade_cfg->grade;
			}
		}

		m_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_EQUIPMENT, __FUNCTION__);
		this->SendUpGreadeInfo();
		return;//突破完直接返回
	}

	//检查消耗
	if ((short)cfg->consume_item_list.size() > 0)
	{
		int stuff_count = 0; ItemExtern::ItemConsumeConfig stuff_list[ItemNamespace::MAX_BATCH_CONSUME_ITEM_NUM];

		for (int t_cur = 0; t_cur < ItemNamespace::MAX_BATCH_CONSUME_ITEM_NUM && t_cur < (short)cfg->consume_item_list.size(); t_cur++)
		{
			const ItemConfigData p_mc = cfg->consume_item_list[t_cur];
			if (p_mc.num <= 0) continue;

			const ItemBase * mate_ib = ITEMPOOL->GetItem(p_mc.item_id);
			if (NULL == mate_ib) continue;

			stuff_list[stuff_count] = ItemExtern::ItemConsumeConfig(p_mc.item_id, p_mc.is_bind ? true : false, p_mc.num);
			++stuff_count;
		}

		static ItemExtern::ItemConsumeList consume_list; consume_list.Reset();
		if (!ItemExtern::GetConsumeListByOrder(m_module_mgr->GetRole(), false, (short)cfg->consume_item_list.size(), stuff_list, &consume_list, true))
		{
			m_module_mgr->NoticeNum(errornum::EN_STUFF_NOT_ENOUGH);
			return;
		}

		if (consume_list.count > 0 && !m_module_mgr->GetKnapsack()->ConsumeItemByIndexList(consume_list.count, consume_list.column_list, consume_list.index_list, consume_list.num_list, "UpGreade")) return;
	}
	else return;

	if (cfg->hole_level > hole_level)
	{
		hole_level = cfg->hole_level;
	}
	this->SendUpGreadeInfo();
	m_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_EQUIPMENT, __FUNCTION__);

	EventHandler::Instance().OnRoleEquipStr(m_module_mgr->GetRole(), idx, hole_level);
}

void EquipmentManager::SendUpGreadeInfo()
{
	Protocol::SCEquipmentJewerlyUpGreadeInfo info;

	memcpy(info.equipment_jewelry_upgreade, m_other_param.equipment_jewelry_upgreade, sizeof(info.equipment_jewelry_upgreade));
	memcpy(info.equipment_jewelry_strengthen, m_other_param.equipment_jewelry_strengthen, sizeof(info.equipment_jewelry_strengthen));

	m_module_mgr->NetSend((const char*)&info, sizeof(info));
}

void EquipmentManager::SendResonaceInfo()
{
	Protocol::SCEquipmentJewerlyResonaceInfo info;
	info.resonace_flag = m_other_param.resonace_open_level;
	m_module_mgr->NetSend((const char *)&info, sizeof(info));
}

int EquipmentManager::GetMinEquipStr()
{
	int min_str_level = MAX_INT;
	for (int i = 0; i < ARRAY_ITEM_COUNT(m_other_param.equipment_jewelry_upgreade); ++i)
	{
		min_str_level = GetMin(min_str_level, m_other_param.equipment_jewelry_upgreade[i]);
	}

	return min_str_level;
}

void EquipmentManager::ReturnGodEquip()
{
	const GodEquipTurnBackListCfg * turn_back_cfg = GODEQUIP_CONFIG->GetTurnBackUserId(m_module_mgr->GetUid());
	if (NULL != turn_back_cfg)
	{
		bool has_change = false;
		for (std::vector<GodEquipTurnBackNode>::const_iterator it = turn_back_cfg->back_item_vec.begin(); it != turn_back_cfg->back_item_vec.end(); it++)
		{
			for (int equip_index = 0; equip_index < Equipment::E_INDEX_MAX; equip_index++)
			{
				if (m_equipment_list[equip_index].Invalid()) continue;
				if (m_equipment_list[equip_index].item_id != it->back_item)continue;
				bool ret = m_module_mgr->GetWorkshop()->CheckGodEquipItemDataReturnBreak(&m_equipment_list[equip_index], &(*it));
				if (ret)
				{
					this->SetEquipGridDirty(equip_index);
					has_change = true;
				}
			}

			for (int equip_index = 0; equip_index < Jewelry::J_TYPE_MAX; equip_index++)
			{
				if (m_jewelry_list[equip_index].Invalid()) continue;
				if (m_jewelry_list[equip_index].item_id != it->back_item)continue;
				bool ret = m_module_mgr->GetWorkshop()->CheckGodEquipItemDataReturnBreak(&m_jewelry_list[equip_index], &(*it));
				if (ret)
				{
					this->SetJewelryGridDirty(equip_index);
					has_change = true;
				}
			}
		}
		if (has_change)
		{
			m_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_EQUIPMENT, __FUNCTION__);
		}
	}
}

void EquipmentManager::ReturnGodEquip2()
{
	const GodEquipTurnBackListCfg * turn_back_cfg = GODEQUIP_CONFIG->GetTurnBackUserId2(m_module_mgr->GetUid());
	if (NULL != turn_back_cfg)
	{
		bool has_change = false;
		for (std::vector<GodEquipTurnBackNode>::const_iterator it = turn_back_cfg->back_item_vec.begin(); it != turn_back_cfg->back_item_vec.end(); it++)
		{
			for (int equip_index = 0; equip_index < Equipment::E_INDEX_MAX; equip_index++)
			{
				if (m_equipment_list[equip_index].Invalid()) continue;
				if (m_equipment_list[equip_index].item_id != it->back_item)continue;
				bool ret = m_module_mgr->GetWorkshop()->CheckGodEquipItemDataReturnBreak(&m_equipment_list[equip_index], &(*it));
				if (ret)
				{
					this->SetEquipGridDirty(equip_index);
					has_change = true;
				}
			}

			for (int equip_index = 0; equip_index < Jewelry::J_TYPE_MAX; equip_index++)
			{
				if (m_jewelry_list[equip_index].Invalid()) continue;
				if (m_jewelry_list[equip_index].item_id != it->back_item)continue;
				bool ret = m_module_mgr->GetWorkshop()->CheckGodEquipItemDataReturnBreak(&m_jewelry_list[equip_index], &(*it));
				if (ret)
				{
					this->SetJewelryGridDirty(equip_index);
					has_change = true;
				}
			}
		}
		if (has_change)
		{
			m_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_EQUIPMENT, __FUNCTION__);
		}
	}
}

void EquipmentManager::GodEquipReBack()
{
	{
		for (int equip_index = 0; equip_index < Equipment::E_INDEX_MAX; equip_index++)
		{
			if (m_equipment_list[equip_index].Invalid()) continue;

			m_module_mgr->GetWorkshop()->CheckGodEquipItemDataReturn(&m_equipment_list[equip_index]);
		}
	}

	{
		for (int equip_index = 0; equip_index < Jewelry::J_TYPE_MAX; equip_index++)
		{
			if (m_jewelry_list[equip_index].Invalid()) continue;

			m_module_mgr->GetWorkshop()->CheckGodEquipItemDataReturn(&m_jewelry_list[equip_index]);
		}
	}
}

void EquipmentManager::GodEquipBackErrUplevel()
{
	//神装返还
	{
		if (m_equipment_list[Equipment::E_INDEX_SHIELD].Invalid()) return;
		ItemID low_god_id[8] = { 3901,3902,3903,3904,3905,3906,3907,3908 };
		ItemID high_god_id[8] = { 3909,3910,3911,3912,3913,3914,3915,3916 };
		bool need_back = false;
		for (int i = 0; i < 8; i++)
		{
			if (low_god_id[i] == m_equipment_list[Equipment::E_INDEX_SHIELD].item_id && m_equipment_list[Equipment::E_INDEX_SHIELD].param->equip_param.strengthen_level >= 41)
			{
				need_back = true;
			}
		}
		for (int i = 0; i < 8; i++)
		{
			if (high_god_id[i] == m_equipment_list[Equipment::E_INDEX_SHIELD].item_id)
			{
				need_back = true;
			}
		}
		if (need_back)
		{
			static MailContentParam contentparam; contentparam.Reset();
			int real_item_num = 0;
			contentparam.item_list[real_item_num].num = 3;
			contentparam.item_list[real_item_num].is_bind = 0;
			contentparam.item_list[real_item_num++].item_id = 26214;

			int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_god_equip_return_subject);
			int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_god_equip_return_content);
			if (lensub > 0 && length > 0)
			{
				MailRoute::MailToUser(m_module_mgr->GetRole()->GetUserId(), MAIL_REASON_DEFAULT, contentparam);
			}
		}
	}
}

void EquipmentManager::GodEquipChangeLevel()
{
	{
		for (int equip_index = 0; equip_index < Equipment::E_INDEX_MAX; equip_index++)
		{
			if (m_equipment_list[equip_index].Invalid()) continue;

			m_module_mgr->GetWorkshop()->CheckGodEquipItemDataChangeLevel(&m_equipment_list[equip_index]);
			this->SetEquipGridDirty(equip_index);
		}
	}

	{
		for (int equip_index = 0; equip_index < Jewelry::J_TYPE_MAX; equip_index++)
		{
			if (m_jewelry_list[equip_index].Invalid()) continue;

			m_module_mgr->GetWorkshop()->CheckGodEquipItemDataChangeLevel(&m_jewelry_list[equip_index]);
			this->SetJewelryGridDirty(equip_index);
		}
	}
}

void EquipmentManager::FixGodEquipBaseVoc()
{
	{
		for (int equip_index = 0; equip_index < Equipment::E_INDEX_MAX; equip_index++)
		{
			if (m_equipment_list[equip_index].Invalid()) continue;
			const ItemBase * i_b = ITEMPOOL->GetItem(m_equipment_list[equip_index].item_id);
			if (NULL == i_b || i_b->GetColor() < ItemBase::I_COLOR_STAR || i_b->GetColor() > ItemBase::I_COLOR_GOD)continue;
			EquipmentParam & p_ep = m_equipment_list[equip_index].param->equip_param;
			const StrengthenCfg *consume_cfg = GODEQUIP_CONFIG->GetStrengthenCfg(m_equipment_list[equip_index].item_id, p_ep.strengthen_level);
			if (consume_cfg == NULL)continue;

			p_ep.attr_num = 0;
			for (int i = 0; i < EquipmentParam::ATTR_NUM_MAX; i++)
			{
				p_ep.attr_list[i].attr_type = consume_cfg->attr_type_list[i];
				p_ep.attr_list[i].attr_value = consume_cfg->attr_add_list[i];
				if (consume_cfg->attr_add_list[i] <= 0)continue;//值为0则词条为假不增加词条数量
				p_ep.attr_num++;
			}
			this->SetEquipGridDirty(equip_index);
		}
	}

	{
		for (int equip_index = 0; equip_index < Jewelry::J_TYPE_MAX; equip_index++)
		{
			if (m_jewelry_list[equip_index].Invalid()) continue;
			const ItemBase * i_b = ITEMPOOL->GetItem(m_jewelry_list[equip_index].item_id);
			if (NULL == i_b || i_b->GetColor() < ItemBase::I_COLOR_STAR || i_b->GetColor() > ItemBase::I_COLOR_GOD)continue;
			JewelryParam & p_ep = m_jewelry_list[equip_index].param->jewelry_param;
			const StrengthenCfg *consume_cfg = GODEQUIP_CONFIG->GetStrengthenCfg(m_jewelry_list[equip_index].item_id, p_ep.strengthen_level);
			if (consume_cfg == NULL)continue;
			p_ep.attr_num = 0;
			for (int i = 0; i < JewelryParam::ATTR_NUM_MAX; i++)
			{
				p_ep.attr_list[i].attr_type = consume_cfg->attr_type_list[i];
				p_ep.attr_list[i].attr_value = consume_cfg->attr_add_list[i];
				if (consume_cfg->attr_add_list[i] <= 0)continue;//值为0则词条为假不增加词条数量
				p_ep.attr_num++;
			}
			this->SetJewelryGridDirty(equip_index);
		}
	}
}

void EquipmentManager::CheckNewSuitChange()
{
	/*if (m_other_param.new_suit_flag == 0)
	{
		this->ChangeNewSuit();
		m_module_mgr->GetKnapsack()->ChangeNewSuit();
		m_other_param.new_suit_flag = 1;
	}*/

	if (m_other_param.new_suit_flag != 2)
	{
		m_module_mgr->GetKnapsack()->ChangeNewSuit();
		m_other_param.new_suit_flag = 2;
	}
}

void EquipmentManager::ChangeNewSuit()
{
	{
		for (int equip_index = 0; equip_index < Equipment::E_INDEX_MAX; equip_index++)
		{
			if (m_equipment_list[equip_index].Invalid()) continue;
			const ItemBase * i_b = ITEMPOOL->GetItem(m_equipment_list[equip_index].item_id);
			if (NULL == i_b)continue;
			EquipmentParam & p_ep = m_equipment_list[equip_index].param->equip_param;
			if (p_ep.suit_effect_change_flag != 1)
			{
				p_ep.suit_effect_change_flag = 1;
				int new_id = LOGIC_CONFIG->GetSuitListConfig()->GetNewSuitByOldSuit(p_ep.suit_effect_id);
				p_ep.suit_effect_id = 0;
				p_ep.new_suit_effect_id = new_id;
				this->SetEquipGridDirty(equip_index);
			}
		}
	}

	{
		for (int equip_index = 0; equip_index < Jewelry::J_TYPE_MAX; equip_index++)
		{
			if (m_jewelry_list[equip_index].Invalid()) continue;
			const ItemBase * i_b = ITEMPOOL->GetItem(m_jewelry_list[equip_index].item_id);
			if (NULL == i_b)continue;
			JewelryParam & p_ep = m_jewelry_list[equip_index].param->jewelry_param;
			if (p_ep.suit_effect_change_flag != 1)
			{
				p_ep.suit_effect_change_flag = 1;
				int new_id = LOGIC_CONFIG->GetSuitListConfig()->GetNewSuitByOldSuit(p_ep.suit_effect_id);
				p_ep.suit_effect_id = 0;
				p_ep.new_suit_effect_id = new_id;
				this->SetJewelryGridDirty(equip_index);
			}
		}
	}
}

void EquipmentManager::CHeckNewVocEatra()
{
	if (m_other_param.new_voc_extra_flag == 0)
	{
		this->ChangeNewVocEatra();
		m_module_mgr->GetKnapsack()->ChangeNewVocEatra();
		m_other_param.new_voc_extra_flag = 1;
	}
}

void EquipmentManager::ChangeNewVocEatra()
{
	{
		for (int equip_index = 0; equip_index < Equipment::E_INDEX_MAX; equip_index++)
		{
			if (m_equipment_list[equip_index].Invalid()) continue;
			const ItemBase * i_b = ITEMPOOL->GetItem(m_equipment_list[equip_index].item_id);
			if (NULL == i_b)continue;
			const Equipment * c_equip = (const Equipment*)i_b;
			EquipmentParam & p_ep = m_equipment_list[equip_index].param->equip_param;
			for (int i = 0; i < p_ep.inc_num && i < EquipmentParam::EXA_ATTR_NUM_MAX; i++)
			{
				AttrVocInfo &node_attr = p_ep.attr_extra_inc[i];
				const AttValueCfg * old_cfg = LOGIC_CONFIG->GetEntryConfig()->GetEntryAttrValueConfigBack(c_equip->GetEquipLevel(), node_attr.attr_type, node_attr.attr_value);
				if (NULL != old_cfg)//有符合的配置
				{
					//计算数值占用旧范围百分比
					double attr_persnt = ((double)(node_attr.attr_value - old_cfg->min) / (old_cfg->max - old_cfg->min));
					const AttValueCfg * new_cfg = LOGIC_CONFIG->GetEntryConfig()->GetEntryAttrValueConfig3(c_equip->GetEquipLevel(), old_cfg->quality, node_attr.attr_type);
					if (new_cfg != NULL)
					{
						int new_value = (int)((new_cfg->max - new_cfg->min) * attr_persnt) + new_cfg->min;
						gamelog::g_log_workshop.printf(LL_INFO, "%s equip_index[%d] user[%s,%d] voc_idx[%d] voc[attr_type:%d] [old_value:%d] [new_attr_value:%d]", __FUNCTION__,
							equip_index, m_module_mgr->GetRoleName(), m_module_mgr->GetUid(), i, node_attr.attr_type, node_attr.attr_value, new_value);
						node_attr.attr_value = new_value;
					}
				}
			}
			this->SetEquipGridDirty(equip_index);
		}
	}

	{
		for (int equip_index = 0; equip_index < Jewelry::J_TYPE_MAX; equip_index++)
		{
			if (m_jewelry_list[equip_index].Invalid()) continue;
			const ItemBase * i_b = ITEMPOOL->GetItem(m_jewelry_list[equip_index].item_id);
			if (NULL == i_b)continue;
			JewelryParam & p_ep = m_jewelry_list[equip_index].param->jewelry_param;
			const Jewelry * j_b = (const Jewelry *)i_b;
			for (int i = 0; i < p_ep.inc_num && i < JewelryParam::EXA_ATTR_NUM_MAX; i++)
			{
				AttrVocInfo &node_attr = p_ep.attr_extra_inc[i];
				const AttValueCfg * old_cfg = LOGIC_CONFIG->GetEntryConfig()->GetEntryAttrValueConfigBack2(j_b->GetJewelryLevel(), node_attr.attr_type, node_attr.attr_value);
				if (NULL != old_cfg)//有符合的配置
				{
					//计算数值占用旧范围百分比
					double attr_persnt = ((double)(node_attr.attr_value - old_cfg->min) / (old_cfg->max - old_cfg->min));
					const AttValueCfg * new_cfg = LOGIC_CONFIG->GetEntryConfig()->GetEntryAttrValueConfig4(j_b->GetJewelryLevel(), old_cfg->quality, node_attr.attr_type);
					if (new_cfg != NULL)
					{
						int new_value = (int)((new_cfg->max - new_cfg->min) * attr_persnt) + new_cfg->min;
						gamelog::g_log_workshop.printf(LL_INFO, "%s jewelry_index[%d] user[%s,%d] voc_idx[%d] voc[attr_type:%d] [old_value:%d] [new_attr_value:%d]", __FUNCTION__,
							equip_index, m_module_mgr->GetRoleName(), m_module_mgr->GetUid(), i, node_attr.attr_type, node_attr.attr_value, new_value);
						node_attr.attr_value = new_value;
					}
				}
			}

			this->SetJewelryGridDirty(equip_index);
		}
	}
}
