#include "bbl_impl_field_boss.hpp"
#include "battle/battle_manager_local.hpp"
#include "obj/character/npc_monster.h"
#include "npc/npcmanager.h"
#include "scene/scenemanager.hpp"
#include "world.h"
#include "obj/character/role.h"
#include "servercommon/logdef.h"

BBLImplFieldBoss::BBLImplFieldBoss()
{

}

BBLImplFieldBoss::~BBLImplFieldBoss()
{

}

void BBLImplFieldBoss::OnBattleStart(battlegameprotocol::BattleGameStartFightAck* ack)
{
	for (int i = 0; i < ack->role_count; ++i)
	{
		int role_id = ack->role_list[i].uid;
		Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(role_id));
		if (NULL == role) continue;

		role->LogActive(LOG_ACTIVE_TYPE_FIELD_BOSS);
	}
}

void BBLImplFieldBoss::OnBattlePrepare(int battle_mode, int role_count, Role** role_list, RoleBattleData** role_data_list)
{
	BattleBusinessLocal::OnBattlePrepare(battle_mode, role_count, role_list, role_data_list);

	NPCMonster* npc_monster = NpcManager::Instance().GetNPCMonster(m_battle_local_info->monster_gen_id);
	if (NULL != npc_monster)
	{
		npc_monster->SetBattleID(m_battle_local_info->battle_id);
	}
	
}

void BBLImplFieldBoss::OnBattleInstanceDeleted()
{
	BattleBusinessLocal::OnBattleInstanceDeleted();

	this->ReleaseBossLock();
}

void BBLImplFieldBoss::OnBattleForceFinish(battlegameprotocol::BattleGameForceFinishFight* ack)
{
	BattleBusinessLocal::OnBattleForceFinish(ack);

	this->ReleaseBossLock();
}

void BBLImplFieldBoss::ReleaseBossLock()
{
	NPCMonster* npc_monster = NpcManager::Instance().GetNPCMonster(m_battle_local_info->monster_gen_id);
	if (NULL != npc_monster)
	{
		if (npc_monster->IsMonsterLock() && npc_monster->GetBattleID() == m_battle_local_info->battle_id)
		{
			npc_monster->SetMonsterLock(false); // 解决因异常情况导致npc_monster一直被锁
		}
	}
}
