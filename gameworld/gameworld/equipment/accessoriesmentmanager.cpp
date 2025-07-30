#include <memory.h>

#include "servercommon/struct/itemlistparam.h"
#include "servercommon/errornum.h"
#include "servercommon/noticenum.h"
#include "servercommon/uniqueidgenerator.hpp"
#include "engineadapter.h"
#include "accessoriesmentmanager.h"
#include "gamelog.h"
#include "obj/character/role.h"
#include "config/logicconfigmanager.hpp"
#include "item/knapsack.h"
#include "item/itempool.h"
#include "item/itembase.h"
#include "other/rolemodulemanager.hpp"
#include "protocol/msgequipment.h"
#include "other/route/mailroute.hpp"
#include "protocol/msgaccessories.h"
#include "other/peishi/peishiconfig.hpp"
#include "other/tianming/tianming.hpp"
#include "servercommon/serverconfig/battleconfig/battle_special_effect_pool.hpp"
#include "servercommon/struct/battle/battle_special_effect_def.hpp"
#include "other/roleattribute/roleexpconfig.hpp"
#include "skill/skillpool.hpp"
#include "config/otherconfig/scoresystemconfig.hpp"
#include "other/capability/capability.hpp"
#include "other/flyupsystem/flyupsystem.hpp"

/////////////////////////////////////////////////////////////////////////////////////////////////

AccessoriesmentManager::AccessoriesmentManager() : m_module_mgr(NULL), m_is_change(false)
{
	memset(m_dirty_mark, 0, sizeof(m_dirty_mark));
	memset(m_old_state, 0, sizeof(m_old_state));
	memset(m_accessories_list, NULL, sizeof(m_accessories_list));
}

AccessoriesmentManager::~AccessoriesmentManager()
{
	for (int i = 0; i < Accessoriesment::A_INDEX_MAX; ++i)
	{
		if (NULL != m_accessories_list[i].param)
		{
			delete m_accessories_list[i].param;
			m_accessories_list[i].param = NULL;
		}
	}
}

void AccessoriesmentManager::Init(RoleModuleManager *module_mgr, int param_list_num, const ItemListParam::ItemListData * param_data_list[], int level_param[ACCESSORIES_SPECIAL_EQUIP_LEVEL])
{
	m_module_mgr = module_mgr;
	for (int i = 0; i < ACCESSORIES_SPECIAL_EQUIP_LEVEL; i++)
	{
		accessories_special_equip_list[i] = level_param[i];
	}
	for (int i = 0; i < param_list_num && i < ItemNamespace::ACCESSORIES_GRID_MAX_COUNT; i++)
	{
		if (NULL == param_data_list[i]) continue;
		if (param_data_list[i]->index >= ItemNamespace::ACCESSORIES_INDEX_BEGIN && param_data_list[i]->index < ItemNamespace::ACCESSORIES_INDEX_END)
		{
			int equip_index = param_data_list[i]->index - ItemNamespace::ACCESSORIES_INDEX_BEGIN; // index经过转换则为装备部位
			if (equip_index < 0 || equip_index >= Accessoriesment::A_INDEX_MAX) continue;

			if (!m_accessories_list[equip_index].Invalid())
			{
				gamelog::g_log_serious_error.printf(LL_CRITICAL, "[AccessoriesmentManager::Init Duplicate][[%s %d] equip_index:%d old_item_id:%d new_item_id:%d]", m_module_mgr->GetRole()->GetName(), UidToInt(m_module_mgr->GetRole()->GetUserId()),
					equip_index, (int)m_accessories_list[equip_index].item_id, (int)param_data_list[i]->item_wrapper.item_id);
				continue;
			}

			UInt16 item_id = param_data_list[i]->item_wrapper.item_id;
			const Accessoriesment *equip = (const Accessoriesment *)ITEMPOOL->GetItem(item_id);
			if (NULL == equip || equip->GetItemType() != I_TYPE_ACCESSORIES)
			{
				gamelog::g_log_serious_error.printf(LL_CRITICAL, "[AccessoriesmentManager::Init IsNotAccessoriesment][[%s %d] equip_index:%d item_id:%d]", m_module_mgr->GetRole()->GetName(), UidToInt(m_module_mgr->GetRole()->GetUserId()),
					equip_index, (int)item_id);
				continue;
			}
		
			NetValueItemParam* param = equip->CreateParamFromParamData(param_data_list[i]->item_wrapper.param_data);
			if (NULL == param) // struct_data有错
			{
				gamelog::g_log_serious_error.printf(LL_CRITICAL, "[AccessoriesmentManager::Init StructDataError][[%s %d] equip_index:%d item_id:%d]", m_module_mgr->GetRole()->GetName(), UidToInt(m_module_mgr->GetRole()->GetUserId()),
					equip_index, (int)item_id);
				continue;
			}
		
			m_accessories_list[equip_index].item_id = item_id;
			m_accessories_list[equip_index].num = param_data_list[i]->item_wrapper.num;
			m_accessories_list[equip_index].invalid_time = param_data_list[i]->item_wrapper.invalid_time;
			m_accessories_list[equip_index].param = param;
			m_accessories_list[equip_index].param->equip_param.CheckModifyVersion();
			m_accessories_list[equip_index].is_market_buy_flag = param_data_list[i]->item_wrapper.is_market_buy_flag;
			m_accessories_list[equip_index].item_unique_id = param_data_list[i]->item_wrapper.item_unique_id;
			if (m_accessories_list[equip_index].item_unique_id == 0)
			{
				long long item_unique_id = 0;
				UniqueIDGenerator::Instance()->GenUniqueID(&item_unique_id);
				if (0 != item_unique_id)
				{
					m_accessories_list[equip_index].item_unique_id = item_unique_id;
					this->SetAccessoriesGridDirty(equip_index, false);
#ifdef TRACK_ITEM_FLAG
					gamelog::g_log_item_track.printf(LL_INFO, "TRACK_ITEM_FLAG %s assign unique_id[%lld] to role[%d,%s] item[%d,%s] num[%d] from index[%d]", __FUNCTION__,
						item_unique_id, m_module_mgr->GetRole()->GetUID(), m_module_mgr->GetRole()->GetName(),
						m_accessories_list[equip_index].item_id, equip->GetItemName(), m_accessories_list[equip_index].num, equip_index);
#endif
				}
			}

			if (m_accessories_list[equip_index].invalid_time > 0 && !equip->HasInvalidTime())
			{
				m_accessories_list[equip_index].invalid_time = 0;
			}

			m_old_state[equip_index] = true;
		}
	}
}

void AccessoriesmentManager::GetInitParam(int level_param[ACCESSORIES_SPECIAL_EQUIP_LEVEL])
{
	for (int i = 0; i < ACCESSORIES_SPECIAL_EQUIP_LEVEL; i++)
	{
		level_param[i] = accessories_special_equip_list[i];
	}
}

const AttributeList & AccessoriesmentManager::ReCalcAttr(AttributeList & base_add, bool is_recalc, int role_type, AttributeList * equip_special_attr)
{
	if (is_recalc)
	{
		m_attrs_add.Reset();
		//套装属性
		int suit_num = 0;
		int suit_type[Accessoriesment::A_INDEX_MAX] = { 0 };
		int suit_type_num[Accessoriesment::A_INDEX_MAX] = { 0 };
		int suit_color_max[Accessoriesment::A_INDEX_MAX] = { 0 };
		int suit_color_min[Accessoriesment::A_INDEX_MAX] = { 0 };
		int suit_color_min_2[Accessoriesment::A_INDEX_MAX] = { 0 };//二件套专用
		//仙饰基础属性
		for (int i = 0; i < ACCESSORIES_SPECIAL_EQUIP_LEVEL; i++)
		{
			int special_level = accessories_special_equip_list[i];
			const PeiShiSpecialEquipCfg * special_cfg = LOGIC_CONFIG->GetPeiShiConfig()->GetSpecialEquip(i + 1, special_level);
			if (special_cfg == NULL)continue;
			for (int attr_idx = 0; attr_idx < MAX_PEISHI_SPECIAL_EQUIP_ATTR_NUM; attr_idx++)
			{
				if(special_cfg->attr_value_list[attr_idx] > 0 && 
					special_cfg->attr_type_list[attr_idx] >= BATTLE_ATTR_MIN &&
					special_cfg->attr_type_list[attr_idx] < BATTLE_ATTR_MAX)
					m_attrs_add.m_attrs[special_cfg->attr_type_list[attr_idx]] += special_cfg->attr_value_list[attr_idx];
			}
		}
		for (int i = 0; i < Accessoriesment::A_INDEX_MAX; ++i)
		{
			if (m_accessories_list[i].Invalid())continue;
			ItemGridData & access_data = m_accessories_list[i];
			const ItemBase * i_b = ITEMPOOL->GetItem(access_data.item_id);
			if (NULL == i_b)continue;

			//基础属性
			{
				const Accessoriesment * a_b = (const Accessoriesment *)i_b;
				AttrInfo base_attr_list[MAX_ACCESSORIES_BASE_VOC_NUM];
				memset(base_attr_list, 0, sizeof(base_attr_list));
				a_b->GetBaseAttr(base_attr_list);

				AttributePlan tmp_ap;
				for (int base_idx = 0; base_idx < MAX_ACCESSORIES_BASE_VOC_NUM; base_idx++)
				{
					if (base_attr_list[base_idx].attr_type >= 0 && base_attr_list[base_idx].attr_type < BATTLE_ATTR_MAX &&
						base_attr_list[base_idx].attr_value > 0)
					{
						// 天命 装备强化 基础属性加成
						int attr_total_per = m_module_mgr->GetTianMing()->GetAttrPer(i + 1);
						int tian_ming_add_val = int(base_attr_list[base_idx].attr_value * 1.0f * attr_total_per / 10000);
						base_attr_list[base_idx].attr_value += tian_ming_add_val;

						if (base_attr_list[base_idx].attr_type < BATTLE_ATTR_ELEMENT_END)
						{
							m_attrs_add.m_attrs[base_attr_list[base_idx].attr_type] += base_attr_list[base_idx].attr_value;
						}
						else
						{
							tmp_ap.add_point_list[base_attr_list[base_idx].attr_type - BATTLE_ADD_POINT_TYPE_BEGIN] += base_attr_list[base_idx].attr_value;
						}
					}
				}
				if (access_data.param->accessoriess_param.strengthen_level > 0)
				{
					//有强化等级，基础属性变更
					const PeiShiStrengthenCfg * level_cfg = LOGIC_CONFIG->GetPeiShiConfig()->GetPeiShiStrengthenCfg(access_data.item_id, access_data.param->accessoriess_param.strengthen_level - 1);
					if (NULL != level_cfg)
					{
						// 仙饰加成
						int special_level = accessories_special_equip_list[PeiShiSpecialEquipCfg::SPECIAL_TYPE_1 - 1];
						const PeiShiSpecialEquipCfg * special_cfg = LOGIC_CONFIG->GetPeiShiConfig()->GetSpecialEquip(PeiShiSpecialEquipCfg::SPECIAL_TYPE_1, special_level);
						
						if(level_cfg->attr_value_1 > 0 && level_cfg->att_type_1 >= BATTLE_ATTR_MIN && level_cfg->att_type_1 < BATTLE_ATTR_MAX)
						{
							int new_value = level_cfg->attr_value_1;
							if (NULL != special_cfg)
							{
								if (special_cfg->spec_value_2 > 0)
								{
									new_value += int(new_value * 1.0f * special_cfg->spec_value_2 / 1000);
								}
							}
							if (level_cfg->att_type_1 < BATTLE_ATTR_ELEMENT_END)
							{
								m_attrs_add.m_attrs[level_cfg->att_type_1] += new_value;
							}
							else
							{
								tmp_ap.add_point_list[level_cfg->att_type_1 - BATTLE_ADD_POINT_TYPE_BEGIN] += new_value;
							}
						}

						if(level_cfg->attr_value_2 > 0 && level_cfg->att_type_2 >= BATTLE_ATTR_MIN && level_cfg->att_type_2 < BATTLE_ATTR_MAX)
						{
							int new_value = level_cfg->attr_value_2;
							if (NULL != special_cfg)
							{
								if (special_cfg->spec_value_2 > 0)
								{
									new_value += int(new_value * 1.0f * special_cfg->spec_value_2 / 1000);
								}
							}
							if (level_cfg->att_type_2 < BATTLE_ATTR_ELEMENT_END)
							{
								m_attrs_add.m_attrs[level_cfg->att_type_2] += new_value;
							}
							else
							{
								tmp_ap.add_point_list[level_cfg->att_type_2 - BATTLE_ADD_POINT_TYPE_BEGIN] += new_value;
							}
						}
					}
				}
				AttributeList extra_attrs;
				LOGIC_CONFIG->GetRoleExpConfig()->ConvertPointToAttribute(tmp_ap, extra_attrs);
				m_attrs_add.Add(extra_attrs.m_attrs);

				int cfg_suit = a_b->GetSuitType();
				if (cfg_suit > 0 && suit_num < Accessoriesment::A_INDEX_MAX)
				{
					bool is_new = true;
					for (int i = 0; i < Accessoriesment::A_INDEX_MAX && i < suit_num; i++)
					{
						if (suit_type[i] == cfg_suit)
						{
							//专门2件套挑选最高品质的第二件
							if (suit_type_num[i] + 1 >= 2)
							{
								if (suit_color_min_2[i] == 0)
								{
									if (a_b->GetColor() >= suit_color_min[i])
									{
										suit_color_min_2[i] = suit_color_min[i];
									}
									else
									{
										suit_color_min_2[i] = a_b->GetColor();
									}
								}
								else
								{
									if (suit_color_min[i] == suit_color_min_2[i])
									{
										if (a_b->GetColor() >= suit_color_max[i])
										{
											suit_color_min_2[i] = suit_color_max[i];
										}
										else
										{
											suit_color_min_2[i] = a_b->GetColor();
										}
									}
								}
							}
							suit_type_num[i] += 1;
							if (suit_color_min[i] > a_b->GetColor())
							{
								suit_color_min[i] = a_b->GetColor();
							}
							if (suit_color_max[i] < a_b->GetColor())
							{
								suit_color_max[i] = a_b->GetColor();
							}
							is_new = false;
							break;;
						}
					}
					if (is_new)
					{
						suit_type[suit_num] += cfg_suit;
						suit_color_min[suit_num] = a_b->GetColor();
						suit_color_max[suit_num] = a_b->GetColor();
						suit_type_num[suit_num++] = 1;
					}
				}
			}
			//副属性
			{
				AttributePlan tmp_ap;//潜能加点
				for (int era_idx = 0; era_idx < MAX_ACCESSORIES_VOC_NUM; era_idx++)
				{
					AccessoriesParam & node_param = access_data.param->accessoriess_param;
					if (node_param.voc_list[era_idx].attr_type >= 0 &&
						node_param.voc_list[era_idx].attr_type < BATTLE_ATTR_MAX &&
						node_param.voc_list[era_idx].attr_value > 0)
					{
						if (node_param.voc_list[era_idx].attr_type < BATTLE_ATTR_ELEMENT_END)
						{
							m_attrs_add.m_attrs[node_param.voc_list[era_idx].attr_type] += node_param.voc_list[era_idx].attr_value;
						}
						else
						{
							tmp_ap.add_point_list[node_param.voc_list[era_idx].attr_type - BATTLE_ADD_POINT_TYPE_BEGIN] += node_param.voc_list[era_idx].attr_value;
						}
					}
				}
				AttributeList extra_attrs;
				LOGIC_CONFIG->GetRoleExpConfig()->ConvertPointToAttribute(tmp_ap, extra_attrs);
				m_attrs_add.Add(extra_attrs.m_attrs);//潜能
			}
		}
		int role_level = m_module_mgr->GetRole()->GetLevel();
		if (suit_num > 0)
		{
			for (int i = 0; i < suit_num && i < Accessoriesment::A_INDEX_MAX; i++)
			{
				{
					const PeishiSkillCfg * suit_cfg_2 = LOGIC_CONFIG->GetPeiShiConfig()->GetPeiShiSkillCfg(suit_type[i], suit_color_min_2[i]);
					if (NULL != suit_cfg_2)
					{
						if (suit_type_num[i] >= 2)
						{
							if (suit_cfg_2->skill1_profession == 0 || m_module_mgr->GetRole()->GetAppearanceProf() == suit_cfg_2->skill1_profession)
							{
								const GamePassiveSkillCfg * passive_skill = SkillPool::Instance().GetPassiveSkillCfg(suit_cfg_2->skill_1);
								if (NULL == passive_skill) continue;
								if (suit_cfg_2->skill_level < 0 || suit_cfg_2->skill_level >= MAX_BATTLE_SKILL_LEVEL + 1)continue;
								int sp_id = passive_skill->level_spid_list[suit_cfg_2->skill_level];
								const SpecialEffectCfg * spe_cfg = BattleSpecialEffectPool::GetInstance().GetSpecialEffectCfg(sp_id);
								if (NULL != spe_cfg)
								{
									for (int k = 0; k < spe_cfg->effect_num && k < EFFECT_DETAIL_NUM; ++k)
									{
										const SpecialEffectKey & key = spe_cfg->effect_detail_list[k];
										if (key.effect_type != SPECIAL_EFFECT_TYPE_ADD_ATTR) continue;

										const EffectDetailCfg * detail_cfg = BattleSpecialEffectPool::GetInstance().GetEffectDetailCfg(key);
										if (NULL == detail_cfg) continue;

										if (SPE_ATTR_ADD_ATTR == detail_cfg->effect_sub_type && detail_cfg->param_num >= 2
											&& detail_cfg->param_list[0] >= BATTLE_ATTR_MIN && detail_cfg->param_list[0] < BATTLE_ATTR_MAX)
										{
											m_attrs_add.m_attrs[detail_cfg->param_list[0]] += detail_cfg->param_list[1];
										}

										if (SPE_ATTR_ADD_ATTR_BY_LEVEL == detail_cfg->effect_sub_type && detail_cfg->param_num >= 2
											&& detail_cfg->param_list[0] >= BATTLE_ATTR_MIN && detail_cfg->param_list[0] < BATTLE_ATTR_MAX)
										{
											BattleAttr add_attr = static_cast<BattleAttr>(ceil(role_level * (double)detail_cfg->param_list[1] / BATTLE_PERCENT_NUM));
											m_attrs_add.m_attrs[detail_cfg->param_list[0]] += add_attr;
										}
									}
								}
							}
						}
					}
				}
				{
					const PeishiSkillCfg * suit_cfg = LOGIC_CONFIG->GetPeiShiConfig()->GetPeiShiSkillCfg(suit_type[i], suit_color_min[i]);
					if (NULL != suit_cfg)
					{
						if (suit_type_num[i] >= 5)
						{
							if (suit_cfg->skill2_profession == 0 || m_module_mgr->GetRole()->GetAppearanceProf() == suit_cfg->skill2_profession)
							{
								const GamePassiveSkillCfg * passive_skill = SkillPool::Instance().GetPassiveSkillCfg(suit_cfg->skill_2);
								if (NULL == passive_skill) continue;
								if (suit_cfg->skill_level < 0 || suit_cfg->skill_level >= MAX_BATTLE_SKILL_LEVEL + 1)continue;
								int sp_id = passive_skill->level_spid_list[suit_cfg->skill_level];
								const SpecialEffectCfg * spe_cfg = BattleSpecialEffectPool::GetInstance().GetSpecialEffectCfg(sp_id);
								if (NULL != spe_cfg)
								{
									for (int k = 0; k < spe_cfg->effect_num && k < EFFECT_DETAIL_NUM; ++k)
									{
										const SpecialEffectKey & key = spe_cfg->effect_detail_list[k];
										if (key.effect_type != SPECIAL_EFFECT_TYPE_ADD_ATTR) continue;

										const EffectDetailCfg * detail_cfg = BattleSpecialEffectPool::GetInstance().GetEffectDetailCfg(key);
										if (NULL == detail_cfg) continue;

										if (SPE_ATTR_ADD_ATTR == detail_cfg->effect_sub_type && detail_cfg->param_num >= 2
											&& detail_cfg->param_list[0] >= BATTLE_ATTR_MIN && detail_cfg->param_list[0] < BATTLE_ATTR_MAX)
										{
											m_attrs_add.m_attrs[detail_cfg->param_list[0]] += detail_cfg->param_list[1];
										}

										if (SPE_ATTR_ADD_ATTR_BY_LEVEL == detail_cfg->effect_sub_type && detail_cfg->param_num >= 2
											&& detail_cfg->param_list[0] >= BATTLE_ATTR_MIN && detail_cfg->param_list[0] < BATTLE_ATTR_MAX)
										{
											BattleAttr add_attr = static_cast<BattleAttr>(ceil(role_level * (double)detail_cfg->param_list[1] / BATTLE_PERCENT_NUM));
											m_attrs_add.m_attrs[detail_cfg->param_list[0]] += add_attr;
										}
									}
								}
							}
						}
					}
				}
			}
		}

		m_capability = m_module_mgr->GetCapability()->GetCalcCapByAttrList(m_attrs_add);
	}

	base_add.Add(m_attrs_add.m_attrs);
	return m_attrs_add;
}

int AccessoriesmentManager::PutOn(short column, short index, short equip_index)
{
	static ItemGridData grid_data; grid_data.Reset();
	int t_role_lv = m_module_mgr->GetRole()->GetLevel();
	if (!m_module_mgr->GetKnapsack()->GetItemGridData(column, index, &grid_data))
	{
		return -2;
	}

	const Accessoriesment * equip = (const Accessoriesment *)ITEMPOOL->GetItem(grid_data.item_id);
	if (NULL == equip || I_TYPE_ACCESSORIES != equip->GetItemType()) return -4;
	if (equip->GetPeiShiType() != equip_index)return -5;
	if (NULL == grid_data.param) return -6;

	if (m_module_mgr->GetFlyUpSystem()->GetFlyUpFlag() < equip->GetFlyFlag())
	{
		m_module_mgr->NoticeNum(errornum::EN_FLY_UP_LEVEL_ERR);
		return -7;
	}

	short equip_type = equip->GetPeiShiType();

	if (equip_index - 1 < 0 || equip_index - 1 >= Accessoriesment::A_INDEX_MAX || equip_type != equip_index) return -7;

	if (t_role_lv < equip->GetLimitLevel())
	{
		m_module_mgr->NoticeNum(errornum::EN_EQUIPMENT_PUT_LEVEL_LIMIT);
		return -10;
	}

	int take_off_column = -1;
	int take_off_index = -1;

	if (!m_accessories_list[equip_index - 1].Invalid()) // 该位置有装备 脱之
	{
		int ret = this->TakeOff(equip_index, true, &take_off_column, &take_off_index);
		if (0 != ret)
		{
			return -11;
		}
	}

	NetValueItemParam * param = equip->CreateParamFromStruct(*grid_data.param);
	if (NULL == param) return -14;
	
	ItemGridData & e_idx = m_accessories_list[equip_index - 1];
	e_idx.item_id = grid_data.item_id;
	e_idx.param = param;
	e_idx.num = 1;
	e_idx.invalid_time = grid_data.invalid_time;
	e_idx.is_bind = grid_data.is_bind;
	e_idx.is_market_buy_flag = grid_data.is_market_buy_flag;
	e_idx.item_unique_id = grid_data.item_unique_id;
	
	if (!m_module_mgr->GetKnapsack()->ConsumeItemByIndex(column, index, 1, __FUNCTION__))
	{
		return -15;
	}
	gamelog::g_log_accessories_ment.printf(LL_INFO, "%s user[%d,%s] column[%d] index[%d] equip_index[%d] new_param[%s]",
		__FUNCTION__, m_module_mgr->GetUid(), m_module_mgr->GetRoleName(), column, index, equip_index, grid_data.param->GetParamLog(I_TYPE_ACCESSORIES));
	m_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_ACCESSORIES, __FUNCTION__);
	this->SetAccessoriesGridDirty(equip_index - 1, true);
	return 0;
}

int AccessoriesmentManager::TakeOff(short equip_index, bool is_from_puton, int * take_off_column, int * take_off_index)
{
	if (equip_index - 1 < 0 || equip_index - 1 >= Accessoriesment::A_INDEX_MAX) return -1;

	if (m_accessories_list[equip_index - 1].Invalid()) return -2;

	AccessoriesParam *equip_param = (AccessoriesParam *)m_accessories_list[equip_index - 1].param;
	if (NULL == equip_param) return -3;

	const Accessoriesment *equip = (const Accessoriesment*)ITEMPOOL->GetItem(m_accessories_list[equip_index - 1].item_id);
	if (NULL == equip || equip->GetItemType() != I_TYPE_ACCESSORIES) return -4;

	if (!m_module_mgr->GetKnapsack()->CheckForPut(equip->GetItemId(), 1))
	{
		m_module_mgr->GetRole()->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return -5;
	}

	ItemDataWrapper item_wrapper(m_accessories_list[equip_index - 1].item_id, 1, (short)m_accessories_list[equip_index - 1].is_bind ? 1 : 0,
		1, m_accessories_list[equip_index - 1].param->data, m_accessories_list[equip_index - 1].invalid_time, 0,
		m_accessories_list[equip_index - 1].item_unique_id, m_accessories_list[equip_index - 1].is_market_buy_flag);

	if (!m_module_mgr->GetKnapsack()->Put(item_wrapper, PUT_REASON_NO_NOTICE, take_off_column, take_off_index))
	{
		return -6;
	}
	gamelog::g_log_accessories_ment.printf(LL_INFO, "%s user[%d,%s] equip_index[%d] param[%s]",
		__FUNCTION__, m_module_mgr->GetUid(), m_module_mgr->GetRoleName(), equip_index, m_accessories_list[equip_index - 1].param->GetParamLog(I_TYPE_ACCESSORIES));
	m_accessories_list[equip_index - 1].Clear();
	if (!is_from_puton)
	{
		m_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_ACCESSORIES, __FUNCTION__);
	}
	this->SetAccessoriesGridDirty(equip_index - 1, !is_from_puton);
	return 0;
}

void AccessoriesmentManager::Strengthen(short is_wear, short index)
{
	const ItemGridData * grid_data = NULL;
	if (is_wear == 1)
	{
		if (index - 1 < 0 || index - 1 >= Accessoriesment::A_INDEX_MAX)return;
		grid_data = &m_accessories_list[index - 1];
	}
	else
	{
		grid_data = m_module_mgr->GetKnapsack()->GetItemGridData2(ItemNamespace::ITEM_COLUMN_TYPE_ACCESSORIES, index);
		if (NULL == grid_data)return;
	}
	if (grid_data == NULL || grid_data->Invalid())return;
	const ItemBase * i_b = ITEMPOOL->GetItem(grid_data->item_id);
	if (i_b == NULL || i_b->GetItemType() != I_TYPE_ACCESSORIES)return;

	AccessoriesParam &access_param = grid_data->param->accessoriess_param;

	const PeiShiStrengthenCfg * level_cfg = LOGIC_CONFIG->GetPeiShiConfig()->GetPeiShiStrengthenCfg(grid_data->item_id, access_param.strengthen_level);
	if (level_cfg == NULL)return;

	if (level_cfg->role_level > m_module_mgr->GetRole()->GetLevel())
	{
		m_module_mgr->NoticeNum(errornum::EN_ROLE_LEVEL_TOO_LOW);
		return;
	}

	if ((int)level_cfg->cost_item_list.size() > 0)
	{
		int stuff_count = 0; ItemExtern::ItemConsumeConfig stuff_list[ItemNamespace::MAX_BATCH_CONSUME_ITEM_NUM];
		//检查消耗
		for (int t_cur = 0; t_cur < (short)level_cfg->cost_item_list.size() && stuff_count < ItemNamespace::MAX_BATCH_CONSUME_ITEM_NUM; ++t_cur)
		{
			const ItemConfigData & p_mc = level_cfg->cost_item_list[t_cur];
			if (p_mc.num <= 0) continue;

			const ItemBase * mate_ib = ITEMPOOL->GetItem(p_mc.item_id);
			if (NULL == mate_ib) continue;

			stuff_list[stuff_count] = ItemExtern::ItemConsumeConfig(p_mc.item_id, false, p_mc.num);
			++stuff_count;
		}

		// 消耗
		if (stuff_count > 0)
		{
			static ItemExtern::ItemConsumeList consume_list; consume_list.Reset();
			if (!ItemExtern::GetConsumeListByOrder(m_module_mgr->GetRole(), false, stuff_count, stuff_list, &consume_list, true))
			{
				m_module_mgr->NoticeNum(errornum::EN_STUFF_NOT_ENOUGH);
				return;
			}

			if (consume_list.count > 0 && !m_module_mgr->GetKnapsack()->ConsumeItemByIndexList(consume_list.count, consume_list.column_list, consume_list.index_list, consume_list.num_list, __FUNCTION__)) return;
		}
	}
	int old_level = access_param.strengthen_level;
	access_param.strengthen_level += 1;
	gamelog::g_log_accessories_ment.printf(LL_INFO, "%s user[%d,%s] item_id[%d] union_id[%lld] old_level[%d] new_level[%d]",
		__FUNCTION__, m_module_mgr->GetUid(), m_module_mgr->GetRoleName(), grid_data->item_id, grid_data->item_unique_id, old_level, access_param.strengthen_level);
	m_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_ACCESSORIES, __FUNCTION__);
	if (!is_wear)m_module_mgr->GetKnapsack()->SetItemDirty(ItemNamespace::ITEM_COLUMN_TYPE_ACCESSORIES, index, true, Knapsack::ITEM_CHANGE_NOTICE_REASON_DATA_CHANGE);
	else this->SetAccessoriesGridDirty(index - 1);
}

void AccessoriesmentManager::ChangeAccessories(short is_put_on, short column, short index, int new_item_id)
{
	ItemGridData *grid_data = NULL;
	if (1 == is_put_on)
	{
		if (index - 1 < 0 || index - 1 >= Accessoriesment::A_INDEX_MAX)return;
		grid_data = &m_accessories_list[index - 1];
	}
	else
	{
		grid_data = m_module_mgr->GetKnapsack()->GetItemGridData2(column, index);
	}
	
	//int t_role_lv = m_module_mgr->GetRole()->GetLevel();
	if (grid_data == NULL)return;
	if (grid_data->item_id == new_item_id)return;//同道具不允许
	const ItemBase * equip = ITEMPOOL->GetItem(grid_data->item_id);
	if (NULL == equip || I_TYPE_ACCESSORIES != equip->GetItemType()) return;
	
	if (NULL == grid_data->param) return;
	const Accessoriesment * accessories = (const Accessoriesment *)ITEMPOOL->GetItem(grid_data->item_id);
	if (NULL == accessories) return;

	const ItemBase * new_ib = ITEMPOOL->GetItem(new_item_id);
	if (NULL == new_ib)return;
	const Accessoriesment * new_accessories = (const Accessoriesment *)new_ib;
	//计算消耗
	AccessoriesParam* old_param = &grid_data->param->accessoriess_param;
	int entry_num = 0;
	for (int i = 0; i < MAX_ACCESSORIES_VOC_NUM; i++)
	{
		if (old_param->voc_list[i].attr_value > 0)
		{
			entry_num += 1;
		}
	}

	if (new_ib->GetColor() != equip->GetColor())return;//同品质
	if (new_accessories->GetPeiShiType() != accessories->GetPeiShiType())return;//同部位

	const PeiShiChangeCfg * change_cfg = LOGIC_CONFIG->GetPeiShiConfig()->GetChangeCfg(new_ib->GetColor(), entry_num - 1);
	if (NULL == change_cfg)return;
	
	const PeishiOtherCfg & other_cfg = LOGIC_CONFIG->GetPeiShiConfig()->GetOtherCfg();

	int cost_item_id = accessories->GetFlyFlag() == other_cfg.need_feisheng ? other_cfg.displace_card_feisheng : other_cfg.displace_card;
	
	if (!m_module_mgr->GetKnapsack()->HasItem(cost_item_id, change_cfg->cost_num))
	{
		m_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_ITEM);
		return;
	}

	if (!m_module_mgr->GetKnapsack()->ConsumeItem(cost_item_id, change_cfg->cost_num, __FUNCTION__))
	{
		return;
	}
	
	//修改道具信息
	int old_item_id = grid_data->item_id;
	grid_data->item_id = new_item_id;
	gamelog::g_log_accessories_ment.printf(LL_INFO, "%s user[%d,%s] old_item_id[%d] new_item_id[%d] union_id[%lld] param[%s]",
		__FUNCTION__, m_module_mgr->GetUid(), m_module_mgr->GetRoleName(), old_item_id, grid_data->item_id, grid_data->item_unique_id, grid_data->param->GetParamLog(I_TYPE_ACCESSORIES));
	m_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_ACCESSORIES, __FUNCTION__);
	if (!is_put_on)m_module_mgr->GetKnapsack()->SetItemDirty(ItemNamespace::ITEM_COLUMN_TYPE_ACCESSORIES, index, true, Knapsack::ITEM_CHANGE_NOTICE_REASON_DATA_CHANGE);
	else this->SetAccessoriesGridDirty(index - 1);
	return;
}

void AccessoriesmentManager::Discompose(short column, short index, int num)
{
	const PeishiOtherCfg & other_cfg = LOGIC_CONFIG->GetPeiShiConfig()->GetOtherCfg();
	static ItemGridData grid_data; grid_data.Reset();
	if (!m_module_mgr->GetKnapsack()->GetItemGridData(column, index, &grid_data))
	{
		return;
	}
	const ItemBase * i_b = ITEMPOOL->GetItem(grid_data.item_id);
	if (NULL == i_b)return;
	int real_add_jifen = 0;

	if (i_b->GetItemType() == I_TYPE_ACCESSORIES)
	{
		const Accessoriesment * a_b = (const Accessoriesment *)i_b;
		int fenjie = a_b->GetFenJieTianMing();
		int attr_num = 0;
		for (int i = 0; i < 7; i++)
		{
			if (grid_data.param->accessoriess_param.voc_list[i].attr_value > 0)
			{
				attr_num += 1;
			}
		}
		real_add_jifen = fenjie + attr_num * other_cfg.entry_exp;
		num = 1;//装备默认消耗1

		if (!this->StrengthenGiveBack(&grid_data))return;
	}
	else if(grid_data.item_id == other_cfg.tianmingdan_id)
	{
		if (grid_data.num < num)return;
		real_add_jifen = other_cfg.tianmingdan_exp * num;
	}

	gamelog::g_log_accessories_ment.printf(LL_INFO, "%s user[%d,%s] column[%d] index[%d] item_id[%d] union_id[%lld] real_add_jifen[%d]",
		__FUNCTION__, m_module_mgr->GetUid(), m_module_mgr->GetRoleName(), column, index, grid_data.item_id, grid_data.item_unique_id, real_add_jifen);

	//消耗
	if (!m_module_mgr->GetKnapsack()->ConsumeItemByIndex(column, index, num, __FUNCTION__))return;
	
	//给天命系统加经验
	m_module_mgr->GetTianMing()->AddExp(real_add_jifen, __FUNCTION__);
}

void AccessoriesmentManager::ComposeVod(bool is_wear, short index, short replace_index, Protocol::CSAccessoriesMentVocComposeReq::ComposeVoc change_voc_list[MAX_ACCESSORIES_VOC_NUM])
{
	const ItemGridData * grid_data = NULL;
	if (is_wear)
	{
		if (index - 1 < 0 || index - 1 >= Accessoriesment::A_INDEX_MAX)return;
		grid_data = &m_accessories_list[index - 1];
	}
	else
	{
		grid_data = m_module_mgr->GetKnapsack()->GetItemGridData2(ItemNamespace::ITEM_COLUMN_TYPE_ACCESSORIES, index);
		if (NULL == grid_data)return;
	}
	if (grid_data == NULL || grid_data->Invalid())return;
	const ItemBase * i_b = ITEMPOOL->GetItem(grid_data->item_id);
	if (i_b == NULL || i_b->GetItemType() != I_TYPE_ACCESSORIES)return;
	static ItemGridData replace_grid_data;
	if (!m_module_mgr->GetKnapsack()->GetItemGridData(ItemNamespace::ITEM_COLUMN_TYPE_ACCESSORIES, replace_index, &replace_grid_data))
	{
		return;
	}

	if (replace_grid_data.Invalid())return;
	const ItemBase * r_b = ITEMPOOL->GetItem(replace_grid_data.item_id);
	if (r_b == NULL || r_b->GetItemType() != I_TYPE_ACCESSORIES)return;

	AccessoriesParam &access_param = grid_data->param->accessoriess_param;
	AccessoriesParam &replace_access_param = replace_grid_data.param->accessoriess_param;
	int real_change_entry = 0;
	for (int voc_i = 0; voc_i < MAX_ACCESSORIES_VOC_NUM; voc_i++)
	{
		if(change_voc_list[voc_i].voc_index == -1)continue;
		if (change_voc_list[voc_i].replace_voc_index == -1)continue;
		real_change_entry += 1;
		for (int voc_j = 0; voc_j < MAX_ACCESSORIES_VOC_NUM; voc_j++)
		{
			if (change_voc_list[voc_j].voc_index == -1)continue;
			if (change_voc_list[voc_j].replace_voc_index == -1)continue;
			if (voc_i == voc_j)continue;
			if (change_voc_list[voc_i].voc_index == change_voc_list[voc_j].voc_index)return;//不允许有重复的替换操作
			if (change_voc_list[voc_i].replace_voc_index == change_voc_list[voc_j].replace_voc_index)return;//不允许有重复的替换操作
		}
	}

	const ChuanChengCostCfg * cost_cfg = LOGIC_CONFIG->GetPeiShiConfig()->GetChuanChengCostCfg(r_b->GetLimitLevel(), r_b->GetColor());
	if (NULL == cost_cfg)return;
	
	if (!m_module_mgr->GetMoney()->CoinBindMoreThan(cost_cfg->coin))
	{
		m_module_mgr->NoticeNum(errornum::EN_COIN_NOT_ENOUGH);
		return;
	}

	if (!this->StrengthenGiveBack(&replace_grid_data))return;

	AccessoriesParam check_access_param;
	memcpy(&check_access_param, &access_param, sizeof(access_param));
	//模拟继承-检查继承后词条数
	for (int voc_real = 0; voc_real < MAX_ACCESSORIES_VOC_NUM; voc_real++)
	{
		int voc_index = change_voc_list[voc_real].voc_index;
		int replace_voc_index = change_voc_list[voc_real].replace_voc_index;
		if (voc_index < 0 || voc_index >= MAX_ACCESSORIES_VOC_NUM)continue;
		if (replace_voc_index < 0 || replace_voc_index >= MAX_ACCESSORIES_VOC_NUM)continue;
		if (replace_access_param.voc_list[replace_voc_index].attr_value == 0)continue;
		check_access_param.voc_list[voc_index] = replace_access_param.voc_list[replace_voc_index];
	}
	int check_entry_num = 0;
	for (int voc_i = 0; voc_i < MAX_ACCESSORIES_VOC_NUM; voc_i++)
	{
		if (check_access_param.voc_list[voc_i].attr_value == 0)continue;
		check_entry_num += 1;
	}
	const Accessoriesment *a_b = (const Accessoriesment *)i_b;
	if (check_entry_num > a_b->GetMaxEntryNum())
	{
		return;
	}
	std::string replace_log;
	replace_log = replace_grid_data.param->GetParamLog(I_TYPE_ACCESSORIES);
	if (!m_module_mgr->GetKnapsack()->ConsumeItemByIndex(ItemNamespace::ITEM_COLUMN_TYPE_ACCESSORIES, replace_index, 1, __FUNCTION__))return;
	if (!m_module_mgr->GetMoney()->UseCoinBind(cost_cfg->coin, __FUNCTION__))return;
	//实际继承
	for (int voc_real = 0; voc_real < MAX_ACCESSORIES_VOC_NUM; voc_real++)
	{
		int voc_index = change_voc_list[voc_real].voc_index;
		if (voc_index < 0 || voc_index >= MAX_ACCESSORIES_VOC_NUM)continue;
		access_param.voc_list[voc_index] = check_access_param.voc_list[voc_index];//把前面模拟记录下来的赋值到实际继承
	}
	gamelog::g_log_accessories_ment.printf(LL_INFO, "%s user[%d,%s] is_wear[%d] index[%d] replace_index[%d] new_param[%s] replace_param[%s]",
		__FUNCTION__, m_module_mgr->GetUid(), m_module_mgr->GetRoleName(), is_wear ? 1 : 0, index, replace_index, grid_data->param->GetParamLog(I_TYPE_ACCESSORIES), replace_log.c_str());

	m_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_ACCESSORIES, __FUNCTION__);
	if (!is_wear)m_module_mgr->GetKnapsack()->SetItemDirty(ItemNamespace::ITEM_COLUMN_TYPE_ACCESSORIES, replace_index, true, Knapsack::ITEM_CHANGE_NOTICE_REASON_DATA_CHANGE);
	else this->SetAccessoriesGridDirty(index - 1);
}

void AccessoriesmentManager::SendAccessoriesMentInfo()
{
	static Protocol::SCAccessoriesMentInfo info;
	info.capability = m_capability;
	for (int i = 0; i < Accessoriesment::A_INDEX_MAX; i++)
	{
		info.equipment_list[i].index = i + 1;
		info.equipment_list[i].item_id = m_accessories_list[i].item_id;
		info.equipment_list[i].num = m_accessories_list[i].num;
		info.equipment_list[i].is_bind = m_accessories_list[i].is_bind;
		info.equipment_list[i].item_unique_id = m_accessories_list[i].item_unique_id;
		if (NULL != m_accessories_list[i].param)
		{
			info.equipment_list[i].param = m_accessories_list[i].param->accessoriess_param;
		}
		else
		{
			memset(&info.equipment_list[i].param, 0, sizeof(info.equipment_list[i].param));
		}
	}

	EngineAdapter::Instance().NetSend(m_module_mgr->GetRole()->GetNetId(), (const char*)&info, sizeof(info));
}

void AccessoriesmentManager::SendAccessoriesmentInfoToRoleByIndex(short equip_index)
{
	if (equip_index < Accessoriesment::A_INDEX_MIN || equip_index >= Accessoriesment::A_INDEX_MAX) return;

	static Protocol::SCAccessoriesMentChange info;
	info.capability = m_capability;
	info.equip_info.index = equip_index + 1;
	info.equip_info.item_id = m_accessories_list[equip_index].item_id;
	info.equip_info.num = m_accessories_list[equip_index].num;
	info.equip_info.is_bind = m_accessories_list[equip_index].is_bind;
	info.equip_info.item_unique_id = m_accessories_list[equip_index].item_unique_id;
	if (NULL != m_accessories_list[equip_index].param)
	{
		info.equip_info.param = m_accessories_list[equip_index].param->accessoriess_param;
	}
	else
	{
		memset(&info.equip_info.param, 0, sizeof(info.equip_info.param));
	}
	EngineAdapter::Instance().NetSend(m_module_mgr->GetRole()->GetNetId(), (const char*)&info, sizeof(info));
}

void AccessoriesmentManager::UpLevelSpecialEquip(int seq)
{
	if (seq - 1 < 0 || seq - 1 >= ACCESSORIES_SPECIAL_EQUIP_LEVEL)return;
	int & level = accessories_special_equip_list[seq - 1];
	const PeiShiSpecialEquipCfg * special_equip_cfg = LOGIC_CONFIG->GetPeiShiConfig()->GetSpecialEquip(seq, level + 1);
	if (NULL == special_equip_cfg)return;
	
	if ((int)special_equip_cfg->cost_vec.size() > 0)
	{
		int stuff_count = 0; ItemExtern::ItemConsumeConfig stuff_list[ItemNamespace::MAX_BATCH_CONSUME_ITEM_NUM];
		for (int t_cur = 0; t_cur < (int)special_equip_cfg->cost_vec.size() && stuff_count < ItemNamespace::MAX_BATCH_CONSUME_ITEM_NUM; ++t_cur)
		{
			const ItemConfigData & mate_item = special_equip_cfg->cost_vec[t_cur];
			stuff_list[stuff_count++] = ItemExtern::ItemConsumeConfig(mate_item.item_id, false, mate_item.num);
		}
		static ItemExtern::ItemConsumeList consume_list; consume_list.Reset();
		if (!ItemExtern::GetConsumeListByOrder(m_module_mgr->GetRole(), false, stuff_count, stuff_list, &consume_list, true))
		{
			m_module_mgr->NoticeNum(errornum::EN_STUFF_NOT_ENOUGH);		// 正常来说不会走到这里
			return;
		}
		if (consume_list.count > 0 && !m_module_mgr->GetKnapsack()->ConsumeItemByIndexList(consume_list.count, consume_list.column_list, consume_list.index_list, consume_list.num_list, __FUNCTION__)) return;
	}

	level += 1;
	m_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_ACCESSORIES, __FUNCTION__);
	if (seq == PeiShiSpecialEquipCfg::SPECIAL_TYPE_2)m_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_EQUIPMENT, __FUNCTION__);//升级的需要重算一次装备
	this->SendSpecialEquiplist();
}

void AccessoriesmentManager::SendSpecialEquiplist()
{
	Protocol::SCAccessoriesSpecialEquip info;

	for (int i = 0; i < ACCESSORIES_SPECIAL_EQUIP_LEVEL; i++)
	{
		info.accessories_special_equip_list[i] = accessories_special_equip_list[i];
	}

	m_module_mgr->NetSend((const char*)&info, sizeof(info));
}

void AccessoriesmentManager::SetAccessoriesGridDirty(short equip_index, bool to_notice_client /* = true */)
{
	if (equip_index < 0 || equip_index >= Accessoriesment::A_INDEX_MAX) return;

	m_dirty_mark[equip_index] = true;

	m_is_change = true;
	
	if (to_notice_client) 
	{
		this->SendAccessoriesmentInfoToRoleByIndex(equip_index);
	}
}

bool AccessoriesmentManager::GetAccessoriesmentGridData(short equip_index, ItemGridDataGetter *equip_data)
{
	if (NULL == equip_data || equip_index < 0 || equip_index >= Accessoriesment::A_INDEX_MAX) return false;

	if (m_accessories_list[equip_index].Invalid()) return false;

	m_accessories_list[equip_index].WrapTo(equip_data);

	return true;
}

void AccessoriesmentManager::GetAccessoriesmentList(int * count, QueryItemListParam::ItemListData * save_list)
{
	int tmp_count = 0;

	for (int i = 0; i < Accessoriesment::A_INDEX_MAX; ++i)
	{
		if (m_accessories_list[i].Invalid() || NULL == m_accessories_list[i].param) continue;

		save_list[tmp_count].index = i + ItemNamespace::ACCESSORIES_INDEX_BEGIN; // 装备存储时的index从这个开始
		m_accessories_list[i].WrapTo(&save_list[tmp_count].item_wrapper);

		save_list[tmp_count].change_state_item = structcommon::CS_NONE;

		if (++tmp_count >= *count) break;
	}

	*count = tmp_count;
}

void AccessoriesmentManager::GetChangeAccessoriesmentList(int * count, ItemListParam::ItemListData * save_list)
{
	if (!m_is_change)
	{
		*count = 0;
		return;
	}

	int tmp_count = 0;

	for (int i = 0; i < Accessoriesment::A_INDEX_MAX && tmp_count < *count; ++i)
	{
		if (m_dirty_mark[i])
		{
			save_list[tmp_count].index = i + ItemNamespace::ACCESSORIES_INDEX_BEGIN;

			if (!m_accessories_list[i].Invalid() && NULL != m_accessories_list[i].param)
			{
				m_accessories_list[i].WrapTo(&save_list[tmp_count].item_wrapper);

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

	*count = tmp_count;
}

void AccessoriesmentManager::ClearDirtyMark()
{
	if (!m_is_change) return;

	for (int i = 0; i < Accessoriesment::A_INDEX_MAX; ++i)
	{
		if (m_dirty_mark[i]) m_old_state[i] = !m_accessories_list[i].Invalid();

	}

	memset(m_dirty_mark, 0, sizeof(m_dirty_mark));

	m_is_change = false;
}

void AccessoriesmentManager::GetUserSpecialEffectList(int max_other_sp_num, short * out_other_sp_num, int * out_other_sp_list)
{
	//套装属性
	int suit_num = 0;
	int suit_type[Accessoriesment::A_INDEX_MAX] = { 0 };
	int suit_type_num[Accessoriesment::A_INDEX_MAX] = { 0 };
	int suit_color_max[Accessoriesment::A_INDEX_MAX] = { 0 };
	int suit_color_min[Accessoriesment::A_INDEX_MAX] = { 0 };
	int suit_color_min_2[Accessoriesment::A_INDEX_MAX] = { 0 };//二件套专用

	for (int i = 0; i < Accessoriesment::A_INDEX_MAX; ++i)
	{
		if (m_accessories_list[i].Invalid())continue;
		ItemGridData & access_data = m_accessories_list[i];
		const ItemBase * i_b = ITEMPOOL->GetItem(access_data.item_id);
		if (NULL == i_b)continue;

		{
			const Accessoriesment * a_b = (const Accessoriesment *)i_b;
			int cfg_suit = a_b->GetSuitType();
			if (cfg_suit > 0 && suit_num < Accessoriesment::A_INDEX_MAX)
			{
				bool is_new = true;
				for (int i = 0; i < Accessoriesment::A_INDEX_MAX && i < suit_num; i++)
				{
					if (suit_type[i] == cfg_suit)
					{
						suit_type_num[i] += 1;
						if (suit_color_min[i] > a_b->GetColor())
						{
							suit_color_min[i] = a_b->GetColor();
						}
						if(suit_color_max[i] < a_b->GetColor())
						{
							suit_color_max[i] = a_b->GetColor();
						}
						//专门2件套挑选最高品质的第二件
						if (suit_type_num[i] >= 2)
						{
							if (suit_color_max[i] >= a_b->GetColor() && suit_color_min_2[i] < a_b->GetColor())
							{
								if (suit_color_min_2[i] == 0)
								{
									suit_color_min_2[i] = suit_color_min[i];
								}
								else
								{
									suit_color_min_2[i] = a_b->GetColor();
								}
							}
						}
						is_new = false;
						break;;
					}
				}
				if (is_new)
				{
					suit_type[suit_num] += cfg_suit;
					suit_color_min[suit_num] = a_b->GetColor();
					suit_color_max[suit_num] = a_b->GetColor();
					suit_type_num[suit_num++] = 1;
				}
			}
		}
	}

	if (suit_num > 0)
	{
		for (int i = 0; i < suit_num && i < Accessoriesment::A_INDEX_MAX; i++)
		{
			{
				const PeishiSkillCfg * suit_cfg = LOGIC_CONFIG->GetPeiShiConfig()->GetPeiShiSkillCfg(suit_type[i], suit_color_min[i]);
				if (NULL != suit_cfg)
				{
					if (suit_type_num[i] >= 5)
					{
						if (suit_cfg->skill2_profession == 0 || m_module_mgr->GetRole()->GetAppearanceProf() == suit_cfg->skill2_profession)
						{
							const GamePassiveSkillCfg * passive_skill = SkillPool::Instance().GetPassiveSkillCfg(suit_cfg->skill_2);
							if (NULL == passive_skill) continue;
							if (suit_cfg->skill_level < 0 || suit_cfg->skill_level >= MAX_BATTLE_SKILL_LEVEL + 1)continue;
							int sp_id = passive_skill->level_spid_list[suit_cfg->skill_level];
							const SpecialEffectCfg * spe_cfg = BattleSpecialEffectPool::GetInstance().GetSpecialEffectCfg(sp_id);
							if (NULL != spe_cfg)
							{
								bool is_has_battle_effect = false;
								for (int j = 0; j < spe_cfg->effect_num && j < EFFECT_DETAIL_NUM; ++j)
								{
									const SpecialEffectKey & key = spe_cfg->effect_detail_list[j];
									if (key.effect_type == SPECIAL_EFFECT_TYPE_ADD_ATTR) continue;

									is_has_battle_effect = true;
									break;
								}

								if (is_has_battle_effect)
								{
									if (*out_other_sp_num < max_other_sp_num)
									{
										out_other_sp_list[(*out_other_sp_num)++] = sp_id;
									}
									else
									{
#ifdef _DEBUG
										assert(false);
#else
										gamelog::g_log_battle.printf(LL_INFO, "%s out_other_sp_num >= max_other_sp_num[%d] ", __FUNCTION__, max_other_sp_num);
#endif
										return;
									}
								}
							}
						}
					}
				}
			}
			{
				const PeishiSkillCfg * suit_cfg_2 = LOGIC_CONFIG->GetPeiShiConfig()->GetPeiShiSkillCfg(suit_type[i], suit_color_min_2[i]);
				if (NULL != suit_cfg_2)
				{
					if (suit_type_num[i] >= 2)
					{
						if (suit_cfg_2->skill1_profession == 0 || m_module_mgr->GetRole()->GetAppearanceProf() == suit_cfg_2->skill1_profession)
						{
							const GamePassiveSkillCfg * passive_skill = SkillPool::Instance().GetPassiveSkillCfg(suit_cfg_2->skill_1);
							if (NULL == passive_skill) continue;
							if (suit_cfg_2->skill_level < 0 || suit_cfg_2->skill_level >= MAX_BATTLE_SKILL_LEVEL + 1)continue;
							int sp_id = passive_skill->level_spid_list[suit_cfg_2->skill_level];
							const SpecialEffectCfg * spe_cfg = BattleSpecialEffectPool::GetInstance().GetSpecialEffectCfg(sp_id);
							if (NULL != spe_cfg)
							{
								bool is_has_battle_effect = false;
								for (int j = 0; j < spe_cfg->effect_num && j < EFFECT_DETAIL_NUM; ++j)
								{
									const SpecialEffectKey & key = spe_cfg->effect_detail_list[j];
									if (key.effect_type == SPECIAL_EFFECT_TYPE_ADD_ATTR) continue;

									is_has_battle_effect = true;
									break;
								}

								if (is_has_battle_effect)
								{
									if (*out_other_sp_num < max_other_sp_num)
									{
										out_other_sp_list[(*out_other_sp_num)++] = sp_id;
									}
									else
									{
#ifdef _DEBUG
										assert(false);
#else
										gamelog::g_log_battle.printf(LL_INFO, "%s out_other_sp_num >= max_other_sp_num[%d] ", __FUNCTION__, max_other_sp_num);
#endif
										return;
									}
								}
							}
						}
					}
				}
			}
		}
	}
}

bool AccessoriesmentManager::StrengthenGiveBack(const ItemGridData * back_item)
{
	if (NULL == back_item)return false;
	const ItemBase * i_b = ITEMPOOL->GetItem(back_item->item_id);
	if (NULL == i_b)return false;
	if (back_item->param == NULL)return false;
	if (i_b->GetItemType() != I_TYPE_ACCESSORIES)return false;
	
	int back_level = back_item->param->accessoriess_param.strengthen_level;
	std::vector<ItemConfigData> real_item_vec;
	if (back_level > 0)
	{
		for (int i = 0; i < back_level && i < MAX_PEISHI_STRENGTHEN_LEVEL; i++)
		{
			const PeiShiStrengthenCfg * level_cfg = LOGIC_CONFIG->GetPeiShiConfig()->GetPeiShiStrengthenCfg(back_item->item_id, i);
			if (NULL != level_cfg)
			{
				for (std::vector<ItemConfigData>::const_iterator it = level_cfg->cost_item_list.begin(); it != level_cfg->cost_item_list.end(); it++)
				{
					bool has_add = false;
					for (std::vector<ItemConfigData>::iterator it2 = real_item_vec.begin(); it2 != real_item_vec.end(); it2++)
					{
						if (it2->item_id == it->item_id)
						{
							it2->num += it->num;
							has_add = true;
							break;
						}
					}
					if (!has_add)
					{
						real_item_vec.push_back(*it);
						break;
					}
				}
			}
		}

		if ((int)real_item_vec.size() > 0 && !m_module_mgr->GetKnapsack()->CheckForPutList((int)real_item_vec.size(), &real_item_vec[0]))
		{
			m_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
			return false;
		}

		if((int)real_item_vec.size() > 0)
		{
			gamelog::g_log_accessories_ment.printf(LL_INFO, "%s user[%d,%s] item_id[%d] back_level[%d]", __FUNCTION__, m_module_mgr->GetUid(), m_module_mgr->GetRoleName(), back_item->item_id, back_level);
			m_module_mgr->GetKnapsack()->PutList((int)real_item_vec.size(), &real_item_vec[0], PUT_REASON_ACCESSORIES_BACK_STRENGTHEN);
		}
	}

	return true;
}
