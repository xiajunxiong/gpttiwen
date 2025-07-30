#ifndef __ROLE_DROP_HPP__
#define __ROLE_DROP_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roledropdef.hpp"
#include <map>

struct UnlockInfo
{
	UnlockInfo() : ref_cnt(0) {}

	int ref_cnt; // 引用计数
};

class RoleModuleManager;
class RoleDrop
{
public:
	typedef std::map<ItemID, UnlockInfo> UnlockDropItems;
	typedef std::map<ItemID, int> DropRecordMap;
	typedef std::map<int, unsigned short> MonsterGroupDropMap;
	typedef std::map<ItemID, unsigned int> ItemDropCoolDownMap;

public:
	RoleDrop();
	~RoleDrop();

	void Init(RoleModuleManager* module, 
		const RoleDropParam& param, 
		const MonsterGroupDropParam& mgdp,
		const ItemCoolDownDropParam& icddp);
	void GetInitParam(RoleDropParam* out_param, 
		MonsterGroupDropParam* out_mgdp,
		ItemCoolDownDropParam* out_icddp);

	void CheckTask();

	void OnRoleAcceptTask(int task_id);
	void OnRoleGiveupTask(int task_id);
	void OnRoleCommitTask(int task_id);

	void * operator new(size_t c);
	void operator delete(void* m);

	void UnlockItemDrop(ItemID item_id);
	void LockItemDrop(ItemID item_id);
	bool IsLockDrop(ItemID item_id);
	void AddDropRecord(ItemID item_id);
	bool IsLockReceipeDrop(ItemID item_id);

	void AddMonsterGroupDrop(int monster_group_id);
	bool IsMonsterGroupDropLimit(int monster_group_id);
	void RemoveMonsterGroupDrop(int monster_group_id);

	void AddItemDropCoolDown(ItemID item_id, int cooldown_s);
	void RemoveItemDropCoolDown(ItemID item_id);
	bool IsItemDropInCoolDown(ItemID item_id);
private:
	UnlockInfo* GetUnlockInfo(ItemID item_id);
	void AddUnlockInfo(ItemID item_id);
	void RemoveUnlockInfo(ItemID item_id);
	void AssembleItemCoolDownDropParam(ItemCoolDownDropParam* out_param);

	RoleModuleManager* m_module_mgr;
	UnlockDropItems m_unlock_items;
	DropRecordMap m_drop_record_map;
	MonsterGroupDropMap m_monster_group_drop_map;
	ItemDropCoolDownMap m_item_drop_cool_down_map;
};


#endif