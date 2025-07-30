#ifndef __DB_DISCONNECT_ROLE_SAVE_CACHE_HPP__
#define __DB_DISCONNECT_ROLE_SAVE_CACHE_HPP__

#include "servercommon/struct/roleinitparam.h"
#include "servercommon/struct/roleotherinitparam.hpp"
#include "servercommon/struct/itemlistparam.h"
#include "servercommon/struct/skilllistparam.hpp"
#include "servercommon/struct/global/maillistparam.hpp"
#include "servercommon/struct/friendparam.h"
#include "servercommon/struct/rolefirstkillparam.hpp"
#include "servercommon/rolecommonsavedef.h"

class DBDisconnectRoleSaveCache
{
public:
	struct RoleSaveCache
	{
		int uid;
		RoleInitParam role_param;
		RoleOtherInitParam other_p;
		ItemListParam item_list_param;
		SkillListParam skill_list_param;
		MailListParam maillist_param;
		RoleFirstKillParamList first_kill_list;
		RandActivityRoleParamList rand_activity_role_list;
		RoleCommonSaveDataParam role_common_save_data;
		RoleCourageChallengeLevelParamList role_courage_challenge_list;
		GodPrintItemListParam god_print_item_list;
		NewCourageChallengeLevelParamList new_courage_challenge_list;
	};

	typedef std::map<int, RoleSaveCache*> RoleSaveCacheMap;
public:
	static DBDisconnectRoleSaveCache& Instance();
	
	void Update(unsigned int now);
	bool CanUserLogin(int uid);

	bool CacheSaveData(int uid,
		const RoleInitParam &p,
		const RoleOtherInitParam &other_p,
		const ItemListParam& item_list_param,
		const SkillListParam& skill_list_param,
		const MailListParam& maillist_param,
		const RoleFirstKillParamList& first_kill_list,
		const RandActivityRoleParamList & rand_activity_role_list,
		const RoleCommonSaveDataParam & role_common_save_list,
		const RoleCourageChallengeLevelParamList & role_courage_challenge_list,
		const GodPrintItemListParam & god_print_item_list,
		const NewCourageChallengeLevelParamList & new_courage_challenge_list);

private:
	RoleSaveCache* MutableRoleSaveCache(int uid);
	void RemoveRoleSaveCache(int uid);

	DBDisconnectRoleSaveCache();
	~DBDisconnectRoleSaveCache();
	NONCOPYABLE(DBDisconnectRoleSaveCache);
	void TrySave();
	bool TrySaveUserCache(RoleSaveCache* cache);

	RoleSaveCacheMap m_role_save_cache_map;
};

#endif