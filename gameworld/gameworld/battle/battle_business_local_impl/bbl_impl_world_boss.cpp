#include "bbl_impl_world_boss.hpp"
#include "battle/battle_manager_local.hpp"
#include "obj/character/npc_monster.h"
#include "npc/npcmanager.h"
#include "global/activity/activityimpl/activityworldboss.hpp"
#include "global/activity/activitymanager.hpp"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "other/roleactivity/roleactivity.hpp"
#include "world.h"
#include "servercommon/logdef.h"
#include "protocol/msgworldboss.h"

BBLImplWorldBoss::BBLImplWorldBoss()
{

}

BBLImplWorldBoss::~BBLImplWorldBoss()
{

}

void BBLImplWorldBoss::OnBattleFinish(battlegameprotocol::BattleGameFightResultAck* ack)
{
	int npc_seq = ack->business_data.monster_gen_id;

	ActivityWorldBoss* act = static_cast<ActivityWorldBoss*>(ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_WORLD_BOSS));
	if (NULL == act) return;
	
	long long team_total_damage = 0;
	long long team_total_heal = 0;
	for (int i = 0; i < ack->attacker_list.role_count; ++i)
	{
		if (ack->attacker_list.role_list[i].uid != 0)
		{
			battlegameprotocol::BattleGameFightResultAck::RoleInfo & role_data = ack->attacker_list.role_list[i];
			//act->AddJoinRecord(ack->attacker_list.role_list[i].uid, (int)role_data.role_statistic.total_damage_to_enemy, (int)role_data.role_statistic.total_heal_other);

			team_total_damage += role_data.role_statistic.total_damage_to_enemy;
			team_total_heal += role_data.role_statistic.total_heal_other;

			for (int k = 0; k < ack->attacker_list.role_list[i].pet_num; ++k)
			{
				team_total_damage += role_data.pet_stat_list[k].total_damage_to_enemy;
				team_total_heal += role_data.pet_stat_list[k].total_heal_other;
			}
		}
	}	

	Protocol::SCWorldBossBattleInfo protocol;
	protocol.team_total_damage = team_total_damage;
	protocol.team_total_heal = team_total_heal;

	for (int i = 0; i < ack->attacker_list.role_count; ++i)
	{
		if (ack->attacker_list.role_list[i].uid != 0)
		{
			Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(ack->attacker_list.role_list[i].uid);
			if (role != NULL)
			{
				EngineAdapter::Instance().NetSend(role->GetNetId(), &protocol, sizeof(protocol));
			}
		}
	}

	if (RESULT_TYPE_ATTACKER_WIN == ack->battle_result)
	{
		act->OnKillBoss();
	}
}

void BBLImplWorldBoss::OnBattleForceFinish(battlegameprotocol::BattleGameForceFinishFight * ack)
{
	// 触发这里表明boss已被打死，
	// 更新伤害排行榜
	ActivityWorldBoss* act = static_cast<ActivityWorldBoss*>(ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_WORLD_BOSS));
	if (NULL == act)
	{
		return;
	}

	for (int i = 0; i < ack->attacker_list.role_count; ++i)
	{
		if (ack->attacker_list.role_list[i].uid != 0)
		{
			battlegameprotocol::BattleGameForceFinishFight::RoleInfo & role_data = ack->attacker_list.role_list[i];
			//act->AddJoinRecord(ack->attacker_list.role_list[i].uid, (int)role_data.role_statistic.total_damage_to_enemy, (int)role_data.role_statistic.total_heal_other);
		}
	}
}

