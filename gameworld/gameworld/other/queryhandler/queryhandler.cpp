#include "queryhandler.hpp"
#include "rmibackobjdef.h"
#include "world.h"
#include "internalcomm.h"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "other/vip/vip.hpp"
#include "other/pet/pet.hpp"
#include "other/rolecross/rolecross.hpp"
#include "other/surface/surface.hpp"
#include "other/facescore/facescore.hpp"
#include "other/facescore/facescoreconfig.hpp"
#include "other/partner/partnerconfig.hpp"
#include "other/partner/partner.hpp"
#include "other/huashen/huashenmanager.hpp"
#include "other/duel/duel.hpp"
#include "config/logicconfigmanager.hpp"
#include "config/otherconfig/smartmountssystemconfig.hpp"
#include "item/itempool.h"
#include "global/guild/guild.hpp"
#include "global/rank/personrank.hpp"
#include "global/rank/rankmanager.hpp"
#include "global/guild/guildmanager.hpp"
#include "global/cross/crossmanager.h"
#include "global/cross/crossuserregister.hpp"
#include "global/offlineregister/offlineregister.hpp"
#include "global/offlineregister/offlineregistermanager.hpp"
#include "global/activity/activitymanager.hpp"
#include "global/usercache/usercache.hpp"
#include "global/jieyi/jieyizumanager.h"
#include "global/qingyuan/qingyuanmanager.h"
#include "global/worldstatus/worldstatus.hpp"
#include "global/activity/activityimpl/activityhuanjiezhanchang.hpp"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "crossserver/crossserver/protocal/msgcross.h"
#include "gameworld/equipment/accessoriesmentmanager.h"
#include "other/courseofgrowth/courseofgrowth.hpp"
#include "other/courseofgrowth/courseofgrowthconfig.hpp"
#include "other/smartmountssystem/smartmountssystem.hpp"

void RMIRoleQueryBackObjectImpl::RoleQueryRet(int ret, const RoleQueryParam &p, const QueryItemListParam &item_list_param, const GodPrintItemListParam &god_print_item_list_param)
{
	QueryHandler::Instance().OnQueryBack(viewer_user_id, target_user_id, ret, p, item_list_param, god_print_item_list_param);
}

void RMIRoleQueryBackObjectImpl::__timeout()
{
	printf("RMIRoleQueryBackObjectImpl::__timeout, viewer: %d, target: %d\n", UidToInt(viewer_user_id), target_user_id);
}

QueryHandler & QueryHandler::Instance()
{
	static QueryHandler qh;
	return qh;
}

void QueryHandler::OnQueryBack(UserID viewer_userid, int target_uid, int ret, const RoleQueryParam & p, const QueryItemListParam & item_list_param, const GodPrintItemListParam &god_print_item_list_param)
{
	if (0 != ret)
	{
		printf("OnQueryBack -> viewer: %d, target: %d, ret: %d\n", UidToInt(viewer_userid), target_uid, ret);
		return;
	}

	if (target_uid != p.role_id)
	{
		return;
	}

	QueryCacheInfo tmp_cache_node;
	this->GetInfoToCacheNode(p, item_list_param, god_print_item_list_param, &tmp_cache_node);

	if (CrossConfig::Instance().IsHiddenServer())
	{
		ActivityHuanJieZhanChang * activity = (ActivityHuanJieZhanChang *)ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_HUAN_JIE_ZHAN_CHANG);
		if (NULL != activity)
		{
			QueryHuanJieInfo info = activity->GetRoleQueryInfo(target_uid);
			tmp_cache_node.rank_score = info.rank_score;
			tmp_cache_node.is_finish_ding_ji = info.is_finish_ding_ji;
		}
	}
	else
	{
		OfflineRegister * target_offline = OfflineRegisterManager::Instance().GetOfflineRegister(target_uid);
		if (NULL != target_offline)
		{
			const OfflineRegisterParam::OfflineRegisterOtherParam & target_offline_param = target_offline->GetOtherParam();
			if (0 != target_offline_param.huan_jie_rank_score)
			{
				tmp_cache_node.rank_score = target_offline_param.huan_jie_rank_score;
				tmp_cache_node.is_finish_ding_ji = target_offline_param.is_finish_ding_ji;
			}
		}
	}
	
	tmp_cache_node.top_level = UserCacheManager::Instance().GetUserTopLevel(tmp_cache_node.role_id);
	
	Guild * guild = GuildManager::Instance().GetGuild(IntToUid(target_uid));
	if (NULL != guild)
	{
		guild->GetName2(tmp_cache_node.guild_name);
	}

	tmp_cache_node.Invaild_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time()) + MAX_QUERY_HANDLER_INVAILD_TIME;
	m_cache_map[target_uid] = tmp_cache_node;
	this->SendInfoToViewer(UidToInt(viewer_userid), &tmp_cache_node);
}

void QueryHandler::OnQueryOther(int target_uid, int viewer_uid, bool is_check_from_game_to_hidden/* = false*/)
{
	if (0 == viewer_uid || 0 == target_uid) return;

	// 先查在线
	Role *target_role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(target_uid));
	if (NULL != target_role)
	{
		this->OnQueryOnline(viewer_uid, target_role);
		return;
	}

	// 再查缓存
	unsigned int now = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	std::map<int, QueryCacheInfo>::iterator iter = m_cache_map.find(target_uid);
	if (iter != m_cache_map.end() && iter->second.Invaild_timestamp > now)
	{
		QueryCacheInfo *tmp_cache_node = &iter->second;
		this->SendInfoToViewer(viewer_uid, tmp_cache_node);
		return;
	}

	// 后查DB
	RMIRoleQueryBackObjectImpl * ribo = new RMIRoleQueryBackObjectImpl();
	ribo->viewer_user_id = IntToUid(viewer_uid);
	ribo->target_user_id = target_uid;

	RMIRoleClient rc;
	const rmi::Session *session = &InternalComm::Instance().GetDBHandler().GetRoleDB();
	if (CrossConfig::Instance().IsHiddenServer())
	{
		int plat_spid = LocalConfig::Instance().GetPlatType();
		UniqueServerID usid(plat_spid, IntToUid(target_uid).db_index);
		const UniqueServerID *mother_usid = World::GetInstWorld()->GetMotherServerId(usid);
		if (mother_usid == NULL)
		{
			delete ribo;
			return;
		}
		const DBHandler* db_handler = InternalComm::Instance().GetHiddenDBHandler(*mother_usid);
		if (NULL == db_handler)
		{
			delete ribo;
			return;
		}

		session = &db_handler->GetRoleDB();
	}
	else
	{
		//发往跨服请求
		if (!LocalConfig::Instance().IsAllowedServerId(IntToUid(target_uid).db_index))//玩家是否本服玩家
		{
			static crossgameprotocal::GameCrossSyncCrossQuery query_msg;
			
			Role *viewer_role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(viewer_uid));
			if (viewer_role == NULL)
			{
				delete ribo;
				return;
			}
			query_msg.viewer_uid = viewer_role->GetUniqueUserID();
			query_msg.target_uid = target_uid;

			InternalComm::Instance().SendToHiddenThroughCross((const char*)&query_msg, sizeof(query_msg));
			
			delete ribo;
			return;
		}
	}
	rc.__bind_session(*session);
	bool ret = rc.RoleQueryAsyn(target_uid, ribo);
	if (!ret)
	{
		printf("OnQuery -> viewer: %d, target: %d, ret: %d\n", viewer_uid, target_uid, ret);
		return;
	}
}

void QueryHandler::OnUserLogout(Role * role)
{
	if (NULL == role) return;

	// 把数据存进缓存表里
	QueryCacheInfo tmp_cache_node;
	this->GetInfoToCacheNode(role, &tmp_cache_node);

	tmp_cache_node.Invaild_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time()) + MAX_QUERY_HANDLER_INVAILD_TIME;
	m_cache_map[role->GetUID()] = tmp_cache_node;		// 直接覆盖

	if (CrossConfig::Instance().IsHiddenServer())
	{
		//发往原服清除离线记录
		static crossgameprotocal::HiddenGameClearQueryInfo query_msg;
		query_msg.clear_target_uid = role->GetUniqueUserID();
		InternalComm::Instance().SendToGameThroughCross(query_msg.clear_target_uid.plat_type,
			query_msg.clear_target_uid.user_id.db_index,
			(const char*)&query_msg, sizeof(query_msg));
	}
	else
	{
		//发往跨服清除离线记录
		static crossgameprotocal::GameHiddenClearQueryInfo query_msg;
		query_msg.clear_target_uid = role->GetUniqueUserID();
		InternalComm::Instance().SendToHiddenThroughCross((const char*)&query_msg, sizeof(query_msg));
	}
}

void QueryHandler::SendQueryFollowUpInfo()
{
}

void QueryHandler::ClearUserCache(int uid)
{
	std::map<int, QueryCacheInfo>::iterator iter = m_cache_map.find(uid);
	if (iter != m_cache_map.end())
	{
		m_cache_map.erase(iter);
	}
}

void QueryHandler::OnQueryOnline(int viewer_uid, Role * target)
{
	if (0 == viewer_uid || NULL == target) return;

	QueryCacheInfo tmp_info;
	this->GetInfoToCacheNode(target, &tmp_info);
	this->SendInfoToViewer(viewer_uid, &tmp_info);
}

void QueryHandler::GetInfoToCacheNode(Role * target, QueryCacheInfo * cache_info)
{
	if (NULL == target || NULL == cache_info) return;

	RoleQueryParam p;
	QueryItemListParam item_list_param; item_list_param.count = 0;
	GodPrintItemListParam god_print_item_list_param; god_print_item_list_param.count = 0;

	p.role_id = target->GetUID();
	F_STRNCPY(p.role_name, target->GetRoleModuleManager()->GetRoleCross()->GetOriginRoleName(), sizeof(p.role_name));
	p.avatar_type = target->GetAvatarType();
	p.headshot_id = target->GetHeadshotID();
	p.sex = (char)target->GetSex();
	p.prof = target->GetProfession();
	p.vip_level = target->GetRoleModuleManager()->GetVip()->GetVipLevel();
	p.level = target->GetLevel();
	p.top_level = target->GetTopLevel();
	p.avatar_timestamp = target->GetAvatarTimestamp();
	p.kill_value = target->GetRoleModuleManager()->GetDuel()->GetKillValue();
	p.appearance_info = target->GetRoleModuleManager()->GetAppearance();
	QueryHuanJieInfo info;
	info = target->GetRoleModuleManager()->GetHuaShenManager()->GetQueryInfo();
	p.rank_score = info.rank_score;
	p.is_finish_ding_ji = info.is_finish_ding_ji;
	p.course_fraction_count = target->GetRoleModuleManager()->GetCourseOfGrowth()->GetCourseFractionCount();
	target->GetRoleModuleManager()->GetPet()->GetInitParam(&p.pet_common_param, NULL, NULL, NULL, NULL, NULL, &god_print_item_list_param,NULL);
	target->GetRoleInitParam(&p.role_init_param);

	Guild * guild = target->GetGuild();
	if (NULL != guild)
	{
		guild->GetName2(cache_info->guild_name);
	}

	int size = ARRAY_ITEM_COUNT(item_list_param.item_list) - item_list_param.count;
	target->GetRoleModuleManager()->GetEquipmentManager()->GetEquipmentList(&size, &item_list_param.item_list[item_list_param.count]);
	item_list_param.count = size;

	size = ARRAY_ITEM_COUNT(item_list_param.item_list) - item_list_param.count;
	target->GetRoleModuleManager()->GetEquipmentManager()->GetJewelryList(&size, &item_list_param.item_list[item_list_param.count]);
	item_list_param.count += size;

	size = ARRAY_ITEM_COUNT(item_list_param.item_list) - item_list_param.count;
	target->GetRoleModuleManager()->GetMedal()->GetMedalList(&size, &item_list_param.item_list[item_list_param.count]);
	item_list_param.count += size;

	size = ARRAY_ITEM_COUNT(item_list_param.item_list) - item_list_param.count;
	target->GetRoleModuleManager()->GetAccessoriesmentManager()->GetAccessoriesmentList(&size, &item_list_param.item_list[item_list_param.count]);
	item_list_param.count += size;

	size = ARRAY_ITEM_COUNT(item_list_param.item_list) - item_list_param.count;
	target->GetRoleModuleManager()->GetHolyEquipManager()->GetHolyEquipList(&size, &item_list_param.item_list[item_list_param.count]);
	item_list_param.count += size;

	p.face_score_param= target->GetRoleModuleManager()->GetFaceScore()->GetFaceScoreParam();

	const CommonDataParam & common_data = target->GetRoleModuleManager()->GetCommonData();
	cache_info->pet_count = 0;
	for (int i = NEW_FIGHT_INDEX; i < NEW_PET_STATUS_MAX && cache_info->pet_count < MAX_FIGHT_AND_STAND_BY_PET_NUM; i++)
	{
		const ItemGridData* pet_item = target->GetRoleModuleManager()->GetPet()->GetPetGridData(common_data.new_pet_status_index[i]);
		if (NULL != pet_item)
		{
			cache_info->pet_list[cache_info->pet_count].pet_pro.index = common_data.new_pet_status_index[i];
			cache_info->pet_list[cache_info->pet_count].pet_pro.is_bind = pet_item->is_bind;
			cache_info->pet_list[cache_info->pet_count].pet_pro.pet_unique_id = pet_item->item_unique_id;
			cache_info->pet_list[cache_info->pet_count].pet_pro.pet_param = pet_item->param->pet_param;
			cache_info->pet_list[cache_info->pet_count].pet_pro.pet_state = i;
			cache_info->pet_list[cache_info->pet_count].pet_pro.capability = target->GetRoleModuleManager()->GetPet()->GetPetCapability_1(common_data.new_pet_status_index[i]);
			cache_info->pet_list[cache_info->pet_count].rank = -1;

			target->GetRoleModuleManager()->GetPet()->GetQueryAttrList(common_data.new_pet_status_index[i], cache_info->pet_attr_list[cache_info->pet_count]);
			//获取对应穿戴神印五件套
			target->GetRoleModuleManager()->GetPet()->GetPetGodPrintList(cache_info->pet_list[cache_info->pet_count].pet_pro.index, &cache_info->pet_god_print_list[cache_info->pet_count]);

			cache_info->pet_count++;
		}
		
	}
	p.common_param = common_data;
	p.partner_param = target->GetRoleModuleManager()->GetPartner()->GetPartnerParam();
	p.smart_mounts_param = target->GetRoleModuleManager()->GetSmartMountsSystem()->GetParam();
	
	this->GetInfoToCacheNode(p, item_list_param, god_print_item_list_param, cache_info);
}

void QueryHandler::GetInfoToCacheNode(const RoleQueryParam & p, const QueryItemListParam &item_list_param, const GodPrintItemListParam &god_print_item_list_param, QueryCacheInfo * cache_info)
{
	if (NULL == cache_info) return;

	// 角色信息
	{
		cache_info->role_id = p.role_id;
		F_STRNCPY(cache_info->role_name, p.role_name, sizeof(cache_info->role_name));
		cache_info->avatar_type = p.avatar_type;
		cache_info->prof = p.prof;
		cache_info->vip_level = p.vip_level;
		cache_info->level = p.level;
		cache_info->top_level = p.top_level;
		cache_info->avatar_timestamp = p.avatar_timestamp;
		cache_info->appearance_info = p.appearance_info;
		cache_info->capability = p.role_init_param.capability;
		cache_info->rank_score = p.rank_score;
		cache_info->is_finish_ding_ji = p.is_finish_ding_ji;
		cache_info->kill_value = p.kill_value;
		cache_info->face_score_level = p.face_score_param.face_level;
		LOGIC_CONFIG->GetCourseOfGrowthConfig()->CalcLevelByCourseFraction(p.course_fraction_count, &cache_info->course_level, &cache_info->course_cur_fraction);
		QingYuanManager::Instance().GetQingYuanInfoByRoleId(cache_info->role_id, cache_info->qing_yuan_role_id, cache_info->qing_yuan_name);
		JieYiZuManager::Instance().GetJieYiZuInfoByRoleId(cache_info->role_id, cache_info->jie_yi_name, ARRAY_ITEM_COUNT(cache_info->jie_yi_name_list), cache_info->jie_yi_name_list);
		memcpy(cache_info->face_equip_list, p.face_score_param.face_equip_list, sizeof(cache_info->face_equip_list));
	}
	if (cache_info->face_score_level < 0)
	{
		int face_score_exp = 0;
		BitMap<FACE_RESOURCE_NUM_MAX> active_flag = p.face_score_param.active_flag;
		for (int seq = 0; seq < FACE_RESOURCE_NUM_MAX; ++seq)
		{
			const FaceResourceInfo * res_cfg = LOGIC_CONFIG->GetFaceScoreConfig()->GetResInfoCfg(seq);
			if (NULL != res_cfg && active_flag.IsBitSet(seq))
			{
				if (res_cfg->exp_add > 0)
				{
					face_score_exp += res_cfg->exp_add;
				}
			}
		}
		LOGIC_CONFIG->GetFaceScoreConfig()->InitLevelAndExpByTotalExp(&cache_info->face_score_level, &face_score_exp, cache_info->appearance_info.prof_base);
		if (cache_info->face_score_level < 0)
		{
			cache_info->face_score_level = 0;
		}
	}

	{
		cache_info->user_attribute.attr_list[QUERY_ATTR_TYPE_MAX_HP] = p.common_param.user_attr[BATTLE_ATTR_MAX_HP];
		cache_info->user_attribute.attr_list[QUERY_ATTR_TYPE_MAX_MP] = p.common_param.user_attr[BATTLE_ATTR_MAX_MP];
		cache_info->user_attribute.attr_list[QUERY_ATTR_TYPE_ATTACK] = p.common_param.user_attr[BATTLE_ATTR_ATTACK];
		cache_info->user_attribute.attr_list[QUERY_ATTR_TYPE_DEFENSE] = p.common_param.user_attr[BATTLE_ATTR_DEFENSE];
		cache_info->user_attribute.attr_list[QUERY_ATTR_TYPE_AGILITY] = p.common_param.user_attr[BATTLE_ATTR_AGILITY];
		cache_info->user_attribute.attr_list[QUERY_ATTR_TYPE_MENTAL] = p.common_param.user_attr[BATTLE_ATTR_MENTAL];
		cache_info->user_attribute.attr_list[QUERY_ATTR_TYPE_RECOVERY] = p.common_param.user_attr[BATTLE_ATTR_RECOVERY];
	}

	// 装备信息
	memset(cache_info->equip_list, 0, sizeof(cache_info->equip_list));
	for (int equip_type = 0; equip_type < Equipment::E_INDEX_MAX; ++equip_type)
	{
		cache_info->equip_list[equip_type].index = equip_type;
	}

	memset(cache_info->jewelry_list, 0, sizeof(cache_info->jewelry_list));
	for (int jewelry_type = 0; jewelry_type < Jewelry::J_TYPE_MAX; ++jewelry_type)
	{
		cache_info->jewelry_list[jewelry_type].jewelry_type = jewelry_type;
	}

	cache_info->accessories_count = 0;
	cache_info->medal_count = 0;
	for (int i = 0; i < item_list_param.count; ++i)
	{
		const QueryItemListParam::ItemListData &item_data = item_list_param.item_list[i];

		if (item_data.index >= ItemNamespace::JEWELRY_INDEX_BEGIN && item_data.index < ItemNamespace::JEWELRY_INDEX_END)
		{
			int jewelry_index = item_data.index - ItemNamespace::JEWELRY_INDEX_BEGIN;
			if (jewelry_index < 0 || jewelry_index >= Jewelry::J_TYPE_MAX) continue;

			const ItemBase *jewelry = ITEMPOOL->GetItem(item_data.item_wrapper.item_id);
			if (NULL == jewelry || jewelry->GetItemType() != I_TYPE_JEWELRY) continue;

			NetValueItemParam data;
			data.SetData(item_data.item_wrapper.param_data);

			JewelryPro & jewelry_cache = cache_info->jewelry_list[jewelry_index];
			jewelry_cache.jewelry_item = item_data.item_wrapper.item_id;
			jewelry_cache.item_unique_id = item_data.item_wrapper.item_unique_id;
			jewelry_cache.j_param = data.jewelry_param;
			jewelry_cache.j_param.CheckModifyVersion();
		}
		else if (item_data.index >= ItemNamespace::EQUIPMENT_INDEX_BEGIN && item_data.index < ItemNamespace::EQUIPMENT_INDEX_END)
		{
			int equip_index = item_data.index - ItemNamespace::EQUIPMENT_INDEX_BEGIN;	// index经过转换则为装备部位
			if (equip_index < 0 || equip_index >= Equipment::E_INDEX_MAX) continue;

			const ItemBase *equip = ITEMPOOL->GetItem(item_data.item_wrapper.item_id);
			if (NULL == equip || equip->GetItemType() != I_TYPE_EQUIPMENT) continue;

			NetValueItemParam data;
			data.SetData(item_data.item_wrapper.param_data);

			EquipmentPro & equip_cache = cache_info->equip_list[equip_index];
			equip_cache.item_id = item_data.item_wrapper.item_id;
			equip_cache.num = item_data.item_wrapper.num;
			equip_cache.is_bind = item_data.item_wrapper.is_bind;
			equip_cache.item_unique_id = item_data.item_wrapper.item_unique_id;
			equip_cache.param = data.equip_param;
			equip_cache.param.CheckModifyVersion();
		}
		else if (item_data.index >= ItemNamespace::PET_INDEX_BEGIN && item_data.index < ItemNamespace::PET_INDEX_END)
		{
			int pet_index = item_data.index - ItemNamespace::PET_INDEX_BEGIN;	// index经过转换则为宠物
			if (pet_index < 0 ) continue;
			if (cache_info->pet_count >= ARRAY_ITEM_COUNT(cache_info->pet_list)) continue;

			NetValueItemParam data;
			data.SetData(item_data.item_wrapper.param_data);

			if (BATTLE_MOVE_TYPE_PERFORM_SKILL == data.pet_param.pet_auto_move_type &&
				data.pet_param.pet_auto_skill_level <= 0)
			{
				data.pet_param.pet_auto_move_type = BATTLE_MOVE_TYPE_NORMAL_ATTACK;
				data.pet_param.pet_auto_skill_id = 0;
				data.pet_param.pet_auto_skill_level = 0;
			}

			{	//检查出战待战宠物
				bool is_fight_or_standby = false;
				int pet_status = -1;
				UNSTD_STATIC_CHECK(NEW_STANDBY_INDEX5 + 1 <= NEW_PET_STATUS_MAX);
				for (int i = NEW_FIGHT_INDEX; i <= NEW_STANDBY_INDEX5; i++)
				{
					if (pet_index == p.common_param.new_pet_status_index[i])
					{
						is_fight_or_standby = true;
						pet_status = i;
						break;
					}
				}
				if (is_fight_or_standby)
				{
					// 去重操作 现在版本(20201204)宠物有BUG，在库中会有多于一个宠物的item_index一致，若BUG解决了，就把该注释删除吧
					bool is_found = false;
					for (int idx = 0; idx < cache_info->pet_count; ++idx)
					{
						if (cache_info->pet_list[idx].pet_pro.index == pet_index)
						{
							is_found = true;
							break;
						}
					}
					if (is_found)
					{
						continue;
					}

					PetPro & pet_cache = cache_info->pet_list[cache_info->pet_count].pet_pro;
					pet_cache.index = pet_index;
					pet_cache.pet_state = pet_status;
					pet_cache.is_bind = item_data.item_wrapper.is_bind ? 1 : 0;
					pet_cache.pet_unique_id = item_data.item_wrapper.item_unique_id;
					pet_cache.pet_param = data.pet_param;
					pet_cache.pet_param.CheckModifyVersion();

					QueryAttrList & pet_query_attr = cache_info->pet_attr_list[cache_info->pet_count];
					pet_query_attr = p.pet_common_param.pet_attr_list[pet_cache.pet_state];
					switch (pet_cache.pet_state)
					{
					case NEW_STANDBY_INDEX1:
					{
						pet_cache.capability = p.pet_common_param.curr_battle_standby_index1_cap;
					}
					break;
					case NEW_STANDBY_INDEX2:
					{
						pet_cache.capability = p.pet_common_param.curr_battle_standby_index2_cap;
					}
					break;
					case NEW_STANDBY_INDEX3:
					{
						pet_cache.capability = p.pet_common_param.curr_battle_standby_index3_cap;
					}
					break;
					case NEW_STANDBY_INDEX4:
					{
						pet_cache.capability = p.pet_common_param.curr_battle_standby_index4_cap;
					}
					break;
					case NEW_STANDBY_INDEX5:
					{
						pet_cache.capability = p.pet_common_param.curr_battle_standby_index5_cap;
					}
					break;
					case NEW_FIGHT_INDEX:
					{
						pet_cache.capability = p.pet_common_param.cur_battle_fight_cap;
					}
					break;
					}
					cache_info->pet_count += 1;
				}
			}
		}
		else if (item_data.index >= ItemNamespace::MEDAL_INDEX_BEGIN && item_data.index < ItemNamespace::MEDAL_INDEX_END)
		{
			int medal_index = item_data.index - ItemNamespace::MEDAL_INDEX_BEGIN;	// index经过转换则为装备部位
			if (medal_index < 0 || medal_index >= MAX_MEDAL_WEAR_GRID) continue;
			if (cache_info->medal_count >= MAX_MEDAL_WEAR_GRID) continue;

			NetValueItemParam data;
			data.SetData(item_data.item_wrapper.param_data);

			MedalPro & medal_cache = cache_info->medal_list[cache_info->medal_count];
			medal_cache.item_id = item_data.item_wrapper.item_id;
			medal_cache.index = medal_index;
			medal_cache.num = item_data.item_wrapper.num;
			medal_cache.is_bind = item_data.item_wrapper.is_bind;
			medal_cache.medal_unique_id = item_data.item_wrapper.item_unique_id;
			medal_cache.param = data.medal_param;
			medal_cache.param.CheckModifyVersion();
			cache_info->medal_count++;
		}
		else if (item_data.index >= ItemNamespace::ACCESSORIES_INDEX_BEGIN && item_data.index < ItemNamespace::ACCESSORIES_INDEX_END)
		{
			int accessories_index = item_data.index - ItemNamespace::ACCESSORIES_INDEX_BEGIN;	// index经过转换则为装备部位
			if (accessories_index < 0 || accessories_index >= ItemNamespace::ACCESSORIES_GRID_MAX_COUNT) continue;
			if (cache_info->accessories_count >= ItemNamespace::ACCESSORIES_GRID_MAX_COUNT) continue;

			NetValueItemParam data;
			data.SetData(item_data.item_wrapper.param_data);

			AccessoriesPro & accessories_cache = cache_info->accessories_list[cache_info->accessories_count];
			accessories_cache.item_id = item_data.item_wrapper.item_id;
			accessories_cache.index = accessories_index + 1;
			accessories_cache.num = item_data.item_wrapper.num;
			accessories_cache.is_bind = item_data.item_wrapper.is_bind;
			accessories_cache.item_unique_id = item_data.item_wrapper.item_unique_id;
			accessories_cache.param = data.accessoriess_param;
			cache_info->accessories_count++;
		}
		else if (item_data.index >= ItemNamespace::HOLY_EQUIP_BAG_INDEX_BEGIN && item_data.index < ItemNamespace::HOLY_EQUIP_BAG_INDEX_END)
		{
			int index = item_data.index - ItemNamespace::HOLY_EQUIP_BAG_INDEX_BEGIN;	// index经过转换则为装备部位
			if (index < 0 || index >= ItemNamespace::MAX_HOLY_EQUIP_BAG_NUM) continue;
			if (cache_info->holy_equip_count < 0 || cache_info->holy_equip_count >= MAX_HOLY_EQUIP_FIGHT_STANDBY_NUM) continue;

			const ItemBase *holy_equip = ITEMPOOL->GetItem(item_data.item_wrapper.item_id);
			if (NULL == holy_equip || holy_equip->GetItemType() != I_TYPE_HOLY_EQUIP) continue;


			NetValueItemParam data;
			data.SetData(item_data.item_wrapper.param_data);

			HolyEquipPro & holy_equip_cache = cache_info->holy_equip_list[cache_info->holy_equip_count];
			holy_equip_cache.item_id = item_data.item_wrapper.item_id;
			holy_equip_cache.reserve_sh = 0;
			holy_equip_cache.param = data.holy_equip_param;

			cache_info->holy_equip_count++;
		}
	}
	
	//伙伴
	cache_info->partner_count = 0;
	memset(cache_info->partner_list, 0, sizeof(cache_info->partner_list));
	for (int i = 0; i < PARTNER_SYNFIGHT_NUM_MAX; i++)
	{
		//修改
		int partner_id = p.partner_param.synfight_list[i];
		if(partner_id <= 0 || partner_id >= PARTNER_NUM_MAX) continue;
		const NewPartnerBaseInfo & base_info = p.partner_param.partner_base_info[partner_id];
		if (base_info.IsInvalid()) continue;
		if (LOCK_STATE_UNLOCK == base_info.partner_list.is_unlocked)
		{
			cache_info->partner_list[cache_info->partner_count].SetData(p.partner_param, partner_id);
			if (4 != base_info.is_set_partner_quality_segment_flag)				//改版后查询未登录玩家处理
			{
				cache_info->partner_list[cache_info->partner_count].quality_segment = (0 == cache_info->partner_list[cache_info->partner_count].quality_segment) ?
					1 : cache_info->partner_list[cache_info->partner_count].quality_segment;
			}
			//伙伴数据处理
			if (0 == cache_info->partner_list[cache_info->partner_count].level)
			{
				cache_info->partner_list[cache_info->partner_count].level = cache_info->level;
			}
			cache_info->partner_count++;
		}
	}

	for (int i = 0; i < cache_info->pet_count; i++)
	{
		PetParam & pet_param = cache_info->pet_list[i].pet_pro.pet_param;
		GodPrintListPro & god_print_pro = cache_info->pet_god_print_list[i];
		for (int j = 0; j < MAX_PET_GOD_PRONT_ONE_SCHEME_NUM; j++)
		{
			int bag_idx = pet_param.god_print_list[j];
			if(bag_idx - 1 < 0 || bag_idx - 1 >= NEW_MAX_PET_GOD_PRONT_BAG_NUM) continue;
			const GodPrintItem * god_print_item = NULL;
			for (int i = 0; i < god_print_item_list_param.count && i < NEW_MAX_PET_GOD_PRONT_BAG_NUM; i++)
			{
				if (god_print_item_list_param.item_list[i].index == bag_idx - 1)
				{
					god_print_item = &god_print_item_list_param.item_list[i].item_data;
				}
			}
			if(NULL == god_print_item) continue;
			god_print_pro.god_print_list[j].has_random_net_value = 1;
			god_print_pro.god_print_list[j].god_print_data = *god_print_item;
		}
	}

	cache_info->smart_mounts_count = 0;
	for (int i = 0; i < ARRAY_LENGTH(p.smart_mounts_param.smart_mounts_list) && cache_info->smart_mounts_count < ARRAY_LENGTH(cache_info->smart_mounts_list); ++i)
	{
		const SmartMountsSystemBaseCfg * base_cfg = LOGIC_CONFIG->GetSmartMountsSystemConfig()->GetBaseCfg(i);
		if (NULL == base_cfg) continue;

		const SmartMountsSystemSingleParam & smart_mounts_data = p.smart_mounts_param.smart_mounts_list[i];
		if (smart_mounts_data.level <= 0 || smart_mounts_data.bicycle_id <= 0) continue;

		const SmartMountsSystemLevelCfg * level_cfg = LOGIC_CONFIG->GetSmartMountsSystemConfig()->GetLevelCfg(smart_mounts_data.bicycle_id, smart_mounts_data.level);
		if (NULL == level_cfg) continue;

		//计算当前灵性值
		int smart_nature_value = base_cfg->bicycle_grow_begin;
		if (0 != smart_mounts_data.jewelry_data.is_active_flag && smart_mounts_data.jewelry_data.strength_level > 0)
		{
			const SmartMountsSystemJewelryCfg * jewelry_cfg = LOGIC_CONFIG->GetSmartMountsSystemConfig()->GetJewelryCfg(base_cfg->jewelry_id, smart_mounts_data.jewelry_data.strength_level);
			if (NULL != jewelry_cfg)
			{
				smart_nature_value = jewelry_cfg->bicycle_grow;
			}
		}

		const std::map<int, SmartMountsSystemBreachCfg>* breach_cfg_map = LOGIC_CONFIG->GetSmartMountsSystemConfig()->GetBreachCfg(smart_mounts_data.bicycle_id);
		const SmartMountsSystemBreachCfg * breach_cfg = NULL;
		if (NULL != breach_cfg_map)
		{
			for (std::map<int, SmartMountsSystemBreachCfg>::const_iterator it = breach_cfg_map->begin(); it != breach_cfg_map->end(); ++it)
			{
				if (smart_mounts_data.level < it->first || (smart_mounts_data.level == it->first && 0 == smart_mounts_data.is_breach_flag)) break;

				breach_cfg = &it->second;
			}
		}

		std::map<int, int> attr_map;
		for (int k = 0; k < MAX_SMART_MOUNTS_SYSTEM_ATTR_NUM; k++)
		{
			if (NULL != breach_cfg && breach_cfg->attr_value[k] > 0 && breach_cfg->attr_type[k] >= BATTLE_ATTR_MIN && breach_cfg->attr_type[k] < BATTLE_ATTR_MAX)
			{
				attr_map[breach_cfg->attr_type[k]] += breach_cfg->attr_value[k];
			}
			if (level_cfg->attr_value[k] > 0 && level_cfg->attr_type[k] >= BATTLE_ATTR_MIN && level_cfg->attr_type[k] < BATTLE_ATTR_MAX)
			{
				attr_map[level_cfg->attr_type[k]] += level_cfg->attr_value[k];
			}
		}

		SmartMountsSystemPro & smart_mounts_pro = cache_info->smart_mounts_list[cache_info->smart_mounts_count++];
		smart_mounts_pro.SetData(smart_mounts_data);
		const SmartMountsSystemJewelryCfg * max_jewelry_cfg = LOGIC_CONFIG->GetSmartMountsSystemConfig()->GetJewelryCfg(base_cfg->jewelry_id, LOGIC_CONFIG->GetSmartMountsSystemConfig()->GetJewelryMaxLevelById(base_cfg->jewelry_id));
		if (NULL != max_jewelry_cfg && max_jewelry_cfg->bicycle_grow > base_cfg->bicycle_grow_begin)
		{
			//当前属性(it->second) = 升级属性+突破属性,当前属性*(1 - 投放比例) + 当前属性*投放比例*((当前灵性 - 初始)/(最大灵性 - 初始)) 
			double attr_scale = 100 - base_cfg->grow_min_proportion;
			double nature_scale = base_cfg->grow_min_proportion * ((smart_nature_value - base_cfg->bicycle_grow_begin) * 1.0 / (max_jewelry_cfg->bicycle_grow - base_cfg->bicycle_grow_begin));
			int idx = 0;
			for (std::map<int, int>::iterator it = attr_map.begin(); it != attr_map.end() && idx < MAX_SMART_MOUNTS_SYSTEM_ATTR_NUM; ++it, idx++)
			{
				smart_mounts_pro.attr_list[idx].attr_type = it->first; 
				smart_mounts_pro.attr_list[idx].attr_value += static_cast<short>((it->second * attr_scale + it->second * nature_scale) / 100);
			}
		}
	}
}

void QueryHandler::SendInfoToViewer(int viewer_uid, QueryCacheInfo * cache_node)
{
	if (0 == viewer_uid || NULL == cache_node) return;

	Protocol::SCQueryRoleInfoRet rei;

	//角色基本信息
	rei.role_id = cache_node->role_id;
	GameName role_name;
	cache_node->GetName(role_name);
	SNPRINTF(rei.role_name, sizeof(GameName), "%s_s%d", role_name, IntToUid(rei.role_id).db_index);
	F_STRNCPY(rei.guild_name, cache_node->guild_name, sizeof(rei.guild_name));
	rei.avatar_type = cache_node->avatar_type;
	rei.prof = cache_node->prof;
	rei.level = cache_node->level;
	rei.top_level = cache_node->top_level;
	rei.appearance_info = cache_node->appearance_info;
	rei.capability = cache_node->capability;
	rei.rank_score = cache_node->rank_score;
	rei.is_finish_ding_ji = cache_node->is_finish_ding_ji;
	rei.kill_value = cache_node->kill_value;
	rei.course_level = cache_node->course_level;
	rei.course_cur_fraction = cache_node->course_cur_fraction;
	rei.face_score_level = cache_node->face_score_level;
	rei.qing_yuan_role_id = cache_node->qing_yuan_role_id;
	F_STRNCPY(rei.qing_yuan_name, cache_node->qing_yuan_name, sizeof(rei.qing_yuan_name));
	rei.jie_yi_name = cache_node->jie_yi_name;
	UNSTD_STATIC_CHECK(ARRAY_ITEM_COUNT(rei.jie_yi_name_list) == ARRAY_ITEM_COUNT(cache_node->jie_yi_name_list));
	for (int i = 0; i < ARRAY_ITEM_COUNT(rei.jie_yi_name_list); ++i)
	{
		F_STRNCPY(rei.jie_yi_name_list[i], cache_node->jie_yi_name_list[i], sizeof(rei.jie_yi_name_list[i]));
	}


	for (int i = 0; i < ARRAY_LENGTH(rei.user_attribute.attr_list) && i < ARRAY_LENGTH(cache_node->user_attribute.attr_list); i++)
	{
		rei.user_attribute.attr_list[i] = cache_node->user_attribute.attr_list[i];
	}

	//角色装备
	for (int i = 0; i < Equipment::E_INDEX_MAX; ++i)
	{
		rei.equip_list[i] = cache_node->equip_list[i];
	}

	//角色灵饰
	for (int i = 0; i < Jewelry::J_TYPE_MAX; ++i)
	{
		rei.jewelry_list[i] = cache_node->jewelry_list[i];
	}

	//角色颜值系统装备
	memcpy(rei.face_equip_list, cache_node->face_equip_list, sizeof(rei.face_equip_list));

	//角色宠物信息
	rei.pet_count = cache_node->pet_count;
	for (int i = 0; i < MAX_FIGHT_AND_STAND_BY_PET_NUM && i < cache_node->pet_count; i++)
	{
		rei.pet_list[i] = cache_node->pet_list[i];
		rei.pet_list[i].rank =
			RankManager::Instance().GetPetTopRankShadow()->
			GetPetTopRank(cache_node->role_id,
				cache_node->pet_list[i].pet_pro.pet_param.pet_id,
				cache_node->pet_list[i].pet_pro.pet_unique_id);
		rei.pet_attr_list[i] = cache_node->pet_attr_list[i];
		rei.pet_god_print_list[i] = cache_node->pet_god_print_list[i];
	}
	//伙伴信息
	rei.partner_count = cache_node->partner_count;
	for (int i = 0; i < cache_node->partner_count && i < PARTNER_SYNFIGHT_NUM_MAX; i++)
	{
		rei.partner_list[i] = cache_node->partner_list[i];
		rei.partner_list[i].rank =
			RankManager::Instance().GetPartnerTopRankShadow()->
			GetPartnerTopRank(cache_node->role_id, 
				cache_node->partner_list[i].partner_id);
	}

	rei.medal_count = cache_node->medal_count;
	for (int i = 0; i < cache_node->medal_count && i < MAX_MEDAL_WEAR_GRID; i++)
	{
		rei.medal_list[i] = cache_node->medal_list[i];
	}

	//配饰

	rei.accessories_count = cache_node->accessories_count;
	for (int i = 0; i < cache_node->accessories_count && i < ItemNamespace::MAX_ACCESSORIES_INDEX_NUM; i++)
	{
		rei.accessories_list[i] = cache_node->accessories_list[i];
	}

	//灵骑
	rei.smart_mounts_count = cache_node->smart_mounts_count;
	for (int i = 0; i < rei.smart_mounts_count && i < ARRAY_LENGTH(rei.smart_mounts_list); ++i)
	{
		rei.smart_mounts_list[i] = cache_node->smart_mounts_list[i];
	}

	//圣器
	rei.holy_equip_count = cache_node->holy_equip_count;
	for (int i = 0; i < rei.holy_equip_count && i < ARRAY_LENGTH(rei.holy_equip_list); ++i)
	{
		rei.holy_equip_list[i] = cache_node->holy_equip_list[i];
	}

	bool is_succ_send = true;
	short is_ret_original_place = 1;
	Role * viwer = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(viewer_uid));
	if (NULL == viwer)
	{
		if (CrossConfig::Instance().IsHiddenServer())
		{
			InternalComm::Instance().SendSerializeMsgToGameUserThroughCross(viewer_uid, &rei);
			is_ret_original_place = 0;
		}
		else
		{
			is_succ_send = false;
		}
	}
	else
	{
		EngineAdapter::Instance().NetSendSerializeMsg(viwer->GetNetId(), &rei);
	}

	//原服,跨服没连接  
	if (is_succ_send)
	{
		if (!CrossConfig::Instance().IsHiddenServer())
		{
			if (!CrossManager::Instance().IsCrossServerConnected())
			{
				Protocol::SCQueryRoleFollowUpInfo info;
				int rank_index = RankManager::Instance().GetPersonRank()->GetPersonRankIndex(PERSON_RANK_TYPE_CAPABILITY, IntToUid(rei.role_id));
				if (rank_index < -1)
				{
					info.comp_rank = -1;
				}
				else
				{
					info.comp_rank = rank_index;
				}

				int rank_prof_type = GetRankTypeByProfType(rei.prof);
				rank_index = RankManager::Instance().GetPersonRank()->GetPersonRankIndex(rank_prof_type, IntToUid(rei.role_id));
				if (rank_index < -1)
				{
					info.prof_rank = -1;
				}
				else
				{
					info.prof_rank = rank_index;
				}
				
				if (NULL != viwer)
				{
					EngineAdapter::Instance().NetSend(viwer->GetNetId(), &info, sizeof(info));
				}
				return;
			}
		}
		crossgameprotocal::GameCrossQueryRankInfoReq gcqrir;
		gcqrir.prof = rei.prof;
		gcqrir.unique_user_id = UniqueUserID(LocalConfig::Instance().GetPlatType(), IntToUid(rei.role_id));
		gcqrir.unique_viewer_uid = UniqueUserID(LocalConfig::Instance().GetPlatType(), IntToUid(viewer_uid));
		gcqrir.is_ret_original_place = is_ret_original_place;

		InternalComm::Instance().SendToCross((const char *)&gcqrir, sizeof(gcqrir));
	}
}