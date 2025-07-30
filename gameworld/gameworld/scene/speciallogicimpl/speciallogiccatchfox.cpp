#include "speciallogiccatchfox.hpp"
#include "world.h"
#include "servercommon/anecdotetaskdef.hpp"
#include "other/anecdotetask/anecdotetask.hpp"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "obj/character/npc_chosed.h"
#include "npc/npcmanager.h"
#include "other/catchfox/catchfoxconfig.hpp"
#include "config/logicconfigmanager.hpp"
#include "other/catchfox/maintaskcatchfoxconfig.hpp"

SpecialLogicCatchFoxFb::SpecialLogicCatchFoxFb(Scene * scene) : SpecialLogic(scene), m_is_main_task_catch_fox(false)
{
	const MainTaskCatchFoxFindFoxConfig* cfg = LOGIC_CONFIG->GetMainTaskCatchFoxFbConfig()->GetFindFoxConfig();
	if (NULL != cfg && cfg->scene_id == scene->GetSceneID())
	{
		m_is_main_task_catch_fox = true;
	}
}

SpecialLogicCatchFoxFb::~SpecialLogicCatchFoxFb()
{
}

void SpecialLogicCatchFoxFb::OnFightFinish(battlegameprotocol::BattleGameFightResultAck * ack)
{
	if (!this->IsMainTaskCatchFox() && RESULT_TYPE_ATTACKER_WIN == ack->battle_result) 
	{
		for (int i = 0; i < ack->attacker_list.role_count && i < SIDE_MAX_ROLE_NUM; ++i)
		{
			Role * mem_role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(ack->attacker_list.role_list[i].uid));
			if (NULL == mem_role) continue;

			mem_role->GetRoleModuleManager()->GetAnecdoteTask()->OnFinishTask(ANECDOTE_TASK_TYPE_CATCH_FOX);
		}
	}
	
	this->ClearAllNpcMonster();		// 清理所有明雷；
	this->DelayKickOutAllRole();	// 直接踢出副本；
}

void SpecialLogicCatchFoxFb::OnRoleRunAway(battlegameprotocol::BattleGameRoleRunAway * ack)
{
	this->ClearAllNpcMonster();		// 清理所有明雷；
	this->DelayKickOutAllRole();	// 直接踢出副本；
}

void SpecialLogicCatchFoxFb::OnBattleForcedFinish(battlegameprotocol::BattleGameForceFinishFight * ack)
{
	this->ClearAllNpcMonster();		// 清理所有明雷；
	this->DelayKickOutAllRole();	// 直接踢出副本；
}

void SpecialLogicCatchFoxFb::OnCatchFox(int x, int y, int dir_y)
{
	if (this->IsMainTaskCatchFox())
	{
		const MainTaskCatchFoxOtherConfig * other_cfg = LOGIC_CONFIG->GetMainTaskCatchFoxFbConfig()->GetOtherCfg();
		if (other_cfg == NULL) return;

		Dir monster_dir = dir_y;
		NPCChosed * monster = NpcManager::Instance().CreateNPCChosed(other_cfg->fox_npc_id, m_scene, Posi(x, y), BATTLE_MODE_CATCH_FOX, (const Dir*)&dir_y);
		if (monster == NULL) return;
	}
	else
	{
		const CatchFoxOtherConfig * other_cfg = LOGIC_CONFIG->GetCatchFoxFbConfig()->GetOtherCfg();
		if (other_cfg == NULL) return;
		
		Dir monster_dir = dir_y;
		NPCChosed * monster = NpcManager::Instance().CreateNPCChosed(other_cfg->fox_npc_id, m_scene, Posi(x, y), BATTLE_MODE_CATCH_FOX, (const Dir*)&dir_y);
		if (monster == NULL) return;
	}
}
