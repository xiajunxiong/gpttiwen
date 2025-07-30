#include "speciallogicfbdujie.hpp"
#include "world.h"
#include "dujie.hpp"
#include "dujieconfig.hpp"

#include "protocol/msgdujie.hpp"

#include "scene/scene.h"
#include "npc/npcmanager.h"
#include "obj/character/role.h"
#include "obj/character/npc_monster.h"
#include "config/logicconfigmanager.hpp"

#include "other/rolemodulemanager.hpp"

SpecialLogicFBDuJie::SpecialLogicFBDuJie(Scene * scene)
	: SpecialLogic(scene)
{
	m_fb_level = 0;
	m_is_finish = 0;

	m_boss_npc_id = 0;
}

void SpecialLogicFBDuJie::OnRoleEnterScene(Role * role)
{
	if (NULL == role)
	{
		return;
	}

	this->SendSceneInfo(role);
}

void SpecialLogicFBDuJie::OnFightFinish(battlegameprotocol::BattleGameFightResultAck * ack)
{
	if (0 != m_is_finish)
	{
		return;
	}

	if (BATTLE_MODE_DU_JIE != ack->battle_mode
		|| RESULT_TYPE_ATTACKER_WIN != ack->battle_result)
	{
		return;
	}

	NPC* npc = NpcManager::Instance().GetServerNPC(ack->business_data.monster_gen_id);
	if (NULL == npc
		|| Obj::OBJ_TYPE_NPC_MONSTER != npc->GetObjType())
	{
		return;
	}

	if (npc->GetNpcId() != m_boss_npc_id)
	{
		return;
	}

	m_is_finish = 1;

	//通关则清除boss
	m_scene->DeleteObj(m_boss_obj_id);

	for (int index = 0
		; index < ack->attacker_list.role_count 
			&& index < SIDE_MAX_ROLE_NUM
		; ++index)
	{
		Role * role = this->GetScene()->GetRoleByUID(ack->attacker_list.role_list[index].uid);
		if (NULL == role)
		{
			continue;
		}

		role->GetRoleModuleManager()->GetDuJie()->OnFBFinish(m_fb_level);
		this->SendSceneInfo(role);
	}
}

void SpecialLogicFBDuJie::SetFBInfo(int fb_level)
{
	m_fb_level = fb_level;

	this->CreateMonster();
}

void SpecialLogicFBDuJie::SendSceneInfo(Role * role)
{
	if (NULL == role)
	{
		return;
	}
	Protocol::SCDuJieFBInfo info;
	info.m_fb_level = m_fb_level;
	info.m_is_finish = m_is_finish;

	role->GetRoleModuleManager()->NetSend(&info, sizeof(info));
}

void SpecialLogicFBDuJie::CreateMonster()
{
	const DuJieFbCfg* fb_cfg = LOGIC_CONFIG->GetDuJieConfig()->GetFbCfg(m_fb_level);
	if (NULL == fb_cfg)
	{
		return;
	}

	NPCMonster * monster = NpcManager::Instance()
		.CreateNPCMonster(fb_cfg->boss_npc_id, m_scene, fb_cfg->boss_pos, BATTLE_MODE_DU_JIE, &fb_cfg->boss_dir);
	if (NULL != monster)
	{
		m_boss_npc_id = monster->GetNpcId();
		m_boss_obj_id = monster->GetId();
	}
}
