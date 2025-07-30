#include "huashenmanager.hpp"

#include "servercommon/errornum.h"
#include "servercommon/rankdef.hpp"
#include "servercommon/performancecatch.hpp"
#include "servercommon/uniqueidgenerator.hpp"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "servercommon/internalprotocal/gglobalprotocal.h"
#include "servercommon/struct/battle/battle_special_effect_def.hpp"
#include "servercommon/serverconfig/battleconfig/battle_special_effect_pool.hpp"

#include "global/guild/guild.hpp"
#include "globalconfig/globalconfig.h"
#include "global/activity/activitymanager.hpp"
#include "global/rank/rankmanager.hpp"
#include "config/logicconfigmanager.hpp"
#include "config/otherconfig/professionconfig.hpp"
#include "config/otherconfig/scoresystemconfig.hpp"
#include "config/otherconfig/roleattrpelletconfig.hpp"
#include "config/otherconfig/smartmountssystemconfig.hpp"
#include "config/activityconfig/huanjiezhanchang/activityhuanjiezhanchangconfig.hpp"
#include "protocol/msgrole.h"

#include "gameworld/gamelog.h"
#include "gameworld/engineadapter.h"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "other/roleattribute/roleexpconfig.hpp"
#include "other/title/title.hpp"
#include "other/train/train.hpp"
#include "other/collection/collection.hpp"
#include "other/fabao/fabao.hpp"
#include "other/facescore/facescore.hpp"
#include "other/surface/surface.hpp"
#include "other/baby/baby.hpp"
#include "other/pet/petconfig.hpp"
#include "other/partner/partner.hpp"
#include "other/medal/medalconfig.hpp"
#include "other/train/trainconfig.hpp"
#include "other/capability/capability.hpp"
#include "other/funopen/funopen.hpp"
#include "other/funopen/funopendef.hpp"
#include "other/skin/skinconfig.hpp"
#include "other/event/eventhandler.hpp"
#include "other/rolelifeskill/rolenewlifeskill.hpp"
#include "other/roleguildinborn/roleguildinborn.hpp"
#include "other/roleheartskill/roleheartskill.hpp"
#include "other/smartmountssystem/smartmountssystem.hpp"
#include "other/roleattrpellet/roleattrpellet.hpp"
#include "item/knapsack.h"
#include "item/elementcrystal/elementcrystal.hpp"
#include "equipment/equipmentmanager.h"
#include "equipment/petequipment.h"
#include "scene/scene.h"
#include "scene/speciallogic.hpp"
#include "skill/skillpool.hpp"
#include "other/advancecareer/advancecareer.hpp"
#include "other/flyupsystem/flyupsystem.hpp"
#include "other/huanshou/rolehuanshou.hpp"

HuaShenManager::HuaShenManager() : m_module_mgr(NULL)
{
	this->Reset();
	m_curr_valid_grid_num = HUASHEN_ITEM_GRID_NUM_MAX;		//化身格子数量固定
}

HuaShenManager::~HuaShenManager()
{
	for (int i = 0; i < MAX_HUASHEN_PET_GRID_NUM; ++i)
	{
		m_pet_grid_list[i].Clear();
	}
	for (int i = 0; i < MAX_MEDAL_WEAR_GRID; i++)
	{
		m_medal_grid_list[i].Clear();
	}
	for (int i = 0; i < HUASHEN_ITEM_GRID_NUM_MAX; ++i)
	{
		if (NULL != m_grid_list[i])
		{
			this->ClearGrid(i);
		}
	}
}

void HuaShenManager::Init(RoleModuleManager * module_mgr, int param_list_num, const ItemListParam::ItemListData *param_data_list[], 
							const HuaShenParam & param, int pet_param_list_num, const ItemListParam::ItemListData *pet_param_data_list[], 
							int medal_param_list_num, const ItemListParam::ItemListData *medal_param_data_list[])
{
	m_module_mgr = module_mgr;
	m_param = param;

	if (0 == m_param.use_new_pet_status_array)
	{
		m_param.use_new_pet_status_array = 1;
		for (int i = 0; i < PET_STATUS_MAX_OBSOLETE; ++i)
		{
			m_param.new_pet_status_index[i] = m_param.pet_status_index_OBSOLETE[i];
		}
		UNSTD_STATIC_CHECK((int)PET_STATUS_MAX_OBSOLETE <= (int)NEW_PET_STATUS_MAX);
	}

	int biggest_knapsack_index = -1;
	for (int i = param_list_num - 1; i >= 0; --i)
	{
		if (NULL == param_data_list[i]) break;

		if (param_data_list[i]->index < HUASHEN_ITEM_INDEX_BEGIN || param_data_list[i]->index >= HUASHEN_ITEM_INDEX_END)
		{
			gamelog::g_log_serious_error.printf(LL_CRITICAL, "[%s] role[%s %d] invalid index,  index[%d]", __FUNCTION__,
				m_module_mgr->GetRole()->GetName(), UidToInt(m_module_mgr->GetRole()->GetUserId()), param_data_list[i]->index);
			continue;
		}
		int real_index = param_data_list[i]->index - HUASHEN_ITEM_INDEX_BEGIN;

		if (NULL != m_grid_list[real_index])
		{
			gamelog::g_log_serious_error.printf(LL_CRITICAL, "[HuaShenManager::Init Duplicate] role[%s %d] index[%d] real_index[%d] old_item_id[%d] new_item_id[%d] old_has_param[%d]",
				m_module_mgr->GetRole()->GetName(), UidToInt(m_module_mgr->GetRole()->GetUserId()), param_data_list[i]->index, real_index,
				(int)m_grid_list[real_index]->item_id, (int)param_data_list[i]->item_wrapper.item_id,
				(int)(NULL != m_grid_list[real_index]->param));
			continue;
		}

		const ItemBase *itembase = ITEMPOOL->GetItem(param_data_list[i]->item_wrapper.item_id);
		if (NULL == itembase || param_data_list[i]->item_wrapper.num <= 0 || param_data_list[i]->item_wrapper.num > itembase->GetPileLimit())
		{
			gamelog::g_log_serious_error.printf(LL_CRITICAL, "[HuaShenManager::Init Error][[%s %d] index:%d  real_index[%d] item_id:%d num:%d]", m_module_mgr->GetRole()->GetName(), UidToInt(m_module_mgr->GetRole()->GetUserId()),
				param_data_list[i]->index, real_index, (int)param_data_list[i]->item_wrapper.item_id, (int)param_data_list[i]->item_wrapper.num);
			continue;
		}

		if (0 != param_data_list[i]->item_wrapper.has_param && itembase->GetItemType() < I_TYPE_NET_VALUE_ITEM_BEGIN)
		{
			gamelog::g_log_serious_error.printf(LL_CRITICAL, "[HuaShenManager::Init HasParamButIsNotNetValueItem][[%s %d] index:%d  real_index[%d] item_id:%d]", m_module_mgr->GetRole()->GetName(), UidToInt(m_module_mgr->GetRole()->GetUserId()),
				param_data_list[i]->index, real_index, (int)param_data_list[i]->item_wrapper.item_id);
			continue;
		}

		m_grid_list[real_index] = new ItemGridData();
		if (0 != param_data_list[i]->item_wrapper.has_param)
		{
			m_grid_list[real_index]->param = ItemBase::CreateParamFromParamData(param_data_list[i]->item_wrapper.param_data);
		}
		m_grid_list[real_index]->item_id = param_data_list[i]->item_wrapper.item_id;
		m_grid_list[real_index]->num = param_data_list[i]->item_wrapper.num;
		m_grid_list[real_index]->is_bind = (0 != param_data_list[i]->item_wrapper.is_bind ? true : false);
		m_grid_list[real_index]->invalid_time = param_data_list[i]->item_wrapper.invalid_time;
		m_grid_list[real_index]->gold_price = param_data_list[i]->item_wrapper.gold_price;
		m_grid_list[real_index]->item_unique_id = param_data_list[i]->item_wrapper.item_unique_id;

		if (m_grid_list[real_index]->invalid_time > 0 && !itembase->HasInvalidTime())
		{
			m_grid_list[real_index]->invalid_time = 0;
		}
		m_old_state[real_index] = true;
		this->PutOn(real_index, m_grid_list[real_index]->item_unique_id, HUASHEN_GRID_INDEX_TYPE_KNAPSACK);
		if (biggest_knapsack_index < real_index && real_index < HUASHEN_ITEM_GRID_NUM_MAX) // 记录下背包有效格子的最大序号
		{
			biggest_knapsack_index = real_index;
		}
		gamelog::g_log_knapsack.printf(LL_INFO, "%s role[%d,%s] item[%d,%s] num[%d] from index[%d]", __FUNCTION__, m_module_mgr->GetRole()->GetUID(), m_module_mgr->GetRole()->GetName(),
			param_data_list[i]->item_wrapper.item_id, itembase->GetItemName(), param_data_list[i]->item_wrapper.num, real_index);
	}

	m_valid_grid_index_end = biggest_knapsack_index + 1;

	m_pet_count = 0;
	for (int i = 0; i < pet_param_list_num; ++i)
	{
		const ItemListParam::ItemListData* item_data = pet_param_data_list[i];
		if (NULL == item_data) continue;

		if (ItemNamespace::PET_HUASHEN_INDEX_BEGIN <= item_data->index && item_data->index < ItemNamespace::PET_HUASHEN_INDEX_END)
		{
			int pet_index = item_data->index - ItemNamespace::PET_HUASHEN_INDEX_BEGIN;
			ItemGridData & grid = m_pet_grid_list[pet_index];
			if (!grid.Invalid())
			{
				gamelog::g_log_serious_error.printf(LL_INFO, "[%s Error Duplicate][[%s %d] pet_index[%d] old_pet_id[%d] new_pet_id[%d]]", __FUNCTION__,
					m_module_mgr->GetRole()->GetName(), m_module_mgr->GetRole()->GetUID(), pet_index, (int)grid.item_id, (int)item_data->item_wrapper.item_id);
				continue;
			}

			grid.item_id = item_data->item_wrapper.item_id;
			grid.num = 1;
			grid.is_bind = (0 != item_data->item_wrapper.is_bind);
			grid.param = ItemBase::CreateParamFromParamData(item_data->item_wrapper.param_data);
			grid.item_unique_id = item_data->item_wrapper.item_unique_id;
			this->PutOn(pet_index, grid.item_unique_id, HUASHEN_GRID_INDEX_TYPE_PET);
			if (grid.param->pet_param.equipment_item_unique_id != 0)
			{
				this->PutOn(pet_index, grid.param->pet_param.equipment_item_unique_id, HUASHEN_GRID_INDEX_TYPE_PET_EQUIPMENT);
			}
#ifdef _DEBUG
			if (BATTLE_MOVE_TYPE_PERFORM_SKILL == grid.param->pet_param.pet_auto_move_type &&
				grid.param->pet_param.pet_auto_skill_level <= 0)
			{
				grid.param->pet_param.pet_auto_move_type = BATTLE_MOVE_TYPE_NORMAL_ATTACK;
				grid.param->pet_param.pet_auto_skill_id = 0;
				grid.param->pet_param.pet_auto_skill_level = 0;
			}
#endif
			m_pet_old_state[pet_index] = true;
			++m_pet_count;
		}
	}

	for (int i = 0; i < medal_param_list_num; ++i)
	{
		const ItemListParam::ItemListData* item_data = medal_param_data_list[i];
		if (NULL == item_data) continue;

		if (ItemNamespace::HUASHEN_MEDAL_INDEX_BEGIN <= item_data->index && item_data->index < ItemNamespace::HUASHEN_MEDAL_INDEX_END)
		{
			int medal_index = item_data->index - ItemNamespace::HUASHEN_MEDAL_INDEX_BEGIN;
			ItemGridData & grid = m_medal_grid_list[medal_index];

			grid.item_id = item_data->item_wrapper.item_id;
			grid.num = 1;
			grid.is_bind = (0 != item_data->item_wrapper.is_bind);
			grid.param = ItemBase::CreateParamFromParamData(item_data->item_wrapper.param_data);
			grid.item_unique_id = item_data->item_wrapper.item_unique_id;
			MedalItemParam & medal_param = grid.param->medal_param;
			medal_param.CheckModifyVersion();
			this->PutOn(medal_index, grid.item_unique_id, HUASHEN_GRID_INDEX_TYPE_MEDAL);

			m_medal_old_state[medal_index] = true;
		}
	}

	//max_role_train_level = LOGIC_CONFIG->GetTrainConfig()->GetMaxTrainLevel(m_param.max_level);
	max_pet_train_level = LOGIC_CONFIG->GetTrainConfig()->GetPetTrainMaxLevelByRoleLevel(m_param.max_level);
}

void HuaShenManager::GetInitParam(HuaShenParam * param)
{
	*param = m_param;
}

void HuaShenManager::InitEnd()
{
	if (this->IsSeasonInTime())
	{
		m_module_mgr->GetPet()->HuaShenSkinSyn(&m_pet_has_skin_id);
	}

	//初始化结束之后再重算宠物各属性
	for (int i = ItemNamespace::PET_HUASHEN_INDEX_BEGIN; i < ItemNamespace::PET_HUASHEN_INDEX_END; i++)
	{
		int pet_index = i - ItemNamespace::PET_HUASHEN_INDEX_BEGIN;
		if(this->PetInvalid(pet_index)) continue;

		this->ReCalcPetAttr(pet_index);
	}
}

void HuaShenManager::Update(time_t now_second)
{
	if(m_param.season_reset == 0) return;
	
	if (now_second >= m_next_rank_update_time)
	{
		m_next_rank_update_time = PERSON_RANK_UPDATE_TO_GLOBAL_TIME + static_cast<unsigned int>(now_second);
	}
}

void HuaShenManager::OnSeasonFirstReset()
{
	if(!ActivityManager::Instance().IsActivityOpen(ACTIVITY_TYPE_HUAN_JIE_ZHAN_CHANG)) return; 
	if(m_param.season_reset != 0 || !FunOpen::IsFunOpen(m_module_mgr->GetRole(), FUN_TYPE_HUANJIEZHANCHANG)) return;
	
	////////////////////////////////////赛季重置数据/////////////////////////////////
	m_param.SeasonReset(true);
	this->SeasonReset();
//	m_module_mgr->GetEquipmentManager()->ClearHuaShenEquipAndJewelry();		//清除化身装备信息

	//初始化一些所需要数据
	m_param.season_reset = 1;		//需要先设置重置,否则无法同步数据
	m_param.max_level = LOGIC_CONFIG->GetActivityHuanJieZhanChangConfig()->GetOtherCfg().level_limit;	
	m_param.plan_reset_flag = 1;	//新号不需要重置
	m_param.is_rule_change_flag = 5;	
	//max_role_train_level = LOGIC_CONFIG->GetTrainConfig()->GetMaxTrainLevel(m_param.max_level);
	max_pet_train_level = LOGIC_CONFIG->GetTrainConfig()->GetPetTrainMaxLevelByRoleLevel(m_param.max_level);
	this->CalcMaxAdvTimes();
	//////////////////////////////////同步所需数据/////////////////////////////////
	//技能信息 1.玩家战斗 2.修炼技能
	m_module_mgr->GetRoleSkill()->GetSkillPosition();
	this->SynBenZunTrain();

	//1.宠物皮肤 2.所有宠物
	m_module_mgr->GetPet()->HuaShenSkinSyn(&m_pet_has_skin_id);
	this->SynBenZunAllPetData();

	//道具 1.本尊穿戴的装备 2.勋章 3.背包   （本尊与化身穿戴的装备栏一致）
	m_module_mgr->GetKnapsack()->SynHuaShenItemData();
	m_module_mgr->GetMedal()->SynHuaShenMedalGrid();
	m_module_mgr->GetEquipmentManager()->SynHuaShenEquipAndJewelry();

	//潜能
	for (int i = 0; i < MAX_HUASHEN_ATTR_PLAN_NUM; i++)
	{
		m_param.attr_plan_list[i].Reset();
		m_param.attr_plan_list[i].remain_point = (m_param.max_level - 1) * UPLEVEL_ADD_ATTR_POINT + ATTR_BASE_POINT_MAX_NUM;
	}


	//法宝信息(化身法宝暂时砍掉)
//	m_module_mgr->GetFaBao()->SynFaBaoEquipmentToHuaShen(m_param.m_fabao_equipment_bar, FABAO_EQUIP_BAR_MAX);

	this->ReCalcAttr(AttributeList::RECALC_REASON_INVALID, __FUNCTION__, true);
	//下发所有信息
	this->SendAllHuaShenInfo();
}

void HuaShenManager::OnSeasonEndReset(bool is_reset_dingji_flag)
{
	m_param.SeasonReset(is_reset_dingji_flag);
	this->SeasonReset();
	//m_module_mgr->GetEquipmentManager()->ClearHuaShenEquip();
	this->SendSeasonInfo();
}

void HuaShenManager::OnMonthChange()
{
	m_param.season_reset_first_dijing = 0;
	//策划让暂时每月不重置数据
	//this->OnSeasonEndReset();
}

void HuaShenManager::OnEnterKeySynchronize(bool is_notice)
{
	if (m_param.season_reset == 0) return;

	if (m_param.plan_reset_flag == 0)
	{
		//潜能
		for (int i = 0; i < MAX_HUASHEN_ATTR_PLAN_NUM; i++)
		{
			m_param.attr_plan_list[i].Reset();
			m_param.attr_plan_list[i].remain_point = (60 - 1) * UPLEVEL_ADD_ATTR_POINT + ATTR_BASE_POINT_MAX_NUM;
		}
		m_param.plan_reset_flag = 1;
	}

	//1.技能信息 1.玩家战斗 2.修炼技能
	m_module_mgr->GetRoleSkill()->GetSkillPosition();
	this->SynBenZunTrain();

	std::set<long long> m_benzun_item_set;
	//2.宠物 1.宠物皮肤 2.宠物数据 (皮肤先同步,后面同步宠物数据会重算属性)
	m_module_mgr->GetPet()->HuaShenSkinSyn(&m_pet_has_skin_id);
	m_module_mgr->GetPet()->HuaShenKeySyn(&m_benzun_item_set);

	//3.道具 1.穿戴的装备 2.勋章 3.背包
	m_module_mgr->GetEquipmentManager()->HuaShenKeySyn(&m_benzun_item_set);
	m_module_mgr->GetMedal()->SynHuaShenMedal(&m_benzun_item_set);
	m_module_mgr->GetKnapsack()->SynHuaShenItemData(&m_benzun_item_set);

	int delete_num = 0;
	//用此时化身存储信息与先前的同步数据来删除一些本尊没有的数据
	if (!m_benzun_item_set.empty())
	{
		for (std::map<long long, int>::iterator it = m_item_map.begin(); it != m_item_map.end();)
		{
			if(m_benzun_item_set.end() == m_benzun_item_set.find(it->first))
			{
				if (this->ConsumeItem(it->second, false, true))
				{
					m_item_map.erase(it++);
					delete_num++;
				}
				else
				{
					it++;
				}
			}
			else
			{
				it++;
			}
		}
	}
	else
	{
		this->ClearAllItemData();
	}
	//延迟处理的道具 目前暂时需要延迟处理的只有宠物护符(宠物删除,宠物身上穿戴的护符自动脱下到背包), 暂时还不用区分类型处理
	if (!m_delay_handle_map.empty())
	{
		for (std::map<long long, ItemDataWrapper>::iterator it = m_delay_handle_map.begin(); it != m_delay_handle_map.end(); it++)
		{
			this->TakeOff(it->first);
			if (m_benzun_item_set.end() == m_benzun_item_set.find(it->first))
			{
				delete_num++;
			}
			else
			{
				this->Put(it->second);
			}		
		}
		m_delay_handle_map.clear();
	}

	//检测此时本尊与化身穿戴的装备是否一致
	this->CheckAllItemIdentical();

	//自动整理
	if (delete_num >= MAX_HUASHEN_DELETE_ITEM_NUM)
	{
		this->InOrder(true);
	}
	if (is_notice)
	{
		//登陆的时候判断一下进阶等级,防止策划改表
		this->CalcMaxAdvTimes();
		//m_module_mgr->NoticeNum(errornum::EN_ACTIVITY_HUANJIEZHANCHANG_HUASHEN_SYN_SUCC);
	}

	this->ReCalcAttr(AttributeList::RECALC_REASON_INVALID, __FUNCTION__, true);

	this->SendAllHuaShenInfo();
}

void HuaShenManager::OnSynKnapackData(bool is_element_crystal)
{
	m_module_mgr->GetKnapsack()->SynHuaShenItemData(NULL, is_element_crystal);
	this->SendKnapsackInfo();
}

bool HuaShenManager::IsSeasonInTime()
{
	return (0 != m_param.season_reset);
}

void HuaShenManager::OnRoleLogin()
{
	if (!FunOpen::IsFunOpen(m_module_mgr->GetRole(), FUN_TYPE_HUANJIEZHANCHANG)) return;
	if (5 != m_param.is_rule_change_flag)
	{
		this->OnSeasonEndReset(false);
		return;
	}

	if (!this->IsSeasonInTime()) return;

	this->OnEnterKeySynchronize(false);
}

void HuaShenManager::SetRoleInfo(int rank_score, int is_finis_ding_ji)
{
	m_param.rank_score = rank_score;
	m_param.is_finish_ding_ji = 0 != is_finis_ding_ji ? 1 : 0;
	EventHandler::Instance().OnHuanJieZhanChanDataChange(m_module_mgr->GetRole(), m_param.rank_score);
}

QueryHuanJieInfo HuaShenManager::GetQueryInfo()
{
	QueryHuanJieInfo info;
	info.rank_score = m_param.rank_score;
	info.is_finish_ding_ji = m_param.is_finish_ding_ji;

	return info;
}

void HuaShenManager::ReCalcAttr(int recalc_reason, const char * recalc_reason_str, bool is_init)
{
	if(m_param.season_reset == 0) return;

	bool recalc_all = (AttributeList::RECALC_REASON_INVALID == recalc_reason || is_init);

	//初始化
	m_attr_list.Reset();
	for (int i = 0; i < MAX_REASON_TYPE_LIST_NUM; i++)
	{
		m_role_attr_reason_list[i].Reset();
	}
	
	//PS: 如果有其他同时会出现小数数值的系统，需要一同计算，否则会出现误差
	//计算自身属性
	m_role_attr_reason_list[ROLE_REASON_LIST_TYPE_BASE] = m_module_mgr->ReCalcSelfAttr(m_attr_list, AttributeList::RECALC_REASON_SELF == recalc_reason || recalc_all);
	m_role_attr_reason_list[ROLE_REASON_LIST_TYPE_POINT] = this->ReCalcAttrPlan(m_attr_list, AttributeList::RECALC_REASON_ATTRIBUTE_PLAN == recalc_reason || recalc_all);
	const AttributeList basic_attr_list = m_attr_list;		// 自身属性表

	// 各个系统的加成
	AttributeList skill_attr_list;			//某些技能是直接配的评分,则计算评分时,属性列表应该剔除这些技能所增加的属性
	m_role_attr_reason_list[ROLE_REASON_LIST_TYPE_COLLE] = m_module_mgr->GetCollection()->ReCalcAttr(m_attr_list, AttributeList::RECALC_REASON_COLLECTION == recalc_reason || recalc_all);
	m_role_attr_reason_list[ROLE_REASON_LIST_TYPE_OTHER].Add(m_module_mgr->GetTitle()->ReCalcAttr(m_attr_list, AttributeList::RECALC_REASON_TITLE == recalc_reason || recalc_all).m_attrs);
	m_role_attr_reason_list[ROLE_REASON_LIST_TYPE_OTHER].Add(m_module_mgr->GetPartner()->ReCalcAttr(m_attr_list, AttributeList::RECALC_REASON_PARTNER == recalc_reason || recalc_all).m_attrs);
	m_role_attr_reason_list[ROLE_REASON_LIST_TYPE_OTHER].Add(m_module_mgr->GetSurface()->ReCalcAttr(m_attr_list, AttributeList::RECALC_REASON_SURFACE == recalc_reason || recalc_all).m_attrs);
	m_role_attr_reason_list[ROLE_REASON_LIST_TYPE_OTHER].Add(m_module_mgr->GetBaby()->ReCalcAttr(m_attr_list, AttributeList::RECALC_REASON_BABY == recalc_reason || recalc_all).m_attrs);
	AttributeList equip_special_attr;	//获取熔炼属性,到其他系统进行限制
	m_role_attr_reason_list[ROLE_REASON_LIST_TYPE_EQUIP].Add(m_module_mgr->GetEquipmentManager()->ReCalcAttr(m_attr_list, AttributeList::RECALC_REASON_EQUIPMENT == recalc_reason || recalc_all, ROLE_TYPE_HUASHEN, &equip_special_attr).m_attrs);
	m_role_attr_reason_list[ROLE_REASON_LIST_TYPE_FABAO].Add(m_module_mgr->GetFaBao()->ReCalcAttr(m_attr_list, AttributeList::RECALC_REASON_FABAO == recalc_reason || recalc_all).m_attrs);
	m_role_attr_reason_list[ROLE_REASON_LIST_TYPE_MEDAL] = this->ReCalcMedalAttr(m_attr_list, AttributeList::RECALC_REASON_MEDAL == recalc_reason || recalc_all);
	m_role_attr_reason_list[ROLE_REASON_LIST_TYPE_OTHER].Add(m_module_mgr->GetFaceScore()->ReCalcAttr(m_attr_list, AttributeList::RECALC_REASON_FACE_SCORE == recalc_reason || recalc_all).m_attrs);
	m_role_attr_reason_list[ROLE_REASON_LIST_TYPE_ADV] = this->ReCalcAdvAttr(m_attr_list, AttributeList::RECALC_REASON_PROF_ADVAN == recalc_reason || recalc_all);
	m_role_attr_reason_list[ROLE_REASON_LIST_TYPE_SMS] = m_module_mgr->GetSmartMountsSystem()->ReCalcAttr(m_attr_list, AttributeList::RECALC_REASON_SMART_MOUNTS_SYSTEM == recalc_reason || recalc_all, skill_attr_list);
	m_role_attr_reason_list[ROLE_REASON_LIST_TYPE_OTHER].Add(m_module_mgr->GetRoleGuildInborn()->ReCalcAttr(m_attr_list, AttributeList::RECALC_REASON_GUILD_INBORN == recalc_reason || recalc_all, skill_attr_list).m_attrs);
	m_role_attr_reason_list[ROLE_REASON_LIST_TYPE_OTHER].Add(m_module_mgr->GetRoleHeartSkill()->ReCalcAttr(m_attr_list, AttributeList::RECALC_REASON_HEART_SKILL == recalc_reason || recalc_all).m_attrs);
	m_role_attr_reason_list[ROLE_REASON_LIST_TYPE_PELLET].Add(m_module_mgr->GetRoleAttrPellet()->ReCalcAttr(m_attr_list, AttributeList::RECALC_REASON_ATTR_PELLET == recalc_reason || recalc_all).m_attrs);
	m_role_attr_reason_list[ROLE_REASON_LIST_TYPE_HUANSHOU].Add(m_module_mgr->GetRoleHuanShou()->ReCalcAttr(m_attr_list, AttributeList::RECALC_REASON_HUAN_SHOU == recalc_reason || recalc_all).m_attrs);

	m_role_attr_reason_list[ROLE_REASON_LIST_TYPE_TRAIN].Add(this->ReCalcTrainAttr(m_attr_list, true).m_attrs);		//需要防御值计算，所以每次都需要重新计算一遍
	m_role_attr_reason_list[ROLE_REASON_LIST_TYPE_BASE].Add(this->ReCalcSkillAttr(m_attr_list, AttributeList::RECALC_REASON_ROLE_SKILL == recalc_reason || recalc_all).m_attrs);
	///////////////////////////////////这后面就是百分比提升的属性了/////////////////////////////////////////////////
	const AttributeList copy_attr_list = m_attr_list;		// 总属性表

	// 只计算自身属性百分比加成的属性
	m_module_mgr->GetEquipmentManager()->ReCalcAttrPer(m_attr_list, basic_attr_list);

	// 计算了各系统加成的百分比属性
	m_role_attr_reason_list[ROLE_REASON_LIST_TYPE_OTHER].Add(m_module_mgr->GetRole()->GetScene()->GetSpecialLogic()->ReCalcAttrPer(m_attr_list, copy_attr_list).m_attrs);
	m_role_attr_reason_list[ROLE_REASON_LIST_TYPE_TRAIN].Add(this->ReCalcTrainAttrPer(m_attr_list, copy_attr_list).m_attrs);
	m_role_attr_reason_list[ROLE_REASON_LIST_TYPE_FABAO].Add(m_module_mgr->GetFaBao()->ReCalcAttrPer(m_attr_list, copy_attr_list).m_attrs);

	m_attr_list.m_attrs[BATTLE_ATTR_CUR_HP] = m_attr_list.m_attrs[BATTLE_ATTR_MAX_HP];
	m_attr_list.m_attrs[BATTLE_ATTR_CUR_MP] = m_attr_list.m_attrs[BATTLE_ATTR_MAX_MP];

	AttributeList no_has_skill_attr_list = m_attr_list;
	no_has_skill_attr_list.Deduct(skill_attr_list.m_attrs);

	//此时如果有限制,评分计算需要在没有技能加成属性列表中再剔除装备(不包含熔炼),才能获得其他系统属性进行限制
	const ActivityHuanJieZhanChangEquipAttrLimitCfg * limit_cfg = LOGIC_CONFIG->GetActivityHuanJieZhanChangConfig()->GetEquipAttrLimitCfg(HUAN_JIE_HUA_SHEN_ATTR_LIMIT_TYPE_OTHER, m_module_mgr->GetRole()->GetAppearanceProf(), 0);
	if (NULL != limit_cfg)
	{
		AttributeList other_attr_list = no_has_skill_attr_list;
		AttributeList single_equip_attr = m_role_attr_reason_list[ROLE_REASON_LIST_TYPE_EQUIP];
		single_equip_attr.Deduct(equip_special_attr.m_attrs);	//不参杂熔炼的装备属性

		other_attr_list.Deduct(single_equip_attr.m_attrs);
		for (int i = 0; i < HJZC_EQUIP_ATTR_LIMIT_MAX_NUM; i++)
		{
			const ActivityHuanJieZhanChangEquipAttrLimitCfg::AttrConCfg & con_cfg = limit_cfg->attr_limit_list[i];
			if (other_attr_list.m_attrs[con_cfg.attr_type] > con_cfg.attr_value)
			{
				other_attr_list.m_attrs[con_cfg.attr_type] = con_cfg.attr_value;
			}
		}
		other_attr_list.Add(single_equip_attr.m_attrs);
		no_has_skill_attr_list.Set(other_attr_list.m_attrs);

		AttributeList real_attr_list = m_attr_list;
		real_attr_list.Deduct(single_equip_attr.m_attrs);	//实际的其它系统属性
		for (int i = 0; i < HJZC_EQUIP_ATTR_LIMIT_MAX_NUM; i++)
		{
			const ActivityHuanJieZhanChangEquipAttrLimitCfg::AttrConCfg & con_cfg = limit_cfg->attr_limit_list[i];
			if (real_attr_list.m_attrs[con_cfg.attr_type] > con_cfg.attr_value)
			{
				real_attr_list.m_attrs[con_cfg.attr_type] = con_cfg.attr_value;
			}
		}

		real_attr_list.Add(single_equip_attr.m_attrs);
		m_attr_list.Set(real_attr_list.m_attrs);
	}

	m_module_mgr->GetCapability()->GetHuaShenAttrListParam(no_has_skill_attr_list);
	if (!is_init)
	{
		this->SendRoleAttrList();
	}

	RankManager::Instance().SyncPersonRankInfo(m_module_mgr->GetRole(), PERSON_RANK_TYPE_HUASHEN_CAPABILITY);
}

void HuaShenManager::SendRoleAttrReasonList(int param1)
{
	Protocol::SCAttrListReason info;
	info.reason_type = Protocol::SCAttrListReason::REASON_TYPE_HUASHEN_ROLE;
	info.param1 = param1;
	for (int i = 0; i < MAX_REASON_TYPE_LIST_NUM; i++)
	{
		info.reason_list[i] = m_role_attr_reason_list[i];
	}

	m_module_mgr->NetSend(&info, sizeof(info));
}

int HuaShenManager::GetPetAttrReasonList(int pet_index)
{
#ifdef _DEBUG
	PerformanceCatch::Instance().GetElapseUsBegin(PERFORMANCE_TEST_PET_TIME_RECALC);
#endif

	if (this->PetInvalid(pet_index)) return -1;
	PetParam& pet = m_pet_grid_list[pet_index].param->pet_param;
	const PetCfg *cfg = LOGIC_CONFIG->GetPetConfig()->GetPetCfg(pet.pet_id);
	if (NULL == cfg) return -2;

	const PetStrengthenCfg * str_cfg = LOGIC_CONFIG->GetPetConfig()->GetPetStrengthenCfg(cfg->strengthen_cfg_id, pet.str_level);
	if (NULL == str_cfg) return -3;

	int str_buff_level = this->GetPetStrBuffLevel(pet_index);

	//初始化
	for (int i = 0; i < MAX_REASON_TYPE_LIST_NUM; i++)
	{
		m_pet_attr_reason_list[i].Reset();
	}

	double total_attr[ADD_POINT_TYPE_MAX] = { 0 };
	// 等级+资质+强化+潜能+共鸣获得的属性
	for (int i = 0; i < ADD_POINT_TYPE_MAX; ++i)
	{
		// 资质 自身 * 强化提升率
		double growth = pet.growth[i] + str_cfg->add_growth[i] / 100.0;

		// 玩家分配的属性点
		double attr_point = pet.attr[i] * 1.0;

		total_attr[i] += (growth * (PET_ATTR_BASE_UP + (pet.level - 1) * PET_ATTR_LEVEL_UP)) + attr_point;
	}
	
	double base_attrs[BATTLE_ATTR_MAX] = { 0 };		//基础属性(强化+资质+共鸣+潜能+突破)(与属性来源对齐)
	AttributeList base_attr_list;					//基础属性(强化+资质+基础+潜能+突破)
	for (int i = 0; i < ADD_POINT_TYPE_MAX; ++i)
	{
		const PetAttrExchangeCfg::ItemCfg* attr_cfg = LOGIC_CONFIG->GetPetConfig()->GetAttrExchange(0, i);
		if (NULL != attr_cfg)
		{
			base_attrs[BATTLE_ATTR_MAX_HP] += total_attr[i] * attr_cfg->maxhp;
			base_attrs[BATTLE_ATTR_MAX_MP] += total_attr[i] * attr_cfg->maxmp;
			base_attrs[BATTLE_ATTR_ATTACK] += total_attr[i] * attr_cfg->gongji;
			base_attrs[BATTLE_ATTR_DEFENSE] += total_attr[i] * attr_cfg->fangyu;
			base_attrs[BATTLE_ATTR_AGILITY] += total_attr[i] * attr_cfg->minjie;
			base_attrs[BATTLE_ATTR_RECOVERY] += total_attr[i] * attr_cfg->huifu;
			base_attrs[BATTLE_ATTR_MENTAL] += total_attr[i] * attr_cfg->jingshen;
		}
	}

	const PetBreakAttrCfg * pet_break_attr_cfg = LOGIC_CONFIG->GetPetConfig()->GetPetBreakAttrCfg(pet.level, pet.is_up_level_break);
	if (pet_break_attr_cfg != NULL)
	{
		for (int i = 0; i < ADD_POINT_TYPE_MAX; ++i)
		{
			std::map<int, PetBreakAttrCfg::AttrItem>::const_iterator break_attr_iter = pet_break_attr_cfg->attr_item.find(i);
			if (break_attr_iter != pet_break_attr_cfg->attr_item.end())
			{
				const PetBreakAttrCfg::AttrItem & break_attr = break_attr_iter->second;
				// 血蓝
				base_attrs[BATTLE_ATTR_MAX_HP] += total_attr[i] * break_attr.maxhp;
				base_attrs[BATTLE_ATTR_MAX_MP] += total_attr[i] * break_attr.maxmp;
				// 基础属性
				base_attrs[BATTLE_ATTR_ATTACK] += total_attr[i] * break_attr.gongji;
				base_attrs[BATTLE_ATTR_DEFENSE] += total_attr[i] * break_attr.fangyu;
				base_attrs[BATTLE_ATTR_AGILITY] += total_attr[i] * break_attr.minjie;
				base_attrs[BATTLE_ATTR_RECOVERY] += total_attr[i] * break_attr.huifu;
				base_attrs[BATTLE_ATTR_MENTAL] += total_attr[i] * break_attr.jingshen;
			}
		}
	}

	// 基础属性
	// 血蓝
	base_attr_list.m_attrs[BATTLE_ATTR_MAX_HP] += static_cast<Attribute>(cfg->maxhp + str_cfg->maxhp + base_attrs[BATTLE_ATTR_MAX_HP] / 100.0 + 1e-6);
	base_attr_list.m_attrs[BATTLE_ATTR_MAX_MP] += static_cast<Attribute>(cfg->maxmp + str_cfg->maxmp + base_attrs[BATTLE_ATTR_MAX_MP] / 100.0 + 1e-6);
	// 基础属性
	base_attr_list.m_attrs[BATTLE_ATTR_ATTACK] += static_cast<Attribute>(cfg->gongji + str_cfg->gongji + base_attrs[BATTLE_ATTR_ATTACK] / 100.0 + 1e-6);
	base_attr_list.m_attrs[BATTLE_ATTR_DEFENSE] += static_cast<Attribute>(cfg->fangyu + str_cfg->fangyu + base_attrs[BATTLE_ATTR_DEFENSE] / 100.0 + 1e-6);
	base_attr_list.m_attrs[BATTLE_ATTR_AGILITY] += static_cast<Attribute>(cfg->minjie + str_cfg->minjie + base_attrs[BATTLE_ATTR_AGILITY] / 100.0 + 1e-6);
	base_attr_list.m_attrs[BATTLE_ATTR_RECOVERY] += static_cast<Attribute>(cfg->huifu + str_cfg->huifu + base_attrs[BATTLE_ATTR_RECOVERY] / 100.0 + 1e-6);
	base_attr_list.m_attrs[BATTLE_ATTR_MENTAL] += static_cast<Attribute>(cfg->jingshen + str_cfg->jingshen + base_attrs[BATTLE_ATTR_MENTAL] / 100.0 + 1e-6);
	// 修正属性
	base_attr_list.m_attrs[BATTLE_ATTR_CRITICAL] += (Attribute)(cfg->bisha);
	base_attr_list.m_attrs[BATTLE_ATTR_HIT] += (Attribute)(cfg->mingzhong);
	base_attr_list.m_attrs[BATTLE_ATTR_DODGE] += (Attribute)(cfg->shanduo);
	base_attr_list.m_attrs[BATTLE_ATTR_COUNTER_ATTACK] += (Attribute)(cfg->fanji);
	base_attr_list.m_attrs[BATTLE_ATTR_MAGIC_ATTACK] += (Attribute)(cfg->mogong);
	base_attr_list.m_attrs[BATTLE_ATTR_MAGIC_DEFENSE] += (Attribute)(cfg->mokang);
	base_attr_list.m_attrs[BATTLE_ATTR_CRITICAL_INC_VALUE] += (Attribute)(cfg->bisha_dmg);
	base_attr_list.m_attrs[BATTLE_ATTR_CRITICAL_DEC_VALUE] += (Attribute)(cfg->jianren);
	// 异常抗性
	base_attr_list.m_attrs[BATTLE_ATTR_ANTI_POISON] += (Attribute)(cfg->def_zhongdu);
	base_attr_list.m_attrs[BATTLE_ATTR_ANTI_FREEZE] += (Attribute)(cfg->def_hunshui);
	base_attr_list.m_attrs[BATTLE_ATTR_ANTI_PETRIFY] += (Attribute)(cfg->def_shihua);
	base_attr_list.m_attrs[BATTLE_ATTR_ANTI_CHAOS] += (Attribute)(cfg->def_hunluan);
	base_attr_list.m_attrs[BATTLE_ATTR_ANTI_DRUNK] += (Attribute)(cfg->def_jiuzui);
	// 元素
	base_attr_list.m_attrs[BATTLE_ATTR_ELEMENT_WATER] += (Attribute)(cfg->element_water);
	base_attr_list.m_attrs[BATTLE_ATTR_ELEMENT_FIRE] += (Attribute)(cfg->element_fire);
	base_attr_list.m_attrs[BATTLE_ATTR_ELEMENT_WIND] += (Attribute)(cfg->element_wind);
	base_attr_list.m_attrs[BATTLE_ATTR_ELEMENT_EARTH] += (Attribute)(cfg->element_land);

	double skin_attr[ADD_POINT_TYPE_MAX] = { 0 };
	bool is_have_skin = false;
	std::map<int, std::vector<int> >::const_iterator  iter = m_pet_has_skin_id.find(pet.pet_id);
	if (iter != m_pet_has_skin_id.end())
	{
		const std::vector<int> & s = iter->second;
		for (std::vector<int>::const_iterator skin_id_iter = s.begin(); skin_id_iter != s.end(); ++skin_id_iter)
		{
			//皮肤资质
			const SkinPetCfg * skin_pet_cfg = LOGIC_CONFIG->GetSkinConfig()->GetSkinPetCfgBySkinId(*skin_id_iter);
			if (skin_pet_cfg != NULL)
			{
				skin_attr[ADD_POINT_TYPE_VITALITY] += skin_pet_cfg->con;
				skin_attr[ADD_POINT_TYPE_STRENGTH] += skin_pet_cfg->str;
				skin_attr[ADD_POINT_TYPE_POWER] += skin_pet_cfg->def;
				skin_attr[ADD_POINT_TYPE_AGILITY] += skin_pet_cfg->agi;
				skin_attr[ADD_POINT_TYPE_MAGIC] += skin_pet_cfg->magic;
				is_have_skin = true;
			}
		}
		total_attr[ADD_POINT_TYPE_VITALITY] += skin_attr[ADD_POINT_TYPE_VITALITY];
		total_attr[ADD_POINT_TYPE_STRENGTH] += skin_attr[ADD_POINT_TYPE_STRENGTH];
		total_attr[ADD_POINT_TYPE_POWER] += skin_attr[ADD_POINT_TYPE_POWER];
		total_attr[ADD_POINT_TYPE_AGILITY] += skin_attr[ADD_POINT_TYPE_AGILITY];
		total_attr[ADD_POINT_TYPE_MAGIC] += skin_attr[ADD_POINT_TYPE_MAGIC];
	}

	//修炼 
	if (m_param.pet_train.level > 0)
	{
		double temp_attrs[BATTLE_ATTR_MAX] = { 0 };
		for (int i = 0; i < ADD_POINT_TYPE_MAX; ++i)
		{
			const PetAttrExchangeCfg::ItemCfg* attr_cfg = LOGIC_CONFIG->GetPetConfig()->GetAttrExchange(m_param.pet_train.level, i);
			const PetAttrExchangeCfg::ItemCfg* attr_cfg1 = LOGIC_CONFIG->GetPetConfig()->GetAttrExchange(0, i);
			if (NULL != attr_cfg && NULL != attr_cfg1)
			{
				// 先算整数 全部计算完再除 100 不然会有误差 每次零点几 积累起来就有 1 了
				// 血蓝
				temp_attrs[BATTLE_ATTR_MAX_HP] += total_attr[i] * attr_cfg->maxhp - (total_attr[i] * attr_cfg1->maxhp);
				temp_attrs[BATTLE_ATTR_MAX_MP] += total_attr[i] * attr_cfg->maxmp - (total_attr[i] * attr_cfg1->maxmp);
				// 基础属性
				temp_attrs[BATTLE_ATTR_ATTACK] += total_attr[i] * attr_cfg->gongji - (total_attr[i] * attr_cfg1->gongji);
				temp_attrs[BATTLE_ATTR_DEFENSE] += total_attr[i] * attr_cfg->fangyu - (total_attr[i] * attr_cfg1->fangyu);
				temp_attrs[BATTLE_ATTR_AGILITY] += total_attr[i] * attr_cfg->minjie - (total_attr[i] * attr_cfg1->minjie);
				temp_attrs[BATTLE_ATTR_RECOVERY] += total_attr[i] * attr_cfg->huifu - (total_attr[i] * attr_cfg1->huifu);
				temp_attrs[BATTLE_ATTR_MENTAL] += total_attr[i] * attr_cfg->jingshen - (total_attr[i] * attr_cfg1->jingshen);
			}
		}
		m_pet_attr_reason_list[PET_REASON_LIST_TYPE_TRAIN].m_attrs[BATTLE_ATTR_MAX_HP] = static_cast<Attribute>(temp_attrs[BATTLE_ATTR_MAX_HP] / 100.0 + 1e-6);
		m_pet_attr_reason_list[PET_REASON_LIST_TYPE_TRAIN].m_attrs[BATTLE_ATTR_MAX_MP] = static_cast<Attribute>(temp_attrs[BATTLE_ATTR_MAX_MP] / 100.0 + 1e-6);

		m_pet_attr_reason_list[PET_REASON_LIST_TYPE_TRAIN].m_attrs[BATTLE_ATTR_ATTACK] = static_cast<Attribute>(temp_attrs[BATTLE_ATTR_ATTACK] / 100.0 + 1e-6);
		m_pet_attr_reason_list[PET_REASON_LIST_TYPE_TRAIN].m_attrs[BATTLE_ATTR_DEFENSE] = static_cast<Attribute>(temp_attrs[BATTLE_ATTR_DEFENSE] / 100.0 + 1e-6);
		m_pet_attr_reason_list[PET_REASON_LIST_TYPE_TRAIN].m_attrs[BATTLE_ATTR_AGILITY] = static_cast<Attribute>(temp_attrs[BATTLE_ATTR_AGILITY] / 100.0 + 1e-6);
		m_pet_attr_reason_list[PET_REASON_LIST_TYPE_TRAIN].m_attrs[BATTLE_ATTR_RECOVERY] = static_cast<Attribute>(temp_attrs[BATTLE_ATTR_RECOVERY] / 100.0 + 1e-6);
		m_pet_attr_reason_list[PET_REASON_LIST_TYPE_TRAIN].m_attrs[BATTLE_ATTR_MENTAL] = static_cast<Attribute>(temp_attrs[BATTLE_ATTR_MENTAL] / 100.0 + 1e-6);
	}

	std::map<int, int> pet_train_2_map = m_module_mgr->GetTrain()->GetPetTrainUp2MapByLevel(m_param.max_level);
	for (std::map<int, int>::const_iterator iter = pet_train_2_map.begin(); iter != pet_train_2_map.end(); ++iter)
	{
		int train_type = iter->first;
		int train_level = iter->second;
		const PetTrainCfg2 * train_cfg = LOGIC_CONFIG->GetTrainConfig()->GetPetTrainCfg2(train_type, train_level);
		if (train_cfg == NULL)
		{
			continue;
		}

		for (std::set<int>::iterator iter2 = train_cfg->attr_type_list.begin(); iter2 != train_cfg->attr_type_list.end(); ++iter2)
		{
			int attr_type = *iter2;
			if (0 <= attr_type && attr_type < BATTLE_ATTR_MAX)
			{
				m_pet_attr_reason_list[PET_REASON_LIST_TYPE_TRAIN].m_attrs[attr_type] += train_cfg->attribute_value;
			}
		}
		const PetTrainCfg2 * train_cfg_1 = LOGIC_CONFIG->GetTrainConfig()->GetPetTrainCfg2(train_type, 0);
		if (NULL == train_cfg_1)
		{
			continue;
		}
		for (std::set<int>::iterator iter2 = train_cfg_1->attr_type_list.begin(); iter2 != train_cfg_1->attr_type_list.end(); ++iter2)
		{
			int attr_type = *iter2;
			if (0 <= attr_type && attr_type < BATTLE_ATTR_MAX)
			{
				m_pet_attr_reason_list[PET_REASON_LIST_TYPE_TRAIN].m_attrs[attr_type] -= train_cfg_1->attribute_value;
			}
		}
	}

	//突破属性归入资质中
	double growth_attrs[BATTLE_ATTR_MAX] = { 0 };
	if (pet_break_attr_cfg != NULL)
	{
		for (int i = 0; i < ADD_POINT_TYPE_MAX; ++i)
		{
			std::map<int, PetBreakAttrCfg::AttrItem>::const_iterator break_attr_iter = pet_break_attr_cfg->attr_item.find(i);
			if (break_attr_iter != pet_break_attr_cfg->attr_item.end())
			{
				const PetBreakAttrCfg::AttrItem & break_attr = break_attr_iter->second;
				// 血蓝
				growth_attrs[BATTLE_ATTR_MAX_HP] += total_attr[i] * break_attr.maxhp;
				growth_attrs[BATTLE_ATTR_MAX_MP] += total_attr[i] * break_attr.maxmp;
				// 基础属性
				growth_attrs[BATTLE_ATTR_ATTACK] += total_attr[i] * break_attr.gongji;
				growth_attrs[BATTLE_ATTR_DEFENSE] += total_attr[i] * break_attr.fangyu;
				growth_attrs[BATTLE_ATTR_AGILITY] += total_attr[i] * break_attr.minjie;
				growth_attrs[BATTLE_ATTR_RECOVERY] += total_attr[i] * break_attr.huifu;
				growth_attrs[BATTLE_ATTR_MENTAL] += total_attr[i] * break_attr.jingshen;
			}
		}
	}

	//基础
	m_pet_attr_reason_list[PET_REASON_LIST_TYPE_BASE].m_attrs[BATTLE_ATTR_MAX_HP] += (Attribute)(cfg->maxhp);
	m_pet_attr_reason_list[PET_REASON_LIST_TYPE_BASE].m_attrs[BATTLE_ATTR_MAX_MP] += (Attribute)(cfg->maxmp);
	// 基础属性
	m_pet_attr_reason_list[PET_REASON_LIST_TYPE_BASE].m_attrs[BATTLE_ATTR_ATTACK] += (Attribute)(cfg->gongji);
	m_pet_attr_reason_list[PET_REASON_LIST_TYPE_BASE].m_attrs[BATTLE_ATTR_DEFENSE] += (Attribute)(cfg->fangyu);
	m_pet_attr_reason_list[PET_REASON_LIST_TYPE_BASE].m_attrs[BATTLE_ATTR_AGILITY] += (Attribute)(cfg->minjie);
	m_pet_attr_reason_list[PET_REASON_LIST_TYPE_BASE].m_attrs[BATTLE_ATTR_RECOVERY] += (Attribute)(cfg->huifu);
	m_pet_attr_reason_list[PET_REASON_LIST_TYPE_BASE].m_attrs[BATTLE_ATTR_MENTAL] += (Attribute)(cfg->jingshen);
	// 修正属性
	m_pet_attr_reason_list[PET_REASON_LIST_TYPE_BASE].m_attrs[BATTLE_ATTR_CRITICAL] += (Attribute)(cfg->bisha);
	m_pet_attr_reason_list[PET_REASON_LIST_TYPE_BASE].m_attrs[BATTLE_ATTR_HIT] += (Attribute)(cfg->mingzhong);
	m_pet_attr_reason_list[PET_REASON_LIST_TYPE_BASE].m_attrs[BATTLE_ATTR_DODGE] += (Attribute)(cfg->shanduo);
	m_pet_attr_reason_list[PET_REASON_LIST_TYPE_BASE].m_attrs[BATTLE_ATTR_COUNTER_ATTACK] += (Attribute)(cfg->fanji);
	m_pet_attr_reason_list[PET_REASON_LIST_TYPE_BASE].m_attrs[BATTLE_ATTR_MAGIC_ATTACK] += (Attribute)(cfg->mogong);
	m_pet_attr_reason_list[PET_REASON_LIST_TYPE_BASE].m_attrs[BATTLE_ATTR_MAGIC_DEFENSE] += (Attribute)(cfg->mokang);
	m_pet_attr_reason_list[PET_REASON_LIST_TYPE_BASE].m_attrs[BATTLE_ATTR_CRITICAL_INC_VALUE] += (Attribute)(cfg->bisha_dmg);
	m_pet_attr_reason_list[PET_REASON_LIST_TYPE_BASE].m_attrs[BATTLE_ATTR_CRITICAL_DEC_VALUE] += (Attribute)(cfg->jianren);
	// 异常抗性
	m_pet_attr_reason_list[PET_REASON_LIST_TYPE_BASE].m_attrs[BATTLE_ATTR_ANTI_POISON] += (Attribute)(cfg->def_zhongdu);
	m_pet_attr_reason_list[PET_REASON_LIST_TYPE_BASE].m_attrs[BATTLE_ATTR_ANTI_FREEZE] += (Attribute)(cfg->def_hunshui);
	m_pet_attr_reason_list[PET_REASON_LIST_TYPE_BASE].m_attrs[BATTLE_ATTR_ANTI_PETRIFY] += (Attribute)(cfg->def_shihua);
	m_pet_attr_reason_list[PET_REASON_LIST_TYPE_BASE].m_attrs[BATTLE_ATTR_ANTI_CHAOS] += (Attribute)(cfg->def_hunluan);
	m_pet_attr_reason_list[PET_REASON_LIST_TYPE_BASE].m_attrs[BATTLE_ATTR_ANTI_DRUNK] += (Attribute)(cfg->def_jiuzui);
	// 元素
	m_pet_attr_reason_list[PET_REASON_LIST_TYPE_BASE].m_attrs[BATTLE_ATTR_ELEMENT_WATER] += (Attribute)(cfg->element_water);
	m_pet_attr_reason_list[PET_REASON_LIST_TYPE_BASE].m_attrs[BATTLE_ATTR_ELEMENT_FIRE] += (Attribute)(cfg->element_fire);
	m_pet_attr_reason_list[PET_REASON_LIST_TYPE_BASE].m_attrs[BATTLE_ATTR_ELEMENT_WIND] += (Attribute)(cfg->element_wind);
	m_pet_attr_reason_list[PET_REASON_LIST_TYPE_BASE].m_attrs[BATTLE_ATTR_ELEMENT_EARTH] += (Attribute)(cfg->element_land);

	double point_attrs[BATTLE_ATTR_MAX] = { 0 };
	double other_attrs[BATTLE_ATTR_MAX] = { 0 };
	double str_attrs[BATTLE_ATTR_MAX] = { 0 };
	double str_buff_attrs[BATTLE_ATTR_MAX] = { 0 };
	//资质 / 潜能 / 其他(皮肤) / 强化 / 共鸣
	for (int i = 0; i < ADD_POINT_TYPE_MAX; ++i)
	{
		const PetAttrExchangeCfg::ItemCfg* attr_cfg = LOGIC_CONFIG->GetPetConfig()->GetAttrExchange(0, i);
		if (NULL != attr_cfg)
		{
			// 先算整数 全部计算完再除 100 不然会有误差 每次零点几 积累起来就有 1 了

			// 资质
			double attr = pet.growth[i] * (PET_ATTR_BASE_UP + (pet.level - 1) * PET_ATTR_LEVEL_UP);
			growth_attrs[BATTLE_ATTR_MAX_HP] += attr * attr_cfg->maxhp;
			growth_attrs[BATTLE_ATTR_MAX_MP] += attr * attr_cfg->maxmp;
			growth_attrs[BATTLE_ATTR_ATTACK] += attr * attr_cfg->gongji;
			growth_attrs[BATTLE_ATTR_DEFENSE] += attr * attr_cfg->fangyu;
			growth_attrs[BATTLE_ATTR_AGILITY] += attr * attr_cfg->minjie;
			growth_attrs[BATTLE_ATTR_RECOVERY] += attr * attr_cfg->huifu;
			growth_attrs[BATTLE_ATTR_MENTAL] += attr * attr_cfg->jingshen;

			//潜能
			point_attrs[BATTLE_ATTR_MAX_HP] += pet.attr[i] * attr_cfg->maxhp;
			point_attrs[BATTLE_ATTR_MAX_MP] += pet.attr[i] * attr_cfg->maxmp;
			point_attrs[BATTLE_ATTR_ATTACK] += pet.attr[i] * attr_cfg->gongji;
			point_attrs[BATTLE_ATTR_DEFENSE] += pet.attr[i] * attr_cfg->fangyu;
			point_attrs[BATTLE_ATTR_AGILITY] += pet.attr[i] * attr_cfg->minjie;
			point_attrs[BATTLE_ATTR_RECOVERY] += pet.attr[i] * attr_cfg->huifu;
			point_attrs[BATTLE_ATTR_MENTAL] += pet.attr[i] * attr_cfg->jingshen;

			//其他(皮肤)
			if (is_have_skin)
			{
				other_attrs[BATTLE_ATTR_MAX_HP] += skin_attr[i] * attr_cfg->maxhp;
				other_attrs[BATTLE_ATTR_MAX_MP] += skin_attr[i] * attr_cfg->maxmp;
				other_attrs[BATTLE_ATTR_ATTACK] += skin_attr[i] * attr_cfg->gongji;
				other_attrs[BATTLE_ATTR_DEFENSE] += skin_attr[i] * attr_cfg->fangyu;
				other_attrs[BATTLE_ATTR_AGILITY] += skin_attr[i] * attr_cfg->minjie;
				other_attrs[BATTLE_ATTR_RECOVERY] += skin_attr[i] * attr_cfg->huifu;
				other_attrs[BATTLE_ATTR_MENTAL] += skin_attr[i] * attr_cfg->jingshen;
			}

			//强化
			double str_attr = str_cfg->add_growth[i] / 100.0 * (PET_ATTR_BASE_UP + (pet.level - 1) * PET_ATTR_LEVEL_UP);
			str_attrs[BATTLE_ATTR_MAX_HP] += str_attr * attr_cfg->maxhp;
			str_attrs[BATTLE_ATTR_MAX_MP] += str_attr * attr_cfg->maxmp;
			str_attrs[BATTLE_ATTR_ATTACK] += str_attr * attr_cfg->gongji;
			str_attrs[BATTLE_ATTR_DEFENSE] += str_attr * attr_cfg->fangyu;
			str_attrs[BATTLE_ATTR_AGILITY] += str_attr * attr_cfg->minjie;
			str_attrs[BATTLE_ATTR_RECOVERY] += str_attr * attr_cfg->huifu;
			str_attrs[BATTLE_ATTR_MENTAL] += str_attr * attr_cfg->jingshen;
		}
	}
	//资质
	m_pet_attr_reason_list[PET_REASON_LIST_TYPE_GROWTH].m_attrs[BATTLE_ATTR_MAX_HP] += static_cast<Attribute>(growth_attrs[BATTLE_ATTR_MAX_HP] / 100.0 + 1e-6);
	m_pet_attr_reason_list[PET_REASON_LIST_TYPE_GROWTH].m_attrs[BATTLE_ATTR_MAX_MP] += static_cast<Attribute>(growth_attrs[BATTLE_ATTR_MAX_MP] / 100.0 + 1e-6);
	m_pet_attr_reason_list[PET_REASON_LIST_TYPE_GROWTH].m_attrs[BATTLE_ATTR_ATTACK] += static_cast<Attribute>(growth_attrs[BATTLE_ATTR_ATTACK] / 100.0 + 1e-6);
	m_pet_attr_reason_list[PET_REASON_LIST_TYPE_GROWTH].m_attrs[BATTLE_ATTR_DEFENSE] += static_cast<Attribute>(growth_attrs[BATTLE_ATTR_DEFENSE] / 100.0 + 1e-6);
	m_pet_attr_reason_list[PET_REASON_LIST_TYPE_GROWTH].m_attrs[BATTLE_ATTR_AGILITY] += static_cast<Attribute>(growth_attrs[BATTLE_ATTR_AGILITY] / 100.0 + 1e-6);
	m_pet_attr_reason_list[PET_REASON_LIST_TYPE_GROWTH].m_attrs[BATTLE_ATTR_RECOVERY] += static_cast<Attribute>(growth_attrs[BATTLE_ATTR_RECOVERY] / 100.0 + 1e-6);
	m_pet_attr_reason_list[PET_REASON_LIST_TYPE_GROWTH].m_attrs[BATTLE_ATTR_MENTAL] += static_cast<Attribute>(growth_attrs[BATTLE_ATTR_MENTAL] / 100.0 + 1e-6);

	//潜能
	m_pet_attr_reason_list[PET_REASON_LIST_TYPE_POINT].m_attrs[BATTLE_ATTR_MAX_HP] += static_cast<Attribute>(point_attrs[BATTLE_ATTR_MAX_HP] / 100.0 + 1e-6);
	m_pet_attr_reason_list[PET_REASON_LIST_TYPE_POINT].m_attrs[BATTLE_ATTR_MAX_MP] += static_cast<Attribute>(point_attrs[BATTLE_ATTR_MAX_MP] / 100.0 + 1e-6);
	m_pet_attr_reason_list[PET_REASON_LIST_TYPE_POINT].m_attrs[BATTLE_ATTR_ATTACK] += static_cast<Attribute>(point_attrs[BATTLE_ATTR_ATTACK] / 100.0 + 1e-6);
	m_pet_attr_reason_list[PET_REASON_LIST_TYPE_POINT].m_attrs[BATTLE_ATTR_DEFENSE] += static_cast<Attribute>(point_attrs[BATTLE_ATTR_DEFENSE] / 100.0 + 1e-6);
	m_pet_attr_reason_list[PET_REASON_LIST_TYPE_POINT].m_attrs[BATTLE_ATTR_AGILITY] += static_cast<Attribute>(point_attrs[BATTLE_ATTR_AGILITY] / 100.0 + 1e-6);
	m_pet_attr_reason_list[PET_REASON_LIST_TYPE_POINT].m_attrs[BATTLE_ATTR_RECOVERY] += static_cast<Attribute>(point_attrs[BATTLE_ATTR_RECOVERY] / 100.0 + 1e-6);
	m_pet_attr_reason_list[PET_REASON_LIST_TYPE_POINT].m_attrs[BATTLE_ATTR_MENTAL] += static_cast<Attribute>(point_attrs[BATTLE_ATTR_MENTAL] / 100.0 + 1e-6);

	//其他(皮肤)
	if (is_have_skin)
	{
		m_pet_attr_reason_list[PET_REASON_LIST_TYPE_OTHER].m_attrs[BATTLE_ATTR_MAX_HP] += static_cast<Attribute>(other_attrs[BATTLE_ATTR_MAX_HP] / 100.0 + 1e-6);
		m_pet_attr_reason_list[PET_REASON_LIST_TYPE_OTHER].m_attrs[BATTLE_ATTR_MAX_MP] += static_cast<Attribute>(other_attrs[BATTLE_ATTR_MAX_MP] / 100.0 + 1e-6);
		m_pet_attr_reason_list[PET_REASON_LIST_TYPE_OTHER].m_attrs[BATTLE_ATTR_ATTACK] += static_cast<Attribute>(other_attrs[BATTLE_ATTR_ATTACK] / 100.0 + 1e-6);
		m_pet_attr_reason_list[PET_REASON_LIST_TYPE_OTHER].m_attrs[BATTLE_ATTR_DEFENSE] += static_cast<Attribute>(other_attrs[BATTLE_ATTR_DEFENSE] / 100.0 + 1e-6);
		m_pet_attr_reason_list[PET_REASON_LIST_TYPE_OTHER].m_attrs[BATTLE_ATTR_AGILITY] += static_cast<Attribute>(other_attrs[BATTLE_ATTR_AGILITY] / 100.0 + 1e-6);
		m_pet_attr_reason_list[PET_REASON_LIST_TYPE_OTHER].m_attrs[BATTLE_ATTR_RECOVERY] += static_cast<Attribute>(other_attrs[BATTLE_ATTR_RECOVERY] / 100.0 + 1e-6);
		m_pet_attr_reason_list[PET_REASON_LIST_TYPE_OTHER].m_attrs[BATTLE_ATTR_MENTAL] += static_cast<Attribute>(other_attrs[BATTLE_ATTR_MENTAL] / 100.0 + 1e-6);
	}

	//强化
	m_pet_attr_reason_list[PET_REASON_LIST_TYPE_STREN].m_attrs[BATTLE_ATTR_MAX_HP] += static_cast<Attribute>(str_attrs[BATTLE_ATTR_MAX_HP] / 100.0 + 1e-6 + str_cfg->maxhp);
	m_pet_attr_reason_list[PET_REASON_LIST_TYPE_STREN].m_attrs[BATTLE_ATTR_MAX_MP] += static_cast<Attribute>(str_attrs[BATTLE_ATTR_MAX_MP] / 100.0 + 1e-6 + str_cfg->maxmp);
	m_pet_attr_reason_list[PET_REASON_LIST_TYPE_STREN].m_attrs[BATTLE_ATTR_ATTACK] += static_cast<Attribute>(str_attrs[BATTLE_ATTR_ATTACK] / 100.0 + 1e-6 + str_cfg->gongji);
	m_pet_attr_reason_list[PET_REASON_LIST_TYPE_STREN].m_attrs[BATTLE_ATTR_DEFENSE] += static_cast<Attribute>(str_attrs[BATTLE_ATTR_DEFENSE] / 100.0 + 1e-6 + str_cfg->fangyu);
	m_pet_attr_reason_list[PET_REASON_LIST_TYPE_STREN].m_attrs[BATTLE_ATTR_AGILITY] += static_cast<Attribute>(str_attrs[BATTLE_ATTR_AGILITY] / 100.0 + 1e-6 + str_cfg->minjie);
	m_pet_attr_reason_list[PET_REASON_LIST_TYPE_STREN].m_attrs[BATTLE_ATTR_RECOVERY] += static_cast<Attribute>(str_attrs[BATTLE_ATTR_RECOVERY] / 100.0 + 1e-6 + str_cfg->huifu);
	m_pet_attr_reason_list[PET_REASON_LIST_TYPE_STREN].m_attrs[BATTLE_ATTR_MENTAL] += static_cast<Attribute>(str_attrs[BATTLE_ATTR_MENTAL] / 100.0 + 1e-6 + str_cfg->jingshen);

	//共鸣
	m_pet_attr_reason_list[PET_REASON_LIST_TYPE_RESON].m_attrs[BATTLE_ATTR_MAX_HP] += static_cast<Attribute>(str_buff_attrs[BATTLE_ATTR_MAX_HP] / 100.0 + 1e-6);
	m_pet_attr_reason_list[PET_REASON_LIST_TYPE_RESON].m_attrs[BATTLE_ATTR_MAX_MP] += static_cast<Attribute>(str_buff_attrs[BATTLE_ATTR_MAX_MP] / 100.0 + 1e-6);
	m_pet_attr_reason_list[PET_REASON_LIST_TYPE_RESON].m_attrs[BATTLE_ATTR_ATTACK] += static_cast<Attribute>(str_buff_attrs[BATTLE_ATTR_ATTACK] / 100.0 + 1e-6);
	m_pet_attr_reason_list[PET_REASON_LIST_TYPE_RESON].m_attrs[BATTLE_ATTR_DEFENSE] += static_cast<Attribute>(str_buff_attrs[BATTLE_ATTR_DEFENSE] / 100.0 + 1e-6);
	m_pet_attr_reason_list[PET_REASON_LIST_TYPE_RESON].m_attrs[BATTLE_ATTR_AGILITY] += static_cast<Attribute>(str_buff_attrs[BATTLE_ATTR_AGILITY] / 100.0 + 1e-6);
	m_pet_attr_reason_list[PET_REASON_LIST_TYPE_RESON].m_attrs[BATTLE_ATTR_RECOVERY] += static_cast<Attribute>(str_buff_attrs[BATTLE_ATTR_RECOVERY] / 100.0 + 1e-6);
	m_pet_attr_reason_list[PET_REASON_LIST_TYPE_RESON].m_attrs[BATTLE_ATTR_MENTAL] += static_cast<Attribute>(str_buff_attrs[BATTLE_ATTR_MENTAL] / 100.0 + 1e-6);
	
	int role_level = this->GetHuaShenLevel();
	// 看看专属技能中是否有被动加属性的技能
	for (size_t i = 0; i < cfg->exclusive_skills.size(); ++i)
	{
		if (PET_EXCLUSIVE_SKILL_TYPE_PASSIVE != cfg->exclusive_skills[i].type) continue;

		const GamePassiveSkillCfg * passive_skill = SkillPool::Instance().GetPassiveSkillCfg(cfg->exclusive_skills[i].skill_id);
		if (NULL == passive_skill) continue;

		// 专属技能是有技能等级的，注意！
		int skill_level = (i == 0 ? str_cfg->exclusive_skill_level : str_cfg->exclusive_skill_2_level);
		int spid = passive_skill->sp_id;
		if (passive_skill->is_level_spid && skill_level >= 1 && skill_level < ARRAY_LENGTH(passive_skill->level_spid_list))
		{
			spid = passive_skill->level_spid_list[skill_level];
		}
		GetSpecialEffectAttr(spid, m_pet_attr_reason_list[PET_REASON_LIST_TYPE_SKILL], base_attr_list, role_level, pet.level);
	}

	// 看看专属技能中是否有被动加属性的技能
	const PetCfg * lian_yao_pet_cfg = LOGIC_CONFIG->GetPetConfig()->GetPetCfg(pet.lian_yao_pet_id);
	if (lian_yao_pet_cfg)
	{
		const PetStrengthenCfg * lian_yao_str_cfg = LOGIC_CONFIG->GetPetConfig()->GetPetStrengthenCfg(lian_yao_pet_cfg->strengthen_cfg_id, pet.lian_yao_pet_str_lv);
		for (size_t i = 0; lian_yao_str_cfg && i < lian_yao_pet_cfg->exclusive_skills.size(); ++i)
		{
			if (PET_EXCLUSIVE_SKILL_TYPE_PASSIVE != lian_yao_pet_cfg->exclusive_skills[i].type) continue;

			const GamePassiveSkillCfg * passive_skill = SkillPool::Instance().GetPassiveSkillCfg(lian_yao_pet_cfg->exclusive_skills[i].skill_id);
			if (NULL == passive_skill) continue;

			// 专属技能是有技能等级的，注意！
			int skill_level = (i == 0 ? lian_yao_str_cfg->exclusive_skill_level : lian_yao_str_cfg->exclusive_skill_2_level);
			int spid = passive_skill->sp_id;
			if (passive_skill->is_level_spid && skill_level >= 1 && skill_level < ARRAY_LENGTH(passive_skill->level_spid_list))
			{
				spid = passive_skill->level_spid_list[skill_level];
			}
			GetSpecialEffectAttr(spid, m_pet_attr_reason_list[PET_REASON_LIST_TYPE_SKILL], base_attr_list, role_level, pet.level);
		}
	}

	m_module_mgr->GetSmartMountsSystem()->GetDominationBaseAttr(m_pet_grid_list[pet_index].item_unique_id, m_pet_attr_reason_list[PET_REASON_LIST_TYPE_SMS]);
	m_module_mgr->GetPet()->OnCalcPetBenedictioncAttr(m_pet_grid_list[pet_index].item_unique_id, m_pet_attr_reason_list[PET_REASON_LIST_TYPE_BENEDICTION], true);
	if (this->IsWaitForFightPet(pet_index))		//法宝提供宠物属性 - 不算入宠物自身提升范围内
	{
		FaBao * fabao_manager = m_module_mgr->GetFaBao();
		for (int i = 0; i < MAX_FABAO_COUNT; ++i)
		{
			if (!fabao_manager->IsActiveFaBao(i)) continue;

			const FaBaoEquip  * equip_ptr = fabao_manager->GetFaBaoEquip(i);
			if (equip_ptr == NULL)
			{
				continue;
			}

			const FaBaoEquip & equip = *equip_ptr;
			int fabao_id = equip.fabao_index;
			const FaBaoInfoCfg * cfg = LOGIC_CONFIG->GetFaBaoConfig()->GetFaBaoInfoCfg(fabao_id);
			if (NULL == cfg || cfg->target != FABAO_TARGET_PET || cfg->fabao_type != FABAO_TYPE_PASSIVE)
			{
				continue;
			}

			const FaBaoJinJieItemCfg * jin_jie_cfg = LOGIC_CONFIG->GetFaBaoConfig()->GetFaBaoJinJieItemCfg(fabao_id, equip.fabao_jie);
			if (jin_jie_cfg == NULL)
			{
				continue;
			}

			if (cfg->effect < 0 || cfg->effect >= BATTLE_ATTR_MAX)
			{
				continue;
			}

			if (!jin_jie_cfg->is_percentage)
			{
				int add_value = 0;
				if (IsRatioAttrType(cfg->effect))
				{
					add_value = static_cast<Attribute>(jin_jie_cfg->advanced_addition / (double)FaBao::FA_BAO_ATTR_CAL_RATE * 10);
				}
				else
				{
					add_value = static_cast<Attribute>(jin_jie_cfg->advanced_addition / (double)FaBao::FA_BAO_ATTR_CAL_RATE);
				}

				m_pet_attr_reason_list[PET_REASON_LIST_TYPE_FABAO].m_attrs[cfg->effect] += add_value;
			}

			if (jin_jie_cfg->is_percentage)
			{
				int add_value = 0;
				if (IsRatioAttrType(cfg->effect))
				{
					add_value = static_cast<Attribute>(base_attr_list.m_attrs[cfg->effect] * (1 + jin_jie_cfg->advanced_addition / (double)FaBao::FA_BAO_ATTR_CAL_RATE * 10));
				}
				else
				{
					add_value = static_cast<Attribute>(base_attr_list.m_attrs[cfg->effect] * (1 + jin_jie_cfg->advanced_addition / (double)FaBao::FA_BAO_ATTR_CAL_RATE));
				}

				m_pet_attr_reason_list[PET_REASON_LIST_TYPE_FABAO].m_attrs[cfg->effect] += add_value;
			}
		}
	}

	PetEffectSkillId skill_list = this->GetPetEffectiveSkill(pet_index);
	for (int i = 0; i < (int)skill_list.passive_list.size(); ++i)
	{
		const PetEffectSkillItem & curr = skill_list.passive_list[i];
		int skill_id = curr.skill_id;
		int skill_level = curr.skill_level;

		if (curr.skill_src_type == PET_SKILL_SRC_TYPE_FEI_SHENG)
		{
			const PetFeiShengSkillUpgradeCfg * fei_sheng_skill_cfg = LOGIC_CONFIG->GetPetConfig()->GetPetFeiShengSkillUpgradeCfg(pet.fei_sheng_select_skill_id, pet.fei_sheng_select_skill_level);
			if (fei_sheng_skill_cfg)
			{
				for (int fei_sheng_skill_attr_index = 0; fei_sheng_skill_attr_index < (int)fei_sheng_skill_cfg->attr_list.size(); ++fei_sheng_skill_attr_index)
				{
					int attr_type = fei_sheng_skill_cfg->attr_list[fei_sheng_skill_attr_index].attr_type;
					int attr_value = fei_sheng_skill_cfg->attr_list[fei_sheng_skill_attr_index].attr_value;
					if (BATTLE_ATTR_MIN <= attr_type && attr_type < BATTLE_ATTR_MAX)
					{
						m_pet_attr_reason_list[PET_REASON_LIST_TYPE_SKILL].m_attrs[attr_type] += attr_value;
					}
				}
			}

			continue;
		}

		const GamePassiveSkillCfg * passive_skill = SkillPool::Instance().GetPassiveSkillCfg(skill_id);
		if (NULL == passive_skill) continue;

		int spid = passive_skill->sp_id;
		if (passive_skill->is_level_spid)
		{
			int idx = skill_level + 1;
			if (0 <= idx && idx < ARRAY_LENGTH(passive_skill->level_spid_list))
			{
				spid = passive_skill->level_spid_list[idx];
			}
		}

		const SpecialEffectCfg * spe_cfg = BattleSpecialEffectPool::GetInstance().GetSpecialEffectCfg(spid);
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
					m_pet_attr_reason_list[PET_REASON_LIST_TYPE_SKILL].m_attrs[detail_cfg->param_list[0]] += detail_cfg->param_list[1];
				}

				if (SPE_ATTR_ADD_ATTR_BY_LEVEL == detail_cfg->effect_sub_type && detail_cfg->param_num >= 2
					&& detail_cfg->param_list[0] >= BATTLE_ATTR_MIN && detail_cfg->param_list[0] < BATTLE_ATTR_MAX)
				{
					BattleAttr add_attr = static_cast<BattleAttr>(ceil(m_param.max_level * (double)detail_cfg->param_list[1] / BATTLE_PERCENT_NUM));
					m_pet_attr_reason_list[PET_REASON_LIST_TYPE_SKILL].m_attrs[detail_cfg->param_list[0]] += add_attr;
				}

				if (SPE_ATTR_ADD_ATTR_BY_PET_LEVEL == detail_cfg->effect_sub_type && detail_cfg->param_num >= 3
					&& detail_cfg->param_list[0] >= BATTLE_ATTR_MIN && detail_cfg->param_list[0] < BATTLE_ATTR_MAX)
				{
					BattleAttr add_attr = static_cast<BattleAttr>(ceil(pet.level * (double)detail_cfg->param_list[1] / BATTLE_PERCENT_NUM) + detail_cfg->param_list[2]);
					m_pet_attr_reason_list[PET_REASON_LIST_TYPE_SKILL].m_attrs[detail_cfg->param_list[0]] += add_attr;
				}

				if (SPE_ATTR_ADD_ATTR_BY_ANOTHER_ATTR == detail_cfg->effect_sub_type && detail_cfg->param_num >= 3
					&& detail_cfg->param_list[0] >= BATTLE_ATTR_MIN && detail_cfg->param_list[0] < BATTLE_ATTR_MAX
					&& detail_cfg->param_list[1] >= BATTLE_ATTR_MIN && detail_cfg->param_list[1] < BATTLE_ATTR_MAX
					&& detail_cfg->param_list[2] > 0)
				{
					int attr_type0 = detail_cfg->param_list[0];
					int attr_type1 = detail_cfg->param_list[1];
					m_pet_attr_reason_list[PET_REASON_LIST_TYPE_SKILL].m_attrs[attr_type0] += static_cast<int>(1.0 * base_attr_list.m_attrs[attr_type1] / (1.0 * detail_cfg->param_list[2] / BATTLE_PERCENT_NUM));
				}
			}
		}
	}

	std::vector<SkillPro> other_sill_list;
	m_module_mgr->GetSmartMountsSystem()->GetBaseSkillList(m_pet_grid_list[pet_index].item_unique_id, &other_sill_list);
	int smart_mouns_skill_count = (int)other_sill_list.size();
	m_module_mgr->GetRoleGuildInborn()->GetSkillList(&other_sill_list, GUILD_INBORN_ADD_TARGET_TYPE_PET);
	for (int i = 0; i < (int)other_sill_list.size(); i++)
	{
		const GamePassiveSkillCfg * passive_skill = SkillPool::Instance().GetPassiveSkillCfg(other_sill_list[i].skill_id);
		if (NULL == passive_skill) continue;

		// 灵骑技能是有技能等级的，注意！
		int skill_level = other_sill_list[i].skill_level;
		int spid = passive_skill->sp_id;
		if (passive_skill->is_level_spid && skill_level >= 1 && skill_level < ARRAY_LENGTH(passive_skill->level_spid_list))
		{
			spid = passive_skill->level_spid_list[skill_level];
		}
		int reason_type = (smart_mouns_skill_count <= i) ? PET_REASON_LIST_TYPE_OTHER : PET_REASON_LIST_TYPE_SMS;

		const SpecialEffectCfg * spe_cfg = BattleSpecialEffectPool::GetInstance().GetSpecialEffectCfg(spid);
		if (NULL == spe_cfg) continue;

		for (int k = 0; k < spe_cfg->effect_num && k < EFFECT_DETAIL_NUM; ++k)
		{
			const SpecialEffectKey & key = spe_cfg->effect_detail_list[k];
			if (key.effect_type != SPECIAL_EFFECT_TYPE_ADD_ATTR) continue;

			const EffectDetailCfg * detail_cfg = BattleSpecialEffectPool::GetInstance().GetEffectDetailCfg(key);
			if (NULL == detail_cfg) continue;

			if (SPE_ATTR_ADD_ATTR == detail_cfg->effect_sub_type && detail_cfg->param_num >= 2
				&& detail_cfg->param_list[0] >= BATTLE_ATTR_MIN && detail_cfg->param_list[0] < BATTLE_ATTR_MAX)
			{
				m_pet_attr_reason_list[reason_type].m_attrs[detail_cfg->param_list[0]] += detail_cfg->param_list[1];
			}

			if (SPE_ATTR_ADD_ATTR_BY_LEVEL == detail_cfg->effect_sub_type && detail_cfg->param_num >= 2
				&& detail_cfg->param_list[0] >= BATTLE_ATTR_MIN && detail_cfg->param_list[0] < BATTLE_ATTR_MAX)
			{
				BattleAttr add_attr = static_cast<BattleAttr>(ceil(this->GetHuaShenLevel() * (double)detail_cfg->param_list[1] / BATTLE_PERCENT_NUM));
				m_pet_attr_reason_list[reason_type].m_attrs[detail_cfg->param_list[0]] += add_attr;
			}

			if (SPE_ATTR_ADD_ATTR_BY_PET_LEVEL == detail_cfg->effect_sub_type && detail_cfg->param_num >= 3
				&& detail_cfg->param_list[0] >= BATTLE_ATTR_MIN && detail_cfg->param_list[0] < BATTLE_ATTR_MAX)
			{
				BattleAttr add_attr = static_cast<BattleAttr>(ceil(pet.level * (double)detail_cfg->param_list[1] / BATTLE_PERCENT_NUM) + detail_cfg->param_list[2]);
				m_pet_attr_reason_list[reason_type].m_attrs[detail_cfg->param_list[0]] += add_attr;
			}

			if (SPE_ATTR_ADD_ATTR_BY_ANOTHER_ATTR == detail_cfg->effect_sub_type && detail_cfg->param_num >= 3
				&& detail_cfg->param_list[0] >= BATTLE_ATTR_MIN && detail_cfg->param_list[0] < BATTLE_ATTR_MAX
				&& detail_cfg->param_list[1] >= BATTLE_ATTR_MIN && detail_cfg->param_list[1] < BATTLE_ATTR_MAX
				&& detail_cfg->param_list[2] > 0)
			{
				int attr_type0 = detail_cfg->param_list[0];
				int attr_type1 = detail_cfg->param_list[1];
				m_pet_attr_reason_list[reason_type].m_attrs[attr_type0] += static_cast<int>(1.0 * base_attr_list.m_attrs[attr_type1] / (1.0 * detail_cfg->param_list[2] / BATTLE_PERCENT_NUM));
			}
		}
	}

	int pet_benediction_sp_id = m_module_mgr->GetPet()->GetPetBenedictionSpId(pet, m_pet_grid_list[pet_index].item_unique_id);
	const SpecialEffectCfg * benediction_spe_cfg = BattleSpecialEffectPool::GetInstance().GetSpecialEffectCfg(pet_benediction_sp_id);
	if (NULL != benediction_spe_cfg)
	{
		for (int k = 0; k < benediction_spe_cfg->effect_num && k < EFFECT_DETAIL_NUM; ++k)
		{
			const SpecialEffectKey & key = benediction_spe_cfg->effect_detail_list[k];
			if (key.effect_type != SPECIAL_EFFECT_TYPE_ADD_ATTR) continue;

			const EffectDetailCfg * detail_cfg = BattleSpecialEffectPool::GetInstance().GetEffectDetailCfg(key);
			if (NULL == detail_cfg) continue;

			if (SPE_ATTR_ADD_ATTR == detail_cfg->effect_sub_type && detail_cfg->param_num >= 2
				&& detail_cfg->param_list[0] >= BATTLE_ATTR_MIN && detail_cfg->param_list[0] < BATTLE_ATTR_MAX)
			{
				m_pet_attr_reason_list[PET_REASON_LIST_TYPE_BENEDICTION].m_attrs[detail_cfg->param_list[0]] += detail_cfg->param_list[1];
			}

			if (SPE_ATTR_ADD_ATTR_BY_LEVEL == detail_cfg->effect_sub_type && detail_cfg->param_num >= 2
				&& detail_cfg->param_list[0] >= BATTLE_ATTR_MIN && detail_cfg->param_list[0] < BATTLE_ATTR_MAX)
			{
				BattleAttr add_attr = static_cast<BattleAttr>(ceil(this->GetHuaShenLevel() * (double)detail_cfg->param_list[1] / BATTLE_PERCENT_NUM));
				m_pet_attr_reason_list[PET_REASON_LIST_TYPE_BENEDICTION].m_attrs[detail_cfg->param_list[0]] += add_attr;
			}

			if (SPE_ATTR_ADD_ATTR_BY_PET_LEVEL == detail_cfg->effect_sub_type && detail_cfg->param_num >= 3
				&& detail_cfg->param_list[0] >= BATTLE_ATTR_MIN && detail_cfg->param_list[0] < BATTLE_ATTR_MAX)
			{
				BattleAttr add_attr = static_cast<BattleAttr>(ceil(pet.level * (double)detail_cfg->param_list[1] / BATTLE_PERCENT_NUM) + detail_cfg->param_list[2]);
				m_pet_attr_reason_list[PET_REASON_LIST_TYPE_BENEDICTION].m_attrs[detail_cfg->param_list[0]] += add_attr;
			}

			if (SPE_ATTR_ADD_ATTR_BY_ANOTHER_ATTR == detail_cfg->effect_sub_type && detail_cfg->param_num >= 3
				&& detail_cfg->param_list[0] >= BATTLE_ATTR_MIN && detail_cfg->param_list[0] < BATTLE_ATTR_MAX
				&& detail_cfg->param_list[1] >= BATTLE_ATTR_MIN && detail_cfg->param_list[1] < BATTLE_ATTR_MAX
				&& detail_cfg->param_list[2] > 0)
			{
				int attr_type0 = detail_cfg->param_list[0];
				int attr_type1 = detail_cfg->param_list[1];
				m_pet_attr_reason_list[PET_REASON_LIST_TYPE_BENEDICTION].m_attrs[attr_type0] += static_cast<int>(1.0 * base_attr_list.m_attrs[attr_type1] / (1.0 * detail_cfg->param_list[2] / BATTLE_PERCENT_NUM));
			}
		}
	}


	//家族灵脉可能会配5项基础属性
	AttributePlan tmp_ap;
	for (int i = 0, k = BATTLE_ADD_POINT_TYPE_BEGIN; i < ADD_POINT_TYPE_MAX && k <= BATTLE_ADD_POINT_TYPE_END; i++, k++)
	{
		tmp_ap.add_point_list[i] += m_pet_attr_reason_list[PET_REASON_LIST_TYPE_OTHER].m_attrs[k];
	}
	LOGIC_CONFIG->GetRoleExpConfig()->ConvertPointToAttribute(tmp_ap, m_pet_attr_reason_list[PET_REASON_LIST_TYPE_OTHER]);

	//属性丹
	for (int i = 0; i < ATTR_PELLET_TYPE_MAX && i < ARRAY_LENGTH(pet.use_attr_pellet_list); ++i)
	{
		if (pet.use_attr_pellet_list[i] <= 0) continue;

		int attr_type = GetAttrTypeByPelletType(i);
		if (attr_type < 0 || attr_type >= BATTLE_ATTR_MAX) continue;

		int max_count = GetMin(LOGIC_CONFIG->GetRoleAttrPelletConfig()->GetOtherCfg().use_limit, (int)pet.use_attr_pellet_list[i]);
		const RoleAttrPelletAttrListCfg * attr_pellet_cfg = LOGIC_CONFIG->GetRoleAttrPelletConfig()->GetAttrPelletCfg(ROLE_ATTR_PELLET_TYPE_PET, cfg->main_attribute_id, attr_type);
		if (NULL != attr_pellet_cfg)
		{
			m_pet_attr_reason_list[PET_REASON_LIST_TYPE_PELLET].m_attrs[attr_type] += attr_pellet_cfg->attr_value * max_count;
		}
	}

	//由于总属性是一起算的，而来源是分开的，此时可能会因为小数点而造成误差，此处误差默认填充资质
	static AttributeList temp_attr; temp_attr.Reset();
	for (int i = 0; i < ARRAY_LENGTH(temp_attr.m_attrs); i++)
	{
		if (BATTLE_ATTR_CUR_HP == i || BATTLE_ATTR_CUR_MP == i) continue;
		for (int j = 0; j < ARRAY_LENGTH(m_pet_attr_reason_list); j++)
		{
			temp_attr.m_attrs[i] += m_pet_attr_reason_list[j].m_attrs[i];
		}
		if (m_pet_base_attr[pet_index].m_attrs[i] != temp_attr.m_attrs[i])
		{
			m_pet_attr_reason_list[PET_REASON_LIST_TYPE_GROWTH].m_attrs[i] += m_pet_base_attr[pet_index].m_attrs[i] - temp_attr.m_attrs[i];
		}
	}

#ifdef _DEBUG
	long long elapse_us = PerformanceCatch::Instance().GetElapseUsEnd(PERFORMANCE_TEST_PET_TIME_RECALC);

	printf("%s pet_index[%d] elapse[%lld]us\n", __FUNCTION__, pet_index, elapse_us);
#endif
	return 0;
}

void HuaShenManager::SendPetAttrReasonList(int pet_index)
{	
	if(this->PetInvalid(pet_index)) return;
	if(this->GetPetAttrReasonList(pet_index)) return;

	Protocol::SCAttrListReason info;
	info.reason_type = Protocol::SCAttrListReason::REASON_TYPE_HUASHEN_PET;
	info.param1 = pet_index;
	for (int i = 0; i < MAX_REASON_TYPE_LIST_NUM; i++)
	{
		info.reason_list[i] = m_pet_attr_reason_list[i];
	}

	m_module_mgr->NetSend(&info, sizeof(info));

}

void HuaShenManager::OnRoleChangeProf()
{
	if(!this->IsSeasonInTime()) return;

	for (int i = 0; i < MAX_HUASHEN_ATTR_PLAN_NUM; i++)
	{
		this->ClearAttributePointReq(i);
	}
}

void HuaShenManager::OnInfoReq(Protocol::CSHuaShenInfoReq req)
{
	if(!FunOpen::IsFunOpen(m_module_mgr->GetRole(), FUN_TYPE_HUANJIEZHANCHANG)) return;
	//玩家如果没重置过,则视为第一次,此时如果是在活动时间外,则不给通过
	if (!this->IsSeasonInTime())
	{
		if (!ActivityManager::Instance().IsActivityOpen(ACTIVITY_TYPE_HUAN_JIE_ZHAN_CHANG)) 
		{
			return;
		} 
	}

	switch (req.op_type)
	{
		case Protocol::CSHuaShenInfoReq::HUASHEN_INFO_TYPE_ALL_INFO:
		{
			this->SendAllHuaShenInfo();
		}
		break;
		case Protocol::CSHuaShenInfoReq::HUASHEN_INFO_TYPE_RENWU:
		{
			this->SendRoleInfo();
			this->SendRoleAttrList();
			m_module_mgr->GetEquipmentManager()->SendHuaShenEquipmentInfo();	//水晶+装备
			m_module_mgr->GetEquipmentManager()->SendHuaShenAllJewelryInfo();	//灵饰
			this->SendMedalInfo();												//勋章
			this->SendBattlePosition();
		}
		break;
		case Protocol::CSHuaShenInfoReq::HUASHEN_INFO_TYPE_POINT:
		{
			this->SendPlanInfo(m_param.cur_plan_index);
		}
		break;
		case Protocol::CSHuaShenInfoReq::HUASHEN_INFO_TYPE_KNAPSACK:
		{
			this->SendKnapsackInfo(Protocol::SCHuaShenKnapsackInfoAck::HUASHEN_SEND_REASON_REQ);
		}
		break;
		case Protocol::CSHuaShenInfoReq::HUASHEN_INFO_TYPE_PET:
		{
			this->SendPetAllInfo();
		}
		break;
		case Protocol::CSHuaShenInfoReq::HUASHEN_INFO_TYPE_PET_POINT:
		{
			this->SendPetAttrInfo(req.param1);
		}
		break;
		case Protocol::CSHuaShenInfoReq::HUASHEN_INFO_TYPE_FIRST_ENTER:
		{
			if (m_param.season_reset == 0)
			{
				this->OnSeasonFirstReset();
			}
			else
			{
				this->OnEnterKeySynchronize();
			}
			m_param.season_reset_first_dijing = 1;
		}
		break;
		case Protocol::CSHuaShenInfoReq::HUASHEN_INFO_TYPE_FINISH_GUIDE:
		{
			m_param.is_finish_guide = 1;
			this->SendSeasonInfo();
		}
		break;
	default:
		break;
	}
}

void HuaShenManager::SendAllHuaShenInfo()
{
	this->SendSeasonInfo();
	this->SendRoleInfo();
	this->SendRoleAttrList();
	this->SendMedalInfo();
	m_module_mgr->GetEquipmentManager()->SendHuaShenEquipmentInfo();
	m_module_mgr->GetEquipmentManager()->SendHuaShenAllJewelryInfo();
	this->SendKnapsackInfo();
	this->SendPlanListInfo();
	this->SendPetAllInfo();
	this->SendPetOtherInfo();
	this->SendBattlePosition();
}

const AttributeList & HuaShenManager::ReCalcAttrPlan(AttributeList & base_add, bool is_recalc)
{
	if (is_recalc)
	{
		m_attrs_plan_add.Reset();

		LOGIC_CONFIG->GetRoleExpConfig()->ConvertPointToAttribute(m_param.attr_plan_list[m_param.cur_plan_index], m_attrs_plan_add);
	}

	base_add.Add(m_attrs_plan_add.m_attrs);
	return m_attrs_plan_add;
}

void HuaShenManager::SendPlanInfo(int cur_index)
{
	static Protocol::SCHuaShenRoleAttributePlanInfo info;
	info.cur_plan_index = cur_index;
	info.attr_plan_info.remain_point = m_param.attr_plan_list[cur_index].remain_point;
	for (int k = 0; k < ADD_POINT_TYPE_MAX; k++)
	{
		info.attr_plan_info.add_point_list[k] = m_param.attr_plan_list[cur_index].add_point_list[k];
	}
	F_STRNCPY(info.attr_plan_info.plan_name, m_param.attr_plan_list[cur_index].plan_name, sizeof(info.attr_plan_info.plan_name));

	EngineAdapter::Instance().NetSend(m_module_mgr->GetRole()->GetNetId(), (const char*)&info, sizeof(info));
}

void HuaShenManager::SendPlanListInfo()
{
	static Protocol::SCHuaShenAttrPlanListInfo info;
	info.cur_plan_index = m_param.cur_plan_index;
	for (int i = 0; i < MAX_HUASHEN_ATTR_PLAN_NUM; i++)
	{
		info.plan_list[i].remain_point = m_param.attr_plan_list[i].remain_point;
		for (int k = 0; k < ADD_POINT_TYPE_MAX; k++)
		{
			info.plan_list[i].add_point_list[k] = m_param.attr_plan_list[i].add_point_list[k];
		}
		F_STRNCPY(info.plan_list[i].plan_name, m_param.attr_plan_list[i].plan_name, sizeof(info.plan_list[i].plan_name));
	}

	EngineAdapter::Instance().NetSend(m_module_mgr->GetRole()->GetNetId(), (const char*)&info, sizeof(info));
}

bool HuaShenManager::SynBenZunItemData(ItemDataWrapper * item_grid_data, int op_type, bool is_notice)
{
	if (m_param.season_reset == 0 || NULL == item_grid_data || op_type < 0 || op_type >= HUASHEN_KNAPSACK_OP_TYPE_MAX) return false;

	bool ret = false;
	std::map<long long, int>::iterator it = m_item_map.find(item_grid_data->item_unique_id);
	switch (op_type)
	{
	case HUASHEN_KNAPSACK_OP_TYPE_UPDATE:
	{
		if (it != m_item_map.end())
		{
			//更新道具信息
			int type = it->second / HUASHEN_GRID_INDEX_TYPE_BASE;
			int index = it->second % HUASHEN_GRID_INDEX_TYPE_BASE;
			switch (type)
			{
				case HUASHEN_GRID_INDEX_TYPE_KNAPSACK:
				{
					if (m_grid_list[index] == NULL)
					{
						ret = this->Put(*item_grid_data, is_notice);
					}
					else
					{
						if (m_grid_list[index]->param == NULL)
						{
							m_grid_list[index]->param = ItemBase::CreateParam();
						}
						m_grid_list[index]->param->SetData(item_grid_data->param_data);
						m_grid_list[index]->is_bind = (0 != item_grid_data->is_bind);
						m_grid_list[index]->item_id = item_grid_data->item_id;
						this->SetItemDirty(index, is_notice, Knapsack::ITEM_CHANGE_NOTICE_REASON_DATA_CHANGE);
						ret = true;
					}
				}
				break;
				case HUASHEN_GRID_INDEX_TYPE_EQUIPMENT:
				{
					ret = m_module_mgr->GetEquipmentManager()->HuaShenEquipmentUpdate(index, item_grid_data);
				}
				break;
				case HUASHEN_GRID_INDEX_TYPE_JEWELRY:
				{
					ret = m_module_mgr->GetEquipmentManager()->HuaShenJewelryUpdate(index, item_grid_data);
				}
				break;
				case HUASHEN_GRID_INDEX_TYPE_CRYSTAL:
				{
					ret = m_module_mgr->GetEquipmentManager()->HuaShenCrystalUpdate(item_grid_data);
				}
				break;
				case HUASHEN_GRID_INDEX_TYPE_MEDAL:
				{
					ret = this->MedalUpdate(index, item_grid_data);
				}
				break;
				case HUASHEN_GRID_INDEX_TYPE_PET:
				{
					ret = this->PetDataUpdate(index, item_grid_data);
				}
				break;
				case HUASHEN_GRID_INDEX_TYPE_PET_EQUIPMENT:
				{
					ret = this->PetEquipmentUpdate(index, item_grid_data);
				}
				break;
			}
		}
	}
	break;
	case HUASHEN_KNAPSACK_OP_TYPE_ADD:
	{
		if (it == m_item_map.end())	//新增道具
		{
			ret = this->Put(*item_grid_data, is_notice);
		}
	}
	break;
	case HUASHEN_KNAPSACK_OP_TYPE_CONSUME:
	{
		if (it == m_item_map.end()) return false;	//化身没有这个装备

		ret = this->ConsumeItem(it->second, is_notice);
		if (ret)
		{
			m_item_map.erase(it->first);
		}
	}
	break;
	case HUASHEN_KNAPSACK_OP_TYPE_KEY_SYN:
	{
		if (it == m_item_map.end())	//新增道具
		{
			ret = this->Put(*item_grid_data, is_notice);
		}
		else
		{
			//更新道具信息
			int type = it->second / HUASHEN_GRID_INDEX_TYPE_BASE;
			int index = it->second % HUASHEN_GRID_INDEX_TYPE_BASE;
			switch (type)
			{
				case HUASHEN_GRID_INDEX_TYPE_KNAPSACK:
				{
					if (m_grid_list[index] == NULL)
					{
						ret = this->Put(*item_grid_data, is_notice);
					}
					else
					{					
						if (m_grid_list[index]->param == NULL)
						{
							m_grid_list[index]->param = ItemBase::CreateParam();
						}
						m_grid_list[index]->param->SetData(item_grid_data->param_data);
						m_grid_list[index]->is_bind = (0 != item_grid_data->is_bind);
						m_grid_list[index]->item_id = item_grid_data->item_id;
						this->SetItemDirty(index, is_notice, Knapsack::ITEM_CHANGE_NOTICE_REASON_DATA_CHANGE);
						ret = true;
					}
				}
				break;
				case HUASHEN_GRID_INDEX_TYPE_EQUIPMENT:
				{
					ret = m_module_mgr->GetEquipmentManager()->HuaShenEquipmentUpdate(index, item_grid_data);
				}
				break;
				case HUASHEN_GRID_INDEX_TYPE_JEWELRY:
				{
					ret = m_module_mgr->GetEquipmentManager()->HuaShenJewelryUpdate(index, item_grid_data);
				}
				break;
				case HUASHEN_GRID_INDEX_TYPE_CRYSTAL:
				{
					ret = m_module_mgr->GetEquipmentManager()->HuaShenCrystalUpdate(item_grid_data);
				}
				break;
				case HUASHEN_GRID_INDEX_TYPE_MEDAL:
				{
					ret = this->MedalUpdate(index, item_grid_data);
				}
				break;
				case HUASHEN_GRID_INDEX_TYPE_PET:
				{
					ret = this->PetDataUpdate(index, item_grid_data);
				}
				break;
				case HUASHEN_GRID_INDEX_TYPE_PET_EQUIPMENT:
				{
					ret = this->PetEquipmentUpdate(index, item_grid_data);
				}
				break;
			}
		}
	}
	break;
	}
	return ret;
}

bool HuaShenManager::SynBenZunItemData(ItemGridData * item_grid_data, int op_type, bool is_notice)
{
	if (m_param.season_reset == 0 || NULL == item_grid_data || op_type < 0 || op_type >= HUASHEN_KNAPSACK_OP_TYPE_MAX) return false;

	ItemDataWrapper temp_item_wrapper;
	item_grid_data->WrapTo(&temp_item_wrapper);

	return this->SynBenZunItemData(&temp_item_wrapper, op_type, is_notice);
}

bool HuaShenManager::IsCheckItemIdentical(long long item_unique_id, int key_index)
{
	std::map<long long, int>::iterator it = m_item_map.find(item_unique_id);
	if (it == m_item_map.end())
	{
		return false;
	}
	if (it->second != key_index)
	{
		return false;
	}

	return true;
}

bool HuaShenManager::ConsumeItem(long long item_unique_id, bool is_notice)
{
	std::map<long long, int>::iterator it = m_item_map.find(item_unique_id);
	if (it == m_item_map.end())
	{
		return true;
	}
	if (this->ConsumeItem(it->second, is_notice))
	{
		m_item_map.erase(it->first);
		return true;
	}

	return false;
}

bool HuaShenManager::ConsumeItem(int key_index, bool is_notice, bool is_key_syn_chronize)
{
	bool ret = false;
	int type = key_index / HUASHEN_GRID_INDEX_TYPE_BASE;
	int index = key_index % HUASHEN_GRID_INDEX_TYPE_BASE;
	switch (type)
	{
		case HUASHEN_GRID_INDEX_TYPE_KNAPSACK:
		{
			ret = this->Consume(index, is_notice);
		}
		break;
		case HUASHEN_GRID_INDEX_TYPE_EQUIPMENT:
		{
			ret = m_module_mgr->GetEquipmentManager()->HuaShenEquipmentReset(index, is_notice);
		}
		break;
		case HUASHEN_GRID_INDEX_TYPE_JEWELRY:
		{
			ret = m_module_mgr->GetEquipmentManager()->HuaShenJewelryReset(index, is_notice);
		}
		break;
		case HUASHEN_GRID_INDEX_TYPE_CRYSTAL:
		{
			ret = m_module_mgr->GetEquipmentManager()->HuaShenCrystalReset(is_notice);
		}
		break;
		case HUASHEN_GRID_INDEX_TYPE_MEDAL:
		{
			ret = this->MedalReset(index, is_notice);
		}
		break;
		case HUASHEN_GRID_INDEX_TYPE_PET:
		{
			ret = this->DeletePet(index, is_notice, is_key_syn_chronize);
		}
		break;
		case HUASHEN_GRID_INDEX_TYPE_PET_EQUIPMENT:
		{
			ret = this->PetEquipmentReset(index, is_notice);
		}
		break;
	}

	return ret;
}

bool HuaShenManager::InOrder(bool ignore_time_interval)
{
	const UInt32 sortinterval = GLOBALCONFIG->GetOtherConfig_SortInOrder_Interval();
	UInt32 now_time = static_cast<UInt32>(EngineAdapter::Instance().Time()); // 背包整理不能频繁调用
	if (!ignore_time_interval && now_time < m_knapsack_last_inorder_time + sortinterval)
	{
		m_module_mgr->GetRole()->NoticeNum(errornum::EN_KNAPSACK_ORDER_TOO_FREQUENT);
		return false;
	}
	m_knapsack_last_inorder_time = now_time;
	int to_check_max_num = m_valid_grid_index_end;
	int max_clear_index = 0;
	int begin = 0;

	static bool empty_mark[HUASHEN_ITEM_GRID_NUM_MAX]; // 标记排序前在begin ~ begin + check_num范围内哪些格子是空的
	memset(empty_mark, 0, sizeof(empty_mark));
	for (int i = begin; i < to_check_max_num; i++)
	{
		empty_mark[i - begin] = (NULL == m_grid_list[i] || m_grid_list[i]->Invalid());
	}
	//int merge_result = -1;
	//int rest_num = -1;

	//化身装备目前不选入可堆叠物品
	/*for (int i = to_check_max_num - 1; i >= begin; i--) // 先将所有能合并的都合并起来
	{
		if (NULL == m_grid_list[i] || m_grid_list[i]->Invalid()) continue;

		const ItemBase *itembase = ITEMPOOL->GetItem(m_grid_list[i]->item_id);
		if (NULL == itembase) continue;

		if (!m_grid_list[i]->CanBeMerge()) continue;

		for (int j = begin; j < i; j++)
		{
			if (NULL == m_grid_list[i] || m_grid_list[i]->Invalid()) continue;

			int merged_gold_price = 0;
			merge_result = m_grid_list[i]->Merge(*m_grid_list[i], &rest_num, &merged_gold_price);
			if (ITEM_MERGE_CANNOT != merge_result)
			{
				if (ITEM_MERGE_COMPLETE == merge_result && 0 == rest_num)
				{
					this->ClearGrid(j);
					empty_mark[j] = true;
					max_clear_index = GetMax(max_clear_index, j);
				}
				else
				{
					m_grid_list[i]->num = rest_num;
					m_grid_list[i]->gold_price -= merged_gold_price;
				}

				//this->SetItemDirty(i, false, ITEM_CHANGE_NOTICE_REASON_MOVE);
				//this->SetItemDirty(j, false, ITEM_CHANGE_NOTICE_REASON_MOVE);

				if (m_grid_list[i]->num >= itembase->GetPileLimit()) break;
			}
		}
	}*/

	static SortStruct sort_stru[HUASHEN_ITEM_GRID_NUM_MAX];
	memset(sort_stru, 0, sizeof(sort_stru));

	int count = 0;

	for (int i = begin; i < to_check_max_num; i++)
	{
		if (NULL != m_grid_list[i] && !m_grid_list[i]->Invalid())
		{
			const ItemBase *itembase = ITEMPOOL->GetItem(m_grid_list[i]->item_id);
			if (NULL == itembase) continue;

			if (count >= HUASHEN_ITEM_GRID_NUM_MAX) return false;

			int prior_id = 0;

			if (I_TYPE_EQUIPMENT == itembase->GetItemType())
			{
				Equipment * equip_data = (Equipment *)itembase;
				if (NULL == m_grid_list[i]->param)
				{
					prior_id = SortStruct::GetPriorId(equip_data->GetEquipLevel(), itembase->GetItemType(), itembase->GetItemSubType(), itembase->GetColor(), equip_data->GetEquipIndex(), itembase->GetItemId(), 0);
				}
				else
				{
					short equip_rate = m_grid_list[i]->param->equip_param.is_treasure;
					prior_id = SortStruct::GetPriorId(equip_data->GetEquipLevel(), itembase->GetItemType(), itembase->GetItemSubType(), itembase->GetColor(), equip_data->GetEquipIndex(), itembase->GetItemId(), equip_rate);
				}
			}
			else if (I_TYPE_JEWELRY == itembase->GetItemType())
			{
				Jewelry * jewelry_data = (Jewelry*)itembase;
				if (NULL == m_grid_list[i]->param)
				{
					prior_id = SortStruct::GetPriorId(jewelry_data->GetJewelryLevel(), itembase->GetItemType(), itembase->GetItemSubType(), itembase->GetColor(), jewelry_data->GetJewelryType() + Equipment::E_INDEX_MAX, itembase->GetItemId(), 0);
				}
				else
				{
					short equip_rate = m_grid_list[i]->param->jewelry_param.is_treasure;
					prior_id = SortStruct::GetPriorId(jewelry_data->GetJewelryLevel(), itembase->GetItemType(), itembase->GetItemSubType(), itembase->GetColor(), jewelry_data->GetJewelryType() + Equipment::E_INDEX_MAX, itembase->GetItemId(), equip_rate);
				}
			}
			else
			{
				prior_id = SortStruct::GetPriorId(0, itembase->GetItemType(), itembase->GetItemSubType(), itembase->GetColor(), 0, itembase->GetItemId(), 0);
			}
			
			sort_stru[count].Set(prior_id, m_grid_list[i]->is_bind, i);

			++count;
		}
	}

	if (count > HUASHEN_ITEM_GRID_NUM_MAX) return false;

	qsort(sort_stru, count, sizeof(SortStruct), SortCompare);

	static ItemDataWrapper tmp_wrapper_list[HUASHEN_ITEM_GRID_NUM_MAX];

	m_valid_grid_index_end = count;

	for (int i = 0; i < count; i++) // 将排序后的物品存到临时空间里
	{
		int src_index = sort_stru[i].src_index;
		m_grid_list[src_index]->WrapTo(&tmp_wrapper_list[i]);

		// 把原位置清空
		this->ClearGrid(src_index);
		this->SetItemDirty(src_index, false, Knapsack::ITEM_CHANGE_NOTICE_REASON_MOVE);
	}
	for (int i = 0; i < count; i++) // 从临时空间拷贝回来
	{
		if (NULL == m_grid_list[begin + i])
		{
			m_grid_list[begin + i] = new ItemGridData();
		}
		m_grid_list[begin + i]->Set(tmp_wrapper_list[i]);
		m_item_map[m_grid_list[begin + i]->item_unique_id] = begin + i;
		this->SetItemDirty(begin + i, false, Knapsack::ITEM_CHANGE_NOTICE_REASON_MOVE);
	}
	for (int i = count; i < to_check_max_num - begin; i++) // 将后面的全部设为空
	{
		if (empty_mark[i]) continue;

		if (NULL == m_grid_list[begin + i] || m_grid_list[begin + i]->Invalid()) continue;

		this->ClearGrid(begin + i);
		this->SetItemDirty(begin + i, false, Knapsack::ITEM_CHANGE_NOTICE_REASON_MOVE);

		max_clear_index = GetMax(max_clear_index, begin + i);
	}
	this->RefreshValidGridIndexEnd(max_clear_index);
	this->SendKnapsackInfo(Protocol::SCHuaShenKnapsackInfoAck::HUASHEN_SEND_REASON_IN_ORDER);
	return true;
}

bool HuaShenManager::Put(ItemDataWrapper &item_grid_data, bool is_notice, int *out_index)
{
	if(m_param.season_reset == 0) return false;
	const ItemBase *itembase = ITEMPOOL->GetItem(item_grid_data.item_id);
	if (NULL == itembase) return false;

	if (itembase->GetBagsBelong() == B_BELONG_PET_BAG)
	{
		return this->AddPet(item_grid_data);
	}
	ItemDataWrapper item_info(item_grid_data);
	int curr_valid_grid_num = m_curr_valid_grid_num;
	int valid_grid_index_end = m_valid_grid_index_end;
	if (valid_grid_index_end > curr_valid_grid_num || curr_valid_grid_num > HUASHEN_ITEM_GRID_NUM_MAX) return false;

	int empty_index = -1;
	for (int i = 0; i < valid_grid_index_end; ++i)
	{
		if (NULL == m_grid_list[i] || m_grid_list[i]->Invalid())
		{
			empty_index = i;
			break;
		}
	}

	if (-1 == empty_index && valid_grid_index_end < curr_valid_grid_num) // 前面全满 则放到下一个空位里
	{
		empty_index = valid_grid_index_end;
	}

	if (empty_index < 0 || empty_index >= HUASHEN_ITEM_GRID_NUM_MAX) return false;

	if (NULL == m_grid_list[empty_index])
	{
		m_grid_list[empty_index] = new ItemGridData();
	}
	m_grid_list[empty_index]->Set(item_info);
	if (NULL != out_index)
	{
		*out_index = empty_index;
	}
	this->RefreshValidGridIndexEnd(empty_index);
	this->PutOn(empty_index, item_info.item_unique_id, HUASHEN_GRID_INDEX_TYPE_KNAPSACK);
	this->SetItemDirty(empty_index, is_notice, Knapsack::ITEM_CHANGE_NOTICE_REASON_MOVE);
	return true;
}

bool HuaShenManager::Consume(int index, bool is_notice)
{
	if(index < 0 || index >= HUASHEN_ITEM_GRID_NUM_MAX || NULL == m_grid_list[index] || m_grid_list[index]->Invalid()) return false;

	const ItemBase *itembase = ITEMPOOL->GetItem(m_grid_list[index]->item_id);
	if (NULL == itembase) return false;

	this->ClearGrid(index);
	this->RefreshValidGridIndexEnd(index);
	this->SetItemDirty(index, is_notice, Knapsack::ITEM_CHANGE_NOTICE_REASON_CONSUME);
	return true;
}

bool HuaShenManager::GetItemGridData(int index, ItemGridData * item_grid_data)
{
	if (NULL == item_grid_data || index < 0 || index >= HUASHEN_ITEM_GRID_NUM_MAX) return false;

	if (NULL == m_grid_list[index] || m_grid_list[index]->Invalid()) return false;

	const ItemBase *itembase = ITEMPOOL->GetItem(m_grid_list[index]->item_id);
	if (NULL == itembase) return false;

	if (m_grid_list[index]->num <= 0 || m_grid_list[index]->num > itembase->GetPileLimit()) return false;

	item_grid_data->Set(*m_grid_list[index]);

	return true;
}

const ItemGridData * HuaShenManager::GetItemGridData2(int index) const
{
	if(index < 0 || index >= HUASHEN_ITEM_GRID_NUM_MAX) return false;

	if (NULL == m_grid_list[index] || m_grid_list[index]->Invalid()) return false;

	const ItemBase *itembase = ITEMPOOL->GetItem(m_grid_list[index]->item_id);
	if (NULL == itembase) return false;

	if (m_grid_list[index]->num <= 0 || m_grid_list[index]->num > itembase->GetPileLimit()) return false;

	return m_grid_list[index];
}

void HuaShenManager::GetChangeItemList(int * count, ItemListParam::ItemListData * save_list)
{
	if (!m_knapsack_is_change)
	{
		*count = 0;
		return;
	}
	int tmp_count = 0;

	for (int i = 0; tmp_count < *count && i < HUASHEN_ITEM_GRID_NUM_MAX; i++)
	{
		if (m_dirty_mark[i])
		{
			if (NULL == m_grid_list[i] || m_grid_list[i]->Invalid())
			{
				if (!m_old_state[i]) continue;

				save_list[tmp_count].change_state_item = structcommon::CS_DELETE;

				save_list[tmp_count].index = HUASHEN_ITEM_INDEX_BEGIN + i;
				save_list[tmp_count].item_wrapper.Reset();
				++tmp_count;

				gamelog::g_log_knapsack.printf(LL_INFO, "%s CS_DELETE role[%d,%s] item from index[%d]", __FUNCTION__,
					m_module_mgr->GetRole()->GetUID(), m_module_mgr->GetRole()->GetName(), i);

			}
			else
			{
				if (m_old_state[i])
				{
					save_list[tmp_count].change_state_item = structcommon::CS_UPDATE;

					gamelog::g_log_knapsack.printf(LL_INFO, "%s CS_UPDATE role[%d,%s] item_id[%d] num[%d] from  index[%d]", __FUNCTION__,
						m_module_mgr->GetRole()->GetUID(), m_module_mgr->GetRole()->GetName(), m_grid_list[i]->item_id, m_grid_list[i]->num, i);
				}
				else
				{
					save_list[tmp_count].change_state_item = structcommon::CS_INSERT;

					gamelog::g_log_knapsack.printf(LL_INFO, "%s CS_INSERT role[%d,%s] item_id[%d] num[%d] from index[%d]", __FUNCTION__, 
						m_module_mgr->GetRole()->GetUID(), m_module_mgr->GetRole()->GetName(), m_grid_list[i]->item_id, m_grid_list[i]->num, i);
				}

				save_list[tmp_count].index = HUASHEN_ITEM_INDEX_BEGIN + i;
				m_grid_list[i]->WrapTo(&save_list[tmp_count].item_wrapper);
				++tmp_count;

			}
		}
	}

	*count = tmp_count;
}

void HuaShenManager::ClearDirtyMark()
{
	if (!m_knapsack_is_change) return;

	for (int i = 0; i < HUASHEN_ITEM_GRID_NUM_MAX; ++i)
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

	m_knapsack_is_change = false;
}

void HuaShenManager::SetItemDirty(int index, bool to_notice_client, short reason_type, short put_reason)
{	
	if(index < 0 || index >= HUASHEN_ITEM_GRID_NUM_MAX) return;
	
	m_dirty_mark[index] = true;
	m_knapsack_is_change = true;
	if (to_notice_client)
	{
		this->SendKnapsackSingleItemChange(index, reason_type, put_reason);
	}
}

void HuaShenManager::SendKnapsackInfo(short reason)
{
	static Protocol::SCHuaShenKnapsackInfoAck info;
	info.item_count = 0;
	info.reason = reason;
	info.valid_grid_num = GetMin((int)m_curr_valid_grid_num, m_module_mgr->GetKnapsack()->GetKnapsackVaildNum(ITEM_COLUMN_TYPE_ITEM) + 100);
	for (int i = 0; i < HUASHEN_ITEM_GRID_NUM_MAX; ++i)
	{
		if (NULL == m_grid_list[i] || m_grid_list[i]->Invalid()) continue;

		const ItemGridData * item_grid_data = m_grid_list[i];
		const ItemBase * item_base = ITEMPOOL->GetItem(item_grid_data->item_id);
		if (NULL == item_base) continue;

		InfoType & ptc_con = info.item_list[info.item_count++];
		ptc_con.item_id = item_grid_data->item_id;
		ptc_con.is_bind = (char)item_grid_data->is_bind;
		ptc_con.column = 0;
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

	EngineAdapter::Instance().NetSendSerializeMsg(m_module_mgr->GetRole()->GetNetId(), &info);
}

void HuaShenManager::SendKnapsackSingleItemChange(int index, short reason_type, short put_reason)
{
	if (index < 0 || index >= HUASHEN_ITEM_GRID_NUM_MAX) return;

	static Protocol::SCHuaShenKnapsackItemChange info;
	info.reason = reason_type;
	info.put_reason = put_reason;
	info.column = 0;
	info.index = static_cast<short>(index);
	if (NULL == m_grid_list[index] || m_grid_list[index]->Invalid())
	{
		info.item_id = 0;
		info.num = 0;
		info.is_bind = 0;
		info.has_param = 0;
		info.invalid_time = 0;
		info.item_unique_id = 0;
		info.is_market_buy_flag = 0;
		info.reserve_ch = 0;
	}
	else
	{
		info.item_id = m_grid_list[index]->item_id;
		info.num = m_grid_list[index]->num;
		info.is_bind = m_grid_list[index]->is_bind ? 1 : 0;
		info.has_param = (NULL != m_grid_list[index]->param);
		info.invalid_time = m_grid_list[index]->invalid_time;
		info.item_unique_id = m_grid_list[index]->item_unique_id;
		info.is_market_buy_flag = m_grid_list[index]->is_market_buy_flag;
		info.reserve_ch = 0;

		const ItemBase* item_base = ITEMPOOL->GetItem(info.item_id);
		if (NULL == item_base)
		{
			return;
		}
		info.param_length = m_grid_list[index]->param->GetDataByItemType(item_base->GetItemType(), info.param_data);
	}

	EngineAdapter::Instance().NetSendSerializeMsg(m_module_mgr->GetRole()->GetNetId(), &info);
}

bool HuaShenManager::PutOn(short index, long long unique_id, int grid_index_type)
{
	if(unique_id == 0) return false;
	switch (grid_index_type)
	{
		case HUASHEN_GRID_INDEX_TYPE_KNAPSACK:
		{
			if(index < 0 || index >= HUASHEN_ITEM_GRID_NUM_MAX) return false;
		}
		break;
		case HUASHEN_GRID_INDEX_TYPE_EQUIPMENT:
		{
			if(index < 0 || index >= Equipment::E_INDEX_MAX) return false;
		}
		break;
		case HUASHEN_GRID_INDEX_TYPE_JEWELRY:
		{
			if(index < 0 || index >= Jewelry::J_TYPE_MAX) return false;
		}
		break;
		case HUASHEN_GRID_INDEX_TYPE_CRYSTAL:
		{
			if(index != 0) return false;
		}
		break;
		case HUASHEN_GRID_INDEX_TYPE_MEDAL:
		{
			if(index < 0 || index >= MAX_MEDAL_WEAR_GRID) return false;
		}
		break;
		case HUASHEN_GRID_INDEX_TYPE_PET:
		{
			if(this->PetInvalid(index)) return false;
		}
		break;
		case HUASHEN_GRID_INDEX_TYPE_PET_EQUIPMENT:
		{
			if (this->PetInvalid(index) || m_pet_grid_list[index].param->pet_param.equipment_item_unique_id == 0) return false;
		}
		break;
	default:
		return false;
	}
	
	m_item_map[unique_id] = index + HUASHEN_GRID_INDEX_TYPE_BASE * grid_index_type;
	return true;
}

bool HuaShenManager::TakeOff(long long unique_id)
{
	if(unique_id == 0) return false;

	m_item_map.erase(unique_id);
	return true;
}

void HuaShenManager::SynBenZunAllPetData()
{
	if(m_param.season_reset != 1) return;
	m_module_mgr->GetPet()->SynHuaShenPetData(m_pet_grid_list, MAX_HUASHEN_PET_GRID_NUM, m_param.new_pet_status_index, NEW_PET_STATUS_MAX);
	
	int	min_level;
	for (int i = 0; i < MAX_HUASHEN_PET_GRID_NUM; i++)
	{
		if(this->PetInvalid(i)) continue;

		//此处需要重新申请一份地址，否则此时化身与本尊都指向同一个净值数据地址
		m_pet_grid_list[i].param = ItemBase::CreateParamFromParamData(m_pet_grid_list[i].param->data);

		//等级限制并自动洗点
		this->OnRefreshPoints(i, false);
		min_level = GetMin(m_pet_grid_list[i].param->pet_param.level, (unsigned short)m_param.max_level);
		m_pet_grid_list[i].param->pet_param.level = min_level;

		//添加宠物映射
		this->PutOn(i, m_pet_grid_list[i].item_unique_id, HUASHEN_GRID_INDEX_TYPE_PET);
		//添加宠物此时佩戴的道具映射
		if (m_pet_grid_list[i].param->pet_param.equipment_item_unique_id != 0)
		{
			this->PutOn(i, m_pet_grid_list[i].param->pet_param.equipment_item_unique_id, HUASHEN_GRID_INDEX_TYPE_PET_EQUIPMENT);
		}

		this->SetPetItemDirty(i, false);
		this->ReCalcPetAttr(i);
	}

}

bool HuaShenManager::PetDataUpdate(int pet_index, ItemDataWrapper * item_data)
{
	if(this->PetInvalid(pet_index) || NULL == item_data || item_data->Invalid() || 0 == item_data->has_param) return false;

	PetParam & pet_param = m_pet_grid_list[pet_index].param->pet_param;

	//先保存一些所需数据
	unsigned short attr[ADD_POINT_TYPE_MAX] = {0};
	memcpy(attr, pet_param.attr, sizeof(attr));
	NetValueItemParam param; param.Reset();
	pet_param.GetPetEquip(&param.pet_equipment_param);
	long long pet_equipment_unique_id = pet_param.equipment_item_unique_id;
	char pet_equipment_is_bind = pet_param.pet_equipment_is_bind;

	m_pet_grid_list[pet_index].param->SetData(item_data->param_data);

	//还原一些不同的数据
	pet_param.level = GetMin((short)pet_param.level, m_param.max_level);
	memcpy(pet_param.attr, attr, sizeof(pet_param.attr));
	pet_param.SetPetEquip(&param.pet_equipment_param);
	pet_param.equipment_item_unique_id = pet_equipment_unique_id;
	m_pet_grid_list[pet_index].is_bind = (0 != item_data->is_bind);
	pet_param.pet_equipment_is_bind = pet_equipment_is_bind;

	this->SetPetItemDirty(pet_index, false);
	this->ReCalcPetAttr(pet_index);
	return true;
}

bool HuaShenManager::PetEquipmentUpdate(int pet_index, ItemDataWrapper * item_data)
{
	if (this->PetInvalid(pet_index) || NULL == item_data || item_data->Invalid() || 0 == item_data->has_param) return false;

	PetParam & pet_param = m_pet_grid_list[pet_index].param->pet_param;
	NetValueItemParam param;
	param.SetData(item_data->param_data);

	pet_param.equipment_item_unique_id = item_data->item_unique_id;
	pet_param.SetPetEquip(&param.pet_equipment_param);
	pet_param.pet_equipment_is_bind = (char)item_data->is_bind;

	this->SetPetItemDirty(pet_index, false);
	this->ReCalcPetAttr(pet_index);
	return true;
}

bool HuaShenManager::PetEquipmentReset(int pet_index, bool is_notice)
{
	if (this->PetInvalid(pet_index)) return false;
		
	PetParam & pet_param = m_pet_grid_list[pet_index].param->pet_param;
	pet_param.ClearPetEquipment();

	this->SetPetItemDirty(pet_index, is_notice);
	if (is_notice)
	{
		this->ReCalcPetAttr(pet_index);
	}
	return true;
}

bool HuaShenManager::AddPet(ItemDataWrapper & pet_data)
{
	if(m_param.season_reset == 0  || 0 == pet_data.has_param) return false;
	
	ItemDataWrapper item_info(pet_data);
	int invalid_index = -1;
	for (int i = 0; i < MAX_HUASHEN_PET_GRID_NUM; ++i)
	{
		if (this->PetInvalid(i))
		{
			invalid_index = i;
			break;
		}
	}
	if(invalid_index < 0) return false;

	m_pet_grid_list[invalid_index].Set(item_info);
	m_pet_count++;
	//同步后需要重置一些数据
	this->OnRefreshPoints(invalid_index, false);
	m_pet_grid_list[invalid_index].param->pet_param.level = GetMin(m_pet_grid_list[invalid_index].param->pet_param.level, (unsigned short)m_param.max_level);
	//此时该宠物穿戴的护符是分开同步的
	if (0 != m_pet_grid_list[invalid_index].param->pet_param.equipment_item_unique_id)	
	{
		m_pet_grid_list[invalid_index].param->pet_param.ClearPetEquipment();
	}

	this->SetPetItemDirty(invalid_index, false);
	this->PutOn(invalid_index, m_pet_grid_list[invalid_index].item_unique_id, HUASHEN_GRID_INDEX_TYPE_PET);
	//此时该宠物可能佩戴了以前同步过的宠物装备，所以在此处不做宠物装备处理，改为外部逻辑
	this->ReCalcPetAttr(invalid_index);
	return true;
}

bool HuaShenManager::DeletePet(int pet_index, bool is_notice, bool is_key_syn_chronize)
{
	if (this->PetInvalid(pet_index)) return false;

	//在删除宠物前需要先检测是否穿戴着宠物护符
	if (0 != m_pet_grid_list[pet_index].param->pet_param.equipment_item_unique_id)
	{
		if (is_key_syn_chronize)
		{
			this->OnDeletePetAddPetEquipDelayHandle(pet_index);
		}
		else
		{
			this->PutOffPetEquipment(pet_index);
		}
	}

	this->OnPetGridInfoReset(pet_index);
	m_pet_count--;
	m_pet_grid_list[pet_index].Clear();
	this->SetPetItemDirty(pet_index, is_notice);

	return true;
}

bool HuaShenManager::SetPetState(int pet_index, int state)
{
	if (m_param.season_reset == 0 || this->PetInvalid(pet_index)) return false;

	short & fight_index = m_param.new_pet_status_index[NEW_FIGHT_INDEX];

	//PetParam & pet = m_pet_grid_list[pet_index].param->pet_param;
	int before_str_buff_level = this->GetPetStrBuffLevel(pet_index);
	switch (state)
	{
	case SET_FIGHT_INDEX:
	{
		if (fight_index == pet_index)
		{
			return 0;
		}
		if (m_module_mgr->GetPet()->IsPetBenedictionVice(m_pet_grid_list[pet_index].item_unique_id))
		{
			m_module_mgr->NoticeNum(errornum::EN_PET_NOT_OP_BY_BENEDICTION);
			return false;
		}

		int stand_by_num = 0; bool is_replaced = false;
		for (int posi_index = NEW_STANDBY_INDEX1; posi_index <= NEW_STANDBY_INDEX5; ++posi_index)
		{
			short & posi_pet_index = m_param.new_pet_status_index[posi_index];
			if (posi_pet_index >= 0) ++stand_by_num;

			if (posi_pet_index == pet_index)	// 是从等待状态中的宠物出战；
			{
				is_replaced = true;
				posi_pet_index = fight_index;
				break;
			}
		}

		if (!is_replaced)	// 不是从等待状态的宠物中出战 --> 检查等待列表还有没有空位
		{
			if (stand_by_num >= MAX_FIGHT_AND_STAND_BY_PET_NUM) m_param.new_pet_status_index[NEW_STANDBY_INDEX5] = -1;	// 如果数量到达上限，把最后一个等待宠物位设置成空；

			if (fight_index >= 0 && stand_by_num < (MAX_FIGHT_AND_STAND_BY_PET_NUM - 1))	// 如果等待列表的宠物小于 2 只，把战斗宠物放进去；
			{
				for (int posi_index = NEW_STANDBY_INDEX1; posi_index < NEW_STANDBY_INDEX5; ++posi_index)
				{
					short & posi_pet_index = m_param.new_pet_status_index[posi_index];
					if (posi_pet_index < 0)
					{
						posi_pet_index = fight_index;
						break;
					}
				}
			}
		}

		fight_index = pet_index;
		//pet.lock = 1;

		//EventHandler::Instance().OnSwitchPetToFight(m_module_mgr->GetRole());
	}
	break;
	case SET_STANDBY_INDEX1:
	case SET_STANDBY_INDEX2:
	case SET_STANDBY_INDEX3:
	case SET_STANDBY_INDEX4:
	case SET_STANDBY_INDEX5:
	{
		if (m_module_mgr->GetPet()->IsPetBenedictionVice(m_pet_grid_list[pet_index].item_unique_id))
		{
			m_module_mgr->NoticeNum(errornum::EN_PET_NOT_OP_BY_BENEDICTION);
			return false;
		}

		int list_index_num = fight_index >= 0 ? 1 : 0; bool is_succ = false;
		for (int posi_index = NEW_STANDBY_INDEX1; posi_index <= NEW_STANDBY_INDEX5; ++posi_index)
		{
			if (fight_index != pet_index && list_index_num >= MAX_FIGHT_AND_STAND_BY_PET_NUM) break;

			short & posi_pet_index = m_param.new_pet_status_index[posi_index];
			if (pet_index == posi_pet_index || posi_pet_index < 0)		// 如果已经是待机状态，或是空位，就替换后跳出；
			{
				posi_pet_index = pet_index;
				//pet.lock = 1;
				is_succ = true;
				break;
			}

			if (posi_pet_index >= 0) ++list_index_num;
		}

		if (is_succ && pet_index == fight_index)
		{
			fight_index = -1;
			//EventHandler::Instance().OnFightPetToStandby(m_module_mgr->GetRole());
		}

		if (!is_succ && list_index_num >= MAX_FIGHT_AND_STAND_BY_PET_NUM)
		{
			m_module_mgr->NoticeNum(errornum::EN_STANDBY_AND_FIGHT_PET_NOT_EXCEED_5);
			return false;
		}
	}
	break;
	case SET_REST:
	{
		for (int posi_index = NEW_FIGHT_INDEX; posi_index <= NEW_STANDBY_INDEX5; ++posi_index)
		{
			short & posi_pet_index = m_param.new_pet_status_index[posi_index];
			if (posi_pet_index != pet_index) continue;

			posi_pet_index = -1;

			//if (NEW_FIGHT_INDEX == posi_index) //EventHandler::Instance().OnFightPetRest(m_role_module_mgr->GetRole());

			break;
		}
	}
	break;
	default:
		return false;
	}
	
	this->SendPetOtherInfo();
	this->ReCalcPetAttr(pet_index);
	this->SetPetItemDirty(pet_index, true, Protocol::SCHuaShenSendPetSingleInfo::PET_NOTIE_TYPE_STATUS);
	return true;
}

bool HuaShenManager::AssignAttributePoints(int pet_index, short con, short str, short def, short agi, short magic)
{
	if (m_param.season_reset == 0 || con < 0 || str < 0 || def < 0 || agi < 0 || magic < 0)return false;
	if (this->PetInvalid(pet_index)) return false;
	PetParam& pet = m_pet_grid_list[pet_index].param->pet_param;
	int add_point[ADD_POINT_TYPE_MAX] = { con, str, def, agi, magic };
	const PetOtherCfg* other_cfg = LOGIC_CONFIG->GetPetConfig()->GetOtherCfg();
	int max_point = other_cfg->att_point_level * (pet.level - 1) + other_cfg->attr_point_init;

	std::map<int, std::vector<int> >::const_iterator  iter = m_pet_has_skin_id.find(pet.pet_id);
	if (iter != m_pet_has_skin_id.end())
	{
		for (std::vector<int>::const_iterator skin_id_iter = iter->second.begin(); skin_id_iter != iter->second.end(); ++skin_id_iter)
		{
			//皮肤资质
			const SkinPetCfg * skin_pet_cfg = LOGIC_CONFIG->GetSkinConfig()->GetSkinPetCfgBySkinId(*skin_id_iter);
			if (skin_pet_cfg != NULL)
			{
				max_point += skin_pet_cfg->extra_point;
			}
		}
	}

	int point = 0;
	for (int i = 0; i < ADD_POINT_TYPE_MAX; ++i)
	{
		point += (pet.attr[i] + add_point[i]);
	}
	if (point > max_point) return false;

	for (int i = 0; i < ADD_POINT_TYPE_MAX; ++i)
	{
		pet.attr[i] += add_point[i];
	}
	this->ReCalcPetAttr(pet_index);
	this->SetPetItemDirty(pet_index, true, Protocol::SCHuaShenSendPetSingleInfo::PET_NOTIE_TYPE_ASSIGN_POINTS);
	return true;
}

bool HuaShenManager::OnRefreshPoints(int pet_index, bool is_notice)
{
	if (m_param.season_reset == 0 || this->PetInvalid(pet_index)) return false;
	PetParam& pet = m_pet_grid_list[pet_index].param->pet_param;

	//化身宠物洗点不需要消耗道具
	memset(pet.attr, 0, sizeof(pet.attr));

	if (is_notice)
	{
		this->ReCalcPetAttr(pet_index);
	}
	this->SetPetItemDirty(pet_index, is_notice, Protocol::SCHuaShenSendPetSingleInfo::PET_NOTIE_TYPE_ASSIGN_POINTS);
	return true;
}
bool HuaShenManager::PutOnPetEquipment(int pet_index, int equipment_bag_index)
{
	if (this->PetInvalid(pet_index) || equipment_bag_index < 0 || equipment_bag_index >= HUASHEN_ITEM_GRID_NUM_MAX) return false;
	if (NULL == m_grid_list[equipment_bag_index] || m_grid_list[equipment_bag_index]->Invalid() || NULL == m_grid_list[equipment_bag_index]->param) return false;
	PetParam& pet_param = m_pet_grid_list[pet_index].param->pet_param;

	const ItemBase * tmp_itembase = ITEMPOOL->GetItem(m_grid_list[equipment_bag_index]->item_id);
	if (tmp_itembase == NULL || tmp_itembase->GetItemType() != I_TYPE_PET_EQUIPMENT)
	{
		m_module_mgr->NoticeNum(errornum::EN_ITEM_NOT_ELIGIBLE);
		return false;
	}

	if (pet_param.pet_equipment_id != 0)//有装备
	{
		if (!this->PutOffPetEquipment(pet_index))
		{
			m_module_mgr->NoticeNum(errornum::EN_KNAPSACK_EQUIP_TAKEOFF_ERROR);
			return false;
		}
	}

	ItemDataWrapper item_data; item_data.Reset();
	m_grid_list[equipment_bag_index]->WrapTo(&item_data);
	NetValueItemParam param;
	param.SetData(item_data.param_data);

	if (!this->Consume(equipment_bag_index, true))
	{
		m_module_mgr->NoticeNum(errornum::EN_KNAPSACK_USE_ITEM_ERROR);
		return false;
	}

	pet_param.equipment_item_unique_id = item_data.item_unique_id;
	pet_param.SetPetEquip(&param.pet_equipment_param);
	pet_param.pet_equipment_is_bind = 1;
	if (item_data.is_bind == 0)
	{
		m_module_mgr->GetKnapsack()->SynItemBindInfo(ITEM_COLUMN_TYPE_ITEM, item_data.item_unique_id, true);
	}
	this->PutOn(pet_index, pet_param.equipment_item_unique_id, HUASHEN_GRID_INDEX_TYPE_PET_EQUIPMENT);
	this->ReCalcPetAttr(pet_index);
	this->SetPetItemDirty(pet_index, true, Protocol::SCHuaShenSendPetSingleInfo::PET_NOTIE_TYPE_PET_EQUIPMENT_CHANGE);
	return true;
}
bool HuaShenManager::PutOffPetEquipment(int pet_index)
{
	if (this->PetInvalid(pet_index)) return false;
	PetParam& pet_param = m_pet_grid_list[pet_index].param->pet_param;
	if (pet_param.pet_equipment_id == 0)
	{
		m_module_mgr->NoticeNum(errornum::EN_PET_NOT_TAKE_EQUIPMENT);
		return false;
	}

	const ItemBase * tmp_itembase = ITEMPOOL->GetItem(pet_param.pet_equipment_id);
	if (tmp_itembase == NULL || tmp_itembase->GetItemType() != I_TYPE_PET_EQUIPMENT)
	{
		m_module_mgr->NoticeNum(errornum::EN_ITEM_NOT_ELIGIBLE);
		return false;
	}

	static NetValueItemParam p_nvip; p_nvip.Reset();
	pet_param.GetPetEquip(&p_nvip.pet_equipment_param);

	ItemDataWrapper pet_equip; pet_equip.Reset();
	pet_equip.item_id = pet_param.pet_equipment_id;
	pet_equip.num = 1;
	pet_equip.has_param = 1;
	pet_equip.is_bind = pet_param.pet_equipment_is_bind;
	pet_equip.item_unique_id = pet_param.equipment_item_unique_id;
	p_nvip.GetData(pet_equip.param_data);

	this->TakeOff(pet_equip.item_unique_id);
	if (!this->Put(pet_equip, true))
	{
		m_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return false;
	}
	pet_param.ClearPetEquipment();
	this->ReCalcPetAttr(pet_index);
	this->SetPetItemDirty(pet_index, true, Protocol::SCHuaShenSendPetSingleInfo::PET_NOTIE_TYPE_PET_EQUIPMENT_CHANGE);
	return true;
}
void HuaShenManager::AddPetBendiction(long long vice_item_unique_id)
{
	if(0 == m_param.season_reset) return;
	std::map<long long, int>::iterator it = m_item_map.find(vice_item_unique_id);
	if (it == m_item_map.end())
	{
		return;
	}
	int type = it->second / HUASHEN_GRID_INDEX_TYPE_BASE;
	int index = it->second % HUASHEN_GRID_INDEX_TYPE_BASE;
	if (HUASHEN_GRID_INDEX_TYPE_PET != type || this->PetInvalid(index))
	{
		return;
	}
	this->OnPetGridInfoReset(index);	
	{
		m_module_mgr->NoticeNum(errornum::EN_PET_CANCEL_HUA_SHEN_PET_STATUS);	//策划让拥有化身时默认提示
	}
}
/*
void HuaShenManager::OnPetUp(int pet_index, int cur_level)
{
	if(m_param.season_reset == 0 || this->PetInvalid(pet_index) || cur_level <= 0 || cur_level > MAX_ROLE_LEVEL) return;

	if(m_pet_grid_list[pet_index].param->pet_param.level >= cur_level || m_pet_grid_list[pet_index].param->pet_param.level >= m_param.max_level) return;

	short min_level = GetMin(m_param.max_level, (short)cur_level);

	if (min_level > m_pet_grid_list[pet_index].param->pet_param.level)
	{
		m_pet_grid_list[pet_index].param->pet_param.level = min_level;
	}
}

void HuaShenManager::OnPetIntensi(int pet_index, int pet_level)
{
	if (m_param.season_reset == 0 || this->PetInvalid(pet_index) || pet_level <= 0) return;

	m_pet_grid_list[pet_index].param->pet_param.str_level = pet_level;
	this->ReCalcPetAttr(pet_index);
	this->SetPetItemDirty(pet_index, false);
	//下发协议
}

void HuaShenManager::OnPetChangeName(int pet_index, GameName name)
{
	if(m_param.season_reset == 0 || this->PetInvalid(pet_index)) return;

	memcpy(m_pet_grid_list[pet_index].param->pet_param.name, name, sizeof(GameName));
	this->SetPetItemDirty(pet_index, false);
	//下发协议
}

void HuaShenManager::OnUseSkillBook(int pet_index, int replace_gift_index, int skill_id)
{
	if(m_param.season_reset == 0 || this->PetInvalid(pet_index) || replace_gift_index < 0 || replace_gift_index >= PET_PASSIVE_MAX_COUNT) return;

	m_pet_grid_list[pet_index].param->pet_param.passive_list[replace_gift_index].passive_skill_id = skill_id;
	this->ReCalcPetAttr(pet_index);
	this->SetPetItemDirty(pet_index, false);
	return;
}

void HuaShenManager::OnPetChangeTalent(int pet_index, PetParam * pet_param)
{
	if(m_param.season_reset == 0 || this->PetInvalid(pet_index) || NULL == pet_param) return;

	memcpy(m_pet_grid_list[pet_index].param->pet_param.passive_list, pet_param->passive_list, sizeof(m_pet_grid_list[pet_index].param->pet_param.passive_list));
	memcpy(m_pet_grid_list[pet_index].param->pet_param.new_passive_list, pet_param->new_passive_list, sizeof(m_pet_grid_list[pet_index].param->pet_param.new_passive_list));
	memcpy(m_pet_grid_list[pet_index].param->pet_param.growth, pet_param->growth, sizeof(m_pet_grid_list[pet_index].param->pet_param.growth));
	memcpy(m_pet_grid_list[pet_index].param->pet_param.new_growth, pet_param->new_growth, sizeof(m_pet_grid_list[pet_index].param->pet_param.new_growth));
	m_pet_grid_list[pet_index].param->pet_param.refined_flag = pet_param->refined_flag;
	m_pet_grid_list[pet_index].param->pet_param.comprehend_gift_num = pet_param->comprehend_gift_num;
	this->ReCalcPetAttr(pet_index);
	this->SetPetItemDirty(pet_index, false);
	//下发协议
}

void HuaShenManager::OnComprehendPassiveSkill(int pet_index, PetParam * pet_param, int f_index)
{
	if(m_param.season_reset == 0 || this->PetInvalid(pet_index) || NULL == pet_param || f_index < 0 || f_index >= PET_PASSIVE_MAX_COUNT) return;

	m_pet_grid_list[pet_index].param->pet_param.comprehend_passive_value = pet_param->comprehend_passive_value;
	m_pet_grid_list[pet_index].param->pet_param.comprehend_gift_num = pet_param->comprehend_gift_num;
	m_pet_grid_list[pet_index].param->pet_param.comprehend_gift_num = pet_param->comprehend_gift_num;
	m_pet_grid_list[pet_index].param->pet_param.passive_list[f_index].passive_skill_id = pet_param->passive_list[f_index].passive_skill_id;
	m_pet_grid_list[pet_index].param->pet_param.passive_list[f_index].passive_source = pet_param->passive_list[f_index].passive_source;

	this->ReCalcPetAttr(pet_index);
	this->SetPetItemDirty(pet_index, false);
}
*/
bool HuaShenManager::OnPetGridInfoReset(int pet_index)
{
	bool is_reset = false;
	for (int i = 0; i < NEW_PET_STATUS_MAX; ++i)
	{
		if (pet_index == m_param.new_pet_status_index[i])
		{
			m_param.new_pet_status_index[i] = -1;
			is_reset = true;
			//下发协议
			//this->SendPetOtherInfo();
		}
	}
	return is_reset;
}

void HuaShenManager::ReCalcPetAttr(int pet_index)
{	
	if (pet_index < 0 || pet_index >= ItemNamespace::MAX_HUASHEN_PET_GRID_NUM) return;
	if (this->GetPetAttributeList(pet_index, m_pet_base_attr[pet_index])) return;

	//幻界战场的化身应该都是可以自动回复
	PetParam& pet = m_pet_grid_list[pet_index].param->pet_param;
	AttributeList& attr_list = m_pet_base_attr[pet_index];

	pet.max_hp = attr_list.m_attrs[BATTLE_ATTR_MAX_HP];
	pet.max_mp = attr_list.m_attrs[BATTLE_ATTR_MAX_MP];

	pet.hp = pet.max_hp;
	pet.mp = pet.max_mp;

	attr_list.m_attrs[BATTLE_ATTR_CUR_HP] = pet.hp;
	attr_list.m_attrs[BATTLE_ATTR_CUR_MP] = pet.mp;

	this->SetPetItemDirty(pet_index, false); // 由外部逻辑去notice

	RankManager::Instance().SyncPetRankInfo(m_module_mgr->GetRole(), PET_RANK_TYPE_HUASHEN_CAPABILITY, pet_index);
}

int HuaShenManager::GetPetAttributeList(int pet_index, AttributeList & attr)
{
	static AttributeList temp_attr; temp_attr.Reset();
	static AttributeList no_skill_attr; no_skill_attr.Reset();

	if (this->PetInvalid(pet_index)) return -1;
	const PetParam & pet = m_pet_grid_list[pet_index].param->pet_param;

	const PetCfg * pet_cfg = LOGIC_CONFIG->GetPetConfig()->GetPetCfg(pet.pet_id);
	if (NULL == pet_cfg)
	{
		return -2;
	}
	std::map<int, int> pet_train_up2 = m_module_mgr->GetTrain()->GetPetTrainUp2MapByLevel(m_param.max_level);
	m_module_mgr->GetPet()->GetAttributeListHelper2(pet, temp_attr, no_skill_attr, m_param.max_level, m_pet_grid_list[pet_index].item_unique_id, 0,  
		this->IsWaitForFightPet(pet_index), m_param.pet_train.level, &pet_train_up2, true);
	
	const ActivityHuanJieZhanChangEquipAttrLimitCfg * limit_cfg = LOGIC_CONFIG->GetActivityHuanJieZhanChangConfig()->GetEquipAttrLimitCfg(HUAN_JIE_HUA_SHEN_ATTR_LIMIT_TYPE_PET, 0, pet_cfg->main_attribute_id);
	if (NULL != limit_cfg)
	{
		for (int i = 0; i < HJZC_EQUIP_ATTR_LIMIT_MAX_NUM; i++)
		{
			const ActivityHuanJieZhanChangEquipAttrLimitCfg::AttrConCfg & con_cfg = limit_cfg->attr_limit_list[i];
			if (temp_attr.m_attrs[con_cfg.attr_type] > con_cfg.attr_value)
			{
				temp_attr.m_attrs[con_cfg.attr_type] = con_cfg.attr_value;
			}
			if (no_skill_attr.m_attrs[con_cfg.attr_type] > con_cfg.attr_value)
			{
				no_skill_attr.m_attrs[con_cfg.attr_type] = con_cfg.attr_value;
			}
		}
	}

	attr = temp_attr;
	m_pet_attr_no_skill[pet_index] = no_skill_attr;

	return 0;
}

const AttributeList * HuaShenManager::GetPetAttrList(long long item_unique_id)
{
	std::map<long long, int>::iterator it = m_item_map.find(item_unique_id);
	if (it == m_item_map.end())
	{
		return NULL;
	}
	int type = it->second / HUASHEN_GRID_INDEX_TYPE_BASE;
	int index = it->second % HUASHEN_GRID_INDEX_TYPE_BASE;
	if (HUASHEN_GRID_INDEX_TYPE_PET != type)
	{
		return NULL;
	}
	if (this->PetInvalid(index))
	{
		return NULL;
	}

	return &m_pet_base_attr[index];
}

int HuaShenManager::GetPetStrBuffLevel(int pet_index)
{
	return 1;
}

bool HuaShenManager::IsWaitForFightPet(int pet_index)
{
	for (int i = NEW_FIGHT_INDEX_BEGIN; i <= NEW_FIGHT_INDEX_END; i++)
	{
		if (pet_index == m_param.new_pet_status_index[i])
		{
			return false;
		}
	}
	
	return true;
}

int HuaShenManager::GetPetCapability(int pet_index)
{
	if (pet_index < 0 || pet_index >= ARRAY_LENGTH(m_pet_attr_no_skill) || this->PetInvalid(pet_index))
	{
		return 0;
	}
	const PetCfg * pet_cfg = LOGIC_CONFIG->GetPetConfig()->GetPetCfg(m_pet_grid_list[pet_index].param->pet_param.pet_id);
	if (NULL == pet_cfg)
	{
		return 0;
	}

	return ReCalcHuaShenPetCapability(m_pet_attr_no_skill[pet_index].m_attrs) + GetPetSkillCapability(pet_index) + pet_cfg->base_score;
}

int HuaShenManager::GetPetSkillCapability(int pet_index)
{
	if (this->PetInvalid(pet_index)) return 0;
	PetParam& pet = m_pet_grid_list[pet_index].param->pet_param;

	const PetSkillAddCapabilityCfg * add_capability_cfg = LOGIC_CONFIG->GetPetConfig()->GetPetSkillAddCapabilityCfg(pet.level);
	if (NULL == add_capability_cfg) return 0;

	PetEffectSkillId skill_list = this->GetPetEffectiveSkill(pet_index);

	int skill_capability = 0;
	for (int i = 0; i < (int)skill_list.passive_list.size(); ++i)
	{
		int skill_id = skill_list.passive_list[i].skill_id;
		int skill_level = skill_list.passive_list[i].skill_level;
		const GamePassiveSkillCfg * passive_skill = SkillPool::Instance().GetPassiveSkillCfg(skill_id);
		if (passive_skill == NULL)
		{
			continue;
		}

		switch (passive_skill->skill_priority)
		{
		case PET_SKILL_PRIORITY_TYPE_HIGH:
		{
			skill_capability += add_capability_cfg->high_skill_add;
		}
		break;
		case PET_SKILL_PRIORITY_TYPE_SUPER:
		{
			if (0 <= skill_level && skill_level < (int)add_capability_cfg->super_skill_add.size())
			{
				skill_capability += add_capability_cfg->super_skill_add[skill_level];
			}
		}
		break;
		default:
		{
			skill_capability += add_capability_cfg->low_skill_add;
		}
		break;
		}
	}
	//灵骑
	std::vector<SkillPro> smart_mounts_base_skill_list;
	m_module_mgr->GetSmartMountsSystem()->GetBaseSkillList(m_pet_grid_list[pet_index].item_unique_id, &smart_mounts_base_skill_list);
	if (!smart_mounts_base_skill_list.empty())
	{
		for (int i = 0; i < (int)smart_mounts_base_skill_list.size(); i++)
		{
			skill_capability += LOGIC_CONFIG->GetSmartMountsSystemConfig()->GetSkillScoreCfgByIdAndLevel(smart_mounts_base_skill_list[i].skill_id, smart_mounts_base_skill_list[i].skill_level);
		}
	}
	//家族灵脉
	skill_capability += m_module_mgr->GetRoleGuildInborn()->GetSkillScroe(GUILD_INBORN_ADD_TARGET_TYPE_PET);
	//神印效果
	skill_capability += m_module_mgr->GetPet()->GetGodPrintEffectScore(pet);
	//宠物祝福
	int pet_benediction_sp_capability = 0;
	m_module_mgr->GetPet()->GetPetBenedictionSpId(pet, m_pet_grid_list[pet_index].item_unique_id, &pet_benediction_sp_capability);
	skill_capability += pet_benediction_sp_capability;
	//宠物天赋
	std::vector<int> talent_sp_list;
	skill_capability += m_module_mgr->GetPet()->GetPetTalentSpIdList(pet, talent_sp_list);

	return skill_capability;
}

int HuaShenManager::ReCalcHuaShenPetCapability(const Attribute * attrs)
{
	double gongjixishu_1 = 1 + attrs[BATTLE_ATTR_CRITICAL_INC_VALUE] * 0.00081 + (attrs[BATTLE_ATTR_CRITICAL] + attrs[BATTLE_ATTR_CRITICAL_DEC_VALUE]) *
							0.000242 + attrs[BATTLE_ATTR_COUNTER_ATTACK] * 0.0054;
	double gongjixishu_2 = 1 + (attrs[BATTLE_ATTR_HIT] + attrs[BATTLE_ATTR_DODGE]) * 0.001;
	double jingshenxishu_1 = 1 + 1.2 * attrs[BATTLE_ATTR_MAGIC_ATTACK] / (attrs[BATTLE_ATTR_MAGIC_ATTACK] + 220) + 0.8 * attrs[BATTLE_ATTR_MAGIC_DEFENSE] /
							(attrs[BATTLE_ATTR_MAGIC_DEFENSE] + 300);
	int capability = (int)(attrs[BATTLE_ATTR_MAX_MP] * 1.6 + attrs[BATTLE_ATTR_MAX_HP] * 0.25 + attrs[BATTLE_ATTR_ATTACK] * 1.55 * gongjixishu_1 * gongjixishu_2 +
						attrs[BATTLE_ATTR_DEFENSE] * 1.2 + attrs[BATTLE_ATTR_MENTAL] * 1.95 * jingshenxishu_1 + attrs[BATTLE_ATTR_RECOVERY] * 1.85 +
						attrs[BATTLE_ATTR_AGILITY] * 2.2);
	return capability;
}

bool HuaShenManager::PetInvalid(int index)
{
	return	index < 0 || index >= MAX_HUASHEN_PET_GRID_NUM || m_pet_grid_list[index].Invalid() || NULL == m_pet_grid_list[index].param;
}

void HuaShenManager::GetChangePetItemList(int * count, ItemListParam::ItemListData * save_list)
{
	if (!m_pet_grid_is_change)
	{
		*count = 0;
		return;
	}
	int tmp_count = 0;
	for (int i = 0; i < MAX_HUASHEN_PET_GRID_NUM && tmp_count < *count; ++i)
	{
		if (!m_pet_dirty_mark[i]) continue;

		if (this->PetInvalid(i))
		{
			if (!m_pet_old_state[i]) continue;

			save_list[tmp_count].change_state_item = structcommon::CS_DELETE;
		}
		else
		{
			if (m_pet_old_state[i])
			{
				save_list[tmp_count].change_state_item = structcommon::CS_UPDATE;
			}
			else
			{
				save_list[tmp_count].change_state_item = structcommon::CS_INSERT;
			}
		}

		save_list[tmp_count].index = i + ItemNamespace::PET_HUASHEN_INDEX_BEGIN;

		ItemDataWrapper & item_data_wrapper = save_list[tmp_count].item_wrapper;
		const ItemGridData & grid_data = m_pet_grid_list[i];
		item_data_wrapper.item_id = grid_data.item_id;
		item_data_wrapper.num = grid_data.num;
		item_data_wrapper.is_bind = (short)grid_data.is_bind;
		item_data_wrapper.invalid_time = grid_data.invalid_time;
		item_data_wrapper.has_param = (NULL != grid_data.param) ? 1 : 0;
		item_data_wrapper.gold_price = grid_data.gold_price;
		item_data_wrapper.item_unique_id = grid_data.item_unique_id;
		if (NULL != grid_data.param)
		{
			memcpy(item_data_wrapper.param_data, grid_data.param->data, sizeof(NetValueItemParamData));
		}

		++tmp_count;
	}
	*count = tmp_count;
}

void HuaShenManager::SetPetItemDirty(int index, bool to_notice_client, int dirty_type)
{
	if (index < 0 || index >= MAX_HUASHEN_PET_GRID_NUM) return;
	
	m_pet_dirty_mark[index] = true;
	m_pet_grid_is_change = true;
	if (to_notice_client)
	{
		this->SendPetSingleInfo(index, dirty_type);
	}
}

void HuaShenManager::ClearPetDirtyMark()
{
	if(!m_pet_grid_is_change) return;
	
	for (int i = 0; i < MAX_HUASHEN_PET_GRID_NUM; i++)
	{
		if (m_pet_dirty_mark[i]) m_pet_old_state[i] = !this->PetInvalid(i);
	}
	memset(m_pet_dirty_mark, 0, sizeof(m_pet_dirty_mark));
	m_pet_grid_is_change = false;
}

void HuaShenManager::SendPetAllInfo(int reason)
{
	Protocol::SCHuaShenSendPetAllInfo info;
	info.reason = reason;
	info.pet_count = 0;
	int offset = 0;
	for (int i = 0; i < ARRAY_LENGTH(info.pet_list) && offset < ARRAY_LENGTH(info.pet_list); ++i)
	{
		if (this->PetInvalid(i)) continue;

		info.pet_list[offset].index = i;
		info.pet_list[offset].pet_param = m_pet_grid_list[i].param->pet_param;
		info.pet_list[offset].is_bind = m_pet_grid_list[i].is_bind;
		info.pet_list[offset].capability = this->GetPetCapability(i);
		info.pet_list[offset].pet_unique_id = m_pet_grid_list[i].item_unique_id;
		info.pet_count++;
		offset += 1;
	}

	int len = sizeof(info) - (ARRAY_LENGTH(info.pet_list) - offset) * sizeof(info.pet_list[0]);
	m_module_mgr->NetSend((const char*)&info, len);
}

void HuaShenManager::SendPetSingleInfo(int pet_index, int notie_type)
{
	if (this->PetInvalid(pet_index)) return;
	PetParam& pet = m_pet_grid_list[pet_index].param->pet_param;

	Protocol::SCHuaShenSendPetSingleInfo info;
	info.notie_type = notie_type;
	info.pet_info.index = pet_index;
	info.pet_info.capability = this->GetPetCapability(pet_index);
	info.pet_info.pet_param = pet;
	info.pet_info.pet_unique_id = m_pet_grid_list[pet_index].item_unique_id;
	m_module_mgr->NetSend((const char*)&info, sizeof(info));
}

void HuaShenManager::SendPetOtherInfo()
{
	Protocol::SCHuaShenSendPetOtherInfo info;

	info.fight_index = m_param.new_pet_status_index[NEW_FIGHT_INDEX];
	info.standby_index1 = m_param.new_pet_status_index[NEW_STANDBY_INDEX1];
	info.standby_index2 = m_param.new_pet_status_index[NEW_STANDBY_INDEX2];
	info.standby_index3 = m_param.new_pet_status_index[NEW_STANDBY_INDEX3];
	info.standby_index4 = m_param.new_pet_status_index[NEW_STANDBY_INDEX4];
	info.standby_index5 = m_param.new_pet_status_index[NEW_STANDBY_INDEX5];

	m_module_mgr->NetSend((const char*)&info, sizeof(info));
	
}

bool HuaShenManager::SendPetAttrInfo(int pet_index)
{
	if (this->PetInvalid(pet_index)) return false;
	static Protocol::SCHuaShenSendPetAttrListInfo info;
	info.attr_list = m_pet_base_attr[pet_index];
	info.index = pet_index;
	info.capability = this->GetPetCapability(pet_index);

	m_module_mgr->NetSend((const char*)&info, sizeof(info));
	return true;
}

bool HuaShenManager::GetBattleCharacterData(int pet_index, PetBattleData * out_data, bool is_use_cur_hp_mp)
{
	if (NULL == out_data) return false;
	if (this->PetInvalid(pet_index)) return false;
	const PetParam& pet = m_pet_grid_list[pet_index].param->pet_param;

	const PetCfg *pet_cfg = LOGIC_CONFIG->GetPetConfig()->GetPetCfg(pet.pet_id);
	if (NULL == pet_cfg) return false;

	BattleCharacterData& bcd = out_data->battle_data;
	bcd.Reset();
	memset(out_data->passive_list, 0, sizeof(out_data->passive_list));
	memset(out_data->other_sp_list, 0, sizeof(out_data->other_sp_list));
	out_data->exclusive_passive_skill_OBSOLETE = 0;
	out_data->auto_move_type = 0;
	out_data->auto_skill_id = 0;
	out_data->auto_skill_level = 0;

	bcd.character_id = pet.pet_id;
	bcd.id_param = pet_index;
	bcd.character_type = BATTLE_CHARACTER_TYPE_PET;
	bcd.level = pet.level;
	memcpy(bcd.name, pet.name, sizeof(GameName));
	bcd.race = pet_cfg->race;
	bcd.is_use_cur_hp_mp = (is_use_cur_hp_mp ? 1 : 0);
	bcd.is_long_range_unit = static_cast<char>(pet_cfg->is_long_range_unit);
	bcd.skill_num = 0;

	static AttributeList attr_list;
	attr_list.Reset();
	this->GetPetAttributeList(pet_index, attr_list);
	memcpy(bcd.attr_list, attr_list.m_attrs, sizeof(bcd.attr_list));

	if (is_use_cur_hp_mp)
	{
		bcd.attr_list[BATTLE_ATTR_MAX_HP] = pet.max_hp;
		bcd.attr_list[BATTLE_ATTR_MAX_MP] = pet.max_mp;
		bcd.attr_list[BATTLE_ATTR_CUR_HP] = pet.hp;
		bcd.attr_list[BATTLE_ATTR_CUR_MP] = pet.mp;
	}
	else
	{
		bcd.attr_list[BATTLE_ATTR_MAX_HP] = pet.max_hp;
		bcd.attr_list[BATTLE_ATTR_MAX_MP] = pet.max_mp;
		bcd.attr_list[BATTLE_ATTR_CUR_HP] = pet.max_hp;
		bcd.attr_list[BATTLE_ATTR_CUR_MP] = pet.max_mp;
	}

	PetEffectSkillId skill_list = this->GetPetEffectiveSkill(pet_index);

	for (int i = 0; i < (int)skill_list.passive_list.size() && i < BATTLE_PET_GIFT_NUM; ++i)
	{
		out_data->passive_list[i] = skill_list.passive_list[i].skill_id;
		out_data->passive_level_list[i] = skill_list.passive_list[i].skill_level;

		/*gamelog::g_log_debug.printf(LL_INFO, "%s role[%d,%s] pet[%s] passive_skill_id[%d] ", __FUNCTION__,
			m_module_mgr->GetRole()->GetUID(), m_module_mgr->GetRole()->GetName(),
			pet.name, out_data->passive_list[i]);*/

	}
	short other_sp_num = 0;
	//灵骑
	m_module_mgr->GetSmartMountsSystem()->GetBaseSpecialEffectList(m_pet_grid_list[pet_index].item_unique_id, BATTLE_OTHER_EFFECT_NUM, &other_sp_num, out_data->other_sp_list);
	//家族灵脉
	m_module_mgr->GetRoleGuildInborn()->GetPassiveSkillByType(GUILD_INBORN_ADD_TARGET_TYPE_PET, BATTLE_OTHER_EFFECT_NUM, &other_sp_num, out_data->other_sp_list);
	//宠物祝福
	if (other_sp_num < BATTLE_OTHER_EFFECT_NUM)
	{
		int pet_benediction_sp_id = m_module_mgr->GetPet()->GetPetBenedictionSpId(pet, m_pet_grid_list[pet_index].item_unique_id);
		const SpecialEffectCfg * spe_cfg = BattleSpecialEffectPool::GetInstance().GetSpecialEffectCfg(pet_benediction_sp_id);
		if (NULL != spe_cfg)
		{
			out_data->other_sp_list[other_sp_num++] = pet_benediction_sp_id;
		}
	}
	//魂器特效
	m_module_mgr->GetPet()->GetSoulEffectList(pet, BATTLE_OTHER_EFFECT_NUM, &other_sp_num, out_data->other_sp_list);
	//神印特效
	m_module_mgr->GetPet()->GetGodPrintEffectList(pet, BATTLE_OTHER_EFFECT_NUM, &other_sp_num, out_data->other_sp_list);
	//宠物天赋
	m_module_mgr->GetPet()->GetPetTalentBattleSpecialEffectList(pet, BATTLE_OTHER_EFFECT_NUM, &other_sp_num, out_data->other_sp_list);

	out_data->auto_move_type = pet.pet_auto_move_type;
	out_data->auto_skill_id = pet.pet_auto_skill_id;
	out_data->auto_skill_level = pet.pet_auto_skill_level;

	bcd.param = (char)pet.str_level;

	bcd.capability = this->GetPetCapability(pet_index);
	out_data->skin_id = pet.skin_id;
	out_data->unique_id = m_pet_grid_list[pet_index].item_unique_id;
	out_data->lianyao_pet_id = pet.lian_yao_pet_id;
	out_data->lianyao_pet_str_lv = pet.lian_yao_pet_str_lv;
	return true;
}

PetEffectSkillId HuaShenManager::GetPetEffectiveSkill(int pet_index)
{
	PetEffectSkillId ret;

	if (this->PetInvalid(pet_index)) return ret;
	const PetParam & pet = m_pet_grid_list[pet_index].param->pet_param;

	return Pet::GetPetEffectiveSkillHelper(pet);
}

const AttributeList & HuaShenManager::ReCalcTrainAttr(AttributeList & base_add, bool is_recalc)
{
	if (is_recalc)
	{
		m_attrs_train_add.Reset();

		int add_attr = 0;
		const TrainCfg * cfg = LOGIC_CONFIG->GetTrainConfig()->GetTrainCfg(TRAIN_PHYSICAL_DEC, m_param.train_list[TRAIN_PHYSICAL_DEC].level);
		if (NULL != cfg && 0 != cfg->attribute_value)
		{
			add_attr = (int)(base_add.m_attrs[BATTLE_ATTR_DEFENSE] * 10.0 / cfg->attribute_value); //防御/参数得出的是百分比
			m_attrs_train_add.m_attrs[BATTLE_ATTR_PHYSICAL_DEC_DAMAGE] += (0 != cfg->level_limit && add_attr >= cfg->level_limit) ? cfg->level_limit : add_attr;
		}

		cfg = LOGIC_CONFIG->GetTrainConfig()->GetTrainCfg(TRAIN_MAGIC_DEC, m_param.train_list[TRAIN_MAGIC_DEC].level);
		if (NULL != cfg && 0 != cfg->attribute_value)
		{
			add_attr = (int)(base_add.m_attrs[BATTLE_ATTR_DEFENSE] * 10.0 / cfg->attribute_value); //防御/参数得出的是百分比
			m_attrs_train_add.m_attrs[BATTLE_ATTR_MAGIC_DEC_DAMAGE] += (0 != cfg->level_limit && add_attr >= cfg->level_limit) ? cfg->level_limit : add_attr;
		}

		for (int i = TRAIN_BEGIN; i < TRAIN_ENG; ++i)
		{
			const TrainAttrCfg * attr_cfg = LOGIC_CONFIG->GetTrainConfig()->GetTrainAttrCfg(m_module_mgr->GetRole()->GetBaseProfession(), i, m_param.train_list[i].level);
			if (NULL != attr_cfg && 0 <= attr_cfg->attribute_type && attr_cfg->attribute_type < BATTLE_ATTR_MAX)
			{
				m_attrs_train_add.m_attrs[attr_cfg->attribute_type] += attr_cfg->attribute_value;
			}
		}
	}

	base_add.Add(m_attrs_train_add.m_attrs);
	return m_attrs_train_add;
}

const AttributeList & HuaShenManager::ReCalcTrainAttrPer(AttributeList & base_add, const AttributeList & cpy_attr_list)
{
	m_attrs_train_per.Reset();

	int add_attr = 0;
	const TrainCfg *cfg = LOGIC_CONFIG->GetTrainConfig()->GetTrainCfg(TRAIN_ATTACK, m_param.train_list[TRAIN_ATTACK].level);
	if (NULL != cfg)
	{
		add_attr = (int)(cfg->attribute_value / 10000.0 * cpy_attr_list.m_attrs[BATTLE_ATTR_ATTACK]);
		m_attrs_train_per.m_attrs[BATTLE_ATTR_ATTACK] += (0 != cfg->level_limit && add_attr >= cfg->level_limit) ? cfg->level_limit : add_attr;
	}

	cfg = LOGIC_CONFIG->GetTrainConfig()->GetTrainCfg(TRAIN_MENTAL, m_param.train_list[TRAIN_MENTAL].level);
	if (NULL != cfg)
	{
		add_attr = (int)(cfg->attribute_value / 10000.0 * cpy_attr_list.m_attrs[BATTLE_ATTR_MENTAL]);
		m_attrs_train_per.m_attrs[BATTLE_ATTR_MENTAL] += (0 != cfg->level_limit && add_attr >= cfg->level_limit) ? cfg->level_limit : add_attr;
	}

	cfg = LOGIC_CONFIG->GetTrainConfig()->GetTrainCfg(TRAIN_MAX_HP, m_param.train_list[TRAIN_MAX_HP].level);
	if (NULL != cfg)
	{
		add_attr = (int)(cfg->attribute_value / 10000.0 * cpy_attr_list.m_attrs[BATTLE_ATTR_MAX_HP]);
		m_attrs_train_per.m_attrs[BATTLE_ATTR_MAX_HP] += (0 != cfg->level_limit && add_attr >= cfg->level_limit) ? cfg->level_limit : add_attr;
	}

	cfg = LOGIC_CONFIG->GetTrainConfig()->GetTrainCfg(TRAIN_MAX_MP, m_param.train_list[TRAIN_MAX_MP].level);
	if (NULL != cfg)
	{
		add_attr = (int)(cfg->attribute_value / 10000.0 * cpy_attr_list.m_attrs[BATTLE_ATTR_MAX_MP]);
		m_attrs_train_per.m_attrs[BATTLE_ATTR_MAX_MP] += (0 != cfg->level_limit && add_attr >= cfg->level_limit) ? cfg->level_limit : add_attr;
	}

	cfg = LOGIC_CONFIG->GetTrainConfig()->GetTrainCfg(TRAIN_MAIN_ATTR, m_param.train_list[TRAIN_MAIN_ATTR].level);
	if (NULL != cfg)
	{
		int attr_add_type = LOGIC_CONFIG->GetTrainConfig()->GetMainAttrType(m_module_mgr->GetRole()->GetProfession());
		if (attr_add_type >= 0 && attr_add_type < BATTLE_ATTR_MAX)
		{
			add_attr = (int)(cfg->attribute_value / 10000.0 * cpy_attr_list.m_attrs[attr_add_type]);
			m_attrs_train_per.m_attrs[attr_add_type] += (0 != cfg->level_limit && add_attr >= cfg->level_limit) ? cfg->level_limit : add_attr;
		}
	}

	cfg = LOGIC_CONFIG->GetTrainConfig()->GetTrainCfg(TRAN_DEFENSE, m_param.train_list[TRAN_DEFENSE].level);
	if (NULL != cfg)
	{
		add_attr = (int)(cfg->attribute_value / 10000.0 * cpy_attr_list.m_attrs[BATTLE_ATTR_DEFENSE]);
		m_attrs_train_per.m_attrs[BATTLE_ATTR_DEFENSE] += (0 != cfg->level_limit && add_attr >= cfg->level_limit) ? cfg->level_limit : add_attr;
	}

	cfg = LOGIC_CONFIG->GetTrainConfig()->GetTrainCfg(TRAN_AGILITY, m_param.train_list[TRAN_AGILITY].level);
	if (NULL != cfg)
	{
		add_attr = (int)(cfg->attribute_value / 10000.0 * cpy_attr_list.m_attrs[BATTLE_ATTR_AGILITY]);
		m_attrs_train_per.m_attrs[BATTLE_ATTR_AGILITY] += (0 != cfg->level_limit && add_attr >= cfg->level_limit) ? cfg->level_limit : add_attr;
	}

	cfg = LOGIC_CONFIG->GetTrainConfig()->GetTrainCfg(TRAN_MAGIC_DEFENSE, m_param.train_list[TRAN_MAGIC_DEFENSE].level);
	if (NULL != cfg)
	{
		m_attrs_train_per.m_attrs[BATTLE_ATTR_MAGIC_DEFENSE] += (int)(cfg->attribute_value / 10000.0 * cpy_attr_list.m_attrs[BATTLE_ATTR_MAGIC_DEFENSE]);
	}

	base_add.Add(m_attrs_train_per.m_attrs);
	return m_attrs_train_per;
}

void HuaShenManager::SynBenZunTrain()
{
	if(m_param.season_reset == 0) return;

	m_module_mgr->GetTrain()->SynHuaShenTrainData(&m_param.pet_train, m_param.train_list, TARIN_TYPE_MAX);
	//限制人物修炼技能
	for (int i = 0; i < TARIN_TYPE_MAX && i < TRAIN_ENG; i++)
	{
		int max_train_level = LOGIC_CONFIG->GetTrainConfig()->GetMaxTrainLevel(i, m_param.max_level);
		if (m_param.train_list[i].level > max_train_level)
		{
			m_param.train_list[i].level = max_train_level;
		}
	}

	//限制宠物修炼技能
	if (m_param.pet_train.level > max_pet_train_level)
	{
		m_param.pet_train.level = max_pet_train_level;
	}
}

void HuaShenManager::OnTrainUp(int type, int skill_level)
{
	if (m_param.season_reset == 0 || type < 0 || type >= TRAIN_ENG) return;

	int max_train_level = LOGIC_CONFIG->GetTrainConfig()->GetMaxTrainLevel(type, m_param.max_level);
	if(skill_level > max_train_level) return;

	if (skill_level > m_param.train_list[type].level)
	{
		m_param.train_list[type].level = skill_level;
		this->ReCalcAttr(AttributeList::RECALC_REASON_TRAIN, __FUNCTION__);
	}
}

void HuaShenManager::OnPetTrainUp(int skill_level)
{
	if (m_param.season_reset == 0 || skill_level <= 0 || skill_level > max_pet_train_level) return;
	
	if (skill_level <= m_param.pet_train.level) return;

	m_param.pet_train.level = skill_level;
	for (int i = 0; i < MAX_HUASHEN_PET_GRID_NUM; i++)
	{
		if(this->PetInvalid(i)) continue;

		this->ReCalcPetAttr(i);
	}
	//下发所有宠物信息
	this->SendPetAllInfo(Protocol::SCHuaShenSendPetAllInfo::HUASHEN_PET_ALL_INFO_REASON_TRAIN_SKILL);
}
const AttributeList & HuaShenManager::ReCalcAdvAttr(AttributeList & base_add, bool is_recalc)
{
	if (is_recalc)
	{
		m_base_attr_prof.Reset();
		const CareerAdvanceCfg* cfg = LOGIC_CONFIG->GetProfessionConfig()->GetCareerAdvanceCfg(m_module_mgr->GetRole()->GetBaseProfession() + role_advance_times , m_module_mgr->GetAdvanceCareer()->GetSubLevel());
		if (NULL == cfg)
		{
			return m_base_attr_prof;
		}
		m_base_attr_prof.Add(cfg->m_attrs);
	}
	base_add.Add(m_base_attr_prof.m_attrs);
	return m_base_attr_prof;
}
void HuaShenManager::CalcMaxAdvTimes()
{
	int adv_times = m_module_mgr->GetRole()->GetAdvanceTimes();
	if (adv_times <= max_advance_times)
	{
		role_advance_times = adv_times;
		return;
	}

	int count = 0;
	for (int i = m_module_mgr->GetRole()->GetBaseProfession() + PROFESSION_ADVANCE_TIMES; i != 0 && count < 10; count++)
	{
		const CareerAdvanceCfg* cfg = LOGIC_CONFIG->GetProfessionConfig()->GetCareerAdvanceCfg(i, m_module_mgr->GetAdvanceCareer()->GetSubLevel());
		if(NULL == cfg) continue;

		if (cfg->need_level <= m_param.max_level)
		{
			max_advance_times = cfg->profession_id % PROFESSION_BASE;
			break;
		}
		i = cfg->pre_profession;
	}

	role_advance_times = adv_times > max_advance_times ? max_advance_times : adv_times;
}
/*
void HuaShenManager::ReCalcFaBaoAttr(AttributeList & base_add, bool is_recalc)
{
	if (is_recalc)
	{
		m_attrs_fabao_add.Reset();

		for (int i = 0; i < FABAO_EQUIP_BAR_MAX; ++i)
		{
			if (!m_module_mgr->GetFaBao()->IsActiveFaBao(m_param.m_fabao_equipment_bar[i]))continue;
			
			const FaBaoEquip * equip = m_module_mgr->GetFaBao()->GetFaBaoEquip(m_param.m_fabao_equipment_bar[i]);
			if(NULL == equip || equip->fabao_type != FABAO_TYPE_PASSIVE)continue;

			const FaBaoInfoCfg * equip_info = LOGIC_CONFIG->GetFaBaoConfig()->GetFaBaoInfoCfg(m_param.m_fabao_equipment_bar[i]);
			if (NULL == equip_info || equip_info->target != FABAO_TARGET_ROLE)continue;
			// 佩戴时计算等级与加成属性
			m_attrs_fabao_add.m_attrs[equip->level_effect] += static_cast<Attribute>(m_param.max_level * (equip_info->base_addition + 
							((equip_info->level_addition * equip->fabao_level) / 1000.0f)) + equip_info->advanced_addition * equip->fabao_jie);
		}
	}

	base_add.Add(m_attrs_fabao_add.m_attrs);
}

void HuaShenManager::FaBaoPutOn(int equip_bar_index, int fabao_index)
{
	if (equip_bar_index < 0 || equip_bar_index >= MAX_FABAO_EQUIPMENT_BAR || fabao_index < 0 || fabao_index >= MAX_FABAO_COUNT)
	{
		m_module_mgr->NoticeNum(errornum::EN_FABAO_NO_THIS_FABAO);
		return;
	}
	if (!m_module_mgr->GetFaBao()->IsActiveFaBao(fabao_index))
	{
		m_module_mgr->NoticeNum(errornum::EN_FABAO_NO_ACTIVE_FABAO);
		return;
	}
	const FaBaoEquip * equip = m_module_mgr->GetFaBao()->GetFaBaoEquip(fabao_index);
	if(NULL == equip) return;

	if (equip->fabao_type != FABAO_TYPE_PASSIVE)
	{
		m_module_mgr->NoticeNum(errornum::EN_FABAO_PASSIVE_NEED_ERR);
		return;
	}
	for (int i = 0; i < MAX_FABAO_EQUIPMENT_BAR; ++i)
	{
		if (m_param.m_fabao_equipment_bar[i] == fabao_index)
		{
			m_module_mgr->NoticeNum(errornum::EN_FACE_SCORE_NOT_REPEAT_WEAR);
			return;
		}
	}
	m_param.m_fabao_equipment_bar[equip_bar_index] = fabao_index;
	this->ReCalcAttr(AttributeList::RECALC_REASON_FABAO, __FUNCTION__);
}

void HuaShenManager::FabaoTakeOff(int equip_bar_index)
{
	if (equip_bar_index < 0 || equip_bar_index >= MAX_FABAO_EQUIPMENT_BAR)
	{
		m_module_mgr->NoticeNum(errornum::EN_FABAO_NO_THIS_FABAO);
		return;
	}
	if (m_param.m_fabao_equipment_bar[equip_bar_index] == FABAO_EQUIP_BAR_NO)
	{
		m_module_mgr->NoticeNum(errornum::EN_FABAO_NO_EQUIP_ERR);
		return;
	}
	m_param.m_fabao_equipment_bar[equip_bar_index] = FABAO_EQUIP_BAR_NO;

	this->ReCalcAttr(AttributeList::RECALC_REASON_FABAO, __FUNCTION__);
}
*/
void HuaShenManager::OnLoginSynBenZunSkillInfo(RoleSkill::SkillItem * skill_list)
{
	if(m_param.season_reset == 0 || NULL == skill_list) return;

	memcpy(m_skill_list, skill_list, sizeof(m_skill_list));
	int prof_max_skill_level = 0;
	int skill_level_limit = 0;
	for (int i = 0; i < MAX_SKILL_NUM; ++i)
	{
		if (m_skill_list[i].Invalid()) continue;

		const SkillCfg* skill_cfg = SkillPool::Instance().GetSkillCfg(m_skill_list[i].skill_id);
		if(NULL == skill_cfg) continue;

		skill_level_limit = skill_cfg->GetMaxLevelForRoleLevel(m_param.max_level);

		if (m_skill_list[i].skill_level > skill_level_limit)
		{
			m_skill_list[i].skill_level = skill_level_limit;
		}
		prof_max_skill_level = LOGIC_CONFIG->GetProfessionConfig()->GetMaxLearnSkillLevel(m_module_mgr->GetRole(), m_skill_list[i].skill_id);
		m_skill_list[i].skill_show_level = GetMin(prof_max_skill_level, (int)m_skill_list[i].skill_level);
	}
}

RoleSkill::SkillItem * HuaShenManager::GetSkill(short skill_index)
{
	if (skill_index < 0 || skill_index >= MAX_SKILL_NUM)
	{
		return NULL;
	}

	return &m_skill_list[skill_index];
}

void HuaShenManager::AddSkill(RoleSkill::SkillItem * skill_item)
{
	if(m_param.season_reset == 0 || NULL == skill_item) return;

	m_skill_list[skill_item->index] = *skill_item;
	//发送技能信息

}

void HuaShenManager::SkillUp(int skill_id, int skill_level)
{
	if(m_param.season_reset == 0 || skill_level <= 0) return;

	const SkillCfg* skill_cfg = SkillPool::Instance().GetSkillCfg(skill_id);
	if (NULL == skill_cfg) return;
	int skill_level_limit = skill_cfg->GetMaxLevelForRoleLevel(m_param.max_level);
	for (int i = 0; i < MAX_SKILL_NUM; ++i)
	{
		if (m_skill_list[i].Invalid()) continue;

		if (m_skill_list[i].skill_id == skill_id)
		{
			if (m_skill_list[i].skill_level >= skill_level_limit)
			{
				break;
			}
			int prof_max_skill_level = LOGIC_CONFIG->GetProfessionConfig()->GetMaxLearnSkillLevel(m_module_mgr->GetRole(), skill_id);
			m_skill_list[i].skill_level = skill_level;
			m_skill_list[i].skill_show_level = GetMin(prof_max_skill_level, (int)m_skill_list[i].skill_level);
		}
	}
	//发送技能信息

}

const AttributeList & HuaShenManager::ReCalcSkillAttr(AttributeList & base_add, bool is_recalc)
{
	// 不管是否recalc都要计算，因为技能里有被动技能是按某些属性的值加其他属性的值
	m_attrs_skill_add.Reset();
	for (int i = 0; i < MAX_SKILL_NUM; ++i)
	{
		if (m_skill_list[i].Invalid()) continue;
		if (m_skill_list[i].skill_show_level <= 0) continue;

		int skill_id = m_skill_list[i].skill_id;
		int skill_level = m_skill_list[i].skill_show_level;
		if (skill_level > MAX_BATTLE_SKILL_LEVEL) continue;

		const GamePassiveSkillCfg * passive_skill = SkillPool::Instance().GetPassiveSkillCfg(skill_id);
		if (NULL == passive_skill) continue;

		int spid = passive_skill->sp_id;
		if (passive_skill->is_level_spid)
		{
			spid = passive_skill->level_spid_list[skill_level];
		}

		const SpecialEffectCfg * spe_cfg = BattleSpecialEffectPool::GetInstance().GetSpecialEffectCfg(spid);
		if (NULL == spe_cfg) continue;

		for (int k = 0; k < spe_cfg->effect_num && k < EFFECT_DETAIL_NUM; ++k)
		{
			const SpecialEffectKey & key = spe_cfg->effect_detail_list[k];
			if (key.effect_type != SPECIAL_EFFECT_TYPE_ADD_ATTR) continue;

			const EffectDetailCfg * detail_cfg = BattleSpecialEffectPool::GetInstance().GetEffectDetailCfg(key);
			if (NULL == detail_cfg) continue;

			if (SPE_ATTR_ADD_ATTR == detail_cfg->effect_sub_type && detail_cfg->param_num >= 2
				&& detail_cfg->param_list[0] >= BATTLE_ATTR_MIN && detail_cfg->param_list[0] < BATTLE_ATTR_MAX)
			{
				m_attrs_skill_add.m_attrs[detail_cfg->param_list[0]] += detail_cfg->param_list[1];
				base_add.m_attrs[detail_cfg->param_list[0]] += detail_cfg->param_list[1];
			}

			if (SPE_ATTR_ADD_ATTR_BY_LEVEL == detail_cfg->effect_sub_type && detail_cfg->param_num >= 2
				&& detail_cfg->param_list[0] >= BATTLE_ATTR_MIN && detail_cfg->param_list[0] < BATTLE_ATTR_MAX)
			{
				BattleAttr add_attr = static_cast<BattleAttr>(ceil(m_module_mgr->GetRole()->GetLevel() * (double)detail_cfg->param_list[1] / BATTLE_PERCENT_NUM));
				m_attrs_skill_add.m_attrs[detail_cfg->param_list[0]] += add_attr;
				base_add.m_attrs[detail_cfg->param_list[0]] += add_attr;
			}

			if (SPE_ATTR_ADD_ATTR_BY_ANOTHER_ATTR == detail_cfg->effect_sub_type && detail_cfg->param_num >= 3
				&& detail_cfg->param_list[0] >= BATTLE_ATTR_MIN && detail_cfg->param_list[0] < BATTLE_ATTR_MAX
				&& detail_cfg->param_list[1] >= BATTLE_ATTR_MIN && detail_cfg->param_list[1] < BATTLE_ATTR_MAX
				&& detail_cfg->param_list[2] > 0)
			{
				int attr_type0 = detail_cfg->param_list[0];
				int attr_type1 = detail_cfg->param_list[1];
				m_attrs_skill_add.m_attrs[attr_type0] += static_cast<int>(1.0 * base_add.m_attrs[attr_type1] / (1.0 * detail_cfg->param_list[2] / BATTLE_PERCENT_NUM));
				base_add.m_attrs[attr_type0] += static_cast<int>(1.0 * base_add.m_attrs[attr_type1] / (1.0 * detail_cfg->param_list[2] / BATTLE_PERCENT_NUM));
			}
		}
	}
	return m_attrs_skill_add;
}

void HuaShenManager::AddAttributePointReq(short vitality, short strength, short power, short speed, short magic, int cur_plan_index)
{
	int total_change_point = vitality + strength + power + speed + magic;
	if (total_change_point > m_param.attr_plan_list[cur_plan_index].remain_point)
	{
		return; // 点数不够
	}

	AttributePlan& plan = m_param.attr_plan_list[cur_plan_index];
	short* point_list = m_param.attr_plan_list[cur_plan_index].add_point_list;

	int total_vit = point_list[ADD_POINT_TYPE_VITALITY] + vitality;
	int total_str = point_list[ADD_POINT_TYPE_STRENGTH] + strength;
	int total_pow = point_list[ADD_POINT_TYPE_POWER] + power;
	int total_agi = point_list[ADD_POINT_TYPE_AGILITY] + speed;
	int total_mag = point_list[ADD_POINT_TYPE_MAGIC] + magic;

	int total_point = plan.remain_point +
		point_list[ADD_POINT_TYPE_VITALITY] +
		point_list[ADD_POINT_TYPE_STRENGTH] +
		point_list[ADD_POINT_TYPE_POWER] +
		point_list[ADD_POINT_TYPE_AGILITY] +
		point_list[ADD_POINT_TYPE_MAGIC];

	if (total_vit * 2 > total_point ||
		total_str * 2 > total_point ||
		total_pow * 2 > total_point ||
		total_agi * 2 > total_point ||
		total_mag * 2 > total_point)
	{
		m_module_mgr->NoticeNum(errornum::EN_ROLE_ATTR_POINT_LIMIT);
		return;
	}

	plan.add_point_list[ADD_POINT_TYPE_VITALITY] += vitality;
	plan.add_point_list[ADD_POINT_TYPE_STRENGTH] += strength;
	plan.add_point_list[ADD_POINT_TYPE_POWER] += power;
	plan.add_point_list[ADD_POINT_TYPE_AGILITY] += speed;
	plan.add_point_list[ADD_POINT_TYPE_MAGIC] += magic;

	m_param.attr_plan_list[cur_plan_index].remain_point -= total_change_point;

	this->ReCalcAttr(AttributeList::RECALC_REASON_ATTRIBUTE_PLAN, __FUNCTION__);
	this->SendPlanInfo(cur_plan_index);
}

void HuaShenManager::ClearAttributePointReq(int cur_plan_index)
{
	int total_change_point =
		m_param.attr_plan_list[cur_plan_index].add_point_list[ADD_POINT_TYPE_VITALITY] +
		m_param.attr_plan_list[cur_plan_index].add_point_list[ADD_POINT_TYPE_STRENGTH] +
		m_param.attr_plan_list[cur_plan_index].add_point_list[ADD_POINT_TYPE_POWER] +
		m_param.attr_plan_list[cur_plan_index].add_point_list[ADD_POINT_TYPE_AGILITY] +
		m_param.attr_plan_list[cur_plan_index].add_point_list[ADD_POINT_TYPE_MAGIC];

	m_param.attr_plan_list[cur_plan_index].add_point_list[ADD_POINT_TYPE_VITALITY] = 0;
	m_param.attr_plan_list[cur_plan_index].add_point_list[ADD_POINT_TYPE_STRENGTH] = 0;
	m_param.attr_plan_list[cur_plan_index].add_point_list[ADD_POINT_TYPE_POWER] = 0;
	m_param.attr_plan_list[cur_plan_index].add_point_list[ADD_POINT_TYPE_AGILITY] = 0;
	m_param.attr_plan_list[cur_plan_index].add_point_list[ADD_POINT_TYPE_MAGIC] = 0;

	m_param.attr_plan_list[cur_plan_index].remain_point += total_change_point;

	if (cur_plan_index == m_param.cur_plan_index)
	{	
		this->ReCalcAttr(AttributeList::RECALC_REASON_ATTRIBUTE_PLAN, __FUNCTION__);
	}

	this->SendPlanInfo(cur_plan_index);
}

void HuaShenManager::SwitchAttrPlan(int plan_index)
{
	if (m_param.cur_plan_index != plan_index)
	{
		m_param.cur_plan_index = plan_index;
		this->ReCalcAttr(AttributeList::RECALC_REASON_ATTRIBUTE_PLAN, __FUNCTION__);
	}

	this->SendPlanInfo(plan_index);
}

void HuaShenManager::AttrPlanChangeName(int plan_index, const char * change_name)
{
	if(plan_index < 0 || plan_index >= MAX_HUASHEN_ATTR_PLAN_NUM) return;
	
	F_STRNCPY(m_param.attr_plan_list[plan_index].plan_name, change_name, sizeof(m_param.attr_plan_list[plan_index].plan_name));

	this->SendPlanInfo(plan_index);
}

const AttributeList & HuaShenManager::ReCalcMedalAttr(AttributeList & base_add, bool is_recalc)
{
	if (is_recalc)
	{
		m_attrs_medal_add.Reset();

		for (int i = 0; i < ARRAY_LENGTH(m_medal_grid_list); ++i)
		{
			const ItemGridData & curr_medal = m_medal_grid_list[i];
			if (curr_medal.Invalid())
			{
				continue;
			}
			const MedalItemParam & medal_item_param = curr_medal.param->medal_param;
			const MedalUpgradeCfg * cfg = LOGIC_CONFIG->GetMedalConfig()->GetMedalUpgradeCfg(medal_item_param.grade);
			if (cfg == NULL)
			{
				continue;
			}
			m_attrs_medal_add.Add(cfg->attrs);
		}
	}

	base_add.Add(m_attrs_medal_add.m_attrs);
	return m_attrs_medal_add;
}

void HuaShenManager::MedalPutOn(int grid_index, int index)
{
	if(index < 0 || index >= HUASHEN_ITEM_GRID_NUM_MAX || grid_index < 0 || grid_index >= MAX_MEDAL_WEAR_GRID) return;

	if(NULL == m_grid_list[index] || m_grid_list[index]->Invalid() || NULL == m_grid_list[index]->param) return;
	const ItemBase * item = ITEMPOOL->GetItem(m_grid_list[index]->item_id);
	if (item == NULL || item->GetItemType() != I_TYPE_MEDAL)
	{
		return;
	}
	//化身穿戴使用本尊等级
	if (item->GetLimitLevel() > m_module_mgr->GetRole()->GetLevel())
	{
		m_module_mgr->NoticeNum(errornum::EN_EQUIPMENT_PUT_LEVEL_LIMIT);
		return;
	}

	if (!m_medal_grid_list[grid_index].Invalid())
	{
		if (!this->MedalTakeOff(grid_index, false))
		{
			return;
		}
	}

	ItemDataWrapper item_data;
	m_grid_list[index]->WrapTo(&item_data);
	if (!this->Consume(index, true))
	{
		return;
	}
	
	m_medal_grid_list[grid_index].Set(item_data);
	if (!item_data.is_bind)
	{
		m_medal_grid_list[grid_index].is_bind = true;
		m_module_mgr->GetKnapsack()->SynItemBindInfo(ITEM_COLUMN_TYPE_ITEM, m_medal_grid_list[grid_index].item_unique_id, true);
	}

	this->PutOn(grid_index, m_medal_grid_list[grid_index].item_unique_id, HUASHEN_GRID_INDEX_TYPE_MEDAL);
	this->ReCalcAttr(AttributeList::RECALC_REASON_MEDAL, __FUNCTION__);
	this->SendMedalInfo();
}

bool HuaShenManager::MedalTakeOff(int grid_index, bool is_notice)
{
	if(grid_index < 0 || grid_index >= MAX_MEDAL_WEAR_GRID) return false;

	ItemGridData & grid_data = m_medal_grid_list[grid_index];

	if(grid_data.Invalid() || NULL == grid_data.param) return false;

	const ItemBase *itembase = ITEMPOOL->GetItem(grid_data.item_id);
	if (NULL == itembase) return false;

	ItemDataWrapper item_data;
	grid_data.WrapTo(&item_data);

	int out_index = 0;
	this->TakeOff(item_data.item_unique_id);
	if (!this->Put(item_data, true, &out_index))
	{
		return false;
	}

	if (!is_notice)
	{
		Protocol::SCHuaShenMedalTakeOffInfo info;
		info.out_index = out_index;

		m_module_mgr->NetSend(&info, sizeof(info));
	}

	grid_data.Clear();
	if (is_notice)
	{
		this->ReCalcAttr(AttributeList::RECALC_REASON_MEDAL, __FUNCTION__);
		this->SendMedalInfo();
	}
	return true;
}

bool HuaShenManager::MedalUpdate(int grid_index, ItemDataWrapper * item_data)
{
	if (grid_index < 0 || grid_index >= MAX_MEDAL_WEAR_GRID || NULL == item_data || 0 == item_data->has_param) return false;

	//防止出现BUG导致服务器挂了
	if (NULL == m_medal_grid_list[grid_index].param)
	{
		m_medal_grid_list[grid_index].param = ItemBase::CreateParam();
	}
	m_medal_grid_list[grid_index].param->SetData(item_data->param_data);
	m_medal_grid_list[grid_index].is_bind = (0 != item_data->is_bind);
	return true;
}

bool HuaShenManager::MedalReset(int grid_index, bool is_notice)
{
	if(grid_index < 0 || grid_index >= MAX_MEDAL_WEAR_GRID) return false;

	m_medal_grid_list[grid_index].Clear();
	if (is_notice)
	{
		this->SendMedalInfo();
	}
	return true;
}

void HuaShenManager::GetChangeMedalItemList(int * count, ItemListParam::ItemListData * save_list)
{
	int tmp_count = 0;
	
	for (int i = 0; i < MAX_MEDAL_WEAR_GRID && tmp_count < *count; ++i)
	{
		const ItemGridData & grid_data = m_medal_grid_list[i];
		if (grid_data.Invalid())
		{
			if (!m_medal_old_state[i]) continue;

			save_list[tmp_count].change_state_item = structcommon::CS_DELETE;
		}
		else
		{

			save_list[tmp_count].change_state_item = m_medal_old_state[i] ? structcommon::CS_UPDATE : structcommon::CS_INSERT;

		}

		save_list[tmp_count].index = i + ItemNamespace::HUASHEN_MEDAL_INDEX_BEGIN;
		ItemDataWrapper & item_data_wrapper = save_list[tmp_count].item_wrapper;
		grid_data.WrapTo(&item_data_wrapper);

		++tmp_count;
	}

	*count = tmp_count;
}

int HuaShenManager::GetMedalSpeScore()
{
	int capability = 0;
	std::set<MedalSpPriority> m_sp_id_set;
	this->GetEffectSp(&m_sp_id_set);
	if (!m_sp_id_set.empty())
	{
		const MedalSpeScoreCfg * spe_score_cfg = LOGIC_CONFIG->GetScoreSystemConfig()->GetMedalSpeScroeCfg(m_module_mgr->GetRole()->GetLevel());
		if (NULL == spe_score_cfg) return capability;

		int prof_type = m_module_mgr->GetRole()->GetAppearanceProf();
		for (std::set<MedalSpPriority>::iterator it = m_sp_id_set.begin(); it != m_sp_id_set.end(); it++)
		{
			if ((it->prof_type != 0 && prof_type != it->prof_type) || it->sp_priority < 0 || it->sp_priority >= MAX_MEDAL_MAX_GRADE) continue;

			capability += spe_score_cfg->score_priority[it->sp_priority];
		}
	}

	return capability;
}

void HuaShenManager::GetMedalBattleData(RoleBattleData * role_data)
{
	if (role_data == NULL)
	{
		return;
	}
	std::set<int> sp_id = this->GetEffectSp();

	for (std::set<int>::iterator iter = sp_id.begin(); iter != sp_id.end() && role_data->other_sp_num < ARRAY_LENGTH(role_data->other_sp_list); ++iter)
	{
		const SpecialEffectCfg * spe_cfg = BattleSpecialEffectPool::GetInstance().GetSpecialEffectCfg(*iter);
		if (NULL == spe_cfg)
		{
			continue;
		}
		if (spe_cfg->prof_type != 0 && m_module_mgr->GetRole()->GetAppearanceProf() != spe_cfg->prof_type)
		{
			continue;
		}
		if (role_data->other_sp_num < ARRAY_LENGTH(role_data->other_sp_list))
		{
			role_data->other_sp_list[role_data->other_sp_num++] = *iter;
		}
		else
		{
			gamelog::g_log_battle.printf(LL_INFO, "%s out_other_sp_num >= max_num[%d] ", __FUNCTION__, ARRAY_LENGTH(role_data->other_sp_list));
		}
	}
}

std::set<int> HuaShenManager::GetEffectSp(std::set<MedalSpPriority>* sp_list)
{
	std::set<int> ret;
	std::set<MedalSpPriority> priority;


	for (int wear_index = 0; wear_index < ARRAY_LENGTH(m_medal_grid_list); ++wear_index)
	{
		const ItemGridData & curr_medal = m_medal_grid_list[wear_index];
		if (curr_medal.Invalid())
		{
			continue;
		}
		const MedalItemParam & medal_item_param = curr_medal.param->medal_param;

		{
			const SpecialEffectCfg * spe_cfg = BattleSpecialEffectPool::GetInstance().GetSpecialEffectCfg(medal_item_param.sp_id);
			if (NULL == spe_cfg)
			{
				continue;
			}

			MedalSpPriority  tmp;
			tmp.sp_id = medal_item_param.sp_id;
			tmp.sp_type = spe_cfg->sp_type;
			tmp.sp_priority = spe_cfg->sp_priority;
			tmp.prof_type = spe_cfg->prof_type;
			priority.insert(tmp);
		}

		{
			const SpecialEffectCfg * spe_cfg_2 = BattleSpecialEffectPool::GetInstance().GetSpecialEffectCfg(medal_item_param.second_sp_id);
			if (NULL == spe_cfg_2)
			{
				continue;
			}

			MedalSpPriority  tmp2;
			tmp2.sp_id = medal_item_param.second_sp_id;
			tmp2.sp_type = spe_cfg_2->sp_type;
			tmp2.sp_priority = spe_cfg_2->sp_priority;
			tmp2.prof_type = spe_cfg_2->prof_type;
			priority.insert(tmp2);
		}
	}

	int last_sp_type = -1;
	for (std::set<MedalSpPriority>::iterator iter = priority.begin(); iter != priority.end(); ++iter)
	{
		if (last_sp_type == iter->sp_type)
		{
			continue;
		}

		last_sp_type = iter->sp_type;
		ret.insert(iter->sp_id);
		if (NULL != sp_list)
		{
			sp_list->insert(*iter);
		}
	}

	return ret;
}

void HuaShenManager::SynBenZunMedalGrid(const ItemDataWrapper& item, int medal_index)
{	
	if(medal_index < 0 || medal_index >= MAX_MEDAL_WEAR_GRID) return;

	m_medal_grid_list[medal_index].Set(item);
	this->PutOn(medal_index, m_medal_grid_list[medal_index].item_unique_id, HUASHEN_GRID_INDEX_TYPE_MEDAL);
}

void HuaShenManager::ClearMedalDirtyMark()
{
	for (int i = 0; i < ARRAY_ITEM_COUNT(m_medal_old_state) 
		&& i < ARRAY_ITEM_COUNT(m_medal_grid_list); ++i)
	{
		m_medal_old_state[i] = !m_medal_grid_list[i].Invalid();
	}
}

void HuaShenManager::SendMedalInfo()
{
	Protocol::SCHuaShenMedalInfo info;
	info.count = 0;

	for (int i = 0; info.count < ARRAY_LENGTH(info.grid_info) && i < ARRAY_LENGTH(m_medal_grid_list); ++i)
	{
		Protocol::SCHuaShenMedalInfo::GridItem & protocol_item = info.grid_info[info.count];
		const ItemGridData & item_grid = m_medal_grid_list[i];
		if (item_grid.Invalid())
		{
			continue;
		}

		protocol_item.grid_index = i;
		protocol_item.item_id = item_grid.item_id;
		protocol_item.is_bind = (char)item_grid.is_bind;
		protocol_item.num = item_grid.num;
		protocol_item.has_param = false;
		protocol_item.invalid_time = item_grid.invalid_time;

		const ItemBase * item_base = ITEMPOOL->GetItem(item_grid.item_id);
		if (NULL != item_grid.param && NULL != item_base) // 只发送有参数的
		{
			protocol_item.has_param = true;
			protocol_item.param_length = item_grid.param->GetDataByItemType(item_base->GetItemType(), protocol_item.param_data);
		}

		info.count += 1;
	}

	EngineAdapter::Instance().NetSendSerializeMsg(m_module_mgr->GetRole()->GetNetId(), &info);
}

void HuaShenManager::OnSetBattlePosition(int position)
{
	m_param.battle_position = position;
	
	this->SendBattlePosition();
}

void HuaShenManager::SendBattlePosition()
{	
	static Protocol::SCSetHuaShenBattlePosition info;
	info.position = m_param.battle_position;
	
	EngineAdapter::Instance().NetSend(m_module_mgr->GetRole()->GetNetId(), &info, sizeof(info));
}

void HuaShenManager::SendRoleInfo()
{
	static Protocol::SCHuaShenRoleInfoAck info;

	info.role_level = m_param.max_level;
	info.appearance = m_module_mgr->GetAppearance();
	m_module_mgr->GetEquipmentManager()->GetCurWeapon(&info.appearance.weapon_id, NULL, &info.appearance.sub_weapon_id, ROLE_TYPE_HUASHEN);

	EngineAdapter::Instance().NetSend(m_module_mgr->GetRole()->GetNetId(), &info, sizeof(info));
}

void HuaShenManager::SendRoleAttrList()
{
	static Protocol::SCHuaShenRoleAttrList info;
	info.attr_count = 0;
	info.notify_reason = 0;
	info.capability = m_module_mgr->GetCapability()->GetRoleHuaShenCapability();
	static Protocol::SCHuaShenRoleAttrList::AttrPair single_info;
	for (int i = BATTLE_ATTR_NORMAL_VALUE_BEGIN; i < BATTLE_ATTR_NORMAL_VALUE_END && info.attr_count < BATTLE_ATTR_MAX; ++i)
	{
		single_info.attr_type = i;
		single_info.attr_value = m_attr_list.m_attrs[i];
		info.attr_list[info.attr_count++] = single_info;
	}

	for (int i = BATTLE_ATTR_CORRECTED_VALUE_BEGIN; i < BATTLE_ATTR_CORRECTED_VALUE_END && info.attr_count < BATTLE_ATTR_MAX; ++i)
	{
		single_info.attr_type = i;
		single_info.attr_value = m_attr_list.m_attrs[i];
		info.attr_list[info.attr_count++] = single_info;
	}

	for (int i = BATTLE_ATTR_ANTI_VALUE_BEGIN; i < BATTLE_ATTR_ANTI_VALUE_END && info.attr_count < BATTLE_ATTR_MAX; ++i)
	{
		single_info.attr_type = i;
		single_info.attr_value = m_attr_list.m_attrs[i];
		info.attr_list[info.attr_count++] = single_info;
	}

	for (int i = BATTLE_ATTR_ELEMENT_BEGIN; i < BATTLE_ATTR_ELEMENT_END && info.attr_count < BATTLE_ATTR_MAX; ++i)
	{
		single_info.attr_type = i;
		single_info.attr_value = m_attr_list.m_attrs[i];
		info.attr_list[info.attr_count++] = single_info;
	}

	int send_len = sizeof(info) - (BATTLE_ATTR_MAX - info.attr_count) * sizeof(Protocol::SCHuaShenRoleAttrList::AttrPair);
	EngineAdapter::Instance().NetSend(m_module_mgr->GetRole()->GetNetId(), (const char*)&info, send_len);
}

void HuaShenManager::SendSeasonInfo()
{
	Protocol::SCHuaShenSeasonInfo info;
	info.season_first_enter = m_param.season_reset;
	info.is_finish_guide = m_param.is_finish_guide;
	info.season_reset_first_dijing = m_param.season_reset_first_dijing;

	EngineAdapter::Instance().NetSend(m_module_mgr->GetRole()->GetNetId(), (const char*)&info, sizeof(info));
}

void HuaShenManager::CollectRoleBattleData(RoleBattleData * out_data, bool need_partner, bool need_pet)
{
	if (NULL == out_data) return;

	//自动战斗设置(此处自动战斗设置使用本尊)
	out_data->is_auto = m_module_mgr->GetCommonData().battle_is_auto;
	out_data->role_stand_row = m_param.battle_position;				//化身战斗位置设置

	out_data->auto_first_move_type = m_module_mgr->GetCommonData().battle_auto_first_move_type;
	out_data->auto_first_skill_id = m_module_mgr->GetCommonData().battle_auto_first_skill_id;
	out_data->auto_first_skill_level = m_module_mgr->GetCommonData().battle_auto_first_skill_level;
	out_data->auto_second_move_type = m_module_mgr->GetCommonData().battle_auto_second_move_type;
	out_data->auto_second_skill_id = m_module_mgr->GetCommonData().battle_auto_second_skill_id;
	out_data->auto_second_skill_level = m_module_mgr->GetCommonData().battle_auto_second_skill_level;

	bool is_use_cur_hp_mp = true;

	// 获取人物数据
	this->GetRoleBattleData(&out_data->role_character_data, is_use_cur_hp_mp);

	// 获取宠物数据
	out_data->pet_num = 0;
	out_data->fight_pet_idx = -1;
	if (need_pet)
	{
		this->GetPetBattleData(ROLE_BATTLE_PET_NUM, &out_data->pet_num,
			out_data->pet_character_data, &out_data->fight_pet_idx, is_use_cur_hp_mp);
	}

	// 获取伙伴数据
	out_data->huanshou_level = 0;
	out_data->partner_num = 0;
	if (need_partner)
	{
		m_module_mgr->GetPartnerBattleData(ARRAY_ITEM_COUNT(out_data->partner_character_data),
			&out_data->partner_num, out_data->partner_character_data, is_use_cur_hp_mp);
	}

	// 获取装备数据
	out_data->weapon_attr_num = 0;
	m_module_mgr->GetEquipmentManager()->GetHuaShenWeaponAttr(BATTLE_WEAPON_ATTR_NUM,
		&out_data->weapon_attr_num, out_data->weapon_attr_list);

	out_data->weapon_sp_num = 0;
	out_data->other_sp_num = 0;
	m_module_mgr->GetEquipmentManager()->GetHuaShenEquipSpecialEffect(BATTLE_WEAPON_EFFECT_NUM,
		&out_data->weapon_sp_num, out_data->weapon_sp_list, BATTLE_OTHER_EQUIP_EFFECT_NUM,
		&out_data->other_sp_num, out_data->other_sp_list);
	
	// 获取灵骑特效数据
	m_module_mgr->GetSmartMountsSystem()->GetUserSpecialEffectList(BATTLE_OTHER_EQUIP_EFFECT_NUM, &out_data->other_sp_num, out_data->other_sp_list);
	//获取勋章数据		
	this->GetMedalBattleData(out_data);
	// 获得家族灵脉数据
	m_module_mgr->GetRoleGuildInborn()->GetPassiveSkillByType(GUILD_INBORN_ADD_TARGET_TYPE_ROLE, BATTLE_OTHER_EQUIP_EFFECT_NUM, &out_data->other_sp_num, out_data->other_sp_list);
	//心法主动技能
	out_data->heart_skill_is_vaild = 1;
	out_data->heart_skill_count = 0;
	m_module_mgr->GetRoleHeartSkill()->GetEquipInitativeSkill(ARRAY_ITEM_COUNT(out_data->heart_skill_list), &out_data->heart_skill_count, out_data->heart_skill_list);
	//心法被动技能
	m_module_mgr->GetRoleHeartSkill()->GetEquipPassiveSkill(BATTLE_OTHER_EQUIP_EFFECT_NUM, &out_data->other_sp_num, out_data->other_sp_list);

	// 获取法宝数据
	out_data->fabao_num = 0;
	m_module_mgr->GetFaBao()->GetFaBaoList(&out_data->fabao_num, out_data->fabao_list);

#ifdef _DEBUG
	assert(out_data->fabao_num <= BATTLE_FABAO_NUM_MAX);
#endif

	//主角外观
	out_data->appearance = m_module_mgr->GetAppearance();
	m_module_mgr->GetEquipmentManager()->GetCurWeapon(&out_data->appearance.weapon_id, NULL, &out_data->appearance.sub_weapon_id, ROLE_TYPE_HUASHEN);
	out_data->top_level = m_module_mgr->GetRole()->GetTopLevel();
	m_module_mgr->GetRoleNewLifeSkill()->GetChangeCardSkill(out_data->change_card_skill_id, out_data->change_card_seq);

	//幻兽
	out_data->huanshou_skill_info.Reset();
	m_module_mgr->GetRoleHuanShou()->OnGetFuShenSkill(&out_data->huanshou_skill_info, out_data->other_sp_list, BATTLE_OTHER_EQUIP_EFFECT_NUM, &out_data->other_sp_num);
}

bool HuaShenManager::GetRoleBattleData(BattleCharacterData * out_data, bool is_use_cur_hp_mp)
{
	if (NULL == out_data)
	{
		return false;
	}
	out_data->Reset();

	out_data->character_id = m_module_mgr->GetRole()->GetUID();
	out_data->character_type = BATTLE_CHARACTER_TYPE_ROLE;
	out_data->level = m_param.max_level;
	F_STRNCPY(out_data->name, m_module_mgr->GetRole()->GetName(), sizeof(out_data->name));

	out_data->profession = m_module_mgr->GetRole()->GetProfession();
	out_data->race = RACE_TYPE_HUMAN;

	out_data->is_use_cur_hp_mp = (is_use_cur_hp_mp ? 1 : 0);

	ItemID weapon_id = 0;
	short weapon_type = WEAPON_TYPE_NONE;
	m_module_mgr->GetEquipmentManager()->GetCurWeapon(&weapon_id, &weapon_type, NULL, ROLE_TYPE_HUASHEN);
	out_data->weapon_type = static_cast<char>(weapon_type);
	out_data->weapon_id = weapon_id;

	memcpy(out_data->attr_list, m_attr_list.m_attrs, sizeof(out_data->attr_list));

	ItemID element_crystal_id = m_module_mgr->GetEquipmentManager()->GetHuaShenElementCrystalItemId();
	int crstal_remain_times = m_module_mgr->GetEquipmentManager()->GetHuaShenElementCrystalRemainTimes();
	if (element_crystal_id > 0 && crstal_remain_times > 0)
	{
		const ItemBase* element_item = ITEMPOOL->GetItem(element_crystal_id);
		if (NULL != element_item)
		{
			const ElementCrystal* element_crystal = (const ElementCrystal*)element_item;
			out_data->attr_list[BATTLE_ATTR_ELEMENT_FIRE] = element_crystal->GetElementFire();
			out_data->attr_list[BATTLE_ATTR_ELEMENT_WATER] = element_crystal->GetElementWater();
			out_data->attr_list[BATTLE_ATTR_ELEMENT_WIND] = element_crystal->GetElementWind();
			out_data->attr_list[BATTLE_ATTR_ELEMENT_EARTH] = element_crystal->GetElementEarth();
		}
		else
		{
			out_data->attr_list[BATTLE_ATTR_ELEMENT_FIRE] = 0;
			out_data->attr_list[BATTLE_ATTR_ELEMENT_WATER] = 0;
			out_data->attr_list[BATTLE_ATTR_ELEMENT_WIND] = 0;
			out_data->attr_list[BATTLE_ATTR_ELEMENT_EARTH] = 0;
		}
	}
	else
	{
		out_data->attr_list[BATTLE_ATTR_ELEMENT_FIRE] = 0;
		out_data->attr_list[BATTLE_ATTR_ELEMENT_WATER] = 0;
		out_data->attr_list[BATTLE_ATTR_ELEMENT_WIND] = 0;
		out_data->attr_list[BATTLE_ATTR_ELEMENT_EARTH] = 0;
	}

	for (int i = 0; i < BATTLE_ATTR_MAX; ++i)
	{
		if (BATTLE_ATTR_CUR_HP == i)
		{
			out_data->attr_list[i] += m_module_mgr->GetCommonData().gm_attr_list[BATTLE_ATTR_MAX_HP];
		}
		else if (BATTLE_ATTR_CUR_MP == i)
		{
			out_data->attr_list[i] += m_module_mgr->GetCommonData().gm_attr_list[BATTLE_ATTR_MAX_MP];
		}
		else
		{
			out_data->attr_list[i] += m_module_mgr->GetCommonData().gm_attr_list[i];
		}

		if (i >= BATTLE_ATTR_ELEMENT_BEGIN && i < BATTLE_ATTR_ELEMENT_END)
		{
			out_data->attr_list[i] = GetMax(out_data->attr_list[i], 0);
			out_data->attr_list[i] = GetMin(out_data->attr_list[i], BATTLE_ELEMENT_MAX);
		}
	}

	for (int skill_idx = 0; skill_idx < MAX_SKILL_NUM && out_data->skill_num < MAX_BATTLE_SKILL_NUM; ++skill_idx)
	{
		RoleSkill::SkillItem* skill = this->GetSkill(skill_idx);
		if (NULL == skill || skill->Invalid()) continue;
		if (skill->skill_show_level <= 0) continue;

		out_data->skill_list[out_data->skill_num].skill_id = skill->skill_id;
		out_data->skill_list[out_data->skill_num].skill_level = skill->skill_show_level;
		++out_data->skill_num;
	}
	
	out_data->capability = m_module_mgr->GetCapability()->GetRoleHuaShenCapability();

	return true;
}

bool HuaShenManager::GetPetBattleData(short max_pet_num, short * out_pet_num, PetBattleData * out_data_list, short * out_fight_pet_idx, bool is_use_cur_hp_mp)
{
	if (NULL == out_pet_num || NULL == out_data_list || NULL == out_fight_pet_idx) return false;

	*out_pet_num = 0;
	if (*out_pet_num < max_pet_num)
	{
		if (this->GetBattleCharacterData(m_param.new_pet_status_index[NEW_FIGHT_INDEX], &out_data_list[*out_pet_num], is_use_cur_hp_mp))
		{
			*out_fight_pet_idx = *out_pet_num;
			*out_pet_num += 1;
		}
	}

	if (*out_pet_num < max_pet_num)
	{
		if (this->GetBattleCharacterData(m_param.new_pet_status_index[NEW_STANDBY_INDEX1], &out_data_list[*out_pet_num], is_use_cur_hp_mp))
		{
			*out_pet_num += 1;
		}
	}

	if (*out_pet_num < max_pet_num)
	{
		if (this->GetBattleCharacterData(m_param.new_pet_status_index[NEW_STANDBY_INDEX2], &out_data_list[*out_pet_num], is_use_cur_hp_mp))
		{
			*out_pet_num += 1;
		}
	}

	if (*out_pet_num < max_pet_num)
	{
		if (this->GetBattleCharacterData(m_param.new_pet_status_index[NEW_STANDBY_INDEX3], &out_data_list[*out_pet_num], is_use_cur_hp_mp))
		{
			*out_pet_num += 1;
		}
	}

	if (*out_pet_num < max_pet_num)
	{
		if (this->GetBattleCharacterData(m_param.new_pet_status_index[NEW_STANDBY_INDEX4], &out_data_list[*out_pet_num], is_use_cur_hp_mp))
		{
			*out_pet_num += 1;
		}
	}

	if (*out_pet_num < max_pet_num)
	{
		if (this->GetBattleCharacterData(m_param.new_pet_status_index[NEW_STANDBY_INDEX5], &out_data_list[*out_pet_num], is_use_cur_hp_mp))
		{
			*out_pet_num += 1;
		}
	}

	return true;
}

void HuaShenManager::GetPersonRankInfo(RoleBaseInfo & _role_info)
{
	_role_info.uid = m_module_mgr->GetUid();
	_role_info.exp = m_module_mgr->GetRole()->GetExp();
	_role_info.role_level = m_param.max_level;
	m_module_mgr->GetRole()->GetName(_role_info.user_name);
	_role_info.prof = m_module_mgr->GetRole()->GetProfession();
	_role_info.avatar_type = m_module_mgr->GetRole()->GetAvatarType();
	_role_info.capability = m_module_mgr->GetCapability()->GetRoleHuaShenCapability();
	_role_info.headshot_id = m_module_mgr->GetRole()->GetHeadshotID();
	_role_info.top_level = m_module_mgr->GetRole()->GetTopLevel();
	_role_info.role_fly_flag = m_module_mgr->GetFlyUpSystem()->GetFlyUpFlag();
}

void HuaShenManager::GetPetRankInfo( int _pet_index , RoleBaseInfo & _role_info, PetPro & _pet_param, AttributeList & _pet_attr)
{
	if (_pet_index < 0 || _pet_index >= MAX_HUASHEN_PET_GRID_NUM)
	{
		return;
	}

	_role_info.uid = m_module_mgr->GetUid();
	_role_info.exp = m_module_mgr->GetRole()->GetExp();
	_role_info.role_level = m_param.max_level;
	m_module_mgr->GetRole()->GetName(_role_info.user_name);
	_role_info.prof = m_module_mgr->GetRole()->GetProfession();
	_role_info.avatar_type = m_module_mgr->GetRole()->GetAvatarType();
	_role_info.capability = m_module_mgr->GetCapability()->GetRoleHuaShenCapability();
	_role_info.headshot_id = m_module_mgr->GetRole()->GetHeadshotID();
	_role_info.top_level = m_module_mgr->GetRole()->GetTopLevel();
	_role_info.role_fly_flag = m_module_mgr->GetFlyUpSystem()->GetFlyUpFlag();

	_pet_param.index = _pet_index;
	_pet_param.pet_param = m_pet_grid_list[_pet_index].param->pet_param;
	_pet_param.pet_unique_id = m_pet_grid_list[_pet_index].item_unique_id;

	this->GetPetAttributeList(_pet_index, _pet_attr);
}

void HuaShenManager::ClearGrid(int index)
{
	// column 和 index的合法性由外部保证

	if (NULL == m_grid_list[index]) return;

	m_grid_list[index]->Clear();
	delete m_grid_list[index];
	m_grid_list[index] = NULL;
}

void HuaShenManager::RefreshValidGridIndexEnd(int change_index)
{
	if (change_index < 0 || change_index >= HUASHEN_ITEM_GRID_NUM_MAX) return;

	if (NULL == m_grid_list[change_index] || m_grid_list[change_index]->Invalid())
	{
		if (m_valid_grid_index_end < change_index)
		{
			m_valid_grid_index_end = change_index;
		}
	}
	else
	{
		if (m_valid_grid_index_end < change_index + 1)
		{
			m_valid_grid_index_end = change_index + 1;
		}
	}

	for (int i = m_valid_grid_index_end - 1; i >= 0; i--) // 往前搜索直到最后的非空格子
	{
		if (NULL != m_grid_list[i] && !m_grid_list[i]->Invalid())
		{
			m_valid_grid_index_end = i + 1;
			return;
		}
	}

	m_valid_grid_index_end = 0;
}

void HuaShenManager::OnDeletePetAddPetEquipDelayHandle(int pet_index)
{
	if (this->PetInvalid(pet_index)) return;
	PetParam& pet_param = m_pet_grid_list[pet_index].param->pet_param;
	if (pet_param.pet_equipment_id == 0)
	{
		return;
	}
	const ItemBase * tmp_itembase = ITEMPOOL->GetItem(pet_param.pet_equipment_id);
	if (tmp_itembase == NULL || tmp_itembase->GetItemType() != I_TYPE_PET_EQUIPMENT)
	{
		return;
	}
	static NetValueItemParam p_nvip; p_nvip.Reset();
	pet_param.GetPetEquip(&p_nvip.pet_equipment_param);

	ItemDataWrapper pet_equip; pet_equip.Reset();
	pet_equip.item_id = pet_param.pet_equipment_id;
	pet_equip.num = 1;
	pet_equip.has_param = 1;
	pet_equip.is_bind = pet_param.pet_equipment_is_bind;
	pet_equip.item_unique_id = pet_param.equipment_item_unique_id;
	p_nvip.GetData(pet_equip.param_data);
	pet_param.ClearPetEquipment();
	m_delay_handle_map[pet_equip.item_unique_id] = pet_equip;
}

void HuaShenManager::CheckAllItemIdentical()
{
	m_module_mgr->GetEquipmentManager()->SynHuaShenEquipAndJewelry(true);
	m_module_mgr->GetMedal()->SynHuaShenMedalGrid(true);
}

void HuaShenManager::SeasonReset()
{
	memset(m_role_attr_reason_list, 0, sizeof(m_role_attr_reason_list));
	memset(m_pet_attr_reason_list, 0, sizeof(m_pet_attr_reason_list));
	m_attr_list.Reset();
	m_attrs_plan_add.Reset();
	m_attrs_medal_add.Reset();
	//	m_attrs_fabao_add.Reset();
	m_attrs_train_add.Reset();
	m_attrs_train_per.Reset();
	m_base_attr_prof.Reset();
	m_attrs_skill_add.Reset();

	for (int i = 0; i < MAX_SKILL_NUM; i++)
	{
		m_skill_list[i].Reset();
	}

	max_role_train_level = 0;
	max_pet_train_level = 0;
	max_advance_times = 0;
	role_advance_times = 0;
	m_pet_has_skin_id.clear();
	m_next_rank_update_time = 0;
	this->ClearAllItemData();
}

void HuaShenManager::ClearAllItemData()
{
	if (1 != m_param.is_reset_grid_flag)
	{
		// 背包
		for (int i = 0; i < m_valid_grid_index_end && i < HUASHEN_ITEM_GRID_NUM_MAX; i++)
		{
			this->Consume(i, false);
		}
		// 装备/灵饰/水晶
		m_module_mgr->GetEquipmentManager()->ClearHuaShenEquip();
		// 勋章(令牌)
		for (int i = 0; i < MAX_MEDAL_WEAR_GRID; i++)
		{
			this->MedalReset(i, false);
		}
		//宠物
		for (int i = 0; i < MAX_HUASHEN_PET_GRID_NUM; i++)
		{
			this->DeletePet(i, false, true);
		}

		m_param.is_reset_grid_flag = 1;
	}
	else
	{
		for (std::map<long long, int>::iterator it = m_item_map.begin(); it != m_item_map.end(); it++)
		{
			this->ConsumeItem(it->second, false, true);
		}
	}

	m_item_map.clear();
	m_delay_handle_map.clear();

	m_pet_count = 0;
	m_knapsack_last_inorder_time = 0;
	memset(m_pet_base_attr, 0, sizeof(m_pet_base_attr));
	memset(m_pet_attr_no_skill, 0, sizeof(m_pet_attr_no_skill));

}

