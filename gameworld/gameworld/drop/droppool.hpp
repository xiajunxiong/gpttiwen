#ifndef __DROP_POOL_HPP__
#define __DROP_POOL_HPP__

#include <string>
#include <vector>

#include "servercommon/basedef.h"
#include "servercommon/serverdef.h"
#include "item/knapsack.h"

#include "gamedef.h"
#include "dropconfig.hpp"

class Scene;
class Role;

#define DROPPOOL DropPool::Instance()

class DropPool
{
public:
	static DropPool * Instance();

	bool Init(const std::string &config_dir, const std::string &configname, std::string *err);
	static bool Reload(const std::string &config_dir, const std::string &configname, std::string *err);

	bool IsUseActivityNotice(int get_reason_type);

	DropConfig * GetDropConfig(UInt16 drop_id);
	bool DropToRole(Role* role, UInt16 dropid, int monster_group_id, bool is_log = true);
	bool DropToRole(Role* role, UInt16 dropid);

	bool DropListToRole(Role* role, const std::vector<int> & dropid_list, int monster_group_id, bool is_log = true, int notice_get_reson = GET_REASON_INVALID, 
		int multipel = 1, std::vector<ItemConfigData> *item_list = NULL, int put_reason = PUT_REASON_INVALID, int param1 = 0, int param2 = 0, int activity_type = 0, bool is_win = true);
	bool DropToRoleAndOutDropItem(Role* role, UInt16 dropid, int monster_group_id, std::vector<ItemConfigData> ARG_OUT & out_drop_item, int multipel = 1);		// multipel:±¶Êý
	bool DropListToRoleAddReveal(Role * role, std::vector<ItemDataWrapper> add_show_list, int notice_get_reson = GET_REASON_INVALID, std::vector<ItemConfigData> *item_list = NULL, int put_reason = PUT_REASON_INVALID, int param1 = 0, int param2 = 0);

	bool RandOutDropItem(Role* role, UInt16 dropid, int monster_group_id, std::vector<ItemConfigData> ARG_OUT & out_drop_item, int multipel = 1, Int64 * out_add_coin = NULL, Int64 * out_add_immortal_coin = NULL, Int64 * out_add_gold = NULL);
	bool RandOutDropItemList(Role* role, const std::vector<int> & dropid_list, int monster_group_id, std::vector<ItemConfigData> *out_item_list, int multipel = 1, Int64 * out_add_coin = NULL, Int64 * out_add_immortal_coin = NULL, Int64 * out_add_gold = NULL);

	int GetDropItemList(UInt16 dropid, ItemConfigData item_list[DropConfig::DROP_ITEM_PROB_MAX]);

private:
	DropPool();
	~DropPool();

	void Release();

	static const int MAX_DROP_ID = (int)USHRT_MAX + 1;
	DropConfig *m_drop_list[MAX_DROP_ID];
};

#endif // __DROP_POOL_HPP__

