#include "speciallogicdreamnoteschallenge.hpp"
#include "world.h"

#include "scene/scene.h"
#include "npc/npcmanager.h"
#include "obj/character/role.h"
#include "obj/character/npc_monster.h"
#include "obj/character/npc_chosed.h"
#include "config/logicconfigmanager.hpp"
#include "other/rolemodulemanager.hpp"
#include "other/dreamnoteschallenge/dreamnoteschallenge.hpp"

SpecialLogicDreamNotesChallengeFb::SpecialLogicDreamNotesChallengeFb(Scene * scene): SpecialLogic(scene)
{
	this->ResetScene();
}

SpecialLogicDreamNotesChallengeFb::~SpecialLogicDreamNotesChallengeFb()
{
}

void SpecialLogicDreamNotesChallengeFb::Update(unsigned long interval, time_t now_second)
{
}

void SpecialLogicDreamNotesChallengeFb::OnRoleEnterScene(Role * role)
{
	if(NULL == role) return;

	if (!m_is_init)
	{
		this->CreateNpc();
		this->CreateMonster();
	}

	this->SendSceneInfo(role);
	//this->SendNotice();
	m_is_init = true;
}

void SpecialLogicDreamNotesChallengeFb::OnRoleLeaveScene(Role * role, bool is_logout)
{
}

void SpecialLogicDreamNotesChallengeFb::OnFightFinish(battlegameprotocol::BattleGameFightResultAck * ack)
{
	if (BATTLE_MODE_DREAM_NOTES_CHALLENGE != ack->battle_mode || RESULT_TYPE_ATTACKER_WIN != ack->battle_result)
	{
		return;
	}
	NPC* npc = NpcManager::Instance().GetServerNPC(ack->business_data.monster_gen_id);
	if (NULL != npc && Obj::OBJ_TYPE_NPC_MONSTER == npc->GetObjType())
	{
		NPCMonster * npc_monster = (NPCMonster*)npc;
		if (npc->GetScene()->GetSceneID() != m_scene->GetSceneID() || npc->GetScene()->GetSceneKey() != m_scene->GetSceneKey())
		{
			return;
		}
		const DreamNotesChallengeCfg * scene_cfg = LOGIC_CONFIG->GetDreamNotesChallengeConfig()->GetChallengeFbCfg(m_level);
		if (NULL == scene_cfg)
		{
			return;
		}
		if (npc_monster->GetNpcId() != m_boss_id)
		{
			return;
		}
		m_boss_id = 0;
		kill_boss_flag = true;
		for (int i = 0; i < ack->attacker_list.role_count && i < SIDE_MAX_ROLE_NUM; ++i)
		{
			Role * mem_role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(ack->attacker_list.role_list[i].uid));
			if (NULL != mem_role)
			{
				mem_role->GetRoleModuleManager()->GetDreamNotesChallenge()->SendPassReward(m_level);
			}
		}
		//通关则清除所有NPC与怪物
		for (int i = 0; i < (int)m_obj_list.size(); i++)
		{
			m_scene->DeleteObj(m_obj_list[i].obj_id);
		}
		m_obj_list.clear();
		this->SendSceneInfo();
	}
}

void SpecialLogicDreamNotesChallengeFb::OnRoleRunAway(battlegameprotocol::BattleGameRoleRunAway * ack)
{
}

void SpecialLogicDreamNotesChallengeFb::OnBattleForcedFinish(battlegameprotocol::BattleGameForceFinishFight * ack)
{
}

void SpecialLogicDreamNotesChallengeFb::OnNPCChosedReward(Role * role, ObjID npc_objid, int option_index)
{
}

void SpecialLogicDreamNotesChallengeFb::OnNPCChosedTalk(Role * role, ObjID npc_objid)
{
	Obj * npc_obj = m_scene->GetObj(npc_objid);
	if (npc_obj != NULL && Obj::OBJ_TYPE_NPC_CHOSED == npc_obj->GetObjType())
	{
		NPCChosed * npc_chosed = (NPCChosed*)npc_obj;
		if (npc_chosed->GetScene()->GetSceneID() == m_scene->GetSceneID() &&
			npc_chosed->GetScene()->GetSceneKey() == m_scene->GetSceneKey())
		{
			int npc_id = npc_chosed->GetNpcId();
			for (int i = 0; i < (int)m_obj_list.size(); i++)
			{
				if (npc_id == m_obj_list[i].npc_id)
				{
					m_obj_list[i].is_need_talk = 0;
					this->SendSceneInfo(role);
					break;
				}
			}
		}
	}
}

void SpecialLogicDreamNotesChallengeFb::SendSceneInfo(Role * role)
{
	Protocol::SCDreamNotesChallengeSceneInfo info;
	info.level = m_level;
	info.kill_boss_flag = kill_boss_flag ? 1 : 0;
	info.count = 0;
	for (int i = 0; i < (int)m_obj_list.size() && i < Protocol::SCDreamNotesChallengeSceneInfo::MAX_SCENE_OBJ_NUM; i++)
	{
		info.obj_list[info.count++] = m_obj_list[i];
	}
	int length = sizeof(info) - (Protocol::SCDreamNotesChallengeSceneInfo::MAX_SCENE_OBJ_NUM - info.count) * sizeof(Protocol::SpecialLogicDreamNotesChallengeObjInfo);
	if (NULL == role)
	{
		m_scene->SendToRole((const char *)&info, length);
	}
	else
	{
		role->GetRoleModuleManager()->NetSend(&info, length);
	}
	
}

void SpecialLogicDreamNotesChallengeFb::SendNotice(Role * role)
{
	Protocol::SCDreamNotesChallengeNotice info;
	info.npc_id = m_plot_npc_id;
	info.is_auto_navigate = m_is_init ? 0 : 1;
	
	if (NULL == role)
	{
		m_scene->SendToRole((const char *)&info, sizeof(info));
	}
	else
	{
		role->GetRoleModuleManager()->NetSend(&info, sizeof(info));
	}
}

void SpecialLogicDreamNotesChallengeFb::RefreshScene(Role * role, int level)
{
	if(NULL == role) return;

	const DreamNotesChallengeCfg * scene_cfg = LOGIC_CONFIG->GetDreamNotesChallengeConfig()->GetChallengeFbCfg(level);
	if (NULL == scene_cfg || level != m_level + 1)
	{
		return;
	}
	//重置玩家位置
	Posi target(0, 0);
	m_scene->GetTownPoint(&target);
	role->ResetPos(target.x, target.y);

	this->ResetScene();
	m_level = level;
	this->CreateNpc();
	this->CreateMonster();
	this->SendSceneInfo(role);
	//this->SendNotice(role);
	m_is_init = true;
}

void SpecialLogicDreamNotesChallengeFb::CreateNpc()
{
	const DreamNotesChallengeCfg * scene_cfg = LOGIC_CONFIG->GetDreamNotesChallengeConfig()->GetChallengeFbCfg(m_level);
	if (NULL == scene_cfg)
	{
		return;
	}

	const ServerNPCInfoCfg *  s_cfg = NPCPOOL->GetServerNPCInfo(scene_cfg->npc_1_id);
	if(NULL == s_cfg) return;

	NPCChosed * npc_chosed_1 = NpcManager::Instance().CreateNPCChosed(scene_cfg->npc_1_id, m_scene, scene_cfg->npc_1_pos, BATTLE_MODE_DREAM_NOTES_CHALLENGE, &scene_cfg->npc_1_dir);
	if (NULL != npc_chosed_1)
	{
		Protocol::SpecialLogicDreamNotesChallengeObjInfo npc_info;
		npc_info.npc_id = npc_chosed_1->GetNpcId();
		npc_info.obj_id = npc_chosed_1->GetId();
		npc_info.is_need_talk = (char)scene_cfg->is_need_talk_1;
		npc_info.obj_type = Protocol::DREAM_NOTES_CHALLENGE_OBJ_TYPE_NPC;
		npc_info.pos_x = npc_chosed_1->GetPos().x;
		npc_info.pos_y = npc_chosed_1->GetPos().y;
	//	m_plot_npc_id = npc_info.npc_id;
		m_obj_list.push_back(npc_info);
	}
	if(0 != scene_cfg->npc_2_id)
	{
		const ServerNPCInfoCfg *  s_cfg = NPCPOOL->GetServerNPCInfo(scene_cfg->npc_2_id);
		if (NULL == s_cfg) return;

		NPCChosed * npc_chosed_2 = NpcManager::Instance().CreateNPCChosed(scene_cfg->npc_2_id, m_scene, scene_cfg->npc_2_pos, BATTLE_MODE_DREAM_NOTES_CHALLENGE, &scene_cfg->npc_2_dir);
		if (NULL != npc_chosed_2)
		{
			Protocol::SpecialLogicDreamNotesChallengeObjInfo npc_info;
			npc_info.npc_id = npc_chosed_2->GetNpcId();
			npc_info.obj_id = npc_chosed_2->GetId();
			npc_info.is_need_talk = (char)scene_cfg->is_need_talk_2;
			npc_info.obj_type = Protocol::DREAM_NOTES_CHALLENGE_OBJ_TYPE_NPC;
			npc_info.pos_x = npc_chosed_2->GetPos().x;
			npc_info.pos_y = npc_chosed_2->GetPos().y;
			m_obj_list.push_back(npc_info);
		}
	}
	if (0 != scene_cfg->npc_3_id)
	{
		const ServerNPCInfoCfg *  s_cfg = NPCPOOL->GetServerNPCInfo(scene_cfg->npc_3_id);
		if (NULL == s_cfg) return;

		NPCChosed * npc_chosed_3 = NpcManager::Instance().CreateNPCChosed(scene_cfg->npc_3_id, m_scene, scene_cfg->npc_3_pos, BATTLE_MODE_DREAM_NOTES_CHALLENGE, &scene_cfg->npc_3_dir);
		if (NULL != npc_chosed_3)
		{
			Protocol::SpecialLogicDreamNotesChallengeObjInfo npc_info;
			npc_info.npc_id = npc_chosed_3->GetNpcId();
			npc_info.obj_id = npc_chosed_3->GetId();
			npc_info.is_need_talk = (char)scene_cfg->is_need_talk_3;
			npc_info.obj_type = Protocol::DREAM_NOTES_CHALLENGE_OBJ_TYPE_NPC;
			npc_info.pos_x = npc_chosed_3->GetPos().x;
			npc_info.pos_y = npc_chosed_3->GetPos().y;
			m_obj_list.push_back(npc_info);
		}
	}
}

void SpecialLogicDreamNotesChallengeFb::CreateMonster()
{
	const DreamNotesChallengeCfg * scene_cfg = LOGIC_CONFIG->GetDreamNotesChallengeConfig()->GetChallengeFbCfg(m_level);
	if (NULL == scene_cfg)
	{
		return;
	}
	NPCMonster * monster = NpcManager::Instance().CreateNPCMonster(scene_cfg->boss_id, m_scene, scene_cfg->boss_pos, BATTLE_MODE_DREAM_NOTES_CHALLENGE, &scene_cfg->boss_dir);
	if (NULL != monster)
	{
		Protocol::SpecialLogicDreamNotesChallengeObjInfo monster_info;
		monster_info.npc_id = monster->GetNpcId();
		monster_info.obj_id = monster->GetId();
		monster_info.obj_type = Protocol::DREAM_NOTES_CHALLENGE_OBJ_TYPE_BOSS;
		monster_info.pos_x = monster->GetPos().x;
		monster_info.pos_y = monster->GetPos().y;
		m_boss_id = monster_info.npc_id;
		kill_boss_flag = false;
		m_obj_list.push_back(monster_info);
	}

}

void SpecialLogicDreamNotesChallengeFb::ResetScene()
{
	m_is_init = false;
	m_level = 0;
	m_boss_id = 0;
	kill_boss_flag = false;
	m_plot_npc_id = 0;
	m_obj_list.clear();
}
