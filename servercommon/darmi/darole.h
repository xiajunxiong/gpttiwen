#ifndef __DA_ROLE_H__
#define __DA_ROLE_H__

#include "damodule.h"

namespace dataaccessrmi
{
	namespace role
	{
		enum ROLE_SAVE_RET
		{
			CONNET_ERROR=-1,
			SUC=0,
			ROLE_ERROR=1<<0,
			ROLE_ATTR_ERROR=1<<1,
			SKILL_ERROR=1<<2,
			KNAPSACK_ERROR=1<<3,
			EQUIPMENT_ERROR=1<<4,
			TASKPHASE_ERROR=1<<5,
			TASKRECORD_ERROR=1<<6,
			HOTKEY_ERROR=1<<7,
			FRINEND_ERROR=1<<8,
			ENEMY_ERROR=1<<9,
			BUSINESS_ERROR=1<<10,
			ACHIEVE_ERROR=1<<11,		
			ROLE_ATTR_DETAIL_ERROR=1<<12,
			BLACKLIST_ERROR=1<<13,
			DAILYFIND_ERROR=1<<14,
			MOUNT_ERROR=1<<15,
			PET_ERROR=1<<16,
			ROLEMAIL_ERROR=1<<17,
			ROLE_FIRST_KILL_ERROR = 1 << 18,
			RAND_ACTIVITY_ROLE_DATA_LIST_ERROR = 1 << 19,
			ROLE_COURAGE_CHALLENGE_ERROR = 1 << 20,
			NEW_COURAGE_CHALLENGE_ERROR = 1 << 21,
		};

		static const char *GET_ROLE_INFO = "GetRoleInfo";
		static const char *ROLE_INIT = "RoleInit";
		static const char *ROLE_SAVE = "RoleSave";
		static const char *CREATE_ROLE = "CreateRole";
		static const char *DESTROY_ROLE = "DestroyRole";
		static const char *ROLE_SAVE_STATUS = "RoleSaveStatus";
		static const char *ROLE_QUERY = "RoleQuery";
		static const char *ROLE_READ_DATA = "RoleReadData";
		static const char *ROLE_SAVE_DATA = "RoleSaveData";
		static const char *QUERY_RETRIEVE_PET = "QueryRetrievePet";
		static const char *ROLE_MUTE = "RoleMute";
		static const char *ROLE_SET_AUTHORITY = "RoleSetAuthority";
	}
}

#endif // __DA_ROLE_H__

