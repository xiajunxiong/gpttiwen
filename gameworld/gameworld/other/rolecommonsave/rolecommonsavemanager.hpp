#ifndef __ROLE_COMMON_SAVE_MANAGER_HPP__
#define __ROLE_COMMON_SAVE_MANAGER_HPP__

#include "other/rolecommonsave/rolecommonsave.hpp"

#include <string>

std::string GetString(const RoleCommonSaveCmdBuyItem & ref);
std::vector<ItemConfigData> BuyItemToItemConfigDataVec(int num, const RoleCommonSaveGameItem * item);
void ItemConfigDataVecToBuyItem(const std::vector<ItemConfigData> & item_config_data, RoleCommonSaveGameItem ARG_OUT * item, int ARG_OUT * num);


class RoleModuleManager;

class RoleCommonSaveManager
{
public:
	RoleCommonSaveManager();
	~RoleCommonSaveManager();

	RoleCommonSave * GetRoleCommonSave(int save_type);

	void Init(RoleModuleManager * mgr, const RoleCommonSaveDataParam & data_param);
	void GetInitParam(RoleCommonSaveDataParam * data_param, bool is_logout);
	void ClearDirtyMark(const RoleCommonSaveDataParam & data_param);

	void Gm(int op, int save_type, int p1, int p2);

	void * operator new(size_t c);
	void operator delete(void * m);
private:	
	void Add(RoleCommonSave * role_common_save);

	RoleModuleManager * m_role_module_mgr;
	RoleCommonSave * m_save_list[ROLE_COMMON_SAVE_DATA_TYPE_MAX];
};

#endif

