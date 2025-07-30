#include "roledrop.hpp"
#include "task/taskmanager.h"
#include "task/taskpool.h"
#include "item/itembase.h"
#include "item/itempool.h"
#include "other/rolemodulemanager.hpp"
#include "gamelog.h"
#include "obj/character/role.h"
#include "task/taskrecorder.h"
#include "monster/monster_group_pool.hpp"
#include "item/knapsack.h"

RoleDrop::RoleDrop() : m_module_mgr(NULL)
{

}

RoleDrop::~RoleDrop()
{

}

void RoleDrop::Init(RoleModuleManager* module, 
	const RoleDropParam& param, 
	const MonsterGroupDropParam& mgdp, 
	const ItemCoolDownDropParam& icddp)
{
	// 应保证RoleDrop先Init于TaskManager

	m_module_mgr = module;

	m_drop_record_map.clear();
	for (int i = 0; i < param.limit_item_num; ++i)
	{
		m_drop_record_map.insert(DropRecordMap::value_type(param.limit_item_list[i], 1));
	}

	m_monster_group_drop_map.clear();
	for (int i = 0; i < mgdp.limit_drop_num; ++i)
	{
		m_monster_group_drop_map.insert(MonsterGroupDropMap::value_type(mgdp.limit_drop_list[i].monster_id, mgdp.limit_drop_list[i].drop_times));
	}

	m_item_drop_cool_down_map.clear();
	for (int i = 0; i < icddp.limit_drop_num; ++i)
	{
		m_item_drop_cool_down_map.insert(
			ItemDropCoolDownMap::value_type(
			icddp.limit_drop_list[i].item_id, 
			icddp.limit_drop_list[i].allow_drop_timestamp
			)
		);
	}
}

void RoleDrop::GetInitParam(RoleDropParam* out_param, 
	MonsterGroupDropParam* out_mgdp, 
	ItemCoolDownDropParam* out_icddp)
{
	if (NULL == out_param) return;

	{
		out_param->limit_item_num = 0;

		DropRecordMap::iterator it = m_drop_record_map.begin();
		for (; it != m_drop_record_map.end() && out_param->limit_item_num < MAX_ROLE_DROP_LIMIT_ITEM_NUM; ++it)
		{
			out_param->limit_item_list[out_param->limit_item_num++] = it->first;
		}
	}
	
	{
		out_mgdp->limit_drop_num = 0;

		MonsterGroupDropMap::iterator it = m_monster_group_drop_map.begin();
		for (; it != m_monster_group_drop_map.end() && out_mgdp->limit_drop_num < MAX_MONSTER_GROUP_DROP_LIMIT_NUM; ++it)
		{
			out_mgdp->limit_drop_list[out_mgdp->limit_drop_num].monster_id = it->first;
			out_mgdp->limit_drop_list[out_mgdp->limit_drop_num].drop_times = it->second;
			++out_mgdp->limit_drop_num;
		}
	}
	
	this->AssembleItemCoolDownDropParam(out_icddp);
}

void RoleDrop::CheckTask()
{
	const TaskParam& param = m_module_mgr->GetTaskManager()->GetTaskParam();
	for (int t_cur = 0; t_cur < param.task_count && t_cur < MAX_ACCEPTED_TASK_NUM; ++t_cur)
	{
		const TaskInfomaConfig* cfg = TASKPOOL->GetTaskCfgByID(param.task_info_list[t_cur].task_id);
		if (NULL != cfg)
		{
			for (int i = 0; i < cfg->limit_drop_count; ++i)
			{
				const ItemBase* item_base = ITEMPOOL->GetItem(cfg->limit_drop_item[i]);
				if (NULL != item_base && item_base->IsLimitDrop())
				{
					this->UnlockItemDrop(cfg->limit_drop_item[i]);

					int monster_group_id = MonsterGroupPool::GetInstance().GetMonsterGroupIDByLimitedDropItemID(cfg->limit_drop_item[i]);
					if (monster_group_id > 0)
					{
						this->RemoveMonsterGroupDrop(monster_group_id);
					}
				}
			}
		}
	}
}

void RoleDrop::OnRoleAcceptTask(int task_id)
{
	const TaskInfomaConfig * t_ti = TASKPOOL->GetTaskCfgByID(task_id);
	if (NULL == t_ti) return;

	if (t_ti->limit_drop_count > 0)
	{
		for (int i = 0; i < t_ti->limit_drop_count; ++i)
		{
			const ItemBase* item_base = ITEMPOOL->GetItem(t_ti->limit_drop_item[i]);
			if (NULL != item_base && item_base->IsLimitDrop())
			{
				this->UnlockItemDrop(t_ti->limit_drop_item[i]);
			}
		}	
	}
}

void RoleDrop::OnRoleGiveupTask(int task_id)
{
	const TaskInfomaConfig * t_ti = TASKPOOL->GetTaskCfgByID(task_id);
	if (NULL == t_ti) return;

	if (t_ti->limit_drop_item > 0)
	{
		for (int i = 0; i < t_ti->limit_drop_count; ++i)
		{
			const ItemBase* item_base = ITEMPOOL->GetItem(t_ti->limit_drop_item[i]);
			if (NULL != item_base && item_base->IsLimitDrop())
			{
				this->LockItemDrop(t_ti->limit_drop_item[i]);
			}
		}
	}
}

void RoleDrop::OnRoleCommitTask(int task_id)
{
	const TaskInfomaConfig * t_ti = TASKPOOL->GetTaskCfgByID(task_id);
	if (NULL == t_ti) return;

	if (t_ti->limit_drop_item > 0)
	{
		for (int i = 0; i < t_ti->limit_drop_count; ++i)
		{
			const ItemBase* item_base = ITEMPOOL->GetItem(t_ti->limit_drop_item[i]);
			if (NULL != item_base && item_base->IsLimitDrop())
			{
				this->LockItemDrop(t_ti->limit_drop_item[i]);
			}
		}
	}
}

void RoleDrop::UnlockItemDrop(ItemID item_id)
{
	UnlockInfo* info = this->GetUnlockInfo(item_id);
	if (NULL != info)
	{
		info->ref_cnt += 1;
	}
	else
	{
		this->AddUnlockInfo(item_id);
	}

	gamelog::g_log_roledrop.printf(LL_INFO, "UnlockItemDrop role[%d,%s] item_id[%d]", 
		m_module_mgr->GetRole()->GetUID(), m_module_mgr->GetRole()->GetName(), item_id);
}

void RoleDrop::LockItemDrop(ItemID item_id)
{
	UnlockInfo* info = this->GetUnlockInfo(item_id);
	if (NULL != info)
	{
		info->ref_cnt -= 1;
		if (info->ref_cnt <= 0)
		{
			this->RemoveUnlockInfo(item_id);
		}
	}

	gamelog::g_log_roledrop.printf(LL_INFO, "LockItemDrop role[%d,%s] item_id[%d]",
		m_module_mgr->GetRole()->GetUID(), m_module_mgr->GetRole()->GetName(), item_id);
}

bool RoleDrop::IsLockDrop(ItemID item_id)
{
	TaskID task_id = TASKPOOL->GetLimitDropItemTask(item_id);
	const TaskInfomaConfig* tic = TASKPOOL->GetTaskCfgByID(task_id);
	if (NULL == tic)
	{
		return false;
	}

	if (TASK_NEED_HAVE_PROPERTY != tic->task_condition && TASK_NEED_HAVE_PROPERTY_LIST != tic->task_condition)
	{
		return false;
	}


	// 如果已完成任务，则限制掉落
	if (m_module_mgr->GetTaskRecorder()->HaveFinished(task_id))
	{
		return true;
	}

	if (TASK_NEED_HAVE_PROPERTY == tic->task_condition)
	{
		int need_num = tic->param2;
		if (m_module_mgr->GetKnapsack()->HasItem(item_id, need_num))
		{
			// 已有足够物品
			return true;
		}
	}
	else
	{
		int need_num = TASK_NEED_HAVE_PROPERTY_LIST_REQUIRE_NUM; // 默认需求1个物品
		if (m_module_mgr->GetKnapsack()->HasItem(item_id, need_num))
		{
			// 已有足够物品
			return true;
		}
	}

	return false;
}

void RoleDrop::AddDropRecord(ItemID item_id)
{
	m_drop_record_map.insert(DropRecordMap::value_type(item_id, 1));
}

bool RoleDrop::IsLockReceipeDrop(ItemID item_id)
{
	if (m_drop_record_map.end() != m_drop_record_map.find(item_id))
	{
		return true;
	}
	return false;
}

void RoleDrop::AddMonsterGroupDrop(int monster_group_id)
{
	const MonsterGroupCfg* cfg = MonsterGroupPool::GetInstance().GetMonsterGroupCfg(monster_group_id);
	if (cfg == NULL || cfg->drop_times <= 0) return;

	MonsterGroupDropMap::iterator it = m_monster_group_drop_map.find(monster_group_id);
	if (it == m_monster_group_drop_map.end())
	{
		it = m_monster_group_drop_map.insert(MonsterGroupDropMap::value_type(monster_group_id, 0)).first;
	}

	it->second += 1;
}

bool RoleDrop::IsMonsterGroupDropLimit(int monster_group_id)
{
	const MonsterGroupCfg* cfg = MonsterGroupPool::GetInstance().GetMonsterGroupCfg(monster_group_id);
	if (cfg == NULL || cfg->drop_times <= 0) return false;

	MonsterGroupDropMap::iterator it = m_monster_group_drop_map.find(monster_group_id);
	if (it == m_monster_group_drop_map.end())
	{
		return false;
	}

	return it->second >= cfg->drop_times;
}

void RoleDrop::RemoveMonsterGroupDrop(int monster_group_id)
{
	const MonsterGroupCfg* cfg = MonsterGroupPool::GetInstance().GetMonsterGroupCfg(monster_group_id);
	if (cfg == NULL || cfg->drop_times <= 0) return;

	m_monster_group_drop_map.erase(monster_group_id);
}

void RoleDrop::AddItemDropCoolDown(ItemID item_id, int cooldown_s)
{
	unsigned int now = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	m_item_drop_cool_down_map[item_id] = now + cooldown_s;

	gamelog::g_log_roledrop.printf(LL_INFO, "AddItemDropCoolDown role[%d,%s] item_id[%d] cd_seconds[%d] allow_drop_timestamp[%u]",
		m_module_mgr->GetUid(), m_module_mgr->GetRoleName(), item_id, cooldown_s, now + cooldown_s);
}

void RoleDrop::RemoveItemDropCoolDown(ItemID item_id)
{
	m_item_drop_cool_down_map.erase(item_id);

	gamelog::g_log_roledrop.printf(LL_INFO, "RemoveItemDropCoolDown role[%d,%s] item_id[%d]",
		m_module_mgr->GetUid(), m_module_mgr->GetRoleName(), item_id);
}

bool RoleDrop::IsItemDropInCoolDown(ItemID item_id)
{
	ItemDropCoolDownMap::const_iterator it = m_item_drop_cool_down_map.find(item_id);
	if (it == m_item_drop_cool_down_map.end()) return false;

	unsigned int now = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	if (now >= it->second)
	{
		// 顺便删掉
		this->RemoveItemDropCoolDown(item_id);
		return false;
	}

	return true;
}

UnlockInfo* RoleDrop::GetUnlockInfo(ItemID item_id)
{
	UnlockDropItems::iterator it = m_unlock_items.find(item_id);
	if (it != m_unlock_items.end())
	{
		return &it->second;
	}
	return NULL;
}

void RoleDrop::AddUnlockInfo(ItemID item_id)
{
	UnlockInfo info;
	info.ref_cnt = 1;
	m_unlock_items.insert(UnlockDropItems::value_type(item_id, info));
}

void RoleDrop::RemoveUnlockInfo(ItemID item_id)
{
	m_unlock_items.erase(item_id);
}

void RoleDrop::AssembleItemCoolDownDropParam(ItemCoolDownDropParam* out_param)
{
	std::vector<ItemID> erase_vec;
	int num = 0;
	unsigned int now = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	ItemDropCoolDownMap::iterator it = m_item_drop_cool_down_map.begin();
	for (; it != m_item_drop_cool_down_map.end()
		&& num < ARRAY_ITEM_COUNT(out_param->limit_drop_list); ++it)
	{
		if (now < it->second)
		{
			out_param->limit_drop_list[num].item_id = it->first;
			out_param->limit_drop_list[num].allow_drop_timestamp = it->second;
			num += 1;
		}
		else
		{
			erase_vec.push_back(it->first);
		}
	}

	out_param->limit_drop_num = num;

	for (size_t i = 0; i < erase_vec.size(); ++i)
	{
		this->RemoveItemDropCoolDown(erase_vec[i]);
	}
}

