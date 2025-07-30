#include "dbdisconnectrolesavecache.hpp"
#include "servercommon/dbhandler.h"
#include "internalcomm.h"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "global/cross/crossuserregister.hpp"
#include "rmibackobjdef.h"
#include "obj/character/role.h"
#include "world.h"
#include "scene/scenemanager.hpp"
#include "gamelog.h"

DBDisconnectRoleSaveCache::DBDisconnectRoleSaveCache()
{

}

DBDisconnectRoleSaveCache::~DBDisconnectRoleSaveCache()
{

}

DBDisconnectRoleSaveCache& DBDisconnectRoleSaveCache::Instance()
{
	static DBDisconnectRoleSaveCache inst;
	return inst;
}

DBDisconnectRoleSaveCache::RoleSaveCache* DBDisconnectRoleSaveCache::MutableRoleSaveCache(int uid)
{
	RoleSaveCacheMap::iterator it = m_role_save_cache_map.find(uid);
	if (it != m_role_save_cache_map.end())
	{
		return it->second;
	}

	return NULL;
}

void DBDisconnectRoleSaveCache::RemoveRoleSaveCache(int uid)
{
	RoleSaveCacheMap::iterator it = m_role_save_cache_map.find(uid);
	if (it != m_role_save_cache_map.end())
	{
		delete it->second;
		m_role_save_cache_map.erase(it);
	}
}

void DBDisconnectRoleSaveCache::Update(unsigned int now)
{
	this->TrySave();
}

bool DBDisconnectRoleSaveCache::CanUserLogin(int uid)
{
	RoleSaveCache* cache = this->MutableRoleSaveCache(uid);
	if (NULL == cache) return true; 

	gamelog::g_log_world.printf(LL_MAINTANCE, "DBDisconnectRoleSaveCache::CanUserLogin role[%d,%s]",
		cache->uid, cache->role_param.role_name);

	if (this->TrySaveUserCache(cache))
	{
		// 能把之前没保存的数据保存好，证明dataaccess已恢复正常，可以让玩家继续登录了
		gamelog::g_log_world.printf(LL_MAINTANCE, "DBDisconnectRoleSaveCache::CanUserLogin Save Succ, Remove Cache And Let User Login! role[%d,%s]",
			cache->uid, cache->role_param.role_name);

		this->RemoveRoleSaveCache(uid);
		return true;
	}
	else
	{
		// 数据服未恢复正常，不让玩家登录
		return false;
	}
}

bool DBDisconnectRoleSaveCache::CacheSaveData(int uid, const RoleInitParam &p,
												const RoleOtherInitParam &other_p,
												const ItemListParam& item_list_param,
												const SkillListParam& skill_list_param,
												const MailListParam& maillist_param,
												const RoleFirstKillParamList& first_kill_list,
												const RandActivityRoleParamList & rand_activity_role_list,
												const RoleCommonSaveDataParam & role_common_save_list,
												const RoleCourageChallengeLevelParamList & role_courage_challenge_list,
												const GodPrintItemListParam & god_print_item_list,
												const NewCourageChallengeLevelParamList & new_courage_challenge_list)
{
	RoleSaveCache* cache = this->MutableRoleSaveCache(uid);
	if (NULL == cache)
	{
		cache = new RoleSaveCache();
		if (NULL == cache)
		{
			return false;
		}
		m_role_save_cache_map.insert(RoleSaveCacheMap::value_type(uid, cache));
	}

	cache->uid = uid;
	cache->role_param = p;
	cache->other_p = other_p;
	cache->item_list_param = item_list_param;
	cache->skill_list_param = skill_list_param;
	cache->maillist_param = maillist_param;
	cache->first_kill_list = first_kill_list;
	cache->rand_activity_role_list = rand_activity_role_list;
	cache->role_common_save_data = role_common_save_list;
	cache->role_courage_challenge_list = role_courage_challenge_list;
	cache->god_print_item_list = god_print_item_list;
	cache->new_courage_challenge_list = new_courage_challenge_list;

	gamelog::g_log_world.printf(LL_MAINTANCE, "DBDisconnectRoleSaveCache::CacheSaveData role[%d,%s]",
		cache->uid, cache->role_param.role_name);

	return true;
}

void DBDisconnectRoleSaveCache::TrySave()
{
	if (m_role_save_cache_map.empty()) return;

	RoleSaveCacheMap::iterator it = m_role_save_cache_map.begin();
	for (; it != m_role_save_cache_map.end();)
	{
		RoleSaveCache* cache = it->second;
		
		Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(cache->uid);
		if (NULL != role)
		{
			gamelog::g_log_world.printf(LL_ERROR, "DBDisconnectRoleSaveCache::TrySave Role Exist... role[%d,%s]", role->GetUID(), role->GetName());

#ifdef _DEBUG
			assert(0); // 不应该在线
#else
			delete cache;
			m_role_save_cache_map.erase(it++);
			continue;
#endif
		}

		if (this->TrySaveUserCache(cache))
		{
			delete cache;
			m_role_save_cache_map.erase(it++);
			continue;
		}
		else
		{
			++it;
		}
	}
}

bool DBDisconnectRoleSaveCache::TrySaveUserCache(RoleSaveCache * cache)
{
	UserID original_user_id = IntToUid(cache->uid);
	const DBSession* db_session = &InternalComm::Instance().GetDBHandler().GetRoleDBSession();

	if (CrossConfig::Instance().IsHiddenServer())
	{
		const CrossUserRegister::CrossUserInfo* cui = CrossUserRegister::Instance().GetCrossUserInfo(original_user_id);
		if (NULL == cui)
		{
			return false;
		}

		UniqueServerID usid(cui->original_plat_type, cui->original_server_id);
		const DBHandler* db_handler = InternalComm::Instance().GetHiddenDBHandler(usid);
		if (NULL == db_handler)
		{
			return false;
		}

		db_session = &db_handler->GetRoleDBSession();
		original_user_id = cui->original_user_id;
	}

	if (db_session->IsNeedReconnect())
	{
		return false;
	}

	RMIRoleSaveBackObjectImplRole *rsbo = new RMIRoleSaveBackObjectImplRole();
	rsbo->log_user_id = original_user_id;
	rsbo->is_logout = true;
	rsbo->role_id = cache->uid;

	RMIRoleClient rc;
	rc.__bind_session(db_session->session);
	bool ret = rc.RoleSaveAsyn(UidToInt(original_user_id), cache->role_param, cache->other_p,
		cache->item_list_param,
		cache->skill_list_param,
		cache->maillist_param,
		cache->first_kill_list,
		cache->rand_activity_role_list,
		cache->role_common_save_data,
		cache->role_courage_challenge_list,
		cache->god_print_item_list,
		cache->new_courage_challenge_list,
		rsbo, true);

	if (ret)
	{
		gamelog::g_log_world.printf(LL_MAINTANCE, "DBDisconnectRoleSaveCache::TrySaveUserCache Save Succ role[%d,%s]",
			cache->uid, cache->role_param.role_name);
		return true;
	}
	else
	{
		gamelog::g_log_world.printf(LL_WARNING, "DBDisconnectRoleSaveCache::TrySaveUserCache Save FAIL!!! role[%d,%s]",
			cache->uid, cache->role_param.role_name);
		return false;
	}
}

