#ifndef __DA_LOGIN_H__
#define __DA_LOGIN_H__

#include "damodule.h"

namespace dataaccessrmi
{
	namespace login
	{
		static const char *GET_PROF_NUM = "GetProfNum";
		static const char *GET_ACCOUNT_INFO = "GetAccountInfo";
		static const char *UPDATE_LAST_LOGIN_TIME = "UpdateLastLoginTime";

		static const char *ADD_ROLE = "AddRole";
		static const char *REMOVE_ROLE = "RemoveRole";
		static const char *FORBID = "Forbid";
		static const char *GET_GOLD = "GetGold";
		static const char *CHANGE_GOLD = "ChangeGold";
		static const char *ADD_NAME_INFO = "AddNameInfo";
		static const char *UPDATE_NAME_INFO = "UpdateNameInfo";
		static const char *DELETE_NAME_INFO = "DeleteNameInfo";
		static const char *DELETE_NAME_INFO_BY_NAME = "ByNameDeleteNameInfo";
		static const char *QUERY_NAME_EXIST = "QueryNameExist";
		static const char *RESET_ROLE_NAME = "ResetRoleName";
		static const char *GET_NAME_CACHE = "GetNameCache";
		static const char *GET_GOLD_HISTORY = "GetGoldHistory";
		static const char *GET_TOTAL_ROLE_NUM = "GetTotalRoleNum";

		static const char * GET_ROLE_CMD = "GetRoleCmd";
		static const char * COMFIRM_ROLE_CMD = "ComfirmRoleCmd";
	}
}

#endif
