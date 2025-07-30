#include <memory.h>
#include "world.h"
#include "money.h"
#include "gamelog.h"
#include "knapsack.h"
#include "itembase.h"
#include "itempool.h"
#include "itemextern.hpp"
#include "engineadapter.h"
#include "expense/norexitem.hpp"
#include "scene/scene.h"
#include "scene/speciallogic.hpp"
#include "obj/character/role.h"
#include "protocol/msgitem.h"
#include "globalconfig/globalconfig.h"
#include "config/logicconfigmanager.hpp"
#include "config/otherconfig/knapsack_config.hpp"
#include "servercommon/errornum.h"
#include "servercommon/noticenum.h"
#include "servercommon/servercommon.h"
#include "servercommon/struct/itemlistparam.h"
#include "servercommon/string/gameworldstr.h"
#include "servercommon/courseofgrowthdef.h"
#include "servercommon/struct/skilllistparam.hpp"
#include "other/monitor/monitor.hpp"
#include "other/rolemodulemanager.hpp"
#include "other/shop/shopconfig.h"
#include "other/pet/pet.hpp"
#include "other/event/eventhandler.hpp"
#include "other/medal/medalconfig.hpp"
#include "other/currencyconfig.hpp"
#include "other/route/mailroute.hpp"
#include "other/funopen/funopendef.hpp"
#include "other/funopen/funopen.hpp"
#include "other/funopen/funopenconfig.hpp"
#include "protocol/msgother.h"
#include "other/surface/surfaceconfig.hpp"
#include "other/roledrop/roledrop.hpp"
#include "other/courseofgrowth/courseofgrowth.hpp"
#include "other/skill/roleskill.hpp"
#include "other/surface/surface.hpp"
#include "other/facescore/facescore.hpp"
#include "other/huashen/huashenmanager.hpp"
#include "other/workshop/workshop.hpp"
#include "other/roleattrpellet/roleattrpellet.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "global/randactivity/randactivitymanager.hpp"
#include "servercommon/uniqueidgenerator.hpp"
#include "item/elementcrystal/elementcrystal.hpp"
#include "global/randactivity/randactivityadapter.hpp"
#include "other/partner/partner.hpp"
#include "config/otherconfig/sundryconfig.hpp"
#include "config/otherconfig/scoresystemconfig.hpp"
#include "config/otherconfig/roleattrpelletconfig.hpp"
#include "equipment/equipmentmanager.h"
#include "servercommon/serverconfig/battleconfig/battle_special_effect_pool.hpp"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "other/workshop/godequipupconfig.hpp"
#include "other/workshop/workshopconfig.hpp"
#include "gameworld/equipment/godprintequipment.h"
#include "item/partnernatalcard/partnernatalcard.hpp"
#include "other/partner/partnerequipmentconfig.hpp"
#include "other/workshop/suitlistconfig.hpp"
#include "config/otherconfig/entryconfig.hpp"
#include "global/zhouqimanager/zhouqiconfig.hpp"
#include "global/zhouqimanager/zhouqimanager.h"
#include "other/title/title.hpp"
#include "other/zaohuaequip/zaohuaequipmanager.hpp"
#include "other/shengqi/holyequipmanager.hpp"
#include "other/workshop/gemstoneconfig.hpp"
#include "global/marketmanager/marketmanager.hpp"
#include "other/roleactivity/impl/roleactivityfortunemaze.hpp"
#include "other/roleactivity/roleactivitymanager.hpp"
#include "other/roleactivity/impl/roleactivitygodtoken.hpp"
#include "protocol/msgworkshop.h"
#include "other/equipstar/equipstarconfig.hpp"
#include "chat/chatmanager.h"
#include "item/dianhuashiitem/dianhuashiitem.hpp"
#include "other/dianhuashi/dianhuashiconfig.hpp"
#include "other/huanshou/rolehuanshou.hpp"

using namespace ItemNamespace;

int SortCompare(const void *arg1, const void *arg2)
{
	SortStruct *s1 = (SortStruct*)arg1;
	SortStruct *s2 = (SortStruct*)arg2;

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

//////////////////////////////////////////////////////////////////////////

Knapsack::Knapsack()
	: m_module_mgr(NULL),
	need_confirm(false),
	m_knapsack_is_change(false),
	m_knapsack_last_inorder_time(0),
	m_storage_last_inorder_time(0),
	m_last_inorder_merge_time(0),
	m_init_flag(0),
	m_clear_temporary_column_time(0u)
{
	memset(m_dirty_mark, 0, sizeof(m_dirty_mark));
	memset(m_old_state, 0, sizeof(m_old_state));
	memset(m_valid_grid_index_end_list, 0, sizeof(m_valid_grid_index_end_list));
	memset(m_curr_valid_grid_num_list, 0, sizeof(m_curr_valid_grid_num_list));
	memset(m_grid_list, 0, sizeof(m_grid_list));
	memset(m_extend_column_times_list, 0, sizeof(m_extend_column_times_list));

#ifdef _DEBUG
	static int STORAGE_UPBOUND = ITEM_COLUMN_GRID_NUM_UPBOUND_LIST[ITEM_COLUMN_TYPE_STORAGE];
	static int STORAGE_LOWBOUND = ITEM_COLUMN_GRID_NUM_LOWBOUND_LIST[ITEM_COLUMN_TYPE_STORAGE];
	assert(MAX_STORAGE_PAGE_NUM * STORAGE_GRID_NUM_EACH_PAGE == STORAGE_UPBOUND);
	assert(STORAGE_GRID_NUM_EACH_PAGE == STORAGE_LOWBOUND);
#endif
}

Knapsack::~Knapsack()
{
	for (int column = ItemNamespace::ITEM_COLUMN_TYPE_MIN; column < ItemNamespace::ITEM_COLUMN_TYPE_MAX; ++column)
	{
		for (int i = 0; i < ITEM_COLUMN_GRID_NUM_MAX; ++i)
		{
			if (NULL != m_grid_list[column][i])
			{
				this->ClearGrid(column, i);
			}
		}
	}
}

void Knapsack::Init(RoleModuleManager* module_mgr, int param_list_num, const ItemListParam::ItemListData *param_data_list[], const KnapsackOtherParam& other_param)
{
	m_module_mgr = module_mgr;

	for (int i = ITEM_COLUMN_TYPE_MIN; i < ITEM_COLUMN_TYPE_MAX; ++i)
	{
		m_curr_valid_grid_num_list[i] = other_param.curr_valid_grid_num_list[i];
		m_extend_column_times_list[i] = other_param.extend_times_list[i];
	}
	m_init_flag = other_param.init_flag;
	this->CheckColumnInitialGridNum();

	static int biggest_knapsack_index_list[ITEM_COLUMN_TYPE_MAX];
	memset(biggest_knapsack_index_list, -1, sizeof(biggest_knapsack_index_list));

	for (int i = param_list_num - 1; i >= 0; --i)
	{
		if (NULL == param_data_list[i]) break;

		int index = param_data_list[i]->index;
		int column = 0;
		int real_index = 0;
		if (!GetRealIndexColumn(index, &column, &real_index))
		{
			continue;
		}

		if (column < ITEM_COLUMN_TYPE_MIN || column >= ITEM_COLUMN_TYPE_MAX)
		{
			gamelog::g_log_serious_error.printf(LL_CRITICAL, "[Knapsack::Init] role[%s %d] invalid index,  index[%d] column[%d]",
				m_module_mgr->GetRole()->GetName(), UidToInt(m_module_mgr->GetRole()->GetUserId()), index, column);
			continue;
		}

		if (NULL != m_grid_list[column][real_index])
		{
			gamelog::g_log_serious_error.printf(LL_CRITICAL, "[Knapsack::Init Duplicate] role[%s %d] index[%d] column[%d] real_index[%d] old_item_id[%d] new_item_id[%d] old_has_param[%d]",
				m_module_mgr->GetRole()->GetName(), UidToInt(m_module_mgr->GetRole()->GetUserId()), index, column, real_index,
				(int)m_grid_list[column][real_index]->item_id, (int)param_data_list[i]->item_wrapper.item_id,
				(int)(NULL != m_grid_list[column][real_index]->param));
			continue;
		}

		ItemID item_id = param_data_list[i]->item_wrapper.item_id;
		int num = param_data_list[i]->item_wrapper.num;

		const ItemBase *itembase = ITEMPOOL->GetItem(item_id);
		if (NULL == itembase || num <= 0)
		{
			gamelog::g_log_serious_error.printf(LL_CRITICAL, "[Knapsack::Init Error][[%s %d] index:%d column[%d] real_index[%d] item_id:%d num:%d]", m_module_mgr->GetRole()->GetName(), UidToInt(m_module_mgr->GetRole()->GetUserId()),
				index, column, real_index, (int)item_id, (int)num);
			continue;
		}

		if (num > MAX_INT16)
		{
			gamelog::g_log_serious_error.printf(LL_CRITICAL, "[Knapsack::Init Error][[%s %d] index:%d column[%d] real_index[%d] item_id:%d num:%d >= MAX_INT16, num assigned to MAX_INT16, discard_num:%d]", m_module_mgr->GetRole()->GetName(), UidToInt(m_module_mgr->GetRole()->GetUserId()),
				index, column, real_index, (int)item_id, (int)num, num - MAX_INT16);

			num = MAX_INT16;
		}

		if (num > itembase->GetPileLimit())
		{
			gamelog::g_log_serious_error.printf(LL_WARNING, "[Knapsack::Init Error][[%s %d] index:%d column[%d] real_index[%d] item_id:%d num:%d >= pile_limit[%d]]", m_module_mgr->GetRole()->GetName(), UidToInt(m_module_mgr->GetRole()->GetUserId()),
				index, column, real_index, (int)item_id, (int)num, itembase->GetPileLimit());
		}

		if (0 != param_data_list[i]->item_wrapper.has_param && itembase->GetItemType() < I_TYPE_NET_VALUE_ITEM_BEGIN)
		{
			gamelog::g_log_serious_error.printf(LL_CRITICAL, "[Knapsack::Init HasParamButIsNotNetValueItem][[%s %d] index:%d column[%d] real_index[%d] item_id:%d]", m_module_mgr->GetRole()->GetName(), UidToInt(m_module_mgr->GetRole()->GetUserId()),
				index, column, real_index, (int)item_id);
			continue;
		}

		m_grid_list[column][real_index] = new ItemGridData();
		if (0 != param_data_list[i]->item_wrapper.has_param)
		{
			m_grid_list[column][real_index]->param = ItemBase::CreateParamFromParamData(param_data_list[i]->item_wrapper.param_data);
		}

		m_grid_list[column][real_index]->item_id = item_id;
		m_grid_list[column][real_index]->num = num;
		m_grid_list[column][real_index]->is_bind = (0 != param_data_list[i]->item_wrapper.is_bind ? true : false);
		m_grid_list[column][real_index]->invalid_time = param_data_list[i]->item_wrapper.invalid_time;
		m_grid_list[column][real_index]->gold_price = param_data_list[i]->item_wrapper.gold_price;
		m_grid_list[column][real_index]->item_unique_id = param_data_list[i]->item_wrapper.item_unique_id;

		if (NULL != itembase && m_grid_list[column][real_index]->param != NULL)
		{
			if (I_TYPE_MEDAL == itembase->GetItemType())
			{
				MedalItemParam & medal_param = m_grid_list[column][real_index]->param->medal_param;
				medal_param.CheckModifyVersion();
			}
			else if (I_TYPE_EQUIPMENT == itembase->GetItemType())
			{
				EquipmentParam & equip_param = m_grid_list[column][real_index]->param->equip_param;
				equip_param.CheckModifyVersion();
				const Equipment * equip = (const Equipment *)itembase;
				{
					//检查套装效果是否符合对应装备id
					const StrengthenCfg *change_cfg = GODEQUIP_CONFIG->GetStrengthenCfg(item_id, equip_param.strengthen_level);
					if (NULL != change_cfg)
					{
						if (change_cfg->suit_id != equip_param.new_suit_effect_id)
						{
							equip_param.suit_effect_id = 0;//清空旧配置
							gamelog::g_log_item_track.printf(LL_CRITICAL, "[Knapsack::Init StructDataError][[%s %d] column[%d] real_index[%d] item_id:%d] old_suit_id[%d] new_suit_id[%d]", m_module_mgr->GetRole()->GetName(), m_module_mgr->GetRoleUID(),
								column, real_index, (int)item_id, equip_param.new_suit_effect_id, change_cfg->suit_id);
							equip_param.new_suit_effect_id = change_cfg->suit_id;//修复不对的套装id
						}
					}
				}

				if (equip_param.inc_num > 0)
				{
					bool check_same = false;
					for (int attr_idx = 0; attr_idx < EquipmentParam::EXA_ATTR_NUM_MAX; attr_idx++)
					{
						AttrVocInfo &node_attr = equip_param.attr_extra_inc[attr_idx];
						if (node_attr.un_lock == 1)
						{
							if (node_attr.attr_value > 0 && !LOGIC_CONFIG->GetEntryConfig()->EntryGroupHasAttrTypr(equip->GetEntryGroup(), node_attr.attr_type))//无符合的配置
							{
								check_same = true;
							}
							for (int check_idx = attr_idx + 1; check_idx < EquipmentParam::EXA_ATTR_NUM_MAX; check_idx++)
							{
								if (node_attr.attr_value <= 0 || equip_param.attr_extra_inc[check_idx].attr_value <= 0)continue;
								if (node_attr.attr_type == equip_param.attr_extra_inc[check_idx].attr_type)
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
						int real_entry_num = LOGIC_CONFIG->GetEntryConfig()->GetEntryRandConfig(equip->GetEntryGroup(), equip_param.inc_num, entry_type_list, NULL);//获得新的词条列表
						if (real_entry_num > 0)
						{
							for (int new_voc_idx = 0; new_voc_idx < equip_param.inc_num && new_voc_idx < real_entry_num && new_voc_idx < EquipmentParam::EXA_ATTR_NUM_MAX; new_voc_idx++)
							{
								AttrVocInfo &node_attr = equip_param.attr_extra_inc[new_voc_idx];
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
										gamelog::g_log_workshop.printf(LL_INFO, "%s bag_colum[%d] bag_idx[%d] user[%s,%d] voc_idx[%d] voc[attr_type:%d] [old_value:%d] [new_attr_value:%d]", __FUNCTION__,
											column, real_index, m_module_mgr->GetRoleName(), m_module_mgr->GetUid(), i, node_attr.attr_type, node_attr.attr_value, new_value);
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
												gamelog::g_log_workshop.printf(LL_INFO, "%s bag_colum[%d] bag_idx[%d] user[%s,%d] voc_idx[%d] old_attr[type:%d value:%d] new_attr[type:%d value:%d]", __FUNCTION__,
													column, real_index, m_module_mgr->GetRoleName(), m_module_mgr->GetUid(), new_voc_idx, node_attr.attr_type, node_attr.attr_value, node_new_attr.att_type, attr_value);
												node_attr.attr_type = node_new_attr.att_type;
												node_attr.attr_value = attr_value;
											}
										}
									}
								}
							}
						}
						this->SetItemDirty(column, real_index, false, ITEM_CHANGE_NOTICE_REASON_DATA_CHANGE);
					}
				}
			}
			else if (I_TYPE_JEWELRY == itembase->GetItemType())
			{
				JewelryParam & jewelry_param = m_grid_list[column][real_index]->param->jewelry_param;
				jewelry_param.CheckModifyVersion();
				const Jewelry * jewelry = (const Jewelry *)itembase;
				{
					//检查套装效果是否符合对应装备id
					const StrengthenCfg *change_cfg = GODEQUIP_CONFIG->GetStrengthenCfg(item_id, jewelry_param.strengthen_level);
					if (NULL != change_cfg)
					{
						if (change_cfg->suit_id != jewelry_param.new_suit_effect_id)
						{
							jewelry_param.suit_effect_id = 0;//清空旧配置
							gamelog::g_log_item_track.printf(LL_CRITICAL, "[Knapsack::Init StructDataError][[%s %d] column[%d] real_index[%d] item_id:%d] old_suit_id[%d] new_suit_id[%d]", m_module_mgr->GetRole()->GetName(), m_module_mgr->GetRoleUID(),
								column, real_index, (int)item_id, jewelry_param.new_suit_effect_id, change_cfg->suit_id);
							jewelry_param.new_suit_effect_id = change_cfg->suit_id;//修复不对的套装id
						}
					}
				}

				if (jewelry_param.inc_num > 0)
				{
					bool check_same = false;
					for (int attr_idx = 0; attr_idx < EquipmentParam::EXA_ATTR_NUM_MAX; attr_idx++)
					{
						AttrVocInfo &node_attr = jewelry_param.attr_extra_inc[attr_idx];
						if (jewelry_param.attr_extra_inc[attr_idx].un_lock == 1)
						{
							if (node_attr.attr_value > 0 && !LOGIC_CONFIG->GetEntryConfig()->EntryGroupHasAttrTypr(jewelry->GetEntryGroup(), node_attr.attr_type))//无符合的配置
							{
								check_same = true;
							}

							for (int check_idx = attr_idx + 1; check_idx < EquipmentParam::EXA_ATTR_NUM_MAX; check_idx++)
							{
								if (node_attr.attr_value <= 0 || jewelry_param.attr_extra_inc[check_idx].attr_value <= 0)continue;
								if (node_attr.attr_type == jewelry_param.attr_extra_inc[check_idx].attr_type)
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
						int real_entry_num = LOGIC_CONFIG->GetEntryConfig()->GetEntryRandConfig(jewelry->GetEntryGroup(), jewelry_param.inc_num, entry_type_list, NULL);//获得新的词条列表
						if (real_entry_num > 0)
						{
							for (int new_voc_idx = 0; new_voc_idx < jewelry_param.inc_num && new_voc_idx < real_entry_num && new_voc_idx < EquipmentParam::EXA_ATTR_NUM_MAX; new_voc_idx++)
							{
								AttrVocInfo &node_attr = jewelry_param.attr_extra_inc[new_voc_idx];
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
										gamelog::g_log_workshop.printf(LL_INFO, "%s column[%d] real_index[%d] user[%s,%d] voc_idx[%d] voc[attr_type:%d] [old_value:%d] [new_attr_value:%d]", __FUNCTION__,
											column, real_index, m_module_mgr->GetRoleName(), m_module_mgr->GetUid(), new_voc_idx, node_attr.attr_type, node_attr.attr_value, new_value);
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
												gamelog::g_log_workshop.printf(LL_INFO, "%s column[%d] real_index[%d] user[%s,%d] voc_idx[%d] old_attr[type:%d value:%d] new_attr[type:%d value:%d]", __FUNCTION__,
													column, real_index, m_module_mgr->GetRoleName(), m_module_mgr->GetUid(), new_voc_idx, node_attr.attr_type, node_attr.attr_value, node_new_attr.att_type, attr_value);
												node_attr.attr_type = node_new_attr.att_type;
												node_attr.attr_value = attr_value;
											}
										}
									}
								}
							}
						}
						this->SetItemDirty(column, real_index, false, ITEM_CHANGE_NOTICE_REASON_DATA_CHANGE);
					}
				}
			}
		}

		if (NULL != itembase && itembase->GetItemType() == I_TYPE_PARTNER_NATAL_CARD && m_grid_list[column][real_index]->param != NULL)
		{
			const PartnerNatalCard * natal_card = (const PartnerNatalCard *)itembase;
			NewPartnerNatalCardParam & partner_card_param = m_grid_list[column][real_index]->param->partner_natal_card_param;
			//检查命格是否正确属性
			for (int i = 0; i < MAX_PARTNER_NATAL_CARD_VOC_NUM; i++)
			{
				if (partner_card_param.m_voc_list[i].attr_value == 0)continue;
				if (!LOGIC_CONFIG->GetPartnerEquipmentConfig()->CheckAttrTypeAndValue(m_grid_list[column][real_index]->item_id, i, partner_card_param.m_voc_list[i].attr_type, partner_card_param.m_voc_list[i].attr_value))
				{
					//属性对不上
					m_grid_list[column][real_index]->param->Reset();
					natal_card->RandomNetValueOnPutItem(m_module_mgr->GetRole(), m_grid_list[column][real_index]->param, PUT_REASON_NO_NOTICE);//重新再随机一次属性
					natal_card->SetRandomNetValueFlag(m_grid_list[column][real_index]->param);
					break;
				}
			}
		}

		if (0 == m_grid_list[column][real_index]->item_unique_id)
		{
			long long item_unique_id = 0;
			UniqueIDGenerator::Instance()->GenUniqueID(&item_unique_id);
			if (0 != item_unique_id)
			{
				m_grid_list[column][real_index]->item_unique_id = item_unique_id;
				this->SetItemDirty(column, real_index, false, ITEM_CHANGE_NOTICE_REASON_DATA_CHANGE);

#ifdef TRACK_ITEM_FLAG
				gamelog::g_log_item_track.printf(LL_INFO, "TRACK_ITEM_FLAG Knapsack::Init assign unique_id[%lld] to role[%d,%s] item[%d,%s] num[%d] from column[%d] index[%d]",
					item_unique_id, m_module_mgr->GetRole()->GetUID(), m_module_mgr->GetRole()->GetName(),
					item_id, itembase->GetItemName(), num, column, real_index);
#endif

			}
		}

		if (m_grid_list[column][real_index]->invalid_time > 0 && !itembase->HasInvalidTime())
		{
			m_grid_list[column][real_index]->invalid_time = 0;
		}

		m_old_state[column][real_index] = true;

		if (biggest_knapsack_index_list[column] < real_index && real_index < ITEM_COLUMN_GRID_NUM_MAX) // 记录下背包有效格子的最大序号
		{
			biggest_knapsack_index_list[column] = real_index;
		}

		gamelog::g_log_knapsack.printf(LL_INFO, "Knapsack::Init role[%d,%s] item[%d,%s] num[%d] from column[%d] index[%d] unique_id[%lld]",
			m_module_mgr->GetRole()->GetUID(), m_module_mgr->GetRole()->GetName(),
			item_id, itembase->GetItemName(), num, column, real_index, param_data_list[i]->item_wrapper.item_unique_id);


	}

	for (int i = ITEM_COLUMN_TYPE_MIN; i < ITEM_COLUMN_TYPE_MAX; ++i)
	{
		m_valid_grid_index_end_list[i] = biggest_knapsack_index_list[i] + 1;
	}

	unsigned int now = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	if (other_param.clear_temp_column_time < now)
	{
		bool has_clear_one = false;
		for (int i = 0; i < m_valid_grid_index_end_list[ITEM_COLUMN_TYPE_TEMPORARY]; ++i)
		{
			ItemGridData* grid = m_grid_list[ITEM_COLUMN_TYPE_TEMPORARY][i];
			if (NULL != grid)
			{
				has_clear_one = true;
				grid->Clear();
				this->SetItemDirty(ITEM_COLUMN_TYPE_TEMPORARY, i, true, ITEM_CHANGE_NOTICE_REASON_CONSUME);
			}
		}

		if (has_clear_one)
		{
			static MailContentParam contentparam; contentparam.Reset();
			int length1 = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_temp_clear_notice_subject);
			int length2 = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_temp_clear_notice_content);
			if (length1 > 0 && length2 > 0)
			{
				MailRoute::MailToUser(m_module_mgr->GetRole()->GetUserId(), MAIL_REASON_DEFAULT, contentparam);
			}
		}
	}

	m_clear_temporary_column_time = 0u;

	//创角赠送礼包
	m_novice_gift_flag = other_param.is_get_novice_gift;
	if (m_module_mgr->GetRole()->GetLevel() < 2 && m_novice_gift_flag != 1)
	{
		const ItemBase *itembase = ITEMPOOL->GetItem(LOGIC_CONFIG->GetSundryConfig()->GetCreateRoleReward());
		if (NULL == itembase) return;

		ItemDataWrapper temp_item_wrapper;
		temp_item_wrapper.item_id = itembase->GetItemId();
		temp_item_wrapper.num = 1;
		temp_item_wrapper.is_bind = itembase->IsBind();
		temp_item_wrapper.invalid_time = itembase->CalInvalidTime();

		NetValueItemParam tmp_item_param;
		tmp_item_param.SetData(temp_item_wrapper.param_data);
		if (!itembase->HasRandomNetValue(&tmp_item_param))
		{
			itembase->RandomNetValueOnPutItem(NULL, &tmp_item_param, PUT_REASON_CREATE_ROLE);
			itembase->SetRandomNetValueFlag(&tmp_item_param);
			tmp_item_param.GetDataByItemType(itembase->GetItemType(), temp_item_wrapper.param_data);
		}

		temp_item_wrapper.gold_price = itembase->GetSellPrice(&tmp_item_param);

		this->Put(temp_item_wrapper, PUT_REASON_CREATE_ROLE);
		m_novice_gift_flag = 1;
	}

}

void Knapsack::GetOtherInitParam(KnapsackOtherParam* other_param)
{
	// 每次保存数据都更新临时背包清空时间。
	m_clear_temporary_column_time = static_cast<unsigned int>(EngineAdapter::Instance().Time()) + 10 * 60 * 60;

	other_param->init_flag = m_init_flag;
	other_param->is_get_novice_gift = m_novice_gift_flag;
	other_param->clear_temp_column_time = m_clear_temporary_column_time;
	for (int i = ITEM_COLUMN_TYPE_MIN; i < ITEM_COLUMN_TYPE_MAX; ++i)
	{
		other_param->curr_valid_grid_num_list[i] = m_curr_valid_grid_num_list[i];
		other_param->extend_times_list[i] = m_extend_column_times_list[i];
	}
}

struct InvalidTimeItemInfo
{
	InvalidTimeItemInfo()
	{
		item_id = 0;
		num = 0;
		sell_price = 0;
	}

	int item_id;
	int num;
	int sell_price;
};

void Knapsack::Update(time_t now_second)
{
	RandActivityManager* ramgr = NULL;

	std::map<int, InvalidTimeItemInfo> invalid_item_map;  //过期的物品id对应的数量

	for (int column = ITEM_COLUMN_TYPE_MIN; column < ITEM_COLUMN_TYPE_MAX; ++column)
	{
		if (ITEM_COLUMN_TYPE_STORAGE == column) continue; // 轮询除了仓库栏外的其他背包格子

		for (int i = 0; i < m_valid_grid_index_end_list[column]; i++)
		{
			ItemGridData * curr = m_grid_list[column][i];
			if (NULL == curr || curr->Invalid()) continue;

			const ItemBase *item = ITEMPOOL->GetItem(curr->item_id);
			if (!item) continue;

			if (curr->invalid_time > 0 && now_second >= curr->invalid_time)
			{
				int sell_price = item->GetSellPrice(curr->param) * curr->num;
				InvalidTimeItemInfo & info = invalid_item_map[curr->item_id];
				info.item_id = curr->item_id;
				info.num += curr->num;
				info.sell_price += sell_price;

#ifdef TRACK_ITEM_FLAG
				gamelog::g_log_item_track.printf(LL_INFO, "TRACK_ITEM_FLAG Knapsack::Update item invalid CLEAR_GRID role[%d,%s] item[%d] unique_id[%lld]",
								 m_module_mgr->GetRole()->GetUID(), m_module_mgr->GetRole()->GetName(), curr->item_id, curr->item_unique_id);
#endif

				this->ClearGrid(column, i);
				this->SetItemDirty(column, i, true, ITEM_CHANGE_NOTICE_REASON_CONSUME);

				continue;
			}

			if (this->CheckZhouQiItemExpired(curr,item))
			{
#ifdef TRACK_ITEM_FLAG
				gamelog::g_log_item_track.printf(LL_INFO, "TRACK_ITEM_FLAG Knapsack::Update zhou_qi_item CLEAR_GRID role[%d,%s] item[%d] unique_id[%lld]",
					m_module_mgr->GetRole()->GetUID(), m_module_mgr->GetRole()->GetName(), m_grid_list[column][i]->item_id, m_grid_list[column][i]->item_unique_id);
#endif


				this->ClearGrid(column, i);
				this->SetItemDirty(column, i, true, ITEM_CHANGE_NOTICE_REASON_CONSUME);

				continue;
			}

			const InvalidTimeItemCfg * item_limit_cfg = LOGIC_CONFIG->GetRandActivityConfigManager()->GetGroupListActivityId(item->GetItemId());
			if (item_limit_cfg == NULL)continue;

			if (item_limit_cfg->activity_id == 0 || m_grid_list[column][i]->num == 0) continue;
			if (NULL == ramgr)
			{
				ramgr = RandActivityAdapter::Instance()->GetRAManager(m_module_mgr->GetRole()->GetUniqueServerID());
			}

			//判断对应活动是否开启
			if (NULL != ramgr && !ramgr->IsRandActivityOpen(item_limit_cfg->activity_id))
			{
				if (CrossConfig::Instance().IsHiddenServer() && item_limit_cfg->local_activity)continue;//本服活动类型不在跨服检查超时道具
				static MailContentParam contentparam;
				int length1 = 0;
				int length2 = 0;

				//发放邮件
				int sell_price = item->GetSellPrice(m_grid_list[column][i]->param) * m_grid_list[column][i]->num;

				contentparam.Reset();
				length1 = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_item_expire_notice_subject);
				length2 = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_item_expire_notice_content, item_limit_cfg->activity_id, item->GetItemId(), m_grid_list[column][i]->num);

				contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_BIND_COIN] = sell_price;
				if (length1 > 0 && length2 > 0)
				{

					MailRoute::MailToUser(m_module_mgr->GetRole()->GetUserId(), MAIL_REASON_DEFAULT, contentparam);
				}
#ifdef TRACK_ITEM_FLAG
				gamelog::g_log_item_track.printf(LL_INFO, "TRACK_ITEM_FLAG Knapsack::Update activity_item CLEAR_GRID role[%d,%s] item[%d] unique_id[%lld]",
					m_module_mgr->GetRole()->GetUID(), m_module_mgr->GetRole()->GetName(), m_grid_list[column][i]->item_id, m_grid_list[column][i]->item_unique_id);
#endif

				this->ClearGrid(column, i);
				this->SetItemDirty(column, i, true, ITEM_CHANGE_NOTICE_REASON_CONSUME);
			}
		}
	}

	if (!invalid_item_map.empty())
	{
		//进入这里不能返回，上面已经将过期物品清了
		std::string str;
		int bind_coin = 0;
		for (std::map<int, InvalidTimeItemInfo>::iterator iter = invalid_item_map.begin(); iter != invalid_item_map.end(); ++iter)
		{
			const InvalidTimeItemInfo & curr = iter->second;
			bind_coin += curr.sell_price;

			if (!str.empty())
			{
				str += ",";
			}

			str += STRING_SPRINTF("{i;%d}*%d", curr.item_id, curr.num);
		}


		static MailContentParam contentparam;
		contentparam.Reset();
		int length1 = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_item_expire_notice_subject_2);
		int length2 = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_item_expire_notice_content_2, str.c_str());
		contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_BIND_COIN] = bind_coin;
		MailRoute::MailToUser(m_module_mgr->GetRole()->GetUserId(), MAIL_REASON_DEFAULT, contentparam);
	}


}

void Knapsack::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	if (old_dayid == now_dayid) return;

	if (FunOpen::IsFunOpen(m_module_mgr->GetRole(), FUN_TYPE_EXP_CRYSTAL))
	{
		int day_add_num = LOGIC_CONFIG->GetCurrencyconfig()->GetExpCrystalCfg()->exp_crystal_day_count;
		int max_num = LOGIC_CONFIG->GetCurrencyconfig()->GetExpCrystalCfg()->exp_crystal_max_count;
		int cur_remain_times = m_module_mgr->GetCommonData().exp_crytal_remain_times;
		m_module_mgr->GetCommonData().exp_crytal_remain_times = (short)GetMin(day_add_num + cur_remain_times, max_num);
		m_module_mgr->GetCommonData().exp_crytal_remain_times = (short)GetMax(m_module_mgr->GetCommonData().exp_crytal_remain_times, 0);
		this->SendExpCrystalInfo();
	}
}

void Knapsack::OnLevelUp(int new_level)
{
	const FunOpenCfg* cfg = LOGIC_CONFIG->GetFunOpenConfig()->GetFunOpenCfg(FUN_TYPE_EXP_CRYSTAL);
	if (NULL == cfg) return;

	if (new_level == cfg->limit_level)
	{
		m_module_mgr->GetCommonData().is_open_exp_crytal = 1;
		m_module_mgr->GetCommonData().exp_crytal_remain_times = LOGIC_CONFIG->GetCurrencyconfig()->GetExpCrystalCfg()->exp_crystal_day_count;
		this->SendExpCrystalInfo();
	}
}

void Knapsack::OnLogin()
{
	this->CheckItemInWrongColumn();
	this->CheckItemReturn();
	this->CheckEquipZhuLing();
}

void Knapsack::EquipGemMounting(int stone_type, int equip_grid, int stone_grid)
{
	ItemGridData * tmp_equip = NULL;
	ItemGridData * tmp_stone = NULL;
	
	if (equip_grid < 0 || equip_grid >= ITEM_COLUMN_GRID_NUM_MAX) return;
	if (stone_grid < 0 || stone_grid >= ITEM_COLUMN_GRID_NUM_MAX) return;
	tmp_equip = m_grid_list[ItemNamespace::ITEM_COLUMN_TYPE_ITEM][equip_grid];
	tmp_stone = m_grid_list[ItemNamespace::ITEM_COLUMN_TYPE_ITEM][stone_grid];

	if (NULL == tmp_equip || NULL == tmp_stone)return;

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

	const GemstoneBasicInfoCfg * mate_gem = LOGIC_CONFIG->GetGemstoneConfig()->GetGemstoneBasicInfoCfg(tmp_stone->item_id);
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
	ItemID get_stone_id = tmp_stone->item_id;
	// 消耗宝石
	if (!this->ConsumeItemByIndex(ItemNamespace::ITEM_COLUMN_TYPE_ITEM, stone_grid, 1, __FUNCTION__))
	{
		m_module_mgr->NoticeNum(errornum::EN_STUFF_NOT_ENOUGH);
		return;
	}

	// 消耗成功，生效
	if (false == tmp_equip->is_bind) tmp_equip->is_bind = true;
	stone_id[t_cur] = get_stone_id;

	ROLE_LOG_QUICK6(LOG_TYPE_GEM_INLAY, m_module_mgr->GetRole(), tmp_equip->item_id, tmp_stone->item_id, "GemMount", NULL);

	this->SetItemDirty(ItemNamespace::ITEM_COLUMN_TYPE_ITEM, equip_grid, true, Knapsack::ITEM_CHANGE_NOTICE_REASON_DATA_CHANGE);

	gamelog::g_log_knapsack.printf(LL_INFO, "%s user[%d,%s] weapon_id{[%d] param: [%s]}", __FUNCTION__,
		m_module_mgr->GetUid(), m_module_mgr->GetRole()->GetName(),
		tmp_equip->item_id, tmp_equip->param->GetParamLog(equip_ib->GetItemType()));
	EventHandler::Instance().OnGemMounting(m_module_mgr->GetRole(), tmp_stone->item_id);
}

void Knapsack::EquipGemDismount(int stone_type, int equip_grid, int stone_slot)
{
	if (stone_slot < 0 || stone_slot >= NEW_EQUIP_STONE_MOUNT_MAX) return;

	ItemGridData * tmp_equip;

	if (equip_grid < 0 || equip_grid >= ITEM_COLUMN_GRID_NUM_MAX) return;
	tmp_equip = m_grid_list[ItemNamespace::ITEM_COLUMN_TYPE_ITEM][equip_grid];

	if (NULL == tmp_equip)return;

	if (tmp_equip->Invalid()) return;

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

	if (!this->Put(ItemConfigData(target_gem, true, 1), PUT_REASON_NO_NOTICE)) return;

	gamelog::g_log_workshop.printf(LL_INFO, "Workshop::EquipGemDismount user[%d,%s] weapon_id{[%d] param: [%s]}",
		m_module_mgr->GetUid(), m_module_mgr->GetRole()->GetName(),
		tmp_equip->item_id, tmp_equip->param->GetParamLog(equip_ib->GetItemType()));

	EventHandler::Instance().OnGemDismount(m_module_mgr->GetRole(), target_gem);

	ROLE_LOG_QUICK6(LOG_TYPE_GEM_INLAY, m_module_mgr->GetRole(), tmp_equip->item_id, target_gem, "GemDismount", NULL);

	// 宝石已放入背包，注意不能中途返回
	target_gem = 0;
	this->SetItemDirty(ItemNamespace::ITEM_COLUMN_TYPE_ITEM, equip_grid, true, Knapsack::ITEM_CHANGE_NOTICE_REASON_DATA_CHANGE);
}

void Knapsack::EquipGemUpgrades(int stone_type, int equip_grid, int stone_slot)
{
	if (stone_slot < 0 || stone_slot >= NEW_EQUIP_STONE_MOUNT_MAX) return;

	ItemGridData * tmp_equip;
	if (equip_grid < 0 || equip_grid >= ITEM_COLUMN_GRID_NUM_MAX) return;
	tmp_equip = m_grid_list[ItemNamespace::ITEM_COLUMN_TYPE_ITEM][equip_grid];

	if (NULL == tmp_equip || tmp_equip->Invalid())return;

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

void Knapsack::ConfirmGemUpgrade()
{
	if (!need_confirm)return;
	if (m_gem_upgrade_param.stone_type < WORKSHOP_STONE_TYPE_EQUIP || m_gem_upgrade_param.stone_type > WORKSHOP_STONE_TYPE_JEWELRY)
	{
		//m_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	if (m_gem_upgrade_param.stone_slot < 0 || m_gem_upgrade_param.stone_slot >= NEW_EQUIP_STONE_MOUNT_MAX)
	{
		m_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	ItemGridData * tmp_equip;
	if (m_gem_upgrade_param.equip_grid < 0 || m_gem_upgrade_param.equip_grid >= ITEM_COLUMN_GRID_NUM_MAX) return;
	tmp_equip = m_grid_list[ItemNamespace::ITEM_COLUMN_TYPE_ITEM][m_gem_upgrade_param.equip_grid];

	if (NULL == tmp_equip || tmp_equip->Invalid())return;

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

		if (consume_list.count > 0 && !this->ConsumeItemByIndexList(consume_list.count, consume_list.column_list, consume_list.index_list, consume_list.num_list, "WorkshopCompoundItem")) return;
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
	this->SetItemDirty(ItemNamespace::ITEM_COLUMN_TYPE_ITEM, m_gem_upgrade_param.equip_grid, true, Knapsack::ITEM_CHANGE_NOTICE_REASON_DATA_CHANGE);

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
		if (NULL != god_token)
		{
			god_token->OnRAGodTokenAddCount(RA_GOD_TOKEN_DAY_TYPE_SHOP_BUY_GEM);
			god_token->OnRAGodTokenWeekAddCount(RA_GOD_TOKEN_TYPE_WEEK_SHOP_BUY_GEM);
		}
	}
	need_confirm = false;
}

void Knapsack::EquipUpStar(int seq, int equip_type, int equip_grid, int cost_grid, int cost_grid_2)
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

	if (equip_grid < 0 || equip_grid >= ITEM_COLUMN_GRID_NUM_MAX) return;
	tmp_equip = m_grid_list[ItemNamespace::ITEM_COLUMN_TYPE_ITEM][equip_grid];

	if (tmp_equip == NULL || tmp_equip->Invalid() || NULL == tmp_equip->param)return;

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
	if (!this->GetItemGridData(ItemNamespace::ITEM_COLUMN_TYPE_ITEM, cost_grid, &cost_tmp1) ||
		!this->GetItemGridData(ItemNamespace::ITEM_COLUMN_TYPE_ITEM, cost_grid_2, &cost_tmp2))
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

	bool ret1 = this->ConsumeItemByIndex(ItemNamespace::ITEM_COLUMN_TYPE_ITEM, cost_grid, 1, __FUNCTION__);
	bool ret2 = this->ConsumeItemByIndex(ItemNamespace::ITEM_COLUMN_TYPE_ITEM, cost_grid_2, 1, __FUNCTION__);
	if (!ret1 || !ret2)
	{
		//消耗失败记录
		gamelog::g_log_workshop.printf(LL_INFO, "%s Equipment user[%d,%s] ConsumeItemByIndex false ret1[%d] cost_grid_1[item_id(%d),cost_grid(%d)] ret2[%d] cost_grid_2[item_id(%d),cost_grid(%d)]",
			__FUNCTION__, m_module_mgr->GetUid(), m_module_mgr->GetRole()->GetName(), ret1 ? 1 : 0, cost_tmp1.item_id, cost_grid,
			ret2 ? 1 : 0, cost_tmp2.item_id, cost_grid_2);
	}

	tmp_equip->item_id = up_star_cfg->compose_id;

	this->SetItemDirty(ItemNamespace::ITEM_COLUMN_TYPE_ITEM, equip_grid, true, Knapsack::ITEM_CHANGE_NOTICE_REASON_DATA_CHANGE);

	if (equip_type)
	{
		if (equip_type < Equipment::E_TYPE_MIN || equip_type >= Equipment::E_TYPE_MAX) return;
		int equip_index = Equipment::E_TYPE_MAX - equip_type - 1;
		m_module_mgr->GetEquipmentManager()->SynItemDataPut(I_TYPE_EQUIPMENT, equip_index, tmp_equip);
	}
	else
	{
		if (equip_type < Jewelry::J_TYPE_GEM_RING || equip_type >= Jewelry::J_TYPE_MAX) return;
		int equip_index = Jewelry::J_TYPE_MAX - equip_type - 1;
		m_module_mgr->GetEquipmentManager()->SynItemDataPut(I_TYPE_JEWELRY, equip_index, tmp_equip);
	}
}

void Knapsack::UpEquipGrade(int equip_type, int equip_grid, int role_type)
{
	ItemGridData * tmp_equip = NULL;

	if (equip_grid < 0 || equip_grid >= ITEM_COLUMN_GRID_NUM_MAX) return;
	tmp_equip = m_grid_list[ItemNamespace::ITEM_COLUMN_TYPE_ITEM][equip_grid];

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

		if (consume_list.count > 0 && !this->ConsumeItemByIndexList(consume_list.count, consume_list.column_list, consume_list.index_list, consume_list.num_list, __FUNCTION__)) return;
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

	this->SetItemDirty(ItemNamespace::ITEM_COLUMN_TYPE_ITEM, equip_grid, true, Knapsack::ITEM_CHANGE_NOTICE_REASON_DATA_CHANGE);
	return;
}

void Knapsack::BreakThroughGarde(int equip_type, int equip_grid, int role_type)
{
	ItemGridData * tmp_equip = NULL;

	if (equip_grid < 0 || equip_grid >= ITEM_COLUMN_GRID_NUM_MAX) return;
	tmp_equip = m_grid_list[ItemNamespace::ITEM_COLUMN_TYPE_ITEM][equip_grid];

	if (tmp_equip == NULL || tmp_equip->Invalid() || NULL == tmp_equip->param)return;

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

		if (consume_list.count > 0 && !this->ConsumeItemByIndexList(consume_list.count, consume_list.column_list, consume_list.index_list, consume_list.num_list, __FUNCTION__)) return;
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
	
	this->SetItemDirty(ItemNamespace::ITEM_COLUMN_TYPE_ITEM, equip_grid, true, Knapsack::ITEM_CHANGE_NOTICE_REASON_DATA_CHANGE);

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

void Knapsack::DianHuaEquip(int equip_grid, int dianhuashi_coloumn, int dianhuashi_grid)
{
	const ItemGridData * tmp_equip = NULL;
	if (equip_grid < 0 || equip_grid >= ITEM_COLUMN_GRID_NUM_MAX) return;
	tmp_equip = m_grid_list[ItemNamespace::ITEM_COLUMN_TYPE_ITEM][equip_grid];

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
	this->SetItemDirty(dianhuashi_coloumn, equip_grid, true, Knapsack::ITEM_CHANGE_NOTICE_REASON_DATA_CHANGE);
}

void Knapsack::SaveDianHuaEquip(int equip_grid)
{
	ItemGridData * tmp_equip = NULL;
	if (equip_grid < 0 || equip_grid >= ITEM_COLUMN_GRID_NUM_MAX) return;
	tmp_equip = m_grid_list[ItemNamespace::ITEM_COLUMN_TYPE_ITEM][equip_grid];

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

	this->SetItemDirty(ItemNamespace::ITEM_COLUMN_TYPE_ITEM, equip_grid, true, Knapsack::ITEM_CHANGE_NOTICE_REASON_DATA_CHANGE);
}

void Knapsack::CalcGemOneKeyUpgradeNeed(std::map<ItemID, int>& owned_list)
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

void Knapsack::SendGemOneKeyUpgradeNeed()
{
	static Protocol::SCGemOneKeyUpgradesNeeds gokun;

	gokun.mate_data = m_gem_upgrade_param.ug_data;

	m_module_mgr->NetSend((const char *)&gokun, sizeof(gokun));
}

void Knapsack::XiFuEquip(int equip_grid)
{
	ItemGridData * tmp_equip = NULL;
	if (equip_grid < 0 || equip_grid >= ITEM_COLUMN_GRID_NUM_MAX) return;
	tmp_equip = m_grid_list[ItemNamespace::ITEM_COLUMN_TYPE_ITEM][equip_grid];

	if (tmp_equip == NULL || tmp_equip->Invalid())return;

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

	this->SetItemDirty(ItemNamespace::ITEM_COLUMN_TYPE_ITEM, equip_grid, true, Knapsack::ITEM_CHANGE_NOTICE_REASON_DATA_CHANGE);
}

void Knapsack::FuMoEquip(int equip_grid, int * new_spid, int * new_level)
{
	ItemGridData * tmp_equip = NULL;
	if (equip_grid < 0 || equip_grid >= ITEM_COLUMN_GRID_NUM_MAX) return;
	tmp_equip = m_grid_list[ItemNamespace::ITEM_COLUMN_TYPE_ITEM][equip_grid];

	if (tmp_equip == NULL || tmp_equip->Invalid())return;

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

	this->SetItemDirty(ItemNamespace::ITEM_COLUMN_TYPE_ITEM, equip_grid, true, Knapsack::ITEM_CHANGE_NOTICE_REASON_DATA_CHANGE);
}

void Knapsack::ComposeDianHuaShi(int main_stone_idx, int cost_stone_1, int cost_stone_2)
{
	if (main_stone_idx == cost_stone_1 || main_stone_idx == cost_stone_2 || cost_stone_1 == cost_stone_2)return;
	ItemGridData * main_tmp_equip = NULL;
	if (main_stone_idx < 0 || main_stone_idx >= ITEM_COLUMN_GRID_NUM_MAX) return;
	main_tmp_equip = m_grid_list[ItemNamespace::ITEM_COLUMN_TYPE_ITEM][main_stone_idx];

	ItemGridData * cost1_tmp_equip = NULL;
	if (cost_stone_1 < 0 || cost_stone_1 >= ITEM_COLUMN_GRID_NUM_MAX) return;
	cost1_tmp_equip = m_grid_list[ItemNamespace::ITEM_COLUMN_TYPE_ITEM][cost_stone_1];

	ItemGridData * cost2_tmp_equip = NULL;
	if (cost_stone_2 < 0 || cost_stone_2 >= ITEM_COLUMN_GRID_NUM_MAX) return;
	cost2_tmp_equip = m_grid_list[ItemNamespace::ITEM_COLUMN_TYPE_ITEM][cost_stone_2];

	if (main_tmp_equip == NULL || cost1_tmp_equip == NULL || cost2_tmp_equip == NULL)return;
	
	if (main_tmp_equip->Invalid() || cost1_tmp_equip->Invalid() || cost2_tmp_equip->Invalid())return;

	const ItemBase * main_ib = ITEMPOOL->GetItem(main_tmp_equip->item_id);
	const ItemBase * cost1_ib = ITEMPOOL->GetItem(cost1_tmp_equip->item_id);
	const ItemBase * cost2_ib = ITEMPOOL->GetItem(cost2_tmp_equip->item_id);

	if (main_ib == NULL || cost1_ib == NULL || cost2_ib == NULL)return;

	if (main_ib->GetItemType() != I_TYPE_DIAN_HUA_SHI || cost1_ib->GetItemType() != I_TYPE_DIAN_HUA_SHI || cost2_ib->GetItemType() != I_TYPE_DIAN_HUA_SHI)return;

	int & main_level = main_tmp_equip->param->dian_hua_shi_param.effect_level;
	int cost1_level = cost1_tmp_equip->param->dian_hua_shi_param.effect_level;
	int cost2_level = cost2_tmp_equip->param->dian_hua_shi_param.effect_level;
	if (main_level != cost1_level || main_level != cost2_level)
	{
		m_module_mgr->NoticeNum(errornum::EN_DIANHUASHI_ERR_NOT_SAME_STONE);
		return;
	}
	bool ret1 = this->ConsumeItemByIndex(ItemNamespace::ITEM_COLUMN_TYPE_ITEM, cost_stone_1, 1, __FUNCTION__);
	bool ret2 = this->ConsumeItemByIndex(ItemNamespace::ITEM_COLUMN_TYPE_ITEM, cost_stone_2, 1, __FUNCTION__);
	if (!ret1 || !ret2)
	{
		return;
	}

	main_level += 1;

	this->SetItemDirty(ItemNamespace::ITEM_COLUMN_TYPE_ITEM, main_stone_idx, true, Knapsack::ITEM_CHANGE_NOTICE_REASON_DATA_CHANGE);
}

bool Knapsack::Use(short column, short index, short num, short param1, short param2, short param3, int  item_param_type, void * param)
{
	if (column < ITEM_COLUMN_TYPE_MIN || column >= ITEM_COLUMN_TYPE_MAX) return false;

	if (num <= 0 || index < 0 || index >= ITEM_COLUMN_GRID_NUM_MAX) return false;

	if (NULL == m_grid_list[column][index]) return false;

	ItemGridData *item_data = m_grid_list[column][index];
	if (item_data->Invalid())
	{
		this->SetItemDirty(column, index, true, ITEM_CHANGE_NOTICE_REASON_CONSUME);
		m_module_mgr->GetRole()->NoticeNum(errornum::EN_KNAPSACK_USE_ITEM_ERROR);

		this->ClearGrid(column, index);

		return false;
	}

	if (NULL != m_module_mgr->GetRole()->GetScene() && !m_module_mgr->GetRole()->GetScene()->GetSpecialLogic()->CanUseItem(m_module_mgr->GetRole(), item_data->item_id, num))
	{
		return false;
	}

	if (item_data->num < num)
	{
		m_module_mgr->GetRole()->NoticeNum(errornum::EN_KNAPSACK_USE_ITEM_NUM_NOT_ENOUGH);
		return false;
	}

	const ItemBase *itemimp = ITEMPOOL->GetItem(item_data->item_id);
	if (NULL == itemimp)
	{
		m_module_mgr->GetRole()->NoticeNum(errornum::EN_KNAPSACK_LOAD_ERROR);
		return false;
	}

	if (NULL != item_data->param) // 如果有净值param 则检查物品是否确实是净值物品
	{
		if (itemimp->GetItemType() < I_TYPE_NET_VALUE_ITEM_BEGIN)
		{
			m_module_mgr->GetRole()->NoticeNum(errornum::EN_KNAPSACK_USE_ITEM_ERROR);
			return false;
		}

		item_data->num = 1; // 此处置为1  是为了下文装备使用后数量确实为0
		this->SetItemDirty(column, index, false, ITEM_CHANGE_NOTICE_REASON_CONSUME);
	}
	else // 如果没有净值参数 必须保证它一定不能是净值物品 
	{
		if (itemimp->GetItemType() >= I_TYPE_NET_VALUE_ITEM_BEGIN)
		{
			m_module_mgr->GetRole()->NoticeNum(errornum::EN_KNAPSACK_USE_ITEM_ERROR);
			return false;
		}
	}

	if (m_module_mgr->GetRole()->GetLevel() < itemimp->GetLimitLevel())
	{
		m_module_mgr->GetRole()->NoticeNum(errornum::EN_ITEM_LEVEL_LIMIT);
		return false;
	}

	if (!m_module_mgr->GetRole()->IsProfSuitable((char)itemimp->GetLimitProf()))
	{
		m_module_mgr->NoticeNum(errornum::EN_ITEM_PROF_LIMIT);
		return false;
	}

	/* 判断物品的性别限制
	if (!m_role->IsSexSuitable((char)itemimp->GetLimitSex()))
	{
	m_role->NoticeNum(errornum::EN_ITEM_SEX_LIMIT);
	return false;
	}
	*/

	bool item_is_bind = item_data->is_bind;

	bool need_set_dirty = false;
	int ret = itemimp->Use(m_module_mgr->GetRole(), *item_data, num, param1, param2, param3, need_set_dirty, item_param_type, param);	// 实际使用数量会在传出后修正
	if (need_set_dirty)
	{
		this->SetItemDirty(column, index, true, ITEM_CHANGE_NOTICE_REASON_DATA_CHANGE);
	}

	if (ret != 0)
	{
		if (itemimp->IsRecord())
		{
			gamelog::g_log_knapsack.printf(LL_INFO, "[Knapsack::Use Fail][user[%s %d] item_id:%d item_name:%s(%d) use_num:%d fail_result:%d]",
				m_module_mgr->GetRole()->GetName(), UidToInt(m_module_mgr->GetRole()->GetUserId()), (int)itemimp->GetItemId(), itemimp->GetItemName(), (int)item_is_bind, (int)num, ret);
		}

		return false;
	}

	if (item_data->gold_price > 0)
	{
		int consume_gold_price = ItemExtern::GetGoldPriceByNum(item_data->item_id, item_data->num, item_data->gold_price, num);
		if (consume_gold_price > 0)
		{
			if (consume_gold_price > item_data->gold_price) consume_gold_price = item_data->gold_price;

			item_data->gold_price -= consume_gold_price;
		}
	}

	ItemID item_id = item_data->item_id;

	item_data->num -= num;
	if (item_data->num <= 0) // 消耗数量 注意这个num已经修正为实际使用数量
	{
#ifdef TRACK_ITEM_FLAG
		gamelog::g_log_item_track.printf(LL_INFO, "TRACK_ITEM_FLAG Knapsack::Use use_all CLEAR_GRID role[%d,%s] item[%d] unique_id[%lld]",
			m_module_mgr->GetRole()->GetUID(), m_module_mgr->GetRole()->GetName(), item_data->item_id, item_data->item_unique_id);
#endif

		item_data = NULL;
		this->ClearGrid(column, index);

		this->RefreshValidGridIndexEnd(column, index);
	}

	if (itemimp->IsRecord())
	{
		gamelog::g_log_knapsack.printf(LL_INFO, "[Knapsack::Use Succ][user[%s %d] item_id:%d item_name:%s(%d) use_num:%d fail_result:0]",
			m_module_mgr->GetRole()->GetName(), UidToInt(m_module_mgr->GetRole()->GetUserId()), (int)item_id, itemimp->GetItemName(), (int)item_is_bind, (int)num);

		ROLE_LOG_QUICK6(LOG_TYPE_ITEM_USE, m_module_mgr->GetRole(), item_id, num, NULL, NULL);
	}

	this->SetItemDirty(column, index, true, ITEM_CHANGE_NOTICE_REASON_CONSUME);

	Protocol::SCUseItemSuc uis;
	uis.item_id = item_id;
	uis.num = num;
	EngineAdapter::Instance().NetSend(m_module_mgr->GetRole()->GetNetId(), (const char *)&uis, sizeof(Protocol::SCUseItemSuc));

	EventHandler::Instance().OnUseItem(m_module_mgr->GetRole(), item_id, num);

	return true;
}

bool Knapsack::Discard(short column, short index, short discard_num)
{
	if (column < ITEM_COLUMN_TYPE_MIN || column >= ITEM_COLUMN_TYPE_MAX) return false;
	if (index < 0 || index >= ITEM_COLUMN_GRID_NUM_MAX) return false;
	if (NULL == m_grid_list[column][index] || m_grid_list[column][index]->Invalid()) return false;

	if (discard_num <= 0 || discard_num > m_grid_list[column][index]->num) return false;

	ItemID item_id = m_grid_list[column][index]->item_id;
	if (ItemBase::INVALID_ITEM_ID == item_id) return false;

	const ItemBase *itemimp = ITEMPOOL->GetItem(item_id);
	if (NULL == itemimp) return false;

	int reason = 0;
	if (!itemimp->CanDiscard(m_grid_list[column][index]->param, &reason))
	{
		if (ItemBase::NDR_INLAY == reason)
		{
			m_module_mgr->GetRole()->NoticeNum(errornum::EN_KNAPSACK_CANNOT_DISCARD_INLAY);
		}
		else
		{
			m_module_mgr->GetRole()->NoticeNum(errornum::EN_KNAPSACK_CANNOT_DISCARD);
		}

		return false;
	}
	int exa_coin = 0;
	if (itemimp->GetItemType() == I_TYPE_EQUIPMENT)
	{
		EquipmentParam& data_param = m_grid_list[column][index]->param->equip_param;
		for (int strone_idx = 0; strone_idx < ARRAY_LENGTH(data_param.new_stone); strone_idx++)
		{
			if (data_param.new_stone[strone_idx] != 0)
			{
				const ItemBase *exa_itemimp = ITEMPOOL->GetItem(data_param.new_stone[strone_idx]);
				if (NULL != exa_itemimp)
				{
					exa_coin += exa_itemimp->GetSellPrice(NULL);
				}
			}
		}
	}
	else if (itemimp->GetItemType() == I_TYPE_JEWELRY)
	{
		JewelryParam& data_param = m_grid_list[column][index]->param->jewelry_param;
		for (int strone_idx = 0; strone_idx < ARRAY_LENGTH(data_param.new_stone); strone_idx++)
		{
			if (data_param.new_stone[strone_idx] != 0)
			{
				const ItemBase *exa_itemimp = ITEMPOOL->GetItem(data_param.new_stone[strone_idx]);
				if (NULL != exa_itemimp)
				{
					exa_coin += exa_itemimp->GetSellPrice(NULL);
				}
			}
		}
	}

	bool item_is_bind = m_grid_list[column][index]->is_bind;
	int item_num = m_grid_list[column][index]->num;

	Int64 add_coin_bind = 0;
	if (itemimp->CanSell(m_grid_list[column][index]->param))
	{
		add_coin_bind = itemimp->GetSellPrice(m_grid_list[column][index]->param) * discard_num;
		m_module_mgr->GetMoney()->AddCoinBind(add_coin_bind + exa_coin, "Discard item");
	}

	bool is_item_clear = false;
	if (discard_num >= m_grid_list[column][index]->num)
	{
		m_grid_list[column][index]->num = 0;			// 如果丢弃的数量大于等于拥有数，就清理格子
		is_item_clear = true;
	}
	else
	{
		m_grid_list[column][index]->num -= discard_num;	// 如果丢弃数量小于拥有数，就减去丢弃数
	}

	if (is_item_clear)
	{
#ifdef TRACK_ITEM_FLAG
		gamelog::g_log_item_track.printf(LL_INFO, "TRACK_ITEM_FLAG Knapsack::Discard CLEAR_GRID role[%d,%s] item[%d] unique_id[%lld]",
			m_module_mgr->GetRole()->GetUID(), m_module_mgr->GetRole()->GetName(), m_grid_list[column][index]->item_id, m_grid_list[column][index]->item_unique_id);
#endif

		this->ClearGrid(column, index);
		this->RefreshValidGridIndexEnd(column, index);
	}

	this->SetItemDirty(column, index, true, ITEM_CHANGE_NOTICE_REASON_CONSUME);

	if (itemimp->IsRecord())
	{
		gamelog::g_log_knapsack.printf(LL_INFO, "[Knapsack::Discard Succ][user[%s %d] item_id:%d item_name:%s(%d) color:%d discard_num:%d]",
			m_module_mgr->GetRole()->GetName(), UidToInt(m_module_mgr->GetRole()->GetUserId()), (int)itemimp->GetItemId(), itemimp->GetItemName(), (int)item_is_bind, (int)itemimp->GetColor(), item_num);

		ROLE_LOG_QUICK6(LOG_TYPE_ITEM_DISCARD, m_module_mgr->GetRole(), itemimp->GetItemId(), item_num, NULL, NULL);
	}

	return true;
}

bool Knapsack::InOrder(short column, short index_start, short index_end, bool ignore_time_interval /* = false */)
{
	if (column < ITEM_COLUMN_TYPE_MIN || column >= ITEM_COLUMN_TYPE_MAX) return false;

	const UInt32 sortinterval = GLOBALCONFIG->GetOtherConfig_SortInOrder_Interval();
	UInt32 now_time = static_cast<UInt32>(EngineAdapter::Instance().Time()); // 背包/仓库整理不能频繁调用

	if (!ignore_time_interval && now_time < m_knapsack_last_inorder_time + sortinterval)
	{
		m_module_mgr->GetRole()->NoticeNum(errornum::EN_KNAPSACK_ORDER_TOO_FREQUENT);
		return false;
	}

	m_knapsack_last_inorder_time = now_time;

	int to_check_max_num = m_valid_grid_index_end_list[column];
	int max_valid_index = m_valid_grid_index_end_list[column];
	int max_clear_index = 0;
	int begin = 0;
	if (index_start > 0 || index_end > 0)
	{
		if (index_end > to_check_max_num)
		{
			index_end = to_check_max_num;
		}

		if (index_start >= index_end)
		{
			m_module_mgr->GetRole()->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
			return false;
		}

		begin = index_start;
		to_check_max_num = index_end;
	}

	static bool empty_mark[ITEM_COLUMN_GRID_NUM_MAX]; // 标记排序前在begin ~ begin + check_num范围内哪些格子是空的
	memset(empty_mark, 0, sizeof(empty_mark));

	for (int i = begin; i < to_check_max_num; i++)
	{
		empty_mark[i - begin] = (NULL == m_grid_list[column][i] || m_grid_list[column][i]->Invalid());
	}

	int merge_result = -1;
	int rest_num = -1;
	for (int i = to_check_max_num - 1; i >= begin; i--) // 先将所有能合并的都合并起来
	{
		if (NULL == m_grid_list[column][i] || m_grid_list[column][i]->Invalid()) continue;

		const ItemBase *itembase = ITEMPOOL->GetItem(m_grid_list[column][i]->item_id);
		if (NULL == itembase) continue;

		if (!m_grid_list[column][i]->CanBeMerge(true)) continue; // 下面会去判断invalid_time 所以先传入ignore_invalid_time=true

		for (int j = begin; j < i; j++)
		{
			if (NULL == m_grid_list[column][j] || m_grid_list[column][j]->Invalid()) continue;

			int merged_gold_price = 0;
			merge_result = m_grid_list[column][i]->Merge(*m_grid_list[column][j], &rest_num, &merged_gold_price);
			if (ITEM_MERGE_CANNOT != merge_result)
			{
				if (ITEM_MERGE_COMPLETE == merge_result && 0 == rest_num)
				{
#ifdef TRACK_ITEM_FLAG
					gamelog::g_log_item_track.printf(LL_INFO, "TRACK_ITEM_FLAG Knapsack::InOrder merge_all CLEAR_GRID role[%d,%s] item[%d] unique_id[%lld]",
						m_module_mgr->GetRole()->GetUID(), m_module_mgr->GetRole()->GetName(), m_grid_list[column][i]->item_id, m_grid_list[column][i]->item_unique_id);
#endif

					this->ClearGrid(column, j);
					empty_mark[j - begin] = true;
					max_clear_index = GetMax(max_clear_index, j);
				}
				else
				{
					m_grid_list[column][j]->num = rest_num;
					m_grid_list[column][j]->gold_price -= merged_gold_price;
				}

				this->SetItemDirty(column, i, false, ITEM_CHANGE_NOTICE_REASON_MOVE);
				this->SetItemDirty(column, j, false, ITEM_CHANGE_NOTICE_REASON_MOVE);

				if (m_grid_list[column][i]->num >= itembase->GetPileLimit()) break;
			}
		}
	}

	static SortStruct sort_stru[ITEM_COLUMN_GRID_NUM_MAX];
	memset(sort_stru, 0, sizeof(sort_stru));

	int count = 0;

	for (int i = begin; i < to_check_max_num; i++)
	{
		if (NULL != m_grid_list[column][i] && !m_grid_list[column][i]->Invalid())
		{
			const ItemBase *itembase = ITEMPOOL->GetItem(m_grid_list[column][i]->item_id);
			if (NULL == itembase) continue;

			if (count >= ITEM_COLUMN_GRID_NUM_MAX) return false;

			long long prior_id = 0;
			if (ITEM_COLUMN_TYPE_SUPPLY == column)
			{
				prior_id = SortStruct::GetSupplyPriorId(itembase->GetItemType(), itembase->GetItemSubType(), itembase->GetColor(), itembase->GetItemId());
			}
			else if (I_TYPE_EQUIPMENT == itembase->GetItemType())
			{
				Equipment * equip_base = (Equipment *)itembase;
				if (NULL == m_grid_list[column][i]->param)
				{
					prior_id = SortStruct::GetPriorId(equip_base->GetEquipLevel(), itembase->GetItemType(), itembase->GetItemSubType(), itembase->GetColor(), equip_base->GetEquipIndex(), itembase->GetItemId(), 0);
				}
				else
				{
					short star_level = m_grid_list[column][i]->param->equip_param.star_level;
					prior_id = SortStruct::GetPriorId(equip_base->GetEquipLevel(), itembase->GetItemType(), itembase->GetItemSubType(), itembase->GetColor(), equip_base->GetEquipIndex(), itembase->GetItemId(), star_level);
				}
			}
			else if (I_TYPE_JEWELRY == itembase->GetItemType())
			{
				Jewelry * jewelry_base = (Jewelry *)itembase;
				if (NULL == m_grid_list[column][i]->param)
				{
					prior_id = SortStruct::GetPriorId(jewelry_base->GetJewelryLevel(), itembase->GetItemType(), itembase->GetItemSubType(), itembase->GetColor(), jewelry_base->GetJewelryType() + Equipment::E_INDEX_MAX, itembase->GetItemId(), 0);
				}
				else
				{
					short star_level = m_grid_list[column][i]->param->jewelry_param.star_level;
					prior_id = SortStruct::GetPriorId(jewelry_base->GetJewelryLevel(), itembase->GetItemType(), itembase->GetItemSubType(), itembase->GetColor(), jewelry_base->GetJewelryType() + Equipment::E_INDEX_MAX, itembase->GetItemId(), star_level);
				}
			}
			else if (I_TYPE_MEDAL == itembase->GetItemType())
			{
				if (NULL == m_grid_list[column][i]->param)
				{
					prior_id = SortStruct::GetPriorId(0, itembase->GetItemType(), itembase->GetItemSubType(), itembase->GetColor(), 0, itembase->GetItemId(), 0);
				}
				else
				{
					short equip_rate = 0;
					{
						int sp_id = m_grid_list[column][i]->param->medal_param.sp_id;
						const SpecialEffectCfg * spe_cfg = BattleSpecialEffectPool::GetInstance().GetSpecialEffectCfg(sp_id);
						if (NULL != spe_cfg)
						{
							equip_rate = spe_cfg->sp_priority;
						}
					}
					{
						int sp_id_2 = m_grid_list[column][i]->param->medal_param.second_sp_id;
						const SpecialEffectCfg * spe_cfg_2 = BattleSpecialEffectPool::GetInstance().GetSpecialEffectCfg(sp_id_2);
						if (NULL != spe_cfg_2)
						{
							equip_rate += spe_cfg_2->sp_priority;
						}
					}
					prior_id = SortStruct::GetPriorId(0, itembase->GetItemType(), itembase->GetItemSubType(), itembase->GetColor(), 0, itembase->GetItemId(), equip_rate);
				}
			}
			else
			{
				prior_id = SortStruct::GetPriorId(0, itembase->GetItemType(), itembase->GetItemSubType(), itembase->GetColor(), 0, itembase->GetItemId(), 0);
			}
			sort_stru[count].Set(prior_id, m_grid_list[column][i]->is_bind, i);

			++count;
		}
	}

	if (count > ITEM_COLUMN_GRID_NUM_MAX) return false;

	qsort(sort_stru, count, sizeof(SortStruct), SortCompare);

	static ItemDataWrapper tmp_wrapper_list[ITEM_COLUMN_GRID_NUM_MAX];

	//m_valid_grid_index_end_list[column] = count; 在RefreshValidGridIndexEnd维护m_valid_grid_index_end_list

	for (int i = 0; i < count; i++) // 将排序后的物品存到临时空间里
	{
		int src_index = sort_stru[i].src_index;
		m_grid_list[column][src_index]->WrapTo(&tmp_wrapper_list[i]);

		// 把原位置清空
		this->ClearGrid(column, src_index);
		this->SetItemDirty(column, src_index, false, ITEM_CHANGE_NOTICE_REASON_MOVE);
	}

	for (int i = 0; i < count; i++) // 从临时空间拷贝回来
	{
		if (NULL == m_grid_list[column][begin + i])
		{
			m_grid_list[column][begin + i] = new ItemGridData();
		}
		m_grid_list[column][begin + i]->Set(tmp_wrapper_list[i]);
		this->SetItemDirty(column, begin + i, false, ITEM_CHANGE_NOTICE_REASON_MOVE);
	}

	for (int i = count; i < to_check_max_num - begin; i++) // 将后面的全部设为空
	{
		if (empty_mark[i]) continue;

		if (NULL == m_grid_list[column][begin + i] || m_grid_list[column][begin + i]->Invalid()) continue;

		this->ClearGrid(column, begin + i);
		this->SetItemDirty(column, begin + i, false, ITEM_CHANGE_NOTICE_REASON_MOVE);

		max_clear_index = GetMax(max_clear_index, begin + i);
	}

	this->RefreshValidGridIndexEnd(column, max_valid_index);
	this->SendKnapsackInfoToRole(Protocol::SCKnapsackInfoAck::SEND_REASON_IN_ORDER);

	return true;
}

bool Knapsack::Deposit(short column, short index, short storage_page)
{
	if (column < ITEM_COLUMN_TYPE_MIN || column >= ITEM_COLUMN_TYPE_NORMAL_COLUMN_END) return false;
	if (index < 0 || index >= ITEM_COLUMN_GRID_NUM_MAX) return false;

	if (NULL == m_grid_list[column][index] || m_grid_list[column][index]->Invalid()) return false;

	if (!this->HaveSpace(ITEM_COLUMN_TYPE_STORAGE, false))
	{
		m_module_mgr->GetRole()->NoticeNum(errornum::EN_STORAGE_SPACE_NOT_ENOUGH);
		return false;
	}

	const KnapsackGridCfg* cfg = LOGIC_CONFIG->GetKnapsackConfig()->GetKnapsackGridCfg(ITEM_COLUMN_TYPE_STORAGE);
	if (NULL == cfg) return false;

	int offset_index = storage_page * cfg->extend_grid_num;
	int range = cfg->extend_grid_num;

	this->CertainMoveHelper(column, index, ITEM_COLUMN_TYPE_STORAGE, offset_index, range);
	return true;
}

bool Knapsack::Withdraw(short column, short index, ARG_OUT short* out_column, bool notice_on_error /* = false */)
{
	if (column < ITEM_COLUMN_TYPE_NORMAL_COLUMN_END || column >= ITEM_COLUMN_TYPE_MAX) return false;
	if (index < 0 || index >= ITEM_COLUMN_GRID_NUM_MAX) return false;

	if (NULL == m_grid_list[column][index] || m_grid_list[column][index]->Invalid()) return false;

	const ItemBase* item_base = ITEMPOOL->GetItem(m_grid_list[column][index]->item_id);
	if (NULL == item_base)
	{
		if (notice_on_error)
		{
			m_module_mgr->GetRole()->NoticeNum(errornum::EN_KNAPSACK_LOAD_ERROR);
		}
		return false;
	}

	if (NULL != out_column)
	{
		*out_column = item_base->GetItemColumn();
	}

	if (!this->HaveSpace(item_base->GetItemColumn(), notice_on_error))
	{
		return false;
	}

	return this->CertainMoveHelper(column, index, item_base->GetItemColumn());
}

bool Knapsack::WithdrawAll(short column)
{
	if (column != ITEM_COLUMN_TYPE_TEMPORARY) return false; // 只能从临时背包一键

	bool COLUMN_NOTICE[ITEM_COLUMN_TYPE_MAX];
	memset(COLUMN_NOTICE, 0, sizeof(COLUMN_NOTICE));

	bool has_succ_once = false;
	bool has_fail_once = false;
	int valid_grid_index_end = m_valid_grid_index_end_list[column];
	short item_belong_column = 0;
	for (int index = valid_grid_index_end - 1; index >= 0; --index)
	{
		bool ret = this->Withdraw(column, index, &item_belong_column, false);
		has_succ_once = has_succ_once || ret;
		has_fail_once = has_fail_once || (!ret);

		if (!ret)
		{
			COLUMN_NOTICE[item_belong_column] = true;
		}
	}

	if (has_fail_once)
	{
		for (int c = ITEM_COLUMN_TYPE_MIN; c < ITEM_COLUMN_TYPE_MAX; ++c)
		{
			if (!COLUMN_NOTICE[c]) continue;

			this->SendNoSpaceNotice(c);
		}
	}

	return has_succ_once;
}

bool Knapsack::Extend(short column, short add_grid_num)
{
	if (add_grid_num <= 0) return false;

	if (column < 0 || column >= ITEM_COLUMN_TYPE_MAX) return false;

	if (ITEM_COLUMN_TYPE_TEMPORARY == column) return false;

	const KnapsackGridCfg* cfg = LOGIC_CONFIG->GetKnapsackConfig()->GetKnapsackGridCfg(column);
	if (NULL == cfg || !cfg->CanExtend()) return false;

	if (m_extend_column_times_list[column] >= cfg->max_extend_times || m_extend_column_times_list[column] < 0)
	{
		m_module_mgr->GetRole()->NoticeNum(errornum::EN_KNAPSACK_EXTEND_TIMES_LIMIT);
		return false;
	}

	if (m_curr_valid_grid_num_list[column] + add_grid_num > cfg->max_grid_num)
	{
		m_module_mgr->GetRole()->NoticeNum(errornum::EN_KNAPSACK_EXTEND_TIMES_LIMIT);
		return false;
	}

	if (add_grid_num != cfg->extend_grid_num)
	{
		m_module_mgr->GetRole()->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return false;
	}

	int cost_coin = cfg->extend_cost_gold_list[m_extend_column_times_list[column]];
	if (!m_module_mgr->GetMoney()->CoinBindMoreThan(cost_coin))
	{
		m_module_mgr->GetRole()->NoticeNum(errornum::EN_COIN_NOT_ENOUGH);
		return false;
	}

	if (!m_module_mgr->GetMoney()->UseCoinBind(cost_coin, __FUNCTION__))
	{
		m_module_mgr->GetRole()->NoticeNum(errornum::EN_COIN_NOT_ENOUGH);
		return false;
	}

	m_curr_valid_grid_num_list[column] += add_grid_num;
	m_extend_column_times_list[column] += 1;

	static Protocol::SCKnapsackExtend msg;
	msg.column = column;
	msg.grid_num = m_curr_valid_grid_num_list[column];
	msg.extend_times = m_extend_column_times_list[column];
	msg.reserve_sh = 0;

	EngineAdapter::Instance().NetSend(m_module_mgr->GetRole()->GetNetId(), (const char*)&msg, sizeof(msg));

	return true;
}

bool Knapsack::DecomposeSurface(short column, short index, short discard_num)
{
	if (column < ITEM_COLUMN_TYPE_MIN || column >= ITEM_COLUMN_TYPE_MAX) return false;
	if (index < 0 || index >= ITEM_COLUMN_GRID_NUM_MAX) return false;
	if (NULL == m_grid_list[column][index] || m_grid_list[column][index]->Invalid()) return false;

	if (discard_num <= 0 || discard_num > m_grid_list[column][index]->num) return false;

	ItemID item_id = m_grid_list[column][index]->item_id;
	if (ItemBase::INVALID_ITEM_ID == item_id) return false;

	const ItemBase *itemimp = ITEMPOOL->GetItem(item_id);
	if (NULL == itemimp) return false;

	bool is_item_clear = false;
	if (discard_num == m_grid_list[column][index]->num)
	{
		m_grid_list[column][index]->num = 0;			// 如果丢弃的数量等于拥有数，就清理格子
		is_item_clear = true;
	}
	else if (discard_num < m_grid_list[column][index]->num)
	{
		m_grid_list[column][index]->num -= discard_num;	// 如果丢弃数量小于拥有数，就减去丢弃数
	}
	else   // 如果丢弃的数量大于拥有数,那分解失败(防止外部调用获得不对应的分解物数量)
	{
		return false;
	}

	if (is_item_clear)
	{
#ifdef TRACK_ITEM_FLAG
		gamelog::g_log_item_track.printf(LL_INFO, "TRACK_ITEM_FLAG Knapsack::DecomposeSurface decompose_all CLEAR_GRID role[%d,%s] item[%d] unique_id[%lld]",
			m_module_mgr->GetRole()->GetUID(), m_module_mgr->GetRole()->GetName(), m_grid_list[column][index]->item_id, m_grid_list[column][index]->item_unique_id);
#endif

		this->ClearGrid(column, index);
		this->RefreshValidGridIndexEnd(column, index);
	}

	this->SetItemDirty(column, index, true, ITEM_CHANGE_NOTICE_REASON_CONSUME);
	return true;
}

bool Knapsack::SendMail(const ItemConfigData * item_list, int item_list_num, int type, bool is_notice, int param1, const char *param2, int param3)
{
	if (NULL == item_list && item_list_num > 0) return false;
	static MailContentParam contentparam; contentparam.Reset();

	UNSTD_STATIC_CHECK(SEND_MAIL_TYPE_MAX == 77);
	switch (type)
	{
		case SEND_MAIL_TYPE_GUILD_INBORN_SKILL_RESET:
		{
			SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_role_guild_inborn_reset_skill_subject);
			SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_role_guild_inborn_reset_skill_content);
		}
		break;
	case SEND_MAIL_TYPE_NEW_COURAGE_CHALLENGE:
		{
			SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_knapsack_full_mail_subject);
			SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_new_courage_challenge_reward_content);
		}
		break;
	case SEND_MAIL_TYPE_COLOSSEUM_BATTLE_REWARD:
		{
			SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_colosseum_battle_reawrd_subject);
			SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_colosseum_battle_reawrd_content);
		}
		break;
		case SEND_MAIL_TYPE_KUA_FU_YING_XING_LU_RANK_END_TASK:
		{
			SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_kua_fu_ying_xiong_lu_rand_end_task_reward_subject);
			SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_kua_fu_ying_xiong_lu_rand_end_task_reward_content, param1);
		}
		break;
		case SEND_MAIL_TYPE_KUA_FU_YING_XING_LU_BAG_FULL:
		{
			SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_kua_fu_ying_xiong_lu_bag_full_subject);
			SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_kua_fu_ying_xiong_lu_bag_full_content);
		}
		break;
		case SEND_MAIL_TYPE_ACTIVITY_BUY_BAG_FULL:
		{
			SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_chong_zhi_buy_item_reward_title);
			SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_chong_zhi_buy_item_reward_content, param1);
		}
		break;
		case SEND_MAIL_TYPE_ACTIVITY_END_REWARD_REISSUE:
		{
			SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_randactivity_reward_subject_1, param1);
			SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_randactivity_reward_end_content, param1);
		}
		break;
	case SEND_MAIL_TYPE_ACTIVITY_REWARD_BAG_FULL:
		{
			SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_randactivity_bag_full_subject, param1);
			SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_randactivity_bag_full_content, param1);
		}
		break;
	case SEND_MAIL_TYPE_CANG_LONG_CI_FU:
		{
			SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_randactivity_tiger_auspicious_snow_subject, RAND_ACTIVITY_TYPE_CANG_LONG_CI_FU);
			SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_randactivity_tiger_auspicious_snow_content, RAND_ACTIVITY_TYPE_CANG_LONG_CI_FU);
		}
		break;
	case SEND_MAIL_TYPE_LAN_TANG_CHU_MU:
		{
			SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_randactivity_tiger_auspicious_snow_subject, RAND_ACTIVITY_TYPE_LAN_TANG_CHU_MU);
			SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_randactivity_tiger_auspicious_snow_content, RAND_ACTIVITY_TYPE_LAN_TANG_CHU_MU);
		}
		break;
	case SEND_MAIL_TYPE_SALTY_SWEET_BATTLE:
		{
			SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_randactivity_unfetch_reward_title);
			SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_randactivity_unfetch_reward_content);
		}
		break; 
	case SEND_MAIL_TYPE_SHAN_HAI_MEET:
		{
			SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_ra_shan_hai_meet_subject);
			SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_ra_shan_hai_meet_content);
		}
		break;
	case SEND_MAIL_TYPE_RELIC_LOOTING:
		{
			SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_lun_hui_wang_chuan_reward_subject);
			SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_lun_hui_wang_chuan_reward_content);
		}
		break;
	case SEND_MAIL_TYPE_LUN_HUI_WANG_CHUAN:
		{
			SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_lun_hui_wang_chuan_reward_subject);
			SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_lun_hui_wang_chuan_reward_content);
		}
		break;
	case SNED_MAIL_TYPE_DEFAULT:
		{
			SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_knapsack_full_mail_subject);
			SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_knapsack_full_send_mail);
		}
		break;
	case SEND_MAIL_TYPE_SEVEN_COLOR:
		{
			SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_seven_color_picture_subject, RAND_ACTIVITY_TYPE_SEVEN_COLOR_PICTURE);
			SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_seven_color_picture_content, RAND_ACTIVITY_TYPE_SEVEN_COLOR_PICTURE, param1);
		}
		break;
	case SEND_MAIL_TYPE_SEVEN_COLOR_COUNT:
		{
			SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_seven_color_picture_subject, RAND_ACTIVITY_TYPE_SEVEN_COLOR_PICTURE);
			SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_seven_color_picture_total_content, RAND_ACTIVITY_TYPE_SEVEN_COLOR_PICTURE);
		}
		break;
	case SEND_MAIL_TYPE_SIGN_UP:
		{
			SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_activity_sign_up_subject);
			SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_activity_sign_up_content, param1);
		}
		break;
	case SEND_MAIL_TYPE_GUILD_BOSS_CHAPTER_KILL:
		{
			SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_guild_boss_chapter_kill_reward_subject);
			if (NULL == param2) return false;
			SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_guild_boss_chapter_kill_reward_content, param2);
		}
		break;
	case SEND_MAIL_TYPE_TASK_REISSUE_ITEM:
		{
			SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_task_reissue_item_subject);
			SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_task_reissue_item_content);
		}
		break;
	case SEND_MAIL_TYPE_MAKE_UP_REWARD:
		{
			SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_randactivity_make_up_reward_subject, param1);
			SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_randactivity_make_up_reward_content);
		}
		break;
	case SEND_MAIL_TYPE_COURAGE_CHALLENGE:
		{
			SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_knapsack_full_mail_subject);
			SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_courage_challenge_reward_content);
		}
		break;
	case SEND_MAIL_TYPE_DREAM_NOTES_CHALLENGE:
		{
			SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_dream_notes_challenge_subject);
			SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_dream_notes_challenge_content);
		}
		break;
	case SEND_MAIL_TYPE_GOOD_GIFT_TURN:
		{
			SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_randactivity_good_gift_turn_subject);
			SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_randactivity_good_gift_turn_content);
		}
		break;
	case SEND_MAIL_TYPE_HELP_OTHERS:
		{
			SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_randactivity_help_others_subject);
			SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_randactivity_help_others_content);
		}
		break;
	case SEND_MAIL_TYPE_SECRET_TREASURE:
		{
			SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_randactivity_secret_treasure_subject);
			SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_randactivity_secret_treasure_content);
		}
		break;
	case SEND_MAIL_TYPE_TOP_LEVEL:
		{
			SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_top_level_subject);
			SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_top_level_content);
		}
		break;
	case SEND_MAIL_TYPE_FIRST_RECHARGE_EXCLUSIVE:
		{
			SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_first_recharge_subject);
			SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_first_recharge_exclusive);
		}
		break;
	case SEND_MAIL_TYPE_MONTH_CARD_BUY_CARD_OR_DAY_REWARD:
		{
			SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_month_week_card_reward_subject);
			SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_buy_month_week_card_reward_content, param1);
		}
		break;
	case SEND_MAIL_TYPE_MONTH_CARD_BUY_ITEM:
		{
			SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_month_week_card_reward_subject);
			SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_buy_moth_week_card_item_content);
		}
		break;
	case SEND_MAIL_TYPE_ACTIVITY_REWARD:
		{
			SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_randactivity_reward_subject);
			SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_randactivity_reward_content, param1);
		}
		break;
	case SEND_MAIL_TYPE_ACTIVITY_BUY_SHOP:
		{
			SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_randactivity_reward_subject);
			SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_randactivity_reward_buy_limit_content, param1);
		}
		break;
	case SEND_MAIL_TYPE_FANG_FRAUD_REWARD:
		{
			SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::fang_fraud_send_mail_subject);
			SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::fang_fraud_send_mail_content);
		}
		break;
	case SEND_MAIL_TYPE_BACK_REWARD:
		{
			SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_ra_back_reward_send_mail_subject);
			SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_ra_back_reward_send_mail_content);
		}
		break;
	case SEND_MAIL_TYPE_SERVER_COMPETITION:
		{
			SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_randactivity_reward_subject);
			SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_randactivity_reward_buy_limit_content, param1, param3);
		}
		break;
	case SEND_MAIL_TYPE_ACTIVITY_END:
		{
			SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_randactivity_reward_subject);
			SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_randactivity_reward_end_content, param1);
		}
		break;
	case SEND_MAIL_TYPE_STONE_BACK:
		{
			SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_stone_mail_back_subject);
			SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_stone_mail_back_content);
		}
		break;
	case SEND_MAIL_TYPE_HELP_OTHERS_2:
		{
			SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_randactivity_help_others_2_subject);
			SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_randactivity_help_others_2_content);
		}
		break;
	case SEND_MAIL_TYPE_SHAN_HAI_QUESTION:
		{
			SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_randactivity_shan_hai_question_subject);
			SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_randactivity_shan_hai_question_sontent);
		}
		break;
	case SEND_MAIL_TYPE_FATE_ENTANGLEMENT:
		{
			SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_randactivity_fate_entanglement_subject);
			SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_randactivity_fate_entanglement_content);
		}
		break;
	case SEND_MAIL_TYPE_YUN_ZE_TERRITORY:
		{
			SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_randactivity_yun_ze_territory_subject, RAND_ACTIVITY_TYPE_YUN_ZE_TERRITORY);
			SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_randactivity_yun_ze_territory_content, RAND_ACTIVITY_TYPE_YUN_ZE_TERRITORY);
		}
		break;
	case SEND_MAIL_TYPE_YUN_ZE_SLOT_MACHINE:
		{
			SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_randactivity_yun_ze_slot_machine_subject, RAND_ACTIVITY_TYPE_YUN_ZE_SLOT_MACHINE);
			SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_randactivity_yun_ze_slot_machine_content, RAND_ACTIVITY_TYPE_YUN_ZE_SLOT_MACHINE);
		}
		break;
	case SEND_MAIL_TYPE_YUN_ZE_ER_SAN_SHI:
		{
			SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_randactivity_yun_ze_er_san_shi_subject, RAND_ACTIVITY_TYPE_YUN_ZE_ER_SAN_SHI);
			SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_randactivity_yun_ze_er_san_shi_content, RAND_ACTIVITY_TYPE_YUN_ZE_ER_SAN_SHI);
		}
		break;
	case SEND_MAIL_TYPE_YUN_ZE_BLOOMING_RECORD:
		{
			SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_randactivity_yun_ze_blooming_record_subject, RAND_ACTIVITY_TYPE_YUN_ZE_BLOOMING_RECORD);
			SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_randactivity_yun_ze_blooming_record_content, RAND_ACTIVITY_TYPE_YUN_ZE_BLOOMING_RECORD);
		}
		break;
	case SEND_MAIL_TYPE_YUNZE_CATCH_ELVES:
		{
			SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_randactivity_yunze_catch_elves_subject, RAND_ACTIVITY_TYPE_YUN_ZE_CATCH_ELVES);
			SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_randactivity_yunze_catch_elves_content, RAND_ACTIVITY_TYPE_YUN_ZE_CATCH_ELVES);
		}
		break;
	case SEND_MAIL_TYPE_YUNZE_HAO_LI:
	{
		SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_randactivity_yunze_hao_li_subject, RAND_ACTIVITY_TYPE_YUN_ZE_HAO_LI);
		SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_randactivity_yunze_hao_li_content, RAND_ACTIVITY_TYPE_YUN_ZE_HAO_LI);
	}
	break;
	case SEND_MAIL_TYPE_RA_XIAN_SHI_LEI_CHONG:
		{
			//复用字符串 ,免得搞一大堆一模一样的字符串出来
			SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_randactivity_yunze_catch_elves_subject, RAND_ACTIVITY_TYPE_XIAN_SHI_LEI_CHONG);
			SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_randactivity_yunze_catch_elves_content, RAND_ACTIVITY_TYPE_XIAN_SHI_LEI_CHONG);
		}
		break;
	case SEND_MAIL_TYPE_YUNZE_WISH:
		{
			//复用字符串
			SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_randactivity_yunze_catch_elves_subject, RAND_ACTIVITY_TYPE_YUN_ZE_WISH);
			SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_randactivity_yunze_catch_elves_content, RAND_ACTIVITY_TYPE_YUN_ZE_WISH);
		}
		break;
	case SEND_MAIL_TYPE_YUNZE_CUTE_PET:
		{
			//复用字符串
			SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_randactivity_yunze_catch_elves_subject, RAND_ACTIVITY_TYPE_YUN_ZE_CUTE_PET);
			SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_randactivity_yunze_catch_elves_content, RAND_ACTIVITY_TYPE_YUN_ZE_CUTE_PET);
		}
		break;
	case SEND_MAIL_TYPE_TREASURE_TASK:
		{
			SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_treasure_task_subject);
			SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_treasure_task_content);
		}
		break;

case SEND_MAIL_TYPE_TIGER_AUSPICIOUS_SNOW:
		{
			SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_randactivity_tiger_auspicious_snow_subject, RAND_ACTIVITY_TYPE_TIGER_AUSPICIOUS_SNOW);
			SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_randactivity_tiger_auspicious_snow_content, RAND_ACTIVITY_TYPE_TIGER_AUSPICIOUS_SNOW);
		}
		break;
	case SEND_MAIL_TYPE_TIGER_FISH_EVERY_YEAR:
		{
			// 与以上文本内容相同,复用
			SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_randactivity_tiger_auspicious_snow_subject, RAND_ACTIVITY_TYPE_TIGER_FISH_EVERY_YEAR);
			SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_randactivity_tiger_auspicious_snow_content, RAND_ACTIVITY_TYPE_TIGER_FISH_EVERY_YEAR);
		}
		break;
	case SEND_MAIL_TYPE_TIGER_FIND_SPRING:
		{
			// 字符串复用
			SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_randactivity_tiger_auspicious_snow_subject, RAND_ACTIVITY_TYPE_TIGER_FIND_SPRING);
			SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_randactivity_tiger_auspicious_snow_content, RAND_ACTIVITY_TYPE_TIGER_FIND_SPRING);
		}
		break;
	case SEND_MAIL_TYPE_TIGER_TEASE_NEW_YEAR:
		{
			// 字符串复用
			SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_randactivity_tiger_auspicious_snow_subject, RAND_ACTIVITY_TYPE_TIGER_TEASE_NEWYEAR);
			SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_randactivity_tiger_auspicious_snow_content, RAND_ACTIVITY_TYPE_TIGER_TEASE_NEWYEAR);
		}
		break;
	case SEND_MAIL_TYPE_TIGER_LUCKY_MONEY:
		{
			// 字符串复用
			SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_randactivity_tiger_auspicious_snow_subject, RAND_ACTIVITY_TYPE_TIGER_LUCKY_MONEY);
			SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_randactivity_tiger_auspicious_snow_content, RAND_ACTIVITY_TYPE_TIGER_LUCKY_MONEY);
		}
		break;
	case SEND_MAIL_TYPE_TIGER_FIND_SPRING_YESTERDAY:
		{
			SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_randactivity_tiger_auspicious_snow_subject, RAND_ACTIVITY_TYPE_TIGER_FIND_SPRING);
			SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_randactivity_tiger_find_spring_yesterday_content, RAND_ACTIVITY_TYPE_TIGER_FIND_SPRING);
		}
		break;
	case SEND_MAIL_TYPE_JINGJI_YUEKA_REISSUE_REWARD:
		{
			SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_randactivity_jingji_yueka_reissue_subject, param1);
			SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_randactivity_jingji_yueka_reissue_content, param1);
		}
		break;
	case SEND_MAIL_TYPE_RONG_LIAN_YOU_LI:
		{ 
			// 复用字符串
			SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_randactivity_jingji_yueka_reissue_subject, RAND_ACTIVITY_TYPE_RONG_LIAN_YOU_LI);
			SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_randactivity_jingji_yueka_reissue_content, RAND_ACTIVITY_TYPE_RONG_LIAN_YOU_LI);
		}
		break;
	case SEND_MAIL_TYPE_WORLD_ARENA_WIN_BOX_REWARD:
		{
			SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_world_arena_win_box_reward_subject);
			SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_world_arena_win_box_reward_content);
		}
		break;
	case SEND_MAIL_TYPE_WORLD_ARENA_SIGNUP_REWARD:
		{
			SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_world_arena_signup_reward_subject);
			SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_world_arena_signup_reward_content);
		}
		break;
	case SEND_MAIL_TYPE_WORLD_ARENA_DAILY_REWARD:
		{
			SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_world_arena_day_reward_subject);
			SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_world_arena_day_reward_content);
		}
		break;
	case SEND_MAIL_TYPE_PET_TALENT_RESET_RETURN:
		{
			SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_pet_talent_reset_return_consume_item_subject);
			SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_pet_talent_reset_return_consume_item_content, param1);
		}
		break; 
	case SEND_MAIL_TYPE_TIAN_GONG_XUN_BAO_REWARD:
		{
			SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_tian_gong_xun_bao_subject);
			SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_tian_gong_xun_bao_content);
		}
		break;
	case SEND_MAIL_TYPE_HUNDRED_GOD_BOSS_TIME_OVER:
		{
			SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_role_hundred_god_boss_time_over_subject);
			SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_role_hundred_god_boss_time_over_content);
		}
		break;
	case SEND_MAIL_TYPE_HUNDRED_GOD_BOSS_REWARD_REISSUE:
		{
			SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_role_hundred_god_boss_time_over_subject);
			SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_role_hundred_god_boss_reward_reissue_content);
		}
		break;
	case SEND_MAIL_TYPE_GOD_BEAST_ADVENT:
		{
			SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_randactivity_tiger_auspicious_snow_subject, RAND_ACTIVITY_TYPE_GOD_BEAST_ADVENT);
			SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_randactivity_tiger_auspicious_snow_content, RAND_ACTIVITY_TYPE_GOD_BEAST_ADVENT);
		}
		break;
	case SEND_MAIL_TYPE_GOD_BEAST_ADVENT_YESTERDAY:
		{
			SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_randactivity_tiger_auspicious_snow_subject, RAND_ACTIVITY_TYPE_GOD_BEAST_ADVENT);
			SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_randactivity_tiger_find_spring_yesterday_content, RAND_ACTIVITY_TYPE_GOD_BEAST_ADVENT);
		}
		break;
	case SEND_MAIL_TYPE_TIGER_ACTIVITY:
		{
			SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_randactivity_tiger_auspicious_snow_subject, param1);
			SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_randactivity_tiger_auspicious_snow_content, param1);
		}
		break;
	case SEND_MAIL_TYPE_BO_ZHONG_YOU_LI:
		{
			SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_randactivity_tiger_auspicious_snow_subject, RAND_ACTIVITY_TYPE_BO_ZHONG_YOU_LI);
			SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_randactivity_tiger_auspicious_snow_content, RAND_ACTIVITY_TYPE_BO_ZHONG_YOU_LI);
		}
		break;
	case SEND_MAIL_TYPE_YUAN_QI_FAN_PAI:
		{
			SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_randactivity_tiger_auspicious_snow_subject, RAND_ACTIVITY_TYPE_YUNA_QI_FAN_PAI);
			SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_randactivity_tiger_auspicious_snow_content, RAND_ACTIVITY_TYPE_YUNA_QI_FAN_PAI);
		}
		break;
	case SEND_MAIL_TYPE_BAO_ZI_LAI_LE:
		{	
			SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_randactivity_tiger_auspicious_snow_subject, RAND_ACTIVITY_TYPE_BAO_ZI_LAI_LE);
			SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_randactivity_tiger_auspicious_snow_content, RAND_ACTIVITY_TYPE_BAO_ZI_LAI_LE);
		}
		break;
	case SEND_MAIL_TYPE_MIAO_BI_SHENG_HUA:
		{
			SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_randactivity_tiger_auspicious_snow_subject, RAND_ACTIVITY_TYPE_MIAO_BI_SHENG_HUA);
			SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_randactivity_tiger_auspicious_snow_content, RAND_ACTIVITY_TYPE_MIAO_BI_SHENG_HUA);
		}
		break;
	case SEND_MAIL_TYPE_CHENG_FENG_PO_LANG:
		{
			SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_randactivity_tiger_auspicious_snow_subject, RAND_ACTIVITY_TYPE_CHENG_FENG_PO_LANG);
			SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_randactivity_tiger_auspicious_snow_content, RAND_ACTIVITY_TYPE_CHENG_FENG_PO_LANG);
		}
		break;
	case SEND_MAIL_TYPE_PINJIA_YOULI:
		{
			SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_pinjia_youli_subject);
			SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_pinjia_youli_content);
		}
		break;
	case SEND_MAIL_TYPE_DUAN_YANG_XIANG_NANG:
		{
			SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_randactivity_duan_yang_xiang_nang_subject);
			SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_randactivity_duan_yang_xiang_nang_content);
		}
		break;
	default:
		return false;
	}

	int index = 0;
	ItemDataWrapper item_info;
	for (int i = 0; i < item_list_num; i++)
	{
		const ItemBase * item_base = ITEMPOOL->GetItem(item_list[i].item_id);
		if (NULL == item_base) continue;

		if (index >= MAX_ATTACHMENT_ITEM_NUM)
		{
			MailRoute::MailToUser(m_module_mgr->GetRole()->GetUID(), MAIL_REASON_DEFAULT, contentparam);
			index = 0;
			memset(contentparam.item_list, 0, sizeof(contentparam.item_list));
		}
		item_info.item_id = item_list[i].item_id;
		item_info.is_bind = item_list[i].is_bind;
		item_info.num = item_list[i].num;
		contentparam.item_list[index++] = item_info;
	}

	MailRoute::MailToUser(m_module_mgr->GetRole()->GetUID(), MAIL_REASON_DEFAULT, contentparam);
	if (is_notice)
	{
		m_module_mgr->NoticeNum(noticenum::NT_KNAPSACKFULL_SEND_MAIL);
	}

	return true;
}

bool Knapsack::SendMail(const std::vector<ItemConfigData>& item_list, int type, bool is_notice, int param1, const char * param2, int param3)
{
	if (item_list.empty())
	{
		return false;
	}

	return this->SendMail(&item_list[0], (int)item_list.size(), type, is_notice, param1, param2, param3);
}

#define PUT_HELPER(item_wrapper, drop_monster_id) \
if (item_wrapper.Invalid()) return false;\
const ItemBase *item = ITEMPOOL->GetItem(item_wrapper.item_id);\
if (NULL == item)\
{\
	m_module_mgr->GetRole()->NoticeNum(errornum::EN_KNAPSACK_PUT_ERROR);\
	return false;\
}\
if (B_BELONG_KNAPSACK != item->GetBagsBelong())\
{\
	if (!this->CheckVirtualBagHaveGrids(item->GetBagsBelong(), item_wrapper.num)) return false;\
	return this->PutItemIntoCorrespondBag(item_wrapper, reason_type);\
}\
if (!this->HaveSpace(item->GetItemColumn()))\
{\
	if (!this->HaveSpace(ITEM_COLUMN_TYPE_TEMPORARY))\
	{\
		m_module_mgr->GetRole()->NoticeNum(errornum::EN_TEMPORARY_KNAPSACK_NO_SPACE);\
		return false;\
	}\
	this->CertainPut(ITEM_COLUMN_TYPE_TEMPORARY, item_wrapper, reason_type, out_column, out_index, drop_monster_id, is_notice);\
}\
else\
{\
	this->CertainPut(item->GetItemColumn(), item_wrapper, reason_type, out_column, out_index, drop_monster_id, is_notice);\
}

bool Knapsack::Put(const ItemConfigData &item_configdata, int reason_type, int* out_column /* = NULL */, int *out_index /* = NULL */, UInt16 drop_monster_id /* = 0 */, int gold_price /* = 0 */, bool is_notice/* = true */)
{
	this->ClearErrorNotice();

	const ItemBase *itembase = ITEMPOOL->GetItem(item_configdata.item_id);
	if (NULL == itembase) return false;

	static ItemDataWrapper temp_item_wrapper;
	temp_item_wrapper.item_id = item_configdata.item_id;
	temp_item_wrapper.num = item_configdata.num;
	temp_item_wrapper.is_bind = (itembase->IsBind() || item_configdata.is_bind) ? 1 : 0;
	temp_item_wrapper.invalid_time = itembase->CalInvalidTime();
	temp_item_wrapper.gold_price = gold_price;

	return this->Put(temp_item_wrapper, reason_type, out_column, out_index, 0, is_notice);
}

bool Knapsack::Put(const ItemDataWrapper &arg_item_wrapper, int reason_type, int* out_column /* = NULL */, int *out_index /* = NULL */, UInt16 drop_monster_id /* = 0 */, bool is_notice /* = true */)
{
	this->ClearErrorNotice();

	ItemDataWrapper item_wrapper = arg_item_wrapper;

	const ItemBase *itembase = ITEMPOOL->GetItem(item_wrapper.item_id);
	if (NULL == itembase) return false;

	// 自动激活伙伴皮肤
	if (m_module_mgr->GetPartner()->AutoActiveSkin(itembase->GetItemId(), item_wrapper.num)) return true;

	if (itembase->GetItemType() == I_TYPE_CONVERT_ITEM)
	{
		int new_item_id = LOGIC_CONFIG->GetKnapsackConfig()->GetItemIdConvertByJob(item_wrapper.item_id, m_module_mgr->GetRole()->GetAppearanceProf());

		if (new_item_id != 0)
		{
			item_wrapper.item_id = new_item_id;

			itembase = ITEMPOOL->GetItem(item_wrapper.item_id);
			if (NULL == itembase)
			{
				return false;
			}
		}
	}

	if (itembase->GetItemType() == I_TYPE_EXPENSE)
	{
		if (this->PutExpenseItemAutoUse(itembase, item_wrapper.num, item_wrapper.is_bind > 0, item_wrapper.invalid_time, reason_type == PUT_REASON_XUN_BAO ? false : is_notice))
		{
				return true;
			}
		}

	item_wrapper.num *= m_module_mgr->GetRoleSkill()->CheckIsGather(itembase->GetItemSubType(), reason_type);

	if (item_wrapper.num <= itembase->GetPileLimit())
	{
		bool ret = this->PutHelper(item_wrapper, reason_type, out_column, out_index, drop_monster_id, is_notice);
		if (!ret)
		{
			return ret;
		}
	}
	else
	{
		// 超过堆叠上限，尝试逐次放入 (假定都采用了CheckForPut判断有足够位置Put)
		static ItemDataWrapper remain_item_data;
		remain_item_data = item_wrapper;

		static ItemDataWrapper tmp_wrapper;
		tmp_wrapper = remain_item_data;

		int times = 0;
		bool final_ret = false;
		while (remain_item_data.num > 0 && times < 1000)
		{
			++times;

			tmp_wrapper.num = GetMin(remain_item_data.num, static_cast<int>(itembase->GetPileLimit()));
			remain_item_data.num -= tmp_wrapper.num;

			bool ret = this->PutHelper(tmp_wrapper, reason_type, out_column, out_index, drop_monster_id, is_notice);
			final_ret = final_ret || ret;
			if (!ret)
			{
				return final_ret;
			}
		}
	}

	if (PUT_REASON_DROP == reason_type)
	{
		EventHandler::Instance().OnRoleDrop(m_module_mgr->GetRole(), item_wrapper.item_id, item_wrapper.num);
	}

	this->ClearErrorNotice();

	return true;
}

bool Knapsack::Put2(int item_id, int num, int reason_type, bool is_bind,int * out_column, int * out_index, UInt16 drop_monster_id, int gold_price, bool is_notice)
{
	ItemConfigData t;
	t.item_id = (ItemID)item_id;
	t.num = num;
	t.is_bind = is_bind;

	return this->Put(t, reason_type, out_column, out_index, drop_monster_id, gold_price, is_notice);;
}

#define PUT_LIST_CHECKER(item_list) \
if (NULL == item_list || list_num <= 0 || list_num > KNAPSACK_MAX_GRID_NUM) return false;\
int need_virtual_grid[B_BELONG_TYPE_MAX] = {0};\
for (int i = 0; i < list_num; ++i)\
{\
	if (item_list[i].num <= 0)\
	{\
		m_module_mgr->GetRole()->NoticeNum(errornum::EN_KNAPSACK_PUT_ERROR);\
		return false;\
	}\
	const ItemBase *itembase = ITEMPOOL->GetItem(item_list[i].item_id);\
	if (NULL == itembase || itembase->GetBagsBelong() >= B_BELONG_TYPE_MAX || itembase->GetBagsBelong() < 0)\
	{\
		m_module_mgr->GetRole()->NoticeNum(errornum::EN_KNAPSACK_PUT_ERROR);\
		return false;\
	}\
	if (B_BELONG_KNAPSACK == itembase->GetBagsBelong())\
	{\
		need_grid_num_list[itembase->GetItemColumn()] += item_list[i].num / itembase->GetPileLimit();\
		if (item_list[i].num % itembase->GetPileLimit() > 0)\
		{\
			need_grid_num_list[itembase->GetItemColumn()] += 1;\
		}\
	}\
	else \
	{\
		if (itembase->IsNetValueItem())\
		{\
			need_virtual_grid[itembase->GetBagsBelong()] += item_list[i].num;\
		}\
		else\
		{\
			need_virtual_grid[itembase->GetBagsBelong()] += item_list[i].num / itembase->GetPileLimit();\
			if (item_list[i].num % itembase->GetPileLimit() > 0)\
			{\
				need_virtual_grid[itembase->GetBagsBelong()] += 1;\
			}\
		}\
	}\
}\
int total_need_more_grid_num = 0;\
for (int column = ITEM_COLUMN_TYPE_MIN; column < ITEM_COLUMN_TYPE_NORMAL_COLUMN_END; ++column)\
{\
	int need_more_grid_num = 0;\
	if (!this->CheckEmptyGridNoLessThan(column, need_grid_num_list[column], &need_more_grid_num, false))\
	{\
		total_need_more_grid_num += need_more_grid_num;\
		need_more_grid_num_list[column] = need_more_grid_num;\
	}\
}\
if (!this->CheckEmptyGridNoLessThanInTemporary(total_need_more_grid_num))\
{\
	m_module_mgr->GetRole()->NoticeNum(errornum::EN_TEMPORARY_KNAPSACK_NO_SPACE);\
	return false; \
}\
for (int virtual_bag_type = B_BELONG_PET_BAG; virtual_bag_type < B_BELONG_TYPE_MAX; ++virtual_bag_type)\
{\
	if (need_virtual_grid[virtual_bag_type] <= 0) continue;\
	if (!this->CheckVirtualBagHaveGrids(virtual_bag_type, need_virtual_grid[virtual_bag_type]))\
	{\
		return false;\
	}\
}

//不带临时背包
#define PUT_LIST_CHECKER2(item_list) \
if (NULL == item_list || list_num <= 0 || list_num > KNAPSACK_MAX_GRID_NUM) return false;\
int need_virtual_grid[B_BELONG_TYPE_MAX] = {0};\
for (int i = 0; i < list_num; ++i)\
{\
	if (item_list[i].num <= 0)\
	{\
		m_module_mgr->GetRole()->NoticeNum(errornum::EN_KNAPSACK_PUT_ERROR);\
		return false;\
	}\
	const ItemBase *itembase = ITEMPOOL->GetItem(item_list[i].item_id);\
	if (NULL == itembase || itembase->GetBagsBelong() >= B_BELONG_TYPE_MAX || itembase->GetBagsBelong() < 0)\
	{\
		m_module_mgr->GetRole()->NoticeNum(errornum::EN_KNAPSACK_PUT_ERROR);\
		return false;\
	}\
	if (B_BELONG_KNAPSACK == itembase->GetBagsBelong())\
	{\
		need_grid_num_list[itembase->GetItemColumn()] += item_list[i].num / itembase->GetPileLimit();\
		if (item_list[i].num % itembase->GetPileLimit() > 0)\
		{\
			need_grid_num_list[itembase->GetItemColumn()] += 1;\
		}\
	}\
	else \
	{\
		++need_virtual_grid[itembase->GetBagsBelong()];\
	}\
}\
int total_need_more_grid_num = 0;\
for (int column = ITEM_COLUMN_TYPE_MIN; column < ITEM_COLUMN_TYPE_NORMAL_COLUMN_END; ++column)\
{\
	int need_more_grid_num = 0;\
	if (!this->CheckEmptyGridNoLessThan(column, need_grid_num_list[column], &need_more_grid_num, false))\
	{\
		total_need_more_grid_num += need_more_grid_num;\
		need_more_grid_num_list[column] = need_more_grid_num;\
	}\
}\
if(total_need_more_grid_num > 0)\
{\
	return false;\
}\
for (int virtual_bag_type = B_BELONG_PET_BAG; virtual_bag_type < B_BELONG_TYPE_MAX; ++virtual_bag_type)\
{\
	if (need_virtual_grid[virtual_bag_type] <= 0) continue;\
	if (!this->CheckVirtualBagHaveGrids(virtual_bag_type, need_virtual_grid[virtual_bag_type]))\
	{\
		return false;\
	}\
}

bool Knapsack::PutList(short put_list_num, const ItemConfigData * item_put_list, int reason_type, bool is_noitce)
{
	this->ClearErrorNotice();

	static int need_grid_num_list[ITEM_COLUMN_TYPE_MAX];		// 某COLUMN总计所需格子数
	static int need_more_grid_num_list[ITEM_COLUMN_TYPE_MAX];	// 某COLUMN所需临时背包格子数
	memset(need_grid_num_list, 0, sizeof(need_grid_num_list));
	memset(need_more_grid_num_list, 0, sizeof(need_more_grid_num_list));

	static ItemConfigData item_configdata_list[ITEM_COLUMN_GRID_NUM_MAX];
	short list_num = 0;
	if (!this->MergeBeforePutList(put_list_num, item_put_list, &list_num, item_configdata_list, ARRAY_ITEM_COUNT(item_configdata_list)))
	{
		m_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return false;
	}

	PUT_LIST_CHECKER(item_configdata_list);

	static ItemDataWrapper temp_item_wrapper;
	for (int i = 0; i < list_num; ++i)
	{
		ItemConfigData item_con = item_configdata_list[i];
		const ItemBase *itembase = ITEMPOOL->GetItem(item_con.item_id);
		if (NULL == itembase) return false;

		if (itembase->GetItemType() == I_TYPE_CONVERT_ITEM)
		{
			int new_item_id = LOGIC_CONFIG->GetKnapsackConfig()->GetItemIdConvertByJob(item_con.item_id, m_module_mgr->GetRole()->GetAppearanceProf());

			if (new_item_id != 0)
			{
				item_con.item_id = new_item_id;

				itembase = ITEMPOOL->GetItem(item_con.item_id);
				if (NULL == itembase)
				{
					gamelog::g_log_knapsack.printf(LL_INFO, "%s role[%d, %s] item_type[%d] old_item_id[%d] new_item_id[%d]", __FUNCTION__, m_module_mgr->GetRole()->GetUID(),
						m_module_mgr->GetRole()->GetName(), I_TYPE_CONVERT_ITEM, item_con.item_id, new_item_id);
					continue;
				}
			}
		}



		// 自动激活伙伴皮肤
		if (m_module_mgr->GetPartner()->AutoActiveSkin(itembase->GetItemId(), item_con.num))
		{
			continue;
		}

		if (itembase->GetItemType() == I_TYPE_EXPENSE)
		{
			if (this->PutExpenseItemAutoUse(itembase, item_con.num, item_con.is_bind, 0, is_noitce))
			{
					continue;
				}
			}

		item_con.num *= m_module_mgr->GetRoleSkill()->CheckIsGather(itembase->GetItemSubType(), reason_type);

		int out_index = -1;
		int out_column = -1;

		if (B_BELONG_KNAPSACK == itembase->GetBagsBelong())
		{
			int times = 0;
			int item_num = item_con.num;
			while (item_num > 0 && times < 100)
			{
				temp_item_wrapper.item_id = item_con.item_id;
				temp_item_wrapper.num = GetMin(item_num, static_cast<int>(itembase->GetPileLimit()));
				temp_item_wrapper.is_bind = (itembase->IsBind() || item_con.is_bind) ? 1 : 0;
				temp_item_wrapper.invalid_time = itembase->CalInvalidTime();
				temp_item_wrapper.item_unique_id = 0LL;
				UniqueIDGenerator::Instance()->GenUniqueID(&temp_item_wrapper.item_unique_id);

				item_num -= temp_item_wrapper.num;
				++times;

				int column = itembase->GetItemColumn();
				if (need_grid_num_list[column] > need_more_grid_num_list[column] || (0 == need_more_grid_num_list[column])) // 当COLUMN总计所需格子数 > COLUMN需要临时背包格子数，则不需要放入临时背包
				{
					this->CertainPut(column, temp_item_wrapper, reason_type, &out_column, &out_index, 0, is_noitce);
					--need_grid_num_list[column];
				}
				else // 当COLUMN总计所需格子数 <= COLUMN需要临时背包格子数，则需要放入临时背包
				{
					this->CertainPut(ITEM_COLUMN_TYPE_TEMPORARY, temp_item_wrapper, reason_type, &out_column, &out_index, 0, is_noitce);
				}
			}
		}
		else
		{
			temp_item_wrapper.item_id = item_con.item_id;
			temp_item_wrapper.num = item_con.num;
			temp_item_wrapper.is_bind = (itembase->IsBind() || item_con.is_bind) ? 1 : 0;
			temp_item_wrapper.invalid_time = itembase->CalInvalidTime();
			temp_item_wrapper.item_unique_id = 0LL;
			UniqueIDGenerator::Instance()->GenUniqueID(&temp_item_wrapper.item_unique_id);

			this->PutItemIntoCorrespondBag(temp_item_wrapper, reason_type);
		}
	}

	if (PUT_REASON_DROP == reason_type)
	{
		for (int i = 0; i < list_num; ++i)
		{
			const ItemConfigData & item_con = item_configdata_list[i];
			EventHandler::Instance().OnRoleDrop(m_module_mgr->GetRole(), item_con.item_id, item_con.num);
		}
	}

	this->ClearErrorNotice();

	return true;
}

bool Knapsack::MergeBeforePutList(short list_num, const ItemConfigData *item_configdata_list, ARG_OUT short* out_list_num, ARG_OUT ItemConfigData* out_item_list, ARG_IN short out_item_max_num)
{
	*out_list_num = 0;
	for (int i = 0; i < list_num; ++i)
	{
		const ItemBase * item = ITEMPOOL->GetItem(item_configdata_list[i].item_id);
		if (NULL == item || item->IsNetValueItem() || 1 == item->GetPileLimit())
		{
			out_item_list[(*out_list_num)++] = item_configdata_list[i];
			continue; // 净值物品和不允许堆叠的物品不能堆叠
		}

		bool is_found = false;
		for (int k = 0; k < *out_list_num; ++k)
		{
			if (item_configdata_list[i].item_id == out_item_list[k].item_id
				&& item_configdata_list[i].is_bind == out_item_list[k].is_bind)
			{
				out_item_list[k].num += item_configdata_list[i].num;
				is_found = true;
				break;
			}
		}

		if (!is_found)
		{
			if (*out_list_num < out_item_max_num)
			{
				out_item_list[(*out_list_num)++] = item_configdata_list[i];
			}
			else
			{
				return false;
			}
		}
	}

	return true;
}

void Knapsack::MergeItemList(std::vector<ItemConfigData> item_configdata_list, std::vector<ItemConfigData> & out_item_list)
{
	if(item_configdata_list.empty()) return;

	std::map<ItemID, ItemConfigData> merge_item_list;
	for (int i = 0; i < (int)item_configdata_list.size(); i++)
	{
		if (merge_item_list.end() != merge_item_list.find(item_configdata_list[i].item_id))
		{
			merge_item_list[item_configdata_list[i].item_id].num += item_configdata_list[i].num;
		}
		else
		{
			merge_item_list[item_configdata_list[i].item_id] = item_configdata_list[i];
		}
	}
	for (std::map<ItemID, ItemConfigData>::const_iterator it = merge_item_list.begin(); it != merge_item_list.end(); ++it)
	{
		out_item_list.push_back(it->second);
	}
}


bool Knapsack::PutList2(short list_num, const ItemConfigData * item_configdata_list, int reason_type, int get_reson, int param1)
{
	if (list_num <= 0)
	{
		return false;
	}

	bool ret = this->PutList(list_num, item_configdata_list, reason_type, reason_type == PUT_REASON_TASK_REWARD);

	if (ret && reason_type != GET_REASON_INVALID)
	{
		this->NotifyGetItemList(list_num, item_configdata_list, get_reson, param1);
	}

	return ret;
}

bool Knapsack::PutList3(short put_list_num, const ItemConfigData * item_put_list, int reason_type, std::vector<std::pair<int, int> > * out_column_index_list, bool is_noitce)
{
	this->ClearErrorNotice();

	static int need_grid_num_list[ITEM_COLUMN_TYPE_MAX];		// 某COLUMN总计所需格子数
	static int need_more_grid_num_list[ITEM_COLUMN_TYPE_MAX];	// 某COLUMN所需临时背包格子数
	memset(need_grid_num_list, 0, sizeof(need_grid_num_list));
	memset(need_more_grid_num_list, 0, sizeof(need_more_grid_num_list));

	static ItemConfigData item_configdata_list[ITEM_COLUMN_GRID_NUM_MAX];
	short list_num = 0;
	if (!this->MergeBeforePutList(put_list_num, item_put_list, &list_num, item_configdata_list, ARRAY_ITEM_COUNT(item_configdata_list)))
	{
		m_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return false;
	}

	if (NULL != out_column_index_list) out_column_index_list->reserve(list_num);

	PUT_LIST_CHECKER(item_configdata_list);

	static ItemDataWrapper temp_item_wrapper;
	for (int i = 0; i < list_num; ++i)
	{
		ItemConfigData item_con = item_configdata_list[i];
		const ItemBase * itembase = ITEMPOOL->GetItem(item_con.item_id);
		if (NULL == itembase) return false;

		if (itembase->GetItemType() == I_TYPE_CONVERT_ITEM)
		{
			int new_item_id = LOGIC_CONFIG->GetKnapsackConfig()->GetItemIdConvertByJob(item_con.item_id, m_module_mgr->GetRole()->GetAppearanceProf());

			if (new_item_id != 0)
			{
				item_con.item_id = new_item_id;

				itembase = ITEMPOOL->GetItem(item_con.item_id);
				if (NULL == itembase)
				{
					gamelog::g_log_knapsack.printf(LL_INFO, "%s role[%d, %s] item_type[%d] old_item_id[%d] new_item_id[%d]", __FUNCTION__, m_module_mgr->GetRole()->GetUID(),
								       m_module_mgr->GetRole()->GetName(), I_TYPE_CONVERT_ITEM, item_con.item_id, new_item_id);
					continue;
				}
			}
		}


		// 自动激活伙伴皮肤
		if (m_module_mgr->GetPartner()->AutoActiveSkin(itembase->GetItemId(), item_con.num))
		{
			continue;
		}

		if (itembase->GetItemType() == I_TYPE_EXPENSE)
		{
			if (this->PutExpenseItemAutoUse(itembase, item_con.num, item_con.is_bind, 0, is_noitce))
			{
					continue;
				}
			}

		item_con.num *= m_module_mgr->GetRoleSkill()->CheckIsGather(itembase->GetItemSubType(), reason_type);

		int out_index = -1;
		int out_column = -1;

		if (B_BELONG_KNAPSACK == itembase->GetBagsBelong())
		{
			int times = 0;
			int item_num = item_con.num;
			while (item_num > 0 && times < 100)
			{
				temp_item_wrapper.item_id = item_con.item_id;
				temp_item_wrapper.num = GetMin(item_num, static_cast<int>(itembase->GetPileLimit()));
				temp_item_wrapper.is_bind = (itembase->IsBind() || item_con.is_bind) ? 1 : 0;
				temp_item_wrapper.invalid_time = itembase->CalInvalidTime();
				temp_item_wrapper.item_unique_id = 0LL;
				UniqueIDGenerator::Instance()->GenUniqueID(&temp_item_wrapper.item_unique_id);

				item_num -= temp_item_wrapper.num;
				++times;

				int column = itembase->GetItemColumn();
				if (need_grid_num_list[column] > need_more_grid_num_list[column] || (0 == need_more_grid_num_list[column])) // 当COLUMN总计所需格子数 > COLUMN需要临时背包格子数，则不需要放入临时背包
				{
					this->CertainPut(column, temp_item_wrapper, reason_type, &out_column, &out_index, is_noitce);
					--need_grid_num_list[column];
				}
				else // 当COLUMN总计所需格子数 <= COLUMN需要临时背包格子数，则需要放入临时背包
				{
					this->CertainPut(ITEM_COLUMN_TYPE_TEMPORARY, temp_item_wrapper, reason_type, &out_column, &out_index, is_noitce);
				}

				if (out_column_index_list != NULL)
				{
					out_column_index_list->push_back(std::make_pair(out_column, out_index));
				}
			}
		}
		else
		{
			temp_item_wrapper.item_id = item_con.item_id;
			temp_item_wrapper.num = item_con.num;
			temp_item_wrapper.is_bind = (itembase->IsBind() || item_con.is_bind) ? 1 : 0;
			temp_item_wrapper.invalid_time = itembase->CalInvalidTime();
			temp_item_wrapper.item_unique_id = 0LL;
			UniqueIDGenerator::Instance()->GenUniqueID(&temp_item_wrapper.item_unique_id);

			this->PutItemIntoCorrespondBag(temp_item_wrapper, reason_type);
		}
	}

	if (PUT_REASON_DROP == reason_type)
	{
		for (int i = 0; i < list_num; ++i)
		{
			const ItemConfigData & item_con = item_configdata_list[i];
			EventHandler::Instance().OnRoleDrop(m_module_mgr->GetRole(), item_con.item_id, item_con.num);
		}
	}

	this->ClearErrorNotice();

	return true;
}

bool Knapsack::PutList(short list_num, const ItemDataWrapper *item_wrapper_list, int reason_type, int *out_column_list /*= NULL*/, int *out_index_list /*= NULL*/)
{
	this->ClearErrorNotice();

	static int need_grid_num_list[ITEM_COLUMN_TYPE_MAX];		// 某COLUMN总计所需格子数
	static int need_more_grid_num_list[ITEM_COLUMN_TYPE_MAX];	// 某COLUMN所需临时背包格子数
	memset(need_grid_num_list, 0, sizeof(need_grid_num_list));
	memset(need_more_grid_num_list, 0, sizeof(need_more_grid_num_list));

	PUT_LIST_CHECKER(item_wrapper_list);

	for (int i = 0; i < list_num; ++i)
	{
		ItemDataWrapper item_con = item_wrapper_list[i];
		const ItemBase *itembase = ITEMPOOL->GetItem(item_con.item_id);
		if (NULL == itembase) return false;

		if (itembase->GetItemType() == I_TYPE_CONVERT_ITEM)
		{
			int new_item_id = LOGIC_CONFIG->GetKnapsackConfig()->GetItemIdConvertByJob(item_con.item_id, m_module_mgr->GetRole()->GetAppearanceProf());

			if (new_item_id != 0)
			{
				item_con.item_id = new_item_id;

				itembase = ITEMPOOL->GetItem(item_con.item_id);
				if (NULL == itembase)
				{
					gamelog::g_log_knapsack.printf(LL_INFO, "%s role[%d, %s] item_type[%d] old_item_id[%d] new_item_id[%d]", __FUNCTION__, m_module_mgr->GetRole()->GetUID(),
						m_module_mgr->GetRole()->GetName(), I_TYPE_CONVERT_ITEM, item_con.item_id, new_item_id);
					continue;
				}
			}
		}

		// 自动激活伙伴皮肤
		if (m_module_mgr->GetPartner()->AutoActiveSkin(itembase->GetItemId(), item_con.num))
		{
			continue;
		}

		if (itembase->GetItemType() == I_TYPE_EXPENSE)
		{
			if (this->PutExpenseItemAutoUse(itembase, item_con.num, item_con.is_bind > 0, item_con.invalid_time, true))
			{
					continue;
				}
			}

		int *out_index = (NULL == out_index_list) ? NULL : &out_index_list[i];
		int *out_column = (NULL == out_column_list) ? NULL : &out_column_list[i];

		if (B_BELONG_KNAPSACK == itembase->GetBagsBelong())
		{
			int times = 0;
			int item_num = item_con.num;
			int column = itembase->GetItemColumn();
			static ItemDataWrapper temp_item_wrapper;
			temp_item_wrapper = item_con;
			while (item_num > 0 && times < 100)
			{
				temp_item_wrapper.num = GetMin(item_num, static_cast<int>(itembase->GetPileLimit()));
				item_num -= temp_item_wrapper.num;
				++times;

				temp_item_wrapper.item_unique_id = 0LL;
				UniqueIDGenerator::Instance()->GenUniqueID(&temp_item_wrapper.item_unique_id);

				if (need_grid_num_list[column] > need_more_grid_num_list[column] || (0 == need_more_grid_num_list[column])) // 当COLUMN总计所需格子数 > COLUMN需要临时背包格子数，则不需要放入临时背包
				{
					this->CertainPut(column, temp_item_wrapper, reason_type, out_column, out_index);
					--need_grid_num_list[column];
				}
				else // 当COLUMN总计所需格子数 <= COLUMN需要临时背包格子数，则需要放入临时背包
				{
					this->CertainPut(ITEM_COLUMN_TYPE_TEMPORARY, temp_item_wrapper, reason_type, out_column, out_index);
				}
			}

		}
		else
		{
			UniqueIDGenerator::Instance()->GenUniqueID(&item_con.item_unique_id);
			this->PutItemIntoCorrespondBag(item_con, reason_type);
		}
	}

	if (PUT_REASON_DROP == reason_type)
	{
		for (int i = 0; i < list_num; ++i)
		{
			const ItemDataWrapper& item_con = item_wrapper_list[i];
			EventHandler::Instance().OnRoleDrop(m_module_mgr->GetRole(), item_con.item_id, item_con.num);
		}
	}

	this->ClearErrorNotice();

	return true;
}

bool Knapsack::PutVec(const std::vector<ItemConfigData> & item_configdata_list, int reason_type)
{
	if (item_configdata_list.empty())
	{
		return true;
	}

	return this->PutList((short)item_configdata_list.size(), &item_configdata_list[0], reason_type);
}

bool Knapsack::PutVec(const std::vector<ItemDataWrapper>& item_wrapper_list, int reason_type)
{
	if (item_wrapper_list.empty())
	{
		return true;
	}

	return this->PutList((short)item_wrapper_list.size(), &item_wrapper_list[0], reason_type);
}

int Knapsack::Count(ItemID item_id, bool is_check_temporary, bool is_check_storage)
{
	if (ItemBase::INVALID_ITEM_ID == item_id) return 0;

	const ItemBase* item_base = ITEMPOOL->GetItem(item_id);
	if (NULL == item_base) return 0;

	int column = item_base->GetItemColumn();
	int count = 0;
	for (int i = 0; i < m_valid_grid_index_end_list[column]; ++i)
	{
		if (NULL != m_grid_list[column][i] && m_grid_list[column][i]->item_id == item_id)
		{
			count += m_grid_list[column][i]->num;
		}
	}

	if (is_check_temporary)
	{
		for (int i = 0; i < m_valid_grid_index_end_list[ITEM_COLUMN_TYPE_TEMPORARY]; ++i)
		{
			if (NULL != m_grid_list[ITEM_COLUMN_TYPE_TEMPORARY][i] && m_grid_list[ITEM_COLUMN_TYPE_TEMPORARY][i]->item_id == item_id)
			{
				count += m_grid_list[ITEM_COLUMN_TYPE_TEMPORARY][i]->num;
			}
		}
	}

	if (is_check_storage)
	{
		for (int i = 0; i < m_valid_grid_index_end_list[ITEM_COLUMN_TYPE_STORAGE]; ++i)
		{
			if (NULL != m_grid_list[ITEM_COLUMN_TYPE_STORAGE][i] && m_grid_list[ITEM_COLUMN_TYPE_STORAGE][i]->item_id == item_id)
			{
				count += m_grid_list[ITEM_COLUMN_TYPE_STORAGE][i]->num;
			}
		}
	}

	return count;
}

bool Knapsack::CheckForPutList(short list_num, const ItemConfigData *item_configdata_list, bool need_check_temperary /* = true */)
{
	static int need_grid_num_list[ITEM_COLUMN_TYPE_MAX];		// 某COLUMN总计所需格子数
	static int need_more_grid_num_list[ITEM_COLUMN_TYPE_MAX];	// 某COLUMN所需临时背包格子数
	memset(need_grid_num_list, 0, sizeof(need_grid_num_list));
	memset(need_more_grid_num_list, 0, sizeof(need_more_grid_num_list));

	this->ClearErrorNotice();

	if (need_check_temperary)
	{
		PUT_LIST_CHECKER(item_configdata_list);
	}
	else
	{
		PUT_LIST_CHECKER2(item_configdata_list);
	}

	return true;
}

bool Knapsack::CheckForPutList2(const std::vector<ItemConfigData> & item_configdata_list, bool need_check_temperary)
{
	if (item_configdata_list.empty())
	{
		return true;
	}

	return this->CheckForPutList((short)item_configdata_list.size(), &item_configdata_list[0], need_check_temperary);
}

bool Knapsack::CheckForPutList(short list_num, const ItemDataWrapper *item_wrapper_list, bool need_check_temperary /* = true */)
{
	static int need_grid_num_list[ITEM_COLUMN_TYPE_MAX];		// 某COLUMN总计所需格子数
	static int need_more_grid_num_list[ITEM_COLUMN_TYPE_MAX];	// 某COLUMN所需临时背包格子数
	memset(need_grid_num_list, 0, sizeof(need_grid_num_list));
	memset(need_more_grid_num_list, 0, sizeof(need_more_grid_num_list));

	this->ClearErrorNotice();

	if (need_check_temperary)
	{
		PUT_LIST_CHECKER(item_wrapper_list);
	}
	else
	{
		PUT_LIST_CHECKER2(item_wrapper_list);
	}

	return true;
}

bool Knapsack::CheckForPutListWithoutTemporary(short list_num, const ItemConfigData * item_configdata_list)
{
	static int need_grid_num_list[ITEM_COLUMN_TYPE_MAX];		// 某COLUMN总计所需格子数
	static int need_more_grid_num_list[ITEM_COLUMN_TYPE_MAX];	// 某COLUMN所需临时背包格子数
	memset(need_grid_num_list, 0, sizeof(need_grid_num_list));
	memset(need_more_grid_num_list, 0, sizeof(need_more_grid_num_list));

	this->ClearErrorNotice();

	PUT_LIST_CHECKER2(item_configdata_list);

	return true;
}

bool Knapsack::CheckForPutListWithoutTemporaryVec(const std::vector<ItemConfigData> & item_configdata_list)
{
	if (item_configdata_list.empty())
	{
		return true;
	}

	return this->CheckForPutListWithoutTemporary((short)item_configdata_list.size(), &item_configdata_list[0]);
}


bool Knapsack::CheckEmptyGridNoLessThan(int column, int check_num, int * out_need_num /* = NULL */, bool is_notice_no_space /* = false */)
{
	if (check_num < 0)
	{
		this->AddErrorNotice(errornum::EN_INPUT_PARAM_ERROR);
		return false;
	}

	if (0 == check_num)
	{
		return true;
	}

	if (column < ITEM_COLUMN_TYPE_MIN || column >= ITEM_COLUMN_TYPE_MAX)
	{
		this->AddErrorNotice(errornum::EN_INPUT_PARAM_ERROR);
		return false;
	}

	if (m_curr_valid_grid_num_list[column] - m_valid_grid_index_end_list[column] >= check_num)
	{
		return true;
	}

	int count = 0;

	for (int i = 0; i < m_curr_valid_grid_num_list[column]; ++i)
	{
		if (NULL == m_grid_list[column][i] || m_grid_list[column][i]->Invalid())
		{
			if (++count >= check_num) return true;
		}
	}
	if (NULL != out_need_num)
	{
		*out_need_num = *out_need_num + (check_num - count); // check_num 必定小于 count
	}

	if (is_notice_no_space)
	{
		this->SendNoSpaceNotice(column);
	}
	else
	{
		this->AddNoSpaceErrorNotice(column);
	}
	return false;
}


bool Knapsack::CheckForPut(ItemID item_id, int num, bool need_check_temporary)
{
	this->ClearErrorNotice();

	if (num <= 0)
	{
		this->AddErrorNotice(errornum::EN_INPUT_PARAM_ERROR);
		return false;
	}

	const ItemBase *item_base = ITEMPOOL->GetItem(item_id);
	if (NULL == item_base)
	{
		this->AddErrorNotice(errornum::EN_UNKNOWN_ERROR);
		return false;
	}

	short pile_limit = item_base->GetPileLimit();
	int need_empty_grid_num = num / pile_limit;
	if (num % pile_limit > 0) ++need_empty_grid_num;

	if (B_BELONG_KNAPSACK != item_base->GetBagsBelong())
	{
		return this->CheckVirtualBagHaveGrids(item_base->GetBagsBelong(), need_empty_grid_num);
	}

	int need_more_grid_in_tmp = 0;
	bool ret = this->CheckEmptyGridNoLessThan(item_base->GetItemColumn(), need_empty_grid_num, &need_more_grid_in_tmp);
	if (!ret && need_check_temporary)
	{
		ret = this->CheckEmptyGridNoLessThanInTemporary(need_more_grid_in_tmp);
	}

	return ret;
}

bool Knapsack::CheckForPut2(const ItemConfigData & item, bool need_notice, bool need_check_temporary)
{
	bool ret = CheckForPut(item.item_id, item.num, need_check_temporary);

	if (need_notice && !ret)
	{
		m_module_mgr->GetRole()->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
	}

	return ret;
}

bool Knapsack::CheckForPutItemDataWrapper(const ItemDataWrapper & item, bool need_notice, bool need_check_temporary)
{
	bool ret = CheckForPut(item.item_id, item.num, need_check_temporary);

	if (need_notice && !ret)
	{
		m_module_mgr->GetRole()->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
	}

	return ret;
}

bool Knapsack::CheckEmptyGridNoLessThanInTemporary(int check_num)
{
	return this->CheckEmptyGridNoLessThan(ITEM_COLUMN_TYPE_TEMPORARY, check_num);
}

bool Knapsack::HaveSpaceInTemporary()
{
	this->ClearErrorNotice();

	return CheckEmptyGridNoLessThanInTemporary(1);
}

bool Knapsack::CheckEmptyGridNoLessThanForItem(ItemID item_id, int num)
{
	const ItemBase* item = ITEMPOOL->GetItem(item_id);
	if (NULL == item) return false;

	return this->CheckEmptyGridNoLessThan(item->GetItemColumn(), num, NULL, false);
}

bool Knapsack::CheckVirtualBagHaveGrids(int bags_type, int grid_num)
{
	if (B_BELONG_KNAPSACK == bags_type || 0 == grid_num) return true;

	bool ret = false;
	UNSTD_STATIC_CHECK(8 == B_BELONG_TYPE_MAX);		// 如果加了新的背包类型，记得加这个位置的开关
	switch (bags_type)
	{
	case B_BELONG_PET_BAG:
		ret = m_module_mgr->GetPet()->HaveSpace(grid_num);
		if (!ret)m_module_mgr->NoticeNum(errornum::EN_PET_COUNT_LIMIT2);
		break;
	case B_BELONG_PET_GOD_PRINT:
		ret = m_module_mgr->GetPet()->HasPetGodPrintBagNum(grid_num);
		if (!ret)m_module_mgr->NoticeNum(errornum::EN_KNAPSACK_GOD_PRINT_COLUMN_NO_SPACE);
		break;
	case B_BELONG_PARTNER_SPIRIT_PACT:
		{//除非物品超过 INT_MAX (系统里有日志)
			ret = true;
		}
		break;
	case B_BELONG_ZAO_HUA_BAG:
		{
			ret = m_module_mgr->GetZaoHuaEquipManager()->HasBagNum(grid_num);
		}
		break;
	case B_BELONG_HOLY_EQUIP_BAG:
		{
			ret = m_module_mgr->GetHolyEquipManager()->HasHolyItemBagNum(grid_num);
			if (!ret)m_module_mgr->NoticeNum(errornum::EN_HOLY_EQUIP_BAG_NO_SPACE);
		}
		break;
	case B_BELONG_RUNE_STONE_BAG:
		{
			ret = m_module_mgr->GetHolyEquipManager()->HasFuShiBagNum(grid_num);
			if (!ret)m_module_mgr->NoticeNum(errornum::EN_FU_SHI_BAG_NO_SPACE);
		}
		break;
	case B_BELONG_HUAN_SHOU_BAG:
		{
			ret = m_module_mgr->GetRoleHuanShou()->HasBagNum(grid_num);
			if (!ret)m_module_mgr->NoticeNum(errornum::EN_HUAN_SHOU_BAG_NOT_SPACE);
		}
		break;
	default:
		break;
	}

	return ret;
}

const char * g_GetPutReasonTypeStr(int reason);
bool Knapsack::PutItemIntoCorrespondBag(const ItemDataWrapper &item_wrapper, int reason_type)
{
	const ItemBase * item_ib = ITEMPOOL->GetItem(item_wrapper.item_id);
	if (NULL == item_ib) return false;

	bool ret = false;
	UNSTD_STATIC_CHECK(8 == B_BELONG_TYPE_MAX);		// 如果加了新的背包类型，记得加这个位置的开关
	switch (item_ib->GetBagsBelong())
	{
	case B_BELONG_RUNE_STONE_BAG:
		{
			if (I_TYPE_RUNE_STONE == item_ib->GetItemType())
			{
				for (int num = 0; num < item_wrapper.num; ++num)
				{
					if (item_wrapper.has_param > 0)
					{
						ret = m_module_mgr->GetHolyEquipManager()->AddNewFuShiItem(&item_wrapper, reason_type);
						if (!ret) break;
					}
					else
					{
						const ItemBase * item_base = ITEMPOOL->GetItem(item_wrapper.item_id);
						if (item_base != NULL)
						{
							const ItemGridData* item_grid = m_module_mgr->GetHolyEquipManager()->AddFuShiItem(item_wrapper.item_id, reason_type);
							if (item_grid != NULL)
							{
								ret = true;
								break;
						}
						}
						else
						{
							break;
						}
					}
				}
			}
		}
		break;
	case B_BELONG_HOLY_EQUIP_BAG:
		{
		if (I_TYPE_HOLY_EQUIP == item_ib->GetItemType())
			{
				for (int num = 0; num < item_wrapper.num; ++num)
				{
					if (item_wrapper.has_param > 0)
					{
						ret = m_module_mgr->GetHolyEquipManager()->AddNewHolyItem(&item_wrapper, reason_type, NULL, HOLY_EQUIP_SEND_REASON_IN);
						if (!ret) break;
					}
					else
					{
						const ItemBase * item_base = ITEMPOOL->GetItem(item_wrapper.item_id);
						if (item_base != NULL)
						{
							const ItemGridData * item_grid = m_module_mgr->GetHolyEquipManager()->AddHolyItem(item_wrapper.item_id, reason_type, NULL, HOLY_EQUIP_SEND_REASON_IN);
							if (item_grid != NULL)
							{
								ret = true;
								break;
							}
						}
						else
						{
							break;
						}
					}
				}
			}
		}
		break;
	case B_BELONG_ZAO_HUA_BAG:
		{
			if (I_TYPE_ZAO_HUA_EQUIP == item_ib->GetItemType())
			{
				for (int num = 0; num < item_wrapper.num; ++num)
				{
					if (item_wrapper.has_param > 0)
					{
						bool ret = m_module_mgr->GetZaoHuaEquipManager()->AddNewZaoHuaItem(&item_wrapper);
						if (!ret) break;
					}
					else
					{
						const ItemBase * item_base = ITEMPOOL->GetItem(item_wrapper.item_id);
						if (item_base != NULL)
						{
							const ItemGridData* ret = m_module_mgr->GetZaoHuaEquipManager()->AddZaoHuaItem(item_wrapper.item_id);
							if (ret == NULL) break;
						}
						else
						{
							break;
						}
					}
				}
			}
		}
		break;
	case B_BELONG_PET_BAG:
		if (I_TYPE_PET == item_ib->GetItemType())
		{
			// 避免一次发放多个宠物且宠物的唯一ID相同
			long long unique_id = item_wrapper.item_unique_id;
			if (item_wrapper.num > 1)
			{
				unique_id = 0;
			}

			for (int num = 0; num < item_wrapper.num; ++num)
			{
				NetValueItemParam * param = (NetValueItemParam *)&item_wrapper.param_data;
				if (item_wrapper.has_param > 0)
				{
					ret = m_module_mgr->GetPet()->AcquirePetWithParam(param, __FUNCTION__, item_wrapper.is_bind > 0, false, unique_id);
					if (!ret)
					{
						break;
					}
				}
				else
				{
					ret = m_module_mgr->GetPet()->AcquirePet2(item_wrapper.item_id, __FUNCTION__, reason_type, item_wrapper.is_bind > 0, param->pet_param.str_level, false, unique_id);
					if (!ret) break;
				}
		}
		}
		else if (I_TYPE_SPECIAL_PET == item_ib->GetItemType())
		{
			// 避免一次发放多个宠物且宠物的唯一ID相同
			long long unique_id = item_wrapper.item_unique_id;
			if (item_wrapper.num > 1)
			{
				unique_id = 0;
			}

			for (int num = 0; num < item_wrapper.num; ++num)
			{
				m_module_mgr->GetPet()->AcquireSpecialPet(item_ib, g_GetPutReasonTypeStr(reason_type), (0 != item_wrapper.is_bind), false, unique_id);
			}
		}

		break;
	case B_BELONG_PET_GOD_PRINT:
		{
			if (I_TYPE_GOD_PRINT == item_ib->GetItemType())
			{
				for (int num = 0; num < item_wrapper.num; ++num)
				{
					if (item_wrapper.has_param > 0)
					{
						const GodPrintItem* ret = m_module_mgr->GetPet()->AddPetGodPrintByUse(&item_wrapper);
						if (ret == NULL) break;
					}
					else
					{
						const ItemBase * item_base = ITEMPOOL->GetItem(item_wrapper.item_id);
						if (item_base != NULL)
						{
							const GodPrintEquipment * god_print = (const GodPrintEquipment *)item_base;
							int param1 = god_print->GetParam1();
							int param2 = god_print->GetParam2();
							int param3 = god_print->GetParam3();
							const GodPrintItem* ret = m_module_mgr->GetPet()->AddPetGodPrintItem(item_wrapper.item_id, param2, param3, param1, item_wrapper.item_unique_id, reason_type);
							if (ret == NULL) break;
						}
						else
						{
							break;
						}
					}
				}
			}
		}
		break;
	case B_BELONG_PARTNER_SPIRIT_PACT:
		{
			if (I_TYPE_PARTNER_PACT == item_ib->GetItemType())
			{
				ret = m_module_mgr->GetPartner()
					->PactGridPutIn(item_wrapper.item_id, item_wrapper.num, reason_type);
			}
		}
		break;
	case B_BELONG_HUAN_SHOU_BAG:
		{	
			//目前只能放幻兽技能书
			if (item_ib->GetItemType() != I_TYPE_HUAN_SHOU_SKILL)
			{
				break;
			}
			//一次最多放入一个格子的道具
			int item_num = item_wrapper.num;
			int times = 0;
			static ItemDataWrapper temp_item_wrapper;
			temp_item_wrapper = item_wrapper;
			while (item_num > 0 && times < 100)
			{
				temp_item_wrapper.num = GetMin(item_num, static_cast<int>(item_ib->GetPileLimit()));
				item_num -= temp_item_wrapper.num;
				++times;

				temp_item_wrapper.item_unique_id = 0LL;
				UniqueIDGenerator::Instance()->GenUniqueID(&temp_item_wrapper.item_unique_id);

				ret = m_module_mgr->GetRoleHuanShou()->Put(temp_item_wrapper, reason_type);
				if (!ret)
				{
					break;
				}
			}
		}
		break;
	default:
		break;
	}

	return ret;
}

bool Knapsack::PutHelper(const ItemDataWrapper &item_wrapper, int reason_type, int* out_column /*= NULL*/, int *out_index /*= NULL*/, UInt16 drop_monster_id /*= 0*/, bool is_notice /*= true */)
{
	PUT_HELPER(item_wrapper, drop_monster_id);

	return true;
}

bool Knapsack::HasItem(ItemID item_id, int num, bool is_check_temporary)
{
	if (ItemBase::INVALID_ITEM_ID == item_id) return false;

	const ItemBase* item_base = ITEMPOOL->GetItem(item_id);
	if (NULL == item_base) return false;

	int column = item_base->GetItemColumn();
	int count = 0;

	for (int i = 0; i < m_valid_grid_index_end_list[column]; ++i)
	{
		if (NULL != m_grid_list[column][i] && m_grid_list[column][i]->item_id == item_id)
		{
			count += m_grid_list[column][i]->num;

			if (count >= num)
			{
				return true;
			}
		}
	}
	if (is_check_temporary)
	{
		for (int i = 0; i < m_valid_grid_index_end_list[ITEM_COLUMN_TYPE_TEMPORARY]; ++i)
		{
			if (NULL != m_grid_list[ITEM_COLUMN_TYPE_TEMPORARY][i] && m_grid_list[ITEM_COLUMN_TYPE_TEMPORARY][i]->item_id == item_id)
			{
				count += m_grid_list[ITEM_COLUMN_TYPE_TEMPORARY][i]->num;

				if (count >= num)
				{
					return true;
				}
			}
		}
	}

	return false;
}

bool Knapsack::HasItemByMap(const std::map<int, int> & check_item_map, bool is_check_temporary)
{
	for (std::map<int, int>::const_iterator iter = check_item_map.begin(); iter != check_item_map.end(); ++iter)
	{
		int item_id = iter ->first;
		int num = iter->second;

		if (item_id <= 0 || num == 0)
		{
			continue;
		}

		if (!this->HasItem(item_id, num, is_check_temporary))
		{
			return false;
		}
	}

	return true;
}

bool Knapsack::ConsumeItemByMap(const std::map<int, int> & check_item_map, const char * reason)
{
	if (!this->HasItemByMap(check_item_map))
	{
		return false;
	}

	for (std::map<int, int>::const_iterator iter = check_item_map.begin(); iter != check_item_map.end(); ++iter)
	{
		int item_id = iter->first;
		int num = iter->second;

		if (item_id <= 0 || num == 0)
		{
			continue;
		}

		this->ConsumeItem(item_id, num, reason);
	}

	return true;
}

bool Knapsack::ConsumeItem(ItemID item_id, int consume_num, const char *reason, int consume_type, bool notify_lack)
{
	if (consume_type < 0 || consume_type >= KNAPSACK_CONSUME_TYPE_MAX) return false;
	if (consume_num < 0 || item_id == ItemBase::INVALID_ITEM_ID) return false;
	if (0 == consume_num) return true;

	const ItemBase *itembase = ITEMPOOL->GetItem(item_id);
	if (NULL == itembase) return false;

	if (itembase->GetItemType() >= I_TYPE_NET_VALUE_ITEM_BEGIN && consume_num > 1)
	{
		m_module_mgr->GetRole()->NoticeNum(errornum::EN_KNAPSACK_USE_ITEM_NUM_NOT_ENOUGH);
		return false;
	}

	static int index_list[KNAPSACK_MAX_GRID_NUM] = { 0 };
	static int column_list[KNAPSACK_MAX_GRID_NUM] = { 0 };

	int index_num = 0;
	int cur_own_count = 0;

	int column = itembase->GetItemColumn();
	int valid_grid_index_end = m_valid_grid_index_end_list[column];
	for (int i = 0; i < valid_grid_index_end; ++i)
	{
		if (NULL == m_grid_list[column][i] || m_grid_list[column][i]->Invalid()) continue;

		if (m_grid_list[column][i]->item_id == item_id)
		{
			cur_own_count += m_grid_list[column][i]->num;
			index_list[index_num] = i;
			column_list[index_num] = column;
			++index_num;

			if (cur_own_count >= consume_num) break;
		}
	}

	int real_need_consume_num = consume_num;
	int need_buy_num = 0; // 需要用元宝购买的数量
	int need_gold = 0; // 需要消耗的元宝
	if (cur_own_count < real_need_consume_num) // 背包物品不足以消耗
	{
		if (notify_lack)
		{
			Protocol::SCLackItem li;
			li.item_id = item_id;
			li.item_count = real_need_consume_num - cur_own_count;
			EngineAdapter::Instance().NetSend(m_module_mgr->GetRole()->GetNetId(), (const char*)&li, sizeof(li));
		}

		return false;
	}

	const char *item_param_string = NULL;

	int total_num = real_need_consume_num;
	for (int i = 0; i < index_num; ++i)
	{
		int index = index_list[i];

		if (NULL == m_grid_list[column][index]) continue;

		ItemGridData* item_grid_data = m_grid_list[column][index];

		int cur_consume_num = (total_num <= item_grid_data->num) ? total_num : item_grid_data->num;
		total_num -= cur_consume_num;

		if (item_grid_data->gold_price > 0)
		{
			int consume_gold_price = ItemExtern::GetGoldPriceByNum(item_grid_data->item_id, item_grid_data->num, item_grid_data->gold_price, cur_consume_num);
			if (consume_gold_price > 0)
			{
				if (consume_gold_price > item_grid_data->gold_price) consume_gold_price = item_grid_data->gold_price;

				item_grid_data->gold_price -= consume_gold_price;
			}
		}

		if (NULL != m_grid_list[column][index]->param)
		{
			item_param_string = m_grid_list[column][index]->param->GetParamLog(itembase->GetItemType());
		}

		item_grid_data->num -= cur_consume_num;


		if (item_grid_data->num <= 0)
		{
#ifdef TRACK_ITEM_FLAG
			gamelog::g_log_item_track.printf(LL_INFO, "TRACK_ITEM_FLAG Knapsack::ConsumeItem CLEAR_GRID role[%d,%s] item[%d] unique_id[%lld]",
				m_module_mgr->GetRole()->GetUID(), m_module_mgr->GetRole()->GetName(), item_grid_data->item_id, item_grid_data->item_unique_id);
#endif
			this->ClearGrid(column, index);
			item_grid_data = NULL;
		}

		this->SetItemDirty(column, index, true, ITEM_CHANGE_NOTICE_REASON_CONSUME);

		if (total_num <= 0) break;
	}

	if (itembase->IsRecord())
	{
		gamelog::g_log_knapsack.printf(LL_INFO, "[Knapsack::ConsumeItem Succ][user[%s %d] item_name:%s item_id:%d real_consume_num:%d gold_buy_num:%d consume_gold:%d reason:%s]",
			m_module_mgr->GetRole()->GetName(), UidToInt(m_module_mgr->GetRole()->GetUserId()), itembase->GetItemName(), (int)item_id, real_need_consume_num, need_buy_num, need_gold, NULL == reason ? "" : reason);

		ROLE_LOG_QUICK6(LOG_TYPE_ITEM_CONSUME, m_module_mgr->GetRole(), item_id, real_need_consume_num, reason, item_param_string);
	}

	if (index_num >= 1) this->RefreshValidGridIndexEnd(column_list[index_num - 1], index_list[index_num - 1]);

	EventHandler::Instance().OnConsumeItem(m_module_mgr->GetRole(), item_id, consume_num);

	return true;
}

bool Knapsack::ConsumeItemByIndex(int column, int index, int num, const char *reason, bool is_real_consume, bool need_audit_log)
{
	if (num < 0) return false;
	if (column < ITEM_COLUMN_TYPE_MIN || column >= ITEM_COLUMN_TYPE_MAX) return false;
	if (index < 0 || index >= ITEM_COLUMN_GRID_NUM_MAX) return false;
	if (NULL == m_grid_list[column][index] || m_grid_list[column][index]->Invalid()) return false;
	if (0 == num) return true;

	ItemGridData* item_grid_data = m_grid_list[column][index];


	if (item_grid_data->num < num)
	{
		return false;
	}

	ItemID item_id = item_grid_data->item_id;
	const ItemBase *itembase = ITEMPOOL->GetItem(item_id);
	if (NULL == itembase)
	{
		return false;
	}

	if (!is_real_consume) return true;

	if (item_grid_data->gold_price > 0)
	{
		int consume_gold_price = ItemExtern::GetGoldPriceByNum(item_id, item_grid_data->num, item_grid_data->gold_price, num);
		if (consume_gold_price > 0)
		{
			if (consume_gold_price > item_grid_data->gold_price) consume_gold_price = item_grid_data->gold_price;

			item_grid_data->gold_price -= consume_gold_price;
		}
	}

	const char *item_param_string = NULL;
	if (NULL != m_grid_list[column][index]->param)
	{
		item_param_string = m_grid_list[column][index]->param->GetParamLog(itembase->GetItemType());
	}

	item_grid_data->num -= num;
	int remain_num = item_grid_data->num;
	bool is_bind = item_grid_data->is_bind;

	if (item_grid_data->num <= 0)
	{
#ifdef TRACK_ITEM_FLAG
		gamelog::g_log_item_track.printf(LL_INFO, "TRACK_ITEM_FLAG Knapsack::ConsumeItemByIndex consume_all CLEAR_GRID role[%d,%s] item[%d] unique_id[%lld]",
			m_module_mgr->GetRole()->GetUID(), m_module_mgr->GetRole()->GetName(), m_grid_list[column][index]->item_id, m_grid_list[column][index]->item_unique_id);
#endif
		this->ClearGrid(column, index);
		item_grid_data = NULL;
		this->RefreshValidGridIndexEnd(column, index);
	}

	this->SetItemDirty(column, index, true, ITEM_CHANGE_NOTICE_REASON_CONSUME);

	EventHandler::Instance().OnConsumeItem(m_module_mgr->GetRole(), item_id, num);

	if (itembase->IsRecord())
	{
		gamelog::g_log_knapsack.printf(LL_INFO, "[Knapsack::ConsumeItemByIndex Succ][user[%s %d] item_id:%d item_name:%s(%d) consume_num:%d remain_num:%d reason:%s]",
			m_module_mgr->GetRole()->GetName(), UidToInt(m_module_mgr->GetRole()->GetUserId()), (int)item_id, itembase->GetItemName(), (int)is_bind, (int)num, (int)remain_num, NULL == reason ? "" : reason);

		ROLE_LOG_QUICK6(LOG_TYPE_ITEM_CONSUME, m_module_mgr->GetRole(), item_id, num, reason, item_param_string);
	}

	return true;
}

bool Knapsack::ConsumeItemByIndexList(int list_num, const short* column_list, const short *knapsack_index_list, const short *consume_num_list, const char *reason, bool is_real_consume, bool need_audit_log)
{
	if (list_num <= 0 || list_num > KNAPSACK_MAX_GRID_NUM || NULL == knapsack_index_list || NULL == consume_num_list) return false;

	int real_count = 0;
	static short real_column[KNAPSACK_MAX_GRID_NUM];
	static short real_index[KNAPSACK_MAX_GRID_NUM];
	static short real_num[KNAPSACK_MAX_GRID_NUM];
	memset(real_index, 0, sizeof(real_index));
	memset(real_num, 0, sizeof(real_num));
	memset(real_column, 0, sizeof(real_column));

	for (int i = 0; i < list_num; i++)  // 检查重复 将实际不同的index分出来
	{
		if (consume_num_list[i] < 0) return false;
		if (0 == consume_num_list[i]) continue;

		bool found_duplicate = false;
		for (int k = 0; k < real_count; k++)
		{
			if (real_index[k] == knapsack_index_list[i] && real_column[k] == column_list[i])
			{
				real_num[k] += consume_num_list[i];
				found_duplicate = true;
				break;
			}
		}
		if (found_duplicate) continue;

		if (knapsack_index_list[i] < 0 || knapsack_index_list[i] >= KNAPSACK_MAX_GRID_NUM) return false;
		if (real_count >= KNAPSACK_MAX_GRID_NUM) return false;

		real_column[real_count] = column_list[i];
		real_index[real_count] = knapsack_index_list[i];
		real_num[real_count] = consume_num_list[i];

		++real_count;
	}

	for (int i = 0; i < real_count; i++) // 检查消费
	{
		if (real_num[i] <= 0) return false;

		int real_i = real_index[i];
		int real_c = real_column[i];
		if (NULL == m_grid_list[real_c][real_i] || m_grid_list[real_c][real_i]->Invalid()) return false;

		const ItemBase *item = ITEMPOOL->GetItem(m_grid_list[real_c][real_i]->item_id);
		if (NULL == item) return false;

		if (m_grid_list[real_c][real_i]->num < real_num[i]) return false;
	}

	if (!is_real_consume) return true;

	for (int i = 0; i< real_count; i++) // 真正消费
	{
		int real_i = real_index[i];
		int real_c = real_column[i];
		if (NULL == m_grid_list[real_c][real_i] || m_grid_list[real_c][real_i]->Invalid()) continue;

		ItemID item_id = m_grid_list[real_c][real_i]->item_id;

		const ItemBase *itembase = ITEMPOOL->GetItem(item_id);
		if (NULL == itembase) continue; // 上文已经检查过了，这里需要这个指针是为了打log

		if (m_grid_list[real_c][real_i]->gold_price > 0)
		{
			int consume_gold_price = ItemExtern::GetGoldPriceByNum(item_id, m_grid_list[real_c][real_i]->num, m_grid_list[real_c][real_i]->gold_price, real_num[i]);
			if (consume_gold_price > 0)
			{
				if (consume_gold_price > m_grid_list[real_c][real_i]->gold_price) consume_gold_price = m_grid_list[real_c][real_i]->gold_price;

				m_grid_list[real_c][real_i]->gold_price -= consume_gold_price;
			}
		}

		const char *item_param_string = NULL;
		if (NULL != m_grid_list[real_c][real_i]->param)
		{
			item_param_string = m_grid_list[real_c][real_i]->param->GetParamLog(itembase->GetItemType());
		}

		m_grid_list[real_c][real_i]->num -= real_num[i];

		int remain_num = m_grid_list[real_c][real_i]->num;
		bool is_bind = m_grid_list[real_c][real_i]->is_bind;

		if (m_grid_list[real_c][real_i]->num <= 0)
		{
			m_grid_list[real_c][real_i]->Clear();
			this->RefreshValidGridIndexEnd(real_c, real_i);
		}

		this->SetItemDirty(real_c, real_i, true, ITEM_CHANGE_NOTICE_REASON_CONSUME);

		EventHandler::Instance().OnConsumeItem(m_module_mgr->GetRole(), item_id, -real_num[i]);

		if (itembase->IsRecord())
		{
			gamelog::g_log_knapsack.printf(LL_INFO, "[Knapsack::ConsumeItemByIndexList Succ][user[%s %d] item_id:%d item_name:%s(%d) consume_num:%d remain_num:%d reason:%s]",
				m_module_mgr->GetRole()->GetName(), UidToInt(m_module_mgr->GetRole()->GetUserId()), (int)itembase->GetItemId(), itembase->GetItemName(), (int)is_bind, (int)real_num[i], (int)remain_num, NULL == reason ? "" : reason);

			ROLE_LOG_QUICK6(LOG_TYPE_ITEM_CONSUME, m_module_mgr->GetRole(), itembase->GetItemId(), real_num[i], reason, item_param_string);
		}
	}

	return true;
}

bool Knapsack::ConsumeKnapsackItem(ItemID item_id, int consume_num, const char * reason, int consume_type, bool notify_lack)
{
	if (consume_type < 0 || consume_type >= KNAPSACK_CONSUME_TYPE_MAX) return false;
	if (consume_num < 0 || item_id == ItemBase::INVALID_ITEM_ID) return false;
	if (0 == consume_num) return true;

	const ItemBase *itembase = ITEMPOOL->GetItem(item_id);
	if (NULL == itembase) return false;

	if (itembase->GetItemType() >= I_TYPE_NET_VALUE_ITEM_BEGIN && consume_num > 1)
	{
		m_module_mgr->GetRole()->NoticeNum(errornum::EN_KNAPSACK_USE_ITEM_NUM_NOT_ENOUGH);
		return false;
	}

	//static int index_list[KNAPSACK_MAX_GRID_NUM] = { 0 };
	//static int column_list[KNAPSACK_MAX_GRID_NUM] = { 0 };
	static int consume_list[ITEM_COLUMN_TYPE_MAX][ITEM_COLUMN_GRID_NUM_MAX];
	memset(consume_list, 0, sizeof(consume_list));

	int index_num_list[ITEM_COLUMN_TYPE_MAX] = { 0 };

	//int index_num = 0;
	int cur_own_count = 0;

	int column = itembase->GetItemColumn();
	for (int k = 0; k < ITEM_COLUMN_TYPE_MAX; k++)
	{
		if (k != column && k != ITEM_COLUMN_TYPE_STORAGE && k != ITEM_COLUMN_TYPE_TEMPORARY) continue;
		if (cur_own_count >= consume_num) break;

		int valid_grid_index_end = m_valid_grid_index_end_list[k];
		for (int i = 0; i < valid_grid_index_end; ++i)
		{
			if (NULL == m_grid_list[k][i] || m_grid_list[k][i]->Invalid()) continue;

			if (m_grid_list[k][i]->item_id == item_id)
			{
				cur_own_count += m_grid_list[k][i]->num;
				//index_list[index_num] = i;
				//column_list[index_num] = k;
				consume_list[k][index_num_list[k]] = i;
				//++index_num;
				++index_num_list[k];

				if (cur_own_count >= consume_num) break;
			}
		}
	}

	int real_need_consume_num = consume_num;
	int need_buy_num = 0; // 需要用元宝购买的数量
	int need_gold = 0; // 需要消耗的元宝
	if (cur_own_count < real_need_consume_num) // 背包物品不足以消耗
	{
		if (notify_lack)
		{
			Protocol::SCLackItem li;
			li.item_id = item_id;
			li.item_count = real_need_consume_num - cur_own_count;
			EngineAdapter::Instance().NetSend(m_module_mgr->GetRole()->GetNetId(), (const char*)&li, sizeof(li));
		}

		return false;
	}

	const char *item_param_string = NULL;

	int total_num = real_need_consume_num;
	for (int i = 0; i < ITEM_COLUMN_TYPE_MAX; i++)
	{
		if (i != column && i != ITEM_COLUMN_TYPE_STORAGE && i != ITEM_COLUMN_TYPE_TEMPORARY) continue;

		for (int k = 0; k < index_num_list[i]; k++)
		{
			int index = consume_list[i][k];

			if (NULL == m_grid_list[i][index]) continue;

			ItemGridData* item_grid_data = m_grid_list[i][index];

			int cur_consume_num = (total_num <= item_grid_data->num) ? total_num : item_grid_data->num;
			total_num -= cur_consume_num;

			if (item_grid_data->gold_price > 0)
			{
				int consume_gold_price = ItemExtern::GetGoldPriceByNum(item_grid_data->item_id, item_grid_data->num, item_grid_data->gold_price, cur_consume_num);
				if (consume_gold_price > 0)
				{
					if (consume_gold_price > item_grid_data->gold_price) consume_gold_price = item_grid_data->gold_price;

					item_grid_data->gold_price -= consume_gold_price;
				}
			}

			if (NULL != m_grid_list[i][index]->param)
			{
				item_param_string = m_grid_list[i][index]->param->GetParamLog(itembase->GetItemType());
			}

			item_grid_data->num -= cur_consume_num;
			if (item_grid_data->num <= 0)
			{
#ifdef TRACK_ITEM_FLAG
				gamelog::g_log_item_track.printf(LL_INFO, "TRACK_ITEM_FLAG Knapsack::ConsumeItem CLEAR_GRID role[%d,%s] item[%d] unique_id[%lld]",
					m_module_mgr->GetRole()->GetUID(), m_module_mgr->GetRole()->GetName(), item_grid_data->item_id, item_grid_data->item_unique_id);
#endif
				this->ClearGrid(i, index);
				item_grid_data = NULL;
			}

			this->SetItemDirty(i, index, true, ITEM_CHANGE_NOTICE_REASON_CONSUME);

			if (total_num <= 0) break;
		}
	}

	if (itembase->IsRecord())
	{
		gamelog::g_log_knapsack.printf(LL_INFO, "[Knapsack::ConsumeItem Succ][user[%s %d] item_name:%s item_id:%d real_consume_num:%d gold_buy_num:%d consume_gold:%d reason:%s]",
			m_module_mgr->GetRole()->GetName(), UidToInt(m_module_mgr->GetRole()->GetUserId()), itembase->GetItemName(), (int)item_id, real_need_consume_num, need_buy_num, need_gold, NULL == reason ? "" : reason);

		ROLE_LOG_QUICK6(LOG_TYPE_ITEM_CONSUME, m_module_mgr->GetRole(), item_id, real_need_consume_num, reason, item_param_string);
	}
	for (int i = 0; i < ITEM_COLUMN_TYPE_MAX; i++)
	{
		if (index_num_list[i] >= 1)
		{
			this->RefreshValidGridIndexEnd(i, consume_list[i][index_num_list[i] - 1]);
		}
	}

	EventHandler::Instance().OnConsumeItem(m_module_mgr->GetRole(), item_id, consume_num);

	return true;
}

void Knapsack::SendKnapsackInfoToRole(short reason /* = 0 */)
{
	static Protocol::SCKnapsackInfoAck kia;
	kia.item_count = 0;
	kia.reason = reason;

	for (int column = ITEM_COLUMN_TYPE_MIN; column < ITEM_COLUMN_TYPE_MAX; ++column)
	{
		kia.valid_grid_num_list[column] = m_curr_valid_grid_num_list[column];
		kia.extend_times_list[column] = m_extend_column_times_list[column];

		for (int i = 0; i < ITEM_COLUMN_GRID_NUM_MAX; ++i)
		{
			if (NULL == m_grid_list[column][i] || m_grid_list[column][i]->Invalid()) continue;

			const ItemGridData * item_grid_data = m_grid_list[column][i];
			const ItemBase * item_base = ITEMPOOL->GetItem(item_grid_data->item_id);
			if (NULL == item_base) continue;

			InfoType & ptc_con = kia.item_list[kia.item_count++];
			ptc_con.item_id = item_grid_data->item_id;
			ptc_con.is_bind = (char)item_grid_data->is_bind;
			ptc_con.column = column;
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
	}

	EngineAdapter::Instance().NetSendSerializeMsg(m_module_mgr->GetRole()->GetNetId(), &kia);
}

void Knapsack::OnCheckNorexConvertItem(ItemConfigData & item_data)
{
	const ItemBase * item_base = ITEMPOOL->GetItem(item_data.item_id);
	if (NULL == item_base || I_TYPE_EXPENSE != item_base->GetItemType()) return;

	const NorexItem * norex_item = (const NorexItem *)item_base;
	if (NorexItem::I_NOREX_ITEM_CONVERT_ITEM != norex_item->GetUseType()) return;

	item_data.item_id = norex_item->GetParam1();
	item_data.num *= norex_item->GetParam2();
	item_data.is_bind = true;
}

bool Knapsack::OnRoleAttrPelletConvert(ItemConfigData item_data, std::vector<ItemConfigData> & convert_item_list)
{
	const std::set<int> * role_pellet_rand_item_set = LOGIC_CONFIG->GetRoleAttrPelletConfig()->GetRolePelletItemCfg(item_data.item_id);
	if (NULL == role_pellet_rand_item_set || item_data.num <= 0)
	{
		return false;
	}
	const RoleAttrPelletOtherCfg & pellet_other_cfg = LOGIC_CONFIG->GetRoleAttrPelletConfig()->GetOtherCfg();
	std::map<int, int> need_count_map;
	std::map<int, int> rand_item_map;
	std::vector<int> valid_item_vec;
	int need_count = 0;
	short role_use_pellet_count[MAX_ATTR_PELLET_TYPE_NUM] = { 0 };
	m_module_mgr->GetRoleAttrPellet()->GetRolePelletUseList(MAX_ATTR_PELLET_TYPE_NUM, role_use_pellet_count);
	for (std::set<int>::const_iterator it = role_pellet_rand_item_set->begin(); it != role_pellet_rand_item_set->end(); ++it)
	{
		const RoleAttrPelletAttrListCfg * pellet_cfg = LOGIC_CONFIG->GetRoleAttrPelletConfig()->GetPelletListCfg(*it, m_module_mgr->GetRole()->GetAppearanceProf());
		if (NULL == pellet_cfg) continue;

		int attr_pellet_type = GetPelletTypeByAttrType(pellet_cfg->attr_type);
		if (attr_pellet_type < 0 || attr_pellet_type >= ATTR_PELLET_TYPE_MAX) continue;

		valid_item_vec.push_back(*it);
		int has_count = this->Count(*it, true, true);
		if (role_use_pellet_count[attr_pellet_type] + has_count >= pellet_other_cfg.use_limit) continue;

		need_count_map[*it] = pellet_other_cfg.use_limit - has_count - role_use_pellet_count[attr_pellet_type];
		need_count += need_count_map[*it];
	}
	if (valid_item_vec.empty()) return false;

	//超出部分正常随机,剩余直接补齐
	if (item_data.num >= need_count)
	{
		int remain_count = item_data.num - need_count;
		for (int i = 0; i < remain_count; i++)
		{
			int rand_index = rand() % (int)valid_item_vec.size();
			rand_item_map[valid_item_vec[rand_index]]++;
		}
		for (std::map<int, int>::iterator it = need_count_map.begin(); it != need_count_map.end(); it++)
		{
			rand_item_map[it->first] += it->second;
		}
	}
	else
	{
		//还未凑齐
		std::vector<ItemConfigData> rand_vec;
		for (std::map<int, int>::iterator it = need_count_map.begin(); it != need_count_map.end(); it++)
		{
			rand_vec.push_back(ItemConfigData(it->first, true, it->second));
		}
		for (int i = 0; i < item_data.num && i < need_count && !rand_vec.empty(); i++)
		{
			int rand_index = rand() % rand_vec.size();
			rand_item_map[rand_vec[rand_index].item_id]++;
			if (--rand_vec[rand_index].num <= 0)
			{
				rand_vec.erase(rand_vec.begin() + rand_index);
			}
		}
	}

	for (std::map<int, int>::iterator it = rand_item_map.begin(); it != rand_item_map.end(); it++)
	{
		convert_item_list.push_back(ItemConfigData(it->first, true, it->second));
	}
	if (convert_item_list.empty()) return false;

	return true;
}

bool Knapsack::GetItemGridData(int column, int index, ItemGridData *item_data)
{
	if (column < ITEM_COLUMN_TYPE_MIN || column >= ITEM_COLUMN_TYPE_MAX) return false;
	if (NULL == item_data || index < 0 || index >= ITEM_COLUMN_GRID_NUM_MAX) return false;

	if (NULL == m_grid_list[column][index] || m_grid_list[column][index]->Invalid()) return false;

	const ItemBase *itembase = ITEMPOOL->GetItem(m_grid_list[column][index]->item_id);
	if (NULL == itembase) return false;

	if (m_grid_list[column][index]->num <= 0 || m_grid_list[column][index]->num > itembase->GetPileLimit()) return false;

	item_data->Set(*m_grid_list[column][index]);

	return true;
}

const ItemGridData* Knapsack::GetItemGridData(int column, int index) const
{
	if (column < ITEM_COLUMN_TYPE_MIN || column >= ITEM_COLUMN_TYPE_MAX) return NULL;
	if (index < 0 || index >= ITEM_COLUMN_GRID_NUM_MAX) return NULL;

	if (NULL == m_grid_list[column][index] || m_grid_list[column][index]->Invalid()) return NULL;

	const ItemBase * itembase = ITEMPOOL->GetItem(m_grid_list[column][index]->item_id);
	if (NULL == itembase) return NULL;

	if (m_grid_list[column][index]->num <= 0 || m_grid_list[column][index]->num > itembase->GetPileLimit()) return NULL;

	return m_grid_list[column][index];
}

ItemGridData * Knapsack::GetItemGridData2(int column, int index)
{
	if (column < ITEM_COLUMN_TYPE_MIN || column >= ITEM_COLUMN_TYPE_MAX) return NULL;
	if (index < 0 || index >= ITEM_COLUMN_GRID_NUM_MAX) return NULL;

	if (NULL == m_grid_list[column][index] || m_grid_list[column][index]->Invalid()) return NULL;

	const ItemBase * itembase = ITEMPOOL->GetItem(m_grid_list[column][index]->item_id);
	if (NULL == itembase) return NULL;

	if (m_grid_list[column][index]->num <= 0 || m_grid_list[column][index]->num > itembase->GetPileLimit()) return NULL;

	return m_grid_list[column][index];
}

bool Knapsack::GetItemGridWrapData(int column, int index, ItemDataWrapper* out_wrapper)
{
	if (NULL == out_wrapper) return false;
	if (column < ITEM_COLUMN_TYPE_MIN || column >= ITEM_COLUMN_TYPE_MAX) return false;
	if (index < 0 || index >= ITEM_COLUMN_GRID_NUM_MAX) return false;

	if (NULL == m_grid_list[column][index] || m_grid_list[column][index]->Invalid()) return false;

	const ItemBase *itembase = ITEMPOOL->GetItem(m_grid_list[column][index]->item_id);
	if (NULL == itembase) return false;

	if (m_grid_list[column][index]->num <= 0 || m_grid_list[column][index]->num > itembase->GetPileLimit()) return false;

	return m_grid_list[column][index]->WrapTo(out_wrapper);
}

void Knapsack::SetItemDirty(int column, int index, bool to_notice_client, short reason_type, short put_reason /* = 0 */)
{
	if (column < ITEM_COLUMN_TYPE_MIN || column >= ITEM_COLUMN_TYPE_MAX) return;
	if (index < 0 || index >= ITEM_COLUMN_GRID_NUM_MAX) return;

	m_dirty_mark[column][index] = true;
	m_knapsack_is_change = true;

	if (to_notice_client) this->SendknapsackGridInfoToRole(column, index, reason_type, put_reason);
}

void Knapsack::RefreshValidGridIndexEnd(int column, int change_index)
{
	if (column < ITEM_COLUMN_TYPE_MIN || column >= ITEM_COLUMN_TYPE_MAX) return;
	if (change_index < 0 || change_index >= ITEM_COLUMN_GRID_NUM_MAX) return;

	if (NULL == m_grid_list[column][change_index] || m_grid_list[column][change_index]->Invalid())
	{
		if (m_valid_grid_index_end_list[column] < change_index)
		{
			m_valid_grid_index_end_list[column] = change_index;
		}
	}
	else
	{
		if (m_valid_grid_index_end_list[column] < change_index + 1)
		{
			m_valid_grid_index_end_list[column] = change_index + 1;
		}
	}

	for (int i = m_valid_grid_index_end_list[column] - 1; i >= 0; i--) // 往前搜索直到最后的非空格子
	{
		if (NULL != m_grid_list[column][i] && !m_grid_list[column][i]->Invalid())
		{
			m_valid_grid_index_end_list[column] = i + 1;
			return;
		}
	}

	m_valid_grid_index_end_list[column] = 0;
}

void Knapsack::GridSetBind(int column, int index, bool need_set_dirty)
{
	if (index < 0 || index >= ITEM_COLUMN_GRID_NUM_MAX) return;

	if (NULL != m_grid_list[column][index] && !m_grid_list[column][index]->Invalid() && !m_grid_list[column][index]->is_bind)
	{
		m_grid_list[column][index]->is_bind = true;

		if (need_set_dirty) this->SetItemDirty(column, index, true, ITEM_CHANGE_NOTICE_REASON_DATA_CHANGE);
	}
}

void Knapsack::GridClearInvalidTime(int column, int index, bool need_set_dirty)
{
	if (index < 0 || index >= ITEM_COLUMN_GRID_NUM_MAX) return;

	if (NULL != m_grid_list[column][index] && !m_grid_list[column][index]->Invalid() && m_grid_list[column][index]->invalid_time > 0)
	{
		m_grid_list[column][index]->invalid_time = 0;

		if (need_set_dirty) this->SetItemDirty(column, index, true, ITEM_CHANGE_NOTICE_REASON_DATA_CHANGE);
	}
}

void Knapsack::GetItemList(int *count, ItemListParam::ItemListData *save_list)
{
	int tmp_count = 0;

	for (int column = ITEM_COLUMN_TYPE_MIN; column < ITEM_COLUMN_TYPE_MAX; ++column)
	{
		for (int i = 0; i < ITEM_COLUMN_GRID_NUM_MAX && tmp_count < *count; ++i)
		{
			if (NULL != m_grid_list[column][i] && !m_grid_list[column][i]->Invalid())
			{
				save_list[tmp_count].change_state_item = structcommon::CS_NONE;
				save_list[tmp_count].index = GetIndexByRealIndexColumn(column, i);
				m_grid_list[column][i]->WrapTo(&save_list[tmp_count].item_wrapper);
				++tmp_count;
			}
		}
	}

	*count = tmp_count;
}

void Knapsack::GetChangeItemList(int *count, ItemListParam::ItemListData *save_list)
{
	if (!m_knapsack_is_change)
	{
		*count = 0;
		return;
	}

	int tmp_count = 0;

	for (int column = ITEM_COLUMN_TYPE_MIN; column < ITEM_COLUMN_TYPE_MAX; ++column)
	{
		for (int i = 0; i < ITEM_COLUMN_GRID_NUM_MAX && tmp_count < *count; ++i)
		{
			if (m_dirty_mark[column][i])
			{
				if (NULL == m_grid_list[column][i] || m_grid_list[column][i]->Invalid())
				{
					if (!m_old_state[column][i]) continue;

					save_list[tmp_count].change_state_item = structcommon::CS_DELETE;

					save_list[tmp_count].index = GetIndexByRealIndexColumn(column, i);
					save_list[tmp_count].item_wrapper.Reset();
					++tmp_count;

					gamelog::g_log_knapsack.printf(LL_INFO, "Knapsack::GetChangeItemList CS_DELETE role[%d,%s] item from column[%d] index[%d]",
						m_module_mgr->GetRole()->GetUID(), m_module_mgr->GetRole()->GetName(), column, i);
				}
				else
				{
					if (m_old_state[column][i])
					{
						save_list[tmp_count].change_state_item = structcommon::CS_UPDATE;

						gamelog::g_log_knapsack.printf(LL_INFO, "Knapsack::GetChangeItemList CS_UPDATE role[%d,%s] item_id[%d] num[%d] from column[%d] index[%d]",
							m_module_mgr->GetRole()->GetUID(), m_module_mgr->GetRole()->GetName(), m_grid_list[column][i]->item_id, m_grid_list[column][i]->num, column, i);
					}
					else
					{
						save_list[tmp_count].change_state_item = structcommon::CS_INSERT;

						gamelog::g_log_knapsack.printf(LL_INFO, "Knapsack::GetChangeItemList CS_INSERT role[%d,%s] item_id[%d] num[%d] from column[%d] index[%d]",
							m_module_mgr->GetRole()->GetUID(), m_module_mgr->GetRole()->GetName(), m_grid_list[column][i]->item_id, m_grid_list[column][i]->num, column, i);
					}

					save_list[tmp_count].index = GetIndexByRealIndexColumn(column, i);
					m_grid_list[column][i]->WrapTo(&save_list[tmp_count].item_wrapper);
					++tmp_count;
				}
			}
		}
	}

	*count = tmp_count;
}

void Knapsack::ClearDirtyMark()
{
	if (!m_knapsack_is_change) return;

	for (int column = ITEM_COLUMN_TYPE_MIN; column < ITEM_COLUMN_TYPE_MAX; ++column)
	{
		for (int i = 0; i < ITEM_COLUMN_GRID_NUM_MAX; ++i)
		{
			if (m_dirty_mark[column][i])
			{
				if (NULL == m_grid_list[column][i] || m_grid_list[column][i]->Invalid())
				{
					m_old_state[column][i] = false;
				}
				else
				{
					m_old_state[column][i] = true;
				}
			}
		}
	}

	memset(m_dirty_mark, 0, sizeof(m_dirty_mark));

	m_knapsack_is_change = false;
}

void Knapsack::SendknapsackGridInfoToRole(int column, int index, short reason_type, short put_reason /* = 0 */)
{
	if (column < ITEM_COLUMN_TYPE_MIN || column >= ITEM_COLUMN_TYPE_MAX) return;
	if (index < 0 || index >= ITEM_COLUMN_GRID_NUM_MAX) return;

	static Protocol::SCKnapsackItemChange kic;
	kic.reason = reason_type;
	kic.put_reason = put_reason;
	kic.column = static_cast<short>(column);
	kic.index = static_cast<short>(index);
	if (NULL == m_grid_list[column][index] || m_grid_list[column][index]->Invalid())
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
		kic.item_id = m_grid_list[column][index]->item_id;
		kic.num = m_grid_list[column][index]->num;
		kic.is_bind = m_grid_list[column][index]->is_bind ? 1 : 0;
		kic.has_param = (NULL != m_grid_list[column][index]->param);
		kic.invalid_time = m_grid_list[column][index]->invalid_time;
		kic.item_unique_id = m_grid_list[column][index]->item_unique_id;
		kic.is_market_buy_flag = m_grid_list[column][index]->is_market_buy_flag;
		kic.reserve_ch = 0;

		const ItemBase* item_base = ITEMPOOL->GetItem(kic.item_id);
		if (NULL == item_base)
		{
			return;
		}
		kic.param_length = m_grid_list[column][index]->param->GetDataByItemType(item_base->GetItemType(), kic.param_data);
	}

	EngineAdapter::Instance().NetSendSerializeMsg(m_module_mgr->GetRole()->GetNetId(), &kic);
}

struct KnapsackPutReasonItem
{
	int reason;
	const char *str;
};

const static KnapsackPutReasonItem gs_put_reason_str_list[] =
{
	{ PUT_REASON_INVALID, "PUT_REASON_INVALID" },
	{ PUT_REASON_NO_NOTICE, "other" },
	{ PUT_PET_CONVERSION_EXP, "put_pet_conversion_exp" },
	{ PUT_PET_REBIRTH, "put_pet_rebirth" },
	{ PUT_REASON_GM, "put_reason_gm" },
	{ PUT_REASON_DEPOSIT, "put_reason_deposit" },
	{ PUT_REASON_WITHDRAW_STORAGE, "put_reason_withdraw_storage" },
	{ PUT_REASON_WITHDRAW_TEMPORARY, "put_reason_withdraw_temporary" },
	{ PUT_REASON_WITHDRAW_ALL_TEMPORARY, "put_reason_withdraw_all_temporary" },
	{ PUT_REASON_PRESTIGE_BUY, "put_reason_prestige_buy" },
	{ PUT_REASON_COLLECTION_ONCONVERT, "put_reason_collection_onconvert" },
	{ PUT_REASON_GIFT_RAND_ITEM, "put_reason_gift_rand_item" },
	{ PUT_REASON_TASK_REWARD, "put_reason_task_reward" },
	{ PUT_REASON_ACHIEVEMENT_REWARD, "put_reason_achievement_reward" },
	{ PUT_REASON_DAY_REWARD, "put_reason_day_reward" },
	{ PUT_REASON_WORKSHOP_COMPOUND, "put_reason_workshop_compound" },
	{ PUT_REASON_GUILD_EXPLORE, "put_reason_guild_explore" },
	{ PUT_REASON_GUILD_BOSS, "put_reason_guild_boss" },
	{ PUT_REASON_MAIL_RECEIVE, "put_reason_mail_receive" },
	{ PUT_REASON_SYSTEM_NOTICE_REWARD, "put_reason_system_notice_reward" },
	{ PUT_REASON_SYSTEM_COURAGE_CHALLENGE, "put_reason_system_courage_challenge" },
	{ PUT_REASON_RESERVE, "put_reason_reserve" },
	{ PUT_REASON_SYSTEM_MONSTER_WAVE, "put_reason_system_monster_wave" },
	{ PUT_REASON_GATHER_OBJECT, "put_reason_gather_object" },
	{ PUT_REASON_GUILD_SHOP, "put_reason_guild_shop" },
	{ PUT_REASON_WILD_BOSS, "put_reason_wild_boss" },
	{ PUT_REASON_MEDAL, "put_reason_medal" },
	{ PUT_REASON_MEDAL_PUT_ON, "put_reason_medal_put_on" },
	{ PUT_REASON_MI_YI, "put_reason_mi_yi" },
	{ PUT_REASON_SCHOOL_TASK, "put_reason_school_task" },
	{ PUT_REASON_DROP, "put_reason_drop" },
	{ PUT_REASON_ESCORT, "put_reason_escort" },
	{ PUT_REASON_TREASURE_MAP, "put_reason_treasure_map" },
	{ PUT_REASON_TRADE_MARKET_BUY, "put_reason_trade_market_buy" },
	{ PUT_REASON_FIND_REWARD, "put_reason_find_reward" },
	{ PUT_REASON_TASK_CHAIN, "put_reason_task_chain" },
	{ PUT_REASON_FABAO_DECOMPOSE, "put_reason_fabao_decompose" },
	{ PUT_REASON_CRYSTAL_FB, "put_reason_crystal_fb" },
	{ PUT_REASON_SHOP, "put_reason_shop" },
	{ PUT_REASON_XUN_BAO, "put_reason_treasure_hunt" },
	{ PUT_REASON_EXCHANGE, "put_reason_exchange" },
	{ PUT_REASON_BRAVE_GROUND, "put_reason_brave_ground" },
	{ PUT_REASON_RA_TIME_LIMIT_STORE, "put_reason_ra_time_limit_store" },
	{ PUT_REASON_MARRIAGE_TASK, "put_reason_marriage_task" },
	{ PUT_REASON_GUILD_FIGHT, "put_reason_guild_fight" },
	{ PUT_REASON_COLOSSEUM, "put_reason_colosseum" },
	{ PUT_REASON_GUILD_ANSWER, "put_reason_guild_answer" },
	{ PUT_REASON_OPTIONAL_GIFT, "put_reason_optional_gift" },
	{ PUT_REASON_CHALLENGE_FB, "put_reason_challenge_fb" },
	{ PUT_REASON_TRADE_MARKET_OTHER, "put_reason_trade_market_other" },
	{ PUT_REASON_DEF_GIFT, "put_reason_def_gift" },
	{ PUT_REASON_RAND_GIFT, "put_reason_rand_gift" },
	{ PUT_REASON_CHONGZHI_FANLI, "put_reason_chongzhi_fanli" },
	{ PUT_REASON_FUNC_GUIDE, "put_reason_func_guide" },
	{ PUT_REASON_GUIDE_FB, "put_reason_guide_fb" },
	{ PUT_REASON_MAZERACER, "put_reason_mazeracer" },
	{ PUT_REASON_TREVIFOUNTION, "put_reason_trevifountion" },
	{ PUT_REASON_LOW_TREASURE_MAP, "put_reason_low_treasure_map" },
	{ PUT_REASON_NPC_SHOP, "put_reason_npc_shop" },
	{ PUT_REASON_CONVERT_SHOP, "put_reason_convert_shop" },
	{ PUT_REASON_GIFT_OPTIONAL_ITEM, "put_reason_gift_optional_item" },

	{ PUT_REASON_CATCH_PET, "put_reason_catch_pet" },
	{ PUT_REASON_PET_ABANDONMENT_SHARDS, "put_pet_abandonment_shards" },
	{ PUT_REASON_DECOMPOSE_SURFACE, "put_reason_decompose_surface" },
	{ PUT_REASON_RA_TURN_TABLE , "put_reason_ra_turn_table" },
	{ PUT_REASON_RA_DISCOUNT_STORE , "put_reason_ra_discount_store" },
	{ PUT_REASON_CATCH_PET_SHARD , "put_reason_catch_pet_shard" },
	{ PUT_REASON_RA_MYSTERIOUS_TRIAL , "put_reason_ra_mysterial_trial" },
	{ PUT_REASON_GUILD_BOSS_CHAPTER_REWARD , "put_reason_guild_boss_chapter_reward" },
	{ PUT_REASON_RA_CONSUME_CAROUSEL , "put_reason_ra_consume_carousel" },
	{ PUT_REASON_RA_FIRST_CHONGZHI_YIXIANER , "put_reason_ra_first_chongzhi_yixianer" },
	{ PUT_REASON_RA_LOGIN_SYSTEM_REWARD , "put_reason_ra_login_system_reward" },
	{ PUT_REASON_BOSS_CHAPTER_BOSS_REWARD, "put_reason_boss_chapter_boss_reward" },
	{ PUT_REASON_BOSS_CHAPTER_CHAPTER_REWARD, "put_reason_boss_chapter_chapter_reward" },
	{ PUT_REASON_BOUNTY_TASK_TURN_TABLE_REWARD, "put_reason_bounty_task_turn_table_reward" },
	{ PUT_REASON_SERVER_FIRST_KILL , "put_reason_server_first_kill" },
	{ PUT_REASON_TRANSACTION_TASK_REWARD , "put_reason_transaction_task_reward" },
	{ PUT_REASON_TASKCHAIN_DROP_SCENE , "put_reason_task_chain_drop_scene" },
	{ PUT_REASON_ACTIVE_SYSTEM_REWARD , "put_reason_active_system_reward" },
	{ PUT_REASON_FANTASY_FB, "put_reason_fantasy_fb" },
	{ PUT_REASON_WORLD_QUESTION_REWARD , "put_reason_world_question_reward" },
	{ PUT_REASON_RA_HERITAGE_TREASURE , "put_reason_ra_heritage_treasure" },
	{ PUT_REASON_EXPEL_MONSTER_BUY_ITEM , "put_reason_expel_monster_buy_item" },
	{ PUT_REASON_ADVANCE_REWARD , "put_reason_advance_reward" },
	{ PUT_REASON_SALE_SHOP_BUY , "put_reason_sale_shop_buy" },
	{ PUT_REASON_HAPPY_UP_EXCHANGE , "put_reason_happy_up_exchange" },
	{ PUT_REASON_RA_LUCKY_BAG , "put_reason_ra_lucky_bag" },
	{ PUT_REASON_RA_TIAN_JIANG_HAO_LI , "put_reason_ra_tian_jiang_hao_li" },
	{ PUT_REASON_EXPEL_MONSTER , "put_reason_ra_expel_monster" },
	{ PUT_REASON_MEDAL_EXCAHNG_MATERIAL , "put_reason_medal_excahng_material" },
	{ PUT_REASON_JOY_SEEKS_FAVORS , "put_reason_ra_joy_seeks_favors" },
	{ PUT_REASON_JIAN_DAN_QIN_XIN , "put_reason_ra_jian_dan_qin_xin" },
	{ PUT_REASON_ONLY_FIGHT, "put_reason_only_fight" },
	{ PUT_REASON_TEAM_FIGHT , "put_reason_team_fight" },
	{ PUT_REASON_LINKAGE_GIFT , "put_reason_linkage_gift" },
	{ PUT_REASON_RELATED_TO_THE_EVENT , "put_reason_related_to_the_event" },
	{ PUT_REASON_RA_PARTNER_EXCHANGE , "put_reason_randactivity_partner_exchange" },
	{ PUT_REASON_RA_COLLECT_WORD_EXCHANGE , "put_reason_ra_collect_word_exchange" },
	{ PUT_REASON_RA_DISCOUNT_CARNIVAL , "put_reason_ra_discount_carnival" },
	{ PUT_REASON_RA_LUCKY_DISCOUNT , "put_reason_ra_lucky_discount" },
	{ PUT_REASON_RA_PET_EXCHANGE , "put_reason_ra_pet_exchange" },
	{ PUT_REASON_RA_ADVENTURE_GIFT , "put_reason_ra_adventure_gift" },
	{ PUT_REASON_LUCKY_TURNTABLE , "put_reason_lucky_turntable" },
	{ PUT_REASON_SURFACE_SHOP , "put_reason_surface_shop" },
	{ PUT_REASON_RA_QUAN_MIN_DISCOUNT, "put_reason_quan_min_discount" },
	{ PUT_REASON_RAND_GIFT_PET_EGG , "put_reason_rand_gift_pet_egg" },
	{ PUT_REASON_OPTIONAL_GIFT_PET_EGG , "put_reason_optional_gift_pet_egg" },
	{ PUT_REASON_DEF_GIFT_PET_EGG, "put_reason_def_gift_pet_egg" },
	{ PUT_REASON_RA_SPROTS_REWARD, "put_reason_sprost_reward" },
	{ PUT_REASON_PET_EQUIPMENT_TAKEOFF, "put_reason_pet_equipment_takeoff" },
	{ PUT_REASON_PARTNER_ELEMENT_TAKEOFF_DEFAULT, "put_reason_partner_element_takeoff" },
	{ PUT_REASON_DECOMPOSE_MEDAL, "put_reason_decompose_medal" },
	{ PUT_REASON_RA_TU_JIAN_TREASURE, "put_reason_ra_tu_jian_treasure" },
	{ PUT_REASON_PARTNER_ELEMENT_TAKEOFF_BAD,"put_reason_partner_element_takeoff_default" },
	{ PUT_REASON_RA_GOD_TOKEN,"put_reason_ra_god_token" },
	{ PUT_REASON_FUNC_PREVIEW, "put_reason_func_preview" },
	{ PUT_REASON_SEVEN_COLOR_PICTURE, "put_reason_seven_color_picture" },
	{ PUT_REASON_RA_ELEMENT_CARDS, "put_reason_ra_element_cards" },
	{ PUT_REASON_SHI_TU_FB, "put_reason_shi_tu_fb" },
	{ PUT_REASON_RA_SIGN_IN_CONTINUITY, "put_reason_ra_sign_in_continuity" },
	{ PUT_REASON_WORKSHOP_BATCH_COMPOUND, "put_reason_workshop_batch_compound" },
	{ PUT_REASON_GUILD_GATHER_DREAM , "put_reason_guild_gather_dream" },
	{ PUT_REASON_MYSTERY_STORE, "put_reason_mystery_sotre" },
	{ PUT_REASON_COMPUOUND_WORK_SHOP , "put_reason_compuond_work_shop" },
	{ PUT_REASON_CREATE_ROLE, "put_reason_create_role" },
	{ PUT_REASON_SHAN_HAI_BOSS, "put_reason_shan_hai_boss" },
	{ PUT_REASON_RA_CONSUME_CAROUSEL_ALL, "put_reason_ra_consume_carousel_all" },
	{ PUT_REASON_DING_ZHI_BAO_XIANG, "put_reason_ding_zhi_bao_xiang" },
	{ PUT_REASON_ZHUAN_SHU_LI_BAO, "put_reason_zhuan_shu_li_bao" },
	{ PUT_REASON_LIU_LI_DING, "put_reason_liu_li_ding" },
	{ PUT_REASON_XIAN_SHI_BAO_HE, "put_reason_xian_shi_bao_he" },
	{ PUT_REASON_XIAN_SHI_BAO_DAI, "put_reason_xian_shi_bao_dai" },
	{ PUT_REASON_CRYSTAL_FB_FAST_PASS , "put_reason_crystal_fb_fast_pass" },

	{ PUT_REASON_ANECDOTE_TASK, "put_reason_anecdote_task" },

	{ PUT_REASON_PARTNER_BACK_REWARD, "put_reason_partner_back_reward" },
	{ PUT_REASON_RA_GIVE_PET_DRAW, "put_reason_ra_give_pet_draw" },
	{ PUT_REASON_DRAW_MANAGER, "put_reason_draw_manager" },
	{ PUT_REASON_RA_BONUS, "put_reason_ra_bonus" },
	{ PUT_REASON_RA_CI_JIU_YING_XIN, "put_reason_ra_ci_jiu_ying_xin" },
	{ PUT_REASON_RA_NIU_XING_DA_YUN, "put_reason_ra_niu_xing_da_yun" },
	{ PUT_REASON_RA_SPRING_FESTIVAL_STORE, "put_reason_ra_spring_festival_store" },
	{ PUT_REASON_RA_COW_EXCHANGE_STORE, "put_reason_ra_cow_exchange" },
	{ PUT_REASON_RA_MONPOLY_GIFT, "put_reason_ra_monpoly_gift" },
	{ PUT_REASON_RA_COW_LUCKY_EXCHANGE, "put_reason_ra_cow_lucky_exchange" },
	{ PUT_REASON_RA_LITTLE_YEAR_GIFT , "put_reason_ra_little_year_gift" },
	{ PUT_REASON_RA_COW_STORE, "put_reason_ra_cow_store" },
	{ PUT_REASON_BUG_REPAIR, "put_reason_bug_repair" },
	{ PUT_REASON_BRAVE_GROUND_SAO_DANG, "put_reason_brave_ground_sao_dang" },
	{ PUT_REASON_GIVE_SECRET_KEY, "put_reason_give_secret_key" },
	{ PUT_REASON_PET_LOOT, "put_reason_pet_loot" },
	{ PUT_REASON_DREAM_NOTES_CHALLENGE , "PUT_REASON_DREAM_NOTES_CHALLENGE" },
	{ PUT_REASON_STAR_TREASURE_MAP, "put_reason_star_treasure_map" },
	{ PUT_REASON_HIGH_TREASURE_MAP, "put_reason_high_treasure_map" },
	{ PUT_REASON_SHEN_SHOU_JIANG_LIN, "put_reason_shen_shou_jiang_lin" },
	{ PUT_REASON_SCHOOL_EXERCISE, "put_reason_school_exercise" },
	{ PUT_REASON_CHIEF_ELECTION, "put_reason_chief_election" },
	{ PUT_REASON_NOREXITEM_EQUIP ,"put_reason_norexitem_equip" },
	{ PUT_REASON_PET_BREAKTHROUGH, "put_reason_pet_breakthrough" },
	{ PUT_REASON_GOOD_GIFT_TURN ,"put_reason_good_gift_turn" },
	{ PUT_REASON_HELP_OTHERS , "put_reason_help_others" },
	{ PUT_REASON_SECRET_TREASURE , "put_reason_secret_treasure" },
	{ PUT_REASON_CHUN_RI_DENG_FENG , "put_reason_chun_ri_deng_feng" },
	{ PUT_REASON_MI_JING_QI_WEN_FB , "put_reason_mi_jing_qi_wen_fb" },
	{ PUT_REASON_ZU_HE_HE_LI , "put_reason_zu_he_he_li" },
	{ PUT_REASON_LING_PAI_TIAN_CI , "put_reason_ling_pai_tian_ci" },
	{ PUT_REASON_FASHION_SHOP , "put_reason_fashion_shop" },
	{ PUT_REASON_PARTNER_BIOGRAPHY , "put_reason_partner_biography" },
	{ PUT_REASON_NOREXITEM_ITEM ,"put_reason_norexitem_item" },
	{ PUT_REASON_ACTIVE_SYSTEM_EXTRA_REWARD, "put_reason_active_system_extra_reward" },
	{ PUT_REASON_SMART_MOUNTS_SYSTEM, "put_reason_smart_mounts_system" },
	{ PUT_REASON_REALM_BREAK, "put_reason_realm_break" },
	{ PUT_REASON_PET_MONOPOLY, "put_reason_pet_monopoly" },
	{ PUT_REASON_LUN_HUI_WANG_CHUAN, "put_reason_lun_hui_wang_chuan" },
	{ PUT_REASON_MATERIAL, "put_reason_material" },
	{ PUT_REASON_MATERIAL_ACHIEVEMENT_REWARD, "put_reason_material_achievement_reward" },
	{ PUT_REASON_CUMULATIVE_RECHARGE, "put_reason_cumulative_recharge" },
	{ PUT_REASON_FA_BAO_DRAW_REWARD , "put_reason_fa_bao_draw_reward" },
	{ PUT_REASON_CLOUD_ARENA_RANK_REWARD, "put_reason_cloud_arena_rank_reward" },
	{ PUT_REASON_PLATFORM_ACHIEVEMENT_REWARD, "put_reason_platform_achievement_reward" },
	{ PUT_REASON_CLOUD_ARENA_ACHIEVE_REWARD, "put_reason_cloud_arena_achieve_reward" },
	{ PUT_REASON_RELIC_LOOTING_REWARD, "put_reason_relic_looting_reward" },
	{ PUT_REASON_CRYSTAL_FB_RETURN_TICKET, "put_reason_crystal_fb_return_ticket" },
	{ PUT_REASON_MAP_EXPLORATION_REWARD, "put_reason_map_exploration_reward" },
	{ PUT_REASON_MARKET , "put_reason_market" },
	{ PUT_REASON_CHIVALROUS_ACHIEVE, "put_reason_chivalrous_achieve" },
	{ PUT_REASON_NOREX_CONVERT_ITEM , "put_reason_norex_convert_item" },
	{ PUT_REASON_TRANSFER_KNAPSACK_COLUMN, "put_reason_transfer_knapsack_column" },
	{ PUT_REASON_MI_YI_FETCH_WEEK_KILL_REWARD, "put_reason_mi_yi_fetch_week_kill_reward" },
	{ PUT_REASON_RESEARCH_TASK_REWARD , "put_reason_research_task_reward" },
	{ PUT_REASON_ROLE_JOIN_ACTIVITY_REWARD, "put_reason_role_join_activity_reward" },
	{ PUT_REASON_SHEN_SHOU_JIANG_LIN_BUY, "put_reason_shen_shou_jiang_lin_buy" },
	{ PUT_REASON_GUILD_ACTIVE_DAY_GIFT  , "put_reason_guild_active_day_figt" },
	{ PUT_REASON_ACHIEVEMENT_PROG_REWARD  , "put_reason_achievement_prog_reward" },
	{ PUT_REASON_PET_SOUL_EQUIPMENT_TAKEOFF ,"put_reason_pet_soul_equipment_takeoff" },
	{ PUT_REASON_TOP_LEVEL , "put_reason_top_level" },
	{ PUT_REASON_GOD_PRINT_WISH_REWARD , "put_reason_god_print_wish_reward" },
	{ PUT_REASON_GOD_PRINT_EXCHANGE_REWARD , "put_reason_god_print_exchange_reward" },
	{ PUT_REASON_NETHER_WORLD_PURGATORY_REWARD, "put_reason_nether_world_purgatory_reward" },
	{ PUT_REASON_ZHU_LING_DECOMPOSE, "put_reason_zhu_ling_decompose" },
	{ PUT_REASON_PET_GOD_FB, "put_reason_pet_god_fb" },
	{ PUT_REASON_HEART_SKILL_COMPOSE, "put_reason_heart_skill_compose" },
	{ PUT_REASON_HEART_SKILL_CONVERT, "put_reason_heart_skill_convert" },
	{ PUT_REASON_ZHU_LING_UP_LEVEL, "put_reason_zhu_ling_up_level" },
	{ PUT_REASON_ZHU_LING_PUT_ON, "put_reason_zhu_ling_put_on" },
	{ PUT_REASON_ZHU_LING_TAKE_OFF, "put_reason_zhu_ling_take_off" },
	{ PUT_REASON_FIRST_RECHARGE_EXCLUSIVE, "put_reason_first_recharge_exclusive" },
	{ PUT_REASON_MONTH_CARD_REWARD, "put_reason_month_card_reward" },
	{ PUT_REASON_BIG_DIPPER_REWARD ,"put_reason_big_dipper_reward" },
	{ PUT_REASON_ONE_KEY_GRADE_MEDAL, "put_reason_one_key_grade_medal" },
	{ PUT_REASON_TIAN_DI_DAO_HEN_ACTIVE, "put_reason_tian_di_dao_hen_active" },
	{ PUT_REASON_TIAN_DI_DAO_HEN_FINAL, "put_reason_tian_di_dao_hen_final" },
	{ PUT_REASON_TIAN_DI_DAO_HEN_DROP, "put_reason_tian_di_dao_hen_drop" },
	{ PUT_REASON_FETCH_GIVE_GIFT, "put_reason_fetch_give_gift" },
	{ PUT_REASON_FETCH_RESOURCE_DOWNLOAD_REWARD , "put_reason_fetch_resource_download_reward" },
	{ PUT_REASON_EXCHANGE_PET_SOUL_EQUIP , "put_reason_exchange_pet_soul_equip" },
	{ PUT_REASON_PRESTIGE_TASK, "put_reason_prestige_task" },
	{ PUT_REASON_HUNDRED_GHOST, "put_reason_hundred_ghost" },
	{ PUT_REASON_FANG_FRUAD_REWARD, "put_reason_fang_fruad_reward" },
	{ PUT_REASON_PET_SOUL_EQUIP_CHANGE , "put_reason_pet_soul_equip_change" },

	{ PUT_REASON_SUMMER_LI_LIAN, "put_reason_summer_li_lian" },
	{ PUT_REASON_COLORFUL_LANTERN, "put_reason_colorful_lantern" },
	{ PUT_REASON_SUMMER_STORE , "put_reason_summer_store" },
	{ PUT_REASON_LUCKY_SIGN , "put_reason_lucky_sign" },
	{ PUT_REASON_QIONG_LOU_YU_YAN , "put_reason_qiong_lou_yu_yan" },
	{ PUT_REASON_SUPER_OFF_SHOP , "put_reason_super_off_shop" },
	{ PUT_REASON_YUAN_QI_TURNING , "put_reason_yuan_qi_turning" },
	{ PUT_REASON_COOL_BREEZE , "put_reason_cool_breeze" },
	{ PUT_REASON_LOTUS_MI_BAO , "put_reason_lotus_mi_bao" },
	{ PUT_REASON_MAZE_SUMMER , "put_reason_maze_summer" },
	{ PUT_REASON_MI_BAO_FANG_DIAN, "put_reason_mi_bao_fang_dian" },
	{ PUT_REASON_ZHEN_PIN_SHANG_PU, "put_reason_zhen_pin_shang_pu" },
	{ PUT_REASON_ATTR_PELLET_RET, "put_reason_attr_pellet_ret" },
	{ PUT_REASON_ACTIVE_TURNTABLE , "put_reason_active_turntable" },
	{ PUT_REASON_PARTNER_PACT_GET, "put_reason_partner_pact_get" },
	{ PUT_REASON_PARTNER_PACT_PUT_ON, "put_reason_partner_pact_put_on" },
	{ PUT_REASON_PARTNER_PACT_UP_STAR, "put_reason_partner_pact_up_star" },
	{ PUT_REASON_RA_BACK_REWARD , "put_reason_ra_back_reward" },
	{ PUT_REASON_DUEL, "PUT_REASON_DUEL" },
	{ PUT_REASON_RA_HAPPY_LOOK , "put_reason_ra_happy_look"},
	{ PUT_REASON_SKILL_BOOK_CONVERT ,"put_reason_skill_book_convert"},
	{ PUT_REASON_SERVER_COMPETITION, "put_reason_server_competition"},
	{ PUT_REASON_DU_JIE, "put_reason_du_jie" },
	{ PUT_REASON_YIN_YANG_EYE, "put_reason_yin_yang_eye" },
	{ PUT_REASON_REALM_BREAK_FIGHT, "put_reason_realm_break_fight" },
	{ PUT_REASON_CAPACITY_TARGET, "put_reason_capacity_target" },
	{ PUT_REASON_HELP_OTHERS_2, "put_reason_help_others_2" },
	{ PUT_REASON_ANECDOTE_BIG_EVENT, "put_reason_anecdote_big_event" },
	{ PUT_REASON_YUN_ZE_TERRITORY, "put_reason_yun_ze_territory" },
	{ PUT_REASON_RA_SIGN_HAPPY, "put_reason_ra_sign_happy" },
	{ PUT_REASON_RA_WANLING_GONGYING, "put_reason_ra_wanling_gongying" },
	{ PUT_REASON_RA_HORCRUX_TIANCHENG, "put_reason_ra_horcrux_tiancheng" },
	{ PUT_REASON_RA_HORCRUX_TIANCHENG_DRAW , "put_reason_ra_horcrux_tianchen_draw" },
	{ PUT_REASON_RA_INTEGRAL_DRAW , "put_reason_ra_integral_draw" },
	{ PUT_REASON_SHAN_HAI_QUESTION_REWARD ,"put_reason_shan_hai_question_reward" },
	{ PUT_REASON_SHAN_HAI_JU_BAO_DRAW, "put_reason_shan_hai_ju_bao_draw" },
	{ PUT_REASON_SHAN_HAI_JU_BAO_EXTRA_REWARD, "put_reason_shan_hai_ju_bao_extra_reward" },
	{ PUT_REASON_SHAN_HAI_MET_REWARD, "put_reason_shan_hai_met_reward" },
	{ PUT_REASON_RA_SHAN_HAI_MEET , "put_reason_ra_shan_hai_meet" },
	{ PUT_REASON_RA_FATE_ENTANGLEMENT ,"put_reason_ra_fate_entanglement" },
	{ PUT_REASON_ENDLESS_TOWER_REWARD , "put_reason_endless_tower_reward" },
	{ PUT_REASON_HUAN_JIE_TODAY_WIN_REWARD, "put_reason_huan_jie_today_win_reward" },
	{ PUT_REASON_RA_ZHAOCAIJINBAO_DRAW, "put_reason_zhaocaijinbao_draw" },
	{ PUT_REASON_RA_ZHAOCAIJINBAO , "put_reason_zhaocaijinbao" },
	{ PUT_REASON_RA_ZAI_ZHAN_YUN_ZE , "put_reason_ra_zai_zhan_yun_ze" },
	{ PUT_REASON_YUN_ZE_SLOT_MACHINE,"put_reason_yun_ze_slot_machine" },
	{ PUT_REASON_MO_ZU_ZAI_NA_LI , "put_reason_mo_zu_zai_na_li" },
	{ PUT_REASON_YUN_ZE_SLOT_MACHINE_BUY_KEY ,"put_reason_yun_ze_slot_machine_buy_key" },
	{ PUT_REASON_CHAOZHI_FENXIANG , "put_reason_chaozhi_fenxiang" },
	{ PUT_REASON_RA_COLORFUL_DOUBLE ,"put_reason_ra_colorful_double" },
	{ PUT_REASON_SHAN_HAI_ZHENG_FENG_REWARD , "put_reason_shan_hai_zheng_feng_reward" },
	{ PUT_REASON_SHAN_HAI_MI_BAO_REWARD , "put_reason_shan_hai_mi_bao_reward" },
	{ PUT_REASON_SHAN_HAI_BAO_XIANG  , "put_reason_shan_hai_bao_xiang" },
	{ PUT_REASON_SHAN_HAI_BAO_HE  , "put_reason_shan_hai_bao_he" },
	{ PUT_REASON_SHAN_HAI_BAO_DAI  , "put_reason_shan_hai_bao_dai" },
	{ PUT_REASON_RA_YUN_ZE_ER_SAN_SHI , "put_reason_ra_yun_ze_er_san_shi" },
	{ PUT_REASON_SHANHAI_HUADENG , "put_reason_shanhai_huadeng" },
	{ PUT_REASON_WAN_LING_FB , "put_reason_wan_ling_fb" },
	{ PUT_REASON_YUNZE_BLOOMING_RECORD , "put_reason_yunze_blooming_record" },
	{ PUT_REASON_RA_JIXING_GAOZHAO, "put_reason_ra_jixing_gaozhao" },
	{ PUT_REASON_RA_JIXING_GAOZHAO_DRAW, "put_reason_ra_jixing_gaozhao_draw" },
	{ PUT_REASON_RA_YUNZE_ER_SAN_SHI_REWARD, "put_reason_ra_yunze_er_san_shi_reward" },
	{ PUT_REASON_BOUNTY_FINISH_REWARD, "put_reason_bounty_finish_reward" },
	{ PUT_REASON_DIAN_HUA_SHI,"put_reason_dian_hua_shi" },
	{ PUT_REASON_FISHING, "put_reason_fishing" },
	{ PUT_REASON_NEW_LIFE_MAKE, "put_reason_new_life_make" },
	{ PUT_REASON_MIRAGE_TERRITORY , "put_reason_mirage_territory" },
	{ PUT_REASON_RA_YUNZE_CATCH_ELVES , "put_reason_yunze_catch_elves" },
	{ PUT_REASON_RA_YUNZE_HAO_LI , "put_reason_yunze_hao_li" },
	{ PUT_REASON_RA_YUNZE_HAO_LI_BUY_GOLD_HAMMER , "put_reason_yunze_hao_li_buy_gold_hammer" },
	{ PUT_REASON_RA_XIAN_SHI_LEI_CHONG , "put_reason_ra_xian_shi_lei_chong" },
	{ PUT_REASON_RA_YUNZE_WISH , "put_reason_yunze_wish" },
	{ PUT_REASON_RA_WEEKEND_GIFT , "PUT_REASON_RA_WEEKEND_GIFT" },
	{ PUT_REASON_RA_YUNZE_CUTE_PET , "put_reason_yunze_cute_pet" },
	{ PUT_REASON_RA_CHAO_ZHI_GIFT , "PUT_REASON_RA_CHAO_ZHI_GIFT" },
	{ PUT_REASON_RA_FLY_UP_REWARD  , "PUT_REASON_RA_FLY_UP_REWARD" },
	{ PUT_REASON_TREASURE_TASK , "put_reason_treasure_task" },
	{ PUT_REASON_RA_TIGER_AUSPICIOUS_SNOW, "put_reason_ra_tiger_auspicious_snow" },
	{ PUT_REASON_RA_TIGER_FISH_EVERY_YEAR, "put_reason_ra_tiger_fish_every_year" },
	{ PUT_REASON_RA_YINHU_WENSHI , "put_reason_ra_yinhu_wenshi" },
	{ PUT_REASON_RA_TIGER_FIND_SPRING , "put_reason_ra_tiger_find_spring" },
	{ PUT_REASON_RA_TIGER_TEASE_NEW_YEAR , "put_reason_ra_tiger_tease_new_year" },
	{ PUT_REASON_RA_CINIU_YINGXINHU , "put_reason_ra_ciniu_yingxinghu" },
	{ PUT_REASON_RA_TIGER_LUCKY_MONEY , "put_reason_ra_tiger_lucky_money" },
	{ PUT_REASON_RA_HU_XING_DA_YUN , "put_reason_ra_hu_xing_da_yun" },
	{ PUT_REASON_RA_YIN_HU_BAI_SUI , "put_reason_ra_yin_hu_bai_sui" },
	{ PUT_REASON_RA_AUSPICIOUS_SNOW_PICK_UP , "put_reason_ra_auspicious_snow_pick_up" },
	{ PUT_REASON_RA_JINGJI_YUEKA_REWARD, "put_reason_ra_jingji_yueka_reward" },
	{ PUT_REASON_WEDDING, "put_reason_wedding" },
	{ PUT_REASON_RONG_LIAN_YOU_LI ,"put_reason_rong_lian_you_li" },
	{ PUT_REASON_FALLEN_GOD_REWARD ,"put_reason_fallen_god_reward" },
	{ PUT_REASON_PET_FEI_SHENG_REWARDS ,"put_reason_pet_fei_sheng_rewards" },
	{ PUT_REASON_RA_QINGYUAN_SHIZHUANG , "put_reason_ra_qingyuan_shizhuang" },
	{ PUT_REASON_PET_BATCH_EXCHANGE , "put_reason_pet_batch_exchange" },
	{ PUT_REASON_MINI_GAME_STAGE_REWARD, "put_reason_mini_game_stage_reward" },
	{ PUT_REASON_MINI_GAME_PHASE_REWARD, "put_reason_mini_game_phase_reward" },
	{ PUT_REASON_QING_YUAN_GIFT_DAY_REWARD, "put_reason_qing_yuan_gift_day_reward" },
	{ PUT_REASON_RA_QINGYUAN_LEICHONG , "put_reason_ra_qingyuan_leichong" },
	{ PUT_REASON_FALLEN_GOD_HELP_PASS_REWARD ,"put_reason_fallen_god_reward" },
	{ PUT_REASON_WORLD_ARENA_WIN_BOX , "put_reason_world_arena_win_box" },
	{ PUT_REASON_GOD_PRINT_EXCHANGE_2 , "put_reason_world_arena_win_box" },
	{ PUT_REASON_QING_YUAN_MARRAY_REWARD , "put_reason_qing_yuan_marray_reward" },
	{ PUT_REASON_WORLD_ARENA_SIGNUP_REWARD, "put_reason_signup_reward" },
	{ PUT_REASON_WORLD_ARENA_DAILY_REWARD, "put_reason_daily_reward" },
	{ PUT_REASON_PET_BADGE_TAKE_OFF , "put_reason_pet_badge_take_off" },
	{ PUT_REASON_PET_BADGE_COMPOSE , "put_reason_pet_badge_compose" },
	{ PUT_REASON_PET_BADGE_DECOMPOSE , "put_reason_pet_badge_decompose" },
	{ PUT_REASON_WORLD_TEAM_ARENA, "put_reason_world_team_arena" },
	{ PUT_REASON_PEAK_TOKEN , "PUT_REASON_PEAK_TOKEN" },
	{ PUT_REASON_PET_STRENGTHEN_REWARD , "PUT_REASON_PET_STRENGTHEN_REWARD" },
	{ PUT_REASON_PARTNER_INTENSIFY, "put_reason_partner_intensify" },
	{ PUT_REASON_QING_YUAN_SHENG_HUI, "put_reason_qing_yuan_sheng_hui" },
	{ PUT_REASON_YAO_XING_SHI_LIAN, "put_reason_yao_xing_shi_lian" },
	{ PUT_REASON_SHI_YU_MILE, "PUT_REASON_SHI_YU_MILE" },
	{ PUT_REASON_SHI_YU_CHUAN_GONG, "PUT_REASON_SHI_YU_CHUAN_GONG" },
	{ PUT_REASON_ACCESSORIES_BACK_STRENGTHEN, "put_reason_accessories_back_strengthen" },
	{ PUT_REASON_SAN_XIAN_GUI_DONG_REWARD, "put_reason_san_xian_gui_dong_reward" },
	{ PUT_REASON_LANTERN_RIDDIE, "put_reason_lantern_riddie" },
	{ PUT_REASON_WHERE_IS_THE_FAIRY, "put_reason_where_is_the_fairy" },
	{ PUT_REASON_TIAN_GONG_XUN_BAO , "put_reason_tian_gong_xun_bao" },
	{ PUT_REASON_QINGYUAN_DUIDUIPENG , "put_reason_qingyuan_duiduipeng" },
	{ PUT_REASON_LOVERS_MARKET , "put_reason_lovers_market" },
	{ PUT_REASON_ANSWER_LANTERN_RIDDIE , "put_reason_answer_lantern_riddie" },
	{ PUT_REASON_CONCENTRIC_KNOT , "put_reason_concentric_knot" },
	{ PUT_REASON_TAO_QUAN_NA_JI ,"put_reason_tao_quan_na_ji" },
	{ PUT_REASON_TU_ZI_SAI_PAO , "put_reason_tu_zi_sai_pao" },
	{ PUT_REASON_GUESS_BET_REWARD , "PUT_REASON_GUESS_BET_REWARD" },
	{ PUT_REASON_GUESS_BET_FAILD_RETURN , "PUT_REASON_GUESS_BET_FAILD_RETURN" },
	{ PUT_REASON_UTA_WIN_BOX, "put_reason_win_box" },
	{ PUT_REASON_UTA_TODAY_JOIN_REWARD, "put_reason_uta_today_join_reward" },
	{ PUT_REASON_UTA_ACTIVE_REWARD, "put_reason_uta_active_reward" },
	{ PUT_REASON_ROLE_HUNDRED_GOD_BOSS, "put_reason_role_hundred_god_boss" },
	{ PUT_REASON_XUN_BAO_GUIDE_PARTNER, "put_reason_xun_bao_guide_partner" },
	{ PUT_REASON_XUN_BAO_GUIDE_PARTNER_2, "put_reason_xun_bao_guide_partner_2" },
	{ PUT_REASON_RA_GOD_BEAST_ADVENT ,"put_reason_ra_god_beast_advent" },
	{ PUT_REASON_RA_YUAN_QI_SPEED , "put_reason_ra_yuan_qi_speed" },
	{ PUT_REASON_RA_YUAN_QI_HELP , "put_reason_ra_yuan_qi_help" },
	{ PUT_REASON_RA_QIYUAN_KONGMINGDENG , "put_reason_ra_qiyuan_kongmingdeng" },
	{ PUT_REASON_RA_XUNMENG_HUDIE , "put_reason_ra_xunmeng_hudie" },
	{ PUT_REASON_RA_BO_ZHONG_YOU_LI , "put_reason_ra_bo_zhong_you_li" },
	{ PUT_REASON_RA_YUAN_QI_FAN_PAI , "put_reason_ra_yuan_qi_fan_pai" },
	{ PUT_REASON_RA_YUAN_QI_FAN_PAI_REWARD , "put_reason_ra_yuan_qi_fan_pai_reward" },
	{ PUT_REASON_RA_WANLING_ZHULI , "put_reason_ra_wanling_zhuli" },
	{ PUT_REASON_RA_BAO_ZI_LAI_LE ,"put_reason_ra_bao_zi_lai_le" },
	{ PUT_REASON_RA_JIN_YI_TIAN_XIANG , "put_reason_ra_jin_yi_tian_xiang" },
	{ PUT_REASON_FETCH_PINJIA_REWARD , "put_reason_fetch_pinjia_reward" },
	{ PUT_REASON_RA_MIAO_BI_SHENG_HUA ,"put_reason_ra_miao_bi_sheng_hua" },
	{ PUT_REASON_RA_YUANQI_JISHIBU , "put_reason_ra_yuan_qi_jishibu" },
	{ PUT_REASON_RA_YUAN_QI_YAN_HUO , "put_reason_ra_yuan_qi_yan_huo" },
	{ PUT_REASON_RA_YUAN_QI_YAN_HUO_OPEN_ITEM , "put_reason_ra_yuan_qi_yan_huo_open_item" },
	{ PUT_REASON_RA_YUAN_QI_FAN_PAI_DAY_FREE_REWARD , "put_reason_ra_yuan_qi_fan_pai_day_free_reward" },
	{ PUT_REASON_RA_CHENG_FENG_PO_LANG , "put_reason_ra_cheng_feng_po_lang" },
	{ PUT_REASON_YUAN_HUN_ZHU_UP_LEVEL , "put_reason_yuan_hun_zhu_up_level" },
	{ PUT_REASON_YUAN_HUN_ZHU_PUT_ON , "put_reason_yuan_hun_zhu_put_on" },
	{ PUT_REASON_YUAN_HUN_ZHU_TAKE_OFF , "put_reason_yuan_hun_zhu_take_off" },
	{ PUT_REASON_ZAO_HUA_DISCOMPOSE , "put_reason_zao_hua_discompose" },
	{ PUT_REASON_HONGMENG_TIANJIE , "put_reason_hongmeng_tianjie" },
	{ PUT_REASON_HONGMENG_TIANJIE_BUY , "put_reason_hongmeng_tianjie_buy" },
	{ PUT_REASON_YUAN_HUN_ZHU_DECOMPOSE , "put_reason_yuan_hun_zhu_decompose" },
	{ PUT_REASON_YUAN_HUN_ZHU_COMPOSE , "put_reason_yuan_hun_zhu_compose" },
	{ PUT_REASON_ZAO_HUA_COMPOSE , "put_reason_zao_hua_compose" },
	{ PUT_REASON_RA_ZONG_XIANG_REWARD , "put_reason_ra_zong_xiang_reward" },
	{ PUT_REASON_RA_SALTY_SWEET_BATTLE , "put_reason_ra_salty_sweet_battle" },
	{ PUT_REASON_RA_DUAN_YANG_XIANG_NANG , "put_reason_ra_duan_yang_xiang_nang" },
	{ PUT_REASON_RA_DUAN_YANG_ZI_XUAN , "put_reason_ra_duan_yang_zi_xuan" },
	{ PUT_REASON_RA_CANG_LONG_CI_FU , "put_reason_ra_cang_long_ci_fu" },
	{ PUT_REASON_RA_LUCKY_BLIND_BOX_REWARD , "put_reason_ra_lucky_blind_box_reward" },
	{ PUT_REASON_RA_LAN_TANG_CHU_MU , "put_reason_ra_lan_tang_chu_mu" },
	{ PUT_REASON_RA_SANBAI_CHOU , "put_reason_ra_sanbai_chou" },
	{ PUT_REASON_RA_FU_LI , "put_reason_ra_fu_li" },
	{ PUT_REASON_RA_LUCKY_BLIND_EXTRA_REWARD , "put_reason_ra_lucky_blind_extra_reward" },
	{ PUT_REASON_RA_JIN_LONG_JU_BAO_DRAW , "put_reason_ra_jin_long_ju_bao_draw" },
	{ PUT_REASON_RA_JIN_LONG_JU_BAO_FETCH , "put_reason_ra_jin_long_ju_bao_fetch" },
	{ PUT_REASON_RA_JIN_LONG_JU_BAO_BUY , "put_reason_ra_jin_long_ju_bao_buy" },
	{ PUT_REASON_SPECIAL_ACTIVE_REWARD , "put_reason_special_active_reward" },
	{ PUT_REASON_RA_FORTUNE_MAZE , "put_reason_ra_fortune_maze" },

	{ PUT_REASON_KE_JU_EXAM_ANSWER_REWARD , "put_reason_ke_ju_exam_answer_reward" },
	{ PUT_REASON_KE_JU_EXAM_LAST_REWARD , "put_reason_ke_ju_exam_last_reward" },
	{ PUT_REASON_GOTO_SCHOOL , "put_reason_goto_school" },
	{ PUT_REASON_ZHOU_QI_SHOP , "put_reason_zhou_qi_shop" },
	{ PUT_REASON_CHONG_JI_FAN_LI, "put_reason_chong_ji_fan_li" },
	{ PUT_REASON_RA_SHEN_SHOU_EN_ZE, "put_reason_ra_shen_shou_en_ze" },
	{ PUT_REASON_RA_SHEN_SHOU_EN_ZE_FETCH, "put_reason_ra_shen_shou_en_ze_fetch" },
	{ PUT_REASON_RA_SHEN_SHOU_EN_ZE_BUY, "put_reason_ra_shen_shou_en_ze_buy" },
	{ PUT_REASON_RA_XIN_FU_QIANG_HUA, "put_reason_ra_xin_fu_qiang_hua" },
	{ PUT_REASON_GOD_EQUIP_WORK_SHOP,"put_reason_god_equip_work_shop" },
	{ PUT_REASON_RA_QI_XI_TE_HUI , "put_reason_special_active_reward" },
	{ PUT_REASON_CHARISMAITC_TANABATA_REWARD , "put_reason_charismatic_tanabata_reward" },
	{ PUT_REASON_XIAN_SHI_BAO_HE_3 , "put_reason_xian_shi_bao_he_3" },
	{ PUT_REASON_XIAN_SHI_BAO_HE_4 , "put_reason_xian_shi_bao_he_4" },
	{ PUT_REASON_RA_PET_EXCHANGE_BUY_ITEM, "put_reason_ra_pet_exchange_buy_item" },
	{ PUT_REASON_HALLOWS_SKILL_CONVERT,"put_reason_hallows_skill_convert" },
	{ PUT_REASON_SHENG_QI_TREASURE_MAP,"put_reason_sheng_qi_treasure_map" },
	{ PUT_REASON_TAKE_OFF_FU_SHI,"put_reason_take_off_fu_shi" },
	{ PUT_REASON_RA_SHEN_CI_HUA_FU_DRAW, "put_reason_ra_shen_ci_hua_fu_draw" },
	{ PUT_REASON_FU_YU_TAKE_OFF,"put_reason_fu_yu_take_off" },
	{ PUT_REASON_FU_YU_UPGRADE_SUCC,"put_reason_fu_yu_upgrade_succ" },
	{ PUT_REASON_FU_YU_UPGRADE_FAIL,"put_reason_fu_yu_upgrade_fail" },
	{ PUT_REASON_FU_YU_USE_GIFT,"put_reason_fu_yu_use_gift" },
	{ PUT_REASON_ELEMENT_ELF_REWARD, "put_reason_element_elf_reward" },
	{ PUT_REASON_COLOSSEUM_BATTLE_REWARD,"put_reason_colosseum_battle_reward" },
	{ PUT_REASON_XIN_FU_LING_CHONG, "put_reason_xin_fu_ling_chong" },
	{ PUT_REASON_RA_HUN_QI_FAN_LI, "put_reason_ra_hun_qi_fan_li" },
	{ PUT_REASON_RA_SHEN_CI_HUA_FU_BUY, "put_reason_ra_pet_exchange_buy_item" },
	{ PUT_REASON_RA_TIAN_YI_YAO_SHI, "put_reason_ra_tian_yi_yao_shi" },
	{ PUT_REASON_KUA_FU_YING_XIONG_LU, "put_reason_kua_fu_ying_xiong_lu" },
	{ PUT_REASON_SMART_MOUNTS_CONVERT_ITEM, "put_reason_smart_mounts_convert_item" },
	{ PUT_REASON_ONE_CLICK_FINISH, "put_reason_one_click_finish" },
	{ PUT_REASON_SMART_MOUNTS_ACTIVE_CONVERT, "put_reason_smart_mounts_active_convert" },
	{ PUT_REASON_DAY_REWARD_ONLINE_GIFT, "put_reason_day_reward_online_gift" },
	{ PUT_REASON_STAR_CHART_RESET, "put_reason_star_chart_reset" },
	{ PUT_REASON_NEW_COURAGE_CHALLENGE, "put_reason_new_courage_challenge" },
	{ PUT_REASON_FASHION_COLLECTION_DECOMPOSE, "put_reason_fashion_collection_decompose" },
	{ PUT_REASON_SMART_MOUNTS_SKILL_COMPOSE, "put_reason_smart_mounts_skill_compose" },
	{ PUT_REASON_STAR_CHART_LEVEL_UP, "put_reason_star_chart_level_up" },
	{ PUT_REASON_YUE_XI_LEI_CHONG, "put_reason_yue_xi_lei_chong" },
	{ PUT_REASON_YUE_XI_DENG_MI, "put_reason_yue_xi_deng_mi" },
	{ PUT_REASON_RA_ZHUI_YUE_SHANG_DIAN_BUY, "put_reason_ra_zhui_yue_shang_dian_buy" },
	{ PUT_REASON_RA_ZHUI_YUE_SHANG_DIAN_DRAW, "put_reason_ra_zhui_yue_shang_dian_draw" },
	{ PUT_REASON_FU_YU_SUBLIME, "put_reason_fu_yu_sublime" },
	{ PUT_REASON_FU_YU_SWITCH, "put_reason_fu_yu_switch" },
	{ PUT_REASON_SPECIAL_ANECDOTE, "PUT_REASON_SPECIAL_ANECDOTE" },
	{ PUT_REASON_TEAM_CHALLENGE , "PUT_REASON_TEAM_CHALLENGE" },
	{ PUT_REASON_HUAN_SHOU_ACTIVE_CONVERT , "put_reason_huan_shou_active_convert" },
	{ PUT_REASON_HUAN_SHOU_LIAN_DAN, "put_reason_huan_shou_lian_dan"},
	{ PUT_REASON_HUAN_SHOU_DECOMPOSE, "put_reason_huan_shou_decompose"},
	{ PUT_REASON_GUILD_TASK_GIVE , "put_reason_guild_task_help" },
	{ PUT_REASON_GUILD_TASK_FINISH , "put_reason_guild_task_finish" },
	{ PUT_REASON_GUILD_TASK_FETCH , "put_reason_guild_task_fetch" },
	{ PUT_REASON_PET_ABANDONMENT_GAI_MING , "put_reason_pet_abandonment_gai_ming" },
	{ PUT_REASON_GUILD_INBORN_RESET_RETURN, "put_reason_guild_inborn_reset_return" },
	{ PUT_REASON_REAL_NAME_REWARD, "put_reason_real_name_reward" },

	//{PUT_REASON_MAX, "MAX"}
};

// 新增reason类型时，记得在上面加一个原因字符串，此处只当作一个编译时提示

UNSTD_STATIC_CHECK(451 == PUT_REASON_MAX);

const char * g_GetPutReasonTypeStr(int reason)
{
	if (reason >= PUT_REASON_INVALID && reason < PUT_REASON_MAX
		&& reason < int(sizeof(gs_put_reason_str_list) / sizeof(gs_put_reason_str_list[0])))
	{
		return gs_put_reason_str_list[reason].str;
	}

	return "NULL";
};

UNSTD_STATIC_CHECK(sizeof(gs_put_reason_str_list) / sizeof(gs_put_reason_str_list[0]) == PUT_REASON_MAX);

void Knapsack::CertainPut(int column, const ItemDataWrapper & item_info, int reason_type, int * out_column, int * out_index, UInt16 drop_monster_id /* = 0 */, bool is_notice /* = true */)
{
	if (column < ITEM_COLUMN_TYPE_MIN || column >= ITEM_COLUMN_TYPE_MAX) return;

	const ItemBase *itembase = ITEMPOOL->GetItem(item_info.item_id);
	if (NULL == itembase) return;

	int curr_valid_grid_num = m_curr_valid_grid_num_list[column];
	int valid_grid_index_end = m_valid_grid_index_end_list[column];
	if (valid_grid_index_end > curr_valid_grid_num || curr_valid_grid_num > ITEM_COLUMN_GRID_NUM_MAX) return;

	bool is_net_value_item = (itembase->GetItemType() >= I_TYPE_NET_VALUE_ITEM_BEGIN);

	static int real_put_index_list[ITEM_COLUMN_GRID_NUM_MAX] = { 0 };
	static int real_put_column_list[ITEM_COLUMN_GRID_NUM_MAX] = { 0 };
	int real_put_index_count = 0;

	ItemDataWrapper tmp_item_wrapper(item_info);
	if (0 == tmp_item_wrapper.item_unique_id)
	{
		UniqueIDGenerator::Instance()->GenUniqueID(&tmp_item_wrapper.item_unique_id);
	}

	if (itembase->IsBind())
	{
		tmp_item_wrapper.is_bind = 1;
	}

	if (itembase->GetItemType() == I_TYPE_MEDAL)
	{
		NetValueItemParam tmp_item_param;
		tmp_item_param.SetData(tmp_item_wrapper.param_data);
		tmp_item_param.medal_param.CheckModifyVersion();
		tmp_item_param.GetData(tmp_item_wrapper.param_data);
	}

	// 判断是否已给净值物品随机净值， 如果未随机，则随机一次
	if (is_net_value_item)
	{
		NetValueItemParam tmp_item_param;
		tmp_item_param.SetData(tmp_item_wrapper.param_data);
		if (!itembase->HasRandomNetValue(&tmp_item_param))
		{
			itembase->RandomNetValueOnPutItem(m_module_mgr->GetRole(), &tmp_item_param, reason_type);
			itembase->SetRandomNetValueFlag(&tmp_item_param);
			tmp_item_param.GetData(tmp_item_wrapper.param_data);
		}
		tmp_item_wrapper.has_param = 1;

		if (itembase->GetItemType() == I_TYPE_EQUIPMENT)
		{
			NetValueItemParam param;
			param.SetData(tmp_item_wrapper.param_data);
			param.equip_param.CheckModifyVersion();
			param.GetData(tmp_item_wrapper.param_data);
		}
		else if (itembase->GetItemType() == I_TYPE_JEWELRY)
		{
			NetValueItemParam param;
			param.SetData(tmp_item_wrapper.param_data);
			param.jewelry_param.CheckModifyVersion();
			param.GetData(tmp_item_wrapper.param_data);
		}
	}
	if (PUT_REASON_MARKET == reason_type)	//市场购买的道具不可与其他来源道具堆叠
	{
		tmp_item_wrapper.is_market_buy_flag = 1;
	}

	int empty_index = -1;
	for (int i = 0; i < valid_grid_index_end; ++i)
	{
		if (is_net_value_item) // 净值物品只需要找到第一个空位置即可
		{
			if (NULL == m_grid_list[column][i] || m_grid_list[column][i]->Invalid())
			{
				empty_index = i;
				break;
			}
		}
		else
		{
			if (NULL == m_grid_list[column][i] || m_grid_list[column][i]->Invalid())
			{
				if (-1 == empty_index) empty_index = i;
			}
			else if (m_grid_list[column][i]->CanBeMerge(true)) // 下面Merge里会去对比invalid_time
			{
				int rest_num = 0;
				int merged_gold_price = 0;
				int merge_result = m_grid_list[column][i]->Merge(tmp_item_wrapper, &rest_num, &merged_gold_price);
				if (ITEM_MERGE_COMPLETE == merge_result)
				{
					real_put_column_list[real_put_index_count] = column;
					real_put_index_list[real_put_index_count] = i; // 已经放完 不需要检查count了
					++real_put_index_count;

					tmp_item_wrapper.num = 0;
					tmp_item_wrapper.gold_price = 0;
					break;
				}
				else if (ITEM_MERGE_NOT_COMPLETE == merge_result)
				{
					real_put_column_list[real_put_index_count] = column;
					real_put_index_list[real_put_index_count] = i;
					++real_put_index_count;

					tmp_item_wrapper.num = rest_num;
					tmp_item_wrapper.gold_price -= merged_gold_price;
				}

				if (real_put_index_count >= ITEM_COLUMN_GRID_NUM_MAX) return;
			}
		}
	}

	if (tmp_item_wrapper.num > 0) // 还有放剩的
	{
		if (-1 == empty_index && valid_grid_index_end < curr_valid_grid_num) // 前面全满 则放到下一个空位里
		{
			empty_index = valid_grid_index_end;
		}

		if (empty_index < 0 || empty_index >= ITEM_COLUMN_GRID_NUM_MAX) return;

		if (NULL == m_grid_list[column][empty_index])
		{
			m_grid_list[column][empty_index] = new ItemGridData();
		}
		if (tmp_item_wrapper.item_unique_id == 0)
		{
			long long item_unique_id = 0;
			UniqueIDGenerator::Instance()->GenUniqueID(&item_unique_id);
			if (0 != item_unique_id)
			{
				tmp_item_wrapper.item_unique_id = item_unique_id;
			}
		}

		m_grid_list[column][empty_index]->Set(tmp_item_wrapper);
		if (real_put_index_count >= ITEM_COLUMN_GRID_NUM_MAX) return;

		real_put_column_list[real_put_index_count] = column;
		real_put_index_list[real_put_index_count] = empty_index;
		++real_put_index_count;

#ifdef TRACK_ITEM_FLAG
		gamelog::g_log_item_track.printf(LL_INFO, "TRACK_ITEM_FLAG Knapsack::CertainPut role[%d,%s] column[%d] index[%d] item_id[%d] num[%d] unique_id[%lld]",
			m_module_mgr->GetRole()->GetUID(), m_module_mgr->GetRole()->GetName(),
			column, empty_index, tmp_item_wrapper.item_id, tmp_item_wrapper.num, tmp_item_wrapper.item_unique_id);
#endif
	}

	if (real_put_index_count <= 0 || real_put_index_count > ITEM_COLUMN_GRID_NUM_MAX) return;

	if (real_put_index_count > 1)
	{
		if (real_put_index_list[real_put_index_count - 1] > real_put_index_list[real_put_index_count - 2]) // 最后面这个不一定比倒数第2的大
		{
			this->RefreshValidGridIndexEnd(column, real_put_index_list[real_put_index_count - 1]);
		}
		else
		{
			this->RefreshValidGridIndexEnd(column, real_put_index_list[real_put_index_count - 2]);
		}
	}
	else
	{
		this->RefreshValidGridIndexEnd(column, real_put_index_list[real_put_index_count - 1]);
	}

	short change_type = ITEM_CHANGE_NOTICE_REASON_PUT;
	switch (reason_type)
	{
	case PUT_REASON_XUN_BAO:
	case PUT_REASON_WORKSHOP_BATCH_COMPOUND:
	case PUT_REASON_RAND_GIFT:
	case PUT_REASON_GIFT_RAND_ITEM:
	case PUT_REASON_GIFT_OPTIONAL_ITEM:
	case PUT_REASON_OPTIONAL_GIFT:
	case PUT_REASON_DEF_GIFT:
	case PUT_REASON_RA_CONSUME_CAROUSEL:
	case PUT_REASON_LUCKY_TURNTABLE:
	case PUT_REASON_COMPUOUND_WORK_SHOP:
	case PUT_REASON_SHEN_SHOU_JIANG_LIN:
	case PUT_REASON_STAR_TREASURE_MAP:
	case PUT_REASON_GOOD_GIFT_TURN:
	case PUT_REASON_NOREXITEM_ITEM:
	case PUT_REASON_MATERIAL_ACHIEVEMENT_REWARD:
	case PUT_REASON_CRYSTAL_FB_RETURN_TICKET:
	case PUT_REASON_SHAN_HAI_BOSS:
	//case PUT_REASON_DREAM_NOTES_CHALLENGE:
	case PUT_REASON_TRANSFER_KNAPSACK_COLUMN:
	case PUT_REASON_ACHIEVEMENT_PROG_REWARD:
	case PUT_REASON_HUNDRED_GHOST:
	case PUT_REASON_ONE_KEY_GRADE_MEDAL:
	case PUT_REASON_ACTIVE_TURNTABLE:
	case PUT_REASON_DU_JIE:
	case PUT_REASON_REALM_BREAK_FIGHT:
	case PUT_REASON_RA_HORCRUX_TIANCHENG_DRAW:
	case PUT_REASON_SHAN_HAI_JU_BAO_DRAW:
	case PUT_REASON_ENDLESS_TOWER_REWARD:
	case PUT_REASON_RA_JIXING_GAOZHAO_DRAW:
	case PUT_REASON_RA_ZAI_ZHAN_YUN_ZE:
	case PUT_REASON_YUN_ZE_SLOT_MACHINE:
	case PUT_REASON_RA_YUN_ZE_ER_SAN_SHI:
	case PUT_REASON_WAN_LING_FB:
	case PUT_REASON_GATHER_OBJECT:
	case PUT_REASON_FISHING:
	case PUT_REASON_TREASURE_TASK:
	case PUT_REASON_RA_YINHU_WENSHI:
	case PUT_REASON_WHERE_IS_THE_FAIRY:
	case PUT_REASON_SHI_YU_CHUAN_GONG:
	case PUT_REASON_HONGMENG_TIANJIE:
		{
			change_type = ITEM_CHANGE_NOTICE_REASON_DATA_CHANGE;	// 不让客户端飘字
		}
		break;
	}

	if (!is_notice)
	{
		change_type = ITEM_CHANGE_NOTICE_REASON_DATA_CHANGE;
	}

	for (int i = 0; i < real_put_index_count; i++)
	{
		this->SetItemDirty(real_put_column_list[i], real_put_index_list[i], true, change_type, reason_type);
	}

	if (NULL != out_index) *out_index = real_put_index_list[0]; // 只传出第一个
	if (NULL != out_column) *out_column = real_put_column_list[0];

	char put_reason_str[64] = { 0 };
	//char *broadcast_str = NULL;

	const char *tmp_reason_str = g_GetPutReasonTypeStr(reason_type);
	STRNCPY(put_reason_str, tmp_reason_str, sizeof(put_reason_str));

	//监控非绑物品获得数量？感觉不必要
	//if (!item_info.is_bind) m_module_mgr->GetMonitor()->AddMonitorNum(MONITOR_TYPE_DAY_NOBIND_ITEM_NUM, item_info.num);

	gamelog::g_log_knapsack.printf(LL_INFO, "[Knapsack::CertainPut Succ][user[%s %d] item_id:%d item_name:%s(%d) color:%d is_net_value_item:%d put_num:%d reason:%s drop_monster_id:%d put_index:%d]",
		m_module_mgr->GetRole()->GetName(), UidToInt(m_module_mgr->GetRole()->GetUserId()), (int)item_info.item_id, itembase->GetItemName(), (int)item_info.is_bind, (int)itembase->GetColor(), (int)is_net_value_item, (int)item_info.num,
		put_reason_str, (int)drop_monster_id, real_put_index_list[0]);
	if (itembase->IsRecord())
	{
		char log_quick_str[64] = { 0 };
		SNPRINTF(log_quick_str, sizeof(log_quick_str), "item_put_%d", (int)reason_type);
		ROLE_LOG_QUICK6(LOG_TYPE_ITEM_PUT, m_module_mgr->GetRole(), item_info.item_id, item_info.num, log_quick_str, NULL);
	}

	EventHandler::Instance().OnGetItem(m_module_mgr->GetRole(), item_info.item_id, item_info.num, reason_type);
}

void Knapsack::CertainPut(const ItemDataWrapper &item_info, int reason_type, int* out_column, int *out_index, UInt16 drop_monster_id /* = 0 */)
{
	const ItemBase *itembase = ITEMPOOL->GetItem(item_info.item_id);
	if (NULL == itembase) return;

	this->CertainPut(itembase->GetItemColumn(), item_info, reason_type, out_column, out_index, drop_monster_id);
}

bool Knapsack::GetListOfItem(ItemID item_id, short max_num,
	short *bind_column_list, short *bind_index_list, short *bind_num_list, short *bind_count,
	short *nobind_column_list, short *nobind_index_list, short *nobind_num_list, short *nobind_count)
{
	if (item_id <= 0 || max_num <= 0 ||
		NULL == bind_index_list || NULL == bind_num_list || NULL == bind_count ||
		NULL == nobind_index_list || NULL == nobind_num_list || NULL == nobind_count)
	{
		return false;
	}

	const ItemBase* item_base = ITEMPOOL->GetItem(item_id);
	if (NULL == item_base)
	{
		return false;
	}
	int column = item_base->GetItemColumn();

	*bind_count = *nobind_count = 0;

	short tmp_bind_count = 0, tmp_nobind_count = 0;

	int valid_grid_index_end = m_valid_grid_index_end_list[column];
	for (int i = 0; i < valid_grid_index_end; ++i)
	{
		if (tmp_bind_count >= max_num || tmp_nobind_count >= max_num)
		{
			return false;
		}

		if (NULL != m_grid_list[column][i] && m_grid_list[column][i]->item_id == item_id)
		{
			if (m_grid_list[column][i]->is_bind)
			{
				bind_column_list[tmp_bind_count] = column;
				bind_index_list[tmp_bind_count] = i;
				bind_num_list[tmp_bind_count] = m_grid_list[column][i]->num;
				++tmp_bind_count;
			}
			else
			{
				nobind_column_list[tmp_nobind_count] = column;
				nobind_index_list[tmp_nobind_count] = i;
				nobind_num_list[tmp_nobind_count] = m_grid_list[column][i]->num;
				++tmp_nobind_count;
			}
		}
	}

	*bind_count = tmp_bind_count;
	*nobind_count = tmp_nobind_count;

	return true;
}

int Knapsack::GetGridIndex(ItemID item_id)
{
	const ItemBase* item_base = ITEMPOOL->GetItem(item_id);
	if (NULL == item_base) return -1;

	int column = item_base->GetItemColumn();
	int curr_valid_grid_num = m_curr_valid_grid_num_list[column];
	for (int index = 0; index < curr_valid_grid_num; ++index)
	{
		if (NULL != m_grid_list[column][index] && m_grid_list[column][index]->item_id == item_id)
		{
			return index;
		}
	}
	return -1;
}

bool Knapsack::GetGridColumnIndex(ItemID item_id, int & out_column, int & out_index)
{
	const ItemBase * item_base = ITEMPOOL->GetItem(item_id);
	if (NULL == item_base) return false;

	int column = item_base->GetItemColumn();
	int curr_valid_grid_num = m_curr_valid_grid_num_list[column];
	for (int index = 0; index < curr_valid_grid_num; ++index)
	{
		if (NULL != m_grid_list[column][index] && m_grid_list[column][index]->item_id == item_id)
		{
			out_column = column;
			out_index = index;
			return true;
		}
	}

	return false;
}

void Knapsack::NotifyGetItemList(short list_num, const ItemConfigData *item_configdata_list, int get_reson, int param1, int param2)
{
	static Protocol::SCItemIntoKnpackSpecialProtocal iiksp;

	iiksp.get_reson = get_reson;
	iiksp.param1 = param1;
	iiksp.param2 = param2;

	int item_num = 0;
	for (int i = 0; item_configdata_list != NULL && i < list_num && i < Protocol::SCItemIntoKnpackSpecialProtocal::MAX_FETCH_ITEM_NUM; ++i)
	{
		iiksp.item_list[item_num].Reset();

		iiksp.item_list[item_num].item_id = item_configdata_list[i].item_id;
		iiksp.item_list[item_num].is_bind = item_configdata_list[i].is_bind;
		iiksp.item_list[item_num].count = item_configdata_list[i].num;

		const ItemBase* item = ITEMPOOL->GetItem(item_configdata_list[i].item_id);
		if (NULL != item && item->GetItemType() == I_TYPE_CONVERT_ITEM)
		{
			int new_item_id = LOGIC_CONFIG->GetKnapsackConfig()->GetItemIdConvertByJob(item_configdata_list[i].item_id, m_module_mgr->GetRole()->GetAppearanceProf());
			if (new_item_id > 0)
			{
				iiksp.item_list[item_num].item_id = new_item_id;
			}
			else
			{
				continue;
			}
		}

		item_num += 1;
	}

	iiksp.count = item_num;

	EngineAdapter::Instance().NetSendSerializeMsg2(m_module_mgr->GetRole()->GetNetId(), &iiksp);
}

void Knapsack::NotifyGetItemListVec(const std::vector<ItemConfigData> & list, int get_reson, int param1, int param2)
{
	if (list.empty())
	{
		return;
	}

	this->NotifyGetItemList((short)list.size(), &list[0], get_reson, param1, param2);
}

void Knapsack::NotifyGetItemList3(int add_gold, int add_coin_bind, int add_exp, short list_num, const ItemConfigData *item_configdata_list, int get_reson /*= 0*/, int param1 /*= 0*/)
{
	Protocol::SCItemIntoKnpackSpecialProtocal iiksp;

	iiksp.get_reson = get_reson;
	iiksp.param1 = param1;

	int item_num = 0;
	for (int i = 0; item_configdata_list != NULL && i < list_num && i < Protocol::SCItemIntoKnpackSpecialProtocal::MAX_FETCH_ITEM_NUM; ++i)
	{
		iiksp.item_list[item_num].Reset();

		iiksp.item_list[item_num].item_id = item_configdata_list[i].item_id;
		iiksp.item_list[item_num].is_bind = item_configdata_list[i].is_bind;
		iiksp.item_list[item_num].count = item_configdata_list[i].num;

		const ItemBase* item = ITEMPOOL->GetItem(item_configdata_list[i].item_id);
		if (NULL != item && item->GetItemType() == I_TYPE_CONVERT_ITEM)
		{
			int new_item_id = LOGIC_CONFIG->GetKnapsackConfig()->GetItemIdConvertByJob(item_configdata_list[i].item_id, m_module_mgr->GetRole()->GetAppearanceProf());
			if (new_item_id > 0)
			{
				iiksp.item_list[i].item_id = new_item_id;
			}
			else
			{
				continue;
			}
		}

		item_num += 1;
	}

	if (add_gold > 0 && item_num < Protocol::SCItemIntoKnpackSpecialProtocal::MAX_FETCH_ITEM_NUM)
	{
		iiksp.item_list[item_num].item_id = GOLD_ITEM_ID;
		iiksp.item_list[item_num].count = add_gold;
		iiksp.item_list[item_num].is_bind = 0;
		iiksp.item_list[item_num].has_param = 0;
		iiksp.item_list[item_num].param_length = 0;
		item_num += 1;
	}

	if (add_coin_bind > 0 && item_num < Protocol::SCItemIntoKnpackSpecialProtocal::MAX_FETCH_ITEM_NUM)
	{
		iiksp.item_list[item_num].item_id = COIN_ITEM_ID;
		iiksp.item_list[item_num].count = add_coin_bind;
		iiksp.item_list[item_num].is_bind = 0;
		iiksp.item_list[item_num].has_param = 0;
		iiksp.item_list[item_num].param_length = 0;
		item_num += 1;
	}

	if (add_exp > 0 && item_num < Protocol::SCItemIntoKnpackSpecialProtocal::MAX_FETCH_ITEM_NUM)
	{
		iiksp.item_list[item_num].item_id = EXP_ITEM_ID;
		iiksp.item_list[item_num].count = add_exp;
		iiksp.item_list[item_num].is_bind = 0;
		iiksp.item_list[item_num].has_param = 0;
		iiksp.item_list[item_num].param_length = 0;
		item_num += 1;
	}

	iiksp.count = item_num;

	EngineAdapter::Instance().NetSendSerializeMsg2(m_module_mgr->GetRole()->GetNetId(), &iiksp);
}

bool Knapsack::GetRealIndexColumn(int index, int *out_real_column, int *out_real_index)
{
	if (index < 0) return false;

	if (NULL != out_real_index) *out_real_index = index % ITEM_COLUMN_GRID_UPBOUND;
	if (NULL != out_real_column) *out_real_column = index / ITEM_COLUMN_GRID_UPBOUND;

	return true;
}

int Knapsack::GetIndexByRealIndexColumn(int real_column, int real_index)
{
	if (real_index < 0 || real_index >= ITEM_COLUMN_GRID_UPBOUND) return 0;
	if (real_column < ITEM_COLUMN_TYPE_MIN || real_column >= ITEM_COLUMN_TYPE_MAX) return 0;

	return real_column * ITEM_COLUMN_GRID_UPBOUND + real_index;
}

bool Knapsack::CertainMoveHelper(int from_column, int from_index, int to_column, int offset_index /* = 0 */, int range /* = 0 */)
{
	if (from_column < ITEM_COLUMN_TYPE_MIN || from_column >= ITEM_COLUMN_TYPE_MAX) return false;
	if (to_column < ITEM_COLUMN_TYPE_MIN || to_column >= ITEM_COLUMN_TYPE_MAX) return false;
	if (from_index < 0 || from_index >= ITEM_COLUMN_GRID_NUM_MAX) return false;

	if (from_column < ITEM_COLUMN_TYPE_NORMAL_COLUMN_END && to_column != ITEM_COLUMN_TYPE_STORAGE)
	{
		// 只能把背包物品放入仓库，而不能是临时背包
		return false;
	}

	if (from_column >= ITEM_COLUMN_TYPE_NORMAL_COLUMN_END && to_column >= ITEM_COLUMN_TYPE_NORMAL_COLUMN_END)
	{
		// 临时背包和仓库的物品不能互相放入
		return false;
	}

	if (NULL == m_grid_list[from_column][from_index] || m_grid_list[from_column][from_index]->Invalid())
	{
		// 源格子没有物品
		return false;
	}

	const ItemBase* item_base = ITEMPOOL->GetItem(m_grid_list[from_column][from_index]->item_id);
	if (NULL == item_base)
	{
		return false;
	}
	if (to_column < ITEM_COLUMN_TYPE_NORMAL_COLUMN_END && to_column != item_base->GetItemColumn())
	{
		// column出错
		return false;
	}

	if (offset_index < 0 || offset_index >= ITEM_COLUMN_GRID_NUM_MAX)
	{
		return false;
	}

	int valid_grid_index_end = m_valid_grid_index_end_list[to_column];
	int curr_valid_grid_num = m_curr_valid_grid_num_list[to_column];
	if (valid_grid_index_end > curr_valid_grid_num || curr_valid_grid_num > ITEM_COLUMN_GRID_NUM_MAX) return false;

	int check_index_start = offset_index;
	int check_index_end = offset_index + range;
	if (check_index_end > ITEM_COLUMN_GRID_NUM_MAX)
	{
		check_index_end = ITEM_COLUMN_GRID_NUM_MAX;
	}

	// 以下实现类似CertainPut的放入逻辑，先Merge再找空位

	static ItemDataWrapper tmp_item_wrapper;
	tmp_item_wrapper.Reset();
	m_grid_list[from_column][from_index]->WrapTo(&tmp_item_wrapper);

#ifdef TRACK_ITEM_FLAG
	gamelog::g_log_item_track.printf(LL_INFO, "TRACK_ITEM_FLAG Knapsack::CertainMoveHelper move_item_to_column[%d] CLEAR_GRID role[%d,%s] item[%d] unique_id[%lld]",
		to_column, m_module_mgr->GetRole()->GetUID(), m_module_mgr->GetRole()->GetName(), m_grid_list[from_column][from_index]->item_id, m_grid_list[from_column][from_index]->item_unique_id);
#endif

	this->ClearGrid(from_column, from_index);

	this->RefreshValidGridIndexEnd(from_column, from_index);
	this->SetItemDirty(from_column, from_index, true, ITEM_CHANGE_NOTICE_REASON_MOVE);

	bool is_net_value_item = (item_base->GetItemType() >= I_TYPE_NET_VALUE_ITEM_BEGIN);
	int empty_index = -1;

	// 1. 从指定范围中寻找可以堆叠或有空位的地方
	if (range > 0)
	{
		for (int i = check_index_start; i < check_index_end; ++i)
		{
			if (is_net_value_item) // 净值物品只要找到第一个空位置即可
			{
				if (NULL == m_grid_list[to_column][i] || m_grid_list[to_column][i]->Invalid())
				{
					empty_index = i;
					break;
				}
			}
			else
			{
				if (NULL == m_grid_list[to_column][i] || m_grid_list[to_column][i]->Invalid())
				{
					if (-1 == empty_index) empty_index = i;
				}
				else if (m_grid_list[to_column][i]->CanBeMerge(true)) // 下面Merge里会去对比invalid_time
				{
					int rest_num = 0;
					int merged_gold_price = 0;
					int merge_result = m_grid_list[to_column][i]->Merge(tmp_item_wrapper, &rest_num, &merged_gold_price);
					if (ITEM_MERGE_COMPLETE == merge_result)
					{
						// 放完了
						tmp_item_wrapper.num = 0;
						tmp_item_wrapper.gold_price = 0;

						this->SetItemDirty(to_column, i, true, ITEM_CHANGE_NOTICE_REASON_MOVE);
						break;
					}
					else if (ITEM_MERGE_NOT_COMPLETE == merge_result)
					{
						tmp_item_wrapper.num = rest_num;
						tmp_item_wrapper.gold_price -= merged_gold_price;

						this->SetItemDirty(to_column, i, true, ITEM_CHANGE_NOTICE_REASON_MOVE);
					}
				}
			}
		}
	}

	//2. 如果找不到空位且没有办法堆叠，则从整个背包栏中找空位或堆叠
	if (-1 == empty_index && tmp_item_wrapper.num > 0)
	{
		for (int i = 0; i < valid_grid_index_end; ++i)
		{
			if (i >= check_index_start && i < check_index_end)
			{
				// 该范围内已经查找过，所以直接continue;
				continue;
			}

			if (is_net_value_item) // 净值物品只要找到第一个空位置即可
			{
				if (NULL == m_grid_list[to_column][i] || m_grid_list[to_column][i]->Invalid())
				{
					empty_index = i;
					break;
				}
			}
			else
			{
				if (NULL == m_grid_list[to_column][i] || m_grid_list[to_column][i]->Invalid())
				{
					if (-1 == empty_index) empty_index = i;
				}
				else if (m_grid_list[to_column][i]->CanBeMerge(true)) // 下面Merge里会去对比invalid_time
				{
					int rest_num = 0;
					int merged_gold_price = 0;
					int merge_result = m_grid_list[to_column][i]->Merge(tmp_item_wrapper, &rest_num, &merged_gold_price);
					if (ITEM_MERGE_COMPLETE == merge_result)
					{
						// 放完了
						tmp_item_wrapper.num = 0;
						tmp_item_wrapper.gold_price = 0;

						this->SetItemDirty(to_column, i, true, ITEM_CHANGE_NOTICE_REASON_MOVE);
						break;
					}
					else if (ITEM_MERGE_NOT_COMPLETE == merge_result)
					{
						tmp_item_wrapper.num = rest_num;
						tmp_item_wrapper.gold_price -= merged_gold_price;

						this->SetItemDirty(to_column, i, true, ITEM_CHANGE_NOTICE_REASON_MOVE);
					}
				}
			}
		}
	}

	//3. 没有堆叠完，则尝试直接放到空位里
	if (tmp_item_wrapper.num > 0)
	{
		if (-1 == empty_index && valid_grid_index_end < curr_valid_grid_num) // 前面全满 则放到下一个空位里
		{
			empty_index = valid_grid_index_end;
		}

		if (empty_index < 0 || empty_index >= ITEM_COLUMN_GRID_NUM_MAX)
		{
			gamelog::g_log_knapsack.printf(LL_ERROR, "Knapsack::CertainMoveHelper can't find empty index, posible lost item! role[%d,%s], item[%d,%d,%d], empty_index[%d]",
				m_module_mgr->GetRole()->GetUID(), m_module_mgr->GetRole()->GetName(), tmp_item_wrapper.item_id, tmp_item_wrapper.num, tmp_item_wrapper.is_bind, empty_index);
			return false;
		}

		if (NULL == m_grid_list[to_column][empty_index])
		{
			m_grid_list[to_column][empty_index] = new ItemGridData();
		}

		// 安全起见不直接调用Set(const ItemGridData &data)函数，因为该函数没有做深拷贝
		m_grid_list[to_column][empty_index]->Set(tmp_item_wrapper);

#ifdef TRACK_ITEM_FLAG
		gamelog::g_log_item_track.printf(LL_INFO, "TRACK_ITEM_FLAG Knapsack::CertainMoveHelper put_remain_item role[%d,%s] column[%d] index[%d] item_id[%d] num[%d] unique_id[%lld]",
			m_module_mgr->GetRole()->GetUID(), m_module_mgr->GetRole()->GetName(),
			to_column, empty_index, tmp_item_wrapper.item_id, tmp_item_wrapper.num, tmp_item_wrapper.item_unique_id);
#endif

		this->RefreshValidGridIndexEnd(to_column, empty_index);
		this->SetItemDirty(to_column, empty_index, true, ITEM_CHANGE_NOTICE_REASON_MOVE);
	}

	return true;
}

void Knapsack::ClearGrid(int column, int index)
{
	// column 和 index的合法性由外部保证

	if (NULL == m_grid_list[column][index]) return;

	m_grid_list[column][index]->Clear();
	delete m_grid_list[column][index];
	m_grid_list[column][index] = NULL;
}

void Knapsack::SendNoSpaceNotice(int column)
{
	int errcode = errornum::EN_KNAPSACK_NO_SPACE;
	switch (column)
	{
	case ITEM_COLUMN_TYPE_ITEM:
		errcode = errornum::EN_KNAPSACK_ITEM_COLUMN_NO_SPACE;
		break;

	case ITEM_COLUMN_TYPE_SUPPLY:
		errcode = errornum::EN_KNAPSACK_SUPPLY_COLUMN_NO_SPACE;
		break;

	case ITEM_COLUMN_TYPE_TASK_MATERIAL:
		errcode = errornum::EN_KNAPSACK_TASK_MATERIAL_COLUMN_NO_SPACE;
		break;

	case ITEM_COLUMN_TYPE_FRAGMENT:
		errcode = errornum::EN_KNAPSACK_FRAGMENT_COLUMN_NO_SPACE;
		break;

	case ITEM_COLUMN_TYPE_ILLUSTRATION:
		errcode = errornum::EN_KNAPSACK_ILLUSTRATION_COLUMN_NO_SPACE;
		break;

	case ITEM_COLUMN_TYPE_STORAGE:
		errcode = errornum::EN_KNAPSACK_STORAGE_COLUMN_NO_SPACE;
		break;

	case ITEM_COLUMN_TYPE_TEMPORARY:
		errcode = errornum::EN_KNAPSACK_TEMPERARY_COLUMN_NO_SPACE;
		break;

	case ITEM_COLUMN_TYPE_PET_SOUL_EQIOP:
		errcode = errornum::EN_KNAPSACK_PET_SOUL_EQUIP_COLUMN_NO_SPACE;
		break;

	case ITEM_COLUMN_TYPE_ACCESSORIES:
		errcode = errornum::EN_KNAPSACK_ACCESSORIES_COLUMN_NO_SPACE;
		break;
	}
	UNSTD_STATIC_CHECK(ITEM_COLUMN_TYPE_MAX == 9);

	m_module_mgr->NoticeNum(errcode);
}

void Knapsack::AddNoSpaceErrorNotice(int column)
{
	int errcode = errornum::EN_KNAPSACK_NO_SPACE;
	switch (column)
	{
	case ITEM_COLUMN_TYPE_ITEM:
		errcode = errornum::EN_KNAPSACK_ITEM_COLUMN_NO_SPACE;
		break;

	case ITEM_COLUMN_TYPE_SUPPLY:
		errcode = errornum::EN_KNAPSACK_SUPPLY_COLUMN_NO_SPACE;
		break;

	case ITEM_COLUMN_TYPE_FRAGMENT:
		errcode = errornum::EN_KNAPSACK_FRAGMENT_COLUMN_NO_SPACE;
		break;

	case ITEM_COLUMN_TYPE_TASK_MATERIAL:
		errcode = errornum::EN_KNAPSACK_TASK_MATERIAL_COLUMN_NO_SPACE;
		break;

	case ITEM_COLUMN_TYPE_ILLUSTRATION:
		errcode = errornum::EN_KNAPSACK_ILLUSTRATION_COLUMN_NO_SPACE;
		break;

	case ITEM_COLUMN_TYPE_STORAGE:
		errcode = errornum::EN_KNAPSACK_STORAGE_COLUMN_NO_SPACE;
		break;

	case ITEM_COLUMN_TYPE_TEMPORARY:
		errcode = errornum::EN_KNAPSACK_TEMPERARY_COLUMN_NO_SPACE;
		break;

	case ITEM_COLUMN_TYPE_PET_SOUL_EQIOP:
		errcode = errornum::EN_KNAPSACK_PET_SOUL_EQUIP_COLUMN_NO_SPACE;
		break;

	case ITEM_COLUMN_TYPE_ACCESSORIES:
		errcode = errornum::EN_KNAPSACK_ACCESSORIES_COLUMN_NO_SPACE;
		break;
	}
	UNSTD_STATIC_CHECK(ITEM_COLUMN_TYPE_MAX == 9);

	this->AddErrorNotice(errcode);
}

void Knapsack::AddErrorNotice(int error_num)
{
	m_error_notice.push_back(error_num);
}

void Knapsack::ClearErrorNotice()
{
	m_error_notice.clear();
}

void Knapsack::CheckItemInWrongColumn()
{
	for (int column = ITEM_COLUMN_TYPE_NORMAL_COLUMN_END - 1; column >= ITEM_COLUMN_TYPE_MIN; --column)
	{
		if (ITEM_COLUMN_TYPE_STORAGE == column || ITEM_COLUMN_TYPE_TEMPORARY == column) continue;

		int valid_grid_index_end = m_valid_grid_index_end_list[column];
		for (int i = 0; i < valid_grid_index_end; i++)
		{
			if (NULL == m_grid_list[column][i] || m_grid_list[column][i]->Invalid()) continue;

			const ItemBase *item = ITEMPOOL->GetItem(m_grid_list[column][i]->item_id);
			if (!item) continue;
			if (item->GetItemColumn() == column) continue;

			// 放错栏的物品，消耗掉，并放到正确的栏
			ItemDataWrapper wrapper;
			m_grid_list[column][i]->WrapTo(&wrapper);
			NetValueItemParam param;
			param.SetData(wrapper.param_data);

			if (!this->ConsumeItemByIndex(column, i, m_grid_list[column][i]->num, "Knapsack::CheckItemInWrongColumn"))
			{
				gamelog::g_log_knapsack.printf(LL_ERROR, "Knapsack::CheckItemInWrongColumn role[%d,%s] consume failed! column[%d] item_param[%s]",
					m_module_mgr->GetUid(), m_module_mgr->GetRoleName(), column, param.GetParamLog(item->GetItemType()));
				continue;
			}

			if (!this->Put(wrapper, PUT_REASON_TRANSFER_KNAPSACK_COLUMN))
			{
				static MailContentParam contentparam;
				int length1 = 0;
				int length2 = 0;

				//发放邮件
				contentparam.Reset();
				length1 = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_transfer_subject);
				length2 = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_transfer_content);
				contentparam.item_list[0] = wrapper;
				if (length1 > 0 && length2 > 0)
				{
					MailRoute::MailToUser(m_module_mgr->GetRole()->GetUserId(), MAIL_REASON_DEFAULT, contentparam, MAIL_TYPE_SYSTEM);
					gamelog::g_log_knapsack.printf(LL_INFO, "Knapsack::CheckItemInWrongColumn role[%d,%s] put fail send mail! column[%d], item_param[%s]",
						m_module_mgr->GetUid(), m_module_mgr->GetRoleName(), column, param.GetParamLog(item->GetItemType()));
				}
				else
				{
					gamelog::g_log_knapsack.printf(LL_ERROR, "Knapsack::CheckItemInWrongColumn role[%d,%s] put fail send mail and FAILED!! column[%d], item_param[%s]",
						m_module_mgr->GetUid(), m_module_mgr->GetRoleName(), column, param.GetParamLog(item->GetItemType()));
				}
			}
			else
			{
				gamelog::g_log_knapsack.printf(LL_ERROR, "Knapsack::CheckItemInWrongColumn role[%d,%s] put succ!! column[%d], item_param[%s]",
					m_module_mgr->GetUid(), m_module_mgr->GetRoleName(), column, param.GetParamLog(item->GetItemType()));
			}
		}
	}
}

void Knapsack::CheckColumnInitialGridNum()
{
	if (2 != m_init_flag)
	{
		m_init_flag = 2;

		for (int i = ITEM_COLUMN_TYPE_MIN; i < ITEM_COLUMN_TYPE_MAX; ++i)
		{
			// 取较大者
			const KnapsackGridCfg* cfg = LOGIC_CONFIG->GetKnapsackConfig()->GetKnapsackGridCfg(i);
			if (NULL != cfg)
			{
				m_curr_valid_grid_num_list[i] =
					(cfg->start_grid_num < m_curr_valid_grid_num_list[i] ? m_curr_valid_grid_num_list[i] : cfg->start_grid_num);
			}
		}
	}
}

void Knapsack::CheckEquipZhuLing()
{
	for (int column = ITEM_COLUMN_TYPE_NORMAL_COLUMN_END - 1; column >= ITEM_COLUMN_TYPE_MIN; --column)
	{
		if (ITEM_COLUMN_TYPE_STORAGE == column || ITEM_COLUMN_TYPE_TEMPORARY == column) continue;

		int valid_grid_index_end = m_valid_grid_index_end_list[column];
		for (int i = 0; i < valid_grid_index_end; i++)
		{
			ItemGridData * curr = m_grid_list[column][i];
			if (NULL == curr || curr->Invalid()) continue;

			const ItemBase * item = ITEMPOOL->GetItem(curr->item_id);
			if (!item) continue;

			ZhuLinParam(*zhu_ling_info)[EQUIP_ZHU_LING_SLOT_NUM] = NULL;
			if (I_TYPE_EQUIPMENT == item->GetItemType())
			{
				EquipmentParam & p_ep = curr->param->equip_param;
				zhu_ling_info = &p_ep.zhu_ling_info;
			}

			if (I_TYPE_JEWELRY == item->GetItemType())
			{
				JewelryParam & p_ep = curr->param->jewelry_param;
				zhu_ling_info = &p_ep.zhu_ling_info;
			}

			if (!zhu_ling_info)
			{
				continue;
			}

			bool set_dirty = false;
			for (int zhu_ling_idx = 0; zhu_ling_idx < ARRAY_LENGTH(*zhu_ling_info); ++zhu_ling_idx)
			{
				ZhuLinParam & zhuling = (*zhu_ling_info)[zhu_ling_idx];
				if (zhuling.zhu_ling_item_id != 0)
				{
					const ItemBase * zhu_ling_item = ITEMPOOL->GetItem(zhuling.zhu_ling_item_id);
					if (zhu_ling_item == NULL || zhu_ling_item->GetItemType() != I_TYPE_ZHU_LING)
					{
						gamelog::g_log_zhu_ling.printf(LL_INFO, "%s line:%d | role[%d %s]  eq_item_id:%d uuid:%lld slot_index:%d zhu_ling_item_id[%d to 0] ", __FUNCTION__, __LINE__, m_module_mgr->GetRole()->GetUID(), m_module_mgr->GetRole()->GetName(),
									       curr->item_id, curr->item_unique_id, zhu_ling_idx, zhuling.zhu_ling_item_id);
						zhuling.zhu_ling_item_id = 0;
						set_dirty = true;
					}
				}
			}

			if (set_dirty)
			{
				this->SetItemDirty(column, i, false, 0, 0);
			}
		}
	}
}

bool Knapsack::PutExpenseItemAutoUse(const ItemBase* itembase, int num, bool is_bind, unsigned int invalid_time, bool is_notice)
{
	if (NULL == itembase || itembase->GetItemType() != I_TYPE_EXPENSE || num <= 0) return false;

	const NorexItem * norex_item = (NorexItem *)itembase;
	if (norex_item->GetUseType() == NorexItem::I_NOREX_ITEM_SURFACE)
	{
		if (m_module_mgr->GetSurface()->AutoDecomposeActivatedSur(norex_item, num, is_notice))
		{
			return true;
		}
	}
	else if (norex_item->GetUseType() == NorexItem::I_NOREX_ITEM_FACESROE_RES || norex_item->GetUseType() == NorexItem::I_NOREX_ITEM_TITLE_CATD)
	{
		if (m_module_mgr->GetFaceScore()->AutoOrUseActiveRes(norex_item->GetParam1(), itembase->GetItemId()))
		{
			return true;
		}
	}
	else if (norex_item->GetUseType() == NorexItem::I_NOREX_ITEM_WORKSHOP_RECIPE)
	{
		if (m_module_mgr->GetRoleDrop()->IsLockReceipeDrop(itembase->GetItemId()))
		{
			return false;
		}
		if (m_module_mgr->GetWorkshop()->AutoActiveNewRecipe(norex_item->GetParam1(), itembase->GetItemId(), true, is_notice))
		{
			return true;
		}
	}
	else if (norex_item->GetUseType() == NorexItem::I_NOREX_ITEM_PET_SKIN)
	{
		if (m_module_mgr->GetPet()->ActivityPetSkin(norex_item->GetParam1(), num, is_notice))
		{
			return true;
		}
	}
	else if (norex_item->GetUseType() == NorexItem::I_NOREX_ITEM_AUTO_QIYUN)
	{
		if (m_module_mgr->GetMoney()->AddOtherCoin(norex_item->GetParam1() * num, MONEY_TYPE_LUCK_COIN, "Norexitem::Put"))
		{
			return true;
		}
	}
	else if (norex_item->GetUseType() == NorexItem::I_NOREX_ITEM_MONEY_AUTO_USE)
	{
		if (m_module_mgr->GetMoney()->OnAddMoney(norex_item->GetParam1(), norex_item->GetParam2() * num, __FUNCTION__))
		{
			return true;
		}
	}
	else if (norex_item->GetUseType() == NorexItem::I_NOREX_ITEM_LIMITED_TIME_TITLE_CARD)
	{
		if (m_module_mgr->GetTitle()->AddTitleOrRenewDueTime(norex_item->GetParam2(), __FUNCTION__, invalid_time))
		{
			return true;
		}
	}

	return false;
}

void Knapsack::SendErrorNotice()
{
	for (size_t i = 0; i < m_error_notice.size(); ++i)
	{
		m_module_mgr->NoticeNum(m_error_notice[i]);
	}
	m_error_notice.clear();
}

void Knapsack::GodEquipReBack()
{
	for (int index = 0; index < ItemNamespace::ITEM_COLUMN_GRID_NUM_MAX; ++index)
	{
		{//检索物品背包
			ItemGridData* grid = m_grid_list[ItemNamespace::ITEM_COLUMN_TYPE_ITEM][index];
			if (NULL == grid || grid->Invalid()) continue;

			m_module_mgr->GetWorkshop()->CheckGodEquipItemDataReturn(grid);
		}
	}

	for (int index = 0; index < ItemNamespace::ITEM_COLUMN_GRID_NUM_MAX; ++index)
	{
		{//检索仓库
			ItemGridData* grid = m_grid_list[ItemNamespace::ITEM_COLUMN_TYPE_STORAGE][index];
			if (NULL == grid || grid->Invalid()) continue;

			m_module_mgr->GetWorkshop()->CheckGodEquipItemDataReturn(grid);
		}
	}
}

void Knapsack::GmCheckItemInWrongColumn()
{
	this->CheckItemInWrongColumn();
}

void Knapsack::CheckPetSoulCollection(bool need_sync)
{
	for (int index = 0;
		index < ItemNamespace::ITEM_COLUMN_GRID_NUM_MAX
		&& index < m_curr_valid_grid_num_list[ItemNamespace::ITEM_COLUMN_TYPE_PET_SOUL_EQIOP];
		index++)
	{
		if (!m_old_state[ItemNamespace::ITEM_COLUMN_TYPE_PET_SOUL_EQIOP][index])
		{
			continue;
		}

		if (NULL == m_grid_list[ItemNamespace::ITEM_COLUMN_TYPE_PET_SOUL_EQIOP][index]
			|| m_grid_list[ItemNamespace::ITEM_COLUMN_TYPE_PET_SOUL_EQIOP][index]->Invalid())
		{
			continue;
		}

		EventHandler::Instance().OnGetPetSoul(m_module_mgr->GetRole(),
			m_grid_list[ItemNamespace::ITEM_COLUMN_TYPE_PET_SOUL_EQIOP][index]->item_id, need_sync);

	}
}

void Knapsack::ChangeNewSuit()
{
	for (int index = 0; index < ItemNamespace::ITEM_COLUMN_GRID_NUM_MAX; ++index)
	{
		{//检索物品背包
			ItemGridData* grid = m_grid_list[ItemNamespace::ITEM_COLUMN_TYPE_ITEM][index];
			if (NULL == grid || grid->Invalid()) continue;

			const ItemBase * i_base = ITEMPOOL->GetItem(grid->item_id);
			if (i_base == NULL)continue;

			if (i_base->GetItemType() == I_TYPE_EQUIPMENT)
			{
				EquipmentParam & p_ep = grid->param->equip_param;
				const Equipment * check_suit = (const Equipment *)i_base;
				if (check_suit->GetSuitRandomGroup() != p_ep.new_suit_effect_id)
				{
					p_ep.new_suit_effect_id = check_suit->GetSuitRandomGroup();
				}
			}
			else if (i_base->GetItemType() == I_TYPE_JEWELRY)
			{
				JewelryParam & p_ep = grid->param->jewelry_param;
				const Jewelry * check_suit = (const Jewelry *)i_base;
				if (check_suit->GetSuitRandomGroup() != p_ep.new_suit_effect_id)
				{
					p_ep.new_suit_effect_id = check_suit->GetSuitRandomGroup();
				}
			}
		}
	}

	for (int index = 0; index < ItemNamespace::ITEM_COLUMN_GRID_NUM_MAX; ++index)
	{
		{//检索仓库
			ItemGridData* grid = m_grid_list[ItemNamespace::ITEM_COLUMN_TYPE_STORAGE][index];
			if (NULL == grid || grid->Invalid()) continue;

			const ItemBase * i_base = ITEMPOOL->GetItem(grid->item_id);
			if (i_base == NULL)continue;

			if (i_base->GetItemType() == I_TYPE_EQUIPMENT)
			{
				EquipmentParam & p_ep = grid->param->equip_param;
				const Equipment * check_suit = (const Equipment *)i_base;
				if (check_suit->GetSuitRandomGroup() != p_ep.new_suit_effect_id)
				{
					p_ep.new_suit_effect_id = check_suit->GetSuitRandomGroup();
				}
			}
			else if (i_base->GetItemType() == I_TYPE_JEWELRY)
			{
				JewelryParam & p_ep = grid->param->jewelry_param;
				const Jewelry * check_suit = (const Jewelry *)i_base;
				if (check_suit->GetSuitRandomGroup() != p_ep.new_suit_effect_id)
				{
					p_ep.new_suit_effect_id = check_suit->GetSuitRandomGroup();
				}
			}
		}
	}
}

void Knapsack::ChangeNewVocEatra()
{
	for (int index = 0; index < ItemNamespace::ITEM_COLUMN_GRID_NUM_MAX; ++index)
	{
		{//检索物品背包
			ItemGridData* grid = m_grid_list[ItemNamespace::ITEM_COLUMN_TYPE_ITEM][index];
			if (NULL == grid || grid->Invalid()) continue;

			const ItemBase * i_base = ITEMPOOL->GetItem(grid->item_id);
			if (i_base == NULL)continue;

			if (i_base->GetItemType() == I_TYPE_EQUIPMENT)
			{
				EquipmentParam & p_ep = grid->param->equip_param;
				const Equipment * e_b = (const Equipment *)i_base;
				for (int i = 0; i < p_ep.inc_num && i < EquipmentParam::EXA_ATTR_NUM_MAX; i++)
				{
					AttrVocInfo &node_attr = p_ep.attr_extra_inc[i];
					const AttValueCfg * old_cfg = LOGIC_CONFIG->GetEntryConfig()->GetEntryAttrValueConfigBack(e_b->GetEquipLevel(), node_attr.attr_type, node_attr.attr_value);
					if (NULL != old_cfg)//有符合的配置
					{
						//计算数值占用旧范围百分比
						double attr_persnt = ((double)(node_attr.attr_value - old_cfg->min) / (old_cfg->max - old_cfg->min));
						const AttValueCfg * new_cfg = LOGIC_CONFIG->GetEntryConfig()->GetEntryAttrValueConfig3(e_b->GetEquipLevel(), old_cfg->quality, node_attr.attr_type);
						if (new_cfg != NULL)
						{
							int new_value = (int)((new_cfg->max - new_cfg->min) * attr_persnt) + new_cfg->min;
							gamelog::g_log_knapsack.printf(LL_INFO, "%s equipment ITEM_COLUMN_TYPE[%d] index[%d] user[%s,%d] voc_idx[%d] voc[attr_type:%d] [old_value:%d] [new_attr_value:%d]", __FUNCTION__,
								ItemNamespace::ITEM_COLUMN_TYPE_ITEM, index, m_module_mgr->GetRoleName(), m_module_mgr->GetUid(), i, node_attr.attr_type, node_attr.attr_value, new_value);
							node_attr.attr_value = new_value;
						}
					}
				}
			}
			else if (i_base->GetItemType() == I_TYPE_JEWELRY)
			{
				JewelryParam & p_ep = grid->param->jewelry_param;
				const Jewelry * j_b = (const Jewelry *)i_base;
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
							gamelog::g_log_knapsack.printf(LL_INFO, "%s Jewelry ITEM_COLUMN_TYPE[%d] index[%d] user[%s,%d] voc_idx[%d] voc[attr_type:%d] [old_value:%d] [new_attr_value:%d]", __FUNCTION__,
								ItemNamespace::ITEM_COLUMN_TYPE_ITEM, index, m_module_mgr->GetRoleName(), m_module_mgr->GetUid(), i, node_attr.attr_type, node_attr.attr_value, new_value);
							node_attr.attr_value = new_value;
						}
					}
				}
			}

			this->SetItemDirty(ItemNamespace::ITEM_COLUMN_TYPE_ITEM, index, false, 0, 0);
		}
	}

	for (int index = 0; index < ItemNamespace::ITEM_COLUMN_GRID_NUM_MAX; ++index)
	{
		{//检索仓库
			ItemGridData* grid = m_grid_list[ItemNamespace::ITEM_COLUMN_TYPE_STORAGE][index];
			if (NULL == grid || grid->Invalid()) continue;

			const ItemBase * i_base = ITEMPOOL->GetItem(grid->item_id);
			if (i_base == NULL)continue;

			if (i_base->GetItemType() == I_TYPE_EQUIPMENT)
			{
				EquipmentParam & p_ep = grid->param->equip_param;
				const Equipment * e_b = (const Equipment *)i_base;
				for (int i = 0; i < p_ep.inc_num && i < EquipmentParam::EXA_ATTR_NUM_MAX; i++)
				{
					AttrVocInfo &node_attr = p_ep.attr_extra_inc[i];
					const AttValueCfg * old_cfg = LOGIC_CONFIG->GetEntryConfig()->GetEntryAttrValueConfigBack(e_b->GetEquipLevel(), node_attr.attr_type, node_attr.attr_value);
					if (NULL != old_cfg)//有符合的配置
					{
						//计算数值占用旧范围百分比
						double attr_persnt = ((double)(node_attr.attr_value - old_cfg->min) / (old_cfg->max - old_cfg->min));
						const AttValueCfg * new_cfg = LOGIC_CONFIG->GetEntryConfig()->GetEntryAttrValueConfig3(e_b->GetEquipLevel(), old_cfg->quality, node_attr.attr_type);
						if (new_cfg != NULL)
						{
							int new_value = (int)((new_cfg->max - new_cfg->min) * attr_persnt) + new_cfg->min;
							gamelog::g_log_knapsack.printf(LL_INFO, "%s equipment ITEM_COLUMN_TYPE[%d] index[%d] user[%s,%d] voc_idx[%d] voc[attr_type:%d] [old_value:%d] [new_attr_value:%d]", __FUNCTION__,
								ItemNamespace::ITEM_COLUMN_TYPE_STORAGE, index, m_module_mgr->GetRoleName(), m_module_mgr->GetUid(), i, node_attr.attr_type, node_attr.attr_value, new_value);
							node_attr.attr_value = new_value;
						}
					}
				}
			}
			else if (i_base->GetItemType() == I_TYPE_JEWELRY)
			{
				JewelryParam & p_ep = grid->param->jewelry_param;
				const Jewelry * j_b = (const Jewelry *)i_base;
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
							gamelog::g_log_knapsack.printf(LL_INFO, "%s Jewelry ITEM_COLUMN_TYPE[%d] index[%d] user[%s,%d] voc_idx[%d] voc[attr_type:%d] [old_value:%d] [new_attr_value:%d]", __FUNCTION__,
								ItemNamespace::ITEM_COLUMN_TYPE_STORAGE, index, m_module_mgr->GetRoleName(), m_module_mgr->GetUid(), i, node_attr.attr_type, node_attr.attr_value, new_value);
							node_attr.attr_value = new_value;
						}
					}
				}
			}

			this->SetItemDirty(ItemNamespace::ITEM_COLUMN_TYPE_STORAGE, index, false, 0, 0);
		}
	}
}

void Knapsack::AddRewards(std::vector<ItemConfigData>& rewards, ItemConfigData item)
{
	const ItemBase * item_base = ITEMPOOL->GetItem(item.item_id);
	if (NULL == item_base) return;

	for (size_t i = 0; i < rewards.size(); i++)
	{
		// 找到了就堆叠放入,否则push_back
		if (rewards[i].item_id == item.item_id && rewards[i].is_bind == item.is_bind)
		{
			int max_pile = item_base->GetPileLimit();

			// 刚好达到堆叠上限,后面可能还有新开的堆叠
			if (rewards[i].num >= max_pile) continue;

			// 加上之后, 溢出的部分直接break到后面去加
			if ((rewards[i].num + item.num) > max_pile)
			{
				int add_num = max_pile - rewards[i].num;
				rewards[i].num += add_num;
				item.num -= add_num;
				break;
			}

			// 不超过堆叠上限直接加就可以了
			rewards[i].num += item.num;
			return;
		}
	}

	// 上面判断全部过了,直接放进去
	rewards.push_back(item);
}

std::string Knapsack::RewardsLog(const std::vector<ItemConfigData>& rewards)
{
	std::string logs = "";
	for (size_t i = 0; i < rewards.size(); i++)
	{
		logs += STRING_SPRINTF("[%d, %d]", rewards[i].item_id, rewards[i].num);
	}

	return logs;
}

bool Knapsack::CheckZhouQiItemExpired(ItemGridData * item_grid,const ItemBase * item_base)
{
	if (!item_grid || !item_base)
	{
		return false;
	}

	if (item_grid->Invalid())
	{
		return false;
	}

	const ZhouQiLimitItemCfg * item_limit_cfg = LOGIC_CONFIG->GetZhouQiConfig()->GetZhouQiLimitItemCfg(item_grid->item_id);
	if (!item_limit_cfg)
	{
		return false;
	}

	int num = ZhouQiManager::Instance().GetZhouQiNum();
	if (num == 0)
	{
		return false;
	}

	if (item_limit_cfg->zhou_qi_num == num)
	{
		return false;
	}

	//发放邮件
	int sell_price = item_base->GetSellPrice(item_grid->param) * item_grid->num;

	static MailContentParam contentparam;
	contentparam.Reset();
	int length1 = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_item_expire_notice_subject_3);
	int length2 = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_item_expire_notice_content_3, item_limit_cfg->zhou_zi_name.c_str(), item_limit_cfg->item_id, item_grid->num);

	contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_BIND_COIN] = sell_price;
	if (length1 > 0 && length2 > 0)
	{

		MailRoute::MailToUser(m_module_mgr->GetRole()->GetUserId(), MAIL_REASON_DEFAULT, contentparam);
	}	

	return true;
}

void Knapsack::ClearAll()
{
	for (int column = 0; column < ItemNamespace::ITEM_COLUMN_TYPE_MAX; ++column)
	{
		for (int index = 0; index < ItemNamespace::ITEM_COLUMN_GRID_NUM_MAX; ++index)
		{
			ItemGridData* grid = m_grid_list[column][index];
			if (NULL == grid) continue;
			this->ConsumeItemByIndex(column, index, grid->num, "ClearAll");
		}
	}
}

void Knapsack::ClearItemList(int column, std::vector<ItemID>* item_list, const char * reason)
{
	if (column < ITEM_COLUMN_TYPE_MIN || column >= ITEM_COLUMN_TYPE_MAX) return;
	if (item_list == NULL)return;

	for (int index = 0; index < ItemNamespace::ITEM_COLUMN_GRID_NUM_MAX; ++index)
	{
		ItemGridData* grid = m_grid_list[column][index];
		if (NULL == grid) continue;
		for (std::vector<ItemID>::iterator it = item_list->begin(); it != item_list->end(); it++)
		{
			if (*it == grid->item_id)
			{
				this->ConsumeItemByIndex(column, index, grid->num, reason);
			}
		}
	}
}

bool Knapsack::UseExpCrystal()
{
	if (0 == m_module_mgr->GetCommonData().is_open_exp_crytal)
	{
		return false;
	}

	if (m_module_mgr->GetCommonData().exp_crytal_remain_times <= 0)
	{
		return false;
	}

	m_module_mgr->GetCommonData().exp_crytal_remain_times -= 1;
	m_module_mgr->GetCourseOfGrowth()->EventOperator(COG_CON_CONSUME_DOUBLE_EXP, 0, 1);
	this->SendExpCrystalInfo();
	return true;
}

void Knapsack::OnChangeProf()
{
	//背包检查对应需要更换职业的道具
	for (int column = 0; column < ItemNamespace::ITEM_COLUMN_TYPE_MAX; ++column)
	{
		for (int index = 0; index < ItemNamespace::ITEM_COLUMN_GRID_NUM_MAX; ++index)
		{
			ItemGridData* grid = m_grid_list[column][index];
			if (NULL == grid) continue;
			if (grid->Invalid())continue;
			if (m_module_mgr->GetWorkshop()->ChnageGodEquip(grid))
			{
				this->SetItemDirty(column, index, true, ITEM_CHANGE_TYPE_CHANGE);
			}
		}
	}
}

void Knapsack::OnRoleLogout()
{
	m_clear_temporary_column_time = static_cast<unsigned int>(EngineAdapter::Instance().Time()) + 10 * 60 * 60;
}

void Knapsack::SendExpCrystalInfo()
{
	Protocol::SCExpCrystalInfo info;
	info.is_open = m_module_mgr->GetCommonData().is_open_exp_crytal;
	info.count = m_module_mgr->GetCommonData().exp_crytal_remain_times;

	m_module_mgr->NetSend(&info, sizeof(info));
}

void Knapsack::OnRoleModifyExpCrystal()
{
	m_module_mgr->GetCommonData().is_open_exp_crytal ^= 1;
	this->SendExpCrystalInfo();
}

void Knapsack::RetrieveItem(ItemID item_id, short num, int retrieve_reason)
{
	if (num <= 0 || NULL == ITEMPOOL->GetItem(item_id))
	{
		return;
	}

	const char* type = g_GetRetrieveReasonTypeStr(retrieve_reason);
	if (NULL == type)
	{
		return;
	}

	static ItemExtern::ItemList item_list;
	item_list.Reset();

	this->GetListOfItem(item_id, ItemNamespace::KNAPSACK_MAX_GRID_NUM, item_list.bind_column_list,
		item_list.bind_index_list, item_list.bind_num_list, &(item_list.bind_count), item_list.nobind_column_list,
		item_list.nobind_index_list, item_list.nobind_num_list, &(item_list.nobind_count));

	int retrieve_num = 0;
	for (int i = 0; i < item_list.bind_count; ++i)
	{
		retrieve_num += item_list.bind_num_list[i];
	}
	for (int i = 0; i < item_list.nobind_count; ++i)
	{
		retrieve_num += item_list.nobind_num_list[i];
	}
	if (retrieve_num > num)
	{
		retrieve_num = num;
	}

	if (retrieve_num > 0)
	{
		this->ConsumeItem(item_id, retrieve_num, type);

		static MailContentParam contentparam; contentparam.Reset();
		int length1 = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_retrieve_mail_subject);
		int length2 = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), RETRIEVE_REASON_REFUND == retrieve_reason ? gamestring::g_retrieve_mail_content_refund_item : gamestring::g_retrieve_mail_content_bug_item, num, item_id);
		if (length1 > 0 && length2 > 0)
		{
			MailRoute::MailToUser(m_module_mgr->GetRole()->GetUserId(), MAIL_REASON_DEFAULT, contentparam);
		}

		gamelog::g_log_knapsack.printf(LL_INFO, "Knapsack::RetrieveItem user[%d,%s], item_id[%d], retrieve_num[%d], reason[%s]", m_module_mgr->GetRole()->GetUID(), m_module_mgr->GetRole()->GetName(), item_id, retrieve_num, type);
	}

	if (retrieve_num < num)
	{
		int storage_retrive_num = num - retrieve_num;
		int log_storage_retrieve_num = storage_retrive_num;

		for (int column_id = 0; column_id < ItemNamespace::ITEM_COLUMN_TYPE_MAX; column_id++)
		{
			for (int index = 0; index < ItemNamespace::ITEM_COLUMN_GRID_NUM_MAX && index < m_curr_valid_grid_num_list[column_id]; index++)
			{
				if (NULL == m_grid_list[column_id][index]) continue;

				if (m_grid_list[column_id][index]->Invalid()) continue;

				if (m_grid_list[column_id][index]->item_id != item_id) continue;

				if (m_grid_list[column_id][index]->num <= storage_retrive_num)
				{
					storage_retrive_num -= m_grid_list[column_id][index]->num;
					m_grid_list[column_id][index]->num = 0;
					m_grid_list[column_id][index]->Clear();
				}
				else
				{
					m_grid_list[column_id][index]->num -= storage_retrive_num;
					storage_retrive_num = 0;
				}

				this->SetItemDirty(column_id, index, true, ITEM_CHANGE_NOTICE_REASON_CONSUME);
			}
		}

		gamelog::g_log_knapsack.printf(LL_INFO, "Knapsack::RetrieveItem in storage,  user[%d,%s], item_id[%d], retrieve_num[%d], reason[%s]", m_module_mgr->GetRole()->GetUID(), m_module_mgr->GetRole()->GetName(), item_id, log_storage_retrieve_num, type);
	}

	ROLE_LOG_QUICK6(LOG_TYPE_RETRIEVE_ITEM, m_module_mgr->GetRole(), item_id, num, type, NULL);
}

void Knapsack::SendGetItemNotice(int notice_type, ItemID item_id, short num, int param1 /* = 0 */)
{
	Protocol::SCKnapsackGetItemNotice info;
	info.notice_type = notice_type;
	info.item_id = item_id;
	info.num = num;
	info.param1 = param1;

	m_module_mgr->NetSend(&info, sizeof(info));
}

int Knapsack::GetKnapsackVaildNum(int column)
{
	if (column < ITEM_COLUMN_TYPE_MIN || column >= ITEM_COLUMN_TYPE_MAX) return ITEM_COLUMN_GRID_NUM_MAX;

	return m_valid_grid_index_end_list[column];
}

void Knapsack::SynItemData(int column, ItemGridData * item_data)
{
	if (column < ITEM_COLUMN_TYPE_MIN || column >= ITEM_COLUMN_TYPE_MAX || NULL == item_data) return;

	const ItemBase *itembase = ITEMPOOL->GetItem(item_data->item_id);
	if (NULL == itembase) return;

	for (int i = 0; i < m_valid_grid_index_end_list[column] && i < ITEM_COLUMN_GRID_NUM_MAX; i++)
	{
		if (NULL == m_grid_list[column][i] || m_grid_list[column][i]->Invalid()) continue;
		const ItemBase *item_base = ITEMPOOL->GetItem(m_grid_list[column][i]->item_id);
		if (NULL == item_base) continue;
		if (m_grid_list[column][i]->item_unique_id != item_data->item_unique_id) continue;

		m_grid_list[column][i]->param->SetData(item_data->param->data);
		m_grid_list[column][i]->is_bind = item_data->is_bind;
		this->SetItemDirty(column, i, true, ITEM_CHANGE_NOTICE_REASON_DATA_CHANGE);
		break;
	}
}

void Knapsack::SynHuaShenItemData(std::set<long long> * item_set, bool is_element_crystal)
{
	for (int i = 0; i < m_valid_grid_index_end_list[ITEM_COLUMN_TYPE_ITEM] && i < ITEM_COLUMN_GRID_NUM_MAX; i++)
	{
		ItemGridData * item_grid = m_grid_list[ITEM_COLUMN_TYPE_ITEM][i];
		if (NULL == item_grid || item_grid->Invalid()) continue;
		const ItemBase *item_base = ITEMPOOL->GetItem(item_grid->item_id);
		if (NULL == item_base || NULL == item_grid->param) continue;

		int item_type = item_base->GetItemType();
		switch (item_type)
		{
			//化身屏蔽废弃水晶
		case I_TYPE_ELEMENT_CRYSTAL:
			{
				if (item_grid->param->element_crystal_param.has_set_net_value <= 0) break;
				if (item_grid->param->element_crystal_param.used_times <= 0) break;
			}
		case I_TYPE_EQUIPMENT:
		case I_TYPE_JEWELRY:
		case I_TYPE_PET_EQUIPMENT:
		case I_TYPE_MEDAL:
			{
				if (is_element_crystal && item_type != I_TYPE_ELEMENT_CRYSTAL) break;
				m_module_mgr->GetHuaShenManager()->SynBenZunItemData(item_grid, HUASHEN_KNAPSACK_OP_TYPE_KEY_SYN);
				if (NULL != item_set)
				{
					item_set->insert(item_grid->item_unique_id);
				}
			}
			break;
		default:
			break;
		}
	}

}

void Knapsack::SynItemBindInfo(int column, long long item_unique_id, bool is_bind)
{
	if (column < ITEM_COLUMN_TYPE_MIN || column >= ITEM_COLUMN_TYPE_MAX || item_unique_id <= 0) return;

	for (int i = 0; i < m_valid_grid_index_end_list[column] && i < ITEM_COLUMN_GRID_NUM_MAX; i++)
	{
		if (NULL == m_grid_list[column][i] || m_grid_list[column][i]->Invalid()) continue;
		const ItemBase *item_base = ITEMPOOL->GetItem(m_grid_list[column][i]->item_id);
		if (NULL == item_base) continue;
		if (m_grid_list[column][i]->item_unique_id != item_unique_id) continue;

		m_grid_list[column][i]->is_bind = is_bind;
		this->SetItemDirty(column, i, true, ITEM_CHANGE_NOTICE_REASON_DATA_CHANGE);
		break;
	}
}

int Knapsack::ReclaimItem(ItemID item_id)
{
	const ItemBase* item_base = ITEMPOOL->GetItem(item_id);
	if (NULL == item_base) return 0;
	int column = item_base->GetItemColumn();

	int count = 0;
	for (int i = 0; i < m_valid_grid_index_end_list[column]; ++i)
	{
		if (NULL == m_grid_list[column][i] || m_grid_list[column][i]->Invalid()) continue;

		const ItemBase *item = ITEMPOOL->GetItem(m_grid_list[column][i]->item_id);
		if (NULL == item) continue;
		if (item->GetItemId() != item_id) continue;

		count += m_grid_list[column][i]->num;
		this->ClearGrid(column, i);
		this->SetItemDirty(column, i, true, Knapsack::ITEM_CHANGE_NOTICE_REASON_CONSUME);
	}

	for (int i = 0; i < m_valid_grid_index_end_list[ITEM_COLUMN_TYPE_TEMPORARY]; ++i)
	{
		if (NULL == m_grid_list[ITEM_COLUMN_TYPE_TEMPORARY][i] || m_grid_list[ITEM_COLUMN_TYPE_TEMPORARY][i]->Invalid()) continue;

		const ItemBase *item = ITEMPOOL->GetItem(m_grid_list[ITEM_COLUMN_TYPE_TEMPORARY][i]->item_id);
		if (NULL == item) continue;
		if (item->GetItemId() != item_id) continue;

		count += m_grid_list[ITEM_COLUMN_TYPE_TEMPORARY][i]->num;
		this->ClearGrid(ITEM_COLUMN_TYPE_TEMPORARY, i);
		this->SetItemDirty(ITEM_COLUMN_TYPE_TEMPORARY, i, true, Knapsack::ITEM_CHANGE_NOTICE_REASON_CONSUME);
	}

	return count;
}

bool Knapsack::ReturnGodEquip()
{
	const GodEquipTurnBackListCfg * turn_back_cfg = GODEQUIP_CONFIG->GetTurnBackUserId(m_module_mgr->GetUid());
	if (NULL != turn_back_cfg)
	{
		for (std::vector<GodEquipTurnBackNode>::const_iterator it = turn_back_cfg->back_item_vec.begin(); it != turn_back_cfg->back_item_vec.end(); it++)
		{
			for (int bag_column = 0; bag_column < ITEM_COLUMN_TYPE_MAX; bag_column++)
			{
				for (int i = 0; i < m_valid_grid_index_end_list[bag_column]; ++i)
				{
					if (NULL == m_grid_list[bag_column][i] || m_grid_list[bag_column][i]->Invalid()) continue;

					const ItemBase *item = ITEMPOOL->GetItem(m_grid_list[bag_column][i]->item_id);
					if (NULL == item) continue;

					if (it->back_item == m_grid_list[bag_column][i]->item_id)
					{
						m_module_mgr->GetWorkshop()->CheckGodEquipItemDataReturnBreak(m_grid_list[bag_column][i], &(*it));
						this->SetItemDirty(bag_column, i, true, Knapsack::ITEM_CHANGE_NOTICE_REASON_DATA_CHANGE);
						gamelog::g_log_knapsack.printf(LL_INFO, "Knapsack::ReturnGodEquip role[%d,%s] clear_item_id[%d] num[%d] is_bind[%s]",
							m_module_mgr->GetUid(), m_module_mgr->GetRoleName(), m_grid_list[bag_column][i]->item_id, m_grid_list[bag_column][i]->num,
							m_grid_list[bag_column][i]->is_bind ? "True" : "False");
					}
				}
			}
		}
	}
	return true;
}

bool Knapsack::ReturnGodEquip2()
{
	const GodEquipTurnBackListCfg * turn_back_cfg = GODEQUIP_CONFIG->GetTurnBackUserId2(m_module_mgr->GetUid());
	if (NULL != turn_back_cfg)
	{
		for (std::vector<GodEquipTurnBackNode>::const_iterator it = turn_back_cfg->back_item_vec.begin(); it != turn_back_cfg->back_item_vec.end(); it++)
		{
			for (int bag_column = 0; bag_column < ITEM_COLUMN_TYPE_MAX; bag_column++)
			{
				for (int i = 0; i < m_valid_grid_index_end_list[bag_column]; ++i)
				{
					if (NULL == m_grid_list[bag_column][i] || m_grid_list[bag_column][i]->Invalid()) continue;

					const ItemBase *item = ITEMPOOL->GetItem(m_grid_list[bag_column][i]->item_id);
					if (NULL == item) continue;

					if (it->back_item == m_grid_list[bag_column][i]->item_id)
					{
						m_module_mgr->GetWorkshop()->CheckGodEquipItemDataReturnBreak(m_grid_list[bag_column][i], &(*it));
						this->SetItemDirty(bag_column, i, true, Knapsack::ITEM_CHANGE_NOTICE_REASON_DATA_CHANGE);
						gamelog::g_log_knapsack.printf(LL_INFO, "Knapsack::ReturnGodEquip role[%d,%s] clear_item_id[%d] num[%d] is_bind[%s]",
							m_module_mgr->GetUid(), m_module_mgr->GetRoleName(), m_grid_list[bag_column][i]->item_id, m_grid_list[bag_column][i]->num,
							m_grid_list[bag_column][i]->is_bind ? "True" : "False");
					}
				}
			}
		}
	}
	return true;
}

void Knapsack::CheckItemReturn()
{
	std::map<int, std::vector<ItemConfigData> > m_return_item_vec;
	for (int bag_column = 0; bag_column < ITEM_COLUMN_TYPE_MAX; bag_column++)
	{
		for (int i = 0; i < m_valid_grid_index_end_list[bag_column]; ++i)
		{
			if (NULL == m_grid_list[bag_column][i] || m_grid_list[bag_column][i]->Invalid()) continue;

			const ItemBase *item = ITEMPOOL->GetItem(m_grid_list[bag_column][i]->item_id);
			if (NULL == item) continue;
			const ItemReturnConfig * return_cfg = LOGIC_CONFIG->GetKnapsackConfig()->GetItemReturnConfig(m_grid_list[bag_column][i]->item_id);
			if (NULL == return_cfg)continue;

			ItemConfigData node;
			node.item_id = m_grid_list[bag_column][i]->item_id;
			node.is_bind = m_grid_list[bag_column][i]->is_bind == 1 ? true : false;
			node.num = m_grid_list[bag_column][i]->num;

			std::map<int, std::vector<ItemConfigData> >::iterator it = m_return_item_vec.find(return_cfg->mail_type);
			if (it == m_return_item_vec.end())
			{
				std::vector<ItemConfigData> &node_vec = m_return_item_vec[return_cfg->mail_type];
			}
			std::vector<ItemConfigData> &node_vec = m_return_item_vec[return_cfg->mail_type];

			bool add_num = false;
			for (std::vector<ItemConfigData>::iterator it = node_vec.begin(); it != node_vec.end(); it++)//累积
			{
				if (it->item_id == m_grid_list[bag_column][i]->item_id)
				{
					it->num += node.num;
					add_num = true;
					break;
				}
			}

			if (!add_num)node_vec.push_back(node);

			this->ClearGrid(bag_column, i);
			this->SetItemDirty(bag_column, i, true, Knapsack::ITEM_CHANGE_NOTICE_REASON_CONSUME);

			gamelog::g_log_knapsack.printf(LL_INFO, "Knapsack::CheckItemReturn role[%d,%s] clear_item_id[%d] num[%d] is_bind[%s]",
				m_module_mgr->GetUid(), m_module_mgr->GetRoleName(), node.item_id, node.num, node.is_bind ? "True" : "False");
		}
	}

	for (std::map<int, std::vector<ItemConfigData> >::iterator it = m_return_item_vec.begin(); it != m_return_item_vec.end(); it++)
	{
		int lensub = 0;
		int length = 0;
		std::vector<ItemConfigData> m_item_vec;
		static MailContentParam contentparam; contentparam.Reset();
		int real_item_num = 0;
		for (std::vector<ItemConfigData>::iterator item_put_it = it->second.begin(); item_put_it != it->second.end(); item_put_it++)
		{
			const ItemReturnConfig * return_cfg = LOGIC_CONFIG->GetKnapsackConfig()->GetItemReturnConfig(item_put_it->item_id);
			if (NULL == return_cfg)continue;

			if (return_cfg->return_item.size() > 0)
			{
				for (std::vector<ItemConfigData>::const_iterator it2 = return_cfg->return_item.begin(); it2 != return_cfg->return_item.end(); it2++)
				{
					bool add_num = false;
					for (std::vector<ItemConfigData>::iterator it3 = m_item_vec.begin(); it3 != m_item_vec.end(); it3++)//累积
					{
						if (it3->item_id == it2->item_id)
						{
							it3->num += it2->num;
							add_num = true;
							break;
						}
					}
					if (!add_num)m_item_vec.push_back(*it2);
				}
			}

			if (return_cfg->price > 0)
			{
				switch (return_cfg->price_type)
				{
				case MONEY_TYPE_GUILD_CONTRIBUTION:
						contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_GUILDGONGXIAN] += return_cfg->price * item_put_it->num;
					break;
				case MONEY_TYPE_IMMORTAL_COIN:
						contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_IMMORTAL_COIN] += return_cfg->price * item_put_it->num;
					break;
				case MONEY_TYPE_GOLD:
						contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_GOLD] += return_cfg->price * item_put_it->num;
					break;
				case MONEY_TYPE_COIN_BIND:
						contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_BIND_COIN] += return_cfg->price * item_put_it->num;
					break;
				case MONEY_TYPE_COLOSSEUM:
						contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_COLOSSEUM_CREDITS] += return_cfg->price * item_put_it->num;
					break;
				case MONEY_TYPE_PLATFORM_BATTLE_CREDITS:
						contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_PLATFORM_BATTLE_CREDITS] += return_cfg->price * item_put_it->num;
					break;
				case MONEY_TYPE_HUAN_JIE_ZHAN_CHANG_CREDITS:
						contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_HUAN_JIE_ZHAN_CHANG_CREDITS] = return_cfg->price * item_put_it->num;
					break;
				case MONEY_TYPE_LUCK_COIN:
						contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_LUCK_COIN] += return_cfg->price * item_put_it->num;
					break;
				case MONEY_TYPE_HUAN_JIE_ZHAN_CHANG_COIN:
						contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_HUAN_JIE_COIN] += return_cfg->price * item_put_it->num;
					break;
				case MONEY_TYPE_YUAN_SUI_ZHI_LING:
						contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_YUAN_SUI_ZHI_LING] += return_cfg->price * item_put_it->num;
					break;
				case MONEY_TYPE_SILVER_COIN:
						contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_SILVER_COIN] += return_cfg->price * item_put_it->num;
					break;
				default:
					gamelog::g_log_knapsack.printf(LL_INFO, "[Knapsack] user[%s,%d] CheckItemReturn err money[%d,%d]", m_module_mgr->GetRole()->GetName(), m_module_mgr->GetUid(), return_cfg->price_type, return_cfg->price);
					break;
				}
			}
			switch (return_cfg->mail_type)
			{
				case ITEM_RETURN_MAIL_TYPE_CRYSTAL:
				{
					lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_return_item_subject);
					length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_return_item_content, item_put_it->item_id, item_put_it->num);
				}
				break;
				case ITEM_RETURN_MAIL_TYPE_DREAM_NOTES:
				{
					lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_return_item_subject2);
					length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_return_item_content2, item_put_it->item_id, item_put_it->num);
				}
				break;
				case ITEM_RETURN_MAIL_TYPE_PET_STR_GUA:
				{
					lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_return_item_subject3);
					length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_return_item_content3, item_put_it->item_id, item_put_it->num);
				}
				break;
				case ITEM_RETURN_MAIL_TYPE_HEART_SKILL:
				{
					lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_return_item_subject4);
					length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_return_item_content4);
				}
				break;
				case ITEM_RETURN_MAIL_TYPE_CRYSTAL_NORMAL:
				{
					lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_return_item_subject5);
					length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_return_item_content5);
				}
				break;
				case ITEM_RETURN_MAIL_TYPE_BREAK_ITEM:
				{
					lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_return_item_subject6);
					length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_return_item_content6);
				}
				break;
				case ITEM_RETURN_MAIL_TYPE_CRYSTAL_JING_YING:
				{
					lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_return_item_subject7);
					length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_return_item_content7, item_put_it->item_id, item_put_it->num);
				}
				break;
				case ITEM_RETURN_MAIL_TYPE_KNAPSACK_BACK:
				{
					lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_return_item_subject8);
					length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_return_item_content8);
				}
				break;
				case ITEM_RETURN_MAIL_TYPE_FIVE_SQUARE:
				{
					lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_return_item_subject9);
					length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_return_item_content9);
				}
				break;
				case ITEM_RETURN_MAIL_TYPE_VOCULTIMATE:
				{
					lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_return_item_subject10);
					length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_return_item_content10);
				}
				break;
				case ITEM_RETURN_MAIL_TYPE_EQUIP_JEWELRY_UP:
				{
					lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_return_item_subject11);
					length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_return_item_content11);
				}
				break;
				case ITEM_RETURN_MAIL_TYPE_PARTNER_SOUL:
				{
					lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_return_item_subject12);
					length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_return_item_content12);
				}
				break;
				case ITEM_RETURN_MAIL_TYPE_SMART_MOUNTS:
				{
					lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_return_item_subject13);
					length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_return_item_content13);
				}
				break;
			case ITEM_RETURN_MAIL_TYPE_CHANGE_CARD:
				{
					lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_return_item_subject14);
					length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_return_item_content14);
				}
				break;
			}

			if (m_item_vec.size() > 0)
			{
				for (std::vector<ItemConfigData>::iterator real_it = m_item_vec.begin(); real_it != m_item_vec.end() && real_item_num < MAX_ATTACHMENT_ITEM_NUM; real_it++)
				{
					contentparam.item_list[real_item_num].num = real_it->num * item_put_it->num; // 每单个回收的物品，就要补偿n个奖励的物品，所以要乘以it->num
					contentparam.item_list[real_item_num].is_bind = real_it->is_bind ? 1 : 0;
					contentparam.item_list[real_item_num++].item_id = real_it->item_id;

					if (real_item_num >= MAX_ATTACHMENT_ITEM_NUM)
					{
						if (lensub > 0 && length > 0)
						{
							MailRoute::MailToUser(m_module_mgr->GetRole()->GetUserId(), MAIL_REASON_DEFAULT, contentparam);
						}
						real_item_num = 0;
						//邮件数量过多，先发一封，清空道具和伙伴，内容不变
						for (int i = 0; i < ARRAY_ITEM_COUNT(contentparam.item_list); ++i)
						{
							contentparam.item_list[i].Reset();
						}
						contentparam.virtual_item_list.Reset();
					}
				}
			}
		}
		if (lensub > 0 && length > 0)
		{
			MailRoute::MailToUser(m_module_mgr->GetRole()->GetUserId(), MAIL_REASON_DEFAULT, contentparam);
		}
	}
}

void Knapsack::CheckGodEquipErrUplevel()
{
	for (int bag_column = 0; bag_column < ITEM_COLUMN_TYPE_MAX; bag_column++)
	{
		for (int i = 0; i < m_valid_grid_index_end_list[bag_column]; ++i)
		{
			if (NULL == m_grid_list[bag_column][i] || m_grid_list[bag_column][i]->Invalid()) continue;

			const ItemBase *item = ITEMPOOL->GetItem(m_grid_list[bag_column][i]->item_id);
			if (NULL == item) continue;

			if (item->GetItemType() == I_TYPE_EQUIPMENT)
			{//神装返还
				if (NULL != m_grid_list[bag_column][i]->param)
				{
					const Equipment * equip_ib = (const Equipment*)item;
					int equip_index = equip_ib->GetEquipType();
					if (equip_index == Equipment::E_TYPE_SHIELD)
					{
						ItemID low_god_id[8] = { 3901,3902,3903,3904,3905,3906,3907,3908 };
						ItemID high_god_id[8] = { 3909,3910,3911,3912,3913,3914,3915,3916 };
						bool need_back = false;
						for (int low_god_idx = 0; low_god_idx < 8; low_god_idx++)
						{
							if (low_god_id[low_god_idx] == m_grid_list[bag_column][i]->item_id && m_grid_list[bag_column][i]->param->equip_param.strengthen_level >= 41)
							{
								need_back = true;
							}
						}
						for (int high_god_idx = 0; high_god_idx < 8; high_god_idx++)
						{
							if (high_god_id[high_god_idx] == m_grid_list[bag_column][i]->item_id)
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
			}
		}
	}
}

void Knapsack::CheckGodEquipChangeLevel()
{
	for (int index = 0; index < ItemNamespace::ITEM_COLUMN_GRID_NUM_MAX; ++index)
	{
		{//检索物品背包
			ItemGridData* grid = m_grid_list[ItemNamespace::ITEM_COLUMN_TYPE_ITEM][index];
			if (NULL == grid || grid->Invalid()) continue;

			if (m_module_mgr->GetWorkshop()->CheckGodEquipItemDataChangeLevel(grid))this->SetItemDirty(ItemNamespace::ITEM_COLUMN_TYPE_ITEM, index, true, ITEM_CHANGE_NOTICE_REASON_DATA_CHANGE);
		}
	}

	for (int index = 0; index < ItemNamespace::ITEM_COLUMN_GRID_NUM_MAX; ++index)
	{
		{//检索仓库
			ItemGridData* grid = m_grid_list[ItemNamespace::ITEM_COLUMN_TYPE_STORAGE][index];
			if (NULL == grid || grid->Invalid()) continue;

			if (m_module_mgr->GetWorkshop()->CheckGodEquipItemDataChangeLevel(grid))this->SetItemDirty(ItemNamespace::ITEM_COLUMN_TYPE_STORAGE, index, true, ITEM_CHANGE_NOTICE_REASON_DATA_CHANGE);
		}
	}
}

void Knapsack::FixGodEquipBaseVoc()
{
	for (int index = 0; index < ItemNamespace::ITEM_COLUMN_GRID_NUM_MAX; ++index)
	{
		{//检索物品背包
			ItemGridData* grid = m_grid_list[ItemNamespace::ITEM_COLUMN_TYPE_ITEM][index];
			if (NULL == grid || grid->Invalid()) continue;
			const ItemBase * i_b = ITEMPOOL->GetItem(grid->item_id);
			if (NULL == i_b || i_b->GetColor() < ItemBase::I_COLOR_STAR || i_b->GetColor() > ItemBase::I_COLOR_GOD)continue;
			if (I_TYPE_EQUIPMENT != i_b->GetItemType() && I_TYPE_JEWELRY != i_b->GetItemType())continue;
			if (i_b->GetItemType() == I_TYPE_EQUIPMENT)
			{
				EquipmentParam & p_ep = grid->param->equip_param;
				const StrengthenCfg *consume_cfg = GODEQUIP_CONFIG->GetStrengthenCfg(grid->item_id, p_ep.strengthen_level);
				if (consume_cfg == NULL)continue;

				p_ep.attr_num = 0;
				for (int i = 0; i < EquipmentParam::ATTR_NUM_MAX; i++)
				{
					p_ep.attr_list[i].attr_type = consume_cfg->attr_type_list[i];
					p_ep.attr_list[i].attr_value = consume_cfg->attr_add_list[i];
					if (consume_cfg->attr_add_list[i] <= 0)continue;//值为0则词条为假不增加词条数量
					p_ep.attr_num++;
				}
			}
			else if (i_b->GetItemType() == I_TYPE_JEWELRY)
			{
				JewelryParam & p_ep = grid->param->jewelry_param;
				const StrengthenCfg *consume_cfg = GODEQUIP_CONFIG->GetStrengthenCfg(grid->item_id, p_ep.strengthen_level);
				if (consume_cfg == NULL)continue;

				p_ep.attr_num = 0;
				for (int i = 0; i < EquipmentParam::ATTR_NUM_MAX; i++)
				{
					p_ep.attr_list[i].attr_type = consume_cfg->attr_type_list[i];
					p_ep.attr_list[i].attr_value = consume_cfg->attr_add_list[i];
					if (consume_cfg->attr_add_list[i] <= 0)continue;//值为0则词条为假不增加词条数量
					p_ep.attr_num++;
				}
			}
			this->SetItemDirty(ItemNamespace::ITEM_COLUMN_TYPE_ITEM, index, true, ITEM_CHANGE_NOTICE_REASON_DATA_CHANGE);
		}
	}

	for (int index = 0; index < ItemNamespace::ITEM_COLUMN_GRID_NUM_MAX; ++index)
	{
		{//检索仓库
			ItemGridData* grid = m_grid_list[ItemNamespace::ITEM_COLUMN_TYPE_STORAGE][index];
			if (NULL == grid || grid->Invalid()) continue;
			const ItemBase * i_b = ITEMPOOL->GetItem(grid->item_id);
			if (NULL == i_b || i_b->GetColor() < ItemBase::I_COLOR_STAR || i_b->GetColor() > ItemBase::I_COLOR_GOD)continue;
			if (I_TYPE_EQUIPMENT != i_b->GetItemType() && I_TYPE_JEWELRY != i_b->GetItemType())continue;
			if (i_b->GetItemType() == I_TYPE_EQUIPMENT)
			{
				EquipmentParam & p_ep = grid->param->equip_param;
				const StrengthenCfg *consume_cfg = GODEQUIP_CONFIG->GetStrengthenCfg(grid->item_id, p_ep.strengthen_level);
				if (consume_cfg == NULL)continue;

				p_ep.attr_num = 0;
				for (int i = 0; i < EquipmentParam::ATTR_NUM_MAX; i++)
				{
					p_ep.attr_list[i].attr_type = consume_cfg->attr_type_list[i];
					p_ep.attr_list[i].attr_value = consume_cfg->attr_add_list[i];
					if (consume_cfg->attr_add_list[i] <= 0)continue;//值为0则词条为假不增加词条数量
					p_ep.attr_num++;
				}
			}
			else if (i_b->GetItemType() == I_TYPE_JEWELRY)
			{
				JewelryParam & p_ep = grid->param->jewelry_param;
				const StrengthenCfg *consume_cfg = GODEQUIP_CONFIG->GetStrengthenCfg(grid->item_id, p_ep.strengthen_level);
				if (consume_cfg == NULL)continue;

				p_ep.attr_num = 0;
				for (int i = 0; i < EquipmentParam::ATTR_NUM_MAX; i++)
				{
					p_ep.attr_list[i].attr_type = consume_cfg->attr_type_list[i];
					p_ep.attr_list[i].attr_value = consume_cfg->attr_add_list[i];
					if (consume_cfg->attr_add_list[i] <= 0)continue;//值为0则词条为假不增加词条数量
					p_ep.attr_num++;
				}
			}
			this->SetItemDirty(ItemNamespace::ITEM_COLUMN_TYPE_STORAGE, index, true, ITEM_CHANGE_NOTICE_REASON_DATA_CHANGE);
		}
	}
}

bool Protocol::SCItemIntoKnpackSpecialProtocal::Serialize(TLVSerializer2 * outstream) const
{
	bool ret = true;

	TLVSerializer2 & out = *outstream;
	ret = ret && out.Push(header.msg_type);
	ret = ret && out.Push(header.server_id);
	ret = ret && out.Push(get_reson);
	ret = ret && out.Push(count);
	ret = ret && out.Push(param1);
	ret = ret && out.Push(param2);
	for (int i = 0; ret && i < count && i < ARRAY_LENGTH(item_list); ++i)
	{
		ItemInfo  curr = item_list[i];

		ret = ret && out.Push(curr.item_id);
		ret = ret && out.Push(curr.is_bind);
		ret = ret && out.Push(curr.count);
		ret = ret && out.Push(curr.reserve_sh);
		ret = ret && out.Push(curr.has_param);

		if (curr.has_param && curr.param_length < (unsigned int)ARRAY_LENGTH(curr.param_data))
		{
			TLVSerializer2 s;
			s.Reset(&curr.param_data, curr.param_length);
			s.MoveCurPos(curr.param_length);
			ret = ret && out.Push(s);
		}
	}

	return ret;
}

static const long long BASE_PRIOR = 10000000000000000LL; //9223372036854775807最大值

long long SortStruct::GetPriorId(short _item_level, short _item_type, short _item_sub_type, short _item_color, short _equip_type, ItemID _item_id, short _param)
{
	// 新的排序优先级：
	// 物品sub类型 > 道具品质 > 等级 > 装备评分 > 道具ID

	long long base_flag = 0;
	if (I_TYPE_EQUIPMENT == _item_type || I_TYPE_JEWELRY == _item_type || I_TYPE_MEDAL == _item_type
		|| ITEM_SUB_TYPE_PARTNER_FRAGMENT == _item_sub_type
		|| ITEM_SUB_TYPE_PET_FRAGMENT == _item_sub_type
		|| ITEM_SUB_TYPE_SHIZHUANG == _item_sub_type
		|| ITEM_SUB_TYPE_APPEARANCE == _item_sub_type
		|| ITEM_SUB_TYPE_SKIN == _item_sub_type
		|| ITEM_SUB_TYPE_NO_CHECK_EQUIP == _item_sub_type
		)
	{
		base_flag = 1;
	}

	if (1 == base_flag)
	{
		return GetPriorId_1(base_flag * BASE_PRIOR, _item_level, _item_type, _item_sub_type, _item_color, _equip_type, _item_id, _param);
	}
	else
	{
		return GetPriorId_2(base_flag * BASE_PRIOR, _item_level, _item_type, _item_sub_type, _item_color, _equip_type, _item_id, _param);
	}

}

long long SortStruct::GetPriorId_1(long long base_prior, short _item_level, short _item_type, short _item_sub_type, short _item_color, short _equip_type, ItemID _item_id, short _param)
{
	// sub_type > color > level > param > type > item_id
	static const long long SUB_TYPE_PRIOR = BASE_PRIOR / 1000;
	static const long long COLOR_PRIOR = SUB_TYPE_PRIOR / 100;
	static const long long EQUIP_LEVEL_PRIOR = COLOR_PRIOR / 1000;
	static const long long PARAM_PRIOR = EQUIP_LEVEL_PRIOR / 100;
	static const long long EQUIP_TYPE_PRIOR = PARAM_PRIOR / 10;


	UNSTD_STATIC_CHECK(ItemBase::I_COLOR_MAX <= 11);
	UNSTD_STATIC_CHECK(Equipment::E_INDEX_MAX + Jewelry::J_TYPE_MAX < 10);
	UNSTD_STATIC_CHECK(ItemNamespace::ITEM_SUB_TYPE_MAX < 100);
	UNSTD_STATIC_CHECK(PARAM_PRIOR > MAX_UINT16);

	if (I_TYPE_EQUIPMENT == _item_type || I_TYPE_JEWELRY == _item_type)
	{
		UNSTD_STATIC_CHECK(ITEM_SUB_TYPE_WEAPON == 1);
		_item_sub_type = ITEM_SUB_TYPE_WEAPON; // 特殊需求 装备和灵饰都当作sub_type是武器
	}

	long long sub_type_prior = LOGIC_CONFIG->GetKnapsackConfig()->GetSubTypePrior(_item_sub_type);
	return base_prior + sub_type_prior * SUB_TYPE_PRIOR + _item_level * EQUIP_LEVEL_PRIOR + EQUIP_TYPE_PRIOR * _equip_type + _item_color * COLOR_PRIOR + _param * PARAM_PRIOR + _item_id;
}

long long SortStruct::GetPriorId_2(long long base_prior, short _item_level, short _item_type, short _item_sub_type, short _item_color, short _equip_type, ItemID _item_id, short _param)
{
	// sub_type > color > level > param > item_id
	static const long long SUB_TYPE_PRIOR = BASE_PRIOR / 1000;
	static const long long COLOR_PRIOR = SUB_TYPE_PRIOR / 100;
	static const long long EQUIP_LEVEL_PRIOR = COLOR_PRIOR / 1000;
	static const long long EQUIP_TYPE_PRIOR = EQUIP_LEVEL_PRIOR / 10;
	static const long long PARAM_PRIOR = EQUIP_TYPE_PRIOR / 100;
	UNSTD_STATIC_CHECK(ItemBase::I_COLOR_MAX <= 11);
	UNSTD_STATIC_CHECK(ItemNamespace::ITEM_SUB_TYPE_MAX < 100);
	UNSTD_STATIC_CHECK(PARAM_PRIOR > MAX_UINT16);

	if (I_TYPE_EQUIPMENT == _item_type || I_TYPE_JEWELRY == _item_type)
	{
		UNSTD_STATIC_CHECK(ITEM_SUB_TYPE_WEAPON == 1);
		_item_sub_type = ITEM_SUB_TYPE_WEAPON; // 特殊需求 装备和灵饰都当作sub_type是武器
	}

	long long sub_type_prior = LOGIC_CONFIG->GetKnapsackConfig()->GetSubTypePrior(_item_sub_type);
	return base_prior + sub_type_prior * SUB_TYPE_PRIOR + _item_color * COLOR_PRIOR + _item_level * EQUIP_LEVEL_PRIOR + EQUIP_TYPE_PRIOR * _equip_type + _param * PARAM_PRIOR + _item_id;
}
