#define  NOMINMAX

#include "pet.hpp"
#include "gamelog.h"
#include "gameworld/config/logicconfigmanager.hpp"
#include "petconfig.hpp"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "item/knapsack.h"
#include "servercommon/errornum.h"
#include "servercommon/noticenum.h"
#include "protocol/msgitem.h"
#include "engineadapter.h"
#include "item/itempool.h"
#include "item/expense/norexitem.hpp"
#include "servercommon/struct/battle/battle_def.hpp"
#include "servercommon/courseofgrowthdef.h"
#include "global/guild/guildmanager.hpp"
#include "other/event/eventhandler.hpp"
#include "other/fabao/fabao.hpp"
#include "other/fabao/fabaoconfig.hpp"
#include "item/specialpet/specialpet.hpp"
#include "servercommon/platformbattledef.hpp"
#include "scene/scene.h"
#include "global/platformbattle/platformbattle.h"
#include "item/itempool.h"
#include "item/itemextern.hpp"
#include <assert.h>
#include "servercommon/userprotocal/msgsystem.h"
#include "gameworld/scene/scenemanager.hpp"
#include "servercommon/string/gameworldstr.h"
#include "global/team/team.hpp"
#include "other/funcguide/funcguideconfig.h"
#include "other/courseofgrowth/courseofgrowth.hpp"
#include "skill/skillpool.hpp"
#include "servercommon/serverconfig/battleconfig/battle_special_effect_pool.hpp"
#include "servercommon/struct/battle/battle_special_effect_def.hpp"
#include "other/train/train.hpp"
#include "other/huashen/huashenmanager.hpp"
#include "global/rank/rankmanager.hpp"
#include "config/otherconfig/guildconfig.hpp"
#include "global/guild/guild.hpp"
#include "servercommon/uniqueidgenerator.hpp"
#include "gameworld/equipment/petequipment.h"
#include "other/workshop/petequipmentconfig.hpp"
#include "world.h"
#include "other/skin/skinconfig.hpp"
#include "chat/chatmanager.h"
#include <functional>
#include "servercommon/performancecatch.hpp"
#include "protocol/msgrole.h"
#include "other/route/mailroute.hpp"
#include "other/roleattribute/roleexpconfig.hpp"
#include "other/roleguildinborn/roleguildinborn.hpp"
#include "other/smartmountssystem/smartmountssystem.hpp"
#include "global/colosseum/colosseummanager.hpp"
#include "config/otherconfig/smartmountssystemconfig.hpp"
#include "other/train/trainconfig.hpp"
#include "other/workshop/petsoulequipmentconfig.hpp"
#include "equipment/petsoulequipment.h"
#include "other/workshop/petgodprintequipmentconfig.hpp"
#include "gameworld/engineadapter.h"
#include "other/funopen/funopen.hpp"
#include "gameworld/equipment/godprintequipment.h"
#include "task/taskmanager.h"
#include "other/collection/collection.hpp"
#include "other/roleattrpellet/roleattrpellet.hpp"
#include "config/otherconfig/roleattrpelletconfig.hpp"
#include "superskillbookconvert.hpp"
#include "other/xiuwei/xiuwei.hpp"
#include "servercommon/protobufmanager.hpp"
#include "petbadgeconfig.hpp"
#include "other/pet/petlianyaoconfig.hpp"
#include "other/dayreward/dayrewardconfig.hpp"
#include "other/dayreward/dayreward.hpp"
#include "config/otherconfig/petinbornconfig.hpp"
#include "global/marketmanager/marketmanager.hpp"
#include "other/formation/formation.hpp"
#include "other/robot/robotconfig.hpp"
#include "servercommon/serverconfig/appearanceconfig.hpp"
#include "other/shengqi/holyequipmanager.hpp"
#include "other/roleactivity/impl/roleactivityxinfuqianghua.hpp"
#include "other/roleactivity/roleactivitymanager.hpp"
#include "other/starchart/starchart.h"
#include "other/pet/petgaimingconfig.hpp"

Pet::Pet() : m_role_module_mgr(NULL), m_pet_count(0), m_pet_grid_is_change(false), m_god_print_item_bag_is_change(false)
{
	for (int i = 0; i < ItemNamespace::MAX_PET_GRID_NUM; ++i)
	{
		m_grid_list[i].Clear();
		m_old_state[i] = false;
		m_dirty_mark[i] = false;
	}

	m_pet_storage_grid_use_num = 0;
	for (int i = 0; i < ItemNamespace::MAX_PET_STORAGE_GRID_NUM; ++i)
	{
		m_storage_grid_list[i].Clear();
		m_storage_old_state[i] = false;
		m_storage_dirty_mark[i] = false;
	}
	memset(m_pet_capability_list, 0, sizeof(m_pet_capability_list));

	m_is_build_skin_id_map = false;
	m_is_update = false;

	m_pet_benediction_map.clear();
	m_pet_benediction_vice_set.clear();
	memset(m_god_print_item_old_state, 0, sizeof(m_god_print_item_old_state));
	memset(m_god_print_item_dirty_mark, 0, sizeof(m_god_print_item_dirty_mark));
	for (int i = 0; i < NEW_MAX_PET_GOD_PRONT_BAG_NUM; i++)
	{
		m_new_god_print_bag[i] = NULL;
	}

	m_pet_fei_sheng_index = -1;
}

Pet::~Pet()
{
	for (int i = 0; i < ItemNamespace::MAX_PET_GRID_NUM; ++i)
	{
		m_grid_list[i].Clear();
	}

	for (int i = 0; i < ItemNamespace::MAX_PET_STORAGE_GRID_NUM; ++i)
	{
		m_storage_grid_list[i].Clear();
	}
	for (int i = 0; i < NEW_MAX_PET_GOD_PRONT_BAG_NUM; ++i)
	{
		if (NULL != m_new_god_print_bag[i])
		{
			m_new_god_print_bag[i]->Reset();
			delete m_new_god_print_bag[i];
			m_new_god_print_bag[i] = NULL;
		}
	}
}

void Pet::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	m_god_print_param.today_free_times = 0;
	this->SendGodPrintBagInfo();
	this->ResetPetFeiShengOnDayChange();
}

void Pet::OnRoleLogin()
{
	this->BuildMapPetQualityToStrLevel();		//CheckUnlockLianYaoFurance 用到，要先调用

	this->CheckPetFeiShengTask();
	this->CheckAllPetYiWenLuReward();
	this->CheckPetBagGetReward();
	this->CheckModVer();
	this->CheckUnlockLianYaoFurance();
}

void Pet::Init(RoleModuleManager *role_module_manager, const PetCommonParam & param, const PetSkinParam & pet_skin_param,
	const PetAutoMoveParam& auto_move_param, const PetStrengthenBuffParam & pet_strengthen_buff_param , int param_list_num, const ItemListParam::ItemListData *param_data_list[])
{
	m_role_module_mgr = role_module_manager;
	m_common_param = param;
	m_pet_skin_param = pet_skin_param;
	m_strengthen_buff_param = pet_strengthen_buff_param;
	
	m_pet_count = 0;	
	m_pet_storage_grid_use_num = 0;
	for (int i = 0; i < param_list_num; ++i)
	{
		const ItemListParam::ItemListData* item_data = param_data_list[i];
		if (NULL == item_data ) continue;

		if (ItemNamespace::PET_INDEX_BEGIN <= item_data->index && item_data->index < ItemNamespace::PET_INDEX_END)
		{
			int pet_index = item_data->index - ItemNamespace::PET_INDEX_BEGIN;
			ItemGridData & grid = m_grid_list[pet_index];
			if (!grid.Invalid())
			{
				gamelog::g_log_serious_error.printf(LL_INFO, "[Pet::Init Error Duplicate][[%s %d] pet_index[%d] old_pet_id[%d] new_pet_id[%d]]",
								    m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetUID(),
								    pet_index, (int)grid.item_id, (int)item_data->item_wrapper.item_id);
				continue;
			}

			grid.item_id = item_data->item_wrapper.item_id;
			grid.num = 1;
			grid.is_bind = (0 != item_data->item_wrapper.is_bind);
			grid.param = ItemBase::CreateParamFromParamData(item_data->item_wrapper.param_data);
			grid.is_market_buy_flag = item_data->item_wrapper.is_market_buy_flag;
			grid.item_unique_id = item_data->item_wrapper.item_unique_id;

			this->CheckPetParamOffsetBeforeInit(grid.item_id, grid.item_unique_id, grid.param);

			grid.param->pet_param.CheckModifyVersion();

			if (0 == grid.item_unique_id)
			{
				long long item_unique_id = 0;
				UniqueIDGenerator::Instance()->GenUniqueID(&item_unique_id);
				if (0 != item_unique_id)
				{
					grid.item_unique_id = item_unique_id;
					this->SetItemDirty(pet_index, false);
#ifdef TRACK_ITEM_FLAG
					gamelog::g_log_item_track.printf(LL_INFO, "TRACK_ITEM_FLAG %s assign unique_id[%lld] to role[%d,%s] item[%d] num[%d] from index[%d]",
						__FUNCTION__, item_unique_id, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
						grid.item_id, grid.num, pet_index);
#endif
				}
			}

#ifdef _DEBUG
			if (BATTLE_MOVE_TYPE_PERFORM_SKILL == grid.param->pet_param.pet_auto_move_type &&
			    grid.param->pet_param.pet_auto_skill_level <= 0)
			{
				grid.param->pet_param.pet_auto_move_type = BATTLE_MOVE_TYPE_NORMAL_ATTACK;
				grid.param->pet_param.pet_auto_skill_id = 0;
				grid.param->pet_param.pet_auto_skill_level = 0;
				this->SetItemDirty(pet_index, false);
			}
#endif

			// 检查资质版本
			if (grid.param->pet_param.CheckGrowthVersion(grid.item_unique_id, "Pet::Init_1"))
			{
				this->SetItemDirty(pet_index, false);
			}

			m_old_state[pet_index] = true;
			++m_pet_count;

			gamelog::g_log_pet.printf(LL_INFO, "Pet::Init role[%d,%s]  item_unique_id:%lld item_id:%d lian_yao_id:%d lian_yao_pet_str_lv:%d",
				m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), grid.item_unique_id, grid.param->pet_param.pet_id,
				grid.param->pet_param.lian_yao_pet_id, grid.param->pet_param.lian_yao_pet_str_lv);

			unique_item_id_to_pet_index[grid.item_unique_id] = pet_index;
		}

		if (ItemNamespace::PET_STORAGE_INDEX_BEGIN <= item_data->index && item_data->index < ItemNamespace::PET_STORAG_INDEX_END)
		{
			int pet_storage_index = item_data->index - ItemNamespace::PET_STORAGE_INDEX_BEGIN;
			ItemGridData & grid = m_storage_grid_list[pet_storage_index];
			if (!grid.Invalid())
			{
				gamelog::g_log_serious_error.printf(LL_INFO, "[Pet::InitStorage Error Duplicate][[%s %d] pet_storage_index[%d] old_pet_id[%d] new_pet_id[%d]]",
								    m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetUID(),
								    pet_storage_index, (int)grid.item_id, (int)item_data->item_wrapper.item_id);
				continue;
			}

			grid.item_id = item_data->item_wrapper.item_id;
			grid.num = 1;
			grid.is_bind = (0 != item_data->item_wrapper.is_bind);
			grid.is_market_buy_flag = item_data->item_wrapper.is_market_buy_flag;
			grid.item_unique_id = item_data->item_wrapper.item_unique_id;
			grid.param = ItemBase::CreateParamFromParamData(item_data->item_wrapper.param_data);
			if (0 == grid.item_unique_id)
			{
				long long item_unique_id = 0;
				UniqueIDGenerator::Instance()->GenUniqueID(&item_unique_id);
				if (0 != item_unique_id)
				{
					grid.item_unique_id = item_unique_id;
					this->SetPetStorageItemDirty(pet_storage_index);
#ifdef TRACK_ITEM_FLAG
					gamelog::g_log_item_track.printf(LL_INFO, "TRACK_ITEM_FLAG %s storage assign unique_id[%lld] to role[%d,%s] item[%d] num[%d] from index[%d]",
						__FUNCTION__, item_unique_id, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
						grid.item_id, grid.num, pet_storage_index);
#endif
				}
			}
			
			// 检查资质版本
			if (grid.param->pet_param.CheckGrowthVersion(grid.item_unique_id, "Pet::Init_2"))
			{
				this->SetPetStorageItemDirty(pet_storage_index);
			}

			m_storage_old_state[pet_storage_index] = true;
			m_pet_storage_grid_use_num += 1;

			gamelog::g_log_pet.printf(LL_INFO, "Pet::Init role[%d,%s]  item_unique_id:%lld item_id:%d lian_yao_id:%d lian_yao_pet_str_lv:%d",
				m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), grid.item_unique_id, grid.param->pet_param.pet_id,
				grid.param->pet_param.lian_yao_pet_id, grid.param->pet_param.lian_yao_pet_str_lv);

			unique_item_id_to_pet_storage_index[grid.item_unique_id] = pet_storage_index;
		}
	}
	if (FunOpen::IsFunOpen(m_role_module_mgr->GetRole(), FUN_TYPE_PET_BENEDICTION, false))
	{
		for (int i = 0; i < ARRAY_LENGTH(m_common_param.benediction_pet_index_list); ++i)
		{
			if (-1 == m_common_param.benediction_pet_index_list[i]) continue;

			if (this->PetInvalid(m_common_param.benediction_pet_index_list[i]) || i == m_common_param.benediction_pet_index_list[i])
			{
				m_common_param.benediction_pet_index_list[i] = -1;
				continue;
			}
			long long item_unique_id = m_grid_list[m_common_param.benediction_pet_index_list[i]].item_unique_id;
			m_pet_benediction_map[item_unique_id].insert(i);
			m_pet_benediction_vice_set.insert(m_grid_list[i].item_unique_id);
		}
	}
	else
	{
		//不满足功能开启,则卸下所有副宠
		for (int i = 0; i < ARRAY_LENGTH(m_common_param.benediction_pet_index_list); ++i)
		{
			m_common_param.benediction_pet_index_list[i] = -1;
		}
	}


	//先把宠物皮肤放入容器中在进行属性计算
	this->BuildPetSkinIdMap(true);

	this->InitPetAutoMoveMap(auto_move_param);
}


void Pet::InitPetAutoMoveMap(const PetAutoMoveParam& param)
{
	for (int i = 0; i < param.pet_count && i < ARRAY_LENGTH(param.pet_list); ++i)
	{
		m_auto_move_map.insert(PetAutoMoveMap::value_type(param.pet_list[i].pet_id, param.pet_list[i]));
	}
}

void Pet::InitGodPrintBag(const GodPrintBagParam & param, const GodPrintBagWishLogParam & param2, const NewGodPrintBagWishLogParam & param3)
{
	m_god_print_param = param;
	m_god_print_log_param = param2;
	m_new_god_print_log_param = param3;

	if (m_god_print_param.tranf_flag == 0)
	{
		//旧神印背包道具转移到新背包
		for (int index = 0; index < MAX_PET_GOD_PRONT_BAG_NUM; index++)
		{
			if (m_god_print_param.bag_list[index].Invalid())continue;
			if (NULL == m_new_god_print_bag[index])m_new_god_print_bag[index] = new GodPrintItem();
			*m_new_god_print_bag[index] = m_god_print_param.bag_list[index];

			m_god_print_item_dirty_mark[index] = true;
			m_god_print_item_bag_is_change = true;
		}
		m_god_print_param.tranf_flag = 1;
	}

	if (m_new_god_print_log_param.tranf_flag == 0)
	{
		for (int i = 0; i < m_god_print_log_param.log_num && i < MAX_GOD_PRINT_WISH_LOG_NUM; i++)
		{
			NewWishLogData &new_log = m_new_god_print_log_param.m_log_list[i];
			WishLogData & old_log = m_god_print_log_param.m_log_list[i];
			new_log.wish_tamp = old_log.wish_tamp;
			new_log.item_data.lock = old_log.item_data.lock;
			new_log.item_data.color = old_log.item_data.color;
			new_log.item_data.level = old_log.item_data.level;
			new_log.item_data.exp = old_log.item_data.exp;
			new_log.item_data.m_max_exp = old_log.item_data.m_max_exp;
			new_log.item_data.m_wear_pet_idx = old_log.item_data.m_wear_pet_idx;
			new_log.item_data.union_item_id = old_log.item_data.union_item_id;
			new_log.item_data.m_posi = old_log.item_data.m_posi;
			new_log.item_data.item_id = old_log.item_data.item_id;
			new_log.item_data.main_attr = old_log.item_data.main_attr;
			new_log.item_data.suit_id = old_log.item_data.suit_id;
			new_log.item_data.attr_num = old_log.item_data.attr_num;
			for (int j = 0; j < MAX_PET_GOD_PRONT_ATTR_NUM; j++)
			{
				new_log.item_data.add_attr_list[j] = old_log.item_data.add_attr_list[j];
			}
		}
		m_new_god_print_log_param.tranf_flag = 1;
	}
}

void Pet::InitNewGodPrintBag(const GodPrintItemListParam & god_print_item_list)
{
	for (int i = 0; i < god_print_item_list.count && i < NEW_MAX_PET_GOD_PRONT_BAG_NUM; i++)
	{
		int index = god_print_item_list.item_list[i].index;
		if (index < 0 || index >= NEW_MAX_PET_GOD_PRONT_BAG_NUM)
		{
			//错误日志
			continue;
		}
		if (NULL == m_new_god_print_bag[index])m_new_god_print_bag[index] = new GodPrintItem();
		*m_new_god_print_bag[index] = god_print_item_list.item_list[i].item_data;
		m_god_print_item_old_state[index] = true;
	}

	//检查是否神印和宠物的对应下标是否正常
	for (int i = 0; i < NEW_MAX_PET_GOD_PRONT_BAG_NUM; i++)
	{
		if (NULL == m_new_god_print_bag[i])continue;
		if (m_new_god_print_bag[i]->Invalid())continue;
		GodPrintItem &bag_data = *m_new_god_print_bag[i];
		if (bag_data.m_wear_pet_idx > 0)
		{
			if (this->PetInvalid(bag_data.m_wear_pet_idx))
			{
				bag_data.m_wear_pet_idx = -1;
			}
			else
			{
				PetParam* pet_param = this->__GetPetParam(bag_data.m_wear_pet_idx);
				if (pet_param != NULL)
				{
					if (bag_data.m_posi >= 0 && bag_data.m_posi < GOD_PRINT_INDEX_TYPE_MAX)
						if (pet_param->god_print_list[bag_data.m_posi] != i + 1)
						{
							//魂器下标对不上宠物下标,重置双方连接下标
							pet_param->god_print_list[bag_data.m_posi] = 0;
							bag_data.m_wear_pet_idx = -1;
						}
				}
			}
		}
	}
}

void Pet::GetInitParam(PetCommonParam * param, PetSkinParam * pet_skin_param, PetAutoMoveParam * auto_move_param, PetStrengthenBuffParam * pet_strengthen_buff_param, GodPrintBagParam * god_print_param, GodPrintBagWishLogParam * god_print_log_param, GodPrintItemListParam * new_god_print_item_list, NewGodPrintBagWishLogParam * new_god_print_log_param)
{	
	if (param != NULL)
	{
		*param = m_common_param;
	}
	
	if (pet_skin_param != NULL)
	{
		*pet_skin_param = m_pet_skin_param;
	}	

	if (auto_move_param != NULL)
	{
		auto_move_param->pet_count = 0;
		PetAutoMoveMap::iterator it = m_auto_move_map.begin();
		for (; it != m_auto_move_map.end() && auto_move_param->pet_count < ARRAY_LENGTH(auto_move_param->pet_list); ++it)
		{
			auto_move_param->pet_list[auto_move_param->pet_count++] = it->second;
		}
	}

	if (pet_strengthen_buff_param)
	{
		*pet_strengthen_buff_param = m_strengthen_buff_param;
	}

	if (god_print_param != NULL)
	{
		*god_print_param = m_god_print_param;
	}

	if (god_print_log_param != NULL)
	{
		*god_print_log_param = m_god_print_log_param;
	}

	if (NULL != new_god_print_item_list)
	{
		for (int index = 0; index < NEW_MAX_PET_GOD_PRONT_BAG_NUM; index++)
		{
			if (NULL != m_new_god_print_bag[index] && !m_new_god_print_bag[index]->Invalid())
			{
				new_god_print_item_list->item_list[new_god_print_item_list->count].index = index;
				new_god_print_item_list->item_list[new_god_print_item_list->count].item_data = *m_new_god_print_bag[index];
				new_god_print_item_list->count++;
			}
		}
	}
	
	if (new_god_print_log_param != NULL)
	{
		*new_god_print_log_param = m_new_god_print_log_param;
	}
}

void Pet::GetRolePetInfoParam(RolePetInfoParam * role_pet_info_param)
{
	PB_RolePetInfoData* pb_data = (PB_RolePetInfoData*)ProtobufMgr::Instance()->GetProtoMessage(PROTO_ROLE_PET_INFO_DATA);
	if (NULL == pb_data)
		return;

	pb_data->mutable_pet_data_list()->Clear();

	RolePetInfoData data;
	for (int i = 0; i < ItemNamespace::MAX_PET_GRID_NUM; i++)
	{
		if (m_grid_list[i].Invalid())
			continue;

		data.Reset();

		data.item_unique_id = m_grid_list[i].item_unique_id;
		data.item_id = m_grid_list[i].item_id;

		PetParam & item_param = m_grid_list[i].param->pet_param;
		data.str_level = item_param.str_level;

		int skill_count = 0;
		for (int j = 0; 
			j < PET_PASSIVE_MAX_COUNT 
			&& skill_count < ARRAY_ITEM_COUNT(data.skill_id_list); 
			j++)
		{
			if (item_param.passive_list[j].passive_skill_id <= 0)
				continue;
			data.skill_id_list[skill_count] = item_param.passive_list[j].passive_skill_id;
			skill_count++;
		}

		//TODO:gfp 魂器2可能需要也要查看
		if (!item_param.m_soul_equip_1.Invalid())
		{
			data.soul_item_id = item_param.m_soul_equip_2.item_id;
			data.soul_level = item_param.m_soul_equip_2.m_level;
		}

		int zhufu_count = 0;
		std::map<long long, std::set<int> >::iterator benediction_it = m_pet_benediction_map.find(m_grid_list[i].item_unique_id);
		if (benediction_it != m_pet_benediction_map.end())
		{
			for (std::set<int>::iterator it = benediction_it->second.begin(); 
				it != benediction_it->second.end() 
				&& zhufu_count < ARRAY_ITEM_COUNT(data.zhufu_pet_item_id);
				++it)
			{
				if (*it < 0 || *it >= ItemNamespace::MAX_PET_GRID_NUM) continue;
				if(m_grid_list[*it].Invalid()) continue;

				data.zhufu_pet_item_id[zhufu_count] = m_grid_list[*it].item_id;
				zhufu_count++;
			}
		}

		int god_print_count = 0;
		for (int j = 0; j < 5 && j < GOD_PRINT_INDEX_TYPE_MAX; j++)
		{
			if(item_param.god_print_list[j] <= 0) continue;
			
			int god_print_index = item_param.god_print_list[j] - 1;
			if (god_print_index < 0 
				|| god_print_index >= ARRAY_ITEM_COUNT(m_new_god_print_bag) 
				|| NULL == m_new_god_print_bag[god_print_index]) continue;

			GodPrintItem &god_print_item = *m_new_god_print_bag[god_print_index];
			data.god_print_item[god_print_count].item_id = god_print_item.item_id;
			data.god_print_item[god_print_count].level = god_print_item.level;

			god_print_count++;
		}

		data.capability = m_pet_capability_list[i];

		data.GetPBData(pb_data->add_pet_data_list());
	}

	if (!pb_data->SerializeToArray(role_pet_info_param->pet_data, ARRAY_LENGTH(role_pet_info_param->pet_data)))
	{
#ifdef _DEBUG
		assert(0);
#endif
		return;
	}
}

void Pet::GetGodPrintChangeItemList(GodPrintItemListParam * new_god_print_item_list)
{
	if (!m_god_print_item_bag_is_change)
	{
		new_god_print_item_list->count = 0;
		return;
	}

	int tmp_count = 0;

	for (int i = 0; i < NEW_MAX_PET_GOD_PRONT_BAG_NUM && tmp_count < NEW_MAX_PET_GOD_PRONT_BAG_NUM; ++i)
	{
		if (m_god_print_item_dirty_mark[i])
		{
			if (NULL == m_new_god_print_bag[i] || m_new_god_print_bag[i]->Invalid())
			{
				if (!m_god_print_item_old_state[i]) continue;

				new_god_print_item_list->item_list[tmp_count].change_state_item = structcommon::CS_DELETE;

				new_god_print_item_list->item_list[tmp_count].index = i;
				new_god_print_item_list->item_list[tmp_count].item_data.Reset();
				++tmp_count;

				gamelog::g_log_pet.printf(LL_INFO, "Pet::GetGodPrintChangeItemList CS_DELETE role[%d,%s] item from index[%d]",
					m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), i);
			}
			else
			{
				if (m_god_print_item_old_state[i])
				{
					new_god_print_item_list->item_list[tmp_count].change_state_item = structcommon::CS_UPDATE;

					gamelog::g_log_pet.printf(LL_INFO, "Pet::GetGodPrintChangeItemList CS_UPDATE role[%d,%s] item_id[%d] num[%d] from index[%d]",
						m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_new_god_print_bag[i]->item_id, 1, i);
				}
				else
				{
					new_god_print_item_list->item_list[tmp_count].change_state_item = structcommon::CS_INSERT;

					gamelog::g_log_knapsack.printf(LL_INFO, "Pet::GetGodPrintChangeItemList CS_INSERT role[%d,%s] item_id[%d] num[%d] index[%d]",
						m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_new_god_print_bag[i]->item_id, 1, i);
				}

				new_god_print_item_list->item_list[tmp_count].index = i;
				new_god_print_item_list->item_list[tmp_count].item_data = *m_new_god_print_bag[i];
				++tmp_count;
			}
		}
	}

	new_god_print_item_list->count = tmp_count;
}

void Pet::ClearGodPrintBagDirtyMark()
{
	if (!m_god_print_item_bag_is_change) return;

	for (int i = 0; i < NEW_MAX_PET_GOD_PRONT_BAG_NUM; ++i)
	{
		if (m_god_print_item_dirty_mark[i])
		{
			if (NULL == m_new_god_print_bag[i] || m_new_god_print_bag[i]->Invalid())
			{
				m_god_print_item_old_state[i] = false;
			}
			else
			{
				m_god_print_item_old_state[i] = true;
			}
		}
	}

	memset(m_dirty_mark, 0, sizeof(m_dirty_mark));

	m_god_print_item_bag_is_change = false;
}

void Pet::GetChangeItemList(int *count, ItemListParam::ItemListData *save_list)
{
	if (!m_pet_grid_is_change)
	{
		*count = 0;
		return;
	}

	int tmp_count = 0;
	for (int i = 0; i < ItemNamespace::MAX_PET_GRID_NUM && tmp_count < *count; ++i)
	{
		if (!m_dirty_mark[i]) continue;

		if (this->PetInvalid(i))
		{
			if (!m_old_state[i]) continue;

			save_list[tmp_count].change_state_item = structcommon::CS_DELETE;
		}
		else
		{
			if (m_old_state[i])
			{
				save_list[tmp_count].change_state_item = structcommon::CS_UPDATE;
			}
			else
			{
				save_list[tmp_count].change_state_item = structcommon::CS_INSERT;
			}
		}

		save_list[tmp_count].index = i + ItemNamespace::PET_INDEX_BEGIN;

		ItemDataWrapper & item_data_wrapper = save_list[tmp_count].item_wrapper;
		const ItemGridData & grid_data = m_grid_list[i];
		item_data_wrapper.item_id = grid_data.item_id;
		item_data_wrapper.num = grid_data.num;
		item_data_wrapper.is_bind = (short)grid_data.is_bind;
		item_data_wrapper.invalid_time = grid_data.invalid_time;
		item_data_wrapper.has_param = (NULL != grid_data.param) ? 1 : 0;
		item_data_wrapper.gold_price = grid_data.gold_price;
		item_data_wrapper.is_market_buy_flag = grid_data.is_market_buy_flag;
		item_data_wrapper.item_unique_id = grid_data.item_unique_id;
		if (NULL != grid_data.param)
		{
			memcpy(item_data_wrapper.param_data, grid_data.param->data, sizeof(NetValueItemParamData));
		}

		++tmp_count;
	}

	for (int i = 0; i < ItemNamespace::MAX_PET_STORAGE_GRID_NUM && tmp_count < *count; ++i)
	{
		if (!m_storage_dirty_mark[i]) continue;

		if (this->PetStorageInvalid(i))
		{
			if (!m_storage_old_state[i]) continue;

			save_list[tmp_count].change_state_item = structcommon::CS_DELETE;
		}
		else
		{
			if (m_storage_old_state[i])
			{
				save_list[tmp_count].change_state_item = structcommon::CS_UPDATE;
			}
			else
			{
				save_list[tmp_count].change_state_item = structcommon::CS_INSERT;
			}
		}

		save_list[tmp_count].index = i + ItemNamespace::PET_STORAGE_INDEX_BEGIN;

		ItemDataWrapper & item_data_wrapper = save_list[tmp_count].item_wrapper;
		const ItemGridData & grid_data = m_storage_grid_list[i];
		grid_data.WrapTo(&item_data_wrapper);

		++tmp_count;
	}

	*count = tmp_count;
}

void Pet::CheckDataIsIllegal()
{
	CommonDataParam & common_data = m_role_module_mgr->GetCommonData();
	for (int status = 0; status < NEW_PET_STATUS_MAX; ++status)
	{
		short & pet_index = common_data.new_pet_status_index[status];
		if (pet_index >= 0 && this->PetInvalid(pet_index)) pet_index = -1;
	}

	// 为 出战与待机列表 去重
	std::set<int> tmp_pet_index_list;
	for (int status = NEW_FIGHT_INDEX; status <= NEW_STANDBY_INDEX5; ++status)
	{
		short & pet_index = common_data.new_pet_status_index[status];
		if ((int)tmp_pet_index_list.size() >= MAX_FIGHT_AND_STAND_BY_PET_NUM || tmp_pet_index_list.end() != tmp_pet_index_list.find(pet_index)) pet_index = -1;
		if (pet_index >= 0) tmp_pet_index_list.insert(pet_index);
	}
}

void Pet::InitEnd()
{
	this->CheckDataIsIllegal();

	//初始化结束之后再重算宠物各属性 
	//宠物祝福中,主宠属性需要依靠副宠,此时先算所有副宠,再算主宠
	std::set<int> main_benediction_pet_index_set;
	for (int i = ItemNamespace::PET_INDEX_BEGIN; i < ItemNamespace::PET_INDEX_END; i++)
	{
		int pet_index = i - ItemNamespace::PET_INDEX_BEGIN;
		ItemGridData & grid = m_grid_list[pet_index];
		if (grid.Invalid())continue;

		if (m_pet_benediction_map.end() != m_pet_benediction_map.find(grid.item_unique_id))
		{
			main_benediction_pet_index_set.insert(pet_index);
			continue;
		}

		this->ReCalcAttr(pet_index, false, true);
	}
	//计算主宠 index已经验证过了
	for (std::set<int>::iterator it = main_benediction_pet_index_set.begin(); it != main_benediction_pet_index_set.end(); it++)
	{
		this->ReCalcAttr(*it, false, true);
	}
}

void Pet::ClearDirtyMark()
{
	if (!m_pet_grid_is_change) return;

	for (int i = 0; i < ItemNamespace::MAX_PET_GRID_NUM; ++i)
	{
		if (m_dirty_mark[i]) m_old_state[i] = !this->PetInvalid(i);
	}
	memset(m_dirty_mark, 0, sizeof(m_dirty_mark));

	for (int i = 0; i < ARRAY_LENGTH(m_storage_dirty_mark); ++i)
	{
		if (m_storage_dirty_mark[i]) m_storage_old_state[i] = !this->PetStorageInvalid(i);

		m_storage_dirty_mark[i] = false;
	}

	m_pet_grid_is_change = false;
}

void Pet::SetItemDirty(int index, bool to_notice_client,int dirty_type)
{
	if (index < 0 || index >= ItemNamespace::MAX_PET_GRID_NUM) return;

	m_dirty_mark[index] = true;
	m_pet_grid_is_change = true;

	if (to_notice_client) this->SendSingleInfo(index, dirty_type);
}

int Pet::OnPetOp(Protocol::CSPetOp * op)
{
	int ret = 0;
	switch (op->op_type)
	{
	case Protocol::CSPetOp::AbandonmentPet:
		ret = this->AbandonmentPet(op->p1);
		break;
	case Protocol::CSPetOp::Rebirth:
		{
			const PetOtherCfg & other_cfg = LOGIC_CONFIG->GetPetConfig()->GetOtherCfgRef();
			if (!m_role_module_mgr->GetMoney()->CoinBindMoreThan(other_cfg.reset_level_need_coint))
			{
				return -99999;
			}

			ret = this->Rebirth(op->p1);

			if (ret == 0)
			{
				m_role_module_mgr->GetMoney()->UseCoinBind(other_cfg.reset_level_need_coint, __FUNCTION__);
			}
		}
		break;
	case Protocol::CSPetOp::Lock:
		ret = this->Lock(op->p1);
		break;
	case Protocol::CSPetOp::Breakthroughs:
		{
			int consume_pet_index_arr[3] = { op->p2, op->p3 ,op->p4 };
			ret = this->Breakthroughs(op->p1, consume_pet_index_arr, op->p5 != 0);
		}		
		break;
	case Protocol::CSPetOp::TransferBreakthroughs:
		ret = this->TransferBreakthroughs(op->p1, op->p2);
		break;
	case Protocol::CSPetOp::SetPetState:
		ret = this->SetPetState(op->p1, op->p2);
		break;
	case Protocol::CSPetOp::PetAttrsReq:
		ret = this->SendPetAttrInfo(op->p1);
		break;
	case Protocol::CSPetOp::AddExp:
		ret = this->OnUseExp(op->p1, op->p2, op->p3);
		break;
	case Protocol::CSPetOp::RefreshPoints:
		ret = this->OnRefreshPoints(op->p1);
		break;
	case Protocol::CSPetOp::Refined:
		ret = this->Refined(op->p1, op->p2 != 0, op->p3 != 0);
		break;
	case Protocol::CSPetOp::ComprehendPassiveSkill:
		ret = this->ComprehendPassiveSkill(op->p1);
		break;
	case Protocol::CSPetOp::Compose:
		ret = this->Compose(op->p1, op->p2);
		break;
	case Protocol::CSPetOp::RefinedSave:
		ret = this->RefinedSave(op->p1);
		break;
	case Protocol::CSPetOp::Convert:
		ret = this->Convert(op->p1);
		break;
	case Protocol::CSPetOp::RefinedGiveUp:
		ret = this->RefinedGiveUp(op->p1);
		break;
	case Protocol::CSPetOp::MoveStorage:
		ret = this->MoveToPetStorage(op->p1,op->p2);
		break;
	case Protocol::CSPetOp::MovePetGrid:
		ret = this->MoveToPetGrid(op->p1, op->p2);
		break;
	case Protocol::CSPetOp::ExtendBag:
		ret = this->ExtendPetBag();
		break;
	case Protocol::CSPetOp::PetEquipmentPutOn:
		this->PutOnPetEquipment(op->p1, op->p2);
		break;
	case Protocol::CSPetOp::PetEquipmentPutOff:
		this->PutOffPetEquipment(op->p1);
		break;
	case Protocol::CSPetOp::PetEquipmentChange:
		this->ChangePetEquipmentToOtherPet(op->p1, op->p2);
		break;
	case Protocol::CSPetOp::UsePetSkin:
		this->UsePetSkin(op->p1, op->p2);
		break;
	case Protocol::CSPetOp::UpLevelBreak:
		this->PetBreakLevel(op->p1);
		break;
	case Protocol::CSPetOp::UpLevelBreakAtrr:
		this->GetPetBreakLevelAttr(op->p1, op->p2);
		break;
	case Protocol::CSPetOp::GuideReplacePassiveSkill:
		this->GuideReplacePassiveSkill(op->p1);
		break;
	case Protocol::CSPetOp::GuideComprehendPassiveSkill:
		this->GuideComprehendPassiveSkill(op->p1);
		break;
	case Protocol::CSPetOp::GuideRefined:
		ret = this->GuideRefined(op->p1, op->p2 != 0, op->p3 != 0);
		break;
	case Protocol::CSPetOp::ReplacePassiveSkillSave:
		ret = this->ReplacePassiveSkillSave(op->p1, op->p2 == 0);
		break;
	case Protocol::CSPetOp::PutOnPetSoulEquipMent:
		this->PutOnPetSoulEquip(op->p1, op->p2, op->p3);
		break;
	case Protocol::CSPetOp::PutOffPetSoulEquipMent:
		this->PutOffPetSoulEquip(op->p1,op->p2);
		break;
	case Protocol::CSPetOp::StepsPetSoulEquipMent:
		this->StepsPetSoulEquip(op->p1, op->p2, op->p3);
		break;
	case Protocol::CSPetOp::RefinePetSoulEquipMent:
		this->RefinePetSoulEquip(op->p1, op->p2, op->p3, op->p4);
		break;
	case Protocol::CSPetOp::LockPetSoulEquipMent:
		this->LockPetSoulEquip(op->p1, op->p2, op->p3 == 1, op->p4);
		break;
	case Protocol::CSPetOp::PutOnPetGodPrintEquipMent:
		this->PutOnPetGodPrintEquip(op->p1, op->p2);
		break;
	case Protocol::CSPetOp::PutOffPetGodPrintEquipMent:
		this->PutOffPetGodPrintEquip(op->p1, op->p2);
		break;
	case Protocol::CSPetOp::UpLevelPetGodPrintEquipMent:
		this->UpLevelPetGodPrintEquip(op->p1, op->p2);
		break;
	case Protocol::CSPetOp::DisComplsePetGodPrintEquipMent:
		this->DiscomplsePetGodPrintEquip(op->p1);
		break;
	case Protocol::CSPetOp::PetGodPrintReqWish:
		this->ReqWishGodPrint(op->p1 == 1, op->p2, op->p3 == 1);
		break;
	case Protocol::CSPetOp::PetGodPrintReqWishLogList:
		this->SendGodPrintWishLogInfo();
		break;
	case Protocol::CSPetOp::PetGodPrintReqWishLogDetail:
		this->SendGodPrintWishDetailInfo(op->p1);
		break;
	case Protocol::CSPetOp::PetGodPrintReqExchange:
		this->ExchangeGodPrint(op->p1);
		break;
	case Protocol::CSPetOp::PetGodPrintReqLock:
		this->LockPetGodPrintEquip(op->p1, op->p2 == 1);
		break;
	case Protocol::CSPetOp::PetGodPrintReqUpLevelItem:
		this->UpLevelPetGodPrintEquip3(op->p1, op->p2, op->p3, op->p4);
		break;
	case Protocol::CSPetOp::PetGodPrintReqWearScheme:
		this->WearShcheme(op->p1, op->p2);
		break;
	case Protocol::CSPetOp::PetGodPrintReqDiscomplse:
		this->DiscomplsePetGodPrintItem(op->p1, op->p2, op->p3);
		break;
	case Protocol::CSPetOp::PET_OP_TYPE_KEY_LEVEL_UP:
		ret = this->OnKeyLevelUp(op->p1);
		break;
	case Protocol::CSPetOp::PetSoulEquipExchange:
		this->ExchangePetSoulEquip(op->p1);
		break;
	case Protocol::CSPetOp::PetSoulEquipChange:
		this->ChangePetSoulEquip(op->p1, op->p2, op->p3, op->p4);
		break;
	case Protocol::CSPetOp::PetMutant:
		ret = this->Mutant(op->p1);
		break;
	case Protocol::CSPetOp::SkillBookConvert:
		SkillBookConvert::ConvertSkillBook(m_role_module_mgr->GetRole(), ItemConfigData(op->p1, false, 1), ItemConfigData(op->p2, false, 1));	
		break;
	case Protocol::CSPetOp::PetBenediction:
		ret = this->OnPetBenediction(op->p1, op->p2);
		break;
	case Protocol::CSPetOp::PetCancelBenediction:
		ret = this->OnCancelBenediction(op->p1);
		break;
	case Protocol::CSPetOp::StartPetFeiShengTask:
		ret = this->ReqStartFeiShengTask(op->p1);
		break;
	case Protocol::CSPetOp::UpLevelPassiveSkill:
		ret = this->OnUpgradePetSkill(op->p1, op->p2, op->p3);
		break;		
	case Protocol::CSPetOp::PetFeiSheng:
		ret = this->ReqFeiSheng(op->p1, op->p2);
		break;
	case Protocol::CSPetOp::PetFeiShengSkillMod:
		ret = this->ReqFeiShengSkillMod(op->p1, op->p2);
		break;
	case Protocol::CSPetOp::PetFeiShengRewardFetch:
		ret = this->FetchYiWenLuReward(op->p1, op->p2);
		break;
	case Protocol::CSPetOp::PetFeiShengSkillUplv:
		ret = this->FeiShengSkillUpLevel(op->p1);
		break;		
	case Protocol::CSPetOp::PetGodPrintReqExchange2:
		this->ExchangeGodPrint(op->p1, op->p2);
		break;
	case Protocol::CSPetOp::PetBadgePutOn:
		ret = this->PetBadgePutOn(op->p1, op->p2, op->p3);
		break;
	case Protocol::CSPetOp::PetBadgeTakeOff:
		ret = this->PetBadgeTakeOff(op->p1);
		break;
	case Protocol::CSPetOp::PetBadgeExChange:
		ret = this->PetBadgeExchange(op->p1, op->p2);
		break;
	case Protocol::CSPetOp::PetBadgeCompose:
		ret = this->PetBadgeCompose(op->p1);
		break;
	case Protocol::CSPetOp::PetLianYao:
		ret = this->PetLianYao(op->p1, op->p2, op->p3);
		break;
	case Protocol::CSPetOp::PetLianYaoComprehendPassiveSkill:
		ret = this->PetLianYaoComprehendPassiveSkill(op->p1);
		break;
	case Protocol::CSPetOp::PetUnlockTalent:
		ret = this->OnPetUnlockTalent(op->p1, op->p2, op->p3);
		break;
	case Protocol::CSPetOp::PetTalentSkillLevelUp:
		ret = this->OnPetTalentSkillLevelUp(op->p1, op->p2);
		break;
	case Protocol::CSPetOp::PetTalentSkillReplace:
		ret = this->OnPetTalentSkillReplace(op->p1, op->p2, op->p3);
		break;
	case Protocol::CSPetOp::PetTalentReset:
		ret = this->OnPetTalentReset(op->p1);
		break;
	case Protocol::CSPetOp::PetStrengthenBuffSet:
		ret = this->SetStrengthenBuffPet(op->p2,op->p3);
		break;
	case Protocol::CSPetOp::PetStrengthenBuffUnSet:
		ret = this->UnSetStrengthenBuffPet(op->p1, op->p2);
		break;
	case Protocol::CSPetOp::PetGodPrintDevour:
		ret = this->DevourGodPrint(op->p1, op->p2, op->p3);
		break;
	case Protocol::CSPetOp::PetGodPrintDrop:
		ret = this->DropGodprintEquip(op->p1);
		break;
	case Protocol::CSPetOp::PetGaiMingUpLevel:
		ret = this->PetGaiMingUpLevel(op->p1, op->p2, op->p3);
		break;
	case Protocol::CSPetOp::PetGaiMingAttrUpLevel:
		ret = this->PetGaiMingAttrUpLevel(op->p1, op->p2);
		break;
	case Protocol::CSPetOp::PetGaiMingAttrReset:
		ret = this->PetGaiMingAttrReset(op->p1);
		break;
	default:
		break;
	}

	return ret;
}

bool Pet::FillSpecialPetData(int item_id, ItemDataWrapper ARG_OUT & item, int pet_level, bool is_guide_pet)
{
	const ItemBase * item_ib = ITEMPOOL->GetItem(item_id);
	if (NULL == item_ib) return false;

	if (item_ib->GetItemType() != I_TYPE_SPECIAL_PET)
	{
		return false;
	}

	const SpecialPet * special_pet = static_cast<const SpecialPet *>(item_ib);
	const PetCfg * cfg = LOGIC_CONFIG->GetPetConfig()->GetPetCfg(special_pet->GetPetId());
	if (NULL == cfg)
	{
		return false;
	}

	int pet_id = special_pet->GetPetId();

	static NetValueItemParam pet; pet.Reset();
	pet.pet_param.pet_id = pet_id;
	memcpy(pet.pet_param.name, cfg->name, sizeof(GameName));
	pet.pet_param.str_level = 1;
	pet.pet_param.level = pet_level;
	pet.pet_param.exp = 0;
	pet.pet_param.hp = 0;
	pet.pet_param.mp = 0;

	std::vector<int> get_gift_list = LOGIC_CONFIG->GetPetConfig()->GetRandPassive(cfg->passive_group_id_vec);
	if (get_gift_list.empty())
	{
		get_gift_list = LOGIC_CONFIG->GetPetConfig()->GetRandPassive(cfg->passive_group_id_lowest_vec);
	}

	if (is_guide_pet)
	{
		get_gift_list = cfg->guide_passive_vec;
		pet.pet_param.guide_pet_flag |= GUIDE_PET_FLAG_TRUE;
	}

	pet.pet_param.ClearGift();
	for (int i = 0; i < (int)get_gift_list.size() && i < ARRAY_LENGTH(pet.pet_param.passive_list); ++i)
	{
		pet.pet_param.passive_list[i].passive_skill_id = get_gift_list[i];
	}

	{
		// 随机资质
		int rand_num = 0;
		for (int i = ADD_POINT_TYPE_MIN; i < ADD_POINT_TYPE_MAX; ++i)
		{
			rand_num = special_pet->GetRandBp(i);
			pet.pet_param.growth[i] = rand_num;
		}
	}

	{// 已分配的属性点
		memset(pet.pet_param.attr, 0, sizeof(pet.pet_param.attr));
	}

	pet.pet_param.pet_growth_version = LOGIC_CONFIG->GetPetConfig()->GetPetCurGrowthVersion(pet.pet_param.pet_id);

	item.item_id = pet_id;
	item.num = 1;
	item.is_bind = true;
	item.gold_price = 0;
	item.invalid_time = 0u;
	UniqueIDGenerator::Instance()->GenUniqueID(&item.item_unique_id);
	item.has_param = 1;
	pet.GetData(item.param_data);

	return true;
}

int Pet::AcquirePet(int pet_id, const char* reason, int str_level /* = 1 */, bool is_max /* = false */, int notice_reason /* = GET_PET_REASON_TYPE_DEFAULT */, bool is_bind /* = false */, bool is_xunbao /* = false */, long long item_unique_id /* = 0LL */)
{
	if (!this->HaveSpace(1))
	{
		return -2;
	}

	const PetCfg *cfg = LOGIC_CONFIG->GetPetConfig()->GetPetCfg(pet_id);
	if (NULL == cfg) return -1;

	NetValueItemParam pet; 
	if (!LOGIC_CONFIG->GetPetConfig()->RandPetParam(pet_id, str_level, is_max, pet))
	{
		return -3;
	}
	int pet_index = this->AddPet(&pet, reason, is_bind, is_xunbao, item_unique_id);

	if (notice_reason != GET_PET_REASON_TYPE_INVALID)
	{
		Protocol::SCSendPetGetRet notice;
		notice.index = pet_index;
		notice.reason = notice_reason;
		EngineAdapter::Instance().NetSend(m_role_module_mgr->GetRole()->GetNetId(), &notice, sizeof(notice));
	}

	ROLE_LOG_QUICK6(LOG_TYPE_PET_ADD, m_role_module_mgr->GetRole(), pet_id, item_unique_id, __FUNCTION__, NULL);
	return pet_index;
}

bool Pet::AcquirePet2(int pet_id, const char * reason, int put_reson, bool is_bind /* = false */, int str_level /* = 1 */, bool is_max /* = false */, long long item_unique_id /* = 0LL */)
{
	int notice_reason = GET_PET_REASON_TYPE_DEFAULT;

	switch (put_reson)
	{
		case PUT_REASON_GIFT_RAND_ITEM:
		case 	PUT_REASON_RAND_GIFT_PET_EGG:
		case	PUT_REASON_OPTIONAL_GIFT_PET_EGG:
		case	PUT_REASON_DEF_GIFT_PET_EGG:
			notice_reason = GET_PET_REASON_TYPE_PET_EGG;
			break;
		case PUT_REASON_SHEN_SHOU_JIANG_LIN:
			notice_reason = GET_PET_REASON_TYPE_SHEN_SHOU_JIANG_LIN;
			break;
		case PUT_REASON_REALM_BREAK_FIGHT:
			notice_reason = GET_PET_REASON_TYPE_REALM_BREAK_FIGHT;
			break;
	}

	bool ret = this->AcquirePet(pet_id, reason, ::GetMax(str_level, 1), is_max, notice_reason, is_bind, false, item_unique_id) >= 0;

	return ret;
}

int Pet::AcquireSpecialPet(const ItemBase* special_pet_item, const char* reason, bool is_bind, bool is_xunbao /* = false */, long long item_unique_id /* = 0LL */)
{
	if (special_pet_item->GetItemType() != I_TYPE_SPECIAL_PET)
	{
		return -3;
	}

	if (!this->HaveSpace(1))
	{
		return -2;
	}

	const SpecialPet * special_pet = static_cast<const SpecialPet *>(special_pet_item);

	ItemDataWrapper temp;
	if (!this->FillSpecialPetData(special_pet_item->GetItemId(), temp, 1, false))
	{
		return -3;
	}

	NetValueItemParam pet;
	pet.SetData(temp.param_data);

	int pet_index = this->AddPet(&pet, reason, is_bind, is_xunbao, item_unique_id);

	{
		Protocol::SCSendPetGetRet notice;
		notice.index = pet_index;
		notice.reason = GET_PET_REASON_TYPE_SPECIAL_PET;
		EngineAdapter::Instance().NetSend(m_role_module_mgr->GetRole()->GetNetId(), &notice, sizeof(notice));
	}
	ROLE_LOG_QUICK6(LOG_TYPE_PET_ADD, m_role_module_mgr->GetRole(), special_pet->GetPetId(), item_unique_id, __FUNCTION__, NULL);
	return pet_index;
}

int Pet::AbandonmentPet(int pet_index)
{
	if (this->PetInvalid(pet_index)) return -1;
	PetParam& pet = m_grid_list[pet_index].param->pet_param;
	if (pet.HasGodPrint())
	{
		if (!this->PutOffAllPetGodPrintEquip(pet_index))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_PET_WEAR_GOD_PRINT);
			return  -__LINE__;
		}
	}

	if (this->PetBadgeTakeOff(pet_index) != 0)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_PET_BADGE_TAKE_OFF_FAIL);
		return  -__LINE__;
	}

	if (!this->PutOffAllPetSoulEquip(pet_index))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_PET_WEAR_SOUL_EQUIP);
		return  -__LINE__;
	}

	if (this->IsLock(pet_index)) return -2;

	if (m_role_module_mgr->GetRole()->GetRoleStatusManager()->IsInBattleStatus())
	{
		m_role_module_mgr->NoticeNum(errornum::EN_ROLE_IN_BATTLE);
		return -__LINE__;
	}

	if (this->IsInPlatfformBattle(m_grid_list[pet_index].item_unique_id, true))
	{
		return -__LINE__;
	}
	if (m_role_module_mgr->GetSmartMountsSystem()->IsPetDomination(m_grid_list[pet_index].item_unique_id))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_SMART_MOUNTS_SYSTEM_PET_DOMINATION);
		return -__LINE__;
	}
	if (-1 != m_common_param.benediction_pet_index_list[pet_index])
	{
		m_role_module_mgr->NoticeNum(errornum::EN_PET_NOT_OP_BY_BENEDICTION);
		return -__LINE__;
	}

	const PetCfg * cfg = LOGIC_CONFIG->GetPetConfig()->GetPetCfg(pet.pet_id);
	if (NULL == cfg)
	{
		return -__LINE__;
	}
	PetEquipMentParam pet_equipment;
	pet.GetPetEquip(&pet_equipment);
	if (pet_equipment.pet_equipment_id != 0)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_PET_HSA_EQUIPMENT_ERR);
		return -__LINE__;
	}
	
	if (0 != this->ConversionExp(pet_index, 1, __FUNCTION__))
	{
		return -__LINE__;
	}
	
	this->PetGaiMingReturn(pet_index, __FUNCTION__);
	
	const PetAbandonmentCfg * abandonment_cfg =LOGIC_CONFIG->GetPetConfig()->GetPetAbandonmentCfg(cfg->abandonment_cfg_id, pet.str_level);
	if (abandonment_cfg && abandonment_cfg->abandonment_get_item_num > 0)
	{
		ItemConfigData ret_item(abandonment_cfg->abandonment_get_item_id, true, abandonment_cfg->abandonment_get_item_num);
		if (!m_role_module_mgr->GetKnapsack()->Put(ret_item, PUT_REASON_PET_ABANDONMENT_SHARDS))
		{
			m_role_module_mgr->GetKnapsack()->SendMail(&ret_item, 1, SNED_MAIL_TYPE_DEFAULT, true);
		}
	}
	ROLE_LOG_QUICK10(LOG_TYPE_PET_ABANDONMENT, m_role_module_mgr->GetRole(), pet_index, pet.pet_id, __FUNCTION__, NULL, pet.equipment_item_unique_id, pet.str_level, 0, 0, 0);

	//宠物排行榜所需
	unsigned long long  pet_unique_id = m_grid_list[pet_index].item_unique_id;

	this->DeletePet(pet_index, __FUNCTION__);
	RankManager::Instance().SyncRemovePet(this->GetRoleModuleManager()->GetRole(), pet_unique_id);
	EventHandler::Instance().OnPetAbandon(m_role_module_mgr->GetRole(), cfg->quality);
	
	return 0;
}

int Pet::AddExp(int pet_index, long long add_exp, const char * reason,int notice_reason)
{
	if (this->PetInvalid(pet_index)) return -1;
	PetParam & pet = m_grid_list[pet_index].param->pet_param;

	const PetLevelCfg * level_cfg = LOGIC_CONFIG->GetPetConfig()->GetLevelCfg(pet.level);
	if (level_cfg == NULL)
	{
		return -101;
	}

	if (level_cfg->is_need_breach && pet.is_up_level_break == 0)
	{
		// 传闻
		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_pet_add_exp_err_without_break_level, pet.name);
		if (length > 0)
		{
			World::GetInstWorld()->GetSceneManager()->SystemMsgPerson2(m_role_module_mgr->GetRole(), gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_SYSTEM_CHAT));
		}

		return -102;
	}

	Role * role = m_role_module_mgr->GetRole();
	if (NULL == role) return -3;

	int role_level = role->GetLevel();
	if (pet.level >= role_level)
	{
		Protocol::SCPetAddExpNotice notice;
		notice.notice_type = Protocol::PET_ADD_EXP_NOTICE_TYPE_GE_ROLE_LVEL;
		notice.pet_index = pet_index;
		m_role_module_mgr->NetSend(&notice, sizeof(notice));

		return -3;			// 宠物等级不得超过玩家等级
	}

	int temp_level = pet.level;
	long long temp_exp = pet.exp;
	int max_exp = level_cfg->exp;
	bool is_need_breach = level_cfg->is_need_breach; //是否需要突破

	gamelog::g_log_pet.printf(LL_INFO, "%s line:%d |%s %s add_exp:%lld reason:%s notice_reason:%d  level:%d exp:%lld ", __FUNCTION__, __LINE__, m_role_module_mgr->GetRoleInfoString().c_str(), pet.GetPetBaseInfoString(this->GetPetUniqueId(pet_index)).c_str(),
				  add_exp, reason, notice_reason, pet.level, pet.exp);

	temp_exp += add_exp;
	bool level_up = false;
	while (max_exp > 0 && temp_exp >= max_exp && temp_level < role_level && (!is_need_breach || pet.is_up_level_break))
	{
		temp_level++;
		temp_exp -= max_exp;

		const PetLevelCfg * level_cfg = LOGIC_CONFIG->GetPetConfig()->GetLevelCfg(temp_level);
		if (level_cfg != NULL)
		{
			max_exp = level_cfg->exp;
			is_need_breach = level_cfg->is_need_breach;
		}

		level_up = true;
		pet.is_up_level_break = 0;
	}

	pet.level = temp_level;
	pet.exp = temp_exp;
	pet.add_exp = add_exp;

	this->SendSingleInfo(pet_index, notice_reason);

	gamelog::g_log_pet.printf(LL_INFO, "%s line:%d | %s %s add_exp:%lld reason:%s notice_reason:%d  level:%d exp:%lld ", __FUNCTION__, __LINE__, m_role_module_mgr->GetRoleInfoString().c_str(), pet.GetPetBaseInfoString(this->GetPetUniqueId(pet_index)).c_str(),
				  add_exp, reason, notice_reason, pet.level, pet.exp);

	if (level_up)
	{
		pet.is_up_level_break = 0;
		this->AutoAssignAttributePoints(pet_index,true);
		this->ReCalcAttr(pet_index);
		this->SendSingleInfo(pet_index, Protocol::SCSendPetSingleInfo::PET_NOTIE_TYPE_AUTO_ASSIGN_POINTS);

		EventHandler::Instance().OnPetBaseInfoChange(m_role_module_mgr->GetRole(), pet_index);
		EventHandler::Instance().OnPetUpLevel(role, pet_index, pet.level);
		if (this->IsInPlatfformBattle(m_grid_list[pet_index].item_unique_id))
		{
			this->SyncPetDataPlatformBattle(pet_index);
		}
	}

	this->SetItemDirty(pet_index);
	return 0;
}

int Pet::OnKeyLevelUp(int pet_index)
{
	if (this->PetInvalid(pet_index)) return -1;
	PetParam & pet = m_grid_list[pet_index].param->pet_param;
	int role_level = m_role_module_mgr->GetRole()->GetLevel();
	if (pet.level >= role_level)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_PET_USE_EXP_PILL);
		return -2;			// 宠物等级不得超过玩家等级
	}

	const PetLevelCfg * level_cfg = LOGIC_CONFIG->GetPetConfig()->GetLevelCfg(pet.level);
	if (level_cfg == NULL)
	{
		return -3;
	}

	//获取此时所拥有的经验药数量
	const std::map<int, int> exp_pill_cfg = LOGIC_CONFIG->GetPetConfig()->GetPetExPillMap();
	std::vector<PetExpPillInfo> has_exp_count_vec;
	for (std::map<int, int>::const_iterator it = exp_pill_cfg.begin(); it != exp_pill_cfg.end(); ++it)
	{
		PetExpPillInfo node;
		node.count = m_role_module_mgr->GetKnapsack()->Count(it->second);
		if (node.count <= 0) continue;

		node.item_id = it->second;
		node.add_exp = it->first;
		has_exp_count_vec.push_back(node);
	}
	//没有经验药
	if (has_exp_count_vec.empty())
	{
		return - 4;
	}

	//计算此时升级所需消耗 
	std::map<int, int> need_consume_list;		//消耗道具列表 key:item_id second:数量
	int temp_level = pet.level;
	long long temp_exp = pet.exp;
	int max_exp = level_cfg->exp;
	bool is_need_breach = level_cfg->is_need_breach; //是否需要突破
	long long add_exp_count = 0;

	bool level_up = 0;
	int count = 0;
	while (temp_level < role_level && count++ < MAX_ROLE_LEVEL)
	{
		//先计算突破
		if (is_need_breach && !pet.is_up_level_break)
		{
			if (NULL == level_cfg || !m_role_module_mgr->GetKnapsack()->ConsumeItem(level_cfg->break_consume_id, level_cfg->break_consume_num, __FUNCTION__))
			{
				m_role_module_mgr->NoticeNum(errornum::EN_USE_PET_EXP_ITEM_ERR_WITHOUT_BREAK_LEVEL);
				break;
			}
			need_consume_list[level_cfg->break_consume_id] += level_cfg->break_consume_num;
			pet.is_up_level_break = 1;
		}
		
		//自身经验足够升级	
		if (temp_exp >= max_exp)
		{
			temp_level++;
			temp_exp -= max_exp;
			level_cfg = LOGIC_CONFIG->GetPetConfig()->GetLevelCfg(temp_level);
			if (level_cfg != NULL)
			{
				max_exp = level_cfg->exp;
				is_need_breach = level_cfg->is_need_breach;
			}
			pet.is_up_level_break = 0;
			level_up = true;
		}
		//经验药升级
		else		
		{
			long long add_exp = 0;
			for (int i = 0; i < (int)has_exp_count_vec.size(); i++)
			{
				PetExpPillInfo & info = has_exp_count_vec[i];
				if (info.count <= 0)
				{
					has_exp_count_vec.erase(has_exp_count_vec.begin() + i);
					i--;
					continue;
				}

				int consume_count = info.count;
				add_exp = 1LL * info.count * info.add_exp;
				if (temp_exp + add_exp >= max_exp)
				{
					consume_count = (max_exp - temp_exp) / info.add_exp + (((max_exp - temp_exp) % info.add_exp) > 0 ? 1 : 0);
					add_exp = consume_count * info.add_exp;
				}
				if (!m_role_module_mgr->GetKnapsack()->ConsumeItem(info.item_id, consume_count, __FUNCTION__))
				{
					break;
				}
				need_consume_list[info.item_id] += consume_count;
				info.count -= consume_count;
				if (temp_exp + add_exp >= max_exp)
				{
					temp_level++;
					temp_exp = temp_exp + add_exp - max_exp;
					level_cfg = LOGIC_CONFIG->GetPetConfig()->GetLevelCfg(temp_level);
					if (level_cfg != NULL)
					{
						max_exp = level_cfg->exp;
						is_need_breach = level_cfg->is_need_breach;
					}
					pet.is_up_level_break = 0;
					level_up = true;
					add_exp_count += consume_count * info.add_exp;
					break;
				}
				else
				{
					temp_exp += add_exp;
					add_exp_count += add_exp;
				}
			}
		}
	}
	pet.level = temp_level;
	pet.exp = temp_exp;
	pet.add_exp = add_exp_count;
	this->SendSingleInfo(pet_index, Protocol::SCSendPetSingleInfo::PET_NOTIE_TYPE_KEY_LEVEL_UP);
	gamelog::g_log_pet.printf(LL_INFO, "%s line:%d | %s %s add_exp:%lld notice_reason:%d  level:%d exp:%lld ", __FUNCTION__, __LINE__, m_role_module_mgr->GetRoleInfoString().c_str(), pet.GetPetBaseInfoString(this->GetPetUniqueId(pet_index)).c_str(),
		add_exp_count, Protocol::SCSendPetSingleInfo::PET_NOTIE_TYPE_KEY_LEVEL_UP , pet.level, pet.exp);
	if (!need_consume_list.empty())
	{
		std::vector<ItemConfigData> notice_list;
		for (std::map<int, int>::iterator it = need_consume_list.begin(); it != need_consume_list.end(); ++it)
		{
			notice_list.push_back(ItemConfigData(it->first, true, it->second));
		}
		m_role_module_mgr->GetKnapsack()->NotifyGetItemList((short)notice_list.size(), &notice_list[0], GET_REASON_PET_KEY_LEVEL_UP);
	}
	if (level_up)
	{
		this->AutoAssignAttributePoints(pet_index, true);
		this->ReCalcAttr(pet_index);
		this->SendSingleInfo(pet_index, Protocol::SCSendPetSingleInfo::PET_NOTIE_TYPE_AUTO_ASSIGN_POINTS);

		EventHandler::Instance().OnPetBaseInfoChange(m_role_module_mgr->GetRole(), pet_index);
		EventHandler::Instance().OnPetUpLevel(m_role_module_mgr->GetRole(), pet_index, pet.level);
		if (this->IsInPlatfformBattle(m_grid_list[pet_index].item_unique_id))
		{
			this->SyncPetDataPlatformBattle(pet_index);
		}
	}

	this->SetItemDirty(pet_index);
	return 0;
}

int Pet::PetBreakLevel(int pet_index)
{
	if (this->PetInvalid(pet_index)) return -1;
	PetParam & pet = m_grid_list[pet_index].param->pet_param;

	Role * role = m_role_module_mgr->GetRole();
	if (NULL == role) return -3;

	if (pet.is_up_level_break)
	{
		return -4;
	}

	int temp_level = pet.level;
	const PetLevelCfg * level_cfg = LOGIC_CONFIG->GetPetConfig()->GetLevelCfg(temp_level);
	if (level_cfg == NULL)
	{
		return -5;
	}

	if (!level_cfg->is_need_breach)
	{
		return -6;
	}


	if (!role->GetRoleModuleManager()->GetKnapsack()->ConsumeItem(level_cfg->break_consume_id, level_cfg->break_consume_num, __FUNCTION__))
	{
		return -8;
	}

	pet.is_up_level_break = 1;	
	this->ReCalcAttr(pet_index);
	this->SetItemDirty(pet_index, true, Protocol::SCSendPetSingleInfo::PET_NOTIE_TYPE_PET_UP_LEVEL_BREAK);	

	gamelog::g_log_pet.printf(LL_INFO, "%s line:%d | %s  %s", __FUNCTION__, __LINE__, m_role_module_mgr->GetRoleInfoString().c_str(), pet.GetPetBaseInfoString(this->GetPetUniqueId(pet_index)).c_str());

	this->AddExp(pet_index, 0, __FUNCTION__, Protocol::SCSendPetSingleInfo::PET_NOTIE_TYPE_NO_TYPE);	

	return 0;
}

int Pet::GetPetBreakLevelAttr(int pet_index, int level)
{
	static AttributeList temp_attr; temp_attr.Reset();
	static AttributeList no_skill_attr; no_skill_attr.Reset();

	const PetParam * p = this->GetPetParam(pet_index);
	if (p != NULL)
	{
		PetParam pet = *p;
		pet.level = level;
		pet.is_up_level_break = true;
		std::map<int, int> pet_train_2_map = m_role_module_mgr->GetTrain()->GetPetTrainUp2Map();
		this->GetAttributeListHelper2(pet, temp_attr, no_skill_attr, m_role_module_mgr->GetRole()->GetLevel(), m_grid_list[pet_index].item_unique_id, this->GetStrBuffLevel(pet_index), this->IsWaitForFightPet(pet_index), m_role_module_mgr->GetTrain()->GetRolePetTrainSkillLevel(), &pet_train_2_map);
	}


	Protocol::SCPetCalAttrRet protocol;
	protocol.index = pet_index;
	protocol.attr_list = temp_attr;
	
	m_role_module_mgr->NetSend(&protocol, sizeof(protocol));

	return 0;
}

int Pet::ConversionExp(int pet_index, int to_level, const char* reason, int put_reason /* = PUT_PET_CONVERSION_EXP */)
{
	if (this->PetInvalid(pet_index)) return -1;
	PetParam& pet = m_grid_list[pet_index].param->pet_param;
	int pet_level = pet.level;
	
	double return_tate = LOGIC_CONFIG->GetPetConfig()->GetOtherCfg()->rate_return_exp / 100.0;

	std::vector<ItemConfigData> ret_reward;
	long long return_exp = pet.exp;

	for (;pet_level > to_level; pet_level--)
	{
		const PetLevelCfg * level_cfg = LOGIC_CONFIG->GetPetConfig()->GetLevelCfg(pet_level - 1);
		if (level_cfg != NULL)
		{
			return_exp += level_cfg->exp;

		}
	}

	for (int lv = to_level + 1; lv <= pet.level; ++lv)
	{
		const PetLevelCfg * level_cfg = LOGIC_CONFIG->GetPetConfig()->GetLevelCfg(lv);
		if (level_cfg == NULL)
		{
			continue;
		}

		if (level_cfg->is_need_breach)
		{
			if (lv == pet.level && !pet.is_up_level_break)
			{
				continue;
			}

			ret_reward.push_back(ItemConfigData(level_cfg->break_consume_id, true, level_cfg->break_consume_num));
		}
	}




	std::vector<ItemConfigData> jin_yan_yao = LOGIC_CONFIG->GetPetConfig()->PutEXPtoConversionPetEXPPill((long long)(return_exp * return_tate));
	ret_reward.insert(ret_reward.end(), jin_yan_yao.begin(), jin_yan_yao.end());

	if (!ret_reward.empty() && !m_role_module_mgr->GetKnapsack()->PutList((short)ret_reward.size(), &ret_reward[0], put_reason))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return -2;
	}
	gamelog::g_log_pet.printf(LL_INFO, "%s line:%d |%s  %s  reason:%s level:%d exp:%lld ", __FUNCTION__, __LINE__, m_role_module_mgr->GetRoleInfoString().c_str(), pet.GetPetBaseInfoString(this->GetPetUniqueId(pet_index)).c_str(),
				 reason, pet.level, pet.exp);

	pet.exp = 0;
	pet.level = to_level;
	memset(pet.attr, 0, sizeof(pet.attr));
	pet.is_up_level_break = 0;

	const PetLevelCfg * level_cfg = LOGIC_CONFIG->GetPetConfig()->GetLevelCfg(to_level);
	if (level_cfg != NULL && level_cfg->is_need_breach)
	{
		pet.is_up_level_break = 1;
	}

	gamelog::g_log_pet.printf(LL_INFO, "%s line:%d |%s  %s  reason:%s level:%d exp:%lld ", __FUNCTION__, __LINE__, m_role_module_mgr->GetRoleInfoString().c_str(), pet.GetPetBaseInfoString(this->GetPetUniqueId(pet_index)).c_str(),
				  reason, pet.level, pet.exp);

	return 0;
}

int Pet::ConversionExpByList(std::set<int> pet_index_set, int to_level, const char * reason, int put_reason /* = PUT_PET_CONVERSION_EXP */)
{
	std::vector<ItemConfigData> ret_reward;
	for (std::set<int>::iterator iter = pet_index_set.begin(); iter != pet_index_set.end(); ++iter)
	{
		int index = *iter;
		const PetParam * ptr = NULL;
		if (index < ItemNamespace::PET_STORAGE_INDEX_BEGIN)
		{
			ptr = this->GetPetParam(index);
		}
		else
		{	
			ptr = this->GetPetParamFromStorage(index);
		}

		if (ptr == NULL)
		{
			return -__LINE__;
		}

		const PetParam & pet = *ptr;
		int pet_level = pet.level;

		long long return_exp = pet.exp;
		for (; pet_level > to_level; pet_level--)
		{
			const PetLevelCfg * level_cfg = LOGIC_CONFIG->GetPetConfig()->GetLevelCfg(pet_level - 1);
			if (level_cfg != NULL)
			{
				return_exp += level_cfg->exp;
			}
		}

		for (int lv = to_level + 1; lv <= pet.level; ++lv)
		{
			const PetLevelCfg * level_cfg = LOGIC_CONFIG->GetPetConfig()->GetLevelCfg(lv);
			if (level_cfg == NULL)
			{
				continue;
			}

			if (level_cfg->is_need_breach)
			{
				if (lv == pet.level && !pet.is_up_level_break)
				{
					continue;
				}

				ret_reward.push_back(ItemConfigData(level_cfg->break_consume_id, true, level_cfg->break_consume_num));
			}
		}

		double return_tate = LOGIC_CONFIG->GetPetConfig()->GetOtherCfg()->rate_return_exp / 100.0;
		std::vector<ItemConfigData> jin_yan_yao = LOGIC_CONFIG->GetPetConfig()->PutEXPtoConversionPetEXPPill((long long)(return_exp * return_tate));
		ret_reward.insert(ret_reward.end(), jin_yan_yao.begin(), jin_yan_yao.end());
	}

	if (!ret_reward.empty() && !m_role_module_mgr->GetKnapsack()->PutList((short)ret_reward.size(), &ret_reward[0], put_reason))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return -__LINE__;
	}

	for (std::set<int>::iterator iter = pet_index_set.begin(); iter != pet_index_set.end(); ++iter)
	{
		int curr = *iter;
		PetParam * ptr = NULL;
		long long pet_uuid = 0;

		if (curr < ItemNamespace::PET_STORAGE_INDEX_BEGIN)
		{
			int index = curr;
			ptr = this->__GetPetParam(index);
			pet_uuid = this->GetPetUniqueId(index);
		}
		else
		{
			int index = curr - ItemNamespace::PET_STORAGE_INDEX_BEGIN;
			ptr = this->GetPetParamFromStorageNoConst(index);
			pet_uuid = this->GetPetStorageUniqueId(index);
		}

		if (ptr == NULL)
		{
			continue;
		}
		
		PetParam & pet = *ptr;

		gamelog::g_log_pet.printf(LL_INFO, "%s line:%d |%s  %s  reason:%s level:%d exp:%lld ", __FUNCTION__, __LINE__, m_role_module_mgr->GetRoleInfoString().c_str(), pet.GetPetBaseInfoString(pet_uuid).c_str(),
					  reason, pet.level, pet.exp);

		pet.exp = 0;
		pet.level = to_level;
		memset(pet.attr, 0, sizeof(pet.attr));
		pet.is_up_level_break = 0;

		const PetLevelCfg * level_cfg = LOGIC_CONFIG->GetPetConfig()->GetLevelCfg(to_level);
		if (level_cfg != NULL && level_cfg->is_need_breach)
		{
			pet.is_up_level_break = 1;
		}

		gamelog::g_log_pet.printf(LL_INFO, "%s line:%d |%s  %s  reason:%s level:%d exp:%lld ", __FUNCTION__, __LINE__, m_role_module_mgr->GetRoleInfoString().c_str(), pet.GetPetBaseInfoString(pet_uuid).c_str(),
					  reason, pet.level, pet.exp);
	}

	return 0;
}

int Pet::Rebirth(int pet_index)
{
	if (this->PetInvalid(pet_index)) return -__LINE__;

	if (this->ConversionExp(pet_index, 1, __FUNCTION__) != 0)
	{
		return -__LINE__;
	}

	PetParam & pet = m_grid_list[pet_index].param->pet_param;

	gamelog::g_log_pet.printf(LL_INFO, "%s line:%d |%s  %s   level:%d exp:%lld ", __FUNCTION__, __LINE__, m_role_module_mgr->GetRoleInfoString().c_str(), pet.GetPetBaseInfoString(this->GetPetUniqueId(pet_index)).c_str(),
				   pet.level, pet.exp);

	pet.exp = 0;
	pet.level = 1;

	gamelog::g_log_pet.printf(LL_INFO, "%s line:%d |%s  %s   level:%d exp:%lld ", __FUNCTION__, __LINE__, m_role_module_mgr->GetRoleInfoString().c_str(), pet.GetPetBaseInfoString(this->GetPetUniqueId(pet_index)).c_str(),
				  pet.level, pet.exp);

	memset(pet.attr, 0, sizeof(pet.attr));

	this->ReCalcAttr(pet_index);
	this->SetItemDirty(pet_index,true,Protocol::SCSendPetSingleInfo::PET_NOTIE_TYPE_REBIRTH);
	
	if (this->IsInPlatfformBattle(m_grid_list[pet_index].item_unique_id))
	{
		this->SyncPetDataPlatformBattle(pet_index);
	}

	return 0;
}

int Pet::Recover(int pet_index)
{
	if (this->PetInvalid(pet_index)) return -1;

	PetParam & pet = m_grid_list[pet_index].param->pet_param;
	pet.max_hp = m_pet_base_attr[pet_index].m_attrs[BATTLE_ATTR_MAX_HP];
	pet.max_mp = m_pet_base_attr[pet_index].m_attrs[BATTLE_ATTR_MAX_MP];

	pet.hp = pet.max_hp;
	pet.mp = pet.max_mp;

	m_pet_base_attr[pet_index].m_attrs[BATTLE_ATTR_CUR_HP] = pet.max_hp;
	m_pet_base_attr[pet_index].m_attrs[BATTLE_ATTR_CUR_MP] = pet.max_mp;

	this->SetItemDirty(pet_index, true, Protocol::SCSendPetSingleInfo::PET_NOTIE_TYPE_RECOVER);
	
	EventHandler::Instance().OnPetBaseInfoChange(m_role_module_mgr->GetRole(), pet_index);
	
	return 0;
}

void Pet::AllPetRecoverHpMpPercent(double percent)
{
	for (int i = 0; i < ARRAY_LENGTH(m_role_module_mgr->GetCommonData().new_pet_status_index); ++i)
	{
		int pet_index = m_role_module_mgr->GetCommonData().new_pet_status_index[i];
		PetParam* pet_param = this->__GetPetParam(pet_index);
		if (NULL == pet_param ) continue;

		if (pet_param->hp >= pet_param->max_hp && pet_param->mp >= pet_param->max_mp)
		{
			continue;
		}

		pet_param->hp += (int)(pet_param->max_hp * percent);
		pet_param->mp += (int)(pet_param->max_mp * percent);
		pet_param->hp = GetMin(pet_param->hp, pet_param->max_hp);
		pet_param->mp = GetMin(pet_param->mp, pet_param->max_mp);
		this->SetItemDirty(pet_index, true, Protocol::SCSendPetSingleInfo::PET_NOTIE_TYPE_HPMP);
	}

	EventHandler::Instance().OnPetBaseInfoChange(m_role_module_mgr->GetRole(), this->GetFightIndex());
}

void Pet::ClearBeforeSale(int pet_index)
{
	if (this->PetInvalid(pet_index)) return;

	PetParam & pet = m_grid_list[pet_index].param->pet_param;
	pet.skin_id = 0;
	if (pet.HasGodPrint())
	{
		this->PutOffAllPetGodPrintEquip(pet_index);
	}

	this->PetBadgeTakeOff(pet_index);
	this->PutOffAllPetSoulEquip(pet_index);
	
	this->OnAttrPelletRet(pet_index);
	this->OnPetBenedictionReset(pet_index);
	this->OnPetTalentReset(&pet);
	this->ReCalcAttr(pet_index);
	this->SetItemDirty(pet_index, false);
}

bool Pet::SystemTakeBackPet(int pet_index,int pet_id)
{
	if (this->PetInvalid(pet_index)) return false;
	PetParam& pet = m_grid_list[pet_index].param->pet_param;
	if (pet_id != pet.pet_id)
	{
		bool is_dif = false;
		const PetCfg* pet_cfg = LOGIC_CONFIG->GetPetConfig()->GetPetCfg(pet.pet_id);
		if (pet_cfg != NULL)
		{
			if (pet_cfg->id != pet_cfg->prototype_pet_id)
			{
				//提交的是变异
				if (pet_id == pet_cfg->prototype_pet_id)//并且是任务需要的宠物的变异
				{
					is_dif = true;
				}
			}
		}
		if (!is_dif)return false;
	}

	if (this->IsLock(pet_index)) return false;

	if (m_role_module_mgr->GetSmartMountsSystem()->IsPetDomination(m_grid_list[pet_index].item_unique_id))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_SMART_MOUNTS_SYSTEM_PET_DOMINATION);
		return false;
	}

	if (-1 != m_common_param.benediction_pet_index_list[pet_index])
	{
		m_role_module_mgr->NoticeNum(errornum::EN_PET_NOT_OP_BY_BENEDICTION);
		return false;
	}

	PetEquipMentParam pet_equipment;
	pet.GetPetEquip(&pet_equipment);
	if (pet_equipment.pet_equipment_id != 0)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_PET_HSA_EQUIPMENT_ERR);
		return false;
	}

	if (m_role_module_mgr->GetRole()->GetRoleStatusManager()->IsInBattleStatus())
	{
		m_role_module_mgr->NoticeNum(errornum::EN_ROLE_IN_BATTLE);
		return false;
	}

	if (this->IsInPlatfformBattle(m_grid_list[pet_index].item_unique_id, true))
	{
		return false;
	}

	{
		int fight_index = -1;
		int standy_1 = -1;
		int standy_2 = -1;
		int standy_3 = -1;
		int standy_4 = -1;
		int standy_5 = -1;
		this->GetFightIndex(&fight_index,&standy_1,&standy_2, &standy_3, &standy_4, &standy_5);
		if (pet_index == fight_index || pet_index == standy_1 || pet_index == standy_2 || pet_index == standy_3 || pet_index == standy_4 || pet_index == standy_5)
		{
			return false;
		}
	}

	for (int i = 0;i < GUILD_PET_TRAINING_PET_MAX_COUNT;i++)
	{
		GuildTrainingPetParam & guild_pet = m_role_module_mgr->GetGuildCommonData().guild_training_pet_list[i];
		if (guild_pet.pet_index != -1 && guild_pet.pet_index == pet_index)
		{
			return false;
		}
	}

	//宠物排行榜所需
	unsigned long long  pet_unique_id = m_grid_list[pet_index].item_unique_id;

	this->DeletePet(pet_index, __FUNCTION__);
	RankManager::Instance().SyncRemovePet(this->GetRoleModuleManager()->GetRole(), pet_unique_id);

	return true;
}

bool Pet::IsPetTraning(int pet_index)
{
	for (int i = 0; i < GUILD_PET_TRAINING_PET_MAX_COUNT; ++i)
	{
		int training_index = m_role_module_mgr->GetGuildCommonData().guild_training_pet_list[i].pet_index;
		if (pet_index == training_index)
		{
			return true;
		}
	}
	return false;
}

void Pet::SetCurHpMp(int pet_id, int pet_index, int cur_hp, int cur_mp)
{
	PetParam* pet_param = this->__GetPetParam(pet_index);
	if (NULL == pet_param || pet_param->pet_id != pet_id) return;

	pet_param->hp = cur_hp;
	pet_param->hp = GetMin(pet_param->hp, pet_param->max_hp);
	pet_param->hp = GetMax(pet_param->hp, 0);

	pet_param->mp = cur_mp;
	pet_param->mp = GetMin(pet_param->mp, pet_param->max_mp);
	pet_param->mp = GetMax(pet_param->mp, 0);

	//m_pet_base_attr[pet_index].m_attrs[BATTLE_ATTR_CUR_HP] = pet_param->hp;
	//m_pet_base_attr[pet_index].m_attrs[BATTLE_ATTR_CUR_MP] = pet_param->mp;

	this->SetItemDirty(pet_index,true,Protocol::SCSendPetSingleInfo::PET_NOTIE_TYPE_HPMP);

	EventHandler::Instance().OnPetBaseInfoChange(m_role_module_mgr->GetRole(), pet_index);

	return;
}

bool Pet::ChangeHP(int pet_index, int ch_hp, bool is_notic/* = false*/, bool is_percent /* = false*/)
{
	if (this->PetInvalid(pet_index)) return false;

	PetParam& pet = m_grid_list[pet_index].param->pet_param;
	if (pet.hp >= pet.max_hp)
	{
		if(is_notic)m_role_module_mgr->NoticeNum(errornum::EN_ATTR_HAS_FULL_ERR);
		return false;
	}
	if (is_percent)
	{
		ch_hp = (int)(1.0 * pet.max_hp * ch_hp / BATTLE_PERCENT_NUM);
	}

	IntHelpAdd(pet.hp, pet.max_hp, ch_hp);

	this->SendSingleInfo(pet_index,Protocol::SCSendPetSingleInfo::PET_NOTIE_TYPE_HPMP);
	
	EventHandler::Instance().OnPetBaseInfoChange(m_role_module_mgr->GetRole(), pet_index);

	return true;
}

bool Pet::ChangeMp(int pet_index, int ch_mp, bool is_notic/* = false*/, bool is_percent /* = false*/)
{
	if (this->PetInvalid(pet_index)) return false;

	PetParam& pet = m_grid_list[pet_index].param->pet_param;
	if (pet.mp >= pet.max_mp)
	{
		if (is_notic)m_role_module_mgr->NoticeNum(errornum::EN_ATTR_HAS_FULL_ERR);
		return false;
	}
	if (is_percent)
	{
		ch_mp = (int)(1.0 * pet.max_mp * ch_mp / BATTLE_PERCENT_NUM);
	}
	IntHelpAdd(pet.mp, pet.max_mp, ch_mp);

	this->SendSingleInfo(pet_index, Protocol::SCSendPetSingleInfo::PET_NOTIE_TYPE_HPMP);

	EventHandler::Instance().OnPetBaseInfoChange(m_role_module_mgr->GetRole(), pet_index);
	return true;
}

int Pet::GetAttributeList(int pet_index, AttributeList& attr)
{
	return this->GetAttributeListHelper(pet_index, false, attr);
}

int Pet::GetBagAttributeList(int pet_storage_index, AttributeList & attr)
{
	return this->GetAttributeListHelper(pet_storage_index, true, attr);
}

int Pet::GetAttributeListHelper(int pet_index, bool is_storage, AttributeList & attr)
{
	static AttributeList temp_attr; temp_attr.Reset();
	static AttributeList no_skill_attr; no_skill_attr.Reset();

	ItemGridData * src_grid = NULL;
	if (!is_storage)
	{
		if (this->PetInvalid(pet_index)) return -1;
		src_grid = &m_grid_list[pet_index];
	}
	else
	{
		if (this->PetStorageInvalid(pet_index))return-2;
		src_grid = &m_storage_grid_list[pet_index];
	}

	if (src_grid == NULL || src_grid->param == NULL)
	{
		return -3;
	}

	const PetParam & pet = src_grid->param->pet_param;

	std::map<int, int> pet_train_2_map = m_role_module_mgr->GetTrain()->GetPetTrainUp2Map();
	this->GetAttributeListHelper2(pet, temp_attr, no_skill_attr, m_role_module_mgr->GetRole()->GetLevel(), src_grid->item_unique_id, this->GetStrBuffLevel(pet_index), 
		!is_storage && this->IsWaitForFightPet(pet_index), m_role_module_mgr->GetTrain()->GetRolePetTrainSkillLevel(), &pet_train_2_map);

	attr = temp_attr;
	
	if (!is_storage)
	{
		m_pet_attr_no_skill[pet_index] = no_skill_attr;
	}	

	return 0;
}

void AddPetAttr(AttributeList & attr, double(& ARG_OUT pet_attr_attr)[ADD_POINT_TYPE_MAX],const AttrItemCfg * attr_cfg)
{
	if (!attr_cfg)
	{
		return;
	}

	if (attr_cfg->is_pet_attr)
	{
		if (0 <= attr_cfg->attr_type && attr_cfg->attr_type < ARRAY_LENGTH(pet_attr_attr))
		{
			pet_attr_attr[attr_cfg->attr_type] += attr_cfg->attr_value;
		}
	}
	else
	{
		attr.AddAttrCfg(*attr_cfg);
	}
}

template<class T>
void ConvertPetAttrToBattleAttr(AttributeList & ARG_OUT attr_list, T(&pet_attr)[ADD_POINT_TYPE_MAX], int pet_train_level = 0)
{
	double battle_attrs[BATTLE_ATTR_MAX] = { 0 };
	for (int i = 0; i < ARRAY_LENGTH(pet_attr); ++i)
	{
		T & attr = pet_attr[i];
		const PetAttrExchangeCfg::ItemCfg * attr_cfg = LOGIC_CONFIG->GetPetConfig()->GetAttrExchange(pet_train_level, i);
		if (NULL != attr_cfg)
		{
			battle_attrs[BATTLE_ATTR_MAX_HP] += attr * attr_cfg->maxhp;
			battle_attrs[BATTLE_ATTR_MAX_MP] += attr * attr_cfg->maxmp;
			battle_attrs[BATTLE_ATTR_ATTACK] += attr * attr_cfg->gongji;
			battle_attrs[BATTLE_ATTR_DEFENSE] += attr * attr_cfg->fangyu;
			battle_attrs[BATTLE_ATTR_AGILITY] += attr * attr_cfg->minjie;
			battle_attrs[BATTLE_ATTR_RECOVERY] += attr * attr_cfg->huifu;
			battle_attrs[BATTLE_ATTR_MENTAL] += attr * attr_cfg->jingshen;
		}
	}

	int type_list[] = { BATTLE_ATTR_MAX_HP ,BATTLE_ATTR_MAX_MP ,BATTLE_ATTR_ATTACK,BATTLE_ATTR_DEFENSE ,
		BATTLE_ATTR_AGILITY ,BATTLE_ATTR_RECOVERY,BATTLE_ATTR_MENTAL };
	for (int i = 0; i < ARRAY_LENGTH(type_list); ++i)
	{
		int type = type_list[i];
		attr_list.m_attrs[type] = battle_attrs[type] / 100.0 + 1e-6;
	}
}

int Pet::GetAttributeListHelper2(const PetParam & pet, AttributeList & attr, AttributeList & no_skill_attr, int role_level, unsigned long long item_unique_id, int str_buff_level, bool is_wait, 
	int pet_train_level, const std::map<int, int> * pet_train_2_map, bool is_huashen) const
{
	const PetCfg * cfg = LOGIC_CONFIG->GetPetConfig()->GetPetCfg(pet.pet_id);
	if (NULL == cfg) return -2;

	const PetStrengthenCfg * str_cfg = LOGIC_CONFIG->GetPetConfig()->GetPetStrengthenCfg(cfg->strengthen_cfg_id, pet.str_level);
	if (NULL == str_cfg) return -3;

	double total_attr[ADD_POINT_TYPE_MAX] = { 0 };
	// 等级+资质+强化+潜能获得的属性
	for (int i = 0; i < ADD_POINT_TYPE_MAX; ++i)
	{
		// 资质 自身 * 强化提升率
		double growth = pet.growth[i] + str_cfg->add_growth[i] / 100.0;

		// 玩家分配的属性点
		double attr_point = pet.attr[i] * 1.0;

		total_attr[i] += (growth * (PET_ATTR_BASE_UP + (pet.level - 1) * PET_ATTR_LEVEL_UP)) + attr_point;
	}

	double base_attrs[BATTLE_ATTR_MAX] = { 0 };		//基础属性(强化+资质+潜能+突破)(与属性来源对齐)
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

	//-------------------------------------------------------------------------上面是基础属性base_attr_list--------------------------------------------
	std::map<int, std::vector<int> >::const_iterator  iter = m_pet_has_skin_id.find(pet.pet_id);
	if (iter != m_pet_has_skin_id.end())
	{
		for (std::vector<int>::const_iterator skin_id_iter = iter->second.begin(); skin_id_iter != iter->second.end(); ++skin_id_iter)
		{
			//皮肤资质
			const SkinPetCfg * skin_pet_cfg = LOGIC_CONFIG->GetSkinConfig()->GetSkinPetCfgBySkinId(*skin_id_iter);
			if (skin_pet_cfg != NULL)
			{
				total_attr[ADD_POINT_TYPE_VITALITY] += skin_pet_cfg->con;
				total_attr[ADD_POINT_TYPE_STRENGTH] += skin_pet_cfg->str;
				total_attr[ADD_POINT_TYPE_POWER] += skin_pet_cfg->def;
				total_attr[ADD_POINT_TYPE_AGILITY] += skin_pet_cfg->agi;
				total_attr[ADD_POINT_TYPE_MAGIC] += skin_pet_cfg->magic;
			}
		}
	}

	//宠物改命
	for (int i = 0; i < ARRAY_LENGTH(pet.pet_gai_ming_attr_level); ++i)
	{
		int attr_level = pet.pet_gai_ming_attr_level[i];
		const AttrItemCfg * attr_cfg = LOGIC_CONFIG->GetPetGaiMingConfig()->GetAttrItemCfg(i, attr_level);
		::AddPetAttr(attr, total_attr, attr_cfg);
	}

	//////////////////////// 这里计算出主属性提供的副属性////////////////////////////////////////////
	double temp_attrs[BATTLE_ATTR_MAX] = { 0 };

	//加点副属性
	for (int i = 0; i < ADD_POINT_TYPE_MAX; ++i)
	{
		const PetAttrExchangeCfg::ItemCfg * attr_cfg = LOGIC_CONFIG->GetPetConfig()->GetAttrExchange(pet_train_level, i);
		if (NULL != attr_cfg)
		{
			// 先算整数 全部计算完再除 100 不然会有误差 每次零点几 积累起来就有 1 了
			// 血蓝
			temp_attrs[BATTLE_ATTR_MAX_HP] += total_attr[i] * attr_cfg->maxhp;
			temp_attrs[BATTLE_ATTR_MAX_MP] += total_attr[i] * attr_cfg->maxmp;
			// 基础属性
			temp_attrs[BATTLE_ATTR_ATTACK] += total_attr[i] * attr_cfg->gongji;
			temp_attrs[BATTLE_ATTR_DEFENSE] += total_attr[i] * attr_cfg->fangyu;
			temp_attrs[BATTLE_ATTR_AGILITY] += total_attr[i] * attr_cfg->minjie;
			temp_attrs[BATTLE_ATTR_RECOVERY] += total_attr[i] * attr_cfg->huifu;
			temp_attrs[BATTLE_ATTR_MENTAL] += total_attr[i] * attr_cfg->jingshen;
		}
	}

	if (pet_train_2_map != NULL)
	{
		for (std::map<int, int>::const_iterator iter = pet_train_2_map->begin(); iter != pet_train_2_map->end(); ++iter)
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
					attr.m_attrs[attr_type] += train_cfg->attribute_value;
				}
			}
		}
	}

	if (pet_break_attr_cfg != NULL)
	{
		for (int i = 0; i < ADD_POINT_TYPE_MAX; ++i)
		{
			std::map<int, PetBreakAttrCfg::AttrItem>::const_iterator break_attr_iter = pet_break_attr_cfg->attr_item.find(i);
			if (break_attr_iter != pet_break_attr_cfg->attr_item.end())
			{
				const PetBreakAttrCfg::AttrItem & break_attr = break_attr_iter->second;
				// 血蓝
				temp_attrs[BATTLE_ATTR_MAX_HP] += total_attr[i] * break_attr.maxhp;
				temp_attrs[BATTLE_ATTR_MAX_MP] += total_attr[i] * break_attr.maxmp;
				// 基础属性
				temp_attrs[BATTLE_ATTR_ATTACK] += total_attr[i] * break_attr.gongji;
				temp_attrs[BATTLE_ATTR_DEFENSE] += total_attr[i] * break_attr.fangyu;
				temp_attrs[BATTLE_ATTR_AGILITY] += total_attr[i] * break_attr.minjie;
				temp_attrs[BATTLE_ATTR_RECOVERY] += total_attr[i] * break_attr.huifu;
				temp_attrs[BATTLE_ATTR_MENTAL] += total_attr[i] * break_attr.jingshen;
			}
		}
	}

	//////////////////////// 总副属性 = 基础属性 + 主属性提供的副属性////////////////////////////////////////////
	// 血蓝
	attr.m_attrs[BATTLE_ATTR_MAX_HP] += (Attribute)(cfg->maxhp + str_cfg->maxhp + (temp_attrs[BATTLE_ATTR_MAX_HP] / 100.0 + 1e-6));
	attr.m_attrs[BATTLE_ATTR_MAX_MP] += (Attribute)(cfg->maxmp + str_cfg->maxmp + (temp_attrs[BATTLE_ATTR_MAX_MP] / 100.0 + 1e-6));
	// 基础属性
	attr.m_attrs[BATTLE_ATTR_ATTACK] += (Attribute)(cfg->gongji + str_cfg->gongji + (temp_attrs[BATTLE_ATTR_ATTACK] / 100.0 + 1e-6));
	attr.m_attrs[BATTLE_ATTR_DEFENSE] += (Attribute)(cfg->fangyu + str_cfg->fangyu + (temp_attrs[BATTLE_ATTR_DEFENSE] / 100.0 + 1e-6));
	attr.m_attrs[BATTLE_ATTR_AGILITY] += (Attribute)(cfg->minjie + str_cfg->minjie + (temp_attrs[BATTLE_ATTR_AGILITY] / 100.0 + 1e-6));
	attr.m_attrs[BATTLE_ATTR_RECOVERY] += (Attribute)(cfg->huifu + str_cfg->huifu + (temp_attrs[BATTLE_ATTR_RECOVERY] / 100.0 + 1e-6));
	attr.m_attrs[BATTLE_ATTR_MENTAL] += (Attribute)(cfg->jingshen + str_cfg->jingshen + (temp_attrs[BATTLE_ATTR_MENTAL] / 100.0 + 1e-6));
	// 修正属性
	attr.m_attrs[BATTLE_ATTR_CRITICAL] += (Attribute)(cfg->bisha);
	attr.m_attrs[BATTLE_ATTR_HIT] += (Attribute)(cfg->mingzhong);
	attr.m_attrs[BATTLE_ATTR_DODGE] += (Attribute)(cfg->shanduo);
	attr.m_attrs[BATTLE_ATTR_COUNTER_ATTACK] += (Attribute)(cfg->fanji);
	attr.m_attrs[BATTLE_ATTR_MAGIC_ATTACK] += (Attribute)(cfg->mogong);
	attr.m_attrs[BATTLE_ATTR_MAGIC_DEFENSE] += (Attribute)(cfg->mokang);
	attr.m_attrs[BATTLE_ATTR_CRITICAL_INC_VALUE] += (Attribute)(cfg->bisha_dmg);
	attr.m_attrs[BATTLE_ATTR_CRITICAL_DEC_VALUE] += (Attribute)(cfg->jianren);
	// 异常抗性
	attr.m_attrs[BATTLE_ATTR_ANTI_POISON] += (Attribute)(cfg->def_zhongdu);
	attr.m_attrs[BATTLE_ATTR_ANTI_FREEZE] += (Attribute)(cfg->def_hunshui);
	attr.m_attrs[BATTLE_ATTR_ANTI_PETRIFY] += (Attribute)(cfg->def_shihua);
	attr.m_attrs[BATTLE_ATTR_ANTI_CHAOS] += (Attribute)(cfg->def_hunluan);
	attr.m_attrs[BATTLE_ATTR_ANTI_DRUNK] += (Attribute)(cfg->def_jiuzui);
	// 元素
	attr.m_attrs[BATTLE_ATTR_ELEMENT_WATER] += (Attribute)(cfg->element_water);
	attr.m_attrs[BATTLE_ATTR_ELEMENT_FIRE] += (Attribute)(cfg->element_fire);
	attr.m_attrs[BATTLE_ATTR_ELEMENT_WIND] += (Attribute)(cfg->element_wind);
	attr.m_attrs[BATTLE_ATTR_ELEMENT_EARTH] += (Attribute)(cfg->element_land);

	if (cfg->magic_pet_extra_attr_type == 1)
	{
		attr.m_attrs[BATTLE_ATTR_MAGIC_ATTACK] += (int)(1.0 * pet.level * 3.5);
	}

	if (cfg->magic_pet_extra_attr_type == 2)
	{
		attr.m_attrs[BATTLE_ATTR_MAGIC_ATTACK] += (int)(1.0 * pet.level * 2.1);
	}

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
		GetSpecialEffectAttr(spid, attr, base_attr_list, role_level, pet.level);
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
			GetSpecialEffectAttr(spid, attr, base_attr_list, role_level, pet.level);
		}
	}
	
	//被统御的宠物,可以加上该灵骑的基础属性
	m_role_module_mgr->GetSmartMountsSystem()->GetDominationBaseAttr(item_unique_id, attr);
	this->OnCalcPetBenedictioncAttr(item_unique_id, attr, is_huashen);

	if (is_wait)		//法宝提供宠物属性 - 不算入宠物自身提升范围内
	{
		FaBao * fabao_manager = m_role_module_mgr->GetFaBao();
		for (int i = 0; i < MAX_FABAO_COUNT; ++i)
		{
			if (!fabao_manager->IsActiveFaBao(i)) continue;

			const FaBaoEquip * equip_ptr = fabao_manager->GetFaBaoEquip(i);
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

				attr.m_attrs[cfg->effect] += add_value;
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

				attr.m_attrs[cfg->effect] += add_value;
			}
		}
	}

	//计算魂器赋予属性
	for (int pet_soul_equipment_index = 0; pet_soul_equipment_index < MAX_PET_SOUL_EQUIP_LIST; ++pet_soul_equipment_index)
	{
		const PetSoulEquip * soul_equip_ptr = pet.GetPetSoulEquipInPetParamConst(pet_soul_equipment_index);
		if (NULL == soul_equip_ptr) continue;
		const PetSoulEquip & soul_equip = *soul_equip_ptr;
		if (soul_equip.Invalid())
		{
			continue;
		}

		for (int soul_idx = 0; soul_idx < MAX_PET_SOUL_EQUIPMENT_ATTR_NUM && soul_idx < soul_equip.attr_num; soul_idx++)
		{
			if (soul_equip.add_attr_list[soul_idx].attr_type < 0 || soul_equip.add_attr_list[soul_idx].attr_type >= BATTLE_ATTR_MAX)continue;
			attr.m_attrs[soul_equip.add_attr_list[soul_idx].attr_type] += soul_equip.add_attr_list[soul_idx].attr_value;
		}

		const ItemBase * soul_base = ITEMPOOL->GetItem(soul_equip.item_id);
		if (soul_base != NULL && soul_base->GetItemType() == I_TYPE_PET_SOUL_EQUIP)
		{
			const PetSoulEquipment * pet_soul = (const PetSoulEquipment *)soul_base;
			const PetSoulEquipBaseCfg * base_cfg = LOGIC_CONFIG->GetPetSoulEquipMentConfig()->GetPetSoulEquipBaseCfg(pet_soul->GetPetWeaponSeq());
			if (base_cfg != NULL)
			{
				if (base_cfg->pet_weapon_pet_id <= 0 || cfg->prototype_pet_id == base_cfg->pet_weapon_pet_id)
				{
					GetSpecialEffectAttr(soul_equip.sp_id, attr, base_attr_list, role_level, pet.level);
				}
				const PetEquipSoulRefineCfg * refine_cfg = LOGIC_CONFIG->GetPetSoulEquipMentConfig()->GetPetSoulEquipRefineCfg(base_cfg->refine_group, soul_equip.m_refine);
				if (refine_cfg != NULL)
				{
					if (refine_cfg->attr_type_1 >= 0 && refine_cfg->attr_type_1 <= BATTLE_ATTR_MAX && refine_cfg->attr_value_1 > 0)
					{
						attr.m_attrs[refine_cfg->attr_type_1] += refine_cfg->attr_value_1;
					}
					if (refine_cfg->attr_type_2 >= 0 && refine_cfg->attr_type_2 <= BATTLE_ATTR_MAX && refine_cfg->attr_value_2 > 0)
					{
						attr.m_attrs[refine_cfg->attr_type_2] += refine_cfg->attr_value_2;
					}
				}
			}
		}
		
	}
	int suit_num = 0;
	SuitEffectReCalc equip_suit_list[GOD_PRINT_INDEX_TYPE_MAX];

	for (int god_print_idx = 0; god_print_idx < GOD_PRINT_INDEX_TYPE_MAX; god_print_idx++)
	{
		int bag_idx = pet.god_print_list[god_print_idx];
		if (bag_idx > 0 && bag_idx <= NEW_MAX_PET_GOD_PRONT_BAG_NUM)
		{
			if (NULL == m_new_god_print_bag[bag_idx - 1])continue;
			if (m_new_god_print_bag[bag_idx - 1]->Invalid())continue;
			const GodPrintItem & node = *m_new_god_print_bag[bag_idx - 1];
			//有神印
			if (node.attr_num > 0)
			{
				if (node.main_attr.attr_value > 0)attr.m_attrs[node.main_attr.attr_type] += node.main_attr.attr_value;
				for (int attr_idx = 0; attr_idx < node.attr_num && attr_idx < MAX_PET_GOD_PRONT_ATTR_NUM; attr_idx++)
				{
					attr.m_attrs[node.add_attr_list[attr_idx].attr_type] += node.add_attr_list[attr_idx].attr_value;
				}
			}
			if (node.devour_num > 0)
			{
				if (node.devour_attr_list.attr_value > 0)attr.m_attrs[node.devour_attr_list.attr_type] += node.devour_attr_list.attr_value;
			}
			if (node.suit_id > 0 && suit_num < GOD_PRINT_INDEX_TYPE_MAX)
			{
				bool has_suit_add = false;
				for (int suit_idx = 0; suit_idx < suit_num && suit_idx < MAX_SUIT_TYPE; suit_idx++)//先检查是否有同类组合，件数堆叠
				{
					SuitEffectReCalc &node_cale = equip_suit_list[suit_idx];
					if (node_cale.sp_id == node.suit_id)
					{
						//检查多件套中最高品质
						bool has_insert = false;
						for (int suit_num_begin = 0; suit_num_begin < node_cale.num; suit_num_begin++)
						{
							if ((int)node_cale.color_top.size() <= suit_num_begin)continue;
							if (node_cale.color_top[suit_num_begin] < (int)node.color)
							{
								std::vector<int>::iterator it = node_cale.color_top.begin();
								node_cale.color_top.insert(it + suit_num_begin, (int)node.color);
								has_insert = true;
								break;
							}
						}
						if (!has_insert)
						{
							node_cale.color_top.push_back((int)node.color);
						}

						node_cale.num += 1;
						has_suit_add = true;
						break;
					}
				}
				if (!has_suit_add)
				{
					equip_suit_list[suit_num].sp_id = node.suit_id;
					equip_suit_list[suit_num].color_top.push_back((int)node.color);
					equip_suit_list[suit_num++].num = 1;
				}
				/*bool is_new = true;
				for (int i = 0; i < GOD_PRINT_INDEX_TYPE_MAX && i < suit_num; i++)
				{
					if (suit_id_list[i] == node.suit_id)
					{
						suit_id_num[i] += 1;
						is_new = false;
						break;;
					}
				}
				if (is_new)
				{
					suit_id_num[suit_num] += 1;
					suit_id_list[suit_num++] = node.suit_id;
				}*/
			}
		}
	}

	if (suit_num > 0)
	{
		for (int i = 0; i < suit_num && i < GOD_PRINT_INDEX_TYPE_MAX; i++)
		{
			SuitEffectReCalc & suit_node = equip_suit_list[i];
			{
				for (int suit_num_idx = 0; suit_num_idx < GOD_PRINT_INDEX_TYPE_MAX && suit_num_idx < suit_node.num && suit_num_idx < (int)suit_node.color_top.size(); suit_num_idx++)
				{
					const PetGodPrintEquipMentSuitCfg * suit_list_cfg = LOGIC_CONFIG->GetPetGodPrintEquipMentConfig()->GetSuitListCfg(suit_node.sp_id, suit_node.color_top[suit_num_idx], suit_num_idx);
					if (NULL != suit_list_cfg)
					{
						switch (suit_list_cfg->effect_type)
						{
						case PetGodPrintEquipMentSuitCfg::EFFECT_TYPE_ATTR:
							if (suit_list_cfg->param1 < 0 || suit_list_cfg->param1 >= BATTLE_ATTR_MAX)continue;
							attr.m_attrs[suit_list_cfg->param1] += suit_list_cfg->param2;
							break;
						case PetGodPrintEquipMentSuitCfg::EFFECT_TYPE_SP_ID:
							{
								GetSpecialEffectAttr(suit_list_cfg->param2, attr, base_attr_list, role_level, pet.level);
							}
							break;
						default:
							break;
						}
					}
				}
			}
		}
	}

	// 修为 看上去应该是加在这里的 ,强烈建议宠物的属性计算规整一下 . 人都看懵逼了
	m_role_module_mgr->GetXiuWei()->ReCalcPetAttr(attr, pet.level);

	// 圣器
	m_role_module_mgr->GetHolyEquipManager()->ReCalcPetAttr(item_unique_id, attr);
	// 星图
	m_role_module_mgr->GetStarChart()->ReCalcPetAttr(attr);

	//-----------------------------------额外计算评分的放这下面------------------------------
	no_skill_attr = attr;
	PetEffectSkillId skill_list = this->GetPetEffectiveSkillHelper(pet);
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
						attr.m_attrs[attr_type] += attr_value;
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
		GetSpecialEffectAttr(spid, attr, base_attr_list, role_level, pet.level);
	}

	std::vector<SkillPro> other_sill_list;
	m_role_module_mgr->GetSmartMountsSystem()->GetBaseSkillList(item_unique_id, &other_sill_list);
	int smart_mouns_skill_count = (int)other_sill_list.size();
	AttributeList guild_inborn_attr;
	m_role_module_mgr->GetRoleGuildInborn()->GetSkillList(&other_sill_list, GUILD_INBORN_ADD_TARGET_TYPE_PET);
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
		if (i >= smart_mouns_skill_count)
		{
			//灵脉属性
			GetSpecialEffectAttr(spid, attr, base_attr_list, role_level, pet.level, &guild_inborn_attr);
		}
		else
		{
			//灵骑属性
			GetSpecialEffectAttr(spid, attr, base_attr_list, role_level, pet.level);
		}
	}
	//家族灵脉可能会配5项基础属性
	AttributePlan tmp_ap;
	for (int i = 0, k = BATTLE_ADD_POINT_TYPE_BEGIN; i < ADD_POINT_TYPE_MAX && k <= BATTLE_ADD_POINT_TYPE_END; i++, k++)
	{
		tmp_ap.add_point_list[i] += guild_inborn_attr.m_attrs[k];
	}
	LOGIC_CONFIG->GetRoleExpConfig()->ConvertPointToAttribute(tmp_ap, attr);

	//宠物祝福
	int pet_benediction_sp_id = this->GetPetBenedictionSpId(pet, item_unique_id);
	GetSpecialEffectAttr(pet_benediction_sp_id, attr, base_attr_list, role_level, pet.level);

	//宠物天赋
	std::vector<int> talent_sp_list;
	this->GetPetTalentSpIdList(pet, talent_sp_list);
	GetSpecialEffectAttr(talent_sp_list, attr, base_attr_list, role_level, pet.level);

	//属性丹
	for (int i = 0; i < ATTR_PELLET_TYPE_MAX && i < ARRAY_LENGTH(pet.use_attr_pellet_list); ++i)
	{
		if(pet.use_attr_pellet_list[i] <= 0) continue;
		
		int attr_type = GetAttrTypeByPelletType(i);
		if (attr_type < 0 || attr_type >= BATTLE_ATTR_MAX) continue;

		int add_times = LOGIC_CONFIG->GetPetConfig()->GetFeiShengShuXingDanUseTimes(pet.fei_sheng_times);
		int max_count = GetMin(LOGIC_CONFIG->GetRoleAttrPelletConfig()->GetOtherCfg().use_limit + add_times, (int)pet.use_attr_pellet_list[i]);
		const RoleAttrPelletAttrListCfg * attr_pellet_cfg = LOGIC_CONFIG->GetRoleAttrPelletConfig()->GetAttrPelletCfg(ROLE_ATTR_PELLET_TYPE_PET, cfg->main_attribute_id, attr_type);
		if (NULL != attr_pellet_cfg)
		{
			attr.m_attrs[attr_type] += attr_pellet_cfg->attr_value * max_count;
			no_skill_attr.m_attrs[attr_type] += attr_pellet_cfg->attr_value * max_count;
		}
	}

	//宠物飞升
	for (int fei_sheng_task_jie_duan = 1; fei_sheng_task_jie_duan <= pet.fei_sheng_task_jie_duan; ++fei_sheng_task_jie_duan)
	{
		const PetFeiShengAttrCfg * pet_fei_sheng_attr_cfg = LOGIC_CONFIG->GetPetConfig()->GetPetFeiShengAttrCfg(pet.pet_id, fei_sheng_task_jie_duan);
		if (pet_fei_sheng_attr_cfg != NULL)
		{
			for (int i = 0; i < (int)pet_fei_sheng_attr_cfg->attr_list.size(); i++)
			{
				const AttrItemCfg & curr = pet_fei_sheng_attr_cfg->attr_list[i];
				if (curr.attr_type < 0 || curr.attr_type >= BATTLE_ATTR_MAX)
				{
					continue;
				}

				attr.m_attrs[curr.attr_type] += curr.attr_value;
				no_skill_attr.m_attrs[curr.attr_type] += curr.attr_value;
			}
		}
	}

	for (int fei_sheng_task_jie_times = 1; fei_sheng_task_jie_times <= pet.fei_sheng_times; ++fei_sheng_task_jie_times)
	{
		const PetFeiShengTimesAttrCfg * pet_fei_sheng_attr_cfg = LOGIC_CONFIG->GetPetConfig()->GetPetFeiShengTimesAttrCfg(pet.pet_id, fei_sheng_task_jie_times);
		if (pet_fei_sheng_attr_cfg != NULL)
		{
			for (int i = 0; i < (int)pet_fei_sheng_attr_cfg->attr_list.size(); i++)
			{
				const AttrItemCfg & curr = pet_fei_sheng_attr_cfg->attr_list[i];
				if (curr.attr_type < 0 || curr.attr_type >= BATTLE_ATTR_MAX)
				{
					continue;
				}

				attr.m_attrs[curr.attr_type] += curr.attr_value;
				no_skill_attr.m_attrs[curr.attr_type] += curr.attr_value;
			}
		}
	}

	//宠物徽章
	const PetBadgeItemParam & badge_item_param = pet.pet_bage_item;
	if (badge_item_param.pet_badgei_item_id != 0)
	{
		for (int i = 0; i < ARRAY_LENGTH(badge_item_param.attrs); i++)
		{
			const AttrInfo & curr = badge_item_param.attrs[i];
			if (curr.attr_type < 0 || curr.attr_type >= BATTLE_ATTR_MAX)
			{
				continue;
			}

			attr.m_attrs[curr.attr_type] += curr.attr_value;
			no_skill_attr.m_attrs[curr.attr_type] += curr.attr_value;
		}
	}

	//宠物强化共鸣
	const PetStrengthenBuffItemCfg * str_buff_cfg = LOGIC_CONFIG->GetPetConfig()->GetPetStrengthenBuffCfg(cfg->quality, str_buff_level);
	if (str_buff_cfg)
	{
		attr.AddAttrCfg(str_buff_cfg->attr_cfg);
		no_skill_attr.AddAttrCfg(str_buff_cfg->attr_cfg);
		
		for (int sp_id_num = 0; sp_id_num < (int)str_buff_cfg->sp_id_list.size(); ++sp_id_num)
		{
			int sp_id = str_buff_cfg->sp_id_list[sp_id_num];
			GetSpecialEffectAttr(sp_id, attr, base_attr_list, role_level, pet.level);
		}
	}

	return 0;
}

int Pet::GetAttrListReason(int pet_index)
{
#ifdef _DEBUG
	PerformanceCatch::Instance().GetElapseUsBegin(PERFORMANCE_TEST_PET_TIME_RECALC);
#endif

	if (this->PetInvalid(pet_index)) return -1;
	PetParam& pet = m_grid_list[pet_index].param->pet_param;
	const PetCfg *cfg = LOGIC_CONFIG->GetPetConfig()->GetPetCfg(pet.pet_id);
	if (NULL == cfg) return -2;

	const PetStrengthenCfg * str_cfg = LOGIC_CONFIG->GetPetConfig()->GetPetStrengthenCfg(cfg->strengthen_cfg_id, pet.str_level);
	if (NULL == str_cfg) return -3;

	int str_buff_level = this->GetStrBuffLevel(pet_index);

	int role_level = m_role_module_mgr->GetRole()->GetLevel();
	//初始化
	for (int i = 0; i < MAX_REASON_TYPE_LIST_NUM; i++)
	{
		m_reason_list[i].Reset();
	}

	double total_attr[ADD_POINT_TYPE_MAX] = { 0 };
	// 等级+资质+强化+潜能获得的属性
	for (int i = 0; i < ADD_POINT_TYPE_MAX; ++i)
	{
		// 资质 自身 * 强化提升率
		double growth = pet.growth[i] + str_cfg->add_growth[i] / 100.0;

		// 玩家分配的属性点
		double attr_point = pet.attr[i] * 1.0;

		total_attr[i] += (growth * (PET_ATTR_BASE_UP + (pet.level - 1) * PET_ATTR_LEVEL_UP)) + attr_point;
	}


	AttributeList base_attr_list;					//基础属性(强化+资质+基础+潜能+突破)
	//宠物改命
	for (int i = 0; i < ARRAY_LENGTH(pet.pet_gai_ming_attr_level); ++i)
	{
		int attr_level = pet.pet_gai_ming_attr_level[i];
		const AttrItemCfg * attr_cfg = LOGIC_CONFIG->GetPetGaiMingConfig()->GetAttrItemCfg(i, attr_level);
		::AddPetAttr(base_attr_list, total_attr, attr_cfg);
	}

	double base_attrs[BATTLE_ATTR_MAX] = { 0 };		//基础属性(强化+资质+潜能+突破)(与属性来源对齐)
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

	if (cfg->magic_pet_extra_attr_type == 1)
	{
		base_attr_list.m_attrs[BATTLE_ATTR_MAGIC_ATTACK] += (int)(1.0 * pet.level * 3.5);
	}

	if (cfg->magic_pet_extra_attr_type == 2)
	{
		base_attr_list.m_attrs[BATTLE_ATTR_MAGIC_ATTACK] += (int)(1.0 * pet.level * 2.1);
	}

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
	if (m_role_module_mgr->GetTrain()->GetRolePetTrainSkillLevel() > 0)
	{
		double temp_attrs[BATTLE_ATTR_MAX] = { 0 };
		for (int i = 0; i < ADD_POINT_TYPE_MAX; ++i)
		{
			const PetAttrExchangeCfg::ItemCfg* attr_cfg = LOGIC_CONFIG->GetPetConfig()->GetAttrExchange(m_role_module_mgr->GetTrain()->GetRolePetTrainSkillLevel(), i);
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
		m_reason_list[PET_REASON_LIST_TYPE_TRAIN].m_attrs[BATTLE_ATTR_MAX_HP] = static_cast<Attribute>(temp_attrs[BATTLE_ATTR_MAX_HP] / 100.0 + 1e-6);
		m_reason_list[PET_REASON_LIST_TYPE_TRAIN].m_attrs[BATTLE_ATTR_MAX_MP] = static_cast<Attribute>(temp_attrs[BATTLE_ATTR_MAX_MP] / 100.0 + 1e-6);

		m_reason_list[PET_REASON_LIST_TYPE_TRAIN].m_attrs[BATTLE_ATTR_ATTACK] = static_cast<Attribute>(temp_attrs[BATTLE_ATTR_ATTACK] / 100.0 + 1e-6);
		m_reason_list[PET_REASON_LIST_TYPE_TRAIN].m_attrs[BATTLE_ATTR_DEFENSE] = static_cast<Attribute>(temp_attrs[BATTLE_ATTR_DEFENSE] / 100.0 + 1e-6);
		m_reason_list[PET_REASON_LIST_TYPE_TRAIN].m_attrs[BATTLE_ATTR_AGILITY] = static_cast<Attribute>(temp_attrs[BATTLE_ATTR_AGILITY] / 100.0 + 1e-6);
		m_reason_list[PET_REASON_LIST_TYPE_TRAIN].m_attrs[BATTLE_ATTR_RECOVERY] = static_cast<Attribute>(temp_attrs[BATTLE_ATTR_RECOVERY] / 100.0 + 1e-6);
		m_reason_list[PET_REASON_LIST_TYPE_TRAIN].m_attrs[BATTLE_ATTR_MENTAL] = static_cast<Attribute>(temp_attrs[BATTLE_ATTR_MENTAL] / 100.0 + 1e-6);
	}

	// 修为 看上去应该是加在这里的 ,强烈建议宠物的属性计算规整一下 . 人都看懵逼了
	m_role_module_mgr->GetXiuWei()->ReCalcPetAttr(m_reason_list[PET_REASON_LIST_TYPE_XIU_WEI], pet.level);

	//圣器
	m_role_module_mgr->GetHolyEquipManager()->ReCalcPetAttr(m_grid_list[pet_index].item_unique_id, m_reason_list[PET_REASON_LIST_TYPE_HOLY_EQUIP]);
	// 星图
	m_role_module_mgr->GetStarChart()->ReCalcPetAttr(m_reason_list[PET_REASON_LIST_TYPE_STAR_CHART]);

	std::map<int, int> pet_train_2_map = m_role_module_mgr->GetTrain()->GetPetTrainUp2Map();
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
				m_reason_list[PET_REASON_LIST_TYPE_TRAIN].m_attrs[attr_type] += train_cfg->attribute_value;
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
				m_reason_list[PET_REASON_LIST_TYPE_TRAIN].m_attrs[attr_type] -= train_cfg_1->attribute_value;
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
	m_reason_list[PET_REASON_LIST_TYPE_BASE].m_attrs[BATTLE_ATTR_MAX_HP] += (Attribute)(cfg->maxhp);
	m_reason_list[PET_REASON_LIST_TYPE_BASE].m_attrs[BATTLE_ATTR_MAX_MP] += (Attribute)(cfg->maxmp);
	// 基础属性
	m_reason_list[PET_REASON_LIST_TYPE_BASE].m_attrs[BATTLE_ATTR_ATTACK] += (Attribute)(cfg->gongji);
	m_reason_list[PET_REASON_LIST_TYPE_BASE].m_attrs[BATTLE_ATTR_DEFENSE] += (Attribute)(cfg->fangyu);
	m_reason_list[PET_REASON_LIST_TYPE_BASE].m_attrs[BATTLE_ATTR_AGILITY] += (Attribute)(cfg->minjie);
	m_reason_list[PET_REASON_LIST_TYPE_BASE].m_attrs[BATTLE_ATTR_RECOVERY] += (Attribute)(cfg->huifu);
	m_reason_list[PET_REASON_LIST_TYPE_BASE].m_attrs[BATTLE_ATTR_MENTAL] += (Attribute)(cfg->jingshen);
	// 修正属性
	m_reason_list[PET_REASON_LIST_TYPE_BASE].m_attrs[BATTLE_ATTR_CRITICAL] += (Attribute)(cfg->bisha);
	m_reason_list[PET_REASON_LIST_TYPE_BASE].m_attrs[BATTLE_ATTR_HIT] += (Attribute)(cfg->mingzhong);
	m_reason_list[PET_REASON_LIST_TYPE_BASE].m_attrs[BATTLE_ATTR_DODGE] += (Attribute)(cfg->shanduo);
	m_reason_list[PET_REASON_LIST_TYPE_BASE].m_attrs[BATTLE_ATTR_COUNTER_ATTACK] += (Attribute)(cfg->fanji);
	m_reason_list[PET_REASON_LIST_TYPE_BASE].m_attrs[BATTLE_ATTR_MAGIC_ATTACK] += (Attribute)(cfg->mogong);
	m_reason_list[PET_REASON_LIST_TYPE_BASE].m_attrs[BATTLE_ATTR_MAGIC_DEFENSE] += (Attribute)(cfg->mokang);
	m_reason_list[PET_REASON_LIST_TYPE_BASE].m_attrs[BATTLE_ATTR_CRITICAL_INC_VALUE] += (Attribute)(cfg->bisha_dmg);
	m_reason_list[PET_REASON_LIST_TYPE_BASE].m_attrs[BATTLE_ATTR_CRITICAL_DEC_VALUE] += (Attribute)(cfg->jianren);
	// 异常抗性
	m_reason_list[PET_REASON_LIST_TYPE_BASE].m_attrs[BATTLE_ATTR_ANTI_POISON] += (Attribute)(cfg->def_zhongdu);
	m_reason_list[PET_REASON_LIST_TYPE_BASE].m_attrs[BATTLE_ATTR_ANTI_FREEZE] += (Attribute)(cfg->def_hunshui);
	m_reason_list[PET_REASON_LIST_TYPE_BASE].m_attrs[BATTLE_ATTR_ANTI_PETRIFY] += (Attribute)(cfg->def_shihua);
	m_reason_list[PET_REASON_LIST_TYPE_BASE].m_attrs[BATTLE_ATTR_ANTI_CHAOS] += (Attribute)(cfg->def_hunluan);
	m_reason_list[PET_REASON_LIST_TYPE_BASE].m_attrs[BATTLE_ATTR_ANTI_DRUNK] += (Attribute)(cfg->def_jiuzui);
	// 元素
	m_reason_list[PET_REASON_LIST_TYPE_BASE].m_attrs[BATTLE_ATTR_ELEMENT_WATER] += (Attribute)(cfg->element_water);
	m_reason_list[PET_REASON_LIST_TYPE_BASE].m_attrs[BATTLE_ATTR_ELEMENT_FIRE] += (Attribute)(cfg->element_fire);
	m_reason_list[PET_REASON_LIST_TYPE_BASE].m_attrs[BATTLE_ATTR_ELEMENT_WIND] += (Attribute)(cfg->element_wind);
	m_reason_list[PET_REASON_LIST_TYPE_BASE].m_attrs[BATTLE_ATTR_ELEMENT_EARTH] += (Attribute)(cfg->element_land);
	
	double point_attrs[BATTLE_ATTR_MAX] = { 0 };
	double other_attrs[BATTLE_ATTR_MAX] = { 0 };
	double str_attrs[BATTLE_ATTR_MAX] = { 0 };

	//宠物改命
	double gai_ming_no_battle_attr[ADD_POINT_TYPE_MAX] = { 0 };
	for (int i = 0; i < ARRAY_LENGTH(pet.pet_gai_ming_attr_level); ++i)
	{
		int attr_level = pet.pet_gai_ming_attr_level[i];
		const AttrItemCfg * attr_cfg = LOGIC_CONFIG->GetPetGaiMingConfig()->GetAttrItemCfg(i, attr_level);
		::AddPetAttr(m_reason_list[PET_REASON_LIST_TYPE_GAI_MING], gai_ming_no_battle_attr, attr_cfg);		
	}
	::ConvertPetAttrToBattleAttr(m_reason_list[PET_REASON_LIST_TYPE_GAI_MING], gai_ming_no_battle_attr);

	//资质 / 潜能 / 其他(皮肤) / 强化 
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
			point_attrs[BATTLE_ATTR_MAX_MP] +=pet.attr[i] * attr_cfg->maxmp;
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
	m_reason_list[PET_REASON_LIST_TYPE_GROWTH].m_attrs[BATTLE_ATTR_MAX_HP] += static_cast<Attribute>(growth_attrs[BATTLE_ATTR_MAX_HP] / 100.0 + 1e-6);
	m_reason_list[PET_REASON_LIST_TYPE_GROWTH].m_attrs[BATTLE_ATTR_MAX_MP] += static_cast<Attribute>(growth_attrs[BATTLE_ATTR_MAX_MP] / 100.0 + 1e-6);
	m_reason_list[PET_REASON_LIST_TYPE_GROWTH].m_attrs[BATTLE_ATTR_ATTACK] += static_cast<Attribute>(growth_attrs[BATTLE_ATTR_ATTACK] / 100.0 + 1e-6);
	m_reason_list[PET_REASON_LIST_TYPE_GROWTH].m_attrs[BATTLE_ATTR_DEFENSE] += static_cast<Attribute>(growth_attrs[BATTLE_ATTR_DEFENSE] / 100.0 + 1e-6);
	m_reason_list[PET_REASON_LIST_TYPE_GROWTH].m_attrs[BATTLE_ATTR_AGILITY] += static_cast<Attribute>(growth_attrs[BATTLE_ATTR_AGILITY] / 100.0 + 1e-6);
	m_reason_list[PET_REASON_LIST_TYPE_GROWTH].m_attrs[BATTLE_ATTR_RECOVERY] += static_cast<Attribute>(growth_attrs[BATTLE_ATTR_RECOVERY] / 100.0 + 1e-6);
	m_reason_list[PET_REASON_LIST_TYPE_GROWTH].m_attrs[BATTLE_ATTR_MENTAL] += static_cast<Attribute>(growth_attrs[BATTLE_ATTR_MENTAL] / 100.0 + 1e-6);
	
	//潜能
	m_reason_list[PET_REASON_LIST_TYPE_POINT].m_attrs[BATTLE_ATTR_MAX_HP] += static_cast<Attribute>(point_attrs[BATTLE_ATTR_MAX_HP] / 100.0 + 1e-6);
	m_reason_list[PET_REASON_LIST_TYPE_POINT].m_attrs[BATTLE_ATTR_MAX_MP] += static_cast<Attribute>(point_attrs[BATTLE_ATTR_MAX_MP] / 100.0 + 1e-6);
	m_reason_list[PET_REASON_LIST_TYPE_POINT].m_attrs[BATTLE_ATTR_ATTACK] += static_cast<Attribute>(point_attrs[BATTLE_ATTR_ATTACK] / 100.0 + 1e-6);
	m_reason_list[PET_REASON_LIST_TYPE_POINT].m_attrs[BATTLE_ATTR_DEFENSE] += static_cast<Attribute>(point_attrs[BATTLE_ATTR_DEFENSE] / 100.0 + 1e-6);
	m_reason_list[PET_REASON_LIST_TYPE_POINT].m_attrs[BATTLE_ATTR_AGILITY] += static_cast<Attribute>(point_attrs[BATTLE_ATTR_AGILITY] / 100.0 + 1e-6);
	m_reason_list[PET_REASON_LIST_TYPE_POINT].m_attrs[BATTLE_ATTR_RECOVERY] += static_cast<Attribute>(point_attrs[BATTLE_ATTR_RECOVERY] / 100.0 + 1e-6);
	m_reason_list[PET_REASON_LIST_TYPE_POINT].m_attrs[BATTLE_ATTR_MENTAL] += static_cast<Attribute>(point_attrs[BATTLE_ATTR_MENTAL] / 100.0 + 1e-6);

	//其他(皮肤)
	if (is_have_skin)
	{
		m_reason_list[PET_REASON_LIST_TYPE_OTHER].m_attrs[BATTLE_ATTR_MAX_HP] += static_cast<Attribute>(other_attrs[BATTLE_ATTR_MAX_HP] / 100.0 + 1e-6);
		m_reason_list[PET_REASON_LIST_TYPE_OTHER].m_attrs[BATTLE_ATTR_MAX_MP] += static_cast<Attribute>(other_attrs[BATTLE_ATTR_MAX_MP] / 100.0 + 1e-6);
		m_reason_list[PET_REASON_LIST_TYPE_OTHER].m_attrs[BATTLE_ATTR_ATTACK] += static_cast<Attribute>(other_attrs[BATTLE_ATTR_ATTACK] / 100.0 + 1e-6);
		m_reason_list[PET_REASON_LIST_TYPE_OTHER].m_attrs[BATTLE_ATTR_DEFENSE] += static_cast<Attribute>(other_attrs[BATTLE_ATTR_DEFENSE] / 100.0 + 1e-6);
		m_reason_list[PET_REASON_LIST_TYPE_OTHER].m_attrs[BATTLE_ATTR_AGILITY] += static_cast<Attribute>(other_attrs[BATTLE_ATTR_AGILITY] / 100.0 + 1e-6);
		m_reason_list[PET_REASON_LIST_TYPE_OTHER].m_attrs[BATTLE_ATTR_RECOVERY] += static_cast<Attribute>(other_attrs[BATTLE_ATTR_RECOVERY] / 100.0 + 1e-6);
		m_reason_list[PET_REASON_LIST_TYPE_OTHER].m_attrs[BATTLE_ATTR_MENTAL] += static_cast<Attribute>(other_attrs[BATTLE_ATTR_MENTAL] / 100.0 + 1e-6);
	}

	//强化
	m_reason_list[PET_REASON_LIST_TYPE_STREN].m_attrs[BATTLE_ATTR_MAX_HP] += static_cast<Attribute>(str_attrs[BATTLE_ATTR_MAX_HP] / 100.0 + 1e-6 + str_cfg->maxhp);
	m_reason_list[PET_REASON_LIST_TYPE_STREN].m_attrs[BATTLE_ATTR_MAX_MP] += static_cast<Attribute>(str_attrs[BATTLE_ATTR_MAX_MP] / 100.0 + 1e-6 + str_cfg->maxmp);
	m_reason_list[PET_REASON_LIST_TYPE_STREN].m_attrs[BATTLE_ATTR_ATTACK] += static_cast<Attribute>(str_attrs[BATTLE_ATTR_ATTACK] / 100.0 + 1e-6 + str_cfg->gongji);
	m_reason_list[PET_REASON_LIST_TYPE_STREN].m_attrs[BATTLE_ATTR_DEFENSE] += static_cast<Attribute>(str_attrs[BATTLE_ATTR_DEFENSE] / 100.0 + 1e-6 + str_cfg->fangyu);
	m_reason_list[PET_REASON_LIST_TYPE_STREN].m_attrs[BATTLE_ATTR_AGILITY] += static_cast<Attribute>(str_attrs[BATTLE_ATTR_AGILITY] / 100.0 + 1e-6 + str_cfg->minjie);
	m_reason_list[PET_REASON_LIST_TYPE_STREN].m_attrs[BATTLE_ATTR_RECOVERY] += static_cast<Attribute>(str_attrs[BATTLE_ATTR_RECOVERY] / 100.0 + 1e-6 + str_cfg->huifu);
	m_reason_list[PET_REASON_LIST_TYPE_STREN].m_attrs[BATTLE_ATTR_MENTAL] += static_cast<Attribute>(str_attrs[BATTLE_ATTR_MENTAL] / 100.0 + 1e-6 + str_cfg->jingshen);

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
		GetSpecialEffectAttr(spid, m_reason_list[PET_REASON_LIST_TYPE_SKILL], base_attr_list, role_level, pet.level);
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
			GetSpecialEffectAttr(spid, m_reason_list[PET_REASON_LIST_TYPE_SKILL], base_attr_list, role_level, pet.level);
		}
	}

	m_role_module_mgr->GetSmartMountsSystem()->GetDominationBaseAttr(m_grid_list[pet_index].item_unique_id, m_reason_list[PET_REASON_LIST_TYPE_SMS]);
	this->OnCalcPetBenedictioncAttr(m_grid_list[pet_index].item_unique_id, m_reason_list[PET_REASON_LIST_TYPE_BENEDICTION], false);
	if (this->IsWaitForFightPet(pet_index))		//法宝提供宠物属性 - 不算入宠物自身提升范围内
	{
		FaBao * fabao_manager = m_role_module_mgr->GetFaBao();
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

				m_reason_list[PET_REASON_LIST_TYPE_FABAO].m_attrs[cfg->effect] += add_value;
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

				m_reason_list[PET_REASON_LIST_TYPE_FABAO].m_attrs[cfg->effect] += add_value;
			}
		}
	}

	//计算魂器赋予属性

	for (int pet_soul_equipment_index = 0; pet_soul_equipment_index < MAX_PET_SOUL_EQUIP_LIST; ++pet_soul_equipment_index)
	{
		const PetSoulEquip * soul_equip_ptr = pet.GetPetSoulEquipInPetParamConst(pet_soul_equipment_index);
		if (NULL == soul_equip_ptr)continue;;
		const PetSoulEquip & soul_equip = *soul_equip_ptr;
		if (soul_equip.Invalid())
		{
			continue;
		}

		for (int soul_idx = 0; soul_idx < MAX_PET_SOUL_EQUIPMENT_ATTR_NUM && soul_idx < soul_equip.attr_num; soul_idx++)
		{
			if (soul_equip.add_attr_list[soul_idx].attr_type < 0 || soul_equip.add_attr_list[soul_idx].attr_type >= BATTLE_ATTR_MAX)continue;
			m_reason_list[PET_REASON_LIST_TYPE_SOUL].m_attrs[soul_equip.add_attr_list[soul_idx].attr_type] += soul_equip.add_attr_list[soul_idx].attr_value;
		}

		const ItemBase * soul_base = ITEMPOOL->GetItem(soul_equip.item_id);
		if (soul_base != NULL && soul_base->GetItemType() == I_TYPE_PET_SOUL_EQUIP)
		{
			const PetSoulEquipment * pet_soul = (const PetSoulEquipment *)soul_base;
			const PetSoulEquipBaseCfg * base_cfg = LOGIC_CONFIG->GetPetSoulEquipMentConfig()->GetPetSoulEquipBaseCfg(pet_soul->GetPetWeaponSeq());
			if (base_cfg != NULL && cfg->prototype_pet_id == base_cfg->pet_weapon_pet_id)
			{
				GetSpecialEffectAttr(soul_equip.sp_id, m_reason_list[PET_REASON_LIST_TYPE_SOUL], base_attr_list, role_level, pet.level);
			}
		}

	}
	int suit_num = 0;
	SuitEffectReCalc equip_suit_list[GOD_PRINT_INDEX_TYPE_MAX];

	for (int god_print_idx = 0; god_print_idx < GOD_PRINT_INDEX_TYPE_MAX; god_print_idx++)
	{
		int bag_idx = pet.god_print_list[god_print_idx];
		if (bag_idx > 0 && bag_idx <= NEW_MAX_PET_GOD_PRONT_BAG_NUM)
		{
			if (NULL == m_new_god_print_bag[bag_idx - 1])continue;
			if (m_new_god_print_bag[bag_idx - 1]->Invalid())continue;
			GodPrintItem & node = *m_new_god_print_bag[bag_idx - 1];
			//有神印
			if (node.attr_num > 0)
			{
				if (node.main_attr.attr_value > 0)m_reason_list[PET_REASON_LIST_TYPE_GOD_PRINT].m_attrs[node.main_attr.attr_type] += node.main_attr.attr_value;
				for (int attr_idx = 0; attr_idx < node.attr_num && attr_idx < MAX_PET_GOD_PRONT_ATTR_NUM; attr_idx++)
				{
					m_reason_list[PET_REASON_LIST_TYPE_GOD_PRINT].m_attrs[node.add_attr_list[attr_idx].attr_type] += node.add_attr_list[attr_idx].attr_value;
				}
			}

			if (node.devour_num > 0)
			{
				if (node.devour_attr_list.attr_value > 0)m_reason_list[PET_REASON_LIST_TYPE_GOD_PRINT].m_attrs[node.devour_attr_list.attr_type] += node.devour_attr_list.attr_value;
			}
			
			if (node.suit_id > 0 && suit_num < GOD_PRINT_INDEX_TYPE_MAX)
			{
				bool has_suit_add = false;
				for (int suit_idx = 0; suit_idx < suit_num && suit_idx < MAX_SUIT_TYPE; suit_idx++)//先检查是否有同类组合，件数堆叠
				{
					SuitEffectReCalc &node_cale = equip_suit_list[suit_idx];
					if (node_cale.sp_id == node.suit_id)
					{
						//检查多件套中最高品质
						bool has_insert = false;
						for (int suit_num_begin = 0; suit_num_begin < node_cale.num; suit_num_begin++)
						{
							if ((int)node_cale.color_top.size() <= suit_num_begin)continue;
							if (node_cale.color_top[suit_num_begin] < (int)node.color)
							{
								std::vector<int>::iterator it = node_cale.color_top.begin();
								node_cale.color_top.insert(it + suit_num_begin, (int)node.color);
								has_insert = true;
								break;
							}
						}
						if (!has_insert)
						{
							node_cale.color_top.push_back((int)node.color);
						}

						node_cale.num += 1;
						has_suit_add = true;
						break;
					}
				}
				if (!has_suit_add)
				{
					equip_suit_list[suit_num].sp_id = node.suit_id;
					equip_suit_list[suit_num].color_top.push_back((int)node.color);
					equip_suit_list[suit_num++].num = 1;
				}
			}
		}
	}

	if (suit_num > 0)
	{
		for (int i = 0; i < suit_num && i < GOD_PRINT_INDEX_TYPE_MAX; i++)
		{
			SuitEffectReCalc & suit_node = equip_suit_list[i];
			{
				for (int suit_num_idx = 0; suit_num_idx < GOD_PRINT_INDEX_TYPE_MAX && suit_num_idx < suit_node.num && suit_num_idx < (int)suit_node.color_top.size(); suit_num_idx++)
				{
					const PetGodPrintEquipMentSuitCfg * suit_list_cfg = LOGIC_CONFIG->GetPetGodPrintEquipMentConfig()->GetSuitListCfg(suit_node.sp_id, suit_node.color_top[suit_num_idx], suit_num_idx);
					if (NULL != suit_list_cfg)
					{
						switch (suit_list_cfg->effect_type)
						{
						case PetGodPrintEquipMentSuitCfg::EFFECT_TYPE_ATTR:
							if (suit_list_cfg->param1 < 0 || suit_list_cfg->param1 >= BATTLE_ATTR_MAX)continue;
							m_reason_list[PET_REASON_LIST_TYPE_GOD_PRINT].m_attrs[suit_list_cfg->param1] += suit_list_cfg->param2;
							break;
						case PetGodPrintEquipMentSuitCfg::EFFECT_TYPE_SP_ID:
							{
								GetSpecialEffectAttr(suit_list_cfg->param2, m_reason_list[PET_REASON_LIST_TYPE_GOD_PRINT], base_attr_list, role_level, pet.level);
							}
							break;
						default:
							break;
						}
					}
				}
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
						m_reason_list[PET_REASON_LIST_TYPE_SKILL].m_attrs[attr_type] += attr_value;
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
		GetSpecialEffectAttr(spid, m_reason_list[PET_REASON_LIST_TYPE_SKILL], base_attr_list, role_level, pet.level);
	}

	std::vector<SkillPro> other_sill_list;
	m_role_module_mgr->GetSmartMountsSystem()->GetBaseSkillList(m_grid_list[pet_index].item_unique_id, &other_sill_list);
	int smart_mouns_skill_count = (int)other_sill_list.size();
	m_role_module_mgr->GetRoleGuildInborn()->GetSkillList(&other_sill_list, GUILD_INBORN_ADD_TARGET_TYPE_PET);
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
		GetSpecialEffectAttr(spid, m_reason_list[reason_type], base_attr_list, role_level, pet.level);
	}

	//家族灵脉可能会配5项基础属性
	AttributePlan tmp_ap;
	for (int i = 0, k = BATTLE_ADD_POINT_TYPE_BEGIN; i < ADD_POINT_TYPE_MAX && k <= BATTLE_ADD_POINT_TYPE_END; i++, k++)
	{
		tmp_ap.add_point_list[i] += m_reason_list[PET_REASON_LIST_TYPE_OTHER].m_attrs[k];
	}
	LOGIC_CONFIG->GetRoleExpConfig()->ConvertPointToAttribute(tmp_ap, m_reason_list[PET_REASON_LIST_TYPE_OTHER]);

	//宠物祝福
	int pet_benediction_sp_id = this->GetPetBenedictionSpId(pet, m_grid_list[pet_index].item_unique_id);
	GetSpecialEffectAttr(pet_benediction_sp_id, m_reason_list[PET_REASON_LIST_TYPE_BENEDICTION], base_attr_list, role_level, pet.level);

	//宠物天赋
	std::vector<int> talent_sp_list;
	this->GetPetTalentSpIdList(pet, talent_sp_list);
	GetSpecialEffectAttr(talent_sp_list, m_reason_list[PET_REASON_LIST_TYPE_TALENT], base_attr_list, role_level, pet.level);

	//属性丹
	for (int i = 0; i < ATTR_PELLET_TYPE_MAX && i < ARRAY_LENGTH(pet.use_attr_pellet_list); ++i)
	{
		if (pet.use_attr_pellet_list[i] <= 0) continue;

		int attr_type = GetAttrTypeByPelletType(i);
		if (attr_type < 0 || attr_type >= BATTLE_ATTR_MAX) continue;

		int add_times = LOGIC_CONFIG->GetPetConfig()->GetFeiShengShuXingDanUseTimes(pet.fei_sheng_times);
		int max_count = GetMin(LOGIC_CONFIG->GetRoleAttrPelletConfig()->GetOtherCfg().use_limit + add_times, (int)pet.use_attr_pellet_list[i]);
		const RoleAttrPelletAttrListCfg * attr_pellet_cfg = LOGIC_CONFIG->GetRoleAttrPelletConfig()->GetAttrPelletCfg(ROLE_ATTR_PELLET_TYPE_PET, cfg->main_attribute_id, attr_type);
		if (NULL != attr_pellet_cfg)
		{
			m_reason_list[PET_REASON_LIST_TYPE_PELLET].m_attrs[attr_type] += attr_pellet_cfg->attr_value * max_count;
		}
	}

	for (int fei_sheng_task_jie_duan = 1; fei_sheng_task_jie_duan <= pet.fei_sheng_task_jie_duan; ++fei_sheng_task_jie_duan)
	{
		const PetFeiShengAttrCfg * pet_fei_sheng_attr_cfg = LOGIC_CONFIG->GetPetConfig()->GetPetFeiShengAttrCfg(pet.pet_id, fei_sheng_task_jie_duan);
		if (pet_fei_sheng_attr_cfg != NULL)
		{
			for (int i = 0; i < (int)pet_fei_sheng_attr_cfg->attr_list.size(); i++)
			{
				const AttrItemCfg & curr = pet_fei_sheng_attr_cfg->attr_list[i];
				if (curr.attr_type < 0 || curr.attr_type >= BATTLE_ATTR_MAX)
				{
					continue;
				}

				m_reason_list[PET_REASON_LIST_TYPE_OTHER].m_attrs[curr.attr_type] += curr.attr_value;
			}
		}
	}

	for (int fei_sheng_task_jie_times = 1; fei_sheng_task_jie_times <= pet.fei_sheng_times; ++fei_sheng_task_jie_times)
	{
		const PetFeiShengTimesAttrCfg * pet_fei_sheng_attr_cfg = LOGIC_CONFIG->GetPetConfig()->GetPetFeiShengTimesAttrCfg(pet.pet_id, fei_sheng_task_jie_times);
		if (pet_fei_sheng_attr_cfg != NULL)
		{
			for (int i = 0; i < (int)pet_fei_sheng_attr_cfg->attr_list.size(); i++)
			{
				const AttrItemCfg & curr = pet_fei_sheng_attr_cfg->attr_list[i];
				if (curr.attr_type < 0 || curr.attr_type >= BATTLE_ATTR_MAX)
				{
					continue;
				}

				m_reason_list[PET_REASON_LIST_TYPE_OTHER].m_attrs[curr.attr_type] += curr.attr_value;
			}
		}
	}

	const PetBadgeItemParam & badge_item_param = pet.pet_bage_item;
	if (badge_item_param.pet_badgei_item_id != 0)
	{
		for (int i = 0; i < ARRAY_LENGTH(badge_item_param.attrs); i++)
		{
			const AttrInfo & curr = badge_item_param.attrs[i];
			if (curr.attr_type < 0 || curr.attr_type >= BATTLE_ATTR_MAX)
			{
				continue;
			}

			m_reason_list[PET_REASON_LIST_TYPE_OTHER].m_attrs[curr.attr_type] += curr.attr_value;
		}
	}

	//宠物强化共鸣
	const PetStrengthenBuffItemCfg * str_buff_cfg = LOGIC_CONFIG->GetPetConfig()->GetPetStrengthenBuffCfg(cfg->quality, str_buff_level);
	if (str_buff_cfg)
	{
		m_reason_list[PET_REASON_LIST_TYPE_RESON].AddAttrCfg(str_buff_cfg->attr_cfg);

		for (int sp_id_num = 0; sp_id_num < (int)str_buff_cfg->sp_id_list.size(); ++sp_id_num)
		{
			int sp_id = str_buff_cfg->sp_id_list[sp_id_num];
			GetSpecialEffectAttr(sp_id, m_reason_list[PET_REASON_LIST_TYPE_RESON], base_attr_list, role_level, pet.level);
		}
	}

	//由于总属性是一起算的，而来源是分开的，此时可能会因为小数点而造成误差，此处误差默认填充资质
	static AttributeList temp_attr; temp_attr.Reset();
	for (int i = 0; i < ARRAY_LENGTH(temp_attr.m_attrs); i++)
	{
		if(BATTLE_ATTR_CUR_HP == i|| BATTLE_ATTR_CUR_MP == i) continue;
		for (int j = 0; j < ARRAY_LENGTH(m_reason_list); j++)
		{
			temp_attr.m_attrs[i] += m_reason_list[j].m_attrs[i];
		}
		if (m_pet_base_attr[pet_index].m_attrs[i] != temp_attr.m_attrs[i])
		{
			m_reason_list[PET_REASON_LIST_TYPE_GROWTH].m_attrs[i] += m_pet_base_attr[pet_index].m_attrs[i] - temp_attr.m_attrs[i];
		}
	}

#ifdef _DEBUG
	long long elapse_us = PerformanceCatch::Instance().GetElapseUsEnd(PERFORMANCE_TEST_PET_TIME_RECALC);

	printf("%s pet_index[%d] elapse[%lld]us\n", __FUNCTION__, pet_index, elapse_us);
#endif
	return 0;
}

void Pet::SendPetAttrListReason(short pet_index)
{
	if (this->PetInvalid(pet_index)) return;
	if(this->GetAttrListReason(pet_index)) return;

	Protocol::SCAttrListReason info;
	info.reason_type = Protocol::SCAttrListReason::REASON_TYPE_PET;
	info.param1 = pet_index;
	for (int i = 0; i < MAX_REASON_TYPE_LIST_NUM; i++)
	{
		info.reason_list[i] = m_reason_list[i];
	}

	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void Pet::ReCalcAttr(int pet_index , bool _is_force /* = false */, bool is_init /* = false */)
{
	if (pet_index < 0 || pet_index >= ItemNamespace::MAX_PET_GRID_NUM) return;
	if (this->GetAttributeList(pet_index, m_pet_base_attr[pet_index])) return;

	this->OnPetAttrChange(pet_index, _is_force, is_init);
	if (!is_init)
	{
		this->OnMainReCalcAttr(m_common_param.benediction_pet_index_list[pet_index]);
	}
}

void Pet::ReCalcAttrByItemUniqueId(long long item_uuid, bool send_attr_info, int notie_type)
{
	int tmp_pet_index = this->GetPetIndexByItemUniqueId(item_uuid);
	this->ReCalcAttr(tmp_pet_index);
	if (send_attr_info)
	{
		this->SendSingleInfo(tmp_pet_index, notie_type);
	}
}

int Pet::GetPetAttr(int pet_index, int attr_type)
{
	if (pet_index < 0 || pet_index >= ItemNamespace::MAX_PET_GRID_NUM) return 0;
	if (this->GetAttributeList(pet_index, m_pet_base_attr[pet_index])) return 0;

	AttributeList & pet_attr_list = m_pet_base_attr[pet_index];
	return pet_attr_list.GetAttr(attr_type);
}

void Pet::OnPetAttrChange(int pet_index, bool _is_force, bool is_init)
{
	if(this->PetInvalid(pet_index)) return;

	if (m_role_module_mgr->GetRole()->GetSceneStatus()->IsInAutoRecoverScene())
	{
		PetParam& pet = m_grid_list[pet_index].param->pet_param;
		AttributeList& attr_list = m_pet_base_attr[pet_index];

		pet.max_hp = attr_list.m_attrs[BATTLE_ATTR_MAX_HP];
		pet.max_mp = attr_list.m_attrs[BATTLE_ATTR_MAX_MP];

		pet.hp = pet.max_hp;
		pet.mp = pet.max_mp;

		attr_list.m_attrs[BATTLE_ATTR_CUR_HP] = pet.hp;
		attr_list.m_attrs[BATTLE_ATTR_CUR_MP] = pet.mp;
	}
	else
	{
		PetParam& pet = m_grid_list[pet_index].param->pet_param;
		AttributeList& attr_list = m_pet_base_attr[pet_index];

		pet.max_hp = attr_list.m_attrs[BATTLE_ATTR_MAX_HP];
		pet.max_mp = attr_list.m_attrs[BATTLE_ATTR_MAX_MP];

		pet.hp = GetMin(pet.hp, attr_list.m_attrs[BATTLE_ATTR_MAX_HP]);
		pet.hp = GetMax(1, pet.hp);

		pet.mp = GetMin(pet.mp, attr_list.m_attrs[BATTLE_ATTR_MAX_MP]);
		pet.mp = GetMax(1, pet.mp);

		attr_list.m_attrs[BATTLE_ATTR_CUR_HP] = pet.hp;
		attr_list.m_attrs[BATTLE_ATTR_CUR_MP] = pet.mp;
	}

	m_pet_capability_list[pet_index] = this->GetPetCapability(pet_index);
	CommonDataParam & common_data = m_role_module_mgr->GetCommonData();
	if (pet_index == common_data.new_pet_status_index[NEW_FIGHT_INDEX])
	{
		m_common_param.cur_battle_fight_cap = m_pet_capability_list[pet_index];
		this->OnSetQueryAttrList(m_common_param.pet_attr_list[NEW_FIGHT_INDEX], m_pet_base_attr[pet_index]);
	}
	else if (pet_index == common_data.new_pet_status_index[NEW_STANDBY_INDEX1])
	{
		m_common_param.curr_battle_standby_index1_cap = m_pet_capability_list[pet_index];
		this->OnSetQueryAttrList(m_common_param.pet_attr_list[NEW_STANDBY_INDEX1], m_pet_base_attr[pet_index]);
	}
	else if (pet_index == common_data.new_pet_status_index[NEW_STANDBY_INDEX2])
	{
		m_common_param.curr_battle_standby_index2_cap = m_pet_capability_list[pet_index];
		this->OnSetQueryAttrList(m_common_param.pet_attr_list[NEW_STANDBY_INDEX2], m_pet_base_attr[pet_index]);
	}
	else if (pet_index == common_data.new_pet_status_index[NEW_STANDBY_INDEX3])
	{
		m_common_param.curr_battle_standby_index3_cap = m_pet_capability_list[pet_index];
		this->OnSetQueryAttrList(m_common_param.pet_attr_list[NEW_STANDBY_INDEX3], m_pet_base_attr[pet_index]);
	}
	else if (pet_index == common_data.new_pet_status_index[NEW_STANDBY_INDEX4])
	{
		m_common_param.curr_battle_standby_index4_cap = m_pet_capability_list[pet_index];
		this->OnSetQueryAttrList(m_common_param.pet_attr_list[NEW_STANDBY_INDEX4], m_pet_base_attr[pet_index]);
	}
	else if (pet_index == common_data.new_pet_status_index[NEW_STANDBY_INDEX5])
	{
		m_common_param.curr_battle_standby_index5_cap = m_pet_capability_list[pet_index];
		this->OnSetQueryAttrList(m_common_param.pet_attr_list[NEW_STANDBY_INDEX5], m_pet_base_attr[pet_index]);
	}

	RankManager::Instance().SyncPetRankInfo(this->GetRoleModuleManager()->GetRole(), PET_RANK_TYPE_CAPABILITY, pet_index, _is_force);
	
	if (!is_init)
	{
		EventHandler::Instance().OnPetCapabilityChange(this->GetRoleModuleManager()->GetRole(), m_pet_capability_list[pet_index]);
		if (pet_index == m_common_param.curr_battle_fight_index)
		{
			ColosseumManager::Instance().SyncUserData(m_role_module_mgr->GetRole());
		}
		
		if(this->IsFightPet(pet_index))
		{
			RankManager::Instance().SyncPersonRankInfo(this->GetRoleModuleManager()->GetRole(), PERSON_RANK_TYPE_CAPABILITY);
		}
	}
}

void Pet::GetQueryAttrList(int pet_index, QueryAttrList & get_attr_list)
{
	if (this->PetInvalid(pet_index)) return;

	this->OnSetQueryAttrList(get_attr_list, m_pet_base_attr[pet_index]);
}

void Pet::GetFightIndex(int* fight_index, int* standby_index1, int* standby_index2, int* standby_index3, int* standby_index4, int* standby_index5)
{
	const CommonDataParam & common_data = m_role_module_mgr->GetCommonData();
	if (NULL != fight_index) *fight_index = common_data.new_pet_status_index[NEW_FIGHT_INDEX];
	if (NULL != standby_index1) *standby_index1 = common_data.new_pet_status_index[NEW_STANDBY_INDEX1];
	if (NULL != standby_index2) *standby_index2 = common_data.new_pet_status_index[NEW_STANDBY_INDEX2];
	if (NULL != standby_index3) *standby_index3 = common_data.new_pet_status_index[NEW_STANDBY_INDEX3];
	if (NULL != standby_index4) *standby_index4 = common_data.new_pet_status_index[NEW_STANDBY_INDEX4];
	if (NULL != standby_index5) *standby_index5 = common_data.new_pet_status_index[NEW_STANDBY_INDEX5];
}

int Pet::GetFightIndex()
{
	const CommonDataParam & common_data = m_role_module_mgr->GetCommonData();
	return common_data.new_pet_status_index[NEW_FIGHT_INDEX];
}

bool Pet::IsFightPet(int pet_index)
{
	const CommonDataParam & common_data = m_role_module_mgr->GetCommonData();
	for (int i = NEW_FIGHT_INDEX_BEGIN; i <= NEW_FIGHT_INDEX_END; ++i)
	{
		if (common_data.new_pet_status_index[i] == pet_index)
		{
			return true;
		}
	}

	return false;
}

int Pet::AssignAttributePoints(int pet_index, unsigned short con, unsigned short str, unsigned short def, unsigned short agi, unsigned short magic)
{
	if (con < 0 || str < 0 || def < 0 || agi < 0 || magic < 0)return -3;
	if (this->PetInvalid(pet_index)) return -1;
	PetParam& pet = m_grid_list[pet_index].param->pet_param;
	int add_point[ADD_POINT_TYPE_MAX] = { con,str,def,agi,magic };
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
	if (point > max_point) return -2;

	for (int i = 0; i < ADD_POINT_TYPE_MAX; ++i)
	{
		pet.attr[i] += add_point[i];
	}

	this->ReCalcAttr(pet_index);
	this->SetItemDirty(pet_index,true,Protocol::SCSendPetSingleInfo::PET_NOTIE_TYPE_ASSIGN_POINTS);
	EventHandler::Instance().OnPetAllotAttrpoint(m_role_module_mgr->GetRole(), con, str, def, agi, magic);

	if (this->IsInPlatfformBattle(m_grid_list[pet_index].item_unique_id))
	{
		this->SyncPetDataPlatformBattle(pet_index);
	}

	return 0;
}

int Pet::AutoAssignAttributePoints(int pet_index, bool is_notice)
{
	if (this->PetInvalid(pet_index)) return -1;
	PetParam & pet = m_grid_list[pet_index].param->pet_param;

	if (pet.has_refresh_points != 0 || pet.source != 0)
	{
		return -2;
	}

	const PetOtherCfg & other_cfg = LOGIC_CONFIG->GetPetConfig()->GetOtherCfgRef();
	if (pet.level > other_cfg.auto_pet_levels)
	{
		return -3;
	}

	const PetCfg * pet_cfg = LOGIC_CONFIG->GetPetConfig()->GetPetCfg(pet.pet_id);
	if (NULL == pet_cfg) return -4;

	int max_point = other_cfg.att_point_level * (pet.level - 1) + other_cfg.attr_point_init;
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
		point += pet.attr[i];
	}

	int add_point = max_point - point;

	if (add_point <= 0)
	{
		return -5;
	}

	int temp[ADD_POINT_TYPE_MAX] = { 0 };
	if (0 <= pet_cfg->main_attribute_id && pet_cfg->main_attribute_id < ARRAY_LENGTH(pet.attr))
	{
		pet.attr[pet_cfg->main_attribute_id] += add_point;
		temp[pet_cfg->main_attribute_id] = add_point;
	}

	EventHandler::Instance().OnPetAllotAttrpoint(m_role_module_mgr->GetRole(), temp[ADD_POINT_TYPE_VITALITY], temp[ADD_POINT_TYPE_STRENGTH],
						     temp[ADD_POINT_TYPE_POWER], temp[ADD_POINT_TYPE_AGILITY], temp[ADD_POINT_TYPE_MAGIC]);

	if (is_notice)
	{
		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF),
				      gamestring::g_auto_assign_attribute_points_notice, pet.name);
		World::GetInstWorld()->GetSceneManager()->SystemMsgPerson2(m_role_module_mgr->GetRole(), gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_SYSTEM_CHAT));
	}
		
	return 0;
}

void Pet::UpdateAutoAssignAttributePoints()
{
	if (m_is_update)
	{
		return;
	}

	m_is_update = true;
	for (int i = 0; i < ItemNamespace::MAX_PET_GRID_NUM; ++i)
	{
		this->AutoAssignAttributePoints(i);
	}	
}

int Pet::ReName(int pet_index, GameName name)
{
	if (this->PetInvalid(pet_index)) return -1;
	PetParam& pet = m_grid_list[pet_index].param->pet_param;
	
	// 扣钱
	int rename_consume = LOGIC_CONFIG->GetPetConfig()->GetOtherCfg()->pet_rename_need;
	if (!m_role_module_mgr->GetMoney()->UseCoinBind(rename_consume, __FUNCTION__))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_COIN_NOT_ENOUGH);
		return -999;
	}

	// 改名
	memcpy(pet.name, name, sizeof(GameName));
	EventHandler::Instance().OnPetChangeName(m_role_module_mgr->GetRole(), pet_index, name);
	this->SetItemDirty(pet_index,true,Protocol::SCSendPetSingleInfo::PET_NOTIE_TYPE_RENAME);
	if (pet_index == m_role_module_mgr->GetCommonData().new_pet_status_index[NEW_FOLLOW_INDEX]) this->SendPetFollowInfo();
	return 0;
}

int Pet::Breakthroughs(int pet_index, const int(&consume_pet_index_arr)[3], bool is_liao_yao_pet)
{
	if (this->PetInvalid(pet_index))
	{
		gamelog::g_log_debug.printf(LL_WARNING, "Pet::Breakthroughs role[%d,%s] pet_index[%d] invalid!!!",
			m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), pet_index);
		return -__LINE__;
	}

	PetParam & pet = m_grid_list[pet_index].param->pet_param;

	int pet_id = is_liao_yao_pet ? pet.lian_yao_pet_id : pet.pet_id;
	unsigned short & pet_str_level = is_liao_yao_pet ? pet.lian_yao_pet_str_lv : pet.str_level;
	
	const PetCfg * pet_cfg = LOGIC_CONFIG->GetPetConfig()->GetPetCfg(pet_id);
	if (!pet_cfg)
	{
		return -__LINE__;
	}

	const PetStrengthenCfg * str_cfg = LOGIC_CONFIG->GetPetConfig()->GetPetStrengthenCfg(pet_cfg->strengthen_cfg_id, pet_str_level);
	if (NULL == str_cfg)
	{
		return-__LINE__;
	}

	if (pet.fei_sheng_times < str_cfg->need_fei_sheng_times)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_PET_FEI_SHENG_TIMES_NOT_ENOUGH);
		return-__LINE__;
	}

	const PetStrengtheConsumeCfg * pet_strengthe_consume_cfg = LOGIC_CONFIG->GetPetConfig()->GetSpecifyPetStrengtheConsumeCfg(pet_cfg->prototype_pet_id, pet_str_level);
	if (!pet_strengthe_consume_cfg)
	{
		pet_strengthe_consume_cfg = LOGIC_CONFIG->GetPetConfig()->GetPetStrengtheConsumeCfg(pet_cfg->quality, pet_str_level);
	}

	if (!pet_strengthe_consume_cfg)
	{
		return -__LINE__;
	}

	if ((int)pet_strengthe_consume_cfg->consume_list.size() > ARRAY_LENGTH(consume_pet_index_arr))
	{
		return -__LINE__;
	}

	std::set<int> consume_pet_index_set;
	for (int i = 0; i < ARRAY_LENGTH(consume_pet_index_arr) && i < (int)pet_strengthe_consume_cfg->consume_list.size(); ++i)
	{
		int consume_index = consume_pet_index_arr[i];
		if (consume_pet_index_set.find(consume_index) != consume_pet_index_set.end())return -__LINE__;
		consume_pet_index_set.insert(consume_index);
		if (this->PetInvalid(consume_index) || this->IsLock(consume_index)) return -__LINE__;
		PetParam & consume_pet_param = m_grid_list[consume_index].param->pet_param;

		const PetCfg * consume_pet_cfg = LOGIC_CONFIG->GetPetConfig()->GetPetCfg(consume_pet_param.pet_id);
		if (!consume_pet_cfg)
		{
			return -__LINE__;
		}

		const PetStrengtheConsumeCfg::ItemCfg & curr_item = pet_strengthe_consume_cfg->consume_list[i];
		if (curr_item.need_same_pet && pet_cfg->mutant_id != consume_pet_cfg->mutant_id)
		{
			return -__LINE__;
		}

		if (curr_item.need_pet_quality != consume_pet_cfg->quality)
		{
			return -__LINE__;
		}

		if (curr_item.pet_strengthen_level > consume_pet_param.str_level)
		{
			return -__LINE__;
		}
	}

	if (consume_pet_index_set.find(pet_index) != consume_pet_index_set.end())
	{
#ifdef _DEBUG
		assert(0);
#endif
		return -__LINE__;
	}

	std::set<long long> pet_unique_id_set;
	for (std::set<int>::const_iterator iter = consume_pet_index_set.begin(); iter != consume_pet_index_set.end(); ++iter)
	{
		int consume_index = *iter;
		if (this->PetInvalid(consume_index) || this->IsLock(consume_index)) return -__LINE__;

		long long  consume_pet_unique_id = m_grid_list[consume_index].item_unique_id;
		PetParam & consume_pet_param = m_grid_list[consume_index].param->pet_param;
		pet_unique_id_set.insert(consume_pet_unique_id);

		if (this->IsFightPet(consume_index))
		{
			return  -__LINE__;
		}

		if (this->IsInPlatfformBattle(consume_pet_unique_id, true))			//消耗的宠物不能在擂台上
		{
			return -__LINE__;
		}
		if (m_role_module_mgr->GetSmartMountsSystem()->IsPetDomination(consume_pet_unique_id))		//消耗的宠物不能处于被统御状态
		{
			m_role_module_mgr->NoticeNum(errornum::EN_SMART_MOUNTS_SYSTEM_PET_DOMINATION);
			return -__LINE__;
		}

		if (-1 != m_common_param.benediction_pet_index_list[consume_index])		//消耗的宠物不能处于祝福副宠
		{
			m_role_module_mgr->NoticeNum(errornum::EN_PET_NOT_OP_BY_BENEDICTION);
			return -__LINE__;
		}

		if (consume_pet_param.pet_equipment_id != 0)
		{
			m_role_module_mgr->NoticeNum(errornum::EN_PET_HSA_EQUIPMENT_ERR);
			return false;
		}

		if (consume_pet_param.HasGodPrint())//消耗宠物不能穿戴神印
		{
			if (!this->PutOffAllPetGodPrintEquip(consume_index))
			{
				m_role_module_mgr->NoticeNum(errornum::EN_PET_WEAR_GOD_PRINT);
				return  -__LINE__;
			}
		}

		if (this->PetBadgeTakeOff(consume_index) != 0)
		{
			m_role_module_mgr->NoticeNum(errornum::EN_PET_BADGE_TAKE_OFF_FAIL);
			return  -__LINE__;
		}

		if (!this->PutOffAllPetSoulEquip(consume_index))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_PET_WEAR_SOUL_EQUIP);
			return  -__LINE__;
		}
	}

	//////////////////////////// 检查 ///////////////////////////////////

	if (m_role_module_mgr->GetMoney()->GetCoinBind() < pet_strengthe_consume_cfg->need_coin)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_COIN_NOT_ENOUGH);
		return -__LINE__;
	}

	if (0 != this->ConversionExpByList(consume_pet_index_set, 1, __FUNCTION__, PUT_REASON_PET_BREAKTHROUGH))
	{
		return -__LINE__;
	}

	//////////////////////////// 消耗 ///////////////////////////////////
	for (std::set<int>::const_iterator iter = consume_pet_index_set.begin(); iter != consume_pet_index_set.end(); ++iter)
	{
		int consume_index = *iter;
		if (this->PetInvalid(consume_index) || this->IsLock(consume_index)) continue;
		//long long  consume_pet_unique_id = m_grid_list[consume_index].item_unique_id;
		PetParam & consume_pet_param = m_grid_list[consume_index].param->pet_param;

		gamelog::g_log_pet.printf(LL_INFO, "%s line:%d | %s , pet_index:%d %s str_level:%d,consume_index:%d %s consume_str_level:%d is_op_lian_yao:%d", __FUNCTION__, __LINE__, m_role_module_mgr->GetRoleInfoString().c_str(),
			pet_index, pet.GetPetBaseInfoString(this->GetPetUniqueId(pet_index)).c_str(), pet_str_level,
			consume_index, consume_pet_param.GetPetBaseInfoString(this->GetPetUniqueId(consume_index)).c_str(), consume_pet_param.str_level, (int)is_liao_yao_pet);
		this->DeletePet(consume_index, __FUNCTION__);
	}
	m_role_module_mgr->GetMoney()->UseCoinBind(pet_strengthe_consume_cfg->need_coin, __FUNCTION__);

	for (std::set<long long>::const_iterator iter = pet_unique_id_set.begin(); iter != pet_unique_id_set.end(); ++iter)
	{
		RankManager::Instance().SyncRemovePet(this->GetRoleModuleManager()->GetRole(), *iter);
	}

	//////////////////////////// 检查 ///////////////////////////////////
	bool is_succ = true;

	int old_str_level = pet_str_level;
	pet_str_level += 1;

	ROLE_LOG_QUICK10(LOG_TYPE_PET_BREAKTHROUGHS, m_role_module_mgr->GetRole(), pet_index, pet_id, __FUNCTION__, is_liao_yao_pet ? "lian_yao_pet" : "pet", old_str_level, pet_str_level, this->GetPetUniqueId(pet_index), pet.pet_id, 0);
	gamelog::g_log_pet.printf(LL_INFO, "%s line:%d |%s  %s  pet_index:%d str_level:%d lian_yao_pet_str_lv:%d is_op_lian_yao:%d", 
		__FUNCTION__, __LINE__, m_role_module_mgr->GetRoleInfoString().c_str(), pet.GetPetBaseInfoString(this->GetPetUniqueId(pet_index)).c_str(),
		pet_index, pet.str_level, pet.lian_yao_pet_str_lv, (int)is_liao_yao_pet);

	this->ReCalcAttr(pet_index);
	EventHandler::Instance().OnPetBaseInfoChange(m_role_module_mgr->GetRole(), pet_index);
	EventHandler::Instance().OnPetIntensi(m_role_module_mgr->GetRole(), pet_index, pet_cfg->quality, pet_str_level);

	Protocol::SCSendPetBreakthroughsRet ret;
	ret.index = pet_index;
	ret.is_succ = is_succ ? 1 : 0;
	ret.old_level = old_str_level;
	ret.is_liao_yao_pet = is_liao_yao_pet;
	m_role_module_mgr->NetSend((const char *)&ret, sizeof(ret));

	this->SetItemDirty(pet_index);
	this->SendSingleInfo(pet_index, Protocol::SCSendPetSingleInfo::PET_NOTIE_TYPE_BREAK_THROUGHS);

	if (is_succ)
	{
		if (this->IsInPlatfformBattle(m_grid_list[pet_index].item_unique_id))
		{
			this->SyncPetDataPlatformBattle(pet_index);
		}

		if (LOGIC_CONFIG->GetPetConfig()->IsStrengthenBroadcast(pet_id, pet_str_level))
		{
			int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_pet_strengthen_broadcast,
				m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), pet_cfg->quality, pet_id, pet_str_level);
			if (length > 0)
			{
				Protocol::SCSystemMsg sm;
				int sendlen = 0;
				if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_SYSTEM_CHAT)))
				{
					World::GetInstWorld()->SendToAllGateway((const char *)&sm, sendlen);		// 通知所有玩家
				}
			}
		}
	}

	m_role_module_mgr->GetDayReward()->OnUnlockPetStrengthenReward(pet_cfg->quality, pet_str_level);

	RoleActivityXinFuQiangHua * xin_fu_qiang_hua = static_cast<RoleActivityXinFuQiangHua *>
		(m_role_module_mgr->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_XIN_FU_QIANG_HUA));
	if (NULL != xin_fu_qiang_hua)
	{
		xin_fu_qiang_hua->StrengthenTaskFinish(pet_cfg->quality, pet_str_level);
	}

	this->SetMapPetQualityToStrLevel(&pet);
	this->CheckPetYiWenLuRewardByIndex(pet_index);
	this->CheckStrengthenBuff(pet_index);
	this->CheckUnlockLianYaoFurance();

	return 0;
}

int Pet::GmBreakthroughs(const int type, int param1, int param2)
{
	int pet_id = 0;
	int str_level = 1;
	int pet_index = -1;
	bool need_call = false;
	bool is_lian_yao_pet = false;

	if (type == 0)
	{
		pet_index = param1;
		const PetParam * pet_ptr = this->GetPetParam(pet_index);
		if (!pet_ptr)
		{
			return -__LINE__;
		}

		const PetParam & pet = *pet_ptr;
		pet_id = pet.pet_id;
		str_level = pet.str_level;
	}

	if (type == 1)
	{
		pet_id = param1;
		str_level = param2;
	}

	if (type == 2)
	{
		need_call = true;
		pet_index = param1;
		const PetParam * pet_ptr = this->GetPetParam(pet_index);
		if (!pet_ptr)
		{
			return -__LINE__;
		}

		const PetParam & pet = *pet_ptr;
		pet_id = pet.pet_id;
		str_level = pet.str_level;
	}

	if (type == 3)
	{
		need_call = true;
		pet_id = param1;
		str_level = param2;
	}

	if (type == 4)
	{
		need_call = true;
		is_lian_yao_pet = true;
		pet_id = param1;
		str_level = param2;		
	}

	const PetCfg * pet_cfg = LOGIC_CONFIG->GetPetConfig()->GetPetCfg(pet_id);
	if (!pet_cfg)
	{
		return -__LINE__;
	}
	                                                                                                                                                                                                                                                                                                                                                    
	const PetStrengtheConsumeCfg * pet_strengthe_consume_cfg = LOGIC_CONFIG->GetPetConfig()->GetPetStrengtheConsumeCfg(pet_cfg->quality, str_level);
	if (!pet_strengthe_consume_cfg)
	{
		return -__LINE__;
	}

	std::map<int, int> m;
	m[1] = 50128;
	m[2] = 50156;
	m[3] = 50098;
	m[4] = 50166;
	m[5] = 50176;

	std::vector<int> v;
	std::set<int> skip_repeat;
	for (int i = 0; i < (int)pet_strengthe_consume_cfg->consume_list.size(); ++i)
	{
		const PetStrengtheConsumeCfg::ItemCfg & curr_item = pet_strengthe_consume_cfg->consume_list[i];
		switch (type)
		{
			case 0:
			case 2:
				{
					int add_id = m[curr_item.need_pet_quality];
					if (curr_item.need_same_pet)
					{
						add_id = pet_id;
					}

					int ret = this->AcquirePet(add_id, __FUNCTION__, curr_item.pet_strengthen_level, false, GET_PET_REASON_TYPE_INVALID);
					if (ret >= 0)
					{
						v.push_back(ret);
					}
				}
				break;
			case 1:
			case 3:
				{
					int min_str = INT_MAX;
					int min_index = -1;
					for (int k = 0; k < ARRAY_LENGTH(m_grid_list); ++k)
					{
						if (skip_repeat.count(k) != 0)
						{
							continue;
						}

						const PetParam * p = this->GetPetParam(k);
						if (!p)
						{
							continue;
						}

						const PetCfg * consume_pet_cfg = LOGIC_CONFIG->GetPetConfig()->GetPetCfg(p->pet_id);
						if (!consume_pet_cfg)
						{
							continue;
						}

						if (curr_item.need_same_pet && pet_cfg->mutant_id != consume_pet_cfg->mutant_id)
						{
							continue;
						}

						if (pet_cfg->quality != consume_pet_cfg->quality)
						{
							continue;
						}

						if (curr_item.pet_strengthen_level > p->str_level)
						{
							continue;
						}

						if (this->IsFightPet(k))
						{
							continue;
						}

						if (k == pet_index)
						{
							continue;
						}

						if (p->str_level < min_str)
						{
							min_index = k;
						}
					}

					skip_repeat.insert(min_index);
					v.push_back(min_index);
				}
				break;
			default:
				return -__LINE__;
		}
	}

	if (need_call)
	{
		v.resize(3);
		int ret = this->Breakthroughs(pet_index, *((const int(*)[3]) (&v[0])), is_lian_yao_pet);
	}	

	return 0;
}

int Pet::TransferBreakthroughs(int pet_index, int consume_index)
{
	if (this->PetInvalid(pet_index)) return -1;
	if (this->PetInvalid(consume_index) || this->IsLock(consume_index)) return -2;

	if (this->IsInPlatfformBattle(m_grid_list[consume_index].item_unique_id, true))			//消耗的宠物不能在擂台上
	{
		return -33;
	}

	if (m_role_module_mgr->GetSmartMountsSystem()->IsPetDomination(m_grid_list[consume_index].item_unique_id))		//消耗的宠物不能处于被统御状态
	{
		m_role_module_mgr->NoticeNum(errornum::EN_SMART_MOUNTS_SYSTEM_PET_DOMINATION);
		return -44;
	}

	if (-1 != m_common_param.benediction_pet_index_list[consume_index])		//消耗的宠物不能处于祝福副宠
	{
		m_role_module_mgr->NoticeNum(errornum::EN_PET_NOT_OP_BY_BENEDICTION);
		return -__LINE__;
	}

	if (m_grid_list[consume_index].param->pet_param.HasGodPrint())//消耗宠物不能穿戴神印
	{
		m_role_module_mgr->NoticeNum(errornum::EN_PET_WEAR_GOD_PRINT);
		return -55;
	}

	int before_str_buff_level = this->GetStrBuffLevel(pet_index);
	PetParam& pet = m_grid_list[pet_index].param->pet_param;
	PetParam& consume_pet = m_grid_list[consume_index].param->pet_param;

	gamelog::g_log_pet.printf(LL_INFO, "%s line:%d |%s  %s   str_level:%d ", __FUNCTION__, __LINE__, m_role_module_mgr->GetRoleInfoString().c_str(), pet.GetPetBaseInfoString(this->GetPetUniqueId(pet_index)).c_str(), pet.str_level);
	gamelog::g_log_pet.printf(LL_INFO, "%s line:%d |%s  %s   consume_pet_str_level:%d ", __FUNCTION__, __LINE__, m_role_module_mgr->GetRoleInfoString().c_str(), consume_pet.GetPetBaseInfoString(this->GetPetUniqueId(pet_index)).c_str(), consume_pet.str_level);


	if (pet.pet_id != consume_pet.pet_id) return -3;
	if (pet.str_level >= consume_pet.str_level) return -4;

	ItemID item_id = LOGIC_CONFIG->GetPetConfig()->GetOtherCfg()->exchange_strengthen_need;
	if (!m_role_module_mgr->GetKnapsack()->ConsumeItem(item_id, 1, __FUNCTION__))
	{
		Protocol::SCLackItem li;
		li.item_id = item_id;
		li.item_count = 1;
		m_role_module_mgr->NetSend((const char*)&li, sizeof(li));
		return -5;
	}

	pet.str_level = consume_pet.str_level;
	consume_pet.str_level = 1;

	gamelog::g_log_pet.printf(LL_INFO, "%s line:%d |%s  %s   str_level:%d ", __FUNCTION__, __LINE__, m_role_module_mgr->GetRoleInfoString().c_str(), pet.GetPetBaseInfoString(this->GetPetUniqueId(pet_index)).c_str(), pet.str_level);
	gamelog::g_log_pet.printf(LL_INFO, "%s line:%d |%s  %s   consume_pet_str_level:%d ", __FUNCTION__, __LINE__, m_role_module_mgr->GetRoleInfoString().c_str(), consume_pet.GetPetBaseInfoString(this->GetPetUniqueId(pet_index)).c_str(), consume_pet.str_level);

	this->ReCalcAttr(pet_index);
	this->SetItemDirty(pet_index);
	this->ReCalcAttr(consume_index);
	this->SetItemDirty(consume_index);

	return 0;
}

int Pet::Lock(int pet_index)
{
	if (this->PetInvalid(pet_index)) return -1;

	//策划说出战与待战不是默认锁定
	/*const CommonDataParam & common_data = m_role_module_mgr->GetCommonData();
	if (common_data.new_pet_status_index[NEW_FIGHT_INDEX] == pet_index)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_PET_IS_FIGHT);
		return -2;
	}
	else if(common_data.new_pet_status_index[NEW_STANDBY_INDEX2] == pet_index ||
		common_data.new_pet_status_index[NEW_STANDBY_INDEX3] == pet_index ||
		common_data.new_pet_status_index[NEW_STANDBY_INDEX1] == pet_index ||
		common_data.new_pet_status_index[NEW_STANDBY_INDEX4] == pet_index ||
		common_data.new_pet_status_index[NEW_STANDBY_INDEX5] == pet_index)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_PET_IS_STANDBY);
		return -2;
	}*/

	PetParam & pet = m_grid_list[pet_index].param->pet_param;
	pet.lock = 0 == pet.lock ? 1 : 0;
	if (1 == pet.lock)
	{
		m_role_module_mgr->NoticeNum(noticenum::NT_PET_LOCK_SUCC);
	}
	else
	{
		m_role_module_mgr->NoticeNum(noticenum::NT_PET_UNLOCK_SUCC);
	}
	
	this->SetItemDirty(pet_index,true,Protocol::SCSendPetSingleInfo::PET_NOTIE_TYPE_LOCK);
	return 0;
}

bool Pet::IsLock(int pet_index)
{
	if (this->PetInvalid(pet_index)) return true;

	PetParam& pet = m_grid_list[pet_index].param->pet_param;
	return 1 == pet.lock;
}

int Pet::SetPetState(int pet_index, int state)
{
	if (this->PetInvalid(pet_index)) return -1;

	CommonDataParam & common_data = m_role_module_mgr->GetCommonData();
	short & fight_index = common_data.new_pet_status_index[NEW_FIGHT_INDEX];

	//PetParam & pet = m_grid_list[pet_index].param->pet_param;
	int before_str_buff_level = this->GetStrBuffLevel(pet_index);
	switch (state)
	{
	case SET_FIGHT_INDEX:
		{
			if (fight_index == pet_index)
			{
				return 0;
			}
			
			if (-1 != m_common_param.benediction_pet_index_list[pet_index])		//副宠不能出战
			{
				m_role_module_mgr->NoticeNum(errornum::EN_PET_NOT_OP_BY_BENEDICTION);
				return 0;
			}

			int stand_by_num = 0; bool is_replaced = false;
			for (int posi_index = NEW_STANDBY_INDEX1; posi_index <= NEW_STANDBY_INDEX5; ++posi_index)
			{
				short & posi_pet_index = common_data.new_pet_status_index[posi_index];
				if (posi_pet_index >= 0) ++stand_by_num;

				if (posi_pet_index == pet_index)	// 是从等待状态中的宠物出战；
				{
					is_replaced = true;
					posi_pet_index = fight_index;
					if (this->PetInvalid(posi_pet_index))	//调换位置后,记录的战力与属性也需要调换
					{
						m_common_param.pet_attr_list[posi_index] = m_common_param.pet_attr_list[SET_FIGHT_INDEX];
						m_common_param.SetCapByFightStatusChange(posi_index, m_common_param.cur_battle_fight_cap);
					}
					break;
				}
			}

			if (!is_replaced)	// 不是从等待状态的宠物中出战 --> 检查等待列表还有没有空位
			{
				if (stand_by_num >= MAX_FIGHT_AND_STAND_BY_PET_NUM) common_data.new_pet_status_index[NEW_STANDBY_INDEX5] = -1;	// 如果数量到达上限，把最后一个等待宠物位设置成空；

				if (fight_index >= 0 && stand_by_num < (MAX_FIGHT_AND_STAND_BY_PET_NUM - 1))	// 如果等待列表的宠物小于 4 只，把战斗宠物放进去；
				{
					for (int posi_index = NEW_STANDBY_INDEX1; posi_index < NEW_STANDBY_INDEX5; ++posi_index)
					{
						short & posi_pet_index = common_data.new_pet_status_index[posi_index];
						if (posi_pet_index < 0)
						{
							posi_pet_index = fight_index;
							if (this->PetInvalid(posi_pet_index))	//调换位置后,记录的战力与属性也需要调换
							{
								m_common_param.pet_attr_list[posi_index] = m_common_param.pet_attr_list[SET_FIGHT_INDEX];
								m_common_param.SetCapByFightStatusChange(posi_index, m_common_param.cur_battle_fight_cap);
							}
							break;
						}						
					}
				}
			}

			fight_index = pet_index;
			//pet.lock = 1;
			EventHandler::Instance().OnPetBaseInfoChange(m_role_module_mgr->GetRole(), pet_index);
			EventHandler::Instance().OnSwitchPetToFight(m_role_module_mgr->GetRole());

			m_role_module_mgr->GetRoleFormation()->SynPetAutoSkill();
		}
		break;
	case SET_FOLLOW_INDEX:
		{
			common_data.new_pet_status_index[NEW_FOLLOW_INDEX] = pet_index;
			this->SendPetFollowInfo();
		}
		break;
	case SET_STANDBY_INDEX1:
	case SET_STANDBY_INDEX2:
	case SET_STANDBY_INDEX3:
	case SET_STANDBY_INDEX4:
	case SET_STANDBY_INDEX5:
		{
			if (-1 != m_common_param.benediction_pet_index_list[pet_index])		//副宠不能待战
			{
				m_role_module_mgr->NoticeNum(errornum::EN_PET_NOT_OP_BY_BENEDICTION);
				return 0;
			}

			int list_index_num = fight_index >= 0 ? 1 : 0; bool is_succ = false;
			for (int posi_index = NEW_STANDBY_INDEX1; posi_index <= NEW_STANDBY_INDEX5; ++posi_index)
			{
				if (fight_index != pet_index && list_index_num >= MAX_FIGHT_AND_STAND_BY_PET_NUM) break;

				short & posi_pet_index = common_data.new_pet_status_index[posi_index];
				if (pet_index == posi_pet_index || posi_pet_index < 0)		// 如果已经是待机状态，或是空位，就替换后跳出；
				{
					posi_pet_index = pet_index;
				//	pet.lock = 1;
					is_succ = true;
					break;
				}

				if (posi_pet_index >= 0) ++list_index_num;
			}

			if (is_succ && pet_index == fight_index)
			{
				fight_index = -1;
				EventHandler::Instance().OnFightPetToStandby(m_role_module_mgr->GetRole());
			}

			if (!is_succ && list_index_num >= MAX_FIGHT_AND_STAND_BY_PET_NUM)
			{
				m_role_module_mgr->NoticeNum(errornum::EN_STANDBY_AND_FIGHT_PET_NOT_EXCEED_5);
				return -2;
			}
		}
		break;
	case SET_REST:
		{
			for (int posi_index = NEW_FIGHT_INDEX; posi_index <= NEW_STANDBY_INDEX5; ++posi_index)
			{
				short & posi_pet_index = common_data.new_pet_status_index[posi_index];
				if (posi_pet_index != pet_index) continue;
				
				posi_pet_index = -1;
				if (NEW_FIGHT_INDEX == posi_index) EventHandler::Instance().OnFightPetRest(m_role_module_mgr->GetRole());
				break;
			}

			if (common_data.new_pet_status_index[NEW_FOLLOW_INDEX] == pet_index)
			{
				common_data.new_pet_status_index[NEW_FOLLOW_INDEX] = -1;
				this->SendPetFollowInfo();
			}
		}
		break;
	case SET_REST_FOLLOW:
		{
			common_data.new_pet_status_index[NEW_FOLLOW_INDEX] = -1;
			this->SendPetFollowInfo();
		}
		break;
	default:
		return -3;
	}

	this->SendPetOtherInfo();
	this->ReCalcAttr(pet_index);
	this->SetItemDirty(pet_index,true,Protocol::SCSendPetSingleInfo::PET_NOTIE_TYPE_STATUS);
	return 0;
}

int Pet::OnUseExp(int pet_index, int item_id, int num)
{
	if (this->PetInvalid(pet_index)) return -1;
	PetParam& pet = m_grid_list[pet_index].param->pet_param;
	int role_level = m_role_module_mgr->GetRole()->GetLevel();
	if (pet.level >= role_level)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_PET_USE_EXP_PILL);
		return -2;			// 宠物等级不得超过玩家等级
	}

	const PetExpPillCfg * cfg = LOGIC_CONFIG->GetPetConfig()->GetExpPillCfg(item_id);
	if (NULL == cfg)return -3;

	const PetLevelCfg * level_cfg = LOGIC_CONFIG->GetPetConfig()->GetLevelCfg(pet.level);
	if (level_cfg == NULL)
	{
		return -301;
	}

	if (level_cfg->is_need_breach && pet.is_up_level_break == 0)
	{		
		m_role_module_mgr->NoticeNum(errornum::EN_USE_PET_EXP_ITEM_ERR_WITHOUT_BREAK_LEVEL);
		return -302;
	}

	int add_exp = cfg->add_exp * num;
	int consume_price = cfg->price * num;
	/////////////////////////////////// 检查 //////////////////////////////////////
	switch (cfg->money_type)
	{
	case MONEY_TYPE_COIN_BIND:
		if (m_role_module_mgr->GetMoney()->GetCoinBind() < consume_price)
		{
			m_role_module_mgr->NoticeNum(errornum::EN_COIN_NOT_ENOUGH);
			return -4;
		}
		break;
	case MONEY_TYPE_IMMORTAL_COIN:
		if (m_role_module_mgr->GetMoney()->GetImmortalCoin() < consume_price)
		{
		m_role_module_mgr->NoticeNum(errornum::EN_MARKET_COIN_BIND_NOT_ENOUGH);
			return -5;
		}
		break;
	case MONEY_TYPE_GOLD:
		if (m_role_module_mgr->GetMoney()->GetGold() < consume_price)
		{
			m_role_module_mgr->NoticeNum(errornum::EN_GOLD_NOT_ENOUGH);
			return -6;
		}
		break;
	default:
		break;
	}

	if (m_role_module_mgr->GetKnapsack()->Count(item_id, false) < num)
	{
		Protocol::SCLackItem li;
		li.item_id = item_id;
		li.item_count = num;
		m_role_module_mgr->NetSend((const char*)&li, sizeof(li));
		return -7;
	}
	/////////////////////////////////// 消耗 //////////////////////////////////////
	m_role_module_mgr->GetKnapsack()->ConsumeItem(item_id, num, __FUNCTION__);
	switch (cfg->money_type)
	{
	case MONEY_TYPE_COIN_BIND:
		m_role_module_mgr->GetMoney()->UseCoinBind(consume_price, __FUNCTION__);
		break;
	case MONEY_TYPE_IMMORTAL_COIN:
		m_role_module_mgr->GetMoney()->UseImmortalCoin(consume_price, __FUNCTION__);
		break;
	case MONEY_TYPE_GOLD:
		m_role_module_mgr->GetMoney()->UseGold(consume_price, __FUNCTION__);
		break;
	default:
		break;
	}

	/////////////////////////////////// 给予 //////////////////////////////////////
	this->AddExp(pet_index, add_exp, __FUNCTION__, Protocol::SCSendPetSingleInfo::PET_NOTIE_TYPE_EXP);
	return 0;
}

int Pet::OnRefreshPoints(int pet_index)
{
	if (this->PetInvalid(pet_index)) return -1;
	PetParam& pet = m_grid_list[pet_index].param->pet_param;

	if (pet.has_refresh_points != 0)		//每一只宠物都有一次免费洗点的机会，洗点之后就不自动加点
	{	/////////////////////////////////// 检查 //////////////////////////////////////
		int consume_item_id = LOGIC_CONFIG->GetPetConfig()->GetOtherCfg()->respec_item;
		int consume_item_num = 1;
		if (m_role_module_mgr->GetKnapsack()->Count(consume_item_id, false) < consume_item_num)
		{
			Protocol::SCLackItem li;
			li.item_id = consume_item_id;
			li.item_count = consume_item_num;
			m_role_module_mgr->NetSend((const char *)&li, sizeof(li));
			return -2;
		}
		/////////////////////////////////// 消耗 //////////////////////////////////////
		m_role_module_mgr->GetKnapsack()->ConsumeItem(consume_item_id, consume_item_num, __FUNCTION__);
	}



	pet.has_refresh_points = 1;
	/////////////////////////////////// 给予 //////////////////////////////////////
	memset(pet.attr, 0, sizeof(pet.attr));

	gamelog::g_log_pet.printf(LL_INFO, "%s line:%d |%s  %s ", __FUNCTION__, __LINE__, m_role_module_mgr->GetRoleInfoString().c_str(), pet.GetPetBaseInfoString(this->GetPetUniqueId(pet_index)).c_str());

	this->ReCalcAttr(pet_index);
	this->SetItemDirty(pet_index,true,Protocol::SCSendPetSingleInfo::PET_NOTIE_TYPE_ASSIGN_POINTS);
	return 0;
}

bool Pet::IsWaitForFightPet(int pet_index, int * out_index)
{
	int fight_index = 0;
	int standby_index1 = 0;
	int standby_index2 = 0;
	int standby_index3 = 0;
	int standby_index4 = 0;
	int standby_index5 = 0;
	this->GetFightIndex(&fight_index, &standby_index1, &standby_index2, &standby_index3, &standby_index4, &standby_index5);
	if (fight_index >= 0 && fight_index == pet_index)
	{
		if (out_index != NULL)*out_index = NEW_FIGHT_INDEX;
		return true;
	}

	if (standby_index1 >= 0 && standby_index1 == pet_index)
	{
		if (out_index != NULL)*out_index = NEW_STANDBY_INDEX1;
		return true;
	}

	if (standby_index2 >= 0 && standby_index2 == pet_index)
	{
		if (out_index != NULL)*out_index = NEW_STANDBY_INDEX2;
		return true;
	}

	if (standby_index3 >= 0 && standby_index3 == pet_index)
	{
		if (out_index != NULL)*out_index = NEW_STANDBY_INDEX3;
		return true;
	}

	if (standby_index4 >= 0 && standby_index4 == pet_index)
	{
		if (out_index != NULL)*out_index = NEW_STANDBY_INDEX4;
		return true;
	}

	if (standby_index5 >= 0 && standby_index5 == pet_index)
	{
		if (out_index != NULL)*out_index = NEW_STANDBY_INDEX5;
		return true;
	}

	return false;
}

int Pet::Refined(int pet_index, bool is_refined_skill,bool is_auto_buy)
{
	if (this->PetInvalid(pet_index)) return -1;
	PetParam& pet_param = m_grid_list[pet_index].param->pet_param;

	const PetCfg *cfg = LOGIC_CONFIG->GetPetConfig()->GetPetCfg(pet_param.pet_id);
	if (NULL == cfg) return -2;

	if (cfg->quality == PET_QUALITY_GOD)		//神兽不能洗练
	{
		return -3;
	}

	{
		int has_item_count = m_role_module_mgr->GetKnapsack()->Count(cfg->refined_need_item_id);
		int total_price = 0, price_type = 0;
		int need_item_count = cfg->refined_need_item_num - has_item_count;
		if (need_item_count > 0)
		{
			if (!is_auto_buy)
			{
				Protocol::SCLackItem li;
				li.item_id = cfg->refined_need_item_id;
				li.item_count = need_item_count;
				m_role_module_mgr->NetSend(&li, sizeof(li));
				return false;
			}
			else
			{
				if (!MarketManager::Instance().GetItemPrice(cfg->refined_need_item_id, need_item_count, &price_type, &total_price))
				{
					m_role_module_mgr->NoticeNum(errornum::EN_WORKSHOP_WITHOUT_LEGAL_RESULTS);
					return false;
				}
				if (!m_role_module_mgr->GetMoney()->MoneyTypeMoreThan(price_type, total_price))
				{
					m_role_module_mgr->NoticeNum(errornum::EN_SILVER_COIN_NOT_ENOUGH);
					return false;
				}
			}
		}
		else
		{
			has_item_count = cfg->refined_need_item_num;
		}

		//消耗
		if (!m_role_module_mgr->GetKnapsack()->ConsumeItem(cfg->refined_need_item_id, has_item_count, __FUNCTION__))
		{
			return false;
		}

		if (total_price > 0)
		{
			if (!m_role_module_mgr->GetMoney()->UseMoneyType(price_type, total_price, __FUNCTION__))
			{
				return false;
			}
			EventHandler::Instance().OnMarketItemPurchasedByOtherSystem(m_role_module_mgr->GetRole(), cfg->refined_need_item_id, need_item_count, 
				total_price, price_type);
		}
	}

	gamelog::g_log_pet.printf(LL_INFO, "%s line:%d | %s old_%s", __FUNCTION__, __LINE__, m_role_module_mgr->GetRoleInfoString().c_str(), pet_param.ToString(this->GetPetUniqueId(pet_index)).c_str());

	pet_param.refined_flag = 0;
	pet_param.ClearNewGift();
	pet_param.ClearNewGrowth();

	const PetOtherCfg & other_cfg = LOGIC_CONFIG->GetPetConfig()->GetOtherCfgRef();
	bool is_mutant = false;

	bool can_mutant = true;
	if (cfg->pet_mutant_times >= 1 && pet_param.fei_sheng_times == 0)
	{
		can_mutant = false;
	}

	if(can_mutant)
	{
		//洗练变异概率
		int r = RandomNum(10000);
		int mutant_rate = other_cfg.refined_mutant_rate;
		if (0 == m_role_module_mgr->GetCommonData().is_pet_refine_flag)
		{
			if (m_role_module_mgr->GetCommonData().pet_refine_count > other_cfg.first_change_max)
			{
				mutant_rate = 10000;
			}
			else if (m_role_module_mgr->GetCommonData().pet_refine_count > other_cfg.first_change_min)
			{
				mutant_rate = other_cfg.first_change_mutant;
			}
		}

		if (r < mutant_rate)
		{
			int mutant_pet_id = LOGIC_CONFIG->GetPetConfig()->GetVariantPetByPetId(cfg->id);
			const PetCfg * mutant_cfg = LOGIC_CONFIG->GetPetConfig()->GetPetCfg(mutant_pet_id);
			if (mutant_cfg != NULL)
			{
				is_refined_skill = false;

				if (cfg->quality == PET_QUALITY_RARE || cfg->quality == PET_QUALITY_GOLD)
				{
					int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::pet_refine_bian_yi_broadcast,
						m_role_module_mgr->GetUid(), m_role_module_mgr->GetRole()->GetName(), m_grid_list[pet_index].item_id, m_grid_list[pet_index].item_id);
					if (length > 0)
					{
						World::GetInstWorld()->GetSceneManager()->SystemMsgAll(gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_ROLLING_NOTICE, SYS_MSG_SYSTEM_CHAT));
					}
				}

				int old_pet_id = pet_param.pet_id;
				m_grid_list[pet_index].item_id = mutant_pet_id;
				pet_param.pet_id = mutant_pet_id;

				if (std::string(pet_param.name) == cfg->name)
				{
					SNPRINTF(pet_param.name, sizeof(pet_param.name), "%s", mutant_cfg->name);
				}


				// 随机资质
				for (int i = 0; i < ADD_POINT_TYPE_MAX; ++i)
				{
					int min_attr = mutant_cfg->growth[i] - LOGIC_CONFIG->GetPetConfig()->GetAttrDecNumMax();
					if (pet_param.growth[i] < min_attr)
					{
						pet_param.growth[i] = min_attr;
					}

					if (pet_param.growth[i] > mutant_cfg->growth[i])
					{
						pet_param.growth[i] = mutant_cfg->growth[i];
					}
				}

				m_role_module_mgr->GetCommonData().is_pet_refine_flag = 1;
				this->ReCalcAttr(pet_index);

				if (this->IsInPlatfformBattle(m_grid_list[pet_index].item_unique_id))
				{
					this->SyncPetDataPlatformBattle(pet_index);
				}
				this->SendSingleInfo(pet_index, Protocol::SCSendPetSingleInfo::PET_NOTIE_TYPE_NO_TYPE);
				this->SendGetPetNotice(pet_index, GET_PET_REASON_TYPE_REFINED_MUTANT_PET);

				gamelog::g_log_pet.printf(LL_INFO, "%s line:%d | %s %s  refined mutan %d to %d", __FUNCTION__, __LINE__, m_role_module_mgr->GetRoleInfoString().c_str(), pet_param.GetPetBaseInfoString(this->GetPetUniqueId(pet_index)).c_str(), old_pet_id, mutant_pet_id);

				cfg = mutant_cfg;
				is_mutant = true;
			}
		}
	}

	if (is_refined_skill)
	{
		std::vector<int> get_gift_list = LOGIC_CONFIG->GetPetConfig()->GetRandPassive(cfg->passive_group_id_vec);

		if (get_gift_list.empty())
		{
			get_gift_list = LOGIC_CONFIG->GetPetConfig()->GetRandPassive(cfg->passive_group_id_lowest_vec);
		}

		for (int i = 0; i < (int)get_gift_list.size() && i < ARRAY_LENGTH(pet_param.new_passive_list); ++i)
		{
			pet_param.new_passive_list[i].passive_skill_id = get_gift_list[i];
		}

		pet_param.refined_flag |= PET_REFINED_FLAG_TYPE_PASSIVE;
	}

	{
		// 随机资质
		for (int i = 0; i < ADD_POINT_TYPE_MAX; ++i)
		{
			int rand_num = LOGIC_CONFIG->GetPetConfig()->RandAttrDecValue();
			pet_param.new_growth[i] = (unsigned char)(cfg->growth[i] - rand_num);
		}

		pet_param.refined_flag |= PET_REFINED_FLAG_TYPE_QUALIFICATIONS;
	}

	gamelog::g_log_pet.printf(LL_INFO, "%s line:%d | %s %s", __FUNCTION__, __LINE__, m_role_module_mgr->GetRoleInfoString().c_str(), pet_param.ToString(this->GetPetUniqueId(pet_index)).c_str());
	m_role_module_mgr->GetCommonData().pet_refine_count++;
	this->SendSingleInfo(pet_index, Protocol::SCSendPetSingleInfo::PET_NOTIE_TYPE_REFINED);

	this->SetItemDirty(pet_index, false);
	EventHandler::Instance().OnPetRefined(m_role_module_mgr->GetRole(), pet_index, cfg->quality, is_mutant);
	return 0;
}

int Pet::GuideRefined(int pet_index, bool is_refined_skill, bool is_auto_buy)
{
	if (this->PetInvalid(pet_index)) return -1;
	PetParam & pet_param = m_grid_list[pet_index].param->pet_param;

	const FuncGuideOtherCfg & guide_other_cfg = LOGIC_CONFIG->GetFuncGuideConfig()->GetOtherCfg();
	if (pet_param.pet_id != guide_other_cfg.guide_pet_id_2)
	{
		return -__LINE__;
	}

	const PetCfg * cfg = LOGIC_CONFIG->GetPetConfig()->GetPetCfg(pet_param.pet_id);
	if (NULL == cfg) return -2;

	if (cfg->quality == PET_QUALITY_GOD)		//神兽不能洗练
	{
		return -3;
	}

	{
		int has_item_count = m_role_module_mgr->GetKnapsack()->Count(cfg->refined_need_item_id);
		int total_price = 0, price_type = 0;
		int need_item_count = cfg->refined_need_item_num - has_item_count;
		if (need_item_count > 0)
		{
			if (!is_auto_buy)
			{
				Protocol::SCLackItem li;
				li.item_id = cfg->refined_need_item_id;
				li.item_count = need_item_count;
				m_role_module_mgr->NetSend(&li, sizeof(li));
				return false;
			}
			else
			{
				if (!MarketManager::Instance().GetItemPrice(cfg->refined_need_item_id, need_item_count, &price_type, &total_price))
				{
					m_role_module_mgr->NoticeNum(errornum::EN_WORKSHOP_WITHOUT_LEGAL_RESULTS);
					return false;
				}
				if (!m_role_module_mgr->GetMoney()->MoneyTypeMoreThan(price_type, total_price))
				{
					m_role_module_mgr->NoticeNum(errornum::EN_SILVER_COIN_NOT_ENOUGH);
					return false;
				}
			}
		}
		else
		{
			has_item_count = cfg->refined_need_item_num;
		}

		//消耗
		if (!m_role_module_mgr->GetKnapsack()->ConsumeItem(cfg->refined_need_item_id, has_item_count, __FUNCTION__))
		{
			return false;
		}

		if (total_price > 0)
		{
			if (!m_role_module_mgr->GetMoney()->UseMoneyType(price_type, total_price, __FUNCTION__))
			{
				return false;
			}
			EventHandler::Instance().OnMarketItemPurchasedByOtherSystem(m_role_module_mgr->GetRole(), cfg->refined_need_item_id, need_item_count,
				total_price, price_type);
		}
	}

	gamelog::g_log_pet.printf(LL_INFO, "%s line:%d | %s old_%s", __FUNCTION__, __LINE__, m_role_module_mgr->GetRoleInfoString().c_str(), pet_param.ToString(this->GetPetUniqueId(pet_index)).c_str());

	pet_param.refined_flag = 0;
	pet_param.ClearNewGift();
	pet_param.ClearNewGrowth();

	const PetOtherCfg & other_cfg = LOGIC_CONFIG->GetPetConfig()->GetOtherCfgRef();
	bool is_mutant = false;

	bool can_mutant = true;
	if (cfg->pet_mutant_times >= 1 && pet_param.fei_sheng_times == 0)
	{
		can_mutant = false;
	}

	if (can_mutant)
	{
		//洗练变异概率
		int r = RandomNum(10000);
		int mutant_rate = other_cfg.refined_mutant_rate;

		if (true || r < mutant_rate) //指引洗练肯定变异
		{
			int mutant_pet_id = LOGIC_CONFIG->GetPetConfig()->GetVariantPetByPetId(cfg->id);
			const PetCfg * mutant_cfg = LOGIC_CONFIG->GetPetConfig()->GetPetCfg(mutant_pet_id);
			if (mutant_cfg != NULL)
			{
				is_refined_skill = false;

				if (cfg->quality == PET_QUALITY_RARE || cfg->quality == PET_QUALITY_GOLD)
				{
					int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::pet_refine_bian_yi_broadcast,
						m_role_module_mgr->GetUid(), m_role_module_mgr->GetRole()->GetName(), m_grid_list[pet_index].item_id, m_grid_list[pet_index].item_id);
					if (length > 0)
					{
						World::GetInstWorld()->GetSceneManager()->SystemMsgAll(gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_ROLLING_NOTICE, SYS_MSG_SYSTEM_CHAT));
					}
				}

				int old_pet_id = pet_param.pet_id;
				m_grid_list[pet_index].item_id = mutant_pet_id;
				pet_param.pet_id = mutant_pet_id;

				if (std::string(pet_param.name) == cfg->name)
				{
					SNPRINTF(pet_param.name, sizeof(pet_param.name), "%s", mutant_cfg->name);
				}


				// 随机资质
				for (int i = 0; i < ADD_POINT_TYPE_MAX; ++i)
				{
					int min_attr = mutant_cfg->growth[i] - LOGIC_CONFIG->GetPetConfig()->GetAttrDecNumMax();
					if (pet_param.growth[i] < min_attr)
					{
						pet_param.growth[i] = min_attr;
					}

					if (pet_param.growth[i] > mutant_cfg->growth[i])
					{
						pet_param.growth[i] = mutant_cfg->growth[i];
					}
				}

				m_role_module_mgr->GetCommonData().is_pet_refine_flag = 1;
				this->ReCalcAttr(pet_index);

				if (this->IsInPlatfformBattle(m_grid_list[pet_index].item_unique_id))
				{
					this->SyncPetDataPlatformBattle(pet_index);
				}
				this->SendSingleInfo(pet_index, Protocol::SCSendPetSingleInfo::PET_NOTIE_TYPE_NO_TYPE);
				this->SendGetPetNotice(pet_index, GET_PET_REASON_TYPE_REFINED_MUTANT_PET);

				gamelog::g_log_pet.printf(LL_INFO, "%s line:%d | %s %s  refined mutan %d to %d", __FUNCTION__, __LINE__, m_role_module_mgr->GetRoleInfoString().c_str(), pet_param.GetPetBaseInfoString(this->GetPetUniqueId(pet_index)).c_str(), old_pet_id, mutant_pet_id);

				cfg = mutant_cfg;
				is_mutant = true;
			}
		}
	}


	if (is_refined_skill)
	{
		pet_param.guide_pet_flag |= GUIDE_PET_FLAG_FIRST_REFINE;
		std::vector<int> get_gift_list = cfg->guide_passive_refined_vec;
		for (int i = 0; i < (int)get_gift_list.size() && i < ARRAY_LENGTH(pet_param.new_passive_list); ++i)
		{
			pet_param.new_passive_list[i].passive_skill_id = get_gift_list[i];
		}

		pet_param.refined_flag |= PET_REFINED_FLAG_TYPE_PASSIVE;
	}

	{
		// 随机资质
		for (int i = 0; i < ADD_POINT_TYPE_MAX; ++i)
		{
			int rand_num = LOGIC_CONFIG->GetPetConfig()->RandAttrDecValue();
			pet_param.new_growth[i] = (unsigned char)(cfg->growth[i] - rand_num);
		}

		pet_param.refined_flag |= PET_REFINED_FLAG_TYPE_QUALIFICATIONS;
	}

	gamelog::g_log_pet.printf(LL_INFO, "%s line:%d | %s %s", __FUNCTION__, __LINE__, m_role_module_mgr->GetRoleInfoString().c_str(), pet_param.ToString(this->GetPetUniqueId(pet_index)).c_str());

	this->SendSingleInfo(pet_index, Protocol::SCSendPetSingleInfo::PET_NOTIE_TYPE_REFINED);
	this->SetItemDirty(pet_index, false);

	return 0;
}
int Pet::RefinedSave(int pet_index)
{
	if (this->PetInvalid(pet_index)) return -1;
	PetParam & pet_param = m_grid_list[pet_index].param->pet_param;

	const PetCfg *cfg = LOGIC_CONFIG->GetPetConfig()->GetPetCfg(pet_param.pet_id);
	if(NULL == cfg) return -2;

	if (pet_param.refined_flag == 0)
	{
		return -3;
	}

	gamelog::g_log_pet.printf(LL_INFO, "%s line:%d | %s old_%s", __FUNCTION__, __LINE__, m_role_module_mgr->GetRoleInfoString().c_str(), pet_param.ToString(this->GetPetUniqueId(pet_index)).c_str());

	if (pet_param.refined_flag & PET_REFINED_FLAG_TYPE_PASSIVE)
	{
		for (int i = 0; i < ARRAY_LENGTH(pet_param.passive_list) && i < ARRAY_LENGTH(pet_param.new_passive_list); ++i)
		{
			pet_param.passive_list[i] = pet_param.new_passive_list[i];
			pet_param.passive_list[i].passive_source = PET_PASSIVE_SOURCE_TYPE_DEFAULT;
			pet_param.new_passive_list[i].passive_skill_id = 0;
		}
	}

	if (pet_param.refined_flag & PET_REFINED_FLAG_TYPE_QUALIFICATIONS)
	{
		for (int i = 0; i < ARRAY_LENGTH(pet_param.growth) && i < ARRAY_LENGTH(pet_param.new_growth); ++i)
		{
			pet_param.growth[i] = pet_param.new_growth[i];
			pet_param.new_growth[i] = 0;
		}
	}
	bool is_max_growth = true;
	for (int i = 0; i < ADD_POINT_TYPE_MAX; i++)
	{
		if (pet_param.growth[i] != cfg->growth[i])
		{
			is_max_growth = false;
		}
	}

	pet_param.refined_flag = 0;
	pet_param.comprehend_gift_num = 0;

	gamelog::g_log_pet.printf(LL_INFO, "%s line:%d | %s %s", __FUNCTION__, __LINE__, m_role_module_mgr->GetRoleInfoString().c_str(), pet_param.ToString(this->GetPetUniqueId(pet_index)).c_str());

	this->ReCalcAttr(pet_index);
	EventHandler::Instance().OnPetChangeTalent(m_role_module_mgr->GetRole(), pet_index, &pet_param);
	this->SetItemDirty(pet_index, true, Protocol::SCSendPetSingleInfo::PET_NOTIE_TYPE_REFINED_SAVE);
	if (this->IsInPlatfformBattle(m_grid_list[pet_index].item_unique_id))
	{
		this->SyncPetDataPlatformBattle(pet_index);
	}

	if (is_max_growth)			//此处放在计算属性后面
	{
		int sign_id1 = 0;
		int sign_id2 = 0;
		ItemDataWrapper item_info;
		m_grid_list[pet_index].WrapTo(&item_info);
		ChatManager::Instance().AddChatItemInfo(&sign_id1, &sign_id2, item_info, &m_pet_base_attr[pet_index]);

		Protocol::SCSystemMsg sm;
		int sendlen = 0;
		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_system_post_pet_max, m_role_module_mgr->GetRole()->GetName(), sign_id1, sign_id2, 
			m_grid_list[pet_index].item_id, cfg->quality);
		if (length > 0)
		{
			if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_SYSTEM_CHAT)))
			{
				World::GetInstWorld()->SendToAllGateway((const char*)&sm, sendlen);
			}
		}
	}
	return 0;
}

int Pet::RefinedGiveUp(int pet_index)
{
	if (this->PetInvalid(pet_index)) return -1;
	PetParam & pet_param = m_grid_list[pet_index].param->pet_param;

	if (pet_param.refined_flag == 0)
	{
		return -2;
	}

	pet_param.refined_flag = 0;
	for (int i = 0; i < ARRAY_LENGTH(pet_param.new_passive_list); ++i)
	{
		pet_param.new_passive_list[i].passive_skill_id = 0;
	}

	for (int i = 0; i < ARRAY_LENGTH(pet_param.new_growth); ++i)
	{
		pet_param.new_growth[i] = 0;
	}

	this->SetItemDirty(pet_index, true, Protocol::SCSendPetSingleInfo::PET_NOTIE_TYPE_REFINED_SAVE);
	return 0;
}

int Pet::ComprehendPassiveSkill(int pet_index)
{
	if (this->PetInvalid(pet_index)) return -1;
	PetParam & pet_param = m_grid_list[pet_index].param->pet_param;

	const PetCfg * cfg = LOGIC_CONFIG->GetPetConfig()->GetPetCfg(pet_param.pet_id);
	if (NULL == cfg) return -2;

	const PetComprehendPassiveCfg * comprehend_cfg = LOGIC_CONFIG->GetPetConfig()->GetPetComprehendPassiveCfg(cfg->comprehend_id);
	if (NULL == comprehend_cfg) return -3;


	int free_index = -1;
	std::set<int> have_gift_skill;
	for (int i = 0; i < ARRAY_LENGTH(pet_param.passive_list); ++i)
	{
		if (pet_param.passive_list[i].passive_skill_id != 0)
		{
			have_gift_skill.insert(pet_param.passive_list[i].passive_skill_id);
		}
		else if (free_index == -1)
		{
			free_index = i;
		}
	}

	if (free_index == -1)
	{
		return -49;
	}

	int new_passive_id = comprehend_cfg->ComprehendOnePassive(have_gift_skill);
	if (new_passive_id == 0 || (int)have_gift_skill.size() >= ARRAY_LENGTH(pet_param.passive_list) || (int)have_gift_skill.size() >= cfg->max_passive_skill_num)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_PET_CAN_NOT_COMPREHEND_GIFT);
		return -5;
	}

	{
		std::set<int> all = have_gift_skill;
		all.insert(new_passive_id);
		int element_skill_num = 0;
		for (std::set<int>::iterator iter = all.begin(); iter != all.end(); ++iter)
		{
			const GamePassiveSkillCfg * passive_skill_cfg = SkillPool::Instance().GetPassiveSkillCfg(*iter);
			if (passive_skill_cfg == NULL)
			{
				continue;
			}

			if (passive_skill_cfg->IsElementalSkillType())
				element_skill_num += 1;
		}

		if (element_skill_num > 1)
		{
			return -__LINE__;
		}
	}
	
	if (have_gift_skill.size() > 0 && !m_role_module_mgr->GetKnapsack()->ConsumeItem(comprehend_cfg->need_item_id, comprehend_cfg->GetComprehendPassiveConsumeItemNum(have_gift_skill.size()), __FUNCTION__))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_ITEM_AMOUNT_NOT_ENOUGH);
		return -6;
	}

	bool is_succ = false;
	int r = rand() % PetComprehendPassiveCfg::PET_COMPREHEND_PASSIVE_MAX_RATE;
	if (r < comprehend_cfg->rate)
	{
		is_succ = true;
	}
	else
	{
		pet_param.comprehend_passive_value += comprehend_cfg->add_value;
		if (pet_param.comprehend_passive_value >= comprehend_cfg->max_value)
		{
			is_succ = true;
		}
	}

	if (is_succ)
	{
		pet_param.comprehend_passive_value = 0;
		pet_param.comprehend_gift_num += 1;
		pet_param.passive_list[free_index].passive_skill_id = new_passive_id;
		pet_param.passive_list[free_index].passive_source = PET_PASSIVE_SOURCE_TYPE_COMPREHEND;

		gamelog::g_log_pet.printf(LL_INFO, "%s line:%d |%s  %s new_passive_id:%d", __FUNCTION__, __LINE__, m_role_module_mgr->GetRoleInfoString().c_str(), pet_param.GetPetBaseInfoString(this->GetPetUniqueId(pet_index)).c_str(), new_passive_id);

		this->ReCalcAttr(pet_index);
		EventHandler::Instance().OnComprehendPassiveSkill(m_role_module_mgr->GetRole(), pet_index, &pet_param, free_index);
		if (this->IsInPlatfformBattle(m_grid_list[pet_index].item_unique_id))
		{
			this->SyncPetDataPlatformBattle(pet_index);
		}
	}

	gamelog::g_log_pet.printf(LL_INFO, "%s line:%d |%s  %s comprehend_passive_value:%d", __FUNCTION__, __LINE__, m_role_module_mgr->GetRoleInfoString().c_str(), pet_param.GetPetBaseInfoString(this->GetPetUniqueId(pet_index)).c_str()
				  , pet_param.comprehend_passive_value);

	this->SetItemDirty(pet_index, true, Protocol::SCSendPetSingleInfo::PET_NOTIE_TYPE_COMPREHEND_PASSIVE);
	return 0;
}

struct PetReplacePassiveSkillItem
{
	PetReplacePassiveSkillItem(int skill_id, unsigned char skill_index, unsigned char skill_type)
	{
		this->skill_id = skill_id;
		this->skill_index = skill_index;
		this->skill_type = skill_type;
	}


	int skill_id;
	unsigned char skill_index;
	unsigned char skill_type;
};

bool Pet::ReplacePassiveSkill(int pet_index, int passive_skill_id, int need_pet_quality, std::set<PetReplacePassiveSkillLockItem> lock_index)
{
	if (this->PetInvalid(pet_index)) return false;
	PetParam& pet_param = m_grid_list[pet_index].param->pet_param;

	const PetCfg *cfg = LOGIC_CONFIG->GetPetConfig()->GetPetCfg(pet_param.pet_id);
	if (NULL == cfg) return false;

	if (need_pet_quality != 0 && cfg->quality != need_pet_quality)
	{
		return false;
	}

	const GamePassiveSkillCfg * target_skill_cfg = SkillPool::Instance().GetPassiveSkillCfg(passive_skill_id);
	if (target_skill_cfg == NULL)
	{
		return false;
	}

	//锁技能的判断和消耗
	//锁定限制
	const PetOtherCfg * other_cfg = LOGIC_CONFIG->GetPetConfig()->GetOtherCfg();
	if (other_cfg == NULL)
	{
		return false;
	}

	int lock_num = (int)lock_index.size();

	if (pet_param.GetPetPassiveSkillAndLianYaoPassiveSkillNum() >= other_cfg->skill_stage_2)
	{
		if (lock_num > 2)
		{
			return false;
		}
	}
	else
	{
		if (pet_param.GetPetPassiveSkillAndLianYaoPassiveSkillNum() >= other_cfg->skill_stage_1)
		{
			if (lock_num > 1)
			{
				return false;
			}
		}
	}

	const PetLockSkillCfg * lock_cfg = LOGIC_CONFIG->GetPetConfig()->GetPetLockSkillCfg(cfg->quality);
	if (lock_cfg == NULL)
	{
		return false;
	}

	std::vector<PetReplacePassiveSkillItem> skill_list;
	for (int i = 0; i < ARRAY_LENGTH(pet_param.passive_list); ++i)
	{
		const PetPassiveItemInfo & curr = pet_param.passive_list[i];
		if (curr.passive_skill_id == 0)
		{
			continue;
		}

		PetReplacePassiveSkillItem item(curr.passive_skill_id, i, PET_REPLACE_SKILL_INDEX_TYPE_DEFAULT);
		skill_list.push_back(item);
	}

	for (int i = 0; i < ARRAY_LENGTH(pet_param.lian_yao_passive_list); ++i)
	{
		const PetPassiveItemInfo & curr = pet_param.lian_yao_passive_list[i];
		if (curr.passive_skill_id == 0)
		{
			continue;
		}

		PetReplacePassiveSkillItem item(curr.passive_skill_id, i, PET_REPLACE_SKILL_INDEX_TYPE_LIAN_YAO);
		skill_list.push_back(item);
	}


	//1、宠物普通被动技能 同类高级技能和低级技能共存时 应该取高级技能的效果
	//2、宠物元素被动技能 同类高级元素技能和低级元素技能不可共存且会被普通技能顶掉 不同元素被动技能必定顶掉原有元素被动
	int replace_gift_idnex = -1;
	std::vector<int> can_replace_skill_index;
	for (int i = 0; i < (int)skill_list.size(); ++i)
	{
		const PetReplacePassiveSkillItem & curr = skill_list[i];
		if (curr.skill_id == passive_skill_id)
		{
			m_role_module_mgr->NoticeNum(errornum::EN_REPEAT_PET_SKILL);
			return false;
		}
	
		const GamePassiveSkillCfg * passive_skill_cfg = SkillPool::Instance().GetPassiveSkillCfg(curr.skill_id);
		if (passive_skill_cfg == NULL)
		{
			return false;
		}

		PetReplacePassiveSkillLockItem tmp(curr.skill_type, curr.skill_index);
		bool is_lock = lock_index.find(tmp) != lock_index.end();

		if (target_skill_cfg->IsElementalSkillType() && passive_skill_cfg->IsElementalSkillType())
		{
			if (target_skill_cfg->skill_type == passive_skill_cfg->skill_type && target_skill_cfg->skill_priority <= passive_skill_cfg->skill_priority)
			{
				m_role_module_mgr->NoticeNum(errornum::EN_CAN_NOT_REPLACE_PET_SKILL);
				return false;
			}

			if (is_lock)
			{		
				m_role_module_mgr->NoticeNum(errornum::EN_CAN_NOT_REPLACE_PET_SKILL_LOCKED_2);				
				m_role_module_mgr->NoticeNum(errornum::EN_CAN_NOT_REPLACE_PET_SKILL_LOCKED);
				return false;
			}

			replace_gift_idnex = i;
		}

		if (is_lock)
		{
			continue;
		}

		can_replace_skill_index.push_back(i);		
	}

	if (replace_gift_idnex == -1)
	{
		if (can_replace_skill_index.empty())
		{
			return false;
		}

		int r = RandomNum((int)can_replace_skill_index.size());
		replace_gift_idnex = can_replace_skill_index[r];
	}

	int consume_num = lock_cfg->GetConsume(lock_num);
	if (consume_num > 0 && !m_role_module_mgr->GetKnapsack()->ConsumeItem(lock_cfg->need_item_id, consume_num, __FUNCTION__, 0, true))
	{
		return false;
	}

	int old_replace_index = pet_param.replace_index;
	int old_replace_skill_id = pet_param.replace_skill_id;
	int old_replace_type = pet_param.replace_type;

	const PetReplacePassiveSkillItem & replace_item = skill_list[replace_gift_idnex];
	pet_param.replace_skill_id = passive_skill_id;
	pet_param.replace_index = replace_item.skill_index;	
	pet_param.replace_type = replace_item.skill_type;
	gamelog::g_log_pet.printf(LL_INFO, "%s line:%d | %s %s oldtype:%d oldindex:%d oldid:%d type:%d index:%d id:%d", __FUNCTION__, __LINE__, m_role_module_mgr->GetRoleInfoString().c_str(), pet_param.GetPetBaseInfoString(this->GetPetUniqueId(pet_index)).c_str(),
		old_replace_type, old_replace_index, old_replace_skill_id, pet_param.replace_type, pet_param.replace_index, pet_param.replace_skill_id);

	this->SetItemDirty(pet_index, true, Protocol::SCSendPetSingleInfo::PET_NOTIE_TYPE_REPLACE_PASSIVE);
	EventHandler::Instance().OnUseSkillBook(m_role_module_mgr->GetRole(), pet_index, replace_gift_idnex, passive_skill_id, cfg->quality, 
		target_skill_cfg->skill_priority);
	return true;
}

int Pet::ReplacePassiveSkillSave(int pet_index,bool is_save)
{
	if (this->PetInvalid(pet_index)) return false;
	PetParam & pet_param = m_grid_list[pet_index].param->pet_param;

	if (is_save)
	{
		int replace_skill_id = pet_param.replace_skill_id;
		int replace_gift_idnex = pet_param.replace_index;
		int replace_gift_type = pet_param.replace_type;	
		if (replace_skill_id == 0)
		{
			return -__LINE__;
		}

		int old_id = 0;
		switch (replace_gift_type)
		{
			case PET_REPLACE_SKILL_INDEX_TYPE_DEFAULT:
				{
					if (replace_gift_idnex < 0 || replace_gift_idnex >= ARRAY_LENGTH(pet_param.passive_list))
					{
						return -__LINE__;
					}

					old_id = pet_param.passive_list[replace_gift_idnex].passive_skill_id;
					pet_param.passive_list[replace_gift_idnex].Reset();
					pet_param.passive_list[replace_gift_idnex].passive_skill_id = replace_skill_id;
				}
				break;
			case PET_REPLACE_SKILL_INDEX_TYPE_LIAN_YAO:
				{
					if (replace_gift_idnex < 0 || replace_gift_idnex >= ARRAY_LENGTH(pet_param.lian_yao_passive_list))
					{
						return -__LINE__;
					}

					old_id = pet_param.lian_yao_passive_list[replace_gift_idnex].passive_skill_id;
					pet_param.lian_yao_passive_list[replace_gift_idnex].Reset();
					pet_param.lian_yao_passive_list[replace_gift_idnex].passive_skill_id = replace_skill_id;
				}
				break;
			default:
				return -__LINE__;
		}

		gamelog::g_log_pet.printf(LL_INFO, "%s line:%d | %s %s old_id:%d type:%d index:%d id:%d", __FUNCTION__, __LINE__, m_role_module_mgr->GetRoleInfoString().c_str(), pet_param.GetPetBaseInfoString(this->GetPetUniqueId(pet_index)).c_str(),
			old_id, replace_gift_type, replace_gift_idnex, replace_skill_id);
	}

	pet_param.replace_skill_id = 0;
	pet_param.replace_index = 0;	
	pet_param.replace_type = 0;

	gamelog::g_log_pet.printf(LL_INFO, "%s line:%d | %s %s is_save:%d", __FUNCTION__, __LINE__, m_role_module_mgr->GetRoleInfoString().c_str(), pet_param.GetPetBaseInfoString(this->GetPetUniqueId(pet_index)).c_str(),(int)is_save);

	this->ReCalcAttr(pet_index);
	this->SetItemDirty(pet_index, true, Protocol::SCSendPetSingleInfo::PET_NOTIE_TYPE_REPLACE_PASSIVE_SAVE);

	return 0;
}

int Pet::Compose(int seq, int use_shards_num, int get_pet_reason_type, int * ARG_OUT pet_index_ptr)
{
	if (!this->HaveSpace(1, false))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_PET_COUNT_LIMIT2);
		return -__LINE__;
	}

	const PetOtherCfg & other_cfg = LOGIC_CONFIG->GetPetConfig()->GetOtherCfgRef();

	const PetComposeCfg *  compose_cfg = LOGIC_CONFIG->GetPetConfig()->GetPetComposeCfg(seq);
	if (compose_cfg == NULL)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return -__LINE__;
	}

	int pet_id = LOGIC_CONFIG->GetPetConfig()->GetPetComposeRandPetId(compose_cfg->compose_reward_id);
	if (pet_id == 0)
	{
		return -__LINE__;
	}

	const PetCfg* pet_cfg = LOGIC_CONFIG->GetPetConfig()->GetPetCfg(pet_id);
	if (NULL == pet_cfg)
	{
		return -__LINE__;
	}

	std::vector<ItemExtern::ItemConsumeConfig> stuff_list;

	int pet_need_item_num = 0;
	int share_item_id = 0;
	switch (compose_cfg->can_use_common_shards)
	{		
		case PET_COMPOSE_SHARE_TYPE_NO:
		case PET_COMPOSE_SHARE_TYPE_NO_2:
		{
			pet_need_item_num = compose_cfg->need_item_num;
			use_shards_num = 0;
		}
		break;
		case PET_COMPOSE_SHARE_TYPE_COMMON:
		case PET_COMPOSE_SHARE_TYPE_XIYOU:
		{
			if (use_shards_num < 0 || use_shards_num > compose_cfg->need_item_num)
			{
				return -__LINE__;
			}

			pet_need_item_num = compose_cfg->need_item_num - use_shards_num;
			if (pet_need_item_num < compose_cfg->personal_patch_num)
			{
				m_role_module_mgr->NoticeNum(errornum::EN_PET_COMPOSE_FAILD_NEED_PET_ITEM_NOT_ENOUGH);
				return -__LINE__;
			}
			if (PET_COMPOSE_SHARE_TYPE_COMMON == compose_cfg->can_use_common_shards)
			{
				share_item_id = other_cfg.common_shards_item_id;
			}
			else
			{
				share_item_id = other_cfg.rare_fragment_item_id;
			}
		}
		break;
	default:
		return __LINE__;
	}

	if (pet_need_item_num > 0)
	{
		ItemExtern::ItemConsumeConfig  stuff_con;
		stuff_con.item_id = compose_cfg->need_item_id;
		stuff_con.num = pet_need_item_num;
		stuff_con.buyable = false;
		stuff_list.push_back(stuff_con);
	}
	if (use_shards_num > 0)
	{
		ItemExtern::ItemConsumeConfig  stuff_con2;
		stuff_con2.item_id = share_item_id;
		stuff_con2.num = use_shards_num;
		stuff_con2.buyable = false;
		stuff_list.push_back(stuff_con2);
	}

	if(stuff_list.empty()) return -__LINE__;

	ItemExtern::ItemConsumeList consume_list;
	if (!ItemExtern::GetConsumeListByOrder(m_role_module_mgr->GetRole(), false, (int)stuff_list.size(), &stuff_list[0], &consume_list, false))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_ITEM_AMOUNT_NOT_ENOUGH);
		return -__LINE__;
	}

	if (consume_list.need_gold_num > 0 && !m_role_module_mgr->GetMoney()->GoldMoreThan(consume_list.need_gold_num))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_GOLD_NOT_ENOUGH);
		return -__LINE__;
	}

	if (consume_list.count > 0 && !m_role_module_mgr->GetKnapsack()->ConsumeItemByIndexList(consume_list.count, consume_list.column_list,
												consume_list.index_list, consume_list.num_list, __FUNCTION__))
	{
		return -__LINE__;
	}

	if (consume_list.need_gold_num > 0)
	{
		if (!m_role_module_mgr->GetMoney()->UseGold(consume_list.need_gold_num, __FUNCTION__))
		{
			return -__LINE__;
		}
	}

	int pet_index = this->AcquirePet(pet_id, __FUNCTION__, 1, false, get_pet_reason_type, compose_cfg->is_bind);
	if (pet_index < 0)
	{
		return -__LINE__;
	}

	if (pet_index_ptr)
	{
		*pet_index_ptr = pet_index;
	}

	EventHandler::Instance().OnPetCompose(m_role_module_mgr->GetRole(), true);
	return 0;
}

int Pet::Convert(int seq)
{
	if (!this->HaveSpace(1, false))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_PET_COUNT_LIMIT2);
		return -__LINE__;
	}

	const PetConvertCfg * convert_cfg = LOGIC_CONFIG->GetPetConfig()->GetPetConvertCfg(seq);
	if (NULL == convert_cfg)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return -__LINE__;
	}
	const PetCfg* pet_cfg = LOGIC_CONFIG->GetPetConfig()->GetPetCfg(convert_cfg->convert_item.item_id);
	if (NULL == pet_cfg)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return -__LINE__;
	}

	if (!m_role_module_mgr->GetKnapsack()->ConsumeItem(convert_cfg->consume_item_id, convert_cfg->consume_num, __FUNCTION__))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_ITEM_AMOUNT_NOT_ENOUGH);
		return -__LINE__;
	}

	if (this->AcquirePet(convert_cfg->convert_item.item_id, __FUNCTION__, 1, false, GET_PET_REASON_TYPE_CONVERT_PET) >= 0)
	{
		EventHandler::Instance().OnPetCompose(m_role_module_mgr->GetRole(), false);
	}
	else
	{
		return -__LINE__;
	}

	if (pet_cfg->quality == PET_QUALITY_GOD)
	{
		static Protocol::SCSystemMsg sm;
		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_pet_compose, m_role_module_mgr->GetRole()->GetName(), pet_cfg->quality, convert_cfg->convert_item.item_id);
		int sendlen = 0;
		if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_SYSTEM_CHAT)))
		{
			World::GetInstWorld()->SendToAllGateway((const char*)&sm, sendlen);
		}
	}
	
	return 0;
}

int Pet::GuideReplacePassiveSkill(int pet_index)
{
	if (this->PetInvalid(pet_index)) return false;
	PetParam & pet_param = m_grid_list[pet_index].param->pet_param;

	const FuncGuideOtherCfg & other_cfg = LOGIC_CONFIG->GetFuncGuideConfig()->GetOtherCfg();
	int passive_skill_id = other_cfg.guide_replace_passive_skill_id;
	std::set<PetReplacePassiveSkillLockItem> lock_index;

	if (pet_param.pet_id != other_cfg.guide_pet_id)
	{
		return -__LINE__;
	}

	if (!m_role_module_mgr->GetKnapsack()->HasItem(other_cfg.guide_replace_passive_skill_item_id, 1))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_ITEM_AMOUNT_NOT_ENOUGH);
		return -__LINE__;
	}

	if (!this->ReplacePassiveSkill(pet_index, passive_skill_id, 0, lock_index))
	{
		return -__LINE__;
	}

	m_role_module_mgr->GetKnapsack()->ConsumeItem(other_cfg.guide_replace_passive_skill_item_id, 1, __FUNCTION__);
	return 0;
}

int Pet::GuideComprehendPassiveSkill(int pet_index)
{
	if (this->PetInvalid(pet_index)) return -__LINE__;	
	PetParam & pet_param = m_grid_list[pet_index].param->pet_param;

	const FuncGuideOtherCfg & other_cfg = LOGIC_CONFIG->GetFuncGuideConfig()->GetOtherCfg();

	if (pet_param.pet_id != other_cfg.guide_pet_id)
	{
		return -__LINE__;
	}

	const PetCfg * cfg = LOGIC_CONFIG->GetPetConfig()->GetPetCfg(pet_param.pet_id);
	if (NULL == cfg) return -__LINE__;

	const PetComprehendPassiveCfg * comprehend_cfg = LOGIC_CONFIG->GetPetConfig()->GetPetComprehendPassiveCfg(cfg->comprehend_id);
	if (NULL == comprehend_cfg) return -__LINE__;

	int free_index = -1;
	std::set<int> have_gift_skill;
	for (int i = 0; i < ARRAY_LENGTH(pet_param.passive_list); ++i)
	{
		if (pet_param.passive_list[i].passive_skill_id != 0)
		{
			have_gift_skill.insert(pet_param.passive_list[i].passive_skill_id);
		}
		else if (free_index == -1)
		{
			free_index = i;
		}
	}

	if (free_index == -1)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_PET_CAN_NOT_COMPREHEND_GIFT);
		return -__LINE__;
	}

	int new_passive_id = 0;
	if (have_gift_skill.find(other_cfg.guide_comprehend_skill_id) == have_gift_skill.end())
	{
		new_passive_id = other_cfg.guide_comprehend_skill_id;
	}
	else
	{
		new_passive_id = comprehend_cfg->ComprehendOnePassive(have_gift_skill);
	}

	if (new_passive_id == 0 || (int)have_gift_skill.size() >= ARRAY_LENGTH(pet_param.passive_list) || (int)have_gift_skill.size() >= cfg->max_passive_skill_num)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_PET_CAN_NOT_COMPREHEND_GIFT);
		return -__LINE__;
	}

	{
		std::set<int> all = have_gift_skill;
		all.insert(new_passive_id);
		int element_skill_num = 0;
		for (std::set<int>::iterator iter = all.begin(); iter != all.end(); ++iter)
		{
			const GamePassiveSkillCfg * passive_skill_cfg = SkillPool::Instance().GetPassiveSkillCfg(*iter);
			if (passive_skill_cfg == NULL)
			{
				continue;
			}

			if (passive_skill_cfg->IsElementalSkillType())
				element_skill_num += 1;
		}

		if (element_skill_num > 1)			
		{
			return -__LINE__;
		}		
	}

	if (!m_role_module_mgr->GetKnapsack()->ConsumeItem(comprehend_cfg->need_item_id, comprehend_cfg->GetComprehendPassiveConsumeItemNum(have_gift_skill.size()), __FUNCTION__))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_ITEM_AMOUNT_NOT_ENOUGH);
		return -__LINE__;
	}

	pet_param.guide_pet_flag |= GUIDE_PET_FLAG_COMPREHEND;
	pet_param.comprehend_gift_num += 1;
	pet_param.passive_list[free_index].passive_skill_id = new_passive_id;
	pet_param.passive_list[free_index].passive_source = PET_PASSIVE_SOURCE_TYPE_COMPREHEND;

	gamelog::g_log_pet.printf(LL_INFO, "%s line:%d | %s %s  new_id:%d", __FUNCTION__, __LINE__, m_role_module_mgr->GetRoleInfoString().c_str(), pet_param.GetPetBaseInfoString(this->GetPetUniqueId(pet_index)).c_str(), new_passive_id);

	this->ReCalcAttr(pet_index);
	EventHandler::Instance().OnComprehendPassiveSkill(m_role_module_mgr->GetRole(), pet_index, &pet_param, free_index);
	this->SetItemDirty(pet_index, true, Protocol::SCSendPetSingleInfo::PET_NOTIE_TYPE_COMPREHEND_PASSIVE);

	return 0;
}

int Pet::Mutant(int pet_index)
{
	if (this->PetInvalid(pet_index)) return -__LINE__;
	PetParam & pet_param = m_grid_list[pet_index].param->pet_param;

	//const PetOtherCfg & other_cfg = LOGIC_CONFIG->GetPetConfig()->GetOtherCfgRef();
	const PetCfg * cfg = LOGIC_CONFIG->GetPetConfig()->GetPetCfg(pet_param.pet_id);
	if (NULL == cfg) return -__LINE__;

	if (cfg->mutant_consume_item_num == 0)
	{
		return -__LINE__;
	}

	if (cfg->pet_mutant_times >= 1)
	{
		//二次变异需要飞升
		if (pet_param.fei_sheng_times == 0)
		{
			m_role_module_mgr->NoticeNum(errornum::EN_PET_TWO_MUTANT_NEED_FEI_SHENG);
			return -__LINE__;
		}
	}

	int mutant_pet_id = LOGIC_CONFIG->GetPetConfig()->GetVariantPetByPetId(cfg->id);
	const PetCfg * mutant_cfg = LOGIC_CONFIG->GetPetConfig()->GetPetCfg(mutant_pet_id);
	if (mutant_cfg == NULL)
	{
		return -__LINE__;
	}

	if (!m_role_module_mgr->GetKnapsack()->ConsumeItem(cfg->mutant_consume_item_id, cfg->mutant_consume_item_num, __FUNCTION__))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_ITEM_AMOUNT_NOT_ENOUGH);
		return -__LINE__;
	}

	int old_pet_id = pet_param.pet_id;
	m_grid_list[pet_index].item_id = mutant_pet_id;
	pet_param.pet_id = mutant_pet_id;

	if (std::string(pet_param.name) == cfg->name)
	{
		SNPRINTF(pet_param.name, sizeof(pet_param.name), "%s", mutant_cfg->name);
	}

	for (int i = 0; i < ADD_POINT_TYPE_MAX; ++i)
	{
		int rand_num = 0;
		if (mutant_cfg->quality < PET_QUALITY_GOD)
		{
			rand_num = LOGIC_CONFIG->GetPetConfig()->RandAttrDecValue();
		}
		pet_param.growth[i] = (unsigned char)(mutant_cfg->growth[i] - rand_num);
	}

	pet_param.ClearNewGift();
	pet_param.ClearNewGrowth();
	pet_param.pet_growth_version = LOGIC_CONFIG->GetPetConfig()->GetPetCurGrowthVersion(pet_param.pet_id);
	this->ReCalcAttr(pet_index);
	if (this->IsInPlatfformBattle(m_grid_list[pet_index].item_unique_id))
	{
		this->SyncPetDataPlatformBattle(pet_index);
	}

	this->SetItemDirty(pet_index, false);
	this->SendSingleInfo(pet_index, Protocol::SCSendPetSingleInfo::PET_NOTIE_TYPE_NO_TYPE);
	this->SendGetPetNotice(pet_index, GET_PET_REASON_TYPE_USE_ITEM_MUTANT_PET);

	gamelog::g_log_pet.printf(LL_INFO, "%s line:%d | %s %s  mutan %d to %d", __FUNCTION__, __LINE__, m_role_module_mgr->GetRoleInfoString().c_str(), pet_param.GetPetBaseInfoString(this->GetPetUniqueId(pet_index)).c_str(), old_pet_id, mutant_pet_id);

	return 0;
}

int Pet::OnPetBenediction(int main_pet_index, int vice_pet_index)
{
	if(!FunOpen::IsFunOpen(m_role_module_mgr->GetRole(), FUN_TYPE_PET_BENEDICTION, true)) return -99;

	if(this->PetInvalid(main_pet_index) || this->PetInvalid(vice_pet_index) || main_pet_index == vice_pet_index) return -1;

	int before_main_pet_index = m_common_param.benediction_pet_index_list[vice_pet_index];
	if (main_pet_index == before_main_pet_index)	//该宠物已经是他的副宠了,无需重复操作
	{
		return -2;
	}
	if (-1 != before_main_pet_index)	//该宠物已经是副宠
	{
		m_role_module_mgr->NoticeNum(errornum::EN_PET_NOT_OP_BY_BENEDICTION);
		return -3;
	}

	if (this->IsFightPet(vice_pet_index))		//出战与待战宠物不能被选为副宠
	{
		return -4;
	}

	if (-1 != m_common_param.benediction_pet_index_list[main_pet_index])	//副宠此时不能充当主宠
	{
		return -5;
	}
	if (m_pet_benediction_map.end() != m_pet_benediction_map.find(m_grid_list[vice_pet_index].item_unique_id))	//主宠此时不能给其他当副宠(防止套娃属性)
	{
		return -6;
	}

	std::map<long long, std::set<int> >::iterator benediction_it = m_pet_benediction_map.find(m_grid_list[main_pet_index].item_unique_id);
	if (benediction_it != m_pet_benediction_map.end() && (int)benediction_it->second.size() >= MAX_PET_BENEDICTION__NUM)	//最多3个
	{
		return -7;
	}
	
	m_common_param.benediction_pet_index_list[vice_pet_index] = main_pet_index;
	m_pet_benediction_map[m_grid_list[main_pet_index].item_unique_id].insert(vice_pet_index);
	m_pet_benediction_vice_set.insert(m_grid_list[vice_pet_index].item_unique_id);
	this->SendPetBenedictionSingleInfo(vice_pet_index);
	this->ReCalcAttr(main_pet_index);
	if (this->IsInPlatfformBattle(m_grid_list[main_pet_index].item_unique_id))
	{
		this->SyncPetDataPlatformBattle(main_pet_index);
	}
	this->SendSingleInfo(main_pet_index, Protocol::SCSendPetSingleInfo::PET_NOTIE_TYPE_BENEDICTION);
	EventHandler::Instance().OnAddPetBenediction(m_role_module_mgr->GetRole(), m_grid_list[vice_pet_index].item_unique_id);
	return 0;
}

int Pet::OnCancelBenediction(int vice_pet_index)
{
	if (this->PetInvalid(vice_pet_index)) return -1;

	int main_pet_index = m_common_param.benediction_pet_index_list[vice_pet_index];
	if (this->PetInvalid(main_pet_index))
	{
		m_common_param.benediction_pet_index_list[vice_pet_index] = -1;
		this->SendPetBenedictionSingleInfo(vice_pet_index);
		return 0;
	}
	std::map<long long, std::set<int> >::iterator benediction_it = m_pet_benediction_map.find(m_grid_list[main_pet_index].item_unique_id);
	if (benediction_it != m_pet_benediction_map.end())
	{
		benediction_it->second.erase(vice_pet_index);
		m_pet_benediction_vice_set.erase(m_grid_list[vice_pet_index].item_unique_id);
		if (benediction_it->second.empty())
		{
			m_pet_benediction_map.erase(benediction_it);
		}
	}

	m_common_param.benediction_pet_index_list[vice_pet_index] = -1;
	this->SendPetBenedictionSingleInfo(vice_pet_index);
	this->ReCalcAttr(main_pet_index);
	if (this->IsInPlatfformBattle(m_grid_list[main_pet_index].item_unique_id))
	{
		this->SyncPetDataPlatformBattle(main_pet_index);
	}
	this->SendSingleInfo(main_pet_index, Protocol::SCSendPetSingleInfo::PET_NOTIE_TYPE_BENEDICTION);
	return 0;
}

bool Pet::PutOnPetEquipment(int pet_index, int equipment_bag_index)
{
	if (this->PetInvalid(pet_index)) return false;
	PetParam& pet_param = m_grid_list[pet_index].param->pet_param;

	ItemDataWrapper tmp_equip1; tmp_equip1.Reset();

	if (!m_role_module_mgr->GetKnapsack()->GetItemGridWrapData(ItemNamespace::ITEM_COLUMN_TYPE_ITEM, equipment_bag_index, &tmp_equip1))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_LOAD_ERROR);
		return false;
	}

	const ItemBase * tmp_itembase = ITEMPOOL->GetItem(tmp_equip1.item_id);
	if (tmp_itembase == NULL || tmp_itembase->GetItemType() != I_TYPE_PET_EQUIPMENT)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_ITEM_NOT_ELIGIBLE);
		return false;
	}
	
	if (pet_param.pet_equipment_id != 0)//有装备
	{
		if (!this->PutOffPetEquipment(pet_index))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_EQUIP_TAKEOFF_ERROR);
			return false;
		}
	}

	if (!m_role_module_mgr->GetKnapsack()->ConsumeItemByIndex(ItemNamespace::ITEM_COLUMN_TYPE_ITEM, equipment_bag_index, 1, __FUNCTION__))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_USE_ITEM_ERROR);
		return false;
	}

	NetValueItemParam param;
	param.SetData(tmp_equip1.param_data);
	pet_param.equipment_item_unique_id = tmp_equip1.item_unique_id;
	pet_param.pet_equipment_is_bind = true;
	pet_param.SetPetEquip(&param.pet_equipment_param);
	
	gamelog::g_log_pet.printf(LL_INFO, "%s user[%s,%d] item_unique_id[%lld] pet_equip_param param[%s]", __FUNCTION__,
		m_role_module_mgr->GetRoleName(), m_role_module_mgr->GetRoleUID(), tmp_equip1.item_unique_id, param.GetParamLog(I_TYPE_PET_EQUIPMENT));
	
	this->ReCalcAttr(pet_index);
	this->SetItemDirty(pet_index, true, Protocol::SCSendPetSingleInfo::PET_NOTIE_TYPE_PET_EQUIPMENT_CHANGE);
	EventHandler::Instance().OnPetPutEquipment(m_role_module_mgr->GetRole());
	if (this->IsInPlatfformBattle(m_grid_list[pet_index].item_unique_id))
	{
		this->SyncPetDataPlatformBattle(pet_index);
	}
	return true;
}

bool Pet::PutOffPetEquipment(int pet_index)
{
	if (this->PetInvalid(pet_index)) return false;
	if (NULL == m_grid_list[pet_index].param)return false;
	PetParam& pet_param = m_grid_list[pet_index].param->pet_param;
	if (pet_param.pet_equipment_id == 0)
	{
		return true;
	}

	const ItemBase * tmp_itembase = ITEMPOOL->GetItem(pet_param.pet_equipment_id);
	if (tmp_itembase == NULL || tmp_itembase->GetItemType() != I_TYPE_PET_EQUIPMENT)
	{
		gamelog::g_log_pet.printf(LL_INFO, "%s err user[%s,%d] pet_index[%d] unique_id[%lld] err_param[item_id[%d] equipment_skill_num[%d] skill_1[%d] skill_2[%d] refresh_skill_1[%d] refresh_skill_2[%d] skin_id[%d]] ",
			__FUNCTION__, m_role_module_mgr->GetRoleName(), m_role_module_mgr->GetRoleUID(), pet_index, pet_param.equipment_item_unique_id, pet_param.pet_equipment_id, pet_param.equipment_skill_num,
			pet_param.equipment_skill_list[0], pet_param.equipment_skill_list[1], pet_param.equipment_skill_refresh_list[0], pet_param.equipment_skill_refresh_list[1], pet_param.skin_id);

		//增加清空异常道具操作--对于道具id存在，但是异常的宠物装备直接清空
		pet_param.ClearPetEquipment();
		this->ReCalcAttr(pet_index);
		this->SetItemDirty(pet_index, true, Protocol::SCSendPetSingleInfo::PET_NOTIE_TYPE_PET_EQUIPMENT_CHANGE);
		if (this->IsInPlatfformBattle(m_grid_list[pet_index].item_unique_id))
		{
			this->SyncPetDataPlatformBattle(pet_index);
		}
		return true;
	}
	static NetValueItemParam p_nvip; p_nvip.Reset();
	pet_param.GetPetEquip(&p_nvip.pet_equipment_param);

	ItemDataWrapper pet_equip; pet_equip.Reset();
	pet_equip.item_id = pet_param.pet_equipment_id;
	pet_equip.num = 1;
	pet_equip.has_param = 1;
	pet_equip.is_bind = 1;
	pet_equip.item_unique_id = pet_param.equipment_item_unique_id;
	p_nvip.GetData(pet_equip.param_data);

	if (!m_role_module_mgr->GetKnapsack()->Put(pet_equip, PUT_REASON_PET_EQUIPMENT_TAKEOFF))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return false;
	}
	gamelog::g_log_pet.printf(LL_INFO, "%s Succ user[%s,%d] pet_index[%d] item_id[%d] param[%s]", __FUNCTION__,
		m_role_module_mgr->GetRoleName(), m_role_module_mgr->GetUid(), pet_index, pet_param.pet_equipment_id, p_nvip.GetParamLog(I_TYPE_PET_EQUIPMENT));
	pet_param.ClearPetEquipment();
	this->ReCalcAttr(pet_index);
	this->SetItemDirty(pet_index, true, Protocol::SCSendPetSingleInfo::PET_NOTIE_TYPE_PET_EQUIPMENT_CHANGE);
	EventHandler::Instance().OnPetPutEquipment(m_role_module_mgr->GetRole());
	if (this->IsInPlatfformBattle(m_grid_list[pet_index].item_unique_id))
	{
		this->SyncPetDataPlatformBattle(pet_index);
	}
	return true;
}

bool Pet::ChangePetEquipmentToOtherPet(int pet_index_get, int pet_index_off)
{
	if (this->PetInvalid(pet_index_get)) return false;
	if (this->PetInvalid(pet_index_off))return false;
	PetParam& pet_param_get = m_grid_list[pet_index_get].param->pet_param;//穿戴的宠物
	PetParam& pet_param_off = m_grid_list[pet_index_off].param->pet_param;//脱下装备的宠物

	const ItemBase * tmp_itembase = ITEMPOOL->GetItem(pet_param_off.pet_equipment_id);
	if (tmp_itembase == NULL || tmp_itembase->GetItemType() != I_TYPE_PET_EQUIPMENT)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_ITEM_NOT_ELIGIBLE);
		return false;
	}

	PetEquipMentParam pet_equipment;

	pet_param_off.GetPetEquip(&pet_equipment);

	const ItemBase * tmp_itembase_get = ITEMPOOL->GetItem(pet_param_get.pet_equipment_id);
	if (tmp_itembase_get != NULL && tmp_itembase_get->GetItemType() == I_TYPE_PET_EQUIPMENT)
	{
		if (!this->PutOffPetEquipment(pet_index_get))
		{
			return false;
		}
	}

	pet_param_get.SetPetEquip(&pet_equipment);
	
	gamelog::g_log_pet.printf(LL_INFO, "%s user[%s,%d] item_unique_id[%lld] pet_equip_param param[item_id[%d] equipment_skill_num[%d] skill_1[%d] skill_2[%d] refresh_skill_1[%d] refresh_skill_2[%d] skin_id[%d]] ", __FUNCTION__,
		m_role_module_mgr->GetRoleName(), m_role_module_mgr->GetRoleUID(), pet_param_off.equipment_item_unique_id, pet_param_off.pet_equipment_id, pet_param_off.equipment_skill_num,
		pet_param_off.equipment_skill_list[0], pet_param_off.equipment_skill_list[1], pet_param_off.equipment_skill_refresh_list[0], pet_param_off.equipment_skill_refresh_list[1],
		pet_param_off.skin_id);

	pet_param_off.ClearPetEquipment();
	this->ReCalcAttr(pet_index_get);
	this->ReCalcAttr(pet_index_off);
	this->SetItemDirty(pet_index_get, true, Protocol::SCSendPetSingleInfo::PET_NOTIE_TYPE_PET_EQUIPMENT_CHANGE);
	this->SetItemDirty(pet_index_off, true, Protocol::SCSendPetSingleInfo::PET_NOTIE_TYPE_PET_EQUIPMENT_CHANGE);
	if (this->IsInPlatfformBattle(m_grid_list[pet_index_get].item_unique_id))
	{
		this->SyncPetDataPlatformBattle(pet_index_get);
	}
	if (this->IsInPlatfformBattle(m_grid_list[pet_index_off].item_unique_id))
	{
		this->SyncPetDataPlatformBattle(pet_index_off);
	}
	return true;
}

int Pet::GetPetEquipmentByColor(int color, bool is_find_storage)
{
	int wear_equipment_num = 0;
	for (int i = 0; i < ARRAY_LENGTH(m_grid_list); ++i)
	{
		if (this->PetInvalid(i)) continue;

		const ItemBase * item_base = ITEMPOOL->GetItem(m_grid_list[i].param->pet_param.pet_equipment_id);
		if(NULL == item_base)
		{
			continue;
		}
		if (ItemBase::I_COLOR_INVALID == color || item_base->GetColor() >= color)
		{
			wear_equipment_num++;
		}
	}
	
	if (is_find_storage)
	{
		for (int i = 0; i < ARRAY_LENGTH(m_storage_grid_list); ++i)
		{
			if (this->PetStorageInvalid(i)) continue;

			const ItemBase * item_base = ITEMPOOL->GetItem(m_storage_grid_list[i].param->pet_param.pet_equipment_id);
			if (NULL == item_base)
			{
				continue;
			}
			if (ItemBase::I_COLOR_INVALID == color || item_base->GetColor() >= color)
			{
				wear_equipment_num++;
			}
		}
	}

	return wear_equipment_num;
}

int Pet::GetPetNumByQuality(int quality, bool is_find_storage)
{
	int pet_num = 0;
	for (int i = 0; i < ARRAY_LENGTH(m_grid_list); ++i)
	{
		if (this->PetInvalid(i)) continue;

		const PetCfg * pet_cfg = LOGIC_CONFIG->GetPetConfig()->GetPetCfg(m_grid_list[i].item_id);
		if (NULL == pet_cfg)
		{
			continue;
		}

		if (PET_QUALITY_INVALID == quality || pet_cfg->quality >= quality)
		{
			pet_num++;
		}
	}

	if (is_find_storage)
	{
		for (int i = 0; i < ARRAY_LENGTH(m_storage_grid_list); ++i)
		{
			if (this->PetStorageInvalid(i)) continue;

			const PetCfg * pet_cfg = LOGIC_CONFIG->GetPetConfig()->GetPetCfg(m_storage_grid_list[i].item_id);
			if (NULL == pet_cfg)
			{
				continue;
			}

			if (PET_QUALITY_INVALID == quality || pet_cfg->quality >= quality)
			{
				pet_num++;
			}
		}
	}

	return pet_num;
}

int Pet::GetPetSoulEquipmentByColor(int color, bool is_find_storage)
{
	int wear_soul_equipment_num = 0;
	for (int i = 0; i < ARRAY_LENGTH(m_grid_list); ++i)
	{
		if (this->PetInvalid(i)) continue;

		const ItemBase * item_base = ITEMPOOL->GetItem(m_grid_list[i].param->pet_param.m_soul_equip_1.item_id);
		if (NULL != item_base)
		{
			if (ItemBase::I_COLOR_INVALID == color || item_base->GetColor() >= color)
			{
				wear_soul_equipment_num++;
			}
		}
		item_base = ITEMPOOL->GetItem(m_grid_list[i].param->pet_param.m_soul_equip_2.item_id);
		if (NULL != item_base)
		{
			if (ItemBase::I_COLOR_INVALID == color || item_base->GetColor() >= color)
			{
				wear_soul_equipment_num++;
			}
		}
	}

	if (is_find_storage)
	{
		for (int i = 0; i < ARRAY_LENGTH(m_storage_grid_list); ++i)
		{
			if (this->PetStorageInvalid(i)) continue;

			const ItemBase * item_base = ITEMPOOL->GetItem(m_storage_grid_list[i].param->pet_param.m_soul_equip_1.item_id);
			if (NULL != item_base)
			{
				if (ItemBase::I_COLOR_INVALID == color || item_base->GetColor() >= color)
				{
					wear_soul_equipment_num++;
				}
			}
			item_base = ITEMPOOL->GetItem(m_storage_grid_list[i].param->pet_param.m_soul_equip_2.item_id);
			if (NULL != item_base)
			{
				if (ItemBase::I_COLOR_INVALID == color || item_base->GetColor() >= color)
				{
					wear_soul_equipment_num++;
				}
			}
		}
	}

	return wear_soul_equipment_num;
}

void Pet::OnDominanceChange(int pet_index)
{
	if(this->PetInvalid(pet_index)) return;

	this->ReCalcAttr(pet_index);
	this->SetItemDirty(pet_index, true, Protocol::SCSendPetSingleInfo::PET_NOTIE_TYPE_DOMINANCE_CHANGE);
}

const PetParam * Pet::GetPetParam(int pet_index)
{
	return this->__GetPetParam(pet_index);
}

const PetParam* Pet::GetPetParam(int pet_id, long long unique_item_id, ARG_OUT int* pet_idx)
{
	return this->MutablePetParam(pet_id, unique_item_id, pet_idx);
}

void Pet::CheckPetCollection()
{
	//宠物背包
	for (int index = 0; index < ItemNamespace::MAX_PET_GRID_NUM; index++)
	{
		if (!m_old_state[index] || m_grid_list[index].Invalid())
		{
			continue;
		}

		if (NULL == m_grid_list[index].param)
		{
			continue;
		}

		PetParam& pet_param = m_grid_list[index].param->pet_param;
		const PetCfg * pet_cfg = LOGIC_CONFIG->GetPetConfig()->GetPetCfg(pet_param.pet_id);
		if (NULL == pet_cfg)
		{
			return;
		}

		m_role_module_mgr->GetCollection()->OnPetCollection(pet_cfg->prototype_pet_id);
	}

	//宠物仓库
	for (int index = 0; index < ItemNamespace::MAX_PET_STORAGE_GRID_NUM; index++)
	{
		if (!m_storage_old_state[index] || m_storage_grid_list[index].Invalid())
		{
			continue;
		}

		if (NULL == m_storage_grid_list[index].param)
		{
			continue;
		}
		PetParam& pet_param = m_storage_grid_list[index].param->pet_param;
		const PetCfg * pet_cfg = LOGIC_CONFIG->GetPetConfig()->GetPetCfg(pet_param.pet_id);
		if (NULL == pet_cfg)
		{
			return;
		}

		m_role_module_mgr->GetCollection()->OnPetCollection(pet_cfg->prototype_pet_id);
	}
}

const PetParam * Pet::GetFightPetParam()
{
	return this->GetPetParam(m_role_module_mgr->GetCommonData().new_pet_status_index[NEW_FIGHT_INDEX]);
}

const PetParam * Pet::GetPetParamFromStorage(int pet_index)
{
	return GetPetParamFromStorageNoConst(pet_index);
}

bool Pet::GetBattleCharacterData(int pet_index, PetBattleData* out_data, bool is_use_cur_hp_mp)
{
	if (NULL == out_data) return false;
	if (this->PetInvalid(pet_index)) return false;
	const PetParam& pet = m_grid_list[pet_index].param->pet_param;

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
	this->GetAttributeList(pet_index, attr_list);
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

	PetEffectSkillId skill_list = GetPetEffectiveSkill(pet_index);

	for (int i = 0; i < (int)skill_list.passive_list.size() && i < BATTLE_PET_GIFT_NUM; ++i)
	{
		out_data->passive_list[i] = skill_list.passive_list[i].skill_id;
		out_data->passive_level_list[i] = skill_list.passive_list[i].skill_level + 1;
	}
	short other_sp_num = 0;

	//宠物共鸣
	this->GetStrengthenBuffSpId(pet_index, BATTLE_OTHER_EFFECT_NUM, &other_sp_num, out_data->other_sp_list);

	//灵骑
	m_role_module_mgr->GetSmartMountsSystem()->GetBaseSpecialEffectList(m_grid_list[pet_index].item_unique_id, BATTLE_OTHER_EFFECT_NUM, &other_sp_num, out_data->other_sp_list);
	//家族灵脉
	m_role_module_mgr->GetRoleGuildInborn()->GetPassiveSkillByType(GUILD_INBORN_ADD_TARGET_TYPE_PET, BATTLE_OTHER_EFFECT_NUM, &other_sp_num, out_data->other_sp_list);
	//宠物祝福
	if (other_sp_num < BATTLE_OTHER_EFFECT_NUM)
	{
		int pet_benediction_sp_id = this->GetPetBenedictionSpId(pet, m_grid_list[pet_index].item_unique_id);
		const SpecialEffectCfg * spe_cfg = BattleSpecialEffectPool::GetInstance().GetSpecialEffectCfg(pet_benediction_sp_id);
		if (NULL != spe_cfg)
		{
			out_data->other_sp_list[other_sp_num++] = pet_benediction_sp_id;
		}
	}
	//魂器特效
	this->GetSoulEffectList(pet, BATTLE_OTHER_EFFECT_NUM, &other_sp_num, out_data->other_sp_list);
	//神印特效
	this->GetGodPrintEffectList(pet, BATTLE_OTHER_EFFECT_NUM, &other_sp_num, out_data->other_sp_list);
	//宠物天赋
	this->GetPetTalentBattleSpecialEffectList(pet, BATTLE_OTHER_EFFECT_NUM, &other_sp_num, out_data->other_sp_list);
	//修为特效
	m_role_module_mgr->GetXiuWei()->OnGetBattleEffect(XIU_WEI_CHARACTER_TYPE_PET, BATTLE_OTHER_EFFECT_NUM, &other_sp_num, out_data->other_sp_list);
	//星图特效
	m_role_module_mgr->GetStarChart()->GetPetEffectList(BATTLE_OTHER_EFFECT_NUM, &other_sp_num, out_data->other_sp_list);

	this->GetPetAutoMoveSetting(pet.pet_id, &out_data->auto_move_type, &out_data->auto_skill_id, &out_data->auto_skill_level);
	
	//gamelog::g_log_debug.printf(LL_INFO, "GetBattleCharacterData role[%d,%s] pet[%d] move_type[%d] skill_id[%d]",
	//	m_role_module_mgr->GetRoleUID(), m_role_module_mgr->GetRoleName(), pet.pet_id, out_data->auto_move_type, out_data->auto_skill_id);

	bcd.param = (char)pet.str_level;

	bcd.capability = this->GetPetCapability_1(pet_index);
	bcd.fly_flag = pet.fei_sheng_times;
	out_data->skin_id = pet.skin_id;
	out_data->unique_id = m_grid_list[pet_index].item_unique_id;
	out_data->lianyao_pet_id = pet.lian_yao_pet_id;
	out_data->lianyao_pet_str_lv = pet.lian_yao_pet_str_lv;
	out_data->lian_yao_exclusive_skill_disable_flag = 0;
	out_data->pet_exclusive_skill_disable_flag = 0;
	return true;
}

bool Pet::GetPetAutoMoveSetting(int pet_id, ARG_OUT short* auto_move_type, ARG_OUT short* auto_skill_id, ARG_OUT short* auto_skill_lv)
{
	PetAutoMoveMap::iterator it = m_auto_move_map.find(pet_id);
	if (it != m_auto_move_map.end())
	{
		*auto_move_type = it->second.auto_move_type;
		*auto_skill_id = it->second.auto_skill_id;
		*auto_skill_lv = it->second.auto_skill_lv;
	}
	else
	{
		const PetCfg* cfg = LOGIC_CONFIG->GetPetConfig()->GetPetCfg(pet_id);
		if (NULL != cfg
			&& cfg->exclusive_skills.size() > 0
			&& PET_EXCLUSIVE_SKILL_TYPE_ZHU_DONG == cfg->exclusive_skills[0].type)
		{
			*auto_move_type = BATTLE_MOVE_TYPE_PERFORM_SKILL;
			*auto_skill_id = cfg->exclusive_skills[0].skill_id;
			*auto_skill_lv = cfg->exclusive_skills[0].skill_level;
		}
		else
		{
			*auto_move_type = BATTLE_MOVE_TYPE_NORMAL_ATTACK;
			*auto_skill_id = 0;
			*auto_skill_lv = 0;
		}

		this->ModifyAutoMoveMap(pet_id, *auto_move_type, *auto_skill_id, *auto_skill_lv);
	}

	return true;
}

bool Pet::GetPetGridData(int pet_index, ItemDataWrapper* out_wrapper)
{
	if (NULL == out_wrapper) return false;
	if (this->PetInvalid(pet_index)) return false;
	
	return m_grid_list[pet_index].WrapTo(out_wrapper);
}

const ItemGridData* Pet::GetPetGridData(int pet_index) const
{
	if (this->PetInvalid(pet_index)) return NULL;

	return &m_grid_list[pet_index];
}

const ItemGridData * Pet::GetPetGridDataFromStorage(int pet_index) const
{
	if (this->PetStorageInvalid(pet_index)) return NULL;

	return &m_storage_grid_list[pet_index];
}

bool Pet::IsHavePet(int pet_id)
{
	for (int idx = 0; idx < ItemNamespace::MAX_PET_GRID_NUM; ++idx)
	{
		if (!this->PetInvalid(idx) && pet_id == m_grid_list[idx].param->pet_param.pet_id) return true;
	}

	return false;
}

int Pet::TheNumOfLevelPet(int level)
{
	int num = 0;
	for (int i = 0; i < ItemNamespace::MAX_PET_GRID_NUM; ++i)
	{
		if (this->PetInvalid(i)) continue;

		const PetParam & pet_data = m_grid_list[i].param->pet_param;
		if (pet_data.level >= level) ++num;
	}

	return num;
}

bool Pet::HaveSpace(int count, bool is_notic)
{
	int now_count = count + m_pet_count;
	if (now_count > ItemNamespace::MAX_PET_GRID_NUM ||
		now_count > LOGIC_CONFIG->GetPetConfig()->GetOtherCfg()->max_pet_bag ||
		now_count > LOGIC_CONFIG->GetPetConfig()->GetOtherCfg()->start_pet_bag + m_common_param.extend_grid_num)
	{
		if (is_notic)
		{
			m_role_module_mgr->NoticeNum(errornum::EN_PET_COUNT_LIMIT);
		}

		return false;
	}

	return true;
}

void Pet::SendPetOtherInfo()
{
	Protocol::SCSendPetOtherInfo info;

	const CommonDataParam & common_data = m_role_module_mgr->GetCommonData();
	info.fight_index = common_data.new_pet_status_index[NEW_FIGHT_INDEX];
	info.follow_index = common_data.new_pet_status_index[NEW_FOLLOW_INDEX];
	info.standby_index1 = common_data.new_pet_status_index[NEW_STANDBY_INDEX1];
	info.standby_index2 = common_data.new_pet_status_index[NEW_STANDBY_INDEX2];
	info.standby_index3 = common_data.new_pet_status_index[NEW_STANDBY_INDEX3];
	info.standby_index4 = common_data.new_pet_status_index[NEW_STANDBY_INDEX4];
	info.standby_index5 = common_data.new_pet_status_index[NEW_STANDBY_INDEX5];
	info.extend_grid_num = m_common_param.extend_grid_num;
	info.extend_times = m_common_param.extend_times;

	m_role_module_mgr->NetSend((const char *)&info, sizeof(info));
}

void Pet::SendAllInfo(int reason)
{
	Protocol::SCSendPetAllInfo info;
	info.reason = reason;
	info.pet_count = 0;
	
	int offset = 0;
	for (int i = 0; i < ARRAY_LENGTH(m_grid_list) && offset < ARRAY_LENGTH(info.pet_list); ++i)
	{
		if (this->PetInvalid(i)) continue;
	
		info.pet_list[offset].pet_pro.pet_unique_id = m_grid_list[i].item_unique_id;
		info.pet_list[offset].pet_pro.index = i;
		info.pet_list[offset].pet_pro.pet_param = m_grid_list[i].param->pet_param;
		info.pet_list[offset].pet_pro.is_bind = m_grid_list[i].is_bind;
		info.pet_list[offset].pet_pro.capability = this->GetPetCapability_1(i);
		info.pet_list[offset].rank = RankManager::Instance().GetPetTopRankShadow()
			->GetPetTopRank(m_role_module_mgr->GetUid(), 
				m_grid_list[i].param->pet_param.pet_id, 
				m_grid_list[i].item_unique_id);

		info.pet_count++;
		offset += 1;
	}

	info.pet_storage_count = 0;
	for (int i = 0; i < ARRAY_LENGTH(m_storage_grid_list) && offset < ARRAY_LENGTH(info.pet_list); ++i)
	{
		if (this->PetStorageInvalid(i)) continue;

		info.pet_list[offset].pet_pro.pet_unique_id = m_storage_grid_list[i].item_unique_id;
		info.pet_list[offset].pet_pro.index = i;
		info.pet_list[offset].pet_pro.pet_param = m_storage_grid_list[i].param->pet_param;
		info.pet_list[offset].pet_pro.is_bind = m_storage_grid_list[i].is_bind;
		info.pet_list[offset].rank = RankManager::Instance().GetPetTopRankShadow()
			->GetPetTopRank(m_role_module_mgr->GetUid(),
				m_storage_grid_list[i].param->pet_param.pet_id,
				m_storage_grid_list[i].item_unique_id);
		info.pet_storage_count++;
		offset += 1;
	}

	int len = sizeof(info) - (ARRAY_LENGTH(info.pet_list) - offset) * sizeof(info.pet_list[0]);
	m_role_module_mgr->NetSend((const char*)&info, len);
}

void Pet::SendSingleInfo(int pet_index,int notie_type)
{
	if (this->PetInvalid(pet_index)) return ;
	PetParam& pet = m_grid_list[pet_index].param->pet_param;

	Protocol::SCSendPetSingleInfo info;
	info.notie_type = notie_type;
	info.pet_info.pet_pro.pet_unique_id = m_grid_list[pet_index].item_unique_id;
	info.pet_info.pet_pro.index = pet_index;
	info.pet_info.pet_pro.capability = this->GetPetCapability_1(pet_index);
	info.pet_info.pet_pro.pet_param = m_grid_list[pet_index].param->pet_param;
	info.pet_info.pet_pro.is_bind = m_grid_list[pet_index].is_bind;
	info.pet_info.rank = RankManager::Instance().GetPetTopRankShadow()
		->GetPetTopRank(m_role_module_mgr->GetUid(),
			m_grid_list[pet_index].param->pet_param.pet_id,
			m_grid_list[pet_index].item_unique_id);
	info.add_exp = (int)pet.add_exp;
	m_role_module_mgr->NetSend((const char*)&info, sizeof(info));
}

int Pet::SendPetAttrInfo(int pet_index)
{
	if (pet_index < 0 || pet_index >= ItemNamespace::MAX_PET_GRID_NUM) return -1;
	static Protocol::SCSendPetAttrListInfo info;
	info.attr_list = m_pet_base_attr[pet_index];
	info.index = pet_index;
	info.capability = this->GetPetCapability_1(pet_index);
	m_role_module_mgr->NetSend((const char*)&info, sizeof(info));
	return 0;
}

void Pet::SendPetIdRecordList()
{
	Protocol::SCPetIdRecordList info;
	info.count = m_common_param.pet_id_record_list_count;

	for (int i = 0; i < ARRAY_LENGTH(info.pet_id_list) && i < ARRAY_LENGTH(m_common_param.pet_id_record_list) && i < m_common_param.pet_id_record_list_count; ++i)
	{
		info.pet_id_list[i] = m_common_param.pet_id_record_list[i];		
	}

	m_role_module_mgr->NetSend((const char *)&info, sizeof(info));
}

void Pet::SendPetBenedictionAllInfo()
{
	Protocol::SCPetBenedictionAllInfo info;
	for (int i = 0; i < ARRAY_LENGTH(info.benediction_pet_index_list) && i < ARRAY_LENGTH(m_common_param.benediction_pet_index_list); i++)
	{
		info.benediction_pet_index_list[i] = m_common_param.benediction_pet_index_list[i];
	}

	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void Pet::SendPetBenedictionSingleInfo(int pet_index)
{
	if (this->PetInvalid(pet_index)) return;

	Protocol::SCPetBenedictionSingleInfo info;
	info.pet_index = pet_index;
	info.main_pet_index = m_common_param.benediction_pet_index_list[pet_index];

	m_role_module_mgr->NetSend(&info, sizeof(info));
}

int Pet::GetStrBuffLevel(int pet_index, bool * is_in_buff_list)
{
	int ret = 0;
	const ItemGridData * grid = this->GetPetGridData(pet_index);
	if (!grid)
	{
		return ret;
	}

	int type_index = this->GetStrengthenBuffTypeIndex(grid->item_id);
	if (type_index < 0 || type_index >= ARRAY_LENGTH(m_strengthen_buff_param.strengthen_buff_item_list))
	{
		return ret;
	}

	PetStrengthenBuffParamItem & type_item = m_strengthen_buff_param.strengthen_buff_item_list[type_index];
	for (int index = 0; index < ARRAY_LENGTH(type_item.grid_pet_uuid); ++index)
	{
		if (type_item.grid_pet_uuid[index] == grid->item_unique_id)
		{
			if (is_in_buff_list)
			{
				*is_in_buff_list = true;
			}

			ret = this->GetStrengthenBuffLevel(type_index);
		}
	}

	return ret;
}

int Pet::GetReachingGrowthCount(int growth)
{
	int num = 0;
	if (0 == growth)
	{
		for (int i = 0; i < ItemNamespace::MAX_PET_GRID_NUM; ++i)
		{
			const PetParam *param = this->GetPetParam(i);
			if (NULL == param) continue;
			const PetCfg *cfg = LOGIC_CONFIG->GetPetConfig()->GetPetCfg(param->pet_id);
			if (NULL == cfg) continue;

			bool is_max = true;
			for (int k = 0; k < ADD_POINT_TYPE_MAX; ++k)
			{
				if (cfg->growth[k] != param->growth[k])
				{
					is_max = false;
					break;
				}
			}
			if (is_max) num++;
		}
	}
	else
	{
		for (int i = 0; i < ItemNamespace::MAX_PET_GRID_NUM; ++i)
		{
			const PetParam *param = this->GetPetParam(i);
			if (NULL == param) continue;

			int temp_growth = 0;
			for (int k = 0; k < ADD_POINT_TYPE_MAX; ++k)
			{
				temp_growth += param->growth[k];
			}

			if (temp_growth >= growth)num++;
		}
	}

	return num;
}

int Pet::GetPetNumByQualityAndStr(int quality, int str)
{
	int pet_num = 0;
	for (int i = 0; i < ItemNamespace::MAX_PET_GRID_NUM; i++)
	{
		if (this->PetInvalid(i)) continue;

		PetParam & pet_param = m_grid_list[i].param->pet_param;
		const PetCfg* pet_cfg = LOGIC_CONFIG->GetPetConfig()->GetPetCfg(pet_param.pet_id);
		if (pet_cfg == NULL)continue;;

		if (pet_cfg->quality == quality && pet_param.str_level >= str)
		{
			pet_num++;
		}
	}

	for (int i = 0; i < ItemNamespace::MAX_PET_STORAGE_GRID_NUM; i++)
	{
		if (this->PetStorageInvalid(i)) continue;

		PetParam & pet_param = m_storage_grid_list[i].param->pet_param;
		const PetCfg* pet_cfg = LOGIC_CONFIG->GetPetConfig()->GetPetCfg(pet_param.pet_id);
		if (pet_cfg == NULL)continue;;

		if (pet_cfg->quality == quality && pet_param.str_level >= str)
		{
			pet_num++;
		}
	}
	return pet_num;
}

int Pet::GetHasMaxPassiveSkillNum()
{
	int max_passive_skill_num = 0;
	for (int i = 0; i < ItemNamespace::MAX_PET_GRID_NUM; ++i)
	{
		if (this->PetInvalid(i)) continue;
		PetParam & pet_param = m_grid_list[i].param->pet_param;
		
		int skill_num = 0;
		for (int i = 0; i < ARRAY_LENGTH(pet_param.passive_list); ++i)
		{
			if (pet_param.passive_list[i].passive_skill_id != 0)
			{
				skill_num++;
			}
			else    //技能开启需要按顺序
			{
				break;
			}
		}
		if (max_passive_skill_num < skill_num)
		{
			max_passive_skill_num = skill_num;
		}
	}
	return max_passive_skill_num;
}

int Pet::GetPetDifferentCount()
{
	std::set<int> m_pet_set;
	for(int i = 0; i < ItemNamespace::MAX_PET_GRID_NUM; i++)
	{ 
		if(this->PetInvalid(i)) continue;

		m_pet_set.insert(m_grid_list[i].item_id);
	}

	return (int)m_pet_set.size();
}

int Pet::GetPetCapabilityMax()
{
	//int capability[2] = {0};
	std::set<int> capability_set;
	for (int i = 0; i < ItemNamespace::MAX_PET_GRID_NUM; i++)
	{
		if (this->PetInvalid(i)) continue;

		capability_set.insert(this->GetPetCapability_1(i));
		/*	capability[1] = this->GetPetCapability(i);
			capability[0] = capability[0] >= capability[1] ? capability[0] : capability[1];*/
	}
	if (capability_set.empty())
	{
		return 0;
	}
	return *capability_set.rbegin();
	//return capability[0];
}

int Pet::GetPetIntensifyMax(int quality)
{
	int intensify = 0;
	for (int i = 0; i < ItemNamespace::MAX_PET_GRID_NUM; i++)
	{
		if(this->PetInvalid(i)) continue;

		const PetCfg * pet_cfg = LOGIC_CONFIG->GetPetConfig()->GetPetCfg(m_grid_list[i].param->pet_param.pet_id);
		if (NULL == pet_cfg || pet_cfg->quality < quality)
		{
			continue;
		}

		intensify = GetMax(intensify, (int)m_grid_list[i].param->pet_param.str_level);
	}
	for (int i = 0; i < ItemNamespace::MAX_PET_STORAGE_GRID_NUM; i++)
	{
		if (this->PetStorageInvalid(i)) continue;

		const PetCfg * pet_cfg = LOGIC_CONFIG->GetPetConfig()->GetPetCfg(m_storage_grid_list[i].param->pet_param.pet_id);
		if (NULL == pet_cfg || pet_cfg->quality < quality)
		{
			continue;
		}
		intensify = GetMax(intensify, (int)m_storage_grid_list[i].param->pet_param.str_level);
	}

	return intensify;
}

int Pet::GetPetCapability(int pet_index)const
{
	if (pet_index < 0 || pet_index >= ARRAY_LENGTH(m_pet_attr_no_skill) || this->PetInvalid(pet_index))
	{
		return 0;
	}
	const PetCfg * pet_cfg = LOGIC_CONFIG->GetPetConfig()->GetPetCfg(m_grid_list[pet_index].param->pet_param.pet_id);
	if (NULL == pet_cfg)
	{
		return 0;
	}

	return ReCalcPetCapability(m_pet_attr_no_skill[pet_index].m_attrs) + GetPetSkillCapability(pet_index) + pet_cfg->base_score + m_common_param.gm_add_capacity;
}

int Pet::GetPetCapability_1(int pet_index) const
{
	if (pet_index < 0 || pet_index >= ARRAY_LENGTH(m_pet_capability_list) || this->PetInvalid(pet_index))
	{
		return 0;
	}

	return m_pet_capability_list[pet_index];
}

int Pet::GetFightPetCapability()
{
	return this->GetPetCapability_1(m_common_param.curr_battle_fight_index);
}

int Pet::GetPetSkillCapability(int pet_index)const
{
	if (this->PetInvalid(pet_index)) return 0;
	PetParam & pet = m_grid_list[pet_index].param->pet_param;

	const PetSkillAddCapabilityCfg * add_capability_cfg = LOGIC_CONFIG->GetPetConfig()->GetPetSkillAddCapabilityCfg(pet.level);
	if (NULL == add_capability_cfg) return 0;

	PetEffectSkillId skill_list = this->GetPetEffectiveSkill(pet_index);

	int skill_capability = 0;
	for (int i = 0; i < (int)skill_list.passive_list.size(); ++i)
	{
		int skill_id = skill_list.passive_list[i].skill_id;
		int skill_level = skill_list.passive_list[i].skill_level;
		int skill_src_type = skill_list.passive_list[i].skill_src_type;

		if (skill_src_type == PET_SKILL_SRC_TYPE_PASSIVE)
		{
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

		if (skill_src_type == PET_SKILL_SRC_TYPE_FEI_SHENG)
		{
			const PetFeiShengSkillUpgradeCfg * fei_sheng_skill_cfg = LOGIC_CONFIG->GetPetConfig()->GetPetFeiShengSkillUpgradeCfg(pet.fei_sheng_select_skill_id, pet.fei_sheng_select_skill_level);
			if (fei_sheng_skill_cfg)
			{
				skill_capability += fei_sheng_skill_cfg->capability;
			}
		}

		if (skill_src_type == PET_SKILL_SRC_TYPE_BADGE)
		{
			skill_capability += LOGIC_CONFIG->GetPetBadgeConfig()->GePetBadgeSkillCapability(pet.fei_sheng_select_skill_id);
		}
	}

	//灵骑
	std::vector<SkillPro> smart_mounts_base_skill_list;
	m_role_module_mgr->GetSmartMountsSystem()->GetBaseSkillList(m_grid_list[pet_index].item_unique_id, &smart_mounts_base_skill_list);
	if (!smart_mounts_base_skill_list.empty())
	{
		for (int i = 0; i < (int)smart_mounts_base_skill_list.size(); i++)
		{
			skill_capability += LOGIC_CONFIG->GetSmartMountsSystemConfig()->GetSkillScoreCfgByIdAndLevel(smart_mounts_base_skill_list[i].skill_id, smart_mounts_base_skill_list[i].skill_level);
		}
	}
	//家族灵脉
	skill_capability += m_role_module_mgr->GetRoleGuildInborn()->GetSkillScroe(GUILD_INBORN_ADD_TARGET_TYPE_PET);
	//神印效果
	skill_capability += this->GetGodPrintEffectScore(pet);
	//宠物祝福
	int pet_benediction_sp_capability = 0;
	this->GetPetBenedictionSpId(pet, m_grid_list[pet_index].item_unique_id, &pet_benediction_sp_capability);
	skill_capability += pet_benediction_sp_capability;
	//宠物天赋
	std::vector<int> talent_sp_list;
	skill_capability += this->GetPetTalentSpIdList(pet, talent_sp_list);

	return skill_capability;
}

int Pet::GetAllFightCapability() const
{
	int fight_capability_count = 0;
	const CommonDataParam & common_data = m_role_module_mgr->GetCommonData();
	for (int i = NEW_FIGHT_INDEX_BEGIN; i < NEW_FIGHT_INDEX_END && i < ARRAY_ITEM_COUNT(common_data.new_pet_status_index); ++i)
	{
		fight_capability_count += this->GetPetCapability_1(common_data.new_pet_status_index[i]);
	}

	return fight_capability_count;
}

int Pet::ReCalcPetCapability(const Attribute* attrs)
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

void Pet::SendGetPetNotice(int pet_index, int notice_reason)
{
	Protocol::SCSendPetGetRet notice;
	notice.index = pet_index;
	notice.reason = notice_reason;
	EngineAdapter::Instance().NetSend(m_role_module_mgr->GetRole()->GetNetId(), &notice, sizeof(notice));
}

void Pet::ReCalcAttrByFaBao()
{
	int fight_index = -1;
	int standby_index1 = -1;
	int standby_index2 = -1;
	int standby_index3 = -1;
	int standby_index4 = -1;
	int standby_index5 = -1;
	this->GetFightIndex(&fight_index, &standby_index1, &standby_index2, &standby_index3, &standby_index4, &standby_index5);
	
	if (fight_index >= 0)
	{
		this->ReCalcAttr(fight_index);
		this->SetItemDirty(fight_index, true, Protocol::SCSendPetSingleInfo::PET_NOTIE_TYPE_FABAO);
	}
	
	if (standby_index1 >= 0)
	{
		this->ReCalcAttr(standby_index1);
		this->SetItemDirty(standby_index1, true, Protocol::SCSendPetSingleInfo::PET_NOTIE_TYPE_FABAO);
	}
	
	if (standby_index2 >= 0)
	{
		this->ReCalcAttr(standby_index2);
		this->SetItemDirty(standby_index2, true, Protocol::SCSendPetSingleInfo::PET_NOTIE_TYPE_FABAO);
	}
	
	if (standby_index3 >= 0)
	{
		this->ReCalcAttr(standby_index3);
		this->SetItemDirty(standby_index3, true, Protocol::SCSendPetSingleInfo::PET_NOTIE_TYPE_FABAO);
	}

	if (standby_index4 >= 0)
	{
		this->ReCalcAttr(standby_index4);
		this->SetItemDirty(standby_index4, true, Protocol::SCSendPetSingleInfo::PET_NOTIE_TYPE_FABAO);
	}

	if (standby_index5 >= 0)
	{
		this->ReCalcAttr(standby_index5);
		this->SetItemDirty(standby_index5, true, Protocol::SCSendPetSingleInfo::PET_NOTIE_TYPE_FABAO);
	}
}

bool Pet::OnTradeConsumePet(int pet_index)
{
	if (this->PetInvalid(pet_index) || this->IsLock(pet_index)) return false;

	if (m_role_module_mgr->GetRole()->GetRoleStatusManager()->IsInBattleStatus())
	{
		m_role_module_mgr->NoticeNum(errornum::EN_ROLE_IN_BATTLE);
		return false;
	}
	if (m_role_module_mgr->GetSmartMountsSystem()->IsPetDomination(m_grid_list[pet_index].item_unique_id))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_SMART_MOUNTS_SYSTEM_PET_DOMINATION);
		return false;
	}

	if (-1 != m_common_param.benediction_pet_index_list[pet_index])		//消耗的宠物不能处于祝福副宠
	{
		m_role_module_mgr->NoticeNum(errornum::EN_PET_NOT_OP_BY_BENEDICTION);
		return false;
	}

	if (this->PetBadgeTakeOff(pet_index) != 0)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_PET_BADGE_TAKE_OFF_FAIL);
		return  false;
	}

	PetParam & pet = m_grid_list[pet_index].param->pet_param;
	if (pet.level > 1)
	{
		return false;
	}

	if (this->PetInvalid(pet_index)) return false;
	
	//宠物排行榜所需
	unsigned long long  pet_unique_id = m_grid_list[pet_index].item_unique_id;

	this->DeletePet(pet_index, __FUNCTION__);
	RankManager::Instance().SyncRemovePet(this->GetRoleModuleManager()->GetRole(), pet_unique_id);
	return true;
}

bool Pet::PetInvalid(int index) const
{
	return index < 0 || index >= ItemNamespace::MAX_PET_GRID_NUM || m_grid_list[index].Invalid() || NULL == m_grid_list[index].param;
}

bool Pet::ModifyAutoMove(int pet_index, short auto_move_type, short auto_skill_id, short auto_skill_level, bool ignore_skill_check /* = false */)
{
	if (this->PetInvalid(pet_index)) return false;
	if (!ignore_skill_check && auto_move_type != BATTLE_MOVE_TYPE_PERFORM_SKILL) return false;

	m_grid_list[pet_index].param->pet_param.pet_auto_move_type = auto_move_type;
	m_grid_list[pet_index].param->pet_param.pet_auto_skill_id = auto_skill_id;
	m_grid_list[pet_index].param->pet_param.pet_auto_skill_level = auto_skill_level;

	this->SetItemDirty(pet_index, false);

	if (pet_index == this->GetFightIndex())
	{
		this->ModifyAutoMoveMap(m_grid_list[pet_index].param->pet_param.pet_id, auto_move_type, auto_skill_id, auto_skill_level);
	}
	
	return true;
}

bool Pet::ModifySpecifyPetAutoMove(int pet_id, long long pet_unique_id, short auto_move_type, short auto_skill_id, short auto_skill_level, bool is_main_fight_pet)
{
	int pet_index = -1;
	PetParam* param = this->MutablePetParam(pet_id, pet_unique_id, &pet_index);
	if (NULL == param) return false;

	param->pet_auto_move_type = auto_move_type;
	param->pet_auto_skill_id = auto_skill_id;
	param->pet_auto_skill_level = auto_skill_level;

	this->SetItemDirty(pet_index, false);

	if (is_main_fight_pet)
	{
		this->ModifyAutoMoveMap(pet_id, auto_move_type, auto_skill_id, auto_skill_level);
	}

	return true;
}

void Pet::GmAddMaxAttrPet(int pet_id, int gift_type, int attr_point_val)
{
	enum GIFT_TYPE
	{
		ATTENTIVE_ATK = 0,		// 重视攻击
		ATTENTIVE_MGC = 1,		// 重视魔法
		ATTENTIVE_DEF = 2,		// 重视防御
		ATTENTIVE_REC = 3,		// 重视回复

		ATTENTIVE_MAX
	};

	if (!this->HaveSpace(1)) return;
	if (gift_type < 0 || gift_type >= ATTENTIVE_MAX) gift_type = 0;

	const PetCfg * cfg = LOGIC_CONFIG->GetPetConfig()->GetPetCfg(pet_id);
	if (NULL == cfg) return;

	int max_str_lv = 0;
	while (NULL != LOGIC_CONFIG->GetPetConfig()->GetPetStrengthenCfg(cfg->strengthen_cfg_id, max_str_lv + 1)) ++max_str_lv;

	NetValueItemParam pet; pet.Reset();
	PetParam & pet_data = pet.pet_param;

	pet_data.pet_id = cfg->id;
	memcpy(pet_data.name, cfg->name, sizeof(GameName));
	pet_data.str_level = max_str_lv;
	pet_data.level = MAX_ROLE_LEVEL;
	pet_data.exp = 0;
	pet_data.hp = 0;
	pet_data.mp = 0;
	pet_data.lock = 0;

	for (int i = 0; i < ADD_POINT_TYPE_MAX; ++i)
	{
		pet_data.growth[i] = (unsigned char)(cfg->growth[i]);
	}

	// 分配属性点
	if (attr_point_val > MAX_UINT16) attr_point_val = MAX_UINT16;
	for (int i = 0; i < ADD_POINT_TYPE_MAX; ++i)
	{
		pet_data.attr[i] = attr_point_val;
	}

	long long item_unique_id = 0;
	UniqueIDGenerator::Instance()->GenUniqueID(&item_unique_id);

	pet.pet_param.CheckModifyVersion();
	pet.pet_param.CheckGrowthVersion(item_unique_id, __FUNCTION__);

	int pet_index = this->AddPet(&pet, __FUNCTION__, false, false, item_unique_id);

	Protocol::SCSendPetGetRet notice;
	notice.index = pet_index;
	notice.reason = 0;
	EngineAdapter::Instance().NetSend(m_role_module_mgr->GetRole()->GetNetId(), &notice, sizeof(notice));
	
	return;
}

void Pet::SetPetPlatformBattleState(unsigned long long pet_unique_id, bool is_platform_battle)
{
	if (pet_unique_id <= 0)
	{
		return;
	}

	if (is_platform_battle)
	{
		m_is_platform_battle.insert(pet_unique_id);
	}
	else
	{
		m_is_platform_battle.erase(pet_unique_id);
	}
}

void Pet::SyncPetDataPlatformBattle(int pet_index)
{
	if (!m_role_module_mgr->GetRole()->IsInCross())
	{
		return;
	}

	if (pet_index != -1)
	{
		PlatformBattlePet platform_battle_pet_data;
		this->GetPlatformBattleCharacterData(pet_index, NULL, platform_battle_pet_data);
		PlatformBattle::Instance().SyncPetData(m_role_module_mgr->GetRole()->GetUID(), platform_battle_pet_data);
	}
	else
	{
		for (int i = 0; i < ARRAY_LENGTH(m_grid_list); ++i)
		{
			if (m_grid_list[i].Invalid())
			{
				continue;
			}

			if (this->IsInPlatfformBattle(m_grid_list[i].item_unique_id))
			{
				PlatformBattlePet platform_battle_pet_data;
				this->GetPlatformBattleCharacterData(i, NULL, platform_battle_pet_data);
				PlatformBattle::Instance().SyncPetData(m_role_module_mgr->GetRole()->GetUID(), platform_battle_pet_data);
			}
		}
	}
}

long long Pet::GetPetUniqueId(int pet_index) const
{
	if (this->PetInvalid(pet_index))
	{
		return 0;
	}
	
	return m_grid_list[pet_index].item_unique_id;
}

long long Pet::GetPetStorageUniqueId(int pet_index) const
{
	if (this->PetStorageInvalid(pet_index))
	{
		return 0;
	}

	return m_storage_grid_list[pet_index].item_unique_id;
}

bool Pet::HasPet(std::set<unsigned long long> pet_unique_ids)
{
	pet_unique_ids.erase(0);

	if (pet_unique_ids.empty())
	{
		return true;
	}

	for (int i = 0; i < ARRAY_LENGTH(m_grid_list); ++i)
	{
		if (m_grid_list[i].Invalid())
		{
			continue;
		}

		pet_unique_ids.erase(m_grid_list[i].item_unique_id);

		if (pet_unique_ids.empty())
		{
			return true;
		}
	}

	for (int i = 0; i < ARRAY_LENGTH(m_storage_grid_list); ++i)
	{
		if (m_storage_grid_list[i].Invalid())
		{
			continue;
		}

		pet_unique_ids.erase(m_storage_grid_list[i].item_unique_id);

		if (pet_unique_ids.empty())
		{
			return true;
		}
	}

	return false;
}

bool Pet::GetPlatformBattleCharacterData(int pet_index, PetBattleData ARG_OUT * pet_battle_data, PlatformBattlePet ARG_OUT & platform_battle_pet_data)
{
	if (this->PetInvalid(pet_index)) return false;
	const ItemGridData & pet_grid = m_grid_list[pet_index];
	const PetParam & pet = pet_grid.param->pet_param;

	PetBattleData temp_pet_battle_data;
	bool ret = this->GetBattleCharacterData(pet_index, &temp_pet_battle_data, false);

	if (pet_battle_data != NULL)
	{
		*pet_battle_data = temp_pet_battle_data;
	}

	{
		platform_battle_pet_data.item_unique_id = pet_grid.item_unique_id;
		platform_battle_pet_data.pet_id = pet.pet_id;
		platform_battle_pet_data.pet_capability = this->GetPetCapability_1(pet_index);
		platform_battle_pet_data.pet_level = pet.level;
		platform_battle_pet_data.pet_strengthen_level = pet.str_level;
		memcpy(platform_battle_pet_data.pet_name, pet.name, sizeof(platform_battle_pet_data.pet_name));
		platform_battle_pet_data.pet_battle_data = temp_pet_battle_data;

		for (int k = 0; k < ARRAY_LENGTH(platform_battle_pet_data.pet_battle_data.other_sp_list); ++k)
		{
			if (0 == platform_battle_pet_data.pet_battle_data.other_sp_list[k])
			{
				platform_battle_pet_data.pet_battle_data.other_sp_list[k] = m_role_module_mgr->GetProfAdvanceSpecialEffectId();
				break;
			}
		}	
	}

	return ret;
}

void Pet::OnRolePetTrainSkillLevelUp()
{
	for (int i = 0; i < ItemNamespace::MAX_PET_GRID_NUM; ++i)
	{
		if (this->PetInvalid(i))
		{
			continue;
		}

		this->ReCalcAttr(i);
	}

	this->SendAllInfo(Protocol::PET_ALL_INFO_REASON_TRAIN_SKILL);

	this->SyncPetDataPlatformBattle();
}

void Pet::OnRoleGuildInbornSkillChange()
{
	for (int i = 0; i < ItemNamespace::MAX_PET_GRID_NUM; ++i)
	{
		if (this->PetInvalid(i))
		{
			continue;
		}

		this->ReCalcAttr(i);
	}

	this->SendAllInfo(Protocol::PET_ALL_INFO_REASON_GUILD_INBORN);

	this->SyncPetDataPlatformBattle();
}

void Pet::OnXiuWeiPetTypeLevelUp()
{
	for (int i = 0; i < ItemNamespace::MAX_PET_GRID_NUM; ++i)
	{
		if (this->PetInvalid(i))
		{
			continue;
		}

		this->ReCalcAttr(i);
	}

	this->SendAllInfo(Protocol::PET_ALL_INFO_REASON_XIU_WEI);

	this->SyncPetDataPlatformBattle();
}

void Pet::OnStarChartPetAttrChange()
{
	for (int i = 0; i < ItemNamespace::MAX_PET_GRID_NUM; ++i)
	{
		if (this->PetInvalid(i))
		{
			continue;
		}

		this->ReCalcAttr(i);
	}

	this->SendAllInfo(Protocol::PET_ALL_INFO_REASON_STAR_CHART);

	this->SyncPetDataPlatformBattle();
}

PetEffectSkillId Pet::GetPetEffectiveSkill(int pet_index, bool is_storage)const
{
	PetEffectSkillId ret;

	const ItemGridData * src_grid = NULL;
	if (!is_storage)
	{
		if (this->PetInvalid(pet_index)) return ret;
		src_grid = &m_grid_list[pet_index];
	}
	else
	{
		if (this->PetStorageInvalid(pet_index))return ret;
		src_grid = &m_storage_grid_list[pet_index];
	}

	if (src_grid == NULL || src_grid->param == NULL)
	{
		return ret;
	}

	const PetParam & pet = src_grid->param->pet_param;


	return this->GetPetEffectiveSkillHelper(pet);
}

PetEffectSkillId Pet::GetPetEffectiveSkillHelper(const PetParam & pet)
{
	PetEffectSkillId ret;

	std::set<PetSkillPriority> skill_priority;
	for (int i = 0; i < ARRAY_LENGTH(pet.passive_list); ++i)
	{
		PetSkillPriority tmp;

		int skill_id = pet.passive_list[i].passive_skill_id;
		int skill_level = pet.passive_list[i].skill_level;
		if (skill_id <= 0)
		{
			continue;
		}

		const GamePassiveSkillCfg * passive_skill_cfg = SkillPool::Instance().GetPassiveSkillCfg(skill_id);
		if (passive_skill_cfg == NULL)
		{
			continue;
		}

		tmp.skill_id = skill_id;
		tmp.skill_level = skill_level;
		tmp.skill_type = passive_skill_cfg->IsElementalSkillType() ? 1 : passive_skill_cfg->skill_type;
		tmp.priority_num = passive_skill_cfg->skill_priority;
		tmp.priority_num_2 = 1;

		skill_priority.insert(tmp);
	}

	for (int i = 0; i < ARRAY_LENGTH(pet.lian_yao_passive_list); ++i)
	{
		PetSkillPriority tmp;

		int skill_id = pet.lian_yao_passive_list[i].passive_skill_id;
		int skill_level = pet.lian_yao_passive_list[i].skill_level;
		if (skill_id <= 0)
		{
			continue;
		}

		const GamePassiveSkillCfg * passive_skill_cfg = SkillPool::Instance().GetPassiveSkillCfg(skill_id);
		if (passive_skill_cfg == NULL)
		{
			continue;
		}

		tmp.skill_id = skill_id;
		tmp.skill_level = skill_level;
		tmp.skill_type = passive_skill_cfg->IsElementalSkillType() ? 1 : passive_skill_cfg->skill_type;
		tmp.priority_num = passive_skill_cfg->skill_priority;
		tmp.priority_num_2 = 1;

		skill_priority.insert(tmp);
	}

	for (int i = 0; i < ARRAY_LENGTH(pet.equipment_skill_list); ++i)
	{
		PetSkillPriority tmp;

		int skill_id = pet.equipment_skill_list[i];
		if (skill_id <= 0)
		{
			continue;
		}

		const GamePassiveSkillCfg * passive_skill = SkillPool::Instance().GetPassiveSkillCfg(skill_id);
		if (passive_skill == NULL)
		{
			continue;
		}

		tmp.skill_id = skill_id;
		tmp.skill_type = passive_skill->IsElementalSkillType() ? 1 : passive_skill->skill_type;
		tmp.priority_num = passive_skill->skill_priority;
		tmp.priority_num_2 = 0;

		skill_priority.insert(tmp);
	}

	if (pet.fei_sheng_times != 0)
	{
		int skill_id = pet.fei_sheng_select_skill_id;
		const GamePassiveSkillCfg * passive_skill = SkillPool::Instance().GetPassiveSkillCfg(skill_id);
		if (passive_skill != NULL)
		{
			PetSkillPriority tmp;
			tmp.skill_id = skill_id;
			tmp.skill_level = pet.fei_sheng_select_skill_level;
			tmp.skill_type = passive_skill->IsElementalSkillType() ? 1 : passive_skill->skill_type;
			tmp.priority_num = passive_skill->skill_priority;
			tmp.priority_num_2 = 0;
			tmp.pet_skill_src_type = PET_SKILL_SRC_TYPE_FEI_SHENG;
			skill_priority.insert(tmp);
		}
	}

	if (pet.pet_bage_item.pet_badgei_item_id != 0 && pet.pet_bage_item.skill_id != 0)
	{
		int skill_id = pet.pet_bage_item.skill_id;
		const GamePassiveSkillCfg * passive_skill = SkillPool::Instance().GetPassiveSkillCfg(skill_id);
		if (passive_skill != NULL)
		{
			PetSkillPriority tmp;
			tmp.skill_id = skill_id;
			tmp.skill_type = passive_skill->IsElementalSkillType() ? 1 : passive_skill->skill_type;
			tmp.priority_num = passive_skill->skill_priority;
			tmp.priority_num_2 = 0;
			tmp.pet_skill_src_type = PET_SKILL_SRC_TYPE_BADGE;
			skill_priority.insert(tmp);
		}
	}

	int last_sp_type = -1;
	for (std::set<PetSkillPriority>::iterator iter = skill_priority.begin(); iter != skill_priority.end(); ++iter)
	{
		if (last_sp_type == iter->skill_type)
		{
			continue;
		}

		last_sp_type = iter->skill_type;

		PetEffectSkillItem item(iter->skill_id, iter->skill_level, iter->pet_skill_src_type);
		ret.passive_list.push_back(item);
	}

	return ret;
}

void Pet::RecordBattlePetIndex()
{
	this->GetFightIndex(&m_common_param.curr_battle_fight_index, 
		&m_common_param.curr_battle_standby_index1, 
		&m_common_param.curr_battle_standby_index2, 
		&m_common_param.curr_battle_standby_index3,
		&m_common_param.curr_battle_standby_index4,
		&m_common_param.curr_battle_standby_index5);
}

void Pet::OnBattleFinishAddExp(long long add_exp)
{
	this->AddExp(m_common_param.curr_battle_fight_index, add_exp, __FUNCTION__, Protocol::SCSendPetSingleInfo::PET_NOTIE_TYPE_BATTLE_ADD_EXP);
	this->AddExp(m_common_param.curr_battle_standby_index1, add_exp, __FUNCTION__, Protocol::SCSendPetSingleInfo::PET_NOTIE_TYPE_NO_TYPE);
	this->AddExp(m_common_param.curr_battle_standby_index2, add_exp, __FUNCTION__, Protocol::SCSendPetSingleInfo::PET_NOTIE_TYPE_NO_TYPE);
	this->AddExp(m_common_param.curr_battle_standby_index3, add_exp, __FUNCTION__, Protocol::SCSendPetSingleInfo::PET_NOTIE_TYPE_NO_TYPE);
	this->AddExp(m_common_param.curr_battle_standby_index4, add_exp, __FUNCTION__, Protocol::SCSendPetSingleInfo::PET_NOTIE_TYPE_NO_TYPE);
	this->AddExp(m_common_param.curr_battle_standby_index5, add_exp, __FUNCTION__, Protocol::SCSendPetSingleInfo::PET_NOTIE_TYPE_NO_TYPE);
}

int Pet::ExtendPetBag()
{
	if (m_common_param.extend_grid_num + LOGIC_CONFIG->GetPetConfig()->GetOtherCfg()->start_pet_bag >= LOGIC_CONFIG->GetPetConfig()->GetOtherCfg()->max_pet_bag)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_PET_BAG_EXTEND_LIMIT);
		return -1;
	}

	int real_add_count = 0, total_price = 0;
	for (int i = 1; i <= LOGIC_CONFIG->GetPetConfig()->GetOtherCfg()->extend_add_num; i++)
	{
		int price = 0;
		if (!LOGIC_CONFIG->GetPetConfig()->GetExtendPrice(m_common_param.extend_times + i, &price))
		{
			break;
		}

		real_add_count++;
		total_price += price;

		if ((LOGIC_CONFIG->GetPetConfig()->GetOtherCfg()->start_pet_bag + (m_common_param.extend_grid_num + real_add_count)) >= LOGIC_CONFIG->GetPetConfig()->GetOtherCfg()->max_pet_bag)
		{
			break;
		}
	}

	if (0 == real_add_count || 0 == total_price)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_PET_BAG_EXTEND_LIMIT);
		return -2;
	}
	
	if (!m_role_module_mgr->GetMoney()->CoinBindMoreThan(total_price))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_COIN_NOT_ENOUGH);
		return -3;
	}

	if (!m_role_module_mgr->GetMoney()->UseCoinBind(total_price, __FUNCTION__))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_COIN_NOT_ENOUGH);
		return -4;
	}

	m_common_param.extend_times += real_add_count;
	m_common_param.extend_grid_num += real_add_count;

	this->SendPetOtherInfo();

	return 0;
}

void Pet::OnRoleLevelUp(int cur_level, int * pet_index)
{
	if(cur_level <= 0 || cur_level > MAX_ROLE_LEVEL) return;

	int i = 0;
	if (NULL != pet_index)
	{
		i = *pet_index;
	}
	for (; i < ARRAY_LENGTH(m_grid_list); i++)
	{
		this->AddExp(i, 0, __FUNCTION__, Protocol::SCSendPetSingleInfo::PET_NOTIE_TYPE_EXP);
		if(NULL != pet_index) break;
	}

	this->CheckUnlockLianYaoFurance();
}

void Pet::SendPetFollowInfo(Role * role)
{
	Protocol::SCPetFollowStatus pfs;
	pfs.role_obj_id = m_role_module_mgr->GetRole()->GetId();
	pfs.follow_pet_id = 0;
	memset(pfs.follow_pet_name, 0, sizeof(GameName));
	pfs.fei_sheng_times = 0;

	int follow_index = m_role_module_mgr->GetCommonData().new_pet_status_index[NEW_FOLLOW_INDEX];
	if (!this->PetInvalid(follow_index))
	{
		const PetParam & pet_param = m_grid_list[follow_index].param->pet_param;
		pfs.follow_pet_id = pet_param.pet_id;
		F_STRNCPY(pfs.follow_pet_name, pet_param.name, sizeof(GameName));
		pfs.skin_id = pet_param.skin_id;
		pfs.fei_sheng_times = pet_param.fei_sheng_times;
	}

	if (NULL != role)
	{
		role->GetRoleModuleManager()->NetSend((const void *)&pfs, sizeof(pfs));
	}
	else
	{
		m_role_module_mgr->GetRole()->GetScene()->GetZoneMatrix()->NotifyAreaMsg(m_role_module_mgr->GetRole()->GetObserHandle(), &pfs, sizeof(pfs));
	}
}

void Pet::SendFightPetInfoToTeamMember()
{
	Role * temp_role = m_role_module_mgr->GetRole();
	Team* team = temp_role->GetMyTeam();

	if (NULL != team)
	{
		Protocol::SCUpdateMemberPetHPMP msg;
		msg.pet_id = 0;
		msg.uid = temp_role->GetUID();

		const PetParam * pet_param = this->GetPetParam(m_role_module_mgr->GetCommonData().new_pet_status_index[NEW_FIGHT_INDEX]);
		if (pet_param != NULL)
		{
			msg.pet_id = pet_param->pet_id;
			msg.pet_level = pet_param->level;
			msg.pet_str_level = pet_param->str_level;
			msg.curr_hp = pet_param->hp;
			msg.max_hp = pet_param->max_hp;
			msg.curr_mp = pet_param->mp;
			msg.max_mp = pet_param->max_mp;
			msg.skin_id = pet_param->skin_id;
		}
		team->SendToMember(&msg, sizeof(msg));
	}
}

bool Pet::UsePetEggItem(int pet_id, bool is_bind)
{
	const PetCfg *  pet_cfg  = LOGIC_CONFIG->GetPetConfig()->GetPetCfg(pet_id);
	if (pet_cfg == NULL)
	{
		return false;
	}

	int r = RandomNum(pet_cfg->catch_reward_rate_count);
	if (r < pet_cfg->catch_reward_rate_variant_pet)
	{
		int variant_pet_id = LOGIC_CONFIG->GetPetConfig()->GetVariantPetByPetId(pet_cfg->id);
		if (variant_pet_id != 0)
		{
			pet_id = variant_pet_id;
		}
	}

	return this->AcquirePet(pet_id, __FUNCTION__, 1, false, GET_PET_REASON_TYPE_PET_EGG, is_bind) >= 0;
}

bool Pet::GetPetItemGridWrapData(int pet_index, ItemDataWrapper & ARG_OUT out_wrapper, AttributeList & attrs)
{
	if (this->PetInvalid(pet_index)) return false;

	const ItemBase *itembase = ITEMPOOL->GetItem(m_grid_list[pet_index].item_id);
	if (NULL == itembase) return false;

	if (m_grid_list[pet_index].num <= 0 || m_grid_list[pet_index].num > itembase->GetPileLimit())
	{
		return false;
	}

	attrs = m_pet_base_attr[pet_index];
	return m_grid_list[pet_index].WrapTo(&out_wrapper);
}

bool Pet::IsInPlatfformBattle(unsigned long long pet_unique_id, bool is_notice)
{
	std::set<unsigned long long>::iterator iter = m_is_platform_battle.find(pet_unique_id);
	if (iter == m_is_platform_battle.end())
	{		
		return false;
	}

	if (is_notice)
	{
		m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_PET_IN_PLATFORM_BATTLE);
	}

	return true;
}

void Pet::ClearPetPlatformBattleState()
{
	m_is_platform_battle.clear();
}

int Pet::AddPet(NetValueItemParam* pet, const char* reason, bool is_bind /* = false */, bool is_xunbao /* = false */, long long item_unique_id /* = 0LL */, bool is_add_guild_eventhander /* = true*/)
{
	if (NULL == pet) return -1;

	int invalid_index = -1;
	for (int i = 0; i < ItemNamespace::MAX_PET_GRID_NUM; ++i)
	{
		if (this->PetInvalid(i))
		{
			invalid_index = i;
			break;
		}
	}

	if (invalid_index < 0)
	{
		gamelog::g_log_pet.printf(LL_INFO, "AddPet Error user[%d %s] reason[%s] pet_id[%d] pet_count[%d]",
								  m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), reason, pet->pet_param.pet_id, m_pet_count);
		return -1;
	}

	ItemGridData& grid_data = m_grid_list[invalid_index];
	grid_data.Clear();
	grid_data.item_id = pet->pet_param.pet_id;
	grid_data.num = 1;
	grid_data.is_bind = is_bind;
	grid_data.gold_price = 0;
	grid_data.invalid_time = 0u;
	if (0 == item_unique_id)
	{
		UniqueIDGenerator::Instance()->GenUniqueID(&item_unique_id);
	}
#ifdef _DEBUG
	if (!this->CheckDuplicateUniqueID(item_unique_id, invalid_index))
	{
		assert(0);
	}
#endif

	grid_data.item_unique_id = item_unique_id;
	grid_data.param = new NetValueItemParam();
	memcpy(grid_data.param, pet, sizeof(NetValueItemParam));
	grid_data.param->pet_param.CheckGrowthVersion(item_unique_id, __FUNCTION__);
	grid_data.param->pet_param.modify_ver = MAX_PET_MODIFY_VER;
	this->ModifyAutoMoveOnAddPet(grid_data.param->pet_param);

	m_pet_count++;

	// 第一只宠物自动上阵
	if (1 == m_pet_count)
	{
		this->SetPetState(invalid_index, SET_FIGHT_INDEX);
	}

	// 这个放到最下面 里面会发一次协议
	this->ReCalcAttr(invalid_index);
	this->Recover(invalid_index);
	gamelog::g_log_pet.printf(LL_INFO, "AddPet user[%d %s] reason[%s] pet_id[%d] unique_item_id[%lld]",
							  m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), reason, pet->pet_param.pet_id, item_unique_id);
	
	{
		// 标记宠物获得
		CommonDataParam & common_data = m_role_module_mgr->GetCommonData();
		common_data.get_pet_list.SetBit(pet->pet_param.pet_id - PET_ID_BEGIN);
		const PetCfg *cfg = LOGIC_CONFIG->GetPetConfig()->GetPetCfg(pet->pet_param.pet_id);
		bool is_max = true;
		int growth = 0;
		if (NULL != cfg)
		{
			for (int i = 0; i < ADD_POINT_TYPE_MAX; ++i)
			{
				growth += pet->pet_param.growth[i];
				if (cfg->growth[i] != pet->pet_param.growth[i])
				{
					is_max = false;
				}
			}
		}
	}
	if (is_add_guild_eventhander)
	{
		if (is_xunbao)
		{
			m_role_module_mgr->OnAddGuildEvent(pet->pet_param.pet_id, 0, GUILD_EVENTHANDLER_TYPE_XUNBAO);
		}
		else
		{
			m_role_module_mgr->OnAddGuildEvent(pet->pet_param.pet_id, 0, GUILD_EVENTHANDLER_TYPE_ADD_PET);
		}
	}

	this->SetMapPetQualityToStrLevel(&grid_data.param->pet_param);
	this->RecordGetPet(pet->pet_param.pet_id);
	EventHandler::Instance().OnRoleGetACreature(m_role_module_mgr->GetRole(), pet->pet_param.pet_id, grid_data.item_unique_id, &pet->pet_param);
	this->AutoAssignAttributePoints(invalid_index);
	this->CheckPetYiWenLuRewardByIndex(invalid_index);	
	this->CheckUnlockLianYaoFurance();

	unique_item_id_to_pet_index[item_unique_id] = invalid_index;

	if (m_common_param.is_first_time_get_pet)
	{
		m_common_param.is_first_time_get_pet = 0;
		this->SetPetState(invalid_index, SET_FOLLOW_INDEX);
	}

	return invalid_index;
}

bool Pet::AcquirePetWithParam(NetValueItemParam* pet, const char* reason, bool is_bind /*= false*/, bool is_xunbao /*= false*/, long long item_unique_id /*= 0LL*/, bool is_add_guild_eventhander /*= true*/)
{
	int pet_index = this->AddPet(pet, reason, is_bind, is_xunbao, item_unique_id, is_add_guild_eventhander);
	if (-1 != pet_index)
	{
		Protocol::SCSendPetGetRet notice;
		notice.index = pet_index;
		notice.reason = GET_PET_REASON_TYPE_DEFAULT;
		EngineAdapter::Instance().NetSend(m_role_module_mgr->GetRole()->GetNetId(), &notice, sizeof(notice));	
	}
	
	return -1 != pet_index;
}

bool Pet::IsBind(int index)
{
	if (this->PetInvalid(index)) return true;
	return m_grid_list[index].is_bind;
}

bool Pet::PetStorageInvalid(int pet_storage_index) const
{
	return pet_storage_index < 0 || pet_storage_index >= ItemNamespace::MAX_PET_STORAGE_GRID_NUM || m_storage_grid_list[pet_storage_index].Invalid() || NULL == m_storage_grid_list[pet_storage_index].param;
}

int Pet::MoveToPetStorage(int pet_index, int pet_storage_index)
{
	if (this->PetInvalid(pet_index) || !this->PetStorageInvalid(pet_storage_index))	//源物品无效，目标格子有物品 返回
	{
		this->SendMoveNotice();
		return -__LINE__;
	}

	if (pet_storage_index < 0 || pet_storage_index >= ARRAY_LENGTH(m_storage_grid_list))
	{
		this->SendMoveNotice();
		return -__LINE__;
	}

	if (this->IsInPlatfformBattle(m_grid_list[pet_index].item_unique_id,true))
	{
		this->SendMoveNotice();
		return -__LINE__;
	}
	if (m_role_module_mgr->GetSmartMountsSystem()->IsPetDomination(m_grid_list[pet_index].item_unique_id))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_SMART_MOUNTS_SYSTEM_PET_DOMINATION);
		return -__LINE__;
	}

	if (-1 != m_common_param.benediction_pet_index_list[pet_index])	
	{
		m_role_module_mgr->NoticeNum(errornum::EN_PET_NOT_OP_BY_BENEDICTION);
		return -__LINE__;
	}

	if (m_grid_list[pet_index].param->pet_param.HasGodPrint())
	{
		if (!this->PutOffAllPetGodPrintEquip(pet_index))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_PET_WEAR_GOD_PRINT);
			return  -__LINE__;
		}
	}

	if (this->PetBadgeTakeOff(pet_index) != 0)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_PET_BADGE_TAKE_OFF_FAIL);
		return  -__LINE__;
	}

	if (!this->PutOffAllPetSoulEquip(pet_index))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_PET_WEAR_SOUL_EQUIP);
		return  -__LINE__;
	}

	this->OnPetGridInfoReset(pet_index);

	ItemGridData & src_grid = m_grid_list[pet_index];
	ItemGridData & target_grid = m_storage_grid_list[pet_storage_index];
	target_grid.Set(src_grid);
	src_grid.Reset();
	m_pet_count -= 1;
	m_pet_storage_grid_use_num += 1;

	this->SetItemDirty(pet_index, false);
	this->SetPetStorageItemDirty(pet_storage_index);

	this->SendMoveNotice(Protocol::MOVE_PET_ITEM_NOTICE_TYPE_STORAGE, pet_index, pet_storage_index);
	EventHandler::Instance().OnPetMoveStorage(m_role_module_mgr->GetRole(), target_grid.item_unique_id);
	return 0;
}

int Pet::MoveToPetGrid(int pet_index, int pet_storage_index)
{
	if (!this->PetInvalid(pet_index) || this->PetStorageInvalid(pet_storage_index))	//源物品无效，目标格子有物品 返回
	{
		this->SendMoveNotice();
		return -__LINE__;
	}

	if (pet_index < 0 || pet_index >= ARRAY_LENGTH(m_grid_list))
	{
		this->SendMoveNotice();
		return -__LINE__;
	}

	ItemGridData & src_grid = m_storage_grid_list[pet_storage_index];
	ItemGridData & target_grid = m_grid_list[pet_index];	
	target_grid.Set(src_grid);
	src_grid.Reset();
	m_pet_count += 1;
	m_pet_storage_grid_use_num -= 1;
	this->OnRoleLevelUp(m_role_module_mgr->GetRole()->GetLevel(), &pet_index);
	this->AutoAssignAttributePoints(pet_index);

	this->SetItemDirty(pet_index, false);
	this->SetPetStorageItemDirty(pet_storage_index);
	this->GetAttributeList(pet_index, m_pet_base_attr[pet_index]);

	this->SendMoveNotice(Protocol::MOVE_PET_ITEM_NOTICE_TYPE_GRID, pet_index, pet_storage_index);
	EventHandler::Instance().OnPetStrorageMoveBag(m_role_module_mgr->GetRole(), target_grid.item_unique_id);

	return 0;
}

void Pet::SetPetStorageItemDirty(int pet_storage_index)
{
	if (pet_storage_index < 0 || pet_storage_index >= ItemNamespace::MAX_PET_STORAGE_GRID_NUM) return;

	m_storage_dirty_mark[pet_storage_index] = true;
	m_pet_grid_is_change = true;
}

void Pet::SendMoveNotice(int notice_type, int pet_index, int pet_storage_index)
{
	Protocol::SCMovePetGridNotice protocol;
	protocol.type = notice_type;
	protocol.pet_index = pet_index;
	protocol.pet_storage_index = pet_storage_index;

	m_role_module_mgr->NetSend(&protocol, sizeof(protocol));
}

void Pet::SynHuaShenPetData(ItemGridData * pet_grid_list, int count, short* pet_status_list, int status_count)
{
	int min_count = GetMin(count, ItemNamespace::MAX_PET_GRID_NUM);
	memcpy(pet_grid_list, m_grid_list, sizeof(ItemGridData) * min_count);
	memcpy(pet_status_list, m_role_module_mgr->GetCommonData().new_pet_status_index, sizeof(short) * status_count);
}

void Pet::HuaShenKeySyn(std::set<long long> * item_set)
{
	for (int i = 0; i < ItemNamespace::MAX_PET_GRID_NUM; ++i)
	{
		if (!this->PetInvalid(i))
		{
			PetParam & pet_param = m_grid_list[i].param->pet_param;
			m_role_module_mgr->GetHuaShenManager()->SynBenZunItemData(&m_grid_list[i], HUASHEN_KNAPSACK_OP_TYPE_KEY_SYN);
			if (NULL != item_set)
			{
				item_set->insert(m_grid_list[i].item_unique_id);
				//判断此时该宠物是否佩戴了装备
				if (m_grid_list[i].param->pet_param.equipment_item_unique_id != 0)
				{
					static NetValueItemParam p_nvip; p_nvip.Reset();
					pet_param.GetPetEquip(&p_nvip.pet_equipment_param);

					static ItemDataWrapper pet_equip; pet_equip.Reset();
					pet_equip.item_id = pet_param.pet_equipment_id;
					pet_equip.num = 1;
					pet_equip.has_param = 1;
					pet_equip.is_bind = 1;
					pet_equip.item_unique_id = pet_param.equipment_item_unique_id;
					p_nvip.GetData(pet_equip.param_data);
					m_role_module_mgr->GetHuaShenManager()->SynBenZunItemData(&pet_equip, HUASHEN_KNAPSACK_OP_TYPE_KEY_SYN);
					item_set->insert(pet_param.equipment_item_unique_id);
				}
			}
		}
	}
}

void Pet::HuaShenSkinSyn(std::map<int, std::vector<int> >* pet_has_skin_id)
{
	if (NULL != pet_has_skin_id)
	{
		*pet_has_skin_id = m_pet_has_skin_id;
		/*pet_has_skin_id->clear();
		std::map<int, std::vector<int> >::const_iterator iter = m_pet_has_skin_id.begin();
		for (; iter != m_pet_has_skin_id.end(); ++iter)
		{
			std::vector<int>::const_iterator it = iter->second.begin();
			for (; it != iter->second.end(); ++it)
			{
				(*pet_has_skin_id)[iter->first].push_back(*it);
		}*/
	}
/*
#ifdef _DEBUG
		{
			printf("Pet::HuaShenSkinSyn role[%d,%s] pet_skin_check!\n", m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName());

			std::map<int, std::set<int> >::const_iterator iter = m_pet_has_skin_id.begin();
			for (; iter != m_pet_has_skin_id.end(); ++iter)
			{
				printf("pet_id[%d] skin[", iter->first);

				std::set<int>::const_iterator it = iter->second.begin();
				for (; it != iter->second.end(); ++it)
				{
					printf("%d", *it);
				}
				printf("]\n");
			}
		}
#endif

#ifdef _DEBUG
		{
			printf("Pet::HuaShenSkinSyn__222 role[%d,%s] pet_skin_check!\n", m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName());

			std::map<int, std::set<int> >::const_iterator iter = pet_has_skin_id->begin();
			for (; iter != pet_has_skin_id->end(); ++iter)
			{
				printf("pet_id[%d] skin[", iter->first);

				std::set<int>::const_iterator it = iter->second.begin();
				for (; it != iter->second.end(); ++it)
				{
					printf("%d", *it);
				}
				printf("]\n");
			}
		}
#endif
	}*/
}

void Pet::RefreshPetEquipment(int seq, int pet_index, PetEquipMentParam * out_data)
{
	if (out_data == NULL)return;
	PetParam* pet_param = this->__GetPetParam(pet_index);
	if (pet_param == NULL)return;

	const ItemBase * tmp_itembase = ITEMPOOL->GetItem(pet_param->pet_equipment_id);
	if (tmp_itembase == NULL || tmp_itembase->GetItemType() != I_TYPE_PET_EQUIPMENT)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_PET_NO_EQUIPMENT);
		return;
	}
	const PetEquipment * pet_equip_ib = (const PetEquipment *)tmp_itembase;
	const PetEquipMentRefiningGroup * refresh_cfg = LOGIC_CONFIG->GetPetEquipMentConfig()->GetPetEquipMentRefiningCfg(seq);
	if (refresh_cfg == NULL)return;

	if (refresh_cfg->id != pet_param->pet_equipment_id)return;

	if (!m_role_module_mgr->GetKnapsack()->ConsumeItem(refresh_cfg->item_id, refresh_cfg->number, __FUNCTION__))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_ITEM_AMOUNT_NOT_ENOUGH);
		return;
	}

	UNSTD_STATIC_CHECK(MAX_PET_EQUIPMENT_SKILL_NUM == 2);
	gamelog::g_log_pet.printf(LL_INFO, "{Pet::RefreshPetEquipment old_data user[%s,%d] pet_index[%d] PetEquipMentParam:{item_id:%d, skill_num:%d, skill_1:%d, skill_2:%d re_skill_1:%d, re_skill_2:%d}}",
		m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetUid(), pet_index, pet_param->pet_equipment_id, pet_param->equipment_skill_num,
		pet_param->equipment_skill_list[0], pet_param->equipment_skill_list[1],
		pet_param->equipment_skill_refresh_list[0], pet_param->equipment_skill_refresh_list[1]);
	//获取随机配置

	if (pet_param->equipment_skill_num == 2)
	{
		//第一个技能
		const PetEquipMentSkillGroup * skill_cfg = LOGIC_CONFIG->GetPetEquipMentConfig()->GetSkillRandMap(pet_equip_ib->GetSkillGroup());
		if (skill_cfg != NULL)
		{
			if (skill_cfg->weight_count > 0)
			{
				int new_rand_num = skill_cfg->weight_count;
				int rand_num = rand() % new_rand_num;
				for (std::vector<PetEquipMentSkill>::const_iterator itor = skill_cfg->rand_vec.begin(); itor != skill_cfg->rand_vec.end(); itor++)
				{
					if (itor->weight >= rand_num)
					{
						pet_param->equipment_skill_refresh_list[0] = itor->gift_id;
						new_rand_num -= itor->weight;
						break;
					}
					else
					{
						rand_num -= itor->weight;
					}
				}
			}
		}
		else
		{
			gamelog::g_log_serious_error.printf(LL_INFO, "Pet::RefreshPetEquipment err %s [PetEquipMentSkill] m_skill_group[%d]", m_role_module_mgr->GetRoleInfoString().c_str(), pet_equip_ib->GetSkillGroup());
		}

		const PetEquipMentSkillGroup * skill_cfg2 = LOGIC_CONFIG->GetPetEquipMentConfig()->GetSkillRandMap(pet_equip_ib->GetSkillGroup2());
		if (skill_cfg2 != NULL)
		{
			if (skill_cfg2->weight_count > 0)
			{
				int new_rand_num = skill_cfg2->weight_count;
				int rand_num = rand() % new_rand_num;
				for (std::vector<PetEquipMentSkill>::const_iterator itor = skill_cfg2->rand_vec.begin(); itor != skill_cfg2->rand_vec.end(); itor++)
				{
					if (itor->weight >= rand_num)
					{
						pet_param->equipment_skill_refresh_list[1] = itor->gift_id;
						new_rand_num -= itor->weight;
						break;
					}
					else
					{
						rand_num -= itor->weight;
					}
				}
			}
		}
		else
		{
			gamelog::g_log_serious_error.printf(LL_INFO, "Pet::RefreshPetEquipment err %s [PetEquipMentSkill] m_skill_group2[%d]", m_role_module_mgr->GetRoleInfoString().c_str(), pet_equip_ib->GetSkillGroup2());
		}
	}
	else
	{
		const PetEquipMentSkillGroup * skill_cfg = LOGIC_CONFIG->GetPetEquipMentConfig()->GetSkillRandMap(pet_equip_ib->GetSkillGroup());
		if (skill_cfg == NULL)
		{
			gamelog::g_log_serious_error.printf(LL_INFO, "Pet::RefreshPetEquipment err %s [PetEquipMentSkill] m_skill_group[%d]", m_role_module_mgr->GetRoleInfoString().c_str(), pet_equip_ib->GetSkillGroup());
			return;
		}

		std::vector<PetEquipMentSkill> new_rand_vec;
		new_rand_vec.assign(skill_cfg->rand_vec.begin(), skill_cfg->rand_vec.end());
		if (skill_cfg->weight_count > 0)
		{
			int new_rand_num = skill_cfg->weight_count;
			for (int i = 0; i < pet_param->equipment_skill_num && i < MAX_PET_EQUIPMENT_SKILL_NUM; i++)
			{
				int rand_num = rand() % new_rand_num;
				for (std::vector<PetEquipMentSkill>::iterator itor = new_rand_vec.begin(); itor != new_rand_vec.end(); itor++)
				{
					if (itor->weight >= rand_num)
					{
						pet_param->equipment_skill_refresh_list[i] = itor->gift_id;
						new_rand_num -= itor->weight;
						new_rand_vec.erase(itor);
						break;
					}
					else
					{
						rand_num -= itor->weight;
					}
				}
			}
		}
	}
	pet_param->GetPetEquip(out_data);
	//this->ReCalcAttr(pet_index);//属性没变化不需要重算战力
	this->SetItemDirty(pet_index, true, Protocol::SCSendPetSingleInfo::PET_NOTIE_TYPE_PET_EQUIPMENT_CHANGE);
	if (this->IsInPlatfformBattle(m_grid_list[pet_index].item_unique_id))
	{
		this->SyncPetDataPlatformBattle(pet_index);
	}
	UNSTD_STATIC_CHECK(MAX_PET_EQUIPMENT_SKILL_NUM == 2);
	gamelog::g_log_pet.printf(LL_INFO, "{Pet::RefreshPetEquipment new_data user[%s,%d] pet_index[%d] PetEquipMentParam:{item_id:%d, skill_num:%d, skill_1:%d, skill_2:%d re_skill_1:%d, re_skill_2:%d}}",
		m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetUid(), pet_index, pet_param->pet_equipment_id, pet_param->equipment_skill_num,
		pet_param->equipment_skill_list[0], pet_param->equipment_skill_list[1],
		pet_param->equipment_skill_refresh_list[0], pet_param->equipment_skill_refresh_list[1]);
}

void Pet::SaveRefreshPetEquipment(int pet_index, PetEquipMentParam * out_data)
{
	if (out_data == NULL)return;
	PetParam* pet_param = this->__GetPetParam(pet_index);
	if (pet_param == NULL)return;

	const ItemBase * tmp_itembase = ITEMPOOL->GetItem(pet_param->pet_equipment_id);
	if (tmp_itembase == NULL || tmp_itembase->GetItemType() != I_TYPE_PET_EQUIPMENT)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_PET_NO_EQUIPMENT);
		return;
	}

	if (pet_param->equipment_skill_refresh_list[0] == 0 && pet_param->equipment_skill_refresh_list[1] == 0)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_ITEM_NOT_ELIGIBLE);
		return;
	}

	UNSTD_STATIC_CHECK(MAX_PET_EQUIPMENT_SKILL_NUM == 2);
	gamelog::g_log_pet.printf(LL_INFO, "{Pet::SaveRefreshPetEquipment old_data user[%s,%d] pet_index[%d] PetEquipMentParam:{item_id:%d, skill_num:%d, skill_1:%d, skill_2:%d re_skill_1:%d, re_skill_2:%d}}",
		m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetUid(), pet_index, pet_param->pet_equipment_id, pet_param->equipment_skill_num,
		pet_param->equipment_skill_list[0], pet_param->equipment_skill_list[1],
		pet_param->equipment_skill_refresh_list[0], pet_param->equipment_skill_refresh_list[1]);

	for (int i = 0; i < pet_param->equipment_skill_num; i++)
	{
		pet_param->equipment_skill_list[i] = pet_param->equipment_skill_refresh_list[i];
		pet_param->equipment_skill_refresh_list[i] = 0;//保存了之后就清理掉
	}
	pet_param->GetPetEquip(out_data);
	this->ReCalcAttr(pet_index);
	this->SetItemDirty(pet_index, true, Protocol::SCSendPetSingleInfo::PET_NOTIE_TYPE_PET_EQUIPMENT_CHANGE);
	if (this->IsInPlatfformBattle(m_grid_list[pet_index].item_unique_id))
	{
		this->SyncPetDataPlatformBattle(pet_index);
	}
	UNSTD_STATIC_CHECK(MAX_PET_EQUIPMENT_SKILL_NUM == 2);
	gamelog::g_log_pet.printf(LL_INFO, "{Pet::SaveRefreshPetEquipment new_data user[%s,%d] pet_index[%d] PetEquipMentParam:{item_id:%d, skill_num:%d, skill_1:%d, skill_2:%d re_skill_1:%d, re_skill_2:%d}}",
		m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetUid(), pet_index, pet_param->pet_equipment_id, pet_param->equipment_skill_num,
		pet_param->equipment_skill_list[0], pet_param->equipment_skill_list[1],
		pet_param->equipment_skill_refresh_list[0], pet_param->equipment_skill_refresh_list[1]);
}

void Pet::ChangePetSoulEquip(int is_put_on, int index, int new_item_id,int pet_soul_equipment_index)
{
	const ItemBase * new_item = ITEMPOOL->GetItem(new_item_id);
	if (new_item == NULL)return;

	if (new_item->GetItemType() != I_TYPE_PET_SOUL_EQUIP)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	//检查消耗
	const PetSoulEquipment* soul_item_base = (const PetSoulEquipment*)new_item;
	const PetSoulEquipBaseCfg * new_item_cfg = LOGIC_CONFIG->GetPetSoulEquipMentConfig()->GetPetSoulEquipBaseCfg(soul_item_base->GetPetWeaponSeq());
	if (NULL == new_item_cfg)return;
	int cost_item_id = new_item_cfg->change_item;
	int cost_item_num = new_item_cfg->change_item_num;
	if (new_item_cfg->change_item > 0 && new_item_cfg->change_item_num)
	{
		if (!m_role_module_mgr->GetKnapsack()->HasItem(cost_item_id, cost_item_num))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_ITEM_AMOUNT_NOT_ENOUGH);
			return;
		}
	}

	ItemGridData* tmp_equip1 = NULL;

	int color = 0;
	short *level = NULL;
	short *steps = NULL;
	int *refine = NULL;
	int *m_exp = NULL;
	int *m_max_exp = NULL;
	unsigned short *pet_equipment_id = NULL;
	short *sp_id = NULL;
	//属性词条
	unsigned short *attr_num = NULL;
	AttrInfo * attr_list[MAX_PET_SOUL_EQUIPMENT_ATTR_NUM] = { NULL };
	if (WORKSHOP_EQUIP_POSI_EQUIP == is_put_on)
	{
		if (this->PetInvalid(index)) return;
		PetParam& pet_param = m_grid_list[index].param->pet_param;

		PetSoulEquip * soul_equip_ptr = pet_param.GetPetSoulEquipInPetParam(pet_soul_equipment_index);
		if (NULL == soul_equip_ptr)return;
		PetSoulEquip & soul_equip = *soul_equip_ptr;

		const ItemBase * item_base = ITEMPOOL->GetItem(soul_equip.item_id);
		if (item_base == NULL || item_base->GetItemType() != I_TYPE_PET_SOUL_EQUIP)return;
		color = item_base->GetColor();
		level = &soul_equip.m_level;
		steps = &soul_equip.m_gread;
		refine = &soul_equip.m_refine;
		pet_equipment_id = &soul_equip.item_id;
		sp_id = &soul_equip.sp_id;
		m_exp = &soul_equip.exp;
		m_max_exp = &soul_equip.m_max_exp;
		attr_num = &soul_equip.attr_num;
		for (int attr_idx = 0; attr_idx < MAX_PET_SOUL_EQUIPMENT_ATTR_NUM && attr_idx < *attr_num; attr_idx++)
		{
			attr_list[attr_idx] = &(soul_equip.add_attr_list[attr_idx]);
		}
	}
	else
	{
		tmp_equip1 = m_role_module_mgr->GetKnapsack()->GetItemGridData2(ItemNamespace::ITEM_COLUMN_TYPE_PET_SOUL_EQIOP, index);
		if (tmp_equip1 == NULL)return;
		const ItemBase * item_base = ITEMPOOL->GetItem(tmp_equip1->item_id);
		if (item_base == NULL || item_base->GetItemType() != I_TYPE_PET_SOUL_EQUIP)return;
		color = item_base->GetColor();
		level = &tmp_equip1->param->pet_soul_equipment_param.level;
		steps = &tmp_equip1->param->pet_soul_equipment_param.steps;
		refine = &tmp_equip1->param->pet_soul_equipment_param.refine;
		pet_equipment_id = &tmp_equip1->param->pet_soul_equipment_param.pet_equipment_id;
		sp_id = &tmp_equip1->param->pet_soul_equipment_param.sp_id;
		m_exp = &tmp_equip1->param->pet_soul_equipment_param.exp;
		m_max_exp = &tmp_equip1->param->pet_soul_equipment_param.m_max_exp;
		attr_num = &tmp_equip1->param->pet_soul_equipment_param.attr_num;
		for (int attr_idx = 0; attr_idx < MAX_PET_SOUL_EQUIPMENT_ATTR_NUM && attr_idx < *attr_num; attr_idx++)
		{
			attr_list[attr_idx] = &(tmp_equip1->param->pet_soul_equipment_param.add_attr_list[attr_idx]);
		}
	}

	//获取新魂器初始配置属性
	NetValueItemParam new_param;
	soul_item_base->RandomNetValueOnPutItem(m_role_module_mgr->GetRole(), &new_param, PUT_REASON_PET_SOUL_EQUIP_CHANGE);
	PetSoulEquipMentParam &new_pet_soul_param = new_param.pet_soul_equipment_param;
	//升级新魂器到目标等级
	{
		for (int level_next = new_pet_soul_param.level; level_next < *level; level_next++)
		{
			//随机增加属性
			for (int attr_idx = 0; attr_idx < *attr_num && attr_idx < MAX_PET_SOUL_EQUIPMENT_ATTR_NUM; attr_idx++)
			{
				const AttributeIntensifyValueCfg * attr_cfg = LOGIC_CONFIG->GetPetSoulEquipMentConfig()->GetPetSoulEquipIntensifyValueCfg(color, new_pet_soul_param.add_attr_list[attr_idx].attr_type);
				if (attr_cfg != NULL)
				{
					int rand_attr = 0;
					if ((attr_cfg->intensify_att_max - attr_cfg->intensify_att_min + 1) != 0)
					{
						rand_attr = rand() % (attr_cfg->intensify_att_max - attr_cfg->intensify_att_min + 1) + attr_cfg->intensify_att_min;
					}
					else
					{
						rand_attr = attr_cfg->intensify_att_min;
					}
					new_pet_soul_param.add_attr_list[attr_idx].attr_value += rand_attr;
				}
			}
		}
		new_pet_soul_param.level = *level;
	}
	//突破新魂器到目标等级
	{
		new_pet_soul_param.attr_num = *attr_num;
		for (int steps_next = new_pet_soul_param.steps; steps_next < *steps; steps_next++)
		{
			for (int attr_idx = 0; attr_idx < new_pet_soul_param.attr_num && attr_idx < MAX_PET_SOUL_EQUIPMENT_ATTR_NUM; attr_idx++)
			{
				const PetSoulEquipStepsAttrCfg * attr_cfg = LOGIC_CONFIG->GetPetSoulEquipMentConfig()->GetPetSoulEquipStepsAddValueCfg(color, new_pet_soul_param.add_attr_list[attr_idx].attr_type);
				if (attr_cfg != NULL)
				{
					int rand_attr = 0;
					if ((attr_cfg->intensify_att_max - attr_cfg->intensify_att_min + 1) != 0)
					{
						rand_attr = rand() % (attr_cfg->intensify_att_max - attr_cfg->intensify_att_min + 1) + attr_cfg->intensify_att_min;
					}
					else
					{
						rand_attr = attr_cfg->intensify_att_min;
					}
					new_pet_soul_param.add_attr_list[attr_idx].attr_value += rand_attr;
				}
			}
		}
	}
	//精炼新魂器到目标等级
	{
		const PetSoulEquipBaseCfg * soul_base_cfg = LOGIC_CONFIG->GetPetSoulEquipMentConfig()->GetPetSoulEquipBaseCfg(soul_item_base->GetPetWeaponSeq());
		if (soul_base_cfg == NULL)return;
		const PetEquipSoulRefineCfg * refine_cfg = LOGIC_CONFIG->GetPetSoulEquipMentConfig()->GetPetSoulEquipRefineCfg(soul_base_cfg->refine_group, *refine);
		if (refine_cfg != NULL)
		{
			new_pet_soul_param.sp_id = refine_cfg->new_spid;
			new_pet_soul_param.refine = *refine;
		}
	}

	//消耗道具

	if (cost_item_num > 0 && !m_role_module_mgr->GetKnapsack()->ConsumeItem(cost_item_id, cost_item_num, __FUNCTION__))return;

	gamelog::g_log_pet.printf(LL_INFO, "%s user[%s,%d] is_puton[%d] pet_soul_equipment_index[%d] old_equip_id[%d] to_new_id[%d]", __FUNCTION__,
		m_role_module_mgr->GetRoleName(), m_role_module_mgr->GetUid(), is_put_on, pet_soul_equipment_index, pet_equipment_id != NULL ? *pet_equipment_id : 0, new_item_id);//日志魂器道具变更基本操作信息

	if (WORKSHOP_EQUIP_POSI_EQUIP == is_put_on)
	{
		gamelog::g_log_pet.printf(LL_INFO, "%s user[%s,%d] item_id[%d] old_param[%s] new_param[%s]", __FUNCTION__,
			m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetUid(), m_grid_list[index].item_id, m_grid_list[index].param->GetParamLog(I_TYPE_PET), new_param.GetParamLog(I_TYPE_PET_SOUL_EQUIP));
	}
	else
	{
		gamelog::g_log_pet.printf(LL_INFO, "%s user[%s,%d] item_id[%d] old_param[%s] new_param[%s]", __FUNCTION__,
			m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetUid(), tmp_equip1->item_id, tmp_equip1->param->GetParamLog(I_TYPE_PET_SOUL_EQUIP), new_param.GetParamLog(I_TYPE_PET_SOUL_EQUIP));
	}

	//新数据替换到旧魂器中
	*pet_equipment_id = (unsigned int)new_item_id;
	*sp_id = new_pet_soul_param.sp_id;
	*attr_num = new_pet_soul_param.attr_num;

	for (int i = 0; i < MAX_PET_SOUL_EQUIPMENT_ATTR_NUM; i++)
	{
		if (attr_list[i] == NULL)continue;
		attr_list[i]->attr_type = 0;
		attr_list[i]->attr_value = 0;
	}

	for (int i = 0; i < new_pet_soul_param.attr_num; i++)
	{
		if (attr_list[i] == NULL)continue;
		attr_list[i]->attr_type = new_pet_soul_param.add_attr_list[i].attr_type;
		attr_list[i]->attr_value = new_pet_soul_param.add_attr_list[i].attr_value;
	}

	if (is_put_on == WORKSHOP_EQUIP_POSI_EQUIP)
	{
		this->ReCalcAttr(index);
		this->SetItemDirty(index, true, Protocol::SCSendPetSingleInfo::PET_NOTIE_TYPE_PET_SOUL_EXCHANGE);
	}
	else
	{
		tmp_equip1->item_id = new_item_id;
		m_role_module_mgr->GetKnapsack()->SetItemDirty(ItemNamespace::ITEM_COLUMN_TYPE_PET_SOUL_EQIOP, index, true, Knapsack::ITEM_CHANGE_NOTICE_REASON_PUT, PUT_REASON_PET_SOUL_EQUIP_CHANGE);
	}
	m_role_module_mgr->NoticeNum(noticenum::NT_PET_SOUL_EQUIP_CHANGE_SUCC);
	this->CheckPetSoulCollection(true);
	m_role_module_mgr->GetKnapsack()->CheckPetSoulCollection(true);
}

void Pet::ExchangePetSoulEquip(int seq)
{
	const PetEquipSoulExchangeCfg * exchange_cfg = LOGIC_CONFIG->GetPetSoulEquipMentConfig()->GetEquipSoulExchangeCfg(seq);
	if (exchange_cfg != NULL)
	{
		if (!m_role_module_mgr->GetKnapsack()->CheckForPut(exchange_cfg->pet_weapon_item_id, 1))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_ITEM_COLUMN_NO_SPACE);
			return;
		}

		if (exchange_cfg->expend_num > 0)
		{
			if (!m_role_module_mgr->GetKnapsack()->HasItem(exchange_cfg->expend_item_id, exchange_cfg->expend_num))
			{
				m_role_module_mgr->NoticeNum(errornum::EN_ITEM_AMOUNT_NOT_ENOUGH);
				return;
			}
			m_role_module_mgr->GetKnapsack()->ConsumeItem(exchange_cfg->expend_item_id, exchange_cfg->expend_num, __FUNCTION__);
		}

		m_role_module_mgr->GetKnapsack()->Put(ItemConfigData(exchange_cfg->pet_weapon_item_id, 1, 1), PUT_REASON_EXCHANGE_PET_SOUL_EQUIP);
	}
}

bool Pet::PutOnPetSoulEquip(int pet_index, int equipment_bag_index,int pet_soul_equipment_index)
{
	if (this->PetInvalid(pet_index)) return false;
	PetParam& pet_param = m_grid_list[pet_index].param->pet_param;

	ItemDataWrapper tmp_equip1; tmp_equip1.Reset();

	if (!m_role_module_mgr->GetKnapsack()->GetItemGridWrapData(ItemNamespace::ITEM_COLUMN_TYPE_PET_SOUL_EQIOP, equipment_bag_index, &tmp_equip1))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_LOAD_ERROR);
		return false;
	}

	const ItemBase * tmp_itembase = ITEMPOOL->GetItem(tmp_equip1.item_id);
	if (tmp_itembase == NULL || tmp_itembase->GetItemType() != I_TYPE_PET_SOUL_EQUIP)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_ITEM_NOT_ELIGIBLE);
		return false;
	}

	const PetSoulEquipment * soul_base = (const PetSoulEquipment *)tmp_itembase;

	const PetSoulEquipBaseCfg * base_cfg = LOGIC_CONFIG->GetPetSoulEquipMentConfig()->GetPetSoulEquipBaseCfg(soul_base->GetPetWeaponSeq());
	if (base_cfg != NULL && base_cfg->pet_weapon_pet_id > 0)
	{
		if (pet_param.pet_id != base_cfg->pet_weapon_pet_id)
		{
			//m_role_module_mgr->NoticeNum(errornum::EN_PET_SOUL_BELONG_ERR);
			//return false;
		}
	}

	PetSoulEquip * soul_equip_ptr = pet_param.GetPetSoulEquipInPetParam(pet_soul_equipment_index);
	if (NULL == soul_equip_ptr) return false;
	PetSoulEquip & soul_equip = *soul_equip_ptr;

	if (!soul_equip.Invalid())//有装备
	{
		if (!this->PutOffPetSoulEquip(pet_index, pet_soul_equipment_index))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_PET_SOUL_WEAPON_PARAM_ERR);
			return false;
		}
	}

	if (!m_role_module_mgr->GetKnapsack()->ConsumeItemByIndex(ItemNamespace::ITEM_COLUMN_TYPE_PET_SOUL_EQIOP, equipment_bag_index, 1, __FUNCTION__))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_USE_ITEM_ERROR);
		return false;
	}

	NetValueItemParam param;
	param.SetData(tmp_equip1.param_data);
	soul_equip.item_id = tmp_equip1.item_id;
	soul_equip.item_unique_id = tmp_equip1.item_unique_id;
	soul_equip.is_bind = (char)tmp_equip1.is_bind;
	pet_param.SetPetSoulEquip(&param.pet_soul_equipment_param, pet_soul_equipment_index);

	this->ReCalcAttr(pet_index);
	this->SetItemDirty(pet_index, true, Protocol::SCSendPetSingleInfo::PET_NOTIE_TYPE_PET_SOUL_EQUIPMENT);
	EventHandler::Instance().OnPetPutSoulEquip(m_role_module_mgr->GetRole());
	if (this->IsInPlatfformBattle(m_grid_list[pet_index].item_unique_id))
	{
		this->SyncPetDataPlatformBattle(pet_index);
	}
	return true;
}

bool Pet::PutOffAllPetSoulEquip(int pet_index)
{
	if (this->PetInvalid(pet_index)) return false;
	PetParam & pet = m_grid_list[pet_index].param->pet_param;

	if (!pet.m_soul_equip_1.Invalid())
	{
		if (!this->PutOffPetSoulEquip(pet_index, 0))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_PET_WEAR_SOUL_EQUIP);
			return false;
		}
	}

	if (!pet.m_soul_equip_2.Invalid())
	{
		if (!this->PutOffPetSoulEquip(pet_index, 1))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_PET_WEAR_SOUL_EQUIP);
			return false;
		}
	}

	return true;
}

bool Pet::PutOffPetSoulEquip(int pet_index, int pet_soul_equipment_index)
{
	if (this->PetInvalid(pet_index)) return false;
	PetParam & pet_param = m_grid_list[pet_index].param->pet_param;

	PetSoulEquip * soul_equip_ptr = pet_param.GetPetSoulEquipInPetParam(pet_soul_equipment_index);
	if (NULL == soul_equip_ptr)return true;
	PetSoulEquip & soul_equip = *soul_equip_ptr;

	if (soul_equip.Invalid())
	{
		return true;
	}

	const ItemBase * tmp_itembase = ITEMPOOL->GetItem(soul_equip.item_id);
	if (tmp_itembase == NULL || tmp_itembase->GetItemType() != I_TYPE_PET_SOUL_EQUIP)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_ITEM_NOT_ELIGIBLE);
		return false;
	}
	static NetValueItemParam p_nvip; p_nvip.Reset();
	pet_param.GetPetSoulEquip(&p_nvip.pet_soul_equipment_param, pet_soul_equipment_index);

	ItemDataWrapper pet_equip; pet_equip.Reset();
	pet_equip.item_id = soul_equip.item_id;
	pet_equip.num = 1;
	pet_equip.has_param = 1;
	pet_equip.is_bind = (short)soul_equip.is_bind;
	pet_equip.item_unique_id = soul_equip.item_unique_id;
	p_nvip.GetData(pet_equip.param_data);

	if (!m_role_module_mgr->GetKnapsack()->Put(pet_equip, PUT_REASON_PET_SOUL_EQUIPMENT_TAKEOFF))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return false;
	}

	pet_param.ClearPetSoulEquipment(pet_soul_equipment_index);
	this->ReCalcAttr(pet_index);
	this->SetItemDirty(pet_index, true, Protocol::SCSendPetSingleInfo::PET_NOTIE_TYPE_PET_SOUL_EQUIPMENT);
	if (this->IsInPlatfformBattle(m_grid_list[pet_index].item_unique_id))
	{
		this->SyncPetDataPlatformBattle(pet_index);
	}
	return true;
}

void Pet::UpLevelPetSoulEquip(int is_put_on, int index, int cost_num, short cost_column_list[Protocol::CSPetSoulUplevelReq::MAX_USE_ITEM_NUM], 
	short cost_item_list[Protocol::CSPetSoulUplevelReq::MAX_USE_ITEM_NUM], short cost_item_num[Protocol::CSPetSoulUplevelReq::MAX_USE_ITEM_NUM], 
	int pet_soul_equipment_index)
{
	ItemGridData tmp_equip1, cost_item; tmp_equip1.Reset(); cost_item.Reset();

	int color = 0;
	short *level = NULL;
	short *steps = NULL;
	int *m_exp = NULL;
	int *m_max_exp = NULL;
	//属性词条
	unsigned short *attr_num = NULL;
	AttrInfo * attr_list[MAX_PET_SOUL_EQUIPMENT_ATTR_NUM] = { NULL };
	if (WORKSHOP_EQUIP_POSI_EQUIP == is_put_on)
	{
		if (this->PetInvalid(index)) return;
		PetParam& pet_param = m_grid_list[index].param->pet_param;
		PetSoulEquip * soul_equip_ptr = pet_param.GetPetSoulEquipInPetParam(pet_soul_equipment_index);
		if (NULL == soul_equip_ptr)return;
		PetSoulEquip & soul_equip = *soul_equip_ptr;

		const ItemBase * item_base = ITEMPOOL->GetItem(soul_equip.item_id);
		if (item_base == NULL || item_base->GetItemType() != I_TYPE_PET_SOUL_EQUIP)return;
		color = item_base->GetColor();
		level = &soul_equip.m_level;
		steps = &soul_equip.m_gread;
		m_exp = &soul_equip.exp;
		m_max_exp = &soul_equip.m_max_exp;
		attr_num = &soul_equip.attr_num;
		for (int attr_idx = 0; attr_idx < MAX_PET_SOUL_EQUIPMENT_ATTR_NUM && attr_idx < *attr_num; attr_idx++)
		{
			attr_list[attr_idx] = &(soul_equip.add_attr_list[attr_idx]);
		}
	}
	else
	{
		if (!m_role_module_mgr->GetKnapsack()->GetItemGridData(ItemNamespace::ITEM_COLUMN_TYPE_PET_SOUL_EQIOP, index, &tmp_equip1)) return;
		const ItemBase * item_base = ITEMPOOL->GetItem(tmp_equip1.item_id);
		if (item_base == NULL || item_base->GetItemType() != I_TYPE_PET_SOUL_EQUIP)return;
		color = item_base->GetColor();
		level = &tmp_equip1.param->pet_soul_equipment_param.level;
		steps = &tmp_equip1.param->pet_soul_equipment_param.steps;
		m_exp = &tmp_equip1.param->pet_soul_equipment_param.exp;
		m_max_exp = &tmp_equip1.param->pet_soul_equipment_param.m_max_exp;
		attr_num = &tmp_equip1.param->pet_soul_equipment_param.attr_num;
		for (int attr_idx = 0; attr_idx < MAX_PET_SOUL_EQUIPMENT_ATTR_NUM && attr_idx < *attr_num; attr_idx++)
		{
			attr_list[attr_idx] = &(tmp_equip1.param->pet_soul_equipment_param.add_attr_list[attr_idx]);
		}
	}

	//计算增加的经验值
	int extra_exp = 0;//额外多余经验
	for (int item_idx = 0; item_idx < cost_num && item_idx < Protocol::CSPetSoulUplevelReq::MAX_USE_ITEM_NUM; item_idx++)
	{
		if (!m_role_module_mgr->GetKnapsack()->GetItemGridData(cost_column_list[item_idx], cost_item_list[item_idx], &cost_item)) continue;
		const ItemBase * item_base = ITEMPOOL->GetItem(cost_item.item_id);
		if(item_base == NULL)continue;

		{
			//检查升级配置
			const AttributeUplevelCfg * pet_soul_level_cfg = LOGIC_CONFIG->GetPetSoulEquipMentConfig()->GetPetSoulEquipUpLevelCfg(color, *level);
			if (pet_soul_level_cfg == NULL)break;
			if (*steps < pet_soul_level_cfg->need_steps)break;
			const AttributeUplevelCfg * next_pet_soul_level_cfg = LOGIC_CONFIG->GetPetSoulEquipMentConfig()->GetPetSoulEquipUpLevelCfg(color, *level + 1);
			if (next_pet_soul_level_cfg == NULL)break;
		}

		int add_exp = 0;
		const PetSoulEquipOtherCfg & other_cfg = LOGIC_CONFIG->GetPetSoulEquipMentConfig()->GetOtherCfg();
		if (item_base->GetItemType() == I_TYPE_PET_SOUL_EQUIP)//消耗的是同等装备
		{
			int real_add_exp = 0;
			const PetSoulEquipment * soul_base = (const PetSoulEquipment *)item_base;
			
			//已有经验
			real_add_exp += cost_item.param->pet_soul_equipment_param.exp;

			//等级换算经验值
			for (int soul_level = cost_item.param->pet_soul_equipment_param.level - 1; soul_level > 0; soul_level--)
			{
				const AttributeUplevelCfg * up_level_cfg = LOGIC_CONFIG->GetPetSoulEquipMentConfig()->GetPetSoulEquipUpLevelCfg(soul_base->GetColor(), soul_level);
				if (up_level_cfg != NULL)
				{
					real_add_exp += up_level_cfg->exp;
				}
			}

			add_exp += real_add_exp * other_cfg.exp_attenuation_100 / 100;

			//基础经验值
			const PetSoulEquipBaseCfg * soul_base_cfg = LOGIC_CONFIG->GetPetSoulEquipMentConfig()->GetPetSoulEquipBaseCfg(soul_base->GetPetWeaponSeq());
			if (NULL != soul_base_cfg)
			{
				add_exp += soul_base_cfg->levels_1_pet_weapon_exp;
			}
		}
		else
		{
			const PetSoulEquipItemIdExpCfg * item_exp_cfg = LOGIC_CONFIG->GetPetSoulEquipMentConfig()->GetPetSoulEquipItemIdExpCfg(cost_item.item_id);
			if (item_exp_cfg != NULL)
			{
				add_exp += item_exp_cfg->item_exp * cost_item_num[item_idx];
			}
		}

		if (add_exp <= 0)return;

		int suppose_level = *level;
		int suppose_exp = *m_exp;
		int suppose_max_exp = *m_max_exp;
		int suppose_steps = *steps;
		int suppose_cost_coin = 0;
		int suppose_cost_exp = 0;

		for (int round = 0; round < 50; round++)//先假设升级
		{
			if (add_exp + suppose_exp >= suppose_max_exp)
			{
				//满足升级了检测一下
				const AttributeUplevelCfg * pet_soul_level_cfg = LOGIC_CONFIG->GetPetSoulEquipMentConfig()->GetPetSoulEquipUpLevelCfg(color, suppose_level);
				if (pet_soul_level_cfg != NULL)
				{
					if (suppose_steps < pet_soul_level_cfg->need_steps)
					{
						break;
					}

					int real_cost_exp = suppose_max_exp - suppose_exp;
					if (real_cost_exp > 0)
					{
						//消耗铜币
						suppose_cost_coin += real_cost_exp * other_cfg.cost_coin / 100;//累积假设铜币消耗数
						if (!m_role_module_mgr->GetMoney()->CoinBindMoreThan(suppose_cost_coin))
						{
							m_role_module_mgr->NoticeNum(errornum::EN_COIN_NOT_ENOUGH);
							return;
						}
					}

					//检测下一级是否存在
					const AttributeUplevelCfg * next_pet_soul_level_cfg = LOGIC_CONFIG->GetPetSoulEquipMentConfig()->GetPetSoulEquipUpLevelCfg(color, suppose_level + 1);
					if (next_pet_soul_level_cfg != NULL)
					{
						add_exp -= real_cost_exp;
						suppose_cost_exp += real_cost_exp;
						suppose_exp = 0;
						suppose_level = next_pet_soul_level_cfg->levels;
						suppose_max_exp = next_pet_soul_level_cfg->exp;
					}
					else//没有下一级了
					{
						suppose_cost_exp += suppose_max_exp - suppose_exp;
						suppose_exp = suppose_max_exp;
						extra_exp = add_exp;//超等级额外经验返还
						add_exp = 0;
						break;
					}
				}
			}
			else
			{
				break;
			}
		}

		if (add_exp > 0)//还有剩余的经验加上
		{
			if (add_exp + suppose_exp >= suppose_max_exp)
			{
				suppose_cost_exp += suppose_max_exp - suppose_exp;
				suppose_cost_coin += (suppose_max_exp - suppose_exp) * other_cfg.cost_coin / 100;//累积假设铜币消耗数
				suppose_exp = suppose_max_exp;
				extra_exp += add_exp - (suppose_max_exp - suppose_exp);//超等级额外经验返还
				if (!m_role_module_mgr->GetMoney()->CoinBindMoreThan(suppose_cost_coin))
				{
					m_role_module_mgr->NoticeNum(errornum::EN_COIN_NOT_ENOUGH);
					return;
				}
			}
			else
			{
				suppose_exp += add_exp;
				suppose_cost_exp += add_exp;
				suppose_cost_coin += add_exp * other_cfg.cost_coin / 100;//累积假设铜币消耗数
				if (!m_role_module_mgr->GetMoney()->CoinBindMoreThan(suppose_cost_coin))
				{
					m_role_module_mgr->NoticeNum(errornum::EN_COIN_NOT_ENOUGH);
					return;
				}
			}
		}

		//检查消耗的是不是道具而非装备
		int real_cost_item_num = 0;
		if (item_base->GetItemType() != I_TYPE_PET_SOUL_EQUIP)
		{
			//检查经验消耗对应消耗的道具数量
			const PetSoulEquipItemIdExpCfg * item_exp_cfg = LOGIC_CONFIG->GetPetSoulEquipMentConfig()->GetPetSoulEquipItemIdExpCfg(cost_item.item_id);
			if (item_exp_cfg != NULL && item_exp_cfg->item_exp > 0)
			{
				real_cost_item_num = suppose_cost_exp / item_exp_cfg->item_exp;
				if (suppose_cost_exp % item_exp_cfg->item_exp != 0)
				{
					real_cost_item_num += 1;//有多余的直接多扣一个道具
				}
			}
		}
		else
		{
			real_cost_item_num = 1;//装备为1
		}

		//消耗道具与铜币

		if (real_cost_item_num > 0 && !m_role_module_mgr->GetKnapsack()->ConsumeItemByIndex(cost_column_list[item_idx], cost_item_list[item_idx], real_cost_item_num, __FUNCTION__))
		{
			return;
		}

		m_role_module_mgr->GetMoney()->UseCoinBind(suppose_cost_coin, __FUNCTION__);

		//多余经验返还
		if (extra_exp > 0)
		{
			//检查经验消耗对应消耗的道具数量
			std::vector<ItemConfigData> back_item_vec;
			LOGIC_CONFIG->GetPetSoulEquipMentConfig()->GetBackItemByExtraExp(extra_exp, &back_item_vec);
			if ((int)back_item_vec.size() > 0)
			{
				if (!m_role_module_mgr->GetKnapsack()->PutList((int)back_item_vec.size(), &back_item_vec[0], PUT_REASON_PET_SOUL_EQUIP_CHANGE))
				{
					m_role_module_mgr->GetKnapsack()->SendMail(&back_item_vec[0], (int)back_item_vec.size());
				}
			}
		}

		for (int level_next = *level; level_next < suppose_level; level_next++)
		{
			//随机增加属性
			for (int attr_idx = 0; attr_idx < *attr_num && attr_idx < MAX_PET_SOUL_EQUIPMENT_ATTR_NUM; attr_idx++)
			{
				if (attr_list[attr_idx] != NULL)
				{
					const AttributeIntensifyValueCfg * attr_cfg = LOGIC_CONFIG->GetPetSoulEquipMentConfig()->GetPetSoulEquipIntensifyValueCfg(color, attr_list[attr_idx]->attr_type);
					if (attr_cfg != NULL)
					{
						int rand_attr = 0;
						if ((attr_cfg->intensify_att_max - attr_cfg->intensify_att_min + 1) != 0)
						{
							rand_attr = rand() % (attr_cfg->intensify_att_max - attr_cfg->intensify_att_min + 1) + attr_cfg->intensify_att_min;
						}
						else
						{
							rand_attr = attr_cfg->intensify_att_min;
						}
						attr_list[attr_idx]->attr_value += rand_attr;
					}
				}
			}
		}

		*level = suppose_level;
		*m_max_exp = suppose_max_exp;
		*m_exp = suppose_exp;
	}
	
	m_role_module_mgr->NoticeNum(noticenum::NT_PET_SOUL_EQUIP_UPLEVEL);

	if (WORKSHOP_EQUIP_POSI_EQUIP == is_put_on)
	{
		gamelog::g_log_pet.printf(LL_INFO, "%s user[%s,%d] item_id[%d] new_param[%s]", __FUNCTION__,
			m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetUid(), m_grid_list[index].item_id, m_grid_list[index].param->GetParamLog(I_TYPE_PET));
	}
	else
	{
		gamelog::g_log_pet.printf(LL_INFO, "%s user[%s,%d] item_id[%d] new_param[%s]", __FUNCTION__,
			m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetUid(), tmp_equip1.item_id, tmp_equip1.param->GetParamLog(I_TYPE_PET_SOUL_EQUIP));
	}

	if (is_put_on == WORKSHOP_EQUIP_POSI_EQUIP)
	{
		this->ReCalcAttr(index);
		this->SetItemDirty(index, true, Protocol::SCSendPetSingleInfo::PET_NOTIE_TYPE_PET_SOUL_DATA_CHANGE);
	}
	else
	{
		m_role_module_mgr->GetKnapsack()->SetItemDirty(ItemNamespace::ITEM_COLUMN_TYPE_PET_SOUL_EQIOP, index, true, Knapsack::ITEM_CHANGE_NOTICE_REASON_DATA_CHANGE);
	}
}

void Pet::StepsPetSoulEquip(int is_put_on, int index,int pet_soul_equipment_index)
{
	ItemGridData tmp_equip1; tmp_equip1.Reset();
	short color = 0;
	short *level = NULL;
	short *steps = NULL;
	int *m_exp = NULL;
	int *m_max_exp = NULL;
	//属性词条
	unsigned short *attr_num = NULL;
	AttrInfo * attr_list[MAX_PET_SOUL_EQUIPMENT_ATTR_NUM] = { NULL };

	if (WORKSHOP_EQUIP_POSI_EQUIP == is_put_on)
	{
		if (this->PetInvalid(index)) return;
		PetParam& pet_param = m_grid_list[index].param->pet_param;
		PetSoulEquip * soul_equip_ptr = pet_param.GetPetSoulEquipInPetParam(pet_soul_equipment_index);
		if (NULL == soul_equip_ptr)return;
		PetSoulEquip & soul_equip = *soul_equip_ptr;

		const ItemBase * item_base = ITEMPOOL->GetItem(soul_equip.item_id);
		if (item_base == NULL || item_base->GetItemType() != I_TYPE_PET_SOUL_EQUIP)return;

		level = &soul_equip.m_level;
		steps = &soul_equip.m_gread;
		m_exp = &soul_equip.exp;
		m_max_exp = &soul_equip.m_max_exp;
		attr_num = &soul_equip.attr_num;
		for (int attr_idx = 0; attr_idx < MAX_PET_SOUL_EQUIPMENT_ATTR_NUM && attr_idx < *attr_num; attr_idx++)
		{
			attr_list[attr_idx] = &(soul_equip.add_attr_list[attr_idx]);
		}
		color = item_base->GetColor();
	}
	else
	{
		if (!m_role_module_mgr->GetKnapsack()->GetItemGridData(ItemNamespace::ITEM_COLUMN_TYPE_PET_SOUL_EQIOP, index, &tmp_equip1)) return;
		const ItemBase * item_base = ITEMPOOL->GetItem(tmp_equip1.item_id);
		if (item_base == NULL || item_base->GetItemType() != I_TYPE_PET_SOUL_EQUIP)return;

		level = &tmp_equip1.param->pet_soul_equipment_param.level;
		steps = &tmp_equip1.param->pet_soul_equipment_param.steps;
		m_exp = &tmp_equip1.param->pet_soul_equipment_param.exp;
		m_max_exp = &tmp_equip1.param->pet_soul_equipment_param.m_max_exp;
		attr_num = &tmp_equip1.param->pet_soul_equipment_param.attr_num;
		for (int attr_idx = 0; attr_idx < MAX_PET_SOUL_EQUIPMENT_ATTR_NUM && attr_idx < *attr_num; attr_idx++)
		{
			attr_list[attr_idx] = &(tmp_equip1.param->pet_soul_equipment_param.add_attr_list[attr_idx]);
		}
		color = item_base->GetColor();
	}

	const PetEquipSoulStepsCfg * steps_cfg = LOGIC_CONFIG->GetPetSoulEquipMentConfig()->GetPetSoulEquipStepsCfg(color, *steps + 1);
	if (steps_cfg == NULL)return;
	if (steps_cfg->min_level > *level)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_PET_SOUL_WEAPON_LEVEL_LIMIT_ERR);
		return;
	}

	std::vector<ItemExtern::ItemConsumeConfig> stuff_list;
	for (int i = 0; i < (int)steps_cfg->m_cost_vec.size(); ++i)
	{
		const ItemConfigData & curr = steps_cfg->m_cost_vec[i];

		ItemExtern::ItemConsumeConfig  stuff_con;
		stuff_con.item_id = curr.item_id;
		stuff_con.num = curr.num;
		stuff_con.buyable = false;
		stuff_list.push_back(stuff_con);
	}

	ItemExtern::ItemConsumeList consume_list;
	if ((int)stuff_list.size() > 0 && !ItemExtern::GetConsumeListByOrder(m_role_module_mgr->GetRole(), false, (int)stuff_list.size(), &stuff_list[0], &consume_list, false))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_ITEM_AMOUNT_NOT_ENOUGH);
		return;
	}

	if (consume_list.count > 0 && !m_role_module_mgr->GetKnapsack()->ConsumeItemByIndexList(consume_list.count, consume_list.column_list,
		consume_list.index_list, consume_list.num_list, __FUNCTION__))
	{
		return;
	}

	*steps += 1;

	//随机增加属性
	for (int attr_idx = 0; attr_idx < *attr_num && attr_idx < MAX_PET_SOUL_EQUIPMENT_ATTR_NUM; attr_idx++)
	{
		if (attr_list[attr_idx] != NULL)
		{
			const PetSoulEquipStepsAttrCfg * attr_cfg = LOGIC_CONFIG->GetPetSoulEquipMentConfig()->GetPetSoulEquipStepsAddValueCfg(color, attr_list[attr_idx]->attr_type);
			if (attr_cfg != NULL)
			{
				int rand_attr = 0;
				if ((attr_cfg->intensify_att_max - attr_cfg->intensify_att_min + 1) != 0)
				{
					rand_attr = rand() % (attr_cfg->intensify_att_max - attr_cfg->intensify_att_min + 1) + attr_cfg->intensify_att_min;
				}
				else
				{
					rand_attr = attr_cfg->intensify_att_min;
				}
				attr_list[attr_idx]->attr_value += rand_attr;
			}
		}
	}

	this->RecalePetSoul(0, &color, level, steps, m_exp, m_max_exp, attr_num, attr_list);//检查是否符合升级

	m_role_module_mgr->NoticeNum(noticenum::NT_PET_SOUL_EQUIP_UPSTEPS);

	if (WORKSHOP_EQUIP_POSI_EQUIP == is_put_on)
	{
		gamelog::g_log_pet.printf(LL_INFO, "%s user[%s,%d] item_id[%d] new_param[%s]", __FUNCTION__,
			m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetUid(), m_grid_list[index].item_id, m_grid_list[index].param->GetParamLog(I_TYPE_PET));
	}
	else
	{
		gamelog::g_log_pet.printf(LL_INFO, "%s user[%s,%d] item_id[%d] new_param[%s]", __FUNCTION__,
			m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetUid(), tmp_equip1.item_id, tmp_equip1.param->GetParamLog(I_TYPE_PET_SOUL_EQUIP));
	}
	if (is_put_on == WORKSHOP_EQUIP_POSI_EQUIP)
	{
		this->ReCalcAttr(index);
		this->SetItemDirty(index, true, Protocol::SCSendPetSingleInfo::PET_NOTIE_TYPE_PET_SOUL_DATA_CHANGE);
	}
	else
	{
		m_role_module_mgr->GetKnapsack()->SetItemDirty(ItemNamespace::ITEM_COLUMN_TYPE_PET_SOUL_EQIOP, index, true, Knapsack::ITEM_CHANGE_NOTICE_REASON_DATA_CHANGE);
	}
}

void Pet::RefinePetSoulEquip(int is_put_on, int index, int equipment_bag_index,int pet_soul_equipment_index)
{
	ItemGridData tmp_equip1, tmp_equip2; tmp_equip1.Reset(); tmp_equip2.Reset();
	short *sp_id = NULL;
	short *level = NULL;
	short *steps = NULL;
	int *refine = NULL;
	//属性词条
	ItemID item_1_id = 0;
	const PetSoulEquipment * pet_soul_base = NULL;
	if (WORKSHOP_EQUIP_POSI_EQUIP == is_put_on)
	{
		if (this->PetInvalid(index)) return;
		PetParam& pet_param = m_grid_list[index].param->pet_param;
		PetSoulEquip * soul_equip_ptr = pet_param.GetPetSoulEquipInPetParam(pet_soul_equipment_index);
		if (NULL == soul_equip_ptr)return;
		PetSoulEquip & soul_equip = *soul_equip_ptr;

		const ItemBase * item_base = ITEMPOOL->GetItem(soul_equip.item_id);
		if (item_base == NULL || item_base->GetItemType() != I_TYPE_PET_SOUL_EQUIP)return;
		pet_soul_base = (const PetSoulEquipment*)item_base;

		level = &soul_equip.m_level;
		steps = &soul_equip.m_gread;
		refine = &soul_equip.m_refine;
		sp_id = &soul_equip.sp_id;
		item_1_id = soul_equip.item_id;
	}
	else
	{
		if (!m_role_module_mgr->GetKnapsack()->GetItemGridData(ItemNamespace::ITEM_COLUMN_TYPE_PET_SOUL_EQIOP, index, &tmp_equip1)) return;
		const ItemBase * item_base = ITEMPOOL->GetItem(tmp_equip1.item_id);
		if (item_base == NULL || item_base->GetItemType() != I_TYPE_PET_SOUL_EQUIP)return;
		pet_soul_base = (const PetSoulEquipment*)item_base;
		level = &tmp_equip1.param->pet_soul_equipment_param.level;
		steps = &tmp_equip1.param->pet_soul_equipment_param.steps;
		refine = &tmp_equip1.param->pet_soul_equipment_param.refine;
		sp_id = &tmp_equip1.param->pet_soul_equipment_param.sp_id;
		item_1_id = tmp_equip1.item_id;
	}

	if (!m_role_module_mgr->GetKnapsack()->GetItemGridData(ItemNamespace::ITEM_COLUMN_TYPE_PET_SOUL_EQIOP, equipment_bag_index, &tmp_equip2)) return;
	const ItemBase * item_base_2 = ITEMPOOL->GetItem(tmp_equip2.item_id);
	if (item_base_2 == NULL || item_base_2->GetItemType() != I_TYPE_PET_SOUL_EQUIP)return;
	if (item_1_id != tmp_equip2.item_id)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_PET_SOUL_REFINE_ITEM_ERR);
		return;
	}

	const PetSoulEquipBaseCfg * soul_base_cfg = LOGIC_CONFIG->GetPetSoulEquipMentConfig()->GetPetSoulEquipBaseCfg(pet_soul_base->GetPetWeaponSeq());
	if (soul_base_cfg == NULL)return;
	const PetEquipSoulRefineCfg * refine_cfg = LOGIC_CONFIG->GetPetSoulEquipMentConfig()->GetPetSoulEquipRefineCfg(soul_base_cfg->refine_group, *refine + 1);
	if (refine_cfg == NULL)return;

	m_role_module_mgr->GetKnapsack()->ConsumeItemByIndex(ItemNamespace::ITEM_COLUMN_TYPE_PET_SOUL_EQIOP, equipment_bag_index, 1, __FUNCTION__);

	*refine += 1;

	*sp_id = refine_cfg->new_spid;
	m_role_module_mgr->NoticeNum(noticenum::NT_PET_SOUL_EQUIP_UPREFINE);
	if (WORKSHOP_EQUIP_POSI_EQUIP == is_put_on)
	{
		gamelog::g_log_pet.printf(LL_INFO, "%s user[%s,%d] item_id[%d] new_param[%s]", __FUNCTION__,
			m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetUid(), m_grid_list[index].item_id, m_grid_list[index].param->GetParamLog(I_TYPE_PET));
	}
	else
	{
		gamelog::g_log_pet.printf(LL_INFO, "%s user[%s,%d] item_id[%d] new_param[%s]", __FUNCTION__,
			m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetUid(), tmp_equip1.item_id, tmp_equip1.param->GetParamLog(I_TYPE_PET_SOUL_EQUIP));
	}
	if (is_put_on == WORKSHOP_EQUIP_POSI_EQUIP)
	{
		this->ReCalcAttr(index);
		this->SetItemDirty(index, true, Protocol::SCSendPetSingleInfo::PET_NOTIE_TYPE_PET_SOUL_DATA_CHANGE);
	}
	else
	{
		m_role_module_mgr->GetKnapsack()->SetItemDirty(ItemNamespace::ITEM_COLUMN_TYPE_PET_SOUL_EQIOP, index, true, Knapsack::ITEM_CHANGE_NOTICE_REASON_DATA_CHANGE);
	}
}

void Pet::LockPetSoulEquip(int is_put_on, int index, bool is_lock,int pet_soul_equipment_index)
{
	ItemGridData tmp_equip1; tmp_equip1.Reset();
	char *lock = NULL;

	if (WORKSHOP_EQUIP_POSI_EQUIP == is_put_on)
	{
		if (this->PetInvalid(index)) return;
		PetParam& pet_param = m_grid_list[index].param->pet_param;
		PetSoulEquip * soul_equip_ptr = pet_param.GetPetSoulEquipInPetParam(pet_soul_equipment_index);
		if (NULL == soul_equip_ptr)return;
		PetSoulEquip & soul_equip = *soul_equip_ptr;

		const ItemBase * item_base = ITEMPOOL->GetItem(soul_equip.item_id);
		if (item_base == NULL || item_base->GetItemType() != I_TYPE_PET_SOUL_EQUIP)return;
		lock = &soul_equip.is_lock;
	}
	else
	{
		if (!m_role_module_mgr->GetKnapsack()->GetItemGridData(ItemNamespace::ITEM_COLUMN_TYPE_PET_SOUL_EQIOP, index, &tmp_equip1)) return;
		const ItemBase * item_base = ITEMPOOL->GetItem(tmp_equip1.item_id);
		if (item_base == NULL || item_base->GetItemType() != I_TYPE_PET_SOUL_EQUIP)return;
		lock = &tmp_equip1.param->pet_soul_equipment_param.is_lock;
	}
	*lock = is_lock ? 1 : 0;

	if (is_put_on == WORKSHOP_EQUIP_POSI_EQUIP)
	{
		this->SetItemDirty(index, true, Protocol::SCSendPetSingleInfo::PET_NOTIE_TYPE_PET_SOUL_LOCK);
	}
	else
	{
		m_role_module_mgr->GetKnapsack()->SetItemDirty(ItemNamespace::ITEM_COLUMN_TYPE_PET_SOUL_EQIOP, index, true, Knapsack::ITEM_CHANGE_NOTICE_REASON_DATA_CHANGE);
	}
}

void Pet::GetSoulEffectList(const PetParam & pet_param, int max_other_sp_num, short * out_other_sp_num, int * out_other_sp_list) const
{
	if(NULL == out_other_sp_num || NULL == out_other_sp_list) return;

	std::set<PetSpPriority> sp_id_set;
	for (int pet_soul_equipment_index = 0; pet_soul_equipment_index < MAX_PET_SOUL_EQUIP_LIST; ++pet_soul_equipment_index)
	{
		const PetSoulEquip * soul_equip_ptr = pet_param.GetPetSoulEquipInPetParamConst(pet_soul_equipment_index);
		if (NULL == soul_equip_ptr)
		{
			continue;
		}

		const PetSoulEquip & soul_equip = *soul_equip_ptr;
		if (soul_equip.Invalid())
		{
			continue;
		}

		const ItemBase * soul_base = ITEMPOOL->GetItem(soul_equip.item_id);
		if (soul_base == NULL || soul_base->GetItemType() != I_TYPE_PET_SOUL_EQUIP)
		{
			continue;
		}

		const PetSoulEquipment * pet_soul = (const PetSoulEquipment *)soul_base;
		const PetSoulEquipBaseCfg * base_cfg = LOGIC_CONFIG->GetPetSoulEquipMentConfig()->GetPetSoulEquipBaseCfg(pet_soul->GetPetWeaponSeq());
		if (!base_cfg)
		{
			continue;
		}

		const PetSoulSpPriority * sp_priority = LOGIC_CONFIG->GetPetSoulEquipMentConfig()->GetPetSoulSpPriority(soul_equip.sp_id);
		if (!sp_priority)
		{
			continue;
		}

		if (base_cfg->pet_weapon_pet_id > 0)
		{
			int is_right = false;

			int pet_id_list[] = { pet_param.pet_id,pet_param.lian_yao_pet_id };
			for (int i = 0; i < ARRAY_LENGTH(pet_id_list); ++i)
			{
				int tmp_pet_id = pet_id_list[i];
				const PetCfg * pet_cfg = LOGIC_CONFIG->GetPetConfig()->GetPetCfg(tmp_pet_id);
				if (pet_cfg && pet_cfg->prototype_pet_id == base_cfg->pet_weapon_pet_id)
				{
					is_right = true;
					break;
				}
			}

			if (!is_right)
			{
				continue;
			}
		}

		PetSpPriority tmp;
		tmp.sp_id = soul_equip.sp_id;
		tmp.sp_type = sp_priority->sp_type;
		tmp.priority_num = sp_priority->priority;

		std::set<PetSpPriority>::iterator iter = sp_id_set.find(tmp);
		if (iter != sp_id_set.end())
		{
			if (iter->priority_num >= tmp.priority_num)
			{
				continue;
			}
		}

		sp_id_set.insert(tmp);
	}

	for (std::set<PetSpPriority>::iterator iter = sp_id_set.begin(); iter != sp_id_set.end(); ++iter)
	{
		if (*out_other_sp_num < max_other_sp_num)
		{
			out_other_sp_list[(*out_other_sp_num)++] = iter->sp_id;
		}
		else
		{
			gamelog::g_log_battle.printf(LL_INFO, "%s out_other_sp_num >= max_num[%d] ", __FUNCTION__, max_other_sp_num);
			return;
		}
	}
}

void Pet::CheckPetSoulCollection(bool need_sync)
{
	//宠物背包
	for (int index = 0; index < ItemNamespace::MAX_PET_GRID_NUM; index++)
	{
		if (!m_old_state[index] || m_grid_list[index].Invalid())
		{
			continue;
		}

		if (NULL == m_grid_list[index].param)
		{
			continue;
		}

		PetParam& pet_param = m_grid_list[index].param->pet_param;

		for (int pet_soul_equipment_index = 0; pet_soul_equipment_index < MAX_PET_SOUL_EQUIP_LIST; ++pet_soul_equipment_index)
		{
			const PetSoulEquip * soul_equip_ptr = pet_param.GetPetSoulEquipInPetParamConst(pet_soul_equipment_index);
			if (NULL == soul_equip_ptr) continue;
			const PetSoulEquip & soul_equip = *soul_equip_ptr;
			if (soul_equip.Invalid())
			{
				continue;
			}

			EventHandler::Instance().OnGetPetSoul(m_role_module_mgr->GetRole(),
				soul_equip.item_id, need_sync);
		}
	}


	//宠物仓库
	for (int index = 0; index < ItemNamespace::MAX_PET_STORAGE_GRID_NUM; index++)
	{
		if (!m_storage_old_state[index] || m_storage_grid_list[index].Invalid())
		{
			continue;
		}

		if (NULL == m_storage_grid_list[index].param)
		{
			continue;
		}
		PetParam& pet_param = m_storage_grid_list[index].param->pet_param;

		for (int pet_soul_equipment_index = 0; pet_soul_equipment_index < MAX_PET_SOUL_EQUIP_LIST; ++pet_soul_equipment_index)
		{
			const PetSoulEquip * soul_equip_ptr = pet_param.GetPetSoulEquipInPetParamConst(pet_soul_equipment_index);
			if (NULL == soul_equip_ptr) continue;
			const PetSoulEquip & soul_equip = *soul_equip_ptr;
			if (soul_equip.Invalid())
			{
				continue;
			}

			EventHandler::Instance().OnGetPetSoul(m_role_module_mgr->GetRole(),
				soul_equip.item_id, need_sync);
		}
	}
}

bool Pet::GetPetGodPrintList(int pet_index, GodPrintListPro * list)
{
	if (pet_index < 0 || pet_index >= ItemNamespace::MAX_PET_GRID_NUM)return false;
	if (m_grid_list[pet_index].Invalid())return false;
	if (NULL == list)return false;
	PetParam &pet_param = m_grid_list[pet_index].param->pet_param;
	
	for (int god_pos = GOD_PRINT_INDEX_TYPE_0; god_pos < GOD_PRINT_INDEX_TYPE_MAX; god_pos++)
	{
		int bag_idx = pet_param.god_print_list[god_pos];
		if (bag_idx - 1 < 0 || bag_idx >= NEW_MAX_PET_GOD_PRONT_BAG_NUM)continue;
		if (NULL == m_new_god_print_bag[bag_idx - 1])continue;
		if (m_new_god_print_bag[bag_idx - 1]->Invalid())continue;
		GodPrintItem &god_print_item = *m_new_god_print_bag[bag_idx - 1];
		list->god_print_list[god_pos].has_random_net_value = 1;
		list->god_print_list[god_pos].god_print_data = god_print_item;
	}
	return true;
}

void Pet::ExchangeGodPrint(int exchange_idx_1, int exchange_idx_2)
{
	if (!FunOpen::IsFunOpen(m_role_module_mgr->GetRole(), FUN_TYPE_NETHER_GOD_PRINT_SYSTEM))return;

	{
		const PetGodPrintEquipMentOtherCfg & other_cfg = LOGIC_CONFIG->GetPetGodPrintEquipMentConfig()->GetOtherCfg();
		if (!m_role_module_mgr->GetKnapsack()->CheckForPut(other_cfg.exchange_get_item_id, 1))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_ITEM_COLUMN_NO_SPACE);
			return;
		}

		if (exchange_idx_1 - 1 < 0 || exchange_idx_1 - 1 >= NEW_MAX_PET_GOD_PRONT_BAG_NUM)return;
		if (NULL == m_new_god_print_bag[exchange_idx_1 - 1])return;
		if (m_new_god_print_bag[exchange_idx_1 - 1]->Invalid())return;
		GodPrintItem &item_param = *m_new_god_print_bag[exchange_idx_1 - 1];
		if (item_param.m_wear_pet_idx >= 0)
		{
			m_role_module_mgr->NoticeNum(errornum::EN_PET_WEAR_GOD_PRINT);
			return;
		}
		if (item_param.lock == 1)
		{
			m_role_module_mgr->NoticeNum(errornum::EN_PET_GOD_PRINT_HAS_LOCK);
			return;
		}

		if (exchange_idx_2 - 1 < 0 || exchange_idx_2 - 1 >= NEW_MAX_PET_GOD_PRONT_BAG_NUM)return;
		if (NULL == m_new_god_print_bag[exchange_idx_2 - 1])return;
		if (m_new_god_print_bag[exchange_idx_2 - 1]->Invalid())return;
		GodPrintItem &item_param2 = *m_new_god_print_bag[exchange_idx_2 - 1];
		if (item_param2.m_wear_pet_idx >= 0)
		{
			m_role_module_mgr->NoticeNum(errornum::EN_PET_WEAR_GOD_PRINT);
			return;
		}
		if (item_param2.lock == 1)
		{
			m_role_module_mgr->NoticeNum(errornum::EN_PET_GOD_PRINT_HAS_LOCK);
			return;
		}

		if (item_param.color < ItemBase::I_COLOR_ORANGE || item_param2.color < ItemBase::I_COLOR_ORANGE)return;
		int new_item_id = 0;
		if (item_param.color == ItemBase::I_COLOR_ORANGE && item_param2.color == ItemBase::I_COLOR_ORANGE)
		{
			new_item_id = other_cfg.exchange_get_item_id;
		}
		else if (item_param.color == ItemBase::I_COLOR_RED && item_param2.color == ItemBase::I_COLOR_RED)
		{
			new_item_id = other_cfg.exchange_get_item_id_1;
		}
		else
		{
			return;
		}

		item_param.Reset();
		item_param2.Reset();
		this->SetGodPrintDirtyMark(exchange_idx_1 - 1);
		this->ClearGodPrintFromDiscomplse(exchange_idx_1);

		this->SetGodPrintDirtyMark(exchange_idx_2 - 1);
		this->ClearGodPrintFromDiscomplse(exchange_idx_2);

		
		ItemDataWrapper reward_item;
		reward_item.item_id = new_item_id;
		reward_item.num = 1;
		reward_item.is_bind = false;
		m_role_module_mgr->GetKnapsack()->Put(reward_item, PUT_REASON_GOD_PRINT_EXCHANGE_2);
	}
}

const GodPrintItem* Pet::AddPetGodPrintItem(ItemID item_id, int seq, int quality, int suit_id, long long union_item_id, int put_reason, int *out_idx)
{
	int bag_idx = this->GetEmptyBag();
	if (bag_idx < 0 || bag_idx >= NEW_MAX_PET_GOD_PRONT_BAG_NUM)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_GOD_PRINT_COLUMN_NO_SPACE);
		return NULL;
	}

	const ItemBase * item_base = ITEMPOOL->GetItem(item_id);
	if (item_base == NULL)return NULL;
	
	if (item_base->GetItemType() != I_TYPE_GOD_PRINT)return NULL;

	GodPrintItem data;
	{
		data.Reset();
		data.item_id = item_id;
		data.level = 1;
		data.color = (char)quality;
		data.m_posi = (short)seq - 1;
		if(union_item_id > 0)data.union_item_id = union_item_id;
		else UniqueIDGenerator::Instance()->GenUniqueID(&data.union_item_id);
		
		const PetGodPrintIntensifyInfoCfg *  exp_cfg = LOGIC_CONFIG->GetPetGodPrintEquipMentConfig()->GetIntensifyInfoCfg(quality, data.level);
		if (exp_cfg != NULL)
		{
			data.m_max_exp = exp_cfg->exp;
		}

		const PetGodPrintEquipMentBaseQualityCfg *  level_cfg = LOGIC_CONFIG->GetPetGodPrintEquipMentConfig()->GetBaseCfg(seq, (int)data.color);
		if (level_cfg != NULL)
		{
			// 随机主属性
			const PetGodPrintMainAttrBaseCfg * main_cfg = LOGIC_CONFIG->GetPetGodPrintEquipMentConfig()->RandMainBaseAttrCfg(level_cfg->main_attribute_group);
			if (main_cfg != NULL)
			{
				if ((main_cfg->att_max - main_cfg->att_min + 1) > 0)
				{
					data.main_attr.attr_value = (rand() % (main_cfg->att_max - main_cfg->att_min + 1)) + main_cfg->att_min;
				}
				else
				{
					data.main_attr.attr_value = main_cfg->att_min;
				}
				data.main_attr.attr_type = main_cfg->att_type;
			}

			// 随机副属性

			const PetGodPrintEquipMentBaseAttrNumCfg * deputy_num_cfg = LOGIC_CONFIG->GetPetGodPrintEquipMentConfig()->RandDeputyAttrNum(level_cfg->deputy_attribute_num);
			if (deputy_num_cfg != NULL)
			{
				if (deputy_num_cfg->num > 0)
				{
					for (int attr_idx = 0; attr_idx < MAX_PET_GOD_PRONT_ATTR_NUM &&
						attr_idx < deputy_num_cfg->num; attr_idx++)
					{
						int attr_type[MAX_PET_GOD_PRONT_ATTR_NUM];
						memset(attr_type, -1, sizeof(attr_type));

						for (int i = 0; i < MAX_PET_GOD_PRONT_ATTR_NUM && i < data.attr_num; i++)
						{
							attr_type[i] = data.add_attr_list[i].attr_type;
						}
						const PetGodPrintdeputyAttrInfoCfg * attr_info = LOGIC_CONFIG->GetPetGodPrintEquipMentConfig()->RandDeputyAttrBaseInfo(level_cfg->deputy_attribute_att, attr_type);
						if (attr_info != NULL)
						{
							if ((attr_info->att_max - attr_info->att_min + 1) > 0)
							{
								data.add_attr_list[data.attr_num].attr_type = attr_info->att_type;
								data.add_attr_list[data.attr_num++].attr_value = (rand() % (attr_info->att_max - attr_info->att_min + 1)) + attr_info->att_min;
							}
							else
							{
								data.add_attr_list[data.attr_num].attr_type = attr_info->att_type;
								data.add_attr_list[data.attr_num++].attr_value = attr_info->att_min;
							}
						}
					}
				}
			}
			data.suit_id = suit_id;
		}
	}
	if (NULL == m_new_god_print_bag[bag_idx])
	{
		m_new_god_print_bag[bag_idx] = new GodPrintItem();
	}
	*m_new_god_print_bag[bag_idx] = data;
	if (out_idx != NULL)*out_idx = bag_idx;
	gamelog::g_log_pet.printf(LL_INFO, "%s user[%d,%s] item[%d] [%s]", __FUNCTION__, m_role_module_mgr->GetUid(), m_role_module_mgr->GetRole()->GetName(),
		data.item_id, data.GetParamLog());
	this->SetGodPrintDirtyMark(bag_idx, put_reason);

	ROLE_LOG_QUICK6(LOG_TYPE_PET_GOD_PRINT, m_role_module_mgr->GetRole(), item_id, 1, __FUNCTION__, data.GetParamLog());

	return m_new_god_print_bag[bag_idx];
}

const GodPrintItem * Pet::AddPetGodPrintByUse(const ItemGridData * item_data, int *out_idx)
{
	int bag_idx = this->GetEmptyBag();
	if (bag_idx < 0 || bag_idx >= NEW_MAX_PET_GOD_PRONT_BAG_NUM)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_GOD_PRINT_COLUMN_NO_SPACE);
		return NULL;
	}
	if (item_data == NULL)return NULL;
	if (NULL == m_new_god_print_bag[bag_idx])
	{
		m_new_god_print_bag[bag_idx] = new GodPrintItem();
	}
	*m_new_god_print_bag[bag_idx] = item_data->param->god_print_param.god_print_data;
	if (out_idx != NULL)*out_idx = bag_idx;
	this->SetGodPrintDirtyMark(bag_idx);
	gamelog::g_log_pet.printf(LL_INFO, "%s user[%d,%s] item[%d] [%s]", __FUNCTION__, m_role_module_mgr->GetUid(), m_role_module_mgr->GetRole()->GetName(),
		m_new_god_print_bag[bag_idx]->item_id, m_new_god_print_bag[bag_idx]->GetParamLog());

	ROLE_LOG_QUICK6(LOG_TYPE_PET_GOD_PRINT, m_role_module_mgr->GetRole(), item_data->item_id, 1, __FUNCTION__, m_new_god_print_bag[bag_idx]->GetParamLog());
	return m_new_god_print_bag[bag_idx];
}

const GodPrintItem * Pet::AddPetGodPrintByUse(const ItemDataWrapper * item_data, int * out_idx)
{
	int bag_idx = this->GetEmptyBag();
	if (bag_idx < 0 || bag_idx >= NEW_MAX_PET_GOD_PRONT_BAG_NUM)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_GOD_PRINT_COLUMN_NO_SPACE);
		return NULL;
	}
	if (item_data == NULL)return NULL;
	if (NULL == m_new_god_print_bag[bag_idx])
	{
		m_new_god_print_bag[bag_idx] = new GodPrintItem();
	}
	NetValueItemParam * param = (NetValueItemParam *)item_data->param_data;
	*m_new_god_print_bag[bag_idx] = param->god_print_param.god_print_data;
	if (out_idx != NULL)*out_idx = bag_idx;
	this->SetGodPrintDirtyMark(bag_idx);
	gamelog::g_log_pet.printf(LL_INFO, "%s user[%d,%s] item[%d] [%s]", __FUNCTION__, m_role_module_mgr->GetUid(), m_role_module_mgr->GetRole()->GetName(),
		m_new_god_print_bag[bag_idx]->item_id, m_new_god_print_bag[bag_idx]->GetParamLog());
	return m_new_god_print_bag[bag_idx];
}

void Pet::GetGodPrintEffectList(const PetParam & pet_param, int max_other_sp_num, short * out_other_sp_num, int * out_other_sp_list) const
{
	if (NULL == out_other_sp_num || NULL == out_other_sp_list) return;

	int suit_num = 0;
	SuitEffectReCalc equip_suit_list[GOD_PRINT_INDEX_TYPE_MAX];

	for (int god_print_idx = 0; god_print_idx < GOD_PRINT_INDEX_TYPE_MAX; god_print_idx++)
	{
		int bag_idx = pet_param.god_print_list[god_print_idx];
		if (bag_idx > 0 && bag_idx < NEW_MAX_PET_GOD_PRONT_BAG_NUM)
		{
			int bag_index = bag_idx - 1;
			if (NULL == m_new_god_print_bag[bag_index])continue;
			if (m_new_god_print_bag[bag_index]->Invalid())continue;
			GodPrintItem & node = *m_new_god_print_bag[bag_index];
			//有神印
			if (node.suit_id > 0 && suit_num < GOD_PRINT_INDEX_TYPE_MAX)
			{
				bool has_suit_add = false;
				for (int suit_idx = 0; suit_idx < suit_num && suit_idx < MAX_SUIT_TYPE; suit_idx++)//先检查是否有同类组合，件数堆叠
				{
					SuitEffectReCalc &node_cale = equip_suit_list[suit_idx];
					if (node_cale.sp_id == node.suit_id)
					{
						//检查多件套中最高品质
						bool has_insert = false;
						for (int suit_num_begin = 0; suit_num_begin < node_cale.num; suit_num_begin++)
						{
							if ((int)node_cale.color_top.size() <= suit_num_begin)continue;
							if (node_cale.color_top[suit_num_begin] < (int)node.color)
							{
								std::vector<int>::iterator it = node_cale.color_top.begin();
								node_cale.color_top.insert(it + suit_num_begin, (int)node.color);
								has_insert = true;
								break;
							}
						}
						if (!has_insert)
						{
							node_cale.color_top.push_back((int)node.color);
						}

						node_cale.num += 1;
						has_suit_add = true;
						break;
					}
				}
				if (!has_suit_add)
				{
					equip_suit_list[suit_num].sp_id = node.suit_id;
					equip_suit_list[suit_num].color_top.push_back((int)node.color);
					equip_suit_list[suit_num++].num = 1;
				}
			}
		}
	}

	if (suit_num > 0)
	{
		for (int i = 0; i < suit_num && i < GOD_PRINT_INDEX_TYPE_MAX; i++)
		{
			SuitEffectReCalc & suit_node = equip_suit_list[i];
			{
				for (int suit_num_idx = 0; suit_num_idx < GOD_PRINT_INDEX_TYPE_MAX && suit_num_idx < suit_node.num && suit_num_idx < (int)suit_node.color_top.size(); suit_num_idx++)
				{
					const PetGodPrintEquipMentSuitCfg * suit_list_cfg = LOGIC_CONFIG->GetPetGodPrintEquipMentConfig()->GetSuitListCfg(suit_node.sp_id, suit_node.color_top[suit_num_idx], suit_num_idx);
					if (NULL != suit_list_cfg)
					{
						switch (suit_list_cfg->effect_type)
						{
						case PetGodPrintEquipMentSuitCfg::EFFECT_TYPE_SP_ID:
							{
								if (*out_other_sp_num < max_other_sp_num)
								{
									out_other_sp_list[(*out_other_sp_num)++] = suit_list_cfg->param2;
								}
								else
								{
									gamelog::g_log_battle.printf(LL_INFO, "%s out_other_sp_num >= max_num[%d] ", __FUNCTION__, max_other_sp_num);
									return;
								}
							}
							break;
						}
					}
				}
			}
		}
	}
}

int Pet::GetGodPrintEffectScore(const PetParam & pet_param) const
{
	int god_print_effect_capability = 0;
	int suit_num = 0;
	SuitEffectReCalc equip_suit_list[GOD_PRINT_INDEX_TYPE_MAX];

	for (int god_print_idx = 0; god_print_idx < GOD_PRINT_INDEX_TYPE_MAX; god_print_idx++)
	{
		int bag_idx = pet_param.god_print_list[god_print_idx];
		if (bag_idx > 0 && bag_idx <= NEW_MAX_PET_GOD_PRONT_BAG_NUM)
		{
			if (NULL == m_new_god_print_bag[bag_idx - 1])continue;
			if (m_new_god_print_bag[bag_idx - 1]->Invalid())continue;
			GodPrintItem & node = *m_new_god_print_bag[bag_idx - 1];
			//有神印
			if (node.suit_id > 0 && suit_num < GOD_PRINT_INDEX_TYPE_MAX)
			{
				bool has_suit_add = false;
				for (int suit_idx = 0; suit_idx < suit_num && suit_idx < MAX_SUIT_TYPE; suit_idx++)//先检查是否有同类组合，件数堆叠
				{
					SuitEffectReCalc &node_cale = equip_suit_list[suit_idx];
					if (node_cale.sp_id == node.suit_id)
					{
						//检查多件套中最高品质
						bool has_insert = false;
						for (int suit_num_begin = 0; suit_num_begin < node_cale.num; suit_num_begin++)
						{
							if ((int)node_cale.color_top.size() <= suit_num_begin)continue;
							if (node_cale.color_top[suit_num_begin] < (int)node.color)
							{
								std::vector<int>::iterator it = node_cale.color_top.begin();
								node_cale.color_top.insert(it + suit_num_begin, (int)node.color);
								has_insert = true;
								break;
							}
						}
						if (!has_insert)
						{
							node_cale.color_top.push_back((int)node.color);
						}

						node_cale.num += 1;
						has_suit_add = true;
						break;
					}
				}
				if (!has_suit_add)
				{
					equip_suit_list[suit_num].sp_id = node.suit_id;
					equip_suit_list[suit_num].color_top.push_back((int)node.color);
					equip_suit_list[suit_num++].num = 1;
				}
			}
		}
	}

	if (suit_num > 0)
	{
		for (int i = 0; i < suit_num && i < GOD_PRINT_INDEX_TYPE_MAX; i++)
		{
			SuitEffectReCalc & suit_node = equip_suit_list[i];
			{
				for (int suit_num_idx = 0; suit_num_idx < GOD_PRINT_INDEX_TYPE_MAX && suit_num_idx < suit_node.num && suit_num_idx < (int)suit_node.color_top.size(); suit_num_idx++)
				{
					const PetGodPrintEquipMentSuitCfg * suit_list_cfg = LOGIC_CONFIG->GetPetGodPrintEquipMentConfig()->GetSuitListCfg(suit_node.sp_id, suit_node.color_top[suit_num_idx], suit_num_idx);
					if (NULL != suit_list_cfg)
					{
						god_print_effect_capability += suit_list_cfg->score;
					}
				}
			}
		}
	}
	return god_print_effect_capability;
}

bool Pet::GetPetGodPrintData(int bag_idx, GodPrintItem * item_data)
{
	if (bag_idx < 0 || bag_idx >= NEW_MAX_PET_GOD_PRONT_BAG_NUM)return false;
	if (NULL == m_new_god_print_bag[bag_idx])return false;
	if (m_new_god_print_bag[bag_idx]->Invalid())return false;
	if (item_data == NULL)return false;
	*item_data = *m_new_god_print_bag[bag_idx];
	return true;
}

void Pet::PutOnPetGodPrintEquip(int pet_index, int index, bool is_from_client)
{
	if (!FunOpen::IsFunOpen(m_role_module_mgr->GetRole(), FUN_TYPE_NETHER_GOD_PRINT_SYSTEM))return;
	//int out_index = -1;
	//if (!this->IsWaitForFightPet(pet_index, &out_index))return;

	if (index - 1 < 0 || index - 1 >= NEW_MAX_PET_GOD_PRONT_BAG_NUM)return;
	if (pet_index < 0 || pet_index >= ItemNamespace::MAX_PET_GRID_NUM)return;
	if (m_grid_list[pet_index].Invalid())return;
	PetParam &pet_param = m_grid_list[pet_index].param->pet_param;
	if (NULL == m_new_god_print_bag[index - 1])return;
	if (m_new_god_print_bag[index - 1]->Invalid())return;
	GodPrintItem &god_print_item = *m_new_god_print_bag[index - 1];
	if (god_print_item.m_posi < 0 || god_print_item.m_posi >= GOD_PRINT_INDEX_TYPE_MAX)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	if (pet_param.god_print_list[god_print_item.m_posi] > 0)
	{
		if (!this->PutOffPetGodPrintEquip(pet_index, god_print_item.m_posi + 1))
		{
			return;
		}
	}

	if (god_print_item.m_wear_pet_idx >= 0)
	{
		if (!this->PutOffPetGodPrintEquip(god_print_item.m_wear_pet_idx, god_print_item.m_posi + 1))
		{
			return;
		}
	}

	if (god_print_item.level <= 0)return;

	const ItemBase * i_b = ITEMPOOL->GetItem(god_print_item.item_id);
	if (i_b == NULL)return;
	
	pet_param.god_print_list[god_print_item.m_posi] = index;
	god_print_item.m_wear_pet_idx = pet_index;
	if(is_from_client)this->PutOffPetGodPrintScheme(pet_index);
	this->ReCalcAttr(pet_index);
	this->SetGodPrintDirtyMark(index - 1);
	this->SetItemDirty(pet_index, true, Protocol::SCSendPetSingleInfo::PET_NOTIE_TYPE_PET_GOD_PRINT);
	if (this->IsInPlatfformBattle(m_grid_list[pet_index].item_unique_id))
	{
		this->SyncPetDataPlatformBattle(pet_index);
	}
}

bool Pet::PutOffPetGodPrintEquip(int pet_index, int posi_index, bool is_from_client)
{
	if (!FunOpen::IsFunOpen(m_role_module_mgr->GetRole(), FUN_TYPE_NETHER_GOD_PRINT_SYSTEM))return false;
	if (posi_index - 1 < 0 || posi_index - 1 >= GOD_PRINT_INDEX_TYPE_MAX)return false;
	if (pet_index < 0 || pet_index >= ItemNamespace::MAX_PET_GRID_NUM)return false;
	if (this->PetInvalid(pet_index))return false;
	PetParam &pet_param = m_grid_list[pet_index].param->pet_param;
	if (pet_param.god_print_list[posi_index - 1] == 0)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_PET_NO_WEAR_GOD_PRINT);
		return false;
	}
	
	int god_print_idx = pet_param.god_print_list[posi_index - 1] - 1;
	if (god_print_idx >= 0 && god_print_idx < NEW_MAX_PET_GOD_PRONT_BAG_NUM)
	{
		if (NULL == m_new_god_print_bag[god_print_idx])return false;
		if (m_new_god_print_bag[god_print_idx]->Invalid())return false;
		m_new_god_print_bag[god_print_idx]->m_wear_pet_idx = -1;
	}
	if (is_from_client)this->PutOffPetGodPrintScheme(pet_index);
	pet_param.god_print_list[posi_index - 1] = 0;
	this->SetGodPrintDirtyMark(god_print_idx);
	this->ReCalcAttr(pet_index);
	this->SetItemDirty(pet_index, true, Protocol::SCSendPetSingleInfo::PET_NOTIE_TYPE_PET_GOD_PRINT);
	if (this->IsInPlatfformBattle(m_grid_list[pet_index].item_unique_id))
	{
		this->SyncPetDataPlatformBattle(pet_index);
	}
	return true;
}

bool Pet::PutOffAllPetGodPrintEquip(int pet_index,bool is_notice)
{
	if (this->PetInvalid(pet_index))return false;
	PetParam &pet_param = m_grid_list[pet_index].param->pet_param;
	
	for (int i = 0; i < GOD_PRINT_INDEX_TYPE_MAX; i++)
	{
		if (pet_param.god_print_list[i] == 0)continue;
		if (!this->PutOffPetGodPrintEquip(pet_index, i + 1, is_notice))
		{
			return false;
		}
	}
	return true;
}

bool Pet::LockPetGodPrintEquip(int bag_id, bool is_lock)
{
	if (!FunOpen::IsFunOpen(m_role_module_mgr->GetRole(), FUN_TYPE_NETHER_GOD_PRINT_SYSTEM))return false;
	if (bag_id - 1 < 0 || bag_id - 1 >= NEW_MAX_PET_GOD_PRONT_BAG_NUM)return false;
	if (NULL == m_new_god_print_bag[bag_id - 1])return false;
	if (m_new_god_print_bag[bag_id - 1]->Invalid())return false;
	GodPrintItem &node_param = *m_new_god_print_bag[bag_id - 1];
	if (node_param.Invalid())
	{
		m_role_module_mgr->NoticeNum(errornum::EN_ITEM_IS_INVALID);
		return false;
	}
	node_param.lock = is_lock ? 1 : 0;
	this->SetGodPrintDirtyMark(bag_id - 1);
	return false;
}

void Pet::WearShcheme(int pet_index, int scheme_id)
{
	if (scheme_id < 0 || scheme_id >= MAX_PET_GOD_PRONT_SCHEME_NUM)return;
	if (pet_index < 0 || pet_index >= ItemNamespace::MAX_PET_GRID_NUM)return;
	this->PutOffPetGodPrintScheme(pet_index);
	GodPrintWearScheme &scheme_info = m_god_print_param.wear_scheme_list[scheme_id];
	for (int i = 0; i < MAX_PET_GOD_PRONT_ONE_SCHEME_NUM; i++)
	{
		if (scheme_info.bag_idx[i] > 0)
		{
			this->PutOnPetGodPrintEquip(pet_index, scheme_info.bag_idx[i], false);
		}
		else
		{
			if (this->PetInvalid(pet_index)) continue;

			PetParam &pet_param = m_grid_list[pet_index].param->pet_param;
			if (pet_param.god_print_list[i] != 0)
			{
				this->PutOffPetGodPrintEquip(pet_index, i + 1, false);
			}
		}
	}
	scheme_info.use_pet_idx = pet_index;
	this->SendSingleGodPrintScheme(scheme_id);
}

void Pet::PutOffPetGodPrintScheme(int pet_idx)
{
	for (int i = 0; i < MAX_PET_GOD_PRONT_SCHEME_NUM; i++)
	{
		if (pet_idx < 0)continue;
		if (m_god_print_param.wear_scheme_list[i].use_pet_idx == pet_idx)
		{
			m_god_print_param.wear_scheme_list[i].use_pet_idx = -1;
			this->SendSingleGodPrintScheme(i);
		}
	}
	return;
}

void Pet::UpLevelPetGodPrintEquip(int posi_index, int add_exp)
{
	if (!FunOpen::IsFunOpen(m_role_module_mgr->GetRole(), FUN_TYPE_NETHER_GOD_PRINT_SYSTEM))return;
	if (posi_index - 1 < 0 || posi_index - 1 >= NEW_MAX_PET_GOD_PRONT_BAG_NUM)return;
	if (add_exp > m_god_print_param.exp)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_GOD_PRINT_EXP_NOT_ENOUGH);
		return;
	}
	if (NULL == m_new_god_print_bag[posi_index - 1])return;
	if (m_new_god_print_bag[posi_index - 1]->Invalid())return;
	GodPrintItem &node_param = *m_new_god_print_bag[posi_index - 1];
	if (node_param.Invalid())
	{
		m_role_module_mgr->NoticeNum(errornum::EN_ITEM_IS_INVALID);
		return;
	}

	const PetGodPrintEquipMentOtherCfg & other_cfg = LOGIC_CONFIG->GetPetGodPrintEquipMentConfig()->GetOtherCfg();

	bool need_recale = false;
	
	for (int round_num = 0; round_num < 30 && add_exp + node_param.exp >= node_param.m_max_exp; round_num++)
	{
		if (add_exp + node_param.exp >= node_param.m_max_exp)
		{
			int real_cost_exp = node_param.m_max_exp - node_param.exp;
			int need_coin = real_cost_exp * other_cfg.need_coin_100_exp / 100;
			add_exp -= real_cost_exp;
			if (need_coin > 0)
			{
				if (!m_role_module_mgr->GetMoney()->CoinBindMoreThan(need_coin))
				{
					m_role_module_mgr->NoticeNum(errornum::EN_COIN_NOT_ENOUGH);
					add_exp = 0;//不足铜币了，剩下的就直接跳过返回了
					break;
				}
				m_role_module_mgr->GetMoney()->UseCoinBind(need_coin, __FUNCTION__);
			}

			m_god_print_param.exp -= real_cost_exp;
			GodPrintItem old_god_print;
			old_god_print = node_param;
			node_param.exp += real_cost_exp;

			//检查下一级
			const PetGodPrintIntensifyInfoCfg *  next_level_cfg = LOGIC_CONFIG->GetPetGodPrintEquipMentConfig()->GetIntensifyInfoCfg((int)node_param.color, node_param.level + 1);
			if (next_level_cfg != NULL)
			{
				node_param.exp -= node_param.m_max_exp;
				node_param.m_max_exp = next_level_cfg->exp;
				node_param.level++;
				need_recale = true;
				//主属性随机
				if (next_level_cfg->main_attribute_up)
				{
					const PetGodPrintIntensifyDetailCfg * main_attr_cfg = LOGIC_CONFIG->GetPetGodPrintEquipMentConfig()->GetIntensifyCfg(PetGodPrintIntensifyDetailCfg::PET_GOD_PRINT_TYPE_MAIN, node_param.color, node_param.main_attr.attr_type);
					if (main_attr_cfg != NULL)
					{
						if ((main_attr_cfg->intensify_att_max - main_attr_cfg->intensify_att_min + 1) != 0)
						{
							int rand_value = (rand() % (main_attr_cfg->intensify_att_max - main_attr_cfg->intensify_att_min + 1)) + main_attr_cfg->intensify_att_min;
							node_param.main_attr.attr_value += rand_value;
						}
						else
						{
							node_param.main_attr.attr_value += main_attr_cfg->intensify_att_min;
						}
					}
				}
				//副属性随机
				if (next_level_cfg->deputy_attribute_up)
				{
					if (node_param.attr_num < MAX_PET_GOD_PRONT_ATTR_NUM)//不满4条则增加条数，满则增强词条
					{
						const PetGodPrintEquipMentBaseQualityCfg * base_cfg = LOGIC_CONFIG->GetPetGodPrintEquipMentConfig()->GetBaseCfg((int)node_param.m_posi + 1, (int)node_param.color);
						if (base_cfg != NULL)
						{
							int attr_type[MAX_PET_GOD_PRONT_ATTR_NUM];
							memset(attr_type, -1, sizeof(attr_type));

							for (int i = 0; i < MAX_PET_GOD_PRONT_ATTR_NUM && i < node_param.attr_num; i++)
							{
								attr_type[i] = node_param.add_attr_list[i].attr_type;
							}

							const PetGodPrintdeputyAttrInfoCfg * deputy_cfg = LOGIC_CONFIG->GetPetGodPrintEquipMentConfig()->RandDeputyAttrBaseInfo(base_cfg->deputy_attribute_att, attr_type);
							if (deputy_cfg != NULL)
							{
								node_param.add_attr_list[node_param.attr_num].attr_type = deputy_cfg->att_type;
								if ((deputy_cfg->att_max - deputy_cfg->att_min + 1) != 0)
								{
									int rand_value = (rand() % (deputy_cfg->att_max - deputy_cfg->att_min + 1)) + deputy_cfg->att_min;
									node_param.add_attr_list[node_param.attr_num].attr_value += rand_value;
								}
								else
								{
									node_param.add_attr_list[node_param.attr_num].attr_value += deputy_cfg->att_min;
								}
								node_param.attr_num++;
							}
						}
					}
					else
					{
						int attr_up_idx = 0;
						if (node_param.attr_num > 0)
						{
							attr_up_idx = rand() % node_param.attr_num;
						}
						if (attr_up_idx < 0 || attr_up_idx >= MAX_PET_GOD_PRONT_ATTR_NUM)
						{
							continue;
						}
						const PetGodPrintIntensifyDetailCfg * deputy_attr_cfg = LOGIC_CONFIG->GetPetGodPrintEquipMentConfig()->GetIntensifyCfg(PetGodPrintIntensifyDetailCfg::PET_GOD_PRINT_TYPE_DEPUTY,
							node_param.color, node_param.add_attr_list[attr_up_idx].attr_type);
						if (deputy_attr_cfg != NULL)
						{
							if ((deputy_attr_cfg->intensify_att_max - deputy_attr_cfg->intensify_att_min + 1) != 0)
							{
								int rand_value = (rand() % (deputy_attr_cfg->intensify_att_max - deputy_attr_cfg->intensify_att_min + 1)) + deputy_attr_cfg->intensify_att_min;
								node_param.add_attr_list[attr_up_idx].attr_value += rand_value;
							}
							else
							{
								node_param.add_attr_list[attr_up_idx].attr_value += deputy_attr_cfg->intensify_att_min;
							}
						}
					}
				}
			}
			else
			{
				add_exp = 0;
			}
			NetValueItemParam old_net_param1;
			old_net_param1.god_print_param.has_random_net_value = 1;
			old_net_param1.god_print_param.god_print_data = old_god_print;
			NetValueItemParam net_param1;
			net_param1.god_print_param.has_random_net_value = 1;
			net_param1.god_print_param.god_print_data = node_param;
			gamelog::g_log_pet.printf(LL_INFO, "PetGodPrint UPLEVEL user[%d,%s] old_god_print[%s]  new_god_print[%s]", m_role_module_mgr->GetUid(), m_role_module_mgr->GetRole()->GetName(),
				old_net_param1.GetParamLog(I_TYPE_GOD_PRINT), net_param1.GetParamLog(I_TYPE_GOD_PRINT));
		}
	}
	
	if (add_exp > 0)
	{
		if (add_exp + node_param.exp > node_param.m_max_exp)
		{
			int real_add_exp = node_param.m_max_exp - node_param.exp;
			if (real_add_exp > 0)
			{
				int need_coin = real_add_exp * other_cfg.need_coin_100_exp / 100;
				if (m_role_module_mgr->GetMoney()->CoinBindMoreThan(need_coin))
				{
					m_role_module_mgr->GetMoney()->UseCoinBind(need_coin, __FUNCTION__);
					node_param.exp += real_add_exp;
					m_god_print_param.exp -= real_add_exp;
				}
			}
		}
		else
		{
			int need_coin = add_exp * other_cfg.need_coin_100_exp / 100;
			if (need_coin > 0)
			{
				if (m_role_module_mgr->GetMoney()->CoinBindMoreThan(need_coin))
				{
					m_role_module_mgr->GetMoney()->UseCoinBind(need_coin, __FUNCTION__);
				}
			}
			m_god_print_param.exp -= add_exp;
			node_param.exp += add_exp;
		}
	}

	//寻找这件神印是否有宠物穿戴中
	if (node_param.m_wear_pet_idx >= 0)
	{
		if (need_recale)
		{
			this->ReCalcAttr(node_param.m_wear_pet_idx);
			this->SetItemDirty(node_param.m_wear_pet_idx, true, Protocol::SCSendPetSingleInfo::PET_NOTIE_TYPE_PET_GOD_PRINT);
			if (this->IsInPlatfformBattle(m_grid_list[node_param.m_wear_pet_idx].item_unique_id))
			{
				this->SyncPetDataPlatformBattle(node_param.m_wear_pet_idx);
			}
		}
	}
	//神印背包改变下发
	this->SetGodPrintDirtyMark(posi_index - 1);
}

void Pet::UpLevelPetGodPrintEquip2(int index, int bag_num, short cost_index[MAX_USE_ITEM_NUM])
{
	if (!FunOpen::IsFunOpen(m_role_module_mgr->GetRole(), FUN_TYPE_NETHER_GOD_PRINT_SYSTEM))return;
	if (index - 1 < 0 || index - 1 >= NEW_MAX_PET_GOD_PRONT_BAG_NUM)return;
	if (NULL == m_new_god_print_bag[index - 1])return;
	if (m_new_god_print_bag[index - 1]->Invalid())return;
	GodPrintItem &node_param1 = *m_new_god_print_bag[index - 1];
	if (node_param1.Invalid())
	{
		m_role_module_mgr->NoticeNum(errornum::EN_ITEM_IS_INVALID);
		return;
	}
	bool need_recale = false;
	const PetGodPrintEquipMentOtherCfg & other_cfg = LOGIC_CONFIG->GetPetGodPrintEquipMentConfig()->GetOtherCfg();
	for (int i = 0; i < MAX_USE_ITEM_NUM && i < bag_num; i++)
	{
		int c_idx = cost_index[i] - 1;
		if (c_idx < 0 || c_idx >= NEW_MAX_PET_GOD_PRONT_BAG_NUM)continue;
		if (NULL == m_new_god_print_bag[c_idx])continue;
		if (m_new_god_print_bag[c_idx]->Invalid())continue;
		GodPrintItem *node_param2 = m_new_god_print_bag[c_idx];
		if (node_param2->Invalid())
		{
			continue;
		}

		if (node_param2->m_wear_pet_idx >= 0)
		{
			continue;
		}

		if (node_param2->lock == 1)
		{
			continue;
		}
		
		const ItemBase * i_base = ITEMPOOL->GetItem(node_param2->item_id);
		if (i_base == NULL)continue;
		int add_exp = 0;
		
		for (int level_id = 1; level_id < node_param2->level; level_id++)
		{
			const PetGodPrintIntensifyInfoCfg * level_cfg = LOGIC_CONFIG->GetPetGodPrintEquipMentConfig()->GetIntensifyInfoCfg(node_param2->color, level_id);
			if (level_cfg == NULL)break;

			add_exp += level_cfg->exp;
		}
		add_exp += node_param2->exp;
		add_exp = add_exp * other_cfg.exp_attenuation_100 / 100;

		const PetGodPrintResolveExpCfg * resolve_cfg = LOGIC_CONFIG->GetPetGodPrintEquipMentConfig()->GetResolveExpCfg(node_param2->color);
		if (resolve_cfg != NULL)add_exp += resolve_cfg->item_exp;

		bool max_level = false;
		for (int round_num = 0; round_num < 30 && add_exp + node_param1.exp >= node_param1.m_max_exp; round_num++)
		{
			if (add_exp + node_param1.exp >= node_param1.m_max_exp)
			{
				int real_cost_exp = node_param1.m_max_exp - node_param1.exp;
				int need_coin = real_cost_exp * other_cfg.need_coin_100_exp / 100;
				add_exp -= real_cost_exp;
				if (need_coin > 0)
				{
					if (!m_role_module_mgr->GetMoney()->CoinBindMoreThan(need_coin))
					{
						m_role_module_mgr->NoticeNum(errornum::EN_COIN_NOT_ENOUGH);
						add_exp = 0;//不足铜币了，剩下的就直接跳过返回了
						break;
					}
					m_role_module_mgr->GetMoney()->UseCoinBind(need_coin, __FUNCTION__);
				}
				GodPrintItem old_god_print;
				old_god_print = node_param1;
				node_param1.exp += real_cost_exp;
				if (NULL != node_param2 && !node_param2->Invalid())
				{
					NetValueItemParam net_param2;
					net_param2.god_print_param.has_random_net_value = 1;
					net_param2.god_print_param.god_print_data = *node_param2;
					gamelog::g_log_pet.printf(LL_INFO, "PetGodPrint UPLEVEL user[%d,%s] cost_item[%s]", m_role_module_mgr->GetUid(), m_role_module_mgr->GetRole()->GetName(),
						net_param2.GetParamLog(I_TYPE_GOD_PRINT));
					this->ClearGodPrintItem(c_idx);
				}
				//检查下一级
				const PetGodPrintIntensifyInfoCfg *  next_level_cfg = LOGIC_CONFIG->GetPetGodPrintEquipMentConfig()->GetIntensifyInfoCfg((int)node_param1.color, node_param1.level + 1);
				if (next_level_cfg != NULL)
				{
					node_param1.exp -= node_param1.m_max_exp;
					node_param1.m_max_exp = next_level_cfg->exp;
					node_param1.level++;
					need_recale = true;
					//主属性随机
					if (next_level_cfg->main_attribute_up)
					{
						const PetGodPrintIntensifyDetailCfg * main_attr_cfg = LOGIC_CONFIG->GetPetGodPrintEquipMentConfig()->GetIntensifyCfg(PetGodPrintIntensifyDetailCfg::PET_GOD_PRINT_TYPE_MAIN, node_param1.color, node_param1.main_attr.attr_type);
						if (main_attr_cfg != NULL)
						{
							if ((main_attr_cfg->intensify_att_max - main_attr_cfg->intensify_att_min + 1) != 0)
							{
								int rand_value = (rand() % (main_attr_cfg->intensify_att_max - main_attr_cfg->intensify_att_min + 1)) + main_attr_cfg->intensify_att_min;
								node_param1.main_attr.attr_value += rand_value;
							}
							else
							{
								node_param1.main_attr.attr_value += main_attr_cfg->intensify_att_min;
							}
						}
					}
					//副属性随机
					if (next_level_cfg->deputy_attribute_up)
					{
						if (node_param1.attr_num < MAX_PET_GOD_PRONT_ATTR_NUM)//不满4条则增加条数，满则增强词条
						{
							const PetGodPrintEquipMentBaseQualityCfg * base_cfg = LOGIC_CONFIG->GetPetGodPrintEquipMentConfig()->GetBaseCfg((int)node_param1.m_posi + 1, (int)node_param1.color);
							if (base_cfg != NULL)
							{
								int attr_type[MAX_PET_GOD_PRONT_ATTR_NUM];
								memset(attr_type, -1, sizeof(attr_type));

								for (int i = 0; i < MAX_PET_GOD_PRONT_ATTR_NUM && i < node_param1.attr_num; i++)
								{
									attr_type[i] = node_param1.add_attr_list[i].attr_type;
								}

								const PetGodPrintdeputyAttrInfoCfg * deputy_cfg = LOGIC_CONFIG->GetPetGodPrintEquipMentConfig()->RandDeputyAttrBaseInfo(base_cfg->deputy_attribute_att, attr_type);
								if (deputy_cfg != NULL)
								{
									node_param1.add_attr_list[node_param1.attr_num].attr_type = deputy_cfg->att_type;
									if ((deputy_cfg->att_max - deputy_cfg->att_min + 1) != 0)
									{
										int rand_value = (rand() % (deputy_cfg->att_max - deputy_cfg->att_min + 1)) + deputy_cfg->att_min;
										node_param1.add_attr_list[node_param1.attr_num].attr_value += rand_value;
									}
									else
									{
										node_param1.add_attr_list[node_param1.attr_num].attr_value += deputy_cfg->att_min;
									}
									node_param1.attr_num++;
								}
							}
						}
						else
						{
							int attr_up_idx = 0;
							if (node_param1.attr_num > 0)
							{
								attr_up_idx = rand() % node_param1.attr_num;
							}
							if (attr_up_idx < 0 || attr_up_idx >= MAX_PET_GOD_PRONT_ATTR_NUM)
							{
								continue;
							}
							const PetGodPrintIntensifyDetailCfg * deputy_attr_cfg = LOGIC_CONFIG->GetPetGodPrintEquipMentConfig()->GetIntensifyCfg(PetGodPrintIntensifyDetailCfg::PET_GOD_PRINT_TYPE_DEPUTY,
								node_param1.color, node_param1.add_attr_list[attr_up_idx].attr_type);
							if (deputy_attr_cfg != NULL)
							{
								if ((deputy_attr_cfg->intensify_att_max - deputy_attr_cfg->intensify_att_min + 1) != 0)
								{
									int rand_value = (rand() % (deputy_attr_cfg->intensify_att_max - deputy_attr_cfg->intensify_att_min + 1)) + deputy_attr_cfg->intensify_att_min;
									node_param1.add_attr_list[attr_up_idx].attr_value += rand_value;
								}
								else
								{
									node_param1.add_attr_list[attr_up_idx].attr_value += deputy_attr_cfg->intensify_att_min;
								}
							}
						}
					}
				}
				else
				{
					m_god_print_param.exp += add_exp;
					max_level = true;
					break;
				}
				NetValueItemParam old_net_param1;
				old_net_param1.god_print_param.has_random_net_value = 1;
				old_net_param1.god_print_param.god_print_data = old_god_print;
				NetValueItemParam net_param1;
				net_param1.god_print_param.has_random_net_value = 1;
				net_param1.god_print_param.god_print_data = node_param1;
				gamelog::g_log_pet.printf(LL_INFO, "PetGodPrint UPLEVEL user[%d,%s] old_god_print[%s]  new_god_print[%s]", m_role_module_mgr->GetUid(), m_role_module_mgr->GetRole()->GetName(),
					old_net_param1.GetParamLog(I_TYPE_GOD_PRINT), net_param1.GetParamLog(I_TYPE_GOD_PRINT));
			}
		}
		if (add_exp > 0)
		{
			if (add_exp + node_param1.exp > node_param1.m_max_exp)
			{
				int real_add_exp = node_param1.m_max_exp - node_param1.exp;
				if (real_add_exp > 0)
				{
					int need_coin = real_add_exp * other_cfg.need_coin_100_exp / 100;
					if (m_role_module_mgr->GetMoney()->CoinBindMoreThan(need_coin))
					{
						m_role_module_mgr->GetMoney()->UseCoinBind(need_coin, __FUNCTION__);
						node_param1.exp += real_add_exp;
						m_god_print_param.exp += ((node_param1.exp + add_exp) - node_param1.m_max_exp);
					}
				}
			}
			else
			{
				int need_coin = add_exp * other_cfg.need_coin_100_exp / 100;
				if (need_coin > 0)
				{
					if (m_role_module_mgr->GetMoney()->CoinBindMoreThan(need_coin))
					{
						m_role_module_mgr->GetMoney()->UseCoinBind(need_coin, __FUNCTION__);
					}
				}
				node_param1.exp += add_exp;
			}
			if (NULL != node_param2 && !node_param2->Invalid())
			{
				NetValueItemParam net_param2;
				net_param2.god_print_param.has_random_net_value = 1;
				net_param2.god_print_param.god_print_data = *node_param2;
				gamelog::g_log_pet.printf(LL_INFO, "PetGodPrint UPLEVEL user[%d,%s] cost_item[%s]", m_role_module_mgr->GetUid(), m_role_module_mgr->GetRole()->GetName(),
					net_param2.GetParamLog(I_TYPE_GOD_PRINT));
				this->ClearGodPrintItem(c_idx);
			}
		}
		this->SetGodPrintDirtyMark(c_idx);
		if (max_level)
		{
			break;
		}
	}

	//寻找这件神印是否有宠物穿戴中
	if (node_param1.m_wear_pet_idx >= 0)
	{
		if (need_recale)
		{
			this->ReCalcAttr(node_param1.m_wear_pet_idx);
			this->SetItemDirty(node_param1.m_wear_pet_idx, true, Protocol::SCSendPetSingleInfo::PET_NOTIE_TYPE_PET_GOD_PRINT);
			if (this->IsInPlatfformBattle(m_grid_list[node_param1.m_wear_pet_idx].item_unique_id))
			{
				this->SyncPetDataPlatformBattle(node_param1.m_wear_pet_idx);
			}
		}
	}
	//神印背包改变下发
	this->SetGodPrintDirtyMark(index - 1);
}

void Pet::UpLevelPetGodPrintEquip3(int posi_index, short cost_column_list, short cost_item_list, short cost_item_num)
{
	if (!FunOpen::IsFunOpen(m_role_module_mgr->GetRole(), FUN_TYPE_NETHER_GOD_PRINT_SYSTEM))return;
	if (posi_index - 1 < 0 || posi_index - 1 >= NEW_MAX_PET_GOD_PRONT_BAG_NUM)return;
	if (NULL == m_new_god_print_bag[posi_index - 1])return;
	if (m_new_god_print_bag[posi_index - 1]->Invalid())return;
	GodPrintItem &node_param1 = *m_new_god_print_bag[posi_index - 1];
	if (node_param1.Invalid())
	{
		m_role_module_mgr->NoticeNum(errornum::EN_ITEM_IS_INVALID);
		return;
	}
	bool need_recale = false;
	const PetGodPrintEquipMentOtherCfg & other_cfg = LOGIC_CONFIG->GetPetGodPrintEquipMentConfig()->GetOtherCfg();
	//计算增加的经验值
	ItemGridData cost_item; cost_item.Reset();
	if (!m_role_module_mgr->GetKnapsack()->GetItemGridData(cost_column_list, cost_item_list, &cost_item)) return;
	const ItemBase * item_base = ITEMPOOL->GetItem(cost_item.item_id);
	if (item_base == NULL)return;

	int add_exp = 0;
	const GodPrintIntensifyItemCfg * item_cfg = LOGIC_CONFIG->GetPetGodPrintEquipMentConfig()->GetGodPrintIntensifyItem(cost_item.item_id);
	if (item_cfg != NULL)
	{
		add_exp += item_cfg->intensify_item_exp * cost_item_num;
	}
	if (add_exp <= 0)return;
	int suppose_level = node_param1.level;
	int suppose_exp = node_param1.exp;
	int suppose_max_exp = node_param1.m_max_exp;
	int suppose_color = node_param1.color;
	int suppose_cost_coin = 0;
	int suppose_cost_exp = 0;
	for (int round_num = 0; round_num < 30; round_num++)//假设升级
	{
		if (add_exp + suppose_exp >= suppose_max_exp)
		{
			int real_cost_exp = suppose_max_exp - suppose_exp;
			if (real_cost_exp > 0)
			{
				//消耗铜币
				suppose_cost_coin += real_cost_exp * other_cfg.need_coin_100_exp / 100;//累积假设铜币消耗数
				if (!m_role_module_mgr->GetMoney()->CoinBindMoreThan(suppose_cost_coin))
				{
					m_role_module_mgr->NoticeNum(errornum::EN_COIN_NOT_ENOUGH);
					return;
				}
			}
			//检查下一级
			const PetGodPrintIntensifyInfoCfg *  next_level_cfg = LOGIC_CONFIG->GetPetGodPrintEquipMentConfig()->GetIntensifyInfoCfg((int)suppose_color, suppose_level + 1);
			if (next_level_cfg != NULL)
			{
				add_exp -= real_cost_exp;
				suppose_cost_exp += real_cost_exp;
				suppose_exp = 0;
				suppose_max_exp = next_level_cfg->exp;
				suppose_level++;
			}
			else
			{
				suppose_cost_exp += suppose_max_exp - suppose_exp;
				suppose_exp = suppose_max_exp;
				add_exp = 0;
				break;
			}
		}
		else
		{
			break;
		}
	}

	if (add_exp > 0)
	{
		if (add_exp + suppose_exp >= suppose_max_exp)
		{
			suppose_cost_exp += suppose_max_exp - suppose_exp;
			suppose_cost_coin += (suppose_max_exp - suppose_exp) * other_cfg.need_coin_100_exp / 100;//累积假设铜币消耗数
			suppose_exp = suppose_max_exp;
			if (!m_role_module_mgr->GetMoney()->CoinBindMoreThan(suppose_cost_coin))
			{
				m_role_module_mgr->NoticeNum(errornum::EN_COIN_NOT_ENOUGH);
				return;
			}
		}
		else
		{
			suppose_exp += add_exp;
			suppose_cost_exp += add_exp;
			suppose_cost_coin += add_exp * other_cfg.need_coin_100_exp / 100;//累积假设铜币消耗数
			if (!m_role_module_mgr->GetMoney()->CoinBindMoreThan(suppose_cost_coin))
			{
				m_role_module_mgr->NoticeNum(errornum::EN_COIN_NOT_ENOUGH);
				return;
			}
		}
	}

	//检查经验消耗对应消耗的道具数量
	int real_cost_item_num = 0;
	real_cost_item_num = suppose_cost_exp / item_cfg->intensify_item_exp;
	if (suppose_cost_exp % item_cfg->intensify_item_exp != 0)
	{
		real_cost_item_num += 1;//有多余的直接多扣一个道具
	}

	//消耗道具与铜币
	if (real_cost_item_num > 0 && !m_role_module_mgr->GetKnapsack()->ConsumeItemByIndex(cost_column_list, cost_item_list, real_cost_item_num, __FUNCTION__))
	{
		return;
	}

	m_role_module_mgr->GetMoney()->UseCoinBind(suppose_cost_coin, __FUNCTION__);

	for (int level_next = node_param1.level; level_next < suppose_level; level_next++)
	{
		const PetGodPrintIntensifyInfoCfg *  next_level_cfg = LOGIC_CONFIG->GetPetGodPrintEquipMentConfig()->GetIntensifyInfoCfg((int)node_param1.color, level_next + 1);
		if (next_level_cfg != NULL)
		{
			//主属性随机
			if (next_level_cfg->main_attribute_up)
			{
				const PetGodPrintIntensifyDetailCfg * main_attr_cfg = LOGIC_CONFIG->GetPetGodPrintEquipMentConfig()->GetIntensifyCfg(PetGodPrintIntensifyDetailCfg::PET_GOD_PRINT_TYPE_MAIN, node_param1.color, node_param1.main_attr.attr_type);
				if (main_attr_cfg != NULL)
				{
					if ((main_attr_cfg->intensify_att_max - main_attr_cfg->intensify_att_min + 1) != 0)
					{
						int rand_value = (rand() % (main_attr_cfg->intensify_att_max - main_attr_cfg->intensify_att_min + 1)) + main_attr_cfg->intensify_att_min;
						node_param1.main_attr.attr_value += rand_value;
					}
					else
					{
						node_param1.main_attr.attr_value += main_attr_cfg->intensify_att_min;
					}
				}
			}
			//副属性随机
			if (next_level_cfg->deputy_attribute_up)
			{
				if (node_param1.attr_num < MAX_PET_GOD_PRONT_ATTR_NUM)//不满4条则增加条数，满则增强词条
				{
					const PetGodPrintEquipMentBaseQualityCfg * base_cfg = LOGIC_CONFIG->GetPetGodPrintEquipMentConfig()->GetBaseCfg((int)node_param1.m_posi + 1, (int)node_param1.color);
					if (base_cfg != NULL)
					{
						int attr_type[MAX_PET_GOD_PRONT_ATTR_NUM];
						memset(attr_type, -1, sizeof(attr_type));

						for (int i = 0; i < MAX_PET_GOD_PRONT_ATTR_NUM && i < node_param1.attr_num; i++)
						{
							attr_type[i] = node_param1.add_attr_list[i].attr_type;
						}

						const PetGodPrintdeputyAttrInfoCfg * deputy_cfg = LOGIC_CONFIG->GetPetGodPrintEquipMentConfig()->RandDeputyAttrBaseInfo(base_cfg->deputy_attribute_att, attr_type);
						if (deputy_cfg != NULL)
						{
							node_param1.add_attr_list[node_param1.attr_num].attr_type = deputy_cfg->att_type;
							if ((deputy_cfg->att_max - deputy_cfg->att_min + 1) != 0)
							{
								int rand_value = (rand() % (deputy_cfg->att_max - deputy_cfg->att_min + 1)) + deputy_cfg->att_min;
								node_param1.add_attr_list[node_param1.attr_num].attr_value += rand_value;
							}
							else
							{
								node_param1.add_attr_list[node_param1.attr_num].attr_value += deputy_cfg->att_min;
							}
							node_param1.attr_num++;
						}
					}
				}
				else
				{
					int attr_up_idx = 0;
					if (node_param1.attr_num > 0)
					{
						attr_up_idx = rand() % node_param1.attr_num;
					}
					if (attr_up_idx < 0 || attr_up_idx >= MAX_PET_GOD_PRONT_ATTR_NUM)
					{
						continue;
					}
					const PetGodPrintIntensifyDetailCfg * deputy_attr_cfg = LOGIC_CONFIG->GetPetGodPrintEquipMentConfig()->GetIntensifyCfg(PetGodPrintIntensifyDetailCfg::PET_GOD_PRINT_TYPE_DEPUTY,
						node_param1.color, node_param1.add_attr_list[attr_up_idx].attr_type);
					if (deputy_attr_cfg != NULL)
					{
						if ((deputy_attr_cfg->intensify_att_max - deputy_attr_cfg->intensify_att_min + 1) != 0)
						{
							int rand_value = (rand() % (deputy_attr_cfg->intensify_att_max - deputy_attr_cfg->intensify_att_min + 1)) + deputy_attr_cfg->intensify_att_min;
							node_param1.add_attr_list[attr_up_idx].attr_value += rand_value;
						}
						else
						{
							node_param1.add_attr_list[attr_up_idx].attr_value += deputy_attr_cfg->intensify_att_min;
						}
					}
				}
			}
			need_recale = true;
		}
	}

	node_param1.level = suppose_level;
	node_param1.m_max_exp = suppose_max_exp;
	node_param1.exp = suppose_exp;

	//寻找这件神印是否有宠物穿戴中
	if (node_param1.m_wear_pet_idx >= 0)
	{
		if (need_recale)
		{
			this->ReCalcAttr(node_param1.m_wear_pet_idx);
			this->SetItemDirty(node_param1.m_wear_pet_idx, true, Protocol::SCSendPetSingleInfo::PET_NOTIE_TYPE_PET_GOD_PRINT);
			if (this->IsInPlatfformBattle(m_grid_list[node_param1.m_wear_pet_idx].item_unique_id))
			{
				this->SyncPetDataPlatformBattle(node_param1.m_wear_pet_idx);
			}
		}
	}
	//神印背包改变下发
	this->SetGodPrintDirtyMark(posi_index - 1);
}

void Pet::DiscomplsePetGodPrintEquip(int posi_index)
{
	if (!FunOpen::IsFunOpen(m_role_module_mgr->GetRole(), FUN_TYPE_NETHER_GOD_PRINT_SYSTEM))return;
	if (posi_index - 1 < 0 || posi_index - 1 >= NEW_MAX_PET_GOD_PRONT_BAG_NUM)return;
	if (NULL == m_new_god_print_bag[posi_index - 1])return;
	if (m_new_god_print_bag[posi_index - 1]->Invalid())return;
	GodPrintItem &item_param = *m_new_god_print_bag[posi_index - 1];
	if (item_param.m_wear_pet_idx >= 0)
	{
		return;
	}
	if (item_param.lock == 1)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_PET_GOD_PRINT_HAS_LOCK);
		return;
	}
	int add_exp = 0;
	for (int i = 1; i < item_param.level; i++)
	{
		const PetGodPrintIntensifyInfoCfg *  level_cfg = LOGIC_CONFIG->GetPetGodPrintEquipMentConfig()->GetIntensifyInfoCfg(item_param.color, i);
		if (level_cfg != NULL)
		{
			add_exp += level_cfg->exp;
		}
	}

	if (item_param.exp > 0)
	{
		add_exp += item_param.exp;
	}

	const PetGodPrintEquipMentOtherCfg & other_cfg = LOGIC_CONFIG->GetPetGodPrintEquipMentConfig()->GetOtherCfg();
	
	add_exp = add_exp * other_cfg.exp_attenuation_100 / 100;
	m_god_print_param.exp += add_exp;
	std::string log_str;
	log_str = item_param.GetParamLog();
	int resolve_exp = 0;
	int god_print_item_id = item_param.item_id;
	int god_print_level = item_param.level;
	const PetGodPrintResolveExpCfg * resolve_cfg = LOGIC_CONFIG->GetPetGodPrintEquipMentConfig()->GetResolveExpCfg(item_param.color);
	if (resolve_cfg != NULL)
	{
		item_param.Reset();
		m_god_print_param.exp += resolve_cfg->item_exp;
		resolve_exp = resolve_cfg->item_exp;
	}

	gamelog::g_log_pet.printf(LL_INFO, "%s user[%d,%s] now_exp[%lld] add_exp[%d] resolve_exp[%d] param[%s]",
		__FUNCTION__, m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), m_god_print_param.exp, add_exp, resolve_exp, log_str.c_str());

	ROLE_LOG_QUICK10(LOG_TYPE_GOD_PRINT_DISCOMPOSE, m_role_module_mgr->GetRole(), god_print_item_id, 1, __FUNCTION__, log_str.c_str(), god_print_level, m_god_print_param.exp, resolve_exp, add_exp, 0);
	this->SetGodPrintDirtyMark(posi_index - 1);
	this->ClearGodPrintFromDiscomplse(posi_index);
}

void Pet::DiscomplsePetGodPrintItem(short cost_column_list, short cost_item_list, short cost_item_num)
{
	if (!FunOpen::IsFunOpen(m_role_module_mgr->GetRole(), FUN_TYPE_NETHER_GOD_PRINT_SYSTEM))return;

	//计算增加的经验值
	ItemGridData cost_item; cost_item.Reset();
	if (!m_role_module_mgr->GetKnapsack()->GetItemGridData(cost_column_list, cost_item_list, &cost_item)) return;
	const ItemBase * item_base = ITEMPOOL->GetItem(cost_item.item_id);
	if (item_base == NULL)return;

	int add_exp = 0;
	const GodPrintIntensifyItemCfg * item_cfg = LOGIC_CONFIG->GetPetGodPrintEquipMentConfig()->GetGodPrintIntensifyItem(cost_item.item_id);
	if (item_cfg != NULL)
	{
		add_exp += item_cfg->intensify_item_exp * cost_item_num;
	}

	if (add_exp <= 0)return;

	if (cost_item_num > 0 && !m_role_module_mgr->GetKnapsack()->ConsumeItemByIndex(cost_column_list, cost_item_list, cost_item_num, __FUNCTION__))
	{
		return;
	}

	m_god_print_param.exp += add_exp;

	ROLE_LOG_QUICK10(LOG_TYPE_GOD_PRINT_DISCOMPOSE, m_role_module_mgr->GetRole(), cost_item.item_id, cost_item_num, __FUNCTION__, "NULL",
		0, m_god_print_param.exp, 0, add_exp, 0);
	this->SendExpInfo();
}

void Pet::ClearGodPrintFromDiscomplse(int posi_index)
{
	if (posi_index - 1 < 0 || posi_index - 1 >= NEW_MAX_PET_GOD_PRONT_BAG_NUM)return;
	for (int i = 0; i < MAX_PET_GOD_PRONT_SCHEME_NUM; i++)
	{
		bool change_shceme = false;
		for (int j = 0; j < MAX_PET_GOD_PRONT_ONE_SCHEME_NUM; j++)
		{
			if (m_god_print_param.wear_scheme_list[i].bag_idx[j] == posi_index)
			{
				m_god_print_param.wear_scheme_list[i].bag_idx[j] = 0;
				change_shceme = true;
			}
		}
		if (change_shceme)this->SendSingleGodPrintScheme(i);
	}
}

void Pet::SetPetGodPrintScheme(Protocol::CSPetGodPrintSetearSchemeReq *msg)
{
	if (NULL == msg)return;
	if (!FunOpen::IsFunOpen(m_role_module_mgr->GetRole(), FUN_TYPE_NETHER_GOD_PRINT_SYSTEM))return;
	if (msg->bag_idx < 0 || msg->bag_idx >= MAX_PET_GOD_PRONT_SCHEME_NUM)return;

	//方案清除宠物标记
	if(msg->bag_list.use_pet_idx >= 0)this->PutOffPetGodPrintScheme(msg->bag_list.use_pet_idx);
	m_god_print_param.wear_scheme_list[msg->bag_idx].use_pet_idx = msg->bag_list.use_pet_idx;
	F_STRNCPY(m_god_print_param.wear_scheme_list[msg->bag_idx].scheme_name, msg->bag_list.scheme_name, sizeof(GameName));
	for (int i = 0; i < MAX_PET_GOD_PRONT_ONE_SCHEME_NUM; i++)
	{
		if(msg->bag_list.bag_idx[i] < 0 || msg->bag_list.bag_idx[i] > NEW_MAX_PET_GOD_PRONT_BAG_NUM)continue;
		m_god_print_param.wear_scheme_list[msg->bag_idx].bag_idx[i] = msg->bag_list.bag_idx[i];
	}

	this->SendSingleGodPrintScheme(msg->bag_idx);
}

void Pet::SendSingleGodPrintScheme(int scheme_id)
{
	if (scheme_id < 0 || scheme_id >= MAX_PET_GOD_PRONT_SCHEME_NUM)return;
	SCPetGodPrintWearSchemeSingleInfo info;
	info.bag_idx = scheme_id;
	info.bag_list = m_god_print_param.wear_scheme_list[scheme_id];
	m_role_module_mgr->NetSend((const char*)&info, sizeof(info));
}

void Pet::SendSingleGodPrintBagInfo(int posi_index, int put_reason)
{
	if (posi_index < 0 || posi_index >= NEW_MAX_PET_GOD_PRONT_BAG_NUM)return;
	Protocol::SCPetGodPrintBagSingleInfo info;
	info.put_reason = put_reason;
	info.exp = m_god_print_param.exp;
	info.bag_idx = posi_index + 1;
	info.bag_list.Reset();
	if (NULL != m_new_god_print_bag[posi_index] && !m_new_god_print_bag[posi_index]->Invalid())
	{
		info.bag_list = *m_new_god_print_bag[posi_index];
	}
	m_role_module_mgr->NetSend((const char*)&info, sizeof(info));
}

void Pet::SendExpInfo()
{
	Protocol::SCPetGodPrintEXPInfo info;
	info.exp = m_god_print_param.exp;
	m_role_module_mgr->NetSend((const char*)&info, sizeof(info));
}

void Pet::SendGodPrintBagInfo()
{
	if (!FunOpen::IsFunOpen(m_role_module_mgr->GetRole(), FUN_TYPE_NETHER_GOD_PRINT_SYSTEM))return;
	Protocol::SCPetGodPrintBagInfo info;
	info.round_seq = m_god_print_log_param.round_seq;
	info.today_free_wish_times = m_god_print_param.today_free_times;
	info.exp = m_god_print_param.exp;
	for (int i = 0; i < MAX_PET_GOD_PRONT_SCHEME_NUM; i++)
	{
		info.wear_scheme_list[i] = m_god_print_param.wear_scheme_list[i];
	}
	info.bag_num = 0;
	for (int i = 0; i < NEW_MAX_PET_GOD_PRONT_BAG_NUM && info.bag_num < NEW_MAX_PET_GOD_PRONT_BAG_NUM; i++)
	{
		if (NULL == m_new_god_print_bag[i])continue;
		if (m_new_god_print_bag[i]->Invalid())continue;
		info.bag_list[info.bag_num].item_info = *m_new_god_print_bag[i];
		info.bag_list[info.bag_num++].bag_idx = i + 1;
	}
	m_role_module_mgr->NetSend((const char*)&info, sizeof(info) - (NEW_MAX_PET_GOD_PRONT_BAG_NUM - info.bag_num) * sizeof(GodPrintItemSendInfo));
}

bool Pet::DevourGodPrint(int main_indnx, int bedevour_idx, int voc_idx)
{
	if (main_indnx - 1< 0 || main_indnx - 1 >= NEW_MAX_PET_GOD_PRONT_BAG_NUM)return false;
	if (bedevour_idx - 1 < 0 || bedevour_idx - 1 >= NEW_MAX_PET_GOD_PRONT_BAG_NUM)return false;
	if (NULL == m_new_god_print_bag[main_indnx - 1])return false;
	if (NULL == m_new_god_print_bag[bedevour_idx - 1])return false;

	if (m_new_god_print_bag[main_indnx - 1]->Invalid())return false;
	if (m_new_god_print_bag[bedevour_idx - 1]->Invalid())return false;

	if (main_indnx == bedevour_idx)return false;

	GodPrintItem &main_god_print_item = *m_new_god_print_bag[main_indnx - 1];
	GodPrintItem &devour_god_print_item = *m_new_god_print_bag[bedevour_idx - 1];

	//检查
	if (main_god_print_item.color != devour_god_print_item.color ||
		main_god_print_item.level != devour_god_print_item.level ||
		main_god_print_item.item_id != devour_god_print_item.item_id)
	{
		return false;
	}

	if (devour_god_print_item.m_wear_pet_idx >= 0)return false;//穿戴中的不允许被吞噬
	//被吞噬词条
	if (devour_god_print_item.devour_num >= 1)return false;//吞噬过词条的不允许作为材料
	if (voc_idx < 0 || voc_idx >= MAX_PET_GOD_PRONT_ATTR_NUM || voc_idx >= devour_god_print_item.attr_num)return false;
	AttrInfo & attr_node = devour_god_print_item.add_attr_list[voc_idx];

	//获得词条
	if (main_god_print_item.devour_num >= 1)//允许吞噬词条数
	{
		return false;
	}

	const PetGodPrintEquipMentOtherCfg & other_cfg = LOGIC_CONFIG->GetPetGodPrintEquipMentConfig()->GetOtherCfg();
	if (other_cfg.swallow_gold_cost > 0)
	{
		if (!m_role_module_mgr->GetMoney()->UseGold(other_cfg.swallow_gold_cost, __FUNCTION__))
		{
			return false;
		}
	}

	AttrInfo & get_attr = main_god_print_item.devour_attr_list;

	get_attr.attr_type = attr_node.attr_type;
	get_attr.attr_value = attr_node.attr_value;
	main_god_print_item.devour_num += 1;
	gamelog::g_log_pet.printf(LL_INFO, "%s user[%d,%s] new_item[%s] cost_item[%s]", __FUNCTION__, m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(),
		main_god_print_item.GetParamLog(), devour_god_print_item.GetParamLog());

	devour_god_print_item.Reset();
	if(main_god_print_item.m_wear_pet_idx >= 0)
	{
		this->ReCalcAttr(main_god_print_item.m_wear_pet_idx);
		this->SetItemDirty(main_god_print_item.m_wear_pet_idx, true, Protocol::SCSendPetSingleInfo::PET_NOTIE_TYPE_PET_GOD_PRINT);
		if (this->IsInPlatfformBattle(m_grid_list[main_god_print_item.m_wear_pet_idx].item_unique_id))
		{
			this->SyncPetDataPlatformBattle(main_god_print_item.m_wear_pet_idx);
		}
	}
	this->SetGodPrintDirtyMark(main_indnx - 1);
	this->SetGodPrintDirtyMark(bedevour_idx - 1);
	this->ClearGodPrintFromDiscomplse(main_indnx);
	this->ClearGodPrintFromDiscomplse(bedevour_idx);
	return true;
}

bool Pet::DropGodprintEquip(int posi_index)
{
	if (!FunOpen::IsFunOpen(m_role_module_mgr->GetRole(), FUN_TYPE_NETHER_GOD_PRINT_SYSTEM))return false;
	if (posi_index - 1 < 0 || posi_index - 1 >= NEW_MAX_PET_GOD_PRONT_BAG_NUM)return false;
	if (NULL == m_new_god_print_bag[posi_index - 1])return false;
	if (m_new_god_print_bag[posi_index - 1]->Invalid())return false;
	GodPrintItem &item_param = *m_new_god_print_bag[posi_index - 1];
	if (item_param.m_wear_pet_idx >= 0)
	{
		return false;
	}
	if (item_param.lock == 1)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_PET_GOD_PRINT_HAS_LOCK);
		return false;
	}

	std::string log_str;
	log_str = item_param.GetParamLog();
	item_param.Reset();
	gamelog::g_log_pet.printf(LL_INFO, "%s user[%d,%s] param[%s]",
		__FUNCTION__, m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), log_str.c_str());

	this->SetGodPrintDirtyMark(posi_index - 1);
	this->ClearGodPrintFromDiscomplse(posi_index);
	return true;
}

void Pet::ReqWishGodPrint(bool use_gold, int cost_coin, bool is_discomplse)
{
	if (!FunOpen::IsFunOpen(m_role_module_mgr->GetRole(), FUN_TYPE_NETHER_WISH_GOD_PRINT))return;
	const PetGodPrintEquipMentOtherCfg & other_cfg = LOGIC_CONFIG->GetPetGodPrintEquipMentConfig()->GetOtherCfg();
	
	if (other_cfg.free_num <= m_god_print_param.today_free_times)
	{
		if (use_gold)
		{
			if (!m_role_module_mgr->GetMoney()->GoldMoreThan(cost_coin))
			{
				m_role_module_mgr->NoticeNum(errornum::EN_GOLD_NOT_ENOUGH);
				return;
			}
		}
		else
		{
			if (!m_role_module_mgr->GetKnapsack()->HasItem(other_cfg.pray_cost_item, cost_coin))
			{
				m_role_module_mgr->NoticeNum(errornum::EN_ITEM_NOT_ELIGIBLE);
				return;
			}
		}
	}

	const PetGodPrintWishCfg *  wish_cfg = LOGIC_CONFIG->GetPetGodPrintEquipMentConfig()->GetWishCfg(m_god_print_log_param.round_seq);
	if (NULL == wish_cfg)return;
	Protocol::SCPetGodPrintWishRewardInfo reward_info;
	reward_info.use_gold = use_gold ? 1 : 0;
	reward_info.real_cost_coin = 0;
	reward_info.reward_num = 0;

	for (int i = 0; i < MAX_GOD_PRINT_WISH_REWARD_NUM; i++)
	{
		reward_info.reward_list[i].bag_idx = -1;
		reward_info.reward_list[i].item_group = 0;
		reward_info.reward_list[i].item_seq = 0;
		reward_info.reward_list[i].wish_idx = -1;
	}

	int round_cost = use_gold ? wish_cfg->expend_gold : wish_cfg->expend_coin;
	int max_round = 0;
	if (cost_coin < round_cost)
	{
		if (use_gold)m_role_module_mgr->NoticeNum(errornum::EN_GOLD_NOT_ENOUGH);
		else m_role_module_mgr->NoticeNum(errornum::EN_ITEM_NOT_ELIGIBLE);
		return;
	}
	bool send_exp = false;
	
	while (cost_coin >= round_cost && max_round < MAX_GOD_PRINT_WISH_REWARD_NUM)//最多20轮
	{
		max_round++;
		//
		if (this->GetBagEmptyNum() < 1)
		{
			m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_GOD_PRINT_COLUMN_NO_SPACE);
			break;
		}
		const PetGodPrintWishRewardCfg * reward_cfg = LOGIC_CONFIG->GetPetGodPrintEquipMentConfig()->RandRewardByGroup(wish_cfg->item_group);
		if (reward_cfg == NULL)break;

		if (m_god_print_param.today_free_times < other_cfg.free_num)
		{
			const PetGodPrintWishCfg * low_cost_cfg = LOGIC_CONFIG->GetPetGodPrintEquipMentConfig()->GetWishCfg(0);
			if (NULL != low_cost_cfg)
			{//免费一次最低消耗
				if (use_gold)
				{
					round_cost -= low_cost_cfg->expend_gold;
				}
				else
				{
					round_cost -= low_cost_cfg->expend_coin;
				}
			}
			m_god_print_param.today_free_times += 1;
		}

		if (use_gold)
		{
			if (!m_role_module_mgr->GetMoney()->UseGold(round_cost, __FUNCTION__))
			{
				m_role_module_mgr->NoticeNum(errornum::EN_GOLD_NOT_ENOUGH);
				break;
			}
		}
		else
		{
			if (!m_role_module_mgr->GetKnapsack()->ConsumeItem(other_cfg.pray_cost_item, round_cost, __FUNCTION__))
			{
				m_role_module_mgr->NoticeNum(errornum::EN_COIN_NOT_ENOUGH);
				break;
			}
		}

		cost_coin -= round_cost;
		reward_info.real_cost_coin += round_cost;
		const ItemBase * item_base = ITEMPOOL->GetItem(reward_cfg->god_print_item);
		if (NULL != item_base)
		{
			if (item_base->GetItemType() == I_TYPE_GOD_PRINT)
			{
				const GodPrintEquipment * god_print = (const GodPrintEquipment*)item_base;
				int param1 = god_print->GetParam1();
				int param2 = god_print->GetParam2();
				int param3 = god_print->GetParam3();
				int out_bag_idx = -1;
				long long unique_id = 0;
				UniqueIDGenerator::Instance()->GenUniqueID(&unique_id);
				const GodPrintItem* god_print_data = this->AddPetGodPrintItem(item_base->GetItemId(), param2, param3, param1, unique_id, 0, &out_bag_idx);
				if (god_print_data != NULL)
				{
					this->AddWishLog(god_print_data);

					if (out_bag_idx >= 0)reward_info.reward_list[reward_info.reward_num].bag_idx = out_bag_idx + 1;
				}

				if (reward_cfg->is_broadcast == 1)
				{
					int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_pet_god_print_wish_broadcast, m_role_module_mgr->GetUid(), m_role_module_mgr->GetRole()->GetName(), item_base->GetItemId());
					if (length > 0)
					{
						World::GetInstWorld()->GetSceneManager()->SystemMsgAll(gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_ROLLING_NOTICE, SYS_MSG_SYSTEM_CHAT));
			}
		}
			}
		}
		
		if ((short)reward_cfg->m_reward_vec.size() > 0)
		{
			//自动分解
			int add_exp = 0;
			for (std::vector<ItemConfigData>::const_iterator it = reward_cfg->m_reward_vec.begin(); it != reward_cfg->m_reward_vec.end(); it++)
			{
				const GodPrintIntensifyItemCfg * item_cfg = LOGIC_CONFIG->GetPetGodPrintEquipMentConfig()->GetGodPrintIntensifyItem(it->item_id);
				if (item_cfg != NULL)
				{
					if (is_discomplse)
					{
						add_exp += item_cfg->intensify_item_exp * it->num;
						if (add_exp > 0)
						{
							m_god_print_param.exp += add_exp;
							send_exp = true;
						}
					}
					else
					{
						if (!m_role_module_mgr->GetKnapsack()->CheckForPut(it->item_id, it->num))
						{
							m_role_module_mgr->GetKnapsack()->SendMail(&(*it), 1, SNED_MAIL_TYPE_DEFAULT);
						}
						else
						{
							m_role_module_mgr->GetKnapsack()->Put(*it, PUT_REASON_GOD_PRINT_WISH_REWARD);
						}
					}
				}
				else
				{
					if (!m_role_module_mgr->GetKnapsack()->CheckForPut(it->item_id, it->num))
					{
						m_role_module_mgr->GetKnapsack()->SendMail(&(*it), 1, SNED_MAIL_TYPE_DEFAULT);
					}
					else
					{
						m_role_module_mgr->GetKnapsack()->Put(*it, PUT_REASON_GOD_PRINT_WISH_REWARD);
					}
				}
			}
			
			reward_info.reward_list[reward_info.reward_num].item_group = reward_cfg->item_group;
			reward_info.reward_list[reward_info.reward_num].item_seq = reward_cfg->item_seq;
		}
		reward_info.reward_list[reward_info.reward_num++].wish_idx = m_god_print_log_param.round_seq;
		if (m_god_print_param.wish_time + 1 >= MAX_INT16)
		{
			m_god_print_param.wish_time = MAX_INT16;
		}
		else
		{
			m_god_print_param.wish_time += 1;
		}
		int rand_rate = rand() % 10000;
		if (rand_rate < wish_cfg->rate_10000)
		{
			m_god_print_log_param.round_seq += 1;
		}
		else
		{
			m_god_print_log_param.round_seq = 0;
		}
		
		wish_cfg = LOGIC_CONFIG->GetPetGodPrintEquipMentConfig()->GetWishCfg(m_god_print_log_param.round_seq);
		if (NULL == wish_cfg)break;

		if (reward_info.reward_num >= MAX_GOD_PRINT_WISH_REWARD_NUM)break;

		round_cost = use_gold ? wish_cfg->expend_gold : wish_cfg->expend_coin;
	}
	reward_info.end_seq = m_god_print_log_param.round_seq;
	reward_info.today_free_times = m_god_print_param.today_free_times;
	m_role_module_mgr->NetSend((const char*)&reward_info, sizeof(reward_info) - (MAX_GOD_PRINT_WISH_REWARD_NUM - reward_info.reward_num) * sizeof(int));
	if (send_exp)this->SendExpInfo();
	
	EventHandler::Instance().OnPetWishGodPrint(m_role_module_mgr->GetRole());
}

void Pet::ExchangeGodPrint(int seq)
{
	if (!FunOpen::IsFunOpen(m_role_module_mgr->GetRole(), FUN_TYPE_NETHER_WISH_GOD_PRINT))return;
	const PetGodPrintWishExchangeCfg * exchange_cfg = LOGIC_CONFIG->GetPetGodPrintEquipMentConfig()->GetExchangeSeq(seq);
	if (exchange_cfg == NULL)return;
	const PetGodPrintWishExchangeItemCfg * reward_cfg = LOGIC_CONFIG->GetPetGodPrintEquipMentConfig()->RandExchangeReward(exchange_cfg->exchange_group);
	if (reward_cfg == NULL)return;
	if ((short)reward_cfg->reward_vec.size() < 0)return;
	const PetGodPrintEquipMentOtherCfg & other_cfg = LOGIC_CONFIG->GetPetGodPrintEquipMentConfig()->GetOtherCfg();
	if (exchange_cfg->expend > 0)
	{
		if (!m_role_module_mgr->GetKnapsack()->ConsumeItem(other_cfg.exchange_item_id, exchange_cfg->expend, __FUNCTION__))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_ITEM_AMOUNT_NOT_ENOUGH);
			return;
		}
	}

	if (reward_cfg->is_broadcast == 1)
	{
		if ((int)reward_cfg->reward_vec.size() > 0)
		{
			int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_pet_god_print_wish_broadcast_2, m_role_module_mgr->GetUid(), m_role_module_mgr->GetRole()->GetName(), reward_cfg->reward_vec[0].item_id);
			if (length > 0)
			{
				World::GetInstWorld()->GetSceneManager()->SystemMsgAll(gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_ROLLING_NOTICE, SYS_MSG_SYSTEM_CHAT));
			}
		}
	}

	if (m_role_module_mgr->GetKnapsack()->CheckForPutList((short)reward_cfg->reward_vec.size(), &reward_cfg->reward_vec[0]))
	{
		m_role_module_mgr->GetKnapsack()->PutList((short)reward_cfg->reward_vec.size(), &reward_cfg->reward_vec[0], PUT_REASON_GOD_PRINT_EXCHANGE_REWARD);
	}
	else
	{
		m_role_module_mgr->GetKnapsack()->SendMail(&reward_cfg->reward_vec[0], (short)reward_cfg->reward_vec.size(), SNED_MAIL_TYPE_DEFAULT);
	}
}

void Pet::AddWishLog(const GodPrintItem * item_data)
{
	if (item_data == NULL)return;
	NewWishLogData log_node;
	log_node.item_data = *item_data;
	log_node.wish_tamp = (unsigned int)EngineAdapter::Instance().Time();
	m_new_god_print_log_param.PushBackData(&log_node);
}

void Pet::SendGodPrintWishLogInfo()
{
	if (!FunOpen::IsFunOpen(m_role_module_mgr->GetRole(), FUN_TYPE_NETHER_WISH_GOD_PRINT))return;
	Protocol::SCPetGodPrintWishLogListInfo info;
	info.log_num = 0;
	for (int i = 0; i < MAX_GOD_PRINT_WISH_LOG_NUM && i < m_new_god_print_log_param.log_num; i++)
	{
		info.log_data[info.log_num].log_tamp = m_new_god_print_log_param.m_log_list[i].wish_tamp;
		info.log_data[info.log_num++].item_id = m_new_god_print_log_param.m_log_list[i].item_data.item_id;
	}
	m_role_module_mgr->NetSend((const char*)&info, sizeof(info));
}

void Pet::SendGodPrintWishDetailInfo(int log_idx)
{
	if (!FunOpen::IsFunOpen(m_role_module_mgr->GetRole(), FUN_TYPE_NETHER_WISH_GOD_PRINT))return;
	if (log_idx < 0 || log_idx >= MAX_GOD_PRINT_WISH_LOG_NUM)return;
	Protocol::SCPetGodPrintWishLogDetailInfo info;
	info.log_detail = m_new_god_print_log_param.m_log_list[log_idx];
	m_role_module_mgr->NetSend((const char*)&info, sizeof(info));
}

bool Pet::HasPetSkin(int skin_id)
{
	for (int i = 0; i < m_pet_skin_param.pet_skin_count && i < ARRAY_LENGTH(m_pet_skin_param.pet_skin_list); ++i)
	{
		if (m_pet_skin_param.pet_skin_list[i] == skin_id)
		{
			return true;
		}
	}

	return false;
}

bool Pet::ActivityPetSkin(int skin_id, short num,bool is_notice)
{
	const SkinPetCfg * skin_pet_cfg = LOGIC_CONFIG->GetSkinConfig()->GetSkinPetCfgBySkinId(skin_id);
	if (skin_pet_cfg == NULL || num <= 0)
	{
		return false;
	}

	if (!this->HasPetSkin(skin_id))
	{	
		if (m_pet_skin_param.pet_skin_count >= PET_SKIN_MAX_NUM)
		{
			return false;
		}

		m_pet_skin_param.pet_skin_list[m_pet_skin_param.pet_skin_count++] = skin_id;

		this->SendPetSkinInfo();

		this->BuildPetSkinIdMap(true);

		for (int i = 0; i < ARRAY_LENGTH(m_grid_list); ++i)
		{
			PetParam * pet_param = this->__GetPetParam(i);
			if (pet_param == NULL)
			{
				continue;
			}

			if (skin_pet_cfg->can_wear_pet_list.find(pet_param->pet_id) != skin_pet_cfg->can_wear_pet_list.end())
			{
				this->ReCalcAttr(i);
				this->SyncPetDataPlatformBattle(i);
			}
		}
		num--;

		ROLE_LOG_QUICK6(LOG_TYPE_PET_SKIN_ACTIVE, m_role_module_mgr->GetRole(), skin_id, 0, __FUNCTION__, "");
	}
	if (num > 0)
	{
		ItemConfigData item;
		item.item_id = skin_pet_cfg->decompose_item_id;
		item.is_bind = true;
		item.num = skin_pet_cfg->decompose_item_num * num;

		bool is_true = false;
		if (m_role_module_mgr->GetKnapsack()->CheckForPut(item.item_id, (short)item.num))
		{
			if (m_role_module_mgr->GetKnapsack()->Put(item, is_notice ? PUT_REASON_DECOMPOSE_SURFACE : PUT_REASON_NO_NOTICE))
			{
				is_true = true;
			}
		}

		if (is_notice)
		{
			m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_PET_SKIN_CONVERT);
		}
		
		if (!is_true)
		{
			m_role_module_mgr->GetKnapsack()->SendMail(&item, 1, SNED_MAIL_TYPE_DEFAULT, true);
		}
	}

	return true;
}

void Pet::SendPetSkinInfo()
{
	Protocol::SCPetSkinInfo protocol;
	protocol.count = 0;
	for (int i = 0; i < m_pet_skin_param.pet_skin_count && i < ARRAY_LENGTH(m_pet_skin_param.pet_skin_list) && i < ARRAY_LENGTH(protocol.pet_skin_list); ++i)
	{
		protocol.pet_skin_list[i] = m_pet_skin_param.pet_skin_list[i];
		protocol.count += 1;
	}
	
	int send_len = sizeof(protocol) - (ARRAY_LENGTH(protocol.pet_skin_list) - protocol.count) * sizeof(protocol.pet_skin_list[0]);
	m_role_module_mgr->NetSend(&protocol, send_len);
}

void Pet::UsePetSkin(int pet_index, int skin_id)
{
	PetParam * pet_param = this->__GetPetParam(pet_index);
	if (pet_param == NULL)
	{
		return;
	}

	if (skin_id != 0)
	{
		if (!this->HasPetSkin(skin_id))
		{
			return;
		}

		const SkinPetCfg * skin_pet_cfg = LOGIC_CONFIG->GetSkinConfig()->GetSkinPetCfgBySkinId(skin_id);
		if (skin_pet_cfg == NULL)
		{
			return;
		}

		if (skin_pet_cfg->can_wear_pet_list.find(pet_param->pet_id) == skin_pet_cfg->can_wear_pet_list.end())
		{
			return;
		}
	}

	pet_param->skin_id = skin_id;

	if (this->GetFightIndex() == pet_index)this->SendFightPetInfoToTeamMember();

	this->ReCalcAttr(pet_index , true);
	if (this->IsInPlatfformBattle(m_grid_list[pet_index].item_unique_id))
	{
		this->SyncPetDataPlatformBattle(pet_index);
	}
	this->SetItemDirty(pet_index, true,Protocol::SCSendPetSingleInfo::PET_NOTIE_TYPE_PET_USE_SKIN);
	this->SendPetAttrInfo(pet_index);

	CommonDataParam & common_data = m_role_module_mgr->GetCommonData();
	if (common_data.new_pet_status_index[NEW_FOLLOW_INDEX] == pet_index)
	{
		this->SendPetFollowInfo();
	}	

	ROLE_LOG_QUICK6(LOG_TYPE_PET_SKIN_ACTIVE, m_role_module_mgr->GetRole(), skin_id, 0, __FUNCTION__, "");
}

int Pet::OnPetUnlockTalent(int pet_index, int talent_type, int inborn_grid)
{
	PetParam * pet_param = this->__GetPetParam(pet_index);
	if (NULL == pet_param)
	{
		return -__LINE__;
	}

	if (talent_type <= 0 || talent_type > ARRAY_LENGTH(pet_param->talent_list))
	{
		return -__LINE__;
	}
	if (inborn_grid <= 0 || inborn_grid > MAX_PET_TALENT_GROUP_SKILL_NUM)
	{
		return -__LINE__;
	}
	PetTalentInfo & talent_info = pet_param->talent_list[talent_type - 1];
	if (talent_info.inborn_level > 0)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_PET_TALENT_NOT_REPEAT_ACTIVED);
		return -__LINE__;
	}
	const PetCfg * pet_cfg = LOGIC_CONFIG->GetPetConfig()->GetPetCfg(pet_param->pet_id);
	if (NULL == pet_cfg)
	{
		return -__LINE__;
	}

	const PetInbornBaseCfg * base_cfg = LOGIC_CONFIG->GetPetInbornConfig()->GetBaseCfg(pet_cfg->inborn_type, talent_type);
	if (NULL == base_cfg)
	{
		return -__LINE__;
	}
	if (pet_param->str_level < base_cfg->unlock_group_str_level)
	{
		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_pet_unlock_talent_group_str_level_not_enough, base_cfg->unlock_group_str_level);
		if (length > 0)
		{
			World::GetInstWorld()->GetSceneManager()->SystemMsgPerson2(m_role_module_mgr->GetRole(), gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT));
		}
		return -__LINE__;
	}

	const PetInbornLevelCfg * level_cfg = LOGIC_CONFIG->GetPetInbornConfig()->GetLevelCfg(base_cfg->inborn_id_list[inborn_grid - 1], 1);
	if (NULL == level_cfg)
	{
		return -__LINE__;
	}
	if (!level_cfg->consume_list.empty())
	{
		int stuff_count = 0;
		ItemExtern::ItemConsumeConfig stuff_list[ItemNamespace::MAX_BATCH_CONSUME_ITEM_NUM];
		for (int i = 0; i < (int)level_cfg->consume_list.size() && stuff_count < ARRAY_LENGTH(stuff_list); ++i)
		{
			stuff_list[stuff_count].item_id = level_cfg->consume_list[i].item_id;
			stuff_list[stuff_count].num = level_cfg->consume_list[i].num;
			stuff_list[stuff_count].buyable = false;
			stuff_count += 1;
		}
		if (stuff_count > 0)
		{
			static ItemExtern::ItemConsumeList consume_list; consume_list.Reset();
			if (!ItemExtern::GetConsumeListByOrder(m_role_module_mgr->GetRole(), false, stuff_count, stuff_list, &consume_list, true))
			{
				m_role_module_mgr->NoticeNum(errornum::EN_STUFF_NOT_ENOUGH);
				return -__LINE__;
			}

			if (consume_list.count > 0)
			{
				if (!m_role_module_mgr->GetKnapsack()->ConsumeItemByIndexList(consume_list.count, consume_list.column_list, consume_list.index_list, consume_list.num_list, __FUNCTION__))
				{
					m_role_module_mgr->NoticeNum(errornum::EN_STUFF_NOT_ENOUGH);
					return -__LINE__;
				}
			}
		}
	}

	talent_info.inborn_grid = (unsigned char)inborn_grid;
	talent_info.inborn_level = 1;
	this->ReCalcAttr(pet_index);
	this->SetItemDirty(pet_index, true,Protocol::SCSendPetSingleInfo::PET_NOTIE_TYPE_UNLOCK_TALENT);

	gamelog::g_log_pet.printf(LL_INFO, "%s pet_unique_id[%lld] pet_index[%d] talent_type[%d] inborn_grid[%d]", __FUNCTION__, this->GetPetUniqueId(pet_index), 
		pet_index, talent_type, inborn_grid);

	return 0;
}

int Pet::OnPetTalentSkillLevelUp(int pet_index, int talent_type)
{
	PetParam * pet_param = this->__GetPetParam(pet_index);
	if (NULL == pet_param)
	{
		return -__LINE__;
	}
	if (talent_type <= 0 || talent_type > ARRAY_LENGTH(pet_param->talent_list))
	{
		return -__LINE__;
	}
	PetTalentInfo & talent_info = pet_param->talent_list[talent_type - 1];

	if (talent_info.inborn_level <= 0)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_PET_TALENT_NOT_YET_ACTIVE);
		return -__LINE__;
	}
	const PetCfg * pet_cfg = LOGIC_CONFIG->GetPetConfig()->GetPetCfg(pet_param->pet_id);
	if (NULL == pet_cfg)
	{
		return -__LINE__;
	}

	const PetInbornBaseCfg * base_cfg = LOGIC_CONFIG->GetPetInbornConfig()->GetBaseCfg(pet_cfg->inborn_type, talent_type);
	if (NULL == base_cfg)
	{
		return -__LINE__;
	}

	if (pet_param->str_level < base_cfg->unlock_group_str_level)
	{
		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_pet_unlock_talent_group_str_level_not_enough, base_cfg->unlock_group_str_level);
		if (length > 0)
		{
			World::GetInstWorld()->GetSceneManager()->SystemMsgPerson2(m_role_module_mgr->GetRole(), gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT));
		}
		return -__LINE__;
	}

	const PetInbornLevelCfg * level_cfg = LOGIC_CONFIG->GetPetInbornConfig()->GetLevelCfg(base_cfg->inborn_id_list[talent_info.inborn_grid - 1], talent_info.inborn_level + 1);
	if (NULL == level_cfg)
	{
		return -__LINE__;
	}
	if (!level_cfg->consume_list.empty())
	{
		int stuff_count = 0;
		ItemExtern::ItemConsumeConfig stuff_list[ItemNamespace::MAX_BATCH_CONSUME_ITEM_NUM];
		for (int i = 0; i < (int)level_cfg->consume_list.size() && stuff_count < ARRAY_LENGTH(stuff_list); ++i)
		{
			stuff_list[stuff_count].item_id = level_cfg->consume_list[i].item_id;
			stuff_list[stuff_count].num = level_cfg->consume_list[i].num;
			stuff_list[stuff_count].buyable = false;
			stuff_count += 1;
		}
		if (stuff_count > 0)
		{
			static ItemExtern::ItemConsumeList consume_list; consume_list.Reset();
			if (!ItemExtern::GetConsumeListByOrder(m_role_module_mgr->GetRole(), false, stuff_count, stuff_list, &consume_list, true))
			{
				m_role_module_mgr->NoticeNum(errornum::EN_STUFF_NOT_ENOUGH);
				return -__LINE__;
			}

			if (consume_list.count > 0)
			{
				if (!m_role_module_mgr->GetKnapsack()->ConsumeItemByIndexList(consume_list.count, consume_list.column_list, consume_list.index_list, consume_list.num_list, __FUNCTION__))
				{
					m_role_module_mgr->NoticeNum(errornum::EN_STUFF_NOT_ENOUGH);
					return -__LINE__;
				}
			}
		}
	}

	talent_info.inborn_level += 1;
	this->ReCalcAttr(pet_index);
	this->SetItemDirty(pet_index, true, Protocol::SCSendPetSingleInfo::PET_NOTIE_TYPE_TALENT_SKILL_LEVEL_UP);
	gamelog::g_log_pet.printf(LL_INFO, "%s pet_unique_id[%lld] pet_index[%d] talent_type[%d] inborn_level[%d]", __FUNCTION__, this->GetPetUniqueId(pet_index), 
		pet_index, talent_type, (int)talent_info.inborn_level);
	return 0;
}

int Pet::OnPetTalentSkillReplace(int pet_index, int talent_type, int new_inborn_grid)
{
	PetParam * pet_param = this->__GetPetParam(pet_index);
	if (NULL == pet_param)
	{
		return -__LINE__;
	}
	if (talent_type <= 0 || talent_type > ARRAY_LENGTH(pet_param->talent_list))
	{
		return -__LINE__;
	}
	PetTalentInfo & talent_info = pet_param->talent_list[talent_type - 1];
	if (new_inborn_grid <= 0 || new_inborn_grid > MAX_PET_TALENT_GROUP_SKILL_NUM || talent_info.inborn_grid == new_inborn_grid)
	{
		return -__LINE__;
	}
	if (!m_role_module_mgr->GetMoney()->UseGold(LOGIC_CONFIG->GetPetInbornConfig()->GetOtherCfg().replace_consume, __FUNCTION__))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_GOLD_NOT_ENOUGH);
		return -__LINE__;
	}

	talent_info.inborn_grid = (unsigned char)new_inborn_grid;
	this->ReCalcAttr(pet_index);
	this->SetItemDirty(pet_index, true, Protocol::SCSendPetSingleInfo::PET_NOTIE_TYPE_TALENT_SKILL_REPLACE);
	gamelog::g_log_pet.printf(LL_INFO, "%s pet_unique_id[%lld] pet_index[%d] talent_type[%d] new_inborn_grid[%d]", __FUNCTION__, this->GetPetUniqueId(pet_index), 
		pet_index, talent_type, (int)talent_info.inborn_grid);

	return 0;
}

int Pet::OnPetTalentReset(int pet_index)
{
	PetParam * pet_param = this->__GetPetParam(pet_index);
	if (NULL == pet_param)
	{
		return -__LINE__;
	}
	
	const PetCfg * pet_cfg = LOGIC_CONFIG->GetPetConfig()->GetPetCfg(pet_param->pet_id);
	if (NULL == pet_cfg)
	{
		return -__LINE__;
	}

	if(!pet_param->IsHasPetTalent())
	{
		return -__LINE__;
	}

	const PetInbornOtherCfg & other_cfg = LOGIC_CONFIG->GetPetInbornConfig()->GetOtherCfg();
	if (!m_role_module_mgr->GetMoney()->GoldMoreThan(other_cfg.return_consume))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_GOLD_NOT_ENOUGH);
		return -__LINE__;
	}
	if (!this->OnPetTalentReset(pet_param))
	{
		return -__LINE__;
	}

	if (!m_role_module_mgr->GetMoney()->UseGold(other_cfg.return_consume, __FUNCTION__))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_GOLD_NOT_ENOUGH);
		return -__LINE__;
	}

	this->ReCalcAttr(pet_index);
	this->SetItemDirty(pet_index, true, Protocol::SCSendPetSingleInfo::PET_NOTIE_TYPE_TALENT_RESET);
	gamelog::g_log_pet.printf(LL_INFO, "%s pet_unique_id[%lld] pet_index[%d]", __FUNCTION__, this->GetPetUniqueId(pet_index), pet_index);
	return 0;
}

bool Pet::OnPetTalentReset(PetParam * pet_param)
{
	if(NULL == pet_param) return false;;

	const PetCfg * pet_cfg = LOGIC_CONFIG->GetPetConfig()->GetPetCfg(pet_param->pet_id);
	if (NULL == pet_cfg)
	{
		return false;
	}

	std::vector<ItemConfigData> reset_return_item_list;
	for (int i = 0; i < ARRAY_LENGTH(pet_param->talent_list); ++i)
	{
		const PetTalentInfo & talent_info = pet_param->talent_list[i];
		if (talent_info.inborn_level <= 0) continue;

		const PetInbornBaseCfg * base_cfg = LOGIC_CONFIG->GetPetInbornConfig()->GetBaseCfg(pet_cfg->inborn_type, i + 1);
		if (NULL == base_cfg || talent_info.inborn_grid <= 0 || talent_info.inborn_grid > ARRAY_LENGTH(base_cfg->inborn_id_list)) continue;

		LOGIC_CONFIG->GetPetInbornConfig()->GetLevelConsumeByReset(base_cfg->inborn_id_list[talent_info.inborn_grid - 1], talent_info.inborn_level, reset_return_item_list);
	}
	std::vector<ItemConfigData> merge_item_list;
	m_role_module_mgr->GetKnapsack()->MergeItemList(reset_return_item_list, merge_item_list);
	if (!merge_item_list.empty())
	{
		m_role_module_mgr->GetKnapsack()->SendMail(merge_item_list, SEND_MAIL_TYPE_PET_TALENT_RESET_RETURN, false, pet_param->pet_id);
	}
	else
	{
		return false;
	}

	ArrayCallReset(pet_param->talent_list);
	return true;
}

void Pet::GetPetTalentBattleSpecialEffectList(const PetParam & pet_param, int max_other_sp_num, short * out_other_sp_num, int * out_other_sp_list) const
{
	if(NULL == out_other_sp_num || NULL == out_other_sp_list) return;

	std::vector<int> sp_list;
	this->GetPetTalentSpIdList(pet_param, sp_list);

	for (int i = 0; i < (int)sp_list.size(); i++)
	{
		const SpecialEffectCfg * spe_cfg = BattleSpecialEffectPool::GetInstance().GetSpecialEffectCfg(sp_list[i]);
		if (NULL == spe_cfg) continue;

		bool is_has_battle_effect = false;
		for (int j = 0; j < spe_cfg->effect_num && j < EFFECT_DETAIL_NUM; ++j)
		{
			const SpecialEffectKey & key = spe_cfg->effect_detail_list[j];
			if (key.effect_type == SPECIAL_EFFECT_TYPE_ADD_ATTR) continue;

			is_has_battle_effect = true;
			break;
		}

		if (!is_has_battle_effect)
		{
			continue;
		}

		if (*out_other_sp_num < max_other_sp_num)
		{
			out_other_sp_list[(*out_other_sp_num)++] = sp_list[i];
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

int Pet::GetPetTalentSpIdList(const PetParam & pet_param, std::vector<int>& sp_list) const
{
	const PetCfg * pet_cfg = LOGIC_CONFIG->GetPetConfig()->GetPetCfg(pet_param.pet_id);
	if (NULL == pet_cfg)
	{
		return 0;
	}
	
	int sp_capability = 0;
	for (int i = 0; i < ARRAY_LENGTH(pet_param.talent_list); ++i)
	{
		const PetTalentInfo & talent_info = pet_param.talent_list[i];
		if (talent_info.inborn_level <= 0) continue;

		const PetInbornBaseCfg * base_cfg = LOGIC_CONFIG->GetPetInbornConfig()->GetBaseCfg(pet_cfg->inborn_type, i + 1);
		if (NULL == base_cfg || talent_info.inborn_grid <= 0 || talent_info.inborn_grid > ARRAY_LENGTH(base_cfg->inborn_id_list)) continue;

		const PetInbornLevelCfg * level_cfg = LOGIC_CONFIG->GetPetInbornConfig()->GetLevelCfg(base_cfg->inborn_id_list[talent_info.inborn_grid - 1], talent_info.inborn_level);
		if (NULL == level_cfg) continue;

		const SpecialEffectCfg * spe_cfg = BattleSpecialEffectPool::GetInstance().GetSpecialEffectCfg(level_cfg->sp_id);
		if (NULL == spe_cfg) continue;

		sp_list.push_back(level_cfg->sp_id);
		sp_capability += level_cfg->inborn_score;
	}

	return sp_capability;
}

bool Pet::RetrievePet(int pet_id, long long unique_item_id)
{
	for (int i = 0; i < MAX_PET_GRID_NUM; ++i)
	{
		if (m_grid_list[i].Invalid()) continue;

		if (m_grid_list[i].item_id != pet_id
			|| NULL == m_grid_list[i].param
			|| m_grid_list[i].item_unique_id != unique_item_id)
		{
			continue;
		}

		PetParam pet_param = m_grid_list[i].param->pet_param;

		//宠物排行榜所需
		unsigned long long  pet_unique_id = m_grid_list[i].item_unique_id;

		this->DeletePet(i, __FUNCTION__);
		
		RankManager::Instance().SyncRemovePet(this->GetRoleModuleManager()->GetRole(), pet_unique_id);
		
		{
			// 邮件通知玩家
			static MailContentParam contentparam;
			contentparam.Reset();
			if (pet_param.pet_equipment_id > 0)
			{
				NetValueItemParam param;
				pet_param.GetPetEquip(&param.pet_equipment_param);

				static ItemDataWrapper pet_equip; 
				pet_equip.Reset();
				pet_equip.item_id = pet_param.pet_equipment_id;
				pet_equip.num = 1;
				pet_equip.has_param = 1;
				pet_equip.is_bind = 1;
				pet_equip.item_unique_id = pet_param.equipment_item_unique_id;
				param.GetData(pet_equip.param_data);

				contentparam.item_list[0] = pet_equip;
			}

			int subject_len = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_retrieve_mail_subject);
			int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_retrieve_pet_notice, pet_id, pet_param.pet_equipment_id > 0 ? gamestring::g_retrieve_pet_return_equip : "");
			if (subject_len > 0 && length > 0)
			{
				MailRoute::Instance().MailToUser(m_role_module_mgr->GetRole()->GetUserId(), MAIL_REASON_DEFAULT, contentparam);
			}
		}
		
		return true;
	}

	return false;
}

bool Pet::QueryPetInfo(int pet_id, std::string& out_msg)
{
	bool is_found = false;
	bool is_first = true;
	out_msg = "[";
	for (int i = 0; i < MAX_PET_GRID_NUM; ++i)
	{
		if (m_grid_list[i].Invalid()) continue;
		if (m_grid_list[i].item_id != pet_id || NULL == m_grid_list[i].param) continue;
	
		if (!is_first)
		{
			out_msg += ",";
		}
		is_first = false;

		is_found = true;

		PetParam& pet_param = m_grid_list[i].param->pet_param;

		char pet_info[1024] = { 0 };
		SNPRINTF(pet_info, sizeof(pet_info), "{\"id\":%d,\"uniqueid\":%lld,\"name\":\"%s\",\"lv\":%d,\"str_lv\":%d,\"growth\":[%d,%d,%d,%d,%d],\"passive\":[%d,%d,%d,%d,%d,%d,%d,%d],\"equipid\":%d,\"equip_skill\":[%d,%d]}",
			pet_id, m_grid_list[i].item_unique_id, pet_param.name, pet_param.level, pet_param.str_level,
			pet_param.growth[0], pet_param.growth[1], pet_param.growth[2], pet_param.growth[3],
			pet_param.growth[4], pet_param.passive_list[0], pet_param.passive_list[1],
			pet_param.passive_list[2], pet_param.passive_list[3], pet_param.passive_list[4],
			pet_param.passive_list[5], pet_param.passive_list[6], pet_param.passive_list[7],
			pet_param.passive_list[8], pet_param.pet_equipment_id, 
			pet_param.equipment_skill_list[0], pet_param.equipment_skill_list[1]);
		UNSTD_STATIC_CHECK(ARRAY_ITEM_COUNT(pet_param.passive_list) >= 9);
		UNSTD_STATIC_CHECK(ARRAY_ITEM_COUNT(pet_param.growth) == 5);

		out_msg += pet_info;
	}
	out_msg += "]";

	return is_found;
}

void Pet::CheckGodPrintAttr()
{
	if (m_common_param.god_print_check_flag == 0)
	{
		if (FunOpen::IsFunOpen(m_role_module_mgr->GetRole(), FUN_TYPE_NETHER_GOD_PRINT_SYSTEM))
		{
			for (int bag_idx = 0; bag_idx < NEW_MAX_PET_GOD_PRONT_BAG_NUM; bag_idx++)
			{
				int pet_idx = -1;
				if (this->CheckGodPrintByIdx(bag_idx, &pet_idx))
				{
					this->SetGodPrintDirtyMark(bag_idx, PUT_REASON_NO_NOTICE);
					this->ReCalcAttr(pet_idx);
				}
			}
		}
		m_common_param.god_print_check_flag = 1;
	}
}

bool Pet::CheckGodPrintByIdx(int bag_idx, int * pet_idx)
{
	if (bag_idx < 0 || bag_idx >= NEW_MAX_PET_GOD_PRONT_BAG_NUM)return false;
	if (NULL == m_new_god_print_bag[bag_idx])return false;
	if (m_new_god_print_bag[bag_idx]->Invalid())return false;
	GodPrintItem &god_print_item = *m_new_god_print_bag[bag_idx];
	if (god_print_item.level < 4)return false;
	int attr_adv_num = 0;//计算属性提升次数
	const PetGodPrintEquipMentBaseQualityCfg * base_cfg = LOGIC_CONFIG->GetPetGodPrintEquipMentConfig()->GetBaseCfg((int)god_print_item.m_posi + 1, (int)god_print_item.color);
	if (base_cfg == NULL)return false;
	int max_attr_num = 0;
	int min_attr_num = 0;
	if (!LOGIC_CONFIG->GetPetGodPrintEquipMentConfig()->GetMaxDeputyAttrNum(base_cfg->deputy_attribute_num, &max_attr_num, &min_attr_num))
	{
		return false;
	}

	for (int attr_idx = 0; attr_idx < god_print_item.attr_num && attr_idx < MAX_PET_GOD_PRONT_ATTR_NUM; attr_idx++)
	{
		const AttrInfo &attr_info = god_print_item.add_attr_list[attr_idx];
		const PetGodPrintdeputyAttrInfoCfg * deputy_base_attr_cfg = LOGIC_CONFIG->GetPetGodPrintEquipMentConfig()->GetDeputyAttrBaseInfo(base_cfg->deputy_attribute_att, attr_info.attr_type);
		const PetGodPrintIntensifyDetailCfg * deputy_attr_cfg = LOGIC_CONFIG->GetPetGodPrintEquipMentConfig()->GetIntensifyCfg(PetGodPrintIntensifyDetailCfg::PET_GOD_PRINT_TYPE_DEPUTY,
			god_print_item.color, attr_info.attr_type);
		if (deputy_attr_cfg != NULL && deputy_base_attr_cfg != NULL)
		{
			//除去基础加成
			//以最大值与最小值之和除以2来计算每次强化的增加数值

			int value = attr_info.attr_value - deputy_base_attr_cfg->att_min;
			int adv_value = (deputy_attr_cfg->intensify_att_max + deputy_attr_cfg->intensify_att_min) / 2.0;
			if (adv_value == 0)continue;
			attr_adv_num += (value / adv_value);
		}
	}

	if (god_print_item.attr_num < max_attr_num)
	{
		int attr_dis = god_print_item.attr_num - max_attr_num;//缺了多少条词条
		if (attr_dis > 0)
		{
			attr_adv_num += attr_dis;
		}
	}

	if (god_print_item.attr_num > min_attr_num)
	{
		int attr_dis = god_print_item.attr_num - min_attr_num;//新增了多少条词条
		if (attr_dis > 0)
		{
			attr_adv_num += attr_dis;
		}
	}

	int real_attr_adv_num = 0;
	for (int level_idx = 1; level_idx <= god_print_item.level; level_idx++)
	{
		const PetGodPrintIntensifyInfoCfg * inten_cfg = LOGIC_CONFIG->GetPetGodPrintEquipMentConfig()->GetIntensifyInfoCfg((int)god_print_item.color, level_idx);
		if (inten_cfg != NULL && inten_cfg->deputy_attribute_up == 1)
		{
			real_attr_adv_num += 1;
		}
	}

	if (real_attr_adv_num <= attr_adv_num)return false;
	
	//以下都是有问题修复
	int attr_dis = god_print_item.attr_num - max_attr_num;//缺了多少条词条
	for (int attr_idx = 0; attr_idx < god_print_item.attr_num && attr_idx < MAX_PET_GOD_PRONT_ATTR_NUM; attr_idx++)//副属性重置
	{
		AttrInfo &attr_info = god_print_item.add_attr_list[attr_idx];
		const PetGodPrintdeputyAttrInfoCfg * deputy_base_attr_cfg = LOGIC_CONFIG->GetPetGodPrintEquipMentConfig()->GetDeputyAttrBaseInfo(base_cfg->deputy_attribute_att, attr_info.attr_type);
		if (NULL != deputy_base_attr_cfg)
		{
			attr_info.attr_value = rand() % (deputy_base_attr_cfg->att_max - deputy_base_attr_cfg->att_min + 1);
			if ((deputy_base_attr_cfg->att_max - deputy_base_attr_cfg->att_min + 1) > 0)
			{
				attr_info.attr_value = (rand() % (deputy_base_attr_cfg->att_max - deputy_base_attr_cfg->att_min + 1)) + deputy_base_attr_cfg->att_min;
			}
			else
			{
				attr_info.attr_value = deputy_base_attr_cfg->att_min;
			}
		}
	}

	for (int begain_level = 1; begain_level <= god_print_item.level; begain_level++)
	{
		const PetGodPrintIntensifyInfoCfg * inten_cfg = LOGIC_CONFIG->GetPetGodPrintEquipMentConfig()->GetIntensifyInfoCfg((int)god_print_item.color, begain_level);
		if (inten_cfg != NULL && inten_cfg->deputy_attribute_up == 1)
		{
			if (attr_dis > 0)
			{
				attr_dis -= 1;//先把多余的减去
				continue;
			}

			if (god_print_item.attr_num < MAX_PET_GOD_PRONT_ATTR_NUM)//不满4条则增加条数，满则增强词条
			{
				int attr_type[MAX_PET_GOD_PRONT_ATTR_NUM];
				memset(attr_type, -1, sizeof(attr_type));

				for (int i = 0; i < MAX_PET_GOD_PRONT_ATTR_NUM && i < god_print_item.attr_num; i++)
				{
					attr_type[i] = god_print_item.add_attr_list[i].attr_type;
				}

				const PetGodPrintdeputyAttrInfoCfg * deputy_cfg = LOGIC_CONFIG->GetPetGodPrintEquipMentConfig()->RandDeputyAttrBaseInfo(base_cfg->deputy_attribute_att, attr_type);
				if (deputy_cfg != NULL)
				{
					god_print_item.add_attr_list[god_print_item.attr_num].attr_type = deputy_cfg->att_type;
					if ((deputy_cfg->att_max - deputy_cfg->att_min + 1) != 0)
					{
						int rand_value = (rand() % (deputy_cfg->att_max - deputy_cfg->att_min + 1)) + deputy_cfg->att_min;
						god_print_item.add_attr_list[god_print_item.attr_num].attr_value += rand_value;
					}
					else
					{
						god_print_item.add_attr_list[god_print_item.attr_num].attr_value += deputy_cfg->att_min;
					}
					god_print_item.attr_num++;
				}
			}
			else
			{
				int attr_up_idx = 0;
				if (god_print_item.attr_num > 0)
				{
					attr_up_idx = rand() % god_print_item.attr_num;
				}
				if (attr_up_idx < 0 || attr_up_idx >= MAX_PET_GOD_PRONT_ATTR_NUM)
				{
					continue;
				}
				const PetGodPrintIntensifyDetailCfg * deputy_attr_cfg = LOGIC_CONFIG->GetPetGodPrintEquipMentConfig()->GetIntensifyCfg(PetGodPrintIntensifyDetailCfg::PET_GOD_PRINT_TYPE_DEPUTY,
					god_print_item.color, god_print_item.add_attr_list[attr_up_idx].attr_type);
				if (deputy_attr_cfg != NULL)
				{
					if ((deputy_attr_cfg->intensify_att_max - deputy_attr_cfg->intensify_att_min + 1) != 0)
					{
						int rand_value = (rand() % (deputy_attr_cfg->intensify_att_max - deputy_attr_cfg->intensify_att_min + 1)) + deputy_attr_cfg->intensify_att_min;
						god_print_item.add_attr_list[attr_up_idx].attr_value += rand_value;
					}
					else
					{
						god_print_item.add_attr_list[attr_up_idx].attr_value += deputy_attr_cfg->intensify_att_min;
					}
				}
			}
		}
	}

	if (NULL != pet_idx)*pet_idx = god_print_item.m_wear_pet_idx;
	return true;
}

void Pet::DeletePetByList(std::set<int> pet_index_set, const char * reason)
{
	for (std::set<int>::iterator iter = pet_index_set.begin(); iter != pet_index_set.end(); ++iter)
	{
		int curr = *iter;
		if (curr < ItemNamespace::PET_STORAGE_INDEX_BEGIN)
		{
			int index = curr;
			this->DeletePet(index, reason);
		}
		else
		{
			int index = curr - ItemNamespace::PET_STORAGE_INDEX_BEGIN;
			this->DeleteStoragePet(index, reason);
		}
	}
}

void Pet::OnUsePetAttrPellet(int pellet_id, int num, int pet_index)
{
	if (this->PetInvalid(pet_index)) return;

	PetParam & pet_param = m_grid_list[pet_index].param->pet_param;
	const PetCfg *cfg = LOGIC_CONFIG->GetPetConfig()->GetPetCfg(pet_param.pet_id);
	if (NULL == cfg) return;

	const RoleAttrPelletAttrListCfg * pellet_cfg  = LOGIC_CONFIG->GetRoleAttrPelletConfig()->GetPelletListCfg(pellet_id, cfg->main_attribute_id);
	if (NULL == pellet_cfg || pellet_cfg->role_attr_pellet_type != ROLE_ATTR_PELLET_TYPE_PET)
	{
		return;
	}
	UNSTD_STATIC_CHECK(ATTR_PELLET_TYPE_MAX <= MAX_ATTR_PELLET_TYPE_NUM);
	int attr_pellet_type = GetPelletTypeByAttrType(pellet_cfg->attr_type);
	if (attr_pellet_type < 0 || attr_pellet_type >= ATTR_PELLET_TYPE_MAX)
	{
		return;
	}

	int add_times =  LOGIC_CONFIG->GetPetConfig()->GetFeiShengShuXingDanUseTimes(pet_param.fei_sheng_times);
	if (num + pet_param.use_attr_pellet_list[attr_pellet_type] > LOGIC_CONFIG->GetRoleAttrPelletConfig()->GetOtherCfg().use_limit + add_times)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_ROLE_ATTR_PELLET_USE_LIMIT);
		return;
	}
	const ItemBase * item_base = ITEMPOOL->GetItem(pellet_id);
	if (NULL == item_base || item_base->GetItemSubType() != ItemNamespace::ITEM_SUB_TYPE_ATTR_PELLET)
	{
		return;
	}

	if (!m_role_module_mgr->GetKnapsack()->ConsumeItem(pellet_id, num, __FUNCTION__))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_ITEM_AMOUNT_NOT_ENOUGH);
		return;
	}
	pet_param.use_attr_pellet_list[attr_pellet_type] += num;
	this->ReCalcAttr(pet_index);
	this->SetItemDirty(pet_index, true, Protocol::SCSendPetSingleInfo::PET_NOTIE_TYPE_ATTR_PELLET);
	if (this->IsInPlatfformBattle(m_grid_list[pet_index].item_unique_id))
	{
		this->SyncPetDataPlatformBattle(pet_index);
	}
	gamelog::g_log_role_attr_pellet.printf(LL_INFO, "%s user[%d, %s] use_pellet_id[%d] use_num[%d] attr_pellet_type[%d] cur_num[%d] role_pellet_type[%d] pet[%d]", __FUNCTION__, m_role_module_mgr->GetUid(),
		m_role_module_mgr->GetRoleName(), pellet_id, num, attr_pellet_type, pet_param.use_attr_pellet_list[attr_pellet_type], ROLE_ATTR_PELLET_TYPE_PET, pet_index);
}

void Pet::OnAttrPelletRet(int pet_index)
{
	if (this->PetInvalid(pet_index)) return;

	PetParam& pet_param = m_grid_list[pet_index].param->pet_param;
	const PetCfg * cfg = LOGIC_CONFIG->GetPetConfig()->GetPetCfg(pet_param.pet_id);
	if (NULL == cfg) return;

	std::vector<ItemConfigData> ret_item_list;
	for (int i = 0; i < ATTR_PELLET_TYPE_MAX && i < ARRAY_LENGTH(pet_param.use_attr_pellet_list); i++)
	{
		if (pet_param.use_attr_pellet_list[i] <= 0) continue;

		int attr_type = GetAttrTypeByPelletType(i);
		if (attr_type < 0 || attr_type >= BATTLE_ATTR_MAX) continue;

		const RoleAttrPelletAttrListCfg * attr_pellet_cfg = LOGIC_CONFIG->GetRoleAttrPelletConfig()->GetAttrPelletCfg(ROLE_ATTR_PELLET_TYPE_PET, cfg->main_attribute_id, attr_type);
		if (NULL != attr_pellet_cfg)
		{
			ret_item_list.push_back(ItemConfigData(attr_pellet_cfg->attr_pellet_id, true, pet_param.use_attr_pellet_list[i]));
		}
	}

	memset(pet_param.use_attr_pellet_list, 0, sizeof(pet_param.use_attr_pellet_list));

	if (!ret_item_list.empty())
	{
		if (!m_role_module_mgr->GetKnapsack()->PutVec(ret_item_list, PUT_REASON_ATTR_PELLET_RET))
		{
			m_role_module_mgr->GetKnapsack()->SendMail(&ret_item_list[0], (int)ret_item_list.size(), SNED_MAIL_TYPE_DEFAULT, true);
		}
	}

}

void Pet::OnPetBenedictionReset(int pet_index)
{
	if (this->PetInvalid(pet_index)) return;

	std::map<long long, std::set<int> >::iterator it = m_pet_benediction_map.find(m_grid_list[pet_index].item_unique_id);
	if (it == m_pet_benediction_map.end() || it->second.empty())
	{
		m_pet_benediction_map.erase(m_grid_list[pet_index].item_unique_id);
		return;
	}
	for (std::set<int>::iterator benediction_it = it->second.begin(); benediction_it != it->second.end(); ++benediction_it)
	{
		if(*benediction_it < 0 || *benediction_it >= ItemNamespace::MAX_PET_GRID_NUM) continue;

		m_common_param.benediction_pet_index_list[*benediction_it] = -1;
		m_pet_benediction_vice_set.erase(m_grid_list[*benediction_it].item_unique_id);
		this->SendPetBenedictionSingleInfo(*benediction_it);
	}
	m_pet_benediction_map.erase(m_grid_list[pet_index].item_unique_id);
}

bool Pet::IsPetBenedictionVice(int pet_index)
{
	if(this->PetInvalid(pet_index)) return true;

	return -1 != m_common_param.benediction_pet_index_list[pet_index];
}

bool Pet::IsPetBenedictionVice(long long item_unique_id)
{
	std::set<long long>::iterator it = m_pet_benediction_vice_set.find(item_unique_id);
	if(it == m_pet_benediction_vice_set.end()) return false;

	return true;
}

void Pet::OnCalcPetBenedictioncAttr(long long item_unique_id, AttributeList & base_add, bool is_huashen)const
{
	std::map<long long, std::set<int> >::const_iterator pet_benediction_it = m_pet_benediction_map.find(item_unique_id);
	if (pet_benediction_it != m_pet_benediction_map.end() && !pet_benediction_it->second.empty())
	{
		for (std::set<int>::const_iterator benediction_it = pet_benediction_it->second.begin(); benediction_it != pet_benediction_it->second.end(); ++benediction_it)
		{
			if(this->PetInvalid(*benediction_it)) continue;
			PetParam & pet_param = m_grid_list[*benediction_it].param->pet_param;
			const PetCfg * pet_cfg = LOGIC_CONFIG->GetPetConfig()->GetPetCfg(pet_param.pet_id);
			if(NULL == pet_cfg) continue;

			const AttributeList * benediction_attr_list = NULL;
			if (is_huashen)
			{
				benediction_attr_list = m_role_module_mgr->GetHuaShenManager()->GetPetAttrList(m_grid_list[*benediction_it].item_unique_id);
			}
			else
			{
				benediction_attr_list = &m_pet_base_attr[*benediction_it];
			}
			if(NULL == benediction_attr_list) continue;

			for (int i = BATTLE_ATTR_NORMAL_VALUE_BEGIN; i <= BATTLE_ATTR_PANEL_END && i < ARRAY_LENGTH(benediction_attr_list->m_attrs); ++i)		//只继承那几个面板属性
			{
				base_add.m_attrs[i] += benediction_attr_list->m_attrs[i] * pet_cfg->bendiction_attr_add_precent / BATTLE_PERCENT_NUM;
			}
		}
	}
}

int Pet::GetPetBenedictionSpId(const PetParam & pet, long long item_unique_id, ARG_OUT int * sp_capability)const
{
	const PetCfg * pet_cfg = LOGIC_CONFIG->GetPetConfig()->GetPetCfg(pet.pet_id);
	if(NULL == pet_cfg || pet_cfg->benediction_id <= 0) return 0;

	std::map<long long, std::set<int> >::const_iterator pet_benediction_it = m_pet_benediction_map.find(item_unique_id);
	if (pet_benediction_it == m_pet_benediction_map.end() || pet_benediction_it->second.empty()) return 0;

	PetBenedictionSimpleCfg simple_cfg;
	std::set<int> pet_index_list;
	for (std::set<int>::const_iterator benediction_it = pet_benediction_it->second.begin(); benediction_it != pet_benediction_it->second.end(); ++benediction_it)
	{
		if (this->PetInvalid(*benediction_it)) continue;

		PetParam & pet_param = m_grid_list[*benediction_it].param->pet_param;
		const PetCfg * active_pet_cfg = LOGIC_CONFIG->GetPetConfig()->GetPetCfg(pet_param.pet_id);
		if (NULL == active_pet_cfg) continue;

		simple_cfg.active_set.insert(active_pet_cfg->prototype_pet_id);
		pet_index_list.insert(*benediction_it);
	}

	if (simple_cfg.active_set.empty() || !LOGIC_CONFIG->GetPetConfig()->IsMeetActiveCond(pet_cfg->benediction_id, simple_cfg))
	{
		return 0;
	}
	
	int min_capbility = MAX_INT;
	for (std::set<int>::const_iterator pet_index_it = pet_index_list.begin(); pet_index_it != pet_index_list.end(); ++pet_index_it)	//此处pet_index安全性已经在上面判断
	{
		min_capbility = GetMin(this->GetPetCapability_1(*pet_index_it), min_capbility);
	}
	const PetBenedictionDetailCfg * detaild_cfg = LOGIC_CONFIG->GetPetConfig()->GetBenedictionDetailCfg(pet_cfg->benediction_id, min_capbility);
	if (NULL == detaild_cfg)
	{
		return 0;
	}
	if (NULL != sp_capability)
	{
		*sp_capability = detaild_cfg->sp_score;
	}

	return detaild_cfg->benediction_sp_id;
}

void Pet::OnMainReCalcAttr(int main_pet_index)
{
	if(this->PetInvalid(main_pet_index)) return;
	if (this->GetAttributeList(main_pet_index, m_pet_base_attr[main_pet_index])) return;

	this->OnPetAttrChange(main_pet_index, false, false);
	this->SendSingleInfo(main_pet_index, Protocol::SCSendPetSingleInfo::PET_NOTIE_TYPE_BENEDICTION);
}

void Pet::OnFinishFeiShengTask(int task_id)
{	
	const TaskInfomaConfig* task_cfg = TASKPOOL->GetTaskCfgByID(task_id);
	if (!task_cfg)
	{
		return;
	}

	if (task_cfg->task_type != TASK_TYPE_PET_FEI_SHENG_TASK)
	{
		return;
	}

	int pet_index = -1;
	PetParam * pet=this->GetFeiShengTaskPet(&pet_index);
	if (!pet)
	{
		return;
	}

	//task_param2 完成这个任务后的阶段
	if (task_cfg->task_param2 <= pet->fei_sheng_task_jie_duan)
	{
		m_role_module_mgr->GetTaskManager()->AcceptTask(task_cfg->nex_task_id);
		return;
	}

	m_common_param.fei_sheng_today_is_finish = 1;
	pet->fei_sheng_task_jie_duan = task_cfg->task_param2;	
	
	this->ReCalcAttr(pet_index);
	this->SetItemDirty(pet_index, true, Protocol::SCSendPetSingleInfo::PET_NOTIE_TYPE_FINISH_FEI_SHENG_TASK);
	this->SendFeiShengInfo();	


	gamelog::g_log_pet.printf(LL_INFO, "%s line:%d |%s %s  task_id:%d fei_sheng_times:%d fei_sheng_task_jie_duan:%d", __FUNCTION__, __LINE__,
		m_role_module_mgr->GetRoleInfoString().c_str(), pet->GetPetBaseInfoString(this->GetPetUniqueId(pet_index)).c_str(),
		m_common_param.fei_sheng_task_id, pet->fei_sheng_times, pet->fei_sheng_task_jie_duan);

	m_role_module_mgr->GetTaskManager()->ClearPetFeiShengTask();
}

bool Pet::CanAcceptFeiShengTask(int task_id)
{
	const TaskInfomaConfig * task_cfg = TASKPOOL->GetTaskCfgByID(task_id);
	if (NULL == task_cfg) return true;

	if (TASK_TYPE_PET_FEI_SHENG_TASK != task_cfg->task_type)
	{
		return true;
	}

	if (m_role_module_mgr->GetTaskManager()->CheckTaskByType(TASK_TYPE_PET_FEI_SHENG_TASK, TASK_CHECK_OPERA_TYPE_HAS_TASK, 0))
	{
		return false;
	}

	return true;
}

bool Pet::CanCommitFeiShengTask(int task_id, bool notify)
{
	const TaskInfomaConfig * task_cfg = TASKPOOL->GetTaskCfgByID(task_id);
	if (NULL == task_cfg) return true;

	if (TASK_TYPE_PET_FEI_SHENG_TASK != task_cfg->task_type)
	{
		return true;
	}

	PetParam * pet = this->GetFeiShengTaskPet();
	if (!pet)
	{
		if (notify)
		{
			m_role_module_mgr->NoticeNum(errornum::EN_NOT_IN_PET_LIST);
		}
		return false;
	}

	if (pet->fei_sheng_task_jie_duan + 1 != task_cfg->task_param1)
	{
		if (notify)
		{
			m_role_module_mgr->NoticeNum(errornum::EN_PET_TASK_NOT_MATCH);
		}
		return false;
	}

	return true;
}

int Pet::ReqStartFeiShengTask(int pet_index)
{
	if (this->PetInvalid(pet_index)) return -__LINE__;

	ItemGridData & grid_data = m_grid_list[pet_index];
	PetParam & pet = grid_data.param->pet_param;

	if (m_common_param.fei_sheng_task_id != 0)
	{
		return -__LINE__;
	}

	int first_task_id = LOGIC_CONFIG->GetPetConfig()->GetPetFeiShengFirstTask(pet.pet_id, pet.fei_sheng_task_jie_duan);
	if (first_task_id == 0)
	{
		return -__LINE__;
	}

	if (pet.fei_sheng_task_jie_duan >= (pet.fei_sheng_times + 1) * PET_JIE_DUAN_NUM_PER_FEI_SHENG_TIMES)
	{
		return -__LINE__;
	}

	std::vector<int>  task_id_vec = m_role_module_mgr->GetTaskManager()->GetPetFeiShengTask();
	if (!task_id_vec.empty())
	{
		return -__LINE__;
	}

	m_role_module_mgr->GetTaskManager()->ClearPetFeiShengTask();

	m_common_param.fei_sheng_pet_uuid = grid_data.item_unique_id;  //先赋值，OnAcceptTask 里会用到
	m_common_param.fei_sheng_task_id = first_task_id; 
	m_common_param.fei_sheng_task_jie_duan = pet.fei_sheng_task_jie_duan;
	if (!m_role_module_mgr->GetTaskManager()->OnAcceptTask(first_task_id))
	{
		m_common_param.fei_sheng_task_id = 0;
		m_common_param.fei_sheng_pet_uuid = 0;
		m_common_param.fei_sheng_task_jie_duan = 0;
		return -__LINE__;
	}
	this->SendFeiShengInfo();

	gamelog::g_log_pet.printf(LL_INFO, "%s line:%d |%s %s  task_id:%d fei_sheng_times:%d fei_sheng_task_jie_duan:%d", __FUNCTION__, __LINE__, 
		m_role_module_mgr->GetRoleInfoString().c_str(), pet.GetPetBaseInfoString(grid_data.item_unique_id).c_str(),
		m_common_param.fei_sheng_task_id, pet.fei_sheng_times, pet.fei_sheng_task_jie_duan);

	return 0;
}

int Pet::ReqFeiSheng(int pet_index, int select_skill_index)
{
	if (this->PetInvalid(pet_index)) return -__LINE__;

	ItemGridData & grid_data = m_grid_list[pet_index];
	PetParam & pet = grid_data.param->pet_param;

	const PetOtherCfg & other_cfg = LOGIC_CONFIG->GetPetConfig()->GetOtherCfgRef();
	if (pet.fei_sheng_task_jie_duan < (pet.fei_sheng_times + 1) * PET_JIE_DUAN_NUM_PER_FEI_SHENG_TIMES)
	{
		return -__LINE__;
	}

	const PetFeiShengConditionsCfg * condition_cfg = LOGIC_CONFIG->GetPetConfig()->GetPetFeiShengConditionsCfg(pet.fei_sheng_times + 1);
	if (condition_cfg == NULL)
	{
		return -__LINE__;
	}

	for (int i = 0; i < (int)condition_cfg->conditions.size(); ++i)
	{
		const PetFeiShengConditionsItemCfg & curr = condition_cfg->conditions[i];
		switch (curr.conditions_type)
		{
			case 1:
				{
					if (pet.level < curr.conditions_value)
					{
						m_role_module_mgr->NoticeNum(errornum::EN_PET_LEVEL_DEFICIENCY);
						return -__LINE__;
					}
				}
				break;
			case 2:
				{
					int t = m_role_module_mgr->GetRoleFeiShengTimes();
					if (t < curr.conditions_value)
					{
						m_role_module_mgr->NoticeNum(errornum::EN_NEED_ROLE_FEI_SHENG);
						return -__LINE__;
					}
				}
				break;
			case 3:
				{
					if (pet.str_level < curr.conditions_value)
					{
						m_role_module_mgr->NoticeNum(errornum::EN_NEED_PET_STR_LEVEL);
						return -__LINE__;
					}
				}
				break;
			case 4:
				{
					if (this->GetPetCapability_1(pet_index) < curr.conditions_value)
					{
						m_role_module_mgr->NoticeNum(errornum::EN_NEED_PET_CAPBILITY_LEVEL);
						return -__LINE__;
					}
				}
				break;
			default:
				return -__LINE__;
		}
	}

	if (pet.fei_sheng_times == 0)
	{
		if (select_skill_index < 0 || select_skill_index >= ARRAY_LENGTH(other_cfg.fei_sheng_skill_id))
		{
			return -__LINE__;
		}

		int skill_id = other_cfg.fei_sheng_skill_id[select_skill_index];
		pet.fei_sheng_select_skill_id = skill_id;
	}
	
	pet.fei_sheng_times += 1;
	this->ReCalcAttr(pet_index);
	this->SetItemDirty(pet_index, true, Protocol::SCSendPetSingleInfo::PET_NOTIE_TYPE_FEI_SHENG);

	this->CheckPetYiWenLuRewardByIndex(pet_index);

	return 0;
}

int Pet::ReqFeiShengSkillMod(int pet_index, int select_skill_index)
{
	if (this->PetInvalid(pet_index)) return -__LINE__;

	ItemGridData & grid_data = m_grid_list[pet_index];
	PetParam & pet = grid_data.param->pet_param;

	if (pet.fei_sheng_times == 0)
	{
		return -__LINE__;
	}

	const PetOtherCfg & other_cfg = LOGIC_CONFIG->GetPetConfig()->GetOtherCfgRef();
	if (select_skill_index < 0 || select_skill_index >= ARRAY_LENGTH(other_cfg.fei_sheng_skill_id))
	{
		return -__LINE__;
	}

	if (!m_role_module_mgr->GetMoney()->UseCoinBind(other_cfg.fei_sheng_skill_change_need_coin, __FUNCTION__))
	{
		return -__LINE__;
	}

	int skill_id = other_cfg.fei_sheng_skill_id[select_skill_index];
	pet.fei_sheng_select_skill_id = skill_id;
	this->ReCalcAttr(pet_index);
	this->SetItemDirty(pet_index, true, Protocol::SCSendPetSingleInfo::PET_NOTIE_TYPE_FEI_SHENG_SKILL_MOD);
	return 0;
}

int Pet::SendFeiShengInfo()
{
	Protocol::SCPetFeiShengTaskInfo protocol;
	protocol.pet_uuid = m_common_param.fei_sheng_pet_uuid;
	protocol.today_is_finish = m_common_param.fei_sheng_today_is_finish;
	protocol.task_id = m_common_param.fei_sheng_task_id;

	m_role_module_mgr->NetSend(&protocol, sizeof(protocol));

	return 0;
}

int Pet::GetFeiShengTaskPetIndex()
{
	if (!this->PetInvalid(m_pet_fei_sheng_index))
	{
		if (m_grid_list[m_pet_fei_sheng_index].item_unique_id == m_common_param.fei_sheng_pet_uuid)
		{
			return m_pet_fei_sheng_index;
		}
	}

	m_pet_fei_sheng_index = -1;
	for (int i = 0; i < ARRAY_LENGTH(m_grid_list); ++i)
	{
		if (this->PetInvalid(i)) continue;

		ItemGridData & pet_grid = m_grid_list[i];
		if (pet_grid.item_unique_id == m_common_param.fei_sheng_pet_uuid)
		{
			m_pet_fei_sheng_index = i;
			break;
		}
	}

	return m_pet_fei_sheng_index;
}

int Pet::FeiShengSkillUpLevel(int pet_index)
{
	if (this->PetInvalid(pet_index)) return -__LINE__;
	PetParam & pet = m_grid_list[pet_index].param->pet_param;
	const PetFeiShengSkillUpgradeCfg * fei_sheng_skill_cfg = LOGIC_CONFIG->GetPetConfig()->GetPetFeiShengSkillUpgradeCfg(pet.fei_sheng_select_skill_id, pet.fei_sheng_select_skill_level);
	const PetFeiShengSkillUpgradeCfg * next_fei_sheng_skill_cfg = LOGIC_CONFIG->GetPetConfig()->GetPetFeiShengSkillUpgradeCfg(pet.fei_sheng_select_skill_id, pet.fei_sheng_select_skill_level + 1);
	if (!fei_sheng_skill_cfg || !next_fei_sheng_skill_cfg)
	{
		return -__LINE__;
	}

	if (!m_role_module_mgr->GetKnapsack()->ConsumeItem(fei_sheng_skill_cfg->consume_item_id, fei_sheng_skill_cfg->consume_item_num, __FUNCTION__))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_ITEM_AMOUNT_NOT_ENOUGH);
		return -__LINE__;
	}

	pet.fei_sheng_select_skill_level += 1;
	this->ReCalcAttr(pet_index);
	this->SetItemDirty(pet_index,true, Protocol::SCSendPetSingleInfo::PET_NOTIE_TYPE_UPGRADE_FEI_SHENG_SKILL);

	return 0;
}

PetParam * Pet::GetFeiShengTaskPet(int ARG_OUT * get_index)
{
	int index = this->GetFeiShengTaskPetIndex();
	if (get_index)
	{
		*get_index = index;
	}
	return this->__GetPetParam(index);
}

void Pet::ResetPetFeiShengOnDayChange()
{
	if (!m_role_module_mgr->GetTaskManager()->HasPetFeiShengTask())
	{
		m_common_param.fei_sheng_task_id = 0;
		m_common_param.fei_sheng_pet_uuid = 0;		
	}	

	m_common_param.fei_sheng_today_is_finish = 0;
	this->SendFeiShengInfo();
}

void Pet::CheckPetFeiShengTask()
{	

}

void Pet::GmFeiSheng(int op_type, int p1,int p2)
{
	if (op_type == 0)
	{
		m_role_module_mgr->GetTaskManager()->ClearPetFeiShengTask();
		m_common_param.fei_sheng_task_id = 0;
		m_common_param.fei_sheng_today_is_finish = 0;
		m_common_param.fei_sheng_pet_uuid = 0;
		this->SendFeiShengInfo();		
	}

	if (op_type == 2)
	{
		for (int i = 0; i < ItemNamespace::MAX_PET_GRID_NUM; ++i)
		{
			PetParam * param = this->__GetPetParam(i);
			if (NULL == param) continue;

			param->fei_sheng_select_skill_id = 0;
			param->fei_sheng_task_jie_duan = 0;
			param->fei_sheng_times = 0;
			this->SetItemDirty(i, false);
		}

		this->SendAllInfo();
	}	

	if (op_type == 3)
	{
		this->ReqStartFeiShengTask(p1);
	}

	if (op_type == 4)
	{
		m_common_param.gm_add_capacity = p1;
		this->SendAllInfo();
	}

	if (op_type == 5)
	{
		for (int i = 0; i < ARRAY_LENGTH(m_grid_list); ++i)
		{
			PetParam * pet_param = this->__GetPetParam(i);
			if (!pet_param)
			{
				continue;
			}

			pet_param->level = 115;
			pet_param->str_level = 10;
		}

		this->SendAllInfo();
	}

	if (op_type == 6)
	{
		this->ReqFeiShengSkillMod(p1, p2);
	}

	if (op_type == 7)
	{
		this->OnUpgradePetSkill(p1, p2, false);
	}

	if (op_type == 8)
	{
		this->OnUpgradePetSkill(p1, p2, true);
	}

	for (int i = 0; i < ARRAY_LENGTH(m_grid_list); ++i)
	{
		this->SendPetAttrInfo(i);
	}
}

void Pet::CheckAllPetYiWenLuReward()
{
	this->ResetPetFeiShengRewardFlag();

	for (int i = 0; i < ARRAY_LENGTH(m_grid_list); ++i)
	{
		this->CheckPetYiWenLuRewardByIndex(i, false);
	}
	
	this->SendYiWenLuRewardInfo();
}

int Pet::FetchYiWenLuReward(int mutant_id, int reward_type)
{
	PetYiWenLuRewardParam * fei_sheng_reward = this->GetPetYiWenLuRewardParam(mutant_id);
	if (!fei_sheng_reward)
	{
		return -__LINE__;
	}

	if (reward_type >= BitCount(fei_sheng_reward->fetch_flag))
	{
		return -__LINE__;
	}

	if (!IsSetBit(fei_sheng_reward->finish_flag, reward_type) || IsSetBit(fei_sheng_reward->fetch_flag, reward_type))
	{
		return -__LINE__;
	}

	const PetYiWenLuRewardCfg * cfg = LOGIC_CONFIG->GetPetConfig()->GetPetYiWenLuRewardCfg(mutant_id, reward_type);
	if (!cfg)
	{
		return -__LINE__;
	}

	if (!m_role_module_mgr->GetKnapsack()->PutVec(cfg->rewards, PUT_REASON_PET_FEI_SHENG_REWARDS))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_HAS_NO_SPACING);
		return -__LINE__;
	}

	SetBit(fei_sheng_reward->fetch_flag, reward_type);
	this->SendYiWenLuRewardInfo();

	return 0;
}

void Pet::SendYiWenLuRewardInfo()
{
	Protocol::SCPetFeiShengRewardInfo protocol;
	protocol.count = 0;
	for (int i = 0; i < ARRAY_LENGTH(m_common_param.yi_wen_lu_rewards) && protocol.count<ARRAY_LENGTH(protocol.infos); ++i)
	{
		if (m_common_param.yi_wen_lu_rewards[i].mutant_id != 0)
		{
			protocol.infos[protocol.count++] = m_common_param.yi_wen_lu_rewards[i];
		}
	}
	
	unsigned int send_len = sizeof(protocol) - (ARRAY_LENGTH(protocol.infos) - protocol.count) * sizeof(protocol.infos[0]);
	assert(send_len <= sizeof(protocol));
	m_role_module_mgr->NetSend((const char *)&protocol, send_len);
}

void Pet::ResetPetFeiShengRewardFlag()
{
	if (m_common_param.reset_fei_sheng_rewards_flag != 0)
	{
		return;
	}

	m_common_param.reset_fei_sheng_rewards_flag = 1;

	for (int i = 0; i < ARRAY_LENGTH(m_common_param.yi_wen_lu_rewards); ++i)
	{
		PetYiWenLuRewardParam & curr = m_common_param.yi_wen_lu_rewards[i];
		WipeBit(curr.fetch_flag, PET_FEI_SHENG_REWARD_TYPE_TWO);
	}
}

void Pet::CheckPetYiWenLuRewardByIndex(int pet_index, bool need_notice)
{
	PetParam * pet = this->__GetPetParam(pet_index);
	if (pet)
	{
		this->CheckPetYiWenLuReward(*pet, need_notice);
	}
}

void Pet::CheckPetYiWenLuReward(const PetParam & pet_param, bool need_notice)
{
	std::map<int, bool> pet_id_to_is_liao_yao_pet_id;
	pet_id_to_is_liao_yao_pet_id[pet_param.pet_id] = false;
	pet_id_to_is_liao_yao_pet_id[pet_param.lian_yao_pet_id] = true;

	bool has_chang = false;
	for (std::map<int, bool>::iterator iter_pet_id = pet_id_to_is_liao_yao_pet_id.begin(); iter_pet_id != pet_id_to_is_liao_yao_pet_id.end(); ++iter_pet_id)
	{
		int pet_id = iter_pet_id->first;
		bool is_liao_yao_pet_id = iter_pet_id->second;

		int fei_sheng_times = pet_param.fei_sheng_times;
		int str_level = pet_param.str_level;

		if (is_liao_yao_pet_id)
		{
			fei_sheng_times = 0;
			str_level = pet_param.lian_yao_pet_str_lv;
		}

		const PetCfg * pet_cfg = LOGIC_CONFIG->GetPetConfig()->GetPetCfg(pet_id);
		if (!pet_cfg)
		{
			continue;
		}

		int mutant_id = pet_cfg->mutant_id;
		const PetYiWenLuCfg * cfg = LOGIC_CONFIG->GetPetConfig()->GetPetYiWenLuCfg(mutant_id);
		if (!cfg)
		{
			continue;
		}

		PetYiWenLuRewardParam * ptr = this->GetPetYiWenLuRewardParam(mutant_id, true);
		if (!ptr)
		{
			continue;
		}

		for (std::map<int, PetYiWenLuRewardCfg>::const_iterator iter = cfg->rewards_cfg.begin(); iter != cfg->rewards_cfg.end(); ++iter)
		{
			const PetYiWenLuRewardCfg & reward_cfg = iter->second;

			bool can_finsih = false;
			switch (reward_cfg.condition)
			{
				case PET_YI_WEN_LU_REWARD_CONDITION_GET:
					{
						can_finsih = true;
					}
					break;
				case PET_YI_WEN_LU_REWARD_CONDITION_STR:
					{
						if (str_level >= reward_cfg.condition_value)
						{
							can_finsih = true;
						}
					}
					break;
				case PET_YI_WEN_LU_REWARD_CONDITION_FEI_SHENG:
					{
						if (fei_sheng_times >= reward_cfg.condition_value)
						{
							can_finsih = true;
						}
					}
					break;
			}

			if (can_finsih)
			{
				bool b = false;
				this->SetPetYiWenLuRewardFinishFlag(ptr, reward_cfg.reward_seq, &b);
				has_chang = has_chang || b;
			}
		}
	}

	if (need_notice && has_chang)
	{
		this->SendYiWenLuRewardInfo();
	}	
}

void Pet::SetPetYiWenLuRewardFinishFlag(int mutant_id, int reward_seq,bool * has_chang)
{
	PetYiWenLuRewardParam * ptr = this->GetPetYiWenLuRewardParam(mutant_id, true);
	this->SetPetYiWenLuRewardFinishFlag(ptr,reward_seq,has_chang);
}

void Pet::SetPetYiWenLuRewardFinishFlag(PetYiWenLuRewardParam * ptr, int reward_seq, bool * has_chang)
{
	if (!ptr)
	{
		return;
	}

	if (::IsSetBit(ptr->finish_flag, reward_seq))
	{
		if (has_chang)
		{
			*has_chang = false;
		}
		return;
	}

	if (has_chang)
	{
		*has_chang = true;
	}
	::SetBit(ptr->finish_flag, reward_seq);
}

PetYiWenLuRewardParam * Pet::GetPetYiWenLuRewardParam(int mutant_id, bool get_empty)
{
	PetYiWenLuRewardParam * ptr = NULL;
	PetYiWenLuRewardParam * empty_ptr = NULL;
	for (int i = 0; i < ARRAY_LENGTH(m_common_param.yi_wen_lu_rewards); ++i)
	{
		PetYiWenLuRewardParam & curr = m_common_param.yi_wen_lu_rewards[i];
		if (curr.mutant_id == mutant_id)   
		{
			ptr = &curr;
			break;
		}

		if (curr.mutant_id == 0 && !empty_ptr)		
		{
			empty_ptr = &curr;
		}
	}

	if (get_empty && ptr == NULL && empty_ptr != NULL)
	{
		empty_ptr->Reset();
		empty_ptr->mutant_id = mutant_id;
		ptr = empty_ptr;
	}

	return ptr;
}

int Pet::PetBadgePutOn(int pet_indx, int col, int indx)
{
	PetParam * pet = this->__GetPetParam(pet_indx);
	if (!pet)
	{
		return -__LINE__;
	}
	
	ItemDataWrapper item_wrapper;
	if (!m_role_module_mgr->GetKnapsack()->GetItemGridWrapData(col, indx, &item_wrapper))
	{
		return -__LINE__;
	}

	const ItemBase * item_base = ITEMPOOL->GetItem(item_wrapper.item_id);
	if (!item_base)
	{
		return -__LINE__;
	}

	if (item_base->GetItemType() != I_TYPE_PET_BADGE)
	{
		return -__LINE__;
	}

	this->PetBadgeTakeOff(pet_indx);
	if (pet->pet_bage_item.pet_badgei_item_id != 0)
	{
		return -__LINE__;
	}


	if (!m_role_module_mgr->GetKnapsack()->ConsumeItemByIndex(col, indx, 1, __FUNCTION__))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_USE_ITEM_ERROR);
		return -__LINE__;
	}

	NetValueItemParam param;
	param.SetData(item_wrapper.param_data);
	param.pet_badge_item_param.item_uuid = item_wrapper.item_unique_id;
	pet->pet_bage_item = param.pet_badge_item_param;

	this->ReCalcAttr(pet_indx);
	this->SetItemDirty(pet_indx, true, Protocol::SCSendPetSingleInfo::PET_NOTIE_TYPE_PET_EQUIPMENT_CHANGE);
	if (this->IsInPlatfformBattle(this->GetPetUniqueId(pet_indx)))
	{
		this->SyncPetDataPlatformBattle(pet_indx);
	}

	return 0;
}

int Pet::PetBadgeTakeOff(int pet_index)
{
	if (this->PetInvalid(pet_index)) return 0;
	PetParam & pet_param = m_grid_list[pet_index].param->pet_param;
	int item_id = pet_param.pet_bage_item.pet_badgei_item_id;
	if (item_id == 0)
	{
		return 0;
	}

	const ItemBase * tmp_itembase = ITEMPOOL->GetItem(item_id);
	if (tmp_itembase == NULL || tmp_itembase->GetItemType() != I_TYPE_PET_BADGE)
	{
		//增加清空异常道具操作--对于道具id存在，但是异常的宠物装备直接清空
		pet_param.pet_bage_item.Reset();
		this->ReCalcAttr(pet_index);
		this->SetItemDirty(pet_index, true, Protocol::SCSendPetSingleInfo::PET_NOTIE_TYPE_BADGE_CHANGE);
		if (this->IsInPlatfformBattle(this->GetPetUniqueId(pet_index)))
		{
			this->SyncPetDataPlatformBattle(pet_index);
		}
		gamelog::g_log_pet.printf(LL_INFO, "PetBadgeTakeOff err user[%s,%d] pet_index[%d] item_id:%d",
			m_role_module_mgr->GetRoleName(), m_role_module_mgr->GetUid(), pet_index, item_id);
		return 0;
	}

	static NetValueItemParam p_nvip; p_nvip.Reset();
	p_nvip.pet_badge_item_param = pet_param.pet_bage_item;

	ItemDataWrapper badge_item; badge_item.Reset();
	badge_item.item_id = pet_param.pet_bage_item.pet_badgei_item_id;
	badge_item.num = 1;
	badge_item.has_param = 1;
	badge_item.is_bind = 1;
	badge_item.is_bind = 1;
	badge_item.item_unique_id = pet_param.pet_bage_item.item_uuid;
	p_nvip.GetData(badge_item.param_data);
	
	if (!m_role_module_mgr->GetKnapsack()->Put(badge_item, PUT_REASON_PET_BADGE_TAKE_OFF))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return -__LINE__;
	}
	pet_param.pet_bage_item.Reset();
	this->ReCalcAttr(pet_index);
	this->SetItemDirty(pet_index, true, Protocol::SCSendPetSingleInfo::PET_NOTIE_TYPE_BADGE_CHANGE);

	if (this->IsInPlatfformBattle(this->GetPetUniqueId(pet_index)))
	{
		this->SyncPetDataPlatformBattle(pet_index);
	}

	return 0;
}

int Pet::PetBadgeExchange(int pet_index_1, int pet_index_2)
{
	PetParam * pet_param_1 = this->__GetPetParam(pet_index_1);
	if (!pet_param_1)
	{
		return -__LINE__;
	}

	PetParam * pet_param_2 = this->__GetPetParam(pet_index_2);
	if (!pet_param_2)
	{
		return -__LINE__;
	}

	std::swap(pet_param_1->pet_bage_item, pet_param_2->pet_bage_item);
	this->ReCalcAttr(pet_index_1);
	this->SetItemDirty(pet_index_1, true, Protocol::SCSendPetSingleInfo::PET_NOTIE_TYPE_PET_EQUIPMENT_CHANGE);
	this->ReCalcAttr(pet_index_2);
	this->SetItemDirty(pet_index_2, true, Protocol::SCSendPetSingleInfo::PET_NOTIE_TYPE_PET_EQUIPMENT_CHANGE);

	if (this->IsInPlatfformBattle(this->GetPetUniqueId(pet_index_1)))
	{
		this->SyncPetDataPlatformBattle(pet_index_1);
	}

	if (this->IsInPlatfformBattle(this->GetPetUniqueId(pet_index_2)))
	{
		this->SyncPetDataPlatformBattle(pet_index_2);
	}

	return 0;
}

int Pet::PetBadgeCompose(int item_id)
{
	const PetBadgeCfg * cfg = LOGIC_CONFIG->GetPetBadgeConfig()->GetPetBadgeCfg(item_id);
	if (!cfg)
	{
		return -__LINE__;
	}

	if (!m_role_module_mgr->GetKnapsack()->CheckForPut(cfg->item_id,1))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return -__LINE__;
	}

	if (cfg->compose_succ_item_id != 0 && !m_role_module_mgr->GetKnapsack()->CheckForPut(cfg->compose_succ_item_id, 1))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return -__LINE__;
	}

	int get_item = cfg->item_id;
	if (cfg->compose_succ_item_id != 0)
	{
		int r = RandomNum(10000);
		if (r < cfg->compose_succ_rate)
		{
			get_item = cfg->compose_succ_item_id;
		}
	}

	if (!m_role_module_mgr->GetKnapsack()->ConsumeItem(cfg->compose_need_item_id, cfg->compose_need_item_num, __FUNCTION__))
	{
		return -__LINE__;
	}

	ItemConfigData item;
	item.item_id = get_item;
	item.num = 1;
	item.is_bind = true;
	m_role_module_mgr->GetKnapsack()->Put(item, PUT_REASON_PET_BADGE_COMPOSE);

	return 0;
}

int Pet::PetBadgeDeCompose(const std::set<KnapsackItemIndex> & list)
{
	std::map<int, int> decompose_get_item;
	for (std::set<KnapsackItemIndex>::const_iterator iter = list.begin(); iter != list.end(); ++iter)
	{
		const KnapsackItemIndex & curr = *iter;
		const ItemGridData * item_grid = m_role_module_mgr->GetKnapsack()->GetItemGridData2(curr.col, curr.idx);
		if (!item_grid)
		{
			return -__LINE__;
		}

		const PetBadgeCfg * cfg = LOGIC_CONFIG->GetPetBadgeConfig()->GetPetBadgeCfg(item_grid->item_id);
		if (!cfg)
		{
			return -__LINE__;
		}

		decompose_get_item[cfg->decompose_get_item_id] += cfg->decompose_get_item_num;
	}

	std::vector<ItemConfigData> rewards;
	for (std::map<int, int>::const_iterator iter = decompose_get_item.begin(); iter != decompose_get_item.end(); ++iter)
	{
		ItemConfigData tmp;
		tmp.item_id = iter->first;
		tmp.num = iter->second;
		tmp.is_bind = true;
		rewards.push_back(tmp);
	}

	if (!m_role_module_mgr->GetKnapsack()->CheckForPutList2(rewards))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return -__LINE__;
	}

	for (std::set<KnapsackItemIndex>::const_iterator iter = list.begin(); iter != list.end(); ++iter)
	{
		const KnapsackItemIndex & curr = *iter;
		m_role_module_mgr->GetKnapsack()->ConsumeItemByIndex(curr.col, curr.idx, 1, __FUNCTION__);
	}

	m_role_module_mgr->GetKnapsack()->PutVec(rewards, PUT_REASON_PET_BADGE_DECOMPOSE);

	return 0;
}

int Pet::PetLianYao(int pet_indx_1, int pet_indx_2, int furnace_id)
{
	if (!IsSetBit(m_common_param.is_unlock_lian_yao_furance_flag, furnace_id))
	{
		return -__LINE__;
	}

	const PetFurnaceCfg * cfg = LOGIC_CONFIG->GetPetLianYaoConfig()->GetPetFurnaceCfg(furnace_id);
	if (!cfg)
	{
		return -__LINE__;
	}

	PetParam * pet = this->__GetPetParam(pet_indx_1);
	PetParam * consume_pet = this->__GetPetParam(pet_indx_2);
	if (!pet || !consume_pet)
	{
		return -__LINE__;
	}

	const PetCfg * pet_cfg = LOGIC_CONFIG->GetPetConfig()->GetPetCfg(pet->pet_id);
	const PetCfg * consume_pet_cfg = LOGIC_CONFIG->GetPetConfig()->GetPetCfg(consume_pet->pet_id);
	if (!pet_cfg || !consume_pet_cfg)
	{
		return -__LINE__;
	}

	if (pet_cfg->mutant_id == consume_pet_cfg->mutant_id)
	{
		return -__LINE__;
	}

	if (pet_cfg->quality < PET_QUALITY_GOLD)
	{
		return -__LINE__;
	}

	if (pet_cfg->quality > cfg->main_pet_max_quality)
	{
		return -__LINE__;
	}

	if (pet->str_level < cfg->main_pet_min_strengthen)
	{
		return -__LINE__;
	}

	if (consume_pet_cfg->quality > cfg->second_pet_max_quality)
	{
		return -__LINE__;
	}

	if (consume_pet->str_level > cfg->second_pet_max_strengthen)
	{
		return -__LINE__;
	}

	if (!m_role_module_mgr->GetMoney()->CoinBindMoreThan(cfg->need_coin))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_COIN_NOT_ENOUGH);
		return -__LINE__;
	}

	if (!this->CanDeletePet(pet_indx_2, __FUNCTION__, (1 << PET_CAN_DELETE_EXCLUDE_FLAG_PET_SOUL) | (1 << PET_CAN_DELETE_EXCLUDE_PLATFORM_BATTLE)))
	{
		return -__LINE__;
	}

	//主宠物 脱第2格子
	if (!this->PutOffPetSoulEquip(pet_indx_1, 1))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_PET_WEAR_SOUL_EQUIP);
		return -__LINE__;
	}

	//消耗的宠物 脱第2格子
	if (!this->PutOffPetSoulEquip(pet_indx_2,1))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_PET_WEAR_SOUL_EQUIP);
		return -__LINE__;
	}

	//消耗的铜币放到最下面
	m_role_module_mgr->GetMoney()->UseCoinBind(cfg->need_coin, __FUNCTION__);
	
	//然后将消耗宠物第二格子魂器移动到主宠物第二格子
	PetSoulEquip * pet_s1 = pet->GetPetSoulEquipInPetParam(1, false);
	PetSoulEquip * consume_pet_s1 = consume_pet->GetPetSoulEquipInPetParam(0);
	if (pet_s1 && consume_pet_s1)
	{
		*pet_s1 = *consume_pet_s1;
		consume_pet_s1->Reset();
	}

	int old_lian_yao_pet_id = pet->lian_yao_pet_id;
	int lian_yao_pet_str_lv = pet->lian_yao_pet_str_lv;

	pet->lian_yao_pet_id = consume_pet->pet_id;
	pet->lian_yao_pet_str_lv = consume_pet->str_level;
	this->PetLianYaoAddPassiveSkill(*pet, *consume_pet);
	EventHandler::Instance().OnPetLianYao(m_role_module_mgr->GetRole(), pet_cfg->quality);

	gamelog::g_log_pet.printf(LL_INFO, "Pet::PetLianYao role[%d,%s]  item_unique_id:%lld item_id:%d lian_yao_id:%d lian_yao_pet_str_lv:%d old_lian_yao_pet_id:%d lian_yao_pet_str_lv:%d",
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), this->GetPetUniqueId(pet_indx_1), pet->pet_id,
		pet->lian_yao_pet_id, pet->lian_yao_pet_str_lv, old_lian_yao_pet_id, lian_yao_pet_str_lv);

	for (int i = 0; i < ARRAY_LENGTH(pet->lian_yao_passive_list); ++i)
	{
		if (pet->lian_yao_passive_list[i].passive_skill_id != 0)
		{
			gamelog::g_log_pet.printf(LL_INFO, "Pet::PetLianYao_2 role[%d,%s]  item_unique_id:%lld item_id:%d skill_id:%d", m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
				this->GetPetUniqueId(pet_indx_1), pet->lian_yao_pet_id, pet->lian_yao_passive_list[i].passive_skill_id);
		}
	}

	this->ReCalcAttr(pet_indx_1);
	this->DeletePet(pet_indx_2, __FUNCTION__);
	consume_pet = NULL;

	this->SetItemDirty(pet_indx_1, true, Protocol::SCSendPetSingleInfo::PET_NOTIE_TYPE_LIAN_YAO);

	return 0;
}

void Pet::PetLianYaoAddPassiveSkill(PetParam & pet_1, const PetParam & pet_2)
{
	const PetFurnaceOtherCfg & other_cfg = LOGIC_CONFIG->GetPetLianYaoConfig()->GetOtherCfg();

	int lian_yao_passive_num = 0;
	int lian_yao_passive_free_index = -1;
	lian_yao_passive_num = this->PetLianYaoPassiveNum(pet_1, &lian_yao_passive_free_index);

	if (lian_yao_passive_free_index == -1)
	{
		return;
	}

	int add_rand_rate = RandomNum(10000);
	if (add_rand_rate >= other_cfg.lian_yao_add_passive_rate)
	{
		return;
	}

	if (lian_yao_passive_num >= other_cfg.lian_yao_add_passive_max_num)
	{
		return;
	}
	
	std::vector<int> rand_passive_index_list;
	for (int i = 0; i < ARRAY_LENGTH(pet_2.passive_list); ++i)
	{
		if (pet_2.passive_list[i].passive_skill_id == 0)
		{
			continue;
		}
		const GamePassiveSkillCfg * passive_skill_cfg = SkillPool::Instance().GetPassiveSkillCfg(pet_2.passive_list[i].passive_skill_id);
		if (passive_skill_cfg == NULL)
		{
			continue;
		}

		if (passive_skill_cfg->IsElementalSkillType())
		{
			continue;
		}

		rand_passive_index_list.push_back(i);
	}

	if (rand_passive_index_list.empty())
	{
		return;
	}

	int rand_add_index = RandomNum((int)rand_passive_index_list.size());
	pet_1.lian_yao_passive_list[lian_yao_passive_free_index] = pet_2.passive_list[rand_passive_index_list[rand_add_index]];
}

int Pet::PetLianYaoPassiveNum(const PetParam & pet_param, int * lian_yao_passive_free_index_ptr)
{
	int lian_yao_passive_num = 0;
	int lian_yao_passive_free_index = -1;
	for (int i = 0; i < ARRAY_LENGTH(pet_param.lian_yao_passive_list); ++i)
	{
		if (pet_param.lian_yao_passive_list[i].passive_skill_id == 0)
		{
			if (lian_yao_passive_free_index == -1)
			{
				lian_yao_passive_free_index = i;
			}
			continue;
		}

		lian_yao_passive_num += 1;
	}

	if (lian_yao_passive_free_index_ptr)
	{
		*lian_yao_passive_free_index_ptr = lian_yao_passive_free_index;
	}

	return lian_yao_passive_num;
}

int Pet::PetLianYaoComprehendPassiveSkill(int pet_index)
{
	PetParam * pet = this->__GetPetParam(pet_index);
	if (!pet)
	{
		return -__LINE__;
	}

	int lian_yao_passive_num = 0;
	int lian_yao_passive_free_index = -1;
	lian_yao_passive_num = this->PetLianYaoPassiveNum(*pet, &lian_yao_passive_free_index);
	if (lian_yao_passive_free_index == -1)
	{
		return -__LINE__;
	}

	if (lian_yao_passive_free_index == -1)
	{
		return -__LINE__;
	}

	const PetFurnaceOtherCfg & other_cfg = LOGIC_CONFIG->GetPetLianYaoConfig()->GetOtherCfg();
	if (lian_yao_passive_num >= other_cfg.lian_yao_add_passive_max_num)
	{
		return -__LINE__;
	}

	if (lian_yao_passive_num < 0 || lian_yao_passive_num >= (int)other_cfg.lian_yao_comprehend_passive_skill_need_num.size())
	{
		return -__LINE__;
	}

	int need_item_num = other_cfg.lian_yao_comprehend_passive_skill_need_num[lian_yao_passive_num];

	if (other_cfg.lian_yao_comprehend_passive_skill_pool.empty())
	{
		return -__LINE__;
	}

	int r = rand() % (int)other_cfg.lian_yao_comprehend_passive_skill_pool.size();
	int skill_id = other_cfg.lian_yao_comprehend_passive_skill_pool[r];

	if (!m_role_module_mgr->GetKnapsack()->ConsumeItem(other_cfg.lian_yao_comprehend_passive_skill_need_item_id, need_item_num, __FUNCTION__))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_ITEM_AMOUNT_NOT_ENOUGH);
		return -__LINE__;
	}

	pet->lian_yao_passive_list[lian_yao_passive_free_index].passive_skill_id = skill_id;
	this->ReCalcAttr(pet_index);
	this->SetItemDirty(pet_index, true, Protocol::SCSendPetSingleInfo::PET_NOTIE_TYPE_LIAN_YAO_COMPREHEND_PASSIVE_SKILL);

		
	return 0;
}

PetPassiveItemInfo * Pet::GetPetPassiveItemInfo(PetParam & pet_param,int passive_skill_type, int passive_skill_index)
{
	if (passive_skill_type == PET_SKILL_SRC_TYPE_PASSIVE)
	{
		if (0 <= passive_skill_index && passive_skill_index < ARRAY_LENGTH(pet_param.passive_list))
		{
			return &pet_param.passive_list[passive_skill_index];
		}
	}

	if (passive_skill_type == PET_SKILL_SRC_TYPE_LIAN_YAO)
	{
		if (0 <= passive_skill_index && passive_skill_index < ARRAY_LENGTH(pet_param.lian_yao_passive_list))
		{
			return &pet_param.lian_yao_passive_list[passive_skill_index];
		}
	}


	return NULL;
}

void Pet::BuildMapPetQualityToStrLevel()
{
	for (int i = 0; i < ARRAY_LENGTH(m_grid_list); ++i)
	{
		const PetParam * p = this->GetPetParam(i);
		this->SetMapPetQualityToStrLevel(p);
	}

	for (int i = 0; i < ARRAY_LENGTH(m_storage_grid_list); ++i)
	{
		const PetParam * p = this->GetPetParamFromStorage(i);
		this->SetMapPetQualityToStrLevel(p);
	}
}

void Pet::SetMapPetQualityToStrLevel(const PetParam * p)
{
	if (!p)
	{
		return;
	}

	std::map<int, int> pet_id_to_str_level;
	pet_id_to_str_level[p->pet_id] = p->str_level;
	pet_id_to_str_level[p->lian_yao_pet_id] = p->lian_yao_pet_str_lv;

	for (std::map<int, int>::iterator iter = pet_id_to_str_level.begin(); iter != pet_id_to_str_level.end(); ++iter)
	{
		int pet_id = iter->first;
		int str_level = iter->second;

		const PetCfg * pet_cfg = LOGIC_CONFIG->GetPetConfig()->GetPetCfg(p->pet_id);
		if (!pet_cfg)
		{
			continue;;
		}

		int * str_level_ptr = MapValuesPtr(m_pet_quality_to_str_level, pet_cfg->quality);
		if (str_level_ptr)
		{
			if (str_level > *str_level_ptr)
			{
				*str_level_ptr = str_level;
			}
		}
		else
		{
			m_pet_quality_to_str_level[pet_cfg->quality] = str_level;
		}
	}

}

void Pet::CheckUnlockLianYaoFurance()
{
	int role_level = m_role_module_mgr->GetRole()->GetLevel();
	int role_fei_sheng_times = m_role_module_mgr->GetRoleFeiShengTimes();
	bool is_need_send = false;
	for (int furnace_id = 0; furnace_id < BitCount(m_common_param.is_unlock_lian_yao_furance_flag); ++furnace_id)
	{
		if (::IsSetBit(m_common_param.is_unlock_lian_yao_furance_flag, furnace_id))
		{
			continue;
		}

		const PetFurnaceCfg * cfg = LOGIC_CONFIG->GetPetLianYaoConfig()->GetPetFurnaceCfg(furnace_id);
		if (!cfg)
		{
			continue;
		}

		if (role_level < cfg->lian_yao_unlock_need_role_level)
		{
			continue;
		}

		if (role_fei_sheng_times < cfg->lian_yao_unlock_need_role_fei_sheng_times)
		{
			continue;
		}

		if (cfg->lian_yao_unlock_need_pet_quality > PET_QUALITY_INVALID)
		{
			const int * str_level = MapValuesConstPtr(m_pet_quality_to_str_level, cfg->lian_yao_unlock_need_pet_quality);
			if (!str_level)
			{
				continue;
			}

			if (*str_level < cfg->lian_yao_unlock_need_pet_strengthen_level)
			{
				continue;
			}
		}

		is_need_send = true;
		::SetBit(m_common_param.is_unlock_lian_yao_furance_flag, furnace_id);
	}

	if (is_need_send)
	{
		this->SendLianYaoCommonInfo();
	}
}

void Pet::SendLianYaoCommonInfo()
{
	Protocol::SCPetLianYaoCommonInfo protocol;
	protocol.is_unlock_lian_yao_furance_flag = m_common_param.is_unlock_lian_yao_furance_flag;

	m_role_module_mgr->NetSend(&protocol, sizeof(protocol));
}

void Pet::CheckPetBagGetReward()
{
	int max_str_level = 0, quality = 0;
	for (int i = 0; i < ARRAY_LENGTH(m_grid_list); ++i)
	{
		if(m_grid_list[i].Invalid()) continue;

		const PetCfg * pet_cfg = LOGIC_CONFIG->GetPetConfig()->GetPetCfg(m_grid_list[i].param->pet_param.pet_id);
		if(pet_cfg == NULL) continue;

		const DayRewardOtherCfg& other_cfg = LOGIC_CONFIG->GetDayRewardConfig()->GetOtherCfg();

		std::set<int>::const_iterator it_quality = other_cfg.pet_strengthen.find(pet_cfg->quality);
		if(it_quality == other_cfg.pet_strengthen.end()) continue;

		if (max_str_level < m_grid_list[i].param->pet_param.str_level)
		{
			max_str_level = m_grid_list[i].param->pet_param.str_level;
			quality = pet_cfg->quality;
		}
	}

	for (int i = 0; i < ARRAY_LENGTH(m_storage_grid_list); ++i)
	{
		if (m_storage_grid_list[i].Invalid()) continue;

		const PetCfg * pet_cfg = LOGIC_CONFIG->GetPetConfig()->GetPetCfg(m_storage_grid_list[i].param->pet_param.pet_id);
		if (pet_cfg == NULL) continue;

		const DayRewardOtherCfg& other_cfg = LOGIC_CONFIG->GetDayRewardConfig()->GetOtherCfg();

		std::set<int>::const_iterator it_quality = other_cfg.pet_strengthen.find(pet_cfg->quality);
		if (it_quality == other_cfg.pet_strengthen.end()) continue;

		if (max_str_level < m_storage_grid_list[i].param->pet_param.str_level)
		{
			max_str_level = m_storage_grid_list[i].param->pet_param.str_level;
			quality = pet_cfg->quality;
		}

	}

	if (max_str_level != 0 && quality != 0)
	{
		m_role_module_mgr->GetDayReward()->OnUnlockPetStrengthenReward(quality, max_str_level);
	}
}

int Pet::OnUpgradePetSkill(int pet_index, int passive_skill_index, int is_lian_yao_skill)
{
	PetParam * pet = this->__GetPetParam(pet_index);
	if (pet == NULL)
	{
		return -__LINE__;;
	}

	if (pet->fei_sheng_times <= 0)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_PET_NEED_FEI_SHENG);
		return -__LINE__;;
	}

	if (passive_skill_index < 0 || passive_skill_index >= ARRAY_LENGTH(pet->passive_list))
	{
		return -__LINE__;
	}

	PetPassiveItemInfo * pet_passive_item_info = this->GetPetPassiveItemInfo(*pet, is_lian_yao_skill ? PET_SKILL_SRC_TYPE_LIAN_YAO : PET_SKILL_SRC_TYPE_PASSIVE, passive_skill_index);
	if (!pet_passive_item_info)
	{
		return -__LINE__;
	}


	unsigned short  skill_id = pet_passive_item_info->passive_skill_id;
	unsigned char & skill_level = pet_passive_item_info->skill_level;

	const PetSkillUpgradeCfg * next_upgrade_skill_cfg = LOGIC_CONFIG->GetPetConfig()->GetPetSkillUpgradeCfg(skill_id, skill_level + 1);
	if (next_upgrade_skill_cfg == NULL)
	{
		return -__LINE__;
	}

	if (!m_role_module_mgr->GetKnapsack()->ConsumeItem(next_upgrade_skill_cfg->consume_item_id, next_upgrade_skill_cfg->consume_num, __FUNCTION__,true))
	{
		return -__LINE__;
	}

	skill_level += 1;

	this->ReCalcAttr(pet_index);
	this->SetItemDirty(pet_index,true, Protocol::SCSendPetSingleInfo::PET_NOTIE_TYPE_UPGRADE_SKILL);

	return 0;
}

void Pet::SetAutoAddInfo(int pet_index, int add_flag)
{
	PetParam * pet = this->__GetPetParam(pet_index);;
	if (pet == NULL)
	{
		return;
	}

	pet->auto_add_points_flag = (unsigned char)add_flag;
	this->SetItemDirty(pet_index, true, Protocol::SCSendPetSingleInfo::PET_NOTIE_TYPE_SET_ADD_POINT_INFO);
}

void Pet::CheckModVer()
{
	this->CheckModVer1();
	this->CheckModVer2();
}

void Pet::CheckModVer1()
{
	if (m_common_param.pet_mod_ver == 0)
	{
		m_common_param.pet_mod_ver = 1;

		int role_id = m_role_module_mgr->GetUid();
		if (role_id == 48235031)
		{
			long long unique_item_id = 1735857028302241792LL;
			int skill_id = 7147;
			for (int i = 0; i < ARRAY_LENGTH(m_grid_list); ++i)
			{
				if (this->PetInvalid(i)) continue;

				ItemGridData & pet_grid = m_grid_list[i];
				if (pet_grid.item_unique_id != unique_item_id)
				{
					continue;
				}

				int old_times = pet_grid.param->pet_param.fei_sheng_times;
				int old_id = pet_grid.param->pet_param.fei_sheng_select_skill_id;
				int old_lv = pet_grid.param->pet_param.fei_sheng_select_skill_level;

				pet_grid.param->pet_param.fei_sheng_times = 1;
				pet_grid.param->pet_param.fei_sheng_task_jie_duan = 3;
				pet_grid.param->pet_param.fei_sheng_select_skill_id = skill_id;
				pet_grid.param->pet_param.fei_sheng_select_skill_level = 4;
				this->SetItemDirty(i);

				gamelog::g_log_pet.printf(LL_INFO, "Pet::CheckModVer1 role[%d,%s]  unique_id[%lld] old_times:%d old_id:%d old_lv:%d",
					m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), unique_item_id, old_times, old_id, old_lv);

				break;
			}

			for (int i = 0; i < ARRAY_LENGTH(m_storage_grid_list); ++i)
			{
				if (this->PetStorageInvalid(i)) continue;

				ItemGridData & pet_grid = m_storage_grid_list[i];
				if (pet_grid.item_unique_id != unique_item_id)
				{
					continue;
				}

				int old_times = pet_grid.param->pet_param.fei_sheng_times;
				int old_id = pet_grid.param->pet_param.fei_sheng_select_skill_id;
				int old_lv = pet_grid.param->pet_param.fei_sheng_select_skill_level;

				pet_grid.param->pet_param.fei_sheng_times = 1;
				pet_grid.param->pet_param.fei_sheng_task_jie_duan = 3;
				pet_grid.param->pet_param.fei_sheng_select_skill_id = skill_id;
				pet_grid.param->pet_param.fei_sheng_select_skill_level = 4;
				this->SetPetStorageItemDirty(i);

				gamelog::g_log_pet.printf(LL_INFO, "Pet::CheckModVer1 role[%d,%s]  unique_id[%lld] old_times:%d old_id:%d old_lv:%d",
					m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), unique_item_id, old_times, old_id, old_lv);

				break;
			}
		}
	}
}

void Pet::CheckModVer2()
{
	if (m_common_param.pet_mod_ver == 1)
	{
		m_common_param.pet_mod_ver = 2;

		int role_id = m_role_module_mgr->GetUid();
		if (role_id == 44040352 && m_common_param.fei_sheng_pet_uuid == 2018458296607572053LL)
		{
			gamelog::g_log_pet.printf(LL_INFO, "Pet::CheckModVer1 role[%d,%s]  unique_id[%lld] fei_sheng_task_id:%d fei_sheng_today_is_finish:%d", m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(),
				m_common_param.fei_sheng_pet_uuid, m_common_param.fei_sheng_task_id, m_common_param.fei_sheng_today_is_finish);
			
			m_role_module_mgr->GetTaskManager()->ClearPetFeiShengTask();
			m_common_param.fei_sheng_task_id = 0;
			m_common_param.fei_sheng_today_is_finish = 0;
			m_common_param.fei_sheng_pet_uuid = 0;
			this->SendFeiShengInfo();
		}
	}
}

void Pet::RandRefinedBroadcast()
{
	const RobotRoleCfg * robot_role = ROBOTCONFIG->RandRobotByLevel(32, ROBOT_CONFIG_USE_TYPE_STATIC_ROBOT);
	if (NULL == robot_role)
	{
		return;
	}
	int m_profession = robot_role->base_prof * PROFESSION_BASE + robot_role->advance_times;
	int m_avatar_type = AppearanceConfig::Instance().RandAvatarByProfession(m_profession);

	GameName robot_name;
	ROBOTCONFIG->GetRandomName(SEX_ARRAY[m_avatar_type], robot_name, false, true, true, RobotConfig::ROBOT_NAME_TYPE_DEF);

	ItemID pet_id = LOGIC_CONFIG->GetPetConfig()->RandPetId();
	const ItemBase* item = ITEMPOOL->GetItem(pet_id);
	if (NULL == item) return;

	const PetCfg* pet_cfg = LOGIC_CONFIG->GetPetConfig()->GetPetCfg(pet_id);
	if (NULL == pet_cfg) return;

	NetValueItemParam pet;
	if (!LOGIC_CONFIG->GetPetConfig()->RandPetParam(pet_id, 1, true, pet)) return;
	pet.pet_param.level = RankManager::Instance().GetPersonRank()->GetAverageLevel();

	if (!LOGIC_CONFIG->GetPetConfig()->RandPetGrowth(pet_id, true, pet)) return;

	const PetCfg * cfg = LOGIC_CONFIG->GetPetConfig()->GetPetCfg(pet_id);
	if (NULL == cfg) return;

	const PetStrengthenCfg * str_cfg = LOGIC_CONFIG->GetPetConfig()->GetPetStrengthenCfg(cfg->strengthen_cfg_id, 1);
	if (NULL == str_cfg) return;

	ItemGridData grid_data;
	grid_data.Clear();
	grid_data.item_id = pet_id;
	grid_data.num = 1;
	grid_data.is_bind = true;
	grid_data.gold_price = 0;
	grid_data.invalid_time = 0u;
	UniqueIDGenerator::Instance()->GenUniqueID(&grid_data.item_unique_id);
	grid_data.param = ItemBase::CreateParam();
	grid_data.param->SetData(pet.data);
	grid_data.param->pet_param.modify_ver = MAX_PET_MODIFY_VER;

 	static AttributeList attr; attr.Reset();

	const PetParam & pet_param = grid_data.param->pet_param;

	double total_attr[ADD_POINT_TYPE_MAX] = { 0 };
	// 等级+资质+强化+潜能获得的属性
	for (int i = 0; i < ADD_POINT_TYPE_MAX; ++i)
	{
		// 资质 自身 * 强化提升率
		double growth = pet_param.growth[i] + str_cfg->add_growth[i] / 100.0;

		// 玩家分配的属性点
		double attr_point = pet_param.attr[i] * 1.0;

		total_attr[i] += (growth * (PET_ATTR_BASE_UP + (pet_param.level - 1) * PET_ATTR_LEVEL_UP)) + attr_point;
	}

	double base_attrs[BATTLE_ATTR_MAX] = { 0 };		//基础属性(强化+资质+潜能+突破)(与属性来源对齐)
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

	const PetBreakAttrCfg * pet_break_attr_cfg = LOGIC_CONFIG->GetPetConfig()->GetPetBreakAttrCfg(pet_param.level, pet_param.is_up_level_break);
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

	double temp_attrs[BATTLE_ATTR_MAX] = { 0 };

	//加点副属性
	for (int i = 0; i < ADD_POINT_TYPE_MAX; ++i)
	{
		const PetAttrExchangeCfg::ItemCfg * attr_cfg = LOGIC_CONFIG->GetPetConfig()->GetAttrExchange(0, i);
		if (NULL != attr_cfg)
		{
			// 先算整数 全部计算完再除 100 不然会有误差 每次零点几 积累起来就有 1 了
			// 血蓝
			temp_attrs[BATTLE_ATTR_MAX_HP] += total_attr[i] * attr_cfg->maxhp;
			temp_attrs[BATTLE_ATTR_MAX_MP] += total_attr[i] * attr_cfg->maxmp;
			// 基础属性
			temp_attrs[BATTLE_ATTR_ATTACK] += total_attr[i] * attr_cfg->gongji;
			temp_attrs[BATTLE_ATTR_DEFENSE] += total_attr[i] * attr_cfg->fangyu;
			temp_attrs[BATTLE_ATTR_AGILITY] += total_attr[i] * attr_cfg->minjie;
			temp_attrs[BATTLE_ATTR_RECOVERY] += total_attr[i] * attr_cfg->huifu;
			temp_attrs[BATTLE_ATTR_MENTAL] += total_attr[i] * attr_cfg->jingshen;
		}
	}

	if (pet_break_attr_cfg != NULL)
	{
		for (int i = 0; i < ADD_POINT_TYPE_MAX; ++i)
		{
			std::map<int, PetBreakAttrCfg::AttrItem>::const_iterator break_attr_iter = pet_break_attr_cfg->attr_item.find(i);
			if (break_attr_iter != pet_break_attr_cfg->attr_item.end())
			{
				const PetBreakAttrCfg::AttrItem & break_attr = break_attr_iter->second;
				// 血蓝
				temp_attrs[BATTLE_ATTR_MAX_HP] += total_attr[i] * break_attr.maxhp;
				temp_attrs[BATTLE_ATTR_MAX_MP] += total_attr[i] * break_attr.maxmp;
				// 基础属性
				temp_attrs[BATTLE_ATTR_ATTACK] += total_attr[i] * break_attr.gongji;
				temp_attrs[BATTLE_ATTR_DEFENSE] += total_attr[i] * break_attr.fangyu;
				temp_attrs[BATTLE_ATTR_AGILITY] += total_attr[i] * break_attr.minjie;
				temp_attrs[BATTLE_ATTR_RECOVERY] += total_attr[i] * break_attr.huifu;
				temp_attrs[BATTLE_ATTR_MENTAL] += total_attr[i] * break_attr.jingshen;
			}
		}
	}

	//////////////////////// 总副属性 = 基础属性 + 主属性提供的副属性////////////////////////////////////////////
	// 血蓝
	attr.m_attrs[BATTLE_ATTR_MAX_HP] += (Attribute)(cfg->maxhp + str_cfg->maxhp + (temp_attrs[BATTLE_ATTR_MAX_HP] / 100.0 + 1e-6));
	attr.m_attrs[BATTLE_ATTR_MAX_MP] += (Attribute)(cfg->maxmp + str_cfg->maxmp + (temp_attrs[BATTLE_ATTR_MAX_MP] / 100.0 + 1e-6));
	// 基础属性
	attr.m_attrs[BATTLE_ATTR_ATTACK] += (Attribute)(cfg->gongji + str_cfg->gongji + (temp_attrs[BATTLE_ATTR_ATTACK] / 100.0 + 1e-6));
	attr.m_attrs[BATTLE_ATTR_DEFENSE] += (Attribute)(cfg->fangyu + str_cfg->fangyu + (temp_attrs[BATTLE_ATTR_DEFENSE] / 100.0 + 1e-6));
	attr.m_attrs[BATTLE_ATTR_AGILITY] += (Attribute)(cfg->minjie + str_cfg->minjie + (temp_attrs[BATTLE_ATTR_AGILITY] / 100.0 + 1e-6));
	attr.m_attrs[BATTLE_ATTR_RECOVERY] += (Attribute)(cfg->huifu + str_cfg->huifu + (temp_attrs[BATTLE_ATTR_RECOVERY] / 100.0 + 1e-6));
	attr.m_attrs[BATTLE_ATTR_MENTAL] += (Attribute)(cfg->jingshen + str_cfg->jingshen + (temp_attrs[BATTLE_ATTR_MENTAL] / 100.0 + 1e-6));
	// 修正属性
	attr.m_attrs[BATTLE_ATTR_CRITICAL] += (Attribute)(cfg->bisha);
	attr.m_attrs[BATTLE_ATTR_HIT] += (Attribute)(cfg->mingzhong);
	attr.m_attrs[BATTLE_ATTR_DODGE] += (Attribute)(cfg->shanduo);
	attr.m_attrs[BATTLE_ATTR_COUNTER_ATTACK] += (Attribute)(cfg->fanji);
	attr.m_attrs[BATTLE_ATTR_MAGIC_ATTACK] += (Attribute)(cfg->mogong);
	attr.m_attrs[BATTLE_ATTR_MAGIC_DEFENSE] += (Attribute)(cfg->mokang);
	attr.m_attrs[BATTLE_ATTR_CRITICAL_INC_VALUE] += (Attribute)(cfg->bisha_dmg);
	attr.m_attrs[BATTLE_ATTR_CRITICAL_DEC_VALUE] += (Attribute)(cfg->jianren);
	// 异常抗性
	attr.m_attrs[BATTLE_ATTR_ANTI_POISON] += (Attribute)(cfg->def_zhongdu);
	attr.m_attrs[BATTLE_ATTR_ANTI_FREEZE] += (Attribute)(cfg->def_hunshui);
	attr.m_attrs[BATTLE_ATTR_ANTI_PETRIFY] += (Attribute)(cfg->def_shihua);
	attr.m_attrs[BATTLE_ATTR_ANTI_CHAOS] += (Attribute)(cfg->def_hunluan);
	attr.m_attrs[BATTLE_ATTR_ANTI_DRUNK] += (Attribute)(cfg->def_jiuzui);
	// 元素
	attr.m_attrs[BATTLE_ATTR_ELEMENT_WATER] += (Attribute)(cfg->element_water);
	attr.m_attrs[BATTLE_ATTR_ELEMENT_FIRE] += (Attribute)(cfg->element_fire);
	attr.m_attrs[BATTLE_ATTR_ELEMENT_WIND] += (Attribute)(cfg->element_wind);
	attr.m_attrs[BATTLE_ATTR_ELEMENT_EARTH] += (Attribute)(cfg->element_land);

	if (cfg->magic_pet_extra_attr_type == 1)
	{
		attr.m_attrs[BATTLE_ATTR_MAGIC_ATTACK] += (int)(1.0 * pet_param.level * 3.5);
	}

	if (cfg->magic_pet_extra_attr_type == 2)
	{
		attr.m_attrs[BATTLE_ATTR_MAGIC_ATTACK] += (int)(1.0 * pet_param.level * 2.1);
	}

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
		GetSpecialEffectAttr(spid, attr, base_attr_list, 32, pet_param.level);
	}
	/////////////////////////////////////////

	int sign_id1 = 0;
	int sign_id2 = 0;
	ItemDataWrapper item_info;
	grid_data.WrapTo(&item_info);
	ChatManager::Instance().AddChatItemInfo(&sign_id1, &sign_id2, item_info, &attr);

	Protocol::SCSystemMsg sm;
	int sendlen = 0;
	int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_system_post_pet_max, robot_name, sign_id1, sign_id2,
		pet_id, pet_cfg->quality);
	if (length > 0)
	{
		if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_SYSTEM_CHAT)))
		{
			World::GetInstWorld()->SendToAllGateway((const char*)&sm, sendlen);
		}
	}	

	grid_data.Clear();
}

int Pet::SetStrengthenBuffPet(int buff_grid_index, int pet_index)
{
	const ItemGridData * grid = this->GetPetGridData(pet_index);
	if (!grid)
	{
		ret_line;
	}
	const PetParam & p = grid->param->pet_param;
	long long item_unique_id = grid->item_unique_id;
	int str_level = p.str_level;

	int type_index = this->GetStrengthenBuffTypeIndex(grid->item_id);
	if (type_index < 0 || type_index >= ARRAY_LENGTH(m_strengthen_buff_param.strengthen_buff_item_list))
	{
		ret_line;
	}

	PetStrengthenBuffParamItem & type_item = m_strengthen_buff_param.strengthen_buff_item_list[type_index];
	if (buff_grid_index < 0 || buff_grid_index >= ARRAY_LENGTH(type_item.grid_pet_uuid))
	{
		ret_line;
	}

	for (int i = 0; i < ARRAY_LENGTH(type_item.grid_pet_uuid); ++i)
	{
		if (type_item.grid_pet_uuid[i] == item_unique_id)
		{
			ret_line;
		}
	}

	int buff_level = this->GetStrengthenBuffLevel(type_index);
	
	long long old_pet_uuid = type_item.grid_pet_uuid[buff_grid_index];
	type_item.grid_pet_uuid[buff_grid_index] = grid->item_unique_id;

	int new_buff_level = this->CalStrengthenBuffLevel(type_index);
	type_item.strengthen_buff_level = std::max(type_item.strengthen_buff_level, new_buff_level);
	
	this->ReCalcAttrByItemUniqueId(old_pet_uuid);

	if (new_buff_level == buff_level)
	{
		this->ReCalcAttrByItemUniqueId(grid->item_unique_id);
	}
	else
	{
		for (int index = 0; index < ARRAY_LENGTH(type_item.grid_pet_uuid); ++index)
		{
			this->ReCalcAttrByItemUniqueId(type_item.grid_pet_uuid[index]);
			
		}
	}

	this->SendInfoStrengthenBuff();
	return 0;
}

int Pet::UnSetStrengthenBuffPet(int type_index, int index)
{
	if (type_index < 0 || type_index >= ARRAY_LENGTH(m_strengthen_buff_param.strengthen_buff_item_list))
	{
		ret_line;
	}

	PetStrengthenBuffParamItem & type_item = m_strengthen_buff_param.strengthen_buff_item_list[type_index];
	if (index < 0 || index >= ARRAY_LENGTH(type_item.grid_pet_uuid))
	{
		ret_line;
	}

	int buff_level = this->GetStrengthenBuffLevel(type_index);

	long long old_pet_uuid = type_item.grid_pet_uuid[index];
	type_item.grid_pet_uuid[index] = 0;

	int new_buff_level = this->CalStrengthenBuffLevel(type_index);

	this->ReCalcAttrByItemUniqueId(old_pet_uuid);

	if (new_buff_level != buff_level)
	{
		for (int index = 0; index < ARRAY_LENGTH(type_item.grid_pet_uuid); ++index)
		{
			this->ReCalcAttrByItemUniqueId(type_item.grid_pet_uuid[index]);

		}
	}

	this->SendInfoStrengthenBuff();
	return 0;
}

int Pet::UnSetStrengthenBuffPetByItemUniqueId(long long item_unique_id)
{
	for (int type_index = 0; type_index < ARRAY_LENGTH(m_strengthen_buff_param.strengthen_buff_item_list); ++type_index)
	{
		PetStrengthenBuffParamItem & type_item = m_strengthen_buff_param.strengthen_buff_item_list[type_index];

		int buff_level = this->GetStrengthenBuffLevel(type_index);
		bool is_find = false;
		for (int i = 0; i < ARRAY_LENGTH(type_item.grid_pet_uuid); ++i)
		{
			if (type_item.grid_pet_uuid[i] == item_unique_id)
			{
				is_find = true;
				type_item.grid_pet_uuid[i] = 0;
				break;
			}
		}

		if (!is_find)
		{
			continue;
		}

		this->SendInfoStrengthenBuff();

		int new_buff_level = this->CalStrengthenBuffLevel(type_index);
		if (new_buff_level != buff_level)
		{
			for (int index = 0; index < ARRAY_LENGTH(type_item.grid_pet_uuid); ++index)
			{
				this->ReCalcAttrByItemUniqueId(type_item.grid_pet_uuid[index]);
			}
		}		
	}

	return 0;
}

void Pet::SendInfoStrengthenBuff()
{
	Protocol::SCPetStrengthenBuffInfo protocol;
	SameArrayTypeCopy(protocol.strengthen_buff_item_list, m_strengthen_buff_param.strengthen_buff_item_list);
	m_role_module_mgr->NetSend(&protocol, sizeof(protocol));
}

int Pet::GetStrengthenBuffLevel(int type_index)
{
	const int * level_ptr = MapValuesConstPtr(m_strengthen_buff_type_index_to_strengthen_buff_level, type_index);

	if (!level_ptr)
	{
		return this->CalStrengthenBuffLevel(type_index);		
	}

	return *level_ptr;
}

int Pet::CalStrengthenBuffLevel(int type_index)
{
	int strengthen_buff_level = 0;
	if (type_index < 0 || type_index >= ARRAY_LENGTH(m_strengthen_buff_param.strengthen_buff_item_list))
	{
		return strengthen_buff_level;
	}

	m_strengthen_buff_type_index_to_strengthen_buff_level[type_index] = strengthen_buff_level;

	PetStrengthenBuffParamItem & type_item = m_strengthen_buff_param.strengthen_buff_item_list[type_index];

	std::vector<int> strengthen_level_list;
	strengthen_level_list.reserve(PET_STRENGTHEN_BUFF_GRID_NUM);

	for (int i = 0; i < ARRAY_LENGTH(type_item.grid_pet_uuid); ++i)
	{
		long long item_uuid = type_item.grid_pet_uuid[i];
		if (item_uuid == 0)
		{
			continue;
		}
		int strengthen_level = this->GetPetStrengthenLevelByItemUniqueId(item_uuid);
		strengthen_level_list.push_back(strengthen_level);
	}

	if ((int)strengthen_level_list.size() < STRENGTHEN_BUFF_NEED_PET_NUM)
	{
		return strengthen_buff_level;
	}

	std::sort(strengthen_level_list.begin(), strengthen_level_list.end(), std::greater<int>());
	strengthen_buff_level = strengthen_level_list[STRENGTHEN_BUFF_NEED_PET_NUM - 1];
	strengthen_buff_level = std::max(1, strengthen_buff_level);
	m_strengthen_buff_type_index_to_strengthen_buff_level[type_index] = strengthen_buff_level;
	return strengthen_buff_level;
}

int Pet::GetPetStrengthenLevelByItemUniqueId(long long item_unique_id)
{
	const PetParam * p = this->GetPetParamByItemUniqueId(item_unique_id);
	if (p)
	{
		return p->str_level;
	}

	return 0;
}

int Pet::GetStrengthenBuffTypeIndex(int pet_id)
{
	const PetCfg * cfg = LOGIC_CONFIG->GetPetConfig()->GetPetCfg(pet_id);
	if (!cfg)
	{
		return -1;
	}

	const PetStrengthenBuffCfg * str_buff_cfg = LOGIC_CONFIG->GetPetConfig()->GetPetStrengthenBuffCfg(cfg->quality);
	if (!str_buff_cfg)
	{
		return -1;
	}

	return str_buff_cfg->pet_strengthen_buff_type;
}

void Pet::CheckStrengthenBuff(int pet_index)
{
	const PetParam * p = this->GetPetParam(pet_index);
	if (!p)
	{
		return;
	}

	bool is_in_buff_list = false;
	int buff_level = this->GetStrBuffLevel(pet_index, &is_in_buff_list);
	if (!is_in_buff_list)
	{
		return;
	}

	if (p->str_level < buff_level)
	{
		return;
	}

	int type_index = this->GetStrengthenBuffTypeIndex(p->pet_id);
	if (type_index < 0 || type_index >= ARRAY_LENGTH(m_strengthen_buff_param.strengthen_buff_item_list))
	{
		return;
	}

	int new_buff_level = this->CalStrengthenBuffLevel(type_index);
	if (new_buff_level == buff_level)
	{
		return;
	}

	PetStrengthenBuffParamItem & type_item = m_strengthen_buff_param.strengthen_buff_item_list[type_index];
	for (int index = 0; index < ARRAY_LENGTH(type_item.grid_pet_uuid); ++index)
	{
		this->ReCalcAttrByItemUniqueId(type_item.grid_pet_uuid[index]);
	}
	type_item.strengthen_buff_level = std::max(type_item.strengthen_buff_level, new_buff_level);
	this->SendInfoStrengthenBuff();
}

void Pet::GetStrengthenBuffSpId(int pet_index, int max_other_sp_num, short * out_other_sp_num, int * out_other_sp_list) 
{
	const PetParam * p = this->GetPetParam(pet_index);
	if (!p)
	{
		return;
	}

	int str_buff_level = this->GetStrBuffLevel(pet_index);
	const PetCfg * pet_cfg = LOGIC_CONFIG->GetPetConfig()->GetPetCfg(p->pet_id);
	if (!pet_cfg)
	{
		return;
	}

	const PetStrengthenBuffItemCfg * str_buff_cfg = LOGIC_CONFIG->GetPetConfig()->GetPetStrengthenBuffCfg(pet_cfg->quality, str_buff_level);
	if (!str_buff_cfg)
	{
		return;
	}

	for (int i = 0; i < (int)str_buff_cfg->sp_id_list.size(); ++i)
	{
		if (*out_other_sp_num >= max_other_sp_num)
		{
			break;
		}

		out_other_sp_list[*out_other_sp_num] = str_buff_cfg->sp_id_list[i];
		*out_other_sp_num += 1;
	}	
}

int Pet::PetGaiMingUpLevel(int pet_index,int times, bool is_break)
{
	bool need_send = false;

	if (is_break)
	{
		times = 1;
	}

	for (int i = 0; i < times && i < 100; ++i)
	{
		if (this->PetGaiMingUpLevelHelper(pet_index, is_break) != 0)
		{
			break;
		}

		need_send = true;
	}

	if (need_send)
	{
		this->SendSingleInfo(pet_index, Protocol::SCSendPetSingleInfo::PET_NOTIE_TYPE_GAI_MING);
	}	

	return 0;
}

int Pet::PetGaiMingUpLevelHelper(int pet_index, bool is_break)
{
	PetParam * p = this->__GetPetParam(pet_index);
	if (!p)
	{
		ret_line;
	}

	const PetGaiMingLevelCfg * cfg = LOGIC_CONFIG->GetPetGaiMingConfig()->GetPetGaiMingLevelCfg(p->pet_gai_ming_level);
	if (!cfg)
	{
		ret_line;
	}

	const PetGaiMingLevelCfg * next_level_cfg = LOGIC_CONFIG->GetPetGaiMingConfig()->GetPetGaiMingLevelCfg(p->pet_gai_ming_level + 1);
	if (!next_level_cfg)
	{
		ret_line;
	}

	const PetGaiMingOtherCfg & other_cfg = LOGIC_CONFIG->GetPetGaiMingConfig()->GetOtherCfg();

	if (p->pet_gai_ming_exp >= cfg->max_exp)
	{
		if (cfg->need_break)
		{
			if (!is_break)
			{
				ret_line;
			}

			if (!m_role_module_mgr->GetKnapsack()->ConsumeItem(cfg->break_item_id, cfg->break_item_num, __FUNCTION__))
			{
				ret_line;
			}
		}

		p->pet_gai_ming_level += 1;
		p->pet_gai_ming_exp -= cfg->max_exp;
	}
	else
	{
		if (is_break)
		{
			ret_line;
		}

		if (!m_role_module_mgr->GetKnapsack()->ConsumeItem(other_cfg.up_level_item_id, 1, __FUNCTION__))
		{
			ret_line;
		}

		p->pet_gai_ming_exp += other_cfg.add_exp;
	}

	for (int i = 0; i < 1000; ++i)
	{
		cfg = LOGIC_CONFIG->GetPetGaiMingConfig()->GetPetGaiMingLevelCfg(p->pet_gai_ming_level);
		if (!cfg)
		{
			break;
		}

		next_level_cfg = LOGIC_CONFIG->GetPetGaiMingConfig()->GetPetGaiMingLevelCfg(p->pet_gai_ming_level + 1);
		if (!next_level_cfg)
		{
			break;
		}

		if (p->pet_gai_ming_exp < cfg->max_exp)
		{
			break;
		}

		if (cfg->need_break)
		{
			break;
		}

		p->pet_gai_ming_level += 1;
		p->pet_gai_ming_exp -= cfg->max_exp;
	}

	const PetParam & pet = *p;
	gamelog::g_log_pet.printf(LL_INFO, "%s role[%d,%s] %s pet_index:%d pet_gai_ming_level:%d pet_gai_ming_exp:%d", __FUNCTION__, m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(),
		pet.GetPetBaseInfoString(this->GetPetUniqueId(pet_index)).c_str(), pet_index, pet.pet_gai_ming_level, pet.pet_gai_ming_exp);

	this->SetItemDirty(pet_index, false);

	return 0;
}

int Pet::PetGaiMingAttrUpLevel(int pet_index, int attr_index)
{
	PetParam * p = this->__GetPetParam(pet_index);
	if (!p)
	{
		ret_line;
	}

	if (attr_index < 0 || attr_index >= ARRAY_LENGTH(p->pet_gai_ming_attr_level))
	{
		ret_line;
	}

	int count = 0;
	for (int i = 0; i < ARRAY_LENGTH(p->pet_gai_ming_attr_level); ++i)
	{
		count += p->pet_gai_ming_attr_level[i];
	}

	if (count >= p->pet_gai_ming_level)
	{
		ret_line;
	}

	const PetGaiMingOtherCfg & other_cfg = LOGIC_CONFIG->GetPetGaiMingConfig()->GetOtherCfg();
	if (p->pet_gai_ming_attr_level[attr_index] >= other_cfg.attr_max_level)
	{
		ret_line;
	}

	p->pet_gai_ming_attr_level[attr_index] += 1;
	this->ReCalcAttr(pet_index);
	this->SetItemDirty(pet_index, true, Protocol::SCSendPetSingleInfo::PET_NOTIE_TYPE_GAI_MING_ATTR);

	const PetParam & pet = *p;
	gamelog::g_log_pet.printf(LL_INFO, "%s role[%d,%s] %s pet_index:%d attr_index:%d attr_level:%d", __FUNCTION__, m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(),
		pet.GetPetBaseInfoString(this->GetPetUniqueId(pet_index)).c_str(), pet_index, attr_index, pet.pet_gai_ming_attr_level[attr_index]);

	return 0;
}

int Pet::PetGaiMingAttrReset(int pet_index)
{

	PetParam * p = this->__GetPetParam(pet_index);
	if (!p)
	{
		ret_line;
	}

	const PetGaiMingLevelCfg * cfg = LOGIC_CONFIG->GetPetGaiMingConfig()->GetPetGaiMingLevelCfg(p->pet_gai_ming_level);
	if (!cfg)
	{
		ret_line;
	}

	if (!m_role_module_mgr->GetKnapsack()->ConsumeItem(cfg->reset_item_id, cfg->reset_item_num, __FUNCTION__))
	{
		ret_line;
	}

	ArrayFillZero(p->pet_gai_ming_attr_level);
	this->ReCalcAttr(pet_index);
	this->SetItemDirty(pet_index, true, Protocol::SCSendPetSingleInfo::PET_NOTIE_TYPE_GAI_MING_ATTR_RESET);

	const PetParam & pet = *p;
	gamelog::g_log_pet.printf(LL_INFO, "%s role[%d,%s] %s pet_index:%d", __FUNCTION__, m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(),
		pet.GetPetBaseInfoString(this->GetPetUniqueId(pet_index)).c_str(), pet_index);

	return 0;
}

void Pet::PetGaiMingReturn(int pet_index,const char * reason)
{
	PetParam * pet_ptr = this->__GetPetParam(pet_index);
	if (!pet_ptr)
	{
		return;
	}

	if (!reason)
	{
		reason = "";
	}

	PetParam & pet = *pet_ptr;

	std::vector<ItemConfigData> gai_ming_return;		//改命物品返还
	int gai_ming_total_exp = pet.pet_gai_ming_exp;
	for (int gai_ming_level = 0; gai_ming_level < pet.pet_gai_ming_level; ++gai_ming_level)
	{
		const PetGaiMingLevelCfg * cfg = LOGIC_CONFIG->GetPetGaiMingConfig()->GetPetGaiMingLevelCfg(gai_ming_level);
		if (!cfg)
		{
			continue;
		}

		gai_ming_total_exp += cfg->max_exp;
		if (cfg->need_break)
		{
			ItemConfigData item_config;
			item_config.item_id = cfg->break_item_id;
			item_config.num = cfg->break_item_num;
			item_config.is_bind = true;
			gai_ming_return.push_back(item_config);
		}
	}
	const PetGaiMingOtherCfg & gai_ming_other_cfg = LOGIC_CONFIG->GetPetGaiMingConfig()->GetOtherCfg();
	int gai_ming_up_level_item_num = gai_ming_total_exp / gai_ming_other_cfg.add_exp;
	ItemConfigData gai_ming_up_level_item;
	gai_ming_up_level_item.item_id = gai_ming_other_cfg.up_level_item_id;
	gai_ming_up_level_item.num = gai_ming_up_level_item_num;
	gai_ming_up_level_item.is_bind = true;
	gai_ming_return.push_back(gai_ming_up_level_item);

	::MergeItemConfigData(gai_ming_return);

	for (int i = 0; i < (int)gai_ming_return.size(); ++i)
	{
		ItemConfigData & curr = gai_ming_return[i];
		curr.num = ::ceil((double)curr.num * gai_ming_other_cfg.return_percent / 100.0);
	}

	if (!m_role_module_mgr->GetKnapsack()->PutVec(gai_ming_return, PUT_REASON_PET_ABANDONMENT_GAI_MING))
	{
		m_role_module_mgr->GetKnapsack()->SendMail(gai_ming_return, SNED_MAIL_TYPE_DEFAULT, true);
	}


	gamelog::g_log_pet.printf(LL_INFO, "%s reason:%s role[%d,%s] %s pet_index:%d pet_gai_ming_level:%d pet_gai_ming_exp:%d", __FUNCTION__, reason,m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(),
		pet.GetPetBaseInfoString(this->GetPetUniqueId(pet_index)).c_str(), pet_index, pet.pet_gai_ming_level, pet.pet_gai_ming_exp);

	pet.pet_gai_ming_level = 0;
	pet.pet_gai_ming_exp = 0;
	ArrayFillZero(pet.pet_gai_ming_attr_level);
}

int Pet::GetPetIndexByItemUniqueId(long long item_unique_id)
{
	int index = -1;
	bool is_in_storge = false;
	const PetParam * p = this->GetPetParamByItemUniqueId(item_unique_id, &index, &is_in_storge);
	if (!p)
	{
		return -1;
	}

	if (is_in_storge)
	{
		return -1;
	}

	return index;
}

const PetParam * Pet::GetPetParamByItemUniqueId(long long item_unique_id, int * out_pet_index, bool * out_is_in_storge)
{
	if (item_unique_id == 0)
	{
		return NULL;
	}

	int * pet_index_ptr = MapValuesPtr(unique_item_id_to_pet_index, item_unique_id);
	if (pet_index_ptr )
	{
		int tmp_index = *pet_index_ptr;
	
		if (this->GetPetUniqueId(tmp_index) == item_unique_id)
		{
			if (out_pet_index)
			{
				*out_pet_index = tmp_index;
			}

			if (out_is_in_storge)
			{
				*out_is_in_storge = false;
			}

			return this->__GetPetParam(tmp_index);
		}
	}

	int * pet_storage_index_ptr = MapValuesPtr(unique_item_id_to_pet_storage_index, item_unique_id);
	if (pet_storage_index_ptr)
	{
		int tmp_index = *pet_storage_index_ptr;

		if (this->GetPetUniqueId(tmp_index) == item_unique_id)
		{
			if (out_pet_index)
			{
				*out_pet_index = tmp_index;
			}

			if (out_is_in_storge)
			{
				*out_is_in_storge = true;
			}

			return this->GetPetParamFromStorageNoConst(tmp_index);
		}
	}

	if (!pet_index_ptr && !pet_storage_index_ptr)
	{
		return NULL;
	}

	for (int i = 0; i < ARRAY_LENGTH(m_grid_list); ++i)
	{
		if (m_grid_list[i].Invalid())
		{
			continue;
		}

		if (m_grid_list[i].item_unique_id != item_unique_id)
		{
			continue;
		}

		if (out_pet_index)
		{
			*out_pet_index = i;
		}

		if (out_is_in_storge)
		{
			*out_is_in_storge = false;
		}

		unique_item_id_to_pet_index[item_unique_id] = i;
		unique_item_id_to_pet_storage_index.erase(item_unique_id);

		return &m_grid_list[i].param->pet_param;
	}

	for (int i = 0; i < ARRAY_LENGTH(m_storage_grid_list); ++i)
	{
		if (m_storage_grid_list[i].Invalid())
		{
			continue;
		}

		if (m_storage_grid_list[i].item_unique_id != item_unique_id)
		{
			continue;
		}

		if (out_pet_index)
		{
			*out_pet_index = i;
		}

		if (out_is_in_storge)
		{
			*out_is_in_storge = true;
		}

		unique_item_id_to_pet_storage_index[item_unique_id] = i;
		unique_item_id_to_pet_index.erase(item_unique_id);

		return &m_storage_grid_list[i].param->pet_param;
	}

	unique_item_id_to_pet_index.erase(item_unique_id);
	unique_item_id_to_pet_storage_index.erase(item_unique_id);

	return NULL;
}

bool Pet::CanDeletePet(int pet_index, const char * reson, unsigned int exclude_flag)
{
	//long long uuid = this->GetPetUniqueId(pet_index);

	if (!IsSetBit(exclude_flag, PET_CAN_DELETE_EXCLUDE_PLATFORM_BATTLE))
	{
		if (this->IsInPlatfformBattle(this->GetPetUniqueId(pet_index), true))			//消耗的宠物不能在擂台上
		{
			return  false;
		}
	}

	if (!m_role_module_mgr->GetSmartMountsSystem()->OnCancelDominancePet(pet_index))		//消耗的宠物不能处于被统御状态
	{
		m_role_module_mgr->NoticeNum(errornum::EN_SMART_MOUNTS_SYSTEM_PET_DOMINATION);
		return  false;
	}

	PetParam * consume_pet_param = this->__GetPetParam(pet_index);
	if (!consume_pet_param)
	{
		return  false;
	}

	if (!this->PutOffPetEquipment(pet_index))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_PET_HSA_EQUIPMENT_ERR);
		return false;
	}

	if (consume_pet_param->HasGodPrint())//消耗宠物不能穿戴神印
	{
		if (!this->PutOffAllPetGodPrintEquip(pet_index, true))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_PET_WEAR_GOD_PRINT);
			return  false;
		}
	}

	if (this->PetBadgeTakeOff(pet_index) != 0)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_PET_BADGE_TAKE_OFF_FAIL);
		return  false;
	}

	if (!IsSetBit(exclude_flag, PET_CAN_DELETE_EXCLUDE_FLAG_PET_SOUL))
	{
		if (!this->PutOffAllPetSoulEquip(pet_index))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_PET_WEAR_SOUL_EQUIP);
			return  false;
		}
	}

	if (reson == NULL)
	{
		reson = __FUNCTION__;
	}

	if (this->ConversionExp(pet_index, 1, reson) != 0)
	{
		return  false;
	}

	this->OnCancelBenediction(pet_index);
	this->OnPetBenedictionReset(pet_index);
	this->SetPetState(pet_index, SET_REST);

	return true;
}

void Pet::CheckPetParamOffsetBeforeInit(int pet_id, long long pet_unique_id, NetValueItemParam* param)
{
	if (pet_id != param->pet_param.pet_id && !this->IsPetParamDataNormal(pet_id, param) && this->IsInOffsetErrorUserList(LocalConfig::Instance().GetPlatType(), m_role_module_mgr->GetUid()))
	{
		gamelog::g_log_pet.printf(LL_INFO, "Pet::CheckPetParamOffsetBeforeInit__1 role[%d,%s] pet_id[%d] unique_id[%lld] pet_data[%s]",
			m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), pet_id, pet_unique_id, param->GetParamLog(I_TYPE_PET));

		NetValueItemParam new_param;
		memcpy(new_param.data + sizeof(int), param->data, sizeof(new_param.data) - sizeof(int));
		memcpy(new_param.data, (const char*)&pet_id, sizeof(pet_id));
		new_param.pet_param.name[sizeof(GameName) - 1] = 0;
		if (new_param.pet_param.exp < 0)
		{
			new_param.pet_param.exp = 0;
		}
		if (this->IsPetParamDataNormal(pet_id, &new_param))
		{
			param->SetData(new_param.data);

			gamelog::g_log_pet.printf(LL_INFO, "Pet::CheckPetParamOffsetBeforeInit__2 role[%d,%s] pet_id[%d] unique_id[%lld] pet_data[%s]",
				m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), pet_id, pet_unique_id, param->GetParamLog(I_TYPE_PET));
		}
	}
}

bool Pet::IsPetParamDataNormal(int pet_id, NetValueItemParam* param)
{
	const PetCfg* pet_cfg = LOGIC_CONFIG->GetPetConfig()->GetPetCfg(pet_id);
	if (NULL == pet_cfg)
	{
		return false;
	}

	// 如果名字与配置相同，那么断定一定没有问题
	if (strncmp(pet_cfg->name, param->pet_param.name, sizeof(GameName)) == 0)
	{
		return true;
	}

	if (param->pet_param.pet_id != pet_id)
	{
		return false;
	}

	if (param->pet_param.str_level < 0 || param->pet_param.str_level > 30)
	{
		return false;
	}

	if (param->pet_param.level <= 0 || param->pet_param.level > MAX_ROLE_LEVEL)
	{
		return false;
	}

	return true;
}

bool Pet::IsInOffsetErrorUserList(int plat_type, int role_id)
{
	return LOGIC_CONFIG->GetPetConfig()->IsInCheckParamUserList(plat_type, role_id);
}

void Pet::OnPetGridInfoReset(int pet_index)
{
	bool is_reset = false;
	CommonDataParam & common_data = m_role_module_mgr->GetCommonData();
	for (int i = 0; i < NEW_PET_STATUS_MAX; ++i)
	{
		if (pet_index == common_data.new_pet_status_index[i])
		{
			common_data.new_pet_status_index[i] = -1;
			m_common_param.pet_attr_list[i].Reset();
			switch (i)
			{
			case NEW_FIGHT_INDEX:
			{
				m_common_param.cur_battle_fight_cap = 0;
			}
			break;
			case NEW_STANDBY_INDEX1:
			{
				m_common_param.curr_battle_standby_index1_cap = 0;
			}
			break;
			case NEW_STANDBY_INDEX2:
			{
				m_common_param.curr_battle_standby_index2_cap = 0;
			}
			break;
			case NEW_STANDBY_INDEX3:
			{
				m_common_param.curr_battle_standby_index3_cap = 0;
			}
			break;
			case NEW_STANDBY_INDEX4:
			{
				m_common_param.curr_battle_standby_index4_cap = 0;
			}
			break;
			case NEW_STANDBY_INDEX5:
			{
				m_common_param.curr_battle_standby_index5_cap = 0;
			}
			break;
			default:
				break;
			}
			is_reset = true;
		}
	}
	if (is_reset)
	{
		this->SendPetOtherInfo();
	}

	if (m_common_param.curr_battle_fight_index == pet_index) m_common_param.curr_battle_fight_index = -1;
	if (m_common_param.curr_battle_standby_index1 == pet_index) m_common_param.curr_battle_standby_index1 = -1;
	if (m_common_param.curr_battle_standby_index2 == pet_index) m_common_param.curr_battle_standby_index2 = -1;
	if (m_common_param.curr_battle_standby_index3 == pet_index) m_common_param.curr_battle_standby_index3 = -1;
	if (m_common_param.curr_battle_standby_index4 == pet_index) m_common_param.curr_battle_standby_index4 = -1;
	if (m_common_param.curr_battle_standby_index5 == pet_index) m_common_param.curr_battle_standby_index5 = -1;

	// 如果该宠物在家族训练则清掉
	for (int i = 0; i < GUILD_PET_TRAINING_PET_MAX_COUNT; ++i)
	{
		if (pet_index == m_role_module_mgr->GetGuildCommonData().guild_training_pet_list[i].pet_index)
		{
			m_role_module_mgr->GetGuildCommonData().guild_training_pet_list[i].Reset();
			GuildManager::Instance().SendPetTrainingInfo(m_role_module_mgr->GetRole());
		}
	}
}

void Pet::DeletePet(int pet_index, const char* reason)
{
	PetParam * pet_param = this->__GetPetParam(pet_index);
	if (!pet_param)
	{
		return;
	}

	this->OnPetGridInfoReset(pet_index);
	this->OnPetBenedictionReset(pet_index);
	this->PetBadgeTakeOff(pet_index);

	int pet_id = m_grid_list[pet_index].item_id;
	long long unique_id = m_grid_list[pet_index].item_unique_id;
	m_pet_count--;
	//魂器脱下，神印脱下

	if (pet_param->HasGodPrint())
	{
		this->PutOffAllPetGodPrintEquip(pet_index);
	}
	this->PutOffAllPetSoulEquip(pet_index);
	this->OnAttrPelletRet(pet_index);
	this->OnPetTalentReset(pet_param);
	

	if (m_common_param.fei_sheng_pet_uuid == m_grid_list[pet_index].item_unique_id)
	{
		m_role_module_mgr->GetTaskManager()->ClearPetFeiShengTask();
	}

	gamelog::g_log_pet.printf(LL_INFO, "DeletePet user[%d %s] reason[%s] pet[%d] index:%d %s", m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
		reason, pet_id, pet_index, pet_param->ToString(unique_id).c_str());

	m_grid_list[pet_index].Clear();
	pet_param = NULL;

	this->SetItemDirty(pet_index, false);
	EventHandler::Instance().OnDeletePet(m_role_module_mgr->GetRole(), unique_id);

	Protocol::SCSendPetDeleteInfo info;
	info.pet_index = pet_index;
	m_role_module_mgr->NetSend((const char*)&info, sizeof(info));	

	unique_item_id_to_pet_index.erase(unique_id);
	unique_item_id_to_pet_storage_index.erase(unique_id);
	this->UnSetStrengthenBuffPetByItemUniqueId(unique_id);

	ROLE_LOG_QUICK6(LOG_TYPE_PET_REMOVE, m_role_module_mgr->GetRole(), pet_id, unique_id, reason, NULL);
}

void Pet::DeleteStoragePet(int pet_index, const char * reason)
{
	if (this->PetStorageInvalid(pet_index)) return;

	int pet_id = m_storage_grid_list[pet_index].item_id;
	long long unique_id = m_storage_grid_list[pet_index].item_unique_id;
	m_pet_storage_grid_use_num--;
	m_storage_grid_list[pet_index].Clear();
	this->SetPetStorageItemDirty(pet_index);
	EventHandler::Instance().OnDeletePet(m_role_module_mgr->GetRole(), unique_id);

	std::string log_str = STRING_SPRINTF("%s in storage", reason);
	gamelog::g_log_pet.printf(LL_INFO, "DeleteStoragePet user[%d %s] reason[%s] pet[%d] uuid:%lld",
				  m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
				  log_str.c_str(), pet_id, unique_id);

	unique_item_id_to_pet_index.erase(unique_id);
	unique_item_id_to_pet_storage_index.erase(unique_id);
	this->UnSetStrengthenBuffPetByItemUniqueId(unique_id);

	ROLE_LOG_QUICK6(LOG_TYPE_PET_REMOVE, m_role_module_mgr->GetRole(), pet_id, unique_id, log_str.c_str(), NULL);
}

PetParam* Pet::__GetPetParam(int pet_index)
{
	if (this->PetInvalid(pet_index)) return NULL;

	return &(m_grid_list[pet_index].param->pet_param);
}

PetParam * Pet::GetPetParamFromStorageNoConst(int pet_index)
{
	if (this->PetStorageInvalid(pet_index))
	{
		return NULL;
	}

	return &(m_storage_grid_list[pet_index].param->pet_param);
}

void Pet::RecordGetPet(unsigned short pet_id)
{
	if (m_common_param.pet_id_record_list_count >= ARRAY_LENGTH(m_common_param.pet_id_record_list))
	{
		return;
	}

	for (int i = 0; i < ARRAY_LENGTH(m_common_param.pet_id_record_list) && i < m_common_param.pet_id_record_list_count; ++i)
	{
		if (m_common_param.pet_id_record_list[i] == pet_id)
		{
			return;
		}
	}

	m_common_param.pet_id_record_list[m_common_param.pet_id_record_list_count++] = pet_id;
	this->SendPetIdRecordList();
}

void Pet::BuildPetSkinIdMap(bool is_re)
{
	if (!is_re && m_is_build_skin_id_map)
	{
		return;
	}

	m_is_build_skin_id_map = true;
	m_pet_has_skin_id.clear();
	for (int i = 0; i < m_pet_skin_param.pet_skin_count && i < ARRAY_LENGTH(m_pet_skin_param.pet_skin_list); ++i)
	{
		int skin_id = m_pet_skin_param.pet_skin_list[i];
		if (skin_id != 0)
		{
			const SkinPetCfg * skin_pet_cfg = LOGIC_CONFIG->GetSkinConfig()->GetSkinPetCfgBySkinId(skin_id);
			if (skin_pet_cfg != NULL)
			{
				for (std::set<int> ::iterator iter = skin_pet_cfg->can_wear_pet_list.begin(); iter != skin_pet_cfg->can_wear_pet_list.end(); ++iter)
				{
					int pet_id = *iter;
					m_pet_has_skin_id[pet_id].push_back(skin_id);
				}
			}
		}
	}
}

void Pet::ModifyAutoMoveOnAddPet(PetParam& pet_param)
{
	PetAutoMoveMap::iterator it = m_auto_move_map.find(pet_param.pet_id);
	if (it != m_auto_move_map.end())
	{
		pet_param.pet_auto_move_type = it->second.auto_move_type;
		pet_param.pet_auto_skill_id = it->second.auto_skill_id;
		pet_param.pet_auto_skill_level = it->second.auto_skill_lv;
	}
	else
	{
		const PetCfg* cfg = LOGIC_CONFIG->GetPetConfig()->GetPetCfg(pet_param.pet_id);
		if (NULL != cfg 
			&& cfg->exclusive_skills.size() > 0 
			&& PET_EXCLUSIVE_SKILL_TYPE_ZHU_DONG == cfg->exclusive_skills[0].type)
		{
			pet_param.pet_auto_move_type = BATTLE_MOVE_TYPE_PERFORM_SKILL;
			pet_param.pet_auto_skill_id = cfg->exclusive_skills[0].skill_id;
			pet_param.pet_auto_skill_level = cfg->exclusive_skills[0].skill_level;
		}
		else
		{
			pet_param.pet_auto_move_type = BATTLE_MOVE_TYPE_NORMAL_ATTACK;
			pet_param.pet_auto_skill_id = 0;
			pet_param.pet_auto_skill_level = 0;
		}
	}
}

void Pet::ModifyAutoMoveMap(int pet_id, short auto_move_type, short auto_skill_id, short auto_skill_level)
{
	PetAutoMoveMap::iterator it = m_auto_move_map.find(pet_id);
	if (it != m_auto_move_map.end())
	{
		it->second.auto_move_type = auto_move_type;
		it->second.auto_skill_id = auto_skill_id;
		it->second.auto_skill_lv = auto_skill_level;
	}
	else
	{
		PetAutoMove pam;
		pam.pet_id = pet_id;
		pam.auto_move_type = auto_move_type;
		pam.auto_skill_id = auto_skill_id;
		pam.auto_skill_lv = auto_skill_level;	
		pam.reserve_sh = 0;
		m_auto_move_map.insert(PetAutoMoveMap::value_type(pet_id, pam));
	}
}

bool Pet::CheckDuplicateUniqueID(long long unique_id, int exclude_index)
{
	for (int i = 0; i < ARRAY_LENGTH(m_grid_list); ++i)
	{
		if (exclude_index == i) continue;

		if (m_grid_list[i].Invalid()) continue;

		if (m_grid_list[i].item_unique_id == unique_id)
		{
			return false;
		}
	}

	return true;
}

int Pet::GetEmptyBag()
{
	for (int i = 0; i < NEW_MAX_PET_GOD_PRONT_BAG_NUM; i++)
	{
		if (NULL != m_new_god_print_bag[i] && !m_new_god_print_bag[i]->Invalid())continue;
		return i;
	}
	return -1;
}

int Pet::GetBagEmptyNum()
{
	int bag_num = 0;
	for (int i = 0; i < NEW_MAX_PET_GOD_PRONT_BAG_NUM; i++)
	{
		if (NULL != m_new_god_print_bag[i] && !m_new_god_print_bag[i]->Invalid())continue;
		bag_num += 1;
	}
	return bag_num;
}

void Pet::ClearGodPrintItem(int index, int clear_reason)
{
	if (index < 0 || index >= NEW_MAX_PET_GOD_PRONT_BAG_NUM)return;
	if (NULL == m_new_god_print_bag[index])return;
	delete m_new_god_print_bag[index];
	m_new_god_print_bag[index] = NULL;
}

void Pet::SetGodPrintDirtyMark(int index, int put_reason)
{
	if (index < 0 || index >= NEW_MAX_PET_GOD_PRONT_BAG_NUM) return;

	m_god_print_item_dirty_mark[index] = true;
	m_god_print_item_bag_is_change = true;
	this->SendSingleGodPrintBagInfo(index, put_reason);
}

void Pet::OnSetQueryAttrList(QueryAttrList & pet_query_attr, const AttributeList & pet_attr)
{
	pet_query_attr.attr_list[QUERY_ATTR_TYPE_MAX_HP] = pet_attr.m_attrs[BATTLE_ATTR_MAX_HP];
	pet_query_attr.attr_list[QUERY_ATTR_TYPE_MAX_MP] = pet_attr.m_attrs[BATTLE_ATTR_MAX_MP];
	pet_query_attr.attr_list[QUERY_ATTR_TYPE_ATTACK] = pet_attr.m_attrs[BATTLE_ATTR_ATTACK];
	pet_query_attr.attr_list[QUERY_ATTR_TYPE_DEFENSE] = pet_attr.m_attrs[BATTLE_ATTR_DEFENSE];
	pet_query_attr.attr_list[QUERY_ATTR_TYPE_AGILITY] = pet_attr.m_attrs[BATTLE_ATTR_AGILITY];
	pet_query_attr.attr_list[QUERY_ATTR_TYPE_MENTAL] = pet_attr.m_attrs[BATTLE_ATTR_MENTAL];
	pet_query_attr.attr_list[QUERY_ATTR_TYPE_RECOVERY] = pet_attr.m_attrs[BATTLE_ATTR_RECOVERY];
}

void Pet::RecalePetSoul(long long add_exp, short * color, short * level, short * steps, int * m_exp, int * m_max_exp, unsigned short * attr_num, AttrInfo * attr_list[MAX_PET_SOUL_EQUIPMENT_ATTR_NUM])
{
	if (NULL == color || 
		NULL == level ||
		NULL == steps ||
		NULL == m_exp ||
		NULL == m_max_exp ||
		NULL == attr_num)return;

	int suppose_level = *level;
	int suppose_exp = *m_exp;
	int suppose_max_exp = *m_max_exp;
	int suppose_steps = *steps;
	int suppose_cost_exp = 0;
	int extra_exp = 0;//额外多余经验
	for (int round = 0; round < 50; round++)//先假设升级
	{
		if (add_exp + suppose_exp >= suppose_max_exp)
		{
			//满足升级了检测一下
			const AttributeUplevelCfg * pet_soul_level_cfg = LOGIC_CONFIG->GetPetSoulEquipMentConfig()->GetPetSoulEquipUpLevelCfg(*color, suppose_level);
			if (pet_soul_level_cfg != NULL)
			{
				if (suppose_steps < pet_soul_level_cfg->need_steps)
				{
					break;
				}

				int real_cost_exp = suppose_max_exp - suppose_exp;

				//检测下一级是否存在
				const AttributeUplevelCfg * next_pet_soul_level_cfg = LOGIC_CONFIG->GetPetSoulEquipMentConfig()->GetPetSoulEquipUpLevelCfg(*color, suppose_level + 1);
				if (next_pet_soul_level_cfg != NULL)
				{
					add_exp -= real_cost_exp;
					suppose_cost_exp += real_cost_exp;
					suppose_exp = 0;
					suppose_level = next_pet_soul_level_cfg->levels;
					suppose_max_exp = next_pet_soul_level_cfg->exp;
				}
				else//没有下一级了
				{
					suppose_cost_exp += suppose_max_exp - suppose_exp;
					suppose_exp = suppose_max_exp;
					extra_exp = add_exp;//超等级额外经验返还
					add_exp = 0;
					break;
				}
			}
		}
		else
		{
			break;
		}
	}

	if (add_exp > 0)//还有剩余的经验加上
	{
		if (add_exp + suppose_exp >= suppose_max_exp)
		{
			suppose_cost_exp += suppose_max_exp - suppose_exp;
			suppose_exp = suppose_max_exp;
			extra_exp += add_exp - (suppose_max_exp - suppose_exp);//超等级额外经验返还
		}
		else
		{
			suppose_exp += add_exp;
			suppose_cost_exp += add_exp;
		}
	}

	for (int level_next = *level; level_next < suppose_level; level_next++)
	{
		//随机增加属性
		for (int attr_idx = 0; attr_idx < *attr_num && attr_idx < MAX_PET_SOUL_EQUIPMENT_ATTR_NUM; attr_idx++)
		{
			if (attr_list[attr_idx] != NULL)
			{
				const AttributeIntensifyValueCfg * attr_cfg = LOGIC_CONFIG->GetPetSoulEquipMentConfig()->GetPetSoulEquipIntensifyValueCfg(*color, attr_list[attr_idx]->attr_type);
				if (attr_cfg != NULL)
				{
					int rand_attr = 0;
					if ((attr_cfg->intensify_att_max - attr_cfg->intensify_att_min + 1) != 0)
					{
						rand_attr = rand() % (attr_cfg->intensify_att_max - attr_cfg->intensify_att_min + 1) + attr_cfg->intensify_att_min;
					}
					else
					{
						rand_attr = attr_cfg->intensify_att_min;
					}
					attr_list[attr_idx]->attr_value += rand_attr;
				}
			}
		}
	}

	*level = suppose_level;
	*m_max_exp = suppose_max_exp;
	*m_exp = suppose_exp;
}

PetParam* Pet::MutablePetParam(int pet_id, long long unique_item_id, ARG_OUT int* pet_idx)
{
	for (int i = 0; i < ARRAY_LENGTH(m_grid_list); ++i)
	{
		if (this->PetInvalid(i)) continue;

		ItemGridData& pet_grid = m_grid_list[i];
		if (pet_grid.item_unique_id == unique_item_id)
		{
			// 只要原型一样，即可
			if (LOGIC_CONFIG->GetPetConfig()->IsSamePrototypePet(pet_grid.param->pet_param.pet_id, pet_id))
			{
				if (NULL != pet_idx)
				{
					*pet_idx = i;
				}

				return &pet_grid.param->pet_param;
			}
		}
	}

	return NULL;
}
