#include "speciallogicpetgodfb.hpp"
#include "world.h"

#include "protocol/msgpetgodfb.hpp"

#include "scene/scene.h"
#include "npc/npcmanager.h"
#include "obj/character/role.h"

#include "obj/character/npc_monster.h"
#include "obj/character/npc_chosed.h"

#include "other/rolemodulemanager.hpp"
#include "other/petgodfb/petgodfb.hpp"

#include "config/logicconfigmanager.hpp"
#include "other/petgodfb/petgodfbconfig.hpp"
#include "other/pet/pet.hpp"
#include "other/partner/partner.hpp"

SpecialLogicPetGodFB::SpecialLogicPetGodFB(Scene * scene)
	: SpecialLogic(scene)
{
	m_is_set_init = 0;
	m_fb_seq = 0;
	m_role_level = 0;

	m_is_finish = 0;

	m_boss_npc_id = 0;

	m_role_uid = 0;
}

void SpecialLogicPetGodFB::OnRoleEnterScene(Role * role)
{
	if (NULL == role)
	{
		return;
	}

	this->SendSceneInfo(role);
}

void SpecialLogicPetGodFB::OnFightFinish(battlegameprotocol::BattleGameFightResultAck * ack)
{
	if (BATTLE_MODE_PET_GOD_FB == ack->battle_mode &&
		ack->special_business_data.data_length >= (int)sizeof(BattleFBData) &&
		BATTLE_BUSINESS_DATA_TYPE_FB == ack->special_business_data.data_type)
	{
		this->SyncHpMp(ack->special_business_data.data);
	}

	if (1 != m_is_set_init)
	{
		return;
	}

	if (0 != m_is_finish)
	{
		return;
	}

	if (BATTLE_MODE_PET_GOD_FB != ack->battle_mode || RESULT_TYPE_ATTACKER_WIN != ack->battle_result)
	{
		return;
	}

	bool is_role_win = false;
	for (int i = 0; i < ack->attacker_list.role_count && i < SIDE_MAX_ROLE_NUM; ++i)
	{
		if (ack->attacker_list.role_list[i].uid == m_role_uid)
		{
			is_role_win = true;
			break;
		}
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

	Role * role = this->GetScene()->GetRoleByUID(m_role_uid);
	if (NULL == role)
	{
		return;
	}

	role->GetRoleModuleManager()->GetPetGodFB()->OnFBFinish(m_fb_seq);

	this->SendSceneInfo(role);
}

void SpecialLogicPetGodFB::OnRoleRunAway(battlegameprotocol::BattleGameRoleRunAway * ack)
{
	if (BATTLE_MODE_PET_GOD_FB == ack->battle_mode &&
		ack->special_business_data.data_length >= (int)sizeof(BattleFBData) &&
		BATTLE_BUSINESS_DATA_TYPE_FB == ack->special_business_data.data_type)
	{
		this->SyncHpMp(ack->special_business_data.data);
	}
}

void SpecialLogicPetGodFB::OnBattleForcedFinish(battlegameprotocol::BattleGameForceFinishFight * ack)
{
	if (BATTLE_MODE_PET_GOD_FB == ack->battle_mode &&
		ack->special_business_data.data_length >= (int)sizeof(BattleFBData) &&
		BATTLE_BUSINESS_DATA_TYPE_FB == ack->special_business_data.data_type)
	{
		this->SyncHpMp(ack->special_business_data.data);
	}
}

void SpecialLogicPetGodFB::OnNPCChosedReward(Role * role, ObjID npc_objid, int option_index)
{
	Obj * npc_obj = m_scene->GetObj(npc_objid);
	if (NULL == role || npc_obj == NULL || Obj::OBJ_TYPE_NPC_CHOSED != npc_obj->GetObjType())return;

	//恢复
	{
		BattleAttr max_hp = role->GetRoleModuleManager()->GetAttrByType(BATTLE_ATTR_MAX_HP);
		BattleAttr max_mp = role->GetRoleModuleManager()->GetAttrByType(BATTLE_ATTR_MAX_MP);

		role->GetRoleModuleManager()->SetCurHpMp(max_hp, max_mp);
	}

	{
		Pet* role_pet = role->GetRoleModuleManager()->GetPet();
		int pet_fight_index = role_pet->GetFightIndex();
		BattleAttr max_hp = role_pet->GetPetAttr(pet_fight_index, BATTLE_ATTR_MAX_HP);
		BattleAttr max_mp = role_pet->GetPetAttr(pet_fight_index, BATTLE_ATTR_MAX_MP);
		role_pet->ChangeHP(pet_fight_index, max_hp);
		role_pet->ChangeMp(pet_fight_index, max_mp);
	}

	{
		Partner * role_partner = role->GetRoleModuleManager()->GetPartner();
		role_partner->AddAllPartnerHpMp(((double)1));
	}
}

void SpecialLogicPetGodFB::SetFBInfo(int role_uid, int fb_seq, int role_level)
{
	if (0 != m_is_set_init)
	{
		return;
	}
	m_is_set_init = 1;

	m_fb_seq = fb_seq;
	m_role_level = role_level;
	m_role_uid = role_uid;

	this->CreateMonster();
	this->CreateNpc();
}

void SpecialLogicPetGodFB::SendSceneInfo(Role * role)
{
	if (NULL == role)
	{
		return;
	}
	Protocol::SCPetGodFBSceneInfo info;

	info.m_fb_seq = m_fb_seq;
	info.m_is_finish = m_is_finish;
	info.m_boss_npc_id = m_boss_npc_id;

	role->GetRoleModuleManager()->NetSend(&info, sizeof(info));
}

void SpecialLogicPetGodFB::CreateMonster()
{
	const PetGodFbConfig::FBCfg * fb_cfg = LOGIC_CONFIG->GetPetGodFbConfig()->GetFBCfg(m_fb_seq);
	if (NULL == fb_cfg)
	{
		return;
	}

	NPCMonster * monster = NpcManager::Instance()
		.CreateNPCMonster(fb_cfg->boss_npc_id, m_scene, fb_cfg->boss_pos, BATTLE_MODE_PET_GOD_FB, &fb_cfg->boss_dir);
	if (NULL != monster)
	{
		m_boss_npc_id = monster->GetNpcId();
		m_boss_obj_id = monster->GetId();
	}
}

void SpecialLogicPetGodFB::CreateNpc()
{
	const PetGodFbConfig::FBCfg * fb_cfg = LOGIC_CONFIG->GetPetGodFbConfig()->GetFBCfg(m_fb_seq);
	if (NULL == fb_cfg)
	{
		return;
	}

	NpcManager::Instance().
		CreateNPCChosed(fb_cfg->recover_npc_id, m_scene, fb_cfg->recover_npc_pos, BATTLE_MODE_PET_GOD_FB, &fb_cfg->recover_npc_dir);

}
