#include "cmdbuyrecordmanager.hpp"

CmdBuyRecordManager & CmdBuyRecordManager::Instance()
{
	static CmdBuyRecordManager r;
	return r;
}

bool CmdBuyRecordManager::GetAnyCmdBuyRecordUniqueIdAndNeedGold(Role * role, int  save_type, long long & uuid, int & need_gold)
{
	if (role == NULL)
	{
		return false;
	}


	RoleCommonSave * rcs = role->GetRoleModuleManager()->GetRoleCommonSaveManager()->GetRoleCommonSave(save_type);
	if (rcs == NULL)
	{
		return false;
	}

	RetRoleCommonSaveData get_data;
	rcs->BeginDataForeach();

	bool b = rcs->GetNextData(get_data);

	need_gold = get_data.data.cmd_buy_item_data.need_gold;
	uuid = get_data.data.cmd_buy_item_data.unique_id;
	return b;
}

CmdBuyRecordManager::CmdBuyRecordManager()
{
}

CmdBuyRecordManager::~CmdBuyRecordManager()
{
}